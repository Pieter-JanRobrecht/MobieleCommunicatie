#define DELA 500
#define DSHORT 250
const int LED = 11;

void setup()
{
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if(Serial.available())
  {light(Serial.read() - '0');}
  delay(DELA);
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
