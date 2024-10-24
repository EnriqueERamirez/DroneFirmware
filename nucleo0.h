///////////////////////////Tarea 1 PID Estabilidad////////////////////////
//////////////////////////////////////////////////////////////////////////
void PIDD(void *parameter) {
  for(;;) {
    
    leer_MPU6050();     
    procesar_MPU6050();
    control_PID();
    motores();

    //Guarda los valores anteriores del calculo de los ángulos
    angulo_x_ant = angulo_x; 
    angulo_y_ant = angulo_y; 
    angulo_z_ant = angulo_z; 
    gyro_x_ant = gyro_x;
    gyro_y_ant = gyro_y;
    gyro_z_ant = gyro_z;
    

    vTaskDelay(4/portTICK_PERIOD_MS);

  }
}



///////////////////////////Tarea 2 Recibir datos RC///////////////////////
//////////////////////////////////////////////////////////////////////////
void recibir_datos_rc(void *parameter) {
  for(;;) {

    if (radio.available()) { 
      // Leer los valores recibidos throttle, pitch y roll
      radio.read(valores_mapeados, sizeof(valores_mapeados));
      
    }
    vTaskDelay(30/portTICK_PERIOD_MS);
  }
}


///////////////////////////Tarea 2 Enviar Datos///////////////////////////
//////////////////////////////////////////////////////////////////////////
void enviar_datos(void *parameter) {
  for(;;) {

      //float datos_dron[7] = {angulo_x, angulo_y, pitch_rc, roll_rc, voltaje,Pitch_I_v,Roll_I_v}  ; 
      float datos_dron[3] = {angulo_x, angulo_y, angulo_z}  ; 
      radio.stopListening();            
      radio.write(&datos_dron, sizeof(datos_dron)); 
      radio.startListening();
          

      vTaskDelay(20/portTICK_PERIOD_MS);
  }  
}



///////////////////////////Tarea 3 PID altura/////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PID_altura(void *parameter) {
  for(;;) {
    
      PID_altura();
      Sensor_inf_ant = Sensor_inf;
      
      vTaskDelay(50/portTICK_PERIOD_MS);
  }  
}


TaskHandle_t tareaRecibirDatosRC;
TaskHandle_t tareaPIDD;
TaskHandle_t tareaenviar_datos;
TaskHandle_t tareaPID_altura;

void setupTask() {
  
  xTaskCreatePinnedToCore(
    PIDD,         
    "PIDD",     
    5000,            
    NULL,            
    3,                
    &tareaPIDD,       
    0                 
  );
  
  xTaskCreatePinnedToCore(
    recibir_datos_rc,         // Función de la tarea
    "recibir_datos_rc",       // Nombre de la tarea
    3000,             // Tamaño de la pila de la tarea
    NULL,              // Parámetros de la tarea
    2,                 // Prioridad de la tarea
    &tareaRecibirDatosRC,     // Guardar la manija de la tarea
    0                  // Núcleo en el que se ejecutará la tarea (núcleo 0)
  );

  xTaskCreatePinnedToCore(
    enviar_datos,         
    "enviar_datos",      
    3000,            
    NULL,             
    0,                
    &tareaenviar_datos,      
    0                  
  );


    xTaskCreatePinnedToCore(
    PID_altura,         
    "PID_altura",      
    5000,            
    NULL,             
    0,                
    &tareaPID_altura,      
    1                  
  );
  
}
