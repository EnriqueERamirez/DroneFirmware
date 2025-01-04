#ifndef MOTORS_CONTROLLER_H
#define MOTORS_CONTROLLER_H

#include <Arduino.h>

class MotorsController {
private:
    // PWM Configuration
    static const uint32_t PWM_FREQUENCY = 2000;
    static const uint8_t PWM_RESOLUTION = 10;

    // Variables para el suavizado
    uint16_t TargetMotor1Speed;
    uint16_t TargetMotor2Speed;
    uint16_t TargetMotor3Speed;
    uint16_t TargetMotor4Speed;
    
    // Factor de suavizado (ajustable entre 0.0 y 1.0)
    float SmoothingFactor;
    
    // Métodos de suavizado
    uint16_t ApplyLogarithmicSmoothing(uint16_t current, uint16_t target);
    uint16_t ApplyExponentialSmoothing(uint16_t current, uint16_t target);
    
    // Motor pins
    uint8_t Motor1Pin;
    uint8_t Motor2Pin;
    uint8_t Motor3Pin;
    uint8_t Motor4Pin;
    
    // Control variables
    uint16_t Throttle;
    uint16_t SpeedLimit;
    bool RCState;
    bool HeightPIDEnabled;
    
    // Motor speeds
    uint16_t Motor1Speed;
    uint16_t Motor2Speed;
    uint16_t Motor3Speed;
    uint16_t Motor4Speed;
    
    // PID Control
    float RollOutput;
    float PitchOutput;
    float YawOutput;
    
    // Private methods
    void UpdateMotorSpeeds();
    void ApplyMotorSpeeds();
    
public:
    MotorsController(uint8_t motor1Pin, uint8_t motor2Pin, uint8_t motor3Pin, uint8_t motor4Pin);
    
    // Configuration methods
    void Initialize();
    void SetSpeedLimit(uint16_t limit) { SpeedLimit = limit; }
    void SetRCState(bool state) { RCState = state; }
    void SetHeightPIDState(bool state) { HeightPIDEnabled = state; }
    
    // Control methods
    void SetThrottle(uint16_t value);
    void SetPIDOutputs(float roll, float pitch, float yaw);
    void UpdateMotors();
    void StopMotors();

    // Setters
    void SetMotor1Speed(uint16_t speed);
    void SetMotor2Speed(uint16_t speed);
    void SetMotor3Speed(uint16_t speed);
    void SetMotor4Speed(uint16_t speed);
    void SetAllMotorsSpeeds(uint16_t speed);
    void SetIndividualMotorsSpeeds(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
    // Getters
    uint16_t GetMotor1Speed() const { return Motor1Speed; }
    uint16_t GetMotor2Speed() const { return Motor2Speed; }
    uint16_t GetMotor3Speed() const { return Motor3Speed; }
    uint16_t GetMotor4Speed() const { return Motor4Speed; }
};

#endif