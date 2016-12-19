#include "pins_arduino.h"
#include "DefuseMe.h"
#include "DipSwitch.h"

DefuseMeModule module;
DipSwitch dip(9, 8, 7, 6, 5, 4, 3, 2);
char sSNO[32];

///////////////////////////////////////////////////////////////////////////////

void GenerateSNO(byte base)
{
  randomSeed(base + 42);

  for (byte i = 0; i < 15; i++)
  {
    byte r = random(10);
    sSNO[i] = r + '0';
  }
  sSNO[15] = 0;
}

///////////////////////////////////////////////////////////////////////////////

void PrintList()
{
  // print headline with keys
  Serial.println("Index,DIP,FlipDIP,SNO");

  for (int i = 0; i < 256; i++)
  {
    GenerateSNO(i);

    //print index
    Serial.print(i);
    Serial.print(",");

    // print DIP switch pattern
    Serial.print("[");
    for (int mask = 0x80; mask; mask >>= 1)
    {
      if (i & mask)
        Serial.print("^");
      else
        Serial.print("v");
    }
    Serial.print("],");

    // print flipped DIP switch pattern
    Serial.print("[");
    for (int mask = 0x01; mask < 256; mask <<= 1)
    {
      if (i & mask)
        Serial.print("v");
      else
        Serial.print("^");
    }
    Serial.print("],");

    // printserial number as string
    Serial.print(sSNO);
    Serial.println();
  }
}

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);

  //print serial number list for documentation
  PrintList();

  //generate preudo random serial number from dip value
  byte base = dip;
  GenerateSNO(base);

  Serial.print("SNO: ");
  Serial.println(sSNO);
  Serial.print("DIP: ");
  Serial.println(base);

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "false"}; //passive module =>no user interaction possible
  ourtags[1] = {.name = F("SNO"), .data = sSNO}; //serial number label

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:0123\n"
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

