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
// TODO: insert other include files here



// TODO: insert other definitions and declarations here

void startUpSCT( ){
	LPC_SCT0->CONFIG |= (1 << 17); // two 16-bit timers, auto limit
	LPC_SCT0->CTRL_L |= (72-1) << 5; // set prescaler, SCTimer/PWM clock == 72mhz / 72 == 1mhz
	LPC_SCT0->MATCHREL[0].L = 1000-1; // match 0 @ 1000/1MHz = 1000 us (1 kHz PWM freq)
	LPC_SCT0->MATCHREL[1].L = 950; // match 1 used for duty cycle (initialize at 50% pwm hopefully)

	/*reassigns output to greenled*/
	Chip_SWM_MovablePortPinAssign( SWM_SCT0_OUT0_O,  0,3);	//greenled port0_pin3


	LPC_SCT0->EVENT[0].STATE = 0xFFFFFFFF; // event 0 happens in all states
	LPC_SCT0->EVENT[0].CTRL = (1 << 12); // match 0 condition only
	LPC_SCT0->EVENT[1].STATE = 0xFFFFFFFF; // event 1 happens in all states
	LPC_SCT0->EVENT[1].CTRL = (1 << 0) | (1 << 12); // match 1 condition only
	LPC_SCT0->OUT[0].SET = (1 << 0); // event 0 will set SCTx_OUT0
	LPC_SCT0->OUT[0].CLR = (1 << 1); // event 1 will clear SCTx_OUT0
	LPC_SCT0->CTRL_L &= ~(1 << 2);// unhalt it by clearing bit 2 of CTRL reg
}



void setPWM( int amount ){
	LPC_SCT0->MATCHREL[1].L = amount;
}
uint16_t getPulseWidth(){
	return LPC_SCT0->MATCHREL[1].L;
}

 void convertToPercent( char* returnableBuffer, uint16_t length, uint16_t countervalue){

	 int temp= 1000;
	 temp -= countervalue;
	 temp/=10;

	 snprintf(returnableBuffer, length, "%u %%\r\n", temp);
 }



void buttonPWMTask(void*pvParameters){

    LimitedCounter amount(950, 1, 999);
    DigitalIoPin sw1(0, 17, DigitalIoPin::pullup, true);
    DigitalIoPin sw2(1,11, DigitalIoPin::pullup,true);
    DigitalIoPin sw3(1,9, DigitalIoPin::pullup,true);
    char buf[31];
    char str[31];
    memset(buf,0,31);
    memset(str,0,31);
    /*sw1 increase duty cycle
     * sw3 decrease duty cycle
     *
     * sw2 does it more rapidly*/
    for(;;){

    	if(sw1.read() && !sw2.read() && !sw3.read() ){	// make brighter slow
    		amount--;
    		setPWM(amount);
    		snprintf(buf, 31, "%u\r\n", getPulseWidth());
    		convertToPercent(str,31,(uint16_t) amount);
    		ITM_write(str);
    		memset(buf,0,31);
    		memset(str,0,31);
    		vTaskDelay(100);
    	}
    	else if (!sw1.read() && !sw2.read() && sw3.read() ){ //make dimmer slow
    		amount++;
    		setPWM(amount);
    		snprintf(buf, 31, "%u\r\n", getPulseWidth());
    		convertToPercent(str,31,(uint16_t) amount);
    		ITM_write(str);
    		memset(buf,0,31);
    		memset(str,0,31);
    		vTaskDelay(100);
    	}
    	else if( sw1.read() && sw2.read() && !sw3.read() ){ //MAKE BRIGHTER FAST!
    		amount--;
    		setPWM(amount);
    		snprintf(buf, 31, "%u\r\n", getPulseWidth());
    		convertToPercent(str,31,(uint16_t) amount);
    		ITM_write(str);
    		memset(str,0,31);
    		memset(buf,0,31);
    		vTaskDelay(10);
    	}
    	else if( !sw1.read() && sw2.read() && sw3.read() ){	//MAKE DIMMER FAST
    		amount++;
    		setPWM(amount);
    		snprintf(buf, 31, "%u\r\n", getPulseWidth());
    		convertToPercent(str,31,(uint16_t) amount);
    		ITM_write(str);
//    		ITM_write(buf);
    		memset(buf,0,31);
    		memset(str,0,31);
    		vTaskDelay(10);
    	}
    	else{vTaskDelay(25);}

    }


}

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, false);
#endif
#endif


    // TODO: insert MAIN CODE HERE

    Chip_SCT_Init(LPC_SCT0);
    startUpSCT();
    ITM_init();
    xTaskCreate(buttonPWMTask, "buttonPWMTask",
       		configMINIMAL_STACK_SIZE+128*2, NULL, (tskIDLE_PRIORITY + 1UL),
   			(TaskHandle_t *) NULL);

       // Start the scheduler
       vTaskStartScheduler();





    return 0 ;
}
