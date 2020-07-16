// ------------------------------------------------------------
// Exceptions workbook exercise
//
// Copyright ARM LTD, 2012
// ------------------------------------------------------------

	.section  VECTORS,"ax"
	.align 12
  
 
	.global el3_vectors
el3_vectors:

	.global fiqHandler
	.global irqHandler

// ------------------------------------------------------------
// Current EL with SP0
// ------------------------------------------------------------
	.balign 128
sync_current_el_sp0:
  B        .                    //        Synchronous

	.balign 128
irq_current_el_sp0:
  B        irqFirstLevelHandler //        IRQ

	.balign 128
fiq_current_el_sp0:
  B        fiqFirstLevelHandler //        FIQ

	.balign 128
serror_current_el_sp0:
  B        .                    //        SError

// ------------------------------------------------------------
// Current EL with SPx
// ------------------------------------------------------------

	.balign 128
sync_current_el_spx:
  B        .                    //        Synchronous

	.balign 128
irq_current_el_spx:
  B        irqFirstLevelHandler //        IRQ

	.balign 128
fiq_current_el_spx:
  B        fiqFirstLevelHandler //        FIQ

	.balign 128
serror_current_el_spx:
  B        .                    //        SError

// ------------------------------------------------------------
// Lower EL using AArch64
// ------------------------------------------------------------

	.balign 128
sync_lower_el_aarch64:
   B        .                    

	.balign 128
irq_lower_el_aarch64:
  B        irqFirstLevelHandler //        IRQ

	.balign 128
fiq_lower_el_aarch64:
  B        fiqFirstLevelHandler //        FIQ

	.balign 128
serror_lower_el_aarch64:
  B        .                    //        SError

// ------------------------------------------------------------
// Lower EL using AArch32
// ------------------------------------------------------------

	.balign 128
sync_lower_el_aarch32:
   B        .

	.balign 128
irq_lower_el_aarch32:
  B        irqFirstLevelHandler //        IRQ

	.balign 128
fiq_lower_el_aarch32:
  B        fiqFirstLevelHandler //        FIQ

	.balign 128
serror_lower_el_aarch32:
  B        .                    //        SError


// ------------------------------------------------------------

irqFirstLevelHandler:
  STP      x29, x30, [sp, #-16]!
  BL       _tx_thread_context_save

  BL       irqHandler

  B        _tx_thread_context_restore
  
fiqFirstLevelHandler:
  STP      x29, x30, [sp, #-16]!
  STP      x18, x19, [sp, #-16]!
  STP      x16, x17, [sp, #-16]!
  STP      x14, x15, [sp, #-16]!
  STP      x12, x13, [sp, #-16]!
  STP      x10, x11, [sp, #-16]!
  STP      x8, x9, [sp, #-16]!
  STP      x6, x7, [sp, #-16]!
  STP      x4, x5, [sp, #-16]!
  STP      x2, x3, [sp, #-16]!
  STP      x0, x1, [sp, #-16]!

  BL       fiqHandler

  LDP      x0, x1, [sp], #16
  LDP      x2, x3, [sp], #16
  LDP      x4, x5, [sp], #16
  LDP      x6, x7, [sp], #16
  LDP      x8, x9, [sp], #16
  LDP      x10, x11, [sp], #16
  LDP      x12, x13, [sp], #16
  LDP      x14, x15, [sp], #16
  LDP      x16, x17, [sp], #16
  LDP      x18, x19, [sp], #16
  LDP      x29, x30, [sp], #16
  ERET

// ------------------------------------------------------------
// End of file
// ------------------------------------------------------------

