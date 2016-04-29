typedef enum interrupt_type {
    timer_interrupt
} Interrupt;

int MAX_LOOPS = 100;

// The number of context switches left before we should print which process we're switching to.
unsigned short cswitch_no = 0;
unsigned long PC = 0;
unsigned long SysStack;
PCB_p current_pcb;
PCB_p idle_pcb;
FIFOq_p ready_PCBs;
FIFOq_p terminated_PCBs;
FILE* output;

// Function Prototypes
void os_loop();
void generate_processes();
void perform_ISR();
void run_scheduler(Interrupt interrupt_type);
void run_dispatcher();