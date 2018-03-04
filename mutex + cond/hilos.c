#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define MAX_PAQUETES	100
#define TIPO1	1
#define TIPO2	2

void entregar (int tipo);
int atenderCliente ();
void *Recepcionista (void *arg);
void *Repartidor (void *t);

//numero de paquetes de cada tipo
int nTipo1 = 0;
int nTipo2 = 0;

pthread_mutex_t mutex;
pthread_cond_t paquetesTipo1;
pthread_cond_t paquetesTipo2;
pthread_cond_t llenoPaquetes;


int
main (int argc, char *argv[])
{

    pthread_t th1, th2, th3;	/*th4 ,th7,th10,th5,th6,th8,th9; */

    pthread_mutex_init (&mutex, NULL);
    pthread_cond_init (&paquetesTipo1, NULL);
    pthread_cond_init (&paquetesTipo2, NULL);
    pthread_cond_init (&llenoPaquetes, NULL);


	//recepcionistas
    pthread_create (&th1, NULL, Recepcionista, NULL);
//   pthread_create(&th10, NULL, Recepcionista,NULL);

	//repartidores de primer tipo
    int primertipo = 1;
    pthread_create (&th2, NULL, Repartidor, &primertipo);
//   pthread_create(&th7, NULL, Repartidor, &primertipo);
//   pthread_create(&th8, NULL, Repartidor, &primertipo);
//   pthread_create(&th9, NULL, Repartidor, &primertipo);

	//repartidores de segundo tipo
    int segundotipo = 2;
    pthread_create (&th3, NULL, Repartidor, &segundotipo);
//   pthread_create(&th4, NULL, Repartidor, &segundotipo);
//   pthread_create(&th5, NULL, Repartidor, &segundotipo);
//   pthread_create(&th6, NULL, Repartidor, &segundotipo);

    pthread_join (th1, NULL);
    pthread_join (th2, NULL);
    pthread_join (th3, NULL);
//   pthread_join(th4, NULL);
//   pthread_join(th5, NULL);
//   pthread_join(th6, NULL);
//   pthread_join(th7, NULL);
//   pthread_join(th8, NULL);
//   pthread_join(th9, NULL);
//   pthread_join(th10, NULL);
//
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&paquetesTipo1);
	pthread_cond_destroy(&paquetesTipo2);
	pthread_cond_destroy(&llenoPaquetes);

	exit(1);
	
}

void *Recepcionista (void *arg)
{

    //hacer indefinidamente
    while (1)
    {

        //vamos a realizar nuestra labor
        pthread_mutex_lock (&mutex);
        //si esta lleno de paquetes
        while ((nTipo1 + nTipo2) >= MAX_PAQUETES)
        {
            time_t tiempo = time (0);
            struct tm *tlocal = localtime (&tiempo);
            char output[128];
            strftime (output, 128, "%H:%M:%S", tlocal);
            printf ("%s ", output);
            printf ("RECEPCIONISTA:-El almacen esta lleno\n");
            //como el almacen esta lleno, no puedo recibir paquetes. Por lo tanto espero a que repartan.
            pthread_cond_wait (&llenoPaquetes, &mutex);
        }

//     time_t tiempo = time(0);
//         struct tm *tlocal = localtime(&tiempo);
//         char output[128];
//         strftime(output,128,"%H:%M:%S",tlocal);
//         printf("%s ",output);
//     printf("RECEPCIONISTA:-El almacen tiene hueco, atendere a un cliente\n");

        //cedo el mutex porque voy a atender a un cliente y no tocara los valores
        pthread_mutex_unlock (&mutex);
        //Atiendo al cliente
        int tipo = atenderCliente ();
        //ahora si voy a modificar los valores
        pthread_mutex_lock (&mutex);
        switch (tipo)
        {
        case TIPO1:
            nTipo1++;
            printf ("RECEPCIONISTA TOTAL: %d\n", nTipo1 + nTipo2);
            //despierto a los repartidores de tipo1
            pthread_cond_signal (&paquetesTipo1);
            break;
        case TIPO2:
            nTipo2++;
            printf ("RECEPCIONISTA TOTAL: %d\n", nTipo1 + nTipo2);
            //despierto a los repartidores de tipo2
            pthread_cond_signal (&paquetesTipo2);
            break;
        }

        pthread_mutex_unlock (&mutex);
    }

    pthread_exit (0);
}

int atenderCliente ()
{


//   time_t tiempo = time(0);
//         struct tm *tlocal = localtime(&tiempo);
//         char output[128];
//         strftime(output,128,"%H:%M:%S",tlocal);
//         printf("%s ",output);
//   printf("RECEPCIONISTA:-Ha entrado un cliente nuevo, voy a tardar %d segundos en atenderle\n",demora);

    usleep (500000);
    int dato = rand () % 99;
    if (dato > 49)
    {
        time_t tiempo = time (0);
        struct tm *tlocal = localtime (&tiempo);
        char output[128];
        strftime (output, 128, "%H:%M:%S", tlocal);
        printf ("%s ", output);
        printf ("RECEPCIONISTA:-Recibido paquete nuevo de TIPO 1\n");
        return TIPO1;
    }
    else
    {
        time_t tiempo = time (0);
        struct tm *tlocal = localtime (&tiempo);
        char output[128];
        strftime (output, 128, "%H:%M:%S", tlocal);
        printf ("%s ", output);
        printf ("RECEPCIONISTA:-Recibido paquete nuevo de TIPO 2\n");
        return TIPO2;
    }

}

void entregar (int tipo)
{
    time_t tiempo = time (0);
    struct tm *tlocal = localtime (&tiempo);
    char output[128];
    strftime (output, 128, "%H:%M:%S", tlocal);
    printf ("%s ", output);
    printf
    ("REPARTIDOR TIPO %d:-Voy a entregar. Tardo 2 segundos. \n",
     tipo);
    sleep (2);
    tiempo = time (0);
    tlocal = localtime (&tiempo);
    strftime (output, 128, "%H:%M:%S", tlocal);
    printf ("%s ", output);
    printf ("REPARTIDOR TIPO %d:-Entregado\n", tipo);
}

void *Repartidor (void *t)
{

    //voy a hacer mi trabajo
    int *tipo = (int *) t;
    while (1)
    {

        pthread_mutex_lock (&mutex);
        if (*tipo == TIPO1)
        {
            while (nTipo1 == 0)
            {
                //      printf("REPARTIDOR TIPO 1:-No hay paquetes de mi tipo, esperare.\n");
                pthread_cond_wait (&paquetesTipo1, &mutex);
            }
            nTipo1--;
            printf ("REPARTIDOR TIPO %d TOTAL: %d\n", *tipo,
                    nTipo1 + nTipo2);
        }
        else
        {
            while (nTipo2 == 0)
            {
                //      printf("REPARTIDOR TIPO 2:-No hay paquetes de mi tipo, esperare.\n");
                pthread_cond_wait (&paquetesTipo2, &mutex);
            }
            nTipo2--;
            printf ("REPARTIDOR TIPO %d TOTAL: %d\n", *tipo,
                    nTipo1 + nTipo2);
        }

        if ((nTipo1 + nTipo2) == MAX_PAQUETES - 1)
        {
            pthread_cond_signal (&llenoPaquetes);
        }

        //me he despertado que voy a hacer??
        pthread_mutex_unlock (&mutex);
        entregar (*tipo);
    }
    pthread_exit (0);
}
