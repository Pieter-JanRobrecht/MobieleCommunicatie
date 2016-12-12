import serial
import time
import sys
thresvalue = 50
ser = serial.Serial('COM5', 9600, timeout = 1)
#while True:
ser.write(sys.argv[1])
#    threshold = ser.readline()
#    print "Inkomend signaal " 
#    if(threshold != ''):
#        print "threshhold = "+ threshold
#        break
    
