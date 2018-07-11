#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Мак адрес
byte ip[] = { 192, 168, 134, 253 }; // IP адрес (В броузере вводим 192.168.0.2)
EthernetServer server(80);

int numPins = 4;
int pins[] = { 2, 3, 4, 5 };    // Пины для реле
int pinState[] = {0, 0, 0, 0};  // Состояние пинов

void setup()
{
  // Изначально выключаем все реле
  for (int i = 0; i < numPins; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 1);
  }
  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop()
{
  EthernetClient client = server.available();
  if (client)
  {
    // Проверяем подключен ли клиент к серверу
    while (client.connected())
    {
      // Проверяем идет ли запрос к серверу
      int dataCount = client.available();
      if (dataCount > 0)
      {
        // Считываем данные передаваемые серверу с клиента (броузера)
        for (int i = 0; i < dataCount; i++)
        {
          char ch = client.read();
          // Если данные передаются, то они будут переданы POST запросом, который начинается с символа 'P'
          if (i == 0 && ch != 'P')
            break;
          if (ch == '\n' && i < dataCount - 1)
          {
             // Находим строку, в которой содержатся передаваемые данные
             char chNext = client.read();
             // Формат строки r2=on&r3=on&r4=on (Пример если нужно включены 2,3,4 реле)
             if (chNext == 'r')
             {
               // Выключаем все реле
               pinState[0] = 0;
               pinState[1] = 0;
               pinState[2] = 0;
               pinState[3] = 0;
               
               // Считываем первый номер реле, который нужно включить
               char relayNum = client.read();
               pinState[relayNum-'0'] = 1;
               Serial.write(relayNum);
               
               // Считываем вспомогательную информацию (=on&)
               relayNum = client.read();
               relayNum = client.read();
               relayNum = client.read();
               relayNum = client.read();
                
               // Пока есть данные об остальных реле, считываем и заносим в массив pinState
               while (relayNum != -1)
               {
                  relayNum = client.read();
                  relayNum = client.read();
                  pinState[relayNum-'0'] = 1;
                  Serial.write(relayNum);
                  relayNum = client.read();
                  relayNum = client.read();
                  relayNum = client.read();
                  relayNum = client.read();
               }
             } else
             {
                // Если не было передано данных, то выключаем все реле
                pinState[0] = 0;
                pinState[1] = 0;
                pinState[2] = 0;
                pinState[3] = 0;
             }
          }
        }
      }
      
      // В соответствие с переданными данными включаем реле
      for (int i = 0; i < 4; i++)
      {
         digitalWrite(pins[i], !pinState[i]);
      }
      
      // Выводим HTML страницу, на которой пользователь может включить или выключить нужные ему реле
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html>");
      client.println("<head>");
      client.println("<title>Zelectro. Relay + Ethernet shield.</title>");
      client.println("</head>");
      client.println("<body>");
      client.println("<h3>Zelectro. Relay + Ethernet shield.</h3>");
      client.println("<form method='post'>");
      client.print("<div>Relay 1 <input type='checkbox' ");
      if (pinState[0] == 1)
        client.print("checked");
      client.println(" name='r0'></div>");
      client.print("<div>Relay 2 <input type='checkbox' ");
      if (pinState[1] == 1)
        client.print("checked");
      client.println(" name='r1'></div>");
      client.print("<div>Relay 3 <input type='checkbox' ");
      if (pinState[2] == 1)
        client.print("checked");
      client.println(" name='r2'></div>");
      client.print("<div>Relay 4 <input type='checkbox' ");
      if (pinState[3] == 1)
        client.print("checked");
      client.println(" name='r3'></div>");
      client.println("<input type='submit' value='Refresh'>");
      client.println("</form>");
      client.println("</body>");
      client.println("</html>");
      client.stop(); 
    }
  }
}

