#include "PinArray.h"

//PinArray pins(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A0, A1, A2, A3, A4);
PinArray pins(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A0, A1, A2, A3, 99);
//PinArray pins(2, 3, 4, 5, 6, 7, 8, 9, A0, A1, A2, A3, A4, A5, 1, 99/*GND*/);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin (115200);
  Serial.println(F("PinArray TEST"));

  pinMode(13, OUTPUT);
}

void loop()
{
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
