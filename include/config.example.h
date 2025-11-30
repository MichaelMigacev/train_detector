#pragma once

// WiFi credentials - COPY TO config.h AND FILL IN YOUR DETAILS
const char *ssid = "YOUR SSID";
const char *password = "YOUR PASSWORD";

// Connection monitoring
const long checkInterval = 30000; // 30 seconds
int connectionAttempts = 0;
const int maxAttempts = 5;

// Current Time
const char *ntpServer = "de.pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// From and To Stations for one Station Trip
const char *fromStation = "YOUR STATION ID";
const char *toStation = "YOUR DESTINATION STATION ID (one station over)";