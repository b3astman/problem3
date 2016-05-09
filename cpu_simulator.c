#include "cpu_simulator.h"

int main(void) {
	// Initialize the queues.
	initQueues();
	int i; 
	for (i = 0; i < 10; i++) {
		initPCB();
		printf("\nPCB initialized");
	}
	int queue_string_size = FIFOq_toString_size(newQueue);
	char* queue_string = malloc((size_t)queue_string_size);
	FIFOq_toString(newQueue, queue_string, queue_string_size);
	printf("%s\n", queue_string);
	free(queue_string);
	
    pthread_create(&timer_thread, NULL, timerIR, NULL);

    printf("in main\n");

	/*for (i = 0; i < 5; i++) {
		CPU_loop();
		printf("\nCPU Loop ran\n");
	}*/

    i = 0;
    while (1) {
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%d: current time and date: %s", i, asctime(timeinfo)); // prints time Antonio!
        i++; 
    }
	
	return 0;
}

void initQueues() {
	printf("we here\n");
	newQueue = FIFOq_construct();
	FIFOq_init(newQueue);
	readyQueue = FIFOq_construct();
	FIFOq_init(readyQueue);
	terminateQueue = FIFOq_construct();
	FIFOq_init(terminateQueue);
	io_queue1 = FIFOq_construct();
	FIFOq_init(io_queue1);
	io_queue2 = FIFOq_construct();
	FIFOq_init(io_queue2);
}

void CPU_loop(void) {

}

void *timerIR(void) {
    pthread_mutex_init(&timer_lock, NULL);
    pthread_cond_init(&timer_cond, NULL);
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 50000000L; // nanoseconds

    int i;
    for (;;) { 
        if (!timer_waiting) {
            pthread_mutex_lock(&timer_lock); // gets mutex lock
            timer_waiting = 1;
            printf("thread loop!\n");
            nanosleep(&tim, &tim2); // puts thread to sleep
            pthread_cond_wait(&timer_cond, &timer_lock); // unlocks mutex and waits until interrupt is read by the CPU
        } 
    }
}

void pseudoISR(void) {
    // Change running process to interrupted.
    PCB_set_state(current_pcb, interrupted);

    // Save CPU state to current PCB (in our case, the current PC value).
    PCB_set_pc(current_pcb, PC);

    // Call scheduler
    run_scheduler(timer_interrupt);

    // Set CPU's PC to what is next on the stack. This allows the next process to run.
    PC = SysStack;
}

void run_scheduler(Interrupt interrupt_type) {
    PCB_p previous_pcb;
    switch (interrupt_type) {
    case timer_interrupt:
        previous_pcb = current_pcb;

        // Put current process back in ready queue. Don't put it into the queue if it's the idle process.
        if (current_pcb != idle_pcb) {
            FIFOq_enqueue(ready_PCBs, current_pcb);
        }

        // Change state of current PCB from interrupted to ready.
        PCB_set_state(current_pcb, ready);

        // Call dispatcher.
        run_dispatcher();

        // Print the state of the queue (if four or more context switches have already been made).
        if (cswitch_no == 0) {
            cswitch_no = 4;

            // Print stuff only if the old PCB wasn't the idle PCB (since it wasn't enqueued).
            if (previous_pcb != idle_pcb) {
                char *pcb_string = malloc(100);
                PCB_toString(previous_pcb, pcb_string);
                fprintf(output, "Returned to ready queue: %s\n", pcb_string);
                free(pcb_string);

                int queue_string_size = FIFOq_toString_size(ready_PCBs);
                char* queue_string = malloc((size_t)queue_string_size);
                FIFOq_toString(ready_PCBs, queue_string, queue_string_size);
                fprintf(output, "%s\n", queue_string);
                free(queue_string);
            }
        } else {
            // If this call wasn't a print call, decrement the counter.
            cswitch_no--;
        }

        break;
    }
    // Housekeeping.
    // Deallocate any terminated PCBs and their resources.
    while (!FIFOq_is_empty(terminated_PCBs)) {
        PCB_p terminated_pcb = FIFOq_dequeue(terminated_PCBs);
        PCB_destruct(terminated_pcb);
    }
}

void io_timer1(void) {

}
void io_timer2(void) {

}
void trap_handler(void) {

}


void initPCB() {
	PCB_p pcb = PCB_construct();
	PCB_init(pcb);
	FIFOq_enqueue(newQueue, pcb);
}