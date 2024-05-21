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
DeviceAddress deviceAddress; // Esta variable almacenará la dirección MAC de cada dispositivo.

int deviceNum   = 0;//variable que enumera los dispositivon en el serial
const int maxDevices = 10; // Número máximo de dispositivos que deseas almacenar.
DeviceAddress deviceAddresses[maxDevices]; // Array para almacenar las direcciones MAC.
String macAddresses[maxDevices];  // Escanea los dispositivos y almacena sus direcciones MAC en un array de strings.
float temperaturesC[maxDevices]; // Array para almacenar las temperaturas en grados centigrados
float temperaturesF[maxDevices]; // Array para almacenar las temperaturas en farenheit
int deviceCount = 0; // Variable para contar los dispositivos encontrados.
void InitDigitalTemperature()
{
    // Limpiar variables antes de escanear nuevamente
  deviceNum = 0;
  memset(deviceAddresses, 0, sizeof(deviceAddresses));
  memset(macAddresses, 0, sizeof(macAddresses));
  
  sensors.begin();
  deviceCount = sensors.getDeviceCount();//manda a llamar la funcion de la libreria dallas y retorna elnumero de dispositivos conectados al bus de datos
  
  // locate devices on the bus
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println(">>>>>>>>>>>>>>>>>>>>>OBTENIENDO DIRECCIONES DE SENSORES DE TEMPERATURA>>>>>>>>>>>>>>>>>>>");
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.print("LOCALIZANDO DISPOSITIVOS...");
  if(deviceCount != 0)
  {
    Serial.print("SE DETECTARON ");
    Serial.print(deviceCount, DEC);
    Serial.println(" SENSORES DE TEMPERATURA.");
  }
  else
  {
    Serial.println("NO SE DETECTARON SENSORES DE TEMPREATURA");
  }

  if(deviceCount > maxDevices)
  {
    Serial.println("NO SE DETECTARON SENSORES DE TEMPREATURA");
  }

  delay(100);
   

  // Escanea los dispositivos y almacena sus direcciones MAC con prefijo "IO27,"
  for (int i = 0; i < maxDevices; i++)
  {
    if (sensors.getAddress(deviceAddress, i))
    {
      deviceNum++;
      memcpy(deviceAddresses[i], deviceAddress, 8);

      // Construye la dirección MAC con el prefijo "IO27,"
      // String macWithPrefix = "IO27,";
      String macWithPrefix = "";
      for (int j = 0; j < 8; j++)
      {
        if (deviceAddress[j] < 16)
        {
          macWithPrefix += "0";
        }
        macWithPrefix += String(deviceAddress[j], HEX);
      }

      // Almacena la dirección MAC con prefijo en el array de strings
      macAddresses[i] = macWithPrefix;

      // Imprime la dirección MAC con prefijo
      Serial.print("Dispositivo ");
      Serial.print(deviceNum);
      Serial.print(" - Dirección MAC: ");
      Serial.println(macWithPrefix);
    }
    else
    {
      break;
    }
  }

  // Ahora tienes la cantidad de dispositivos encontrados en 'deviceCount' y sus direcciones MAC en 'deviceAddresses'.

  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>    SECCION TEMPERATURA TERMINADO    >>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}



void GetTemperature()
{
  // ... (código previo)

  for (int i = 0; i < maxDevices; i++)
  {
    // ... (código previo para obtener las direcciones MAC)

    // Lee la temperatura del dispositivo y guárdala en el array
    float tempC,tempF;
    if (sensors.requestTemperaturesByIndex(i))
    {
      tempC = sensors.getTempCByIndex(i);
      temperaturesC[i] = tempC;
      tempF = sensors.getTempFByIndex(i);
      temperaturesF[i] = tempF;
    }
  }
  // ... (código posterior)
}





