/*
LED.h - LED output library for project DefuseMe
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
#include <Arduino.h>

class LED
{
  public:
    LED(int Pin, byte bactiveHigh = 0) {
      _PinPort = Pin;
      _bactiveHigh = bactiveHigh;

      pinMode(_PinPort, OUTPUT);
      set(0);   // default off
    }
    ~LED() {};

    inline void set(byte value) {
      digitalWrite(_PinPort, (_bactiveHigh) ? value : !value);
    };

    inline byte get() {
      byte value = digitalRead(_PinPort);
      return ((_bactiveHigh) ? value : !value);
    };

    /// use PWM to dim light. 0=off  128=50% 255=100%. On AVR Mega 328 only D9, D6, D5 and D3 can be used for dimming
    inline void dim(byte value) {
      analogWrite(_PinPort, (_bactiveHigh) ? value : ~value);
    };

    inline void on() {
      set(1);
    }
    inline void off() {
      set(0);
    }
	inline void toggle() {
		set(!get());
	}

    void operator = (byte value) {
      set (value);
    }

    operator byte() {
      return get();
    }

  protected:
    int _PinPort;
    byte _bactiveHigh;
};

