#include "pins_arduino.h"
#include "DefuseMe.h"

DefuseMeModule module;

int getBatteries() {
  float volts = analogRead(A0) * (5.0 / 1023.0);
  if (volts < 1.0) {
    return 0;
  } else if ((volts >= 1.0) && (volts < 2.0) ) {
    return 1;
  } else if ((volts >= 2.0) && (volts < 3.3) ) {
    return 2;
  } else if ((volts >= 3.3) ) {
    return 3;
  }
}


int numbats = getBatteries();

void setup (void)
{
  Serial.begin (9600);
  Serial.println(F("startstart"));

  //The Values we want to read from our neighbours
  TaggedValue* interestingTags[0] =  {};


  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  char buffer[4];
  itoa(numbats, buffer, 10);
  ourtags[1] = {.name = F("BATTERY"), .data = buffer}; //number of batteries


  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags, 0, F("ID:BAAD\n"
                     "VERSION:0\n"
                     "URL:https://example.com/\n"
                     "AUTHOR:Pfoet\n"
                     "DESC:Battery count\n"
                     "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 2);


  //those are not needed anymore
  delete ourtags;
}


void loop (void)
{
  //tamper protection:
  if (numbats != getBatteries()) {
    module.trigger();
  }

}

