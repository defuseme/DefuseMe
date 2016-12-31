#include "pins_arduino.h"
#include "DefuseMe.h"

DefuseMeModule module;

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  //Serial.begin (115200);
  //Serial.println(F("startstart"));

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[4];
  ourtags[0] = {.name = F("ACTIVE"), .data = "false"}; //passive module =>no user interaction possible
  ourtags[1] = {.name = F("LED"), .data = "23"}; //23 leds present
  ourtags[2] = {.name = F("BLINKINGLED"), .data = "12"}; //12 leds are blinking
  ourtags[3] = {.name = F("SNO"), .data = "A7B5C1D0D4"}; //serial number label

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:2357\n"
                                "VERSION:0\n"
                                "URL:https://example.com/\n"
                                "AUTHOR:Testperson\n"
                                "DESC:Button and Led Module\n"
                                "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 4);

  //those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    // Demo...
    long countdown = module.getGameState().time;
    byte state = module.getGameState().state;
    byte strikes = module.getGameState().strikes;
  }
}

///////////////////////////////////////////////////////////////////////////////

