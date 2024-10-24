//#include "vuelo_autonomo.h"
//Funcion para configurar cada motor
void configurarMotores() {
  // Configurar PWM para motores
  for (int channel = 0; channel < 4; ++channel) {
    ledcSetup(channel, 2000, 10); // Canal, Frecuencia, resolucion 
  }
  // Se define motor y canal
  ledcAttachPin(motor1, 0);
  ledcAttachPin(motor2, 1);
  ledcAttachPin(motor3, 2);
  ledcAttachPin(motor4, 3);
}

//Funcion para controlar los motores
void motores(){

  if (estado_rc == 1) {

     throttle = map(valores_mapeados[0], 50, 1024, 0, velocidad_limite); // Mapear el throttle
     throttle = constrain (throttle, 0, velocidad_limite);
    
  }

  else {
    
//vuelo_autonomo();
    
 }

//--------------------------Anticolision----------------------//
////////////////////////////////////////////////////////////////
sensor_altura();   //Anticolision de altura
if (PID_h == 1) {

      altura();
}

else {}


  

throttle = constrain(throttle, 0, velocidad_limite);

    if (throttle == 0) {
      // Si el throttle es 0, entonces las velocidades de todos los motores deben ser 0
      velocidad_M1 = 0;
      velocidad_M2 = 0;
      velocidad_M3 = 0;
      velocidad_M4 = 0;
    } 
    
    else {
      

            // Si el throttle no es 0, calcular las velocidades como antes, constrain limita el rango de los valores pwm para los motores
      velocidad_M1 = constrain(throttle - Roll_out_v + Pitch_out_v - Yaw_out_v,  0, velocidad_limite); // FR
      velocidad_M2 = constrain(throttle + Roll_out_v + Pitch_out_v + Yaw_out_v,  0, velocidad_limite); // FL
      velocidad_M3 = constrain(throttle + Roll_out_v - Pitch_out_v - Yaw_out_v,  0, velocidad_limite); // RL
      velocidad_M4 = constrain(throttle - Roll_out_v - Pitch_out_v + Yaw_out_v,  0, velocidad_limite); // RR
    }

//    // Iniciar los motores
    ledcWrite(0, velocidad_M1);
    ledcWrite(1, velocidad_M2);
    ledcWrite(2, velocidad_M3);
    ledcWrite(3, velocidad_M4);

  
}
