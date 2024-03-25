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

#pragma once

#include "ESPAsyncWebServer.h"
#include <Update.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");


// Firmware
size_t content_len;
#define U_PART U_SPIFFS

// Particionar el código
#include "esp32a_api.hpp"


void initServer(){
    // -------------------------------------------------------------------
    // Zona API REST
    // -------------------------------------------------------------------
    
    // -------------------------------------------------------------------
    // Parametros de configuración Index
    // url: /api/index
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/index",HTTP_GET, handleApiIndex);
    // -------------------------------------------------------------------
    // Parámetros de configuración WiFi
    // url: /api/connection/wifi
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/connection/wifi", HTTP_GET, handleApiWifi);
    // -------------------------------------------------------------------
    // Parámetros de configuración WiFi
    // url: /api/status/wifi
    // Método: GET
    // -------------------------------------------------------------------    
    server.on("/api/status/wifi", HTTP_GET, handleApistatusWifi);
    // -------------------------------------------------------------------
    // Método POST actualizar configuraciones WiFi
    // url: /api/connection/wifi
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/connection/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostWiFi);
    // -------------------------------------------------------------------
    // Escanear todas las redes WIFI al alcance de la señal
    // url: /api/connection/wifi-scan
    // Método: GET
    // Notas: La primera solicitud devolverá 0 resultados a menos que comience
    // a escanear desde otro lugar (ciclo / configuración).
    // No solicite más de 3-5 segundos. \ ALT + 92
    // -------------------------------------------------------------------
    server.on("/api/connection/wifi-scan", HTTP_GET, handleApiWifiScan);
    // -------------------------------------------------------------------
    // Parámetros de configuración MQTT
    // url: /api/connection/mqtt
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/connection/mqtt", HTTP_GET, handleApiMQTT);
        // -------------------------------------------------------------------
    // Parámetros de estatus MQTT
    // url: /api/status/mqtt
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/status/mqtt", HTTP_GET, handleApiStatusMQTT);
    // -------------------------------------------------------------------
    // Actualizar las configuraciones del MQTT Conexiones
    // url: /api/connection/mqtt
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/connection/mqtt", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostMQTT);
    // -------------------------------------------------------------------
    // Manejo de la descarga del archivo setting.json
    // url: "/api/device/download"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/device/download", HTTP_GET, handleApiDownload);
    // -------------------------------------------------------------------
    // Manejo de la carga del archivo settings.json
    // url: "/api/device/upload"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/upload", HTTP_POST, [](AsyncWebServerRequest *request)
        { opened = false; },
        [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            handleApiUpload(request, filename, index, data, len, final);
        });
    
    // -------------------------------------------------------------------
    // Manejo de la actualización del Firmware a FileSystem
    // url: /api/device/firmware
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/firmware", HTTP_POST, [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            handleApiFirmware(request, filename, index, data, len, final);
        });
    
    // -------------------------------------------------------------------
    // Manejo de parámetros de estados Globales
    // url: "/api/device/status"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/device/status", HTTP_GET, handleApiGetStatus);
    // -------------------------------------------------------------------
    // Manejo de parámetros de corriente
    // url: "/api/device/constantecorriente"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/device/constantecorriente", HTTP_GET, handleApiGetconstantecorriente);
    // -------------------------------------------------------------------
    // Manejo del reinicio
    // url: "/api/device/restart"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/restart", HTTP_POST, handleApiPostRestart);
    // -------------------------------------------------------------------
    // Manejo de la restauración
    // url: "/api/device/restore"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/restore", HTTP_POST, handleApiPostRestore);
    // -------------------------------------------------------------------
    // Actualizar las configuraciones del acceso al servidor - contraseña
    // url: /api/perfil/user
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/perfil/user", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostUser);
    // -------------------------------------------------------------------
    // On/Off los relays
    // url: /api/device/relays
    // Método: POST
    // -------------------------------------------------------------------
    // server.on("/api/device/relays", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostRelays);
    // -------------------------------------------------------------------
    // Regulador Dimmer
    // url: /api/device/dimmer
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/dimmer", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostDimmer);
    // -------------------------------------------------------------------
    // cerrar sesión
    // url: /api/esp-logout
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/esp-logout", HTTP_POST, [](AsyncWebServerRequest *request ){
        if (security){
            if (!request->authenticate(device_user, device_password))
                return request->requestAuthentication();
        } 
        request->send(401, dataType, "{ \"session\": false, \"msg\": \"Sesión cerrada correctamente\"}");
    });
    // -------------------------------------------------------------------
    // constante de corriente
    // url: /api/constantecorriente
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/constantecorriente", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostconstantecorriente);
    // -------------------------------------------------------------------
    // request de Sensores de temperatura
    // url: /api/connection/temp
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/connection/temp", HTTP_GET, handleApiTemp);
    // manejador de error 404
    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404);
        }
    });

    // -------------------------------------------------------------------
    // Iniciar el Servidor WEB + CORS
    // -------------------------------------------------------------------
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Method", "POST, GET, OPTIONS, DELETE");

    server.begin();
    log("INFO", "Servidor Asincrono HTTP iniciado");

    // -------------------------------------------------------------------
    // Manejo de parámetros de corriente
    // url: "/api/device/constantecorriente"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/device/tiposidon", HTTP_GET, handleApiGettiposidon);

    // -------------------------------------------------------------------
    // tipo de Sidon
    // url: /api/tiposidon
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/tiposidon", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPosttiposidon);
}