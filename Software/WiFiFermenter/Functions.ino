/* Initialize WiFi */
boolean InitWiFi(void) {
  WiFi.persistent( false );                                             /* Turn off saving parameters in FLASH */
  WiFi.mode(WIFI_STA);                                                  /* set this module operates as a station */
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to " + String(ssid));
  long timeout = millis(); WiFiflag = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
    digitalWrite(BuiltinLED, !digitalRead(BuiltinLED));
    if (millis() - timeout > 60000) {                                  /* If can't connect in 1 minutes exit */
      Serial.println();
      WiFi.printDiag(Serial);                                           /* Print out Diagnostics */
      WiFiflag = false;                                                 /* Set Not connected */  
      return false;
    }
  }
  Serial.println();
  if (WiFiflag) {
    WiFi.setAutoReconnect(true);                                          /* Try to reconnect, does not always work */
    Serial.print ("Mac Address "); Serial.println (WiFi.macAddress());    /* Pring new Mac Address */
    String IPaddr = WiFi.localIP().toString();
    Serial.println("IP Address: " + IPaddr);   
    RSSIlvl = WiFi.RSSI();
  }  
  return true;
}

// Color Red = No Temperature Sensors work
// Color Yellow = One Temperature Sensors does not work
// Color White = File System Corrupted or Config File Corrupted
// Color Blue = Wifi Could Not Connect
// Color Cyan = MDNS Responder Error
// Color Magenta = Failed to Set Time
// Color Green all tests Pass
/* This function set the RGB LED to the color on the first call */
void SetRGBLED (String Color) {
  int ColorNum = 0; static int RGBLEDCurrentColor = 0;
  String RGBColorNames[] = {"Black", "Blue", "Green", "Cyan", "Red", "Magenta", "Yellow", "White"};
  unsigned int RGBColorCodes[8][3] = {{0, 0, 0}, {0, 0, 255}, {0, 255, 0}, {0, 255, 255}, {255, 0, 0}, {255, 0, 255}, {255, 255, 0}, {255, 255, 255}};

  if(Color == "Off") {
    RGBLEDCurrentColor = 0; 
    Color = "Black";
    strip.setLedColorData(0, 0, 0, 0);
    strip.show();
    return;
  }           
  if(RGBLEDCurrentColor != 0) return;                                     //RGB LED already set;
  for (ColorNum = 0; ColorNum < NumRGBColors; ColorNum++) {
    if (RGBColorNames[ColorNum] == Color) {
      Serial.println("Set LED to " + RGBColorNames[ColorNum]);
      strip.setLedColorData(0, RGBColorCodes[ColorNum][0], RGBColorCodes[ColorNum][1], RGBColorCodes[ColorNum][2]);
      strip.show();
      delay(3000);
      RGBLEDCurrentColor = ColorNum;
    }
  }
}
void UpdateTemperature(void) { 
  if (bitRead(BrewMode, ColdCrash) == 1) {
    if (fdays > ColdCrashDay) {
      goal = goal - 5;
      if (goal < ColdCrashTemp){
        goal = ColdCrashTemp;
      }
      else {
        ColdCrashDay = ColdCrashDay + 0.5;
        Serialprintln("ColdCrash goal decrease 5 degs");
      }  
    }
  }  
  else if (bitRead(BrewMode, Lager) == 1) {
    int inc = 0;
    if (goal < MaxLagerTemp){
      if (fdays > LagerDay) {
        if (LagerDay >= LagerDayEnd) {
          inc = MaxLagerTemp - goal;
        }  
        else {  
          inc = ((MaxLagerTemp - goal)/(LagerDayEnd - LagerDay));
        }  
      LagerDay++;
      Serialprintln("Lager day Goal Increase " + String(inc));
      goal = goal + inc;    
      }  
    }
  }  
  else if (bitRead(BrewMode, Ale) == 1) {
    if (fdays > AleDay) {
      if (goal < MaxAleTemp){
        goal = goal + 1;
        AleDay++;
        Serialprintln("Ale day Goal increase 1 degree");
      }
    }
  }
  else {
    //Other  
  }
}
/*
  For times east of the Prime Meridian use 0 through 12
  For times west of the Prime Meridian negative offset 0 through -12.
  Ex. American/Denver GMT would be -6. -6 * 60  */
boolean SetRTCTime(void) {
    struct tm timeinfo;
    if (WiFiflag) {
      int trys = 0;
      while (trys < 3) {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        if (getLocalTime(&timeinfo)) {
          Serial.println("Got Time from ntpServer");
          Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
          setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);  
          return true;
        }
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer2);
        if (getLocalTime(&timeinfo)) {
          Serial.println("Got Time from ntpServer2");
          Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
          setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);  
          return true;
        }
        trys++; if (trys >= 3) return false;
        Serial.println("Trying again to get time");
        delay(1000);
      }
    }  
    return false;
}  
    
/* Date/Time Formatter */
String DateTimeStr (int DOW) {
  String s = ""; char buf[10];
  if (ValidTime) {
    if (DOW == 1) {s =  DayofWeek(weekday());}  
    sprintf(buf, " %d/%d/%d ", month(), day(), year()-2000);
    s = s + String(buf);
    sprintf(buf, "%d:%02d ", hourFormat12(), minute());
    s = s + String(buf);
    if (isAM()) {
      s = s + "AM";
    }
    else {
      s = s + "PM";
    }
  }
  else {
    s = "Time Not Available";  
  }  
  //Serialprintln(s);
  return s;
}
String DayofWeek(int Day) {
  // Day of the week (1-7), Sunday is day 1
  if (Day == 1) return "Sunday";
  if (Day == 2) return "Monday";
  if (Day == 3) return "Tuesday";
  if (Day == 4) return "Wednesday";
  if (Day == 5) return "Thursday";
  if (Day == 6) return "Friday";
  if (Day == 7) return "Saturday";
}
void CheckTime(void) {
  if (WiFiflag) {                                                     /* Is WiFi connected? */
    if (timeStatus() == timeSet) {
      //Serialprintln("Time is Synced");
    }
    else {
      //Serialprintln("Time needs to be Resynced");
      if (SetRTCTime()) {
        Serialprintln("Time Resync Successful");
      }
      else {
        Serialprintln("Time Resync Failed");
        //SystemStatus = "Time Resync Failed";
      }
    }
  }
}
