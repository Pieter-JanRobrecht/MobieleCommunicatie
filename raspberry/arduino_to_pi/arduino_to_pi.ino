char dataString[50] = {0};
int a =0;
const int TEMP = 11;
int sensorValue=0;

void setup() {
  Serial.begin(9600);             //Starting serial communication
  pinMode(TEMP,INPUT);
}
  
void loop() {
  sensorValue = analogRead(TEMP);
  Serial.println(sensorValue,BIN); //Data verzonden in bit vorm laatste 4 bits zijn na de komma
//  a++;                          // a value increase every loop
//  sprintf(dataString,"%02X",a); // convert a value to hexa 
//  Serial.println(dataString);   // send the data
  delay(3000);                  // give the loop some break
}
