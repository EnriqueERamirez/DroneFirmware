#ifndef DRONE_CONTROL_TASKS_H
#define DRONE_CONTROL_TASKS_H

#include <Arduino.h>
#include "MPU6050.h"
#include "DronePID.h"
#include "MotorsController.h"
#include "DistanceController.h"
#include "VL53L0XSensorArray.h"

class DroneControlTasks {
private:
    MPU6050& Imu;
    DronePID& PidController;
    MotorsController& Motors;
    DistanceController& DistanceSensor;
    VL53L0XSensorArray& SensorArray;
    
    TaskHandle_t StabilityTaskHandle;
    TaskHandle_t HeightTaskHandle;
    
    static void StabilityControlTask(void* parameter);
    static void HeightControlTask(void* parameter);
    static DroneControlTasks* Instance;

public:
    DroneControlTasks(MPU6050& imu, 
                     DronePID& pidController, 
                     MotorsController& motors,
                     DistanceController& distanceSensor,
                     VL53L0XSensorArray& sensorArray);
    
    void Initialize();
    void StopTasks();
    void ProcessStabilityControl();
    void ProcessHeightControl();
    void SetTargetHeight(float heightMm);
    float GetCurrentHeight() const { return DistanceSensor.GetCurrentHeight(); }
};

#endif