#include "barrier.h"
#include <errno.h>


#ifdef POSIX_BARRIER

/* Wrapper functions to use pthread barriers */

int sys_barrier_init(sys_barrier_t* barrier, unsigned int nthreads)
{
	return pthread_barrier_init(barrier,NULL,nthreads);
}

int sys_barrier_destroy(sys_barrier_t* barrier)
{
	return pthread_barrier_destroy(barrier);
}

int sys_barrier_wait(sys_barrier_t *barrier)
{
	return pthread_barrier_wait(barrier);
}

#else


/* Barrier initialization function */
int sys_barrier_init(sys_barrier_t *barrier, unsigned int nr_threads)
{

	//mutex
	pthread_mutex_init(&(barrier->mutex), NULL);

	//variables condicion
	pthread_cond_init(&(barrier->cond), NULL);



	//maximo de hilos -> definir una constante o el parametro??
	barrier->max_threads = nr_threads;

	//inicializacion de hilos a llegar
	barrier->nr_threads_arrived = barrier->max_threads;

	return 0;
}

/* Destroy barrier resources */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
	pthread_mutex_destroy(&(barrier->mutex));
	pthread_cond_destroy(&(barrier->cond));

	//eliminar campos de la struc????

	return 0;
}

/* Main synchronization operation */
int sys_barrier_wait(sys_barrier_t *barrier)
{

	pthread_mutex_lock(&barrier->mutex);

	barrier->nr_threads_arrived--;

	if (barrier->nr_threads_arrived != 0) {

		//controlamos despertares espurios
		while (barrier->nr_threads_arrived < barrier->max_threads)
			pthread_cond_wait(&barrier->cond, &barrier->mutex);

	}

	else {

		//ponemos los hilos a maxhilos
		barrier->nr_threads_arrived = barrier->max_threads;

		//despertar a todos
		pthread_cond_broadcast(&barrier->cond);
	}

	//sotlamos mutex
	pthread_mutex_unlock(&barrier->mutex);

		return 0;


}

#endif /* POSIX_BARRIER */



/*
 * pseudocodigo mutex + variables de condicion:
 *
 * lock mutex
 *
 * numero de hilos en espera--;
 *
 * if (numero de hilos en espera != 0) {
 *
 * 		while hilos en espera != 0 {
 *
 * 			espero ¿ con la variable de condicion?
 * 		}
 *
 * }
 *
 * else { //si el numero de hilos en espera = 0, es por que soy el ultimo hilo en llegar
 *
 * 		hilos en espera = max_hilos;
 *
 * 		despierto a todos los hilos
 *
 * }
 *
 * unlock mutex
 *
 */

/*
 * pseudocodigo semaforos
 *
 *
 *
 *
 */












