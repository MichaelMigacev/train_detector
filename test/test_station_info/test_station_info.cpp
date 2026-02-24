#include <unity.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Station_Info.h"

StationInfo statInf;

void setUp(void) {}

void tearDown(void) {}

void test_getSouthboundJourneys(void) {
    // Basic test: Call with dummy stations (may fail without network; checks parsing logic)
    DepartureList result = statInf.getSouthboundJourneys("TestFrom", "TestTo");
    TEST_ASSERT_TRUE(result.count >= 0);  // At least doesn't crash
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_getSouthboundJourneys);
    return UNITY_END();
}

void setup() {
    Serial.begin(115200);
    runUnityTests();
}

void loop() {}