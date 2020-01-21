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

    float accVectorRaw[3] = {0.0, 0.0, 0.0}; //Reading of the IMU
    float oriVectorRaw[3] = {0.0, 0.0, 0.0}; //Reading of the IMU
    float accVector[3] = {0.0, 0.0, 0.0}; //Set in the new frame
    float oriVector[3] = {0.0, 0.0, 0.0}; //Set in the new frame
    ORIENTATION oriRaw;
    ORIENTATION ori;

    MovementComputation(SerialCommunication* comPort);

    //// CALIBRATION ////
    /////////////////////

    // GOAL / Calculate rotAngle1, rotAngle2, MPUtoG. Do not move the accelerometer for 5 secondes.
    bool calibration();

    // GOAL / Generate acceleration vector in a certain position (average _nbrSample_ data collected at _frequency_) and MPUtoG
    //        Blocking function
    bool setCalibrationAcceleration(uint8_t nbrSample = 100, uint8_t frequency = 25);

    // GOAL / Compute _rotAngle1 and _rotAngle2 given accVectorCalibration
    bool computeRotationAngle();

    //// UPDATE MOVEMENT ////
    /////////////////////////

    void updateData();

    // GOAL  / Change frame of the vector to the one defined by rotation by _rotAngle1 on axis 1 and _rotAngle2 on axis 2
    void convertVectorFrame(float* oldVector, float* newVector);

    // GOAL / Compute deltaX, distance travalled by the car during deltaT
    void computeLinearMovement();

    void computeRotationMovement();


    //// DEBUG ////
    ///////////////

    void sendRotationMovement(float dTh = 0.087266, float dX = 1.0);


private:
    MPU9250_DMP _imu;
    SerialCommunication* _comPort_ptr;

    // Data computation
    unsigned long _deltaT = 0;
    unsigned long _lastTime = 0;

    float _deltaTheta = 0.0;
    float _lastTheta = 0.0;

    float _deltaX = 0.0;
    float _lastX = 0.0;

    // Calibration
    double _rotAngle1 = 0.0;
    double _rotAngle2 = 0.0;
    float accVectorCalibration[3] = {0.0, 0.0, 0.0}; //[g] Calculated when the controller is not moving
    double MPUtoG = 1.0;
    double GtoMs = 9.80733; // w/ altitude = 100m ; latitude = 46.5833deg  (Poitiers)
};


#endif