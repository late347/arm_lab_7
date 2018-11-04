/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "task.h"
#include "ITM_write.h"

#include <mutex>
#include "Fmutex.h"
#include "user_vcom.h"

// TODO: insert other definitions and declarations here
#include "DigitalIoPin.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdlib.h>
#include "queue.h"
#include <time.h>
#include <atomic>
#include "ITM_write.h"
#include <cstring>
#include <stdio.h>
#include "LimitedCounter.h"

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}
/* end runtime statictics collection */

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);

}


uint16_t getRed(){
	return LPC_SCT0->MATCHREL[1].H;
}
uint16_t getGreen(){
	return LPC_SCT0->MATCHREL[2].L;
}
uint16_t getBlue(){
	return LPC_SCT1->MATCHREL[1].L;
}


void setRGBValues(uint8_t red, uint8_t green, uint8_t blue){


	red = 255-red;
	green= 255 - green;
	blue=255 -blue;

	LPC_SCT0->MATCHREL[1].H = red;	// sct0 highcounter pulsewidth REDLED
	LPC_SCT0->MATCHREL[2].L = green;	// sct0 lowcounter pulsewidth GREENLED
	LPC_SCT1->MATCHREL[1].L= blue; //sct1 lowcounter pulsewidth BLUELED
}


/* LED1 rgb thread */
static void receive_task(void *pvParameters) {
//	bool LedState = false;
	int charcount=0;
	char buf[65];
	memset(buf,0,65);
	bool readyToPrint=false;
	int j=0; //ind variable for recovering buffer, printing [you]format

	while (1) {
		char str[65];
		char colorbuf[65];
		char redbuf[3];
		char greenbuf[3];
		char bluebuf[3];
		memset(redbuf,0,3);
		memset(greenbuf,0,3);
		memset(bluebuf,0,3);
		memset(colorbuf, 0,65);
		uint32_t len = USB_receive((uint8_t *)str, 64);
		str[len] = 0; /* make sure we have a zero at the end so that we can print the data */
		charcount+=len;
		ITM_write(str);
		for(int k=0; k<len; ++k ){ //note increment j also here!!!
			//echo tha chars into the usb putty terminal EXCEPT \r\n
			//CHARS SHOULD BE IN THE str char array after this loop, but they will be wiped  next iteration
			if(str[k] != '\r' && str[k] != '\n'){
				USB_send((uint8_t*)str,1 );
				buf[j]=str[k];
				++j;
			}
			else{
				readyToPrint=true;
			}
		}
		if(charcount>=64 || readyToPrint){
			USB_send((uint8_t*)"\r[you said:]",12);
			USB_send((uint8_t*)buf, 64);
			USB_send((uint8_t*)"\r\n",2);
			charcount=0;
			j=0;
			readyToPrint=false;

			int red=0, green=0,blue=0;
			unsigned int colorvalues=0;
			auto res = sscanf(buf,"rgb #%x",&colorvalues);
			if( res ==1){ //EDITED CODE CONDITIONS FOR SSCANF now only get green value

				if(colorvalues <= 0xFFFFFF && colorvalues >=0 ){
					red = (colorvalues) & (16711680);	// ANDing with mask  get first 2 hex digits
					red = (red >> 16);
					green = (colorvalues) & (65280); //ANDing with mask get center 2 hex digits
					green = (green >> 8);
					blue = (colorvalues) & (255); //ANDing with mask get last 2 hex digits
					setRGBValues((uint16_t)red, (uint16_t)green, (uint16_t)blue);
					snprintf(colorbuf, 65, "register clrs r=%u, g=%u, b=%u",getRed(),getGreen(),getBlue());
					ITM_write(colorbuf);
					ITM_write("\r\n");
					memset(colorbuf,0,65);
				}
				else{
					//nothing
				}

			}
			else{
				ITM_write("WRONG INPUTn disregarded!\r\n");
			}
			memset(buf,0,65);
		}
	}
}


void setupSCTLED(){
	Chip_SWM_MovablePortPinAssign( SWM_SCT0_OUT0_O,  0,3);	//greenled port0_pin3
	Chip_SWM_MovablePortPinAssign( SWM_SCT0_OUT1_O,  0,25);	//reassigns output to redled
	Chip_SWM_MovablePortPinAssign( SWM_SCT1_OUT0_O,  1,1);	/*reassign output to blueled*/

	/*setup counters sct1low and sct0high and sct0low*/
	LPC_SCT1->CONFIG |= (1<<17); //sct1 lowcounter used autolimit
	LPC_SCT1->CTRL_L |= (72-1)<<5; //prescale lowcounter sct1
	LPC_SCT1->MATCHREL[0].L= 255-1;	//sct1 lowcoutner freq

	LPC_SCT0->CONFIG |= (3<<17); //autolimit lowcounter and highcounter
	LPC_SCT0->CTRL_L |= (72-1) << 5; //prescale lowcounter
	LPC_SCT0->CTRL_H |= (72-1) << 21; //prescale highcounter
	LPC_SCT0->MATCHREL[0].L = 255-1;	//sct0 lowcounter  freq
	LPC_SCT0->MATCHREL[0].H = 255-1;	//sct0 highcounter  freq

	/*set the pulsewidths into matchreload*/
	LPC_SCT0->MATCHREL[2].L = 250;	// sct0 lowcounter pulsewidth GREENLED
	LPC_SCT0->MATCHREL[1].H = 250;	// sct0 highcounter pulsewidth REDLED
	LPC_SCT1->MATCHREL[1].L= 250; //sct1 lowcounter pulsewidth BLUELED

	/*events configured
	 * 1st frequecny events*/
	LPC_SCT0->EVENT[0].STATE = 0xFFFFFFFF;	//all states allowed event0
	LPC_SCT0->EVENT[1].STATE = 0xFFFFFFFF;	//all states allowed event1
	LPC_SCT1->EVENT[2].STATE = 0xFFFFFFFF; //all states allowed event2

	LPC_SCT0->EVENT[0].CTRL= 1<<12;	//event0 sct0 lowcounter frequency match, select reg0
	LPC_SCT0->EVENT[1].CTRL = (1<<4) |(1<<12)  ; //event1 sct0 highcounter frequency match, select reg0
	LPC_SCT1->EVENT[2].CTRL = (1<<12); //event2 sct1 lowcounter frequency match, select reg0

	/*2ndly we have the COUNTER-MATCH events*/
	LPC_SCT0->EVENT[3].STATE= 0xFFFFFFFF;
	LPC_SCT0->EVENT[4].STATE= 0xFFFFFFFF;
	LPC_SCT1->EVENT[5].STATE= 0xFFFFFFFF;

	LPC_SCT0->EVENT[3].CTRL = (1<<12) | (2); //event3 sct0 lowcounter match, select reg2
	LPC_SCT0->EVENT[4].CTRL = (1<<4) | (1<<12) | (1); //event4 sct0 highcounter match, HEVENTbitTrue,  select reg1
	LPC_SCT1->EVENT[5].CTRL = (1<<12) | (1); //event5 sct1 lowcounter match, select reg1 (default)

	/*set outputs*/
	LPC_SCT0->OUT[0].SET =  (1<<0); //event0 sets  sct0 output0
	LPC_SCT0->OUT[1].SET = (1<<1); //event1 sets sct0 output1 //1<<0
	LPC_SCT1->OUT[0].SET = (1<<2); //event2 sets sct1 output0

	/*clear outputs with countermatches*/
	LPC_SCT0->OUT[0].CLR = 	1<<3;			//event3 clears sct0 output0
	LPC_SCT0->OUT[1].CLR =	1<<4;		//event4 clears sct0 output1
	LPC_SCT1->OUT[0].CLR =	1<<5;		//event5 clears sct1 output0

	/*unhalt timers*/
	LPC_SCT0->CTRL_L &=  ~(1<<2);
	LPC_SCT0->CTRL_H &= ~(1<<2);
	LPC_SCT1->CTRL_L &= ~(1<<2);

}


//void startUpGreenSCT( ){
//	LPC_SCT0->CONFIG |= (1 << 17); // two 16-bit timers, auto limit green = Lcounter
//	LPC_SCT0->CTRL_L |= (72-1) << 5; // set prescaler, SCTimer/PWM clock == 72mhz / 72 == 1mhz
//	LPC_SCT0->MATCHREL[0].L = 1000-1; // match 0 @ 1000/1MHz = 1000 us (1 kHz PWM freq) lowcounter
//	LPC_SCT0->MATCHREL[1].L = 950; // match 1 used for duty cycle (initialize at 5% pwm hopefully) lowcounter
//
//	/*reassigns output to greenled*/
//	Chip_SWM_MovablePortPinAssign( SWM_SCT0_OUT0_O,  0,3);	//greenled port0_pin3
//	LPC_SCT0->EVENT[0].STATE = 0xFFFFFFFF; // event 0 happens in all states
//	LPC_SCT0->EVENT[0].CTRL = (1 << 12); // match 0 condition only HEVENT bit==low lowcounter
//	LPC_SCT0->EVENT[1].STATE = 0xFFFFFFFF; // event 1 happens in all states
//	LPC_SCT0->EVENT[1].CTRL = (1 << 0) | (1 << 12); //  select reg1, match 1 condition only,, (HEVENTbit=0)
//	LPC_SCT0->OUT[0].SET = (1 << 0); // event 0 will set SCTx_OUT0
//	LPC_SCT0->OUT[0].CLR = (1 << 1); // event 1 will clear SCTx_OUT0
//	LPC_SCT0->CTRL_L &= ~(1 << 2);// unhalt it by clearing bit 2 of CTRL reg
//}
//void startUpRedSCT(){
//	LPC_SCT0->CONFIG |= ( 1 << 18); //two 16bit timers red and green use them, enable automatchH match[0], red == Hcounter
//	LPC_SCT0->CTRL_H |= (72-1) << 21; //prescale Hcounter
//	LPC_SCT0->MATCHREL[0].H= 1000-1; //match0 causes event0, at 1,0khz autoreset highcounter
//	LPC_SCT0->MATCHREL[2].H= 950; //match2 event2 Hcounter
//
//	//reassigns output to redled
//	Chip_SWM_MovablePortPinAssign( SWM_SCT0_OUT1_O,  0,25);
//	LPC_SCT0->EVENT[0].STATE= 0xFFFFFFFF;
//	LPC_SCT0->EVENT[0].CTRL = (1 << 12) | (16); //match 0 condition and Hcounter HEVENT bit, matchsel == 0register
//	LPC_SCT0->EVENT[2].STATE = 0xFFFFFFFF; // event 2 happens in all states
//	LPC_SCT0->EVENT[2].CTRL = (2) | (1 << 12) | (16); // match 2 condition only, Hcounter HEVENTbit, and select reg2
//	LPC_SCT0->OUT[1].SET = (1 << 0); // event 0 will set SCTx_OUT1
//	LPC_SCT0->OUT[1].CLR = (1 << 2); // event 2 will clear SCTx_OUT1
//	LPC_SCT0->CTRL_H &= ~(1 << 18);// unhalt it by clearing bit 18 of CTRL reg Hcounter
//}
//
//void startUpBlueSCT(){
//	LPC_SCT1->CONFIG |= (1<<17);	//Lcounter setup with two counters
//	LPC_SCT1->CTRL_L |= (72-1)<<5; // Lcounter prescaler
//	LPC_SCT1->MATCHREL[0].L = 1000-1;	//match 0 cause by 1khz event0
//	LPC_SCT1->MATCHREL[3].L=950;	// match 3 event 3
//
//	/*reassign output to blueled*/
//	Chip_SWM_MovablePortPinAssign( SWM_SCT1_OUT0_O,  1,1);
//	LPC_SCT1->EVENT[0].STATE = 0xFFFFFFFF;
//	LPC_SCT1->EVENT[0].CTRL = (1<<12); //match lowcounter, HEVENTbit=0
//	LPC_SCT1->EVENT[3].STATE= 0xFFFFFFFF;
//	LPC_SCT1->EVENT[3].CTRL = (3) | (1<<12);   //select reg3, heventbit=0, match3 condition only
//	LPC_SCT1->OUT[0].SET= (1<<0); //event 0 will set sct.1_out.0
//	LPC_SCT1->OUT[0].CLR = (1<<3); //event 3 will clear sct.1_out.0
//	LPC_SCT1->CTRL_L &= ~(1<<2); //clear bit 2 of low counter from ctrl reg
//}





int main(void) {
	Chip_SCT_Init(LPC_SCT0);
	Chip_SCT_Init(LPC_SCT1);
	prvSetupHardware();
	ITM_init();
//startUpGreenSCT();
//startUpRedSCT();
//startUpBlueSCT();

	setupSCTLED();

	/* LED1 toggle thread */
	xTaskCreate(receive_task, "Rx",
				configMINIMAL_STACK_SIZE * 8, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(cdc_task, "CDC",
				configMINIMAL_STACK_SIZE * 5, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);


	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
