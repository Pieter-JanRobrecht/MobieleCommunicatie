import serial
import time
thresvalue = 20
ser = serial.Serial('COM3', 9600, timeout = 1)
while True:
    ser.write('30')
    threshold = ser.readline()
    print "Inkomend signaal " 
    if(threshold != ''):
        print "threshhold = "+ threshold
        break
    
