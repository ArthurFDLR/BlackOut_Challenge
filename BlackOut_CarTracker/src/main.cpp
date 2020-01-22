#define _MAIN_IMU_
#ifdef _MAIN_IMU_

#include <FreematicsPlus.h>
#include <SerialCommunication.h>
#include <MovementComputation.h>

ORIENTATION ori;
SerialCommunication communicationPort;
MovementComputation *moveComputation_ptr;

float acc[3], gyr[3], mag[3];
int i = 0;
unsigned long timeLast1, timeLast2;
bool communicationState = false;
const float loopFrequency = 10.0;

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);
  timeLast1 = millis();
  timeLast2 = millis();
  moveComputation_ptr = new MovementComputation(loopFrequency,&Serial);

  delay(50);

  moveComputation_ptr->calibration();
}

void loop()
{

  if (millis() - timeLast1 > ((int) 1000/loopFrequency))
  {
    i++;
    timeLast1 = millis();

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

    moveComputation_ptr->updateData();


    // Simulate movement
    if (communicationState)
    {
      communicationPort.printDebugVector(&(moveComputation_ptr->accVec));
    }

    // Send debug
    communicationPort.print("Acc : ");
    communicationPort.printDebugVector(&(moveComputation_ptr->accVecRaw));
    communicationPort.print("   |   Gyr : ");
    communicationPort.printDebugVector(&(moveComputation_ptr->gyrVecRaw));
    communicationPort.print("   |   Ori : ");
    communicationPort.printDebugVector(&(moveComputation_ptr->oriVecRaw));
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