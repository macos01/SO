#include "sched.h"

static int compare_tasks_rq_prio(void *task1, void *task2){
	task_t* tsk1 = (task_t*) task1;
	task_t* tsk2 = (task_t*) task2;

	return (tsk1->prio - tsk2->prio);
}

/**
 *
 * La prioridad inicial de las tareas esta determinada en los ejemplos. NO aumentamos prioridad con la edad;
 * solamente comprobamos si llega una tarea con mas prioridad o no
 *
 *
 *
 * example1:
 *  P1 1 0 1 5 4
	P2 1 1 3 1 1
	P3 1 0 5
	P4 1 3 3 2 1 1

	La primera columna representa el nombre de la tarea.
	La segunda columna, su prioridad.
	Posteriormente se indica el tiempo de llegada de la tarea al sistema (si es 0, se indica que la tarea está disponible
	desde el comienzo de la simulación).
	A partir de ahí encontramos el perfil de ejecución de cada tarea: el primer número indica el tiempo de ejecución de la primera ráfaga de CPU; luego,
	tiempo de espera (por E/S o similar).
	Y el patrón se repite hasta que la tarea finaliza (es decir, el mismo formato que se usa en la hoja de ejercicios).

	Por ejemplo, en example1.txt se especifica que se crearán 4 tareas:
	La primera tarea, llamada P1 comienza su ejecución al principio de la simulación y tiene prioridad 1.
	Al comenzar,tratará de usar la CPU durante una unidad de tiempo.
	Posteriormente, se bloqueará durante 5 unidades de tiempo.
	Finalmente, volverá a requerir la CPU durante 4 unidades de tiempo y finalizará su ejecución.

 *
 */

static void task_tick_prio(runqueue_t* rq)
{
	task_t* current=rq->cur_task;

	if (is_idle_task(current))
		return;

	rq->need_resched=TRUE; //Force a resched !!

	// para que sirve comprobar cuanto le queda a la tarea en prio??? ..> solo tiene sentido en fcfs

	//por que tiene sentido saber cuanto tiempo de vida le queda a la tarea???
//	if (current->runnable_ticks_left==1)
//		rq->nr_runnable--; // The task is either exiting or going to sleep right now

}

static void enqueue_task_prio(task_t* t,runqueue_t* rq, int preempted)
{

	if (t->on_rq || is_idle_task(t))
		return;

	//encolar segun orden....
	if (t->flags & TF_INSERT_FRONT) {
		//Clear flag
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_rq_prio);  //Push task
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_rq_prio);  //Push task


	//si la tarea ha sido expulsada?? .> pues replanificamos, ponemos la sustituria y la otra se encola y listo

    /* If the task was not using the current CPU, check whether a preemption is in order or not */

	//si no ha sido expulsada, lo que podria ocurrir en prio.
	if (!preempted) {


		//habria que aumentar:??
		//rq->nr_runnable++;

		//tarea en la cpu, puede ser tarea idle
		task_t* current=rq->cur_task;

		//si la tarea no es la idle, y la prioridad de la tarea a encolarse es mayor que la que se esta ejecutando..
		//preemptive_scheduler valido
		if (preemptive_scheduler && !is_idle_task(current) && t->prio > current->prio) {
			rq->need_resched=TRUE; //marcamos replanificar

			//el caso rarito:
			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
                                                another task with the same length wakes up as well*/
		}
	}
}

//siempre giuales:

static task_t* pick_next_task_prio(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

static task_t* steal_task_prio(runqueue_t* rq)
{

	//la de menos prioridad. es decir la ultima
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}


sched_class_t prio_sched= {
	.pick_next_task=pick_next_task_prio,
	.enqueue_task=enqueue_task_prio,
	.steal_task=steal_task_prio,
	.task_tick=task_tick_prio
};
