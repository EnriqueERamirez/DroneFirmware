#include "LEDController.h"

// Constructor con pines por defecto
LEDController::LEDController(uint8_t ledCount, unsigned long minInterval, unsigned long maxInterval)
    : NumberOfLEDs(min(ledCount, MAX_LEDS)),
      CurrentLED(-1),
      LastLEDChange(0),
      MinInterval(minInterval),
      MaxInterval(maxInterval) {
    LEDPins = new uint8_t[NumberOfLEDs];
    
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
    
    LEDPins = new uint8_t[NumberOfLEDs];
    
    for (uint8_t i = 0; i < NumberOfLEDs; i++) {
        LEDPins[i] = pins[i];
    }
}

LEDController::~LEDController() {
    delete[] LEDPins;
}

void LEDController::Initialize() {
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
        if (CurrentLED != -1) {
            TurnOffLED(CurrentLED);
        }
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