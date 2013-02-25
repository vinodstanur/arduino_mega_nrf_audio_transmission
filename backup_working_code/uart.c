#include<avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "uart.h"

#define USART_BAUD 115200ul
#define baud (F_CPU/(8*USART_BAUD))-1

void uart_init()
{
	
	// Set baud rate 
	UBRR0H = (unsigned char)(baud >> 8);
	UBRR0L = (unsigned char)baud;
	/// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	// Set frame format: 8data, 2stop bit
	UCSR0C = (3 << UCSZ00);
	UCSR0A = 1 << U2X0;
	
}

void uart_print_num(int i)
{
	if (i < 0) {
		uart_send_byte('-');
		i *= -1;
	} else if (i == 0) {
		uart_send_byte('0');
		return;
	} else
		uart_send_byte(' ');
	char count = 0;
	unsigned char b[5] = { 0 };
	while (i) {
		b[count++] = i % 10;
		i /= 10;
	}
	for (count = 4; count >= 0; count--)
		uart_send_byte(b[count] + '0');
}

void uart_send_byte(unsigned char c)
{
	while (!(UCSR0A & (1 << UDRE0))) ;
	UDR0 = c;
}

void uart_print_string(unsigned char *p)
{
	while (*p)
		uart_send_byte(*p++);
}
