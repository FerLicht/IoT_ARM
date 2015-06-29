/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/


#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "rl_net.h"

extern osThreadId t_Client;
extern void Client( void const *);
osThreadDef(Client, osPriorityNormal, 2, 0);

extern void Init_Timers( void);

// Board Support includes

#include "Board_LED.h"
#include "ADC.h"

netStatus netstat;


/*----------------------------------------------------------------------------
  Task 1 'ledOnOff1': switches the LED on and off
 *---------------------------------------------------------------------------*/
 void ledOnOff1 (void const *argument) {
  for (;;) {
    LED_On(0);                          /* Turn LED On                    */
    osDelay(500);												/* for half a second							*/
    LED_Off(0);                         /* Turn LED Off                   */
    osDelay(4500);                      /* delay 4500ms                   */
  }

}
osThreadId t_ledOnOff1;
osThreadDef(ledOnOff1, osPriorityNormal, 2, 0); 


/*----------------------------------------------------------------------------
  Task 1 'NetMain': Updates network stat
 *---------------------------------------------------------------------------*/
 void NetMain (void const *argument) {
	while(1){
		
		net_main();
    osThreadYield();                         /* delay 100ms                     */
  }

}
 
osThreadId t_NetMain;
osThreadDef(NetMain, osPriorityNormal, 2, 0);
/*
 * main: initialize and start the system
 */
int main (void) {

  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialize peripherals here

  Init_Timers();
	LED_Initialize();
	ADC_Initialize();
	netstat = net_initialize();

	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);

	t_Client = osThreadCreate (osThread(Client), NULL);
  t_ledOnOff1 = osThreadCreate(osThread(ledOnOff1),  NULL);  /* start task 'ledOnOff'  */	
  t_NetMain = osThreadCreate(osThread(NetMain),  NULL);  /* start task 'N'  */	

	osKernelStart ();                         // start thread execution 
	osThreadTerminate( osThreadGetId ());

	
}
