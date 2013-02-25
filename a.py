#!/usr/bin/python
import sys,time,serial

ser=serial.Serial()
ser.baudrate=115200
ser.port=sys.argv[1]
ser.timeou=1
ser.open()
while 1:
	print ser.read()
	
