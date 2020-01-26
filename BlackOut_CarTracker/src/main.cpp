#define _MAIN_IMU_
#ifdef _MAIN_IMU_

#include <FreematicsPlus.h>
#include <SerialCommunication.h>
#include <MovementComputation.h>

SerialCommunication communicationPort;
MovementComputation *moveComputation_ptr;

Movement move;
unsigned long timeLast;
bool communicationState = false;
const float loopFrequency = 5.0;

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);
  timeLast = millis();
  moveComputation_ptr = new MovementComputation(loopFrequency, &Serial);

  delay(50);
}

void loop()
{

  if (millis() - timeLast > ((int)1000 / loopFrequency))
  {
    timeLast = millis();

    // Data reception
    switch (communicationPort.updateReception())
    {
    case Ping:
      communicationPort.sendDebugMessage("Pong");
      break;

    case Start:
      communicationState = true;
      communicationPort.sendDebugMessage("Com started");
      break;

    case Stop:
      communicationState = false;
      communicationPort.sendDebugMessage("Com stopped");
      break;

    case Calibration:
      communicationPort.sendDebugMessage("Calibration begin");
      moveComputation_ptr->calibration();
      break;

    default:
      break;
    }

    moveComputation_ptr->update();
    move = moveComputation_ptr->getMovement();

    communicationPort.sendMovement(move);
  }
}
#endif