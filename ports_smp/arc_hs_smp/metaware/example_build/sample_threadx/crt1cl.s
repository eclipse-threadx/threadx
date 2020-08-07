; Copyright (c) 2000-2009 ARC International
	.file	"crt1cl.s"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Startup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.text
	.weak	_SDA_BASE_

	.macro jmpl,target
		jl	target		; "large" model call
	.endm

    	.on	assume_short, assume_nop_short
	.define S, _s

	; Indicate whether registers should be initialized with 0
	; This might be desirable if executing in an environment where
	; registers may contain undefined values on startup.
	.define PERFORM_REGISTER_INITIALIZATION, 0

	.section .text$crt00, text
	.global	_start
	.type	_start, @function
	.reloc __crt_callmain, 0
_start:

.if PERFORM_REGISTER_INITIALIZATION
	; Initialize the register file.
	; Compiled code may generate sub  r0, r1, r1, expecting to
	; load 0 into r0, but in some execution environments the individual
	; reads of r1 for the two source operands may return different
	; (garbage) values if r1 had never been written to since reset.

    .ifdef _ARC_RF16
	.irep num, 1, 2, 3, 10, 11, 12, 13, 14, 15
		mov	r\&num, 0
	.endr
    .else
	.irep num, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
		mov	r\&num, 0
	.endr
    .endif

.endif

	.weak __xcheck

.ifdef _PICTABLE
	; under PICTABLE, r0 is assumed to contain the base of the data segment
	; so must preserve it.  __xcheck modifies only r0 and r1 and is
	; position-independent
	mov	r2, r0
        mov.f	r0, __xcheck
	beq	.Linit_pic

	bl	__xcheck
	mov	r0, r2

    .Linit_pic:
	.weak	__PICTABLE__
	; In PIC mode, we assume that we were loaded by a loader and may not
	; reside at the addresses that the ELF file was linked with.  In
	; this case, the address of where the data segment has been loaded
	; is passed to _start in the r0 register.  We figure out the new code
	; segment base ourselves.
	mov	r2, __PICTABLE__
	sub.f	0, r2, 0
	nop		; required for -a4 -core5
	beq	no_pic
	bl	_init_pic

no_pic:
.endif

    .Lsetstack:
	; like smp_processor_id() from arc.h
    ; AUX_IDENTITY Reg [ 3  2  1  0 ]
    ;                  ^^^      => 0 for Master, !0 otherwise
    ; Note: this is setup for 2 cores and needs to be augmented if more than 2 cores
    lr   r0, [identity]
    lsr  r0, r0, 8
    bmsk r0, r0, 7
    .ifndef TX_ZERO_BASED_CORE_ID
    breq r0, 2, .Lsecondary
    .else
    breq r0, 1, .Lsecondary
    .endif
    mov sp, STACK1_END ; initialize stack pointer, core 1
	b .Lcontinue
.Lsecondary:
    mov sp, STACK2_END ; initialize stack pointer, core 2
.Lcontinue:
	mov	gp, _SDA_BASE_	; initialize small-data base register
	mov	fp, 0		; initialize frame pointer

__crt_start:
	; various components may get placed here by the linker:
	; priority    optional component                  command-line option
	;----------------------------------------------------------------------
	; 10          _init_ldi,_init_jli,_init_ei,       automated
	;             _init_sjli
	; 20          __crt_initcopy                      -Hcrt_initcopy
	; 25          __crt_initbss                       -Hcrt_initbss
	; 30          __crt_invcache                      -Hcrt_invcache
	; 70          __crt_inittimer                     -Hcrt_inittimer
	; 80          _init                               forced included below
	; 90          __crt_callmain                      forced, with
	;                                                   optional -Hcrt_argv

	.cfa_bf	_start

	.section .text$crt99, text
	.sectflag .text$crt99, include
; INIT CODE FALLS THROUGH TO _exit_halt
	.cfa_ef

	.global	_exit_halt
	.type	_exit_halt, @function
_exit_halt:
	.cfa_bf	_exit_halt

	flag	0x01
	nop
    .if ! $is_arcv2
	; ARC 700 serializes on 'flag', so no way to get back to here.
	nop
	nop
    .endif
	b	_exit_halt

	.reloc	main, 0; force main in if its in a library
	.cfa_ef

	.section .text$crt80, text
	.sectflag .text$crt80, include
__crt_init:
	bl	_init

	.previous
	; weak versions of small data symbols normally defined by the linker.
	.weak	_fsbss
	.weak	_esbss
	.set	_fsbss,0
	.set	_esbss,0

	; weak versions of BSS section symbols in case there is no .bss section
	.weak	_fbss
	.weak	_ebss
	.set	_fbss,0
	.set	_ebss,0

	; weak versions of heap section boundaries.  If a .heap section
	; is provided, our low-level allocator "sbrk" allocates within it.
	; If no .heap is provided, we allocate from _end to the end of memory.
	.weak	_fheap
	.weak	_eheap
	.set	_fheap,0
	.set	_eheap,0

	; reference the beginning of the stack for debugger's stack checking
	.weak	_fstack
	.set	_fstack,0

	.end

