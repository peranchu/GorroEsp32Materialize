//____________________________________SCRIPT CONTROL________________________________

/*
============================INSTACIACIÓN ELEMENTOS DEL DOMM=================================
*/
const fileInput = document.getElementById('entrada');                             //instancia a File_in en HTML
var progress = document.getElementById('relleno');                                //instancia a barra de progreso HTML
document.querySelector('#traer_json_SD').addEventListener('click', traerJSON_SD); //Boton para cargar los archivos en SD
document.querySelector('#conexionWs_on').addEventListener('click', conexion_WS); //Boton conexion ON
document.querySelector('#conexionWs_off').addEventListener('click', desconectar); //Boton desconexion OFF
document.querySelector('#BtnStop').addEventListener('click', StopSD);             //Botón STOP 
document.getElementById('volumen').addEventListener('change', envioVolumen);      //Slider Volumen
//====================FIN ELEMENTOS INSTANCIACIÓN DOMM=======================



/*
=============FUNCIONES RELACIONADAS CON EL FORMULARIO=================================
*/
//Limpia el campo de Texto del Formulario de Subida 
function limpiarCampoTexto() {
  document.getElementById("campoTexto").value = "";
}

//Borra la cache del formulario de subida
function limpiarFormSubida() {
  document.getElementById('form_datos').reset();
}
//=======================FIN FUNCIONES DE FORMULARIO===============================



/*
================LEE EL JSON DEL ARCHIVO SPIFFS Y DIBUJA EN TABLA========================
*/
function traerJSON_SD() {  //Reacciona al evento del botón "REFRESH"

  PeticionRefrescoSD();   //Refresca la lista de archivos de la SD antes de leerlos
                          // Función dobre WebSocket

  //Empieza a leer el archvio JSON con los ficheros de la SD
  fetch('dir.json')
    .then(res => res.json())
    .then(content => {

      //=========Cuando hay más de un Archivo en la SD=============================
      try {

        //Muestra los datos del JSON en la tabla
        document.getElementById('lista').className = 'center'; //Muestra la lista cambia la propiedad hide

        let res = document.querySelector('#response');
        res.innerHTML = '';

        for (let item of content) {  //Bucle rellena la tabla "Template String"
          res.innerHTML += `
          <li class="collection-item avatar">
            <a href="javascript: ReproducirSD('${item.titulo}')" class="sencondary-content"><i class="material-icons circle red">play_arrow</i></a>
            <span class="title">${item.titulo}</span>
            <p>${item.size} Mb<br>
            </p>
            <a href="javascript: BorrarDatos('${item.titulo}')" class="sencondary-content"><i class="material-icons">clear</i></a>
          </li> 
          `
        }
        document.getElementById('stop').className = 'center';
        document.getElementById('volumen').className = 'center';
      }

      //=================Solo un Archivo en la SD==========================
      catch (e) {

        //Muestra los datos del JSON en la tabla
        document.getElementById('lista').className = 'center'; //Muestra la lista cambia la propiedad hide

        let res = document.querySelector('#response');
        res.innerHTML = '';

        //"_____________Template String"________________
        res.innerHTML = `  
          <li class="collection-item avatar">
            <a href="javascript: ReproducirSD('${content.titulo}')" class="sencondary-content"><i class="material-icons circle red">play_arrow</i></a>
            <span class="title">${content.titulo}</span>
            <p>${content.size} Mb<br>
            </p>
            <a href="javascript: BorrarDatos('${content.titulo}')" class="sencondary-content"><i class="material-icons">clear</i></a>
          </li> 
          `
      }
      document.getElementById('stop').className = 'center';
      document.getElementById('volumen').className = 'center';
    })

    //===================Si no hay archivos borra la tabla=======================
    .catch(error => {
      console.log(error);
      document.getElementById('lista').className = 'hide'; //Oculta la lista cambia la propiedad hide
    });
}
//===========================FIN PINTADO DE TABLA========================================



/*
///////////////////SUBIDA ARCHIVOS AL SD ////////////////////////////////
*/

//Validaciones Subida Archivo al SD
fileInput.addEventListener('change', (e) => { //detecta cuando se sube un archivo
  const file = (event.target.files[0]); //Accede a la informacion del fichero
  //console.log(file);

  var extensiones_p = ['mpeg', 'wav']; //Extensiones permitidas de los archivos

  //Funcion que convierte el tamaño a MEGAS
  var tamano_p = function (mega) {
    return Math.pow(2, 20) * mega;
  }

  var extension = file.type.split('/').pop();   //Separa / y Elimina del tipo "type: audio/mpeg"


  if (extensiones_p.indexOf(extension) != -1) { //Si encuentra un archivo con las extensiones admitidas...
    console.log('encontrado: ' + extension + ' ' + file.size);

    if (file.size < tamano_p(5)) { //Si su tamaño...

      Subir_Buffer(file); //Sube la imagen

    }else {
      M.toast({ //Avisos de ALERTA
        html: 'Archivo demasiado Grande',
        displayLength: 1500,
        classes: 'red lighten-1 rounded'
      });

      limpiarCampoTexto();
      limpiarFormSubida();
    }

  }else {
    M.toast({
      html: 'Archivo incorrecto: ' + extensiones_p.toString(),
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });

    limpiarCampoTexto();
    limpiarFormSubida();
  }
});
//=============================FIN SUBIDA ARCHIVOS AL SD==================================




//==================SUBIR ARCHIVO BUFFER DE MEMORIA========================
function Subir_Buffer(file) {

  const file_r = new FileReader(); //Maneja el archivo para subirlo al buffer, lee sus propiedades

  file_r.readAsDataURL(file);     // Leer desde una ubicacion

  file_r.onloadstart = (event) => {
    console.log("comenzando");
    console.log(event.loaded);
  }
  file_r.onloadend = (event) => {
    console.log("termino");
    console.log("Se cargó: " + event.loaded);
  }
  file_r.onload = (event) => {

  }
}
//=====================FIN SUBIR ARCHIVO A BUFFER========================



/*
==============ENVIO FORMULARIO CON EL ARCHIVO================================
*/
//Envio Formulario, Subida de Archivo al SD
var formulario = document.getElementById("form_datos");

formulario.addEventListener('submit', (event) => {
  event.preventDefault();    //previene las acciones comunes del formulario

  if (fileInput.files.length == 0) { //Si se intenta subir sin seleccionar archivo
    M.toast({
      html: 'Selecciona un Archivo de Audio',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });
    return;
  }

  //Mete los datos del archivo dentro del formulario
  var dataform = new FormData(formulario);
  dataform.append('musica', fileInput.files[0]); //Adjunta el archivo al formulario


  //////////__Peticion de envio AJAX -- Sube archivo de Audio a la SD__//////////////////////
  var xhr = new XMLHttpRequest();

  xhr.open('POST', '/subida');

  //Muestra la barra de Estado de la subida, cambia atributo hide
  document.getElementById('barra').className = 'center';

  xhr.upload.addEventListener("progress", (e) => {
    const percent = e.lengthComputable ? (e.loaded / e.total) * 100 : 0; //Calculo del porcentaje de subida

    progress.style.width = percent.toFixed(2) + "%"; //dibuja el porcentaje en la barra porpiedad "width"
  });

  xhr.onload = () => { //Archivo subido
    if (xhr.status === 200) {
      document.getElementById('barra').className = 'hide'; //oculta barra de subida
      limpiarCampoTexto(); //borra el campo de texto del file_input
      limpiarFormSubida();
      M.toast({
        html: 'Archivo Subido',
        displayLength: 1500,
        classes: "purple lighten-3 rounded"
      });
    } else {
      M.toast({
        html: 'Imposible subir Archivo, Revise conexión',
        displayLength: 2500,
        classes: "red lighten-1 rounded"
      });
      limpiarCampoTexto();
      limpiarFormSubida();
      document.getElementById('barra').className = 'hide';
    }
  }
  xhr.send(dataform);
});
//_____________________FIN AJAX___________________________
//
//==============FIN SUBIDA ARCHIVOS AL SD=================================




/*
//==================MANEJO CONEXIONES WEBSOCKET===================
*/
//____________CREACIÓN SOCKET DE CONEXIÓN__________________

//Se ejecuta al crear una conexión
function conexion_WS() {
  conexionWs = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);

  conexionWs.onopen = function () {
    console.log("conexión abierta");
    M.toast({
      html: 'Conexión Abierta',
      displayLength: 2500,
      classes: "purple lighten-3 rounded"
    });

    //Cambio estado de color botones de conexión
    document.getElementById('conexionWs_off').className = 'btn waves-effect waves-light red lighten-1';
    document.getElementById('conexionWs_on').className = 'btn waves-effect waves-light purple lighten-1';
  };

  conexionWs.onerror = function (error) {
    console.log("WebSocket Error", error);
    M.toast({
      html: 'Imposible Conectarse',
      displayLength: 2500,
      classes: "red lighten-1 rounded"
    });
  };

  conexionWs.onmessage = function (event) {
    console.log("Server: ", event.data);

    var datosINServidor = event.data;
    var dataJson = JSON.parse(datosINServidor);

    var Estado = dataJson.estado;
    var ip = dataJson.IP;

    if(Estado === 3){
      console.log("conectado");
      console.log(ip);
      document.getElementById('conex').innerHTML= `<h5>Conectado IP: ${ip}</h5>`
    }

  };

  conexionWs.onclose = function () {
    console.log("conexion Cerrada");
    M.toast({
      html: 'Conexión Cerrada',
      displayLength: 2500,
      classes: "red lighten-1 rounded"
    });

    document.getElementById('conex').innerHTML= '';

    //Cambio Estado de color Botones Conexión
    document.getElementById('conexionWs_off').className = 'btn disabled';
    document.getElementById('conexionWs_on').className = 'btn waves-effect waves-light purple lighten-4';
  };
}


//Funcion de dexconexión del Socket
function desconectar() {
  conexionWs.close();
}
/*
==============FIN FUNCIONES CONEXIÓN SOCKET=====================
*/




/*
=================FUNCIONES ENVIO MENSAJES SOCKET==============================
*/
//Envia Mensaje para refrescar el listado de Archivos de la SD
function PeticionRefrescoSD() {
  var full_data = '{"REFRESH":' + 1 + '}'; //Prepara Envio mensaje en formato JSON
  conexionWs.send(full_data); //Envía los datos
}
//===============FIN REFRESCO SD=========================================



/*
===================REPRODUCE ARCHIVOS DE LA SD ==============================
*/
//parámetros pasados desde el "href" del icono play
function ReproducirSD(nombreFile){
  //Crea un objeto para almacenar el nombre del fichero a reproducir
  var objetoJS_repro = {
    PLAY: nombreFile
  };

  //convierte el objeto javascript en JSON
  var envioPlay = JSON.stringify(objetoJS_repro);
  console.log(envioPlay);
  conexionWs.send(envioPlay);  //Envio...
  document.getElementById('nombreArchivoRepro').innerHTML = `<h7>${nombreFile}</h7>`
}
//===============FIN REPRODUCIR ARCHIVOS SD=================================


/*
=======================CONTROL VOLUMEN SD====================================
*/
function envioVolumen(){
  const volumenSD = document.querySelector('#vol').value; //Recoje el Volumen del Slider
  console.log(volumenSD);
  var envioVolumenSD = '{"VOL":' + volumenSD + '}';
 
  conexionWs.send(envioVolumenSD);
}

//========================BOTON STOP SD=======================================
function StopSD(){
  var SD_stop = '{"STOP":' + 1 + '}';
  conexionWs.send(SD_stop);
  document.getElementById('nombreArchivoRepro').innerHTML = '';
}


/*========BORRA LOS FICHEROS DE LA SD==========================
 */
//Parámetro pasado desde el "href" del icono invocando a la función
function BorrarDatos(nombreFichero) {
  //Crea un objeto para almacenar el nombre de fichero a borrar
  var objetoJS = {
    ERASE: nombreFichero
  };

  var envioDelete = JSON.stringify(objetoJS); //convierte el objeto JavaScript en JSON para enviarlo
  //console.log(envioDelete);
  conexionWs.send(envioDelete);
}
//======================FIN BORRADO ARCHIVOS SD==============================


/*
=========================PETICIONES DATOS AL SERVIDOR==================================
*/
