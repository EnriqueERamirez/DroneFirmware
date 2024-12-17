#include "MPU6050.h"

MPU6050::MPU6050() : IsCalibrated(false), CycleTimeUs(4000) {
    // Inicialización de variables
    AccXRaw = AccYRaw = AccZRaw = 0;
    GyroXRaw = GyroYRaw = GyroZRaw = 0;
    GyroXCal = GyroYCal = GyroZCal = 0;
    AccXCal = AccYCal = AccZCal = 0;
    AngleX = AngleY = AngleZ = 0;
}

void MPU6050::Initialize() {
    Wire.begin();
    
    // Despertar el MPU6050
    Wire.beginTransmission(MPU6050Address);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);
    
    // Configurar giroscopio a 500°/s
    Wire.beginTransmission(MPU6050Address);
    Wire.write(0x1B);
    Wire.write(0x08);
    Wire.endTransmission(true);
    
    // Configurar acelerómetro a +/-8g
    Wire.beginTransmission(MPU6050Address);
    Wire.write(0x1C);
    Wire.write(0x10);
    Wire.endTransmission(true);
    
    // Configurar filtro pasa bajos a 5Hz
    Wire.beginTransmission(MPU6050Address);
    Wire.write(0x1A);
    Wire.write(0x06);
    Wire.endTransmission(true);
}

void MPU6050::ReadRawData() {
    Wire.beginTransmission(MPU6050Address);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050Address, 14);
    
    while (Wire.available() < 14);
    
    AccXRaw = Wire.read() << 8 | Wire.read();
    AccYRaw = Wire.read() << 8 | Wire.read();
    AccZRaw = Wire.read() << 8 | Wire.read();
    Temperature = Wire.read() << 8 | Wire.read();
    GyroXRaw = Wire.read() << 8 | Wire.read();
    GyroYRaw = Wire.read() << 8 | Wire.read();
    GyroZRaw = Wire.read() << 8 | Wire.read();
}

void MPU6050::Calibrate() {
    int32_t GyroXSum = 0, GyroYSum = 0, GyroZSum = 0;
    int32_t AccXSum = 0, AccYSum = 0, AccZSum = 0;
    const int CalibrationCount = 4000;
    
    for (int i = 0; i < CalibrationCount; i++) {
        ReadRawData();
        GyroXSum += GyroXRaw;
        GyroYSum += GyroYRaw;
        GyroZSum += GyroZRaw;
        AccXSum += AccXRaw;
        AccYSum += AccYRaw;
        AccZSum += AccZRaw;
        delayMicroseconds(500);
    }
    
    // Calcular promedios
    GyroXCal = GyroXSum / CalibrationCount;
    GyroYCal = GyroYSum / CalibrationCount;
    GyroZCal = GyroZSum / CalibrationCount;
    AccXCal = AccXSum / CalibrationCount;
    AccYCal = AccYSum / CalibrationCount;
    AccZCal = (AccZSum / CalibrationCount) - 4096; // Corrección para el eje Z
    
    IsCalibrated = true;
}

void MPU6050::ProcessData() {
    // Corregir valores con calibración
    GyroXRaw -= GyroXCal;
    GyroYRaw -= GyroYCal;
    GyroZRaw -= GyroZCal;
    AccXRaw -= AccXCal;
    AccYRaw -= AccYCal;
    AccZRaw -= AccZCal;
    
    // Convertir giroscopio a grados por segundo
    GyroX = GyroXRaw / 65.5f;
    GyroY = GyroYRaw / 65.5f;
    GyroZ = GyroZRaw / 65.5f;
    
    // Integrar ángulos
    float deltaTime = CycleTimeUs / 1000000.0f;
    AngleX += GyroX * deltaTime;
    AngleY += GyroY * deltaTime;
    AngleZ += GyroZ * deltaTime;
    
    // Calcular ángulos del acelerómetro
    AngleYAcc = atan(-AccXRaw / sqrt(pow(AccYRaw, 2) + pow(AccZRaw, 2))) * RAD_TO_DEG;
    AngleXAcc = atan(AccYRaw / sqrt(pow(AccXRaw, 2) + pow(AccZRaw, 2))) * RAD_TO_DEG;
    
    // Aplicar filtro complementario
    const float Alpha = 0.997f;
    AngleX = AngleX * Alpha + AngleXAcc * (1.0f - Alpha);
    AngleY = AngleY * Alpha + AngleYAcc * (1.0f - Alpha);
    
    // Calcular aceleraciones en g
    AccXG = AccXRaw / 4096.0f;
    AccYG = AccYRaw / 4096.0f;
}

void MPU6050::Update() {
    if (!IsCalibrated) {
        return;
    }
    ReadRawData();
    ProcessData();
}