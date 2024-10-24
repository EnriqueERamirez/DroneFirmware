//////////////////////////////////////////////////////DECLARACION DE TODAS LAS VARIABLES//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool estado_rc = 1;       //0: No usar el radio control, 1: Usar Radio control.
bool medir_dist = 0;      //0: No sar sensores láser, 1: Usar sensores láser.
bool PID_h = 0;           // 0: No usar PID altura, 1: Usar PID altura.

//Declaración de variables para los PID///
//////////////////////////////////////////
//VARIABLES PID PARA PID VELOCIDAD
float Pitch_error_v, Pitch_P_v, Pitch_I_v, Pitch_D_v, Pitch_out_v;//Variables Pitch
float Roll_error_v, Roll_P_v, Roll_I_v, Roll_D_v, Roll_out_v;//Variables Roll
float Yaw_error_v, Yaw_P_v, Yaw_I_v, Yaw_D_v, Yaw_out_v;//Variables Yaw

//VARIABLES PID PARA PID ANGULO
float Pitch_error, Pitch_P, Pitch_I, Pitch_D, Pitch_out;//Variables Pitch
float Roll_error, Roll_P, Roll_I, Roll_D, Roll_out;//Variables Roll

float gyro_x_ant, gyro_y_ant, gyro_z_ant; //Variables para guardar lectura anterior del GIROSCOPIO------PID velocidad-Radiocontrol
float angulo_x_ant, angulo_y_ant, angulo_z_ant; //Variables angulo anterior ACELEROMETRO Y GIROSCOPIO-------PID angulo
float Pitch_v_consigna, Roll_v_consigna;

double setpoint = 0; //YAW

int PID_limite1 = 300;  // Limitar parte integral PID ángulo
int PID_limite2 = 430;  // Limitar salida del PID ángulo

int PID_limite1_v = 300;  // Limitar parte integral PID velocidad
int PID_limite2_v = 430;  // Limitar salida del PID velocidad


//Parametros PID angulo     
float Pitch_Kp = 0.55,             Pitch_Ki = 0.0006,          Pitch_Kd = 0; //Pitch
float Roll_Kp  = 0.50,             Roll_Ki  = 0.00065,          Roll_Kd  = 0; //Roll

//Parametros PID Velocidad angular ----Radio control
float Pitch_Kp_v = 1.35,          Pitch_Ki_v = 0.0007,        Pitch_Kd_v = 0.0; //Pitch
float Roll_Kp_v  = 1.3,           Roll_Ki_v  = 0.0007,        Roll_Kd_v  = 0.0; //Roll 
float Yaw_Kp_v   = 2.5,           Yaw_Ki_v   = 0.0004,        Yaw_Kd_v   = 0.0; //Yaw


//Variables para el PID de altura//
///////////////////////////////////
float altura_kp = 1,        altura_ki = 0.0,            altura_kd = 0.0;
int PID_limite1_altura = 200;
int PID_limite2_altura = 1000;
int setpoint_altura= 200;  //700mm = 0,7mts
uint16_t Sensor_inf_ant;
float altura_error, altura_P, altura_I, altura_D, altura_out;


//Declaración de variables de tiempo//////
//////////////////////////////////////////
unsigned long tiempo_ciclo_us =4000;// 
unsigned long tiempo_ac;//
unsigned long tiempo_ant = 0, tiempo_ant2 = 0;
unsigned long delta, delta2;


// Declaración de variables para Motores//
/////////////////////////////////////////
// Pines de cada motor
int motor1 = 39;  // FR 1
int motor2 = 4;  // FL
int motor3 = 14; // RL 
int motor4 = 47; // RR
int velocidad_limite = 750;//650 se levanta el dron
// Calcular la velocidad de cada motor
int velocidad_M1, velocidad_M2, velocidad_M3, velocidad_M4;
int throttle=0;
unsigned long startTime;
//Segundos que permanece encendido throttle en microsegundos
unsigned long  t1 = 12000000; 
unsigned long  t2 = 20000000;


 
//Declaración de variables para MPU-6050//
//////////////////////////////////////////
#define MPU6050_address 0x68
#define RAD_TO_DEG 57.295779 //Conversión de radianes a grados
int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw, acc_x_raw, acc_y_raw, acc_z_raw, temperature; //Variables  de datos RAW
int32_t gyro_x_cal, gyro_y_cal, gyro_z_cal, acc_z_cal, acc_y_cal, acc_x_cal; //Variables de calibración
float angulo_x, angulo_y, angulo_z, angulo_x_acc, angulo_y_acc; //Variables de los angulos obtenidos
float gyro_x, gyro_y, gyro_z;// Variables para convertir los valores brutos del giroscopio
bool MPU6050_calibrado = false;



// Declaración de variables para sensores láser//
/////////////////////////////////////////////////
uint16_t Sensor_izq;
uint16_t Sensor_tras;
uint16_t Sensor_der;
uint16_t Sensor_del;
uint16_t Sensor_inf;
uint16_t Sensor_sup; 
// Definición de constantes para las direcciones y pines de los sensores
const uint8_t DIRECCIONES_LOX[] = {0x34, 0x31, 0x32, 0x33, 0x35, 0x36}; // Agregado 0x36 para el nuevo sensor
const uint8_t PINES_LOX[] = {17, 18, 35, 37, 41, 43}; // Agregado el pin 38 para el nuevo sensor
const uint8_t NUM_SENSORES = 6; // Incrementado a 6 para el nuevo sensor
const uint8_t RANGO_OK = 6;



// Variables para mapear señales del radiocontrol//
///////////////////////////////////////////////////
int pitch_rc, roll_rc;
int pitch_map, roll_map;
int valores_mapeados[3] = {0}; // Crear un arreglo para almacenar los valores recibidos rc



// Variables para lectura de voltaje//
///////////////////////////////////////////////////
float lectura;
float voltaje;
float voltaje2;
float r1 = 20000;
float r2 = 50000;
float vin = 3.25;



// Declaración de variables para LEDs
// Definición de los pines para los LEDs
int pinesLED[] = {2, 3, 5, 6, 21, 8, 45, 42};
int numLEDs = sizeof(pinesLED) / sizeof(pinesLED[0]);
// Definir el intervalo mínimo y máximo entre cambios de estado de LED (en milisegundos)
unsigned long intMin = 10000; // Cambia estos valores según tu preferencia
unsigned long intMax = 20000;
// Variable para almacenar el tiempo del último cambio de estado de LED
unsigned long ultCambioLED = 0;
int LEDActual = -1;


// Declaración de variables para buzzer//
/////////////////////////////////////////
#define BUZZER_PIN 7 // Definimos el pin al que está conectado el buzzer
// Notas de la escala musical en frecuencia (Hz)
const int NOTE_C4 = 262;
const int NOTE_D4 = 294;
const int NOTE_E4 = 330;
const int NOTE_F4 = 349;
const int NOTE_G4 = 392;
const int NOTE_A4 = 440;
const int NOTE_B4 = 494;
const int NOTE_C5 = 523;
// Duraciones de las notas (en milisegundos)
const int QUARTER_NOTE = 250;
const int HALF_NOTE = 500;
const int WHOLE_NOTE = 1000;
