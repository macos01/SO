#include "sched.h"

/* Global RR quantum parameter */ //parametro para cada x quanto cambiar ; maximo tiempo sin ser expropiado
int rr_quantum=3;

/* Structure to store RR thread-specific fields */ //refion de memoria con los cuantos que le quedan (timeslice ó ticks_slice
struct rr_data {
	int remaining_ticks_slice;
};

static int task_new_rr(task_t* t) //funcion util para crear la region de memoria que tendra cada tarea con el timeslice restante
{
	struct rr_data* cs_data=malloc(sizeof(struct rr_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum */
	cs_data->remaining_ticks_slice=rr_quantum; //inicializamos los cuantos  cons los que empieza
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_rr(task_t* t) //librear la memoria con datos utiles para el planificador (ticks)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_rr(runqueue_t* rq) ///la utilizamos para pillar una tarea  de la cola y meterla en la cpu
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t) 
		remove_slist(&rq->tasks,t);

	return t;
}

//encolamos tareas siempre
static void enqueue_task_rr(task_t* t,runqueue_t* rq, int preempted)
{
	//obtenemos el campo data
	struct rr_data* cs_data=(struct rr_data*) t->tcs_data;

	if (t->on_rq || is_idle_task(t))
		return;

	//la insertamos
	insert_slist(&rq->tasks,t); //Push task

	//reset cuantos
	cs_data->remaining_ticks_slice=rr_quantum; // Reset slice
}

//Realiza el accounting propio de la p ́litica por cada tick (ejecucion de la cpu)
static void task_tick_rr(runqueue_t* rq)
{
	//tarea actual
	task_t* current=rq->cur_task;

	//obtenemos los datos de los quantos de la tarea
	struct rr_data* cs_data=(struct rr_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--; /* Charge tick */ //decrementamos lo que le queda a esa tarea

	//si la tarea ha agotado sus ticks pues es necesario replanificar
	if (cs_data->remaining_ticks_slice<=0)
		rq->need_resched=TRUE; //Force a resched !!
}

static task_t* steal_task_rr(runqueue_t* rq)
{

	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

sched_class_t rr_sched= {
	.task_new=task_new_rr,
	.task_free=task_free_rr,
	.pick_next_task=pick_next_task_rr,
	.enqueue_task=enqueue_task_rr,
	.task_tick=task_tick_rr,
	.steal_task=steal_task_rr
};
