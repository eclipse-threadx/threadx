
    .text
    .align 4

    .global _gcc_setup
_gcc_setup:

	sub 	sp, sp, 16		// Store other preserved registers here
	str 	x30, [sp]

    ldr     x3, =__FLASH_segment_start__
    ldr     x4, =__RAM_segment_start__
    mov     x5,x0

// This is for modules linked to absolute addresses
// On PIC modules x9 (or r9) will hold the data base address
    ldr     x9, =__RAM_segment_start__

    /* Copy GOT table. */
  
    ldr     x0, =__got_load_start__
    sub     x0 ,x0, x3
    add     x0, x0, x5
    ldr     x1, =__new_got_start__
    sub     x1, x1, x4
    add     x1, x1, x9
    ldr     x2, =__new_got_end__
    sub     x2, x2, x4
    add     x2, x2, x9

new_got_setup:
    cmp     x1, x2          // See if there are more GOT entries
    beq     got_setup_done  // No, done with GOT setup
    ldr     x6, [x0]        // Pickup current GOT entry
    cmp     x6, #0          // Is it 0?
    beq     address_built   // Yes, just skip the adjustment
    cmp     x6, x4          // Is it in the code or data area?
    blt     flash_area      // If less than, it is a code address
    sub     x6, x6, x4      // Compute offset of data area
    add     x6, x6, x9      // Build address based on the loaded data address
    b       address_built   // Finished building address
flash_area:
    sub     x6, x6, x3      // Compute offset of code area
    add     x6, x6, x5      // Build address based on the loaded code address
address_built:
    str     x6, [x1]        // Store in new GOT table
    add     x0, x0, #4      // Move to next entry
    add     x1, x1, #4      //
    b       new_got_setup   // Continue at the top of the loop
got_setup_done:


    /* Copy initialised sections into RAM if required. */
  
    ldr     x0, =__data_load_start__
    sub     x0, x0, x3
    add     x0, x0, x5
    ldr     x1, =__data_start__
    sub     x1, x1, x4
    add     x1, x1, x9
    ldr     x2, =__data_end__
    sub     x2, x2, x4
    add     x2, x2, x9
    bl      crt0_memory_copy
  
    /* Zero bss. */
    
    ldr     x0, =__bss_start__
    sub     x0, x0, x4
    add     x0, x0, x9
    ldr     x1, =__bss_end__
    sub     x1, x1, x4
    add     x1, x1, x9
    mov     x2, #0
    bl      crt0_memory_set


    /* Setup heap - not recommended for Threadx but here for compatibility reasons */

    ldr     x0, =__heap_start__
    sub     x0, x0, x4
    add     x0, x0, x9
    ldr     x1, =__heap_end__
    sub     x1, x1, x4
    add     x1, x1, x9
    sub     x1, x1, x0
    mov     x2, #0
    str     x2, [x0]
    add     x0, x0, #4
    str     x1, [x0]
    
	ldr 	x30, [sp]		// Restore other preserved registers
	add 	sp, sp, 16

    ret	                    // Return to caller
  

  /* Startup helper functions. */

    .align 4
crt0_memory_copy:
    cmp     x0, x1
    beq     memory_copy_done
    cmp     x2, x1
    beq     memory_copy_done
    sub     x2, x2, x1
memory_copy_loop:
    ldrb    w3, [x0]
    add     x0, x0, #1
    strb    w3, [x1]
    add     x1, x1, #1
    sub     x2, x2, #1
    cmp     x2, #0
    bne     memory_copy_loop
memory_copy_done:
    ret

crt0_memory_set:
    cmp     x0, x1
    beq     memory_set_done
    str     w2, [x0]
    add     x0, x0, #1
    b       crt0_memory_set
memory_set_done:
    ret

    /* Setup attibutes of heap section so it doesn't take up room in the elf file */
    .section .heap, "wa", %nobits
  
