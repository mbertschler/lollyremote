#include <Arduino.h>

// include libraries that aren't needed here, but are needed for the build to succeed
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define BUTTON_RED 15   // RED button on the breadboard
#define BUTTON_GREEN 16 // RED button on the breadboard
#define BUTTON_BLUE 17  // RED button on the breadboard
#define BUTTON_BOARD 0  // the built-in BOOT button on D0
#define LED_BOARD 2     // the built-in LED pin for ESP32 on D2

void setup()
{
    pinMode(LED_BOARD, OUTPUT);

    pinMode(BUTTON_RED, INPUT_PULLUP);
    pinMode(BUTTON_GREEN, INPUT_PULLUP);
    pinMode(BUTTON_BLUE, INPUT_PULLUP);
    pinMode(BUTTON_BOARD, INPUT_PULLUP);

    // start with the LED off
    digitalWrite(LED_BOARD, LOW);

    Serial.begin(115200);
    Serial.println("initialized, waiting for button presses");
}

void loop()
{
    // Read the state of the button
    int buttonState = digitalRead(BUTTON_GREEN);

    // If the button is pressed
    if (buttonState == LOW)
    {
        Serial.println("green button pressed");

        // Toggle the state of the LED
        digitalWrite(LED_BOARD, !digitalRead(LED_BOARD));

        // Wait for the button to be released
        while (digitalRead(BUTTON_GREEN) == LOW)
        {
            delay(10);
        }

        // Debounce delay
        delay(50);
    }
}