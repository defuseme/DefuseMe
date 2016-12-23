#include "pins_arduino.h"
#include "DefuseMe.h"
#include "PinArray.h"
#include "LED.h"

DefuseMeModule module;
PinArray pins(2, 3, 4, 5, 6, 7, 8, 9, A0, A1, A2, A3, A4, A5, 1, 99/*GND*/);
LED armedLED(0);

///////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin (115200);
  Serial.println(F("PlugArray4x4"));

  // init the module engine with SPI and random seed
  module.begin();

  // the Values we want to read from our neighbours
  IntTaggedValue snoTag = IntTaggedValue(F("SNO"));
  IntTaggedValue blueblinkTag = IntTaggedValue(F("BLUEBLINK"));
  TaggedValue* interestingTags[2] =  {&snoTag, &blueblinkTag};

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; // passive module =>no user interaction possible
  ourtags[1] = {.name = F("PLUG"), .data = "16"}; // 16 Post plugs

  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags,  2, F("ID:0044\n"
                     "VERSION:0.0\n"
                     "URL:https://defuseme.org/\n"
                     "AUTHOR:JK\n"
                     "DESC:Plug Array 4x4 - AT WORK\n"
                     "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 2);

  // those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:
  pins.DoProcess();
  if (pins.IsJustChanged())
  {
    //    digitalWrite(0, pins.GetConnections());

    Serial.print("Connections: ");
    Serial.print((int)pins.GetConnections());
    Serial.println();

    if (pins.IsAnyGrounded())
    {
      Serial.println("Grounded");
    }

    pins.Print();

    PinArrayConnection c[10];
    byte cons = 0;
    cons = pins.GetConnections(&c[0], 10);
    for (byte i = 0; i < cons; i++) {
      Serial.print(c[i].a);
      Serial.print("~");
      Serial.print(c[i].b);
      Serial.println();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

