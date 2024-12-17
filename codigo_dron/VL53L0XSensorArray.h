#ifndef VL53L0X_SENSOR_ARRAY_H
#define VL53L0X_SENSOR_ARRAY_H

#include <Arduino.h>
#include "Adafruit_VL53L0X.h"

class VL53L0XSensorArray {
private:
    static const uint8_t MAX_SENSORS = 6;
    
    // Default configuration constants
    static constexpr uint8_t DEFAULT_I2C_ADDRESSES[MAX_SENSORS] = {0x34, 0x31, 0x32, 0x33, 0x35, 0x36};
    static constexpr uint8_t DEFAULT_XSHUT_PINS[MAX_SENSORS] = {17, 18, 35, 37, 41, 43};
    static const uint8_t RANGE_OK_STATUS = 6;
    
    struct SensorData {
        uint16_t Distance;
        bool IsValid;
    };
    
    // Componentes del sensor
    Adafruit_VL53L0X* Sensors;
    uint8_t* XshutPins;
    uint8_t* I2CAddresses;
    uint8_t SensorCount;
    
    // Datos de medición
    VL53L0X_RangingMeasurementData_t* Measurements;
    SensorData* SensorReadings;
    
    // Constantes
    static const uint16_t MAX_DISTANCE = 1100;
    static const uint8_t VALID_RANGE_STATUS = 0;
    
    // Métodos privados
    void ResetSensors();
    bool InitializeSensor(uint8_t index);
    bool ValidateMeasurement(const VL53L0X_RangingMeasurementData_t& measurement) const;
    
public:
    // Enumeración para identificar sensores
    enum SensorPosition {
        LEFT = 0,
        BACK = 1,
        RIGHT = 2,
        FRONT = 3,
        TOP = 4,
        BOTTOM = 5
    };
    
    // Constructor with default values
    VL53L0XSensorArray(const uint8_t* xshutPins = DEFAULT_XSHUT_PINS,
                       const uint8_t* i2cAddresses = DEFAULT_I2C_ADDRESSES,
                       uint8_t sensorCount = MAX_SENSORS);
    ~VL53L0XSensorArray();
    
    bool Initialize();
    void Update();
    
    // Métodos para obtener lecturas
    uint16_t GetDistance(SensorPosition position) const;
    bool IsSensorValid(SensorPosition position) const;
    
    // Método de diagnóstico
    bool PerformDiagnostics();
};

#endif