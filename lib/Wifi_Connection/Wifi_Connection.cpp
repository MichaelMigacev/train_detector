#include <Arduino.h>
#include <WiFi.h>
#include "WiFi_Connection.h"
#include "config.h"

unsigned long lastCheck = 0;

void WiFiConnection::setupWiFi(
    const char *ssid,
    const char *password,
    int connectionAttempts,
    int maxAttempts)
{
    Serial.println();
    Serial.println("Initializing WiFi...");

    // WiFi configuration for stability
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    // Set hostname for identification
    WiFi.setHostname("esp32-transport");

    connectToWiFi(
        ssid,
        password,
        connectionAttempts,
        maxAttempts);
}

void WiFiConnection::connectToWiFi(
    const char *ssid,
    const char *password,
    int connectionAttempts,
    int maxAttempts)
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
}

void WiFiConnection::maintainWiFi(
    const char *ssid,
    const char *password,
    int connectionAttempts,
    int maxAttempts,
    const long checkInterval)
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
            connectToWiFi(
                ssid,
                password,
                connectionAttempts,
                maxAttempts);
            break;

        default:
            Serial.println("WiFi not connected. Attempting reconnect...");
            connectToWiFi(
                ssid,
                password,
                connectionAttempts,
                maxAttempts);
            break;
        }
    }
}

void WiFiConnection::printConnectionDetails()
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
}