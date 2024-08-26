//Universidad del Valle de Guatemala 
//Electrónica Digital 2 - BE3029
//Julio Pérez 221023
//Proyecto 1

//Librerías utilizadas en el circuito.
#include <Arduino.h>
#include "driver/ledc.h"
#include "config.h"
#include "display7seg.h"

// Definiciones de pines y configuración
#define DIS1 0
#define DIS2 2
#define DIS3 15
#define sA 32
#define sB 33
#define sC 26
#define sD 25
#define sE 12
#define sF 14
#define sG 27
#define sP 13
#define canalPWM_R 10
#define canalPWM_V 11
#define canalPWM_A 6
#define canalPWM_Servo 9
#define freqPWM 50
#define freqPWMLED 50
#define resPWM 10
#define resPWMLED 10
#define LEDR 23
#define LEDA 22
#define LEDV 1
#define ADC_VREF_mV 3300.0
#define ADC_RESOLUCION 4096.0
#define lm35Pin 36
#define BTN1 18
#define pinServo 21
#define PWM_min 26
#define PWM_max 128
#define PWM_minLed 0
#define PWM_maxLed 250

int currentfunction = 0;
int estadopas = 0;
int reloj;
float temperature; 

volatile bool enviar = false; 

void IRAM_ATTR onButton1Press() {
    enviar = true;
}

void initPWMLEDs(void) {
    ledcSetup(canalPWM_R, freqPWMLED, resPWMLED);
    ledcAttachPin(LEDR, canalPWM_R);
    ledcWrite(canalPWM_R, 0);
    ledcSetup(canalPWM_V, freqPWMLED, resPWMLED);
    ledcAttachPin(LEDV, canalPWM_V);
    ledcWrite(canalPWM_V, 0);
    ledcSetup(canalPWM_A, freqPWMLED, resPWMLED);
    ledcAttachPin(LEDA, canalPWM_A);
    ledcWrite(canalPWM_A, 0);
}

void initPWMServo(void) {
    ledcSetup(canalPWM_Servo, freqPWM, resPWM);
    ledcAttachPin(pinServo, canalPWM_Servo);
    ledcWrite(canalPWM_Servo, PWM_min);
}
// Funciones de display
void mostrador() {
    int parte_entera = int(temperature);
    int decimal = int((temperature - parte_entera) * 10 + 0.5);
    int decena = (parte_entera / 10);
    int unidad = (parte_entera % 10);

    desplegarDisplay(decena);
    desplegarPunto(0);
    digitalWrite(DIS1, HIGH);
    delay(1);
    digitalWrite(DIS1, LOW);

    desplegarDisplay(unidad);
    desplegarPunto(1);
    digitalWrite(DIS2, HIGH);
    delay(1);
    digitalWrite(DIS2, LOW);

    desplegarDisplay(decimal);
    desplegarPunto(0);
    digitalWrite(DIS3, HIGH);
    delay(1);
    digitalWrite(DIS3, LOW);  
}

void temporizacion() {
    int contret = 1000;
    while (contret > 0) {
        mostrador();
        contret--;
    }
}

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
    pinMode (DIS1, OUTPUT);
    pinMode (DIS2, OUTPUT);
    pinMode (DIS3, OUTPUT);
    pinMode (lm35Pin, INPUT);
    pinMode (LEDR, OUTPUT);
    pinMode (LEDA, OUTPUT);
    pinMode (LEDV, OUTPUT);
    initPWMLEDs();
    button();
    initPWMServo();
}

void loop() {
    temporizacion();
    if (enviar) {
        enviar = false;
        temperature = analogRead(lm35Pin);
        float milivolt = temperature * (ADC_VREF_mV / ADC_RESOLUCION);
        temperature = milivolt / 10;

        Serial.print("Temperatura: ");
        Serial.print(temperature);
        Serial.println(" °C");

        if (temperature < 37.0) {
            currentfunction = 1;
            estadopas = 1;
        } else if (temperature >= 37.0 && temperature <= 37.5) {
            currentfunction = 2;
            estadopas = 2;
        } else {
            currentfunction = 3;
            estadopas = 3;
        }
        controlLEDs();
        Canal1->save(temperature);
    }
    setServoPosition();
}
