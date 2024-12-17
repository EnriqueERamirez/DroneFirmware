#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(uint8_t batteryPin, 
                             uint8_t rgbLedPin,
                             float inputVoltage,
                             float r1,
                             float r2,
                             float lowVoltageThreshold)
    : BatteryPin(batteryPin),
      RGBLedPin(rgbLedPin),
      InputVoltage(inputVoltage),
      R1Value(r1),
      R2Value(r2),
      LowVoltageThreshold(lowVoltageThreshold),
      CurrentVoltage(0.0f) {
}

void BatteryMonitor::Initialize() {
    // Configurar el pin analógico para la lectura
    pinMode(BatteryPin, INPUT);
    
    // El pin RGB LED normalmente no necesita inicialización
    // ya que la función neopixelWrite lo maneja internamente
}

float BatteryMonitor::CalculateVoltage(int reading) const {
    // Calcular el voltaje usando la fórmula del divisor de voltaje
    float voltage = static_cast<float>(reading) / 4096.0f * InputVoltage;
    return voltage / (R2Value / (R1Value + R2Value));
}

void BatteryMonitor::UpdateLEDStatus() {
    if (CurrentVoltage < LowVoltageThreshold) {
        // LED rojo para indicar bajo voltaje
        neopixelWrite(RGBLedPin, 250, 0, 0);
    } else {
        // LED apagado cuando el voltaje es normal
        neopixelWrite(RGBLedPin, 0, 0, 0);
    }
}

void BatteryMonitor::Update() {
    // Leer el valor analógico
    int reading = analogRead(BatteryPin);
    
    // Calcular el voltaje actual
    CurrentVoltage = CalculateVoltage(reading);
    
    // Actualizar el estado del LED
    UpdateLEDStatus();
    
    // Para depuración - se puede remover en producción
    #ifdef DEBUG_BATTERY
    Serial.print("Voltaje: ");
    Serial.println(CurrentVoltage);
    #endif
}

void BatteryMonitor::SetVoltageParameters(float inputVoltage, float r1, float r2) {
    InputVoltage = inputVoltage;
    R1Value = r1;
    R2Value = r2;
}