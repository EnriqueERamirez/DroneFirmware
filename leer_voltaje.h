void leerVoltajeBateria() {

//  if (valores_mapeados[0] == 0){         //si throttle = 0 leer el voltaje de la bateria

    lectura = analogRead(1);
    voltaje =((lectura / 4096) * vin ) /((r2 / (r1 + r2)));

     if (voltaje < 3.7) {
      neopixelWrite(RGB_BUILTIN,250,0,0); //Enciende LED RGB del BPI LEAF S3
      
      }

     else { 
      neopixelWrite(RGB_BUILTIN,0,0,0); //Enciende LED RGB del BPI LEAF S3
  
      }
      
     Serial.print("voltaje: ");
     Serial.println(voltaje);
    
//  }
  
}
