/* File and Network variabes */
const String HTMLFN = "/Ferminator.html";                   /* HTML file name */
const String ConfigFN = "/config.txt";                    /* Configuration file name */ 
const char* newhostname = "ferm1";       
const String host = newhostname;
const char* ssid     = "networkname";
const char* password  = "password"; 

/* eMail names and Password */
  #define AUTHOR_EMAIL "boardname@gmail.com"
  #define AUTHOR_PASSWORD "gobblegoop"                  //App Password
  #define RECIPIENT_EMAIL1 "mainaccount@hotmail.com"
  #define RECIPIENT_EMAIL2 "phone@tmomail.net"
  #define SMTP_HOST "smtp.gmail.com"
  #define SUBJECT "Alert from Ferminator"                                          /* eMail subject line */

/* Temperature Sensor Configuration */
/* You must have at least one sensor, a maximum of 2 of each type of sensor are allowed */
#define MCPSensors 1     //Number of MCP9808 Sensors
#define DHTSensors 2     //Number of DHT Sensors
#define DHTType 2        //DHT11 is 1, All others 2 (DHT21, DHT22, AM2301, AM2302)


/* Hardware Pin assignments */
#define BuiltinLED 2
#define DHT1Pin 4                   
#define DHT2Pin 5
#define Available12 12               // boot fails if pulled high
#define DHT3Pin 13
#define Available14 14                //outputs PWM signal at boot
#define Available15 15                //outputs PWM signal at boot
#define LEDS_PIN 16                   //UART 2 RX
#define Available17 17                //UART 2 TX
#define Available18 18                 
#define Available19 19
#define SDAPin 21                     //SDA
#define SCLPin 22                     //SCL
#define Available23 23
#define HeaterControlPin 25
#define FrigControlPin 26
#define Available27 27
#define Available32 32
#define Available33 33
#define Available34 34                  //Receive Only
#define Available35 35                  //Receive Only
#define Available36 36                  //Receive Only
#define Available39 37                  //Receive Only
