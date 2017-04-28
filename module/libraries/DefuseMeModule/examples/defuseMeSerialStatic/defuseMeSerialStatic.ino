#include "pins_arduino.h"
#include "DefuseMe.h"

DefuseMeModule module;

void setup (void)
{
  Serial.begin (9600);
  Serial.println(F("startstart"));

  //The Values we want to read from our neighbours
  TaggedValue* interestingTags[0] =  {};


  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "false"}; //inactive module =>user interaction not possible
  ourtags[1] = {.name = F("SERIAL"), .data = "189993"}; //serialnumber


  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags, 0, F("ID:BEEF\n"
                                "VERSION:0\n"
                                "URL:https://example.com/\n"
                                "AUTHOR:Pfoet\n"
                                "DESC:Serialnumber\n"
                                "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 2);


  //those are not needed anymore
  delete ourtags;
}


void loop (void)
{
//do nothing
}

