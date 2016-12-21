#include "pins_arduino.h"
#include "DefuseMe.h"
#include "MultiSwitch.h"
#include "PushButton.h"
#include "LED.h"
#include "StateMachine.h"

class StateMachineBS : public StateMachine
{
  public:
    enum EState : byte {
      S_Init = 0,   // Init must be 0 for base class initialisation and watchdog
      S_Wait4Switch0,
      S_SetLED,
      S_Wait4SwitchChange,
      S_BOOM,
      S_BOOM_0,
      S_BOOM_1,
      S_Disarmed,
      S_Wait4Godot,
    };

  protected:
    byte nBlink;
    byte nStep;

    virtual byte DoProcessInternal();
};

#define PIN_BUTTON_INPUT 2
#define PIN_BUTTON_LED 9
#define PIN_ARMED_LED 0
#define LED_ON 0
#define LED_OFF 1

DefuseMeModule module;
StateMachineBS engine;
PushButton blueButton(PIN_BUTTON_INPUT);
LED led[4] { (9), (6), (5), (3) };   // L-Pk, L-Or, R-Pk, R-Or
LED armedled(2);
MultiSwitch multiswitch(A0, A1);   // L, R to GND

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  //Serial.begin (115200);
  //Serial.println(F("startstart"));

  // init "Armed LED" - pin 0 (RX) can not be uses in combination with serial
  pinMode(PIN_ARMED_LED, OUTPUT);
  digitalWrite(PIN_ARMED_LED, LED_ON);

  pinMode(PIN_BUTTON_LED, OUTPUT);

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("SWITCH"), .data = "1"}; //1 switch
  ourtags[2] = {.name = F("LED"), .data = "4"}; //4 leds

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:2352\n"
                                "VERSION:0\n"
                                "URL:https://example.com/\n"
                                "AUTHOR:JK\n"
                                "DESC:Bif Switch - 4 Lamps\n"
                                "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 3);
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    engine.SetGameState(module.getGameState());
  }

  multiswitch.DoProcess();
  engine.DoProcess();
}

///////////////////////////////////////////////////////////////////////////////

byte StateMachineBS::DoProcessInternal()
{
  byte mask = 1;

  switch (_eState)
  {

    // S_Init - initialize state machine. Also called on reentry from watchdog

    default:
    case S_Init:
      WatchDog(0);   // reset watchdog
      armedled = 1;
      module.setMyState(1);   // armed
      nStep = 0;
      return S_Wait4Switch0;

    // S_WaitSwitch0 - Blink till the user/admin set the switch to '0'

    case S_Wait4Switch0:
      if (multiswitch == 0)
        return S_SetLED;

      nBlink = !nBlink;
      for (byte i = 0; i < 4; i++)
        led[i] = nBlink;
      DelayNext(100);   // flicker with 5Hz
      break;   // stay in this state

    case S_SetLED:
      for (byte i = 0; i < 4; i++)
      {
        led[i] = 1;
        mask <<= 1;
      }
      return S_Wait4SwitchChange;

    // S_ButtonPressed - action at button was pressed

    case S_Wait4SwitchChange:
      if (!multiswitch.IsChanged())
        break;

      if (multiswitch == 1)
      {
        nStep++;
        if (0)
          return S_Disarmed;

        return S_SetLED;
      }
      return S_BOOM;

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
      if (blueButton.IsJustReleased())
        return S_BOOM;
      break;
  }

  return _eState;   // no state change - repeat actual state
}


