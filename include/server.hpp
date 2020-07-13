//
//____________CONFIGURACIÓN SERVIDOR//_______________________________

File archivo;   //Utilizado para escribir en la SD los archivos que se envian
     

AsyncWebServer server (80);     // INSTANCIA Y PUERTO SERVIDOR


//===================MANEJO CARGA DE ARCHIVOS A LA SD===============================

//Función llamada desde el botón de envío de Formulario
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    if(!filename.startsWith("/")) filename = "/" + filename;  //coloca el caracter "/" al archivo para poder ser escrito en la SD 
    Serial.println("UploadStart: " + filename);
  
    archivo = SD.open(filename, FILE_WRITE);     //prepara el archvio para ser escrito
  }
  if(len){
    Serial.println("escribiendo...");
    archivo.write(data, len);    //Escribe el archivo en la SD
  }

  if(final){
    Serial.println("UploadEnd: " + filename + "," + index + "," + len);
    archivo.close();
    request->send(200, "text/plain", "Archivo Subido");  // Devuelve al cliente "OK"
  }
}
//========================FIN MANEJO CARGA DE ARCHIVOS A LA SD=================================


///////////////ARCHIVOS SERVIDOS//////////////////////////////
void InitServer(){

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
    request->send(200);}, handleUpload);  // Manejo de las peticiones de subida de archivos al servidor


  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(400, "text/plain", "Not found");
  });


  //Inicio Servidor
  server.begin();
  Serial.println("HTTP Server Iniciado");
}
//=========================FIN ARCHIVOS SERVIDOS=============================