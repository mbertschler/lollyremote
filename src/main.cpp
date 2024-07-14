#include <Arduino.h>

#include <ESP32Encoder.h>

// include libraries that aren't needed here, but are needed for the build to succeed
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define NUM_BUTTONS 4
#define BUTTON_RED 15     // RED button on the breadboard
#define BUTTON_GREEN 16   // GREEN button on the breadboard
#define BUTTON_ENCODER 17 // encoder SW pin on the breadboard
#define ENCODER_DT 18     // encoder DT pin on the breadboard
#define ENCODER_CLK 19    // encoder CLK pin on the breadboard
#define BUTTON_BOARD 0    // the built-in BOOT button on D0
#define LED_BOARD 2       // the built-in LED pin for ESP32 on D2

// button array indices
#define RED 0
#define GREEN 1
#define ENCODER 2
#define BOARD 3

int buttonPins[NUM_BUTTONS] = {BUTTON_RED, BUTTON_GREEN, BUTTON_ENCODER, BUTTON_BOARD}; // the number of the pushbutton pin
int buttonStates[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH};                               // the current reading from the input pins
int lastButtonStates[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH};                           // the previous reading from the input pins
unsigned long lastDebounceTimes[NUM_BUTTONS] = {0, 0, 0, 0};                            // the last time the input pins were toggled
String buttonNames[NUM_BUTTONS] = {"red", "green", "encoder", "board"};                 // the names of the buttons

// the debounce time, increase if the output flickers
unsigned long debounceDelay = 50;

ESP32Encoder firstEncoder;
long firstEncoderPosition = 0;

// function declarations
void checkButton(int buttonIndex);
void checkEncoder();

void setup()
{
    pinMode(LED_BOARD, OUTPUT);

    pinMode(BUTTON_RED, INPUT_PULLUP);
    pinMode(BUTTON_GREEN, INPUT_PULLUP);
    pinMode(BUTTON_ENCODER, INPUT_PULLUP);
    pinMode(BUTTON_BOARD, INPUT_PULLUP);

    firstEncoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
    firstEncoder.setCount(0);

    // start with the LED off
    digitalWrite(LED_BOARD, LOW);

    Serial.begin(115200);
    Serial.println("initialized, waiting for button presses");
}

void loop()
{
    checkButton(RED);
    checkButton(GREEN);
    checkButton(ENCODER);
    checkButton(BOARD);
    checkEncoder();
}

void checkButton(int buttonIndex)
{
    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPins[buttonIndex]);

    // check if the button has been pressed (i.e., reading is different from lastButtonState)
    if (reading != lastButtonStates[buttonIndex])
    {
        // reset the debouncing timer
        lastDebounceTimes[buttonIndex] = millis();
    }

    if ((millis() - lastDebounceTimes[buttonIndex]) > debounceDelay)
    {
        // whatever the reading is at, it's been there for longer than the debounce delay
        // so take it as the actual current state:
        if (reading != buttonStates[buttonIndex])
        {
            buttonStates[buttonIndex] = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonStates[buttonIndex] == LOW)
            {
                Serial.print("button ");
                Serial.print(buttonNames[buttonIndex]);
                Serial.println(" pressed");
                digitalWrite(LED_BOARD, !digitalRead(LED_BOARD));
            }
        }
    }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonStates[buttonIndex] = reading;
}

void checkEncoder()
{
    long newPosition = firstEncoder.getCount() / 2;
    if (newPosition != firstEncoderPosition)
    {
        firstEncoderPosition = newPosition;
        Serial.print("encoder position changed: ");
        Serial.println(newPosition);
    }
}