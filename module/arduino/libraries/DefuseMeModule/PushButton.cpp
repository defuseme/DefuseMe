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

