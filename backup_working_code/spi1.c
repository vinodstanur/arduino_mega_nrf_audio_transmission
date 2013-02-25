#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spi1.h"
#include "uart.h"

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB3
#define DD_MOSI     DDB2
#define DD_SS       DDB0
#define DD_SCK      DDB1


void spi_init()
// Initialize pins for spi communication
{
    DDR_SPI &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
    // Define the following pins as output
    DDR_SPI |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK));

	
    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
            (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

    SPSR = (1<<SPI2X);              // Double Clock Rate   
	_delay_ms(10);
}

unsigned char spi1_send_read_byte(unsigned char byte)
{
    SPDR = byte;
		while((SPSR & (1<<SPIF))==0);
		return SPDR;
}	
