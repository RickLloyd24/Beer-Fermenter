/* Brew Modes */
#define Lager 2
#define Ale 1
#define Other 0
#define ColdCrash 4    

/* RGB LED Variables */
#define LEDS_COUNT 1
#define CHANNEL  0
#define NumRGBColors 8

/* Temperature Variables */
#define WindowSamples 1800                   // 30 minutes
#define MaxTemp 80
#define MinTemp 32
#define MaxDeltaTemp 3.0

/* Lager, Ale and ColdCrash Definitions */
#define LagerDayStart 6
#define MaxLagerTemp 65
#define LagerDayEnd 12
float LagerDay = LagerDayStart;
#define AleDayEnd 10
#define AleDayStart 3
#define MaxAleTemp 68
float   AleDay = AleDayStart;
#define ColdCrashTemp 35
float ColdCrashDay = 0.5;

/* Common global defines */
#define MINUTES1 1000*60*1
#define MINUTES2 1000*60*2
#define MINUTES3 1000*60*3
#define MINUTES5 1000*60*5
#define MINUTES10 1000*60*10
#define MINUTES30 1000*60*30
#define SECONDMILS 1000
#define TENSECONDMILS SECONDMILS*10
#define MINUTEMILS SECONDMILS*60
#define HOURMILLS 1000*60*60
#define DAYSECONDS 3600*24
#define MaxBufferSize 7000
