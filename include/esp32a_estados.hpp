/* -------------------------------------------------------------------
 ;* ECOSAT- ECOSAT 2024
 ;* Correo: agonzalez@ecosat.com.mx
 ;* Plataforma: SIDON 2.9.0
 ;* Framework:  Arduino - Platformio - VSC
 ;* Proyecto: Panel Administrativo 
 ;* Nombre: SIDON 2.0
 ;* Autor: Ing. ANDRE GONZALEZ
 ;* -------------------------------------------------------------------
*/
int Gpio[5] = {5,17,16,4};
int valoresDigitales[5];
String pin[5] = {"SE1","SE2","SE3","SE4"};
const int numPins = 4;
int lastState[numPins];

void SensorEstados(){
  for (int i = 0; i < numPins; i++) {
    valoresDigitales[i] = digitalRead(Gpio[i]); // Leer el estado digital de cada pin
    lastState[i] = digitalRead(Gpio[i]);
  }
}

void initSensorEstados()
{
    for (int i = 0; i < numPins; i++) {
    pinMode(Gpio[i], INPUT); // Establecer los pines como entradas digitales
  }
}





