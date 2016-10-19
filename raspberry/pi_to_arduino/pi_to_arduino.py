import serial
import time

ser = serial.Serial('/dev/ttyACM0',9600,timeout=1)
#while True:
ser.write(b'temp')

while True:
    incoming = ser.readline()
    print "Inkomend signaal " + incoming
    #Weg halen 4 bits voor na de komma + 2 extra voor ?
    if(incoming != ''):
        #print "Vier bits weg " + str(int(incoming[:-4], 2))
	print "Zes bits weg: temp="+ str(int(incoming[:-6],2))
	break
