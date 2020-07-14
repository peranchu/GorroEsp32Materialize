//
//_______________CONFIGURACIÓN DE LOS SOCKETS ASÍNCRONOS DEL SERVIDOR//______________________

AsyncWebSocket ws("/ws");  //Objeto AsyncWebSocket "ws" y ruta de escucha de las peticiones del socket

AsyncWebSocketClient *globalClient = NULL;

//variables generales
bool RefreshEstado;         //Estado del botón de "refresh"
bool Stop_SD_Card;          //Almacena el estado de STOP
int Volumen;                //Almacena el volumen de SD
int volumenActual;
int volumenAnterior;
const char* NombreBorrado;  //Alamcena el nombre del Fichero a borrar
const char* ArchivoPlay;    //Almacena el archivo a reproducir de la SD

//===========================================================================

/*
=========================ENVIO DATOS DESDE EL SERVIDOR AL CLIENTE===================
*/
//Envio Petición Inicial de estado de conexión
void EnvioInicial(){
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED){  //Si hay algún cliente y esta conectado...
        StaticJsonDocument<300> Jsondoc;    //Se crea el documento JSON

        String response;      //Almacenará el JSON

        //Crea el Formato con los datos requeridos
        Jsondoc["estado"] = WiFi.status();
        Jsondoc["IP"] = WiFi.localIP().toString();
        Jsondoc["MDNS"] = hostname;

        serializeJson(Jsondoc, response);  //crea el documento JSON

        globalClient->text(response);      //Envía el Json al Cliente
    }
}
////////////////FIN PETICIÓN ESTADO DE CONEXIÓN//////////////////////////////////////////////



//////////////////Envío Estado de Reproducción de la SD///////////////////////////////////////
void EstadoRepro(){
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
        StaticJsonDocument<200> JsonErepro;

        String EstaRepro;
        JsonErepro["EREPRO"] = audio.isRunning();
        serializeJson(JsonErepro, EstaRepro);

        globalClient->text(EstaRepro);
    }
}
///////////////////////////FIN ESTADO REPRODUCCIÓN SD/////////////////////////////////////////
/*
====================FIN ENVIO DATOS SERVIDOR A CLIENTE==========================================
*/



/*
========ANÁLISIS DE DATOS DE LAS CADENAS DE ENTRADA=======================================
*/
//Datos que llegan desde el cliente al Servidor
void CadenaEntrada(String datosEntrada){
    //Serial.println(datosEntrada);
    DynamicJsonDocument doc(200);   //creación del objeto JSON

    DeserializationError error = deserializeJson(doc, datosEntrada); //comprueba que el JSON entrante es correcto
    if(error){
        Serial.print("Deserializacion JSON Fallida: ");
        Serial.println(error.c_str());
        return;
    }

    ///////////////////////MENSAJES DE ENTRADA DESDE EL CLIENTE///////////////////////////////
    //Mensaje de "refresh" de la lista
    RefreshEstado = doc["REFRESH"];     //Estado Botón Refresco de Datos SD
    if(RefreshEstado){
        explorar_ficheros();            //Refresca los ficheros de la SD, "archivos.hpp"
        RefreshEstado = false;
    }

    //Mensaje de Borrado de Archivos de la SD 
    int posDelete = datosEntrada.indexOf("ERASE");  //Si la cadena coincide con el mensaje
    if(posDelete >=0){
        NombreBorrado = doc["ERASE"];               //Captura del valor desde la clave enviada en el Json del servidor
        BorradoArchivosSD(NombreBorrado);           //Borra el archivo seleccionado, "archivos.hpp"
    } 

    //Mensaje de Reproduccion de Archivo de la SD
    int posPlay = datosEntrada.indexOf("PLAY");  //Si la cadena coincide con el mensaje...
    if(posPlay >=0){
        ArchivoPlay = doc["PLAY"];
        Serial.println(ArchivoPlay);
        PlayFilesSD(ArchivoPlay);     //Reproduce el archivo, "audio.hpp"
    }

    //Mensaje de STOP del Archivo SD
    int Stop_SD = datosEntrada.indexOf("STOP");
    if(Stop_SD >=0){
        Stop_SD_Card = doc["STOP"];
        Serial.println(Stop_SD_Card);
        Stop_Reproductor();           //Deteine la reproducción, "audio.hpp"
    }

    //Mensaje de Control VOLUMEN
    int Volumen_SD =  datosEntrada.indexOf("VOL");
    if(Volumen_SD >=0){
        Volumen = doc["VOL"];
        volumenActual = GetVolume(Volumen);  //Maneja el volumen de la reproducción, "audio.hpp"
    } 
}
/*
====================FIN ANÁLISIS CADENA DE ENTRADA=============================
*/



/*
====================FUNCIÓN QUE RECOGE LOS EVENTOS ENTRANTES A TRAVÉS DEL SOCKET===================
*/
//_ESCUCHA LOS EVENTOS DEL SOCKET_______________________

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                AwsEventType type, void *arg, uint8_t *data, size_t len){

    if(type == WS_EVT_CONNECT){   //Cliente conectado
        Serial.println("");
        Serial.println("Conexión Cliente al WebSocket Recibida");
        globalClient = client;

        EnvioInicial();   //Envio del estado de la conexión al cliente "websocket.hpp"

    }else if(type == WS_EVT_DISCONNECT){   //Cliente desconectado
        Serial.println("");
        Serial.println("Ciente Desconectado");
        Serial.println("-----------------------");
        globalClient = NULL;

    }else if(type == WS_EVT_DATA){  //Llegada de lagún tipo de dato...
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        String msg = "";
        if(info->final && info->index == 0 && info->len == len){
            //the whole message is in a single frame and we got all of it's data
            //Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
            if (info->opcode == WS_TEXT) {
                //Serial.printf("[%u] texto: %s\n", client->id(), data);
                for(size_t i = 0; i < info->len; i++){
                    msg += (char) data[i];
                }
                CadenaEntrada(msg);   //Envia la cadena de Entrada para su análisis "websocket.hpp"
            }
        }
    }
}
/*
================FIN ESCUCHA DE EVENTOS DEL SOCKET===========================
*/


//__________________INICIALIZACIÓN DEL SERVICIO DE SOCKETS//_______________________________
void InitWebSocket(){
    ws.onEvent(onWsEvent);          //Se Ejecuta cuando ocurre un evento a través del Socket
    server.addHandler(&ws);        //Registro del objeto AsyncWebserver en el servidor http asíncrono

    Serial.println("WebSocket Server Iniciado");
}