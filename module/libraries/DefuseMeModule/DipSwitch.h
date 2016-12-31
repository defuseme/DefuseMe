#ifndef __DIPSWITCH__
#define __DIPSWITCH__

#include <Arduino.h>

class DipSwitch
{
  public:
    DipSwitch(int Pin0 /*LSB*/, int Pin1, int Pin2 = -1, int Pin3 = -1, int Pin4 = -1, int Pin5 = -1, int Pin6 = -1, int Pin7 = -1 /*MSB*/);
    ~DipSwitch() {};

    byte GetValue() {
      return _nValue;
    };

    void operator = (const byte &value) {
      _nValue = value;
    }

    void operator = (byte value) {
      _nValue = value;
    }

    operator int() {
      return _nValue;
    }


  protected:
    byte _nValue;
};

#endif // __DIPSWITCH__
