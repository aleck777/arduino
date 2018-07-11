#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
 
void setup() {
 
  Serial.begin(9600);
  mySwitch.enableTransmit(10);
  mySwitch.setPulseLength(267);
  mySwitch.setProtocol(1);
  // mySwitch.setRepeatTransmit(15);
   
}
 
void loop() {
  mySwitch.send("110011111100111111000000");
  delay(5000);
  mySwitch.send("110011111100111111000000");
  delay(5000);
  mySwitch.send("110011111100111100110000");
  delay(5000);
  mySwitch.send("110011111100111100110000");
  delay(5000);

  mySwitch.send("110011111100111100001100");
  delay(5000);
  mySwitch.send("110011111100111100000011");
  delay(5000);
}
