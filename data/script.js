const fileInput = document.getElementById('entrada'); //instancia a File_in en HTML

var progress = document.getElementById('relleno'); //instancia a barra de progreso HTML

document.querySelector('#traer_texto').addEventListener('click', traerTexto); //Boton para cargar los archivos en SD


//Lee el texto del archivo archivos en SD
var lines = new Array();

function traerTexto() {
  fetch('dir.json')
    .then(res => res.json())
    .then(content => {
      console.log(content);
    });
}

///////////////////////////////////////////


//////////////////////SUBIDA ARCHIVOS AL SD ////////////////////////////////7
//Validaciones Subida Archivo al SD
fileInput.addEventListener('change', (e) => {  //detecta cuando se sube un archivo
  const file = (event.target.files[0]);       //Accede a la informacion del fichero
  //console.log(file);

  var extensiones_p = ['mpeg', 'wav'];  //Extensiones permitidas de los archivos

  //Funcion que convierte el tamaño a MEGAS
  var tamano_p = function (mega) {
    return Math.pow(2, 20) * mega;
  }

  var extension = file.type.split('/').pop(); //Separa / y Elimina del tipo "type: audio/mpeg"


  if (extensiones_p.indexOf(extension) != -1) {  //Si encuentra un archivo
    console.log('encontrado: ' + extension + ' ' + file.size);

    if (file.size < tamano_p(5)) {  //Si su tamaño

      Subir_Buffer(file);    //Sube la imagen

    } else {
      M.toast({
        html: 'Archivo demasiado Grande',
        displayLength: 1500,
        classes: 'red lighten-1 rounded'
      });
      //alert("Archivo demasiado grande");
      //console.log('Archivo demasiado grande');
      fileInput.value = "";
    }
  } else {
    M.toast({
      html: 'Archivo incorrecto: ' + extensiones_p.toString(),
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });
    //alert("Archivo incorrecto: " + extensiones_p.toString());
    //console.log('extension incorrecta: ' + extensiones_p.toString());
    fileInput.value = "";
  }
});
/////////////////////////////////////////

//Subir imagen al buffer de memoria
function Subir_Buffer(file) {

  const file_r = new FileReader(); //Maneja el archivo para subirlo al buffer

  file_r.readAsDataURL(file); // Leer desde una ubicacion

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

//Envio Formulario, Subida de Archivo al SD
var formulario = document.getElementById("form_datos");

formulario.addEventListener('submit', (event) => {
  event.preventDefault();

  if (fileInput.files.length == 0) {  //Si se intenta subir sin seleccionar archivo
    M.toast({
      html: 'Selecciona un Archivo de Audio',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });
    return;
  }

  //Mete los datos del archivo dentro del formulario
  var dataform = new FormData(formulario);
  dataform.append('musica', fileInput.files[0]);


  //Peticion de envio AJAX -- Sube archivo de Audio a la SD
  var xhr = new XMLHttpRequest();

  xhr.open('POST', '/subida');

  //Muestra la barra de Estado de la subida, cambia atributo hide
  document.getElementById('barra').className = 'center';

  xhr.upload.addEventListener("progress", (e) => {
    const percent = e.lengthComputable ? (e.loaded / e.total) * 100 : 0; //Calculo del porcentaje de subida

    progress.style.width = percent.toFixed(2) + "%"; //dibuja el porcentaje en la barra porpiedad "width"
  });

  xhr.onload = () => {      //Archivo subido
    if (xhr.status === 200) {
      document.getElementById('barra').className = 'hide';  //oculta barra de subida
      fileInput.value = "";  //borra el campo de texto del file_input
      M.toast({
        html: 'Archivo Subido',
        displayLength: 1500,
        classes: "purple lighten-3 rounded"
      });
    } else {
      M.toast({
        html: 'Imposible subir Archivo',
        displayLength: 1500,
        classes: "red lighten-1 rounded"
      });
      fileInput.value = "";
      document.getElementById('barra').className = 'hide';
    }
  }
  xhr.send(dataform);
});