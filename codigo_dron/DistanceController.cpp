#include "DistanceController.h"

DistanceController::DistanceController()
    : UpperSensorDistance(0), LowerSensorDistance(0),
      LeftSensorDistance(0), RightSensorDistance(0),
      FrontSensorDistance(0), BackSensorDistance(0),
      Throttle(0), RollTarget(0), PitchTarget(0), 
      HeightOutput(0), HeightTarget(0) {  // Inicializado HeightTarget
}

void DistanceController::SetSensorDistances(VL53L0XSensorArray& sensorArray) {
    UpperSensorDistance = sensorArray.GetDistance(VL53L0XSensorArray::TOP);
    LowerSensorDistance = sensorArray.GetDistance(VL53L0XSensorArray::BOTTOM);
    LeftSensorDistance = sensorArray.GetDistance(VL53L0XSensorArray::LEFT);
    RightSensorDistance = sensorArray.GetDistance(VL53L0XSensorArray::RIGHT);
    FrontSensorDistance = sensorArray.GetDistance(VL53L0XSensorArray::FRONT);
    BackSensorDistance = sensorArray.GetDistance(VL53L0XSensorArray::BACK);
    
    // For debugging purposes
    static unsigned long lastDebugOutput = 0;
    const unsigned long DEBUG_INTERVAL = 1000;
    
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebugOutput >= DEBUG_INTERVAL) {
        Serial.println("Sensor Distances (mm):");
        Serial.printf("Upper: %d, Lower: %d\n", UpperSensorDistance, LowerSensorDistance);
        Serial.printf("Left: %d, Right: %d\n", LeftSensorDistance, RightSensorDistance);
        Serial.printf("Front: %d, Back: %d\n", FrontSensorDistance, BackSensorDistance);
        
        lastDebugOutput = currentMillis;
    }
}

void DistanceController::UpdateHeightControl() {
    if (IsUpperObstacleDetected()) {
        // Descenso de emergencia si se detecta obstáculo arriba
        Throttle -= EMERGENCY_DESCENT_RATE;
    }
    
}

void DistanceController::UpdateCollisionAvoidance() {
    // Restablecer objetivos de control
    RollTarget = 0;
    PitchTarget = 0;
    
    // Control lateral (Roll)
    if (LeftSensorDistance <= SIDE_THRESHOLD) {
        RollTarget = 20.0f;  // Mover hacia la derecha
    }
    if (RightSensorDistance <= SIDE_THRESHOLD) {
        RollTarget = -20.0f; // Mover hacia la izquierda
    }
    
    // Control frontal/trasero (Pitch)
    if (FrontSensorDistance <= FRONT_THRESHOLD) {
        PitchTarget = 20.0f; // Mover hacia atrás
    }
    if (BackSensorDistance <= BACK_THRESHOLD) {
        PitchTarget = -20.0f; // Mover hacia adelante
    }
}

void DistanceController::ProcessHeightAdjustment() {
    Throttle += HeightOutput;
}

bool DistanceController::IsCollisionDetected() const {
    return LeftSensorDistance <= SIDE_THRESHOLD ||
           RightSensorDistance <= SIDE_THRESHOLD ||
           FrontSensorDistance <= FRONT_THRESHOLD ||
           BackSensorDistance <= BACK_THRESHOLD ||
           UpperSensorDistance <= UPPER_THRESHOLD;
}