#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pcb.h"
#include "fifo_queue.h"

typedef enum interrupt_type {
    timer_interrupt
} Interrupt;

int MAX_PC = 2345;

PCB_p current_process;
PCB_p idle_process;

// The number of context switches left before we should print which process we're switching to.
unsigned short cswitch_no = 0;
unsigned long PC = 0;
unsigned long SysStack;
int quantum = 300; // number of execution cycles.

FIFOq_p readyQueue;
FIFOq_p waitQueue; // I think we need this? - Elijah
FIFOq_p terminateQueue;
FIFOq_p io_queue1;
FIFOq_p io_queue2;

//thread ids
pthread_t timer_thread;
pthread_t io1_thread;
pthread_t io2_thread;

//mutexes
pthread_mutex_t timer_lock;
pthread_mutex_t io1_lock;
pthread_mutex_t io2_lock;

pthread_cond_t timer_cond;

int timer_waiting = 0; // boolean that helps with timer ir

FILE* output;

void initQueues();
void CPU_loop();
void *timerIR();
void pseudoISR();
void run_scheduler(Interrupt);
void run_dispatcher();
void io_timer1();
void io_timer2();
void trap_handler(int trap_service_routine_number);
void initPCB();
