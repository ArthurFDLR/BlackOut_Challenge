#include <FreematicsPlus.h>

MPU9250_DMP imu;
float acc[3], gyr[3], mag[3];
ORIENTATION ori;

void setup()
{
  Serial.begin(115200);
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

void serialFloatPrint(HardwareSerial SerialComm ,float f) {
  byte * b = (byte *) &f;
  //Serial.print("f:");
  for(int i=0; i<4; i++) {
    
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    
    SerialComm.print(c1);
    SerialComm.print(c2);
  }
}

void sendData(String Name, float Value, HardwareSerial SerialComm)
{
    SerialComm.print('@');
    SerialComm.print(Name);
    SerialComm.print("#");
    serialFloatPrint(SerialComm, Value);
}

void loop()
{
  for (int i=0 ; i < 10 ; i++)
  {
    
    if (Serial.available() > 0)
    {
      if (char(Serial.read()) == '!')
      {
        Serial.print("|");
        Serial.println(i);
      }
    }

    sendData("PIs", i*PI, Serial);
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