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
// Berlin timezone with automatic summer/winter time switching
const char *tzInfo = "CET-1CEST,M3.5.0/2,M10.5.0/3";

// From and To Stations for one Station Trip
String fromStation = "INSERT_FROM_STATION_ID";
String toStation = "INSERT_TO_STATION_ID";

// From and To Stations for one Station Trip for second Station Trip
String fromStationTwo = "INSERT_FROM_STATION_ID";
String toStationTwo = "INSERT_TO_STATION_ID";