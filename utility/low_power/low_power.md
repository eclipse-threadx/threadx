---
title: Low Power Utility
author: sclarson
ms.author: sclarson
description: This article is a summary of the Low Power utility APIs available in ThreadX.
ms.date: 03/02/2021
ms.topic: article
ms.service: rtos
---

# Low Power APIs

## Summary of Low Power APIs

There are additional API functions available for low power, as follows:

- ***tx_low_power_enter*** - *Enter low power mode*
- ***tx_low_power_exit*** - *Exit low power mode*
- ***tx_time_increment*** - *Increment ThreadX timers by specific amount*
- ***tx_timer_get_next*** - *Get next ThreadX timer expiration*

---
## User-defined Macros

- **TX_LOW_POWER_TIMER_SETUP** - an optional macro to a user routine that sets up a low power clock. To set up a low power timer or operate ticklessly in low power mode, this must be defined. This is called in **tx_low_power_enter**.
- **TX_LOW_POWER_TICKLESS** - an optional define to operate ticklessly in low power mode. Symbol **TX_LOW_POWER_TIMER_SETUP** must also be defined. With this defined, there is no need to set up a low power timer to keep track of time. ThreadX will not maintain/update the internal tick count during or after exiting low power mode.
- **TX_LOW_POWER_USER_ENTER** - an optional macro to a user routine that configures the processor for entering low power mode (e.g. turn off peripherals and select a sleep mode). This is called in **tx_low_power_enter**.
- **TX_LOW_POWER_USER_EXIT** - an optional macro to a user routine that configures the processor for exiting low power mode (e.g. turn on peripherals). This is called in **tx_low_power_exit**.
- **TX_LOW_POWER_USER_TIMER_ADJUST** - an optional user macro to determine how much time has elapsed while in low power mode (in units of ThreadX ticks). This is called in **tx_low_power_exit** to get the number of ticks needed to adjust the ThreadX timers.

## tx_low_power_enter

Enter low power mode.

### Prototype

```c
VOID tx_low_power_enter(VOID);
```

### Description

This service enters low power mode.
For keeping track of time while in low power mode, there are two possibilities:

1. A ThreadX timer is active. Function **tx_timer_get_next** returns **TX_TRUE**. Note that in this situation, a low power clock must be used in order to wake up the CPU for the next ThreadX timer expiration. Therefore an alternative clock must be programmed. Program the hardware timer source such that the next timer interrupt is equal to: *tx_low_power_next_expiration \* tick_frequency*. The *tick_frequency* is application-specific and typically set up in **tx_low_level_initialize**.

2. There are no ThreadX timers active. Function **tx_timer_get_next** returns **TX_FALSE**.
    1. The application may choose not to keep the ThreadX internal
   tick count updated (define **TX_LOW_POWER_TICKLESS**), therefore there is no need
   to set up a low power clock.

    2. The application still needs to keep the ThreadX tick up-to-date. In this case
   a low power clock needs to be set up.

### Input parameters

- **none**

### Return values

- **none**

### Allowed from

Internal ThreadX code, application

### Example

ARM assembly
```c
#ifdef TX_LOW_POWER
    PUSH    {r0-r3}
    BL      tx_low_power_enter                      // Enter low power mode
    POP     {r0-r3}
#endif

#ifdef TX_ENABLE_WFI
    DSB                                             // Ensure no outstanding memory transactions
    WFI                                             // Wait for interrupt
    ISB                                             // Ensure pipeline is flushed
#endif

#ifdef TX_LOW_POWER
    PUSH    {r0-r3}
    BL      tx_low_power_exit                       // Exit low power mode
    POP     {r0-r3}
#endif
```
### See also

- tx_low_power_exit

---

## tx_low_power_exit

Exit low power mode.

### Prototype

```c
VOID tx_low_power_exit(VOID);

```

### Description

This service exits low power mode.

### Input parameters

- **none**

### Return values

- **none**

### Allowed from

Internal ThreadX code, application

### Example

```c
#ifdef TX_LOW_POWER
    PUSH    {r0-r3}
    BL      tx_low_power_enter                      // Enter low power mode
    POP     {r0-r3}
#endif

#ifdef TX_ENABLE_WFI
    DSB                                             // Ensure no outstanding memory transactions
    WFI                                             // Wait for interrupt
    ISB                                             // Ensure pipeline is flushed
#endif

#ifdef TX_LOW_POWER
    PUSH    {r0-r3}
    BL      tx_low_power_exit                       // Exit low power mode
    POP     {r0-r3}
#endif
```

### See also

- tx_low_power_enter

---

## tx_time_increment

This function increments the current time by a specified value. The value was derived by the application by calling the **tx_timer_get_next** function prior to this call, which was right before the processor was put in low power mode.

### Prototype

```c
VOID tx_time_increment(ULONG time_increment);
```

### Description

This function increments the current time by a specified value. The value was derived by the application by calling the **tx_timer_get_next** function prior to this call, which was right before the processor was put in low power mode.

### Input parameters

- **time_increment** Number of ThreadX ticks to increment time and timers.

### Return values

- **none**

### Allowed from

Internal ThreadX code, application

### Example

From **tx_low_power_exit**:

```c
        /* Call the low-power timer driver code to obtain the amount of time (in ticks)
        the system has been in low power mode. */
#ifdef TX_LOW_POWER_TIMER_ADJUST
        tx_low_power_adjust_ticks = TX_LOW_POWER_USER_TIMER_ADJUST;
#else
        tx_low_power_adjust_ticks = (ULONG) 0;
#endif

        /* Determine if the ThreadX timer needs incrementing.  */
        if (tx_low_power_adjust_ticks)
        {
            /* Yes, the ThreadX time must be incremented.  */
            tx_time_increment(tx_low_power_adjust_ticks);
        }
```

### See also

- tx_timer_get_next

---

## tx_timer_get_next

Get next ThreadX timer expiration

### Prototype

```c
ULONG tx_timer_get_next(ULONG *next_timer_tick_ptr);
```

### Description

This service gets the next ThreadX timer expiration, in ticks. 

### Input parameters

- **next_timer_tick_ptr** pointer to hold number of ticks

### Return values

- **TX_TRUE** (1) At least one timer is active.
- **TX_FALSE** (0) No timers are currently active.

### Allowed from

Internal ThreadX code, application

### Example

From **tx_low_power_enter**:

```c
ULONG   tx_low_power_next_expiration;   /* The next timer expiration (units of ThreadX timer ticks). */
ULONG   timers_active;

    /*  At this point, we want to enter low power mode, since nothing
        meaningful is going on in the system. However, in order to keep
        the ThreadX timer services accurate, we must first determine the
        next ThreadX timer expiration in terms of ticks. This is
        accomplished via the tx_timer_get_next API.  */
    timers_active =  tx_timer_get_next(&tx_low_power_next_expiration);
```

### See also

- tx_time_increment
