#ifndef GPIO_TEST_H
#define GPIO_TEST_H

#include <linux/ioctl.h>


#define GPIO_IOC_MAGIC		'r'

#define GPIO_SET_PIN	_IOW(GPIO_IOC_MAGIC, 1, int)
#define GPIO_GET_PIN	_IOR(GPIO_IOC_MAGIC, 2, int)
#define GPIO_SET_DIR	_IOW(GPIO_IOC_MAGIC, 3, int)
#define GPIO_GET_DIR	_IOR(GPIO_IOC_MAGIC, 4, int)
#define GPIO_SET_VAL	_IOW(GPIO_IOC_MAGIC, 5, int)
#define GPIO_GET_VAL	_IOR(GPIO_IOC_MAGIC, 6, int)

typedef struct device_context {
	int gpio;
	int val;
	int dir;
	} gpio_dev;
	
	
#endif