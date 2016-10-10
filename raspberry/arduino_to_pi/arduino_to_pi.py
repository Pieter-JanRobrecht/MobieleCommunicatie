import serial

ser = serial.Serial('/dev/ttyACM0',9600,timeout=1)
s = [0]
while True:
    try:
        read_serial=ser.readline()
    except SerialException:
        print "Oops"
    #s[0] = str(int (ser.readline(),16))
    #print s[0]
    print read_serial
    #Stuurt blijkbaar op het einde nog 2 spaties door?
    read_serial = read_serial[:-6]
    print read_serial
    print int(read_serial, 2)
