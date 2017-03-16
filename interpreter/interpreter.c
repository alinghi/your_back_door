// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)

// Buffer declaration motivated by P. Conrad's arith.c
// Upto 256 instructor(issue number 66 in GIT)
#define BUFF_SIZE 1024 
char buffer[BUFF_SIZE]; 


// Global variable that indicates if the process is running.
static bool is_running = true;

void usageExit() {
    //show usage
    printf("USAGE: ./interpreter [bytecode_file_name]\n");
    //exit with failure
    exit(1);
}

void halt(struct VMContext* ctx, const uint32_t instr) {
    /*
    This instruction does not require any operand. This instruction
    simply terminates the current process.
    */

    //This printf statement is for debug purpose
    //Will be changed to comment at final commit
    printf("--Halt--\n");



    //Motivated by P.Conrad's arith.c
    is_running = false; 
}

void puti(struct VMContext* ctx, const uint32_t instr) {
    /*
    This instruction moves an 8-bit immediate value to a register. The
    upper 24-bit of the destination register is zeroed out.
    */

    //Extract Operand1 as register Operand2 as immediate
    const uint8_t destination_register = EXTRACT_B1(instr); 
    const uint8_t immediate = EXTRACT_B2(instr);

    //This printf statement is for debug purpose
    //Will be changed to comment at final commit
    printf("--Puti--\n");
    printf("destination_register : %d, %x\n",destination_register,destination_register);
    printf("immediate : %d, %x\n",immediate,immediate); 



    //Actual Operation
    ctx->r[destination_register].value=immediate;
    
    //Test Purpose Printf
    printf("check %d = %d\n",ctx->r[destination_register].value,immediate);


} 


void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = NULL;
    }

    // TODO: initialize function pointers
    f[0x00] = halt;
    f[0x40] = puti;
    // f[0x10] = load;
}

void initRegs(Reg *r, uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}

int main(int argc, char** argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    FILE* bytecode;
    uint32_t* pc;

    //Heap declaration
    uint8_t* heap;

    // Buffer declaration motivated by P. Conrad's arith.c
    // Upto 256 instructor(issue number 66 in GIT)
    char buffer[BUFF_SIZE];
    int bytes_loaded;//bytes_loaded
    int program_counter;//program_counter


    // There should be at least one argument.
    if (argc < 2) usageExit();

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, r, f);

    // Load bytecode file
    bytecode = fopen(argv[1], "rb");
    if (bytecode == NULL) {
        perror("fopen");
        return 1;
    }

    // Read bytecode
    bytes_loaded = fread((void*)&buffer, 1, 1024, bytecode);
    pc = (uint32_t*) &buffer;

    // Allocate Heap(8192)
    heap=(uint8_t*)malloc(8192);
    // Check allocation fail
    if(heap==NULL){
        fprintf(stderr,"heap allocation failed\n");
        exit(1);
    }

    // Init program_counter
    program_counter=0;

    while (is_running) {
        //Read 4-byte bytecode, and set the pc accordingly
        stepVMContext(&vm, &pc);
        program_counter++;
    }

    fclose(bytecode);

    // Zero indicates normal termination.
    return 0;
}
