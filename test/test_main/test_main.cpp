#include <unity.h>
#include <Arduino.h>

void setUp(void) {}

void tearDown(void) {}

void test_basic_setup_logic(void)
{
    TEST_ASSERT_TRUE(true);
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_basic_setup_logic);
    return UNITY_END();
}

void setup()
{
    Serial.begin(115200);
    runUnityTests();
}

void loop() {}