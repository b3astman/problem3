#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "fifo_queue.h"

FIFOq_p readyQueue;
FIFOq_p newQueue;
FIFOq_p terminateQueue;
FIFOq_p io_queue1;
FIFOq_p io_queue2;

void initQueues();
void CPU_loop();
void timerIR();
void pseudoISR();
void io_timer1();
void io_timer2();
void trap_handler();
void initPCB();

//#include "cpu_simulator.c"

