/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Memory Layout of the SpacemiT K1 SoC.
 *
 * Memory map:
 *
 *   Physical Address       Size        Usage
 *   ──────────────────     ──────────  ──────────────────────────────
 *   0x0000_0000            512 KiB     Reserved - M-mode / OpenSBI
 *   0x0008_0000            ~2 GiB      Usable DRAM Bank 0
 *   0x0020_0000            -           ← ThreadX kernel load address
 *   0x7F00_0000            16 MiB      Reserved - framebuffer / runtime
 *   0x8000_0000–0xFFFF_FFFF            PCI / MMIO hole (not DRAM)
 *   0x1_0000_0000          ~14 GiB     Usable DRAM Bank 1
 *
 * Peripheral MMIO:
 *
 *   0xD401_7000            256 B       UART0  (serial console)
 *   0xE000_0000            64 MiB      PLIC   (interrupt controller)
 *   0xE400_0000            64 KiB      CLINT  (timer / IPI)
 *
 * see more details on K1 Spec: 
 *  Web : https://www.spacemit.com/community/document/?k1
 *  PDF : https://cdn-resource.spacemit.com/file/chip/K1/K1_User_Manual_en.pdf
 */


OUTPUT_ARCH( "riscv" )
ENTRY( _start )

PHDRS
{
  text PT_LOAD FLAGS(5);   /* PF_R | PF_X */
  data PT_LOAD FLAGS(6);   /* PF_R | PF_W */
}

SECTIONS
{
  . = 0x00200000;

  .text : {
    KEEP(*(.text.entry))
    *(.text .text.*)
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
  } :text

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*)
    . = ALIGN(16);
    *(.rodata .rodata.*)
  } :text

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*)
    . = ALIGN(16);
    *(.data .data.*)
  } :data

  .bss : {
    . = ALIGN(16);
    _bss_start = .;
    *(.sbss .sbss.*)
    . = ALIGN(16);
    *(.bss .bss.*)
    _bss_end = .;
  } :data

  .stack : {
    . = ALIGN(4096);
    _sysstack_start = .;
    . += 0x4000;
    _sysstack_end = .;
  } :data

  PROVIDE(_end = .);
}
