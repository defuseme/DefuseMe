/*
StateMachine.cpp - Base state machine library for project DefuseMe
Copyright (c) 2016 Nerd2Nerd.org - Jochen Krapf. All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
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



