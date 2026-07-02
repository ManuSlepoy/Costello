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
