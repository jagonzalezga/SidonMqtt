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
void handleCommand(byte* payload, unsigned int length);
void handleGetStatus(DynamicJsonDocument& doc);
void handleRestart(DynamicJsonDocument& doc);
void handleFirmwareUpdate(byte* payload, unsigned int length);
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
            log("ERROR","MQTT - Falló la suscripción al tópico 'command'"); 
        }

        String topic_firmware = PathMqttTopic_sub("firmware");
        topic_firmware.toCharArray(topic, 150);
        if (mqttClient.subscribe(topic, mqtt_qos)) {
            log("INFO", "Suscrito al tópico: " + String(topic));
        } else {
            log("ERROR", "MQTT - Falló la suscripción al tópico 'firmware'");
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
void callback(char* topic, byte* payload, unsigned int length) {
    String str_topic = String(topic);

    if (str_topic == "Sidon/command") {
        // Manejar comandos JSON
        handleCommand(payload, length);
    } else if (str_topic == "Sidon/firmware") {
        // Manejar actualización de firmware binaria
        handleFirmwareUpdate(payload, length);
    } else {
        log("WARNING", "Tópico desconocido: " + str_topic);
    }
}

void handleCommand(byte* payload, unsigned int length) {
    // Imprime el payload recibido para depuración
    Serial.print("Payload recibido: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Define el tamaño adecuado para el documento JSON
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(1) + 100; // Ajusta el tamaño según tus necesidades
    DynamicJsonDocument doc(capacity);

    // Analiza el payload MQTT en el documento JSON
    DeserializationError error = deserializeJson(doc, payload, length);

    // Verifica si hubo un error al analizar el JSON
    if (error) {
        Serial.print(F("[Error] al analizar JSON: "));
        Serial.println(error.c_str());
        return;
    }

    // Verifica el comando en el JSON
    const char* command = doc["command"];

    if (strcmp(command, "getStatus") == 0) {
        handleGetStatus(doc);
    } else if (strcmp(command, "restart") == 0) {
        handleRestart(doc);
    } else {
        Serial.println("Comando no válido.");
    }
}

void handleGetStatus(DynamicJsonDocument& doc) {
    log("INFO", "Comando getStatus recibido");
    // Verifica si el JSON contiene la clave "devices"
    if (doc.containsKey("devices")) {
        JsonArray devices = doc["devices"].as<JsonArray>();
        for (const JsonVariant& device : devices) {
            // Comprueba si devices contiene el mismo DeviceID que el dispositivo o si devices contiene "*"
            if ((device.as<String>() == DeviceID()) || (devices.size() == 1 && devices[0] == "*")) {
                // Realiza la acción que deseas cuando se encuentra una coincidencia
                log("INFO", "Se encontró una coincidencia con el deviceid.");
                mqtt_publish();
            }
        }
    } else {
        log("INFO", "No se encontró la clave 'devices' en el JSON.");
    }
}

void handleRestart(DynamicJsonDocument& doc) {
    log("INFO", "Comando restart recibido");
    // Verifica si el JSON contiene la clave "devices"
    if (doc.containsKey("devices")) {
        JsonArray devices = doc["devices"].as<JsonArray>();
        for (const JsonVariant& device : devices) {
            // Comprueba si devices contiene el mismo DeviceID que el dispositivo o si devices contiene "*"
            if ((device.as<String>() == DeviceID()) || (devices.size() == 1 && devices[0] == "*")) {
                // Realiza la acción que deseas cuando se encuentra una coincidencia
                log("INFO", "Se encontró una coincidencia con el deviceid.");
                log("INFO", "Reseteando sidon");
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                ESP.restart();
            }
        }
    } else {
        log("INFO", "No se encontró la clave 'devices' en el JSON.");
    }
}
// -------------------------------------------------------------------
// Función para actualizar firmware a tarves de MQTT
// -------------------------------------------------------------------
void handleFirmwareUpdate(byte* payload, unsigned int length) {
    log("INFO", "Iniciando actualización de firmware...");

    if (Update.begin(length)) {
        size_t written = Update.write(payload, length);
        if (written == length) {
            log("INFO", "Escribiendo firmware...");
        } else {
            log("ERROR", "Falló la escritura del firmware");
            Update.end();
            return;
        }
        if (Update.end(true)) {
            log("INFO", "Actualización completa");
            ESP.restart();
        } else {
            log("ERROR", "Error en la actualización: " + String(Update.getError()));
        }
    } else {
        log("ERROR", "Error al iniciar la actualización: " + String(Update.getError()));
    }
}
// -------------------------------------------------------------------
// Manejo de los Mensajes Salientes y mensajes de sensores de estado
// ------------------------------------------------------------------- 
void mqtt_publish(){
    String topic = PathMqttTopic("device");
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
String Json() {
    String response;
    DynamicJsonDocument jsonDoc(3000);

    // Llamadas a funciones de sensor
    GetTemperature();
    getCorrientes();
    SensorEstados();

    // Configuración básica del JSON
    jsonDoc["CodigoMDC"] = DeviceID();
    jsonDoc["VersionFw"] = device_fw_version;
    jsonDoc["VersionHw"] = device_hw_version;
    jsonDoc["tiempoActivo"] = longTimeStr(millis() / 1000);
    JsonArray dataObj = jsonDoc.createNestedArray("lecturas");

    // Temperaturas
    for (int i = 0; i < deviceCount; i++) {
        JsonObject tempObj = dataObj.createNestedObject();
        tempObj["Valor"] = temperaturesC[i];
        tempObj["GPIO"] = "IO27";
        tempObj["MAC"] = macAddresses[i];
    }

    // Corrientes adc1
    if (activeADC1) {
    for (int i = 0; i < 4; i++) {
        JsonObject currentObj = dataObj.createNestedObject();
        currentObj["Valor"] = corrienteArray[i];
        currentObj["GPIO"] = Ax[i];
    }
    }

    // Corrientes adc2
    if (activeADC2) {
    for (int i = 4; i < 8; i++) {
        JsonObject currentObj = dataObj.createNestedObject();
        currentObj["Valor"] = corrienteArray[i];
        currentObj["GPIO"] = Ax[i];
    }
    }

    // Estados Digitales
    for (int i = 0; i < 5; i++) {
        JsonObject stateObj = dataObj.createNestedObject();
        stateObj["Valor"] = valoresDigitales[i];
        stateObj["GPIO"] = String("SE") + (i + 1);
    }

    // Serialización y limpieza
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