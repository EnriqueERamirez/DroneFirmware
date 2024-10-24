#include "Adafruit_VL53L0X.h"

// Declaración de los objetos VL53L0X para cada sensor y estructura para las mediciones
Adafruit_VL53L0X lox[NUM_SENSORES];
VL53L0X_RangingMeasurementData_t medidas[NUM_SENSORES];


// Función para configurar los pines de los sensores VL53L0X
void iniciarVL53L0X() {
  // Configurar los pines de los sensores VL53L0X
  for (int i = 0; i < NUM_SENSORES; i++) {
    pinMode(PINES_LOX[i], OUTPUT);
    digitalWrite(PINES_LOX[i], LOW);
  }
}

// Función para configurar las identificaciones de los sensores
void configurarVL53L0X() {
  // Establecer los pines de los sensores en LOW para configurarlos
  for (int i = 0; i < NUM_SENSORES; i++) {
    digitalWrite(PINES_LOX[i], LOW);
  }
  delay(10);

  // Inicializar cada sensor y verificar si se ha iniciado correctamente
  for (int i = 0; i < NUM_SENSORES; i++) {
    digitalWrite(PINES_LOX[i], HIGH);
    if (i > 0) {
      delay(10);
    }

    if (!lox[i].begin(DIRECCIONES_LOX[i])) {
      Serial.print(F("Error en la lectura del sensor "));
      Serial.println(i + 1);
      while (1);
    }
  }
}

// Función para leer los sensores
void leerVL53L0X() {
  for (int i = 0; i < NUM_SENSORES; i++) {
    lox[i].rangingTest(&medidas[i], false); // Realizar la medición del sensor i
    // Verificar si la medición es válida
    if (medidas[i].RangeStatus != RANGO_OK) { // Estado de rango válido
      // Almacenar la lectura en la variable correspondiente
      int lectura = medidas[i].RangeMilliMeter;
      // Si la lectura es superior a 1100, mantenerla en 1100
      if (lectura > 1100) {
        lectura = 1100;
      }
      switch (i) {
        case 0:
          Sensor_izq = lectura;
          break;
        case 1:
          Sensor_tras = lectura;
          break;
        case 2:
          Sensor_der = lectura;
          break;
        case 3:
          Sensor_del = lectura;
          break;
        case 4:
          Sensor_sup = lectura;
          break;
        case 5:
          Sensor_inf = lectura; // Agregado para el nuevo sensor
          break;
        default:
          break;
      }
    }
  }
}
