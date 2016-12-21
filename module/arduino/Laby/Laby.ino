#include "DefuseMe.h"
#include "PushButton.h"
#include "LED.h"
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#define LABY_MOVE_U 1
#define LABY_MOVE_R 2
#define LABY_MOVE_D 4
#define LABY_MOVE_L 8
#define LABY_MOVE_MASK 15
#define LABY_START 16
#define LABY_DEST 32

const byte aMatrix[8][8][8] =
{
  /*
    4	,	6	,	10	,	10	,	12	,	2	,	12	,	4	,
    5	,	7	,	8	,	6	,	9	,	6	,	11	,	13	,
    3	,	29	,	2	,	9	,	2	,	13	,	6	,	9	,
    6	,	15	,	8	,	4	,	6	,	9	,	5	,	4	,
    1	,	7	,	10	,	9	,	7	,	12	,	35	,	9	,
    6	,	9	,	2	,	12	,	1	,	5	,	6	,	8	,
    5	,	6	,	10	,	13	,	6	,	15	,	11	,	12	,
    3	,	11	,	8	,	3	,	9	,	1	,	2	,	9,
  */
  //0
  4 , 4 , 6 , 12  , 2 , 14  , 8 , 4 ,
  7 , 27  , 9 , 7 , 10  , 9 , 6 , 9 ,
  7 , 8 , 6 , 9 , 4 , 6 , 9 , 4 ,
  1 , 6 , 11  , 8 , 3 , 13  , 2 , 13  ,
  4 , 5 , 6 , 12  , 4 , 3 , 12  , 5 ,
  7 , 11  , 13  , 3 , 13  , 6 , 43  , 9 ,
  1 , 6 , 9 , 6 , 9 , 3 , 10  , 12  ,
  2 , 11  , 8 , 3 , 10  , 10  , 10  , 9 ,
  //1
  2 , 12  , 6 , 8 , 2 , 14  , 10  , 12  ,
  6 , 13  , 7 , 10  , 12  , 1 , 6 , 13  ,
  5 , 3 , 9 , 4 , 5 , 2 , 45  , 1 ,
  19  , 12  , 6 , 11  , 9 , 6 , 13  , 4 ,
  6 , 9 , 5 , 2 , 12  , 1 , 3 , 13  ,
  3 , 12  , 3 , 12  , 3 , 14  , 14  , 9 ,
  6 , 11  , 12  , 5 , 6 , 9 , 3 , 12  ,
  1 , 2 , 9 , 3 , 11  , 10  , 8 , 1 ,
  //2
  6 , 8 , 6 , 26  , 12  , 6 , 14  , 8 ,
  7 , 10  , 13  , 6 , 9 , 1 , 3 , 12  ,
  1 , 6 , 9 , 3 , 14  , 8 , 6 , 13  ,
  2 , 13  , 4 , 2 , 9 , 6 , 13  , 1 ,
  6 , 11  , 11  , 12  , 2 , 13  , 3 , 12  ,
  7 , 8 , 6 , 9 , 6 , 9 , 2 , 13  ,
  1 , 6 , 9 , 6 , 9 , 4 , 6 , 9 ,
  2 , 11  , 10  , 9 , 2 , 11  , 43  , 8 ,
  //3
  4 , 2 , 12  , 6 , 8 , 2 , 14  , 8 ,
  7 , 14  , 11  , 11  , 12  , 4 , 7 , 12  ,
  1 , 5 , 6 , 8 , 3 , 11  , 9 , 5 ,
  6 , 9 , 3 , 28  , 2 , 14  , 10  , 13  ,
  5 , 2 , 14  , 11  , 12  , 1 , 6 , 9 ,
  3 , 12  , 3 , 8 , 5 , 4 , 5 , 4 ,
  2 , 13  , 6 , 10  , 9 , 5 , 3 , 13  ,
  2 , 11  , 11  , 8 , 2 , 11  , 10  , 41 ,
  //4
  2 , 14  , 10  , 8 , 6 , 8 , 6 , 8 ,
  2 , 13  , 6 , 12  , 3 , 14  , 13  , 4 ,
  4 , 3 , 29  , 3 , 10  , 9 , 7 , 13  ,
  7 , 10  , 9 , 2 , 12  , 2 , 13  , 1 ,
  7 , 8 , 6 , 14  , 11  , 8 , 3 , 12  ,
  1 , 6 , 9 , 3 , 12  , 6 , 8 , 5 ,
  6 , 11  , 12  , 2 , 43  , 9 , 6 , 13  ,
  1 , 2 , 11  , 10  , 10  , 10  , 9 , 1 ,
  //5
  6 , 10  , 12  , 6 , 14  , 8 , 6 , 8 ,
  1 , 4 , 7 , 9 , 3 , 10  , 13  , 4 ,
  6 , 9 , 5 , 2 , 12  , 6 , 9 , 5 ,
  3 , 14  , 11  , 8 , 7 , 9 , 6 , 9 ,
  4 , 7 , 12  , 2 , 13  , 2 , 43  , 12  ,
  21  , 1 , 7 , 8 , 3 , 12  , 6 , 9 ,
  3 , 14  , 9 , 4 , 6 , 11  , 13  , 4 ,
  2 , 11  , 8 , 3 , 11  , 8 , 3 , 9 ,
  //6
  6 , 10  , 12  , 6 , 8 , 6 , 12  , 4 ,
  1 , 6 , 13  , 5 , 6 , 9 , 7 , 13  ,
  6 , 9 , 3 , 15  , 9 , 2 , 9 , 5 ,
  7 , 8 , 4 , 1 , 6 , 26  , 12  , 5 ,
  3 , 10  , 15  , 12  , 1 , 6 , 11  , 9 ,
  6 , 8 , 1 , 3 , 12  , 3 , 12  , 4 ,
  3 , 14  , 12  , 6 , 13  , 2 , 15  , 13  ,
  2 , 41  , 3 , 9 , 3 , 8 , 1 , 1 ,
  //7
  4 , 2 , 14  , 14  , 8 , 22  , 12  , 4 ,
  7 , 14  , 9 , 1 , 2 , 13  , 7 , 9 ,
  1 , 3 , 14  , 12  , 4 , 1 , 7 , 12  ,
  6 , 10  , 9 , 7 , 11  , 12  , 1 , 5 ,
  3 , 12  , 2 , 9 , 4 , 7 , 8 , 5 ,
  6 , 13  , 2 , 46  , 13  , 3 , 10  , 13  ,
  1 , 7 , 10  , 13  , 3 , 14  , 8 , 1 ,
  2 , 9 , 2 , 9 , 2 , 11  , 10  , 8
};

byte labyRot = 0;   // 0..3 - rotate display and joystick
byte labyVar = 0;   // 0..7 - variant of the labyrinth
byte labySwap = 0;   // 0..1 - swap start and destination
byte labyCursorX = 0;
byte labyCursorY = 0;
byte labyDestX = 7;
byte labyDestY = 7;

DefuseMeModule module;
PushButton cursorButtonU(2);   //TODO richtige Pin-Nummern
PushButton cursorButtonR(5);
PushButton cursorButtonD(3);
PushButton cursorButtonL(4);
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
LED armedLED(6);

void swap(byte &a, byte &b)
{
  byte tmp = a;
  a = b;
  b = tmp;
}



void LabyInit()
{
  randomSeed(analogRead(A6)+analogRead(A7));
  labyVar = random(8);
  labyRot = random(4);
  labySwap = random(2);

  Serial.print("Var: ");
  Serial.print(labyVar);
  Serial.print(", Rot: ");
  Serial.print(labyRot);
  Serial.print(", Swap: ");
  Serial.print(labySwap);
  Serial.println();

  for (byte y = 0; y < 8; y++)
    for (byte x = 0; x < 8; x++)
    {
      byte a = aMatrix[labyVar][y][x];

      if (a & LABY_START)
      {
        labyCursorX = x;
        labyCursorY = y;
      }
      if (a & LABY_DEST)
      {
        labyDestX = x;
        labyDestY = y;
      }
    }

  if (labySwap)
  {
    swap(labyCursorX, labyDestX);
    swap(labyCursorY, labyDestY);
  }

  Serial.print("Corsor: ");
  Serial.print((int)labyCursorX);
  Serial.print(", ");
  Serial.print((int)labyCursorY);
  Serial.println();

  Serial.print("Dest: ");
  Serial.print((int)labyDestX);
  Serial.print(", ");
  Serial.print((int)labyDestY);
  Serial.println();

  armedLED = 1;
}



void LabyDisplay(byte state = 1)
{
  matrix.clear();      // clear display
  matrix.setRotation(labyRot);

  switch (state)
  {
    case 2:   // BOOM
      matrix.fillRect(0, 0, 8, 8, LED_YELLOW);
    //matrix.drawLine(0, 0, 7, 7, LED_YELLOW);
    //matrix.drawLine(0, 7, 7, 0, LED_YELLOW);
    //vvv

    case 1:   // armed
    default:
      matrix.drawPixel(labyDestX, labyDestY, LED_RED);
      matrix.drawPixel(labyCursorX, labyCursorY, LED_GREEN);
      armedLED = 1;
      break;

    case 0:   // disarmed
      armedLED = 0;
      break;
  }

  matrix.writeDisplay();  // write the changes we just made to the display
}



void LabyMoveCursor(byte joystick)
{
  joystick &= LABY_MOVE_MASK;

  // rotate joystick to fit the display
  joystick |= joystick << 4;
  joystick >>= labyRot;
  joystick &= LABY_MOVE_MASK;

  byte a = aMatrix[labyVar][labyCursorY][labyCursorX];

  if (joystick & a)   // can move
  {
    if (joystick & LABY_MOVE_U) labyCursorY--;
    if (joystick & LABY_MOVE_R) labyCursorX++;
    if (joystick & LABY_MOVE_D) labyCursorY++;
    if (joystick & LABY_MOVE_L) labyCursorX--;

    // display changes
    LabyDisplay();
    Serial.print("Corsor: ");
    Serial.print((int)labyCursorX);
    Serial.print(", ");
    Serial.print((int)labyCursorY);
    Serial.println();

    // check if destination reached
    if (labyCursorX == labyDestX && labyCursorY == labyDestY)
    {
      // dirarmed
      module.setMyState(0);   // set bomb to disarmed
      LabyDisplay(0);
      Serial.println(F("Disarmed"));
    }
  }
  else   // hit the wall
  {
    //trigger
    module.trigger();   // triger produces a strike on each call
    LabyDisplay(2);
    Serial.println(F("BOOM"));
  }
}


void setup() {
  Serial.begin (115200);
  Serial.println(F("Labyrinth"));

  matrix.begin(0x70);  // pass in the address

  LabyInit();
  LabyDisplay();

  //The Values we want to read from our neighbours
  IntTaggedValue test = IntTaggedValue(F("TEST"));
  IntTaggedValue button = IntTaggedValue(F("BUTTON"));
  TaggedValue* interestingTags[2] =  {&test, &button};


  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("BUTTON"), .data = "4"}; //4 button
  ourtags[2] = {.name = F("MATRIX"), .data = "1"}; //1 matrix

  
    //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
    module.waitForInit(interestingTags, 2, F("ID:1234\n"
                                  "VERSION:0\n"
                                  "URL:https://example.com/\n"
                                  "AUTHOR:JK\n"
                                  "DESC:Labyrinth\n"
                                  "REPO:https://github.com/me/awesome-module.git\n"),
                       ourtags, 3);
    

    //those are not needed anymore
    delete ourtags;
 

}

void loop() {
  if (module.updateState())
  {
    LabyDisplay(module.getGameState().state);
  }

  if (cursorButtonU.IsJustPressed())
    LabyMoveCursor(LABY_MOVE_U);
  if (cursorButtonR.IsJustPressed())
    LabyMoveCursor(LABY_MOVE_R);
  if (cursorButtonD.IsJustPressed())
    LabyMoveCursor(LABY_MOVE_D);
  if (cursorButtonL.IsJustPressed())
    LabyMoveCursor(LABY_MOVE_L);
}
