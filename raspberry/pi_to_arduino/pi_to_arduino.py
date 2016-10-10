import serial

ser = serial.Serial('/dev/ttyACM0',9600)
while True:
    ser.write('2');
