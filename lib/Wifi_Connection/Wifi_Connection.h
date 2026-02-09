#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiConnection
{
public:
    void setupWiFi(
        const char *ssid,
        const char *password,
        uint8_t connectionAttempts,
        uint8_t maxAttempts);
    void connectToWiFi(
        const char *ssid,
        const char *password,
        uint8_t connectionAttempts,
        uint8_t maxAttempts);
    void maintainWiFi(
        const char *ssid,
        const char *password,
        uint8_t connectionAttempts,
        uint8_t maxAttempts,
        const uint32_t checkInterval);
    void printConnectionDetails();
};