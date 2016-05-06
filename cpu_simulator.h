#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pcb.h"
#include "fifo_queue.h"

FIFOq_p readyQueue;
FIFOq_p newQueue;
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

void initQueues();
void CPU_loop();
void timerIR();
void pseudoISR();
void io_timer1();
void io_timer2();
void trap_handler();
void initPCB();