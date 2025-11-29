#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiConnection
{
public:
    void setupWiFi(
        const char *ssid,
        const char *password,
        int connectionAttempts,
        int maxAttempts);
    void connectToWiFi(
        const char *ssid,
        const char *password,
        int connectionAttempts,
        int maxAttempts);
    void maintainWiFi(
        const char *ssid,
        const char *password,
        int connectionAttempts,
        int maxAttempts,
        const long checkInterval);
    void printConnectionDetails();
};