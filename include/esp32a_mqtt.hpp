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

#include <PubSubClient.h>//libreria encargada de la comunicacion mqtt

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char topic[150];

String mqtt_data = "";

// -------------------------------------------------------------------
// TIEMPOS DE RECONEXION DE MQTT
// -------------------------------------------------------------------
long lastMqttReconnectAttempt = 0; 
long lasMsg = 0;

// -------------------------------------------------------------------
// DEFINICION DE FUNCIONES
// -------------------------------------------------------------------
boolean mqtt_connect();
void callback(char *topic, byte *payload, unsigned int length);
void mqtt_publish();
String Json();
void mqttloop();
String JsonEST(String pin_,int currentState_);

// -------------------------------------------------------------------
// MQTT Connect
// -------------------------------------------------------------------
boolean mqtt_connect(){
    mqttClient.setServer(mqtt_server, mqtt_port);//definicion del server con el servidor(host) y puerto
    mqttClient.setCallback(callback);//encargado de las peticiones que llegan del broker ya sea comandos u otras definiciones
    log("MQTT", "Intentando conexión al Broker MQTT ...");
    // https://pubsubclient.knolleary.net/api.html  
    // https://www.amebaiot.com/zh/rtl8722dm-arduino-api-pubsubclient/

    // boolean connect (clientID)  
    // boolean connect (clientID, willTopic, willQoS, willRetain,willMessage)  
    // boolean connect (clientID, username, password)  
    // boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage)  
    // * boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage, cleanSession) 

    /* 
        Parámetros id: ID de cliente, un identificador de cadena único 
        usuario: nombre de usuario para autenticación, valor predeterminado NULL 
        pass: contraseña para autenticación, valor predeterminado NULL 
        willTopic: el tema que utilizará el mensaje 
        willQoS: la calidad del servicio que utilizará el mensaje will 
        willRetain: si el testamento debe publicarse con el indicador de retención 
        willMessage: la carga del mensaje del testamento
        cleanSession: Si cleansession se establece en true , se eliminarán todos los temas suscritos
    */
    String topic_publish = PathMqttTopic("willmsg");//define el path completo para la conexion emqx/ESP329B1C52100C3D/status usuario/clientid/comando y aqui se publica el json con todas las variables del estatus de lo que se monitoreara
    topic_publish.toCharArray(mqtt_willTopic, 150);//mensaje de ultima voluntad

    if(mqttClient.connect(mqtt_id, mqtt_user, mqtt_password, mqtt_willTopic, mqtt_willQoS, mqtt_willRetain, mqtt_willMessage, mqtt_clean_sessions)){
        log("INFO","Conectado al Broker MQTT");
        vTaskDelay(10/portTICK_PERIOD_MS);
        log("debug", mqtt_willTopic);
        String deviceID = DeviceID();
        // Construir la cadena JSON utilizando std::string
        String jsonMessage = "{\"connected\": true,\"Device\":" + deviceID + "}";
        mqttClient.publish(mqtt_willTopic,jsonMessage.c_str(), mqtt_retain );
        jsonMessage = "";

        mqttClient.setBufferSize(1024*5);//ya que la liberia te trunca a solo 256 bytes se puede aumentar el buffer con esta funcion para que realice el envio de datos
        log("INFO", "Buffer MQTT Size: " +String(mqttClient.getBufferSize())+ " Bytes");

        String topic_subscribe = PathMqttTopic_sub("command");//como la parte de arriba aqui se suscribe al path con la ruta especificada con la terminacion /command
        topic_subscribe.toCharArray(topic, 150);//aqui pasamos lo que se guardo en topic_subscribe a topic a una arreglo de caracteres de tama;o 150
        // Nos suscribimos a comandos Topico: v1/device/usuario/dispositivo/comando
        // boolean subscribe (topic)
        // * boolean subscribe (topic, [qos])
        // qos - optional the qos to subscribe at (int: 0 or 1 only)
        // Función para suscribirnos al Topico
        if(mqttClient.subscribe(topic, mqtt_qos)){
            log("INFO","Suscrito al tópico: " + String(topic));
        }else{
            log("ERROR","MQTT - Falló la suscripción"); 
        }

    }
    else
    {
        log("ERROR","MQTT - Falló, código de error = " + String(mqttClient.state()));
        return (0);
    }
    return (1);
}
// -------------------------------------------------------------------
// Manejo de los Mensajes Entrantes --- controlar los relays
// -------------------------------------------------------------------
void callback(char *topic, byte *payload, unsigned int length){
  // Define el tamaño adecuado para el documento JSON
  const size_t capacity = length + 50; // Ajusta el tamaño según tus necesidades

  // Crea un DynamicJsonDocument con el tamaño adecuado
  DynamicJsonDocument doc(capacity);

  // Analiza el payload MQTT en el documento JSON
  DeserializationError error = deserializeJson(doc, payload, length);

  // Verifica si hubo un error al analizar el JSON
  if (error) {
    Serial.print(F("Error al analizar JSON: "));
    Serial.println(error.c_str());
    return;
  }

// Verifica si el valor de "command" es igual a "getStatus"
if (doc["command"] == "getStatus") 
{
    // Realiza la acción que deseas cuando el comando es "getStatus"
    Serial.println("Comando getStatus recibido");
    // Verifica si el JSON contiene la clave "devices"
    if (doc.containsKey("devices")) 
    {
        JsonArray devices = doc["devices"].as<JsonArray>();
        for (const JsonVariant& device : devices) 
        {  //Comprueba si devices contiene el mismo DeviceID que el dispositivo o Comprueba si "devices" contiene solo un elemento con valor "*"
            if ((device.as<String>() == DeviceID()) || (devices.size() == 1 && devices[0] == "*")) {
                // Realiza la acción que deseas cuando se encuentra una coincidencia
                Serial.println("Se encontró una coincidencia con  el deviceid.");
                // Puedes realizar la acción deseada aquí
                mqtt_publish();
            }
        }
    }
    else 
    {
        Serial.println("No se encontró la clave 'devices' en el JSON.");
    }
}
else if (doc["command"] == "restart")
{
    // Realiza la acción que deseas cuando el comando es "getStatus"
    Serial.println("Comando restart recibido");
    // Verifica si el JSON contiene la clave "devices"
    if (doc.containsKey("devices")) 
    {
        JsonArray devices = doc["devices"].as<JsonArray>();
        for (const JsonVariant& device : devices) 
        {  //Comprueba si devices contiene el mismo DeviceID que el dispositivo o Comprueba si "devices" contiene solo un elemento con valor "*"
            if ((device.as<String>() == DeviceID()) || (devices.size() == 1 && devices[0] == "*")) {
                // Realiza la acción que deseas cuando se encuentra una coincidencia
                Serial.println("Se encontró una coincidencia con  el deviceid.");
                Serial.println("Reseteando sidon");
                vTaskDelay(3000/portTICK_PERIOD_MS);
                ESP.restart();
            }
        }
    }
    else 
    {
        Serial.println("No se encontró la clave 'devices' en el JSON.");
    }
}
else 
{
    Serial.println("Comando no válido.");
}
 // Limpia el documento JSON si es necesario
  doc.clear();

//   // Ahora puedes acceder a los datos del JSON de manera dinámica
//   //const char *value = doc["command"]; // Reemplaza "clave" por la clave que necesites
//   //Serial.println(value);


//   // Comprueba si el valor de "command" es igual a "getStatus"
//   if (doc["command"] == "getStatus") {
//     // Realiza la acción que deseas cuando el comando es "getStatus"
//     // Por ejemplo, puedes enviar una respuesta MQTT o realizar alguna operación específica.
//     Serial.println("Comando getStatus recibido");
//     if
//     mqtt_publish();
    
//     // Puedes acceder a otros campos del JSON si es necesario, por ejemplo, "devices".
//     // const char *devices = doc["devices"];
  
    // String command = "";
    // String str_topic(topic);

    // for(int16_t i = 0; i < length; i++){
    //     command += (char)payload[i];
    //     // TODO: pestañeo de led MQTT

    // }   

    // command.trim();
    // log("INFO","MQTT Tópico  --> " + str_topic);
    // log("INFO","MQTT Mensaje --> " + command);
    
    // // TODO: responder al MQTT el estado de los relays
    // if(command=="")
    // OnOffRelays(command);

}
// -------------------------------------------------------------------
// Manejo de los Mensajes Salientes y mensajes de sensores de estado
// ------------------------------------------------------------------- 
void mqtt_publish(){
    String topic = PathMqttTopic("status");
    log("MQTT", topic);

    mqtt_data = Json();
    mqttClient.publish(topic.c_str(), mqtt_data.c_str(), mqtt_retain);
    mqtt_data = "";
}
void mqtt_publishEst(String pin, int currentState){
    String topic = PathMqttTopic("EST");
    log("MQTT", topic);

    mqtt_data = JsonEST(pin,currentState);
    mqttClient.publish(topic.c_str(), mqtt_data.c_str(), mqtt_retain);
    mqtt_data = "";
}
// -------------------------------------------------------------------
// JSON con información del Dispositivo para envio por MQTT
// ------------------------------------------------------------------- 
String Json(){
    String response;
    DynamicJsonDocument jsonDoc(3000);
    JsonObject device[deviceCount];
    GetTemperature();
    getCorrientes();
    SensorEstados();
    // Crear JSON
    jsonDoc["CodigoMDC"]          = DeviceID();//numero de serie del sidon 
    //jsonDoc["deviceManufacturer"] = String(device_manufacturer);//FABRICANTE
    jsonDoc["VersionFw"]    = device_fw_version;//FECHA DE CREACION/MODIFICACION DEL FIRMWARE
    jsonDoc["VersionHw"]    = String(device_hw_version);//VERSION DEL HARDWARE
    //jsonDoc["deviceSdk"]          = String(ESP.getSdkVersion());
    jsonDoc["tiempoActivo"]   = longTimeStr(millis() / 1000);//tiempo activo desde que se encendio el sidon
    JsonArray dataObj = jsonDoc.createNestedArray("lecturas");
    // -------------------------------------------------------------------
    //SECCION DE TEMPERATURAS 
    // -------------------------------------------------------------------
    for (int i = 0; i < deviceCount; i++)
    {
        device[i] = dataObj.createNestedObject();
        device[i] ["Valor"]  = temperaturesC[i];
        device[i] ["GPIO"]   = "IO27";
        device[i] ["MAC"]    = macAddresses[i];
    }

    // -------------------------------------------------------------------
    //SECCION DE CORRIENTES
    // -------------------------------------------------------------------
    JsonObject device8            = dataObj.createNestedObject();
    device8["Valor"] = corrienteArray[0];
    device8["GPIO"]  = Ax[0];
    JsonObject device9            = dataObj.createNestedObject();
    device9["Valor"] = corrienteArray[1];
    device9["GPIO"]  = Ax[1] ;
    JsonObject device10            = dataObj.createNestedObject();
    device10["Valor"] = corrienteArray[2];
    device10["GPIO"]  = Ax[2];
    JsonObject device11            = dataObj.createNestedObject();
    device11["Valor"] = corrienteArray[3];
    device11["GPIO"]  = Ax[3];
    currentIndex = 0;//variable que se usa en la parte de corrientes
    // -------------------------------------------------------------------
    //SECCION DE ESTADOS
    // -------------------------------------------------------------------
    JsonObject device12            = dataObj.createNestedObject();
    device12["Valor"] = valoresDigitales[0];
    device12["GPIO"]  = "SE1";
    JsonObject device13            = dataObj.createNestedObject();
    device13["Valor"] = valoresDigitales[1];
    device13["GPIO"]  = "SE2";
    JsonObject device14            = dataObj.createNestedObject();
    device14["Valor"] = valoresDigitales[2];
    device14["GPIO"]  = "SE3";
    JsonObject device15            = dataObj.createNestedObject();
    device15["Valor"] = valoresDigitales[3];
    device15["GPIO"]  = "SE4";
    JsonObject device16            = dataObj.createNestedObject();
    device16["Valor"] = valoresDigitales[4];
    device16["GPIO"]  = "SE5"; 
       
	// dataObj["deviceRamAvailable"] = ESP.getFreeHeap();
	// dataObj["deviceRamSizeKb"]    = ESP.getHeapSize();
    // dataObj["deviceRelay1Status"] = RELAY1_STATUS ? true : false;
	// dataObj["deviceRelay2Status"] = RELAY2_STATUS ? true : false;
	// dataObj["deviceDimmer"]       = dim;
	// dataObj["deviceCpuTemp"]      = TempCPUValue();
	// dataObj["deviceDS18B20TempC"] = temperatureC;
    // dataObj["deviceDS18B20TempF"] = temperatureF;
    
    // dataObj["deviceSpiffsUsed"]   = String(SPIFFS.usedBytes());
    // dataObj["deviceCpuClock"]     = String(getCpuFrequencyMhz());
    // dataObj["deviceFlashSize"]    = String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);
    // dataObj["deviceRestartS"]     = String(device_restart);
	// dataObj["mqttStatus"]         = mqttClient.connected() ? true : false;
	// dataObj["mqttServer"]         = mqttClient.connected() ? F(mqtt_server) : F("server not connected");
	// dataObj["wifiStatus"]         = WiFi.status() == WL_CONNECTED ? true : false;
	// dataObj["wifiRssiStatus"]     = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
	// dataObj["wifiQuality"]        = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0;
    
    serializeJson(jsonDoc, response);
    jsonDoc.clear();
    return response;
}
// -------------------------------------------------------------------
// Json encargado de generar el mensaje de sensores de estado
// -------------------------------------------------------------------
String JsonEST(String pin_,int currentState_){
    String response;
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["CodigoMDC"]          = DeviceID();//numero de serie del sidon 
    jsonDoc["VersionFw"]    = device_fw_version;//FECHA DE CREACION/MODIFICACION DEL FIRMWARE
    jsonDoc["VersionHw"]    = String(device_hw_version);//VERSION DEL HARDWARE
    jsonDoc["tiempoActivo"]   = longTimeStr(millis() / 1000);//tiempo activo desde que se encendio el sidon
    JsonArray dataObj = jsonDoc.createNestedArray("lecturas");
    JsonObject device1            = dataObj.createNestedObject();
    device1["Valor"] = currentState_;
    device1["GPIO"]  = pin_;
    serializeJson(jsonDoc, response);
    Serial.println(response);
    jsonDoc.clear();
    return response;
}
// -------------------------------------------------------------------
// MQTT Loop Principal
// -------------------------------------------------------------------
void mqttloop(){
    if (mqtt_enable){
        if (!mqttClient.connected())
        {
            long now = millis();
            if ((now < 60000) || ((now - lastMqttReconnectAttempt) > 120000))
            {
                lastMqttReconnectAttempt = now;
                if(mqtt_connect())
                {
                    lastMqttReconnectAttempt = 0;
                }
            }            
        }
        else
        {
            mqttClient.loop();//aqui escucha todas las conexiones
        }        
    }    
}