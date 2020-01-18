#include <FreematicsPlus.h>
#include <SerialCommunication.h>

MPU9250_DMP imu;
ORIENTATION ori;
SerialCommunication communicationPort;

float acc[3], gyr[3], mag[3];

void setup()
{
  Serial.begin(115200);
  communicationPort.Setup(&Serial, &Serial);

  /*
  Serial.println("MPU-9250 DMP Quaternion Test");

  if (!imu.begin(true, 10)) //Activate data fusion
  {
    Serial.println("Unable to communicate with MPU-9250");
    while (1)
      ;
  }
  Serial.println("MPU-9250 OK");

  if (imu.setSensors(INV_XYZ_COMPASS | INV_XYZ_GYRO | INV_XYZ_ACCEL) != INV_SUCCESS) //250dps enough for a car
  {
    Serial.println("Unable to set all sensors");
    while (1)
      ;
  }
  */
}

void loop()
{
  for (int i = 0; i < 10; i++)
  {
    if (communicationPort.updateReception() == Ping)
      communicationPort.sendDebugMessage(String(10*i));

    communicationPort.sendData("Hello", PI * i);
    delay(500);
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
}