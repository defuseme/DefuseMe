#include "pins_arduino.h"
#include "DefuseMe.h"
#include "LED.h"

///////////////////////////////////////////////////////////////////////////////

#define PIN_ARMED_LED 3

DefuseMeModule module;
LED armedLED(PIN_ARMED_LED);

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);
  Serial.println(F("Simple Demo"));

  // init the module engine with SPI and random seed
  module.begin();

  // init "Armed LED" - pin 0 (RX) can not be uses in combination with serial
  armedLED = 1;   // switch LED on

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[2] {
    tag(F("ACTIVE"), "true"), //active module =>user interaction possible
    tag(F("ANYTHING"), "1"), //1 ...
  };

  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:0815\n"
                                "VERSION:0.1\n"
                                "URL:https://defuseme.org/\n"
                                "AUTHOR:JK\n"
                                "DESC:Simple Demo\n"
                                "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 2);

  // those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())   // any change from bomb data
  {
    // demo
    unsigned long bombTime = module.getGameState().time;
    byte bombState = module.getGameState().state;
    byte bombStrikes = module.getGameState().strikes;
    // >>> do somthing with game state for game logic
    if (0)   // if game logic says BOOM
    {
      module.trigger();   // triger produces a strike on each call
      // begin blinking/flickering - repeat with armed state because a trigger adds a strike 
    }
    if (0)   // if game logic says disarmed
    {
      module.setDisarmed();   // set module to disarmed
      armedLED = 0;   // switch LED off
      // set lighting/GUI to disarmed
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

