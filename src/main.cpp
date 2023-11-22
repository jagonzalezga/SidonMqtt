/* -------------------------------------------------------------------
 ;* ECOSAT- ECOSAT 2023
 ;* Correo: agonzalez@ecosat.com.mx
 ;* Plataforma: SIDON 2.7
 ;* Framework:  Arduino - Platformio - VSC
 ;* Proyecto: Panel Administrativo 
 ;* Nombre: SIDON 2.0
 ;* Autor: Ing. ANDRE GONZALEZ
 ;* -------------------------------------------------------------------
;*/

#include <Arduino.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <WIFI.h>
#include <DNSServer.h>
#include <ESPmDNS.h> 
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_SSD1306.h>
// -------------------------------------------------------------------
// Configuración del sensor de temperatura DS18B20
// -------------------------------------------------------------------
#define ONE_WIRE_BUS 27
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// -------------------------------------------------------------------
// Archivos *.hpp - Fragmentar el Código
// -------------------------------------------------------------------
#include "esp32a_definitions_variables.hpp"
#include "esp32a_estados.hpp"
#include "esp32a_functions.hpp"
#include "esp32a_settings.hpp"
#include "esp32a_wifi.hpp"
#include "esp32a_temperatura.hpp"
#include "esp32a_corrientes.hpp"
#include "esp32a_mqtt.hpp"
#include "esp32a_server.hpp"
#include "esp32a_task.hpp"
#include "esp32a_oled.hpp"


void setup() {
  Serial.begin(115200);
  log("INFO", "Iniciando el Setup");
  EEPROM.begin(256);                                          // Memoria EEPROM init 256 bytes
  EEPROM.get(Restart_Address, device_restart);                // Leer el valor de la memoria
  device_restart++;                                           // Aumentar el contador
  EEPROM.put(Restart_Address, device_restart);                // Guardar el valor a la memoria
  EEPROM.commit();
  EEPROM.end();
  log("INFO", "Cantidad de reinicios :" + String(device_restart));
  // inicializar el spiffs
  if(!SPIFFS.begin(true)){
    log("ERROR", "Falló la inicialización del SPIFFS");
    while(true);
  }
  // traer las configuraciones desde json
  if(!settingsRead()){
    settingsSave();
  }
  // definir los pines y setearlos
  gpioDefine();
  //inicializacion de los dispositivos de temperatura
  InitDigitalTemperature();
  //inicializacion de los dispositivos de corriente
  initADS();
  // Paso estados a los pines de los Relays
  setOnOffSingle(RELAY1, RELAY1_STATUS);
  setOnOffSingle(RELAY2, RELAY2_STATUS);
  // Pasar el dato del dimmer
  ledcWrite(ledChannel, dim * 2.55); // dim => 0 - 100 
  // iniciar el wifi
  wifi_setup();
  // iniciar la pantalla oled
  initDisplay(); 
  // inicializar el servidor
  initServer();

  // Crear Tarea Reconexión WIFI
  xTaskCreate(TaskWifiReconnect, "TaskWifiReconnect", 1024*6, NULL, 2, NULL);
  // Crear Tarea Reconexión MQTT
  xTaskCreate(TaskMqttReconnect, "TaskMqttReconnect", 1024*6, NULL, 2, NULL);
  // LED MQTT Task
  xTaskCreate( TaskMQTTLed, "TaskMQTTLed", 2048, NULL, 1, NULL);
 

}

unsigned long mtime = 0;
void loop() {
  // every 10 seconds
  if(millis()-mtime > 10000 )
  {
    mtime = millis();
    textDraw();
  }
}