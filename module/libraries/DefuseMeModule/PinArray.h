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
    PinArray(int Pin0, int Pin1, int Pin2 = -1, int Pin3 = -1, int Pin4 = -1, int Pin5 = -1, int Pin6 = -1, int Pin7 = -1, int Pin8 = -1, int Pin9 = -1, int Pin10 = -1, int Pin11 = -1, int Pin12 = -1, int Pin13 = -1, int Pin14 = -1, int Pin15 = -1 );
    ~PinArray() {};

    //* call in loop as often as possible */
    void DoProcess();

    //* check if pin with index a is connected to pin with index b */
    bool IsConnected(int a, int b);
    //* check if pin with index a is connected to any other pin */
    bool IsAnyConnected(int a);
    //* check if any pin is connected to any other pin */
    bool IsAnyConnected();

    //* check if pin with index a is connected to GND */
    bool IsGrounded(int a);
    //* check if pin  is connected to GND */
    bool IsAnyGrounded();

    //* get number of connections. If 3 pins are connected together the functin returns 2. For 4 pins 3 is returned */
    byte GetConnections();
    //* get number of connections. If 3 pins are connected together the functin returns 3. For 4 pins 6 is returned */
    byte GetConnectionsEx();
    //* get number of connections to GND */
    byte GetGroundConnections();

    //* check if any connection has changed since last call */
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

