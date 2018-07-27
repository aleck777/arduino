#define Debug 1 // При расскоментаривании диагностическая информация будет попадать в сом-порт 

// int PLATE_LED = 13; // Светодиод на плате, для диагностики работы

// the digital pins that connect to the LEDs
#define redLEDpin 3   // светодиод на плате с SD
#define greenLEDpin 3 // светодиод на плате с SD
#define SD_CHIP_SELECT 4 // SD-карта на плате Ethernet
// #define SD_CHIP_SELECT 10 // SD-карта на плате LogShield
#define earth_ds18b20 32 // пин для датчика почвы
#define DHT11_PIN 22
#define DHTTYPE DHT11


// include the SD library:
#include <SPI.h>
#include <SD.h>

#include <RTClib.h>

// Библиотеки для температурного датчика ds18b20
#include <OneWire.h>
#include <DallasTemperature.h>

#include <DHT.h>


// Переменная для часов реального времени
RTC_DS1307 rtc;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(earth_ds18b20);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress devAddEarthTemp;


const char daysOfTheWeek[7][24] = {"Восресенье", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота"};

const String DATA_LOG_FILE = "datalog.txt";
const int logTimer = 30 ; // количество секунд для следующей записи

boolean sdStatus;

int count;
long timeForDelay; 

DHT dht(DHT11_PIN, DHTTYPE);


// Функция моргания (ошибка - быстро, нет ошибки - медленно)
void statusBlink (boolean status) {
    if (! status) {
    // при ошибке - моргаем быстро
    for (int i=0; i<20; i++) {
      digitalWrite(redLEDpin, HIGH);   
      delay(100);              
      digitalWrite(redLEDpin, LOW);    
      delay(100);              
    }
  } else {
    // без ошибки - моргаем медленно
    for (int i=0; i<=2; i++) {
      digitalWrite(redLEDpin, HIGH);   
      delay(1000);              
      digitalWrite(redLEDpin, LOW);    
      delay(1000);              
    }
    digitalWrite(redLEDpin, HIGH);   
  }
  return;
}

// Функция логгирования на SD-карту
void dataLogging(String str ) {
    DateTime now = rtc.now();
    
    String dataString = "";

    dataString += String(now.year());
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
    #if defined (Debug)
       Serial.println(dataString); // print to the serial port too:
    #endif    
  }
  
}

// Функция чтения температуры с датчика и логирования на карту
void logTemperature(DeviceAddress deviceAddress)
{  
  float tempC = sensors.getTempC(deviceAddress);
  String logString = "Температура почвы: ";
  logString += String(tempC);
  dataLogging(logString);
}



void setup() {
  #if defined (Debug)
      // start serial port
     Serial.begin(9600);
     Serial.println("Debug mode: включено");
  #endif
  
  DateTime now = rtc.now();
  // Устанавливаем переменные
  timeForDelay = now.unixtime()-logTimer*60;
  count = 1;

  // активизурем выходы светодиодов
  pinMode(greenLEDpin, OUTPUT);   
  pinMode(redLEDpin, OUTPUT);   

    #if defined (Debug)
       Serial.println("Запускаем часы"); 
    #endif   
  statusBlink(rtc.begin()) ;
  delay(1000);
  statusBlink(rtc.isrunning());
  delay(3000);

    #if defined (Debug)
       Serial.println("Проверяем SD-карту"); 
    #endif   
  sdStatus = SD.begin(SD_CHIP_SELECT);
  statusBlink(sdStatus);
  delay(2000);
  
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
      #if defined (Debug)
       Serial.print("Цикл: ");
       Serial.println(count);; 
      #endif   

  if (sdStatus) {
   DateTime now = rtc.now();
   // Устанавливаем переменные
   long timeNow = now.unixtime();
   if (timeNow-timeForDelay > logTimer) {  
     // Лоигрование значения счетчика
     String dataString = "";
     dataString += String(count);
     dataLogging(dataString);
     count++;

     // Логирование значения датчика температуры
      #if defined (Debug)
       Serial.println("Посылаем команду на сенсор температуры"); 
      #endif   
      sensors.requestTemperatures(); // Send the command to get temperatures
      delay(50);
      #if defined (Debug)
       Serial.println("Логируем данные с сенсора температуры"); 
      #endif   
      logTemperature(devAddEarthTemp);

      // Логирование датчика температуры и влажности 
      int chk;
      #if defined (Debug)
        Serial.println("Логируем данные с температуры + влажности: DHT11"); 
      #endif   

      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
       
      #if defined (Debug)
          Serial.print("Влажность: ");
          Serial.print(h);
          Serial.print(" %\t");
          Serial.print("Температура: ");
          Serial.print(t);
          Serial.println(" *C ");
      #endif
      dataString = "Влажность воздуха:";  
      dataString += String(h);
      dataLogging(dataString);
      dataString = "Температура воздуха:";  
      dataString += String(t);
      dataLogging(dataString);

      timeForDelay = timeNow;
      digitalWrite(greenLEDpin, HIGH);   
      delay(5000);              
      digitalWrite(greenLEDpin, LOW);    
      delay(5000); 
     
    } 
  } 
  statusBlink(sdStatus);
}






