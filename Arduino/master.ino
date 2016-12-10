#include <SoftwareSerial.h>
#define buttonPin 13
#define redPin 12
#define greenPin 11
#define bluePin 10
#define tempPin A0
#define lightPin A1

SoftwareSerial xbee =  SoftwareSerial(2, 3);
int buttonState = 0;
int lastButtonState = 1;
boolean booly = false;
unsigned long previousMillis = 0;
const long interval = 20000;
byte addr[3][8];
byte master[8] = {};
bool first = true;
bool low = false;
bool high = false;
int light = 0;
int temp = 0;


void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(lightPin, INPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  xbee.begin(9600);
  Serial.begin(9600);
  setRGB(255, 0, 0);
}


void loop() {
  unsigned long currentMillis = millis();

  xbee.listen();
  if (first) {
    delay(1000);
    getMasterAddress();
    first = false;
  }




  if (checkButton() == true) {
    for (int i = 0; i < 3; i++) {
      if (!emptyRow(i)) {
        sendCommandTo(i, 0x02, 0x00, 0x00, 0x00);
      }
    }
  }

  checkReceived();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (low && high) {
      sendDiscover();
    }
    else getMasterAddress();
  }
}

//--------------------------------------------------------------------ReceiveFrames-------------------------

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
//------------------------------------handleFrames-----------------------------------
void handleAtCommandResponse() {
  nextRead();
  byte one = nextRead();
  byte two = nextRead();
  if (one == 0x4E && two == 0x44) {
    Serial.print("ND command-----");
    if (nextRead() == 00) {
      Serial.println(": oke");
      nextRead();
      nextRead();
      saveMacAddress();
      printMacAdress();
      setRGB(0, 255, 0);
    }
    else {
      Serial.println(": error");
    }
  } else if (one == 0x53 && two == 0x48) {
    Serial.print("SH command-----");
    if (nextRead() == 00) {
      Serial.println(": oke");
      for (int i = 0; i < 4; i++) {
        master[i] = nextRead();
      }
      high = true;
      if (low)sendDiscover();
    }
    else {
      Serial.println(": error");
    }

  } else if (one == 0x53 && two == 0x4C) {
    Serial.print("SL command-----");
    if (nextRead() == 00) {
      Serial.println(": oke");
      for (int i = 4; i < 8; i++) {
        master[i] = nextRead();
      }
      low = true;
      if (high)sendDiscover();
    }
    else {
      Serial.println(": error");
    }

  } else {
    Serial.println(one, HEX);
    Serial.println(two, HEX);
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
  switch (var) {
    case 1:
      Serial.println("info ontvangen");
      setRGB(0, 0, 255);
      light = nextRead();
      temp = nextRead();
      Serial.println(light);
      Serial.println(temp);
      break;
    default:

      break;
  }
}

//----------------------------------------sendFrames----------------------------------------------------------
void sendDiscover() {
  Serial.println("send Discover");
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x04);
  xbee.write(0x08);
  xbee.write(0x01);
  xbee.write(0x4E);
  xbee.write(0x44);
  xbee.write(0x64);
}

void sendCommandTo(int row, byte command, byte r, byte g, byte b) {
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x12);
  xbee.write(0x10);
  xbee.write(0x01);
  xbee.write(addr[row][0]);
  xbee.write(addr[row][1]);
  xbee.write(addr[row][2]);
  xbee.write(addr[row][3]);
  xbee.write(addr[row][4]);
  xbee.write(addr[row][5]);
  xbee.write(addr[row][6]);
  xbee.write(addr[row][7]);
  xbee.write(0xFF);
  xbee.write(0xFE);
  xbee.write(0x02);
  xbee.write((byte)0X0);
  xbee.write(command);
  xbee.write(r);
  xbee.write(g);
  xbee.write(b);
  byte checksum = 0x10 + command + 0x02 + 0x01 + r + g + b + addr[row][0] + addr[row][1] + addr[row][2] + addr[row][3] + addr[row][4] + addr[row][5] + addr[row][6] + addr[row][7] + 0xFF + 0xFE;
  checksum = 0xff - checksum;
  xbee.write(checksum);
  Serial.println(checksum);
}

void getMasterAddress() {
  Serial.println("Getting MAC Adress ...");
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x04);
  xbee.write(0x08);
  xbee.write(0x01);
  xbee.write(0x53);
  xbee.write(0x48);
  xbee.write(0x5B);
  delay(20);
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x04);
  xbee.write(0x08);
  xbee.write(0x01);
  xbee.write(0x53);
  xbee.write(0x4C);
  xbee.write(0x57);
}


void sendMasterAddress(int row) {
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x17);
  xbee.write(0x10);
  xbee.write(0x01);
  xbee.write(addr[row][0]);
  xbee.write(addr[row][1]);
  xbee.write(addr[row][2]);
  xbee.write(addr[row][3]);
  xbee.write(addr[row][4]);
  xbee.write(addr[row][5]);
  xbee.write(addr[row][6]);
  xbee.write(addr[row][7]);
  xbee.write(0xFF);
  xbee.write(0xFE);
  xbee.write(0x02);
  xbee.write((byte)0X0);
  xbee.write((byte)0X0); //command 0
  xbee.write(master[0]);
  xbee.write(master[1]);
  xbee.write(master[2]);
  xbee.write(master[3]);
  xbee.write(master[4]);
  xbee.write(master[5]);
  xbee.write(master[6]);
  xbee.write(master[7]);
  byte checksum = 0x10 + 0x02 + 0x01 + master[0] + master[1] + master[2] + master[3] + master[4] + master[5] + master[6] + master[7] + addr[row][0] + addr[row][1] + addr[row][2] + addr[row][3] + addr[row][4] + addr[row][5] + addr[row][6] + addr[row][7] + 0xFF + 0xFE;
  checksum = 0xff - checksum;
  xbee.write(checksum);
}
//---------------------------------------------------------basic's---------------------------------------------------------

void saveMacAddress() {
  for (int j = 0; j < 3; j++) {
    byte arr[8];
    if (emptyRow(j)) {
      for (int i = 0; i < 8; i++) {
        arr[i] = nextRead();
      }
      if (isNewInMatrix(arr)) {
        for (int i = 0; i < 8; i++) {
          addr[j][i] = arr[i];
          //Serial.print(arr[i]);
        }
        sendMasterAddress(j);
        break;
      }
      break;
    }
  }
}

boolean emptyRow(int j) {
  bool empty = true;
  for (int i = 0; i < 8; i++) {
    if (addr[j][i] != (byte)0x0)empty = false;
  }
  return empty;
}

boolean isNewInMatrix(byte arr[]) {
  for (int j = 0; j < 3; j++) {
    bool equalRow = true;
    for (int i = 0; i < 8; i++) {
      if (addr[j][i] != arr[i])equalRow = false;
    }
    if (equalRow)return false;
  }
  return true;
}

void printMacAdress() {
  for (int i = 0; i < 8; i++) {
    Serial.print(master[i]);      Serial.print(" ");
  }
  Serial.println();
  Serial.println("----------------");
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 8; i++) {
      Serial.print(addr[j][i]);      Serial.print(" ");

    }
    Serial.println();
  }
  Serial.println();
}

byte nextRead() {
  byte b = xbee.read();
  //Serial.println(String(b, HEX));
  delay(20);
  return b;
}

void receiveAll() {
  if (xbee.available()) {
    String stringHex =  String(xbee.read(), HEX);
    Serial.println(stringHex);
  }
}

boolean checkButton() {
  booly = false;
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      booly = true;
      Serial.println("presssss----------------------");
    }
  }
  lastButtonState = buttonState;
  return booly;
}

void setRGB(int r, int g, int b) {
  analogWrite(redPin, 255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin, 255 - b);
}




