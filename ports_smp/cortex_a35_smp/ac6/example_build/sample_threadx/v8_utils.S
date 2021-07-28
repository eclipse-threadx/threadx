//
// Simple utility routines for baremetal v8 code
//
// Copyright (c) 2013-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your
// possession of a valid DS-5 end user licence agreement and your compliance
// with all applicable terms and conditions of such licence agreement.
//

#include "v8_system.h"

    .text
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame

//
// void *ZeroBlock(void *blockPtr, unsigned int nBytes)
//
// Zero fill a block of memory
//   Fill memory pages or similar structures with zeros.
//   The byte count must be a multiple of the block fill size (16 bytes)
//
//  Inputs:
//    blockPtr - base address of block to fill
//    nBytes - block size, in bytes
//
//  Returns:
//    pointer to just filled block, NULL if nBytes is
//  incompatible with block fill size
//
    .global ZeroBlock
    .type ZeroBlock, "function"
    .cfi_startproc
ZeroBlock:

    //
    // we fill data by steam, 16 bytes at a time: check that
    // blocksize is a multiple of that
    //
    ubfx x2, x1, #0, #4
    cbnz x2, incompatible

    //
    // we already have one register full of zeros, get another
    //
    mov x3, x2

    //
    // OK, set temporary pointer and away we go
    //
    add x0, x0, x1

loop0:
    subs x1, x1, #16
    stp  x2, x3, [x0, #-16]!
    b.ne loop0

    //
    // that's all - x0 will be back to its start value
    //
    ret

    //
    // parameters are incompatible with block size - return
    // an indication that this is so
    //
incompatible:
    mov x0,#0
    ret
    .cfi_endproc
