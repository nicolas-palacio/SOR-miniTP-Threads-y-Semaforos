# Sistemas Operativos y Redes 💻

##  Trabajo Práctico :  hilos y semáforos

*Alumno: Nicolás Palacio
Comisión: 01*

###  Introducción

------------


El objetivo de este programa es simular una competencia de cocina entre 3 equipos, los cuales deberán leer una receta y llevarla a cabo paso por paso para cocinar una hamburguesa. El seguimiento de dicha receta deberá cumplirse estrictamente, por ejemplo, no podrán preparar los medallones de carne sin antes haber cortado la cebolla, el ajo y el perejil. Para llevar a cabo la implementación se utilizarán hilos, semáforos y estructuras.

### Explicación del código

------------

Mutex definidos globalmente: salero, horno, plancha, escribir, equipoTermino.

Los mutex "salero", "horno" y "plancha" cumplirán la función saber si algun equipo esta utilizando alguno de estos elementos de cocina y de ser así, negarle su uso a los otros equipos. El mutex "escribir" permitirá que solo un hilo escriba en el archivo a la vez. 

Hilos definidos en el main: equipo1 , equipo2, equipo3. 

Cada hilo apunta a la función *ejecutarReceta()*.

- *ejecutarReceta() :*  crea y setea los hilos y semáforos necesarios para la ejecución. Cada hilo creado aqui representa los pasos de dicha receta y apuntan a sus funciones correspondientes, por ejemplo el hilo del paso 1, apunta a la función Cortar(), que es el primer paso de la receta. También dentro de esta función, se llama a leerReceta(), que es la encargada de leer el archivo de la receta y setear los pasos en los respectivos hilos.

- *cortar() , mezclar() , salar() , armarMedallones(), planchar(), hornearPanes(), cortarLechugayTomate(), armarHamburguesa()* :  cada una de estas funciones representa un paso de la receta, dentro de cada una se hace una llamada a imprimirAccion() en la que además de imprimir la acción pasada por parámetro también la escribe en un archivo(con ayuda del mutex "escribir" para lograr que una acción de cada equipo escriba a la vez). En cada función mencionada, con ayuda de los semáforos, se logra sincronizar los pasos para ejecutarlos en el orden que plantea la receta, y con ayuda de los mutex, solo un equipo a la vez podrá acceder a un elemento de cocina (salero, horno o plancha). 
  En cada función que representa los pasos se pregunta si ya hubo un ganador, de ser así no imprimirá la acción del paso y eliminará el hilo. La encargada de definir al equipo ganador es la función *armarHamburguesa() *, la cual cambia la variable global entera *algunEquipoGano* a 1.


### Prueba de escritorio

------------
**Funciones (pasos de la receta) :** cortar() , mezclar(), salar(), armarMedallones(), planchar(), hornearPanes(), cortarLechugayTomate(), armarHambuguesa().

**Semáforos:** sem_mezclar,sem_armarMedallones,sem_salar,sem_planchar,   sem_hornear,sem_armarHambuguesa,sem_panesListos,sem_lecuhgaytomate.

**Semáforos(mutex):** salero,horno,plancha,escribir,equipoTermino.

*P()=wait()  V()= signal()*

![pruebaEscritorio](https://user-images.githubusercontent.com/69064260/96062095-36e1f680-0e6b-11eb-9330-f41f8720e9c2.png)


