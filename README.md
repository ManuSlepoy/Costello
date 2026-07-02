# Caso 1
## Descripción: 
Testeo de la instrucción ADDI (Aritmética con Inmediato) en el espacio de memoria de usuario (`0x00000000`) para comprobar la inicialización de registros y la extensión de signo.

## Instructions:
Instrucciones utilizadas (Formato I):
* ADDI $8, $0, 15  -> Código de máquina: 0x2008000F
* ADDI $9, $0, -5  -> Código de máquina: 0x2009FFFB

## Precondiciones:
* Se realiza un `reset` del sistema, situando al Program Counter (PC) inicialmente en `0xF0000000`.
* Se cargan las instrucciones de forma directa en las direcciones de memoria de usuario `0x00` y `0x04` mediante el comando `s`.
* Se reubica manualmente el registro `PC` a la dirección `0x00000000` mediante el comando `set PC`.

## Code
RTM32> reset
System reset sequence complete. Target PC: 0xF0000000 (Mode: KERNEL)
RTM32> s [0x00] 0x2008000F
RTM32> s [0x04] 0x2009FFFB
RTM32> set PC 0x00000000
Program Counter (PC) set to 0x00000000
RTM32> n 2
Stepped instructions. Target PC: 0x00000008
RTM32> r

# Caso 2
## Descripción:
Testeo de la instrucción J (Jump - Salto Incondicional) para verificar la correcta decodificación del campo de dirección de salto y la modificación directa sobre el Program Counter (PC).

## Instructions:
Instrucción utilizada en formato J:
* J 0x00000010 -> Código de máquina: 0x10000004 (Dirección dividida por 4 para remover alineación: 4 = 0x0000004)

## Precondiciones:
* Se realiza un `reset` del sistema (PC inicializa en `0xF0000000`).
* Se almacena el código hexadecimal de la instrucción en la dirección de memoria `0x00000000` con el comando `s`.
* Se reubica manualmente el `PC` a `0x00000000` usando `set PC`.

## Code
RTM32> reset
System reset sequence complete. Target PC: 0xF0000000 (Mode: KERNEL)
RTM32> s [0x00] 0x10000004
RTM32> set PC 0x00000000
Program Counter (PC) set to 0x00000000
RTM32> n 1
Stepped instructions. Target PC: 0x00000010
RTM32> r

# Caso 3
## Descripción:
Testeo de la instrucción JAL (Jump and Link) para verificar la ejecución simultánea de la bifurcación incondicional del flujo del programa y el almacenamiento automático de la dirección de retorno ($PC + 4$) en el registro de enlace `R[31]` ($ra$).

## Instructions:
Instrucción utilizada en formato J:
* JAL 0x00000020 -> Código de máquina: 0x18000008 (Dirección dividida por 4 para remover alineación: 8 = 0x0000008)

## Precondiciones:
* Se realiza un `reset` del sistema (PC inicializa en `0xF0000000`).
* Se inyecta la instrucción en la dirección de memoria de usuario `0x00000000` mediante el comando `s`.
* Se reubica manualmente el registro `PC` a `0x00000000` mediante el comando `set PC`.

## Code
RTM32> reset
System reset sequence complete. Target PC: 0xF0000000 (Mode: KERNEL)
RTM32> s [0x00] 0x18000008
RTM32> set PC 0x00000000
Program Counter (PC) set to 0x00000000
RTM32> n 1
Stepped instructions. Target PC: 0x00000020
RTM32> r

# Caso 4
## Descripción:
Testeo de la instrucción BEQ (Branch if Equal) para verificar la correcta evaluación de la condición de igualdad entre dos registros y la bifurcación condicional relativa al PC.

## Instructions:
Instrucción utilizada en formato I (Branch):
* BEQ $0, $1, 4  -> Código de máquina: 0x20020004
* Lógica esperada: Dado que R[0] y R[1] contienen inicialmente 0x00000000, la condición de igualdad se cumple de forma efectiva y la CPU debería saltar a PC + 4 + (4 * 4) = 0x00000014.

## Precondiciones:
* Se realiza un reset de la máquina.
* Se inyecta la instrucción BEQ en la dirección de memoria 0x00000000 mediante el comando s.
* Se reubica manualmente el registro PC a 0x00000000 mediante el comando set PC.

## Code
RTM32> reset
System reset sequence complete. Target PC: 0xF0000000 (Mode: KERNEL)
RTM32> s [0x00] 0x20020004
RTM32> s [0x04] 0x2009FFFB
RTM32> set PC 0x00000000
Program Counter (PC) set to 0x00000000
RTM32> n 1
Stepped instructions. Target PC: 0x00000004
RTM32> r

## Postcondiciones:
* La consola reporta un avance a Target PC: 0x00000004.
* Al inspeccionar con el comando r, el registro PC se encuentra efectivamente en 0x00000004 en lugar del destino calculado (0x14).
* El indicador de excepción CAUSE se mantiene en 0x00000000, confirmando que la instrucción fue decodificada pero no ejecutó la acción esperada.

## Conclusiones:
Fallo detectado. La instrucción BEQ no funciona de la forma esperada bajo el Modo Kernel (o presenta un defecto de diseño en la unidad de control del hardware). A pesar de cumplirse estrictamente la condición de igualdad aritmética (0 = 0), el procesador omitió la señal de control de toma de salto condicional (Branch Taken) y ejecutó un avance de ciclo puramente secuencial (PC + 4), ignorando el desplazamiento inmediato calculado.

# Caso 5
## Descripción:
Testeo de la instrucción BNE (Branch if Not Equal) para verificar la correcta evaluación de la condición de desigualdad entre dos registros y la bifurcación condicional relativa al PC.

## Instructions:
Instrucción utilizada en formato I (Branch):
* BNE $0, $1, 4  -> Código de máquina: 0x28020004
* Lógica esperada: Dado que R[0] y R[1] contienen inicialmente 0x00000000, la condición de desigualdad es falsa (0 != 0 es falso) por lo que la CPU NO debe tomar el salto y debe avanzar secuencialmente a PC = 0x00000004 sin alterar ningún registro.

## Precondiciones:
* Se realiza un reset de la máquina.
* Se inyecta la instrucción BNE en la dirección de memoria 0x00000000 mediante el comando s.
* Se reubica manualmente el registro PC a 0x00000000 mediante el comando set PC.

## Code
RTM32> reset
System reset sequence complete. Target PC: 0xF0000000 (Mode: KERNEL)
RTM32> s [0x00] 0x28020004
RTM32> set PC 0x00000000
Program Counter (PC) set to 0x00000000
RTM32> n 1
Stepped instructions. Target PC: 0x00000004
RTM32> r

## Postcondiciones:
* La consola reporta un avance secuencial a Target PC: 0x00000004.
* Al inspeccionar el banco de registros con r, se observa un comportamiento anómalo: el registro R[1] fue modificado de forma inesperada adquiriendo el valor 0x00000004.
* El indicador de excepción CAUSE permanece en 0x00000000.

## Conclusiones:
Fallo detectado. Aunque el Program Counter (PC) avanzó de forma secuencial simulando la caída por falso de la condición, la instrucción BNE alteró de forma destructiva el contenido del registro de destino R[1], cargando en él el valor del desplazamiento inmediato (0x4). Esto delata un error crítico de diseño en los caminos de datos o en la decodificación de opcodes por parte de la CPU, confundiendo la lógica del salto condicional BNE con una operación de escritura o carga inmediata.

# Caso 6
## Descripción:
Testeo de la instrucción ANDI (And Immediate) perteneciente al Formato L, para comprobar la operación lógica bit a bit contra una constante inmediata extendida con ceros y verificar el comportamiento reportado en la documentación sobre posibles fallos de diseño.

## Instructions:
Instrucción utilizada en formato L:
* ANDI $1, $0, 0xFFFF -> Código de máquina: 0x2001FFFF
* Lógica esperada: La CPU debería realizar la operación lógica AND bit a bit entre el contenido de R[0] (0x00000000) y la constante inmediata extendida con ceros (0x0000FFFF), almacenando el resultado (0x00000000) en el registro de destino R[1].

## Precondiciones:
* Se realiza un reset completo del sistema.
* Se escribe el código hexadecimal de la instrucción en la dirección de memoria 0x00000000 mediante el comando s.
* Se fuerza manualmente el registro PC a la dirección de memoria de usuario 0x00000000 usando set PC.

## Code
RTM32> reset
System reset sequence complete. Target PC: 0xF0000000 (Mode: KERNEL)
RTM32> s [0x00] 0x2001FFFF
RTM32> set PC 0x00000000
Program Counter (PC) set to 0x00000000
RTM32> n 1
Stepped instructions. Target PC: 0x00000004
RTM32> r

## Postcondiciones:
* El Program Counter (PC) avanza de manera exitosa a la dirección secuencial inmediata 0x00000004.
* Al inspeccionar el banco de registros generales mediante el comando r, se comprueba que todos los registros, incluido R[1], permanecen inalterados en 0x00000000.
* El registro especial CAUSE se mantiene en 0x00000000, indicando la ausencia de excepciones por código de operación inválido.

## Conclusiones:
Anduvo de forma nominal teórica, pero bajo sospecha por documentación. Aunque el resultado final en el registro R[1] es matemáticamente correcto (0 AND 0xFFFF = 0), debido a que la CPU no actualizó ninguna bandera y considerando las advertencias explícitas del manual de usuario sobre un bug crítico latente en la unidad de control para la instrucción ANDI[cite: 125], este test se considera inconcluso para validar el camino de datos lógicos. No es posible determinar únicamente con valores en cero si el bit 'h' de control de parte superior/inferior del Formato L [cite: 100] está operando de forma defectuosa o si el bus de la ALU hacia el banco de registros se encuentra abierto para este opcode.
