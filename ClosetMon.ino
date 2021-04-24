/*
  Name:		HeatingTempMon.ino
  Created:	10/28/2019 2:46:05 PM
  Author:	nusbaum
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Defines.h"
#include "Status.h"
#include "DeviceAddresses.h"
#include "TempSensor.h"

#define DEBUG

#define MAXBUSSES 4

WiFiClient net;
MQTTClient client(4096);

char mqtt_client_id[] = DEVICENAME;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org");

int numbusses = 1;
SensorBus busses[1];


void config() {
  DEBUG_PRINTF("client id: %s\n", mqtt_client_id);
  DEBUG_PRINTF("number of interfaces: %d\n", numbusses);

  int pin_number = -1;
  int num_sensors = -1;
  // bus 0
  num_sensors = 6;
  pin_number = 2;
  busses[0].initialize(pin_number, num_sensors);
  busses[0].initsensor(0, "LIBRARY-IN", "0x28, 0x00, 0x62, 0x94, 0x97, 0x10, 0x03, 0x3F");
  busses[0].initsensor(1, "MBATH-IN", "0x28, 0x65, 0x5D, 0x94, 0x97, 0x09, 0x03, 0xA2");
  busses[0].initsensor(2, "MBR-IN", "0x28, 0xB1, 0x20, 0x94, 0x97, 0x10, 0x03, 0x65");
  busses[0].initsensor(3, "MBR-OUT", "0x28, 0x8D, 0x23, 0x94, 0x97, 0x10, 0x03, 0xBB");
  busses[0].initsensor(4, "LIBRARY-OUT", "0x28, 0x30, 0x50, 0x94, 0x97, 0x01, 0x03, 0xFD");
  busses[0].initsensor(5, "MBATH-OUT", "0x28, 0x0B, 0x54, 0x94, 0x97, 0x01, 0x03, 0xF7");

  busses[0].begin();
  
  publishStatus(client, timeClient, "CONFIGURED");
}


void connect() {
  DEBUG_PRINT("Wait for WiFi... ");
  while (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINT(".");
    delay(500);
  }
  
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("WiFi connected");
  DEBUG_PRINTLN("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINTLN("MAC address: ");
  DEBUG_PRINTLN(WiFi.macAddress());

  DEBUG_PRINT("\nconnecting to MQTT...");
  while (!client.connect(mqtt_client_id)) {
    DEBUG_PRINT(".");
    delay(500);
  }
  DEBUG_PRINTLN("\nconnected!");
}


void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin("nusbaum-24g", "we live in Park City now");
  client.begin(MQTTHOST, net);

  connect();

  timeClient.begin();
  timeClient.forceUpdate();

  publishStatus(client, timeClient, "STARTING");

  config();
}


unsigned long period = PERIOD;
unsigned long previousMillis = 0;

void loop() 
{
  client.loop();
  delay(10);
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > period)
  {
    if (!client.connected()) 
    {
      connect();
      publishStatus(client, timeClient, "RECONNECTED");
    }

    timeClient.update();

    previousMillis = currentMillis;

    unsigned long etime = timeClient.getEpochTime();
    for (int x = 0; x < numbusses; ++x)
    {
      busses[x].requestTemps();
    }
    DEBUG_PRINTF("time = %lu\n", etime);
    delay(500);
    
    for (int x = 0; x < numbusses; ++x)
    {
      busses[x].processTemps(client, timeClient);
    }
        
    publishStatus(client, timeClient, "RUNNING");
  }
}
