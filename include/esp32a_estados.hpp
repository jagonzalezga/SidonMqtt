int Gpio[5] = {4, 15, 13, 17, 16};
int valoresDigitales[5];
void SensorEstados(){
  for (int i = 0; i < 5; i++) {
    valoresDigitales[i] = digitalRead(Gpio[i]); // Leer el estado digital de cada pin
  }
}

void initSensorEstados()
{
    for (int i = 0; i < 5; i++) {
    pinMode(Gpio[i], INPUT); // Establecer los pines como entradas digitales
  }
}