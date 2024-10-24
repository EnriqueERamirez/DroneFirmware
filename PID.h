void control_PID() {

  if (estado_rc == 1) {
    

    pitch_map = map(valores_mapeados[1], 0, 120, 20, -20);
    roll_map = map(valores_mapeados[2], 0, 142, -20, 20);

    //Se elimina valores que afectan a los valores enviado del radiocontrol
    if (valores_mapeados[1] >= 56 && valores_mapeados[1] <= 68) {pitch_map = 0;}
    if (valores_mapeados[2] >= 70 && valores_mapeados[2] <= 77) {roll_map = 0;}
    
    pitch_rc = constrain(pitch_map, -20, 20);
    roll_rc = constrain(roll_map, -20, 20);
      
        if (medir_dist == 1){
          
        medir_distancia();
          
        } else{}
        
  }


  
  else if (estado_rc == 0) {
    pitch_rc = 0;
    roll_rc = 0;

        if (medir_dist == 1){
          
        medir_distancia();
          
        } else{}
  }



//-----------------------Control PID para el angulo-----------------------------
///////////////////////////////////////////////////////////////////////////////
    //---------------------------PID Pitch---------------------------
  Pitch_error = pitch_rc - angulo_x;// Error entre la Referencia y el angulo medido
  Pitch_P  = Pitch_Kp  * Pitch_error;// Parte proporcional
  Pitch_I += (Pitch_Ki * Pitch_error);// Parte integral (suma del error en el tiempo)
  Pitch_I  = constrain(Pitch_I, -PID_limite1, PID_limite1);// Limitar parte integral
  Pitch_D  = Pitch_Kd * (angulo_x - angulo_x_ant);// Parte derivativa (diferencia entre el error actual y el anterior)
  Pitch_out =  Pitch_P + Pitch_I + Pitch_D;// Salida PID
  Pitch_out = constrain(Pitch_out, -PID_limite2, PID_limite2);// Limitar salida del PID

  //---------------------------PID Roll---------------------------
  Roll_error = roll_rc -  angulo_y;// Error entre la Referencia y el angulo medido
  Roll_P  = Roll_Kp  * Roll_error;// Parte proporcional
  Roll_I += (Roll_Ki * Roll_error);// Parte integral (suma del error en el tiempo)
  Roll_I  = constrain(Roll_I, -PID_limite1, PID_limite1);// Limitar parte integral
  Roll_D  = Roll_Kd * (angulo_y - angulo_y_ant);// Parte derivativa (diferencia entre el error actual y el anterior)
  Roll_out =  Roll_P + Roll_I + Roll_D;// Salida PID
  Roll_out = constrain(Roll_out, -PID_limite2, PID_limite2);// Limitar salida del PID



//-----------------------Control PID para velocidad angular-----------------------------
////////////////////////////////////////////////////////////////////////////////////////

  //---------------------------PID Pitch---------------------------
  Pitch_error_v = Pitch_out - gyro_x;                        // Error entre lectura y consigna
  Pitch_P_v  = Pitch_Kp_v  * Pitch_error_v;                        // Parte proporcional
  Pitch_I_v += (Pitch_Ki_v * Pitch_error_v);                     // Parte integral (sumatorio del error en el tiempo)
  Pitch_I_v  = constrain(Pitch_I_v, -PID_limite1_v, PID_limite1_v);    // Limitar parte integral
  Pitch_D_v  = Pitch_Kd_v * ( gyro_x - gyro_x_ant);         // Parte derivativa (diferencia entre el error actual y el anterior)
  Pitch_out_v =  Pitch_P_v + Pitch_I_v + Pitch_D_v;    // Salida PID
  Pitch_out_v = constrain(Pitch_out_v, -PID_limite2_v, PID_limite2_v); // Limitar salida del PID

  //---------------------------PID Roll---------------------------
  Roll_error_v = Roll_out -  gyro_y;                        // Error entre lectura y consigna
  Roll_P_v  = Roll_Kp_v  * Roll_error_v;                        // Parte proporcional
  Roll_I_v += (Roll_Ki_v * Roll_error_v);                     // Parte integral (sumatorio del error en el tiempo)
  Roll_I_v  = constrain(Roll_I_v, -PID_limite1_v, PID_limite1_v);    // Limitar parte integral
  Roll_D_v  = Roll_Kd_v * (gyro_y - gyro_y_ant);         // Parte derivativa (diferencia entre el error actual y el anterior)
  Roll_out_v =  Roll_P_v + Roll_I_v + Roll_D_v;    // Salida PID
  Roll_out_v = constrain(Roll_out_v, -PID_limite2_v, PID_limite2_v); // Limitar salida del PID


  //---------------------------PID Yaw---------------------------
  Yaw_error_v = setpoint - gyro_z;                        // Error entre lectura y consigna
  Yaw_P_v  = Yaw_Kp_v  * Yaw_error_v;                        // Parte proporcional
  Yaw_I_v += (Yaw_Ki_v * Yaw_error_v);                       // Parte integral (sumatorio del error en el tiempo)
  Yaw_I_v  = constrain(Yaw_I_v, -PID_limite1_v, PID_limite1_v);    // Limitar parte integral
  Yaw_D_v  = Yaw_Kd_v * (gyro_z - gyro_z_ant);           // Parte derivativa (diferencia entre el error actual y el anterior)
  Yaw_out_v =  Yaw_P_v + Yaw_I_v + Yaw_D_v;    // Salida PID
  Yaw_out_v = constrain(Yaw_out_v, -PID_limite2_v, PID_limite2_v); // Limitar salida del PID
}
