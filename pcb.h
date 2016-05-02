
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

  int MAX_PC = 2345;
  //Computer clock time when process was created. You will need
  // to capture the time when the process is instantiated and
  // store it here. 
  char* creationTime;
  //Computer clock time when the process terminates and goes into
  //the termination list. If the process does not terminiate 
  //during the run no time need be recorded. 
  char* terminationTime;
  //This is a control field. if the number is 0 then the process 
  //is not one that terminates over the course of the simulation
  //run.If it is greater than 0 then this is the number of times 
  //that the PC will pass its MAX_PC value. for example if this 
  //number is set to 15 then the process will terminate after 15
  //times it passes 2345 execution cycles. 
  int terminateCount;
  //This is the counter to keep track of how many times the 
  //process has passed its MAX_PC value.
  Term_Count
  //These two fields can be simple integer arrays. Each one contain
  //four numbers representing the PC count where the process will
  //execute an I/O service trap. Each number in each array should
  // be assigned randomly but you must make sure there are no 
  //duplicates. The algorithm for invoking the trap call given.
  I/O_1/2_Traps
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

#endif
