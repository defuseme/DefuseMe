#include "pins_arduino.h"
#include "DefuseMe.h"
#include "PinArray.h"
#include "LED.h"

DefuseMeModule module;
PinArray pins(2, 3, 4, 5, 6, 7, 8, 9, A0, A1, A2, A3, A4, A5, 1, 99);
LED armedLED(0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  Serial.println(F("PlugArray4x4"));


}

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
    for (byte i = 0; i < cons; i++){
      Serial.print(c[i].a);
      Serial.print("~");
      Serial.print(c[i].b);
      Serial.println();
    }

}

}
