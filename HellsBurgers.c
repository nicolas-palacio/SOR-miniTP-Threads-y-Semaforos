
#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50

pthread_mutex_t salero=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t horno=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plancha=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t escribir=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t equipoTermino=PTHREAD_MUTEX_INITIALIZER;

FILE * registroDeEquipos;

int algunEquipoGano;
//creo estructura de semaforos
struct semaforos {
    sem_t sem_mezclar,sem_armarMedallon,sem_salar,sem_planchar,sem_hornear,sem_armarHambuguesa;
    sem_t sem_panesListos,sem_lecuhgaytomate;
	//poner demas semaforos aqui
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];

};

//creo los parametros de los hilos
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[10];
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
			pthread_mutex_lock(&escribir);
			printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
			fprintf(registroDeEquipos,"\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion); //escribo en el archivo
			//calculo la longitud del array de ingredientes
			int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
			//indice para recorrer array de ingredientes
			int h;
			printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param);
				for(h = 0; h < sizeArrayIngredientes; h++) {
					//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion
					if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
								printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
								fprintf(registroDeEquipos,"\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
					}
				} pthread_mutex_unlock(&escribir);
		}
	}
}

//funcion para tomar de ejemplo
void* cortar(void *data) {

	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;


	//creo el nombre de la accion de la funcion
	char *accion = "Cortar";

	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep(2000000);
	//doy la señal a la siguiente accion (cortar me habilita mezclar)


	sem_post(&mydata->semaforos_param.sem_mezclar);
    pthread_exit(NULL);

}

void* mezclar(void *data){

	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_mezclar);
	if(algunEquipoGano==0){
		char *accion = "Mezclar";

		imprimirAccion(mydata,accion);
		usleep(2000000);
	}

	sem_post(&mydata->semaforos_param.sem_salar);
	pthread_exit(NULL);

}

void* salar(void *data){

	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_salar);
	if(algunEquipoGano==0){
		pthread_mutex_lock(&salero);

		char *accion = "Salar";

		imprimirAccion(mydata,accion);
		usleep(2000000);
		pthread_mutex_unlock(&salero);
	}
	sem_post(&mydata->semaforos_param.sem_armarMedallon);
	pthread_exit(NULL);

}

void* armarMedallones(void *data){

	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armarMedallon);

	if(algunEquipoGano==0){
		char *accion = "Armar medallones";

		imprimirAccion(mydata,accion);
		usleep(2000000);
	}

	sem_post(&mydata->semaforos_param.sem_planchar);
	pthread_exit(NULL);

}

void* planchar(void *data){

	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_planchar);

	if(algunEquipoGano==0){
		pthread_mutex_lock(&plancha);

		char *accion = "Planchar";

		imprimirAccion(mydata,accion);
		usleep(2000000);

		pthread_mutex_unlock(&plancha);
	}
	sem_post(&mydata->semaforos_param.sem_armarHambuguesa);
	pthread_exit(NULL);

}

void* hornearPanes(void *data){
	struct parametro *mydata = data;
	pthread_mutex_lock(&horno);
	if(algunEquipoGano==0){
		struct parametro *mydata = data;
		char *accion = "Hornear";

		imprimirAccion(mydata,accion);
		usleep(2000000);
	}

	pthread_mutex_unlock(&horno);

    sem_post(&mydata->semaforos_param.sem_panesListos);
	pthread_exit(NULL);

}

void* cortarLechugayTomate(void *data){
	struct parametro *mydata = data;
	if(algunEquipoGano==0){
		char *accion = "Cortar otros ingredientes";

		imprimirAccion(mydata,accion);
		usleep(2000000);
	}
    sem_post(&mydata->semaforos_param.sem_lecuhgaytomate);
	pthread_exit(NULL);

}

void* armarHamburguesas(void *data){

	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armarHambuguesa);
	sem_wait(&mydata->semaforos_param.sem_panesListos);
	sem_wait(&mydata->semaforos_param.sem_lecuhgaytomate);
	pthread_mutex_lock(&equipoTermino);

	if(algunEquipoGano==0){

		char *accion = "Armar hambuguesas";
		imprimirAccion(mydata,accion);
		usleep(2000000);

		algunEquipoGano=1;
		int equipo=mydata->equipo_param;
		pthread_mutex_lock(&escribir);

		printf("\t¡El equipo %d gano! \n",equipo);
		fprintf(registroDeEquipos,"\t¡El equipo %d gano! \n",equipo);
		pthread_mutex_unlock(&escribir);

	}

	pthread_mutex_unlock(&equipoTermino);
	pthread_exit(NULL);
}


void* leerReceta(void *data){
	char caracter;
	char linea[90];
	char *token;
	int index=0;
	int pasoIndex=0;
	int ingredienteIndex=0;
	struct parametro *mydata=data;
	int equipo=mydata->equipo_param;
	FILE * archivo;

	if(equipo==1){
		archivo=fopen("recetaEq1.txt","r");
	}else if(equipo==2){
		archivo=fopen("recetaEq2.txt","r");
	}else if(equipo==3){
		archivo=fopen("recetaEq3.txt","r");
	}

	while((caracter= fgetc(archivo)) !=EOF){
			linea[index]=caracter;
			index++;
			if(caracter=='\n'){
				token=strtok(linea,"|");//tengo el nombre del paso
				strcpy(mydata->pasos_param[pasoIndex].accion,token);
				token=strtok(NULL,"|"); //tengo el primer ingrediente

				while(token){
					strcpy(mydata->pasos_param[pasoIndex].ingredientes[ingredienteIndex],token);
					token=strtok(NULL,"|");
					ingredienteIndex++;
				}
				memset(linea,'\0',index); //limpio la linea
				index=0;
				ingredienteIndex=0;
				pasoIndex++;
			}
	}
	fclose(archivo);
}

void* ejecutarReceta(void *i) {

	//variables semaforos
	sem_t sem_mezclar,sem_salar,sem_armarMedallon,sem_planchar,sem_hornear,sem_armarHambuguesa;
    sem_t sem_panesListos,sem_lecuhgaytomate;

	//variables hilos
	pthread_t p1,p2,p3,p4,p5,p6,p7,p8;

	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct

	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_armarMedallon = sem_armarMedallon;
	pthread_data->semaforos_param.sem_planchar = sem_planchar;
	pthread_data->semaforos_param.sem_hornear = sem_hornear;
	pthread_data->semaforos_param.sem_armarHambuguesa = sem_armarHambuguesa;
	pthread_data->semaforos_param.sem_panesListos = sem_panesListos;
	pthread_data->semaforos_param.sem_lecuhgaytomate = sem_lecuhgaytomate;



	leerReceta(pthread_data);

	//inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarMedallon),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_planchar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_hornear),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarHambuguesa),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_panesListos),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_lecuhgaytomate),0,0);


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

	rc=pthread_create(&p2,NULL,mezclar,pthread_data);
	rc=pthread_create(&p3,NULL,salar,pthread_data);
	rc=pthread_create(&p4,NULL,armarMedallones,pthread_data);
	rc=pthread_create(&p5,NULL,planchar,pthread_data);
	rc=pthread_create(&p6,NULL,hornearPanes,pthread_data);
	rc=pthread_create(&p7,NULL,cortarLechugayTomate,pthread_data);
	rc=pthread_create(&p8,NULL,armarHamburguesas,pthread_data);

	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p5,NULL);
	pthread_join (p7,NULL);
	pthread_join (p8,NULL);


	//valido que el hilo se alla creado bien
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }


	//destruccion de los semaforos
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_armarMedallon);
	sem_destroy(&sem_planchar);
	sem_destroy(&sem_hornear);
	sem_destroy(&sem_armarHambuguesa);
    sem_destroy(&sem_panesListos);
    sem_destroy(&sem_lecuhgaytomate);


	//salida del hilo
	pthread_exit(NULL);
}


int main ()
{
	//creo los nombres de los equipos
	registroDeEquipos=fopen("registroDeEquipos.txt","w");
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

	algunEquipoGano=0;

	//creo las variables los hilos de los equipos
	pthread_t equipo1;
	pthread_t equipo2;
	pthread_t equipo3;

	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1);

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);
   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);

	pthread_mutex_destroy(&salero);
	pthread_mutex_destroy(&horno);
	pthread_mutex_destroy(&plancha);
	pthread_mutex_destroy(&equipoTermino);
	pthread_mutex_destroy(&escribir);

    pthread_exit(NULL);
	fclose(registroDeEquipos);
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
