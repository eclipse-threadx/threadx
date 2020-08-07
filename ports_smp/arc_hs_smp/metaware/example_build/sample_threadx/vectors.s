   
.file "vectors.s"
.section .ivt,text
;; This directive forces this section to stay resident even if stripped out by the -zpurgetext linker option
.sectflag .ivt,include

;// handler's name                  type      number  name                    offset in IVT (hex/dec)
.long   _start                  ;   exception   0   program entry point offset  0x0     0
.long   _tx_memory_error        ;   exception   1   memory_error        offset  0x4     4
.long   _tx_instruction_error   ;   exception   2   instruction_error   offset  0x8     8
.long   _tx_ev_machine_check    ;   exception   3   EV_MachineCheck     offset  0xC     12
.long   _tx_ev_tblmiss_inst     ;   exception   4   EV_TLBMissI         offset  0x10    16
.long   _tx_ev_tblmiss_data     ;   exception   5   EV_TLBMissD         offset  0x14    20
.long   _tx_ev_protection_viol  ;   exception   6   EV_ProtV            offset  0x18    24
.long   _tx_ev_privilege_viol   ;   exception   7   EV_PrivilegeV       offset  0x1C    28
.long   _tx_ev_software_int     ;   exception   8   EV_SWI              offset  0x20    32
.long   _tx_ev_trap             ;   exception   9   EV_Trap             offset  0x24    36
.long   _tx_ev_extension        ;   exception   10  EV_Extension        offset  0x28    40
.long   _tx_ev_divide_by_zero   ;   exception   11  EV_DivZero          offset  0x2C    44
.long   _tx_ev_dc_error         ;   exception   12  EV_DCError          offset  0x30    48
.long   _tx_ev_maligned         ;   exception   13  EV_Maligned         offset  0x34    52
.long   _tx_unsued_0            ;   exception   14  unused              offset  0x38    56
.long   _tx_unused_1            ;   exception   15  unused              offset  0x3C    60
.long   _tx_timer_0             ;   IRQ         16  Timer 0             offset  0x40    64
.long   _tx_timer_1             ;   IRQ         17  Timer 1             offset  0x44    68
.long   _tx_undefined_0         ;   IRQ         18                      offset  0x48    72
.long   _tx_smp_inter_core      ;   IRQ         19                      offset  0x4C    76
.long   _tx_undefined_2         ;   IRQ         20                      offset  0x50    80

