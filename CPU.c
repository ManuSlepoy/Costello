#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Configuración de la CPU
#define MEM_SIZE 65536    // 64 KBytes
#define NUM_REGS 16       // 16 registros
#define PC_START 0        // PC inicial

// Opcodes de instrucciones
typedef enum {
    OP_ADD = 0,   // R-type
    OP_LW = 1,    // I-type (load word)
    OP_SW = 2,    // I-type (store word)
    OP_BEQ = 3,   // I-type (branch if equal)
    OP_J = 4      // J-type (jump)
} Opcode;

// Estructura de la CPU
typedef struct {
    uint8_t memoria[MEM_SIZE];     // Memoria de 64 KB
    int32_t regs[NUM_REGS];        // 16 registros de 32 bits
    uint16_t pc;                   // Program Counter (16 bits para 64KB)
    int32_t ir;                    // Instruction Register (32 bits)
    int running;                   // Flag de ejecución
} CPU;

// Formato de instrucción R-type: [opcode(4) | rs(4) | rt(4) | rd(4) | funct(16)]
typedef struct {
    uint8_t opcode : 4;
    uint8_t rs : 4;
    uint8_t rt : 4;
    uint8_t rd : 4;
    uint16_t funct : 16;
} RType;

// Formato de instrucción I-type: [opcode(4) | rs(4) | rt(4) | immediate(20)]
typedef struct {
    uint8_t opcode : 4;
    uint8_t rs : 4;
    uint8_t rt : 4;
    int32_t immediate : 20;
} IType;

// Formato de instrucción J-type: [opcode(4) | address(28)]
typedef struct {
    uint8_t opcode : 4;
    uint32_t address : 28;
} JType;

// Inicializar la CPU
void inicializar_cpu(CPU *cpu) {
    memset(cpu->memoria, 0, MEM_SIZE);
    memset(cpu->regs, 0, NUM_REGS * sizeof(int32_t));
    cpu->pc = PC_START;
    cpu->ir = 0;
    cpu->running = 1;
    
    // Inicializar stack pointer (registro 15)
    cpu->regs[15] = MEM_SIZE - 4;
}

// Cargar instrucción desde memoria
int32_t fetch_instruction(CPU *cpu) {
    if (cpu->pc + 3 >= MEM_SIZE) {
        printf("Error: PC fuera de límites\n");
        cpu->running = 0;
        return 0;
    }
    
    // Leer 4 bytes de memoria (little-endian)
    int32_t instruccion = cpu->memoria[cpu->pc] |
                         (cpu->memoria[cpu->pc + 1] << 8) |
                         (cpu->memoria[cpu->pc + 2] << 16) |
                         (cpu->memoria[cpu->pc + 3] << 24);
    
    return instruccion;
}

// Decodificar y ejecutar instrucción
void ejecutar_instruccion(CPU *cpu) {
    cpu->ir = fetch_instruction(cpu);
    if (!cpu->running) return;
    
    // Extraer opcode (bits 28-31)
    uint8_t opcode = (cpu->ir >> 28) & 0x0F;
    uint16_t pc_incremento = 4;
    
    switch (opcode) {
        case OP_ADD: {
            // ADD rd, rs, rt (R-type)
            RType *r = (RType*)&cpu->ir;
            
            if (r->rd == 0) {
                printf("Error: No se puede modificar el registro \$0\n");
                break;
            }
            
            cpu->regs[r->rd] = cpu->regs[r->rs] + cpu->regs[r->rt];
            printf("ADD r%d = r%d + r%d = %d\n", 
                   r->rd, r->rs, r->rt, cpu->regs[r->rd]);
            break;
        }
        
        case OP_LW: {
            // LW rt, immediate(rs) (I-type)
            IType *i = (IType*)&cpu->ir;
            
            if (i->rt == 0) {
                printf("Error: No se puede modificar el registro \$0\n");
                break;
            }
            
            int32_t addr = cpu->regs[i->rs] + i->immediate;
            
            if (addr < 0 || addr + 3 >= MEM_SIZE) {
                printf("Error: Dirección de memoria inválida %d\n", addr);
                cpu->running = 0;
                break;
            }
            
            // Leer palabra de memoria (little-endian)
            cpu->regs[i->rt] = cpu->memoria[addr] |
                              (cpu->memoria[addr + 1] << 8) |
                              (cpu->memoria[addr + 2] << 16) |
                              (cpu->memoria[addr + 3] << 24);
            
            printf("LW r%d = MEM[%d] = %d\n", i->rt, addr, cpu->regs[i->rt]);
            break;
        }
        
        case OP_SW: {
            // SW rt, immediate(rs) (I-type)
            IType *i = (IType*)&cpu->ir;
            
            int32_t addr = cpu->regs[i->rs] + i->immediate;
            
            if (addr < 0 || addr + 3 >= MEM_SIZE) {
                printf("Error: Dirección de memoria inválida %d\n", addr);
                cpu->running = 0;
                break;
            }
            
            // Escribir palabra en memoria (little-endian)
            int32_t valor = cpu->regs[i->rt];
            cpu->memoria[addr] = valor & 0xFF;
            cpu->memoria[addr + 1] = (valor >> 8) & 0xFF;
            cpu->memoria[addr + 2] = (valor >> 16) & 0xFF;
            cpu->memoria[addr + 3] = (valor >> 24) & 0xFF;
            
            printf("SW MEM[%d] = r%d = %d\n", addr, i->rt, valor);
            break;
        }
        
        case OP_BEQ: {
            // BEQ rs, rt, immediate (I-type)
            IType *i = (IType*)&cpu->ir;
            
            if (cpu->regs[i->rs] == cpu->regs[i->rt]) {
                // Saltar: PC = PC + 4 + (immediate * 4)
                pc_incremento = 4 + (i->immediate * 4);
                printf("BEQ: Branch tomado a PC=%d\n", cpu->pc + pc_incremento);
            } else {
                printf("BEQ: Branch no tomado\n");
            }
            break;
        }
        
        case OP_J: {
            // J address (J-type)
            JType *j = (JType*)&cpu->ir;
            
            // PC = (PC & 0xF0000000) | (address << 2)
            uint16_t nuevo_pc = (j->address << 2) & 0xFFFF;
            cpu->pc = nuevo_pc;
            pc_incremento = 0;  // No incrementar más
            
            printf("J: Saltando a dirección %d\n", nuevo_pc);
            break;
        }
        
        default:
            printf("Error: Opcode desconocido %d\n", opcode);
            cpu->running = 0;
            break;
    }
    
    // Incrementar PC (si no es salto J que ya modificó PC)
    if (opcode != OP_J) {
        cpu->pc += pc_incremento;
    }
}

// Cargar programa en memoria
void cargar_programa(CPU *cpu, int32_t *programa, int tamano) {
    if (tamano * 4 > MEM_SIZE) {
        printf("Error: Programa demasiado grande\n");
        return;
    }
    
    for (int i = 0; i < tamano; i++) {
        int32_t instruccion = programa[i];
        int direccion = i * 4;
        
        // Almacenar en formato little-endian
        cpu->memoria[direccion] = instruccion & 0xFF;
        cpu->memoria[direccion + 1] = (instruccion >> 8) & 0xFF;
        cpu->memoria[direccion + 2] = (instruccion >> 16) & 0xFF;
        cpu->memoria[direccion + 3] = (instruccion >> 24) & 0xFF;
    }
}

// Mostrar estado de la CPU
void mostrar_estado(CPU *cpu) {
    printf("\n=== Estado de la CPU ===\n");
    printf("PC: %d\n", cpu->pc);
    printf("IR: 0x%08X\n", cpu->ir);
    printf("\nRegistros:\n");
    for (int i = 0; i < NUM_REGS; i++) {
        printf("$r%d: %d\t", i, cpu->regs[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n");
}

// Ejecutar ciclo completo
void ejecutar_ciclo(CPU *cpu) {
    if (cpu->pc >= MEM_SIZE - 3) {
        printf("Fin de ejecución: PC fuera de memoria\n");
        cpu->running = 0;
        return;
    }
    
    ejecutar_instruccion(cpu);
}

// Ejemplo de uso
int main() {
    CPU cpu;
    inicializar_cpu(&cpu);
    
    // Programa de ejemplo:
    // ADD r1, r0, r5  -> r1 = 5 (r0 siempre es 0)
    // ADD r2, r1, r3  -> r2 = r1 + 3
    // SW r2, 0(r0)    -> MEM[0] = r2
    // LW r4, 0(r0)    -> r4 = MEM[0]
    
    // Construir instrucciones manualmente
    int32_t programa[] = {
        // ADD r1, r0, r5 (R-type)
        // R[opcode=0][rs=0][rt=0][rd=1][funct=5]
        0x01050000,
        
        // ADD r2, r1, r3 (R-type)  
        // R[opcode=0][rs=1][rt=0][rd=2][funct=3]
        0x02130000,
        
        // SW r2, 0(r0) (I-type)
        // I[opcode=2][rs=0][rt=2][immediate=0]
        0x22000000,
        
        // LW r4, 0(r0) (I-type)
        // I[opcode=1][rs=0][rt=4][immediate=0]
        0x14000000,
        
        // HALT (instrucción nula para detener)
        0x00000000
    };
    
    cargar_programa(&cpu, programa, 5);
    
    printf("Iniciando ejecución...\n");
    mostrar_estado(&cpu);
    
    // Ejecutar ciclo por ciclo
    while (cpu.running && cpu.pc < MEM_SIZE - 3) {
        // Verificar si llegamos a instrucción nula
        int32_t instr = fetch_instruction(&cpu);
        if (instr == 0) {
            printf("Instrucción nula encontrada. Deteniendo ejecución.\n");
            break;
        }
        
        printf("\n--- Ciclo %d ---\n", cpu.pc / 4);
        ejecutar_ciclo(&cpu);
    }
    
    mostrar_estado(&cpu);
    
    return 0;
}
