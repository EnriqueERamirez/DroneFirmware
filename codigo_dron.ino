//-----------------------------Control de estabilidad del dron BPI------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////

// Incluir la librería Arduino
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>


//////-------Archivos incluidos-----////////////
#include "variables.h"
#include "LEDS.h"
#include "buzzer.h"
#include "mpu6050.h"
#include "VL53L0X.h"
#include "anticolision.h"
#include "PID.h"
#include "Control_Motores.h"
#include "NRF24L01.h"
#include "lectura_datos.h"
#include "leer_voltaje.h"
#include "nucleo0.h"
#include "darth_vader.h"

void setup() {
    delay(1000);
    Serial.begin(115200);
    
    Wire.setClock(400000); // 400 kHz
    //Setup nucleo 0
    setupTask();
    pinMode(BUZZER_PIN, OUTPUT);
    darth_vader();
    //buzzer();
    //delay(1000);
    
    inicializarMPU6050();//Se inicia SENSOR MPU6050
    calibrarMPU6050(); //Se inicia la calibracion del sensor
    Serial.println("Sensor MPU6050 Calibrado...");
    
    iniciarLEDs();
    inicioLEDs();
  
    configurarMotores();
    inicializarNRF24L01();
    iniciarVL53L0X();
    configurarVL53L0X(); // Iniciar la configuración de los sensores

    pinMode(1, INPUT); //Entrada analogica para lectura de voltaje
    Serial.println("Iniciando Motores...");
    leerVoltajeBateria();

        // TIEMPO DE INICIO THROTTLE
    startTime = millis(); // Guardar el tiempo de inicio throttle


}



void loop() { 

//leerVL53L0X();

imprimirLectura(); //Imprime lecturas de datos en el monitor Serial




  
//  unsigned long tiempo_ac = micros();
//  delta= tiempo_ac - tiempo_ant;
//  if (delta >= tiempo_ciclo_us) {
//     tiempo_ant = tiempo_ac; // Actualizar el tiempo de la última ejecución
  

// }


//Si throttle es igual a cero, encender leds aleatorios
    if  ( throttle ==0 ){
          
        inicioLEDs();

        //reiniciar parametros integral PID
        Pitch_I=0;
        Roll_I=0;
        Pitch_I_v=0;
        Roll_I_v=0;
        Yaw_I_v=0;

        }

    else { 
      apagarTodosLEDs();
    }

    
}
