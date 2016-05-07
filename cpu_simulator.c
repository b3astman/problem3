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
        printf("%d: current time and date: %s", i, asctime(timeinfo)); i++; // prints time Antonio!
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
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 50000000L;

    int i;
    for (;;) { // make this for (;;) at the end so this loops forever.
        printf("thread loop!\n");
        nanosleep(&tim, &tim2);
    }
}

void pseudoISR(void) {

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