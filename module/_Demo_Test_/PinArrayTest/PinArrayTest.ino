#include "PinArray.h"

PinArray pins(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A0, A1, A2, A3, A4);

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  Serial.println(F("PinArray"));

  pinMode(13, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  pins.DoProcess();
  if (pins.IsJustChanged())
  {
    digitalWrite(13, pins.GetConnections());

    Serial.print("Connections: ");
    Serial.print((int)pins.GetConnections());
    Serial.println();

    if (pins.IsAnyGrounded())
    {
      Serial.println("Grounded");
    }

    pins.Print();
  }

}
