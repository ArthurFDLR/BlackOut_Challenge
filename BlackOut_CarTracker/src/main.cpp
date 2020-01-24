#define _MAIN_IMU_
#ifdef _MAIN_IMU_

#include <FreematicsPlus.h>
#include <SerialCommunication.h>
#include <MovementComputation.h>

ORIENTATION ori;
SerialCommunication communicationPort;
MovementComputation *moveComputation_ptr;

unsigned long timeLast;
bool communicationState = false;
const float loopFrequency = 5.0;

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);
  timeLast = millis();
  moveComputation_ptr = new MovementComputation(loopFrequency, &Serial, false);

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

    moveComputation_ptr->updateDataIMU();
    moveComputation_ptr->computeRotationMovement();

    /*
    communicationPort.print("| Delta_theta : ");
    communicationPort.print(moveComputation_ptr->_deltaTheta);
    communicationPort.print("\n");
    */

    /*
    moveComputation_ptr->updateDataOBD();
    moveComputation_ptr->computeLinearMovement();
    communicationPort.print("    |    Delta_X : ");
    communicationPort.print(moveComputation_ptr->_deltaX);
    */

    // Simulate movement
    if (communicationState)
    {
      String listName[] = {"dTh", "dX"};
      float listValue[] = {moveComputation_ptr->_deltaTheta, 5};
      communicationPort.sendData(2, listName, listValue);
    }
  }
}
#endif