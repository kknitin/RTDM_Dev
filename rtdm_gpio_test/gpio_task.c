#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>


#include "gpio_task.h"

RT_TASK demo_task;
int device_handle;
int gpio_pin;
int val;


void rt_gpio_task(void *arg)
{
	int result;
	rt_printf("RT task started!\n");
	struct timespec ts;
	result = ioctl(device_handle, GPIO_SET_PIN, &gpio_pin);
	
	if ( result < 0) {
	perror("ioctl GPIO_SET_PIN Failed:  RT Task ended! \n");
	return;
    }
	
	result = ioctl(device_handle, GPIO_SET_DIR, &gpio_pin);
	
	if ( result < 0) {
	perror("ioctl GPIO_SET_DIR Failed:  RT Task ended! \n");
	return;
    }
	
	result = ioctl(device_handle, GPIO_SET_VAL, &val);
	
	if ( result < 0) {
	perror("ioctl GPIO_SET_VAL Failed:  RT Task ended! \n");
	return;
    }
	
	ts.tv_sec = 3;
    ts.tv_nsec = 500000000; 
    clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
	
	
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

	
	
}
int main(int argc, char* argv[])
{
	
	char  str[10] ;
	
	if (argc > 1)
	{
		gpio_pin = atoi(argv[1]);
		val = atoi(argv[2]);
	}	
	else
	{
		gpio_pin = 337;
		val = 1;
		printf("Arguments i.e gpio number and val to be set should be passed Default gpio number = 337 and val = 1 !\n");
	}
		
	
	signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);

	

	// Lock memory : avoid memory swapping for this program
	mlockall(MCL_CURRENT|MCL_FUTURE);

	if (open_device() < 0) {
	printf("open failed !");
	exit(-1);
    }
   
	sprintf(str,"Gpio testing example");                                                                                                                               
	rt_task_create(&demo_task, str, 0, 99, 0);                                                                                                          
   
	rt_task_start(&demo_task, &rt_gpio_task, 0);   

	rt_task_delete(&demo_task);
	
	close(device_handle);
}      

