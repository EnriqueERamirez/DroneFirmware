#ifndef DRONE_PID_H
#define DRONE_PID_H

class DronePID {
private:
    // Variables para PID de ángulos
    float PitchError;
    float PitchP;
    float PitchI;
    float PitchD;
    float PitchOut;
    
    float RollError;
    float RollP;
    float RollI;
    float RollD;
    float RollOut;
    
    // Variables para PID de velocidad angular
    float PitchErrorV;
    float PitchPV;
    float PitchIV;
    float PitchDV;
    float PitchOutV;
    
    float RollErrorV;
    float RollPV;
    float RollIV;
    float RollDV;
    float RollOutV;
    
    float YawErrorV;
    float YawPV;
    float YawIV;
    float YawDV;
    float YawOutV;

    // Variables para PID de altura
    float AltitudeError;
    float AltitudeP;
    float AltitudeI;
    float AltitudeD;
    float AltitudeOut;
    
    // Constantes PID
    const float PitchKp;
    const float PitchKi;
    const float PitchKd;
    const float RollKp;
    const float RollKi;
    const float RollKd;
    
    const float PitchKpV;
    const float PitchKiV;
    const float PitchKdV;
    const float RollKpV;
    const float RollKiV;
    const float RollKdV;
    const float YawKpV;
    const float YawKiV;
    const float YawKdV;
    
    const float AltitudeKp;
    const float AltitudeKi;
    const float AltitudeKd;
    
    // Límites PID
    const float PIDLimit1;
    const float PIDLimit2;
    const float PIDLimit1V;
    const float PIDLimit2V;
    const float PIDLimit1Altitude;
    const float PIDLimit2Altitude;
    
    // Variables de estado
    float AngleX;
    float AngleY;
    float AngleXPrev;
    float AngleYPrev;
    float GyroX;
    float GyroY;
    float GyroZ;
    float GyroXPrev;
    float GyroYPrev;
    float GyroZPrev;
    float Setpoint;
    float PitchSetpoint;
    float RollSetpoint;
    float YawSetpoint; 
    float AltitudeSetpoint;
    float LowerSensor;
    float LowerSensorPrev;

public:
    DronePID();
    void UpdateAnglePID();
    void UpdateAngularVelocityPID();
    void UpdateAltitudePID();
    
    float GetPitchOutput() const { return PitchOutV; }
    float GetRollOutput() const { return RollOutV; }
    float GetYawOutput() const { return YawOutV; }
    float GetAltitudeOutput() const { return AltitudeOut; }
    
    void SetAngles(float angleX, float angleY);
    void SetGyros(float gyroX, float gyroY, float gyroZ);
    void SetAltitude(float altitude);
    void SetSetpoints(float rollTarget, float pitchTarget, float yawTarget);
    void SetAltitudeSetpoint(float target);
};

#endif