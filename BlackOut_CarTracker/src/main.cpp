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

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);
  timeLast1 = millis();
  timeLast2 = millis();
  moveComputation_ptr = new MovementComputation(&communicationPort);
}

void loop()
{
  if (millis() - timeLast1 > 100)
  {
    i++;
    timeLast1 = millis();

    if (communicationPort.updateReception() == Ping)
      communicationPort.sendDebugMessage(String(10 * i));
    
    // Send Yaw
    String listName[] = {"Y"};
    float listVal[] = {moveComputation_ptr->getYaw()};
    timeLast1 = millis();
    communicationPort.sendDebugMessage(String(moveComputation_ptr->getYaw()));
    communicationPort.sendData(1, listName, listVal);

    // Simulate movement
    moveComputation_ptr->sendRotationMovement();

    /*
    float listVal[3];
    listVal[0] = (float)i * PI;
    listVal[1] = (float)i * PI * 2;
    listVal[2] = (float)i * PI * 3;
    String listName[3] = {"dX", "dY", "dTh"};
    communicationPort.sendData(3, listName, listVal);
    */
  }
}

/*
if (imu.read(acc, gyr, mag, 0, &ori)) {
  Serial.print("Accelerometer: X=");
  Serial.print(acc[0]);
  Serial.print("g Y=");
  Serial.print(acc[1]);
  Serial.print("g Z=");
  Serial.print(acc[2]);
  Serial.println("g");
}
*/

#endif
/*
#include <FreematicsPlus.h>
#include <SerialCommunication.h>

MPU9250_DMP imu;

SerialCommunication communicationPort;

void setup()
{
  delay(1000);
  Serial.begin(115200);

  if (!imu.begin(true, 10))
  {

    for (;;)
      ;
  }

}
float acc[3];
ORIENTATION ori;

void loop()
{

  if (imu.read(acc, 0, 0, 0, &ori))
  {
    String listName[] = {"Yaw"};
    float listVal[] = {imu.yaw};
    communicationPort.sendData(1, listName, listVal);
  }

  delay(50);
  return;
}
*/