#include <string.h>
#include <stdio.h>
#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "rl_net.h"
#include "ADC.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//   <h>Remote IP Address
//   ====================
//
//     <o>IP1: Address byte 1 <0-255>
//     <i> Default: 192
#define IP1            184

//     <o>IP2: Address byte 2 <0-255>
//     <i> Default: 168
#define IP2            106

//     <o>IP3: Address byte 3 <0-255>
//     <i> Default: 0
#define IP3            153

//     <o>IP4: Address byte 4 <0-255>
//     <i> Default: 100
#define IP4            149

//   </h>

//   <o>Remote Port <1-65535>
//   <i> Do not set number of port too small,
//   <i> maybe it is already used.
//   <i> Default: 1001
#define PORT_NUM       80
/*
#define IP1            192
#define IP2            168
#define IP3            2
#define IP4            80
#define PORT_NUM       14000
*/
//   <o>Communication Protocol <0=> Stream (TCP) <1=> Datagram (UDP)
//   <i> Selecet a protocol for sending data.
#define PROTOCOL       0

#if (PROTOCOL == 0)
 #define SOCKTYPE   SOCK_STREAM
#else
 #define SOCKTYPE   SOCK_DGRAM
#endif



osThreadId t_Client;

void Client (void const *arg) {
  SOCKADDR_IN addr;
  int sock;
	static uint16_t adc_val1, adc_val2;

	volatile int res1, res2;

	char header[] = "POST /update HTTP/1.1\r\nHost: api.thingspeak.com:80\r\n"
				"Accept-Encoding: identity\r\n"
				"Content-type: application/x-www-form-urlencoded\r\n"
				"Accept: text/plain\r\n"
				"X-THINGSPEAKAPIKEY: XVW54JW29JN1ZQ4W\r\n"
				"Content-Length: ";
	char content_length[8];
	char payload[40];
 	char rbuf[80];

  while (1) {

		adc_val1 = 0;
		adc_val2 = 0;

// Four consecutive conversions to filter out noise		
		
		for (uint8_t i = 0; i < 4 ; i++) {
			ADC_StartConversion();
			while (ADC_ConversionDone() != 0);
			adc_val1 += ADC_GetValue(0);
			adc_val2 += ADC_GetValue(1);
		}
		
		adc_val1 /= 4;		// Values are averaged
		adc_val2 /= 4;

		sprintf(payload,"field1=%d&field2=%4.1f\r\n", (adc_val1>>4),(adc_val2*327.)/4096);		
		sprintf(content_length,"%d\r\n\r\n", strlen(payload));			
		
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addr.sin_port      = htons(PORT_NUM);
    addr.sin_family    = PF_INET;
    addr.sin_addr.s_b1 = IP1;
    addr.sin_addr.s_b2 = IP2;
    addr.sin_addr.s_b3 = IP3;
    addr.sin_addr.s_b4 = IP4;
		
    res1 = connect (sock, (SOCKADDR *)&addr, sizeof (addr));

		res2 = send (sock, (char *)&header, strlen(header), 0);
    res2 = send (sock, (char *)&content_length, strlen(content_length), 0);
    res2 = send (sock, (char *)&payload, strlen(payload), 0);

		while (1) {
      res2 = recv (sock, rbuf, sizeof (rbuf), 0);
      if (res2 <= 0) {
        break;
      }
		}

		closesocket (sock);

      osSignalWait (0x01, osWaitForever);		// Wait for next timer expiration
			osSignalClear (t_Client, 0x01);				// Clear signal and continue

  }
}

