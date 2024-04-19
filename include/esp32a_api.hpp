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
// Zona API REST
// -------------------------------------------------------------------
// URL:                                       Método:         Estado
// -------------------------------------------------------------------
// /api/index                                   GET     --      OK
// /api/connection/wifi                         GET     --      OK
// /api/connection/wifi                         POST    --      OK
// /api/connection/wifi-scan                    GET     --      OK
// /api/connection/mqtt                         GET     --      OK
// /api/connection/mqtt                         POST    --      OK
// /api/device/download                         GET     --      OK
// /api/device/upload                           POST    --      OK
// /api/device/firmware                         POST    --      OK
// /api/device/status                           GET     --      OK
// /api/device/restart                          POST    --      OK
// /api/device/restore                          POST    --      OK
// /api/perfil/user                             POST    --      OK
// /api/device/relays                           POST    --      OK
// /api/device/dimmer                           POST    --      OK


// API v1 json desde String

bool security = true;
const char *dataType = "application/json";

// -------------------------------------------------------------------
// Parametros de configuración Index
// url: /api/index
// Método: GET
// -------------------------------------------------------------------
void handleApiIndex(AsyncWebServerRequest *request){

    // agregar el usuario y contraseña
    if(security){
        if(!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"reboots\":" + String(device_restart);
    json += ",\"activeTime\": \"" + String(longTimeStr(millis() / 1000)) + "\"";
    json += ",\"wifi\":{";
        WiFi.status() == WL_CONNECTED ? json += "\"Estado_WiFi\": \"ONLINE\" " : json += "\"Estado_WiFi\": \"OFFLINE\"";
        wifi_mode == WIFI_STA ? json += ",\"SSID_WiFi\": \"" + String(wifi_ssid) + "\"" : json += ",\"SSID_WiFi\": \"" + String(ap_ssid) + "\"";
        wifi_mode == WIFI_STA ? json += ",\"IPv4_WiFi\": \"" + ipStr(WiFi.localIP()) + "\"" : json += ",\"IPv4_WiFi\": \"" + ipStr(WiFi.softAPIP()) + "\"";
        json += ",\"MAC_WiFi\": \"" + String(WiFi.macAddress()) + "\",";
        json += "\"wifiIpStatic\":" + String(wifi_ip_static ? "true" : "false");
    json += "},";
    json += "\"mqtt\":{";
        mqttClient.connected() ? json += "\"Estado_MQTT\": \"ONLINE\" " : json += "\"Estado_MQTT\": \"OFFLINE\"";
        mqttClient.connected() ? json += ",\"Servidor_MQTT\": \"" + String(mqtt_server) + "\"" : json += ",\"Servidor_MQTT\": \"server not connected\"";
        json += ",\"Usuario_MQTT\": \"" + String(mqtt_user) + "\"";
        json += ",\"Cliente_ID_MQTT\": \"" + String(mqtt_id) + "\"";
    json += "},";
    json += "\"info\":{";
        json += "\"Identificacion\": \"" + String(device_name) + "\"";
        json += ",\"Numero_de_Serie\": \"" + DeviceID() + "\"";
        json += ",\"mDNS_Url\":  \"" + String("http://"+String(device_name)+".local/") + "\"";
        json += ",\"Direccion_IP_del_Cliente\": \"" + ipStr(request->client()->remoteIP()) + "\"";
        json += ",\"Navegador_del_Cliente\": \"" + String(request->getHeader("User-Agent")->value().c_str()) + "\"";
        json += ",\"Version_del_Firmware\": \"" + String(device_fw_version) + "\"";
        json += ",\"Version_del_Hardware\": \"" + String(device_hw_version) + "\"";
        json += ",\"CPU_FREQ_MHz\":" + String(getCpuFrequencyMhz());
        json += ",\"RAM_SIZE_KB\":" + String(ESP.getHeapSize() / 1024);
        json += ",\"SPIFFS_SIZE_KB\":" + String(SPIFFS.totalBytes() / 1024);
        json += ",\"FLASH_SIZE_MB\":" + String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);
        json += ",\"Fabricante\": \"" + String(device_manufacturer) + "\"";  
        json += ",\"Tiempo_de_Actividad_del_Sistema\": \"" + String(longTimeStr(millis() / 1000)) + "\""; 
        json += ",\"Cantidad_de_Reinicios\":" + String(device_restart);
    json += "}";
    json += ",\"spiffsUsed\":" + String(SPIFFS.usedBytes() / 1024);
    json += ",\"ramAvailable\":" + String(ESP.getFreeHeap() / 1024);
    json += ",\"cpuTemp\":" + String(TempCPUValue());
    json += ",\"relays\":[";
        json += "{";
            json += "\"name\": \"" + String("RELAY1") + "\"";
            RELAY1_STATUS ? json += ",\"status\": true" : json += ",\"status\": false"; 
        json += "},";
        json += "{";
            json += "\"name\": \"" + String("RELAY2") + "\"";
            RELAY2_STATUS ? json += ",\"status\": true" : json += ",\"status\": false"; 
        json += "}";
    json += "]"; 
    json += ",\"dimmer\":" + String(dim);   
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);

}
// -------------------------------------------------------------------
// Parametros de configuración de constante de corriente
// url: /api/constantecorriente
// Método: GET
// -------------------------------------------------------------------
void handleApiGetconstantecorriente(AsyncWebServerRequest *request){
    // agregar el usuario y contraseña
    if(security){
        if(!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    json += "\"constantecorriente\": \"" + String(constanteCorriente) + "\"";
    json += "}";

    request->addInterestingHeader("API SIDON Server");
    request->send(200, dataType, json);
}
// -------------------------------------------------------------------
// Leer parámetros de configuración WiFi
// url: /api/connection/wifi
// Método: GET
// -------------------------------------------------------------------
void handleApiWifi(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"wifi\":";
        json += "{";
        wifi_mode ? json += "\"wifi_mode\": true" : json += "\"wifi_mode\": false";
        json += ",\"wifi_ssid\": \"" + String(wifi_ssid) + "\"";
        json += ",\"wifi_password\": \"""\"";
        wifi_ip_static ? json += ",\"wifi_ip_static\": true" : json += ",\"wifi_ip_static\": false";
        json += ",\"wifi_ipv4\": \"" + String(wifi_ipv4) + "\"";
        json += ",\"wifi_gateway\": \"" + String(wifi_gateway) + "\"";
        json += ",\"wifi_subnet\": \"" + String(wifi_subnet) + "\"";
        json += ",\"wifi_dns_primary\": \"" + String(wifi_dns_primary) + "\"";
        json += ",\"wifi_dns_secondary\": \"" + String(wifi_dns_secondary) + "\"";
        json += ",\"ap_ssid\": \"" + String(ap_ssid) + "\"";
        json += ",\"ap_password\": \"""\"";
        json += ",\"ap_chanel\":" + String(ap_chanel);
        ap_visibility ? json += ",\"ap_visibility\": false" : json += ",\"ap_visibility\": true";
        json += ",\"ap_connect\":" + String(ap_connect);
        json += "},";
    json += "\"code\": 1 ";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}
// -------------------------------------------------------------------
// Leer parámetros de configuración WiFi
// url: /api/status/wifi
// Método: GET
// -------------------------------------------------------------------
void handleApistatusWifi(AsyncWebServerRequest *request){
        if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    WiFi.status() == WL_CONNECTED ? json += "\"wifiStatus\": true" : json += "\"wifiStatus\": false";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}
// -------------------------------------------------------------------
// Método POST actualizar configuraciones WiFi
// url: /api/connection/wifi
// Método: POST
// -------------------------------------------------------------------
void handleApiPostWiFi(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };
    // -------------------------------------------------------------------
    // WIFI Cliente settings.json
    // -------------------------------------------------------------------
    
    String s = "";
    // APP Mode
    wifi_mode = doc["wifi_mode"].as<bool>();
    // SSID Client
    if (doc["wifi_ssid"] != ""){
        s = doc["wifi_ssid"].as<String>();
        s.trim();
        strlcpy(wifi_ssid, s.c_str(), sizeof(wifi_ssid));
        s = "";
    }
    // Password SSID Client
    if (doc["wifi_password"] != ""){
        s = doc["wifi_password"].as<String>();
        s.trim();
        strlcpy(wifi_password, s.c_str(), sizeof(wifi_password));
        s = "";
    }
    // DHCP
    wifi_ip_static = doc["wifi_ip_static"].as<bool>();
    // IPV4
    if (doc["wifi_ipv4"] != ""){
        s = doc["wifi_ipv4"].as<String>();
        s.trim();
        strlcpy(wifi_ipv4, s.c_str(), sizeof(wifi_ipv4));
        s = "";
    }
    // Gateway
    if (doc["wifi_gateway"] != ""){
        s = doc["wifi_gateway"].as<String>();
        s.trim();
        strlcpy(wifi_gateway, s.c_str(), sizeof(wifi_gateway));
        s = "";
    }             
    // Subned
    if (doc["wifi_subnet"] != ""){
        s = doc["wifi_subnet"].as<String>();
        s.trim();
        strlcpy(wifi_subnet, s.c_str(), sizeof(wifi_subnet));
        s = "";
    } 
    // DNS Primary
    if (doc["wifi_dns_primary"] != ""){
        s = doc["wifi_dns_primary"].as<String>();
        s.trim();
        strlcpy(wifi_dns_primary, s.c_str(), sizeof(wifi_dns_primary));
        s = "";
    }
    // DNS Secondary
    if (doc["wifi_dns_secondary"] != ""){
        s = doc["wifi_dns_secondary"].as<String>();
        s.trim();
        strlcpy(wifi_dns_secondary, s.c_str(), sizeof(wifi_dns_secondary));
        s = "";
    }
    // -------------------------------------------------------------------
    // WIFI AP settings.json
    // -------------------------------------------------------------------  
    // AP SSID
    if (doc["ap_ssid"] != ""){
        s = doc["ap_ssid"].as<String>();
        s.trim();
        strlcpy(ap_ssid, s.c_str(), sizeof(ap_ssid));
        s = "";
    }          
    // AP Password
    if (doc["ap_password"] != ""){
        s = doc["ap_password"].as<String>();
        s.trim();
        strlcpy(ap_password, s.c_str(), sizeof(ap_password));
        s = "";
    }
    // AP visibility 0 Visible - 1 Oculto
    ap_visibility = !doc["ap_visibility"].as<int>(); 
    // AP Channel
    if (doc["ap_chanel"] != ""){
        ap_chanel = doc["ap_chanel"].as<int>();
    }
    // AP number of Connections
    if (doc["ap_connect"] != ""){
        ap_connect = doc["ap_connect"].as<int>();
    }
    // Save Settings.json
    if (settingsSave()){
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true }");
    }else{
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false }");
    }
}
// -------------------------------------------------------------------
// Escanear todas las redes WIFI al alcance de la señal
// url: /api/connection/wifi-scan
// Método: GET
// Notas: La primera solicitud devolverá 0 resultados a menos que comience
// a escanear desde otro lugar (ciclo / configuración).
// No solicite más de 3-5 segundos. \ ALT + 92
// -------------------------------------------------------------------
void handleApiWifiScan(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }  
log("INFO", "escanenado redes");
    String json = "";
    int n = WiFi.scanComplete();
    if (n == -2){
        json = "{";
        json += "\"meta\": { \"serial\": \"" + DeviceID() + "\", \"count\": 0},";
        json += "\"data\": [";
        json += "],";
        json += "\"code\": 0 ";
        json += "}";   
        WiFi.scanNetworks(true, true);
        log("INFO", "escanenado redes 1");
    } 
    else if (n){
        json = "{";
        json += "\"meta\": { \"serial\": \"" + DeviceID() + "\", \"count\":" + String(n) + "},";
        json += "\"data\": [";

        for (int i = 0; i < n; ++i){
            if (i)
                json += ",";
            json += "{";
            json += "\"n\":" + String(i + 1);
            json += ",\"rssi\":" + String(WiFi.RSSI(i));
            json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
            json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
            json += ",\"channel\":" + String(WiFi.channel(i));
            json += ",\"secure\":\"" + EncryptionType(WiFi.encryptionType(i)) + "\"";
            json += "}";
        }

        json += "],";
        json += "\"code\": 1 ";
        json += "}";

        WiFi.scanDelete();
        if (WiFi.scanComplete() == -2){
            WiFi.scanNetworks(true, true);
        }
        log("INFO", "escanenado redes 2");
    }

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
    log("INFO", "escanenado redes 3");
}
// -------------------------------------------------------------------
// Parámetros de configuración MQTT
// url: /api/connection/mqtt
// Método: GET
// -------------------------------------------------------------------
void handleApiMQTT(AsyncWebServerRequest *request){    
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    
    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"mqtt\":{";
        mqtt_enable ? json += "\"mqtt_enable\": true" : json += "\"mqtt_enable\": false";
        json += ",\"mqtt_server\": \"" + String(mqtt_server) + "\"";
        json += ",\"mqtt_port\":" + String(mqtt_port);
        mqtt_retain ? json += ",\"mqtt_retain\": true" : json += ",\"mqtt_retain\": false";
        json += ",\"mqtt_qos\":" + String(mqtt_qos);
        json += ",\"mqtt_id\": \"" + String(mqtt_id) + "\"";
        json += ",\"mqtt_user\": \"" + String(mqtt_user) + "\"";
        json += ",\"mqtt_password\": \"""\"";
        mqtt_clean_sessions ? json += ",\"mqtt_clean_sessions\": true" : json += ",\"mqtt_clean_sessions\": false";
        mqtt_time_send ? json += ",\"mqtt_time_send\": true" : json += "\"mqtt_time_send\": false";
        json += ",\"mqtt_time_interval\":" + String(mqtt_time_interval / 1000); // 60000 / 1000 = 30s
        mqtt_status_send ? json += ",\"mqtt_status_send\": true" : json += ",\"mqtt_status_send\": false";
    json += "},";
    json += "\"code\": 1 ";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}

// -------------------------------------------------------------------
// Parámetros de configuración MQTT
// url: /api/status/mqtt
// Método: GET
// -------------------------------------------------------------------
void handleApiStatusMQTT(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    mqttClient.connected() ? json += "\"mqttStatus\": true" : json += "\"mqttStatus\": false";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);

}
// -------------------------------------------------------------------
// Actualizar las configuraciones del MQTT Conexiones
// url: /api/connection/mqtt
// Método: POST
// -------------------------------------------------------------------
void handleApiPostMQTT(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    //  -------------------------------------------------------------------
    //  MQTT Conexión settings.json
    //  -------------------------------------------------------------------
    String s = "";
    // MQTT Enable
    mqtt_enable = doc["mqtt_enable"].as<bool>();
    // MQTT Server
    if (doc["mqtt_server"] != ""){
        s = doc["mqtt_server"].as<String>();
        s.trim();
        strlcpy(mqtt_server, s.c_str(), sizeof(mqtt_server));
        s = "";
    }
    // MQTT Port
    if (doc["mqtt_port"] != ""){
        mqtt_port = doc["mqtt_port"].as<int>();
    }
    // MQTT Retain
    mqtt_retain = doc["mqtt_retain"].as<bool>();
    // MQTT QOS 0-1-2
    mqtt_qos = doc["mqtt_qos"].as<int>();
    // MQTT ID
    if (doc["mqtt_id"] != ""){
        s = doc["mqtt_id"].as<String>();
        s.trim();
        strlcpy(mqtt_id, s.c_str(), sizeof(mqtt_id));
        s = "";
    }
    // MQTT User
    if (doc["mqtt_user"] != ""){
        s = doc["mqtt_user"].as<String>();
        s.trim();
        strlcpy(mqtt_user, s.c_str(), sizeof(mqtt_user));
        s = "";
    }
    // MQTT Password
    if (doc["mqtt_password"] != ""){
        s = doc["mqtt_password"].as<String>();
        s.trim();
        strlcpy(mqtt_password, s.c_str(), sizeof(mqtt_password));
        s = "";
    }
    // mqtt_clean_sessions
    mqtt_clean_sessions = doc["mqtt_clean_sessions"].as<bool>();    
    // -------------------------------------------------------------------
    // MQTT Envio de datos settings.json
    // -------------------------------------------------------------------
    // mqtt_time_send
    mqtt_time_send = doc["mqtt_time_send"].as<bool>();
    // mqtt_time_interval
    mqtt_time_interval = doc["mqtt_time_interval"].as<int>() * 1000; // 60 * 1000 = 60000 = 60s
    // mqtt_status_send
    mqtt_status_send = doc["mqtt_status_send"].as<bool>();
    // Save Settings.json
    if (settingsSave()){
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true }");
    }else{
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false }");
    }
}
// -------------------------------------------------------------------
// Manejo de la descarga del archivo setting.json
// url: "/api/device/download"
// Método: GET
// -------------------------------------------------------------------
void handleApiDownload(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    
    log("INFO", "Descarga del archivo settings.json");
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settings.json", dataType, true);
    request->send(response);
}
// -------------------------------------------------------------------
// Manejo de la carga del archivo settings.json
// url: "/api/device/upload"
// Método: POST
// -------------------------------------------------------------------
// Variables para la carga del archivo
File file;
bool opened = false;
void handleApiUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 

    if (!index){
        Serial.printf("INFO", "Upload Start: %s\n", filename.c_str());
    }
    // Validar el archivo si esta abierto settings.json
    if (opened == false){
        opened = true;
        file = SPIFFS.open(String("/") + filename, FILE_WRITE);
        if (!file){
            log("ERROR", "No se pudo abrir el archivo para escribir");
            request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo abrir el archivo para escribir!\"}");
            return;
        }
    }

    // Escribir el archivo en la memoria
    if (file.write(data, len) != len){
        log("ERROR", "No se pudo escribir el archivo");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo escribir el archivo: " + filename + " !\"}");
        return;
    }

    // Finalizada la carga del archivo.
    if (final){
        AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"msg\": \"¡Carga del archivo: " + filename + " completada!\"}");
        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Location", "/");
        request->send(response);
        file.close();
        opened = false;
        log("INFO", "Carga del archivo " + filename + " completada");
        // Esperar la transmisión de los datos seriales
        Serial.flush();
        // Reiniciar el ESP32
        ESP.restart();
    }
}
// -------------------------------------------------------------------
// Manejo de la actualización del Firmware a FileSystem
// url: /api/device/firmware
// Método: POST
// -------------------------------------------------------------------
void handleApiFirmware(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    // Si el nombre de archivo incluye ( spiffs ), actualiza la partición de spiffs
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
    String updateSystem = cmd == U_PART ? "FileSystem" : "Firmware";

    if (!index){
        content_len = request->contentLength();
        log("INFO", "Actualización del " + updateSystem + " iniciada...");
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)){
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el " + updateSystem + " Index: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
            log("ERROR", "Update del " + updateSystem + " ternimado");
        }
    }
    // escribir e firmware o el filesystem
    if (Update.write(data, len) != len){
        Update.printError(Serial);
    }
    // Terminado
    if (final){
        if (!Update.end(true)){
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el " + updateSystem + " Final: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
        }else{
            AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"type\": \"" + updateSystem + "\"}");
            response->addHeader("Cache-Control", "no-cache");
            response->addHeader("Location", "root@" + updateSystem + "");
            request->send(response);
            log("INFO", "Update del " + updateSystem + " completado");
            // Esperar la Transmisión de los datos seriales
            Serial.flush();
            ESP.restart();
        }
    }
}
// -------------------------------------------------------------------
// Manejo de parámetros de estados Globales
// url: /api/device/status
// Método: GET
// -------------------------------------------------------------------
void handleApiGetStatus(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 

    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";    
    json += ",\"code\": 1 ";
    json += "}";
    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}
// -------------------------------------------------------------------
// Manejo del reinicio del dispositivo
// url: /api/device/restart
// Método: POST
// -------------------------------------------------------------------
void handleApiPostRestart(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 
    // Retornamos la respuesta
    request->send(200, dataType, "{ \"restart\": true }");
    // Reiniciar el ESP32
    String origin = "API";
    restart(origin);
}
// -------------------------------------------------------------------
// Manejo de la restauración del dispositivo
// url: /api/device/restore
// Método: POST
// -------------------------------------------------------------------
void handleApiPostRestore(AsyncWebServerRequest *request){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 
    // Retornamos la respuesta
    request->send(200, dataType, "{ \"restore\": true }");
    // Reiniciar el ESP32
    String origin = "API";
    restore(origin);
}
// -------------------------------------------------------------------
// Actualizar las configuraciones del acceso al servidor - contraseña
// url: /api/perfil/user
// Método: POST
// -------------------------------------------------------------------
void handleApiPostUser(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    // -------------------------------------------------------------------
    // Contraseña settings.json
    // -------------------------------------------------------------------
    String p, np, cp;
    // capturamos las variables enviadas en el JSON
    p = doc["device_old_password"].as<String>();    // Contraseña Actual
    np = doc["device_new_password"].as<String>();   // Nueva Contraseña
    cp = doc["device_c_new_password"].as<String>(); // Confirmación de nueva Contraseña
    // Limpiamos de espacios vacios
    p.trim();
    np.trim();
    cp.trim();

    // Validar que los datos de lacontraseña anterior no este en blanco
    if(p != ""){
        // validar que la contraseña coincida con la anterior
        if( p == device_password ){
            if(np == "" && cp == ""){
                request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se permite los datos de la nueva contraseña y confirmación vacíos!\"}");
                return;
            } else if(np != "" && cp != "" && np == cp){
                if(np == device_password){
                    request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, La contraseña nueva no puede ser igual a la anterior\"}");
                    return;
                }
                strlcpy(device_password, np.c_str(), sizeof(device_password));
                if (settingsSave()) {
                    request->send(200, dataType, "{ \"save\": true, \"msg\": \"¡Contraseña actualizada correctamente!\" }");
                } else {
                    request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡ Internar server error !\" }");
                }
            }else if( np !=  cp ){
                request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, La nueva contraseña y confirmación de nueva contraseña no coinciden!\"}");
                return;
            }else{}
        }else{
           AsyncWebServerResponse *response = request->beginResponse(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo guardar, la contraseña anterior no coincide\"}");
            request->send(response);
            return;
        }
    }else{
        request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se permite contraseña anterior en blanco\"}");
        return;
    }
}
// -------------------------------------------------------------------
// Manejo de las salidas a relay
// url: /api/device/relays
// Método: POST
// -------------------------------------------------------------------
// void handleApiPostRelays(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
//     if (security){
//         if (!request->authenticate(device_user, device_password))
//             return request->requestAuthentication();
//     } 

//     // capturamos la data en string
//     String bodyContent = GetBodyContent(data, len);
//     // Validar que sea un JSON
//     StaticJsonDocument<384> doc;
//     DeserializationError error = deserializeJson(doc, bodyContent);
//     if (error){
//         request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
//         return;
//     };

//     if(OnOffRelays(bodyContent)){
//         // Retornamos la respuesta
//         if(settingsSave())
//             request->send(200, dataType, "{ \"relay\": true, \"name\":  \"" + doc["output"].as<String>() + "\", \"status\": true }"); 
//     }else{
//         if(settingsSave())
//             request->send(200, dataType, "{ \"relay\": true, \"name\":  \"" + doc["output"].as<String>() + "\", \"status\": false }");
//     }
// }
// -------------------------------------------------------------------
// Manejo del dimmer
// url: /api/device/dimmer
// Método: POST
// -------------------------------------------------------------------
void handleApiPostDimmer(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 
    // capturamos la data en string
    String bodyContent = GetBodyContent(data, len);
    // Validar que sea un JSON
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    dimmer(bodyContent);
    request->send(200, dataType, "{ \"dimmer\": true, \"value\": \"" + String(dim) + "\" }");
}
// -------------------------------------------------------------------
// Manejo de la constante de corriente
// url: /api/device/constantecorriente
// Método: POST
// -------------------------------------------------------------------
void handleApiPostconstantecorriente(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 
    // capturamos la data en string
    String bodyContent = GetBodyContent(data, len);
    // Validar que sea un JSON
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    constanteCorriente = doc["constantecorriente"];
    request->send(200, dataType, "{ \"Save\": true, \"value\": \"" + String(constanteCorriente) + "\" }");
}
// -------------------------------------------------------------------
// Manejo de request sensores de temperatura
// url: /api/connection/temp
// Método: POST
// -------------------------------------------------------------------
void handleApiTemp(AsyncWebServerRequest *request) {
    if (security) {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    } 

    InitDigitalTemperature();
    
    // Imprimir el valor de deviceCount para verificar si se detectan los sensores correctamente
    Serial.print("Número de sensores detectados: ");
    Serial.println(deviceCount);

    DynamicJsonDocument jsonDoc(1024);
    // Crear un array JSON para almacenar las direcciones de los sensores
    JsonArray sensorArray = jsonDoc.to<JsonArray>();

    // Agregar las direcciones de los sensores al array JSON
    for (int i = 0; i < deviceCount; i++) {
        JsonObject sensor = sensorArray.createNestedObject();
        sensor["GPIO"] = "IO27";
        sensor["MAC"] = macAddresses[i];
    }

    // Enviar la respuesta JSON con las direcciones de los sensores
    String jsonString;
    serializeJson(sensorArray, jsonString);
    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, (deviceCount != 0 ? jsonString : "No se detectan sensores"));

}
