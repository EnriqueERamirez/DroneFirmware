// Función para leer datos del MPU6050
void leer_MPU6050() {
  Wire.beginTransmission(MPU6050_address); // Inicia la transmisión
  Wire.write(0x3B); // Selecciona el registro de inicio de lectura
  Wire.endTransmission(false); // Finaliza la transmisión
  Wire.requestFrom(MPU6050_address, 14); // Solicita la lectura de 14 bytes desde el MPU6050
  while (Wire.available() < 14); // Espera a que estén disponibles los 14 bytes
  acc_x_raw = Wire.read() << 8 | Wire.read(); // Lee y combina los bytes para obtener el valor bruto del acelerómetro en los ejes X, Y y Z.
  acc_y_raw = Wire.read() << 8 | Wire.read(); 
  acc_z_raw = Wire.read() << 8 | Wire.read(); 
  temperature = Wire.read() << 8 | Wire.read(); // Lee y combina los bytes para obtener el valor bruto de la temperatura
  gyro_x_raw = Wire.read() << 8 | Wire.read(); // Lee y combina los bytes para obtener el valor bruto del giroscopio en los ejes X, Y y Z.
  gyro_y_raw = Wire.read() << 8 | Wire.read();
  gyro_z_raw = Wire.read() << 8 | Wire.read(); 
}

void calibrarMPU6050() {
  // Calibración MPU6050
  for (int cal_int = 0; cal_int < 4000; cal_int++) {
    leer_MPU6050(); // Asume que esta función lee los datos crudos del MPU6050
    gyro_x_cal += gyro_x_raw;
    gyro_y_cal += gyro_y_raw;
    gyro_z_cal += gyro_z_raw;
    acc_x_cal += acc_x_raw;
    acc_y_cal += acc_y_raw;
    acc_z_cal += acc_z_raw;
    neopixelWrite(RGB_BUILTIN,20,20,20); //Enciende LED RGB del BPI LEAF S3
    delayMicroseconds(500); // Pequeña pausa entre lecturas
  }

  // Calcula el promedio de los valores de calibración
  gyro_x_cal /= 4000;
  gyro_y_cal /= 4000;
  gyro_z_cal /= 4000;
  acc_x_cal /= 4000;
  acc_y_cal /= 4000;
  acc_z_cal /= 4000;
  neopixelWrite(RGB_BUILTIN,10,10,15);//Enciende LED RGB
  delay(500);
  neopixelWrite(RGB_BUILTIN,0,0,0);//Apaga el LED RGB 
  
  // Indica que el MPU6050 ha sido calibrado
  MPU6050_calibrado = true;
 
}


void inicializarMPU6050() {
  // Inicializa el bus I2C con los pines predeterminados del ESP32 (SDA, SCL)
  Wire.begin(); 

  // Despertar el MPU6050
  Wire.beginTransmission(MPU6050_address);// Inicia el MPU6050
  Wire.write(0x6B); // Accede al registro
  Wire.write(0x00); // Asigna valor al registro
  Wire.endTransmission(true);// Finaliza la comunicación

  // Configurar giroscopio a 500°/s
  Wire.beginTransmission(MPU6050_address);
  Wire.write(0x1B); 
  Wire.write(0x08);
  Wire.endTransmission(true);

  // Configurar acelerómetro a +/-8g
  Wire.beginTransmission(MPU6050_address);
  Wire.write(0x1C); 
  Wire.write(0x10);
  Wire.endTransmission(true);

  // Filtro pasa bajos MPU6050
  Wire.beginTransmission(MPU6050_address);
  Wire.write(0x1A);
  Wire.write(0x06);
  Wire.endTransmission(true);

  /*
    Frecuencia de corte del filtro pasa bajos:
    256Hz(0ms):0x00
    188Hz(2ms):0x01
    98Hz(3ms):0x02
    42Hz(4.9ms):0x03
    20Hz(8.5ms):0x04
    10Hz(13.8ms):0x05
    5Hz(19ms):0x06
  */
}


//// Variables globales
//unsigned long tiempo_anterior = 0;
//const unsigned long intervalo_reinicio = 5000000; // 5 segundos en milisegundos
//
//// Función para reiniciar el ángulo z cada 5 segundos
//void reiniciar_angulo_z() {
//  unsigned long tiempo_actual = micros();
//  unsigned long tiempo_transcurrido = tiempo_actual - tiempo_anterior;
//  
//  if (tiempo_transcurrido >= intervalo_reinicio) {
//    angulo_z = 0; // Reiniciar el ángulo z
//    tiempo_anterior = tiempo_actual; // Actualizar el tiempo anterior al reinicio
//  }
//}



void procesar_MPU6050() {
  // Corrección de los valores brutos del giroscopio y el acelerómetro con los valores de calibración
  gyro_x_raw -= gyro_x_cal;
  gyro_y_raw -= gyro_y_cal;
  gyro_z_raw -= gyro_z_cal;
  acc_x_raw -= acc_x_cal;
  acc_y_raw -= acc_y_cal;
  acc_z_raw -= acc_z_cal - 4096;// Corrección adicional para el eje Z del acelerómetro

// Convertir los valores brutos del giroscopio a grados por segundo
  gyro_x = gyro_x_raw / 65.5;
  gyro_y = gyro_y_raw / 65.5;
  gyro_z= gyro_z_raw / 65.5;
  
// Integración de los valores del giroscopio para obtener los ángulos de inclinación
  angulo_x += gyro_x * (tiempo_ciclo_us / 1000000.0);
  angulo_y += gyro_y * (tiempo_ciclo_us / 1000000.0);
  angulo_z += gyro_z * (tiempo_ciclo_us / 1000000.0);
  
// Cálculo de los ángulos de inclinación a partir de los datos del acelerómetro
  angulo_y_acc = atan(-acc_x_raw / sqrt(pow(acc_y_raw, 2) + pow(acc_z_raw, 2))) * RAD_TO_DEG;
  angulo_x_acc = atan(acc_y_raw / sqrt(pow(acc_x_raw, 2) + pow(acc_z_raw, 2))) * RAD_TO_DEG;

// Aplicación del filtro complementario para combinar los datos del giroscopio y el acelerómetro
  angulo_x = angulo_x * 0.997 + angulo_x_acc * 0.003;
  angulo_y = angulo_y * 0.997 + angulo_y_acc * 0.003;




////Reiniciar YAW cada 5 segundos
//  reiniciar_angulo_z();
}

float acc_x_g;
float acc_y_g;

void aceleracion(){

  // Convertir los valores brutos del acelerómetro a g
  acc_x_g = acc_x_raw / 4096.0;
  acc_y_g = acc_y_raw / 4096.0;



  // Imprimir valores en g
//Serial.print(acc_x_g);
//Serial.print("\t");
//Serial.println(acc_y_g); 

}
