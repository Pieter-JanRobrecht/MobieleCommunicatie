#define DELA 500
#define DSHORT 250

int buttonPin = 7;
boolean currentState = LOW;
boolean lastState = LOW;

String pressed = "pressed";
String released = "released";
String button = "button";

void setup()
{
  pinMode(buttonPin,INPUT);
  Serial.begin(9600);
}

void loop(){
  
  //digitalWrite(LED,LOW);
  if(Serial.available()){
   
    //Ontvangen data van pi
    String incoming = Serial.readString();
    
    if (incoming == button){
      currentState = digitalRead(buttonPin);
      
      
      if (currentState == HIGH && lastState == LOW) {
       Serial.println("released");
       delay(1);
      
      } else if(currentState == LOW && lastState == HIGH) {
         Serial.println("pressed"); 
         delay(1);
        }
      
     }
     lastState = currentState;
  }
}

