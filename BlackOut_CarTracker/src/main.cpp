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
  moveComputation_ptr = new MovementComputation(loopFrequency,&Serial, false);

  delay(50);
}

void loop()
{

  if (millis() - timeLast > ((int) 1000/loopFrequency))
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

    // moveComputation_ptr->updateDataOBD();
    // moveComputation_ptr->computeLinearMovement();

    // Simulate movement
    if (communicationState)
    {
      communicationPort.printDebugVector(&(moveComputation_ptr->accVec));
    }

    // Send debug
    communicationPort.print("Delta_theta : ");
    communicationPort.print(moveComputation_ptr->_deltaTheta);
    
    // communicationPort.print("    |    Delta_X : ");
    // communicationPort.print(moveComputation_ptr->_deltaX);
    
    communicationPort.print("\n");

    /*
    // Send rotation in new frame
    communicationPort.printOrientation(&moveComputation_ptr->oriRaw);
    communicationPort.printOrientation(&moveComputation_ptr->ori);
    */
  }
}

/*

#include <FreematicsPlus.h>

MPU9250_DMP imu;

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("MPU-9250 DMP Quaternion Test");
  if (!imu.begin(true, 10))
  {
    Serial.println("Unable to communicate with MPU-9250");
    Serial.println("Check connections, and try again.");
    for (;;);
  }
  Serial.println("MPU-9250 OK");
}

void loop()
{
  float acc[3];
  ORIENTATION ori;
  if (imu.read(acc, 0, 0, 0, &ori)) {
    Serial.print("Accelerometer: X=");
    Serial.print(acc[0]);
    Serial.print("g Y=");
    Serial.print(acc[1]);
    Serial.print("g Z=");
    Serial.print(acc[2]);
    Serial.println("g");
    Serial.print("Orientation: ");
    Serial.print(imu.yaw);
    Serial.print(' ');
    Serial.print(imu.pitch);
    Serial.print(' ');
    Serial.println(imu.roll);
  }
  delay(50);
  return;
}
*/
#endif