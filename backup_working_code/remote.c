#include <avr/io.h>
#include "nrf24l01.h"
//#define F_CPU 8000000UL
#include <util/delay.h>
#include "uart.h"
#include "spi1.h"

void Initialize(void);
void InitializeIO(void);
void vibrate(void);
void led_blink(void); //toggle the current state of the on-board LED

unsigned char buf[32];

//main routine
void main(void) 
{
   unsigned char data; //register to hold letter received and sent 
   Initialize(); //initialize PLL, IO, SPI, set up nRF24L01 as RX 
	 nrf24l01_flush_rx();
   //main program loop 
   while(1) 
   { 
			//wait until a packet has been received 
			while(!(nrf24l01_irq_pin_active() && nrf24l01_irq_rx_dr_active())); 
      nrf24l01_read_rx_payload(buf, 32); //read the packet into data 
	//		if(buf[0] == 'A')
		//		uart_print_string("GOT DATA\n");					//
			nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01 
   } 
} 

//initialize routine
void Initialize(void) 
{ 
	InitializeIO(); //set up IO (directions and functions)
	uart_init();
	nrf24l01_initialize_debug(true, 32, true); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack disabled
}

void InitializeIO(void)
{
	DDRL |= 1<<PL1;
	DDRL |= 1<<PL0;
	csn_high;
	spi_init();
}

