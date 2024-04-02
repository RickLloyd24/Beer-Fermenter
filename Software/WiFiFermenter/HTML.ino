String blank = "                              ";
/* This section includes building page and updating variables */
/* Build Page */
void BuildPage(void) {
/* Insert all current values */ 
  InsertValue(Sprintf("%5.1f", Temperature[0]), indx[0]);         /* Average Temperature */ 
  InsertValue(         fstr(DateTimeStr(1),40), indx[1]);         /* Date and Time */ 
  InsertValue(          Sprintf("%6.2f",fdays), indx[2]);         /* Days Running */ 
  InsertValue(                    String(goal), indx[3]);         /* Current Goal */ 
  InsertValue(          fstr(SystemStatus, 40), indx[4]);         /* SystemStatus */ 
  InsertValue(                  DisplayTemps(), indx[5]);         /* Sensor Temperatures */ 
  InsertValue(       Sprintf("%5.1f", Coldest), indx[6]);         /* Coldest Temperature deg F */ 
  InsertValue(       Sprintf("%5.1f", Warmest), indx[7]);         /* Warmest Temperature deg F */ 
  InsertValue(         OnColor(FrigControlPin), indx[8]);         /* Frig Color */ 
  InsertValue(        OnStatus(FrigControlPin), indx[9]);         /* Frig On/Off */   
  InsertValue(               OnPerStr(FonTime), indx[10]);        /* Frig on Time */ 
  InsertValue(       OnColor(HeaterControlPin), indx[11]);        /* Heater Green/Red */ 
  InsertValue(      OnStatus(HeaterControlPin), indx[12]);        /* Heater On/Off */
  InsertValue(               OnPerStr(HonTime), indx[13]);        /* Heater On Time */ 
  InsertValue(                  String(Reboot), indx[14]);        /* Reboot Count */ 
  InsertValue(       Sprintf("%5.1f", RSSIlvl), indx[15]);        /* RSSI Lvl */ 
  InsertValue(    Sprintf("%5.1f", HeaterGoal), indx[16]);        /* Heater - goal */ 
  InsertValue(    Sprintf("%5.1f", FridgeGoal), indx[17]);        /* Fridge - goal */ 
  InsertValue(              GetBrewMode(Lager), indx[18]);        /* Lager Mode Selected  */ 
  InsertValue(                GetBrewMode(Ale), indx[19]);        /* Ale Mode Selected  */ 
  InsertValue(              GetBrewMode(Other), indx[20]);        /* Other Mode Selected  */ 
  InsertValue(          GetBrewMode(ColdCrash), indx[21]);        /* Other Mode Selected  */ 
  InsertValue(                           myVer, indx[22]);        /* Software Version  */ 
}  

/* Find Variable */
int FindVar (int ptr) {
  String v = String(HTMLpage[ptr+1]) + String(HTMLpage[ptr+2]);    /* Get Variable Code */
  //Serial.print("Search Variable %"); Serial.println(v);
  if (v == "TE") {return 0;}                         /* Average Temperature */
  else if (v == "DT") {return 1;}                    /* Date and Time */
  else if (v == "DA") {return 2;}                    /* Days Running */
  else if (v == "GO") {return 3;}                    /* Current Goal */
  else if (v == "ST") {return 4;}                    /* SystemStatus */
  else if (v == "T1") {return 5;}                    /* Sensor #1 Temperature */
  else if (v == "CO") {return 6;}                   /* Coldest Temperature deg F */
  else if (v == "WA") {return 7;}                   /* Warmest Temperature deg F */
  else if (v == "Fc") {return 8;}                   /* Frig On Color */
  else if (v == "Fs") {return 9;}                   /* Frig Status (On/Off) */
  else if (v == "FT") {return 10;}                   /* Frig on Time */
  else if (v == "Hc") {return 11;}                   /* Heater on Color */
  else if (v == "Hs") {return 12;}                   /* Heater Status (On/Off) */
  else if (v == "HT") {return 13;}                   /* Heater on Time  */
  else if (v == "RE") {return 14;}                   /* Reboots */
  else if (v == "RS") {return 15;}                   /* RSSI lvl */
  else if (v == "OS") {return 16;}                   /* Heater - goal */
  else if (v == "US") {return 17;}                   /* Fridge - goal */
  else if (v == "C1") {return 18;}                   /* Lager Mode Selected */
  else if (v == "C2") {return 19;}                   /* Ale Mode Selected */
  else if (v == "C3") {return 20;}                   /* Other Mode Selected */
  else if (v == "C4") {return 21;}                   /* Cold Crash Selected */
  else if (v == "SV") {return 22;}                   /* Software Version */
  else {
    Serial.print("--- Error: Variable not found, Code= ");
    Serial.println(v);
    return 30;
  }
}

/* Insert Value Function */
/* Inserts the passed string at the location defined in the passed pointer */
void InsertValue(String v, int ptr) {
  if (v.length() < 3) {v = v + blank.substring(0, 3 - v.length());}  /* Pad to 3 chars */
  for (int i = 0; i < v.length(); i++) {                             /* Insert all chars from string */
    HTMLpage[ptr] = v[i];                                            /* Insert char */
    ptr++;                                                           /* move to next position */
  }
}

/* Arduino does not support sprintf with floats 
 * This function simulates the sprinf for floats 
 * * format i.e. %6.2f - 34.12, %4.0f  - 1234
 */
String Sprintf(String format, float value) {
  char str[10];                                                    // Allocate a character array to store the result

  if (format.charAt(0) != '%') return "Error1";
  int field = format.charAt(1) - '0';
  if (format.charAt(2) != '.') return "Error2";
  int numdecimal = format.charAt(3) - '0';
  if (format.charAt(4) != 'f') return "Error3";
  if (numdecimal == 0) {
    //Serial.println("numdecimal " + String(value));
    return String(int(value));
  }
  else {
    dtostrf(value, field, numdecimal, str);
  }  
  return String(str);
}

/* Return percentage on time */
float OnPer (float OnTime) {
  return (OnTime / ((now() + 1 - StartTime) * 1000)) * 100.0;
}
String OnPerStr (float OnTime) {
  float x = OnPer(OnTime);
  return Sprintf("%6.1f", x); 
}  

/* Fixed String Length */
String fstr(String v, int len) {
  if (v.length() >= len) {                                            /* check if string is too long */
    Serial.println("String is too long ");
    Serial.println(v);
    v = v.substring(0, len - 1);                                     /* cut off end of string */
    return v;
  }  
  v = v + blank.substring(0, len - v.length());                     /* Pad to len chars */
  return v;
}
String GetBrewMode(int i) {
  if (bitRead(BrewMode, i) == 1) {return "checked";}
  return "       ";
}

String DisplayTemps(void) {
  String s = "  Using #" + String(UsingSensor) + ", "; int i;
  for (i = 1;  i < numTS; i++) {
    s = s + String(Temperature[i], 1) + ", ";  
  }
  s = s + String(Temperature[i], 1);
  return s;
}
String OnStatus(int pin) {
  if (digitalRead(pin) == 1 ) {
    return "On ";
  }
  else {
    return "Off";  
  }
}

String OnColor (int pin) {
  if (digitalRead(pin) == 1 ) {
    return "blue";
  }
  else {
    return "red ";  
  }
}
