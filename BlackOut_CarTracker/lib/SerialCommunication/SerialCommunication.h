/*************************************************************************
* Arduino library establishing communication protocol on Serial ComPort
* BlackOut_Challenge\Python_FrontEnd\serialParser.py can be used as interpreter
* Part of a prototype to the Black-Out Challenge organized by SAFRAN
* Developed by Arthur Findelair, January 2020
*************************************************************************/

#ifndef SERIALCOMMUNICATION_H_
#define SERIALCOMMUNICATION_H_

#include <Arduino.h>
#include <FreematicsPlus.h>
#include <MovementComputation.h>

#define CHAR_PING_SIGNAL '!'
#define CHAR_START_SIGNAL 'y'
#define CHAR_STOP_SIGNAL 'n'
#define CHAR_CALIBRATION_SIGNAL 'c'

typedef enum
{
    Void,
    Ping,
    Start,
    Stop,
    Calibration
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

    // GOAL / Send multiple value on data format (example : '*nbrValue@name1#value1@name2#value2...') on telemetryPort
    // IN   / int nbrValue : Number of value to send, 1 to 9
    //        String* Names : list of names
    //        String* values : list of values
    int sendData(int nbrValue, String* names, float* values);

    // GOAL / Send a message on data format (example : '|message\n') on debugPort if debugPortEnable
    //        Do not use | and \n
    int sendDebugMessage(const String &message);

    // GOAL / Check if CHAR_PING_SIGNAL, CHAR_START_SIGNAL, CHAR_STOP_SIGNAL are received on telemetryPort
    // OUT  / signalReception : element associated to the received signal, if nothing relevant received : Void
    signalReception updateReception();

    // GOAL / Print to debugPort. Use with care !
    void print(const String &message);
    void print(float nbr);

    void printDebugVector(Vector* vec);

private:
    HardwareSerial *_telemetryPort;
    HardwareSerial *_debugPort;
    bool _debugPortEnable;

    // GOAL / Send a value on data format (example : '@name#value') on telemetryPort
    int sendNamedValue(String name, float value);

    void serialFloatPrint(float f);
};

#endif