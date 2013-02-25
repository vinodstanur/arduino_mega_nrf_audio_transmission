# AVR-GCC Makefile
SOURCES1 = nrf24l01.c spi1.c uart.c
SOURCES2 = nrf24l01.c spi1.c uart.c
MAIN1=remote.c
MAIN2=local.c
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2560

CFLAGS=-mmcu=$(MMCU)

remote.hex local.hex: local.out remote.out
		$(OBJCOPY) -j .text -j .data -O ihex local.out local.hex
		$(OBJCOPY) -j .text -j .data -O ihex remote.out remote.hex
local.out remote.out: local.c remote.c $(SOURCES1) $(SOURCES2)
		$(CC) $(CFLAGS) -Os -I./ -o local.out local.c $(SOURCES1)
		$(CC) $(CFLAGS) -Os -I./ -o remote.out remote.c $(SOURCES2)

pt: local.hex
	avrdude -V -F -c stk500v2 -p m2560 -b 115200 -P /dev/ttyACM0 -U flash:w:local.hex
pr: remote.hex
	avrdude -V -F -c stk500v2 -p m2560 -b 115200 -P /dev/ttyACM1 -U flash:w:remote.hex


clean:
	rm -f *.out
	rm -f *.hex
	rm -f *.s
