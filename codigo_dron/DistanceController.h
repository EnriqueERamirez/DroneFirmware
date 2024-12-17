#ifndef DISTANCE_CONTROLLER_H
#define DISTANCE_CONTROLLER_H

#include <Arduino.h>

class DistanceController {
private:
    // Parámetros de sensores
    uint16_t UpperSensorDistance;
    uint16_t LowerSensorDistance;
    uint16_t LeftSensorDistance;
    uint16_t RightSensorDistance;
    uint16_t FrontSensorDistance;
    uint16_t BackSensorDistance;
    
    // Umbrales de seguridad
    const uint16_t UPPER_THRESHOLD = 500;
    const uint16_t SIDE_THRESHOLD = 200;
    const uint16_t FRONT_THRESHOLD = 300;
    const uint16_t BACK_THRESHOLD = 200;
    
    // Valores de control
    float Throttle;
    float RollTarget;
    float PitchTarget;
    float HeightOutput;
    
    // Parámetros de ajuste
    const float EMERGENCY_DESCENT_RATE = 80.0f;
    
public:
    DistanceController();
    
    // Métodos de actualización de sensores
    void SetSensorDistances(uint16_t upper, uint16_t lower, uint16_t left, 
                          uint16_t right, uint16_t front, uint16_t back);
    
    // Métodos de control
    void UpdateHeightControl();
    void UpdateCollisionAvoidance();
    void ProcessHeightAdjustment();
    
    // Setters
    void SetThrottle(float throttle) { Throttle = throttle; }
    void SetHeightOutput(float output) { HeightOutput = output; }
    
    // Getters
    float GetThrottle() const { return Throttle; }
    float GetRollTarget() const { return RollTarget; }
    float GetPitchTarget() const { return PitchTarget; }
    
    // Métodos de estado
    bool IsUpperObstacleDetected() const { return UpperSensorDistance <= UPPER_THRESHOLD; }
    bool IsCollisionDetected() const;
};

#endif