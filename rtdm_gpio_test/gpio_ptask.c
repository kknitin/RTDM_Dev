#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/mman.h>
#include <rtdm/rtdm.h>

#include "gpio_task.h"

int device_handle;
gpio_dev gpio_pin;


void *rt_gpio_task(void *arg)
{
	int result, ioarg;
	rt_printf("RT task started!\n");
	struct timespec ts;
	ioarg = gpio_pin.gpio;
	
	result = ioctl(device_handle, GPIO_SET_PIN, &ioarg);
	
	if ( result < 0) {
	perror("ioctl GPIO_SET_PIN Failed:  RT Task ended! \n");
	pthread_exit(NULL);
    }
	ioarg = 1; // for out direction
	result = ioctl(device_handle, GPIO_SET_DIR, &ioarg);
	
	if ( result < 0) {
	perror("ioctl GPIO_SET_DIR Failed:  RT Task ended! \n");
	pthread_exit(NULL);
    }
	ioarg = gpio_pin.val;
	
	result = ioctl(device_handle, GPIO_SET_VAL, &ioarg);
	
	if ( result < 0) {
	perror("ioctl GPIO_SET_VAL Failed:  RT Task ended! \n");
	pthread_exit(NULL);
    }
	
	ts.tv_sec = 3;
    ts.tv_nsec = 500000000; 
    clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
	
	pthread_exit(NULL);
}



void catch_signal(int sig)
{
	printf("Exiting ...\n");
	exit(0);
}


int open_device()
{
	int res;
	int value;

	/* Open device */
	res = open("/dev/rtdm/rtdm_gpio", O_RDWR);
	if (res < 0) {
		printf("%s: Could not open spi device, open has failed with %d (%s).", __FUNCTION__, errno, strerror(errno));
		return res;
	} else {
		printf("%s: Device opened.", __FUNCTION__);
		device_handle = res;
	}

	return res;
	
}

int main(int argc, char* argv[])
{
	 
    
	if (argc > 1)
	{
		gpio_pin.gpio = atoi(argv[1]);
		gpio_pin.val = atoi(argv[2]);
	}	
	else
	{
		gpio_pin.gpio = 337;
		gpio_pin.val = 1;
		printf("Arguments i.e gpio number and val to be set should be passed Default gpio number = %d and val = %d !\n",gpio_pin.gpio,gpio_pin.val);
	}
		
	
	signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);


	
	// Lock memory : avoid memory swapping for this program
	mlockall(MCL_CURRENT|MCL_FUTURE);

	if (open_device() < 0) {
	printf("open failed !");
	exit(-1);
    }
	
	struct sched_param rtparam = { .sched_priority = 99 };
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attributes, SCHED_FIFO);
    pthread_attr_setschedparam(&attributes, &rtparam);

	pthread_t rt_thread;
	
	pthread_create(&rt_thread, &attributes, &rt_gpio_task, NULL);
	
	pthread_join(rt_thread, NULL);
	
	close(device_handle);
}
	
