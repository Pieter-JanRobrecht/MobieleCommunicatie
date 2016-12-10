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
unsigned long previousMillis2 = 0;
const long interval = 7800;
const long interval2 = 3000;
byte all[3][8]; // alle aanwezige nodes =  mac addressen
bool first = true;
int light[3]  = {0,0,0};// de matrix waar alle light data van iedere slave inzit, bv: light[1] is de lichtwaarde van de slave met mac address all[1][]
int temp[3]  = {0,0,0};// zowel light en temp worden upgedate om de 3 seallden
int index = 0;
bool zero=true;
byte connMaster[3][8];
byte conn1[3][8];
byte conn2[3][8];
byte conn3[3][8];
int connMatrix[4][4];


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
  initializeConnMatrix();
}


void loop() {
  unsigned long currentMillis = millis();

  xbee.listen();
  if (first) {
    delay(1000);
    //getMasterAddress();
    delay(1000);
    first = false;
  }

  checkReceived();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
      sendDiscover();
      }
  
  if (currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    if (index > 2)index = 0;
    if (!emptyRowInAll(index)){
      sendMasterCheck(index, 0xFF, 0xFF, 0xFF, 0x69);
      sendCommandTo(index, 0x03, 0x00, 0x00, 0x00);//commando om alle info op te halen van de i'de slave
    }
    index++;
    
    bool zero = true;
    for(int i =0;i<3;i++){
      if(!emptyRowInAll(i))zero=false;
    }
    if(zero){
      setRGB(255,0,0);
    }
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
      saveMacAddressInAll();
      setRGB(0, 255, 0);
    }
    else {
      Serial.println(": error");
    }
  } else if (one == 0x46 && two == 0x4E) {
    Serial.print("FN command-----");
    if (nextRead() == 00) {
      Serial.println(": oke");
     
   //TODO---------------------------------Milan----------------------Hier krijg je antwoord op de FN command-----------Dit zal een bericht zijn per buur, de structuur van een bericht kan je bekijken via XCTU
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
  else if (test == 0x25) {
    Serial.println("Transmit: AllectionLost");
    deleteRowInAll(index - 1);
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
    case 0:
      Serial.println("Check address opslaan");
      saveMacAddressInAll();
      setRGB(0, 255, 0);
      break;
    case 1:
      Serial.println("info ontvangen");
      setRGB(0, 0, 255);
      light[index] = nextRead();
      temp[index] = nextRead();
      Serial.println(light[index]);
      Serial.println(temp[index]);
      printInfo();
      break;
    case 0xFF:
      Serial.println("Check Received");
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

void sendNeighbors() {
  Serial.println("send Neighbors");
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x04);
  xbee.write(0x08);
  xbee.write(0x01);
  xbee.write(0x46);
  xbee.write(0x4E);
  xbee.write(0x62);
}


void sendNeighbors(int row) {
  Serial.println(" remote send Neighbors");
 xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x0F);
  xbee.write(0x17);
  xbee.write(0x01);
  xbee.write(all[row][0]);
  xbee.write(all[row][1]);
  xbee.write(all[row][2]);
  xbee.write(all[row][3]);
  xbee.write(all[row][4]);
  xbee.write(all[row][5]);
  xbee.write(all[row][6]);
  xbee.write(all[row][7]);
  xbee.write(0xFF);
  xbee.write(0xFE);
  xbee.write(0x02);
  xbee.write(0x46);
  xbee.write(0x4E);
  byte checksum = 0x17+ 0x46+0x4E + 0x02 + 0x01 + all[row][0] + all[row][1] + all[row][2] + all[row][3] + all[row][4] + all[row][5] + all[row][6] + all[row][7] + 0xFF + 0xFE;
  checksum = 0xff - checksum;
  xbee.write(checksum);
  Serial.println(checksum);
}

void sendCommandTo(int row, byte command, byte r, byte g, byte b) {
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x12);
  xbee.write(0x10);
  xbee.write(0x01);
  xbee.write(all[row][0]);
  xbee.write(all[row][1]);
  xbee.write(all[row][2]);
  xbee.write(all[row][3]);
  xbee.write(all[row][4]);
  xbee.write(all[row][5]);
  xbee.write(all[row][6]);
  xbee.write(all[row][7]);
  xbee.write(0xFF);
  xbee.write(0xFE);
  xbee.write(0x02);
  xbee.write((byte)0X0);
  xbee.write(command);
  xbee.write(r);
  xbee.write(g);
  xbee.write(b);
  byte checksum = 0x10 + command + 0x02 + 0x01 + r + g + b + all[row][0] + all[row][1] + all[row][2] + all[row][3] + all[row][4] + all[row][5] + all[row][6] + all[row][7] + 0xFF + 0xFE;
  checksum = 0xff - checksum;
  xbee.write(checksum);
  Serial.println(checksum);
}

void sendMasterCheck(int row, byte command, byte r, byte g, byte b) {
  Serial.println("send MasterCheck");
  xbee.write(0x7E);
  xbee.write((byte)0x0);
  xbee.write(0x12);
  xbee.write(0x10);
  xbee.write(0x01);
  xbee.write(all[row][0]);
  xbee.write(all[row][1]);
  xbee.write(all[row][2]);
  xbee.write(all[row][3]);
  xbee.write(all[row][4]);
  xbee.write(all[row][5]);
  xbee.write(all[row][6]);
  xbee.write(all[row][7]);
  xbee.write(0xFF);
  xbee.write(0xFC);
  xbee.write(0x02);
  xbee.write((byte)0x0);
  xbee.write(command);
  xbee.write(r);
  xbee.write(g);
  xbee.write(b);
  byte checksum = 0x10 +0x02+ command  + 0x01 + r + g + b + all[row][0] + all[row][1] + all[row][2] + all[row][3] + all[row][4] + all[row][5] + all[row][6] + all[row][7] + 0xFF + 0xFC;
  checksum = 0xff - checksum;
  xbee.write(checksum);
  Serial.println(checksum);
}
//---------------------------------------------------------basic's---all matrix------------------------------------------------------

void saveMacAddressInAll() {
  for (int j = 0; j < 3; j++) {
    byte arr[8];
    if (emptyRowInAll(j)) {
      for (int i = 0; i < 8; i++) {
        arr[i] = nextRead();
      }
      if (isNewInAll(arr)) {
        for (int i = 0; i < 8; i++) {
          all[j][i] = arr[i];
          //Serial.print(arr[i]);
        }
        break;
      }
      break;
    }
  }
  printAll();
}

boolean emptyRowInAll(int j){
  bool empty = true;
  for (int i = 0; i < 8; i++) {
    if (all[j][i] != (byte)0x0)empty = false;
  }
  return empty;
}


boolean isNewInAll(byte arr[]) {
  for (int j = 0; j < 3; j++) {
    bool equalRow = true;
    for (int i = 0; i < 8; i++) {
      if (all[j][i] != arr[i])equalRow = false;
    }
    if (equalRow)return false;
  }
  return true;
}

void deleteRowInAll(int j) {
  for (int i = 0; i < 8; i++) {
    all[j][i] = 0;
  }
  printAll();
}

void printAll() {

  Serial.println();
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 8; i++) {
      Serial.print(all[j][i]);      Serial.print(" ");

    }
    Serial.println();
  }
  Serial.println();
}

void printInfo() {
  Serial.println();
  Serial.println("-------INFO--TEMP--LIGHT---------");
    for (int i = 0; i < 3; i++) {
      Serial.print(temp[i]);      Serial.print(" ");
    }
    for (int i = 0; i < 3; i++) {
      Serial.print(light[i]);      Serial.print(" ");
    }
    Serial.println();
 }


//---------------------------basics--------------------------
byte nextRead() {
  byte b = xbee.read();
  //Serial.println(String(b, HEX));
  delay(1);
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
 void initializeConnMatrix(){
for(int i=0;i<4;i++)
       {for(int j=0;j<4;j++)
             {if(i==j)
                   {connMatrix[i][j]=1;
                   }
                   else{connMatrix[i][j]=0;
                       }
             }
       }
 }
 
boolean compareArray(byte a[],byte b[]){
   for (int i = 0; i < 8; ++i){
     if (a[i] != b[i]){
       return(false);
                      }
                              }
   return(true);
}

void fillConnMatrixM(){
for(int i=0;i<3;i++){
  for(int j=0;j<3;j++){
    if(compareArray(connMaster[i],all[j])){connMatrix[0][j+1]=1;
                                           connMatrix[j+1][0]=1;
                             }
                      }
                    }
                      }
                      
void fillConnMatrixN1(){
for(int i=0;i<3;i++){
  if(compareArray(conn1[i],all[1])){connMatrix[1][2];
                                    connMatrix[2][1];
                      }
                    }
for(int i=0;i<3;i++){
  if(compareArray(conn1[i],all[2])){connMatrix[1][3];
                                    connMatrix[3][1];
                      }
                    }
                       }
                       
void fillConnMatrixN2(){
for(int i=0;i<3;i++){
  if(compareArray(conn1[i],all[2])){connMatrix[2][3];
                                    connMatrix[3][2];
                      }
                    }
  
  }

void getConnMatrix(){
  fillConnMatrixM();
  fillConnMatrixN1();
  fillConnMatrixN2();
                     }
