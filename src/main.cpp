#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Audio.h>

#include "config.h"
#include "server.hpp"
#include "Esp32_utils.hpp"
#include "archivos.hpp"
#include "websocket.hpp"

/////////////////////////////////////////

////////////PINES AUDIO MAX98357A/////////////////////////////
#define I2S_DOUT 25   //DIN
#define I2S_BCLK 27   //Bit Clock
#define I2S_LRC  26   //Left Right Clock

Audio audio;


bool SD_present;   //Almacena Si la SD Está Presente


void setup() {
  Serial.begin(115200);

  pinMode(19, INPUT_PULLUP);   //PIN CS Tarjeta SD

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

  ConnectWiFi_STA();         //INICIA CONEXIÓN WiFi

  InitWebSocket();          //INICIA LOS SERVICIOS WEBSOCKET

  InitServer();            //INICIA SERVIDOR

  //explorar_ficheros();    //PRIMERA EXPLORACIÓN ARCHIVOS SD


 /*  //Configuración AUDIO
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(10); //0...21
audio.connecttoFS(SD, "/0002.mp3"); */
}

void loop() {
  //audio.loop();
}




