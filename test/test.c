#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
//#include <pthread.h>
//#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>


static void panic(char *message)
{
        fprintf(stderr,"Fatal error: %s\n", message);
        exit(1);
}



static void dumptimestamps(int unused)
{
        
        exit(0);
}

// Initialize a GPIO pin in Linux using the sysfs interface
FILE *init_gpio(int gpioport)
{
        // Export the pin to the GPIO directory
        FILE *fp = fopen("/sys/class/gpio/export","w");
        fprintf(fp,"%d",gpioport);
        fclose(fp);

        // Set the pin as an output
        char filename[256];
        sprintf(filename,"/sys/class/gpio/gpio%d/direction",gpioport);
        fp = fopen(filename,"w");
        if(!fp){
                panic("Could not open gpio file");
        }
        fprintf(fp,"out");
        fclose(fp);

        // Open the value file and return a pointer to it.
        sprintf(filename,"/sys/class/gpio/gpio%d/value",gpioport);
        fp = fopen(filename,"w");
        if(!fp){
                panic("Could not open gpio file");
        }
        return fp;
}

// Given a FP in the stepper struct, set the I/O pin
// to the specified value. Uses the sysfs GPIO interface.
void setiopin(FILE *fp, int val)
{
        fprintf(fp,"%d\n",val);
        fflush(fp);
}

// Demo program for running a stepper connected to the Raspberry PI
// platform.
int main(int argc, char **argv)
{
        int delay = 1000000; // Note: delay in us here
        FILE *pin0 = init_gpio(337);
        

        signal(SIGINT, dumptimestamps);

        while(1){
                usleep(delay); 
				setiopin(pin0,1);
				printf("LED set ON\n");
                usleep(delay);  
				setiopin(pin0,0);
				printf("LED set Off\n");
               
        }
}