#include <stdio.h>
#include<stdlib.h>
#include "fifo_queue.h"

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

int main() {
    // Initialize file output.
    output = fopen("scheduleTrace.txt", "w");

    // Initialize the queues.
    ready_PCBs = FIFOq_construct();
    FIFOq_init(ready_PCBs);
    terminated_PCBs = FIFOq_construct();
    FIFOq_init(terminated_PCBs);

    // Initialize the idle process and set it as the current process.
    idle_pcb = PCB_construct();
    PCB_init(idle_pcb);
    PCB_set_state(idle_pcb, running);
    PCB_set_pc(idle_pcb, PC);
    PCB_set_pid(idle_pcb, 0xFFFFFFFF);
    current_pcb = idle_pcb;

    for(int i = 0; i < MAX_LOOPS; i++) {
        os_loop();
    }

    return 0;
}

void os_loop() {
    // Create 0-5 new processes.
    generate_processes();

    // Simulate running of current process.
    PC += ((rand() % 1000) + 3000);
    PCB_set_pc(current_pcb, PC);

    // Push PC to the system stack (pseudo-push).
    SysStack = PC;

    // ISR-call (simulate a timer interrupt).
    perform_ISR();
}

void generate_processes() {
    static int processes_created = 0;

    // Create 0-5 new processes.
    int quantity = rand() % 5;

    for (int i = 0; i < quantity; i++) {
        if(processes_created >= 30) {
            return;
        }

        PCB_p new_process = PCB_construct();
        PCB_init(new_process);
        PCB_set_pid(new_process, (unsigned long) rand());
        PCB_set_state(new_process, new);
        FIFOq_enqueue(ready_PCBs, new_process);

        char* pcb_string = malloc(100);
        PCB_toString(new_process, pcb_string);
        fprintf(output, "Enqueued PCB: %s\n", pcb_string);
        free(pcb_string);

        processes_created++;
    }
}

void perform_ISR() {
    // Change running process to interrupted.
    PCB_set_state(current_pcb, interrupted);

    // Save CPU state to current PCB (in our case, the current PC value).
    PCB_set_pc(current_pcb, PC);

    // Call scheduler
    run_scheduler(timer_interrupt);

    // Set CPU's PC to what is next on the stack. This allows the next process to run.
    PC = SysStack;
}

void run_scheduler(Interrupt interrupt_type) {
    PCB_p previous_pcb;
    switch(interrupt_type) {
        case timer_interrupt:
            previous_pcb = current_pcb;

            // Put current process back in ready queue. Don't put it into the queue if it's the idle process.
            if(current_pcb != idle_pcb) {
                FIFOq_enqueue(ready_PCBs, current_pcb);
            }

            // Change state of current PCB from interrupted to ready.
            PCB_set_state(current_pcb, ready);

            // Call dispatcher.
            run_dispatcher();

            // Print the state of the queue (if four or more context switches have already been made).
            if(cswitch_no == 0) {
                cswitch_no =  4;

                // Print stuff only if the old PCB wasn't the idle PCB (since it wasn't enqueued).
                if(previous_pcb != idle_pcb) {
                    char *pcb_string = malloc(100);
                    PCB_toString(previous_pcb, pcb_string);
                    fprintf(output, "Returned to ready queue: %s\n", pcb_string);
                    free(pcb_string);

                    int queue_string_size = FIFOq_toString_size(ready_PCBs);
                    char* queue_string = malloc((size_t) queue_string_size);
                    FIFOq_toString(ready_PCBs, queue_string, queue_string_size);
                    fprintf(output, "%s\n", queue_string);
                    free(queue_string);
                }
            } else {
                // If this call wasn't a print call, decrement the counter.
                cswitch_no--;
            }

            break;
    }

    // Housekeeping.
    // Deallocate any terminated PCBs and their resources.
    while(!FIFOq_is_empty(terminated_PCBs)) {
        PCB_p terminated_pcb = FIFOq_dequeue(terminated_PCBs);
        PCB_destruct(terminated_pcb);
    }
}

void run_dispatcher() {
    // Save CPU state to current PCB (in our case, the current PC value).
    PCB_set_pc(current_pcb, PC);

    // Dequeue next waiting PCB, or the idle PCB if no ready PCBs exist.
    if(FIFOq_is_empty(ready_PCBs)) {
        current_pcb = idle_pcb;
    } else {
        current_pcb = FIFOq_dequeue(ready_PCBs);
    }

    // Print what process is to be dispatched (if four or more context switches have already been made.
    if(cswitch_no == 0) {
        char* pcb_string = malloc(100);
        PCB_toString(current_pcb, pcb_string);
        fprintf(output, "Switching to: %s\n", pcb_string);
        free(pcb_string);
    }

    // Change state of the new current PCB to running.
    PCB_set_state(current_pcb, running);

    // Print what process is now dispatched (if four or more context switches have already been made.
    if(cswitch_no == 0) {
        char* pcb_string = malloc(100);
        PCB_toString(current_pcb, pcb_string);
        fprintf(output, "Now running: %s\n", pcb_string);
        free(pcb_string);
    }

    // Copy new PC value to system stack.
    SysStack = PCB_get_pc(current_pcb);
}