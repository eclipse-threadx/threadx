/*  Startup code for the Cortex-M23 example.
 *
 *  This is the Armv7-M crt0 moved to unified assembler syntax.
 *
 *  The Armv7-M file carries no syntax directive, so GNU as reads it in the
 *  legacy divided syntax, where a Thumb data-processing instruction sets the
 *  flags whether or not the mnemonic says so: "mov r2, #0" assembles to 2200,
 *  MOVS. LLVM's integrated assembler implements unified syntax only, and there
 *  those same mnemonics mean the non-flag-setting forms, which Armv8-M Baseline
 *  does not provide. It rejects the file rather than guessing.
 *
 *  So this file selects unified syntax and spells every one of those
 *  instructions with its trailing s. The encodings do not change: disassembling
 *  the Cortex-M4 object built by GNU shows 2200 movs, 1a52 subs and 3001 adds
 *  already, so this states explicitly what that toolchain was choosing
 *  implicitly. The flags are load bearing -- crt0_memory_copy branches on the
 *  result of "subs r2, r2, r1" and again on "subs r2, #1", so a genuinely
 *  non-flag-setting form would break both loops.
 */

 .global _start
  .extern main


  .syntax unified

  .section .init, "ax"
  .code 16
  .align 2
  .thumb_func


_start:
  CPSID   i
  ldr r1, =__stack_end__
  mov sp, r1

  /* Copy initialised sections into RAM if required. */
  ldr r0, =__data_load_start__
  ldr r1, =__data_start__
  ldr r2, =__data_end__
  bl crt0_memory_copy
  ldr r0, =__text_load_start__
  ldr r1, =__text_start__
  ldr r2, =__text_end__
  bl crt0_memory_copy
  ldr r0, =__fast_load_start__
  ldr r1, =__fast_start__
  ldr r2, =__fast_end__
  bl crt0_memory_copy
  ldr r0, =__ctors_load_start__
  ldr r1, =__ctors_start__
  ldr r2, =__ctors_end__
  bl crt0_memory_copy
  ldr r0, =__dtors_load_start__
  ldr r1, =__dtors_start__
  ldr r2, =__dtors_end__
  bl crt0_memory_copy
  ldr r0, =__rodata_load_start__
  ldr r1, =__rodata_start__
  ldr r2, =__rodata_end__
  bl crt0_memory_copy


  /* Zero bss. */
  ldr r0, =__bss_start__
  ldr r1, =__bss_end__
  movs r2, #0
  bl crt0_memory_set

  /* Setup heap - not recommended for Threadx but here for compatibility reasons */
  ldr r0, = __heap_start__
  ldr r1, = __heap_end__
  subs r1, r1, r0
  movs r2, #0
  str r2, [r0]
  adds r0, r0, #4
  str r1, [r0]

  /* constructors in case of using C++ */
  ldr r0, =__ctors_start__
  ldr r1, =__ctors_end__
crt0_ctor_loop:
  cmp r0, r1
  beq crt0_ctor_end
  ldr r2, [r0]
  adds r0, #4
  push {r0-r1}
  blx r2
  pop {r0-r1}
  b crt0_ctor_loop
crt0_ctor_end:

  /* Setup call frame for main() */
  movs r0, #0
  mov lr, r0
  mov r12, sp

start:
  /* Jump to main() */
  movs r0, #0
  movs r1, #0
  ldr r2, =main
  blx r2
  /*  when main returns, loop forever. */
crt0_exit_loop:
  b crt0_exit_loop



  /* Startup helper functions. */

crt0_memory_copy:
  cmp r0, r1
  beq memory_copy_done
  subs r2, r2, r1
  beq memory_copy_done
memory_copy_loop:
  ldrb r3, [r0]
  adds r0, r0, #1
  strb r3, [r1]
  adds r1, r1, #1
  subs r2, r2, #1
  bne memory_copy_loop
memory_copy_done:
  bx lr

crt0_memory_set:
  cmp r0, r1
  beq memory_set_done
  strb r2, [r0]
  adds r0, r0, #1
  b crt0_memory_set
memory_set_done:
  bx lr

  /* Setup attibutes of stack and heap sections so they don't take up room in the elf file */
  .section .stack, "wa", %nobits
  .section .stack_process, "wa", %nobits
  .section .heap, "wa", %nobits
