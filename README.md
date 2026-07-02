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
