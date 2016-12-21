
#include <Arduino.h>

class MultiSwitch
{
  public:
    MultiSwitch(int Pin0, int Pin1, int Pin2 = -1, int Pin3 = -1, int Pin4 = -1, int Pin5 = -1, int Pin6 = -1, int Pin7 = -1);
    ~MultiSwitch() {};

    inline bool IsChanged() {
      return _bChanged;
    };
    inline byte get() {
      _bChanged = false;
      return _value;
    };

    void DoProcess();

    operator byte() {
      return get();
    };

  protected:
    byte _nPins;
    int _aPinPort[8];
    byte _value;
    byte _valueScan;
    byte _nDebounceCounter;
    bool _bChanged;
    unsigned long _millisLast;

    void Scan();
};

