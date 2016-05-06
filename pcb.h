
#ifndef PCB_H
#define PCB_H

/* Default values */
#define DEFAULT_PID 0
#define DEFAULT_STATE 0
#define DEFAULT_PC 0
#define DEFAULT_PRIORITY 0

/* Error Handling Values */
#define SUCCESS 0
#define NULL_OBJECT -1

/* typedefs for State, PCB, and PCB_p */
typedef enum state_type {
  new, ready, running, interrupted, waiting, halted
} State;

typedef struct pcb {
  unsigned long pid;
  State state;
  unsigned short priority;
  unsigned long pc;

  int MAX_PC;// = 2345; // after PC reaches this, reset to zero
  char * creation; //computer clock time when a process is created. 
  char * termination; //computer clock time when process terminates and goes into termination list
  int TERMINATE; //process will terminate after this (1) many times it passes the MAX_PC
  int term_count; // counter to keep track of how many times the process has passed MAX_PC
  int IO_trap1[4]; //four numbers representing the PC counter where the process will execute -
  int IO_trap2[4]; //an I/O service trap. Each # assigned random, no duplicates

} PCB;

typedef PCB * PCB_p;

/* Function Prototypes */
PCB_p PCB_construct(void);
void PCB_destruct(PCB_p pcb);
int PCB_init(PCB_p pcb);
int PCB_set_pid(PCB_p pcb, unsigned long pid);
unsigned long PCB_get_pid(PCB_p pcb);
int PCB_set_state(PCB_p pcb, State state);
State PCB_get_state(PCB_p pcb);
int PCB_set_priority(PCB_p pcb, unsigned short priority);
unsigned short PCB_get_priority(PCB_p pcb);
int PCB_set_pc(PCB_p pcb, unsigned long pc);
unsigned long PCB_get_pc(PCB_p pcb);
char * PCB_toString(PCB_p pcb, char * string);
//// new fields (getters/setters)
int PCB_get_MAX_PC();
void PCB_set_creation(PCB_p, char*);
void PCB_set_termination(PCB_p, char*);
int PCB_get_TERMINATE();
void PCB_set_term_count(PCB_p, int);
int PCB_get_term_count(PCB_p);
//void PCB_set_trap1(int);
int PCB_get_trap1(PCB_p, int);
//void PCB_set_trap2(int);
int PCB_get_trap2(PCB_p, int);

#include "pcb.c"
#endif
