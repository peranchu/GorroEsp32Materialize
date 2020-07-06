//
//____________CONFIGURACIÓN SERVIDOR//_______________________________

File archivo;   //Utilizado para escribir en la SD los archivos que se envian
     

AsyncWebServer server (80);     // INSTANCIA Y PUERTO SERVIDOR

//Manejo de la carga de Archivos
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    if(!filename.startsWith("/")) filename = "/" + filename;
    Serial.println("UploadStart: " + filename);
  
    archivo = SD.open(filename, FILE_WRITE);
  }
  if(len){
    Serial.println("escribiendo...");
    archivo.write(data, len);
  }

  if(final){
    Serial.println("UploadEnd: " + filename + "," + index + "," + len);
    archivo.close();
    request->send(200, "text/plain", "Archivo Subido");
  }
}
/////////////////////////////////////////////



void InitServer(){

///////////////ARCHIVOS SERVIDOS//////////////////////////////

//ARCHIVOS DEL FRAMEWORK MATERIALIZE
  server.on("/materialize.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/materialize.min.css", "text/css");
  });

  server.on("/icon.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/icon.css", "text/css");
  });

  server.on("/materialize.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/materialize.min.js", "text/javascript");
  });

//ARCHIVOS DE LAS PETICIONES DEL CLIENTE///////////////////////////////
server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/dir.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/dir.json", "text/json");
  });

  server.on("/subida", HTTP_POST, [](AsyncWebServerRequest *request){  
    request->send(200);}, handleUpload);

  server.begin();
  Serial.println("HTTP Server Iniciado");
}