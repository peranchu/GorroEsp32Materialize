#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Audio.h>

#define I2S_DOUT 25 //DIN
#define I2S_BCLK 27 //Bit Clock
#define I2S_LRC  26 //Left Right Clock

Audio audio;


bool SD_present;
File archivo;      //Utilizado para escribir en la SD los archivos que se envian
File root;        //Explorar Archivos tarjeta SD
File directorios; //Guardar nombres de archivos en JSON

void explorar_ficheros(File dir, int numTabs);

const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2);
DynamicJsonDocument doc(capacity);

const char *ssid = "Bocarral";
const char *pass = "TexucaLana72";

AsyncWebServer server (80);

//Manejo de la carga de Archivos
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    if(!filename.startsWith("/")) filename = "/" + filename;
    Serial.println("UploadStart: " + filename);
  
    //request->_tempFile = SD.open(filename, FILE_WRITE);
    archivo = SD.open(filename, FILE_WRITE);
  }
  if(len){
    Serial.println("escribiendo...");
    //request->_tempFile.write(data, len);
    archivo.write(data, len);
  }

  if(final){
    Serial.println("UploadEnd: " + filename + "," + index + "," + len);
    //request->_tempFile.close();
    archivo.close();

    request->send(200, "text/plain", "Archivo Subido");
  }
}
/////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  //SPI.begin(18,19,23);

  pinMode(19, INPUT_PULLUP);

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

//Inicio conexión WiFi
  WiFi.begin(ssid, pass);

  while (WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.println("Conectando..");
  }

  Serial.println(WiFi.localIP());

////////////////////////////////////////////////////

//Servir archivos desde el servidor
  server.on("/materialize.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/materialize.min.css", "text/css");
  });

  server.on("/icon.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/icon.css", "text/css");
  });

  server.on("/materialize.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/materialize.min.js", "text/javascript");
  });

////////////////////////////////////////
server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/upload.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/upload.js", "text/javascript");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/dir.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/dir.txt", "text/text");
  });

  server.on("/subida", HTTP_POST, [](AsyncWebServerRequest *request){ 
    request->send(200);}, handleUpload);

  server.begin();

  ////////////////////////////////////////////////////////////////

root = SD.open("/");
explorar_ficheros(root, 0);


 /*  //Configuración AUDIO
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(10); //0...21
audio.connecttoFS(SD, "/0002.mp3"); */

}

void loop() {
  //audio.loop();
}

//Explora los ficheros de la tarjeta SD
void explorar_ficheros(File dir, int numTabs){
  
  SPIFFS.remove("/dir.txt");  //Borra el contenido existente
  

  while(true){
    File entry = dir.openNextFile();
    if(!entry){
      //No hay más ficheros
      break;
    }
    /* for (uint8_t i = 0; i < numTabs; i++){
      Serial.print('\t');
    } */
    
    directorios = SPIFFS.open("/dir.txt", "a");
    //Formatea el mensaje a guardar
    Serial.printf("%s\n", entry.name());
    directorios.printf("%s\n", entry.name());
  
    if(entry.isDirectory()){
      Serial.println("/");
      explorar_ficheros(entry, numTabs + 1);
    }else{
      //los archivos tienen tamaño, los directorios no
      //Serial.printf("%i\n", entry.size());
      //directorios.printf("%i\n", entry.size());
      //Serial.print("\t");
      //Serial.println(entry.size(), DEC);
    }
    entry.close();
    directorios.close();
  }
}