/* -------------------------------------------------------------------
 ;* ECOSAT- ECOSAT 2024
 ;* Correo: agonzalez@ecosat.com.mx
 ;* Plataforma: SIDON 2.7
 ;* Framework:  Arduino - Platformio - VSC
 ;* Proyecto: Panel Administrativo 
 ;* Nombre: SIDON 2.0
 ;* Autor: Ing. ANDRE GONZALEZ
 ;* -------------------------------------------------------------------
*/
#include <Adafruit_ADS1X15.h>
// Variables para controlar qué ADCs están activos
bool activeADC1 = true;
bool activeADC2 = true;
Adafruit_ADS1115 ads;  // Address of ADC1
Adafruit_ADS1115 ads2; // Address of ADC2

const float Valorminin = 0.0; // Valor mínimo de entrada
const float Valormaxin = 26666; // ESTA ES PARA UNA GANANCIA DE 0 A 5
const float ValorminCorriente = 0.0; // Valor mínimo de corriente

#define MAX_NUM_VALUES  8
extern const char* Ax[MAX_NUM_VALUES];  // Declaración externa
const char* Ax[] = {"A0", "A1", "A2", "A3","A4", "A5", "A6", "A7"};  // Definición real
#define TOTAL_CHANNELS 8
float corrienteArray[TOTAL_CHANNELS]; // Array para almacenar los valores de corriente


// Configuración inicial de los ADCs
void initADS() {
    ads.setGain(GAIN_TWOTHIRDS);
    ads.begin(0x48);
    Serial.println("ADC1 activado");
    
    ads2.setGain(GAIN_TWOTHIRDS);
    ads2.begin(0x49);
    Serial.println("ADC2 activado");
}

// Función para leer el valor de cualquier canal como si fueran uno solo
int16_t readADC(int channel) {
    if (channel < 4) {
        // Leer del primer ADC
        return ads.readADC_SingleEnded(channel);
    } else {
        // Leer del segundo ADC, ajustando el canal
        return ads2.readADC_SingleEnded(channel - 4);
    }
}

// Calcula la corriente basada en la posición del ADC y los valores de escala
float getCurrent(float positionAds, float inMin, float inMax, float outMin, float outMax) {
    return (positionAds - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

// Función para procesar un canal específico
void processChannel(int channel) {
    float positionAds = readADC(channel);
    float corriente = getCurrent(positionAds, Valorminin, Valormaxin, ValorminCorriente, constanteCorriente);
    corrienteArray[channel] = corriente;

    Serial.print("Corriente en canal ");
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(corriente);
}

void getCorrientes() {
    for (int channel = 0; channel < TOTAL_CHANNELS; ++channel) {
        processChannel(channel);
    }
}