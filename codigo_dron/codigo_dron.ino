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
const float MINIMUM_VOLTAGE = 3.2f;  // Minimum voltage threshold
const uint16_t INITIAL_HEIGHT = 250;  // Initial target height in millimeters
const uint16_t HEIGHT_INCREMENT = 10; // Height change increment in millimeters
const uint16_t MAX_HEIGHT = 500;     // Maximum allowed height in millimeters

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

// Function to convert state to string for debugging
String getStateName(TestState state) {
    switch(state) {
        case INIT: return "INIT";
        case SENSOR_TEST: return "SENSOR_TEST";
        case MOTOR_TEST: return "MOTOR_TEST";
        case FLIGHT_PREP: return "FLIGHT_PREP";
        case TEST_FLIGHT: return "TEST_FLIGHT";
        case LANDING: return "LANDING";
        case COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}
void setup() {
    delay(1000);
    //disableCore0WDT();
    Serial.begin(115200);
    delay(1000);  // Breve pausa para asegurar una inicialización estable
    Serial.println("\n=== Drone System Initialization Starting ===");
    Serial.printf("ESP32-S3 Free Heap: %d\n", ESP.getFreeHeap());
    Wire.setClock(400000);  // 400kHz I2C clock
    Serial.println("I2C clock set to 400kHz");
    
    // Initialize LED controller and start initialization pattern
    Serial.println("Initializing LED controller...");
    leds.Initialize();
    leds.RunStartupSequence();
    
    // Initialize IMU with calibration
    Serial.println("Initializing IMU...");
    imu.Initialize();
    Serial.println("Starting IMU calibration...");
    imu.Calibrate();
    
    if (!imu.IsDeviceCalibrated()) {
        Serial.println("ERROR: IMU calibration failed!");
        playFailureSequence();
        while(1) {
            Serial.println("System halted due to IMU calibration failure");
            delay(5000);
        }
    }
    Serial.println("IMU calibration successful");
    
    // Initialize distance sensors
    Serial.println("Initializing distance sensors...");
    if (!sensors.Initialize()) {
        Serial.println("ERROR: Distance sensors initialization failed!");
        playFailureSequence();
        while(1) {
            Serial.println("System halted due to sensor initialization failure");
            delay(5000);
        }
    }
    Serial.println("Distance sensors initialized successfully");
    
    // Initialize remaining components
    Serial.println("Initializing motors...");
    motors.Initialize();
    Serial.println("Initializing battery monitor...");
    battery.Initialize();
    
    // Create and initialize task controller
    Serial.println("Creating task controller...");
    taskController = new DroneControlTasks(imu, pidController, motors, distanceController, sensors);
    
    Serial.println("=== System initialization complete ===");
    Serial.println("Starting test sequence...");
    currentState = SENSOR_TEST;
}

void loop() {
    static uint16_t targetHeight = 0; // Target height in mm
    static unsigned long lastDebugPrint = 0;
    static unsigned long remainingTime = 0;  // Moved the declaration here
    static unsigned long heightUpdateTimer = 0;
    // Update battery status first
    battery.Update();
    float currentVoltage = battery.GetCurrentVoltage();
    
    // Print debug info every second
    if (millis() - lastDebugPrint >= 1000) {
        Serial.printf("Current State: %s, Battery: %.2fV\n", 
                     getStateName(currentState).c_str(), 
                     currentVoltage);
        lastDebugPrint = millis();
    }
    
    // Check for low voltage condition
    //if (battery.IsLowVoltage()) {
    //    Serial.printf("ERROR: Low battery detected! Voltage: %.2fV\n", currentVoltage);
    //    motors.StopMotors();
    //    playFailureSequence();
    //    while(1) {
    //        Serial.println("System halted due to low battery");
    //        delay(5000);
    //    }
    //}
    
    switch (currentState) {
        case SENSOR_TEST:
            Serial.println("Running sensor diagnostics...");
            if (performSensorTest()) {
                Serial.println("Sensor tests passed successfully");
                playSuccessSequence();
                currentState = MOTOR_TEST;
            } else {
                Serial.println("ERROR: Sensor tests failed!");
                playFailureSequence();
                while(1) {
                    Serial.println("System halted due to sensor test failure");
                    delay(5000);
                }
            }
            break;
            
        case MOTOR_TEST:
            Serial.println("Starting motor test sequence...");
            if (performMotorTest()) {
                Serial.println("Motor tests completed successfully");
                playSuccessSequence();
                currentState = FLIGHT_PREP;
                stateTimer = millis();
            } else {
                Serial.println("ERROR: Motor tests failed!");
                playFailureSequence();
                while(1) {
                    Serial.println("System halted due to motor test failure");
                    delay(5000);
                }
            }
            break;
            
        case FLIGHT_PREP:
            remainingTime = (millis() - stateTimer >= 5000) ? 0 : (5000 - (millis() - stateTimer));
            if (remainingTime == 0) {
                Serial.println("Flight preparation complete, initiating takeoff sequence");
                playTakeoffWarning();
                currentState = TEST_FLIGHT;
                flightTimer = millis();
                taskController->Initialize();
            } else {
                Serial.printf("Flight preparation in progress - Time remaining: %d seconds\n", 
                            remainingTime / 1000);
            }
            break; 
        case TEST_FLIGHT:
            if (millis() - flightTimer <= 30000) {  // 30-second flight test
                // Initial takeoff
                if (targetHeight < INITIAL_HEIGHT) {
                    targetHeight = INITIAL_HEIGHT;
                    taskController->SetTargetHeight(targetHeight);
                    Serial.printf("Taking off - Setting height to %d mm\n", targetHeight);
                }
                
                // Update height every 5 seconds for testing
                if (millis() - heightUpdateTimer >= 5000) {
                    // Optional: Add height variations during flight for testing
                    if (targetHeight == INITIAL_HEIGHT) {
                        targetHeight += HEIGHT_INCREMENT;
                    } else {
                        targetHeight = INITIAL_HEIGHT;
                    }
                    
                    // Ensure we don't exceed maximum height
                    targetHeight = min(targetHeight, MAX_HEIGHT);
                    taskController->SetTargetHeight(targetHeight);
                    Serial.printf("Updating flight height - New target: %d mm\n", targetHeight);
                    heightUpdateTimer = millis();
                }
                
                // Debug output
                if (millis() - lastDebugPrint >= 1000) {
                    Serial.printf("Flight in progress - Target height: %d mm, Current height: %.2f mm\n",
                                targetHeight, taskController->GetCurrentHeight());
                    lastDebugPrint = millis();
                }
            } else {
                Serial.println("Flight time complete, initiating landing sequence");
                currentState = LANDING;
            }
            break;
            
        case LANDING:
            if (targetHeight > 0) {
                targetHeight = max(0, targetHeight - 5);  // More gradual descent
                taskController->SetTargetHeight(targetHeight);
                
                if (millis() - lastDebugPrint >= 1000) {
                    Serial.printf("Landing in progress - Target: %d mm, Current: %.2f mm\n",
                                targetHeight, taskController->GetCurrentHeight());
                    lastDebugPrint = millis();
                }
            } else {
                Serial.println("Landing complete, stopping motors");
                taskController->StopTasks();
                motors.StopMotors();
                currentState = COMPLETED;
                playSuccessSequence();
            }
            break;
            
        case COMPLETED:
            // No need for continuous printing in completed state
            leds.UpdateRandomPattern();
            break;
    }
    
    // Regular system updates
    sensors.Update();
    leds.UpdateRandomPattern();
}

bool performSensorTest() {
    Serial.println("Testing sensors:");
    
    bool sensorsDiagnostic = sensors.PerformDiagnostics();
    Serial.printf("- Distance sensors diagnostic: %s\n", sensorsDiagnostic ? "PASS" : "FAIL");
    
    bool imuCalibrated = imu.IsDeviceCalibrated();
    Serial.printf("- IMU calibration check: %s\n", imuCalibrated ? "PASS" : "FAIL");
    
    float voltage = battery.GetCurrentVoltage();
    bool voltageOk = voltage > MINIMUM_VOLTAGE;
    Serial.printf("- Battery voltage (%.2fV): %s\n", voltage, voltageOk ? "PASS" : "FAIL");
    
    return sensorsDiagnostic && imuCalibrated && voltageOk;
}

bool performMotorTest() {
    const uint16_t MAX_TEST_POWER = 100; 
    const uint16_t POWER_STEP = 25;    
    const unsigned long STEP_DURATION = 500;
    
    Serial.println("Starting progressive motor test...");
    
    // Test cada motor individualmente
    for (int motor = 0; motor < 4; motor++) {
        Serial.printf("\nTesting motor %d with progressive power:\n", motor + 1);
        
        // Incremento progresivo de potencia
        for (uint16_t power = 0; power <= MAX_TEST_POWER; power += POWER_STEP) {
            Serial.printf("Motor %d at %d%% power\n", motor + 1, (power * 100) / 750);
            
            // Apagar todos los motores primero
            motors.SetAllMotorsSpeeds(0);
            
            // Aplicar potencia al motor específico
            switch(motor) {
                case 0:
                    motors.SetMotor1Speed(power);
                    break;
                case 1:
                    motors.SetMotor2Speed(power);
                    break;
                case 2:
                    motors.SetMotor3Speed(power);
                    break;
                case 3:
                    motors.SetMotor4Speed(power);
                    break;
            }
            
            delay(STEP_DURATION);
        }
        
        // Apagar el motor y esperar
        motors.SetAllMotorsSpeeds(0);
        delay(1000);
    }
    Serial.println("\nTesting all motors simultaneously...");
    for (uint16_t power = 0; power <= MAX_TEST_POWER; power += POWER_STEP) {
        Serial.printf("All motors at %d%% power\n", (power * 100) / 750);
        motors.SetAllMotorsSpeeds(power);
        delay(STEP_DURATION);
    }
    
    // Apagar todos los motores
    Serial.println("Motor test sequence completed");
    motors.StopMotors();
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