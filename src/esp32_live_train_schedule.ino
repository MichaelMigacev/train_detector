#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"
#include "config.h"
#include "Wifi_Connection.h"
#include "Station_Info.h"
#include "LCD_Interactions.h"

WiFiConnection wifiConn;

StationInfo statInf;

LCDInteractions lcdInt;

// Train monitoring
const long trainCheckInterval = 20000;
unsigned long trainLastCheck = 0;
unsigned long previousMillis = 0;
const long UPDATE_INTERVAL = 30000; // 30 seconds

String lcdMessage1 = "füuuuräsötß";
String lcdMessage2 = "message";
String lcdMessage3 = "AAAAA";
String lcdMessage4 = "ÄÖÜ";

void setup()
{
  Serial.begin(115200);

  lcdInt.setupLCD();

  Serial.println("=== ESP32 Long-Term WiFi Connection ===");
  pinMode(LED_BUILTIN, OUTPUT);
  wifiConn.setupWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts);

  // init and get time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLCDMessages();
}

void loop()
{
  wifiConn.maintainWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts,
      checkInterval);
  // getStationDepartures("900093201"); // S Waidmannslust: 900094101, S Hermsdorf: 900093201
  // printLCDMessages();
  // delay(500);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= UPDATE_INTERVAL)
  {
    previousMillis = currentMillis;

    // Get departures
    DepartureList departures = statInf.getSouthboundJourneys(fromStation, toStation);
    lcdInt.displayDepartures(departures.departures, departures.count);

    // Print to serial monitor
    Serial.println("\n=== Updated Departures ===");
    Serial.print("Current time: ");

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
      Serial.println(&timeinfo, "%H:%M:%S");
    }

    Serial.println("Next journeys:");

    for (int i = 0; i < departures.count; i++)
    {
      Serial.print(i + 1);
      Serial.print(". ");
      Serial.println(departures.departures[i]);
    }

    if (departures.count == 0)
    {
      Serial.println("No departures found");
    }

    Serial.println("========================");
    }
}

void printLCDMessages()
{
  lcdInt.printLine(0, lcdMessage1);
  lcdInt.printLine(1, lcdMessage2);
  lcdInt.printLine(2, lcdMessage3);
  lcdInt.printLine(3, lcdMessage4);
}

