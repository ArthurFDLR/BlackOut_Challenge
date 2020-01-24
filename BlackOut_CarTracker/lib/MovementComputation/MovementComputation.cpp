
#include <MovementComputation.h>

MovementComputation::MovementComputation(float updateFrequency, HardwareSerial *comPort, bool OBD, bool IMU)
{
    _comPort_ptr = comPort;

    // initializations OBD
    _comPort_ptr->print("Launch system \n");

    if (OBD)
    {
        _comPort_ptr->print("Connecting to OBD");
        while (!sys.begin())
        {
            _comPort_ptr->print(".");
        }
        obd.begin(sys.link);

        
        if (obd.init())
        {
            obdConnected = true;
            _comPort_ptr->print("OK");
        }
        else
        {
            _comPort_ptr->print(".");
        }
    }

    if (IMU)
    {
        _comPort_ptr->print("Connecting to IMU");
        if (!_imu.begin(true, 10)) //Activate data fusion
        {
            _comPort_ptr->println("Unable to communicate with MPU-9250");
            while (1)
                ;
        }
        calibration();
        filterMahony.begin(updateFrequency);
        _comPort_ptr->println("MPU-9250 OK and calibrated");
    }

    _lastTime = millis();
}

bool MovementComputation::calibration()
{
    setCalibrationAcceleration();
    computeRotationAngle();

    _comPort_ptr->print("\n");

    return false;
}

bool MovementComputation::computeRotationAngle()
{
    _rotAngle1 = asin(-accVectorCalibration[1]);
    _rotAngle2 = atan(accVectorCalibration[0] / accVectorCalibration[2]);
    return true;
}

void MovementComputation::convertVectorFrame(Vector *oldVector, Vector *newVector)
{
    newVector->x = (cos(_rotAngle2) * oldVector->x) - (sin(_rotAngle2) * oldVector->z);
    newVector->y = (-sin(_rotAngle1) * sin(_rotAngle2) * oldVector->x) + (cos(_rotAngle2) * oldVector->y) - (sin(_rotAngle1) * cos(_rotAngle2) * oldVector->z);
    newVector->z = (cos(_rotAngle1) * sin(_rotAngle2) * oldVector->x) + (sin(_rotAngle1) * oldVector->y) + (cos(_rotAngle2) * cos(_rotAngle1) * oldVector->z);
}

bool MovementComputation::updateDataOBD()
{
    bool out = true;
    if (!obdConnected)
    {
        _comPort_ptr->print("Reconnecting to OBD");
        if (obd.init())
        {
            obdConnected = true;
            _comPort_ptr->print("OK");
        }
        else
        {
            _comPort_ptr->print(".");
        }
    }

    int speed;
    if (! obd.readPID(PID_SPEED, speed))
    {
        _comPort_ptr->print(speed);
        carSpeedRaw = speed;
        out = false;
        _comPort_ptr->println("error OBD");
    }

    carSpeedRaw = speed;
    carSpeed = (float) KmH2MS * carSpeedRaw;
    // _comPort_ptr->println(speed);

    if (obd.errors > 2)
    {
        _comPort_ptr->print("Reconnecting to OBD");
        obdConnected = false;
        obd.reset();
    }

    return out;
}

void MovementComputation::computeLinearMovement()
{
    _deltaX = ((float) (_deltaT / 1000.0)) * carSpeed;
}

void MovementComputation::computeRotationMovement(bool rawData)
{
    if (rawData)
    {
        if ((oriVecRawLast.z - oriVecRaw.z < 200.0) && (oriVecRawLast.z - oriVecRaw.z > -200.0))  // Avoid problem when total rotation
        {
            _deltaTheta = (oriVecRawLast.z - oriVecRaw.z) * expFilterCoeff + _deltaTheta * (1.0 - expFilterCoeff);
        }
        else
        {
            _deltaTheta = (oriVecRawLast.z - oriVecRaw.z - ((oriVecRawLast.z > oriVecRaw.z) ? 1.0 : -1.0) * 360.0) * expFilterCoeff + _deltaTheta * (1.0 - expFilterCoeff);
        }
    }
    else
    {
        _deltaTheta = (oriVecLast.z - oriVec.z) * expFilterCoeff + _deltaTheta * (1.0 - expFilterCoeff);
    }
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

void MovementComputation::updateDataIMU()
{
    float acc[3];
    float gyr[3];
    ORIENTATION ori;

    while (_imu.read(acc, gyr, 0, 0, &ori)) // To empty buffer and get last value
    {
        delay(10);
    }

    _deltaT = millis() - _lastTime;
    _lastTime = millis();

    accVecRaw.x = _imu.ax * ( GtoMs / MPUtoG);
    accVecRaw.y = _imu.ay * ( GtoMs / MPUtoG);
    accVecRaw.z = _imu.az * ( GtoMs / MPUtoG);
    convertVectorFrame(&accVecRaw, &accVec);

    gyrVecRaw.x = (((gyr[0] < pow(10.0, 38.0)) & (gyr[0] > -pow(10.0, 38.0))) ? gyr[0] : 0.0);
    gyrVecRaw.y = (((gyr[1] < pow(10.0, 38.0)) & (gyr[1] > -pow(10.0, 38.0))) ? gyr[1] : 0.0);
    gyrVecRaw.z = (((gyr[2] < pow(10.0, 38.0)) & (gyr[2] > -pow(10.0, 38.0))) ? gyr[2] : 0.0);
    convertVectorFrame(&gyrVecRaw, &gyrVec);

    filterMahony.updateIMU(gyrVecRaw.x, gyrVecRaw.y, gyrVecRaw.z, accVecRaw.x, accVecRaw.y, accVecRaw.z);

    oriVecRawLast = oriVecRaw;
    oriVecRaw.x = filterMahony.getPitch();
    oriVecRaw.y = filterMahony.getRoll();
    oriVecRaw.z = filterMahony.getYaw();
    oriVecLast = oriVec;
    convertVectorFrame(&oriVecRaw, &oriVec);
}

Movement MovementComputation::getMovement()
{
    Movement moveOut;
    moveOut.dTheta = _deltaTheta;
    moveOut.dX = _deltaX;
    return moveOut;
}

void MovementComputation::update()
{
    updateDataOBD();
    updateDataIMU();

    computeRotationMovement();
    computeLinearMovement();
}