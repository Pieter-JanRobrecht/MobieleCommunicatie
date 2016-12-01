#include <SoftwareSerial.h>
#define redPin 12
#define greenPin 11
#define bluePin 10
#define tempPin A0
#define lightPin A1

SoftwareSerial xbee =  SoftwareSerial(2, 3);
byte master[8];


void setup() {
  xbee.begin(9600);
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(tempPin, INPUT);
  pinMode(lightPin, INPUT);

  setRGB(255, 0, 0);
}

void loop() {
  xbee.listen();
  checkReceived();
}
//---------------------------------------ontvangen van berichten------
void checkReceived() {
  if (xbee.available()) {
    if (nextRead() == 0x7E) {
      nextRead();     //length
      nextRead();      //length
      byte test = nextRead();
      if (test == 0x88) {
        Serial.println("AT Command Response------------------ ");
        //Serial.println(test);
        handleAtCommandResponse();
      } else if (test == 0x90) {
        Serial.println("Transmit Request------------------ ");
        //Serial.println(test);
        handleTransmitRequest();
      } else if (test == 0x8B) {
        Serial.println("Transmit Status------------------ ");
        handleTransmitStatus();
      }
      else {
        Serial.print("not: ");
        Serial.println(test);
      }
    }
    nextRead();
  }
}

void receiveAll() {
  if (xbee.available()) {
    String stringHex =  String(xbee.read(), HEX);
    Serial.println(stringHex);
  }
}

//------------------------------------handle frames----------------

void handleAtCommandResponse() {
  nextRead();
  if (nextRead() == 0x4E && nextRead() == 0x44) {
    Serial.print("ND command-----");
    if (nextRead() == 00) {
      Serial.println(": oke");
      nextRead();
      nextRead();
      //saveMacAddress();
      //printMacAdress();
    }
    else {
      Serial.println(": error");
    }
  }
}

void handleTransmitRequest() {
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//source address
  nextRead();//receive options
  byte var =   nextRead();
  byte r = 0;
  byte g = 0;
  byte b = 0;
  switch (var) {
    case 0:
      Serial.println("Address master oplsaan");
      for (int i = 0; i < 8; i++) {
        master[i] = nextRead();
      }
      setRGB(0, 255, 0);
      break;
    case 1:
      Serial.println("rgb aanpassen");
      r = nextRead();
      g = nextRead();
      b = nextRead();
      setRGB(r, g, b);
      break;
    case 2:
      Serial.println("motor aanpassen");
      r = nextRead();
      setMotor(r);
      break;
    case 3:
      Serial.println("alle info opvragen en versturen");
      setRGB(200, 20, 20);
      sendInfo();
      break;
    default:
      break;
  }
}

void handleTransmitStatus() {
  nextRead();
  nextRead();
  nextRead();
  nextRead();
  byte test = nextRead();
  if (test == 0x00) {
    Serial.println("Transmit :Succes");
  }
  else {
    Serial.print("Transmit: error ");
    Serial.println(test, HEX);
  }
}
//------------------------------------versturen berichten------

void sendInfo() {
  int light = getLight();
  int temp = getTemp();
  Serial.println(light);
  Serial.println(temp);
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x11);
  xbee.write(0x10);
  xbee.write(0x01);
  xbee.write(master[0]);
  xbee.write(master[1]);
  xbee.write(master[2]);
  xbee.write(master[3]);
  xbee.write(master[4]);
  xbee.write(master[5]);
  xbee.write(master[6]);
  xbee.write(master[7]);
  xbee.write(0xFF);
  xbee.write(0xFE);
  xbee.write(0x02);
  xbee.write((byte)0X0);
  xbee.write(0x01);//command
  xbee.write(light);
  xbee.write(temp);
  byte checksum = 0x10 + 0x01 + 0x02 + 0x01 + light + temp + master[0] + master[1] + master[2] + master[3] + master[4] + master[5] + master[6] + master[7] + 0xFF + 0xFE;
  checksum = 0xff - checksum;
  xbee.write(checksum);
  Serial.println(checksum);
}


//----------------------------------------basic's-------------------

byte nextRead() {
  byte nr = xbee.read();
  //Serial.println(String(b, HEX));
  delay(20);
  return nr;
}

void setRGB(int r, int g, int b) {
  analogWrite(redPin, 255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin, 255 - b);
}

int getTemp() {
  return analogRead(tempPin);
}

int getLight() {
  return analogRead(lightPin);
}

void setMotor(int pwm){
  //TODO
}

}

