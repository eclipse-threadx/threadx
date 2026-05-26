# Eclipse ThreadX Coding Conventions
_Version: 2026.01-DRAFT_

All ThreadX software conforms to a strict set of coding conventions. 

## 1. C coding conventions
Versions of ThreadX 6.0 and greater follow the MISRA-C:2004 and MISRA C:2012 coding conventions. 

### 1.1. ThreadX File Names
All ThreadX C file names take the form `tx_x.c` where `x` represents the component name (e.g., `tx_thread_create.c` contains the function `_tx_thread_create`). Component specification file names take the form `tx_x.h` (e.g., `tx_thread.h`). These naming conventions distinguish ThreadX files from all other application source files.

### 1.2. ThreadX Name Space
All ThreadX functions and global data have a leading `_tx` in their name. This keeps ThreadX global symbols separate from the application symbols and in one contiguous area of the load map created by the linker.

### 1.3. ThreadX Constant Names
All ThreadX constants have the form `TX_NAME` or `TX_C_NAME` and are comprised of capital letters, numerals, and underscores. System constants defined in `tx_api.h` or `tx_port.h` take the form `TX_NAME` (e.g., `TX_SUCCESS`). Component constants defined in component specification files take the form `TX_x_NAME` where `x` represents the capitalized entire component name (e.g., `TX_INITIALIZE_IN_PROGRESS`).

In header files, related `#define` constants are column-padded so that all values begin at the same column, making groups of definitions easier to scan:

```c
#define TX_THREAD_ID                            ((ULONG) 0x54485244)
#define TX_THREAD_PRIORITY_GROUP_MASK           ((ULONG) 0xFF)
#define TX_THREAD_EXECUTE_LOG_SIZE              ((UINT) 8)
```

### 1.4. ThreadX Struct and Typedef Names
ThreadX C structure and typedef names are similar to component-specific constant names. System-wide typedefs have the form `TX_X_NAME`, where `x` is the capitalized component name (e.g., `TX_QUEUE`). Component-specific typedefs are contained in `tx_api.h`. Primitive data types like `UINT`, `ULONG`, and `VOID` do not require the leading `TX` modifier and are defined in `tx_port.h`.

#### 1.4.1 ThreadX Member Names
Structure member names are all lower case and take the form `tx_x_name` where `x` is the entire component name (e.g., `tx_thread_id` in the `TX_THREAD` structure).

### 1.5. ThreadX Global Data Names
Each component has a small amount of global C data elements. These elements are lower-case and have the form `_tx_x_name` (e.g., `_tx_thread_current_ptr`).

### 1.6. ThreadX Local Data Names
Local variable names are lower-case and follow the same `x_name` style as other names in the component (e.g., `next_thread`, `previous_mutex`).

Declarations are written one variable per line, with no inline initialisation. Type names and variable names are column-aligned within a declaration block so that all variable names begin at the same column:

```c
TX_MUTEX        *next_mutex;
TX_MUTEX        *previous_mutex;
UINT            status;
```

Frequently used elements are typically assigned the `register` modifier if supported by the target compiler.

### 1.7. ThreadX Function Names
All component function names have the form `_tx_x_name` in lower-case, where `x` represents the entire component name (e.g., `_tx_thread_create`).

### 1.8. Source Code Layout

**Indentation.** The standard indentation increment is four spaces. Tab characters are avoided to make the source code less sensitive to text editors.

**Blank lines.** A blank line separates each logical block of code. A blank line also follows the opening brace of a function body and precedes the closing brace.

**Assignment column alignment.** Within a block of related assignments, values are padded with spaces so that the assigned values all begin at the same column:

```c
mutex_ptr -> tx_mutex_name =             name_ptr;
mutex_ptr -> tx_mutex_inherit =          inherit;
mutex_ptr -> tx_mutex_created_next =     mutex_ptr;
```

**Two-space rule.** All assignments use two spaces before the `=` operator:

```c
status =  TX_SUCCESS;
```

**Return statements.** Return values are always written with parentheses:

```c
return(TX_SUCCESS);
```

**Multi-line function signatures.** When a function signature does not fit on one line, continuation lines are indented to align with the first parameter on the opening line:

```c
UINT  _tx_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, VOID (*entry_function)(ULONG id), ULONG entry_input,
                            VOID *stack_start, ULONG stack_size, UINT priority, UINT preempt_threshold,
                            ULONG time_slice, UINT auto_start)
```

### 1.9. Comments

Each non-trivial C statement has a meaningful inline comment. Comments start with a capital letter and end with a period. Two spaces precede the closing `*/`:

```c
/* Default status to success.  */
/* Disable interrupts to place the mutex on the created list.  */
```

Every function is preceded by a structured header comment block. The sections, in order, are: **FUNCTION**, **RELEASE**, **AUTHOR**, **DESCRIPTION**, **INPUT**, **OUTPUT**, **CALLS**, and **CALLED BY**. The **RELEASE** field identifies the ThreadX release in which this code block was introduced. Header files use the same block but with **COMPONENT DEFINITION** in place of **FUNCTION** and **RELEASE**.

```c
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_mutex_create                                    PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Bruce TheShark, Eclipse Foundation                                  */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a mutex with optional priority inheritance as */
/*    specified in this call.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    mutex_ptr                             Pointer to mutex control block*/
/*    name_ptr                              Pointer to mutex name         */
/*    inherit                               Priority inheritance option   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_SUCCESS                        Successful completion status      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
```


### 1.10. MISRA Deviation Policy

All C source code complies with MISRA C:2004 and MISRA C:2012. Where a deviation is necessary it must be justified with a comment referencing the specific rule number, placed immediately adjacent to the non-conforming code:

```c
/* MISRA C:2012 Rule 11.5 deviation: required for VOID* to typed-pointer conversion.  */
```

Pointer type conversions must use the `TX_*_CONVERT` macros defined in `tx_api.h` rather than direct C casts. No deviation is permitted without an explicit justification comment.


### 1.11. Critical Sections

Interrupt locking is managed through three macros:

- `TX_INTERRUPT_SAVE_AREA` declares the storage needed to save the interrupt state.
- `TX_DISABLE` disables interrupts and saves the current state.
- `TX_RESTORE` restores the previously saved interrupt state.

`TX_INTERRUPT_SAVE_AREA` must always be the very first item inside a function body, before any variable declarations. All three macros are written as bare identifiers — no parentheses and no terminating semicolon:

```c
UINT  _tx_thread_delete(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD       *next_thread;
UINT            status;

    /* Lockout interrupts.  */
    TX_DISABLE

    /* ... critical section code ... */

    /* Restore interrupts.  */
    TX_RESTORE
}
```


### 1.12. Conditional Compilation

Optional features are guarded with `#ifdef TX_<FEATURE_NAME>` / `#endif`. Features that are active by default and can be disabled use `#ifndef TX_DISABLE_<FEATURE_NAME>`. The `#endif` line is never annotated with a comment repeating the condition name:

```c
#ifdef TX_ENABLE_STACK_CHECKING
ALIGN_TYPE              new_stack_start;
ALIGN_TYPE              updated_stack_start;
#endif

#ifndef TX_DISABLE_STACK_FILLING
    TX_MEMSET(stack_start, ((UCHAR) TX_STACK_FILL), stack_size);
#endif
```

Conditional `#include` directives follow the same pattern:

```c
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif
```


### 1.13. Error Handling

Error conditions are evaluated immediately after `TX_DISABLE`. On detecting an error, `TX_RESTORE` is called and a status variable is set before leaving the check block. The entire normal success path is then wrapped in a single `if (status == TX_SUCCESS)` block. This keeps error paths shallow and avoids nesting the success path inside a chain of guards:

```c
    /* Default status to success.  */
    status =  TX_SUCCESS;

    /* Lockout interrupts.  */
    TX_DISABLE

    /* Check for valid state.  */
    if (thread_ptr -> tx_thread_state != TX_COMPLETED)
    {
        if (thread_ptr -> tx_thread_state != TX_TERMINATED)
        {

            /* Restore interrupts.  */
            TX_RESTORE

            /* Thread is not in a deletable state.  */
            status =  TX_DELETE_ERROR;
        }
    }

    /* Proceed only if no error was detected.  */
    if (status == TX_SUCCESS)
    {

        /* ... normal path code ... */

        /* Restore interrupts.  */
        TX_RESTORE
    }

    return(status);
```


### 1.14. Component Protection Macros

Components that require serialisation define paired `XX_PROTECT` / `XX_UNPROTECT` macros that abstract the component's chosen strategy (typically a ThreadX mutex). The opening macro is called once near the top of a function, after tracing. The closing macro is called at **every** exit point — each error return as well as the normal return. It is never placed only at the end:

```c
    FX_PROTECT

    /* Check for write-protect.  */
    if (media_ptr -> fx_media_write_protect)
    {

        FX_UNPROTECT
        return(FX_WRITE_PROTECT);
    }

    /* ... normal path work ... */

    FX_UNPROTECT
    return(FX_SUCCESS);
```


### 1.15. Two-Level Protection (Mutex + Interrupt Disable)

When an operation must be serialised between threads *and* protected from ISRs, two layers of protection are applied in sequence: the component mutex is acquired first, then interrupts are disabled within that scope. They are released in reverse order:

```c
    tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);

    /* Disable interrupts.  */
    TX_DISABLE

    /* ... critical work ... */

    /* Restore interrupts.  */
    TX_RESTORE

    tx_mutex_put(&(ip_ptr -> nx_ip_protection));
```


### 1.16. Control Block Initialisation

Control blocks are zeroed with `memset` using an explicit `(VOID *)` cast and `sizeof(TYPE)` — never a byte-count literal. This ensures the zero-initialisation is type-safe and self-maintaining as the structure grows:

```c
    TX_MEMSET(mutex_ptr, 0, (sizeof(TX_MUTEX)));
```

After full initialisation, the control block's `_id` field is set to the type's magic-constant identifier, which serves as the validity token for subsequent calls:

```c
    mutex_ptr -> tx_mutex_id =  TX_MUTEX_ID;
```

Conversely, the `_id` field is cleared to `TX_CLEAR_ID` when the object is deleted.


### 1.17. Callback Invocation

Stored function pointer callbacks are always checked against the component null constant before being called. The check and call occupy a simple `if` block with no `else`. The null constant is `TX_NULL`, `NX_NULL`, `UX_NULL`, `FX_NULL`, or `LX_NULL` depending on the component:

```c
    if (thread_ptr -> tx_thread_entry_exit_notify != TX_NULL)
    {
        (thread_ptr -> tx_thread_entry_exit_notify)(thread_ptr, TX_THREAD_ENTRY);
    }
```


### 1.18. Driver Invocation via Control-Block Function Pointer

Hardware and I/O drivers are invoked by dereferencing a single function pointer stored in the component control block. All request parameters and the returned status are communicated through other fields in the same control block — there are no additional arguments on the call itself:

```c
    /* Setup the driver request.  */
    media_ptr -> fx_media_driver_request =        FX_DRIVER_READ;
    media_ptr -> fx_media_driver_logical_sector =  logical_sector;
    media_ptr -> fx_media_driver_buffer =          cache_entry -> fx_cached_sector_memory_buffer;

    /* Invoke the driver.  */
    (media_ptr -> fx_media_driver_entry)(media_ptr);

    /* Check driver status.  */
    if (media_ptr -> fx_media_driver_status != FX_SUCCESS)
```


### 1.19. Error-Checking Wrapper Functions

Where a public API performs significant parameter validation, the validation is separated into a companion function whose name carries an extra `e` after the component prefix (e.g., `_uxe_`, `_nxe_`). The wrapper validates inputs and returns early on error; it then delegates to the core implementation which carries no defensive checks:

```c
UINT  _uxe_host_stack_device_get(ULONG device_index, UX_DEVICE **device)
{

    /* Check for invalid pointer.  */
    if (device == UX_NULL)
        return(UX_INVALID_PARAMETER);

    return(_ux_host_stack_device_get(device_index, device));
}
```


### 1.20. Unused Parameter Suppression

When a function parameter is intentionally unused due to conditional compilation, it is marked with the component's `XX_PARAMETER_NOT_USED(p)` macro. This suppresses compiler warnings without requiring a cast to `VOID` and makes the intent explicit:

```c
#else
    NX_PARAMETER_NOT_USED(socket_ptr);
    NX_PARAMETER_NOT_USED(packet_ptr);
    return(NX_NOT_SUPPORTED);
#endif
```


### 1.21. Event Tracing

Every non-trivial public function inserts a trace event using the component's `XX_TRACE_IN_LINE_INSERT` macro. The macro takes eight arguments: event identifier, up to four data values, an event-category filter, and two optional output pointers for updatable trace entries. When the trace entry does not need post-operation updating, the last two arguments are `0, 0`. The call is placed immediately after protection is acquired, before any work begins:

```c
    FX_TRACE_IN_LINE_INSERT(FX_TRACE_FILE_WRITE, file_ptr, buffer_ptr, size, 0,
                            FX_TRACE_FILE_EVENTS, &trace_event, &trace_timestamp)
```

When the trace entry carries values that are only known after the work completes (e.g., a byte count), the same event is updated at the end of the function using `XX_TRACE_EVENT_UPDATE`.


### 1.22. Diagnostic Counters

Component control blocks include per-operation diagnostic counters (e.g., `_read_requests`, `_write_requests`, `_sector_not_found`). They are incremented unconditionally at the start of the relevant operation, making them always available for debugging. When a component provides many counters, the entire statistics block can be compiled out with a single `#ifndef XX_STATISTICS_DISABLE` guard:

```c
    nor_flash -> lx_nor_flash_read_requests++;
```


### 1.23. Bit-Clear Cast Convention

When clearing a bit in a flag word, the bit-mask constant is always cast explicitly to the unsigned type of the target variable, wrapped in its own parentheses, even when the constant is already unsigned. This satisfies MISRA rules on signed/unsigned mixing in bitwise operations:

```c
    old_mapping_entry =  old_mapping_entry & ~((ULONG) LX_NOR_PHYSICAL_SECTOR_SUPERCEDED);
```


## 2. Assembly coding conventions

Assembly source files provide port-specific implementations of the ThreadX scheduler, context save/restore, and interrupt management routines. Assembly conventions closely mirror the C conventions in §1.

### 2.1. File naming and directory layout

Assembly source files come in two forms matched to the supported toolchains:

- **GNU/GCC**: `tx_x.S` — uppercase `.S` extension. The build system passes these through the C preprocessor before assembling.
- **IAR**: `tx_x.s` — lowercase `.s` extension.

The filename stem always matches the function it contains (e.g., `tx_thread_schedule.S` contains `_tx_thread_schedule`). GNU and IAR files live in separate sibling directories: `gnu/src/` and `iar/src/` respectively. Toolchain-specific syntax must never be mixed in a single file.

### 2.2. File structure

Every assembly file follows this top-to-bottom order:

1. Copyright and license header (same `/* */` format as C — see §3.1)
2. Component description block (see §3)
3. `#ifdef TX_INCLUDE_USER_DEFINE_FILE` / `#include "tx_user.h"` / `#endif`
4. External symbol declarations (IAR: `EXTERN` lines; GNU: not required — see §2.8)
5. Section and mode directives (IAR: `SECTION` and `THUMB`/`CODE32`; GNU: after the comment block — see §2.7)
6. Function header comment block (see §2.3)
7. C-pseudocode signature comment (see §2.4)
8. Section, alignment, and symbol directives (GNU — see §2.7)
9. Function label
10. Function body
11. Closing pseudocode comment `// }`
12. `.end` (GNU) or `END` (IAR) as the very last line

### 2.3. Function header comment block

Assembly functions use the exact same structured comment block as C functions (see §1.9), with the same sections in the same order. The **RELEASE** field carries the ThreadX release number and the target architecture/toolchain pair on the same line:

```asm
/*    _tx_thread_schedule                               Cortex-M33/GNU    */
/*                                                           6.2.1        */
```

### 2.4. C-pseudocode signature comment

Immediately before the section/symbol directives, a `//`-prefixed C-style comment shows the function's equivalent C signature followed by an opening brace. A matching closing brace comment follows the last instruction:

```asm
// VOID   _tx_thread_schedule(VOID)
// {
    .section .text
    ...
_tx_thread_schedule:
    ...
    BX      lr
// }
    .end
```

For functions with parameters or a return value, the full signature is shown:

```asm
// UINT   _tx_thread_interrupt_control(UINT new_posture)
// {
```

### 2.5. Label naming

**Global labels** (exported function entry points) follow the same `_tx_x_name` convention as C functions — all lower-case with underscores:

```asm
_tx_thread_schedule:
_tx_thread_context_save:
_tx_thread_interrupt_control:
```

Exception: labels that implement standard ARM/CMSIS interrupt vectors use the conventional handler name (e.g., `PendSV_Handler`, `SVC_Handler`). When a ThreadX routine aliases a standard vector, both labels may mark the same entry point.

**Local and branch labels** use a double-underscore prefix followed by a descriptive name that reflects the control-flow state. Names are entirely lower-case with underscores:

```asm
__tx_ts_handler:
__tx_ts_new:
__tx_wait_here:
_skip_vfp_save:
```

All labels appear at column 0 with no indentation.

### 2.6. Instruction layout and comment alignment

Instructions are indented by one tab stop from column 0. Inline `//` comments begin at a consistent column within a file (typically around column 44). Preprocessor directives (`#ifdef`, `#else`, `#endif`) appear at column 0:

```asm
    MOV     r0, #0                                  // Build value for TX_FALSE
    LDR     r2, =_tx_thread_preempt_disable         // Build address of preempt disable flag
    STR     r0, [r2, #0]                            // Clear preempt disable flag
```

Logical groupings of instructions are preceded by a `/* */` block comment explaining their purpose, matching the inline-comment style from §1.9:

```asm
    /* Recover PSP and preserve current thread context.  */

    STR     r3, [r0]                                // Set _tx_thread_current_ptr to NULL
    MRS     r12, PSP                                // Pickup PSP pointer (thread's stack pointer)
    STMDB   r12!, {r4-r11}                          // Save its remaining registers
```

Every non-trivial instruction must carry an inline comment explaining what it does at the logical level, not merely restating the mnemonic.

### 2.7. Section and symbol directives

These directives are toolchain-specific and must appear in the order shown.

**GNU (`.S`):**

```asm
    .section .text
    .balign 4
    .syntax unified
    .eabi_attribute Tag_ABI_align_preserved, 1
    .global  _tx_thread_schedule
    .thumb_func
.type _tx_thread_schedule, function
_tx_thread_schedule:
```

- `.eabi_attribute Tag_ABI_align_preserved, 1` is required on all Cortex-M GNU files.
- `.thumb_func` must immediately precede the label on Thumb targets.
- `.type` appears at column 0 (no indentation), unlike the other directives.

**IAR (`.s`):**

```asm
    SECTION `.text`:CODE:NOROOT(2)
    THUMB
    PUBLIC  _tx_thread_schedule
_tx_thread_schedule:
```

Replace `THUMB` with `CODE32` for 32-bit ARM mode.

### 2.8. External symbol declarations

**GNU:** External symbols are resolved by the linker and referenced via the load-address pseudo-op. No explicit declaration is required:

```asm
    LDR     r0, =_tx_thread_current_ptr             // Build current thread pointer address
```

**IAR:** All external symbols must be declared with `EXTERN` before the `SECTION` directive, one symbol per line. Symbols only used within a conditional block are guarded with the matching `#ifdef`:

```asm
    EXTERN  _tx_thread_current_ptr
    EXTERN  _tx_thread_execute_ptr
    EXTERN  _tx_timer_time_slice
#ifdef TX_LOW_POWER
    EXTERN  tx_low_power_enter
    EXTERN  tx_low_power_exit
#endif
    SECTION `.text`:CODE:NOROOT(2)
```

### 2.9. Conditional assembly

Both toolchains pass assembly files through the C preprocessor, so C-style `#ifdef` / `#ifndef` / `#else` / `#endif` is used for all conditional assembly. Feature macros follow the same `TX_ENABLE_*` / `TX_DISABLE_*` / `TX_PORT_*` naming as the C source. Architecture-detection macros (e.g., `__ARM_FP`) are also used directly:

```asm
#ifdef __ARM_FP
    /* Clear CONTROL.FPCA bit so VFP registers aren't unnecessarily stacked.  */
    MRS     r0, CONTROL                             // Pickup current CONTROL register
    BIC     r0, r0, #4                              // Clear the FPCA bit
    MSR     CONTROL, r0                             // Setup new CONTROL register
#endif
```

For longer conditional blocks, the `#endif` line carries a comment identifying the condition being closed:

```asm
#ifdef TX_PORT_USE_BASEPRI
    LDR     r1, =TX_PORT_BASEPRI                    // Mask interrupt priorities
    MSR     BASEPRI, r1
#else
    CPSID   i                                       // Disable interrupts
#endif  /* TX_PORT_USE_BASEPRI */
```

### 2.10. Function return

Use the architecture-correct explicit return form. Do not use assembler pseudo-instructions where the explicit form is available:

- **Thumb / Thumb-2**: `BX lr`
- **ARM32**: `MOV pc, lr` (leaf function) or restore `pc` from the stack via `LDMFD`

### 2.11. File-end directive

The last line of every assembly file is `.end` (GNU) or `END` (IAR), with no trailing blank lines:

```asm
    BX      lr
// }
    .end
```


## 3. File headers
ThreadX file headers are made of two distinct sections.

The first section states the copyright owners of the code found in the file, and states the applicable license. The MIT license applies to the vast majority of the files making up ThreadX. It is mandatory to include the applicable SPDX license identifier.

The second section describes the ThreadX component the file belongs to.

### 3.1. Source file headers (`.c`)

Immediately after the file header, every `.c` implementation file defines `TX_SOURCE_CODE` before any `#include` directives. This allows internal headers to expose declarations that are not part of the public API:

```c
#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"
```

Below is a sample `.c` file header.

```c
/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX Contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Block Pool                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
```

### 3.2. Header file headers (`.h`)

Header files use the same two-section file header as source files. The component description block is followed by a `COMPONENT DEFINITION` comment block (the header-file equivalent of the function block described in §1.9), and then the include guard:

```c
/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX Contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Mutex                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    tx_mutex.h                                          PORTABLE C      */
/*                                                           6.2          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Bruce TheShark, Eclipse Foundation                                  */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the ThreadX mutex control component, including    */
/*    data types and external references.                                 */
/*                                                                        */
/**************************************************************************/

#ifndef TX_MUTEX_H
#define TX_MUTEX_H

/* Component definitions...  */

#endif
```

The include guard name is always `TX_<FILENAME>_H` in all capitals, matching the file name with the `.` replaced by `_`.




