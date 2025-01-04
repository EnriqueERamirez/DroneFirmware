#include "DroneControlTasks.h"

DroneControlTasks* DroneControlTasks::Instance = nullptr;

DroneControlTasks::DroneControlTasks(MPU6050& imu, 
                     DronePID& pidController, 
                     MotorsController& motors,
                     DistanceController& distanceSensor,
                     VL53L0XSensorArray& sensorArray)
    : Imu(imu),
      PidController(pidController),
      Motors(motors),
      DistanceSensor(distanceSensor),
      SensorArray(sensorArray),
      StabilityTaskHandle(nullptr),
      HeightTaskHandle(nullptr) {
    Instance = this;
    Serial.println("DroneControlTasks initialized");
}

void DroneControlTasks::Initialize() {
    Serial.println("Starting task initialization...");
    const uint32_t STABILITY_STACK_SIZE = 4096;
    const uint32_t HEIGHT_STACK_SIZE = 4096;
    
    BaseType_t stabilityTaskCreated = xTaskCreatePinnedToCore(
        StabilityControlTask,
        "StabilityControl",
        STABILITY_STACK_SIZE,
        nullptr,
        2, 
        &StabilityTaskHandle,
        0
    );

    if (stabilityTaskCreated != pdPASS) {
        Serial.println("ERROR: Failed to create stability task");
        Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        return;
    }
    Serial.println("Stability task created successfully on Core 0");
    
    // Pequeña pausa para asegurar la inicialización correcta
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Configuración de la tarea de control de altura
    BaseType_t heightTaskCreated = xTaskCreatePinnedToCore(
        HeightControlTask,
        "HeightControl",
        HEIGHT_STACK_SIZE,
        nullptr,
        1,  // Prioridad más baja
        &HeightTaskHandle,
        1    // Core 1
    );

    if (heightTaskCreated != pdPASS) {
        Serial.println("ERROR: Failed to create height control task");
        Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        if (StabilityTaskHandle != nullptr) {
            vTaskDelete(StabilityTaskHandle);
            StabilityTaskHandle = nullptr;
            Serial.println("Stability task deleted due to height task creation failure");
        }
        return;
    }
    Serial.println("Height control task created successfully on Core 1");
}

void DroneControlTasks::StopTasks() {
    Serial.println("Stopping all tasks...");
    
    if (StabilityTaskHandle != nullptr) {
        vTaskDelete(StabilityTaskHandle);
        StabilityTaskHandle = nullptr;
        Serial.println("Stability task stopped");
    }
    
    if (HeightTaskHandle != nullptr) {
        vTaskDelete(HeightTaskHandle);
        HeightTaskHandle = nullptr;
        Serial.println("Height control task stopped");
    }
    
    Motors.StopMotors();
    Serial.println("Motors stopped");
}

void DroneControlTasks::ProcessStabilityControl() {
    static unsigned long lastDebugOutput = 0;
    const unsigned long DEBUG_INTERVAL = 1000; // Print debug every 1 second
    
    Imu.Update();
    PidController.SetAngles(Imu.GetAngleX(), Imu.GetAngleY());
    PidController.SetGyros(Imu.GetGyroX(), Imu.GetGyroY(), Imu.GetGyroZ());
    
    PidController.UpdateAnglePID();
    PidController.UpdateAngularVelocityPID();
    
    // Debug output every second to avoid flooding Serial
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebugOutput >= DEBUG_INTERVAL) {
        Serial.print("Angles (X/Y): ");
        Serial.print(Imu.GetAngleX());
        Serial.print("/");
        Serial.println(Imu.GetAngleY());
        
        Serial.print("PID Outputs (Roll/Pitch/Yaw): ");
        Serial.print(PidController.GetRollOutput());
        Serial.print("/");
        Serial.print(PidController.GetPitchOutput());
        Serial.print("/");
        Serial.println(PidController.GetYawOutput());
        
        lastDebugOutput = currentMillis;
    }
    
    Motors.SetPIDOutputs(
        PidController.GetRollOutput(),
        PidController.GetPitchOutput(),
        PidController.GetYawOutput()
    );
    
    Motors.UpdateMotors();
}

void DroneControlTasks::ProcessHeightControl() {
    static unsigned long lastDebugOutput = 0;
    const unsigned long DEBUG_INTERVAL = 1000;
    
    // Update all sensor readings
    SensorArray.Update();
    DistanceSensor.SetSensorDistances(SensorArray);
    
    // Get current height from bottom sensor
    float currentHeight = DistanceSensor.GetCurrentHeight();
    
    // Update PID controller with current height
    PidController.SetAltitudeSetpoint(currentHeight);
    PidController.SetAltitude(SensorArray.GetDistance(VL53L0XSensorArray::BOTTOM));
    PidController.UpdateAltitudePID();
    
    // Get and apply height adjustment
    float heightAdjustment = PidController.GetAltitudeOutput();
    DistanceSensor.SetHeightOutput(heightAdjustment);
    
    // Process height adjustments and collision avoidance
    DistanceSensor.UpdateHeightControl();
    //DistanceSensor.UpdateCollisionAvoidance();
    DistanceSensor.ProcessHeightAdjustment();
    
    Motors.SetThrottle(DistanceSensor.GetThrottle());
    
    // Debug output every second
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebugOutput >= DEBUG_INTERVAL) {
        Serial.print("Current Height: ");
        Serial.print(currentHeight);
        Serial.print("mm Target Height: ");
        Serial.print(DistanceSensor.GetHeightTarget());
        Serial.print("mm Throttle: ");
        Serial.println(DistanceSensor.GetThrottle());
        
        if (DistanceSensor.IsCollisionDetected()) {
            Serial.println("Warning: Collision avoidance active!");
        }
        
        lastDebugOutput = currentMillis;
    }
}


void DroneControlTasks::StabilityControlTask(void* parameter) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(4);
    
    Serial.println("Stability control task started");
    
    while (true) {
        unsigned long cycleStartTime = micros();
        
        if (Instance != nullptr) {
            Instance->ProcessStabilityControl();
        }
        
        // Monitoreo del tiempo de ciclo
        unsigned long cycleDuration = micros() - cycleStartTime;
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 5000) {  // Cada 5 segundos
            Serial.printf("Stability cycle time: %lu us\n", cycleDuration);
            Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
            lastPrint = millis();
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void DroneControlTasks::HeightControlTask(void* parameter) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(50);
    
    Serial.println("Height control task started");
    
    while (true) {
        if (Instance != nullptr) {
            Instance->ProcessHeightControl();
        }
        
        // Permitir que otras tareas se ejecuten
        taskYIELD();
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void DroneControlTasks::SetTargetHeight(float heightMm) {
    if (heightMm < 0) {
        Serial.println("Warning: Negative height target requested, setting to 0");
        heightMm = 0;
    }
    const float MAX_HEIGHT_MM = 2000.0f;
    if (heightMm > MAX_HEIGHT_MM) {
        Serial.printf("Warning: Height target exceeds maximum allowed (%0.2f mm), limiting to %0.2f mm\n", 
                     heightMm, MAX_HEIGHT_MM);
        heightMm = MAX_HEIGHT_MM;
    }
    
    DistanceSensor.SetHeightTarget(heightMm);
    Serial.printf("New target height set: %0.2f mm\n", heightMm);
}