#include <Arduino.h>

#include <ESP32Encoder.h>

// include libraries that aren't needed here, but are needed for the build to succeed
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SLAVE_ADDRESS 0x42

#define NUM_BUTTONS 10
// #define BUTTON_RED 15     // RED button on the breadboard
// #define BUTTON_GREEN 16   // GREEN button on the breadboard
// #define BUTTON_ENCODER 17 // encoder SW pin on the breadboard
#define ENCODER_LEFT_DT 18  // encoder DT pin on the breadboard
#define ENCODER_LEFT_CLK 5  // encoder CLK pin on the breadboard
#define ENCODER_RIGHT_DT 16 // encoder DT pin on the breadboard
#define ENCODER_RIGHT_CLK 4 // encoder CLK pin on the breadboard
#define BUTTON_BOARD 0      // the built-in BOOT button on D0
#define LED_BOARD 2         // the built-in LED pin for ESP32 on D2

// button array indices
#define RED 0
#define GREEN 1
#define ENCODER 2
#define BOARD 3

int buttonPins[NUM_BUTTONS] = {
    23,
    33,
    26,
    13,
    32,
    25,
    27,
    14,
    19,
    17,
}; // the number of the pushbutton pin
int buttonStates[NUM_BUTTONS] = {
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
}; // the current reading from the input pins
int lastButtonStates[NUM_BUTTONS] = {
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
    HIGH,
}; // the previous reading from the input pins
unsigned long lastDebounceTimes[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                // the last time the input pins were toggled
String buttonNames[NUM_BUTTONS] = {"1", "2", "3", "4", "5", "6", "7", "8", "enc 1", "enc 2"}; // the names of the buttons

// the debounce time, increase if the output flickers
unsigned long debounceDelay = 50;

ESP32Encoder leftEncoder;
ESP32Encoder rightEncoder;
long leftEncoderPosition = 0;
long rightEncoderPosition = 0;

// function declarations
void checkButton(int buttonIndex);
void checkEncoders();
void receiveEvent(int howMany);
void requestEvent();

unsigned int nextTransmission = 0;

void setup()
{
    pinMode(LED_BOARD, OUTPUT);

    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    leftEncoder.attachHalfQuad(ENCODER_LEFT_DT, ENCODER_LEFT_CLK);
    leftEncoder.setCount(0);
    rightEncoder.attachHalfQuad(ENCODER_RIGHT_DT, ENCODER_RIGHT_CLK);
    rightEncoder.setCount(0);

    // start with the LED off
    digitalWrite(LED_BOARD, LOW);

    Wire.begin(SLAVE_ADDRESS);    // join i2c bus with address
    Wire.onReceive(receiveEvent); // register event
    Wire.onRequest(requestEvent); // register event

    Serial.begin(115200);
    Serial.println("initialized, waiting for button presses");
}

void loop()
{
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        checkButton(i);
    }
    checkEncoders();
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

                nextTransmission |= 1 << buttonIndex;
            }
        }
    }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonStates[buttonIndex] = reading;
}

void checkEncoders()
{
    long pos = leftEncoder.getCount() / 2;
    if (pos != leftEncoderPosition)
    {
        leftEncoderPosition = pos;
        Serial.print("left encoder position changed: ");
        Serial.println(pos);
    }

    pos = rightEncoder.getCount() / 2;
    if (pos != rightEncoderPosition)
    {
        rightEncoderPosition = pos;
        Serial.print("right encoder position changed: ");
        Serial.println(pos);
    }
}

void receiveEvent(int howMany)
{
    Serial.print("receiveEvent ");
    Serial.println(howMany);
    while (1 < Wire.available()) // loop through all but the last
    {
        char c = Wire.read(); // receive byte as a character
        Serial.print(c);      // print the character
    }
    int x = Wire.read(); // receive byte as an integer
    Serial.println(x);   // print the integer
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
    Serial.println("requestEvent");
    int buttonsPressed = nextTransmission;
    Wire.write(buttonsPressed);
    int left = leftEncoderPosition;
    Wire.write(left);
    int right = rightEncoderPosition;
    Wire.write(right);
    nextTransmission = 0;
    // Wire.write(" hello ");
}
