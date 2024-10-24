// Función para inicializar los pines de los LEDs
void iniciarLEDs() {
  for (int i = 0; i < numLEDs; i++) {
    pinMode(pinesLED[i], OUTPUT);
  }
}

void manejadorLEDAleatorio() {
  
  unsigned long tiempoActual = micros();

  // Verificar si ha pasado el tiempo suficiente desde el último cambio de estado de LED
  if (tiempoActual - ultCambioLED >= random(intMin, intMax)) {
    // Apagar el LED anterior
    if (LEDActual != -1) {
      digitalWrite(pinesLED[LEDActual], LOW);
    }
    
    // Encender un LED aleatorio
    LEDActual = random(0, numLEDs);
    digitalWrite(pinesLED[LEDActual], HIGH);

    // Actualizar el tiempo del último cambio de estado de LED
    ultCambioLED = tiempoActual;
  }
}

void apagarTodosLEDs() {
  // Iterar sobre todos los pines en el arreglo pinesLED
  for (int i = 0; i < sizeof(pinesLED) / sizeof(pinesLED[0]); i++) {
    // Establecer el pin actual en LOW
    digitalWrite(pinesLED[i], LOW);
  }
}

void inicioLEDs(){
    // Tiempo inicial en milisegundos
  unsigned long tiempoInicio = millis();

  // Ejecutar la función durante 2 segundos
  while (millis() - tiempoInicio < 5000) {
      unsigned long tiempoActual = micros();

  // Verificar si ha pasado el tiempo suficiente desde el último cambio de estado de LED
  if (tiempoActual - ultCambioLED >= random(intMin, intMax)) {
    // Apagar el LED anterior
    if (LEDActual != -1) {
      digitalWrite(pinesLED[LEDActual], LOW);
    }
    
    // Encender un LED aleatorio
    LEDActual = random(0, numLEDs);
    digitalWrite(pinesLED[LEDActual], HIGH);

    // Actualizar el tiempo del último cambio de estado de LED
    ultCambioLED = tiempoActual;
  }

    
}

  // Apagar todos los LEDs después de 2 segundos
  for (int i = 0; i < numLEDs; i++) {
    digitalWrite(pinesLED[i], LOW);
  }
}
