
//MANEJO DE ARCHIVOS EN LA SD

File root;                   //Accede a la raiz de la tarjeta SD
File multi;                  //Ficheros multiples
File single;                 //Ficheros simples
File contador;               //Explorar Archivos tarjeta SD
File directorios;            //Guardar nombres de archivos en JSON
int numero_ficheros;         //Numero de archivos guardados en SD
int total_ficheros = 0; 

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
    Serial.printf("%3.2f", float((multi.size()) / 1048576.0));
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
    Serial.printf("%3.2f", float((multi.size()) / 1048576.0));
    directorios.printf("%i", multi.size());
    Serial.print("}");
    directorios.print("}");
    Serial.print("]");
    directorios.print("]");
  } 
  directorios.close(); 
  multi.close();
}