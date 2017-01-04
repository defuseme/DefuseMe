#include "pins_arduino.h"
#include "DefuseMe.h"
#include "PushButton.h"
#include "LED.h"
#include "StateMachine.h"
#include "GameCode.h"
#include <Adafruit_NeoPixel.h>

GameCode game;

///////////////////////////////////////////////////////////////////////////////

class StateMachineBlueButton : public StateMachine
{
public:
  enum EState : byte {
    S_Init = 0,   // Init must be 0 for base class initialisation and watchdog
    S_FadeUp,
    S_FadeDown,
    S_ButtonPressed,
    S_Wait4ButtonReleased,
    S_ButtonReleased,
    S_BOOM,
    S_BOOM_0,
    S_BOOM_1,
    S_Disarmed,
    S_Wait4Godot,
  };

  byte sec1Digit;

protected:
  byte nFadeValue;

  virtual byte DoProcessInternal();
};

#define PIN_BUTTON_INPUT 8
#define PIN_BUTTON_LED 9
#define PIN_ARMED_LED 2

uint32_t color[5]{ 0x300000, 0x202000, 0x003000, 0x000030, 0x200020 };  // 0x002020, cyan is not descripable

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(12, 5, NEO_GRB + NEO_KHZ800);
DefuseMeModule module;
StateMachineBlueButton engine;
PushButton blueButton(PIN_BUTTON_INPUT);
LED armedLED(PIN_ARMED_LED);
LED buttonLED(PIN_BUTTON_LED);

byte variant = 0;

///////////////////////////////////////////////////////////////////////////////

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Blue Button - LED Stripe"));

  game.PrintList();

  // init the module engine with SPI and random seed
  module.begin();
  pixels.begin(); // This initializes the NeoPixel library.

  variant = random(5);

  for (byte i = 0; i < 12; i++)
    pixels.setPixelColor(i, color[variant]);
  pixels.show(); // This sends the updated pixel color to the hardware.

  // init "Armed LED" - pin 0 (RX) can not be uses in combination with serial

  armedLED = 1;
  buttonLED = 1;

  // the Values we want to read from our neighbours
  IntTaggedValue blinking(F("BLINKINGLED"));
  SnoTaggedValue sno(F("SNO"));
  TaggedValue* interestingTags[2] = { &blinking, &sno };

  // the Values we want to send out to our neighbours
/*
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("BUTTON"), .data = "1"}; //1 button
  ourtags[2] = {.name = F("STRIPE"), .data = "1"}; //1 leds stripe

  ourtags[0] = tag(F("ACTIVE"), "true"); //active module =>user interaction possible
  ourtags[1] = tag(F("BUTTON"), "1"); //1 button
  ourtags[2] = tag(F("STRIPE"), "1"); //1 leds stripe
*/
  tag *ourtags = new tag[3]{
    tag(F("ACTIVE"), "true"), //active module =>user interaction possible
    tag(F("BUTTON"), "1"), //1 button
    tag(F("STRIPE"), "1") //1 leds stripe
  };


  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags, 2, F("ID:1111\n"
    "VERSION:0.1\n"
    "URL:https://defuseme.org/\n"
    "AUTHOR:JK\n"
    "DESC:Blue Button - LED Stripe\n"
    "REPO:https://github.com/defuseme/DefuseMe\n"),
    ourtags, 3);

  byte digit1 = 255;
  byte digit2 = 255;
  byte digit3 = 255;
  byte blueLED = 0;

  // parse tags from neighbours
  if (blinking.hasValue()) {
    Serial.print(F("BLINKINGLED was set. Value: "));
    Serial.println(blinking.getValue());
    blueLED = blinking.getValue();
  }
  else
    Serial.println("No blinking LED");

  if (sno.hasValue()) {
    Serial.print(F("SNO was set. Value: "));
    Serial.println((char*)sno.getString());
    if (1)   // test only
    {
      for (byte i = 0; i < 12; i++)
        pixels.setPixelColor(i, 0, 0, (sno.getDigit(i) - '0') * 20);
      pixels.show(); // This sends the updated pixel color to the hardware.
    }

    digit1 = sno.getDigit(0) - '0';
    digit2 = sno.getDigit(1) - '0';
    digit3 = sno.getDigit(2) - '0';
  }
  else
    Serial.println("No SNO");

  game.CalcSNOCode(digit1, digit2, digit3, blueLED);

  Serial.print(F("Button action: "));
  Serial.print(game._buttonPressAt);
  Serial.print(F(", "));
  Serial.print(game._buttonReleaseAt);
  Serial.println();

  // those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop(void)
{
  if (module.updateState())   // any change from bomb data
  {
    engine.SetGameState(module.getGameState());

    int sec = module.getGameState().time / 1000;
    engine.sec1Digit = sec % 10;
  }

  engine.DoProcess();
}

///////////////////////////////////////////////////////////////////////////////

byte StateMachineBlueButton::DoProcessInternal()
{
  switch (_eState)
  {
    // S_Init - initialize state machine. Also called on reentry from watchdog

  default:
  case S_Init:
    WatchDog(0);   // reset watchdog
    blueButton.DoReset();   // reset pressed and released flags in button class
    buttonLED = 1;   // turn the LED on
    armedLED = 1;
    module.setArmed();   // set module to armed
    nFadeValue = 255;
    return S_FadeDown;

    // S_FadeUp / S_FadeDown - fade button LED up und down and up und down and up und down and ... leave when button is pressed

  case S_FadeUp:
    if (blueButton.IsJustPressed())
      return S_ButtonPressed;
    nFadeValue++;
    buttonLED.dim(nFadeValue);
    DelayNext(2);
    if (nFadeValue == 255)
      return S_FadeDown;
    break;

  case S_FadeDown:
    if (blueButton.IsJustPressed())
      return S_ButtonPressed;
    nFadeValue--;
    buttonLED.dim(nFadeValue);
    DelayNext(2);
    if (nFadeValue == 0)
      return S_FadeUp;
    break;

    // S_ButtonPressed - action at button was pressed

  case S_ButtonPressed:
    buttonLED = 1;
    // >>> code your game logic here
    if (!game.IsButtonPressAt(sec1Digit))
      return S_BOOM;
    WatchDog(10000);   // if button pressed for more than 10sec, reset state machine
    return S_Wait4ButtonReleased;

  case S_Wait4ButtonReleased:
    if (blueButton.IsJustReleased())
      return S_ButtonReleased;
    break;

    // S_ButtonReleased - action at button was released

  case S_ButtonReleased:
    WatchDog(0);   // reset watchdog
    // >>> code your game logic here
    if (!game.IsButtonReleaseAt(sec1Digit))
      return S_BOOM;
    //      if (blueButton.GetDownTime() < 2000)
    //        return S_BOOM;
    //      else
    return S_Disarmed;
    break;

    // S_BOOM - set bomb to triggered. Forces a strike and come back to armed state

  case S_BOOM:
    module.trigger();   // triger produces a strike on each call
    WatchDog(1000);   // reset state machine after 2sec flickering
    return S_BOOM_0;

  case S_BOOM_0:
    armedLED = 1;
    buttonLED = 0;
    DelayNext(50);   // flicker with 10Hz
    return S_BOOM_1;   // endless loop - ended by watchdog

  case S_BOOM_1:
    armedLED = 0;
    buttonLED = 1;
    DelayNext(50);   // flicker with 10Hz
    return S_BOOM_0;   // endless loop - ended by watchdog

  // S_Disarmed - set bomb to disarmed and be quiet

  case S_Disarmed:
    module.setDisarmed();   // set module to disarmed
    armedLED = 0;
    buttonLED = 0;
    pixels.clear();
    pixels.show(); // This sends the updated pixel color to the hardware.
    return S_Wait4Godot;

    // S_Wait4Godot - wait forever (until button pressed again)

  case S_Wait4Godot:
    if (blueButton.IsJustPressed())
      buttonLED = 1;
    if (blueButton.IsJustReleased())
      return S_BOOM;
    break;
  }

  return _eState;   // no state change - repeat actual state
}


