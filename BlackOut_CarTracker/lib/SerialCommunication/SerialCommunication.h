/*************************************************************************
* Arduino library establishing communication protocol on Serial ComPort
* BlackOut_Challenge\Python_FrontEnd\serialParser.py can be used as interpreter
* Part of a prototype to the Black-Out Challenge organized by SAFRAN
* Developed by Arthur Findelair, January 2020
*************************************************************************/

#ifndef SERIALCOMMUNICATION_H_
#define SERIALCOMMUNICATION_H_

#include <Arduino.h>
#include <Print.h>

#define CHAR_PING_SIGNAL '!'
#define CHAR_START_SIGNAL 'y'
#define CHAR_STOP_SIGNAL 'n'

typedef enum
{
    Void,
    Ping,
    Start,
    Stop
} signalReception;

class SerialCommunication
{
public:
    SerialCommunication(bool debugPortEnable = true);

    // GOAL / Setup references to communication ports
    // IN   / HardwareSerial *telemetryPort : Pointer to main telemetry port,
    //                                        scruted by updateReception() and used by sendData()
    //        HardwareSerial *debugPort : Pointer to debug port, only activate if debugPortEnable
    //                                    used by sendDebugMessage()
    int Setup(HardwareSerial *telemetryPort = &Serial, HardwareSerial *debugPort = &Serial);

    // GOAL / Send a value on data format (example : '@name#value') on telemetryPort
    int sendData(String name, float value);

    // GOAL / Send a message on data format (example : '|message\n') on debugPort if debugPortEnable
    //        Do not use | and \n
    int sendDebugMessage(const String &message);

    // GOAL / Check if CHAR_PING_SIGNAL, CHAR_START_SIGNAL, CHAR_STOP_SIGNAL are received on telemetryPort
    // OUT  / signalReception : element associated to the received signal, if nothing relevant received : Void
    signalReception updateReception();

private:
    HardwareSerial *_telemetryPort;
    HardwareSerial *_debugPort;
    bool _debugPortEnable;

    void serialFloatPrint(float f);
};

#endif