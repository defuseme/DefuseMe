#include "pins_arduino.h"
#include "DefuseMe.h"
#include "MultiSwitch.h"
#include "LED.h"
#include "StateMachine.h"

struct STATES {
  byte lamps;
  byte switchpos;
  };
  
// Generated wit Excel sheet BigSwitch.xlsx - do not change manually				
// Lamps: 1=L-Pk 2=L-Or 4=R-Pk 8=R-Or   Switch: 0=0 L=1 R=2				
STATES states[10][10] = {				
1	,	0	,	// 0
2	,	0	,	
4	,	0	,	
8	,	0	,	
0	,	1	,	
0	,	2	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 1
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 2
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 3
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 4
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 5
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 6
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 7
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 8
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	// 9
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0	,	
0	,	0		
};			

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
    byte _blink;
    byte _step;
    byte _variant;

    virtual byte DoProcessInternal();
};

DefuseMeModule module;
StateMachineBS engine;
LED gameLED[4] { (9), (6), (5), (3) };   // L-Pk, L-Or, R-Pk, R-Or
LED armedLED(2);
MultiSwitch multiswitch(A0, A1);   // L, R to GND

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);
  Serial.println(F("Big Switch"));

  // init the module engine with SPI and random seed
  module.begin();

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("SWITCH"), .data = "1"}; //1 switch
  ourtags[2] = {.name = F("LED"), .data = "4"}; //4 gameLEDs

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:0104\n"
                                "VERSION:0.0\n"
                                "URL:https://defuseme.org/\n"
                                "AUTHOR:JK\n"
                                "DESC:Bif Switch - 4 Lamps - AT WORK\n"
                                "REPO:https://github.com/defuseme/DefuseMe\n"),
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

    // S_Init - initialize state machine. Also calgameLED on reentry from watchdog

    default:
    case S_Init:
      WatchDog(0);   // reset watchdog
      armedLED = 1;
      module.setMyState(1);   // armed
      _step = 0;
      _variant = 0;//TODO
      return S_Wait4Switch0;

    // S_WaitSwitch0 - Blink till the user/admin set the switch to '0'

    case S_Wait4Switch0:
      if (multiswitch == 0)
        return S_SetLED;

      _blink = !_blink;
      for (byte i = 0; i < 4; i++)
        gameLED[i] = _blink;
      DelayNext(100);   // flicker with 5Hz
      break;   // stay in this state

    case S_SetLED:
      for (byte i = 0; i < 4; i++)
      {
        gameLED[i] = states[_variant][_step].lamps & mask;
        mask <<= 1;
      }
      return S_Wait4SwitchChange;

    // S_ButtonPressed - action at button was pressed

    case S_Wait4SwitchChange:
      if (! multiswitch.IsChanged())
        break;

      if (multiswitch == states[_variant][_step].switchpos)
      {
        _step++;
        if (states[_variant][_step].lamps == 0)
          return S_Disarmed;

        return S_SetLED;
      }
      return S_BOOM;

    // S_BOOM - set bomb to triggered. Forces a strike and come back to armed state

    case S_BOOM:
      module.trigger();   // triger produces a strike on each call
      WatchDog(1000);   // reset state machine after 2sec flickering
      return S_BOOM_0;

    case S_BOOM_0:
      armedLED = 1;
      for (byte i = 0; i < 4; i++)
        gameLED[i] = (random(3) == 0);
      DelayNext(50);   // flicker with 10Hz
      return S_BOOM_1;   // endless loop - ended by watchdog

    case S_BOOM_1:
      armedLED = 0;
      for (byte i = 0; i < 4; i++)
        gameLED[i] = (random(3) == 0);
      DelayNext(50);   // flicker with 10Hz
      return S_BOOM_0;   // endless loop - ended by watchdog

    // S_Disarmed - set bomb to disarmed and be quiet

    case S_Disarmed:
      module.setMyState(0);   // set bomb to disarmed
      armedLED = 0;
      for (byte i = 0; i < 4; i++)
        gameLED[i] = 0;
      return S_Wait4Godot;

    // S_Wait4Godot - wait forever (until button pressed again)

    case S_Wait4Godot:
      if (multiswitch)   // want to touch a disarmed module? OK!?! As you wish!!!
        return S_BOOM;
      break;
  }

  return _eState;   // no state change - repeat actual state
}


