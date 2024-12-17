unsigned long Tiempo_inicio_loop;
unsigned long Tiempo_fin_loop;
unsigned long tiempoTranscurrido;

// Función para imprimir una lectura en el monitor serie
void imprimirLectura() {


////  // Imprimir tiempo transcurrido (en segundos) desde que inicia el loop 
//    Serial.print("us:   ");
//    Serial.print(Tiempo_inicio_loop);
//    Serial.print("\t");
//  
//   //Lecturas Angulos de Inclinacion Pitch
    //Serial.print("Pitch: ");
   // Serial.print(angulo_x);
    //Serial.print("\t");
    //   //Lecturas Angulos de Inclinacion Pitch
    Serial.print("gyro_z: ");
    Serial.println(angulo_z);

    
     //Lecturas de las salidas PID Pitch
//    Serial.print("Pitch_error: ");
//    Serial.print(Pitch_error);
//    Serial.print("\t");
//    Serial.print("Pitch_P: ");
//    Serial.print(Pitch_P);
//    Serial.print("\t");
//    Serial.print("Pitch_I: ");
//    Serial.print(Pitch_I);
//    Serial.print("\t");
//    Serial.print("Pitch_I_v: ");
//    Serial.print(Pitch_I_v);
//    Serial.print("\t");
//    Serial.print("Pitch_D: ");
//    Serial.print(Pitch_D);
//    Serial.print("\t");
//    Serial.print("Pitch_out: ");
//    Serial.print(Pitch_out);
//    Serial.print("\t");
//    Serial.print("Pitch_out_v: ");
//    Serial.print(Pitch_out_v);
    //Serial.print("\t");

//   //Lecturas Angulos de Inclinacion Roll
    //Serial.print("Roll: ");
   // Serial.println(angulo_y);

      // Imprimir los valores de los sensores en el monitor serial
//  Serial.print("Sensor Izquierdo: ");
//  Serial.print(Sensor_izq);
//  Serial.print("\t");
//  Serial.print("Sensor Trasero: ");
//  Serial.print(Sensor_tras);
//  Serial.print("\t");
//  Serial.print("Sensor Derecho: ");
//  Serial.print(Sensor_der);
//  Serial.print("\t");
//  Serial.print("Sensor Delantero: ");
//  Serial.print(Sensor_del);
//  Serial.print("\t");
//  Serial.print("Sensor Superior: ");
//  Serial.print(Sensor_sup);
//  Serial.print("\t");
//  Serial.print("Sensor Inferior: ");
//  Serial.println(Sensor_inf);
}
