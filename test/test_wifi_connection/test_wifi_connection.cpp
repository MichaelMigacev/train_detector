#include <unity.h>
#include <Arduino.h>
#include <WiFi.h> // For WiFi functions
#include "Wifi_Connection.h"

WiFiConnection wifiConn;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_setupWiFi(void)
{
    wifiConn.setupWiFi("test_ssid", "test_pass", 1, 3);
    TEST_ASSERT_TRUE(true);
}

void test_maintainWiFi(void)
{
    wifiConn.maintainWiFi("test_ssid", "test_pass", 1, 3, 5000);
    TEST_ASSERT_TRUE(true);
}

void test_printConnectionDetails(void)
{
    wifiConn.printConnectionDetails();
    TEST_ASSERT_TRUE(true);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_setupWiFi);
    RUN_TEST(test_maintainWiFi);
    RUN_TEST(test_printConnectionDetails);
    return UNITY_END();
}

void setup()
{
    Serial.begin(115200);
    runUnityTests();
}

void loop() {}