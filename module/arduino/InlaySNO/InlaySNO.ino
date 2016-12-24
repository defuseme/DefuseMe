#include "pins_arduino.h"
#include "DefuseMe.h"
#include "DipSwitch.h"

DefuseMeModule module;
DipSwitch dip(9/*LSB*/, 8, 7, 6, 5, 4, 3, 2/*MSB*/);
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
  Serial.println(F("Inlay SNO"));

  // init the module engine with SPI and random seed
  module.begin();

  // print serial number list for documentation
  PrintList();

  // generate preudo random serial number from dip value
  byte base = dip;
  GenerateSNO(base);

  Serial.print("SNO: ");
  Serial.println(sSNO);
  Serial.print("DIP: ");
  Serial.println(base);

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "false"}; // passive module =>no user interaction possible
  ourtags[1] = {.name = F("SNO"), .data = sSNO}; // serial number label with 15 numerical digits

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:0123\n"
                                "VERSION:0.2\n"
                                "URL:https://defuseme.org/\n"
                                "AUTHOR:JK\n"
                                "DESC:Inlay Serial Number\n"
                                "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 2);
  
  // set to disarmed
  module.setMyState(0);
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    // demo
    byte state = module.getGameState().state;
  }

  // do nothing
}

///////////////////////////////////////////////////////////////////////////////

