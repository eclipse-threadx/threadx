# MCUboot Secure Boot and Firmware Upgrade Integration for Eclipse ThreadX

## Overview

This guide describes how to integrate [MCUboot](https://github.com/mcu-tools/mcuboot) with [Eclipse ThreadX](https://github.com/eclipse-threadx/threadx) to provide secure boot validation, cryptographic signature verification, dual-slot fail-safe firmware updates, and automatic rollback capabilities.

MCUboot provides first-class support for Eclipse ThreadX under `boot/threadx/` in the MCUboot repository.

---

## Architecture and Memory Layout

The reference layout targets dual-bank NOR flash microcontrollers (such as STM32F429ZI Arm Cortex-M4 with 2 MB internal flash):

| Partition Name | Flash Area ID | Start Offset | Size | Flash Address Range |
| :--- | :--- | :--- | :--- | :--- |
| **MCUboot Bootloader** | N/A | `0x00000000` | 128 KB | `0x08000000` - `0x0801FFFF` |
| **Primary Slot (Active Application)** | `FLASH_AREA_IMAGE_0` (1) | `0x00020000` | 384 KB | `0x08020000` - `0x0807FFFF` |
| **Secondary Slot (Upgrade Staging)** | `FLASH_AREA_IMAGE_1` (2) | `0x00080000` | 384 KB | `0x08080000` - `0x080DFFFF` |
| **Scratch Slot** | `FLASH_AREA_IMAGE_SCRATCH` (3) | `0x000E0000` | 128 KB | `0x080E0000` - `0x080FFFFF` |
| **User Data / Storage** | N/A | `0x00100000` | 1024 KB | `0x08100000` - `0x081FFFFF` |

---

## Bootloader Execution Flow

1. On system reset, the CPU executes the MCUboot bootloader located at `0x08000000`.
2. MCUboot validates the integrity and cryptographic signature (ECDSA P-256 via SHA-256) of the active image in the Primary Slot (`0x08020000`).
3. MCUboot inspects the image trailers in the Primary, Secondary, and Scratch slots to check if an update is staged or in-progress.
4. If an update is pending:
   - MCUboot swaps sectors between the Primary and Secondary slots using the Scratch partition.
   - The swap operation is atomic and power-loss resilient: if power is interrupted mid-swap, MCUboot recovers and completes the swap upon the next boot.
5. MCUboot sets the stack pointer and vector table (`SCB->VTOR = 0x08020000 + 0x200`) and jumps to the ThreadX application entry point.

---

## ThreadX Application Workflow

### 1. Linking MCUboot Client Services

To enable in-application firmware upgrade management, the ThreadX application links against `bootutil_threadx` (built from `mcuboot/boot/threadx/`) or includes MCUboot's `bootutil` library directly.

Include the following headers:
```c
#include "tx_api.h"
#include "bootutil/bootutil.h"
#include "sysflash/sysflash.h"
```

### 2. Confirming a Newly Booted Image

When MCUboot performs a test upgrade, the new image must self-confirm by writing a confirmation flag to the image trailer. If the image reboots without confirming (e.g. crash, watchdog reset, or self-test failure), MCUboot automatically rolls back to the previous known-good version.

```c
VOID tx_application_define(void *first_unused_memory)
{
    /* Initialize application threads */
    tx_thread_create(&update_thread, "Update Thread", update_thread_entry, 0,
                     update_stack, sizeof(update_stack),
                     10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
}

VOID update_thread_entry(ULONG parameter)
{
    /* Perform system health checks, hardware diagnostics, and network verification */
    if (run_system_diagnostics() == TX_SUCCESS)
    {
        /* Confirm the active image so MCUboot does not initiate a rollback */
        int rc = boot_set_confirmed_multi(0);
        if (rc == 0)
        {
            /* Image confirmed as stable */
        }
    }
    else
    {
        /* Diagnostics failed; trigger software reset to force rollback */
        NVIC_SystemReset();
    }
}
```

### 3. Staging and Triggering a Firmware Update

When a new firmware binary is received over the network (e.g. via NetX Duo MQTT or HTTP):

```c
UINT stage_firmware_update(const UCHAR *firmware_image, ULONG image_size)
{
    const struct flash_area *fa;
    int rc;

    /* 1. Open Secondary Slot */
    rc = flash_area_open(FLASH_AREA_IMAGE_1, &fa);
    if (rc != 0) return TX_START_ERROR;

    /* 2. Erase Secondary Slot */
    rc = flash_area_erase(fa, 0, fa->fa_size);
    if (rc != 0) {
        flash_area_close(fa);
        return TX_START_ERROR;
    }

    /* 3. Write new signed image to Secondary Slot */
    rc = flash_area_write(fa, 0, firmware_image, image_size);
    flash_area_close(fa);
    if (rc != 0) return TX_START_ERROR;

    /* 4. Request test swap on next reboot */
    rc = boot_set_pending_multi(0, 0); /* 0 = test swap, 1 = permanent */
    if (rc != 0) return TX_START_ERROR;

    /* 5. Reboot to trigger MCUboot swap */
    NVIC_SystemReset();

    return TX_SUCCESS;
}
```

---

## Signing Application Images

Applications must be signed with `imgtool.py` to be validated by MCUboot:

```bash
# 1. Generate an ECDSA P-256 keypair
python scripts/imgtool.py keygen -k root-ec-p256.pem -t ecdsa-p256

# 2. Sign the raw application binary
python scripts/imgtool.py sign \
    --key root-ec-p256.pem \
    --header-size 0x200 \
    --align 8 \
    --version 1.0.0+0 \
    --slot-size 0x60000 \
    --pad-header \
    build/threadx_app.bin \
    build/threadx_app_signed.bin
```

---

## References

- [MCUboot Documentation for Eclipse ThreadX](https://github.com/mcu-tools/mcuboot/blob/main/docs/readme-threadx.md)
- [MCUboot Repository](https://github.com/mcu-tools/mcuboot)
- [ThreadX Issue #778](https://github.com/eclipse-threadx/threadx/issues/778)
- [SampleX Reference Targets](https://github.com/eclipse-threadx/samplex)
