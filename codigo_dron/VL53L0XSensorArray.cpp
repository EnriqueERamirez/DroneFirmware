#include "VL53L0XSensorArray.h"

VL53L0XSensorArray::VL53L0XSensorArray(const uint8_t* xshutPins, 
                                     const uint8_t* i2cAddresses, 
                                     uint8_t sensorCount)
    : SensorCount(min(sensorCount, MAX_SENSORS)) {
    
    // Asignar memoria para los arrays
    Sensors = new Adafruit_VL53L0X[SensorCount];
    XshutPins = new uint8_t[SensorCount];
    I2CAddresses = new uint8_t[SensorCount];
    Measurements = new VL53L0X_RangingMeasurementData_t[SensorCount];
    SensorReadings = new SensorData[SensorCount];
    
    // Copiar configuraciones usando los valores por defecto si no se proporcionan
    for (uint8_t i = 0; i < SensorCount; i++) {
        XshutPins[i] = xshutPins ? xshutPins[i] : DEFAULT_XSHUT_PINS[i];
        I2CAddresses[i] = i2cAddresses ? i2cAddresses[i] : DEFAULT_I2C_ADDRESSES[i];
        SensorReadings[i] = {0, false};
    }
}

VL53L0XSensorArray::~VL53L0XSensorArray() {
    delete[] Sensors;
    delete[] XshutPins;
    delete[] I2CAddresses;
    delete[] Measurements;
    delete[] SensorReadings;
}

void VL53L0XSensorArray::ResetSensors() {
    // Establecer todos los pines XSHUT en LOW
    for (uint8_t i = 0; i < SensorCount; i++) {
        pinMode(XshutPins[i], OUTPUT);
        digitalWrite(XshutPins[i], LOW);
    }
    delay(10);
}

bool VL53L0XSensorArray::InitializeSensor(uint8_t index) {
    if (index >= SensorCount) return false;
    
    digitalWrite(XshutPins[index], HIGH);
    if (index > 0) delay(10);
    
    if (!Sensors[index].begin(I2CAddresses[index])) {
        return false;
    }
    
    return true;
}

bool VL53L0XSensorArray::Initialize() {
    ResetSensors();
    
    // Inicializar cada sensor secuencialmente
    for (uint8_t i = 0; i < SensorCount; i++) {
        if (!InitializeSensor(i)) {
            return false;
        }
    }
    
    return true;
}

bool VL53L0XSensorArray::ValidateMeasurement(
    const VL53L0X_RangingMeasurementData_t& measurement) const {
    return measurement.RangeStatus == VALID_RANGE_STATUS;
}

void VL53L0XSensorArray::Update() {
    for (uint8_t i = 0; i < SensorCount; i++) {
        // Realizar medición
        Sensors[i].rangingTest(&Measurements[i], false);
        
        // Validar y procesar la medición
        if (ValidateMeasurement(Measurements[i])) {
            uint16_t distance = Measurements[i].RangeMilliMeter;
            SensorReadings[i].Distance = min(distance, MAX_DISTANCE);
            SensorReadings[i].IsValid = true;
        } else {
            SensorReadings[i].IsValid = false;
        }
    }
}

uint16_t VL53L0XSensorArray::GetDistance(SensorPosition position) const {
    if (static_cast<uint8_t>(position) >= SensorCount) {
        return MAX_DISTANCE;
    }
    return SensorReadings[position].Distance;
}

bool VL53L0XSensorArray::IsSensorValid(SensorPosition position) const {
    if (static_cast<uint8_t>(position) >= SensorCount) {
        return false;
    }
    return SensorReadings[position].IsValid;
}

bool VL53L0XSensorArray::PerformDiagnostics() {
    bool allSensorsWorking = true;
    
    for (uint8_t i = 0; i < SensorCount; i++) {
        VL53L0X_RangingMeasurementData_t measurement;
        Sensors[i].rangingTest(&measurement, false);
        
        if (!ValidateMeasurement(measurement)) {
            allSensorsWorking = false;
        }
    }
    
    return allSensorsWorking;
}