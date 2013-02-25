#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <math.h>
#include "mag_acc.h"
#include "uart.h"
#define pi 3.14159265

////////////////////////////////////
//calibration value 
#define minx -314
#define miny -422
#define minz -218
#define maxx 382
#define maxy 293
#define maxz 397
///////////////////////////////////

static double pitch, roll, heading, tilt_com_heading, mx, my, mz;
static double mx2, my2, mz2;
static double ax, ay, az;
static double ACC, MAG;
static int max = 0;

int read_compass()
{
	ax = (double)(((int)readAccReg(0x29) << 8 | readAccReg(0x28)) /
		      256);
	ay = (double)(((int)readAccReg(0x2b) << 8 | readAccReg(0x2a)) /
		      256);
	az = (double)(((int)readAccReg(0x2d) << 8 | readAccReg(0x2c)) /
		      256);
	mx = (double)((int16_t) readMagReg(0x3) << 8 | readMagReg(0x4));
	mz = (double)((int16_t) readMagReg(0x5) << 8 | readMagReg(0x6));
	my = (double)((int16_t) readMagReg(0x7) << 8 | readMagReg(0x8));
	mx = ((mx - minx) / (maxx - minx)) * 2 - 1.0;
	my = ((my - miny) / (maxy - miny)) * 2 - 1.0;
	mz = ((mz - minz) / (maxz - minz)) * 2 - 1.0;
	ACC = sqrt((ax * ax) + (ay * ay) + (az * az));
	pitch = asin(-ax / ACC);
	roll = asin((ay / ACC) / cos(pitch));
	mx2 = mx * cos(pitch) + mz * sin(pitch);
	my2 =
	    mx * sin(pitch) * sin(roll) + my * cos(roll) -
	    mz * sin(roll) * cos(pitch);
	if (mx2 > 0 && my2 >= 0) {
		return ((int)(atan(my2 / mx2) * 180 / pi));
		//uart_print_num((int)(atan(my / mx) * 180 / pi));
	} else if (mx2 < 0) {
		return (180 + (int)(atan(my2 / mx2) * 180 / pi));
		//uart_print_num(180 + (int)(atan(my / mx) * 180 / pi));
	} else if (mx2 > 0 && my2 <= 0) {
		return (360 + (int)(atan(my2 / mx2) * 180 / pi));
		//uart_print_num(360 + (int)(atan(my / mx) * 180 / pi));
	} else if (mx2 == 0 && my2 < 0) {
		return (90);
		//uart_print_num(90);
	} else if (mx2 == 0 && my2 > 0) {
		return (270);
		//uart_print_num(270);
	}
}

void mag_init()
{
	i2c_init();
	_delay_ms(10);
	writeAccReg(0x20, 0x27);
	writeMagReg(0x02, 0);
	writeMagReg(0x00, 0b00011000);
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) ;
}

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) ;
}

unsigned char i2c_read(unsigned char isLast)
{
	if (isLast == 0)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	else
		TWCR = (1 << TWINT) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) ;
	return TWDR;
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void i2c_init(void)
{
	TWSR = 1;		//PRE 0
	TWBR = 0x47;		//SCL 100k AT 16MHz
	TWCR = 4;		//ENABLE TWI MODULE
}

void writeAccReg(unsigned char reg, unsigned char value)
{
	i2c_start();
	i2c_write(0b00110000);
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}

unsigned char readAccReg(unsigned char reg)
{
	unsigned char v;
	i2c_start();
	i2c_write(0b00110000);
	i2c_write(reg);
	i2c_stop();
	i2c_start();
	i2c_write(0b00110001);
	v = i2c_read(1);
	i2c_stop();
	return v;
}

void writeMagReg(unsigned char reg, unsigned char value)
{
	i2c_start();
	i2c_write(0b00111100);
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}
unsigned char readMagReg(unsigned char reg)
{
	unsigned int v;
	i2c_start();
	i2c_write(0b00111100);
	i2c_write(reg);
	i2c_stop();
	i2c_start();
	i2c_write(0b00111101);
	//_delay_ms(10);
	v = i2c_read(1);
	i2c_stop();
	return v;
}

