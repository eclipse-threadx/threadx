/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.6.0 system_ARMv81MML.c
 * Git SHA: b5f0603d6a584d1724d952fd8b0737458b90d62b
 */

#include "SSE300MPS3.h"

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  #include "partition_CS300.h"
#endif

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
 #define  XTAL             (32000000UL)
 #define  SYSTEM_CLOCK     (XTAL)
 #define  PERIPHERAL_CLOCK (25000000UL)

/*----------------------------------------------------------------------------
  Externals
 *----------------------------------------------------------------------------*/
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    extern uint32_t __VECTOR_TABLE;
#endif



/*****************************************************************/
/*------------------- Memory Protection Controller -----------------------------*/
typedef struct
{
  __IOM  uint32_t CTRL;                     /* Offset: 0x000 (R/W) Control Register */
         uint32_t RESERVED0[3];
  __IM   uint32_t BLK_MAX;                  /* Offset: 0x010 (R/ ) Block Maximum Register */
  __IM   uint32_t BLK_CFG;                  /* Offset: 0x014 (R/ ) Block Configuration Register */
  __IOM  uint32_t BLK_IDX;                  /* Offset: 0x018 (R/W) Block Index Register */
  __IOM  uint32_t BLK_LUT;                  /* Offset: 0x01C (R/W) Block Lookup Tabe Register */
  __IM   uint32_t INT_STAT;                 /* Offset: 0x020 (R/ ) Interrupt Status Register */
  __OM   uint32_t INT_CLEAR;                /* Offset: 0x024 ( /W) Interrupt Clear Register */
  __IOM  uint32_t INT_EN;                   /* Offset: 0x028 (R/W) Interrupt Enable Register */
  __IM   uint32_t INT_INFO1;                /* Offset: 0x02C (R/ ) Interrupt Info1 Register */
  __IM   uint32_t INT_INFO2;                /* Offset: 0x030 (R/ ) Interrupt Info2 Register */
  __OM   uint32_t INT_SET;                  /* Offset: 0x034 ( /W) Interrupt Set Register */
} MPC_TypeDef;

#define MPC_SRAM    ((MPC_TypeDef *) MPC_SRAM_BASE_S )
#define MPC_ISRAM0  ((MPC_TypeDef *) MPC_ISRAM0_BASE_S )


/*------------------- Secure Privilege Control Block -----------------------------*/
typedef struct
{
         uint32_t RESERVED0[5U];
  __IOM  uint32_t NSCCFG;                   /* Offset: 0x014 (R/W) Non Secure Callable Configuration for IDAU */
} SAC_TypeDef;

#define SAC         ((SAC_TypeDef *) SSE300_SACFG_BASE_S )


/*------------------- TCM Gating Unit --------------------------------------------*/
typedef struct /* see "Arm Cortex-Mxx Technical Reference Manual r0p1"
" */
{
  __IOM  uint32_t CTRL;                     /* Offset: 0x000 (R/W) Gate Control Register */
  __IM   uint32_t CFG;                      /* Offset: 0x004 (R/ ) Gate Configuration Register */
         uint32_t RESERVED0[2];
  __IOM  uint32_t LUT[16];                  /* Offset: 0x010 (R/W) Gate Look-up Table Register */
} TGU_TypeDef;

#define ITGU_BASE    (0xE001E500UL)         /* ITCM Gating Unit */
#define DTGU_BASE    (0xE001E600UL)         /* DTCM Gating Unit */

#define ITGU         ((TGU_TypeDef *) ITGU_BASE)     
#define DTGU         ((TGU_TypeDef *) DTGU_BASE)     
/*****************************************************************/



/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;
uint32_t PeripheralClock = PERIPHERAL_CLOCK;

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)
{
    SystemCoreClock = SYSTEM_CLOCK;
    PeripheralClock = PERIPHERAL_CLOCK;
}

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t)(&__VECTOR_TABLE);
#endif

#if (defined (__FPU_USED) && (__FPU_USED == 1U)) || \
    (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
  SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */

  /* Set low-power state for PDEPU                */
  /*  0b00  | ON, PDEPU is not in low-power state */
  /*  0b01  | ON, but the clock is off            */
  /*  0b10  | RET(ention)                         */
  /*  0b11  | OFF                                 */

  /* Clear ELPSTATE, value is 0b11 on Cold reset */
  PWRMODCTL->CPDLPSTATE &= ~(PWRMODCTL_CPDLPSTATE_ELPSTATE_Msk);

  /* Favor best FP/MVE performance by default, avoid EPU switch-ON delays */
  /* PDEPU ON, Clock OFF */
  PWRMODCTL->CPDLPSTATE |= 0x1 << PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos;
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

    /* Enable Loop and branch info cache */
    SCB->CCR |= SCB_CCR_LOB_Msk;
    __DSB();
    __ISB();


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  /* Enable BusFault, UsageFault, MemManageFault and SecureFault to ease diagnostic */
  SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk  |
                 SCB_SHCSR_BUSFAULTENA_Msk  |
                 SCB_SHCSR_MEMFAULTENA_Msk  |
                 SCB_SHCSR_SECUREFAULTENA_Msk);

  /* BFSR register setting to enable precise errors */
  SCB->CFSR |= SCB_CFSR_PRECISERR_Msk;

  /* configure unsecure code area: ITCM 512K 0x00080000 - 0x00100000 */
  // blk_cfg = ITGU->CFG & 0xF;             /* = 0x7 */
  // blk_size = 1UL << (blk_cfg + 5U);      /* = 0x1000 (4K) */ 
  ITGU->LUT[4] = 0xFFFFFFFF;
  ITGU->LUT[5] = 0xFFFFFFFF;
  ITGU->LUT[6] = 0xFFFFFFFF;
  ITGU->LUT[7] = 0xFFFFFFFF;

  /* configure unsecure data area: DTCM 512K 0x20080000 - 0x20100000 */
  // blk_cfg = DTGU->CFG & 0xF;             /* = 0x7 */
  // blk_size = 1UL << (blk_cfg + 5U);      /* = 0x1000 (4K) */ 
  DTGU->LUT[4] = 0xFFFFFFFF;
  DTGU->LUT[5] = 0xFFFFFFFF;
  DTGU->LUT[6] = 0xFFFFFFFF;
  DTGU->LUT[7] = 0xFFFFFFFF;

  /* Non Secure Callable Configuration for IDAU (NSCCFG register) */
  SAC->NSCCFG |= 1U; /* Configure CODE region (0x1000_0000 to 0x1FFF_FFFF) Non-secure Callable */
//SAC->NSCCFG |= 2U; /* Configure RAM  region (0x3000_0000 to 0x3FFF_FFFF) Non-secure Callable */


  TZ_SAU_Setup();
#endif

  SystemCoreClock = SYSTEM_CLOCK;
}
