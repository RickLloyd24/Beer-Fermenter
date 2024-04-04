/* Use ESP32 WROOM DA Module, Minimal SPIFFS with OTA, Core 1 Events Core 0*/
#include <Arduino.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "FS.h"
#include "LittleFS.h"
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <WebSerialLite.h>
#include <Wire.h>
#include <DHTSimple.h>
#include "mcp9808.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "Constants.h"
#include "Passwords.h"
#include <Update.h>

boolean LoggingOn = false;
boolean LoggingDelta = false;
const String imageString = "data:image/bmp;base64,Qk0uCAAAAAAAAHYAAAAoAAAAZwAAACYAAAABAAQAAAAAALgHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAICAgADAwMAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////APhmj/////////////////////////////////////////////////////////////////CGd2/////////////////////////////////////////////////////////////////wZ3dn////////////////////////////////////////////////////////////////8Gd3Z/////////////////////////////////////////////////////////////////Bnd2eIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiI/wZ3dmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZm8Gd3Znd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dnBnd2Z3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dgZ3dmd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3YGd3Znd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d2Bnd2Z3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dgZ3dmd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3iHd3eId3d3d3d3d3d3d3YGd3Znd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d4h3d3iHd3d3d3d3d3d3d2Bnd2Z3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3eId3d4h3d3d3d3d3d3d3dgZ3dmd3d3d3d3d/h3d373d393d3/393f/+HeId493d/h3d373iP+HeI/4d3d3d3d3d3d3YGd3Znd3d3d3d3ePd3d4h3d/d3ePd/d/h3+HiH73d3ePd3d4h493+Hj3f4d3d3d3d3d3d2Bnd2Z3d3d3d3d3f3d3f4d3f3d++Hf373d3d4h/h3d3f3d3f4eId494h3j3d3d3d3d3d3dgZ3dmd3d3d3d3d3j///93d393f4d39493d3ePj3d3d3j///93iHfveId+93d3d3d3d3d3YGd3Znd3d3d3d3d+93f4d3d///+Hd/fvd3d3iPh3d3d+93f4d4h3j3iHePd3d3d3d3d3d2Bnd2Z3d3d3d3d3d/h+93d3f3d3j3f3f4d/d4jvh3d3d/h+93ePd/h493+Hd3d3d3d3d3dgZ3dmd3d3d3d3d3ePeId3d393d39393f/+HeIfvh3d3ePeId3iP/3eI//d3d3d3d3d3d3YGd3Znd3d3d3d3d3f493d3d/d3d/d3d3d3d3iHd3d3d3f493d3d3d3d3d3d3d3d3d3d3d2Bnd2Z3d3d3d3d3d3iPd3d3f3d3j3d3d3d3d4h3d3d3d3iPd3d3d3d3d3d3d3d3d3d3d3dgZ3dmd3d3d3d3d3d++Hd3d3////d393d3d3eId3d3d3d++Hd3d3d3d3d3d3d3d3d3d3d3YGd3Znd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d2Bnd2Z3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dgZ3dmd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3YGdnZnd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d2BmZmZ3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dgZmZmd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3YGdmZnd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d2B2dmZ3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dg92ZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmZmYP/4iIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiIdmZmD////////////////////////////////////////////////////////////////3ZmZg////////////////////////////////////////////////////////////////+GZmYP////////////////////////////////////////////////////////////////92ZoD/////////////////////////////////////////////////////////////////93jw";

/* File Variables */
unsigned long StartTime = 1692554329;          /* Cycle StartTime (Unix format) */
int goal;                                      /* Current goal */
int BrewMode;                                  /* Mode Normal and Other Radio Selected */
int Reboot = 0;                                /* Reboot Count */
float FonTime = 0;
float HonTime = 0;
unsigned int HeaterOnDur = 50;
unsigned int FridgeOnDur = 60;
float TempBias[4];                             /* Delta (#1-#2), Sensor #1 Offset, Sensor #2 Offset, Sensor #3 Offset */
float Tolerance = 1.5; 

/* HTML Variables */
int ptr = 0;                                   /* ptr into file string */ 
char HTMLpage[MaxBufferSize];                  /* Page to send to browser */
int filesize = 0;
int indx[30];                                  /* Ptrs into buffer to insert variables */

/* HTML Variables to display on website */
String SystemStatus = "";                            /* Current SystemStatus */
float RSSIlvl = 0;                             /* Current RSSI level dBm */
float Coldest = 0;                             /* Save Coldest Temperature */
float Warmest = 0;                             /* Save Warmest Temperature */
float fdays = 0;                               /* The number of days running */
float HeaterGoal = 0;
float FridgeGoal = 0;
unsigned long HeaterEndTime = 0;
unsigned long HeaterWaitTime = 0;
unsigned long FridgeEndTime = 0;
unsigned long FridgeWaitTime = 0;
const char myVer[] = __DATE__ " @ " __TIME__;

/* Flags */   
boolean TempGood = false;                      /* Flag that shows Temp is in range */
boolean WiFiflag = false;                      /* Flag that shows connected or not */
boolean alertflag = false;
boolean ValidTime = false;

/* Temperature and Freezer Global Variables */
float Temperature[] = {0, 0, 0, 0};           /* Current Temperature, Sensor #1, Sensor #2, Sensor #3 */
int numTS = 0;
int UsingSensor= 1;

/* Define Structures and Objects */
AsyncWebServer server(80);
SMTPSession smtp;                                      /* Declare the global used SMTPSession object for SMTP transport */ 
SMTP_Message message;                                  /* Declare the message class */
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
MCP9808 Sensor1(0x18);
MCP9808 Sensor2(0x19);
MCP9808 Sensor3(0x1A);
DHTSimple dht1(DHT1Pin, DHTType);
DHTSimple dht2(DHT2Pin, DHTType);
DHTSimple dht3(DHT3Pin, DHTType);

void smtpCallback(SMTP_Status status);                 /* Callback function to get the Email sending status */

/* Task variables */
unsigned long curtime;
unsigned long OnesecondTasks;
unsigned long TensecondTasks;
unsigned long OneMinuteTasks;
unsigned long OneHourTasks;

/* --------------------------------------------------------------------------------------------------------- */ 
void setup ( void ) {
/* Start RGB LED */
  strip.begin();
  strip.setBrightness(10);  
  SetRGBLED("Off");
  
/* Setup Serial Interface */
  Serial.begin (115200);
  delay (500);
  Serial.println ("");
  delay(1000);
  Serialprintln("Ferminator Booting");

/* Setup GPIO Pin Modes */
   pinMode(FrigControlPin, OUTPUT);
   pinMode (HeaterControlPin, OUTPUT);  
   pinMode (BuiltinLED, OUTPUT);                                /* On Board LED */
   digitalWrite(FrigControlPin, LOW);                            /* Turn Frig OFF */
   digitalWrite(HeaterControlPin, LOW);                          /* Turn Heater OFF */  
   digitalWrite(BuiltinLED, HIGH);                                /* Turn LED OFF */  
   Serialprintln("Hardware Pins Set");

/* Connect to WiFi */
   if (InitWiFi()) {
     Serialprintln("WiFi Connection Sucessful!");     
   }
   else {
     Serialprintln("WiFi Connection Failure!");
     SetRGBLED("Blue"); delay(100); 
   }

/* Setup GPIO Pin Modes */
   pinMode(FrigControlPin, OUTPUT);
   pinMode (HeaterControlPin, OUTPUT);  
   pinMode (BuiltinLED, OUTPUT);                                /* On Board LED */
   digitalWrite(FrigControlPin, LOW);                            /* Turn Frig OFF */
   digitalWrite(HeaterControlPin, LOW);                          /* Turn Heater OFF */  
   digitalWrite(BuiltinLED, HIGH);                                /* Turn LED OFF */  
   Serialprintln("Hardware Pins Set");
 
/* Connect Additional WiFi services */
  if(WiFiflag) {
    if (!MDNS.begin(host)) {
      Serial.println("Error on MDNS responder!");  
      SetRGBLED("Cyan");
    }
    else {
      Serial.println("MDNS responder Started with name " + host); 
    }
    WebSerial.begin(&server);
    Serial.println("WebSerial started");
    WebSerial.onMessage(recvMsg);    /* Attach Message Callback */
    AsyncElegantOTA.begin(&server); 
    Serial.println("Elegant OTA started");
    
/* Start HTML Server Functions */
    InitServerFunctions();      
    Serialprintln( "HTTP server started" );
      
    MDNS.addService("http", "tcp", 80);                                  /* Add service to MDNS-SD */
    Serialprintln("mDNS Service Running");
    server.begin();
  }
    
  /* Read Configuration File */
  if(!LittleFS.begin()){
    Serialprintln("Error mounting LittleFS, Halting");
    SetRGBLED("White");delay(100); while(1);
  }
  else {
    Serialprintln("LittleFS Successfully Mounted");
  }
/* Get stored data from Config File */  
  listDir("/", 1);                                                             /* List Files in FS */
  if (ReadConfigFile()) {                                                      /* Read Configuration data from file */
    Serialprintln("Config Data Good!");                                        /* Set Good Status */
    Reboot++;                                                                  /* Increment Reboot count */
    WriteConfigFile();                                                         /* Save new count */
    ReadHTMLFile();                                                            /* Copy HTML data from file */
    Serialprintln("HTML Page Complete");
  }
  else {                                                                       /* Configuration data corrupted */
    Serialprintln("Config Data Bad, Halting");                                 /* Set Critical Status */
    SetRGBLED("White"); delay(100); while(1);  
  }
   
/* Set Time from NTP Service */
  Serialprintln("Getting Current Time");
  if (!SetRTCTime()) {
    Serialprintln("Failed to obtain Time");
    SetRGBLED("Magenta"); delay(100); 
    while(1);
  }
  ValidTime = true;
  Serialprintln("Current Time is " + DateTimeStr(1));
  fdays = float(now() - StartTime)/86400.0;
  Temperature[1] = goal; Temperature[2] = goal; Temperature[3] = goal;
  
/* Get Initial Temperature readings */ 
  int GoodTS = InitTempSensors();
  if (GoodTS == 0) {
    Serialprintln("No good Temperature Sensors!");
    SetRGBLED("Red"); delay(100); while(1);  
  }
  else if (GoodTS < numTS) {
    SetRGBLED("Yellow");
  }
  Serialprintln(String(GoodTS) + " out of " + String(numTS) + " Temperature Sensors are good");

/* Initialize Variables */
  Warmest = goal; Coldest = goal;

  Serialprintln("Ferminator Running");
  SetRGBLED("Green");
  Serialprintln(SystemStatus);
  delay(1000);
  
/* Offset Tasks */
  curtime = millis();
  OnesecondTasks = curtime + SECONDMILS + 100;
  TensecondTasks = curtime + TENSECONDMILS + 200;
  OneHourTasks = curtime + MINUTEMILS + 400;
  
}
/* -------------------------------------------------------------------------------------------------------- */
void loop ( void ) {
  curtime = millis(); 
  //ElegantOTA.loop();
/* 1 Second Tasks */
  if (curtime > OnesecondTasks) {                                                     
    OnesecondTasks = OnesecondTasks + SECONDMILS;
    TurnOff(Temperature[UsingSensor]);
    if (WiFiflag) {digitalWrite(BuiltinLED, !digitalRead(BuiltinLED));}
    else {digitalWrite(BuiltinLED, HIGH);}
  }  
/* 10 Second Tasks */  
  if (curtime > TensecondTasks) {
    TensecondTasks = TensecondTasks + TENSECONDMILS;
    GetTemperature();
  }  
/* 1 Hour Tasks */  
/* Check System Time */
  if (curtime > OneHourTasks) {
    OneHourTasks = OneHourTasks + HOURMILLS;
    CheckTime();
    fdays = float(now() - StartTime)/86400.0;
    UpdateTemperature();
  } 
}
