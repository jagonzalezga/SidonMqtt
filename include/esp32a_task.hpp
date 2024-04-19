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
// -------------------------------------------------------------------
// Declaración de funciones
// -------------------------------------------------------------------
void TaskWifiReconnect(void *pvParamenters);
void TaskMqttReconnect(void *pvParamenters);
void TaskMQTTLed(void *pvParameters);

// -------------------------------------------------------------------
// Tarea Loop WIFI & Reconectar modo Cliente
// -------------------------------------------------------------------
void TaskWifiReconnect(void *pvParamenters){
  (void) pvParamenters;
  while(1){
    vTaskDelay(10/portTICK_PERIOD_MS);
    if(wifi_app == WIFI_STA){
      wifiLoop();
    } else if(wifi_app == WIFI_AP){
      wifiAPLoop();
    }else{}
  }
}
// -------------------------------------------------------------------
// Tarea Loop MQTT & Reconectar
// -------------------------------------------------------------------
void TaskMqttReconnect(void *pvParamenters){
  (void) pvParamenters;
  while(1){
    if ((WiFi.status() == WL_CONNECTED) && (wifi_app == WIFI_AP_STA))
    {
      if(mqtt_server != 0)
      {
        // llamar la función del loop mqtt
        mqttloop();
        // Enviar por MQTT el JSON
        if(mqttClient.connected() && mqtt_status_send)//checa si esta habilitado el envio y si esta conectado 
        {
          if(millis()-lasMsg > mqtt_time_interval)//aqui comparamos el tiempo en este caso 60 segundos o podemos modificarlo 
          {
            lasMsg = millis();
            mqtt_publish();
            log("INFO", "Mansaje enviado por MQTT...");
          }
        }
      }
    }    
  }
}
// -------------------------------------------------------------------
// Tarea MQTT LED pestañeo
// -------------------------------------------------------------------
// void TaskMQTTLed(void *pvParameters){
//   (void) pvParameters;
//   while(1){    
//     vTaskDelay(10/portTICK_PERIOD_MS);
//     if(mqtt_enable && mqttClient.connected()){
//       digitalWrite(MQTTLED, HIGH);
//       vTaskDelay(50/portTICK_PERIOD_MS);
//       digitalWrite(MQTTLED, LOW);
//       vTaskDelay(1000/portTICK_PERIOD_MS);              
//     }else{
//       digitalWrite(MQTTLED, LOW);
//     }       
//   }
// }

// -------------------------------------------------------------------
// Tarea ESTADOS
// -------------------------------------------------------------------
void TaskEstados(void *pvParameters){
  (void) pvParameters;
  

  while(1){    
    // Verifica cada pin para detectar cambios de estado
    for (int i = 0; i < numPins; i++) 
    {
      vTaskDelay(100/portTICK_PERIOD_MS);
      int currentState = digitalRead(Gpio[i]);

      // Compara el estado actual con el estado anterior
      if (currentState != lastState[i]) 
      {
        // Si hay un cambio, imprime en el monitor serial
        Serial.print("Gpio ");
        String pin_ = String(pin[i]);
        Serial.print(pin_);
        Serial.print(" cambió a estado ");
        Serial.println(currentState);

        // Actualiza el estado anterior
        lastState[i] = currentState;
        mqtt_publishEst(pin_,currentState);
      }
  } 
  }
}


