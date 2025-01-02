#include "DroneControlTasks.h"

DroneControlTasks* DroneControlTasks::Instance = nullptr;

DroneControlTasks::DroneControlTasks(MPU6050& imu, 
                                   DronePID& pidController, 
                                   MotorsController& motors,
                                   DistanceController& distanceSensor)
    : Imu(imu),
      PidController(pidController),
      Motors(motors),
      DistanceSensor(distanceSensor),
      StabilityTaskHandle(nullptr),
      HeightTaskHandle(nullptr) {
    Instance = this;
    Serial.begin(115200);  // Initialize Serial with high baud rate
    Serial.println("DroneControlTasks initialized");
}

void DroneControlTasks::Initialize() {
    Serial.println("Starting task initialization...");
    
    // Configuración específica para ESP32-S3
    const uint32_t STABILITY_STACK_SIZE = 4096;  // Ajustado para ESP32-S3
    const uint32_t HEIGHT_STACK_SIZE = 4096;     // Ajustado para ESP32-S3
    
    // Configuración de la tarea de estabilidad
    BaseType_t stabilityTaskCreated = xTaskCreatePinnedToCore(
        StabilityControlTask,
        "StabilityControl",
        STABILITY_STACK_SIZE,
        nullptr,
        2,  // Prioridad ajustada para ESP32-S3
        &StabilityTaskHandle,
        0    // Core 0
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
    
    // Monitoreo de memoria inicial
    Serial.printf("Initial free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("Largest free block: %d\n", ESP.getMaxAllocHeap());
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
    const unsigned long DEBUG_INTERVAL = 1000; // Print debug every 1 second
    
    DistanceSensor.UpdateHeightControl();
    DistanceSensor.UpdateCollisionAvoidance();
    DistanceSensor.ProcessHeightAdjustment();
    
    Motors.SetThrottle(DistanceSensor.GetThrottle());
    
    // Debug output every second
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebugOutput >= DEBUG_INTERVAL) {
        Serial.print("Throttle: ");
        Serial.println(DistanceSensor.GetThrottle());
        
        if (DistanceSensor.IsCollisionDetected()) {
            Serial.println("Collision avoidance active!");
        }
        
        lastDebugOutput = currentMillis;
    }
    
    if (DistanceSensor.IsCollisionDetected()) {
        float rollTarget = DistanceSensor.GetRollTarget();
        float pitchTarget = DistanceSensor.GetPitchTarget();
        
        PidController.SetSetpoints(rollTarget, pitchTarget, 0.0f);
        
        // Debug inmediato cuando cambian los setpoints
        Serial.print("Setting new PID setpoints - Roll: ");
        Serial.print(rollTarget);
        Serial.print(", Pitch: ");
        Serial.println(pitchTarget);
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