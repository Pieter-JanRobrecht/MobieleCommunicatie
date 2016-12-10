import serial
import time

#ser = serial.Serial('/dev/cu.usbmodemfa131', 9600, timeout=1)
ser = serial.Serial()
ser.port = '/dev/cu.usbmodemfa131'
ser.baudrate = 9600
ser.timeout = 1
ser.setDTR(False)
ser.open()#while True:
#ser.write('sensors')

while True:
    ser.write('t1')
    temp1 = ser.readline()
    #print "Inkomend signaal " 
    if(temp1 != ''):
        print "temp1 = "+ temp1
        break

while True:
    ser.write('t2')
    temp2 = ser.readline()
    #print "Inkomend signaal " 
    if(temp2 != ''):
        print "temp2 = "+ temp2
        break
    
while True:
    ser.write('t3')
    temp3 = ser.readline()
    #print "Inkomend signaal " 
    if(temp3 != ''):
        print "temp3 = "+ temp3
        break

while True:
    ser.write('l1')
    light1 = ser.readline()
    #print "Inkomend signaal " 
    if(light1 != ''):
        print "light1 = "+ light1
        break

while True:
    ser.write('l2')
    light2 = ser.readline()
    #print "Inkomend signaal " 
    if(light2 != ''):
        print "light2 = "+ light2
        break

while True:
    ser.write('l3')
    light3 = ser.readline()
    #print "Inkomend signaal " 
    if(light3 != ''):
        print "light3 = "+ light3
        ser.close()
        break
ser.close()
