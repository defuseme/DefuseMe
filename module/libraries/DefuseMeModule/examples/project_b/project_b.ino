#include "pins_arduino.h"
#include "DefuseMe.h"

DefuseMeModule module;

void setup (void)
{
  Serial.begin (9600);
  Serial.println(F("startstart"));

  //The Values we want to read from our neighbours
  IntTaggedValue test = IntTaggedValue(F("TEST"));
  IntTaggedValue button = IntTaggedValue(F("BUTTON"));
  TaggedValue* interestingTags[2] =  {&test, &button};


  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("BUTTON"), .data = "1"}; //1 button
  ourtags[2] = {.name = F("LED"), .data = "4"}; //4 leds


  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags, 2, F("ID:2355\n"
                                "VERSION:0\n"
                                "URL:https://example.com/\n"
                                "AUTHOR:Testperson\n"
                                "DESC:Button and Led Module\n"
                                "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 3);


  if (button.hasValue()) {
    Serial.print(F("BUTTON was set. Value: "));
    Serial.println(  button.getValue());
  } else {
    Serial.println(F("BUTTON was not set!"));
  }

  //those are not needed anymore
  delete ourtags;
}


//trigered is used to check if the module already was triggered once
boolean triggered = false;


void loop (void)
{
  if (module.updateState()) {
    int tt = module.getGameState().time;
    //automaticaly explode if there are only 6 sec left
    if ((tt < 6000) && !triggered) {
      Serial.println(F("TRIGGERED"));
      module.trigger();//triger produces a strike on each call
      triggered = true;
    }
/* defuse this module if there are only 7 sec left
    if (tt < 7000) {
      Serial.println(F("entschärft"));
      module.setMyState(0);
    } */
  }
}

