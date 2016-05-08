//#include <stdio.h>
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
	/*
	for (i = 0; i < 5; i++) {
		CPU_loop();
		printf("\nCPU Loop ran");
	}
	*/
	return 0;
}

void initQueues() {
	printf("we here");
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


void timerIR(void) {

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