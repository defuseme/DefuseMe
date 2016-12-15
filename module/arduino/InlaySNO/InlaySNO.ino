#include "pins_arduino.h"
#include "DefuseMe.h"
#include "DipSwitch.h"

DefuseMeModule module;
DipSwitch dip(9, 8, 7, 6, 5, 4, 3, 2);
char sSNO[32];

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);

  //TODO generate hash serial number from dip value
  
  sprintf(sSNO, ">%d<", (int)dip);
  Serial.println(sSNO);

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "false"}; //passive module =>no user interaction possible
  ourtags[1] = {.name = F("SNO"), .data = sSNO}; //serial number label

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:0815\n"
                                "VERSION:0.1\n"
                                "URL:https://example.com/\n"
                                "AUTHOR:JK\n"
                                "DESC:Inlay Serial Number\n"
                                "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 2);

}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    byte state = module.getGameState().state;
  }

  // do nothing
}

///////////////////////////////////////////////////////////////////////////////

