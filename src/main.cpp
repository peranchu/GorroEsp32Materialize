#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Audio.h>

//Ficheros de la carpeta include
#include "audio.hpp"
#include "config.h"
#include "server.hpp"
#include "Esp32_utils.hpp"
#include "archivos.hpp"
#include "websocket.hpp"

/////////////////////////////////////////

bool EstadoReproActual;     //Estado de la Reproducción de la SD
bool SD_present;            //Almacena Si la SD Está Presente


void setup() {
  Serial.begin(115200);

  pinMode(19, INPUT_PULLUP);   //PIN MISO PULL_UP Tarjeta SD

//Inicio Sistema de Archivos SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("Error SPIFFS");
    return;
  }
//////////////////////////////////////

//Inicio Sistema de Archivos SD
  Serial.print(F("iniciando Sd card..."));
  if(!SD.begin(5)){
    Serial.println(F("Error SD"));
    SD_present = false;
  }
  else{
    Serial.println(F("Sd iniciada..."));
    SD_present = true;
  }
  /////////////////////////////////////

  ConnectWiFi_STA();         //INICIA CONEXIÓN WiFi y MDNS

  InitWebSocket();          //INICIA LOS SERVICIOS WEBSOCKET

  InitServer();            //INICIA SERVIDOR

  audioConfig();          //CONFIGURACIÓN SISTEMA DE AUDIO

}


void loop() {
  audio.loop();

  if(volumenActual != volumenAnterior){
    audio.setVolume(volumenActual);
    volumenAnterior = volumenActual;
    Serial.println(volumenActual);
  }

  if (audio.isRunning() == 1 && EstadoReproActual == false){
    EstadoReproActual = true;
  }

  if(audio.isRunning() == 0 && EstadoReproActual == true){
    EstadoReproActual = false;
    EstadoRepro();   //Envía el estado de reproducción al cliente
  }
}





