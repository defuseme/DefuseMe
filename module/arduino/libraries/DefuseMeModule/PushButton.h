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
