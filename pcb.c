#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "pcb.h"

// Function Prototypes
char* getStateName(State state);

PCB_p PCB_construct (void) {
  return malloc(sizeof(PCB));
}

void PCB_destruct(PCB_p pcb) {
  if (pcb != NULL) free(pcb);
}

int PCB_init(PCB_p pcb) {
    if (pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pid = DEFAULT_PID;
    pcb->state = (State) DEFAULT_STATE;
    pcb->priority = DEFAULT_PRIORITY;
    pcb->pc = DEFAULT_PC;
    return SUCCESS;
}

int PCB_set_pid(PCB_p pcb, unsigned long pid) {
  if (pcb == NULL) {
    return NULL_OBJECT;
  }
  pcb->pid = pid;
  return SUCCESS;
}

unsigned long PCB_get_pid(PCB_p pcb) {
  //error checking in controller
  return pcb->pid;
}

int PCB_set_state(PCB_p pcb, State state) {
  if (pcb == NULL) {
    return NULL_OBJECT;
  }
  pcb->state = state;
  return SUCCESS;
}

State PCB_get_state(PCB_p pcb) {
  return pcb->state;
}

int PCB_set_priority(PCB_p pcb, unsigned short priority) {
  if (pcb == NULL) {
    return NULL_OBJECT;
  }
  pcb->priority = priority;
  return SUCCESS;
}

unsigned short PCB_get_priority(PCB_p pcb) {
  return pcb->priority;
}

int PCB_set_pc(PCB_p pcb, unsigned long pc) {
    if(pcb == NULL) {
        return NULL_OBJECT;
    }
    pcb->pc = pc;
    return SUCCESS;
}

unsigned long PCB_get_pc(PCB_p pcb) {
    return pcb->pc;
}

char* getStateName(State state) {
    switch(state) {
        case new:
            return "new";
        case ready:
            return "ready";
        case running:
            return "running";
        case interrupted:
            return "interrupted";
        case waiting:
            return "waiting";
        case halted:
            return "halted";
    }
}

char * PCB_toString(PCB_p pcb, char * string) {
  sprintf(string, "PID: 0x%0lX, State: %s, Priority: 0x%0X, PC: 0x%04lX",
          pcb->pid, getStateName(pcb->state), pcb->priority, pcb->pc);
  return string;
}

/*
int MAX_PC = 2345; // after PC reaches this, reset to zero
char* creation; //computer clock time when a process is created.
char* termination; //computer clock time when process terminates and goes into termination list
int TERMINATE = 1; //process will terminate after this (1) many times it passes the MAX_PC
int term_count; // counter to keep track of how many times the process has passed MAX_PC
int[] I / O_trap1; //four numbers representing the PC counter where the process will execute -
int[] I / O_trap2; //an I/O service trap. Each # assigned random, no duplicates
*/
//// new fields getters and setters
int PCB_get_MAX_PC(PCB_p pcb) {
	return pcb->MAC_PC;
}

void PCB_set_creation(PCB_p pcb, char* currentTime) {
	pcb->creation = currentTime;
}

void PCB_set_termination(PCB_p, char* currentTime) {
	pcb->termination = currentTime;
}

int PCB_get_TERMINATE(PCB_p pcb) {
	return pcb->TERMINATE;
}

void PCB_set_term_count(PCB_p pcb, int termCount) {
	pcb->term_count = termCount;
}

int PCB_get_term_count(PCB_p pcb) {
	return pcb->term_count;
}

void PCB_set_trap1(PCB_p pcb, int trap1, int index) {
	pcb->trap1[0] = trap1;
}

int[] PCB_get_trap1(PCB_p pcb) {
	pcb->trap1;
}

void PCB_set_trap2(PCB_p, int trap2, int index) {
	pcb->trap2[index] = trap2;
}

int[] PCB_get_trap2(PCB_p) {
	pcb->trap2;
}
