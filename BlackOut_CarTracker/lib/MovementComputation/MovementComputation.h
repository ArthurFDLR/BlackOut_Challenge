/*************************************************************************
* Part of a prototype to the Black-Out Challenge organized by SAFRAN
* Developed by Arthur Findelair, January 2020
*************************************************************************/

#ifndef MOVEMENT_COMPUTATION_H_
#define MOVEMENT_COMPUTATION_H_

#include <Arduino.h>
#include <FreematicsPlus.h>
#include <MadgwickAHRS.h>

#define KmH2MS 0.27777F
#define GtoMs 9.80733F // w/ altitude = 100m ; latitude = 46.5833deg  (Poitiers)

struct Vector
{
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
};

class MovementComputation
{
public:
    MovementComputation(float updateFrequency, HardwareSerial* comPort, bool OBD = true, bool IMU = true);

    Vector gyrVecRaw; // [deg/s] Reading of the IMU
    Vector gyrVec; // [deg/s] Set in the new frame

    Vector accVecRaw; // [m/s^2]
    Vector accVec; // [m/s^2]

    Vector oriVecRawLast; // [deg]
    Vector oriVecRaw; // [deg]
    Vector oriVecLast; // [deg]
    Vector oriVec; // [deg]

    int carSpeed; // [m/s]

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

    void updateDataIMU();

    bool updateDataOBD();

    // GOAL  / Change frame of the vector to the one defined by rotation by _rotAngle1 on axis 1 and _rotAngle2 on axis 2
    void convertVectorFrame(Vector* oldVector, Vector* newVector);

    // GOAL / Compute deltaX, distance travalled by the car during deltaT
    void computeLinearMovement();

    void computeRotationMovement(bool rawData = true);


    //// DEBUG ////
    ///////////////

    // void sendRotationMovement(float dTh = 0.087266, float dX = 1.0);


private:
    MPU9250_DMP _imu;
    HardwareSerial* _comPort_ptr;
    Madgwick filterMahony;

    // Data computation
    const float expFilterCoeff = 0.8;
    unsigned long _deltaT = 0;
    unsigned long _lastTime = 0;

    float _deltaTheta = 0.0;

    float _deltaX = 0.0;
    float _lastX = 0.0;

    // OBD

    FreematicsESP32 sys; // Necessary to communication through OBD
    COBD obd;
    bool obdConnected = false;

    // Calibration IMU
    double _rotAngle1 = 0.0;
    double _rotAngle2 = 0.0;
    float accVectorCalibration[3] = {0.0, 0.0, 0.0}; //[g] Calculated when the controller is not moving
    double MPUtoG = 1.0;
    
};


#endif