
#include <cmsis_os.h>                                           // CMSIS RTOS header file
#define INTERVAL 5																				// Measurement interval in minutes
/*----------------------------------------------------------------------------
 *      Timer: Sample timer functions
 *---------------------------------------------------------------------------*/
 
extern osThreadId t_Client;
/*----- Periodic Timer Example -----*/
static void Timer1_Callback  (void const *arg);           // prototype for timer callback function

struct clock {
	unsigned char hour;
	unsigned char	min;
	unsigned char sec;
};

static struct clock time;

static osTimerId id1;                                     // timer id
static uint32_t  exec1;                                   // argument for the timer call back function
static osTimerDef (Timer1, Timer1_Callback);              // define timers

// Periodic Timer Function
static void Timer1_Callback  (void const *arg)  {
	static unsigned char next_min = 0;
	if (++time.sec == 60)  {
		time.sec = 0;
		if (++time.min == 60 ) {
			time.min = 0;
			if ( ++time.hour == 24 ) {
				time.hour = 0;
			}
		}
	}
	if  (time.min == next_min)  {
		
		next_min = (time.min + INTERVAL) % 60;
		osSignalSet( t_Client, 0x01);
		
	}
}	
	

// Create and Start timers

void Init_Timers (void) {
  osStatus  status;                                        // function return status

	// Initialize time structure
	
	time.sec = 0;
	time.min = 0;
	time.hour = 0;
	
  // Create periodic timer
  exec1 = 1;
  id1 = osTimerCreate (osTimer(Timer1), osTimerPeriodic, &exec1);
  if (id1 != NULL)  {     // Periodic timer created
    // start timer with delay 1000ms
		status = osTimerStart (id1, 1000UL);            
    if (status != osOK)  {
      // Timer could not be started
    }
	}
 

}
