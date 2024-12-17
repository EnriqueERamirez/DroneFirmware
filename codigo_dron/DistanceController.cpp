#include "DistanceController.h"

DistanceController::DistanceController()
    : UpperSensorDistance(0), LowerSensorDistance(0),
      LeftSensorDistance(0), RightSensorDistance(0),
      FrontSensorDistance(0), BackSensorDistance(0),
      Throttle(0), RollTarget(0), PitchTarget(0), 
      HeightOutput(0), HeightTarget(0) {  // Inicializado HeightTarget
}

void DistanceController::SetSensorDistances(uint16_t upper, uint16_t lower, 
                                          uint16_t left, uint16_t right, 
                                          uint16_t front, uint16_t back) {
    UpperSensorDistance = upper;
    LowerSensorDistance = lower;
    LeftSensorDistance = left;
    RightSensorDistance = right;
    FrontSensorDistance = front;
    BackSensorDistance = back;
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