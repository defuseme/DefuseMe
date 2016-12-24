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

