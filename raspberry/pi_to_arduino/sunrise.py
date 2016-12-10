import serial
import time

ser = serial.Serial('COM5', 9600, timeout=1)


while True:
    ser.write('sunrise1')
    sunrise = ser.readline()
    #print "Inkomend signaal " 
    if(sunrise != ''):
        print "sunrise = "+ sunrise
        break
