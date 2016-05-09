/*
* Antonio Alvillar, Bethany Eastman, Edgardo Gutierrez, Gabriel Houle
* Problem 3 OS
*/

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
	//creation; //computer clock time when a process is created.
	//termination; //computer clock time when process terminates and goes into termination list
	pcb->TERMINATE = 1; //process will terminate after this (1) many times it passes the MAX_PC
	pcb->term_count = 2; // counter to keep track of how many times the process has passed MAX_PC
    pcb->MAX_PC = 2345;
	int i;
	for (i = 0; i < 4; i++) {
		pcb->IO_trap1[i] = rand() % pcb->MAX_PC;
		pcb->IO_trap2[i] = rand() % pcb->MAX_PC;
	}
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
  sprintf(string, "PID: 0x%0lX, State: %s, Priority: 0x%0X, PC: 0x%04lX",//, Creation Time: %s",
      pcb->pid, getStateName(pcb->state), pcb->priority, pcb->pc);//, pcb->creation);
  return string;
}

// new fields getters and setters
int PCB_get_MAX_PC(PCB_p pcb) {
	return pcb->MAX_PC;
}

void PCB_set_creation(PCB_p pcb, char* currentTime) {
	pcb->creation = currentTime;
}

void PCB_set_termination(PCB_p pcb, char* currentTime) {
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

int PCB_get_trap1(PCB_p pcb, int index) {
	return (pcb->IO_trap1)[index];
}

int PCB_get_trap2(PCB_p pcb, int index) {
	return (pcb->IO_trap2)[index];
}
