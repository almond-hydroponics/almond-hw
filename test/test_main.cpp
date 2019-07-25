#include <Arduino.h>
#include <unity.h>

int LED = 12;

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_led_builtin_pin_number(void) {
    TEST_ASSERT_EQUAL(LED, 12);
}

void test_led_state_high(void) {
    digitalWrite(LED, HIGH);
    TEST_ASSERT_EQUAL(digitalRead(LED), HIGH);
}

void test_led_state_low(void) {
    digitalWrite(LED, LOW);
    TEST_ASSERT_EQUAL(digitalRead(LED), LOW);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_led_builtin_pin_number);

    pinMode(LED, OUTPUT);
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop() {
    if (i < max_blinks)
    {
        RUN_TEST(test_led_state_high);
        delay(1000);
        RUN_TEST(test_led_state_low);
        delay(1000);
        i++;
    }
    else if (i == max_blinks) {
      UNITY_END(); // stop unit testing
    }
}