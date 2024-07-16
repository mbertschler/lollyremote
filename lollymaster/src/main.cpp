#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x42

void setup()
{
  Wire.begin(); // join I2C bus as master
  pinMode(LED_BUILTIN, OUTPUT);
  SerialUSB.begin(115200);
  while (!SerialUSB)
    ;
  SerialUSB.println("lollyremote master initialized");
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);

  Wire.requestFrom(SLAVE_ADDRESS, 8);

  unsigned long start;
  start = millis();

  int i = 0;
  while (Wire.available())
  { // slave may send less than requested
    if (i == 0)
    {
      char buttonState = Wire.read(); // receive a byte as character
      if (buttonState > 0)
      {
        SerialUSB.print("button pressed ");
        SerialUSB.println(buttonState, BIN);
      }
    }
    i++;
    if (i == 8)
    {
      break;
    }
  }

  unsigned long took = millis() - start;

  SerialUSB.print("read ");
  SerialUSB.print(i);
  SerialUSB.print(" bytes from I2C in ");
  SerialUSB.print(took);
  SerialUSB.println("ms");
  delay(200);
}