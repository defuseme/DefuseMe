/*
StateMachine.h - Base state machine library for project DefuseMe
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
