#include "pins_arduino.h"
#include "DefuseMe.h"
#include "PushButton.h"
#include "LED.h"
#include "StateMachine.h"

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

protected:
  byte nFadeValue;

  virtual byte DoProcessInternal();
};

#define PIN_BUTTON_INPUT 2
#define PIN_BUTTON_LED 9
#define PIN_ARMED_LED 3

DefuseMeModule module;
StateMachineBlueButton engine;
PushButton blueButton(PIN_BUTTON_INPUT);
LED armedLED(PIN_ARMED_LED);
LED buttonLED(PIN_BUTTON_LED);

///////////////////////////////////////////////////////////////////////////////

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("State Machine Demo"));

  // init the module engine with SPI and random seed
  module.begin();

  // init "Armed LED" - pin 0 (RX) can not be uses in combination with serial
  armedLED = 1;
  buttonLED = 1;

  // the Values we want to read from our neighbours
  IntTaggedValue blinkingTag(F("BLINKINGLED"));
  SnoTaggedValue snoTag(F("SNO"));
  TaggedValue* interestingTags[2] = { &blinkingTag, &snoTag };

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[2]{
    tag(F("ACTIVE"), "true"), //active module =>user interaction possible
    tag(F("BUTTON"), "1"), //1 button
  };

  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags, 2, F("ID:0815\n"
    "VERSION:0.1\n"
    "URL:https://defuseme.org/\n"
    "AUTHOR:JK\n"
    "DESC:State Machine Demo\n"
    "REPO:https://github.com/defuseme/DefuseMe\n"),
    ourtags, 2);

  // parse tags from neighbours
  if (blinkingTag.hasValue()) {
    Serial.print(F("BLINKINGLED was set. Value: "));
    Serial.println(blinkingTag.getValue());
    // >>> do somthing with the count of LEDs for game logic
    // blueLED = blinkingTag.getValue();
  }
  else
    Serial.println("No blinking LED");

  if (snoTag.hasValue()) {
    Serial.print(F("SNO was set. Value: "));
    Serial.println((char*)snoTag.getString());
    // >>> do somthing with the serial number for game logic
    // digit1 = snoTag.getDigit(0) - '0';
  }
  else
    Serial.println("No SNO");

  // those are not needed anymore
  delete ourtags;
}

///////////////////////////////////////////////////////////////////////////////

void loop(void)
{
  if (module.updateState())   // any change from bomb data
  {
    engine.SetGameState(module.getGameState());
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
    if (blueButton.GetDownTime() < 2000)
      return S_BOOM;
    else
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

