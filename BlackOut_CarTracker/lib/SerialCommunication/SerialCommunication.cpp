/*************************************************************************
* Arduino library establishing communication protocol on Serial ComPort
* BlackOut_Challenge\Python_FrontEnd\serialParser.py can be used as interpreter
* Part of a prototype to the Black-Out Challenge organized by SAFRAN
* Developed by Arthur Findelair, January 2020
*************************************************************************/

#include "SerialCommunication.h"

SerialCommunication::SerialCommunication(bool debugPortEnable)
{
    _debugPortEnable = debugPortEnable;
}

int SerialCommunication::Setup(HardwareSerial *telemetryPort, HardwareSerial *debugPort)
{
    _telemetryPort = telemetryPort;

    if (_debugPortEnable)
        _debugPort = debugPort;

    return 0;
}

void SerialCommunication::serialFloatPrint(float f)
{

    byte *b = (byte *)&f;
    //Serial.print("f:");
    for (int i = 0; i < 4; i++)
    {
        byte b1 = (b[i] >> 4) & 0x0f;
        byte b2 = (b[i] & 0x0f);

        char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
        char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;

        _telemetryPort->print(c1);
        _telemetryPort->print(c2);
    }
}

int SerialCommunication::sendNamedValue(String name, float value)
{
    _telemetryPort->print('@');
    _telemetryPort->print(name);
    _telemetryPort->print("#");
    serialFloatPrint(value);
    return 0;
}

int SerialCommunication::sendData(int nbrValue, String* names, float* values)
{
    _telemetryPort->print('*');
    _telemetryPort->print(nbrValue);
    for(int i = 0; i<nbrValue ; i++)
    {
        sendNamedValue(names[i], values[i]);
    }
    _telemetryPort->print('\n');
    return 0;
}

int SerialCommunication::sendDebugMessage(const String &message)
{
    if (_debugPortEnable)
    {
        _debugPort->print('|');
        _debugPort->println(message);
        return 0;
    }
    return 1;
}

signalReception SerialCommunication::updateReception()
{
    if (_telemetryPort->available() > 0)
    {
        switch (char(_telemetryPort->read()))
        {
        case CHAR_PING_SIGNAL:
            return Ping;
            break;
        
        case CHAR_STOP_SIGNAL:
            return Stop;
            break;
        
        case CHAR_START_SIGNAL:
            return Start;
            break;

        default:
            break;
        }
    }
    return Void;
}
/* Function to call for Magnetometer calibration with MotionCal

int calibration_MotionCal()
{
  if (imu.fifoAvailable())
  {
    if (imu.update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS))
    {
    
    Serial.print("Raw:");
    Serial.print(imu.ax);
    Serial.print(',');
    Serial.print(imu.ay);
    Serial.print(',');
    Serial.print(imu.az);
    Serial.print(',');
    Serial.print(imu.gx);
    Serial.print(',');
    Serial.print(imu.gy);
    Serial.print(',');
    Serial.print(imu.gz);
    Serial.print(',');
    Serial.print(imu.mx);
    Serial.print(',');
    Serial.print(imu.my);
    Serial.print(',');
    Serial.print(imu.mz);
    Serial.println();
    return 0;
      
    }
  }
  return 1;
}

*/