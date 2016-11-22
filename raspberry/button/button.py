import serial
import time

ser = serial.Serial('/dev/ttyACM1',9600,timeout=1)
print "Verzenden button"
ser.write('button')
print "Verzonden"

while True:
    time.sleep(0.2)
    print "Verzenden button"
    ser.write('button') 
    print "Verzonden"
    

    incoming = ser.readline()
    print "Inkomend signaal " + incoming
    if(incoming != ''):
	if (incoming == "pressed"):
		print "pressed"
	if(incoming == "released"):
		print "released"
	
