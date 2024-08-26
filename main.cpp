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
//Configuración de canales de Adafruit 
// Set up the 'counter' feed
AdafruitIO_Feed *Canal1 = io.feed("temperature");



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

//Definiciones para el control del sensor de temperatura 
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

//Variable de control de LEDs
int currentfunction = 0;

//Variable de estado pasado para controlar el servo
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

//Función para controlar el botón 

void button (void);

//Funciones para controlar el display 
void mostrador();
void temporizacion();
void controlLEDs (void);


// Configuración inicial
void setup() {
    // Start the serial connection
  Serial.begin(115200);

  // Wait for serial monitor to open
  while (!Serial);

  Serial.print("Connecting to Adafruit IO");

  // Connect to io.adafruit.com
  io.connect();

  // Wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // We are connected
  Serial.println();
  Serial.println(io.statusText());
    //Configura los pines 
    configurarDisplay(sA, sB,sC, sD, sE, sF, sG, sP);
    pinMode (DIS1, OUTPUT);
    pinMode (DIS2, OUTPUT);
    pinMode (DIS3, OUTPUT);
    pinMode (lm35Pin, INPUT);

    pinMode (LEDR, OUTPUT);
    pinMode (LEDA, OUTPUT);
    pinMode (LEDV, OUTPUT);

    // Configura el PWM de las LEDs
    initPWMLEDs();

    //Conigura las funciones auxiliares
    button();
    initPWMServo();
    mostrador();
    temporizacion();
    controlLEDs ();
    setServoPosition();

}

void loop() {
    temporizacion ();
    if (enviar) {
        enviar = false; // Resetea el estado del botón

        // Leer el valor del ADC del canal del LM35
        temperature = analogRead(lm35Pin);
        
        float milivolt = temperature *(ADC_VREF_mV/ADC_RESOLUCION);

        temperature = milivolt /10;
    

        // Mostrar la temperatura en el monitor serial
        Serial.print("Temperatura: ");
        Serial.print(temperature);
        Serial.println(" °C");

        
        // Controlar los LEDs según la temperatura
        if (temperature < 37.0) {
            // Temperatura < 37.0°C: Enciende LED verde
            currentfunction = 1;
            estadopas = 1;
        } else if (temperature >= 37.0 && temperature <= 37.5) {
            // 37.0°C <= Temperatura <= 37.5°C: Enciende LED amarillo 
            currentfunction = 2;
            estadopas = 2;
        } else {
            // Temperatura > 37.5°C: Enciende LED rojo
            currentfunction = 3;
            estadopas = 3;
        }
        controlLEDs ();
        

        // Save count to the 'counter' feed on Adafruit IO
        Serial.print("sending -> ");
        Serial.println();
        Canal1->save(temperature);
  

        // Ajustar la posición del servo motor según la temperatura

    }
    setServoPosition();
    

    

}


//Inicializa el botón 
void button(void) {
  pinMode(BTN1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN1), onButton1Press, FALLING);
}

//Inicializació de PWM para el Servo Motor
void initPWMServo(void) {
    ledcSetup(canalPWM_Servo, freqPWM, resPWM);
    ledcAttachPin(pinServo, canalPWM_Servo);
    ledcWrite(canalPWM_Servo, PWM_min); // Inicializa el servo en la posición mínima
}

// Inicialización de PWM para los LEDs
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

//Función para controlar la LED que debe encenderse en función de la temperatura medida 

void controlLEDs(void) {
    switch (currentfunction) {
        case 0:
            ledcWrite(canalPWM_R, PWM_minLed); // Apaga LED rojo
            ledcWrite(canalPWM_A, PWM_minLed); // Apaga LED azul
            ledcWrite(canalPWM_V, PWM_minLed);  // Enciende LED verde
            break;
        case 1:
            ledcWrite(canalPWM_V, PWM_maxLed); // Enciende LED verde
            ledcWrite(canalPWM_R, PWM_minLed); // Apaga LED rojo
            ledcWrite(canalPWM_A, PWM_minLed);  //Apaga LED amarillo
            break;
        case 2:
            ledcWrite(canalPWM_R, PWM_minLed); // Apaga LED rojo
            ledcWrite(canalPWM_V, PWM_minLed); // Apaga LED verde
            ledcWrite(canalPWM_A, PWM_maxLed);  // Enciende LED amarillo
            break;
        case 3:
            ledcWrite(canalPWM_R, PWM_maxLed); // Enciende LED rojo
            ledcWrite(canalPWM_V, PWM_minLed); // Apaga LED verde
            ledcWrite(canalPWM_A, PWM_minLed); // Apaga LED amarillo
            break;
        default:

            break;
    }
}

//Función de PWM para cambiar la posición del servo en función de la temperatura medida 
void setServoPosition(void) {
    int pwmValue;

    if (estadopas == 1){
        // Temperatura < 37.0°C: Posición del servo entre 121° a 180°
        pwmValue = 111;
    } else if (estadopas ==2) {
        // 37.0°C <= Temperatura <= 37.5°C: Posición del servo entre 61° a 120°
        pwmValue = 77;
    } else if (estadopas == 3){
        // Temperatura > 37.5°C: Posición del servo entre 0° a 60°
        pwmValue = 43;
    }
    else if (estadopas == 0){
        pwmValue = 26;
    }


    ledcWrite(canalPWM_Servo, pwmValue);
    Serial.print ("\n");
    Serial.print (pwmValue);
}

//Inicializa las funciones para controlar los valores del display 

void mostrador (){

//Obtener la parte entera de la temperatura
int parte_entera = int(temperature);

//Obtener el decimal de la temperatura
int decimal = int((temperature - parte_entera) * 10 + 0.5);

//Obtener la decena y la unidad de la parte entera
int decena = (parte_entera/10);
int unidad = (parte_entera % 10);

  //Mostrar en display digito decena
   
   desplegarDisplay(decena);
   desplegarPunto(0);
   digitalWrite(DIS1, HIGH);
   delay(1);
   digitalWrite(DIS1, LOW);



    //Mostrar en display unidad
   desplegarDisplay(unidad);
   desplegarPunto (1);
   digitalWrite(DIS2, HIGH);
   delay(1);
   digitalWrite(DIS2, LOW);

   //Mostrar en display decimal 
   
   desplegarDisplay(decimal);
   desplegarPunto(0);
   digitalWrite(DIS3, HIGH);
   delay(1);
   digitalWrite(DIS3, LOW);  

}

void temporizacion() 
{
    //Función para crear el efecto de multiplexación del display 

   int contret=1000;       //Cargue con 1000 la variable CONTRET
   while (contret>0)       //Mientras que la variable CONTRET sea mayor que cero
   {
      mostrador();        //Llamar la rutina mostrador
      contret--;        // Decremente la variable CONTRET  
   }
}

