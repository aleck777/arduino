  /*
 
 Демонстрация работы с 16x2 LCD дисплеем.  Библиотека LiquidCrystal
 работает со всеми LCD совместимыми с Hitachi HD44780.
  
 Данный скетч отображает на LCD "Hobbytronics".
 Скетч основан на оригинальном скетче Arduino 
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 с модификациями, позволяющими вручную устанавливать
 контрастность дисплея
  
  Схема подключения:
 * LCD RS пин к цифр. выходу 12
 * LCD Enable пин к цифр. выходу 11
 * LCD R/W к Общему
 * LCD VO пин (пин 3) к ШИМ-выходу 9
 * LCD D4 пин к цифр. выходу 5
 * LCD D5 пин к цифр. выходу 4
 * LCD D6 пин к цифр. выходу 3
 * LCD D7 пин к цифр. выходу 2
 
 */
 
#include <LiquidCrystal.h>   // подключаем библиотеку
#include <DHT.h>
#include <IRremote.h>

// IR Remote - begin
int RECV_PIN = 22;
IRrecv irrecv(RECV_PIN);
decode_results results;

int Code=0;
// IR Remote - end


DHT  sensor;
int result;

int hum = 0;
int temp=0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // инициализация библиотеки с перечнем задействованных выводов

int Button(long CodeB) {
  // Serial.println(CodeB);
  if (CodeB == 1053031451) {
    return 9;
  } else if (CodeB == 465573243) {
    return 8;
  } else if (CodeB == 0x32C6FDF7) {
    return 7;
  } else if (CodeB == 0x449E79F) {
    return 6;
  } else if (CodeB == 0x488F3CBB) {
    return 5;
  } else if (CodeB == 0x8C22657B) {
    return 4;
  } else if (CodeB == 0x6182021B) {
    return 3;
  } else if (CodeB == 0x3D9AE3F7) {
    return 2;
  } else if (CodeB == 0x9716BE3F) {
    return 1;
  } else if (CodeB == 0xC101E57B) {
    return 0;
  } else if (CodeB == 0xE318261B) {
    return 11;
  } else if (CodeB == 0x511DBB) {
    return 12;
  } else if (CodeB == 0xEE886D7F) {
    return 13;
  } else if (CodeB == 0x52A3D41F) {
    return 21;
  } else if (CodeB == 0xD7E84B1B) {
    return 22;
  } else if (CodeB == 0x20FE4DBB) {
    return 23;
  } else if (CodeB == 0xF076C13B) {
    return 31;
  } else if (CodeB == 0xA3C8EDDB) {
    return 32;
  } else if (CodeB == 0xE5CFBD7F) {
    return 33;
  } else if (CodeB == 0x97483BFB) {
    return 42;
  } else if (CodeB == 0xF0C41643) {
    return 43;
  } else {
    return 0xFFFF;
  }
}

String primer = "2+2= ";
int otvet = 0;
String vl = "";
  int t;

void setup() {
  Serial.begin(9600);
  
  randomSeed(analogRead(0));

  t = random(10);
  otvet = random(10);

  primer = (String)""+otvet+"+"+t+"= ";
  otvet = otvet + t;

  pinMode(9, OUTPUT);  
  analogWrite(9, 50);   // устанавливаем ШИМ выход   
  lcd.begin(16, 2);     // устанавливаем кол-во столбцов и строк
  lcd.print(" Aleck Novikov  ");  // печать сообщения на LCD

  lcd.setCursor(0, 1);
  lcd.print((String)"" +primer + "   "); // печать числа секунд после сброса

  irrecv.enableIRIn(); // Start the receiver
  pinMode(26,OUTPUT);
}
 
void loop() {
  int bb = 0;
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.println(results.value);
//    dump(&results);
//    irrecv.resume(); // Receive the next value

   bb = Button(results.value);
   if ((bb>=0) and (bb<10)) {
     vl = vl+(String)""+bb;
   } else if ((bb == 22) and (vl.length()>0)) {
     vl = vl.substring(0,vl.length()-1);
   } else if (bb == 21) {
      if (otvet == vl.toInt()) {
        vl = vl + " OK";  
        digitalWrite(26,HIGH);
       } else {
        vl = vl + " Error"; 
      }
      lcd.setCursor(0, 1);
      lcd.print((String)"" +primer + vl+"   "); // печать числа секунд после сброса
      vl="";

      t = random(10);
      otvet = random(10);
      primer = (String)""+otvet+"+"+t+"= ";
      otvet = otvet + t;

      delay(2000);
      digitalWrite(26,LOW);

   }
    // устанавливаем курсор в 0-ом столбце, 1 строка (начинается с 0)
    lcd.setCursor(0, 1);
    lcd.print((String)"" +primer + vl+"         "); // печать числа секунд после сброса
    delay(50);

    irrecv.resume(); // Receive the next value
  }
}
