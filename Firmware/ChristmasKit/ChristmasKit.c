/********************************************************************************
						Christmas Board Code
					  Copyright Sparfun Electronics
					    Written by Ryan Owens
						      12/4/2008
*********************************************************************************
*/

//***************************************
//			Include Libraries
//***************************************
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//***************************************
//		Macros and Definitions
//***************************************
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

//LED Output Pins (PortB)
#define LED1	0
#define LED4	2
#define LED2	1
#define LED3	3

//LED Macros
#define LED1_ON()	sbi(PORTB, LED1)
#define LED1_OFF()	cbi(PORTB, LED1)

#define LED2_ON()	sbi(PORTB, LED2)
#define LED2_OFF()	cbi(PORTB, LED2)

#define LED3_ON()	sbi(PORTB, LED3)
#define LED3_OFF()	cbi(PORTB, LED3)

#define LED4_ON()	sbi(PORTB, LED4)
#define LED4_OFF()	cbi(PORTB, LED4)

//Program Definitions
#define INT_FREQ 4000
#define FRAME_DEPTH 511

//***************************************
//			Global Variables
//***************************************
volatile int led_brightness;
volatile unsigned long int light_up=0;
volatile int frame_count=0;
volatile int LED=1;

//***************************************
//		Function Prototypes
//***************************************
void ioinit (void);      // initializes IO
void delay_ms(uint16_t x);

//***************************************
//			Main Code
//***************************************
ISR(SIG_OUTPUT_COMPARE0A){
	cli();
	
	
	if(frame_count < FRAME_DEPTH)frame_count+=1;
	else frame_count=0;
	
	if(light_up>0){
		light_up-=1;
		if(led_brightness > frame_count){
			switch (LED)
			{
				case 1:
					LED1_ON();
					break;
				case 2:
					LED2_ON();
					break;
				case 3:
					LED3_ON();
					break;
			}
		}
		else{
			LED1_OFF();
			LED2_OFF();
			LED3_OFF();
		}
	}
	else{
		LED1_OFF();
		LED2_OFF();
		LED3_OFF();
	}
	sei();
}

int main (void)
{
	ioinit();
	light_up=0;

	while(1){
		LED4_ON();
		for(int s_led=2; s_led<5; s_led++)
		{
			LED=s_led;
			for(int brightness=0; brightness <= 100; brightness+=5)
			{
				light_up=2500;
				led_brightness=brightness;
				while(light_up>0);
			}
			for(int brightness=100; brightness >= 0; brightness-=5)
			{
				light_up=2500;
				led_brightness=brightness;
				while(light_up>0);
			}		
		}
		LED4_OFF();
		for(int i=0; i<8; i++)delay_ms(30000);
	}
	
return 0;	
}

void ioinit (void) 
{

	//Set up the I/O
    //1 = output, 0 = input
	DDRB = (1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4); //Enable the LED pins as outputs

	TCCR0A=(1<<WGM01);	//Clear timer on Compare
	TCCR0B=(1<<CS01); //Divide Clock(8MHZ) by 8
	OCR0A=30;	//Set Interrupt frequency to Timer(1mHz)/100=10kHz
	TIMSK = (1<<OCIE0A);		//Interrupt when timer0=OCA
	
	sei(); //Enable interrupts	//Enable Interrupts
}

//General short delays
void delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--){
    for ( y = 0 ; y < 10 ; y++){
      for ( z = 0 ; z < 10 ; z++){
        asm volatile ("nop");
      }
    }
  }
}
