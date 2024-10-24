void PID_altura(){
  //---------------------------PID altura-------------------------------------------------------------
  altura_error = setpoint_altura - Sensor_inf;                             // Error entre lectura y consigna
  altura_P  = altura_kp  * altura_error;                                   // Parte proporcional
  altura_I += (altura_ki * altura_error);                                  // Parte integral (sumatorio del error en el tiempo)
  altura_I  = constrain(altura_I, -PID_limite1_altura, PID_limite1_altura);// Limitar parte integral
  altura_D  = altura_kd * (Sensor_inf - Sensor_inf_ant);                   // Parte derivativa (diferencia entre el error actual y el anterior)
  altura_out =  altura_P + altura_I + altura_D;    // Salida PID
  altura_out = constrain(altura_out, -PID_limite2_altura, PID_limite2_altura);// Limitar salida del PID

  
}


void sensor_altura(){

  
    if (Sensor_sup <= 500) { 
      throttle -= 80; 
    }
  
    else {

    }

}

void altura(){

  
  throttle= throttle + altura_out;
}



void medir_distancia() {

  // Sensor izquierdo
  if (Sensor_izq <= 200) {
    roll_rc = 20;
  }

  // Sensor derecho  
  if (Sensor_der <= 200) {
    roll_rc = -20;
  }

  // Sensor delantero
  if (Sensor_del <= 300) {
    pitch_rc = 20;
  }

  // Sensor trasero
  if (Sensor_tras <= 200) {
    pitch_rc = -20;
  }


  
}
