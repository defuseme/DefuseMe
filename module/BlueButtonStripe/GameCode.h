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

      for (int buttonReleaseAt = 0; buttonReleaseAt < 10; buttonReleaseAt++)
      {
        for (int buttonPressAt = 0; buttonPressAt < 10; buttonPressAt++)
        {
          byte buttoncode = GetButtonCode(buttonPressAt, buttonReleaseAt);

          if (buttoncode == snocode)
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
          byte code = GetButtonCode(buttonPressAt, buttonReleaseAt);

          //print code
          Serial.print(code);
          Serial.print(";");
        }
        Serial.println();
      }

      Serial.println("===========================");
    }

  protected:

    byte GetSNOIndex(byte digit1, byte digit2, byte digit3)
    {
      switch (digit1)
      {
        case 0:
          break;

        case 1:
          break;

        case 2:
          break;

        case 3:
          break;

        case 4:
          break;

        case 5:
          break;

        case 6:
          break;

        case 7:
          break;

        case 8:
          break;

        case 9:
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
      randomSeed((buttonPressAt << 4) + buttonReleaseAt + 2342);
      return random(100);
    }


};
