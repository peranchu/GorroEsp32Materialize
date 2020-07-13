//INICIA LA CONEXIÓN WiFi

void ConnectWiFi_STA(bool useStaticIP = true){
    Serial.println("");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if(useStaticIP) WiFi.config(ip, gateway, subnet);

    while(WiFi.status() !=WL_CONNECTED){
        delay(100);
        Serial.print('.');
    }

    Serial.println("");
    Serial.print("Iniciando STA:\t");
    Serial.println(ssid);
    Serial.print("IP Address:\t");
    Serial.println(WiFi.localIP()); 

    //Server MDNS
    if(MDNS.begin(hostname)){  //Gorro_1.local
        Serial.println("");
        Serial.println("MDNS Iniciado");
    }
}