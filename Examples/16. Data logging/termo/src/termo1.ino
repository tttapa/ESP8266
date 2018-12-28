
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <Ticker.h>  //Ticker Library
#include <string>
#include <time.h>

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <WebSocketsServer.h>

/*
To store

const char* ssid = "***";
const char* password = "***";
*/
#include "pass.h"

//local wifi
ESP8266WiFiMulti wifiMulti;

//dns init
MDNSResponder mdns;
const char* mdnsName = "termo";        // Domain name for the mDNS responder

//web server init
ESP8266WebServer server(80);

//web socket
WebSocketsServer webSocketServer(81);    // create a websocket server on port 81

//udp
WiFiUDP UDP;                     // Create an instance of the WiFiUDP class to send and receive

IPAddress timeServerIP;          // NTP server address
const char* ntpServer0 =  "0.ubuntu.pool.ntp.org";
const char* ntpServer1 =  "1.ubuntu.pool.ntp.org";
const char* ntpServer2 =  "2.ubuntu.pool.ntp.org";


const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
uint32_t timeUNIX = 0;

//ports
const int led = 13;
const int ledRed1 = 14;
#define ONE_WIRE_BUS 2

//files
const char* tempLog1 = "/temp.csv";

//term
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensor1(&oneWire);

//Tickers
Ticker tempTicker;

void setup(void){
  //delay(3000); to debug purpose

  Serial.begin(115200);
  Serial.println("Begin setup");

  sensor1.begin();

  //set pins
  pinMode(led, OUTPUT);
  pinMode(ledRed1, OUTPUT);

  digitalWrite(led, 0);

  setupWiFi();

  setupOta();

  setupFile();

  setupMDNS();

  setupWebServer();

  startWebSocket();            // Start a WebSocket server

  setupUdp();

  int timezone = 1 * 3600;
  int dst = 0;

   configTime(timezone, dst, ntpServer0,ntpServer1, ntpServer2);

   while(!time(nullptr)){
    Serial.print("*");
    delay(1000);
   }
   Serial.println("\nTime response....OK");

  tickerSetup();
  Serial.println("End setup");
}


//---------------------------------------- TICKS :)
void tickerSetup(){
    Serial.println("Begin TickerSetup");
    tempTicker.attach(60*1, writeTempLog1); // 1 min step
    Serial.println("End TickerSetup");
}


//---------------------------------- OTA //

void setupOta(){
  Serial.println("Begin setup OTA");

  //ArduinoOTA.setHostname(OTAName);
  //ArduinoOTA.setPassword(OTAPassword);

    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\r\nEnd");
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

//------------------------------------------------- WIFI

void setupWiFi() { // Try to connect to some given access points. Then wait for a connection
  Serial.println("Begin setup Wifi");

  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
  //wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());             // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  Serial.println("\r\n");

  Serial.println("End setup Wifi");
}

//------------------------------------------------- WEB SOCKET method

void startWebSocket() { // Start a WebSocket server
  webSocketServer.begin();                          // start the websocket server
  webSocketServer.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}


/* Web Socket Handlers */


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);

      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocketServer.remoteIP(num);

        //String  log1 = sprintf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        webSocketServer.sendTXT(num, "Connected hurra");
        broadcastTempLog1(num, sensor1);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      Serial.printf("[%u] get Text: %s\n", num, payload);

      break;
  }
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

  server.on("/clearFiles", handleClearFiles);

  server.on("/handleTime", handleTime);

  server.on("/upload", HTTP_GET, []() {                 // if the client requests the upload page
    server.send(200, "text/html", "<!DOCTYPE html><html><head><title>ESP8266 SPIFFS File Upload</title><link rel='stylesheet' type='text/css' href='main.css'></head><body><h1>ESP8266 SPIFFS File Upload</h1><p>Select a new file to upload to the ESP8266. Existing files will be replaced.</p><form method='POST' enctype='multipart/form-data'><input type='file' name='data'><input class='button' type='submit' value='Upload'></form></body></html>");
  });

  server.on("/edit.html",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("End setup webserver");
}

void handleClearFiles(){
      Serial.println("Begin handleClearFiles");
      String fileName = "/temperatureGraph.js";
      SPIFFS.remove(fileName);
      if (!SPIFFS.exists(fileName))
          Serial.println("has been removed: " + fileName);
      else{
        Serial.println("Removing problem: " + fileName);
      }

      Serial.println("End handleClearFiles");

}

void handleTime(){
  Serial.println("Begin udpRequest\n");

      time_t now = time(nullptr);
      struct tm* p_tm = localtime(&now);

      Serial.print("TimeStamp is" );
      Serial.println(now);

      Serial.print(p_tm->tm_mday);
      Serial.print("/");
      Serial.print(p_tm->tm_mon + 1);
      Serial.print("/");
      Serial.print(p_tm->tm_year + 1900);

      Serial.print(" ");

      Serial.print(p_tm->tm_hour);
      Serial.print(":");
      Serial.print(p_tm->tm_min);
      Serial.print(":");
      Serial.println(p_tm->tm_sec);
}

void handleNotFound() { // if the requested file or page doesn't exist, return a 404 not found error
  if (!handleFileRead(server.uri())) {        // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.html";
            // If a folder is requested, send the index file

    }
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}


void handleFileUpload() { // upload a new file to the SPIFFS
  File fsUploadFile;                                    // a File variable to temporarily store the received file
  HTTPUpload& upload = server.upload();
  String path;
  if (upload.status == UPLOAD_FILE_START) {
    path = upload.filename;
    if (!path.startsWith("/")) path = "/" + path;
    if (!path.endsWith(".gz")) {                         // The file server always prefers a compressed version of a file
      String pathWithGz = path + ".gz";                  // So if an uploaded file is not compressed, the existing compressed
      if (SPIFFS.exists(pathWithGz))                     // version of that file must be deleted (if it exists)
        SPIFFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: "); Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");               // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
  http://termo.local/  if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                   // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location", "/success.html");     // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

//----------------------------- MDNS

void setupMDNS(){
  Serial.println("Begin setup mdns");
  MDNS.begin(mdnsName);
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
  Serial.println("End setup mdns");
}

//----------------------------- FILES

void setupFile(){
   Serial.println("Begin setup File");

  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {                      // List the file system contents
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
  }
  Serial.printf("\n");

  Serial.println("End setup File");
}

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

void writeTempLog1(){
  writeTempLog("t1", sensor1, tempLog1);
}

void broadcastTempLog1(uint8_t num, DallasTemperature sensor){
  //writeTempLog("t1", sensor1, tempLog1);

  Serial.print("Temp requested: ");
  sensor.requestTemperatures();
  Serial.println("");
  delay(700); //wait 700 msec
  float sensorValue1 = sensor.getTempCByIndex(0);

  String temp = String((sensorValue1));
  webSocketServer.sendTXT(num, "updateTemp:" + String(temp));
}

void writeTempLog(String etykiet, DallasTemperature sensor, const char* fileName){
  Serial.println("Begin writeTempLog");
 //read temp
 sensor.requestTemperatures();
 Serial.println("");
 delay(700); //wait 700 msec

 Serial.print("Temp requested: ");
 float sensorValue1 = sensor.getTempCByIndex(0);

 String temp = String((sensorValue1));
 Serial.println("Current temperature is " + temp + " Celsius's");

  time_t timeStamp = time(nullptr);

  //write to file
  File tempLog = SPIFFS.open(fileName, "a");

  if (!tempLog) {
      Serial.println("file open to write failed");
   }
   else{
      tempLog.print(timeStamp);
      tempLog.print(',');
      tempLog.println(temp);

      //tempLog.print(',');
      //tempLog.println(etykiet);

      webSocketServer.broadcastTXT("updateTemp:" + String(temp));
    }

    tempLog.close();  //Close file


  Serial.println("End writeTempLog");
}

//Read File data
String readTempLog(const char* fileName){
    Serial.println("Begin readTempLog");
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
     Serial.println("End readTempLog");
     return data;
 }


//----------------------------------------- UDP TIME
void setupUdp(){
  Serial.println("Begin setupUDP");
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  Serial.print("Local port:\t");
  Serial.println(UDP.localPort());
  Serial.println("End setupUDP");
}


 //------------------------------------ MAIN LOOP
void loop() {
  server.handleClient();
  webSocketServer.loop();
  ArduinoOTA.handle();
}
