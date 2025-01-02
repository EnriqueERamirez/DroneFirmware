#include "MotorsController.h"

MotorsController::MotorsController(uint8_t motor1Pin, uint8_t motor2Pin, uint8_t motor3Pin, uint8_t motor4Pin)
    : Motor1Pin(motor1Pin), Motor2Pin(motor2Pin), Motor3Pin(motor3Pin), Motor4Pin(motor4Pin),
      Throttle(0), SpeedLimit(750), RCState(false), HeightPIDEnabled(false),
      Motor1Speed(0), Motor2Speed(0), Motor3Speed(0), Motor4Speed(0),
      RollOutput(0), PitchOutput(0), YawOutput(0) {
}

void MotorsController::Initialize() {
    // Initialize LEDC for each motor using the new API
    // The channel parameter is removed as it's now automatically managed
    if (!ledcAttach(Motor1Pin, PWM_FREQUENCY, PWM_RESOLUTION)) {
        Serial.println("Failed to initialize Motor 1");
    }
    if (!ledcAttach(Motor2Pin, PWM_FREQUENCY, PWM_RESOLUTION)) {
        Serial.println("Failed to initialize Motor 2");
    }
    if (!ledcAttach(Motor3Pin, PWM_FREQUENCY, PWM_RESOLUTION)) {
        Serial.println("Failed to initialize Motor 3");
    }
    if (!ledcAttach(Motor4Pin, PWM_FREQUENCY, PWM_RESOLUTION)) {
        Serial.println("Failed to initialize Motor 4");
    }
}

void MotorsController::SetThrottle(uint16_t value) {
    if (RCState) {
        // Map and limit throttle for RC control
        Throttle = map(value, 50, 1024, 0, SpeedLimit);
        Throttle = constrain(Throttle, 0, SpeedLimit);
    } else {
        // Autonomous mode - value used directly
        Throttle = constrain(value, 0, SpeedLimit);
    }
}

void MotorsController::SetPIDOutputs(float roll, float pitch, float yaw) {
    RollOutput = roll;
    PitchOutput = pitch;
    YawOutput = yaw;
}

void MotorsController::UpdateMotorSpeeds() {
    if (Throttle == 0) {
        Motor1Speed = 0;
        Motor2Speed = 0;
        Motor3Speed = 0;
        Motor4Speed = 0;
        return;
    }
    
    // Calculate motor speeds with PID compensation
    // FR - Front Right
    Motor1Speed = constrain(Throttle - RollOutput + PitchOutput - YawOutput, 0, SpeedLimit);
    
    // FL - Front Left
    Motor2Speed = constrain(Throttle + RollOutput + PitchOutput + YawOutput, 0, SpeedLimit);
    
    // RL - Rear Left
    Motor3Speed = constrain(Throttle + RollOutput - PitchOutput - YawOutput, 0, SpeedLimit);
    
    // RR - Rear Right
    Motor4Speed = constrain(Throttle - RollOutput - PitchOutput + YawOutput, 0, SpeedLimit);
}

void MotorsController::ApplyMotorSpeeds() {
    // Use ledcWrite with pin instead of channel
    ledcWrite(Motor1Pin, Motor1Speed);
    ledcWrite(Motor2Pin, Motor2Speed);
    ledcWrite(Motor3Pin, Motor3Speed);
    ledcWrite(Motor4Pin, Motor4Speed);
}

void MotorsController::UpdateMotors() {
    UpdateMotorSpeeds();
    ApplyMotorSpeeds();
}

void MotorsController::StopMotors() {
    Throttle = 0;
    UpdateMotorSpeeds();
    ApplyMotorSpeeds();
}
// ... (código existente) ...

void MotorsController::SetMotor1Speed(uint16_t speed) {
    Motor1Speed = constrain(speed, 0, SpeedLimit);
    ledcWrite(Motor1Pin, Motor1Speed);
}

void MotorsController::SetMotor2Speed(uint16_t speed) {
    Motor2Speed = constrain(speed, 0, SpeedLimit);
    ledcWrite(Motor2Pin, Motor2Speed);
}

void MotorsController::SetMotor3Speed(uint16_t speed) {
    Motor3Speed = constrain(speed, 0, SpeedLimit);
    ledcWrite(Motor3Pin, Motor3Speed);
}

void MotorsController::SetMotor4Speed(uint16_t speed) {
    Motor4Speed = constrain(speed, 0, SpeedLimit);
    ledcWrite(Motor4Pin, Motor4Speed);
}

void MotorsController::SetAllMotorsSpeeds(uint16_t speed) {
    uint16_t constrainedSpeed = constrain(speed, 0, SpeedLimit);
    Motor1Speed = constrainedSpeed;
    Motor2Speed = constrainedSpeed;
    Motor3Speed = constrainedSpeed;
    Motor4Speed = constrainedSpeed;
    
    ledcWrite(Motor1Pin, Motor1Speed);
    ledcWrite(Motor2Pin, Motor2Speed);
    ledcWrite(Motor3Pin, Motor3Speed);
    ledcWrite(Motor4Pin, Motor4Speed);
}

void MotorsController::SetIndividualMotorsSpeeds(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    Motor1Speed = constrain(m1, 0, SpeedLimit);
    Motor2Speed = constrain(m2, 0, SpeedLimit);
    Motor3Speed = constrain(m3, 0, SpeedLimit);
    Motor4Speed = constrain(m4, 0, SpeedLimit);
    
    ledcWrite(Motor1Pin, Motor1Speed);
    ledcWrite(Motor2Pin, Motor2Speed);
    ledcWrite(Motor3Pin, Motor3Speed);
    ledcWrite(Motor4Pin, Motor4Speed);
}