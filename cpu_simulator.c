#include <stdio.h>
#include <stdlib.h>
#include "fifo_queue.h"
#include "cpu_simulator.h"

int main(void) {

	return 0;
}

void CPU_loop(void);
void timerIR(void) {

}
void pseudoISR(void);
void io_timer1(void);
void io_timer2(void);
void trap_handler(void);
PCB_p initPCB();