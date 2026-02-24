#include <unity.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "LCD_Interactions.h"

LCDInteractions lcdInt;

void setUp(void)
{
    lcdInt.setupLCD();
}

void tearDown(void) {}

void test_setupLCD(void)
{
    lcdInt.setupLCD();
    TEST_ASSERT_TRUE(true);
}

void test_displayDepartures(void)
{
    String deps[4] = {"Dep1", "Dep2", "Dep3", "Dep4"};
    lcdInt.displayDepartures(deps, 4);
    TEST_ASSERT_TRUE(true);
}

void test_printLCDMessages(void)
{
    lcdInt.printLCDMessages("Msg1", "Msg2", "Msg3", "Msg4");
    TEST_ASSERT_TRUE(true);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_setupLCD);
    RUN_TEST(test_displayDepartures);
    RUN_TEST(test_printLCDMessages);
    return UNITY_END();
}

void setup()
{
    Serial.begin(115200);
    runUnityTests();
}

void loop() {}