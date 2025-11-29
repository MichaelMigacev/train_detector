#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"
#include "config.h"
#include "Wifi_Connection.h"

WiFiConnection wifiConn;

LiquidCrystal_I2C lcd(0x27, 20, 4);

/* // Connection monitoring
unsigned long lastCheck = 0;
const long checkInterval = 30000; // 30 seconds
int connectionAttempts = 0;
const int maxAttempts = 5; */

// Train monitoring
const long trainCheckInterval = 20000;
unsigned long trainLastCheck = 0;

// Current Time
const char *ntpServer = "de.pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

String lcdMessage1 = "first";
String lcdMessage2 = "message";
String lcdMessage3 = "second";
String lcdMessage4 = "message";

void setup()
{
  Serial.begin(115200);
  delay(1000);
  lcd.init();
  lcd.backlight();

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
  getStationDepartures("900093201");
  printLCDMessages();
  delay(1000);
}

/* void setupWiFi()
{
  Serial.println();
  Serial.println("Initializing WiFi...");

  // WiFi configuration for stability
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // Set hostname for identification
  WiFi.setHostname("esp32-transport");

  connectToWiFi();
} */

/* void connectToWiFi()
{
  if (connectionAttempts >= maxAttempts)
  {
    Serial.println("Too many connection attempts. Restarting ESP32...");
    ESP.restart();
  }

  Serial.print("Connection attempt ");
  Serial.print(connectionAttempts + 1);
  Serial.print("/");
  Serial.print(maxAttempts);
  Serial.println(" to WiFi...");

  WiFi.disconnect();
  delay(1000);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  bool connected = false;

  while (millis() - startTime < 15000)
  { // 15 second timeout
    if (WiFi.status() == WL_CONNECTED)
    {
      connected = true;
      break;
    }
    delay(500);
    Serial.print(".");
  }

  if (connected)
  {
    connectionAttempts = 0; // Reset counter on success
    Serial.println();
    Serial.println("WiFi connected successfully!");
    printConnectionDetails();
  }
  else
  {
    connectionAttempts++;
    Serial.println();
    Serial.println("WiFi connection failed");
    Serial.println("Will retry in main loop...");
  }
} */

/* void maintainWiFi()
{
  unsigned long currentMillis = millis();

  if (currentMillis - lastCheck >= checkInterval)
  {
    lastCheck = currentMillis;

    switch (WiFi.status())
    {
    case WL_CONNECTED:
      // Connection is good
      Serial.print("✓ WiFi stable - RSSI: ");
      Serial.println(WiFi.RSSI());
      break;

    case WL_CONNECTION_LOST:
    case WL_DISCONNECTED:
      Serial.println("⚠ WiFi connection lost. Reconnecting...");
      connectToWiFi();
      break;

    default:
      Serial.println("WiFi not connected. Attempting reconnect...");
      connectToWiFi();
      break;
    }
  }
} */

void printLCDMessages()
{
  lcd.setCursor(0, 0);
  lcd.print(lcdMessage1);
  lcd.setCursor(0, 1);
  lcd.print(lcdMessage2);
  lcd.setCursor(0, 2);
  lcd.print(lcdMessage3);
  lcd.setCursor(0, 3);
  lcd.print(lcdMessage4);
}

/* void printConnectionDetails()
{
  Serial.println("=== Connection Details ===");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal Strength: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());
  Serial.println("==========================");
} */

void parseDepartures(String jsonResponse)
{
  // Create a JSON document with enough capacity
  DynamicJsonDocument doc(8192); // 4KB for response data

  // 1. Convert JSON string to usable object
  DeserializationError error = deserializeJson(doc, jsonResponse);

  // Get current time in both formats
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain current time");
    return;
  }

  time_t now;
  time(&now); // Get current time as time_t for calculations

  // Display current time for reference
  Serial.print("Current time: ");
  Serial.println(&timeinfo, "%H:%M:%S");

  // 2. Check for parsing errors
  if (error)
  {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }
  // count departures to print first 2
  int depCount = 0;
  // 3. Extract the departures array
  JsonArray departures = doc["departures"].as<JsonArray>();

  Serial.println("Next departures:");

  // 4. Loop through each departure
  for (JsonObject departure : departures)
  {
    // Extract data from each departure object
    String lineName = departure["line"]["name"] | "Unknown";
    String direction = departure["direction"] | "Unknown";
    const char *arrTime = departure["when"].as<const char *>();
    if (!arrTime)
    {
      arrTime = departure["plannedWhen"].as<const char *>();
    }
    const char *platform = departure["platform"];

    // 5. Print formatted output
    // Serial.printf("Line %s → %s | Platform %s | %s\n",
    //              lineName, direction, platform, arrTime);

    struct tm arrival_tm = {0};
    sscanf(arrTime, "%d-%d-%dT%d:%d:%d",
           &arrival_tm.tm_year, &arrival_tm.tm_mon, &arrival_tm.tm_mday,
           &arrival_tm.tm_hour, &arrival_tm.tm_min, &arrival_tm.tm_sec);

    arrival_tm.tm_year -= 1900;
    arrival_tm.tm_mon -= 1;
    arrival_tm.tm_isdst = -1;

    time_t arrival = mktime(&arrival_tm);

    // Calculate difference using time_t values
    double diff_seconds = difftime(arrival, now);
    int diff_minutes = (int)(diff_seconds / 60);

    if (diff_minutes >= 0)
    {
      Serial.print("Line " + lineName + " to " + direction + ": ");

      if (diff_minutes == 0)
      {
        Serial.println("NOW");
      }
      else if (diff_minutes == 1)
      {
        Serial.println("1 min");
      }
      else
      {
        Serial.printf("%d mins\n", diff_minutes);
      }
    }
    if (depCount == 0)
    {
      lcdMessage1 = makeMessage(lineName, direction, diff_minutes);
    }
    else if (depCount == 1)
    {
      lcdMessage2 = makeMessage(lineName, direction, diff_minutes);
    }
    else if (depCount == 2)
    {
      lcdMessage3 = makeMessage(lineName, direction, diff_minutes);
    }
    else if (depCount == 3)
    {
      lcdMessage4 = makeMessage(lineName, direction, diff_minutes);
    }
    depCount++;
  }
}

void getStationDepartures(const char *stationId)
{
  unsigned long currentTrainMillis = millis();
  // 1. Create HTTP client
  if (currentTrainMillis - trainLastCheck >= trainCheckInterval)
  {
    trainLastCheck = currentTrainMillis;

    HTTPClient http;

    // 2. Build the API URL (equivalent to your curl command)
    String url = "https://v6.bvg.transport.rest/stops/";
    url += stationId;
    url += "/departures?results=5";

    Serial.print("Querying: ");
    Serial.println(url);

    // 3. Start the connection
    http.begin(url);
    http.addHeader("Accept", "application/json"); // Tell server we want JSON

    // 4. Make the GET request
    int httpCode = http.GET();

    // 5. Check if request was successful
    if (httpCode == 200)
    {
      // Success - get the response data
      String payload = http.getString();

      // 6. Parse and process the JSON response
      parseDepartures(payload);
    }
    else
    {
      // Error handling
      Serial.printf("API request failed. Error code: %d\n", httpCode);
    }

    // 7. Always clean up
    http.end();
  }
}

String makeMessage(String lineName, String direction, int diffMinutes)
{
  String message = "";
  message += cutString(lineName, 3);
  message += " ";
  message += cutString(direction, 13);
  message += " ";
  message += cutString(String(diffMinutes), 2);
  return message;
}

String cutString(String input, int maxLength)
{
  if (input.length() < maxLength)
  {
    while (input.length() < maxLength)
    {
      input += " ";
    }
    return input;
  }
  else if (input.length() > maxLength)
  {
    input = input.substring(0, maxLength);
  }
  return input;
}
