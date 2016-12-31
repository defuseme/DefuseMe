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
