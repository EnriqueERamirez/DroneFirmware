// Arreglo de la melodía
int melody[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};

// Duraciones correspondientes a cada nota (en milisegundos)
int noteDurations[] = {
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE
};



void buzzer() {

  for (int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i], noteDurations[i]);
    // Esperar la duración de la nota + 10 ms de pausa entre notas
    delay(noteDurations[i] + 10);
  }

}
