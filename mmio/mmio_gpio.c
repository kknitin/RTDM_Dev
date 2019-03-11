#include <mraa.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>



void catch_signal(int sig)
{
	mraa_gpio_context pin1 = mraa_gpio_init(100);
	mraa_gpio_context pin2 = mraa_gpio_init(102);
	mraa_gpio_write(pin1, 0);
	mraa_gpio_write(pin2, 0);
	printf("Exiting ...\n");
	
	exit(0);
}


int main()
{
	int result = 0;
	signal(SIGINT, catch_signal);	
	mraa_init(); 
	mraa_gpio_context pin1 = mraa_gpio_init(100);
	mraa_gpio_dir(pin1, MRAA_GPIO_OUT_HIGH);
	mraa_gpio_use_mmaped(pin1,1);
	mraa_gpio_context pin2 = mraa_gpio_init(102);
	mraa_gpio_dir(pin2, MRAA_GPIO_OUT_LOW);
	result = mraa_gpio_use_mmaped(pin2,1);
	printf(" \n MMIO result = %d \n",result);
	for (;;) {
		mraa_gpio_write(pin1, 0);
		mraa_gpio_write(pin2, 1);
		usleep(500000);
		mraa_gpio_write(pin1, 1);
		mraa_gpio_write(pin2, 0);
		usleep(500000);
	}
	
	return MRAA_SUCCESS;
}