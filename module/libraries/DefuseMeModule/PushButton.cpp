/*
PushButton.cpp - Button input library for project DefuseMe
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
#include "PushButton.h"

PushButton::PushButton (int Pin) : DebounceInput(Pin, HIGH)
{
  DoReset();
}

void PushButton::DoReset()
{
  _IsPushed = false;
  _IsReleased = false;
  _millisPressed = 0;
}

void PushButton::ChangeTo0()
{
  _IsPushed = true;
  _IsReleased = false;
  _millisPressed = millis();
}

void PushButton::ChangeTo1()
{
  _IsReleased = true;
  _millisReleased = millis();
}

bool PushButton::IsJustPressed()
{
  DoProcess();
  if (_IsPushed)
  {
    _IsPushed = false;
    return true;
  }

  return false;
}

bool PushButton::IsJustReleased()
{
  DoProcess();
  if (_IsReleased)
  {
    _IsReleased = false;
    return true;
  }

  return false;
}

bool PushButton::IsDown()
{
  DoProcess();
  return !GetValue();
}

void PushButton::DoProcess()
{
  DebounceInput::DoProcess();
}

unsigned int PushButton::GetDownTime()
{
  return _millisReleased - _millisPressed;
}

