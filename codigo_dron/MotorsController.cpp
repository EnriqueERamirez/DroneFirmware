#include "MotorsController.h"

MotorsController::MotorsController(uint8_t motor1Pin, uint8_t motor2Pin, uint8_t motor3Pin, uint8_t motor4Pin)
    : Motor1Pin(motor1Pin), Motor2Pin(motor2Pin), Motor3Pin(motor3Pin), Motor4Pin(motor4Pin),
      Throttle(0), SpeedLimit(750), RCState(false), HeightPIDEnabled(false),
      Motor1Speed(0), Motor2Speed(0), Motor3Speed(0), Motor4Speed(0),
      RollOutput(0), PitchOutput(0), YawOutput(0) {
}

void MotorsController::Initialize() {
    // Configurar canales PWM
    for (int channel = 0; channel < PWM_CHANNELS; ++channel) {
        ledcSetup(channel, PWM_FREQUENCY, PWM_RESOLUTION);
    }
    
    // Asignar pines a canales
    ledcAttachPin(Motor1Pin, 39);
    ledcAttachPin(Motor2Pin, 4);
    ledcAttachPin(Motor3Pin, 14);
    ledcAttachPin(Motor4Pin, 47);
}

void MotorsController::SetThrottle(uint16_t value) {
    if (RCState) {
        // Mapear y limitar el throttle para control RC
        Throttle = map(value, 50, 1024, 0, SpeedLimit);
        Throttle = constrain(Throttle, 0, SpeedLimit);
    } else {
        // Modo autónomo - el valor se usa directamente
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
    
    // Calcular velocidades de los motores con compensación PID
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
    ledcWrite(0, Motor1Speed);
    ledcWrite(1, Motor2Speed);
    ledcWrite(2, Motor3Speed);
    ledcWrite(3, Motor4Speed);
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