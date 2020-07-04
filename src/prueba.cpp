 /* if(contador_archivos() == 1){  //Cuando solo hay un archivo en la SD
    Serial.println("dentro_1");
    File single = root.openNextFile();
    StaticJsonDocument<57> archivo;

    directorios = SPIFFS.open("/dir.json", FILE_WRITE);
    if (!directorios) {
      Serial.println("No se pudo abrir el archivo");
    }

    archivo["titulo"] = single.name();
    archivo["size"] = single.size(); 

    serializeJson(archivo, Serial);

    single.close();
    directorios.close();
    return;
  } */

  /* if(contador_archivos() > 1){
    
    Serial.println("dentro");
    for(int i = 0; i < contador_archivos(); i++){
      multi = root.openNextFile();

      if(!multi){
        //Sin archivos
        Serial.println("No hay archivos");
        return;
      }
      //Varios Archivos en la tarjeta SD
      directorios = SPIFFS.open("/dir.txt", "a");
      if (!directorios) {
        Serial.println("No se pudo abrir el archivo");
      }

      Serial.println(i);
      JsonObject posicionesFicheros[i] = doc.createNestedObject();
      posicionesFicheros[i] ["titulo"] = multi.name();
      posicionesFicheros[i] ["size"] = multi.size();

      serializeJson(doc, Serial);

      if (!directorios) {
        Serial.println("No se pudo abrir el archivo");
      }
    }
    multi.close();
    directorios.close();
  }  
} */