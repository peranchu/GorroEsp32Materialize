/*
=================CONFIGURACIÓN SISTEMA DE AUDIO===========================
*/
Audio audio;  // Instancia del objeto Audio

bool estado;

////////////PINES AUDIO MAX98357A/////////////////////////////
#define I2S_DOUT 25   //DIN
#define I2S_BCLK 27   //Bit Clock
#define I2S_LRC  26   //Left Right Clock

//Configuración Inicial Audio
void audioConfig(){
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); //0...21
//audio.connecttoFS(SD, "/0002.mp3");
}


//====================REPRODUCIR ARCHIVOS TARJETA SD============================
void PlayFilesSD(String PlayFile){
    Serial.println("DentroAudio");
    audio.connecttoFS(SD, PlayFile);
    estado = true;
}
//========================FIN REPRODUCTOR ARCHIVOS SD=========================

//========================STOP SD REPRODUCTOR================================
void Stop_Reproductor(){
    audio.stopSong();
}