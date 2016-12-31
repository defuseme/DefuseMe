#include "pins_arduino.h"
#include "DefuseMe.h"
#include "DebounceInput.h"
#include "PushButton.h"
#include "StateMachine.h"

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
#define PIN_ARMED_LED 0
#define LED_ON 0
#define LED_OFF 1

DefuseMeModule module;
StateMachineBlueButton engine;
PushButton blueButton(PIN_BUTTON_INPUT);

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  //Serial.begin (115200);
  //Serial.println(F("startstart"));

  // init "Armed LED" - pin 0 (RX) can not be uses in combination with serial
  pinMode(PIN_ARMED_LED, OUTPUT);
  digitalWrite(PIN_ARMED_LED, LED_ON);

  pinMode(PIN_BUTTON_LED, OUTPUT);

  //The Values we want to read from our neighbours
  IntTaggedValue test = IntTaggedValue(F("TEST"));
  IntTaggedValue button = IntTaggedValue(F("BUTTON"));
  TaggedValue* interestingTags[2] =  {&test, &button};


  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("BUTTON"), .data = "1"}; //1 button
  ourtags[2] = {.name = F("LED"), .data = "4"}; //4 leds

  /*
    //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
    module.waitForInit(interestingTags, 2, F("ID:2355\n"
                                  "VERSION:0\n"
                                  "URL:https://example.com/\n"
                                  "AUTHOR:Testperson\n"
                                  "DESC:Button and Led Module\n"
                                  "REPO:https://github.com/me/awesome-module.git\n"),
                       ourtags, 3);


    if (button.hasValue()) {
      Serial.print(F("BUTTON was set. Value: "));
      Serial.println(  button.getValue());
    } else {
      Serial.println(F("BUTTON was not set!"));
    }

    //those are not needed anymore
    delete ourtags;
  */
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    // Demo...
    long countdown = module.getGameState().time;
    byte state = module.getGameState().state;
    byte strikes = module.getGameState().strikes;

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
      digitalWrite(PIN_BUTTON_LED, LED_ON);
      digitalWrite(PIN_ARMED_LED, LED_ON);
      module.setMyState(1);   // armed
      nFadeValue = 255;
      return S_FadeDown;

    // S_FadeUp / S_FadeDown - fade button LED up und down and up und down and up und down and ... leave when button is pressed

    case S_FadeUp:
      if (blueButton.IsJustPressed())
        return S_ButtonPressed;
      nFadeValue++;
      analogWrite(PIN_BUTTON_LED, nFadeValue);
      DelayNext(2);
      if (nFadeValue == 255)
        return S_FadeDown;
      break;

    case S_FadeDown:
      if (blueButton.IsJustPressed())
        return S_ButtonPressed;
      nFadeValue--;
      analogWrite(PIN_BUTTON_LED, nFadeValue);
      DelayNext(2);
      if (nFadeValue == 0)
        return S_FadeUp;
      break;

    // S_ButtonPressed - action at button was pressed

    case S_ButtonPressed:
      digitalWrite(PIN_BUTTON_LED, LED_ON);
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
      WatchDog(2000);   // reset state machine after 2sec flickering
      return S_BOOM_0;

    case S_BOOM_0:
      digitalWrite(PIN_ARMED_LED, LED_ON);
      digitalWrite(PIN_BUTTON_LED, LED_OFF);
      DelayNext(50);   // flicker with 10Hz
      return S_BOOM_1;   // endless loop - ended by watchdog

    case S_BOOM_1:
      digitalWrite(PIN_ARMED_LED, LED_OFF);
      digitalWrite(PIN_BUTTON_LED, LED_ON);
      DelayNext(50);   // flicker with 10Hz
      return S_BOOM_0;   // endless loop - ended by watchdog

    // S_Disarmed - set bomb to disarmed and be quiet

    case S_Disarmed:
      module.setMyState(0);   // set bomb to disarmed
      digitalWrite(PIN_ARMED_LED, LED_OFF);
      digitalWrite(PIN_BUTTON_LED, LED_OFF);
      return S_Wait4Godot;

    // S_Wait4Godot - wait forever (until button pressed again)

    case S_Wait4Godot:
      if (blueButton.IsJustPressed())
        digitalWrite(PIN_BUTTON_LED, LED_ON);
      if (blueButton.IsJustReleased())
        return S_BOOM;
      break;
  }

  return _eState;   // no state change - repeat actual state
}


