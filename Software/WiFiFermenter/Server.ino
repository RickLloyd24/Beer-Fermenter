unsigned long ota_progress_millis = 0;

void InitServerFunctions() {
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
     int args = request->args();
     BuildPage();
     request->send(200, "text/html", HTMLpage);
   });
  
   server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
     int args = request->args();
     String parms1 = ""; String parms2 = "";
     for(int i=0;i<args;i++){
       parms1 = request->argName(i).c_str();
       parms2 = request->arg(i).c_str();
     }
     handleGet(parms1, parms2);
     request->send(200, "text/html", HTMLpage);
    });

   server.on ("/favicon.ico", HTTP_GET, [] (AsyncWebServerRequest *request) {
      request->send(200, "image/png", imageString);
   });  
    
   server.onNotFound (notFound);
}
/*
request->version();       // uint8_t: 0 = HTTP/1.0, 1 = HTTP/1.1
request->method();        // enum:    HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_PUT, HTTP_PATCH, HTTP_HEAD, HTTP_OPTIONS
request->url();           // String:  URL of the request (not including host, port or GET parameters)
request->host();          // String:  The requested host (can be used for virtual hosting)
request->contentType();   // String:  ContentType of the request (not avaiable in Handler::canHandle)
request->contentLength(); // size_t:  ContentLength of the request (not avaiable in Handler::canHandle)
request->multipart();     // bool:    True if the request has content type "multipart"
request->getParam(i)
request->hasParam("download")
request->args()
*/

void handleGet(String Parameter, String Value) {
  Serial.println("Get Request Received ");
   
/* Process Get Request and Send Response */        
   if (Parameter == "Refresh"){ }                                                /* Refresh Page Request received */
   else if (Parameter == "Clear"){ProcClear();}                                  /* Clear Button Pressed  */
   else if (Parameter == "Start"){ProcStart();}                                  /* Reset Button pressed */
   else if (Parameter == "brewtype"){SetBrewMode(Value);}
   else if (Parameter == "NewGoal"){ProcNewGoal(Value.toInt());}                 /* New Goal Button pressed */
   BuildPage();                                                               /* Build the page to send */
   delay(100);
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
void ProcStart(void) {
   StartTime = now();                                              /* Set start time of cycle */
   if (bitRead(BrewMode, ColdCrash) == 1) SetBrewMode("Other");
   Reboot = 0;                                                       /* Reset variables */ 
   HonTime = 0; FonTime = 0;                                       /* Reset Sdata variables */ 
   ProcClear();
   WriteConfigFile();                                                   /* Write default values to Flash */
   fdays = 0;
   Serialprintln("Starting a new batch");
}
/* New Goal Received */  
int ProcNewGoal (int newgoal) {
  if (newgoal < 34 || newgoal > 80) {                                 /* New Goal not valid */
    SystemStatus = "Invalid goal Input";
    return false;
  }
  //Serial.print ("New Goal Received= "); Serial.println(newgoal);
  goal = newgoal;                    
  TempGood = false;                  
  ProcClear();                       
  WriteConfigFile ();                
  Serialprintln("New Goal Received");
  return true;                                                    /* Return Sucess */
}  
/* Process Clear Command */
void ProcClear(void) {
  Serialprintln("Clear Command Received");                              /* Set SystemStatus */
  MovingAverage(WindowSamples, -1);
  Warmest = goal; Coldest = goal; 
}    

/* Set the current Brew Mode */
bool SetBrewMode(String value) {
  BrewMode = 0;
  if (value == "ColdCrash") {
    bitSet(BrewMode, ColdCrash); 
    StartTime = now();                                                      /* Reset Start Time */  
    goal = goal - 5;                                                 /* Set goal to new goal */
    TempGood = false;                                               /* Reset Variables */
    HonTime = 0; FonTime = 0;                                       /* Reset Sdata variables */ 
    fdays = 0;
    ColdCrashDay = 0.5;
    MovingAverage(WindowSamples, -1);
    Warmest = goal; Coldest = goal;                                 /* Min and Max Temperature */
    WriteConfigFile ();    
    Serialprintln("Cold Crash Mode");
  }
  if (value == "Lager") {
    bitSet(BrewMode, Lager); 
    LagerDay = LagerDayStart;
  }
  else if (value == "Ale") {
    bitSet(BrewMode, Ale);
    AleDay = AleDayStart;  
  }
  else if (value == "Other") {
    bitSet(BrewMode, Other);  
  }
  else {
    Serial.println(value + "for Brew Mode is Unknown, length " + String(value.length()));
    return false;
  }
  return true;
}
void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}
