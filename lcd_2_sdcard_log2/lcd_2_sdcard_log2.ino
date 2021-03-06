/*
 
 Демонстрация работы с 16x2 LCD дисплеем.  Библиотека LiquidCrystal
 работает со всеми LCD совместимыми с Hitachi HD44780.
  
  Схема подключения:
 * LCD RS пин к цифр. выходу 12
 * LCD Enable пин к цифр. выходу 11
 * LCD R/W к Общему
 * LCD VO пин (пин 3) к ШИМ-выходу 9
 * LCD D4 пин к цифр. выходу 5
 * LCD D5 пин к цифр. выходу 4
 * LCD D6 пин к цифр. выходу 3
 * LCD D7 пин к цифр. выходу 2


  The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 */
 
#include <LiquidCrystal.h>   // подключаем библиотеку
#include <DHT.h>

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;


DHT  sensor;
int result;

int hum = 0;
int temp=0;
int dht_pin = 30;
int timer = 0;
int timer_2 = 0;
int timer_delay = 1000;
int timer_count = 60;
String FileName = "datalog1.txt";
String dataString = "";
int photo_pin = 0;
int photo_value = 0;


LiquidCrystal lcd(26, 27, 24, 25, 22, 23);  // инициализация библиотеки с перечнем задействованных выводов


void setup() {
  // Serial.begin(9600);
  pinMode(photo_pin, INPUT);
 
  lcd.begin(16, 2);     // устанавливаем кол-во столбцов и строк
  lcd.print("  HobbyTronics");  // печать сообщения на LCD

  pinMode(10, OUTPUT);                       // set the SS pin as an output (necessary!)
  digitalWrite(10, HIGH); 
  
  if (!SD.begin(chipSelect)) {
//    if (false) {
       lcd.setCursor(0, 1);
       lcd.print("Card failed");
       delay(2000);
     //return;
  } 
  lcd.setCursor(0, 1);
  lcd.print("card initialized.");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("starting         ");
  delay(2000);

  File dataFile = SD.open(FileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(String(timer_delay)+"="+String(timer_count));
    dataFile.close();
    // Serial.println(dataString);
  }

}


void loop() {
    dataString = "";
  timer++;
  photo_value = analogRead(photo_pin);
  result = sensor.read(dht_pin); // нужно указать № вывода (dht_pin)

  switch(result){
    case DHT_OK:  hum = sensor.hum;
      temp = sensor.tem;
  dataString += String(timer_2);
//  dataString += ":";
//  dataString += String(timer);
  dataString += "=";
  dataString += String(hum);
  dataString += "%;";
  dataString += String(temp);
  dataString += "C;";
  dataString += String(photo_value);
  dataString += "  ";
  break;
    case DHT_ERROR_CHECKSUM:  dataString +=  "DHT_ERROR_CHECKSUM";       break;
    case DHT_ERROR_DATA:    dataString +=   "DHT_ERROR_DATA"; break;
    case DHT_ERROR_NO_REPLY:  dataString +=    "DHT_ERROR_NO_REPLY";        break;
    default:      dataString +=   "   : ERROR";         break;
  } 


  // устанавливаем курсор в 0-ом столбце, 1 строка (начинается с 0)
  lcd.setCursor(0, 1);
  lcd.print(dataString); 

  if (timer % timer_count == 0) {
    timer_2++;
    timer = 0;
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(FileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    lcd.setCursor(0, 1);
    lcd.print("E SD ");
  }
  }
  delay(timer_delay);
}
