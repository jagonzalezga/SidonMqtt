/* -------------------------------------------------------------------
 ;* ECOSAT- ECOSAT 2023
 ;* Correo: agonzalez@ecosat.com.mx
 ;* Plataforma: SIDON 2.2
 ;* Framework:  Arduino - Platformio - VSC
 ;* Proyecto: Panel Administrativo 
 ;* Nombre: SIDON 2.0
 ;* Autor: Ing. ANDRE GONZALEZ
 ;* -------------------------------------------------------------------
;*/
int Gpio[5] = {15, 13, 14, 16, 17};
int valoresDigitales[5];
String pin[5] = {"SE1","SE2","SE3","SE4","SE5"};
const int numPins = 5;
int lastState[numPins];

void SensorEstados(){
  for (int i = 0; i < 5; i++) {
    valoresDigitales[i] = digitalRead(Gpio[i]); // Leer el estado digital de cada pin
    lastState[i] = digitalRead(Gpio[i]);
  }
}

void initSensorEstados()
{
    for (int i = 0; i < 5; i++) {
    pinMode(Gpio[i], INPUT); // Establecer los pines como entradas digitales
  }
}





