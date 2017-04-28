#include "pins_arduino.h"
#include "DefuseMe.h"
#include "LED.h"

DefuseMeModule module;
LED led[14] { (2), (3), (4), (5), (6), (7), (8), (9), (A0), (A1), (A2), (A3), (A4), (A6) };

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);
  Serial.println(F("22 Blinking Blue LEDs"));

  // init the module engine with SPI and random seed
  module.begin();

  byte predOn = random(3, 15);
  byte actOn = 0;

  Serial.print(F("LEDs: "));
  for (byte i = 0; i < 14; i++)
  {
    byte x = random(14);
    if ( x < predOn )
    {
      led[i] = 1;
      actOn++;
      Serial.print(F("*"));
    }
    else
      Serial.print(F("."));
  }
  Serial.println();

  Serial.print(F("Pred On: "));
  Serial.print(predOn);
  Serial.println();
  Serial.print(F("Act On: "));
  Serial.print(actOn);
  Serial.println();

  char sBlinking[8];
  sprintf(sBlinking, "%d", actOn);

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3] {
    tag(F("ACTIVE"), "false"), //passive module =>no user interaction possible
    tag(F("LED"), "22"), //22 leds present
    tag(F("BLINKINGLED"), sBlinking), //x leds are blinking
  };

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:0022\n"
                                "VERSION:0.0\n"
                                "URL:https://defuseme.org/\n"
                                "AUTHOR:JK\n"
                                "DESC:22 Blinking Blue LEDs\n"
                                "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 3);

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
    // do nothing
  }
}

///////////////////////////////////////////////////////////////////////////////

