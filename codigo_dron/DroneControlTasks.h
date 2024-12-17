#ifndef DRONE_CONTROL_TASKS_H
#define DRONE_CONTROL_TASKS_H

#include <Arduino.h>
#include "MPU6050.h"
#include "DronePID.h"
#include "MotorsController.h"
#include "DistanceController.h"

class DroneControlTasks {
private:
    // Componentes del sistema
    MPU6050& Imu;
    DronePID& PidController;
    MotorsController& Motors;
    DistanceController& DistanceSensor;
    
    // Handles de tareas
    TaskHandle_t StabilityTaskHandle;
    TaskHandle_t HeightTaskHandle;
    
    // Métodos estáticos para las tareas
    static void StabilityControlTask(void* parameter);
    static void HeightControlTask(void* parameter);
    
    // Puntero a la instancia para callbacks
    static DroneControlTasks* Instance;
    
public:
    DroneControlTasks(MPU6050& imu, 
                     DronePID& pidController, 
                     MotorsController& motors,
                     DistanceController& distanceSensor);
    
    void Initialize();
    void StopTasks();
    
    // Métodos de control interno
    void ProcessStabilityControl();
    void ProcessHeightControl();
};

#endif