/*************************************************************************
* Arduino library establishing communication protocol on Serial ComPort
* BlackOut_Challenge\Python_FrontEnd\serialParser.py can be used as interpreter
* Part of a prototype to the Black-Out Challenge organized by SAFRAN
* Developed by Arthur Findelair, January 2020
*************************************************************************/

#include <SerialCommunication.h>

SerialCommunication communicationPort;

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);
}

void loop()
{
  for (int i = 0; i < 10; i++)
  {
    if (communicationPort.updateReception() == Ping)
      communicationPort.sendDebugMessage(String(10*i));

    float listVal[2];
    listVal[0] = (float) i * PI;
    listVal[1] = (float) i * PI * 2;
    String listName[2] = {"dX", "dY"};

    communicationPort.sendData(2,listName,listVal);
    delay(500);
  }