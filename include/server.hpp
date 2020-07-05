//Configuración SERVIDOR

File archivo;                //Utilizado para escribir en la SD los archivos que se envian
     //Almacena numero total del archivos de la SD


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



void InitServer(){

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
  Serial.println("HTTP Server Iniciado");
}