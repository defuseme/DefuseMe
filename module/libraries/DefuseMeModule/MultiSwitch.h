/*
MultiSwitch.h - Multi position/layer switch input library for project DefuseMe
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
#ifndef __MULTISWITCH__
#define __MULTISWITCH__

#include <Arduino.h>

class MultiSwitch
{
  public:
    MultiSwitch(int Pin0, int Pin1, int Pin2 = -1, int Pin3 = -1, int Pin4 = -1, int Pin5 = -1, int Pin6 = -1, int Pin7 = -1);
    ~MultiSwitch() {};

    inline bool IsChanged() {
      return _bChanged;
    };

    inline byte get() {
      _bChanged = false;
      return _value;
    };

    void DoProcess();

    operator byte() {
      return get();
    };

  protected:
    byte _nPins;
    int _aPinPort[8];
    byte _value;
    byte _valueScan;
    int _nDebounceCounter;
    bool _bChanged;
    unsigned long _millisLast;

    void Scan();
};

#endif // __MULTISWITCH__
