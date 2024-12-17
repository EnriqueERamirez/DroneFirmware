#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

class LEDController {
private:
    static const uint8_t MAX_LEDS = 8;  // Máximo número de LEDs soportados
    
    // Array de pines por defecto
    static constexpr uint8_t DEFAULT_PINS[MAX_LEDS] = {2, 3, 5, 6, 21, 8, 45, 42};
    
    uint8_t* LEDPins;         // Array dinámico para los pines de LED
    uint8_t NumberOfLEDs;     // Número actual de LEDs
    int8_t CurrentLED;        // LED actualmente encendido
    
    // Temporizadores y intervalos
    unsigned long LastLEDChange;
    unsigned long MinInterval;
    unsigned long MaxInterval;
    
    // Métodos privados auxiliares
    void TurnOffLED(uint8_t index);
    void TurnOnLED(uint8_t index);
    
public:
    // Constructor con pines por defecto
    LEDController(uint8_t ledCount = MAX_LEDS, 
                 unsigned long minInterval = 10000, 
                 unsigned long maxInterval = 20000);
    
    // Constructor con pines personalizados
    LEDController(const uint8_t* pins, uint8_t ledCount, 
                 unsigned long minInterval = 10000, 
                 unsigned long maxInterval = 20000);
                 
    ~LEDController();
    
    // Métodos de inicialización y configuración
    void Initialize();
    void SetIntervals(unsigned long minInterval, unsigned long maxInterval);
    
    // Métodos de control
    void UpdateRandomPattern();
    void TurnOffAll();
    void RunStartupSequence(unsigned long duration = 5000);
    
    // Getters
    uint8_t GetCurrentLED() const { return CurrentLED; }
    uint8_t GetNumberOfLEDs() const { return NumberOfLEDs; }
};

#endif