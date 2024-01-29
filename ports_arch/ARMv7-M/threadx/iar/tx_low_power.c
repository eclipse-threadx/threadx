/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
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
/**   Low Power Timer Management                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_low_power.h"


/* Define low power global variables.  */

/* Flag to determine if we've entered low power mode or not. */
UINT    tx_low_power_entered;


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_low_power_enter                                  PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the low power entry function. This function is     */
/*    assumed to be called from the idle loop of tx_thread_schedule. It   */
/*    is important to note that if an interrupt managed by ThreadX occurs */
/*    anywhere where interrupts are enabled in this function, the entire  */
/*    processing of this function is discarded and the function won't be  */
/*    re-entered until the idle loop in tx_thread_schedule is executed    */
/*    again.                                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_timer_get_next                     Get next timer expiration     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_schedule                   Thread scheduling loop        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
VOID  tx_low_power_enter(VOID)
{

TX_INTERRUPT_SAVE_AREA
ULONG   tx_low_power_next_expiration;
ULONG   any_expired;


    /*  The below macro is user-defined code to determine
        if low power mode is beneficial for the application.
        Reasons for not entering low power mode include 
        the overhead associated with entering and exiting low power mode 
        outweighs the savings given when the next interrupt is expected. 
        In addition, the application might also be in a state where 
        responsiveness is more important than power savings. In such 
        situations, using a "reduced power mode" might make more sense. 
        In any case, if low power mode is not desired, simply return at
        this point in the code.  */
    #ifdef TX_LOW_POWER_USER_CHECK
    TX_LOW_POWER_USER_CHECK;
    #endif

    /* Disable interrupts while we prepare for low power mode.  */
    TX_DISABLE
    
    /*  At this point, we want to enter low power mode, since nothing
        meaningful is going on in the system. However, in order to keep 
        the ThreadX timer services accurate, we must first determine the 
        next ThreadX timer expiration in terms of ticks. This is 
        accomplished via the tx_timer_get_next API.  */
    any_expired =  tx_timer_get_next(&tx_low_power_next_expiration);
    
    /* There are two possibilities:
        1:  A ThreadX timer is active. tx_timer_get_next returns TX_TRUE.
            Program the hardware timer source such that the next timer
            interrupt is equal to: tx_low_power_next_expiration*tick_frequency. 
            In most applications, the tick_frequency is 10ms, but this is
            completely application specific in ThreadX, typically set up 
            in tx_low_level_initialize.
        2:  There are no ThreadX timers active. tx_timer_get_next returns TX_FALSE.
            If you don't care about maintaining the ThreadX system clock, you can simply 
            sleep forever (until an interrupt wakes you up). 
            If you do want to maintain the ThreadX system clock, 
            program the hardware timer so you can keep track of elapsed time.  */
    #ifdef TX_LOW_POWER_USER_TIMER_SETUP
    TX_LOW_POWER_USER_TIMER_SETUP(any_expired, tx_low_power_next_expiration);
    #endif
    

    /* Set the flag indicating that low power has been entered. This 
       flag is checked in tx_low_power_exit to determine if the logic
       used to adjust the ThreadX time is required.  */
    tx_low_power_entered =  TX_TRUE;
    
    /* Re-enable interrupts before low power mode is entered.  */
    TX_RESTORE
    
    /* User code to enter low power mode.  */
    #ifdef TX_LOW_POWER_USER_ENTER
    TX_LOW_POWER_USER_ENTER;
    #endif

    /* If the low power code returns, this routine returns to the 
       tx_thread_schedule loop.  */
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_low_power_exit                                   PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the low power exit function. This function must    */
/*    be called from any interrupt that can wakeup the processor from     */
/*    low power mode. If nothing needs to be done, this function simply   */
/*    returns.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_time_increment                     Update the ThreadX timer      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                  Front-end of Interrupt        */
/*                                            Service Routines            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
VOID  tx_low_power_exit(VOID)
{
ULONG   tx_low_power_adjust_ticks;

    /* Determine if the interrupt occurred in low power mode.  */
    if (tx_low_power_entered)
    {

        /* Yes, low power mode was interrupted.   */

        /* Clear the low power entered flag.  */
        tx_low_power_entered =  TX_FALSE;
        
        /* User code to exit low power mode and reprogram the 
           timer to the desired interrupt frequency.  */
        #ifdef TX_LOW_POWER_USER_EXIT
        TX_LOW_POWER_USER_EXIT;
        #endif
    
        /* User code to determine how many timer ticks (interrupts) that 
           the ThreadX time should be incremented to properly adjust 
           for the time in low power mode. The result is assumed to be 
           placed in tx_low_power_adjust_ticks.  */
        #ifdef TX_LOW_POWER_USER_TIMER_ADJUST
        tx_low_power_adjust_ticks = TX_LOW_POWER_USER_TIMER_ADJUST;
        #else
        tx_low_power_adjust_ticks = (ULONG)0;
        #endif
        
        /* Determine if the ThreadX timer needs incrementing.  */
        if (tx_low_power_adjust_ticks)
        {

            /* Yes, the ThreadX time must be incremented. Call tx_time_increment
               to accomplish this.  */
            tx_time_increment(tx_low_power_adjust_ticks);
        }
    }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_timer_get_next                                   PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calculates the next expiration time minus 1 tick for  */
/*    the currently active ThreadX timers.  If no timer is active, this   */
/*    routine will return a value of TX_FALSE and the next ticks value    */
/*    will be set to zero.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    next_timer_tick_ptr               Pointer to destination for next   */
/*                                        timer expiration value          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE (1)                       At least one timer is active      */
/*    TX_FALSE (0)                      No timers are currently active    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    tx_low_power_enter                                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
ULONG  tx_timer_get_next(ULONG *next_timer_tick_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_TIMER_INTERNAL           **timer_list_head;
TX_TIMER_INTERNAL           *next_timer;
UINT                        i;
ULONG                       calculated_time;
ULONG                       expiration_time = (ULONG) 0xFFFFFFFF;


    /* Disable interrupts.  */
    TX_DISABLE

    /* Look at the next timer entry.  */
    timer_list_head =  _tx_timer_current_ptr;

    /* Loop through the timer list, looking for the first non-NULL
       value to signal an active timer.  */
    for (i = (UINT)0; i < TX_TIMER_ENTRIES; i++)
    {

        /* Now determine if there is an active timer in this slot.  */
        if (*timer_list_head)
        {

            /* Setup the pointer to the expiration list.  */
            next_timer =  *timer_list_head;

            /* Loop through the timers active for this relative time slot (determined by i).  */
            do
            {

                /* Determine if the remaining time is larger than the list.  */
                if (next_timer -> tx_timer_internal_remaining_ticks > TX_TIMER_ENTRIES)
                {

                    /* Calculate the expiration time.  */
                    calculated_time =  next_timer -> tx_timer_internal_remaining_ticks - (TX_TIMER_ENTRIES - i);
                }
                else
                {

                    /* Calculate the expiration time, which is simply the number of entries in this case.  */
                    calculated_time =  i;
                }

                /* Determine if a new minimum expiration time is present.  */
                if (expiration_time > calculated_time)
                {

                    /* Yes, a new minimum expiration time is present - remember it!  */
                    expiration_time =  calculated_time;
                }

                /* Move to the next entry in the timer list.  */
                next_timer =  next_timer -> tx_timer_internal_active_next;

            } while (next_timer != *timer_list_head);
        }
        
        /* This timer entry is NULL, so just move to the next one.  */
        timer_list_head++;

        /* Check for timer list wrap condition.  */
        if (timer_list_head >= _tx_timer_list_end)
        {

            /* Wrap to the beginning of the list.  */
            timer_list_head =  _tx_timer_list_start;
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Determine if an active timer was found.  */
    if (expiration_time != 0xFFFFFFFF)
    {

        /* Yes, an active timer was found.  */
        *next_timer_tick_ptr =  expiration_time;
        return(TX_TRUE);
    }
    else
    {

        /* No active timer was found.  */
        *next_timer_tick_ptr =  0;
        return(TX_FALSE);
    }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_time_increment                                   PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function increments the current time by a specified value.     */
/*    The value was derived by the application by calling the             */
/*    tx_timer_get_next function prior to this call, which was right      */
/*    before the processor was put in sleep mode.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    time_increment                    The amount of time to catch up on */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_timer_system_activate         Timer activate service            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    tx_low_power_exit                                                   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
VOID  tx_time_increment(ULONG time_increment)
{

TX_INTERRUPT_SAVE_AREA
UINT                        i;
TX_TIMER_INTERNAL           **timer_list_head;
TX_TIMER_INTERNAL           *next_timer;
TX_TIMER_INTERNAL           *temp_list_head;


    /* Determine if there is any time increment.  */
    if (time_increment == 0)
    {

        /* Nothing to do, just return.  */
        return;
    }

    /* Disable interrupts.  */
    TX_DISABLE

    /* Adjust the system clock.  */
    _tx_timer_system_clock =  _tx_timer_system_clock + time_increment;

    /* Adjust the time slice variable.  */
    if (_tx_timer_time_slice)
    {

        /* Decrement the time-slice variable.  */
        if (_tx_timer_time_slice > time_increment)
            _tx_timer_time_slice =  _tx_timer_time_slice - time_increment;
        else
            _tx_timer_time_slice =  1;
    }
    
    /* Calculate the proper place to position the timer.  */
    timer_list_head =  _tx_timer_current_ptr;

    /* Setup the temporary list pointer.  */
    temp_list_head =  TX_NULL;

    /* Loop to pull all timers off the timer structure and put on the 
       the temporary list head.  */
    for (i = 0; i < TX_TIMER_ENTRIES; i++)
    {

        /* Determine if there is a timer list in this entry.  */
        if (*timer_list_head)
        {

            /* Walk the list and update all the relative times to actual times.  */

            /* Setup the pointer to the expiration list.  */
            next_timer =  *timer_list_head;

            /* Loop through the timers active for this relative time slot (determined by i).  */
            do
            {

                /* Determine if the remaining time is larger than the list.  */
                if (next_timer -> tx_timer_internal_remaining_ticks > TX_TIMER_ENTRIES)
                {

                    /* Calculate the actual expiration time.  */
                    next_timer -> tx_timer_internal_remaining_ticks =  
                                    next_timer -> tx_timer_internal_remaining_ticks - (TX_TIMER_ENTRIES - i) + 1;
                }
                else
                {

                    /* Calculate the expiration time, which is simply the number of entries in this
                       case.  */
                    next_timer -> tx_timer_internal_remaining_ticks =  i + 1;
                }

                /* Move to the next entry in the timer list.  */
                next_timer =  next_timer -> tx_timer_internal_active_next;

            } while (next_timer != *timer_list_head);

            /* NULL terminate the current timer list.  */
            ((*timer_list_head) -> tx_timer_internal_active_previous) -> tx_timer_internal_active_next =  TX_NULL;

            /* Yes, determine if the temporary list is NULL.  */
            if (temp_list_head == TX_NULL)
            {

                /* First item on the list.  Move the entire 
                   linked list.  */
                temp_list_head =  *timer_list_head;
            }
            else
            {

                /* No, the temp list already has timers on it. Link the next
                   timer list to the end.  */
                (temp_list_head -> tx_timer_internal_active_previous) -> tx_timer_internal_active_next =  *timer_list_head;

                /* Now update the previous to the new list's previous timer pointer.  */
                temp_list_head -> tx_timer_internal_active_previous =  (*timer_list_head) -> tx_timer_internal_active_previous;
            }

            /* Now clear the current timer head pointer.  */
            *timer_list_head =  TX_NULL;
        }
        
        /* Move to next timer entry.  */
        timer_list_head++;

        /* Determine if a wrap around condition has occurred.  */
        if (timer_list_head >= _tx_timer_list_end)
        {

            /* Wrap from the beginning of the list.  */
            timer_list_head =  _tx_timer_list_start;
        }
    }

    /* Set the current timer pointer to the beginning of the list.  */
    _tx_timer_current_ptr =  _tx_timer_list_start;

    /* Loop to update and reinsert all the timers in the list.  */
    while (temp_list_head)
    {

        /* Pickup the next timer to update and reinsert.  */
        next_timer =  temp_list_head;

        /* Move the temp list head pointer to the next pointer.  */
        temp_list_head =  next_timer -> tx_timer_internal_active_next;

        /* Determine if the remaining time is greater than the time increment 
           value - this is the normal case.  */
        if (next_timer -> tx_timer_internal_remaining_ticks > time_increment)
        {

            /* Decrement the elapsed time.  */
            next_timer -> tx_timer_internal_remaining_ticks =  next_timer -> tx_timer_internal_remaining_ticks - time_increment;
        }
        else
        {

            /* Simply set the expiration value to expire on the next tick.  */
            next_timer -> tx_timer_internal_remaining_ticks =  1;
        }

        /* Now clear the timer list head pointer for the timer activate function to work properly.  */
        next_timer -> tx_timer_internal_list_head =  TX_NULL;

        /* Now re-insert the timer into the list.  */
        _tx_timer_system_activate(next_timer);
    }

    /* Restore interrupts.  */
    TX_RESTORE
}
