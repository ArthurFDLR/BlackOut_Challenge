/*************************************************************************
* Part of a prototype to the Black-Out Challenge organized by SAFRAN
* Developed by Arthur Findelair, January 2020
*************************************************************************/

#ifndef MOVEMENT_COMPUTATION_H_
#define MOVEMENT_COMPUTATION_H_

#include <Arduino.h>
#include <FreematicsPlus.h>
#include <SerialCommunication.h>


class MovementComputation
{
public:
    MovementComputation(SerialCommunication* comPort);

    // GOAL / Calculate rotAngle1 and rotAngle2. Do not move the accelerometer for 5 secondes.
    int orientationCalibration();

    // GOAL / Compute deltaX, distance travalled by the car during deltaT
    void linearMovementComputation();

    void rotationMovementComputation();

    float getYaw();

private:
    double _rotAngle1 = 0.0;
    double _rotAngle2 = 0.0;
    unsigned long _deltaT = 0;
    MPU9250_DMP _imu;
    SerialCommunication* _comPort_ptr;
};


#endif