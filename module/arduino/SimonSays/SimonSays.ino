#include "pins_arduino.h"
#include "DefuseMe.h"
#include "DebounceInput.h"
#include "PushButton.h"
#include "StateMachine.h"
#include "LED.h"

class AirplaneSwitchesSM : public StateMachine
{
  public:
    enum EState : byte {
      S_SelfTest = 0, // Make sure that all LEDs work and all button are down.
      S_Init,         // Start Game
      S_WaitOn,       // Wait for active button
      S_Button,       // A button has been activated
      S_WaitOff,      // Wait until all buttons to be off again
      S_Boom,         // Ups..
    };

  protected:
    byte ledstate;  // state displayed by the LEDs
    byte button;    // which button has been activated
    byte strikes;   // how many strikes have happend

    virtual byte DoProcessInternal();

  private:
    byte nextstate_0[8] = {6, 10, 7, 1, 0, 10, 2, 1}; // 10 is leads to a trigger but is not used as new ledstate
    byte nextstate_1[8] = {3, 4, 10, 6, 10, 7, 10, 5};
    byte nextstate_2[8] = {10, 0, 5, 10, 2, 3, 4, 10};

    bool running = false;

    short c_boom = 0;
};

#define PIN_SWITCH_1 A1
#define PIN_SWITCH_2 A2
#define PIN_SWITCH_3 A3
#define PIN_SWITCH_4 10


class StateMachineSimon : public StateMachine
{
  public:
    enum EState : byte {
      S_Init = 0,   // Init must be 0 for base class initialisation and watchdog
      S_ButtonPressed,
      S_ButtonReleased,
      S_SequencePlay,
      S_BOOM,
      S_Disarmed,
      S_Wait4Godot,
    };

  protected:
    byte nFadeValue;

    virtual byte DoProcessInternal();

};

byte sequence[8];
#define DIFFICULTY 8


LED simonsLEDs[4] { (9), (6), (5), (3) }; 


DefuseMeModule module;
StateMachineSimon engine;

void setup (void)
{
  Serial.begin (115200);
  Serial.println(F("Simon Says"));

  // init the module engine with SPI and random seed
  module.begin();

  for (int i=0; i<DIFFICULTY; i++) {
    sequence[i] = random(1,4);
  }

  for (int i=0; i < DIFFICULTY; i++) {
    simonsLEDs[sequence[i]] = 1;
    delay(1000);
    simonsLEDs[sequence[i]] = 0;
    delay(1000);
  }

  pinMode(PIN_SWITCH_1, INPUT_PULLUP);
  pinMode(PIN_SWITCH_2, INPUT_PULLUP);
  pinMode(PIN_SWITCH_3, INPUT_PULLUP);
  pinMode(PIN_SWITCH_4, INPUT_PULLUP);

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("BUTTON"), .data = "3"};    //1 button
  ourtags[2] = {.name = F("LED"), .data = "3"};       //3 leds (without armed LED)

  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:2359\n"
                                "VERSION:0\n"
                                "URL:https://defuseme.org/\n"
                                "AUTHOR:Petschge\n"
                                "DESC:3 safety switches and 3 LEDs\n"
                                "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 3);

  // those are not needed anymore
  delete ourtags;
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

  }
//  engine.DoProcess();
  

}

int mapLed(int switchNum, bool even) {
  if (even) return (switchNum+1)%3;
  else return (switchNum+2)%3;
}

