#include <SoftwareSerial.h>
#define redPin 12
#define greenPin 11
#define bluePin 10
#define tempPin A0
#define lightPin A1

SoftwareSerial xbee =  SoftwareSerial(2, 3);
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 10200;
const long interval2 = 2500;
bool gotMaster = false;
bool low = false;
bool high = false;
bool first = true;
byte addr[3][8];
byte master[8];
byte source[8];
int index = 0;
bool connectionMode = true;



void setup() {
  xbee.begin(9600);
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(tempPin, INPUT);
  pinMode(lightPin, INPUT);

  if(connectionMode)setRGB(255, 0, 0);
}

void loop() {
  unsigned long currentMillis = millis();
  xbee.listen();
  checkReceived();

  if (first) {
    sendDiscover();
    delay(1000);
    first = false;
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendDiscover();
  }

  if (currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    if (index > 2)index = 0;
    delay(10);
    if (!emptyRow(index))sendCheck(index, 0xFE, 0x10, 0x34, 0x69);
    if (!emptyRow(index) && !isMaster(index) && gotMaster)sendMasterAddress(index);
    index++;

    bool zero = true;
    for(int i =0;i<3;i++){
      if(!emptyRow(i))zero=false;
    }
    if(zero&&connectionMode)setRGB(255,0,0);
  }


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
  byte one = nextRead();
  byte two = nextRead();
  if (one == 0x4E && two == 0x44) {
    Serial.print("ND command-----");
    if (nextRead() == 00) {
      Serial.println(": oke");
      nextRead();
      nextRead();
      saveMacAddress();
      if(connectionMode)setRGB(200, 200, 0);
      if(gotMaster)setRGB(0, 255, 0);

    }
    
    else {
      Serial.println(": error");
    }
  }
//  } else if (one == 0x53 && two == 0x48) {
//    Serial.print("SH command-----");
//    if (nextRead() == 00) {
//      Serial.println(": oke");
//      for (int i = 0; i < 4; i++) {
//        own[i] = nextRead();
//      }
//      high = true;
//      if (low)sendDiscover();
//    }
//    else {
//      Serial.println(": error");
//    }
//
//  } else if (one == 0x53 && two == 0x4C) {
//    Serial.print("SL command-----");
//    if (nextRead() == 00) {
//      Serial.println(": oke");
//      for (int i = 4; i < 8; i++) {
//        own[i] = nextRead();
//      }
//      low = true;
//      if (high)sendDiscover();
//    }
//    else {
//      Serial.println(": error");
//    }

//  } else {
//    Serial.println(one, HEX);
//    Serial.println(two, HEX);
//  }
}

void handleTransmitRequest() {
      
  for (int i = 0; i < 8; i++) {
    source[i] = nextRead();
  }

  nextRead();//source address 16-bit
  nextRead();//source address 16-bit
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
      if(connectionMode)setRGB(0, 255, 0);
      saveMacAddress();
      gotMaster = true;
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
      if(!gotMaster){
        for (int i = 0; i < 8; i++) {
          master[i] = source[i];
        }
        gotMaster=true;
      }
      if(connectionMode)setRGB(200, 20, 20);
      sendInfo();
      break;
    case 0xFE:
      Serial.println("Check Received");
      r = nextRead();
      g = nextRead();
      b = nextRead();
      setRGB(r, g, b);
      delay(100);
      if(connectionMode)setRGB(200,200,0x0);
      if(gotMaster&&connectionMode)setRGB(0, 255, 0);
      break; 
    case 0xFF:
      Serial.println("Master Check Received");
      for (int i = 0; i < 8; i++) {
        master[i] = source[i];
      }
      r = nextRead();
      g = nextRead();
      b = nextRead();
      setRGB(r, g, b);
      delay(100);
      gotMaster = true;
      if(connectionMode)setRGB(0,255,0);
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
  else if (test == 0x25) {
    Serial.println("Transmit: ConnectionLost");
    deleteRow(index-1);
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




void sendMasterAddress(int row) {
  Serial.println();
  Serial.println("sendMasterAddres to: ");
  Serial.print(row);
  Serial.println();
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

void sendCheck(int row, byte command, byte r, byte g, byte b) {
  Serial.println("send Check");
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
  byte checksum = 0x10 + command+0x02 + 0x01 + r + g + b + addr[row][0] + addr[row][1] + addr[row][2] + addr[row][3] + addr[row][4] + addr[row][5] + addr[row][6] + addr[row][7] + 0xFF + 0xFE;
  checksum = 0xff - checksum;
  xbee.write(checksum);
  Serial.println(checksum);
}
//----------------------------------------basic's-------------------

byte nextRead() {
  byte nr = xbee.read();
 // Serial.println(String(nr, HEX));
  delay(1);
  return nr;
}

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
        break;
      }
      break;
    }
  }
  printMacAddress();
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
    bool equalToRow = true;
    for (int i = 0; i < 8; i++) {
      if (addr[j][i] != arr[i])equalToRow = false;
    }
    if (equalToRow)return false;
  }
  return true;
}

boolean isMaster(int j) {
  bool equalToRow = true;
  for (int i = 0; i < 8; i++) {
    if (addr[j][i] != master[i])equalToRow = false;
  }
  if (equalToRow)return true;
  else return false;
}

void deleteRow(int j) {
  for (int i = 0; i < 8; i++) {
    addr[j][i] = 0;
  }
  printMacAddress();
}

void printMacAddress() {
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

void setMotor(int pwm) {
  //TODO
}



