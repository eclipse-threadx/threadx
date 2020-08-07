;----------------------------------------------------------------
; Copyright (c) 2005-2018 Arm Limited (or its affiliates). All rights reserved.
; Use, modification and redistribution of this file is subject to your possession of a
; valid End User License Agreement for the Arm Product of which these examples are part of 
; and your compliance with all applicable terms and conditions of such licence agreement.
;
; Cortex-A SMP example - Startup Code
;----------------------------------------------------------------

        

        AREA  MP_SCU, CODE, READONLY

;----------------------------------------------------------------
; Misc
;----------------------------------------------------------------

        EXPORT getNumCPUs
        ; unsigned int getNumCPUs(void)
        ; Returns the number of CPUs in the Cluster
getNumCPUs PROC

        ; Get base address of private peripheral space
        MRC     p15, 4, r0, c15, c0, 0     ; Read periph base address
  
        LDR     r0, [r0, #0x004]           ; Read SCU Configuration register
        AND     r0, r0, #0x3               ; Bits 1:0 gives the number of cores-1
        ADD     r0, r0, #1
        BX      lr
        ENDP

;----------------------------------------------------------------
; SCU
;----------------------------------------------------------------

        ; SCU offset from base of private peripheral space --> 0x000

        EXPORT  enableSCU
        ; void enableSCU(void)
        ; Enables the SCU
enableSCU PROC

        MRC     p15, 4, r0, c15, c0, 0     ; Read periph base address

        LDR     r1, [r0, #0x0]             ; Read the SCU Control Register
        ORR     r1, r1, #0x1               ; Set bit 0 (The Enable bit)
        STR     r1, [r0, #0x0]             ; Write back modifed value

        BX      lr
        ENDP

; ------------------------------------------------------------

        EXPORT getCPUsInSMP
        ; unsigned int getCPUsInSMP(void)
        ; The return value is 1 bit per core:
        ; bit 0 - CPU 0
        ; bit 1 - CPU 1
        ; etc...
getCPUsInSMP PROC

        MRC     p15, 4, r0, c15, c0, 0     ; Read periph base address

        LDR     r0, [r0, #0x004]           ; Read SCU Configuration register
        MOV     r0, r0, LSR #4             ; Bits 7:4 gives the cores in SMP mode, shift then mask
        AND     r0, r0, #0x0F

        BX      lr
        ENDP

; ------------------------------------------------------------

        EXPORT enableMaintenanceBroadcast
        ; void enableMaintenanceBroadcast(void)
        ; Enable the broadcasting of cache & TLB maintenance operations
        ; When enabled AND in SMP, broadcast all "inner sharable"
        ; cache and TLM maintenance operations to other SMP cores
enableMaintenanceBroadcast PROC
        MRC     p15, 0, r0, c1, c0, 1      ; Read Aux Ctrl register
		MOV     r1, r0
        ORR     r0, r0, #0x01              ; Set the FW bit (bit 0)
		CMP     r0, r1
        MCRNE   p15, 0, r0, c1, c0, 1      ; Write Aux Ctrl register

        BX      lr
        ENDP

; ------------------------------------------------------------

        EXPORT disableMaintenanceBroadcast
        ; void disableMaintenanceBroadcast(void)
        ; Disable the broadcasting of cache & TLB maintenance operations
disableMaintenanceBroadcast PROC
        MRC     p15, 0, r0, c1, c0, 1      ; Read Aux Ctrl register
        BIC     r0, r0, #0x01              ; Clear the FW bit (bit 0)
        MCR     p15, 0, r0, c1, c0, 1      ; Write Aux Ctrl register

        BX      lr
        ENDP

; ------------------------------------------------------------

        EXPORT secureSCUInvalidate
        ; void secureSCUInvalidate(unsigned int cpu, unsigned int ways)
        ; cpu: 0x0=CPU 0 0x1=CPU 1 etc...
        ; This function invalidates the SCU copy of the tag rams
        ; for the specified core.  Typically only done at start-up.
        ; Possible flow:
        ; - Invalidate L1 caches
        ; - Invalidate SCU copy of TAG RAMs
        ; - Join SMP
secureSCUInvalidate PROC
        AND     r0, r0, #0x03              ; Mask off unused bits of CPU ID
        MOV     r0, r0, LSL #2             ; Convert into bit offset (four bits per core)
  
        AND     r1, r1, #0x0F              ; Mask off unused bits of ways
        MOV     r1, r1, LSL r0             ; Shift ways into the correct CPU field

        MRC     p15, 4, r2, c15, c0, 0     ; Read periph base address

        STR     r1, [r2, #0x0C]            ; Write to SCU Invalidate All in Secure State

        BX      lr

        ENDP


  END

;----------------------------------------------------------------
; End of MP_SCU.s
;----------------------------------------------------------------
