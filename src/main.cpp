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
File archivo;                //Utilizado para escribir en la SD los archivos que se envian
File root;                   //Accede a la raiz de la tarjeta SD
File multi;                  //Ficheros multiples
File single;                 //Ficheros simples
File contador;               //Explorar Archivos tarjeta SD
File directorios;            //Guardar nombres de archivos en JSON
int numero_ficheros;         //Numero de archivos guardados en SD
int total_ficheros = 0;      //Almacena numero total del archivos de la SD


void explorar_ficheros();
int contador_archivos();

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

  server.on("/dir.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/dir.json", "text/json");
  });

  server.on("/subida", HTTP_POST, [](AsyncWebServerRequest *request){ 
    request->send(200);}, handleUpload);

  server.begin();

  ////////////////////////////////////////////////////////////////


//contador_archivos();
explorar_ficheros();


 /*  //Configuración AUDIO
audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
audio.setVolume(10); //0...21
audio.connecttoFS(SD, "/0002.mp3"); */

}

void loop() {
  //audio.loop();
}


//Contador Numero de archivos en SD
int contador_archivos(){
  root = SD.open("/");
  File cuenta;
  numero_ficheros = 0;
  while(true){
    cuenta = root.openNextFile();
    if(!cuenta){
      //No más ficheros
      cuenta.close();
      return numero_ficheros;
    }
    numero_ficheros ++;
  }
}


//Explora los ficheros de la tarjeta SD
void explorar_ficheros(){
  numero_ficheros = contador_archivos(); //Cuenta el numero de archivos escritos en la SD
  root = SD.open("/");

  SPIFFS.remove("/dir.json");  //Limpia el archivo para ser leido

  directorios = SPIFFS.open("/dir.json", "a");
    if (!directorios) {
      Serial.println("No se pudo abrir el archivo");
    }

  if(numero_ficheros == 1){  //Cuando solo hay un archivo en la SD
    Serial.println("dentro_1");
    single = root.openNextFile();


    Serial.print("{\"titulo\":");
    directorios.print("{\"titulo\":");
    Serial.printf("\"%s\"", multi.name());
    directorios.printf("\"%s\"", multi.name());
    Serial.print(",\"size\":");
    directorios.print(",\"size\":");
    Serial.print("}");
    directorios.print("}");

    single.close();
    directorios.close();
    return;  
  }
  
  if(numero_ficheros > 1){  // Cuando hay más de un archivo
    
    Serial.print("[");
    directorios.print("[");

    for(int i = 0; i < numero_ficheros - 1; i++){
    multi = root.openNextFile();

    if(!multi){
      Serial.println("sin archivos");
      return;
    }
    Serial.print("{\"titulo\":");
    directorios.print("{\"titulo\":");
    Serial.printf("\"%s\"", multi.name());
    directorios.printf("\"%s\"", multi.name());
    Serial.print(",\"size\":");
    directorios.print(",\"size\":");
    Serial.printf("%i", multi.size());
    directorios.printf("%i", multi.size());
    Serial.print("},");
    directorios.print("},");
    }
    //Imprime el último para cerrar el format JSON
    multi = root.openNextFile();

    Serial.print("{\"titulo\":");
    directorios.print("{\"titulo\":");
    Serial.printf("\"%s\"", multi.name());
    directorios.printf("\"%s\"", multi.name());
    Serial.print(",\"size\":");
    directorios.print(",\"size\":");
    Serial.printf("%i", multi.size());
    directorios.printf("%i", multi.size());
    Serial.print("}");
    directorios.print("}");
    Serial.print("]");
    directorios.print("]");
  } 
  directorios.close(); 
  multi.close();
}

