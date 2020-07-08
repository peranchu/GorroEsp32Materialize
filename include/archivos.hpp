
//
//////////////MANEJO DE ARCHIVOS EN LA SD //////////////////////////

File root;                   //Accede a la raiz de la tarjeta SD
File multi;                  //Ficheros multiples
File single;                 //Ficheros simples
File contador;               //Explorar Archivos tarjeta SD
File directorios;            //Guardar nombres de archivos en JSON

int numero_ficheros;         //Numero de archivos guardados en SD
int total_ficheros = 0; 

////////////////////////////////////////////////////////////7
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
//=================FIN CONTADOR ARCHIVOS SD=============================


//==============BORRA FICHEROS TARJETA SD======================
void BorradoArchivosSD(String nombre_borrar){
  SD.remove(nombre_borrar);
  Serial.println("");
  Serial.print("Archivo borrado: ");
  Serial.println(nombre_borrar);
}
//==============FIN BORRADO ARCHIVOS SD=====================================


/*
==============EXPLORACIÓN DE LA TARJETA SD===================================
*/
//Explora los ficheros de la tarjeta SD
void explorar_ficheros(){
  numero_ficheros = contador_archivos();  //Cuenta el numero de archivos escritos en la SD
  root = SD.open("/");

  SPIFFS.remove("/dir.json");     //Limpia el archivo para ser leido

  //Prepara La Memoria SPIFFS para Escribir
  directorios = SPIFFS.open("/dir.json", "a");
    if (!directorios) {
      Serial.println("No se pudo abrir el archivo");
    }
  
  ///////////SI EXISTE SÓLO UN FICHERO EN LA TARJETA SD//////////////////////////////
  if(numero_ficheros == 1){  //Cuando solo hay un archivo en la SD
    Serial.println("dentro_1");
    single = root.openNextFile();

    //formatea el mensaje para JSON
    directorios.print("{\"titulo\":");
    directorios.printf("\"%s\"", single.name());
    directorios.print(",\"size\":");
    directorios.printf("%3.2f", float((single.size()) / 1048576.0)); //Convierte a Mb el tamaño del Archivo
    directorios.print("}");

    ///////////////DEPURACIÓN/////////////////////
    Serial.print("{\"titulo\":");
    Serial.printf("\"%s\"", single.name());
    Serial.print(",\"size\":");
    Serial.printf("%3.2f", float((single.size()) / 1048576.0));
    Serial.print("}");
    
    single.close();
    directorios.close();
    return;  
  }
  
  ///////////SI EXISTEN MÁS DE UN FICHERO EN LA TARJETA SD////////////////////
  if(numero_ficheros > 1){  // Cuando hay más de un archivo
    
    Serial.print("[");
    directorios.print("[");

    for(int i = 0; i < numero_ficheros - 1; i++){
    multi = root.openNextFile();

    if(!multi){      //si no hay más archivos...
      Serial.println("sin archivos");
      return;
    }

    //Formatea mensaje para JSON
    directorios.print("{\"titulo\":");
    directorios.printf("\"%s\"", multi.name());
    directorios.print(",\"size\":");
    directorios.printf("%3.2f", float((multi.size()) / 1048576.0));
    directorios.print("},");
    
    /////////////DEPURACIÓN//////////////////
    Serial.print("{\"titulo\":");
    Serial.printf("\"%s\"", multi.name());
    Serial.print(",\"size\":");
    Serial.printf("%3.2f", float((multi.size()) / 1048576.0));
    Serial.print("},");
    }

    //Imprime el último para cerrar el format JSON
    multi = root.openNextFile();

    directorios.print("{\"titulo\":");
    directorios.printf("\"%s\"", multi.name());
    directorios.print(",\"size\":");
    directorios.printf("%3.2f", float((multi.size()) / 1048576.0));
    directorios.print("}");
    directorios.print("]");

    //////////////DEPURACIÓN/////////////////////////
    Serial.print("{\"titulo\":");
    Serial.printf("\"%s\"", multi.name());
    Serial.print(",\"size\":");
    Serial.printf("%3.2f", float((multi.size()) / 1048576.0));
    Serial.print("}");
    Serial.print("]");
  } 

  directorios.close(); 
  multi.close();
}
//==================FIN EXPLORACIÓN TARJETA SD==========================
