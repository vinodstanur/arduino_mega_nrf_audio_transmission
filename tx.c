#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <uart.h>
#include "nrf24l01.h"

void Initialize(void);
void InitializeIO(void);
void ToggleLED(void); 

//main routine
void main(void)
{
	unsigned char data; //register to hold letter sent and received
	unsigned int count; //counter for for loop
	
	Initialize(); //initialize IO, UART, SPI, set up nRF24L01 as TX

	while(1)
	{
		  
		//check UART status register to see if data has been received.  if so, process
		while(1)
		{
			data = 'A';
			
			nrf24l01_write_tx_payload(&data, 1, true); //transmit received char over RF

			//wait until the packet has been sent or the maximum number of retries has been reached
			while(!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()));

			nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01
			nrf24l01_set_as_rx(true); //change the device to an RX to get the character back from the other 24L01

			//wait a while to see if we get the data back (change the loop maximum and the lower if
			//  argument (should be loop maximum - 1) to lengthen or shorten this time frame
			for(count = 0; count < 20000; count++)
			{
				//check to see if the data has been received.  if so, get the data and exit the loop.
				//  if the loop is at its last count, assume the packet has been lost and set the data
				//  to go to the UART to "?".  If neither of these is true, keep looping.
				if((nrf24l01_irq_pin_active() && nrf24l01_irq_rx_dr_active()))
				{
					nrf24l01_read_rx_payload(&data, 1); //get the payload into data
					break;
				}
				
				//if loop is on its last iteration, assume packet has been lost.
				if(count == 19999)
					data = '?';
			}
			
			nrf24l01_irq_clear_all(); //clear interrupts again
			uart_send_byte(data);
			DelayUS(130); //wait for receiver to come from standby to RX
			nrf24l01_set_as_tx(); //resume normal operation as a TX
											
			ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
		}
	}
}

//initialize routine
void Initialize(void)
{
	InitializeIO(); //set up IO (directions and functions)
	uart_init();
	spi_init();
	nrf24l01_initialize_debug(false, 1, false); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack disabled
}

//initialize IO pins
void InitializeIO(void)
{
	DDRD |= 1<<PD3;
	PORTD |= 1<<PD3;  //LED PIN
	DDRB &= ~(1<<PB2); //IRQ pin
	spi_init();
	csn_low;
}	

//toggles on-board LED
void ToggleLED(void)
{
	PORTD ^= 1<<PD3;
}

	
