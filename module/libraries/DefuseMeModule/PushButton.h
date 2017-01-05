/*
PushButton.h - Button input library for project DefuseMe
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
#ifndef __PUSHBUTTON__
#define __PUSHBUTTON__
#include <Arduino.h>
#include "DebounceInput.h"

class PushButton : protected DebounceInput
{
  public:
    PushButton (int Pin);

    bool IsJustPressed();
    bool IsJustReleased();
    bool IsDown();
    unsigned int GetDownTime();

    /// DoProcess - called from main loop as often as possible. This Function is/should not blocking
    void DoProcess();

    void DoReset();

  protected:
    bool _IsPushed;
    bool _IsReleased;
    unsigned long _millisPressed;
    unsigned long _millisReleased;

    virtual void ChangeTo0();
    virtual void ChangeTo1();
};
#endif   // __PUSHBUTTON__
