#include <Arduino.h>

// Pin definitions
#define LED_GREEN 25
#define LED_YELLOW 26
#define LED_RED 27
#define BUZZER 32
#define TOUCH_BUTTON 33 // Touch button on GPIO 33

// Buzzer settings
#define BUZZER_CHANNEL 0
#define BUZZER_FREQUENCY_GREEN 2000
#define BUZZER_FREQUENCY_RED 2500
#define BUZZER_RESOLUTION 8

bool pedestrianRequest = false;

void setup()
{
    Serial.begin(115200);

    // Configure pins
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(TOUCH_BUTTON, INPUT_PULLUP); // Internal pull-up

    // Ensure LEDs start off
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    // Buzzer setup
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY_GREEN, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER, BUZZER_CHANNEL);

    Serial.println("Initializing system...");

    // Required startup sequence: Red -> Red-Yellow -> Green
    Serial.println("RED light ON (10s)...");
    digitalWrite(LED_RED, HIGH);
    delay(10000);

    Serial.println("RED-YELLOW light ON (2s)...");
    digitalWrite(LED_YELLOW, HIGH);
    delay(2000);

    Serial.println("GREEN light ON...");
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, HIGH);
}

void loop()
{
    Serial.println("GREEN light active. Waiting for pedestrian...");
    unsigned long startTime = millis();

    while ((millis() - startTime < 5000) || digitalRead(TOUCH_BUTTON) == LOW)
    {
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY_GREEN);
        delay(500);
        ledcWriteTone(BUZZER_CHANNEL, 0);
        delay(1500);

        if (digitalRead(TOUCH_BUTTON) == LOW)
        {
            pedestrianRequest = true;
            Serial.println("Pedestrian button pressed!");
            break;
        }
    }

    // Change to YELLOW (2s)
    Serial.println("Switching to YELLOW...");
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    delay(2000);

    // Change to RED (10s)
    Serial.println("Switching to RED...");
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
    startTime = millis();

    while (millis() - startTime < 10000)
    {
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY_RED);
        delay(250);
        ledcWriteTone(BUZZER_CHANNEL, 0);
        delay(250);
    }

    // Change to RED-YELLOW (2s)
    Serial.println("Switching to RED-YELLOW...");
    digitalWrite(LED_YELLOW, HIGH);
    delay(2000);

    // Back to GREEN
    Serial.println("Switching back to GREEN...");
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, HIGH);
}
