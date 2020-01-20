
#include <MovementComputation.h>

MovementComputation::MovementComputation(SerialCommunication *comPort)
{
    _comPort_ptr = comPort;

    _comPort_ptr->sendDebugMessage("MPU-9250 DMP Quaternion Test");

    
    if (!_imu.begin(true, 10)) //Activate data fusion
    {
        _comPort_ptr->sendDebugMessage("Unable to communicate with MPU-9250");
        while (1)
            ;
    }
    _comPort_ptr->sendDebugMessage("MPU-9250 OK");

    /*
    if (_imu.setSensors(INV_XYZ_COMPASS | INV_XYZ_GYRO | INV_XYZ_ACCEL) != INV_SUCCESS) //250dps enough for a car
    {
        _comPort_ptr->sendDebugMessage("Unable to set all sensors");
        while (1)
            ;
    }
    */
}

float MovementComputation::getYaw()
{
    ORIENTATION ori;
    float acc[3];

    if (!_imu.read(acc, 0, 0, 0, &ori))
    {
        //_comPort_ptr->sendDebugMessage("Not updated");
    }
    return ori.yaw;
}

int MovementComputation::orientationCalibration()
{
    return 1;
}

void MovementComputation::linearMovementComputation()
{

}

void MovementComputation::rotationMovementComputation()
{

}