#include "StateMachine.h"

/// Constructor
StateMachine::StateMachine()
{
  _eState = 0;
  _millisWait = 0;
  _millisWatchDog = 0xFFFFFFFF;
}

void StateMachine::DelayNext(unsigned int t)
{
  _millisWait = _millisAct + t;
}

void StateMachine::WatchDog(unsigned int t)
{
  if ( t == 0)
    _millisWatchDog = 0xFFFFFFFF;
  else
    _millisWatchDog = _millisAct + t;
}

void StateMachine::DoProcess()
{
  _millisAct = millis();
  if (_millisAct < _millisWait)
    return;
  if (_millisAct > _millisWatchDog)
    _eState = 0;

  _eState = DoProcessInternal();   // call derived class method
}

void StateMachine::SetGameState(GameState gamestate)
{
  _gamestate = gamestate;
}



