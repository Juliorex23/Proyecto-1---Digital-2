//Universidad del Valle de Guatemala 
//Electrónica Digital 2 - BE3029
//Julio Pérez 221023
//Proyecto 1

//Librerías utilizadas en el circuito.
#include <Arduino.h>
#include "driver/ledc.h"
#include "config.h"
#include "display7seg.h"

// Definición de pines para los dígitos
#define DIS1 0
#define DIS2 2
#define DIS3 15
// Definición de segmentos 
#define sA 32
#define sB 33
#define sC 26
#define sD 25
#define sE 12
#define sF 14
#define sG 27
#define sP 13

// Definición de canales PWM para los LEDs
#define canalPWM_R 10
#define canalPWM_V 11
#define canalPWM_A 6

// Definición de canal PWM para el servo
#define canalPWM_Servo 9

// Frecuencia en Hz
#define freqPWM 50
#define freqPWMLED 50

// Resolución del duty cycle en bits
#define resPWM 10
#define resPWMLED 10

// Definición de los pines para los LEDs
#define LEDR 23
#define LEDA 22
#define LEDV 1

// Definiciones para el control del sensor de temperatura 
#define ADC_VREF_mV 3300.0
#define ADC_RESOLUCION 4096.0

// Definición de pines y variables
#define lm35Pin 36
#define BTN1 18

// Definición del pin del servo motor
#define pinServo 21

// Definición de valores PWM para el Servo
#define PWM_min 26
#define PWM_max 128

// Definiciones de PWM para los LEDs
#define PWM_minLed 0
#define PWM_maxLed 250

// Variable de control de LEDs
int currentfunction = 0;

// Variable de estado pasado para controlar el servo
int estadopas = 0;
int reloj;
float temperature; 

volatile bool enviar = false; // Bandera que indica si se debe leer el sensor

// Funciones ISR para los botones
void IRAM_ATTR onButton1Press() {
    enviar = true;
}

// Función de inicialización de PWMs
void initPWMLEDs(void);
void initPWMServo(void);

// Función para controlar el servo motor
void setServoPosition(void);

// Función para controlar el botón
void button (void);

// Funciones para controlar el display 
void mostrador();
void temporizacion();
void controlLEDs (void);

// Configuración inicial
void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.print("Connecting to Adafruit IO");
    io.connect();
    while (io.status() < AIO_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println(io.statusText());
}
void loop() {
}
