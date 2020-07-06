//
//_______________CONFIGURACIÓN DE LOS SOCKETS ASÍNCRONOS DEL SERVIDOR//______________________

AsyncWebSocket ws("/ws");  //Objeto AsyncWebSocket "ws" y ruta de escucha de las peticiones del socket

AsyncWebSocketClient *globalClient = NULL;

bool RefreshEstado;

/*
========ANÁLISIS DE DATOS DE LAS CADENAS DE ENTRADA=======================================
*/
void CadenaEntrada(String datosEntrada){
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, datosEntrada);
    if(error){
        Serial.print("Deserializacion JSON Fallida: ");
        Serial.println(error.c_str());
        return;
    }
    RefreshEstado = doc["REFRESH"];  //Estado Botón Refresco de Datos SD
    if(RefreshEstado){
        explorar_ficheros();        //Refresca los ficheros de la SD
        RefreshEstado = false;
    } 
}

//
//___________ESCUCHA LOS EVENTOS DEL SOCKET//_______________________
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                AwsEventType type, void *arg, uint8_t *data, size_t len){

    if(type == WS_EVT_CONNECT){
        Serial.println("");
        Serial.println("Conexión Cliente al WebSocket Recibida");
        globalClient = client;

    }else if(type == WS_EVT_DISCONNECT){
        Serial.println("");
        Serial.println("Ciente Desconectado");
        Serial.println("-----------------------");
        globalClient = NULL;

    }else if(type == WS_EVT_DATA){
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
                CadenaEntrada(msg);   //Envia la cadena de Entrada para su análisis
            }
        }
    }
}



//__________________INICIALIZACIÓN DEL SERVICIO DE SOCKETS//_______________________________
void InitWebSocket(){
    ws.onEvent(onWsEvent);          //Se Ejecuta cuando ocurre un evento a través del Socket
    server.addHandler(&ws);        //Registro del objeto AsyncWebserver en el servidor http asíncrono

    Serial.println("WebSocket Server Iniciado");
}