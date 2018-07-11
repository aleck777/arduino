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
DHT  sensor;
int result;

int hum = 0;
int temp=0;
int dht_pin = 8;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // инициализация библиотеки с перечнем задействованных выводов
 
void setup() {
 // pinMode(9, OUTPUT);  
 // analogWrite(9, 50);   // устанавливаем ШИМ выход   
  lcd.begin(16, 2);     // устанавливаем кол-во столбцов и строк
  lcd.print("  HobbyTronics");  // печать сообщения на LCD

  lcd.setCursor(0, 1);
  lcd.print(" test 5");
}
 
void loop() {
  result = sensor.read(dht_pin); // нужно указать № вывода (dht_pin)

 // DISPLAT DATA
 if (result == DHT_OK) {
  hum = sensor.hum;
  temp = sensor.tem;
//  Serial.println((String) "CEHCOP B KOMHATE: "+sensor.hum+"% - "+sensor.tem+"*C");
 } else {
  hum=99;
  temp=99;
 }

  // устанавливаем курсор в 0-ом столбце, 1 строка (начинается с 0)
  lcd.setCursor(0, 1);
  lcd.print("= "); 
  lcd.print(hum);
  lcd.print("%; ");
  lcd.print(temp);
  lcd.print("C"); 
  //lcd.print((String)"" + hum+"%; "+temp+"C"); // печать числа секунд после сброса
  delay(2000);
}
