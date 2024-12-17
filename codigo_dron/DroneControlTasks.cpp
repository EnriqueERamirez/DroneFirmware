#include "DroneControlTasks.h"

// Inicialización del puntero estático
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
}

void DroneControlTasks::Initialize() {
    // Configuración de la tarea de estabilidad con alta prioridad en Core 0
    BaseType_t stabilityTaskCreated = xTaskCreatePinnedToCore(
        StabilityControlTask,
        "StabilityControl",
        5000,
        nullptr,
        3,
        &StabilityTaskHandle,
        0
    );

    if (stabilityTaskCreated != pdPASS) {
        // Manejar error de creación de tarea
        return;
    }
    
    // Configuración de la tarea de control de altura con baja prioridad en Core 1
    BaseType_t heightTaskCreated = xTaskCreatePinnedToCore(
        HeightControlTask,
        "HeightControl",
        5000,
        nullptr,
        0,
        &HeightTaskHandle,
        1
    );

    if (heightTaskCreated != pdPASS) {
        // Si falla la creación de la tarea de altura, eliminar la tarea de estabilidad
        if (StabilityTaskHandle != nullptr) {
            vTaskDelete(StabilityTaskHandle);
            StabilityTaskHandle = nullptr;
        }
        return;
    }
}

void DroneControlTasks::StopTasks() {
    // Detener tareas de manera segura
    if (StabilityTaskHandle != nullptr) {
        vTaskDelete(StabilityTaskHandle);
        StabilityTaskHandle = nullptr;
    }
    
    if (HeightTaskHandle != nullptr) {
        vTaskDelete(HeightTaskHandle);
        HeightTaskHandle = nullptr;
    }
    
    // Asegurarse de que los motores se detengan
    Motors.StopMotors();
}

void DroneControlTasks::ProcessStabilityControl() {
    // Actualizar lecturas del sensor IMU
    Imu.Update();
    
    // Actualizar valores para el control PID
    PidController.SetAngles(Imu.GetAngleX(), Imu.GetAngleY());
    PidController.SetGyros(Imu.GetGyroX(), Imu.GetGyroY(), Imu.GetGyroZ());
    
    // Ejecutar cálculos PID
    PidController.UpdateAnglePID();
    PidController.UpdateAngularVelocityPID();
    
    // Aplicar salidas PID a los motores
    Motors.SetPIDOutputs(
        PidController.GetRollOutput(),
        PidController.GetPitchOutput(),
        PidController.GetYawOutput()
    );
    
    // Actualizar velocidades de los motores
    Motors.UpdateMotors();
}

void DroneControlTasks::ProcessHeightControl() {
    // Procesar control de altura y evitación de colisiones
    DistanceSensor.UpdateHeightControl();
    DistanceSensor.UpdateCollisionAvoidance();
    DistanceSensor.ProcessHeightAdjustment();
    
    // Actualizar el throttle basado en el control de altura
    Motors.SetThrottle(DistanceSensor.GetThrottle());
    
    // Actualizar objetivos de orientación para evitar colisiones
    if (DistanceSensor.IsCollisionDetected()) {
        PidController.SetSetpoints(
            DistanceSensor.GetRollTarget(),
            DistanceSensor.GetPitchTarget(),
            0.0f  // Mantener el yaw actual
        );
    }
}

void DroneControlTasks::StabilityControlTask(void* parameter) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(4); // 4ms para estabilidad
    
    while (true) {
        if (Instance != nullptr) {
            Instance->ProcessStabilityControl();
        }
        
        // Esperar hasta el siguiente ciclo con timing preciso
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void DroneControlTasks::HeightControlTask(void* parameter) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // 50ms para control de altura
    
    while (true) {
        if (Instance != nullptr) {
            Instance->ProcessHeightControl();
        }
        
        // Esperar hasta el siguiente ciclo con timing preciso
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}