/*
DebounceInput.h - Debounce button and switch input library for project DefuseMe
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
#ifndef __DEBOUNCEINPUT__
#define __DEBOUNCEINPUT__
#include <Arduino.h>

/*
Example of derived class:

class DebounceButton : public DebounceInput
{
public:
  DebounceButton (int Pin, byte bStartValue = HIGH) : DebounceInput(Pin, bStartValue) { pinMode(9, OUTPUT); };
  virtual void ChangeTo0() { digitalWrite(9, 0); };
  virtual void ChangeTo1() { digitalWrite(9, 1); };
};

DebounceButton button(2);
*/

class DebounceInput
{
  public:
    DebounceInput(int Pin, byte bStartValue = HIGH);
    ~DebounceInput();

    void Init(byte bStartValue);

    /// DoProcess - called from main loop as often as possible. This Function is/should not blocking
    void DoProcess();

    /// GetValue - get the stable (debounced) state of the input
    byte GetValue() {
      return _bValue;
    };

  protected:
    int _Pin;
    volatile byte _bValue;

    virtual void ChangeTo0() {};
    virtual void ChangeTo1() {};

  private:
    volatile byte _bState;
    static byte gbStateTable[32];
    unsigned long _millisSlot;
};

#endif   // __DEBOUNCEINPUT__
