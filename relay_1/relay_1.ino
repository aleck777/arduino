// Реле модуль подключен к цифровому выводу 4
int Relay = 4;
int Led = 13;
int i = 0;
void setup() 
{                
  pinMode(Relay, OUTPUT);     
  pinMode(Led, OUTPUT);     
}

void loop() 
{
  digitalWrite(Relay, LOW);   // реле включено
  for (i=0; i<150; i++) {
    digitalWrite(Led, LOW);   // реле включено
    delay(1000); 
    digitalWrite(Led, HIGH);   // реле включено
    delay(1000); 
  }                

  digitalWrite(Relay, HIGH);  // реле выключено
  for (i=0; i<300; i++) {
    digitalWrite(Led, LOW);   // реле включено
    delay(500); 
    digitalWrite(Led, HIGH);   // реле включено
    delay(500); 
  }                
}
