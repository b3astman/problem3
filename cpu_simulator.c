#include "cpu_simulator.h"

int main(void) {
    //initialize file output.
    output = fopen("scheduleTrace.txt", "w");

	// Initialize the queues.
	initQueues();
	int i;
	for (i = 0; i < 10; i++) {
		initPCB();
		//printf("\nPCB initialized");
	}

    idle_process = PCB_construct();
    PCB_init(idle_process);
    PCB_set_state(idle_process, running);
    PCB_set_pc(idle_process, PC);
    PCB_set_pid(idle_process, 0xFFFFFFFF);
    current_process = idle_process;
	
    pthread_create(&timer_thread, NULL, timerIR, NULL);

    for (i = 0; i < 2000; i++) { // this loop will call cpu_loop as many times as we want 
		CPU_loop();             // rather than having cpu_loop have a loop inside of it.
		int first = io_timer1();
		int second = io_timer2();
	}

    int queue_string_size = FIFOq_toString_size(readyQueue); // this block prints new Queue
    char* queue_string = malloc((size_t)queue_string_size);
    FIFOq_toString(readyQueue, queue_string, queue_string_size);
    printf("%s\n", queue_string);
    free(queue_string);

    char* pcb_string = malloc(100);
    PCB_toString(current_process, pcb_string);
    printf("last running: %s\n", pcb_string);
    free(pcb_string);

    //i = 0;
    /*while (1) { // for timer interrupt testing.
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%d: current time and date: %s", i, asctime(timeinfo)); // prints time Antonio!
        i++; 
    }*/
	
	return 0;
}

void initQueues() {
	//newQueue = FIFOq_construct();
	//FIFOq_init(newQueue);
	readyQueue = FIFOq_construct();
	FIFOq_init(readyQueue);
	terminateQueue = FIFOq_construct();
	FIFOq_init(terminateQueue);
	io_queue1 = FIFOq_construct();
	FIFOq_init(io_queue1);
	io_queue2 = FIFOq_construct();
	FIFOq_init(io_queue2);
	waitQueue = FIFOq_construct();
	FIFOq_init(waitQueue);
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
        printf("timerwaiting!\n");
        if (pthread_mutex_trylock(&timer_lock) == 0) {
            printf("timer interrupt!\n");
            //timer interrupt has happened

            // Push PC to the system stack (pseudo-push).
            SysStack = PC;

            // ISR-call (simulate a timer interrupt).
            pseudoISR();

            timer_waiting = 0;
            pthread_mutex_unlock(&timer_lock);// releases control on mutex
            pthread_cond_signal(&timer_cond); // signal to timer to start over
        }
    }
	// "the CPU must compare the PC value with each of the values in the PCB I/O arrays"
	/*for (i = 0; i < 4; i++) {
		if (PCB_get_trap1(current_process, i) == pc) {
			//calls the I/O trap handler passing the trap service routine number (which I/O device is needed).
			trap_handler(PCB_get_trap1(current_process, i));
		}
		if (PCB_get_trap2(current_process, i) == pc) {
			//calls the I/O trap handler passing the trap service routine number (which I/O device is needed).
			trap_handler(PCB_get_trap2(current_process, i));
		}
	}*/
	printf("\nCPU Loop ran\n");
}

void *timerIR(void) {
    pthread_mutex_init(&timer_lock, NULL);
    pthread_cond_init(&timer_cond, NULL);
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 50000000L; // nanoseconds

    int i;
    for (;;) { 
        printf("thread loop!\n");
        if (!timer_waiting) {
            pthread_mutex_lock(&timer_lock); // gets mutex lock
            timer_waiting = 1;
            nanosleep(&tim, &tim2); // puts thread to sleep
            pthread_cond_wait(&timer_cond, &timer_lock); // unlocks mutex and waits until interrupt is read by the CPU
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
                char *pcb_string = malloc(100);
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
    }
    //// Housekeeping.
    //// Deallocate any terminated PCBs and their resources.
    //while (!FIFOq_is_empty(terminateQueue)) {
    //    PCB_p terminated_pcb = FIFOq_dequeue(terminateQueue);
    //    PCB_destruct(terminated_pcb);
    //}
}

void run_dispatcher() {
    // Save CPU state to current PCB (in our case, the current PC value).
    PCB_set_pc(current_process, PC);
    printf("dispatcher!");

    // Dequeue next waiting PCB, or the idle PCB if no ready PCBs exist.
    if (FIFOq_is_empty(readyQueue)) {
        current_process = idle_process;
    } else {
        //printf("switching to new process");
        current_process = FIFOq_dequeue(readyQueue);
    }

    // Print what process is to be dispatched (if four or more context switches have already been made.
    //if (cswitch_no == 0) {
        char* pcb_string = malloc(1000);
        PCB_toString(current_process, pcb_string);
        printf("Switching to: %s\n", pcb_string);
        free(pcb_string);
    //}

    // Change state of the new current PCB to running.
    PCB_set_state(current_process, running);

    // Print what process is now dispatched (if four or more context switches have already been made.
    if (cswitch_no == 0) {
        char* pcb_string = malloc(100);
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
		timer1 = 900;
	}
	return returnNum;
}

int io_timer2(void) {
	timer2--;
	int returnNum = timer2;
	if (timer2 == 0) {
		timer2 = 1200;
	}
	return returnNum;
}
void trap_handler(int trap_service_routine_number) {
	// active the IO device?
	int what = io_timer1(); // How do I get the 1 back to CPU
	
	// "taking the running process out of that state and putting it into the waiting queue for the appropriate device"

	FIFOq_enqueue(waitQueue, current_process);

	// "This act also activates an internal timer in the device"
	timerIR();

	// Should we make a device class?! - Elijah
}


void initPCB() {
	PCB_p pcb = PCB_construct();
	PCB_init(pcb);
    PCB_set_state(pcb, ready);
	FIFOq_enqueue(readyQueue, pcb);
}
