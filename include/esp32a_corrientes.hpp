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
Adafruit_ADS1115 ads;

const float Valorminin          = 0.0;//Valor minimo de entrada 
const float Valormaxin          = 26666;//ESTA ES PARA UNA GANANCIA DE 0 A 5
const float ValorminCorriente   = 0.0;//Valor minimo de corriente
enum AdsChannel { ADS_A0, ADS_A1, ADS_A2, ADS_A3 };
const AdsChannel channels[] = {ADS_A0, ADS_A1, ADS_A2, ADS_A3};
// En el archivo esp32a_corrientes.hpp
extern const char* Ax[4];  // Declaración externa

// En el archivo esp32a_corrientes.cpp
const char* Ax[4] = {"A0", "A1", "A2", "A3"};  // Definición real

// Definir un array para almacenar los valores de corriente
#define MAX_NUM_VALUES  4
float corrienteArray[MAX_NUM_VALUES]; // MAX_NUM_VALUES es el número máximo de valores que deseas almacenar
static int currentIndex = 0;

void initADS()
{
        Serial.print("INICIO CORRIENTES");
        ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
        ads.begin();
}

void checkInfoCor(AdsChannel channel, float value) {
    if (currentIndex < MAX_NUM_VALUES) {
        corrienteArray[currentIndex] = value;
        Serial.print("Corriente en ");
        Serial.print(channel);
        Serial.print(": ");
        Serial.println(value);
        currentIndex++;
    }
}

int16_t getPositionADS(AdsChannel channel) {
    int16_t result = ads.readADC_SingleEnded(static_cast<int>(channel));
    Serial.println(static_cast<int>(channel));
    return result;
}

float getCurrent(float positionAds, float inMin, float inMax, float outMin, float outMax) {
    return (positionAds - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void getCorrientes() {
    currentIndex = 0;
    for (int i = 0; i < 4; ++i) {
        delay(10);  // Pequeño delay opcional para estabilización
        float positionAds = getPositionADS(channels[i]);
        float corriente = getCurrent(positionAds, Valorminin, Valormaxin, ValorminCorriente, constanteCorriente);
        checkInfoCor(channels[i], corriente);
    }
}