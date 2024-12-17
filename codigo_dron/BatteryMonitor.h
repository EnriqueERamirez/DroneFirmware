#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

class BatteryMonitor {
private:
    // Pines y configuración
    uint8_t BatteryPin;
    uint8_t RGBLedPin;
    
    // Parámetros del divisor de voltaje
    float InputVoltage;
    float R1Value;
    float R2Value;
    
    // Umbrales y estados
    float LowVoltageThreshold;
    float CurrentVoltage;
    
    // Métodos privados
    float CalculateVoltage(int reading) const;
    void UpdateLEDStatus();
    
public:
    BatteryMonitor(uint8_t batteryPin, 
                   uint8_t rgbLedPin = RGB_BUILTIN,
                   float inputVoltage = 3.25f,
                   float r1 = 20000.0f,
                   float r2 = 50000.0f,
                   float lowVoltageThreshold = 3.7f);
    
    void Initialize();
    void Update();
    
    // Getters
    float GetCurrentVoltage() const { return CurrentVoltage; }
    bool IsLowVoltage() const { return CurrentVoltage < LowVoltageThreshold; }
    
    // Setters
    void SetLowVoltageThreshold(float threshold) { LowVoltageThreshold = threshold; }
    void SetVoltageParameters(float inputVoltage, float r1, float r2);
};

#endif