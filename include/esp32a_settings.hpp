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

// -------------------------------------------------------------------
// Declaración de funciones
// -------------------------------------------------------------------
boolean settingsRead();
void settingsReset();
boolean settingsSave();

// -------------------------------------------------------------------
// Leer settings.json
// -------------------------------------------------------------------
boolean settingsRead(){

    DynamicJsonDocument jsonSettings(capacitySettings);
    File file = SPIFFS.open("/settings.json", "r");
    if(deserializeJson(jsonSettings, file))//si no puede deserealizar quiere decir que no existe y realiza la siguiente funcion
    {
        settingsReset();      // llamar la función que formatea a fabrica                                                                  // Tomar los valores de Fábrica
        DeserializationError error = deserializeJson(jsonSettings, file);
        log("ERROR","Falló la lectura de las configuraciones, tomando valores por defecto");
        if(error){
            log("ERROR", String(error.c_str()));
        }
        return false;
    }
    else
    {
        // -------------------------------------------------------------------
        // Dispositivo settings.json
        // -------------------------------------------------------------------
        strlcpy(device_id, jsonSettings["device_id"], sizeof(device_id)); 
        strlcpy(device_name, jsonSettings["device_name"], sizeof(device_name)); 
        strlcpy(device_user, jsonSettings["device_user"], sizeof(device_user)); 
        strlcpy(device_password, jsonSettings["device_password"], sizeof(device_password));
        // -------------------------------------------------------------------
        // WIFI Cliente settings.json
        // -------------------------------------------------------------------
        wifi_mode = jsonSettings["wifi"]["wifi_mode"];
        strlcpy(wifi_ssid, jsonSettings["wifi"]["wifi_ssid"], sizeof(wifi_ssid));
        strlcpy(wifi_password, jsonSettings["wifi"]["wifi_password"], sizeof(wifi_password));        
        wifi_ip_static = jsonSettings["wifi"]["wifi_ip_static"];
        strlcpy(wifi_ipv4, jsonSettings["wifi"]["wifi_ipv4"], sizeof(wifi_ipv4));
        strlcpy(wifi_subnet, jsonSettings["wifi"]["wifi_subnet"], sizeof(wifi_subnet));
        strlcpy(wifi_gateway, jsonSettings["wifi"]["wifi_gateway"], sizeof(wifi_gateway));
        strlcpy(wifi_dns_primary, jsonSettings["wifi"]["wifi_dns_primary"], sizeof(wifi_dns_primary));
        strlcpy(wifi_dns_secondary, jsonSettings["wifi"]["wifi_dns_secondary"], sizeof(wifi_dns_secondary));
        // -------------------------------------------------------------------
        // WIFI AP settings.json
        // -------------------------------------------------------------------        
        strlcpy(ap_ssid, jsonSettings["wifi"]["ap_ssid"], sizeof(ap_ssid));
        strlcpy(ap_password, jsonSettings["wifi"]["ap_password"], sizeof(ap_password));
        ap_chanel = jsonSettings["wifi"]["ap_chanel"];
        ap_visibility = jsonSettings["wifi"]["ap_visibility"];
        ap_connect = jsonSettings["wifi"]["ap_connect"];
        // -------------------------------------------------------------------
        // MQTT settings.json
        // -------------------------------------------------------------------
        mqtt_enable = jsonSettings["mqtt"]["mqtt_enable"];
        strlcpy(mqtt_server, jsonSettings["mqtt"]["mqtt_server"], sizeof(mqtt_server));
        mqtt_port = jsonSettings["mqtt"]["mqtt_port"];
        mqtt_retain = jsonSettings["mqtt"]["mqtt_retain"];
        mqtt_qos = jsonSettings["mqtt"]["mqtt_qos"];
        strlcpy(mqtt_id, jsonSettings["mqtt"]["mqtt_id"], sizeof(mqtt_id));
        strlcpy(mqtt_user, jsonSettings["mqtt"]["mqtt_user"], sizeof(mqtt_user));
        strlcpy(mqtt_password, jsonSettings["mqtt"]["mqtt_password"], sizeof(mqtt_password));
        mqtt_clean_sessions = jsonSettings["mqtt"]["mqtt_clean_sessions"];
        strlcpy(mqtt_willTopic, jsonSettings["mqtt"]["mqtt_willTopic"], sizeof(mqtt_willTopic));
        strlcpy(mqtt_willMessage, jsonSettings["mqtt"]["mqtt_willMessage"], sizeof(mqtt_willMessage));
        mqtt_willQoS = jsonSettings["mqtt"]["mqtt_willQoS"];
        mqtt_willRetain = jsonSettings["mqtt"]["mqtt_willRetain"];
        mqtt_time_send = jsonSettings["mqtt"]["mqtt_time_send"];
        mqtt_time_interval = jsonSettings["mqtt"]["mqtt_time_interval"];
        mqtt_status_send = jsonSettings["mqtt"]["mqtt_status_send"];
        // -------------------------------------------------------------------
        // Relays settings.json
        // -------------------------------------------------------------------
        RELAY1_STATUS = jsonSettings["relay"]["RELAY1_STATUS"];
        RELAY2_STATUS = jsonSettings["relay"]["RELAY2_STATUS"];
        // -------------------------------------------------------------------
        // Dimmer settings.json
        // -------------------------------------------------------------------
        dim = jsonSettings["dimmer"]["dim_value"];
        // cerrar el archivo
        file.close();
        log("INFO", "Lectura de las configuraciones correctamente");
        return true;
    }

}



// -------------------------------------------------------------------
// Valores de Fábrica al settings.json
// -------------------------------------------------------------------
void settingsReset(){
    // -------------------------------------------------------------------
    // Dispositivo settings.json
    // -------------------------------------------------------------------
    strlcpy(device_id, DeviceID().c_str(), sizeof(device_id));   // ESP32DC15B80C703E
    strlcpy(device_name, extractNumbers(DeviceID()).c_str(), sizeof(device_name));
    strlcpy(device_user, "admin", sizeof(device_user)); 
    strlcpy(device_password, "admin", sizeof(device_password)); 
    // -------------------------------------------------------------------
    // WIFI Cliente settings.json
    // -------------------------------------------------------------------
    wifi_mode = true; // false = AP true = cliente
    strlcpy(wifi_ssid, "Ecosat WiFi", sizeof(wifi_ssid));
    strlcpy(wifi_password, "wifi.1ng3n13r142023#", sizeof(wifi_password));
    wifi_ip_static = false; // false dhcp true fijo
    strlcpy(wifi_ipv4, "192.168.30.150", sizeof(wifi_ipv4));
    strlcpy(wifi_subnet, "255.255.255.0", sizeof(wifi_subnet));
    strlcpy(wifi_gateway, "192.168.30.1", sizeof(wifi_gateway));
    strlcpy(wifi_dns_primary, "8.8.8.8", sizeof(wifi_dns_primary));
    strlcpy(wifi_dns_secondary, "8.8.4.4", sizeof(wifi_dns_secondary));
    // -------------------------------------------------------------------
    // WIFI AP settings.json
    // -------------------------------------------------------------------    
    strlcpy(ap_ssid, DeviceID().c_str(), sizeof(ap_ssid));
    strlcpy(ap_password, "ecosat201.", sizeof(ap_password));//minimo 8 caracteres,si no ingresara sin clave
    ap_chanel = 9;         
    ap_visibility = false;     // false visible, true oculta   
    ap_connect = 2;//cantidad de conexiones permitidas (maximo 8 conexiones segun la libreria de wifi.h)
    // -------------------------------------------------------------------
    // MQTT settings.json
    // -------------------------------------------------------------------
    mqtt_enable = true;
    strlcpy(mqtt_server, "192.168.200.126", sizeof(mqtt_server));
    mqtt_port = 1883;//puerto del broker mqtt
    mqtt_retain = false;
    mqtt_qos = 1;//quiality of service
    strlcpy(mqtt_id, DeviceID().c_str(), sizeof(mqtt_id));
    strlcpy(mqtt_user, "admin", sizeof(mqtt_user));
    strlcpy(mqtt_password, "admin", sizeof(mqtt_password));
    mqtt_clean_sessions = true;
    strlcpy(mqtt_willTopic, PathMqttTopic("status").c_str(), sizeof(mqtt_willTopic));
    strlcpy(mqtt_willMessage, "{\"connected\": false}", sizeof(mqtt_willMessage));
    mqtt_willQoS = 0;
    mqtt_willRetain = false;
    mqtt_time_send = true;
    mqtt_time_interval = 60000;//intervalo de envio de mensajes 60 segundos como ejemplo se puede modificar con la API
    mqtt_status_send = false;//esta variable habilita o deshabilita el envio de mensaje
    // -------------------------------------------------------------------
    // Relays settings.json
    // -------------------------------------------------------------------        
    RELAY1_STATUS = false;
    RELAY2_STATUS = false;
    // -------------------------------------------------------------------
    // Dimmer settings.json
    // -------------------------------------------------------------------
    dim = 0;

    log("INFO","Se reiniciaron todos los valores por defecto");  
}



// -------------------------------------------------------------------
// Guardar settings.json por el API
// -------------------------------------------------------------------
boolean settingsSave(){
    // StaticJsonDocument<capacitySettings> jsonSettings;
    DynamicJsonDocument jsonSettings(capacitySettings);
    
    File file = SPIFFS.open("/settings.json", "w+");

    if (file){
        // -------------------------------------------------------------------
        // Dispositivo settings.json
        // -------------------------------------------------------------------
        jsonSettings["device_id"] = device_id;
        jsonSettings["device_name"] = device_name;
        jsonSettings["device_user"] = device_user; 
        jsonSettings["device_password"] = device_password;
        // -------------------------------------------------------------------
        // WIFI Cliente settings.json
        // -------------------------------------------------------------------
        JsonObject wifiObj = jsonSettings.createNestedObject("wifi");
        wifiObj["wifi_mode"] = wifi_mode;
        wifiObj["wifi_ssid"] = wifi_ssid;
        wifiObj["wifi_password"] = wifi_password;
        wifiObj["wifi_ip_static"] = wifi_ip_static;
        wifiObj["wifi_ipv4"] = wifi_ipv4;
        wifiObj["wifi_subnet"] = wifi_subnet;
        wifiObj["wifi_gateway"] = wifi_gateway;
        wifiObj["wifi_dns_primary"] = wifi_dns_primary;
        wifiObj["wifi_dns_secondary"] = wifi_dns_secondary;
        // -------------------------------------------------------------------
        // WIFI AP settings.json
        // -------------------------------------------------------------------
        wifiObj["ap_ssid"] = ap_ssid;
        wifiObj["ap_password"] = ap_password;
        wifiObj["ap_chanel"] = ap_chanel;
        wifiObj["ap_visibility"] = ap_visibility;
        wifiObj["ap_connect"] = ap_connect;
        // -------------------------------------------------------------------
        // MQTT settings.json
        // -------------------------------------------------------------------
        JsonObject mqttObj = jsonSettings.createNestedObject("mqtt");
        mqttObj["mqtt_enable"] = mqtt_enable;
        mqttObj["mqtt_server"] = mqtt_server;
        mqttObj["mqtt_port"] = mqtt_port;
        mqttObj["mqtt_retain"] = mqtt_retain;
        mqttObj["mqtt_qos"] = mqtt_qos;
        mqttObj["mqtt_id"] = mqtt_id;
        mqttObj["mqtt_user"] = mqtt_user;
        mqttObj["mqtt_password"] = mqtt_password;
        mqttObj["mqtt_clean_sessions"] = mqtt_clean_sessions;
        mqttObj["mqtt_willTopic"] = mqtt_willTopic;
        mqttObj["mqtt_willMessage"] = mqtt_willMessage;
        mqttObj["mqtt_willQoS"] = mqtt_willQoS;
        mqttObj["mqtt_willRetain"] = mqtt_willRetain;
        mqttObj["mqtt_time_send"] = mqtt_time_send;
        mqttObj["mqtt_time_interval"] = mqtt_time_interval;
        mqttObj["mqtt_status_send"] = mqtt_status_send; 
        // -------------------------------------------------------------------
        // Relays settings.json
        // -------------------------------------------------------------------
        JsonObject relayObj = jsonSettings.createNestedObject("relay");
        relayObj["RELAY1_STATUS"] = RELAY1_STATUS;
        relayObj["RELAY2_STATUS"] = RELAY2_STATUS;
        // -------------------------------------------------------------------
        // Dimmer settings.json
        // -------------------------------------------------------------------
        JsonObject dimmerObj = jsonSettings.createNestedObject("dimmer");
        dimmerObj["dim_value"] = dim;
        
        jsonSettings["file_version"] = "3.0.0";

        serializeJsonPretty(jsonSettings, file);
        file.close();
        log("INFO","Configuración guardada correctamente");
        serializeJsonPretty(jsonSettings, Serial);
        return true;
    }else{
        log("ERROR","Falló el guardado de la configuración");
        return false;
    }
}