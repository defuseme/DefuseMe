#include "DipSwitch.h"

DipSwitch::DipSwitch(int Pin0, int Pin1, int Pin2, int Pin3, int Pin4, int Pin5, int Pin6, int Pin7)
{
  int _aPinPort[8];

  _aPinPort[0] = Pin0;
  _aPinPort[1] = Pin1;
  _aPinPort[2] = Pin2;
  _aPinPort[3] = Pin3;
  _aPinPort[4] = Pin4;
  _aPinPort[5] = Pin5;
  _aPinPort[6] = Pin6;
  _aPinPort[7] = Pin7;

  for (byte i = 0; i < 8; i++)
  {
    if (_aPinPort[i] >= 0)
    {
      pinMode(_aPinPort[i], INPUT_PULLUP);
    }
  }

  byte mask = 1;
  byte value = 0;

  for (byte i = 0; i < 8; i++)   // Note: splitted in two loops to delay read port from set pullups
  {
    if (_aPinPort[i] >= 0)
    {
      if (! digitalRead(_aPinPort[i]))
        value |= mask;
    }
    mask <<= 1;
  }

  _nValue = value;
}

