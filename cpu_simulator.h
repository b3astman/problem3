/*
 * Antonio Alvillar, Bethany Eastman, Edgardo Gutierrez, Gabriel Houle
 * Problem 3 OS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pcb.h"
#include "fifo_queue.h"

typedef enum interrupt_type {
    timer_interrupt,
    io1_interrupt,
    io2_interrupt
} Interrupt;

int MAX_PC = 2345;

PCB_p current_process;
PCB_p idle_process;

// The number of context switches left before we should print which process we're switching to.
unsigned short cswitch_no = 0;
unsigned long PC = 0;
unsigned long SysStack;
int quantum = 300; // number of execution cycles.
int timer1 = 900;
int timer2 = 1200;

FIFOq_p readyQueue;
FIFOq_p waitQueue1;
FIFOq_p waitQueue2;
FIFOq_p terminateQueue;
FIFOq_p io_queue1;
FIFOq_p io_queue2;

//thread id
pthread_t timer_thread;

//mutex
pthread_mutex_t timer_lock;

//cond var
pthread_cond_t timer_cond;

int timer_waiting = 0; // boolean that helps with timer ir
int timer_ir_count = 0; // used for testing delete!

FILE* output;

void initQueues();
void CPU_loop();
void *timerIR();
void pseudoISR();
void run_scheduler(Interrupt);
void run_dispatcher();
int io_timer1();
int io_timer2();
void trap_handler(int);
void io_ISR(int);
void initPCB(int);
