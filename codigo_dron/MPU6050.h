#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Wire.h>

class MPU6050 {
private:
    // Dirección I2C del MPU6050
    const uint8_t MPU6050Address = 0x68;
    #define MPU_RAD_TO_DEG 57.295779f
    // Variables para datos crudos
    int16_t AccXRaw, AccYRaw, AccZRaw;
    int16_t GyroXRaw, GyroYRaw, GyroZRaw;
    int16_t Temperature;
    
    // Variables de calibración
    int32_t GyroXCal, GyroYCal, GyroZCal;
    int32_t AccXCal, AccYCal, AccZCal;
    
    // Variables procesadas
    float GyroX, GyroY, GyroZ;
    float AngleX, AngleY, AngleZ;
    float AngleXAcc, AngleYAcc;
    float AccXG, AccYG;
    
    // Variables de control
    bool IsCalibrated;
    float CycleTimeUs;
    
    // Métodos privados
    void ReadRawData();
    void ProcessData();
    
public:
    MPU6050();
    
    // Métodos de configuración
    void Initialize();
    void Calibrate();
    void SetCycleTime(float microseconds) { CycleTimeUs = microseconds; }
    
    // Métodos de actualización y obtención de datos
    void Update();
    
    // Getters
    float GetGyroX() const { return GyroX; }
    float GetGyroY() const { return GyroY; }
    float GetGyroZ() const { return GyroZ; }
    float GetAngleX() const { return AngleX; }
    float GetAngleY() const { return AngleY; }
    float GetAngleZ() const { return AngleZ; }
    float GetAccXG() const { return AccXG; }
    float GetAccYG() const { return AccYG; }
    bool IsDeviceCalibrated() const { return IsCalibrated; }
};

#endif