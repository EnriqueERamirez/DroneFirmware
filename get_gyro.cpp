#include <Arduino.h>
#include "MotorsController.h"
#include "MPU6050.h"

MPU6050 imu;
MotorsController motors(39, 4, 14, 47);
const uint16_t SPEED = 650;

unsigned long lastPrint = 0;
const unsigned long PRINT_INTERVAL = 0.5;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Wire.begin();
    imu.Initialize();
    imu.Calibrate();
    imu.SetCycleTime(4000);
    
    motors.Initialize();
    motors.SetSpeedLimit(750);
    
    // Encender motores
    motors.SetMotor1Speed(SPEED);
    delay(5000);
    motors.SetMotor2Speed(SPEED);
    delay(5000);
    motors.SetMotor3Speed(SPEED);
    
    Serial.println("GyroX,GyroY,GyroZ,AngleX,AngleY,AngleZ");
}

void loop() {
    if (millis() - lastPrint >= PRINT_INTERVAL) {
        imu.Update();
        Serial.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                     imu.GetGyroX(),
                     imu.GetGyroY(),
                     imu.GetGyroZ(),
                     imu.GetAngleX(),
                     imu.GetAngleY(),
                     imu.GetAngleZ());
        lastPrint = millis();
    }
}