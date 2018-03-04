#include "sched.h"

static task_t* pick_next_task_fcfs(runqueue_t* rq)
{

	task_t* t=head_slist(&rq->tasks);


	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}


static void enqueue_task_fcfs(task_t* t,runqueue_t* rq, int preempted) //preempted == runnable
{

	if (t->on_rq || is_idle_task(t)) //si esta en cola o es la tarea IDLE , no hacerr nada
		return;

	//insertamos al final
	insert_slist(&rq->tasks,t);
	//marcamos la tarea como que esta en la cola.
	t->on_rq = TRUE;

	//habria que aumentar:?? no encuentro sentido
	//rq->nr_runnable++;

	//habria que añadir nueva tarea runnable a la rq?

    /* If the task was not using the current CPU, check whether a preemption is in order or not */
	//if (!preempted) {
		//task_t* current=rq->cur_task;

		//preemptive scheduler no vale con fcfs, y ademas es no expriptiavo
//		/* Trigger a preemption if this task has a shorter CPU burst than current */
//		if (preemptive_scheduler && t->runnable_ticks_left<current->runnable_ticks_left) {
//			rq->need_resched=TRUE;
//			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
//                                                another task with the same length wakes up as well*/
//		}
	//}
}

static task_t* steal_task_fcfs(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

sched_class_t fcfs_sched= {
	.pick_next_task=pick_next_task_fcfs,
	.enqueue_task=enqueue_task_fcfs,
	.steal_task=steal_task_fcfs,
};
