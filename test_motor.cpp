#include <Arduino.h>
#include "MotorsController.h"

MotorsController motors(39, 4, 14, 47);
const uint16_t SPEED = 100; // 80% de 750

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Iniciando prueba de motores...");
    motors.Initialize();
    motors.SetSpeedLimit(750);
}

void loop() {
    // Ejemplo de uso de las nuevas funciones
    
    // Controlar un solo motor
    Serial.println("Controlando motor 1");
    motors.SetMotor1Speed(SPEED);
    delay(5000);
    Serial.println("Controlando motor 2");
    motors.SetMotor2Speed(SPEED);
    delay(5000);
    Serial.println("Controlando motor 3");
    motors.SetMotor3Speed(SPEED);
    delay(5000);
    Serial.println("Controlando motor 4");
    motors.SetMotor4Speed(SPEED);
    
    Serial.println("Todos los motores a la misma velocidad");
    // Controlar todos los motores a la misma velocidad
    //motors.SetAllMotorsSpeeds(SPEED);
    //delay(2000);
  
    delay(20000);
    
    // Detener todos los motores
    Serial.println("Deteniendo motores...");
    motors.StopMotors();
    delay(10000);
}