#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include <Arduino.h>

class MelodyPlayer {
private:
    struct Note {
        uint16_t Frequency;
        uint16_t Duration;
    };

    // Default pin configuration
    static const uint8_t DEFAULT_BUZZER_PIN = 7;
    
    // Musical note frequencies (Hz)
    static const uint16_t NOTE_C4 = 262;
    static const uint16_t NOTE_D4 = 294;
    static const uint16_t NOTE_E4 = 330;
    static const uint16_t NOTE_F4 = 349;
    static const uint16_t NOTE_G4 = 392;
    static const uint16_t NOTE_A4 = 440;
    static const uint16_t NOTE_B4 = 494;
    static const uint16_t NOTE_C5 = 523;

    // Standard note durations (ms)
    static const uint16_t QUARTER_NOTE = 250;
    static const uint16_t HALF_NOTE = 500;
    static const uint16_t WHOLE_NOTE = 1000;

    uint8_t BuzzerPin;
    Note* Melody;
    uint8_t MelodyLength;
    uint16_t PauseDuration;
    
    // Método privado para validación
    bool IsValidNote(uint16_t frequency, uint16_t duration) const;
    
public:
    // Constructor with default pin value
    explicit MelodyPlayer(uint8_t buzzerPin = DEFAULT_BUZZER_PIN, uint16_t pauseDuration = 10);
    ~MelodyPlayer();
    
    // Métodos de configuración
    void SetMelody(const uint16_t* frequencies, const uint16_t* durations, uint8_t length);
    void SetPauseDuration(uint16_t pause) { PauseDuration = pause; }
    
    // Métodos de reproducción
    void Play();
    void PlayNote(uint16_t frequency, uint16_t duration);
    void Stop();
    
    // Getters
    uint8_t GetMelodyLength() const { return MelodyLength; }
    uint16_t GetPauseDuration() const { return PauseDuration; }
    
    // Note frequency getters
    static uint16_t GetNoteC4() { return NOTE_C4; }
    static uint16_t GetNoteD4() { return NOTE_D4; }
    static uint16_t GetNoteE4() { return NOTE_E4; }
    static uint16_t GetNoteF4() { return NOTE_F4; }
    static uint16_t GetNoteG4() { return NOTE_G4; }
    static uint16_t GetNoteA4() { return NOTE_A4; }
    static uint16_t GetNoteB4() { return NOTE_B4; }
    static uint16_t GetNoteC5() { return NOTE_C5; }
    
    // Duration getters
    static uint16_t GetQuarterNote() { return QUARTER_NOTE; }
    static uint16_t GetHalfNote() { return HALF_NOTE; }
    static uint16_t GetWholeNote() { return WHOLE_NOTE; }
};

#endif