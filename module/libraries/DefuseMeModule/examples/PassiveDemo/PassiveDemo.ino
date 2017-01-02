#include "pins_arduino.h"
#include "DefuseMe.h"

DefuseMeModule module;

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);
  Serial.println(F("Passive Demo"));

  // init the module engine with SPI and random seed
  module.begin();

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[4] {
    tag(F("ACTIVE"), "false"), //passive module =>no user interaction possible
    tag(F("LED"), "23"), //23 leds present
    tag(F("BLINKINGLED"), "12"), //12 leds are blinking
    tag(F("SNO"), "A7B5C1D0D4"), //serial number label
  };

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:2357\n"
                                "VERSION:0.0\n"
                                "URL:https://defuseme.org/\n"
                                "AUTHOR:you\n"
                                "DESC:Passive Demo\n"
                                "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 4);

  // set module to disarmed
  module.setDisarmed();

  //those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    // Demo...
    long bombCountdown = module.getGameState().time;
    byte bombState = module.getGameState().state;
    byte bombStrikes = module.getGameState().strikes;
  }

  // do nothing
}

///////////////////////////////////////////////////////////////////////////////

