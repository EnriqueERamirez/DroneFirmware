#ifndef DISTANCE_CONTROLLER_H
#define DISTANCE_CONTROLLER_H

#include <Arduino.h>
#include "VL53L0XSensorArray.h"

class DistanceController {
private:
    uint16_t UpperSensorDistance;
    uint16_t LowerSensorDistance;
    uint16_t LeftSensorDistance;
    uint16_t RightSensorDistance;
    uint16_t FrontSensorDistance;
    uint16_t BackSensorDistance;
    
    const uint16_t UPPER_THRESHOLD = 500;
    const uint16_t SIDE_THRESHOLD = 200;
    const uint16_t FRONT_THRESHOLD = 300;
    const uint16_t BACK_THRESHOLD = 200;
    
    float Throttle;
    float RollTarget;
    float PitchTarget;
    float HeightOutput;
    float HeightTarget;
    
    const float EMERGENCY_DESCENT_RATE = 80.0f;

public:
    DistanceController();
    
    void SetSensorDistances(VL53L0XSensorArray& sensorArray);
    void UpdateHeightControl();
    void UpdateCollisionAvoidance();
    void ProcessHeightAdjustment();
    
    void SetThrottle(float throttle) { Throttle = throttle; }
    void SetHeightOutput(float output) { HeightOutput = output; }
    void SetHeightTarget(float target) { HeightTarget = target; }
    
    float GetThrottle() const { return Throttle; }
    float GetRollTarget() const { return RollTarget; }
    float GetPitchTarget() const { return PitchTarget; }
    float GetHeightTarget() const { return HeightTarget; }
    float GetCurrentHeight() const { return static_cast<float>(LowerSensorDistance); }
    
    bool IsUpperObstacleDetected() const { return UpperSensorDistance <= UPPER_THRESHOLD; }
    bool IsCollisionDetected() const;
};

#endif