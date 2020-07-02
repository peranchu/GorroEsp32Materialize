
var archivo = document.getElementById('entrada'); //instancia a File in en HTML

var progress = document.getElementById('relleno'); //instancia a barra de progrso HTML
  
//Validaciones
archivo.onchange = (event) =>{
  var file = (event.target.files[0]);

  var extensiones_p = ['mpeg', 'wav'];

  var tamano_p = function(mega){
    return Math.pow(2,20) * mega;
  }
  var extension = file.type.split('/').pop();


  if(extensiones_p.indexOf(extension) != -1){
    console.log('encontrado: ' + extension +' ' + file.size);
    if(file.size < tamano_p(5)){
  
      subirImagen(file);
    }else{
      M.toast({html: 'Archivo demasiado Grande',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
      });
      //alert("Archivo demasiado grande");
      //console.log('Archivo demasiado grande');
      archivo.value = "";
    }
  }else{
    M.toast({html: 'Archivo incorrecto: ' +  extensiones_p.toString(),
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
      });
    //alert("Archivo incorrecto: " + extensiones_p.toString());
    //console.log('extension incorrecta: ' + extensiones_p.toString());
    archivo.value = "";
  }
}

//Subir imagen al buffer de memoria
function subirImagen(file){

  var file_r = new FileReader();  //Maneja el archivo para subirlo al buffer

  file_r.onloadstart = (event)=>{
    console.log("comenzando");
    console.log(event.loaded);
  }

  file_r.onloadend = (event) =>{
    console.log("termino");
    console.log("Se cargó: " + event.loaded);
  }

  file_r.onload = (event) =>{

  }

  file_r.readAsDataURL(file);
}

//Envio Formulario
var formulario = document.getElementById("form_datos");

formulario.addEventListener('submit', (event) =>{
  event.preventDefault();

  if(archivo.files.length == 0){
    M.toast({html: 'Selecciona un Archivo de Audio',
    displayLength: 1500,
    classes: 'red lighten-1 rounded'
    });
    //alert("seleccione un archivo de audio");
    //console.log("Seleccione un archivo");
    return;
  }
  //Mete los datos del archivo dentro del formulario
  var dataform = new FormData(formulario);
  dataform.append('musica', archivo.files[0]);


  
  //Peticion de envio AJAX
  var xhr = new XMLHttpRequest();

  xhr.open('POST', '/subida');

  //Muestra la barra de Estado de la subida
  document.getElementById('barra').className ='center';

  xhr.upload.addEventListener("progress", e =>{
    const percent = e.lengthComputable ? (e.loaded / e.total) * 100 : 0;

    progress.style.width = percent.toFixed(2) + "%";
  });

  xhr.onload = ()=>{
    if(xhr.status ===200){
      document.getElementById('barra').className ='hide';
      archivo.value = "";
      //console.log("se envian datos");
      M.toast({html: 'Archivo Subido',
      displayLength: 1500,
      classes: "purple lighten-3 rounded" 
      
      });
      //alert("Archivo Subido");
    }else{
      //console.log("Error" + xhr.status);
      M.toast({html: 'Imposible subir Archivo',
      displayLength: 1500,
      classes: "red lighten-1 rounded"
      });
      //alert("Imposible subir archivo" + xhr.status);
      archivo.value = "";
      document.getElementById('barra').className ='hide';
    }
  }
  
  xhr.send(dataform);
});