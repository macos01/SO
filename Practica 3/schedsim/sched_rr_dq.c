#include "sched.h"

typedef enum t_type {CPU_BOUND, INTERACTIVE} tast_type;

// Structure to store RR thread-specific fields
struct rr_data {
    int remaining_ticks_slice;
    tast_type type;
};

// Global RR quantum parameter for INTERACTIVE tasks
int rr_quantum_int=2;
// Global RR quantum parameter for CPU_BOUND tasks
int rr_quantum_cpu=4;

static int sched_init_rr_dq(void) {
    int cpu;
    runqueue_t* cpu_rq;
    slist_t *tasks_cpu_bound;

    for (cpu=0; cpu<nr_cpus; cpu++) {
        cpu_rq=get_runqueue_cpu(cpu);
        
        // Use rq_cs_data for cpu_bound queue tasks
        if( (tasks_cpu_bound=malloc(sizeof(slist_t))) == NULL) {
            fprintf(stderr,"Cannot allocate memory for private run queues\n");
            return 1;
        }
        // Init slist
        init_slist(tasks_cpu_bound, offsetof(task_t,rq_links));
        
        // Assign pointer
        cpu_rq->rq_cs_data=tasks_cpu_bound;
    }
    return 0;
}

void sched_destroy_rr_dq(void) {
    int cpu;
    runqueue_t* cpu_rq;

    for (cpu=0; cpu<nr_cpus; cpu++) {
        cpu_rq=get_runqueue_cpu(cpu);
        // Free up data structure
        free(cpu_rq->rq_cs_data);
        cpu_rq->rq_cs_data=NULL;
    }    
}

static int task_new_rr_dq(task_t* t)
{
	struct rr_data* cs_data=malloc(sizeof(struct rr_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum interactive*/
	cs_data->remaining_ticks_slice=rr_quantum_int;
	cs_data->type=INTERACTIVE;
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_rr_dq(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_rr_dq(runqueue_t* rq)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;
    
   //tomamos primero las tareas de la interactive, luego las de bounds

    task_t* t=head_slist(&rq->tasks);

    if (is_empty_slist(&rq->tasks))
    	t = head_slist(cpu_bound_tasks);

    else
    	t=head_slist(&rq->tasks);

    if (t == NULL) {

    	if (t) {
    		//de BOUNDS
    		remove_slist(cpu_bound_tasks,t);
    		return t;
    	}
    }

    else {
    	//de INTERACTIVE
    	remove_slist(&rq->tasks,t);
    }

    return t;
    
}

static void enqueue_task_rr_dq(task_t* t,runqueue_t* rq, int preempted)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;
    
    struct rr_data* cs_data=(struct rr_data*) t->tcs_data;



    //si es la tarea idle; no hacemos nada
    if (t->on_rq || is_idle_task(t))
    		return;
    
    //si la tarea es de tipo interactivo
    if (cs_data->type == INTERACTIVE){

    	//la insertamos

    	insert_slist(&rq->tasks,t); //Push task
    	cs_data->remaining_ticks_slice = rr_quantum_int;

    }
    //si la tarea es de tipo BOUNDS
    else {

    	insert_slist(cpu_bound_tasks,t); //Push task
    	cs_data->remaining_ticks_slice = rr_quantum_cpu;

    }

    if (!preempted) {

//    		task_t* current=rq->cur_task;
//
//    		if (current->prio < t->prio) {
//    			rq->need_resched=TRUE;
//    		}
//
//    		if (preemptive_scheduler && t->runnable_ticks_left<current->runnable_ticks_left) {
//    			rq->need_resched=TRUE;
//    			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
//                                                    another task with the same length wakes up as well*/
//    		}
    }

}

static void task_tick_rr_dq(runqueue_t* rq)
{
    task_t* current=rq->cur_task;
    struct rr_data* cs_data=(struct rr_data*) current->tcs_data;

    if (is_idle_task(current))
    		return;

    cs_data->remaining_ticks_slice--; /* Charge tick */ //decrementamos lo que le queda a esa tarea

    //si la tarea ha agotado sus ticks pues es necesario replanificar
    if (cs_data->remaining_ticks_slice <= 0){
    	cs_data->type=CPU_BOUND;
    	rq->need_resched=TRUE; //Force a resched !!
    }
    
    else {
    	if (cs_data->remaining_ticks_slice == 1){
    		cs_data->type=INTERACTIVE;
    	}
    }

}

static task_t* steal_task_rr_dq(runqueue_t* rq)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;

    task_t* stealTask=tail_slist(cpu_bound_tasks);

    if (is_empty_slist(cpu_bound_tasks))
    	stealTask = head_slist(&rq->tasks);

       else
    	 stealTask=head_slist(&rq->tasks);

    if (stealTask == NULL) {

    	//si no hay tareas en BOUNDS
    	stealTask = tail_slist(&rq->tasks);

    	if (stealTask) {
    		//de INTERACTIVE
    		remove_slist(&rq->tasks,stealTask);
    		return stealTask;
    	}
    }
    
    else {

    	//BOUNDS
       	remove_slist(cpu_bound_tasks,stealTask);
    }

    return stealTask;

}

sched_class_t rr_sched_dq= {
    .sched_init=sched_init_rr_dq,
    .sched_destroy=sched_destroy_rr_dq,
	.task_new=task_new_rr_dq,
	.task_free=task_free_rr_dq,
	.pick_next_task=pick_next_task_rr_dq,
	.enqueue_task=enqueue_task_rr_dq,
	.task_tick=task_tick_rr_dq,
	.steal_task=steal_task_rr_dq
};
