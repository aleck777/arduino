#define Debug 1 // При расскоментаривании диагностическая информация будет попадать в сом-порт 

// int PLATE_LED = 13; // Светодиод на плате, для диагностики работы

// the digital pins that connect to the LEDs
#define redLEDpin 3   // светодиод на плате с SD
#define greenLEDpin 3 // светодиод на плате с SD
#define SD_CHIP_SELECT 4 // SD-карта на плате Ethernet
// #define SD_CHIP_SELECT 10 // SD-карта на плате LogShield
#define earth_ds18b20 32 // пин для датчика почвы


// include the SD library:
#include <SPI.h>
#include <SD.h>

// #include <DHT.h>

#include <RTClib.h>

// Библиотеки для температурного датчика ds18b20
#include <OneWire.h>
#include <DallasTemperature.h>


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

// function to logging the temperature for a device
void logTemperature(DeviceAddress deviceAddress)
{  
  float tempC = sensors.getTempC(deviceAddress);
  String logString = "Temp C: ";
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

     digitalWrite(greenLEDpin, HIGH);   
     delay(5000);              
     digitalWrite(greenLEDpin, LOW);    
     delay(5000); 
     timeForDelay = timeNow;
   } 
  } 
  statusBlink(sdStatus);

}






