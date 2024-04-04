/* Read the Configuration File */
boolean ReadConfigFile() {
   File file = LittleFS.open(ConfigFN, "r");
   int code = 1;
   if(!file){
    Serial.println("No Saved Data!");
    return false;
   }
   while(file.available()){
     String s = file.readString();
     //Serial.println("Config file data "); Serial.println (s);
     ptr = 0;
     StartTime = StrtoUL(GetSubString(s));
     goal = GetSubString(s).toInt();
     BrewMode = GetSubString(s).toInt();
     Reboot = GetSubString(s).toInt();
     HonTime = GetSubString(s).toFloat();
     FonTime = GetSubString(s).toFloat();
     HeaterOnDur = GetSubString(s).toInt();
     FridgeOnDur = GetSubString(s).toInt();
     TempBias[1] = GetSubString(s).toFloat();
     TempBias[2] = GetSubString(s).toFloat();
     TempBias[3] = GetSubString(s).toFloat();
     Tolerance = GetSubString(s).toFloat();
     code = GetSubString(s).toInt();
   }   
   file.close();
   if (code == 19772) return true; else return false;
}

void WriteConfigFile (void) {
  String s = "StartTime:     " + String(StartTime) + ",\n";
     s = s + "Goal:          " + String(goal) + ",\n";
     s = s + "BrewMode:      " + String(BrewMode) + ",\n";
     s = s + "Reboot Cnt:    " + String(Reboot) + ",\n";
     s = s + "FonTime:       " + String(FonTime, 1) + ",\n";
     s = s + "HonTime:       " + String(HonTime, 1) + ",\n";
     s = s + "HeaterOnDur:   " + String(HeaterOnDur) + ",\n";
     s = s + "FridgeOnDur:   " + String(FridgeOnDur) + ",\n";
     s = s + "Bias #1:       " + String(TempBias[1],2) + ",\n";
     s = s + "Bias #2:       " + String(TempBias[2],2) + ",\n";
     s = s + "Bias #3:       " + String(TempBias[3],2) + ",\n";
     s = s + "Tolerance:     " + String(Tolerance,1) + ",\n";
     s = s + "Code:        19772,\n\n";
  File file = LittleFS.open(ConfigFN, "w");
  file.print(s);
  Serial.println ("Data Written to Config File");
  Serial.println(s);
  file.close();
  return;
}

/* List files */
void listDir(const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = LittleFS.open(dirname, "r");
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
/* Read HTML File Function */
void ReadHTMLFile(void) {
  File file = LittleFS.open(HTMLFN, "r");
  if(!file){
    Serial.println("No Saved Data!");
    return;
  }
  filesize = file.size();
  if (filesize > MaxBufferSize){
    Serial.print("Buffer is not big enough to read file. Filesize is "); Serial.print(filesize); Serial.print(" Buffer is "); Serial.println(MaxBufferSize);
    return;
  }
  file.readBytes(HTMLpage, filesize);
  file.close();
  for (int i = 0; i < filesize; i++) {
    char c = HTMLpage[i];
    if(!isAscii(c)) {
      if (!((c == '\r')  || (c == '\n'))) {
        Serial.print("Non ASCII charactor found: "); Serial.println(c - '0');  
        c = ' ';
      }
    }
    if(c == '@') {
      indx[FindVar(i)] = i;
    }
    //Serial.print(HTMLpage[i]);
  }
  HTMLpage[filesize] = '\0';HTMLpage[filesize + 1] = '\0';
  //Serial.println();
  //Serial.println("HTML file read complete");
  return;
}
/* Find the Sub String */
String GetSubString (String s) {
  int start = 0; int last = 0; int i; boolean flag = true;
  //Serial.print("ptr ");Serial.print(ptr); Serial.print(" s.length "); Serial.println(s.length());
  for (i = ptr; i < s.length(); i++) {
    //Serial.print(s[i]);
    if ((s[i] != ' ') && (flag == true)) {
      start = i; flag = false;
      //Serial.print("Start "); Serial.print(s[i]); Serial.print("  "); Serial.println(start);
    }
    if (s[i] == ':')  {
      flag = true;
      //Serial.println("flag reset");
    }
    if (s[i] == ',') {
      last = i;
      ptr = last + 1;
      //Serial.print("End "); Serial.println(last);
      break;
    }
  }
  String v = s.substring(start, last);
  //Serial.print("Returned String: "); Serial.println(v);
  return v;
}
/* String to Unsigned Long Function */
unsigned long StrtoUL (String s) {
  unsigned long x = 0; unsigned long p10 = 1;
  for (int i = s.length() - 1; i >= 0; i--) {
    if (isDigit(s[i])) {
      x = x + (int(s[i] - '0')) * p10;
      //Serial.print("char ");Serial.print(i);Serial.print(" is ");Serial.print(s[i]);
      //Serial.print(" power is "); Serial.println(p10);
      p10 = p10 * 10;
    }
    else {
      return x;
    }
  }
  return x;
}
