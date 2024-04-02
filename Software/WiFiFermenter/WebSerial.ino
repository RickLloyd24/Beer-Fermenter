/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
  Serial.println(d);
  d.toLowerCase();
  String SubString = d.substring(0, 4); int x = d.length();
  if (d == "?") {
    //                           111111111122222222223
    //                 0123456789012345678901234567890
    WebSerial.println("Commands Are:");
    WebSerial.println("Bias i f.f (Current " + String(TempBias[1],1) + ", " + String(TempBias[2],1) + ", " + String(TempBias[3],1) + ")");
    WebSerial.println("Day f.f (Current " + String(fdays,1) + ")");
    WebSerial.println("Log On/Off");
    WebSerial.println("Heater On ii Seconds (Current " + String(HeaterOnDur) + ")");
    WebSerial.println("Fridge On ii Seconds (Current " + String(FridgeOnDur) + ")");
  }
  else if (SubString == "bias") {
    int sn = d.charAt(5) - '0';
    if (sn < 0 || sn > 4) {
      WebSerial.println("Invalid Sensor Number 1 to 3 only"); 
    }
    else {
      SubString = d.substring(7, x);
      TempBias[sn] = SubString.toFloat();
      WebSerial.println("Temperature Bias #" + String(sn) + " is " + String(TempBias[sn], 1));
      WriteConfigFile();
    }
  }
  else if (SubString == "day ") { 
    SubString = d.substring(4, x);
    fdays = SubString.toFloat();
    StartTime = now() - (fdays * DAYSECONDS);
    WebSerial.println("New StartTime is " + String(StartTime));
    WebSerial.println("New Day is " + String(fdays,1));
    WriteConfigFile();
  }  
  else if (SubString == "heat") { 
    SubString = d.substring(9, x);
    int temp = SubString.toInt();
    WebSerial.println("Heater On Time is now " + String(temp));
    HeaterOnDur = temp;
    WriteConfigFile();
  }
  else if (SubString == "frid") { 
    SubString = d.substring(9, x);
    int temp = SubString.toInt();
    WebSerial.println("Fridge On Time is now " + String(temp));
    FridgeOnDur = temp;
    WriteConfigFile();
  }
  else if (SubString == "font") { 
    WebSerial.println("Fridge On Time is " + String(FonTime));
  }
  else if (SubString == "hont") { 
    WebSerial.println("Heater On Time is " + String(HonTime));
  }
  else if (SubString == "log ") { 
    SubString = d.substring(4, x);
    if (SubString == "on") {
      LoggingOn = true;
      WebSerial.println("Logging is On");
      String s = "Goal ";
      for (int i = 1; i <= numTS; i++) {
        s = s + "  T" + String(i) + "  ";
      }
      s = s + "Hon Fon Hgoal Fgoal";
      WebSerial.println(s);
    }
    else {
      LoggingOn = false;
      WebSerial.println("Logging is Off");
    }
  }  
  else if {
    WebSerial.println("Command not Recognized");
  }
}
/* Write to display and serial during boot */
void Serialprintln(String s) {
  WebSerial.println(s);
  Serial.println(s);
  SystemStatus = s; 
}
