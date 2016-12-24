#ifndef __STATEMACHINE__
#define __STATEMACHINE__
#include <Arduino.h>
#include "DefuseMe.h"

class StateMachine
{
  public:
    StateMachine();
    ~StateMachine() {};

    /// DoProcess - called from main loop as often as possible. This Function is/should not blocking
    void DoProcess();

    void SetGameState(GameState gamestate);

  protected:
    byte _eState;
    unsigned long _millisAct;
    unsigned long _millisWait;
    unsigned long _millisWatchDog;
    GameState _gamestate;

    /// DelayNext - delay the next state excecution by 't' ms. Can be used as a wait function without blocking loop
    void DelayNext(unsigned int t);

    /// WatchDog - set timeout in 't' ms. After timeout state machine switches to state S_Init
    void WatchDog(unsigned int t);

    /// DoProcessInternal - called from DoProcess as abstract method
    virtual byte DoProcessInternal() = 0;   // must be implemented in derived class
};

#endif   // __STATEMACHINE__
