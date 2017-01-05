/*
DebounceInput.cpp - Debounce button and switch input library for project DefuseMe
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
#include "DebounceInput.h"

// defines for state machine - do not change unless you know what you do!
#define DEBOUNCEINPUT_IN			0x10

#define DEBOUNCEINPUT_STATE_0		0x00
#define DEBOUNCEINPUT_STATE_1		0x08

#define DEBOUNCEINPUT_TRIGGER_0		0x40
#define DEBOUNCEINPUT_TRIGGER_1		0x80

#define DEBOUNCEINPUT_STATE_MASK	0x0F

byte DebounceInput::gbStateTable[] = {
  //IN=0
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,
  DEBOUNCEINPUT_STATE_0 + 0,

  DEBOUNCEINPUT_STATE_0 + 0 + DEBOUNCEINPUT_TRIGGER_0,
  DEBOUNCEINPUT_STATE_1 + 0,
  DEBOUNCEINPUT_STATE_1 + 1,
  DEBOUNCEINPUT_STATE_1 + 2,
  DEBOUNCEINPUT_STATE_1 + 3,
  DEBOUNCEINPUT_STATE_1 + 4,
  DEBOUNCEINPUT_STATE_1 + 5,
  DEBOUNCEINPUT_STATE_1 + 6,

  //IN=1
  DEBOUNCEINPUT_STATE_0 + 1,
  DEBOUNCEINPUT_STATE_0 + 2,
  DEBOUNCEINPUT_STATE_0 + 3,
  DEBOUNCEINPUT_STATE_0 + 4,
  DEBOUNCEINPUT_STATE_0 + 5,
  DEBOUNCEINPUT_STATE_0 + 6,
  DEBOUNCEINPUT_STATE_0 + 7,
  DEBOUNCEINPUT_STATE_1 + 7 + DEBOUNCEINPUT_TRIGGER_1,

  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
  DEBOUNCEINPUT_STATE_1 + 7,
};

DebounceInput::DebounceInput(int Pin, byte bStartValue)
{
  _Pin = Pin;

  Init(bStartValue);

  pinMode(_Pin, INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(_Pin), isrProcessEncoder, CHANGE);
}

DebounceInput::~DebounceInput()
{
}

void DebounceInput::Init(byte bStartValue)
{
  _bValue = bStartValue;
  if (bStartValue)
    _bState = DEBOUNCEINPUT_STATE_1 + 3;
  else
    _bState = DEBOUNCEINPUT_STATE_0 + 0;

  _millisSlot = 0;
}

void DebounceInput::DoProcess()
{
  unsigned long millisAct = millis();
  if (millisAct < _millisSlot)
    return;
  _millisSlot = millisAct + 5;

  _bState &= DEBOUNCEINPUT_STATE_MASK;

  if (digitalRead(_Pin))
    _bState |= DEBOUNCEINPUT_IN;

  _bState = gbStateTable[_bState];

  if (_bState & DEBOUNCEINPUT_TRIGGER_0)
  {
    _bValue = 0;
    ChangeTo0();
  }
  if (_bState & DEBOUNCEINPUT_TRIGGER_1)
  {
    _bValue = 1;
    ChangeTo1();
  }

}


