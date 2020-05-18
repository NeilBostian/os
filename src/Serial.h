#pragma once

#include <Types.h>

typedef uint16 SerialPort;

class Serial
{
public:
    static void OutB(SerialPort port, uint8 val);
    static void OutW(SerialPort port, uint16 val);
    //static void OutDW(SerialPort port, uint32 val);

    static uint8 InB(SerialPort port);
    static uint16 InW(SerialPort port);
    //static uint32 InDW(SerialPort port);
};