#pragma once
#include <Arduino.h>
#include <WiFi.h>

const uint32_t timeout_interval = 15000UL;

class WiFiConnection
{
public:
    void setupWiFi(
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

private:
    void connectToWiFi(
        const char *ssid,
        const char *password,
        uint8_t connectionAttempts,
        uint8_t maxAttempts);
};