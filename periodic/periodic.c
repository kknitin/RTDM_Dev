#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <task.h>
#include <timer.h>

#include <rtdm/rtdm.h>

RT_TASK demo_task;


void rt_thread(void *arg)
{
		int x,i, ret,cnt = 0;
        RTIME now, previous;
		long task_delay,misses=0;
		struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000; 
        task_delay = 800000;        
        previous = rt_timer_read();
        while (1) {
				
				rt_printf("Task started \n");
				/* Task work load is here (For now sleep is used) */
				
				for(i=0;i<task_delay;i++)
				{
					//clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
					x = i + 5;
					x= (x-5)*3;
					
				
				}
				
				rt_printf("Task ended \n");
				
               ret = rt_task_wait_period(&misses);
                now = rt_timer_read();
                
                rt_printf("Time since last turn: %ld.%06ld ms\n",
                       (long)(now - previous) / 1000000,
                       (long)(now - previous) % 1000000);
				
				rt_printf("%d: \t Misses: %ld\n",ret,misses );
                       previous = now;
					   if(ret != 0)
					   {	cnt++;
						   rt_printf("Missed deadline ! \n");
						   if(cnt > 3)
						   {
							task_delay = 150000;
							misses =0;
						   }
						     //break;
					   }
					   else
					   {
						   cnt = 0 ;
						   task_delay = 800000;
					   }
						 
        }
}

void catch_signal(int sig)
{
	exit(0);
}





int main(int argc, char* argv[])
{		
		int i = atoi (argv[1]);
		
		
		printf("Period is %d ms\n",i);
		i = i* 1000000;
		
        signal(SIGTERM, catch_signal);
        signal(SIGINT, catch_signal);
        /* Avoids memory swapping for this program */
        mlockall(MCL_CURRENT|MCL_FUTURE);


        /*
         * Arguments: &task,
         *            name,
         *            stack size (0=default),
         *            priority,
         *            mode (FPU, start suspended, ...)
         */
        rt_task_create(&demo_task, "trivial", 0, 99, 0);
		rt_task_set_periodic(&demo_task, TM_NOW, (RTIME)i);
        /*
         * Arguments: &task,
         *            task function,
         *            function argument
         */
        rt_task_start(&demo_task, &rt_thread, NULL);
        pause();
        rt_task_delete(&demo_task);
        return 0;
}
