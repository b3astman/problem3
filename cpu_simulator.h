#include <stdio.h>
#include <stdlib.h>
#include "fifo_queue.h"
#include "cpu_simulator.h"

fifo_q readyQueue;
fifo_q newQueue;
fifo_q terminate;
fifo_q io_queue1;
fifo_q io_queue2;

void CPU_loop(void);
void timerIR(void);
void pseudoISR(void);
void io_timer1(void);
void io_timer2(void);
void trap_handler(void);
PCB_p initPCB();

