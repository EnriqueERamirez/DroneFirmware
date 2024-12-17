#include <Arduino.h>
#include <Wire.h>

// Custom class headers
#include "MPU6050.h"
#include "DronePID.h"
#include "MotorsController.h"
#include "DistanceController.h"
#include "VL53L0XSensorArray.h"
#include "LEDController.h"
#include "MelodyPlayer.h"
#include "BatteryMonitor.h"
#include "DroneControlTasks.h"

// System components using default configurations
MPU6050 imu;
DronePID pidController;
MotorsController motors(39, 4, 14, 47);  // Default motor pins from MotorsController.h
DistanceController distanceController;
VL53L0XSensorArray sensors;  // Using default pins and addresses
LEDController leds;          // Using default LED configuration
MelodyPlayer buzzer;         // Using default buzzer pin
BatteryMonitor battery(1);  // Using default analog pin for battery monitoring
DroneControlTasks* taskController = nullptr;

// Constants
const float MINIMUM_VOLTAGE = 3.7f;  // Minimum voltage threshold

// Test sequence states
enum TestState {
    INIT,
    SENSOR_TEST,
    MOTOR_TEST,
    FLIGHT_PREP,
    TEST_FLIGHT,
    LANDING,
    COMPLETED
};

TestState currentState = INIT;
unsigned long stateTimer = 0;
unsigned long flightTimer = 0;

void setup() {
    delay(1000);
    Serial.begin(115200);
    Wire.setClock(400000);  // 400kHz I2C clock
    
    // Initialize LED controller and start initialization pattern
    leds.Initialize();
    leds.RunStartupSequence();
    
    // Initialize IMU with calibration
    imu.Initialize();
    Serial.println("Calibrating IMU...");
    imu.Calibrate();
    
    if (!imu.IsDeviceCalibrated()) {
        Serial.println("IMU calibration failed");
        playFailureSequence();
        while(1);
    }
    
    // Initialize distance sensors
    if (!sensors.Initialize()) {
        Serial.println("Distance sensors initialization failed");
        playFailureSequence();
        while(1);
    }
    
    // Initialize remaining components
    motors.Initialize();
    battery.Initialize();
    
    // Create and initialize task controller
    taskController = new DroneControlTasks(imu, pidController, motors, distanceController);
    
    Serial.println("System initialized, starting test sequence...");
    currentState = SENSOR_TEST;
}

void loop() {
    static uint16_t targetHeight = 100; // Target height in mm
    
    // Update battery status first
    battery.Update();
    
    // Check for low voltage condition
    if (battery.IsLowVoltage()) {
        motors.StopMotors();
        Serial.println("Low battery detected!");
        playFailureSequence();
        while(1);
    }
    
    switch (currentState) {
        case SENSOR_TEST:
            if (performSensorTest()) {
                playSuccessSequence();
                currentState = MOTOR_TEST;
            } else {
                playFailureSequence();
                while(1);
            }
            break;
            
        case MOTOR_TEST:
            if (performMotorTest()) {
                playSuccessSequence();
                currentState = FLIGHT_PREP;
                stateTimer = millis();
            } else {
                playFailureSequence();
                while(1);
            }
            break;
            
        case FLIGHT_PREP:
            if (millis() - stateTimer >= 5000) { // 5 second wait
                playTakeoffWarning();
                currentState = TEST_FLIGHT;
                flightTimer = millis();
                taskController->Initialize();
            }
            break;
            
        case TEST_FLIGHT:
            if (millis() - flightTimer <= 30000) { // 30 second flight
                distanceController.SetHeightTarget(targetHeight);
            } else {
                currentState = LANDING;
            }
            break;
            
        case LANDING:
            if (targetHeight > 0) {
                targetHeight = max(0, targetHeight - 2);  // Gradual descent
                distanceController.SetHeightTarget(targetHeight);
            } else {
                taskController->StopTasks();
                motors.StopMotors();
                currentState = COMPLETED;
                playSuccessSequence();
            }
            break;
            
        case COMPLETED:
            leds.UpdateRandomPattern();
            break;
    }
    
    // Regular system updates
    sensors.Update();
    leds.UpdateRandomPattern();
}

bool performSensorTest() {
    // Comprehensive sensor check
    return sensors.PerformDiagnostics() && 
           imu.IsDeviceCalibrated() && 
           battery.GetCurrentVoltage() > MINIMUM_VOLTAGE;
}

bool performMotorTest() {
    const uint16_t testPower = 100;
    const unsigned long testDuration = 1000;
    
    // Test each motor sequentially
    motors.SetThrottle(testPower);
    
    for (int i = 0; i < 4; i++) {
        motors.UpdateMotors();
        delay(testDuration);
        motors.SetThrottle(0);
        motors.UpdateMotors();
        delay(500);
    }
    
    return true;
}

void playSuccessSequence() {
    buzzer.PlayNote(buzzer.GetNoteC5(), buzzer.GetQuarterNote());
    delay(100);
    buzzer.PlayNote(buzzer.GetNoteE4(), buzzer.GetQuarterNote());
    delay(100);
    buzzer.PlayNote(buzzer.GetNoteG4(), buzzer.GetHalfNote());
}

void playFailureSequence() {
    buzzer.PlayNote(buzzer.GetNoteC4(), buzzer.GetHalfNote());
    delay(200);
    buzzer.PlayNote(buzzer.GetNoteC4(), buzzer.GetWholeNote());
}

void playTakeoffWarning() {
    for (int i = 0; i < 3; i++) {
        buzzer.PlayNote(buzzer.GetNoteA4(), buzzer.GetQuarterNote());
        delay(200);
    }
}