#include "pins_arduino.h"
#include "DefuseMe.h"
#include "LED.h"
#include <Wire.h> // lib for I2C
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

DefuseMeModule module;
Adafruit_7segment matrix = Adafruit_7segment();
LED armedLED(2);
LED strikeLED[4]{ (9), (6), (5), (3) };

byte blink = 0;
byte dim = 0;
long bombCountdown = 0;
byte bombState = 1;
byte bombStrikes = 0;
unsigned long millisSlot = 0;

///////////////////////////////////////////////////////////////////////////////

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Main Display");

  // init the module engine with SPI and random seed
  module.begin();
  matrix.begin(0x70);
  matrix.setBrightness(15);

  matrix.writeDigitRaw(0, 0x40);
  matrix.writeDigitRaw(1, 0x40);
  matrix.drawColon(true);
  matrix.writeDigitRaw(2, 0x40);
  matrix.writeDigitRaw(3, 0x40);
  matrix.writeDisplay();

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[3]{
    tag(F("ACTIVE"), "false"), //passive module =>no user interaction possible
    tag(F("7SEG"), "1"), //7 segment panels
  tag(F("MAINDISPLAY"), "1"), //main display
  };

  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:8888\n"
    "VERSION:0.2\n"
    "URL:https://defuseme.org/\n"
    "AUTHOR:JK\n"
    "DESC:Main Display\n"
    "REPO:https://github.com/defuseme/DefuseMe\n"),
    ourtags, 3);

  // set module to disarmed
  module.setDisarmed();

  // those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop(void)
{
  if (module.updateState())
  {
    bombCountdown = module.getGameState().time;
    bombState = module.getGameState().state;
    bombStrikes = module.getGameState().strikes;
    if (bombState == 0) {
      Serial.println("success");
    }
    armedLED = bombState;   // switch LED on on armed and BOOM

    blink = !blink;

    int sec = bombCountdown / 1000;
    int min = sec / 60;
    sec = sec % 60;

    matrix.writeDigitNum(0, min / 10);
    matrix.writeDigitNum(1, min % 10);
    matrix.drawColon(blink);
    matrix.writeDigitNum(3, sec / 10);
    matrix.writeDigitNum(4, sec % 10);
    matrix.writeDisplay();
  }

  unsigned long millisAct = millis();
  if (millisAct < millisSlot)
    return;
  millisSlot = millisAct + 4;

  if (bombState == 2)   // BOOM
  {
    for (byte i = 0; i < 4; i++)
      strikeLED[i] = !random(4);
    matrix.setBrightness(random(5, 16));
  }
  else if (bombState == 0)   // disarmed
  {
    for (byte i = 0; i < 4; i++)
      strikeLED[i] = 0;
    for (byte i = 0; i <= 15; i++)   // dim down
    {
      matrix.setBrightness(15 - i);
      matrix.writeDisplay();
      delay(100);
    }
  }
  else   // armed
  {
    dim--;
    for (byte i = 0; i < 4; i++)
      strikeLED[i].dim((bombStrikes > i) ? dim + 23 * i : 0);
  }
}

///////////////////////////////////////////////////////////////////////////////
