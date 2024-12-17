#include "MelodyPlayer.h"

MelodyPlayer::MelodyPlayer(uint8_t buzzerPin, uint16_t pauseDuration)
    : BuzzerPin(buzzerPin),
      Melody(nullptr),
      MelodyLength(0),
      PauseDuration(pauseDuration) {
    pinMode(BuzzerPin, OUTPUT);
}

MelodyPlayer::~MelodyPlayer() {
    if (Melody != nullptr) {
        delete[] Melody;
    }
}

bool MelodyPlayer::IsValidNote(uint16_t frequency, uint16_t duration) const {
    // Validar que la frecuencia y duración estén en rangos razonables
    return (frequency > 0 && frequency < 20000) && 
           (duration > 0 && duration < 5000);
}

void MelodyPlayer::SetMelody(const uint16_t* frequencies, const uint16_t* durations, uint8_t length) {
    // Liberar memoria si ya existe una melodía
    if (Melody != nullptr) {
        delete[] Melody;
    }
    
    // Asignar nueva memoria para la melodía
    Melody = new Note[length];
    MelodyLength = length;
    
    // Copiar las notas a la nueva melodía
    for (uint8_t i = 0; i < length; i++) {
        if (IsValidNote(frequencies[i], durations[i])) {
            Melody[i].Frequency = frequencies[i];
            Melody[i].Duration = durations[i];
        } else {
            // En caso de nota inválida, usar valores seguros por defecto
            Melody[i].Frequency = NOTE_A4;  // Nota A4
            Melody[i].Duration = QUARTER_NOTE;
        }
    }
}

void MelodyPlayer::Play() {
    if (Melody == nullptr || MelodyLength == 0) {
        return;
    }
    
    for (uint8_t i = 0; i < MelodyLength; i++) {
        PlayNote(Melody[i].Frequency, Melody[i].Duration);
        delay(PauseDuration);
    }
}

void MelodyPlayer::PlayNote(uint16_t frequency, uint16_t duration) {
    if (IsValidNote(frequency, duration)) {
        tone(BuzzerPin, frequency, duration);
        delay(duration);
    }
}

void MelodyPlayer::Stop() {
    noTone(BuzzerPin);
}