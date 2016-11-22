#define DELA 500
#define DSHORT 250

const int LED = 9;
const int LIGHT = 0;
int sensorValue=0;

String light = "light";

void setup()
{
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  //digitalWrite(LED,LOW);
  if(Serial.available()){
   
    //Ontvangen data van pi
    String incoming = Serial.readString();
    
    //Als het commando temp is pink dan met het licht en stuur licht
    if (incoming == light){
      licht(1);
      sendLight();
    } 
  }
  
}

void licht(int n){
  for (int i=0; i< n; i++)
  {
    digitalWrite(LED,HIGH);
    delay(DSHORT);
    digitalWrite(LED,LOW);
    delay(DSHORT);
  }
  
}

void sendLight(){

  sensorValue = analogRead(LIGHT);
  Serial.println(sensorValue,BIN);
}
