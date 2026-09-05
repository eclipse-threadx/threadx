/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 * Modifications Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

ENTRY(_start)

/* ------------------------------------------------------------------------------
 * Vendor ROM & Peripheral Symbol Linker Script Inclusions
 * Reuses Espressif ROM API symbols and peripheral base register definitions
 * ------------------------------------------------------------------------------
 */
INCLUDE "esp32c6.peripherals.ld"
INCLUDE "esp32c6.rom.ld"
INCLUDE "esp32c6.rom.api.ld"
INCLUDE "esp32c6.rom.libgcc.ld"
INCLUDE "esp32c6.rom.newlib.ld"

/* ------------------------------------------------------------------------------
 * Memory Map Definition
 * ------------------------------------------------------------------------------
 * ESP32-C6 512 KB Internal SRAM (shared between IRAM and DRAM)
 */
MEMORY
{
  ram_seg (RWAX) : ORIGIN = 0x40800000, LENGTH = 0x80000
}

SECTIONS
{
  /* Boot Assembly Entry Point, Vendor App Descriptor, & RISC-V Vector Table */
  .iram0.text :
  {
    _text_start = .;

    /*
     * Espressif 2nd-stage Bootloader Requirement:
     * App description header (.rodata_desc) MUST be at the very top of segment 0.
     * KEEP ensures --gc-sections does not discard the app descriptor.
     */
    KEEP(*(.rodata_desc .rodata_desc.*))
    KEEP(*(.app_desc .app_desc.*))

    KEEP(*(.iram0.text))
    KEEP(*(.vectors))
    KEEP(*(.entry))
    *(.text .text.*)
    _text_end = .;
  } > ram_seg

  /* Read-Only Data (String Literals & Constants) */
  .rodata :
  {
    . = ALIGN(4);
    _rodata_start = .;
    *(.rodata .rodata.*)
    *(.srodata .srodata.*)
    . = ALIGN(4);
    _rodata_end = .;
  } > ram_seg

  /* Initialized Global Data */
  .data :
  {
    . = ALIGN(4);
    _data_start = .;
    PROVIDE(__global_pointer$ = . + 0x800);
    *(.data .data.*)
    *(.sdata .sdata.*)
    . = ALIGN(4);
    _data_end = .;
  } > ram_seg

  /* Uninitialized Global Data (BSS - Zeroed by entry.S) */
  .bss :
  {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } > ram_seg

  /* Early System Initialization Stack (16-Byte Aligned for RISC-V C ABI) */
  .stack :
  {
    . = ALIGN(16);
    . += 0x1000; /* 4 KB Stack */
    _sp = .;
  } > ram_seg

  /* First available RAM address for ThreadX System Byte Pool */
  . = ALIGN(16);
  _tx_initialize_unused_memory = .;
}
