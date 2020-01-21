
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

    /*
    if (_imu.setGyroFSR(250) != INV_SUCCESS)
    {
        _comPort_ptr->sendDebugMessage("Unable to set gyro dps");
        while (1)
            ;
    }

    if (_imu.setAccelFSR(2) != INV_SUCCESS)
    {
        _comPort_ptr->sendDebugMessage("Unable to set accelerometer resolution");
        while (1)
            ;
    }
    */
    /*
    if (_imu.setLPF(20) != INV_SUCCESS)
    {
        _comPort_ptr->sendDebugMessage("Unable to set filter");
        while (1)
            ;
    }
    */
    _comPort_ptr->sendDebugMessage("MPU-9250 OK");

    _lastTime = millis();
}


void MovementComputation::sendRotationMovement(float dTh, float dX)
{
    String listName[] = {"dX", "dTh"};
    float listVal[] = {dX, dTh};
    _comPort_ptr->sendData(2, listName, listVal);
}

bool MovementComputation::calibration()
{
    setCalibrationAcceleration();
    computeRotationAngle();
    
    _comPort_ptr->print("\n");
    _comPort_ptr->sendDebugMessage("Calibration done");
    _comPort_ptr->sendDebugMessage("Gravity vector : " + String(accVectorCalibration[0]) + " ; " + String(accVectorCalibration[1]) + " ; " + String(accVectorCalibration[2]));
    _comPort_ptr->sendDebugMessage("Rotation angles : " + String(_rotAngle1) + " ; " + String(_rotAngle2));

    return false;
}

bool MovementComputation::computeRotationAngle()
{
    _rotAngle1 = asin(-accVectorCalibration[1]);
    _rotAngle2 = atan(accVectorCalibration[0] / accVectorCalibration[2]);
    return true;
}

void MovementComputation::convertVectorFrame(float* oldVector, float* newVector)
{
    newVector[0] = (cos(_rotAngle2) * oldVector[0]) - (sin(_rotAngle2) * oldVector[2]);
    newVector[1] = (- sin(_rotAngle1) * sin(_rotAngle2) * oldVector[0]) + (cos(_rotAngle2) * oldVector[1]) - (sin(_rotAngle1) * cos(_rotAngle2) * oldVector[2]);
    newVector[2] = (cos(_rotAngle1) * sin(_rotAngle2) * oldVector[0]) + (sin(_rotAngle1) * oldVector[1]) + (cos(_rotAngle2) * cos(_rotAngle1) * oldVector[2]);
}

void MovementComputation::computeLinearMovement()
{
}

void MovementComputation::computeRotationMovement()
{
    _deltaTheta = oriVector[2] - _lastTheta;
    _lastTheta = oriVector[2];
}

bool MovementComputation::setCalibrationAcceleration(uint8_t nbrSample, uint8_t frequency)
{
    unsigned int delay = 1000 / frequency;
    unsigned long timeLast;
    timeLast = millis();
    uint16_t i = 1;
    float averageAccVect[3] = {0.0, 0.0, 0.0};

    while (i < nbrSample)
    {
        if (millis() - timeLast > delay)
        {
            timeLast = millis();
            _comPort_ptr->print(".");

            if (_imu.updateAccel() == INV_SUCCESS)
            {
                averageAccVect[0] = (averageAccVect[0] * i + _imu.ax) / (i + 1);
                averageAccVect[1] = (averageAccVect[1] * i + _imu.ay) / (i + 1);
                averageAccVect[2] = (averageAccVect[2] * i + _imu.az) / (i + 1);
                i += 1;
            }
        }
    }

    MPUtoG = sqrt(((averageAccVect[0] * averageAccVect[0]) + (averageAccVect[1] * averageAccVect[1]) + (averageAccVect[2] * averageAccVect[2])));

    accVectorCalibration[0] = averageAccVect[0] / MPUtoG;
    accVectorCalibration[1] = averageAccVect[1] / MPUtoG;
    accVectorCalibration[2] = averageAccVect[2] / MPUtoG;
    
    return true;
}

void MovementComputation::updateData()
{
    float acc[3];

    while (_imu.read(acc, 0, 0, 0, &oriRaw)) // To empty buffer and get last value
    {
        _comPort_ptr->print("_");
        delay(10);
    }

    _deltaT = millis() - _lastTime;
    _lastTime = millis();

    accVectorRaw[0] = _imu.ax * (GtoMs / MPUtoG);
    accVectorRaw[1] = _imu.ay * (GtoMs / MPUtoG);
    accVectorRaw[2] = _imu.az * (GtoMs / MPUtoG);
    convertVectorFrame(accVectorRaw, accVector);

    oriVectorRaw[0] = oriRaw.pitch;
    oriVectorRaw[1] = oriRaw.roll;
    oriVectorRaw[2] = oriRaw.yaw;
    
    convertVectorFrame(oriVectorRaw, oriVector);

    ori.pitch = oriVector[0];
    ori.roll = oriVector[1];
    ori.yaw = oriVector[2];
}