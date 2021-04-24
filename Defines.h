#ifndef Defines_h
#define Defines_h

#define DEVICENAME "CLOSET"
#define MQTTHOST "192.168.0.134"

// 10 minutes                                                                   
#define MIN_RUN_TIME 300000
// temp check interval                                                          
#define PERIOD 10000

#define HIGH_POINT 125
#define LOW_POINT 110

#define BASETOPIC "sorrelhills"
#define TEMPTOPIC BASETOPIC "/temperature/"
#define RELAYTOPIC BASETOPIC "/relay/"
#define STATUSTOPIC BASETOPIC "/device/status/" DEVICENAME
#define CONFIGREQUESTTOPIC BASETOPIC "/device/config-request/" DEVICENAME
#define CONFIGRECEIVETOPIC BASETOPIC "/device/config/" DEVICENAME


#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x)     Serial.print (x)
#define DEBUG_PRINTF(x, y)     Serial.printf (x, y)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x, y)
#define DEBUG_PRINTLN(x)
#endif

#endif
