#ifdef _MAIN_IMU_

#include <FreematicsPlus.h>
#include <SerialCommunication.h>

MPU9250_DMP imu;
ORIENTATION ori;
SerialCommunication communicationPort;

float acc[3], gyr[3], mag[3];
int i=0;
unsigned long timeLast = 0;

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);
  timeLast = millis();
  //setupIMU();
}

void loop()
{
  if (millis() - timeLast > 500)
  {
    i ++;
    timeLast = millis();

    if (communicationPort.updateReception() == Ping)
      communicationPort.sendDebugMessage(String(10*i));

    float listVal[2];
    listVal[0] = (float) i * PI;
    listVal[1] = (float) i * PI * 2;
    String listName[2] = {"dX", "dY"};
    communicationPort.sendData(2,listName,listVal);
  }
}



void setupIMU(){
  communicationPort.sendDebugMessage("MPU-9250 DMP Quaternion Test");

  if (!imu.begin(true, 10)) //Activate data fusion
  {
    communicationPort.sendDebugMessage("Unable to communicate with MPU-9250");
    while (1)
      ;
  }
  communicationPort.sendDebugMessage("MPU-9250 OK");

  if (imu.setSensors(INV_XYZ_COMPASS | INV_XYZ_GYRO | INV_XYZ_ACCEL) != INV_SUCCESS) //250dps enough for a car
  {
    communicationPort.sendDebugMessage("Unable to set all sensors");
    while (1)
      ;
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