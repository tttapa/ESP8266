
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <Ticker.h>  //Ticker Library
#include <string>

#include <Ethernet.h>
#include <EthernetUdp.h>

/*
To store

const char* ssid = "***";
const char* password = "***";
*/
#include "pass.h"

//local wifi

MDNSResponder mdns;
ESP8266WebServer server(80);


//udp
WiFiUDP UDP;                     // Create an instance of the WiFiUDP class to send and receive

IPAddress timeServerIP;          // NTP server address
const char* NTPServerName =  "0.ubuntu.pool.ntp.org";//"time.nist.gov";

const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
uint32_t timeUNIX = 0;

//
const int led = 13;
const int ledRed1 = 14;

#define ONE_WIRE_BUS 2
const char* tempLog1 = "/temp1.log";

//term
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensor1(&oneWire);

//Tickers
Ticker tempTicker;
Ticker udpTicker;

void setup(void){
  //delay(3000); to debug purpose

  Serial.begin(115200);
  Serial.println("Begin setup");

  sensor1.begin();

  //set pins
  pinMode(led, OUTPUT);
  pinMode(ledRed1, OUTPUT);

  digitalWrite(led, 0);

  //setup WifireplyPacket
  WiFi.begin(ssid, password);

  //OTA init
  setupOta();

  setupFile();

  setupWebServer();
  
  //Udp
  setupUdp();

  WiFi.hostByName(ntpServerName, timeServerIP); // Get the IP address of the NTP server
  Serial.print("Time server IP: " + timeServerIP);
  
  //First init Udp
  sendNTPpacket(timeServerIP);
  
  //init udp request
  udpRequest();
  
  delay(500);
  Serial.println("End setup");
}

//---------------------------------------- TICKS :)
void tickerSetup(){
    tempTicker.attach(60*5, writeTempLog(sensor1, tempLog1)); // 5 min step
    udpTicker.attach(3600, UdpRequest); // 1 hour step
}



//------------------------------------------------- WEB METHODS - SETUP&HANDLE 

void setupWebServer(){
  Serial.println("Begin setup webServer");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Serial.print("Connected to " + ssid);
  Serial.print("IP address: " + WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/udpRequest", [](){
    server.send(200, "text/html", "Current time stamp is:  " + udpRequest());
  });

  server.on("/ledRed1_On", [](){
    server.send(200, "text/plain", "run Red diode 1 ON heh");
    digitalWrite(ledRed1, HIGH);   // włączmy diodę, podajemy stan wysoki
    Serial.println("ledRed1 On");

  });

  server.on("/ledRed1_Off", [](){
    server.send(200, "text/plain", "run Red diode 1 ON");
    digitalWrite(ledRed1, LOW);   // włączmy diodę, podajemy stan wysoki
    Serial.println("ledRed1 Off");
  });

   server.on("/getLog1", [](){
        server.send(200, "text/html",  readTempLog(tempLog1));
  });

  server.on("/Sensor1", [](){
     sensor1.requestTemperatures();
     delay(500);
     float sensorValue = sensor1.getTempCByIndex(0);
     String temp = String(sensorValue);
     String message = "Current temperature from sensor 1 is: " + temp + "C";
     Serial.println(message);
     server.send(200, "text/html", message);
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266 marysia and tosia and iza 76!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
             
//----------------------------- FILES 
             
void setupFile(){
   Serial.println("Begin setup File");
  
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
  
  Serial.println("End setup File");
}

             
void writeTempLog(DallasTemperature sensor, const char* fileName){
 //read temp
 sensor.requestTemperatures();
 Serial.println("");
 delay(700); //wait 700 msec
  
 Serial.print("Temp requested: ");
 float sensorValue1 = sensor.getTempCByIndex(0);

 String temp = String((sensorValue1));
 Serial.println("Current temperature is " + temp + " Celsius's");
 uint32_t timeStamp  = getTime()/1000;   

  //write to file
  File tempLog = SPIFFS.open(fileName, "a");

  if (!f) {
      Serial.println("file open to write failed");
   }
   else{
      tempLog.print(timeStamp);
      tempLog.print(',');
      tempLog.println(temp);
    }

  tempLog.close();  //Close file
}
     
//Read File data
String readTempLog(const char* fileName){
    String data;
    File f = SPIFFS.open(fileName, "r");

    if (!f) {
      Serial.println("file open failed");
    }
    else{
        Serial.println("Reading Data from File:");
        //Data from file

         while (f.available()){
           data += char(f.read());
         }

        Serial.println("File Closed");
     }
     f.close();  //Close file      
     return data;
 }
  
//---------------------------------- OTA
 void setupOta(){
    Serial.println("Begin setup OTA");

    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();

   Serial.println("End setup OTA");
}

//----------------------------------------- UDP TIME
void setupUdp(){
  Serial.println("Starting UDP");
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  Serial.print("Local port:\t");
  Serial.println(UDP.localPort());
  Serial.println();
}


uint32_t udpRequest(){
  Serial.println("Sending NTP request ...\n");
  sendNTPpacket(timeServerIP);               // Send an NTP request

  uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time
  if (time) {                                  // If a new timestamp has been received
   Serial.print("NTP response: " + time);
  }
  else{
    Serial.print("Error NTP response");
  }
  
  return time;
}

uint32_t getTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

 //------------------------------------ MAIN LOOP 
void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}
