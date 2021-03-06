//#define Debug 1 // При расскоментаривании диагностическая информация будет попадать в сом-порт 

// int PLATE_LED = 13; // Светодиод на плате, для диагностики работы

// the digital pins that connect to the LEDs
#define redLEDpin 3   // светодиод на плате с SD
#define greenLEDpin 3 // светодиод на плате с SD
#define SD_CHIP_SELECT 4 // SD-карта на плате Ethernet
// #define SD_CHIP_SELECT 10 // SD-карта на плате LogShield
#define earth_ds18b20 32 // пин для датчика почвы
#define DHT11_PIN 22
#define DHTTYPE DHT11
#define HumEarthPIN 31   // PIN для подключения датчика влажности почвы (питание)
#define HumEarthDPIN 33  // PIN для подключения датчика влажности почвы (значение полива)
#define HumEarthAPIN A13   // PIN для подключения датчика влажности почвы

#define relayPIN 46
// PIN для подключения релюшки

#define logTimer  900   // количество секунд для следующей записи
#define waitTimer  30   // количество секунд для следующей записи
#define polivTimer  5   // количество секунд для следующей записи

// include the SD library:
#include <SPI.h>
#include <SD.h>

// Библиотеки для температурного датчика ds18b20
#include <OneWire.h>
#include <DallasTemperature.h>

// include the library code:
#include <LiquidCrystal.h>

#include <DHT.h>
#include <RTClib.h>



// Переменная для часов реального времени
RTC_DS1307 rtc;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(earth_ds18b20);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress devAddEarthTemp;

const String DATA_LOG_FILE = "datalog.txt";

boolean sdStatus;

int count;
unsigned long timeForDelay; 

DHT dht(DHT11_PIN, DHTTYPE);


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 47, en = 45, d4 = 43, d5 = 41, d6 = 39, d7 = 37;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Функция моргания (ошибка - быстро, нет ошибки - медленно)
void statusBlink (boolean status) {
    if (! status) {
    // при ошибке - моргаем быстро
    for (int i=0; i<10; i++) {
      digitalWrite(redLEDpin, HIGH);   
      delay(100);              
      digitalWrite(redLEDpin, LOW);    
      delay(100);              
    }
  } else {
    // без ошибки - моргаем медленно
    for (int i=0; i<=1; i++) {
      digitalWrite(redLEDpin, HIGH);   
      delay(1000);              
      digitalWrite(redLEDpin, LOW);    
      delay(100);              
    }
    digitalWrite(redLEDpin, HIGH);   
  }
  return;
}

// Функция логгирования на SD-карту
void dataLogging(String str ) {
    DateTime now = rtc.now();
    
    String dataString = String(now.year());
    dataString += '-';
    dataString += String(now.month());
    dataString += '-';
    dataString += String(now.day());
    dataString += ' ';
    dataString += String(now.hour());
    dataString += ':';
    dataString += String(now.minute());
    dataString += ':';
    dataString += String(now.second());
    dataString += "  :  ";

    dataString += str;

  File dataFile = SD.open(DATA_LOG_FILE, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close(); 
  }
  #if defined (Debug)
       Serial.println(dataString); // print to the serial port too:
  #endif   
}

// Функция чтения температуры с датчика и логирования на карту
float logTemperature(DeviceAddress deviceAddress, boolean loggingOrNo)
{  
  float tempC = sensors.getTempC(deviceAddress);
  if (loggingOrNo) {
    String logString = "Температура почвы: ";
    logString += String(tempC);
    dataLogging(logString);
  }
  return tempC;
}



void setup() {

  lcd.begin(16, 2);
    
  #if defined (Debug)
      // start serial port
     Serial.begin(9600);
     Serial.println("Debug mode: включено");
  #endif
  
  #if defined (Debug)
       Serial.println("Запускаем часы"); 
  #endif  
  
  lcd.setCursor(0, 0);
  lcd.print("Starting Clock");

  statusBlink(rtc.begin()) ;
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("working"); 

  statusBlink(rtc.isrunning());
  delay(3000);
  
  DateTime now = rtc.now();
  // Устанавливаем переменные
  timeForDelay = now.unixtime()-logTimer*61;
  count = 1;
    #if defined (Debug)
       Serial.print("Время начала: \t"); 
       Serial.println(timeForDelay); 
    #endif   
  // активизурем выходы светодиодов
  pinMode(greenLEDpin, OUTPUT);   
  pinMode(redLEDpin, OUTPUT); 
  // Активизируем вход для датчика влажности почвы  
//  pinMode(HumEarthPIN, INPUT);
  pinMode(HumEarthPIN, OUTPUT);
  pinMode(HumEarthAPIN, INPUT);
  pinMode(HumEarthDPIN, INPUT);
  pinMode(relayPIN, OUTPUT);

  #if defined (Debug)
       Serial.println("Проверяем SD-карту"); 
  #endif   
  sdStatus = SD.begin(SD_CHIP_SELECT);
  
  if (sdStatus) {
    #if defined (Debug) 
       Serial.println("SD-карта работает"); 
    #endif
    lcd.setCursor(0, 1);
    lcd.print("SD working");
  } else {
    #if defined (Debug) 
      Serial.println("SD-карта не работает");
    #endif
    lcd.setCursor(0, 1);
    lcd.print("SD card error");
  }
   
   statusBlink(sdStatus);
  
    #if defined (Debug)
       Serial.println("Запускаем DHT11"); 
    #endif   
    dht.begin();

  if (sdStatus) {
    dataLogging("Reboot");

    // Активизируем бибилотеку датчика тепмературы
    sensors.begin();
    int dev_count = sensors.getDeviceCount();
    sensors.getAddress(devAddEarthTemp, 0);  
    int resolution = sensors.getResolution(devAddEarthTemp);
  
    String logString = "Количество датчиков температуры: ";
    logString += String(dev_count);
    dataLogging(logString);
    logString = "Адрес 0го датчика: ";
    logString += String(devAddEarthTemp[0]);
    logString += ':';
    logString += String(devAddEarthTemp[1]);
    logString += ':';
    logString += String(devAddEarthTemp[2]);
    logString += ':';
    logString += String(devAddEarthTemp[3]);
    logString += ':';
    logString += String(devAddEarthTemp[4]);
    logString += ':';
    logString += String(devAddEarthTemp[5]);
    logString += ':';
    logString += String(devAddEarthTemp[6]);
    logString += ':';
    logString += String(devAddEarthTemp[7]);
    dataLogging(logString);
    logString = "Разрешение 0го датчика: ";
    logString += String(resolution);
    dataLogging(logString);
  }  
}


void loop() {
  lcd.begin(16, 2);
  String dataString ;
  #if defined (Debug)
   Serial.print("Цикл: ");
   Serial.println(count); 
  #endif   

  if (sdStatus) {
   digitalWrite(greenLEDpin, HIGH);
   
   DateTime now = rtc.now();
   // Устанавливаем переменные
   unsigned long timeNow = now.unixtime();
   boolean loggingOrNo = (timeNow-timeForDelay > logTimer);
   if (loggingOrNo) {  
     // Лоигрование значения счетчика
     dataString = "";
     dataString += String(count);
     dataLogging(dataString);
     count++;
   }
   
     // Логирование значения датчика температуры
      #if defined (Debug)
       Serial.println("Посылаем команду на сенсор температуры"); 
      #endif   
      sensors.requestTemperatures(); // Send the command to get temperatures
      delay(50);
      #if defined (Debug)
       Serial.println("Логируем данные с сенсора температуры"); 
      #endif   
      float earthTemp = logTemperature(devAddEarthTemp, loggingOrNo);
      
      // Значения с датчика влажности почвы
      digitalWrite(HumEarthPIN, HIGH);
      delay(50);   
      int ah=analogRead(HumEarthAPIN);
      int dh=digitalRead(HumEarthDPIN); // 1 можно поливать, 0 - хватит поливать 
      digitalWrite(HumEarthPIN, LOW);
      float earthHum = (1024-ah)/7.74;
      if (loggingOrNo) {
        dataString = "Влажность почвы: ";  
        dataString += String(dh);
        dataString += " : ";  
        dataString += String(ah);
        dataString += " : ";  
        dataString += String(earthHum);
        dataString += "%";  
        dataLogging(dataString);
        // Логирование датчика температуры и влажности 
        // int chk;
        #if defined (Debug)
          Serial.println("Логируем данные с температуры + влажности: DHT11"); 
        #endif   
      }
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      if (loggingOrNo) {
        dataString = "Влажность воздуха:";  
        dataString += String(h);
        dataLogging(dataString);
        dataString = "Температура воздуха:";  
        dataString += String(t);
        dataLogging(dataString);
      }
      // Вывод значений на экран 
      dataString = "Z:";   
      dataString += String(earthTemp);
      dataString += "C  ";  
      dataString += String(earthHum);   
      dataString += "%";  
      lcd.setCursor(0, 0);
      lcd.print(dataString);
      dataString = "V:";   
      dataString += String(t);
      dataString += "C  ";  
      dataString += String(h);   
      dataString += "%";  
      lcd.setCursor(0, 1);
      lcd.print(dataString);
      
      if (loggingOrNo) {
        timeForDelay = timeNow;
      }   
      delay(1000);
    if ((earthHum<69) and (dh==1) ) {
      digitalWrite(relayPIN, HIGH);
      delay(1000);
      dataLogging("Полив включен");
      lcd.setCursor(0, 1);
      lcd.print("Relay ON         ");
      delay(polivTimer*1000); // Задержка полива       
      digitalWrite(relayPIN, LOW);
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("Relay OFF        ");
      dataLogging("Полив выключен");
      delay(waitTimer*1000); // Задержка перед повторным поливом
    }
     
  } 
  statusBlink(sdStatus);
}






