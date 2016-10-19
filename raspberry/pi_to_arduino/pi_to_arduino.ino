#define DELA 500
#define DSHORT 250

const int LED = 9;
const int TEMP = 11;
int sensorValue=0;

String temperature = "temp";

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
    
    //Als het commando temp is pink dan met het licht en stuur temp
    if (incoming == temperature){
      light(1);
      sendTemp();
    } 
  }
  
}

void light(int n){
  for (int i=0; i< n; i++)
  {
    digitalWrite(LED,HIGH);
    delay(DSHORT);
    digitalWrite(LED,LOW);
    delay(DSHORT);
  }
  
}

void sendTemp(){
  sensorValue = analogRead(TEMP);
  Serial.println(sensorValue,BIN);
}
