
#include "DronePID.h"
#include <Arduino.h>

DronePID::DronePID() :
    // Constantes PID ángulo
    PitchKp(0.55f), PitchKi(0.0006f), PitchKd(0.0f),
    RollKp(0.50f), RollKi(0.00065f), RollKd(0.0f),
    
    // Constantes PID velocidad angular
    PitchKpV(1.35f), PitchKiV(0.0007f), PitchKdV(0.0f),
    RollKpV(1.3f), RollKiV(0.0007f), RollKdV(0.0f),
    YawKpV(2.5f), YawKiV(0.0004f), YawKdV(0.0f),
    
    // Constantes PID altura
    AltitudeKp(1.0f), AltitudeKi(0.0f), AltitudeKd(0.0f),
    
    // Límites
    PIDLimit1(300.0f), PIDLimit2(430.0f),
    PIDLimit1V(300.0f), PIDLimit2V(430.0f),
    PIDLimit1Altitude(200.0f), PIDLimit2Altitude(1000.0f),

    // Altitud
    AltitudeSetpoint(10.0f)
{
    // Inicialización de variables
    PitchI = RollI = PitchIV = RollIV = YawIV = AltitudeI = 0.0f;
    Setpoint = 0.0f; 
    PitchSetpoint = RollSetpoint = YawSetpoint = 0.0f;
}

void DronePID::UpdateAnglePID() {
    // PID Pitch (ángulo)
    PitchError = PitchSetpoint - AngleX;
    PitchP = PitchKp * PitchError;
    PitchI += PitchKi * PitchError;
    PitchI = constrain(PitchI, -PIDLimit1, PIDLimit1);
    PitchD = PitchKd * (AngleX - AngleXPrev);
    PitchOut = PitchP + PitchI + PitchD;
    PitchOut = constrain(PitchOut, -PIDLimit2, PIDLimit2);

    // PID Roll (ángulo)
    RollError = RollSetpoint - AngleY;
    RollP = RollKp * RollError;
    RollI += RollKi * RollError;
    RollI = constrain(RollI, -PIDLimit1, PIDLimit1);
    RollD = RollKd * (AngleY - AngleYPrev);
    RollOut = RollP + RollI + RollD;
    RollOut = constrain(RollOut, -PIDLimit2, PIDLimit2);
    
    AngleXPrev = AngleX;
    AngleYPrev = AngleY;
}

void DronePID::UpdateAngularVelocityPID() {
    // PID Pitch (velocidad angular)
    PitchErrorV = PitchOut - GyroX;
    PitchPV = PitchKpV * PitchErrorV;
    PitchIV += PitchKiV * PitchErrorV;
    PitchIV = constrain(PitchIV, -PIDLimit1V, PIDLimit1V);
    PitchDV = PitchKdV * (GyroX - GyroXPrev);
    PitchOutV = PitchPV + PitchIV + PitchDV;
    PitchOutV = constrain(PitchOutV, -PIDLimit2V, PIDLimit2V);

    // PID Roll (velocidad angular)
    RollErrorV = RollOut - GyroY;
    RollPV = RollKpV * RollErrorV;
    RollIV += RollKiV * RollErrorV;
    RollIV = constrain(RollIV, -PIDLimit1V, PIDLimit1V);
    RollDV = RollKdV * (GyroY - GyroYPrev);
    RollOutV = RollPV + RollIV + RollDV;
    RollOutV = constrain(RollOutV, -PIDLimit2V, PIDLimit2V);

    // PID Yaw (velocidad angular)
    YawErrorV = YawSetpoint - GyroZ;
    YawPV = YawKpV * YawErrorV;
    YawIV += YawKiV * YawErrorV;
    YawIV = constrain(YawIV, -PIDLimit1V, PIDLimit1V);
    YawDV = YawKdV * (GyroZ - GyroZPrev);
    YawOutV = YawPV + YawIV + YawDV;
    YawOutV = constrain(YawOutV, -PIDLimit2V, PIDLimit2V);
    
    GyroXPrev = GyroX;
    GyroYPrev = GyroY;
    GyroZPrev = GyroZ;
}

void DronePID::UpdateAltitudePID() {
    AltitudeError = AltitudeSetpoint - LowerSensor; 
    AltitudeP = AltitudeKp * AltitudeError;
    AltitudeI += AltitudeKi * AltitudeError;
    AltitudeI = constrain(AltitudeI, -PIDLimit1Altitude, PIDLimit1Altitude);
    AltitudeD = AltitudeKd * (LowerSensor - LowerSensorPrev);
    AltitudeOut = AltitudeP + AltitudeI + AltitudeD;
    AltitudeOut = constrain(AltitudeOut, -PIDLimit2Altitude, PIDLimit2Altitude);
    
    LowerSensorPrev = LowerSensor;
}

void DronePID::SetAngles(float angleX, float angleY) {
    AngleX = angleX;
    AngleY = angleY;
}

void DronePID::SetGyros(float gyroX, float gyroY, float gyroZ) {
    GyroX = gyroX;
    GyroY = gyroY;
    GyroZ = gyroZ;
}

void DronePID::SetAltitude(float altitude) {
    LowerSensor = altitude;
}

void DronePID::SetSetpoints(float rollTarget, float pitchTarget, float yawTarget) {
    RollSetpoint = rollTarget;
    PitchSetpoint = pitchTarget;
    YawSetpoint = yawTarget;
}
void DronePID::SetAltitudeSetpoint(float target) {
    AltitudeSetpoint = target;
}