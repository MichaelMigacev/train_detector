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
unsigned long previousMillis = -30000;
const long UPDATE_INTERVAL = 30000; // 30 seconds

String lcdMessage1 = "Loading...";
String lcdMessage2 = "";
String lcdMessage3 = "";
String lcdMessage4 = "";

const int LEVER_PIN = 15;
bool lastLeverState = HIGH;
bool currentLeverState = HIGH;

String myStartStation = fromStation;
String myEndStation = toStation;

void setup()
{
  Serial.begin(115200);

  lcdInt.setupLCD();

  wifiConn.setupWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts);

  // init and get time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLCDMessages();

  pinMode(LEVER_PIN, INPUT_PULLUP); // Internal pull-up

  currentLeverState = digitalRead(LEVER_PIN);
  if (currentLeverState == LOW)
  {
    myStartStation = fromStationTwo;
    myEndStation = toStationTwo;
  }
  else
  {
    myStartStation = fromStation;
    myEndStation = toStation;
  }
}

void loop()
{
  wifiConn.maintainWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts,
      checkInterval);
  unsigned long currentMillis = millis();

  bool reading = digitalRead(LEVER_PIN);

  if (currentMillis - previousMillis >= UPDATE_INTERVAL)
  {
    previousMillis = currentMillis;

    // Get departures
    DepartureList departures = statInf.getSouthboundJourneys(myStartStation, myEndStation);
    lcdInt.displayDepartures(departures.departures, departures.count);

    /* struct tm timeinfo;
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

    Serial.println("========================");*/
  }

  if (reading != lastLeverState)
  {
    // Small debounce delay
    delay(20);
    reading = digitalRead(LEVER_PIN); // Read again

    if (reading != currentLeverState)
    {
      currentLeverState = reading;
      if (currentLeverState == LOW)
      {
        myStartStation = fromStationTwo;
        myEndStation = toStationTwo;
      }
      else
      {
        myStartStation = fromStation;
        myEndStation = toStation;
      }
      DepartureList departures = statInf.getSouthboundJourneys(myStartStation, myEndStation);
      lcdInt.displayDepartures(departures.departures, departures.count);
    }
  }
  lastLeverState = reading;
}

void printLCDMessages()
{
  lcdInt.printLine(0, lcdMessage1);
  lcdInt.printLine(1, lcdMessage2);
  lcdInt.printLine(2, lcdMessage3);
  lcdInt.printLine(3, lcdMessage4);
}
