#pragma once
#include <Arduino.h>

class GameCode
{
public:
  GameCode() {};
  ~GameCode() {};
  byte _buttonPressAt;
  byte _buttonReleaseAt;

  bool IsButtonPressAt(byte digit)
  {
    return (digit == _buttonPressAt);
  }

  bool IsButtonReleaseAt(byte digit)
  {
    return (digit == _buttonReleaseAt);
  }

  void CalcSNOCode(byte digit1, byte digit2, byte digit3, byte blueLED)
  {
    byte index = GetSNOIndex(digit1, digit2, digit3);

    byte snocode = GetSNOCode(index, blueLED);

    for (byte buttonReleaseAt = 0; buttonReleaseAt < 10; buttonReleaseAt++)
    {
      for (byte buttonPressAt = 0; buttonPressAt < 10; buttonPressAt++)
      {
        byte code = buttoncode[buttonReleaseAt][buttonPressAt];

        if (code == snocode)
        {
          _buttonPressAt = buttonPressAt;
          _buttonReleaseAt = buttonReleaseAt;
          break;
        }
      }
    }

  }

  void PrintList()
  {
    // print headline with keys
    Serial.println("===========================");

    for (int index = 0; index < 32; index++)
    {
      for (int blueLED = 0; blueLED <= 22; blueLED++)
      {
        byte code = GetSNOCode(index, blueLED);

        //print code
        Serial.print(code);
        Serial.print(";");
      }
      Serial.println();
    }

    Serial.println("===========================");

    for (int buttonReleaseAt = 0; buttonReleaseAt < 10; buttonReleaseAt++)
    {
      for (int buttonPressAt = 0; buttonPressAt < 10; buttonPressAt++)
      {
        byte code = buttoncode[buttonReleaseAt][buttonPressAt];

        //print code
        Serial.print(code);
        Serial.print(";");
      }
      Serial.println();
    }

    Serial.println("===========================");
  }

protected:

  static byte buttoncode[10][10];

  byte GetSNOIndex(byte digit1, byte digit2, byte digit3)
  {
    switch (digit1)
    {
    case 0:
      if (digit2 == 0)
        return 0;
      else if (digit2 == 1)
        return 1;
      else if (digit2 == 2)
        return 2;
      else if (digit2 == 3)
        return 3;
      else if ((digit2 >= 4) && (digit2 <= 7))
      {
        if (digit3 == 0)
          return 4;
        else
          return 5;
      }
      else
        return 6;
      break;

    case 1:
      if (digit2 & 1)   // odd
      {
        if (digit3 & 1)   // odd
          return 10;
        else
          return 9;
      }
      else   // even
      {
        if (digit3 & 1)   // odd
          return 8;
        else
          return 7;
      }
      break;

    case 2:
      if (digit2 == 9)
        return 11;
      else if ((digit2 >= 0) && (digit2 <= 4))
        return 12;
      else
        return 13;
      break;

    case 3:
      if ((digit2 >= 0) && (digit2 <= 3))
        return 14;
      else
        return 15;
      break;

    case 4:
      if (digit2 == 2)
        return 16;
      else if (digit2 == 3)
        return 17;
      else
        return 18;
      break;

    case 5:
      if (digit2 > 3)
      {
        if (digit3 == 0)
          return 19;
        else
          return 20;
      }
      else
        return 21;
      break;

    case 6:
      if ((digit2 == 6) && (digit3 == 6))
        return 22;
      else
        return 23;
      break;

    case 7:
      if ((digit2 == 1) || (digit2 == 3) || (digit2 == 8))
        return 24;
      else
        return 25;
      break;

    case 8:
      if ((digit2 >= 3) && (digit2 <= 6))
        return 26;
      else
        return 27;
      break;

    case 9:
      if (digit2 == 0)
      {
        if ((digit3 == 1) || (digit3 == 3) || (digit3 == 5) || (digit3 == 8) || (digit3 == 9))
          return 28;
        else
          return 29;
      }
      else
        return 30;
      break;

    default:   // no valid serial number or no module with a serial number
      return 31;

    }
    return 255;
  }

  byte GetSNOCode(byte index, byte blueLED)
  {
    randomSeed((index << 5) + blueLED + 4223);
    return random(100);
  }

  byte GetButtonCode(byte buttonPressAt, byte buttonReleaseAt)
  {
    return buttoncode[buttonReleaseAt][buttonPressAt];
  }


};

byte GameCode::buttoncode[10][10] = {
  19	,	22	,	63	,	97	,	30	,	36	,	39	,	2	,	23	,	12	,
  40	,	71	,	50	,	21	,	86	,	98	,	49	,	82	,	93	,	28	,
  95	,	44	,	87	,	7	,	58	,	53	,	3	,	4	,	67	,	15	,
  14	,	31	,	46	,	42	,	33	,	96	,	65	,	80	,	73	,	74	,
  18	,	85	,	35	,	92	,	29	,	84	,	68	,	20	,	37	,	41	,
  88	,	61	,	13	,	16	,	75	,	0	,	94	,	62	,	9	,	60	,
  54	,	43	,	99	,	27	,	70	,	55	,	47	,	17	,	91	,	26	,
  5	,	45	,	81	,	69	,	48	,	76	,	90	,	78	,	59	,	77	,
  6	,	51	,	66	,	8	,	24	,	10	,	1	,	83	,	57	,	25	,
  64	,	32	,	72	,	56	,	52	,	38	,	89	,	11	,	34	,	79	,
};

