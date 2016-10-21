/*
  XBEE_XCTU

  Program used to create a uart bridge between the virtual COM port of the arduino
  and the XBEE module on the DraMCo shield.
  
  Connect the RX pin with pint 2 and TX with pin 3

  When using XCTU (http://www.digi.com/products/xbee-rf-solutions/xctu-software/xctu#productsupport-utilities)
  select the arduino as the serial device.
*/

#include <SoftwareSerial.h>
 
SoftwareSerial XBee(2, 3); // RX, TX

void setup(void) {
  // start hard en software serial port
  Serial.begin(9600);
  XBee.begin(9600);
}

void loop(void) {
  if (XBee.available()) {
    Serial.write(XBee.read());
  }
  if (Serial.available()) {
    XBee.write(Serial.read());
  }
}
