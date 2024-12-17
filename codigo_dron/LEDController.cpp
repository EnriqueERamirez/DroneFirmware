#include "LEDController.h"

// Constructor con pines por defecto
LEDController::LEDController(uint8_t ledCount, unsigned long minInterval, unsigned long maxInterval)
    : NumberOfLEDs(min(ledCount, MAX_LEDS)),
      CurrentLED(-1),
      LastLEDChange(0),
      MinInterval(minInterval),
      MaxInterval(maxInterval) {
    
    // Asignar memoria para los pines de LED
    LEDPins = new uint8_t[NumberOfLEDs];
    
    // Copiar los pines por defecto
    for (uint8_t i = 0; i < NumberOfLEDs; i++) {
        LEDPins[i] = DEFAULT_PINS[i];
    }
}

// Constructor con pines personalizados
LEDController::LEDController(const uint8_t* pins, uint8_t ledCount, 
                           unsigned long minInterval, unsigned long maxInterval)
    : NumberOfLEDs(min(ledCount, MAX_LEDS)),
      CurrentLED(-1),
      LastLEDChange(0),
      MinInterval(minInterval),
      MaxInterval(maxInterval) {
    
    // Asignar memoria para los pines de LED
    LEDPins = new uint8_t[NumberOfLEDs];
    
    // Copiar los pines al array
    for (uint8_t i = 0; i < NumberOfLEDs; i++) {
        LEDPins[i] = pins[i];
    }
}

// El resto del código permanece igual
LEDController::~LEDController() {
    delete[] LEDPins;
}

void LEDController::Initialize() {
    // Configurar todos los pines como salidas
    for (uint8_t i = 0; i < NumberOfLEDs; i++) {
        pinMode(LEDPins[i], OUTPUT);
        digitalWrite(LEDPins[i], LOW);
    }
}

void LEDController::SetIntervals(unsigned long minInterval, unsigned long maxInterval) {
    MinInterval = minInterval;
    MaxInterval = maxInterval;
}

void LEDController::TurnOffLED(uint8_t index) {
    if (index < NumberOfLEDs) {
        digitalWrite(LEDPins[index], LOW);
    }
}

void LEDController::TurnOnLED(uint8_t index) {
    if (index < NumberOfLEDs) {
        digitalWrite(LEDPins[index], HIGH);
    }
}

void LEDController::UpdateRandomPattern() {
    unsigned long currentTime = micros();
    
    if (currentTime - LastLEDChange >= random(MinInterval, MaxInterval)) {
        // Apagar el LED actual si existe
        if (CurrentLED != -1) {
            TurnOffLED(CurrentLED);
        }
        
        // Seleccionar y encender un nuevo LED aleatorio
        CurrentLED = random(0, NumberOfLEDs);
        TurnOnLED(CurrentLED);
        
        LastLEDChange = currentTime;
    }
}

void LEDController::TurnOffAll() {
    for (uint8_t i = 0; i < NumberOfLEDs; i++) {
        TurnOffLED(i);
    }
    CurrentLED = -1;
}

void LEDController::RunStartupSequence(unsigned long duration) {
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        UpdateRandomPattern();
    }
    
    TurnOffAll();
}