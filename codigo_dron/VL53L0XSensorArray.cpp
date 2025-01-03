#include "VL53L0XSensorArray.h"

VL53L0XSensorArray::VL53L0XSensorArray(const uint8_t* xshutPins, 
                                     const uint8_t* i2cAddresses, 
                                     uint8_t sensorCount)
    : SensorCount(min(sensorCount, MAX_SENSORS)) {
    
    Serial.println("Initializing VL53L0X Sensor Array");
    Serial.printf("Configured for %d sensors\n", SensorCount);
    
    Sensors = new Adafruit_VL53L0X[SensorCount];
    XshutPins = new uint8_t[SensorCount];
    I2CAddresses = new uint8_t[SensorCount];
    Measurements = new VL53L0X_RangingMeasurementData_t[SensorCount];
    SensorReadings = new SensorData[SensorCount];
    
    for (uint8_t i = 0; i < SensorCount; i++) {
        XshutPins[i] = xshutPins ? xshutPins[i] : DEFAULT_XSHUT_PINS[i];
        I2CAddresses[i] = i2cAddresses ? i2cAddresses[i] : DEFAULT_I2C_ADDRESSES[i];
        SensorReadings[i] = {0, false};
        Serial.printf("Sensor %d configuration: XSHUT Pin: %d, I2C Address: 0x%02X\n", 
                     i, XshutPins[i], I2CAddresses[i]);
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
    Serial.println("Resetting all VL53L0X sensors...");
    
    for (uint8_t i = 0; i < SensorCount; i++) {
        pinMode(XshutPins[i], OUTPUT);
        digitalWrite(XshutPins[i], LOW);
        Serial.printf("Reset: XSHUT pin %d set to LOW\n", XshutPins[i]);
    }
    delay(10);
}

bool VL53L0XSensorArray::InitializeSensor(uint8_t index) {
    if (index >= SensorCount) {
        Serial.printf("Error: Invalid sensor index %d\n", index);
        return false;
    }
    
    Serial.printf("Initializing VL53L0X sensor %d...\n", index);
    
    digitalWrite(XshutPins[index], HIGH);
    delay(20);
    
    Serial.printf("Attempting to start sensor %d at I2C address 0x%02X\n", index, I2CAddresses[index]);
    
    if (!Sensors[index].begin(I2CAddresses[index])) {
        Serial.printf("Error: Failed to initialize sensor %d\n", index);
        return false;
    }
    
    // Using the correct method name for setting timing budget
    Sensors[index].setMeasurementTimingBudgetMicroSeconds(20000);
    
    Serial.printf("Successfully initialized sensor %d\n", index);
    return true;
}

bool VL53L0XSensorArray::Initialize() {
    Serial.println("\n=== Starting VL53L0X Sensor Array Initialization ===");
    
    ResetSensors();
    
    bool success = true;
    for (uint8_t i = 0; i < SensorCount; i++) {
        if (!InitializeSensor(i)) {
            Serial.printf("Error: Sensor array initialization failed at sensor %d\n", i);
            success = false;
            break;
        }
        delay(50);
    }
    
    if (success) {
        Serial.println("=== Sensor array initialization completed successfully ===\n");
    }
    
    return success;
}

bool VL53L0XSensorArray::ValidateMeasurement(
    const VL53L0X_RangingMeasurementData_t& measurement) const {
    return measurement.RangeStatus != 4;
}

void VL53L0XSensorArray::Update() {
    static unsigned long lastDebugPrint = 0;
    const unsigned long DEBUG_INTERVAL = 1000;
    
    for (uint8_t i = 0; i < SensorCount; i++) {
        Sensors[i].rangingTest(&Measurements[i], false);
        
        if (ValidateMeasurement(Measurements[i])) {
            uint16_t distance = Measurements[i].RangeMilliMeter;
            SensorReadings[i].Distance = min(distance, MAX_DISTANCE);
            SensorReadings[i].IsValid = true;
            
            if (millis() - lastDebugPrint >= DEBUG_INTERVAL) {
                Serial.printf("Sensor %d reading: %d mm\n", i, distance);
            }
        } else {
            SensorReadings[i].IsValid = false;
            if (millis() - lastDebugPrint >= DEBUG_INTERVAL) {
                Serial.printf("Sensor %d: Out of range or invalid reading\n", i);
            }
        }
    }
    
    if (millis() - lastDebugPrint >= DEBUG_INTERVAL) {
        lastDebugPrint = millis();
    }
}

bool VL53L0XSensorArray::PerformDiagnostics() {
    Serial.println("\n=== Running VL53L0X Sensor Diagnostics ===");
    bool criticalSensorsWorking = true;
    
    for (uint8_t i = 0; i < SensorCount; i++) {
        Serial.printf("Testing sensor %d...\n", i);
        
        VL53L0X_RangingMeasurementData_t measurement;
        Sensors[i].rangingTest(&measurement, false);
        
        if (!ValidateMeasurement(measurement)) {
            if (i == static_cast<uint8_t>(TOP)) {
                Serial.println("Notice: Top sensor (4) failed diagnostic test - Non-critical sensor");
            } else {
                Serial.printf("Error: Sensor %d failed diagnostic test\n", i);
                criticalSensorsWorking = false;
            }
        } else {
            Serial.printf("Sensor %d passed - Distance: %d mm\n", 
                         i, measurement.RangeMilliMeter);
        }
    }
    
    Serial.printf("Diagnostic results: %s\n", 
                 criticalSensorsWorking ? "All critical sensors operational" : "One or more critical sensors failed");
    Serial.println("=== Diagnostics Complete ===\n");
    return criticalSensorsWorking;
}
uint16_t VL53L0XSensorArray::GetDistance(SensorPosition position) const {
    if (static_cast<uint8_t>(position) >= SensorCount) {
        Serial.printf("Error: Invalid sensor position %d\n", position);
        return MAX_DISTANCE;
    }
    
    return SensorReadings[static_cast<uint8_t>(position)].Distance;
}

bool VL53L0XSensorArray::IsSensorValid(SensorPosition position) const {
    if (static_cast<uint8_t>(position) >= SensorCount) {
        Serial.printf("Error: Invalid sensor position %d\n", position);
        return false;
    }
    
    return SensorReadings[static_cast<uint8_t>(position)].IsValid;
}