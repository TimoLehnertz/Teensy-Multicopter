/**
 * @file sensorInterface.h
 * @author Timo Lehnertz
 * @brief 
 * @version 0.1
 * @date 2022-01-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <error.h>
#include <flightModes.h>
#include <maths.h>
#include <lpf.h>

/**
 * General data type for all sensors on board
 *      last change: last time the readings got updated in micro seconds
 *      error: 0 => OK, 1 => Warning(typically missing calibration but should be possible to fly), 2 => Critical error
 */
struct Sensor {
    uint64_t lastChange;
    uint64_t lastPollTime; // Supposed to be measured and set By SensorInterface implementation
    Error::Error_t error;
    FlightMode::FlightMode_t minFlightMode;

    Sensor(FlightMode::FlightMode_t minFlightMode) : minFlightMode(minFlightMode) {}

    virtual void checkError() = 0;

    bool isError() {
        return error == Error::CRITICAL_ERROR;
    }
};

struct Vec3Sensor : public Sensor {
public:
    float x, y, z, lastX, lastY, lastZ;
    int similarCount;
    Vec3 last;
    // float lpf = 1.0f;
    

    Vec3Sensor(FlightMode::FlightMode_t minFlightMode) : Sensor(minFlightMode), similarCount(0) {}

    void update(Vec3 vec) {
        update(vec.x, vec.y, vec.z);
    }

    void update(float x1, float y1, float z1) {
        Vec3 vec(x1, y1, z1);
        if(x1 != x || y1 != y || z1 != z) {
            lastChange = micros();
            // for (size_t i = 0; i < 1; i++) {
            //     last = lpfs[i].update(vec);
            // }
            last = vec;
            
            x = last.x;
            y = last.y;
            z = last.z;

            // x = x1 * lpf + (1.0 - lpf) * last.x;
            // y = y1 * lpf + (1.0 - lpf) * last.y;
            // z = z1 * lpf + (1.0 - lpf) * last.z;
            // last = Vec3(x, y, z);


        }
    }

    Vec3 getVec3() {
        return Vec3(x, y, z);
    }

    /**
     * check if sensor readings change and if not for 100 readings raise CRITICAL_ERROR
     */
    void checkError() {
        if(micros() - lastChange > 100000) {//100ms
            error = Error::CRITICAL_ERROR;
            return;
        }
        if(x == lastX && y == lastY && z == lastZ) {
            similarCount++;
            if(similarCount >= 100) {
                similarCount = 100;
                error = Error::CRITICAL_ERROR;
            }
        } else {
            similarCount = 0;
            lastX = x;
            lastY = y;
            lastZ = z;
            error = Error::NO_ERROR;
        }
    }

    void setLpf(int filter, double deltaT, int freq) {
        lpfs[filter].reconfigureFilter(deltaT, freq);
    }
    int getLpfFreq(int filter) {
        return lpfs[filter].getCutoffFreq();
    }
private:
    LowPassFilterVec3 lpfs[1];
};

struct Accelerometer : public Vec3Sensor {
    Accelerometer() : Vec3Sensor(FlightMode::level) {}
};

struct Gyroscope : public Vec3Sensor {
    Gyroscope() : Vec3Sensor(FlightMode::rate) {}
};

struct Magnetometer : public Vec3Sensor {
    Magnetometer() : Vec3Sensor(FlightMode::gpsHold) {}
};

struct Barometer : public Sensor{
    float altitude, lastAltitude;     //Meters
    float preassure;    //atmospheres
    float temperature;  //celcius

    int similarCount;

    Barometer() : Sensor(FlightMode::altitudeHold), similarCount(0) {}

    void checkError() {
        if(altitude == lastAltitude) {
            similarCount++;
            if(similarCount >= 1000) {
                similarCount = 1000;
                error = Error::CRITICAL_ERROR;
            }
        } else {
            similarCount = 0;
            lastAltitude = altitude;
            error = Error::NO_ERROR;
        }
    }
};

struct GPS : public Sensor{
    bool locationValid = false;
    double lat, lng, lastLat, lastLng; //degrees
    int similarCount;

    //date
    bool dateValid = false;
    int year;
    int month;
    int day;

    //time
    bool timeValid = false;
    int hour;
    int minute;
    int second;
    int centisecond;

    //speed
    float speed; //m/s
    bool speedValid = false;

    //course
    float course; //degrees
    bool courseValid = false;

    //altitude
    float altitude; //Meters
    bool altitudeValid = false;

    //satelites
    int satelites;

    //precision
    float hdop; // hdop < 2 = good, hdop < 8 = ok

    GPS() : Sensor(FlightMode::gpsHold) {}

    void checkError() {
        if(micros() - lastChange > 2000000) {// || satelites < 3) { // 2 sec timeout
            error = Error::CRITICAL_ERROR;
        } else {
            error = Error::NO_ERROR;
        }
    }
};


struct Ultrasonic : public Sensor {
    double distance = 0.0; //m
    double speed = 0.0; //m/s
    bool outOfRange  = false;
    bool connected = false;

    Ultrasonic() : Sensor(FlightMode::dreaming) {}

    void update(double distance, double speed, bool outOfRange) {
        this->distance = distance;
        this->speed = speed;
        this->outOfRange = outOfRange;
    }

    void checkError() {}
};

struct Battery : public Sensor {
    float vBat; // Volts
    float vCell; // Volts
    byte cellCount;

    Battery() : Sensor(FlightMode::dreaming) {}

    void checkError() {} // do nothing
};

/**
 * Abstract class for interfacing with all sensors.
 * stores only raw sensor data
 */
class SensorInterface {
public:
    /**
     * Enum used for accelerometer calibration
     */
    enum Side {
        bottom = 0,
        left = 1,
        right = 2,
        front = 3,
        back = 4,
        top = 5
    };

    bool useAcc = true;
    bool useMag = true;

    static constexpr byte sensorCount = 6;

    Sensor* sensors[sensorCount];

    Accelerometer acc;
    Gyroscope gyro;
    Magnetometer mag;

    Barometer baro;

    GPS gps;

    Ultrasonic ultrasonic;

    Battery bat;

    float batLpf = 0.0001;
    float batOffset = -0.105;
    float vBatMul = 9.85000;

    float accLpf = 1.0f;
    float gyroLpf = 1.0f;

    SensorInterface() {
        sensors[0] = &acc;
        sensors[1] = &gyro;
        sensors[2] = &mag;
        sensors[3] = &baro;
        sensors[4] = &gps;
        sensors[5] = &bat;
    }

    virtual void begin() = 0;
    virtual void handle() = 0;

    virtual void setAccCal (Vec3 gVecOffset, Vec3 scale)  = 0;
    virtual void setGyroCal(Vec3 degVecOffset, Vec3 gyroScale) = 0;
    virtual void setMagCal (Vec3 offset, Vec3 scale)  = 0;

    virtual void calibrateAcc() = 0;
    virtual void calibrateGyroOffset() = 0;
    virtual void calibrateGyroScale() = 0;
    virtual void calibrateMag() = 0;

    virtual Vec3 getAccOffset() = 0;
    virtual Vec3 getAccScale() = 0;
    virtual Vec3 getGyroOffset() = 0;
    virtual Vec3 getGyroScale() = 0;
    virtual Vec3 getMagOffset() = 0;
    virtual Vec3 getMagScale() = 0;

    virtual void calibrateBat(float actualVoltage) = 0;

    /**
     * Find the highest Flight mode that is possible to achieve with the sensors with an equal or lower error that maxError
     */
    FlightMode::FlightMode_t getHighestFM(Error::Error_t maxError = Error::WARNING) {
        FlightMode::FlightMode_t fm = FlightMode::FlightModeSize;
        for (size_t i = 0; i < sensorCount; i++) {
            if(sensors[i]->error > maxError) {
                fm = FlightMode::FlightMode_t(min(fm, sensors[i]->minFlightMode - 1));
            }
        }
        return fm;
    }
};