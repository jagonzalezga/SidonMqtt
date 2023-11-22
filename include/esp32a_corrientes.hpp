Adafruit_ADS1115 ads;

const float Valorminin          = 0.0;//Valor minimo de entrada 
const float Valormaxin          = 26666;//ESTA ES PARA UNA GANANCIA DE 0 A 5
const float ValorminCorriente   = 0.0;//Valor minimo de corriente

String      Ax[4]               = {"A0", "A1", "A2", "A3"};//Pines de adc
String      output;
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

void checkInfoCor(String Ax,float value)
{

    // Asegurarte de que no excedas el tamaño máximo del array
    if (currentIndex < MAX_NUM_VALUES) 
    {
        corrienteArray[currentIndex] = value;
        Serial.println(corrienteArray[currentIndex]);
        currentIndex++;
    }

}

int16_t getPositionADS(String Address)
{
    if(Address == "A0")
    {
        return ads.readADC_SingleEnded(0);
         Serial.println("0");
    }
    else if(Address == "A1")
    {
        return ads.readADC_SingleEnded(1);
        Serial.println("1");
    }
    else if(Address == "A2")
    {
        return ads.readADC_SingleEnded(2);
        Serial.println("2");
    }
    else if(Address == "A3")
    {
        return ads.readADC_SingleEnded(3);
         Serial.println("3");
    }
    else{Serial.println("error con el adc"); return 0;}
}

float getCurrent(float positionads,float inMin,float inMax,float outMin,float outMax)
{
    
    float corriente;
   corriente =(positionads - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
   return corriente;
}

void getCorrientes()
{
    for(auto i: Ax) 
    {
        
        float corriente = getCurrent(getPositionADS(i), Valorminin, Valormaxin, ValorminCorriente, constanteCorriente);
        checkInfoCor(i, corriente);
    }
}