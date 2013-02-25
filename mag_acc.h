#include <avr/io.h>
unsigned char i2c_read(unsigned char isLast);
void mag_init();
void i2c_write(unsigned char data);
void i2c_start(void);
void i2c_stop();
void i2c_init();
void writeAccReg(unsigned char reg, unsigned char value);
unsigned char readAccReg(unsigned char reg);
void writeMagReg(unsigned char reg, unsigned char value);
unsigned char readMagReg(unsigned char reg);
int read_compass();
