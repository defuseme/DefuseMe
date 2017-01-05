/*
MultiSwitch.cpp - Multi position/layer switch input library for project DefuseMe
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
#include "MultiSwitch.h"

///////////////////////////////////////////////////////////////////////////////

MultiSwitch::MultiSwitch(int Pin0, int Pin1, int Pin2, int Pin3, int Pin4, int Pin5, int Pin6, int Pin7)
{
  _aPinPort[0] = Pin0;
  _aPinPort[1] = Pin1;
  _aPinPort[2] = Pin2;
  _aPinPort[3] = Pin3;
  _aPinPort[4] = Pin4;
  _aPinPort[5] = Pin5;
  _aPinPort[6] = Pin6;
  _aPinPort[7] = Pin7;

  _nPins = 0;

  for (byte i = 0; i < 8; i++)
  {
    if (_aPinPort[i] >= 0)
    {
      pinMode(_aPinPort[i], INPUT_PULLUP);
      _nPins = i + 1;
    }
  }

  _nDebounceCounter = 999;
  _valueScan = 255;
  Scan();
  _value = _valueScan;
  _bChanged = false;
}

///////////////////////////////////////////////////////////////////////////////

void MultiSwitch::DoProcess()
{
  unsigned long millisAct = millis();
  if (_millisLast == millisAct)
    return;
  _millisLast = millisAct;

  Scan();
}

void MultiSwitch::Scan()
{
  byte mask = 1;
  byte value = 0;

  for (byte i = 0; i < _nPins; i++)
  {
    if (_aPinPort[i] >= 0)
    {
      if (!digitalRead(_aPinPort[i]))
        value |= mask;
    }
    mask <<= 1;
  }

  if (_valueScan != value)
  {
    _nDebounceCounter = 0;
    _valueScan = value;
  }
  else
  {
    if (_nDebounceCounter < 0)
      ; // do nothing
    else if (_nDebounceCounter > 250)
    {
      if (_value != _valueScan)
      {
        _value = _valueScan;
        _bChanged = true;
      }
      _nDebounceCounter = -1;
      //Serial.print(_value);
    }
    else
      _nDebounceCounter++;
  }
}

///////////////////////////////////////////////////////////////////////////////

