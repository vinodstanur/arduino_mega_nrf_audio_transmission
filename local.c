#include <avr/io.h>
#include "nrf24l01.h"
//#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "uart.h"
#include "nrf24l01.h"
#include "spi1.h"

void Initialize(void);
void InitializeIO(void);
void ToggleLED(void); 
void vibrate();
void system_shutdown(void);
void adc_init(void);
uint16_t adc_read(unsigned char channel);
void wd_reset();
void vibrate_left();
void vibrate_right();

unsigned char ttt;


unsigned int countt;

unsigned char buf[32];

volatile unsigned char uart_rx[5], pass, indx;

char nrf_send(unsigned char *data)
{
	unsigned char d=0;
	do {
		nrf24l01_read_register(nrf24l01_CD, &d, 1);
	}while(d);
	nrf24l01_write_tx_payload(data, 32, true); //transmit received char over RF
	//wait until the packet has been sent or the maximum number of retries has been active
	while(!(nrf24l01_irq_pin_active() && (nrf24l01_irq_tx_ds_active() || nrf24l01_irq_max_rt_active())));
	//check to see if the maximum number of retries has been hit.  if not, wait for the RX device
	// to send the char back.  if so, assume the packet is lost and send "*" back to UART
	if(!nrf24l01_irq_max_rt_active()){
		//uart_print_string("ack ok\n");
		countt++;
		if(countt>=1024) {
						countt=0;
						uart_print_string("32KB sent!\n");
		}
	}
	else {
		nrf24l01_flush_tx(); //get the unsent character out of the TX FIFO
		nrf24l01_irq_clear_all(); //clear all interrupts
		uart_send_byte('*'); //print "*" to the screen to show that the receiver did not receive the packet
	}
}

ISR(USART0_RX_vect)
{
}

//main routine
void main(void)
{
		volatile int  ccc=0;
	unsigned char data; //register to hold letter sent and received
	unsigned int count; //counter for for loop
	Initialize(); //initialize IO, UART, SPI, set up nRF24L01 as TX
	sei();
	uart_init();
	uart_print_string("CHIP RESET!\n");
	nrf24l01_flush_tx(); //get the unsent character out of the TX FIFO
	/*while(1) {
					csn_high;
					ce_low;
					_delay_ms(100);
					csn_low;
					ce_high;
					_delay_ms(100);
	}
	*/
	buf[0]='A';
	buf[1]='B';
	buf[2]='C';

	while(1){
					nrf_send(buf);
/*		uart_print_string("compass value =");
			uart_print_num(read_compass());
			uart_send_byte('\n');
			_delay_ms(100);
*/	}
}

//initialize routine
void Initialize(void)
{
	InitializeIO(); //set up IO (directions and functions)
	uart_init();
	nrf24l01_initialize_debug(false, 32, true);
	//nrf24l01_initialize_debug(false, 1, true); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack enabled
}

//initialize IO pins
void InitializeIO(void)
{
	DDRL |= 1<<PL1;
	DDRL |= 1<<PL0;
	csn_high;
	spi_init();
}	

