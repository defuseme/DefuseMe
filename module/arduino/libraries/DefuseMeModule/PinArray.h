#ifndef __PINARRAY__
#define __PINARRAY__
#include <Arduino.h>

struct PinArrayConnection
{
	byte a;
	byte b;
};

class PinArray
{
  public:
    PinArray(int Pin0, int Pin1, int Pin2, int Pin3 = -1, int Pin4 = -1, int Pin5 = -1, int Pin6 = -1, int Pin7 = -1, int Pin8 = -1, int Pin9 = -1, int Pin10 = -1, int Pin11 = -1, int Pin12 = -1, int Pin13 = -1, int Pin14 = -1, int Pin15 = -1 );
    ~PinArray() {};

    bool IsConnected(int a, int b);
    bool IsAnyConnected(int a);

    bool IsGrounded(int a);
    bool IsAnyGrounded();

    byte GetConnections();

    void DoProcess();
    
    bool IsJustChanged();
    
    void Print();
	byte GetConnections(PinArrayConnection c[], byte maxCount);
    
  protected:
    byte _nPins;
    int _aPinPort[16];
    int _aMap[17];
    int _aMapScan[17];
    byte _nDebounceCounter;
    bool _bChanged;
    unsigned long _millisSlot;

    int ScanInput();
    void Scan();
};

#endif // __PINARRAY__

