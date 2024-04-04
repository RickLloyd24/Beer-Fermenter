/* Temperature and Freezer Global Variables */
float HeaterOffTemp;  
boolean HeaterOffFlag = false;
float FrigOffTemp;  
boolean FrigOffFlag = false;
boolean TempAvailable = true;

/* Intialize Temperature Sensors */
int InitTempSensors(void) {
  numTS = MCPSensors + DHTSensors;
  if (MCPSensors > 0) Wire.begin(SDAPin, SCLPin);
  Serialprintln(String(MCPSensors) + " MCP9808 and " + String(DHTSensors) + " DHT Sensors Installed"); 
  if (numTS > 3) {
    Serialprintln("Too many Temperature Sensors Defined");
    return 0;
  }
  if(DHTSensors > 0) {
    dht1.begin();
    if(DHTSensors > 1) dht2.begin();
    if(DHTSensors > 2) dht3.begin();
  }
  ReadTempSensors();  
  int goodTS = 0;
  for (int i = 1; i <= numTS; i++) {
    if (Temperature[i] > 0) goodTS++;
  }  
  if (goodTS > 0) {return goodTS;} else {return 0; }
}  

/* Get Temperature  */ 
void GetTemperature(void) {
   ReadTempSensors();
   for (int i = 1; i<= numTS; i++) {
    if (Temperature[i] > 0) Temperature[i] = Temperature[i] + TempBias[i];    
  }
  if (LoggingOn) {
    String s = "  " + String(goal) + ", ";
    for (int i = 1; i <= numTS; i++){
      s = s + String(Temperature[i], 1) + ", ";
    }  
    s = s + String(digitalRead(HeaterControlPin)) + ", " + String(digitalRead(FrigControlPin)) + ", "; 
    s = s + String(HeaterGoal, 3) + ", " + String(FridgeGoal, 3);
    Serial.println(s);    
    WebSerial.println(s);
  }
  if (SelectTemperatureSensor()) {
    Temperature[0] = MovingAverage(Temperature[UsingSensor], 1);
    CheckTempStatus(Temperature[UsingSensor]);
    TurnOn(Temperature[UsingSensor]);
  }  
}
/* Decide which Temperature Sensor to Use */
boolean SelectTemperatureSensor(void) {
  boolean TempGood[4];
  
  if (numTS == 1) {
    if (Temperature[1] > 0) {UsingSensor = 1; return true;} else {UsingSensor = 0; return false;}
  }
  for (int i = 1; i <= numTS; i++){
   if (Temperature[i] > 0) {TempGood[i] = true;} else {TempGood[i] = false;}
  }
  if (numTS == 2) {
    if (TempGood[1] == false && TempGood[2] == false) {                                /* Are both bad? */
      Serialprintln("Both Temp Sensors Bad"); 
      digitalWrite(FrigControlPin, LOW);
      digitalWrite(HeaterControlPin, LOW);
      TempAvailable = false;
      return false;
    }
    if (TempGood[1] == true && TempGood[2] == true) {                                  /* Are both good? */
      if (abs(Temperature[1]-Temperature[2]) < MaxDeltaTemp) {                     
        UsingSensor = 1;
        return true;
      }
      Serialprintln("Sensors don't match ");
      int Temp1GoalDelta = abs(Temperature[1] - goal);
      int Temp2GoalDelta = abs(Temperature[2] - goal);
      if (Temp1GoalDelta > Temp2GoalDelta) {                                     /* Pick Sensor closest to goal */
        UsingSensor = 2; return true;
      }
      else {
        UsingSensor = 1; return true;
      }
    }  
  }
  if (numTS == 3) {  
    if(TempGood[1] == false && TempGood[2] == false && TempGood[3] == false) {       /* All temperature Sensors Bad */
      Serialprintln("All Temp Sensors Bad"); 
      digitalWrite(FrigControlPin, LOW);
      digitalWrite(HeaterControlPin, LOW);
      TempAvailable = false;
      return false;
    }
    if(TempGood[1] && TempGood[2] && TempGood[3]) {                                /* All temperature Sensors are good */
      if((Temperature[1] - Temperature[2]) < MaxDeltaTemp  && (Temperature[1] - Temperature[3] < MaxDeltaTemp)) {
        UsingSensor = 1; return true;
      }
      else {                                                                        /* Not within tolerance */
        float delta12 = abs(Temperature[1] - Temperature[2]);
        float delta13 = abs(Temperature[1] - Temperature[3]);
        float delta23 = abs(Temperature[2] - Temperature[3]);
        if (delta12 < delta13 && delta12 < delta23) {                              /* delta12 is smallest */
          UsingSensor = 1; return true;
        }  
        if (delta13 < delta12 && delta13 < delta23) {                              /* delta13 is smallest */
          UsingSensor = 1; return true;
        }
        if (delta23 < delta12 && delta23 < delta13) {                              /* delta23 is smallest */  
          UsingSensor = 2; return true;
        }  
      }  
    }
    for (int i = 1; i <= numTS; i++){                                              /* at least one Sensor is good */
      if (TempGood[i] = true) {
        UsingSensor = i; return true;
      }
    }    
  }
}
/* Calculate Moving Average */
float MovingAverage(float Sample, int WindowSize) {
  static int SampleWindow = WindowSamples;                                                /* moving average sample window */
  static int SampleCount = 1;                                                   /* Count of samples */
  static float AvgValue = 0;
  static float AvgSum = 0;
  
  if (WindowSize == -1) {                                                     /* Reset Moving Average? */  
    SampleWindow = int(Sample);
    SampleCount = 1;
    AvgValue = AvgSum = 0; 
    return 0;
  }
  if (SampleCount < SampleWindow) {                                             /* Still Getting Samples */
    AvgSum = AvgSum + Sample;
    AvgValue = AvgSum/SampleCount;
    SampleCount++;
  }
  else {                                                                           /* Enough samples calculate */
    AvgSum = AvgSum - AvgValue + Sample;
    AvgValue = AvgSum/SampleWindow;  
  }
  return AvgValue;
}
void CheckTempStatus(float temperature) {
  /* Check if temperature has gotten within range */  
  if (TempGood) {                                                       /* is Temperature good? */ 
    if (temperature > Warmest) {
      Warmest = temperature; 
    }  
    if (temperature < Coldest) {
      Coldest = temperature;   
    }  
    EstHeaterGoal(temperature);
    EstFridgeGoal(temperature);
    /* Check if bad things have happened */   
    if (temperature >= (goal + Tolerance*3)) {                                 /* Check if Temperature is too high */
      TempGood = false;                      
      digitalWrite(HeaterControlPin, LOW); 
      SystemStatus = "Temp High " + String(temperature, 1) + " F";   
      if (!alertflag) {eMail(SystemStatus, 3); alertflag = true;}
      Serialprintln(SystemStatus); 
    }
    if (temperature <= (goal - Tolerance*3)) {                                  /* Temperature is too low */
      TempGood = false;
      digitalWrite(FrigControlPin, LOW); 
      SystemStatus = "Temp Low " + String(temperature, 1) + " F";
      if (!alertflag) {eMail(SystemStatus, 3); alertflag = true;}
      Serialprintln(SystemStatus);
    }  
    if (temperature >= MaxTemp) {                                               /* Check if it got too hot */
      TempGood = false;                                    
      digitalWrite(HeaterControlPin, LOW); 
      SystemStatus = "High Temp" + String(temperature, 1) + " F";                
      if (!alertflag) {eMail(SystemStatus, 3); alertflag = true;}
      Serialprintln(SystemStatus);
    }
  }
  else {
    SystemStatus = "Waiting for Good Temp";                                     /* Temperature has not gotten in range */
    if ((temperature <= goal + Tolerance) && (temperature >= goal - Tolerance)) {       /* Temperature is in range  */
      TempGood = true; alertflag = false;                                   /* Set flags */
      Serialprintln("Temp is Good");                                              /* Set SystemStatus */
    }  
  }  
} 
void TurnOn(float temperature) {
 /* Check if Refrigerator needs to be turned ON of OFF */
    if (temperature >= (goal + Tolerance)) {   
      if (curtime >= FridgeWaitTime) {
        if (digitalRead(FrigControlPin) == 0) {                                   /* Fridge Off? */
         digitalWrite(FrigControlPin, HIGH);                
         digitalWrite(HeaterControlPin, LOW);               
         if (LoggingDelta) LoggingOn = true;
         FridgeEndTime = curtime + FridgeOnDur * 1000;                         /* Set Fridge On Time Duration */
        }
      }  
    }
  /* Check if Heater needs to be turned ON or OFF */
    if (temperature <= (goal - Tolerance))   {
      if (curtime >= HeaterWaitTime) {
        if (digitalRead(HeaterControlPin) == 0) {
          digitalWrite(HeaterControlPin, HIGH);
          digitalWrite(FrigControlPin, LOW);
          if (LoggingDelta) LoggingOn = true;
          HeaterEndTime = curtime + HeaterOnDur * 1000;
        }  
      }
    }
}
void EstHeaterGoal(float temperature) {
  if (HeaterOffFlag) {
    if (LoggingDelta) LoggingOn = true;
    if (temperature >= HeaterOffTemp) {
      HeaterGoal = temperature - goal;
      HeaterOffTemp = temperature;
    }
    else {
      HeaterOffFlag = false;
      if (LoggingDelta) LoggingOn = false;
    }
  }
}
void EstFridgeGoal(float temperature) {
  if (FrigOffFlag) {
    if (LoggingDelta) LoggingOn = true;
    if (temperature <= FrigOffTemp) {
      FridgeGoal = temperature - goal;
      FrigOffTemp = temperature;
    }
    else {
      FrigOffFlag = false; 
      if (LoggingDelta) LoggingOn = false;
    }
  }
}
void TurnOff (float temperature) {
  if(TempAvailable) {
    if (curtime >= HeaterEndTime) {                                       
      if (digitalRead(HeaterControlPin) == 1) {
        if (temperature < (goal - Tolerance * 2))  {                              /* temperature not close */
          HeaterEndTime = curtime + HeaterOnDur * 1000;
          HonTime = HonTime + HeaterOnDur;
        }
        else {
          digitalWrite(HeaterControlPin, LOW); 
          HeaterWaitTime = curtime + MINUTEMILS*2;
          HonTime = HonTime + HeaterOnDur;
          HeaterOffTemp = temperature;  HeaterOffFlag = true;
        }
      }
    }
    if (curtime >= FridgeEndTime) {                                          
      if (digitalRead(FrigControlPin) == 1) {                                     /* Is Frig On? */
        if (temperature > (goal + Tolerance * 2))  {                              /* temperature not close */
          FridgeEndTime = curtime + FridgeOnDur * 1000;
          FonTime = FonTime + FridgeOnDur;
        }
        else {
          digitalWrite(FrigControlPin, LOW);                                    /* Turn Frig OFF */
          FridgeWaitTime = curtime + MINUTEMILS*2;                                /* time to temp to settle */
          FonTime = FonTime + FridgeOnDur;
          FrigOffTemp = temperature;  FrigOffFlag = true;
        }  
      }
    }
  }  
}
void ReadTempSensors(void) {
  int SN = 1;
  if (MCPSensors > 0) {
    Temperature[SN] = ReadMCPSensor(Sensor1, SN);
    SN++;
    if (MCPSensors > 1) {
      Temperature[SN] = ReadMCPSensor(Sensor2, SN);
      SN++;
    }
    if (MCPSensors > 2) {
      Temperature[SN] = ReadMCPSensor(Sensor3, SN);
    }
  }
  if (DHTSensors > 0) {
    Temperature[SN] = ReadDHT(dht1, SN);
    SN++;
    if (DHTSensors > 1) {
      Temperature[SN] = ReadDHT(dht2, SN);
      SN++;
    }
    if (DHTSensors > 2) {
      Temperature[SN] = ReadDHT(dht3, SN);
    }
  }
}
float ReadMCPSensor(MCP9808 &sensor, int sn) {
  static int count[] = {0, 0, 0};
  static float previous[3]; float temp = -1;

  if (sensor.getManufacturerID() != 0x0054) {
    Serialprintln("I2C Bus Error #" + String(sn));
    return -1;
  }
  if (sensor.getDeviceID() != 0x04) {
    Serialprintln("Device ID Error, Sensor #" + String(sn));
    return -2;
  }
  temp = CtoF(sensor.getTemperature());
  sn--;
  if (temp == previous[sn]) {
    count[sn]++;
    if (count[sn] > 2*60*60) {      // 2 hours same temperature
      Serialprintln("Temperature Stuck, Sensor #" + String(sn));
      return -3;
    }
  }
  else {
    previous[sn] = temp;
    count[sn] = 0; 
  }
  return temp;
}
float CtoF(float C) {
  return ((C*9.0/5.0) + 32.0);
}
float FtoC(float F) {
  return ((F - 32.0) * 5.0/9.0);
}
/* Read Temperature from Sensor */
float ReadDHT(DHTSimple &sensor, int sn) {
  float t = 0; float h = 0; String err = "";
  err = sensor.getDataRetry(h, t);
  if (err != "OK") {
    Serialprintln("Sensor #" + String(sn) + " " + err); 
    return -1;
  }  
  if (t > 80 || t < 0) {
    Serialprintln("Sensor #" + String(sn) + " unreasonable, value " + String(t,1)); 
    return -2;
  }  
  t = CtoF(t);
  return t;  
}
