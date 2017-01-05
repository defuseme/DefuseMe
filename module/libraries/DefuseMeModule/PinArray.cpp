/*
PinArray.cpp - Multi pin input library for project DefuseMe
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
#include "PinArray.h"

////////////////////////////////////////////////////////////////////////////

PinArray::PinArray(int Pin0, int Pin1, int Pin2, int Pin3, int Pin4, int Pin5, int Pin6, int Pin7, int Pin8, int Pin9, int Pin10, int Pin11, int Pin12, int Pin13, int Pin14, int Pin15)
{
  _aPinPort[0] = Pin0;
  _aPinPort[1] = Pin1;
  _aPinPort[2] = Pin2;
  _aPinPort[3] = Pin3;
  _aPinPort[4] = Pin4;
  _aPinPort[5] = Pin5;
  _aPinPort[6] = Pin6;
  _aPinPort[7] = Pin7;
  _aPinPort[8] = Pin8;
  _aPinPort[9] = Pin9;
  _aPinPort[10] = Pin10;
  _aPinPort[11] = Pin11;
  _aPinPort[12] = Pin12;
  _aPinPort[13] = Pin13;
  _aPinPort[14] = Pin14;
  _aPinPort[15] = Pin15;

  for (byte i = 0; i < 17; i++)
  {
    _aMap[i] = _aMapScan[i] = 0;
  }
  _nPins = 16;
  _nDebounceCounter = -1;
  _bChanged = false;

  for (byte i = 0; i < _nPins; i++)
  {
    if (_aPinPort[i] >= 0)
    {
      pinMode(_aPinPort[i], INPUT_PULLUP);
    }
  }

  _millisSlot = 0;
}

////////////////////////////////////////////////////////////////////////////

bool PinArray::IsConnected(int a, int b)
{
  if (a >= 16 || b >= 16)
    return false;

  int map = _aMap[a];
  int mask = (1 << b);

  return (map & mask);
}

////////////////////////////////////////////////////////////////////////////

bool PinArray::IsAnyConnected(int a)
{
  if (a >= 16)
    return false;

  return (_aMap[a] != 0);
}

////////////////////////////////////////////////////////////////////////////

bool PinArray::IsAnyConnected()
{
  for (byte i = 0; i < _nPins; i++)
  {
    if ( _aMap[i] )
      return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////

bool PinArray::IsGrounded(int a)
{
  if (a >= 16)
    return false;

  int map = _aMap[16];
  int mask = (1 << a);

  return (map & mask);
}

////////////////////////////////////////////////////////////////////////////

bool PinArray::IsAnyGrounded()
{
  int map = _aMap[16];

  return (map != 0);
}

////////////////////////////////////////////////////////////////////////////

byte PinArray::GetConnections()
{
  byte nConnections = 0;

  int mask = 1;
  int printed = 0;
  int map = 0;

  for (byte i = 0; i < 16; i++)
  {
    int map = _aMap[i];
    int mask = 1;

    if (map && !(printed & (1 << i)))
    {
      for (byte j = 0; j < 16; j++)
      {
        if ((map & mask) && !(printed & mask))
        {
          nConnections++;
          printed |= mask;
        }
        mask <<= 1;
      }
    }
  }

  return nConnections;
}

////////////////////////////////////////////////////////////////////////////

byte PinArray::GetConnectionsEx()
{
  byte nConnections = 0;

  for (byte i = 0; i < _nPins; i++)
  {
    int map = _aMap[i];
    int mask = 1;
    for (byte j = 0; j < 16; j++)
    {
      if (map & mask)
      {
        nConnections++;
        if (_aPinPort[i] == 99)
          nConnections++;
      }
      mask <<= 1;
    }
  }

  nConnections >>= 1;

  return nConnections;
}

////////////////////////////////////////////////////////////////////////////

byte PinArray::GetGroundConnections()
{
  byte nConnections = 0;

  int map = _aMap[16];
  int mask = 1;
  for (byte j = 0; j < 16; j++)
  {
    if (map & mask)
      nConnections++;
    mask <<= 1;
  }

  return nConnections;
}

////////////////////////////////////////////////////////////////////////////

int PinArray::ScanInput()
{
  int mask = 1;
  int map = 0;

  for (byte i = 0; i < _nPins; i++)
  {
    if (_aPinPort[i] >= 0 && _aPinPort[i] < 99)
    {
      if (!digitalRead(_aPinPort[i]))
        map |= mask;
    }
    mask <<= 1;
  }

  return map;
}

////////////////////////////////////////////////////////////////////////////

void PinArray::Scan()
{
  int mapGND;
  int mask = 1;
  int map = 0;
  bool bScanChanged = false;

  mapGND = ScanInput();

  if (_aMapScan[16] != mapGND)
  {
    bScanChanged = true;
    _aMapScan[16] = mapGND;
  }

  for (byte i = 0; i < _nPins; i++)
  {
    if (_aPinPort[i] == 99)
    {
      _aMapScan[i] = mapGND;
    }
    else if (_aPinPort[i] >= 0)
    {
      pinMode(_aPinPort[i], OUTPUT);
      digitalWrite(_aPinPort[i], 0);

      map = ScanInput();

      pinMode(_aPinPort[i], INPUT_PULLUP);

      map &= ~mask;
      map &= ~mapGND;

      if (_aMapScan[i] != map)
      {
        bScanChanged = true;
        _aMapScan[i] = map;
      }
    }
    mask <<= 1;
  }

  if (bScanChanged)
  {
    _nDebounceCounter = 0;
  }
  else
  {
    if (_nDebounceCounter < 50 && _nDebounceCounter >= 0)
      _nDebounceCounter++;
    else
    {
      for (byte i = 0; i < 17; i++)   // check 16 pind plus GND
      {
        if (_aMap[i] != _aMapScan[i])
          _bChanged = true;
        _aMap[i] = _aMapScan[i];
      }
      _nDebounceCounter = -1;
    }
  }
}

////////////////////////////////////////////////////////////////////////////

void PinArray::DoProcess()
{
  unsigned long millisAct = millis();
  if (millisAct < _millisSlot)
    return;
  _millisSlot = millisAct + 1;

  Scan();
}

////////////////////////////////////////////////////////////////////////////

bool PinArray::IsJustChanged()
{ byte bChanged = _bChanged;
  _bChanged = false;
  return bChanged;
}

////////////////////////////////////////////////////////////////////////////

void PinArray::Print()
{
  int mask = 1;
  int printed = 0;
  int map = 0;

  for (byte i = 0; i < 17; i++)
  {
    int map = _aMap[i];
    int mask = 1;

    if (map && !(printed & (1 << i)))
    {
      //        Serial.print("# ");
      if (i == 16)
        Serial.print("GND");
      else
        Serial.print(i);
      printed |= (1 << i);

      for (byte j = 0; j < 16; j++)
      {
        if ((map & mask) && !(printed & mask))
        {
          Serial.print("-");
          Serial.print(j);
          printed |= mask;
        }
        mask <<= 1;
      }
      Serial.print(", ");
    }
  }

  Serial.println();
}

////////////////////////////////////////////////////////////////////////////

byte PinArray::GetConnections(PinArrayConnection c[], byte maxCount)
{
  byte nCount = 0;
  int mask = 1;
  int printed = 0;
  int map = 0;

  for (byte i = 0; i < 17; i++)
  {
    int map = _aMap[i];
    int mask = 1;

    if (map && !(printed & (1 << i)))
    {
      //        Serial.print("# ");
      printed |= (1 << i);

      for (byte j = 0; j < 16; j++)
      {
        if ((map & mask) && !(printed & mask))
        {
          if (i == 16)
            c[nCount].a = 99;
          else
            c[nCount].a = i;

          c[nCount].b = j;
          nCount++;
          if (nCount >= maxCount)
            return nCount;
          printed |= mask;
        }
        mask <<= 1;
      }
    }
  }

  return nCount;
}

////////////////////////////////////////////////////////////////////////////

