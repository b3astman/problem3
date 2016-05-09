#include "cpu_simulator.h"

int main(void) {
    //initialize file output.
    output = fopen("scheduleTrace.txt", "w");

	// Initialize the queues.
	initQueues();
	int i;
	for (i = 0; i < 10; i++) {
		initPCB(i);
	}

    idle_process = PCB_construct();
    PCB_init(idle_process);
    PCB_set_state(idle_process, running);
    PCB_set_pc(idle_process, PC);
    PCB_set_pid(idle_process, 0xFFFFFFFF);
    current_process = idle_process;
	
    pthread_create(&timer_thread, NULL, timerIR, NULL);

    for (i = 0; i < 4000; i++) {
        //fprintf(output, "%d ", i);
        printf("%d ", i);
		CPU_loop();             
	}

    fprintf(output, "\nReady Queue contents:\n");
    int queue_string_size = FIFOq_toString_size(readyQueue); // this block prints new Queue
    char* queue_string = malloc((size_t)queue_string_size);
    FIFOq_toString(readyQueue, queue_string, queue_string_size);
    fprintf(output,"%s\n", queue_string);
    free(queue_string);

    fprintf(output, "Waiting Queue1 Contents:\n");
    queue_string_size = FIFOq_toString_size(waitQueue1); // this block prints new Queue
    queue_string = malloc((size_t)queue_string_size);
    FIFOq_toString(waitQueue1, queue_string, queue_string_size);
    fprintf(output, "%s\n", queue_string);
    free(queue_string);

    fprintf(output, "Waiting Queue2 Contents:\n");
    queue_string_size = FIFOq_toString_size(waitQueue2); // this block prints new Queue
    queue_string = malloc((size_t)queue_string_size);
    FIFOq_toString(waitQueue2, queue_string, queue_string_size);
    fprintf(output, "%s\n", queue_string);
    free(queue_string);

    char* pcb_string = malloc(200);
    PCB_toString(current_process, pcb_string);
    fprintf(output, "Last running: %s\n", pcb_string);
    free(pcb_string);

    fprintf(output, "Timer interrupt called %d times!", timer_ir_count);


	/*
    i = 0;
    while (1) { // for timer interrupt testing.
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%d: current time and date: %s", i, asctime(timeinfo)); // prints time Antonio!
        i++; 
    }*/
	
    fclose(output);
	return 0;
}

void initQueues() {
	readyQueue = FIFOq_construct();
	FIFOq_init(readyQueue);
	terminateQueue = FIFOq_construct();
	FIFOq_init(terminateQueue);
	io_queue1 = FIFOq_construct();
	FIFOq_init(io_queue1);
	io_queue2 = FIFOq_construct();
	FIFOq_init(io_queue2);
	waitQueue1 = FIFOq_construct();
	FIFOq_init(waitQueue1);
    waitQueue2 = FIFOq_construct();
    FIFOq_init(waitQueue2);
}

// "CPU is a loop that represents an execution cycle"
void CPU_loop(void) {
    int i;
	// "each iteration represents a single instruction"
    // "PC will be incremented by one each time through the loop"
	// "...meaning that after the PC reaches 2345 it is reset to zero"
	PC = (PC + 1) % MAX_PC;
    PCB_set_pc(current_process, PC);
    // check timer interrupt
    if (timer_waiting) {
        if (pthread_mutex_trylock(&timer_lock) == 0) {
            fprintf(output, "Timer interrupt!\n");
            //timer interrupt has happened
            timer_ir_count++;
            // Push PC to the system stack (pseudo-push).
            SysStack = PC;

            // ISR-call (simulate a timer interrupt).
            pseudoISR();

            timer_waiting = 0;
            pthread_mutex_unlock(&timer_lock);// releases control on mutex
            pthread_cond_signal(&timer_cond); // signal to timer to start over
        }
    }
    //do io interrupt check here.
    if (!FIFOq_is_empty(waitQueue1)) {
        int first = io_timer1();
        if (first == 1) {
            fprintf(output, "IO completion Interrupt!");
            io_ISR(1);
        }
    }
    if (!FIFOq_is_empty(waitQueue2)) {
        int second = io_timer2();
        if (second == 1) {
            fprintf(output, "IO completion Interrupt!");
            io_ISR(2);
        }
    }

    // check for io trap calls
	for (i = 0; i < 4; i++) {
        if (current_process != idle_process) {
            if (PCB_get_trap1(current_process, i) == PC) {
                //calls the I/O trap handler passing the device number
                trap_handler(1);
                fprintf(output, "Trap call for IO device 1!\n");
            }
            if (PCB_get_trap2(current_process, i) == PC) {
                //calls the I/O trap handler passing the device number
                trap_handler(2);
                fprintf(output, "Trap call for IO device 2!\n");
            }
        }
	}
	//fprintf(output, "CPU Loop ran\n");
    printf("CPU Loop ran\n");
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
            // gets mutex lock
            pthread_mutex_lock(&timer_lock); 
            timer_waiting = 1;
            // puts thread to sleep
            nanosleep(&tim, &tim2); 
            // unlocks mutex and waits until interrupt is read by the CPU
            pthread_cond_wait(&timer_cond, &timer_lock); 
            pthread_mutex_unlock(&timer_lock);
        } 
    }
}

void pseudoISR(void) {
    // Change running process to interrupted.
    PCB_set_state(current_process, interrupted);

    // Save CPU state to current PCB (in our case, the current PC value).
    PCB_set_pc(current_process, PC);

    // Call scheduler
    run_scheduler(timer_interrupt);

    // Set CPU's PC to what is next on the stack. This allows the next process to run.
    PC = SysStack;
}

void run_scheduler(Interrupt interrupt_type) {
    PCB_p previous_pcb;
    PCB_p finished_pcb;
    switch (interrupt_type) {
    case timer_interrupt:
        previous_pcb = current_process;

        //Put current process back in ready queue. Don't put it into the queue if it's the idle process.
        if (current_process != idle_process) {
            FIFOq_enqueue(readyQueue, current_process);
        }

        // Change state of current PCB from interrupted to ready.
        PCB_set_state(current_process, ready);

        // Call dispatcher.
        run_dispatcher();

        // Print the state of the queue (if four or more context switches have already been made).
        if (cswitch_no == 0) {
            cswitch_no = 4;

            // Print stuff only if the old PCB wasn't the idle PCB (since it wasn't enqueued).
            if (previous_pcb != idle_process) {
                char *pcb_string = malloc(200);
                PCB_toString(previous_pcb, pcb_string);
                fprintf(output, "Returned to ready queue: %s\n", pcb_string);
                free(pcb_string);

                int queue_string_size = FIFOq_toString_size(readyQueue);
                char* queue_string = malloc((size_t)queue_string_size);
                FIFOq_toString(readyQueue, queue_string, queue_string_size);
                fprintf(output, "%s\n", queue_string);
                free(queue_string);
            }
        } else {
            // If this call wasn't a print call, decrement the counter.
            cswitch_no--;
        }

        break;
    // io is done, takes process out of waiting queue and puts in ready queue.
    case io1_interrupt:
        finished_pcb = FIFOq_dequeue(waitQueue1);
        PCB_set_state(finished_pcb, ready);
        FIFOq_enqueue(readyQueue, finished_pcb);

        break;
    case io2_interrupt:
        finished_pcb = FIFOq_dequeue(waitQueue2);;
        PCB_set_state(finished_pcb, ready);
        FIFOq_enqueue(readyQueue, finished_pcb);

        break;
    }

    // Housekeeping.
    // Deallocate any terminated PCBs and their resources.
    while (!FIFOq_is_empty(terminateQueue)) {
        PCB_p terminated_pcb = FIFOq_dequeue(terminateQueue);
        PCB_destruct(terminated_pcb);
    }
}

void run_dispatcher() {
    // Save CPU state to current PCB (in our case, the current PC value).
    PCB_set_pc(current_process, PC);

    // Dequeue next waiting PCB, or the idle PCB if no ready PCBs exist.
    if (FIFOq_is_empty(readyQueue)) {
        current_process = idle_process;
    } else {
        current_process = FIFOq_dequeue(readyQueue);
    }

    // Print what process is to be dispatched (if four or more context switches have already been made.
    if (cswitch_no == 0) {
        char* pcb_string = malloc(200);
        PCB_toString(current_process, pcb_string);
        fprintf(output, "Switching to: %s\n", pcb_string);
        free(pcb_string);
    }

    // Change state of the new current PCB to running.
    PCB_set_state(current_process, running);

    // Print what process is now dispatched (if four or more context switches have already been made.
    if (cswitch_no == 0) {
        char* pcb_string = malloc(200);
        PCB_toString(current_process, pcb_string);
        fprintf(output, "Now running: %s\n", pcb_string);
        free(pcb_string);
    }

    // Copy new PC value to system stack.
    SysStack = PCB_get_pc(current_process);
}

int io_timer1(void) {
	timer1--;
	int returnNum = timer1;
	if (timer1 == 0) {
		timer1 = 300;//300
	}
	return returnNum;
}

int io_timer2(void) {
	timer2--;
	int returnNum = timer2;
	if (timer2 == 0) {
		timer2 = 400; //400
	}
	return returnNum;
}

void trap_handler(int device_num) {

    PCB_set_pc(current_process, PC);
    PCB_set_state(current_process, waiting);
    if (device_num == 1) { // puts process that called trap into waiting queue
        FIFOq_enqueue(waitQueue1, current_process);
    } else if (device_num == 2) {
        FIFOq_enqueue(waitQueue2, current_process);
    }
    // gets next process to run
    if (FIFOq_is_empty(readyQueue)) { 
        current_process = idle_process;
    } else {
        current_process = FIFOq_dequeue(readyQueue);
    }

    PCB_set_state(current_process, running);
    SysStack = PCB_get_pc(current_process);
}

void io_ISR(int device_num) {
    if (device_num == 1) {
        run_scheduler(io1_interrupt);
    } else if (device_num == 2) {
        run_scheduler(io2_interrupt);
    }
}


void initPCB(int pid) {
	PCB_p pcb = PCB_construct();
	PCB_init(pcb);
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char* theFTime = asctime(timeinfo);
	//char* theTime = malloc(100);
	//strncpy(theTime, theFTime + 11, 8);
	PCB_set_creation(pcb, theFTime);
    PCB_set_state(pcb, ready);
    PCB_set_pid(pcb, pid);
	FIFOq_enqueue(readyQueue, pcb);
}
