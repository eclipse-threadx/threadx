# ESP32-C6 ThreadX Bring-Up and SampleX Native Wi-Fi Integration

## 1. Objective

Deliver ESP32-C6 support at the appropriate level in two Eclipse ThreadX
repositories:

- A basic ESP32-C6 ThreadX bring-up example under the generic RV32 GNU port's
  `example_build` directory in the ThreadX repository.
- A complete ESP32-C6 sample integration in SampleX.
- The ESP32-C6 integrated Wi-Fi radio.
- A native ThreadX adaptation of the Espressif Wi-Fi OS interface.
- NetX Duo as the only TCP/IP stack.
- No FreeRTOS runtime, lwIP, `esp_netif`, `esp_event`, or stock ESP-IDF
  `esp_timer` runtime component.
- No external network or USB controller hardware.
- No USBX.

The Espressif Wi-Fi and PHY binary libraries remain required. Reimplementing the
closed MAC/PHY functionality is outside the scope of the project. "ThreadX-native
Wi-Fi" therefore means that the vendor libraries run on ThreadX services directly,
without a FreeRTOS compatibility layer.

The ThreadX repository will not ship a complete ESP32-C6 BSP or the vendor Wi-Fi
integration. Its ESP32-C6 content will be limited to the minimum board adaptation
needed to demonstrate and validate the generic RISC-V32 port. SampleX will own the
full board integration, ESP-IDF dependency, Wi-Fi adaptation, NetX Duo driver, and
network application.

Selected ESP-IDF timer sources are an intentional exception to treating ESP-IDF
only as a hardware reference. SampleX will maintain a pinned, narrowly patched
import of the `esp_timer` core, its ESP32-C6 SYSTIMER backend, and the ETS
compatibility layer. All operating-system integration in that import will use
native ThreadX services; it will not pull in the ESP-IDF component manager,
startup framework, FreeRTOS, or interrupt allocator.

## 2. Architectural decision

ESP-IDF will be used as a silicon reference and vendor SDK, not as the application
framework. A conventional ESP-IDF application is unsuitable because its startup
flow and many of its components assume FreeRTOS.

Repository ownership is deliberately split:

| Repository | Owns | Does not own |
|---|---|---|
| ThreadX | Generic RV32 port fixes and a minimal ESP32-C6 example under `ports/risc-v32/gnu/example_build/esp32c6` | Full BSP productization, ESP-IDF integration, Wi-Fi, NetX Duo, or application features |
| SampleX | Complete Espressif/ESP32-C6 sample, board support needed by that sample, pinned ESP-IDF integration, native Wi-Fi OS adapter, WPA support, NetX Duo link driver, and demo application | Generic ThreadX kernel/port implementation |

SampleX consumes ThreadX and NetX Duo through its existing `libs/threadx` and
`libs/netxduo` submodules. It should reuse the basic ESP32-C6 bring-up support from
the pinned ThreadX submodule where practical, then layer the vendor-specific Wi-Fi
services on top.

The target architecture is:

```text
ESP32-C6 mask ROM
    |
    v
Espressif second-stage bootloader
    |
    v
ThreadX-native application and board support
    +-- startup, memory, cache, and flash conventions
    +-- RISC-V trap entry and C6 PLIC handling
    +-- ThreadX SYSTIMER tick
    +-- ThreadX-adapted IDF high-resolution timer and ETS service
    +-- console, reset, random, eFuse, and MAC services
    |
    v
ThreadX
    +-- application threads
    +-- Wi-Fi threads created through the native OS adapter
    +-- NetX Duo threads
    |
    v
Espressif Wi-Fi OS adapter implemented with ThreadX
    |
    v
Espressif Wi-Fi, PHY, and WPA supplicant code
    |
    v
ESP32-C6 integrated radio

Wi-Fi Ethernet frames <--> NetX Duo IP driver
```

This boundary minimizes FreeRTOS emulation, gives ThreadX ownership of interrupt
and scheduling semantics, and keeps NetX Duo in direct control of IP networking.
The minimal version of the path through `tx_kernel_enter()` is proven first by the
ThreadX `example_build`; the complete path shown above is delivered by SampleX.

## 3. ESP-IDF reuse boundary

### 3.1 Reuse

Reuse the following from an exactly pinned ESP-IDF revision:

- `riscv32-esp-elf` compiler, assembler, linker, and binary utilities.
- The Espressif second-stage bootloader.
- ESP application image and partition-table formats.
- `esptool.py` image generation and flashing support.
- Espressif OpenOCD configuration.
- ESP32-C6 ROM symbol linker scripts.
- ESP32-C6 register definitions and capability headers.
- Selected RTOS-independent HAL and LL headers and implementations.
- eFuse and base-MAC-address access logic.
- PHY initialization data and calibration behavior.
- Prebuilt ESP32-C6 Wi-Fi and PHY archives.
- The minimum required Espressif WPA supplicant sources.
- Small RTOS-independent portions of clock, modem, reset, and peripheral setup.
- The task-dispatch portions of `esp_timer`, the ESP32-C6 SYSTIMER timer backend,
  and `ets_timer_legacy`, maintained as a small SampleX-local adaptation.

The stock second-stage bootloader should be retained initially. Replacing it would
add flash setup, validation, partition, and security work without advancing the
ThreadX or networking demonstration.

### 3.2 Do not reuse as frameworks

Do not include these subsystems in the ThreadX application:

- FreeRTOS or the ThreadX FreeRTOS compatibility layer.
- The normal `esp_system` application startup sequence.
- `esp_event`.
- The stock `esp_timer` component build, startup hooks, FreeRTOS integration,
  profiling, ISR-dispatch mode, ETM, wall-clock integration, and sleep-time
  compensation. Only the explicitly listed timer sources are adapted.
- `esp_netif`.
- lwIP.
- VFS and pthread integration.
- IDF task and interrupt watchdog services initially.
- IDF power-management, light-sleep, and retention frameworks.
- The default ESP-IDF Wi-Fi OS adapter.
- Bluetooth, IEEE 802.15.4, and coexistence initially.

High-level ESP-IDF drivers may be considered later only after verifying that they
do not introduce FreeRTOS or other excluded dependencies. The minimal ThreadX
example and the fuller SampleX board integration should prefer ROM calls, SoC
headers, and HAL/LL operations.

## 4. Source and version policy

Pin exact commits for:

- ESP-IDF.
- ThreadX.
- NetX Duo.

ThreadX and SampleX changes should be developed as separate, ordered changes:

1. Land any genuinely generic RV32 fixes and the minimal ESP32-C6
   `example_build` in ThreadX.
2. Update the SampleX `libs/threadx` submodule to that ThreadX revision.
3. Add the complete ESP32-C6 Wi-Fi/NetX Duo sample in SampleX.

ESP32-C6-specific source must not be added to the generic ThreadX port's `inc` or
`src` directories unless it exposes and fixes a genuinely architecture-generic
problem. Board startup, linker, kernel-tick timer, PLIC, and console files belong
below the ESP32-C6 `example_build` directory. High-resolution/ETS timers, Wi-Fi,
PHY, WPA, ESP-IDF integration, and NetX link code belong only in SampleX.

The Wi-Fi OS interface, internal Layer-2 APIs, binary archive dependencies, ROM
symbols, and linker fragments can change between ESP-IDF revisions. The build must
fail if the expected Wi-Fi OS adapter version changes.

Follow the useful integration pattern demonstrated by Zephyr's Espressif HAL:

- Pin one matching ESP-IDF/HAL revision rather than consume the developer's
  installed SDK implicitly.
- Compile only an explicit source manifest.
- Keep RTOS substitutions in a small, reviewable patch series or adaptation
  layer rather than modifying a full imported SDK tree by hand.
- Use native kernel objects and native interrupt dispatch instead of emulating a
  FreeRTOS runtime.
- Retain vendor data structures and behavioral semantics at the binary-library
  boundary.
- Make patch application fail when its expected source context changes.

The current ESP-IDF Wi-Fi adapter declares `ESP_WIFI_OS_ADAPTER_VERSION` in:

```text
components/esp_wifi/include/esp_private/wifi_os_adapter.h
```

Record the following in a machine-readable manifest:

- ESP-IDF commit.
- ThreadX and NetX Duo commits.
- Wi-Fi OS adapter version.
- List and checksums of linked vendor archives.
- Compiler version and target flags.
- Required private Espressif symbols.
- Imported timer source paths, hashes, configuration, and adaptation patch level.

Preserve all Espressif copyright and Apache-2.0 notices when adapting source.
Outside the explicit imported-source manifests, prefer vendor headers or behavior
implemented from public specifications over copying source bodies.

## 5. Proposed repository layouts

### 5.1 ThreadX repository

```text
threadx/
`-- ports/risc-v32/gnu/
    |-- inc/                         # Generic RV32 GNU port only
    |-- src/                         # Generic RV32 GNU port only
    `-- example_build/
        `-- esp32c6/
            |-- CMakeLists.txt
            |-- README.md
            |-- cmake/
            |   `-- esp32c6-toolchain.cmake
            |-- platform/
            |   |-- startup.c
            |   |-- interrupt.c
            |   |-- systimer.c
            |   |-- console.c
            |   `-- gpio.c
            |-- include/
            |   |-- esp32c6_example.h
            |   `-- tx_user.h
            |-- entry.S
            |-- vectors.S
            |-- tx_initialize_low_level.S
            |-- link.ld
            |-- demo_threadx.c
            `-- tests/
```

The example proves only startup, the ThreadX timer tick, context switching, basic
interrupt dispatch, console output, and a simple GPIO or timer demonstration. It
must not link NetX Duo or the Espressif Wi-Fi/PHY libraries.

### 5.2 SampleX repository

Follow SampleX's existing vendor/board organization:

```text
samplex/
|-- libs/
|   |-- threadx/                    # Existing submodule
|   `-- netxduo/                    # Existing submodule
`-- Espressif/
    `-- ESP32-C6/
        |-- CMakeLists.txt
        |-- README.md
        |-- NOTICE.md
        |-- app/
        |   |-- CMakeLists.txt
        |   |-- common/
        |   |   |-- board_init.c
        |   |   |-- console.c
        |   |   `-- wifi_manager.c
        |   `-- starter/
        |       `-- main.c
        |-- cmake/
        |   |-- esp32c6-toolchain.cmake
        |   |-- espressif-libraries.cmake
        |   `-- make-image.cmake
        |-- deps/
        |   `-- esp-idf/             # Pinned dependency or fetched equivalent
        |-- vendor/
        |   `-- esp_timer/
        |       |-- README.md         # Origin revision and supported feature set
        |       |-- sources.cmake     # Explicit imported-source manifest
        |       |-- patches/          # Small ThreadX adaptation patch series
        |       `-- include/          # Narrow configuration/adaptation headers
        |-- lib/
        |   |-- CMakeLists.txt
        |   |-- esp32c6_base/        # Reuses ThreadX example support
        |   |-- esp32c6_wifi/
        |   |   |-- esp_wifi_os_adapter.c
        |   |   |-- esp_wifi_platform.c
        |   |   |-- esp_wifi_events.c
        |   |   `-- esp_phy_platform.c
        |   |-- esp32c6_timer/
        |   |   |-- esp_timer_threadx.c
        |   |   |-- esp_timer_platform.c
        |   |   `-- esp_timer_platform.h
        |   |-- netxduo/
        |   |   |-- nx_user.h
        |   |   |-- nx_driver_esp32c6_wifi.c
        |   |   `-- nx_wifi_manager.c
        |   `-- threadx/
        |       `-- tx_user.h
        |-- scripts/
        |   |-- build.sh
        |   `-- deploy.sh
        `-- tests/
```

`lib/esp32c6_base` should compile the basic startup, vectors, PLIC, SYSTIMER, and
console support from the pinned ThreadX submodule's ESP32-C6 `example_build` when
the files are suitable for reuse. SampleX-local wrappers may expose a cleaner board
API, but should not fork the generic ThreadX assembly without a documented reason.

The preferred ESP-IDF dependency model is a pinned SampleX dependency under the
board directory so the sample is reproducible. If repository-size policy rules out
an ESP-IDF submodule, provide a setup script that fetches and verifies an exact
commit; an unconstrained `ESP_IDF_ROOT` from the developer's environment is not
sufficient for CI or release validation.

Keep the pinned ESP-IDF checkout pristine. At configure time, copy only the files
listed in `vendor/esp_timer/sources.cmake` into a generated build staging directory,
verify their hashes, and apply the ordered adaptation patches there. Patch failure
is a configuration error. If SampleX policy instead requires committed imported
sources, record the same hashes and provide a script that reproduces the import
from the pinned ESP-IDF commit.

## 6. Build and image strategy

There are two builds with different purposes.

### 6.1 ThreadX basic example build

The ThreadX repository build compiles the generic `risc-v32/gnu` port together with
the files under `example_build/esp32c6`. It produces a minimal application image
that proves the port on hardware. Its CMake files may use the Espressif compiler,
ROM linker symbols, image conversion tool, and stock second-stage bootloader, but
must not acquire a dependency on the ESP-IDF application framework.

### 6.2 SampleX integration build

The full standalone CMake and Ninja build lives under
`SampleX/Espressif/ESP32-C6` and is driven by Espressif's toolchain:

```sh
cd Espressif/ESP32-C6
source deps/esp-idf/export.sh

cmake -S . -B build -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=cmake/esp32c6-toolchain.cmake \
    -DESP_IDF_ROOT=$PWD/deps/esp-idf

cmake --build build
```

The toolchain file must use the C6 ISA, ABI, code model, and linker conventions
from the pinned ESP-IDF revision. Do not guess these flags independently.

The build should produce:

- A standard ELF with debug information.
- An Espressif application binary produced with `esptool.py elf2image`.
- A map file.
- A manifest of linked vendor libraries and unresolved-symbol closure.
- Flash arguments for the stock bootloader, partition table, and application.

Initially disable secure boot and flash encryption. Add them only after the basic
image, reset, and flash paths are stable.

The application linker script must provide:

- Correct ESP32-C6 IRAM, DRAM, RTC, and flash mappings.
- Espressif application metadata expected by the bootloader.
- ROM symbol resolution.
- Dedicated startup/system stack space.
- ThreadX code and data placement.
- IRAM placement for interrupt and cache-disabled paths.
- Explicit regions for ThreadX stacks and network packet pools where useful.

## 7. Basic ESP32-C6 adaptation in the ThreadX RV32 example

Add the minimum C6-specific interrupt and initialization layer below
`ports/risc-v32/gnu/example_build/esp32c6`. The generic port's interrupt-frame
contract is not directly compatible with ESP-IDF's RISC-V vector implementation,
so the example supplies its own board-level entry and vector code while continuing
to build the generic RV32 GNU port sources unchanged wherever possible.

### 7.1 Startup

The application entry path must:

1. Accept control from the Espressif second-stage bootloader.
2. Establish or verify `gp`, `sp`, and machine-mode state.
3. Initialize `.data`, `.bss`, and any required C runtime state not already handled.
4. Preserve the flash/cache configuration established by the bootloader.
5. Initialize the console and early diagnostics.
6. Install the ThreadX-compatible trap vector in `mtvec`.
7. Initialize the C6 interrupt controller to a known state.
8. Call `tx_kernel_enter()`.

`tx_application_define()` creates only the simple validation threads and objects
needed by the ThreadX example. The full bootstrap, vendor services, and networking
threads are SampleX responsibilities.

### 7.2 Interrupt and trap handling

Implement a C6-specific trap entry around the ThreadX RV32 context contract:

- Save the interrupted context in the format expected by ThreadX.
- Support machine-mode execution.
- Decode synchronous exceptions separately from interrupts.
- Decode C6 PLIC interrupt numbers.
- Switch to the ThreadX system/interrupt stack on first-level interrupt entry.
- Track nested interrupt state even if nesting is initially disabled.
- Dispatch through a small example-local interrupt table.
- Return through ThreadX context-restore logic so preemption occurs correctly.

The initial implementation should disable nested interrupts. Add prioritized nesting
only after thread switching, timer interrupts, and peripheral interrupts pass stress
tests. When nesting is enabled, account for both `mstatus.MIE` and the C6 PLIC
threshold.

Expose only the minimal example-local interrupt API needed to validate the port,
such as:

```c
typedef void (*c6_example_isr_t)(void *context);

int c6_example_interrupt_attach(unsigned source,
                                unsigned priority,
                                c6_example_isr_t handler,
                                void *context);
void c6_example_interrupt_enable(unsigned interrupt_number);
void c6_example_interrupt_disable(unsigned interrupt_number);
```

### 7.3 ThreadX tick

Use one C6 SYSTIMER comparator for the ThreadX tick:

- Configure a periodic alarm.
- Route it through the interrupt matrix and PLIC.
- Clear the comparator interrupt in the ISR.
- Invoke `_tx_timer_interrupt` with the required ThreadX context-save/restore flow.
- Start with a 100 Hz tick.

Initial timing configuration:

```c
#define TX_TIMER_TICKS_PER_SECOND 100
```

### 7.4 SampleX extension: ThreadX-adapted IDF timer service

The minimal ThreadX example does not require the Wi-Fi timer service. SampleX
adds it by adapting the timer implementation from the exact ESP-IDF revision that
supplies the Wi-Fi libraries. This preserves Espressif's ordered expiration,
periodic rearm, callback, cancellation, and deferred-delete semantics instead of
reimplementing them independently.

The initial source manifest contains only:

```text
components/esp_timer/src/esp_timer.c
components/esp_timer/src/esp_timer_impl_common.c
components/esp_timer/src/esp_timer_impl_systimer.c
components/esp_timer/src/ets_timer_legacy.c
components/esp_hal_systimer/systimer_hal.c
components/esp_hw_support/port/esp32c6/systimer.c
```

Include only the corresponding public/private timer headers, ESP32-C6 SoC
definitions, ROM `ETSTimer` definition, and SYSTIMER HAL/LL headers required by
that manifest. Re-evaluate the list against the pinned revision rather than
assuming paths from another ESP-IDF release.

Configure the imported timer code for:

- Single-core ESP32-C6 operation.
- `ESP_TIMER_TASK` dispatch only.
- No profiling, ETM, system-time provider, RTC synchronization, light sleep, or
  power-management compensation.
- Internal RAM allocation only.
- Explicit SampleX initialization and no ESP-IDF startup macros.

Map the RTOS-dependent operations as follows:

| IDF timer dependency | SampleX ThreadX implementation |
|---|---|
| Timer task | Statically allocated `TX_THREAD` and stack |
| ISR-to-task notification | One-bit `TX_EVENT_FLAGS_GROUP` using `TX_OR`/`TX_OR_CLEAR` |
| `portMUX_TYPE` list/register locks | Saved C6 interrupt posture with nesting-safe restore |
| Current task | `tx_thread_identify()` |
| Tick count and one-tick delay | `tx_time_get()` and `tx_thread_sleep(1)` |
| Internal allocation | Dedicated `TX_BYTE_POOL`, nonblocking allocation plus explicit zeroing for `calloc` semantics |
| ISR-context query | SampleX C6 interrupt-context helper |
| `esp_intr_alloc` | SampleX native interrupt attach/enable API |
| ESP-IDF startup hook | Explicit SampleX board/application initialization |

Use event flags rather than an unbounded counting semaphore because IDF task
notifications coalesce repeated alarm interrupts. The SYSTIMER ISR clears alarm 2
and sets the event bit; ThreadX performs any necessary preemption when the ISR
returns through its normal context-restore path.

Preserve the `ETSTimer` ABI and legacy wrapper behavior. In particular, keep
deletion deferred to the timer thread and store `pfunction` in
`ETSTimer.timer_func` as a defensive compatibility measure used by the Zephyr
adaptation because vendor blobs may inspect that field.

The adapted code is an internal Espressif compatibility service. Applications
should use ThreadX facilities unless they specifically require the Espressif timer
ABI.

### 7.5 Shared SYSTIMER ownership and initialization

Retain ESP-IDF's nonconflicting ESP32-C6 resource allocation:

| Consumer | Counter | Alarm |
|---|---:|---:|
| ThreadX kernel tick | 1 | 0 |
| Espressif high-resolution timer | 0 | 2 |

SampleX owns the SYSTIMER peripheral globally. It enables and resets the peripheral
exactly once before either consumer is configured. The adapted
`esp_timer_impl_early_init()` must not reset an already configured SYSTIMER or use
ESP-IDF's peripheral reference-counting service.

Initialization order is mandatory:

1. Enable and reset SYSTIMER once in early SampleX platform initialization.
2. Initialize high-resolution counter 0 and one-shot alarm 2.
3. Configure the ThreadX periodic tick on counter 1 and alarm 0.
4. Enter ThreadX.
5. In `tx_application_define()`, create the timer thread and event-flags object,
   attach the alarm-2 ISR, and enable its interrupt before Wi-Fi starts.

Accesses to shared SYSTIMER interrupt-enable and configuration registers must use
the platform interrupt-posture lock. Timer initialization and ThreadX tick
initialization must never independently reset the peripheral.

### 7.6 Memory ownership

The ThreadX example uses static internal SRAM for its stacks and objects. SampleX
adds the following, still using internal SRAM during initial development:

- Statically allocated critical ThreadX stacks and control blocks.
- A ThreadX byte pool for general dynamic objects.
- A separate internal-memory allocator for vendor Wi-Fi allocations.
- Explicit alignment and capability checks for DMA-visible buffers.
- A dedicated NetX packet pool.

Leave PSRAM disabled until Wi-Fi and NetX are stable. Define clear ownership rules
for every allocation crossing the Wi-Fi/NetX boundary.

### 7.7 Responsibility boundary

The ThreadX `example_build` implements:

- Early console output.
- Reset and reboot.
- PLIC and interrupt-matrix control.
- SYSTIMER kernel tick.
- GPIO output and interrupt support.

SampleX adds:

- ThreadX-adapted IDF high-resolution and ETS timers.
- Hardware random-number access.
- eFuse/base-MAC access.
- Flash reads.
- Modem and Wi-Fi clock control.
- PHY/RF power-domain control.
- Cache-safe ISR placement.

Both deliverables defer sleep, dynamic frequency scaling, watchdog integration,
PSRAM, and generalized peripheral-driver APIs until the full sample is stable.

## 8. SampleX minimal Wi-Fi feature profile

All Wi-Fi work from this point forward belongs under
`SampleX/Espressif/ESP32-C6`. None of these sources or dependencies are added to
the ThreadX repository.

The first working configuration should support only:

- Station mode.
- WPA2-Personal/PSK.
- IPv4.
- NetX Duo DHCP client.
- NetX Duo DNS client.
- `WIFI_PS_NONE`.

Disable initially:

- SoftAP.
- WPA Enterprise.
- WPA3/SAE.
- WPS and SmartConfig.
- ESP-NOW, mesh, NAN, roaming, and FTM.
- Bluetooth and IEEE 802.15.4.
- Radio coexistence.
- Modem sleep, light sleep, MAC/BB retention, and tickless operation.
- PSRAM.
- Wi-Fi NVS persistence.

The feature configuration must remove source and dependency paths, not merely leave
unused APIs uncalled where that would still pull unwanted initialization code.

## 9. SampleX Espressif Wi-Fi and PHY linkage

The pinned ESP-IDF tree supplies ESP32-C6 Wi-Fi archives including:

```text
components/esp_wifi/lib/esp32c6/libcore.a
components/esp_wifi/lib/esp32c6/libnet80211.a
components/esp_wifi/lib/esp32c6/libpp.a
components/esp_wifi/lib/esp32c6/libespnow.a
components/esp_wifi/lib/esp32c6/libmesh.a
components/esp_wifi/lib/esp32c6/libsmartconfig.a
components/esp_wifi/lib/esp32c6/libwapi.a
```

The PHY libraries are under:

```text
components/esp_phy/lib/esp32c6/
```

Begin with `libcore.a`, `libnet80211.a`, `libpp.a`, and the required PHY library,
then determine the actual closure mechanically. Do not assume that archive names
fully describe their shared modem dependencies.

Use `riscv32-esp-elf-nm` to generate a report of undefined symbols from every
selected archive. Classify each symbol as:

- ESP ROM.
- SampleX board/platform hardware service.
- Wi-Fi OS adapter function.
- PHY platform function.
- WPA supplicant function.
- Required vendor support code.
- Disabled feature that should be removed.
- Unsupported or unexplained dependency.

No unexplained symbols or permissive catch-all stubs should remain at the end of
the closure phase.

## 10. SampleX native ThreadX Wi-Fi OS adapter

Implement `g_wifi_osi_funcs` directly with ThreadX. Preserve the exact structure
layout, version, magic value, function signatures, and return conventions expected
by the pinned vendor archives.

### 10.1 Object mappings

| Espressif service | Native implementation |
|---|---|
| Task creation/deletion | `tx_thread_create`, terminate, and delete |
| Delay | `tx_thread_sleep` with explicit tick conversion |
| Current task | `tx_thread_identify` |
| Counting/binary semaphore | `TX_SEMAPHORE` wrapper |
| Mutex | `TX_MUTEX` with priority inheritance |
| Recursive mutex | Owner and recursion wrapper around `TX_MUTEX` |
| Queue | Arbitrary-item-size queue wrapper |
| Event group | `TX_EVENT_FLAGS_GROUP` |
| Per-thread semaphore | `TX_THREAD_USER_EXTENSION` or adapter registry |
| Interrupt lock/restore | SampleX C6 board interrupt posture API |
| Software timers | ThreadX-adapted IDF `esp_timer`/ETS service |
| Time in microseconds | Adapted `esp_timer_get_time()` on SYSTIMER counter 0 |
| Dynamic memory | ThreadX byte pool/SampleX internal heap |
| Free heap query | SampleX allocator accounting |
| Event posting | Native ThreadX Wi-Fi event queue |
| Random data | C6 hardware RNG |
| Base MAC | eFuse/ROM service |
| Logging | SampleX board console logger |
| NVS | Disabled adapter initially; native storage later |

### 10.2 Priority mapping

Espressif uses FreeRTOS-style priorities where larger numbers are more urgent;
ThreadX uses smaller values for higher priorities. Use a reviewed lookup table,
not a blind arithmetic mapping.

Example allocation:

```text
ThreadX 0-3    reserved for kernel/critical deferred work
ThreadX 4-7    Espressif Wi-Fi protocol threads
ThreadX 8-9    Wi-Fi timer and receive-deferred threads
ThreadX 10-14  NetX and application networking threads
ThreadX 20+    background application work
```

Validate that every vendor-requested priority maps into a legal ThreadX range.

### 10.3 Queue adapter

Espressif queues accept arbitrary item sizes, while a ThreadX queue transfers one
or more `ULONG` words. Implement an adapter object containing:

- Fixed-size item storage.
- A free-slot pool.
- A ThreadX queue carrying slot pointers or indices.
- Send-to-front and send-to-back support.
- Nonblocking ISR send.
- Accurate waiting-message count.
- Timeout conversion.
- Defined deletion behavior.

Test ordering, full/empty behavior, finite and infinite waits, ISR posting, and
multiple producers/consumers independently of Wi-Fi.

### 10.4 Recursive mutexes and thread-local state

Implement recursive mutex ownership explicitly if the underlying ThreadX mutex
configuration does not provide identical semantics. Store per-Wi-Fi-thread data
through a `TX_THREAD_USER_EXTENSION` field or a protected registry indexed by
`TX_THREAD *`.

Thread deletion must release adapter-owned thread-local objects without leaking
memory or deleting objects still in use.

### 10.5 Events

Replace `esp_event_post` with a bounded native event queue. Copy event payloads
whose lifetime does not extend beyond the vendor callback.

The vendor callback must only enqueue the event. A dedicated Wi-Fi manager thread
updates state and calls application handlers.

### 10.6 Power-management and coexistence hooks

With power saving, Bluetooth, 802.15.4, and coexistence disabled, some hooks may be
valid no-ops. Document every such hook and verify its expected disabled-feature
behavior in the pinned IDF implementation. Do not silently return success for a
hook whose side effects are required by active station mode.

### 10.7 Vendor-source adaptation discipline

Use the same general technique that Zephyr applies to Espressif integrations:
retain vendor-facing structures and algorithms while replacing the operating
system boundary with a small native layer. For each imported source group:

1. Record its exact upstream commit and source hashes.
2. Maintain an explicit source and configuration manifest.
3. Keep ThreadX substitutions in focused patches or wrapper headers.
4. Avoid compatibility macros that pretend ThreadX is FreeRTOS across the whole
   application.
5. Compile with warnings enabled and fail on stale patches or ABI assertions.
6. Test the adapted service independently before linking the Wi-Fi archives.

The ThreadX FreeRTOS compatibility layer may be useful as a behavioral reference,
but it is not linked into the demonstration. Direct ThreadX implementations keep
object ownership, ISR legality, and priority behavior visible and reviewable.

## 11. SampleX PHY initialization and persistence

For the first milestone:

- Embed the normal ESP32-C6 PHY initialization data in the application.
- Disable Wi-Fi NVS persistence in the initialization configuration.
- Perform full PHY calibration at boot if required.
- Accept increased boot time.

If the vendor interface still calls NVS functions, initially provide a deliberate
in-memory implementation or correctly return "not found"/unsupported according to
the API contract. Do not persist credentials in an ad hoc format during bring-up.

After networking is stable, add a native flash-backed storage service for:

- PHY calibration data.
- Wi-Fi configuration values required by the vendor driver.
- Application credentials, if desired.

Reusing the IDF NVS on-flash format is optional. A smaller ThreadX-native store may
be preferable if interoperability with IDF firmware is not needed.

## 12. SampleX WPA supplicant strategy

The Espressif Wi-Fi stack depends on modified WPA supplicant sources in:

```text
components/wpa_supplicant/
```

Create an explicit source manifest for the minimum WPA2-Personal client path. Do
not import the complete IDF component definition, which enables broad Enterprise,
WPS, WPA3, AP, DPP, and roaming functionality.

The initial manifest should contain only what is required for:

- WPA/RSN state handling.
- WPA2-PSK key derivation and four-way handshake.
- Required IEEE 802.11 information-element parsing.
- Espressif station glue.
- The minimal supplicant event loop.
- Required hash, HMAC, AES, and PBKDF2 functions.

Use either the supplicant's internal crypto subset or a standalone Mbed TLS build.
NetX Secure may later motivate a shared crypto configuration, but that should not
delay basic WPA2, DHCP, and UDP operation.

## 13. SampleX NetX Duo port and Wi-Fi link driver

NetX Duo needs a small RISC-V/little-endian `nx_port.h` and a hardware link driver.
Configure NetX for only the protocols used by the demo to control code size.

### 13.1 Packet pool

Create at least separate control and receive/transmit considerations, with:

- Payload capacity for a full Ethernet frame plus NetX headroom.
- At least four-byte alignment.
- Internal SRAM placement initially.
- Sufficient packets to tolerate Wi-Fi RX bursts and TCP retransmission.
- Instrumentation for allocation failures and low-water marks.

Begin with roughly 1536-byte payloads plus required NetX and Ethernet header space.
Tune counts from measured traffic rather than hiding exhaustion with unbounded
allocation.

### 13.2 Required driver commands

Support at least:

- `NX_LINK_INITIALIZE`.
- `NX_LINK_ENABLE`.
- `NX_LINK_DISABLE`.
- `NX_LINK_PACKET_SEND`.
- `NX_LINK_ARP_SEND`.
- `NX_LINK_ARP_RESPONSE_SEND`.
- `NX_LINK_BROADCAST`.
- `NX_LINK_GET_STATUS`.

Add multicast commands when required by enabled NetX features.

### 13.3 Transmit path

Initial transmit flow:

1. Accept the NetX driver request.
2. Construct or preserve the Ethernet header required by the command.
3. Linearize chained `NX_PACKET` data into an internal Wi-Fi buffer.
4. Call the pinned Espressif Layer-2 transmit API.
5. Release the NetX packet according to the command contract.
6. Update detailed success, retry, drop, and allocation counters.

Use copying initially. Investigate zero-copy only after the ownership and completion
semantics of the vendor transmit path are proven.

### 13.4 Receive path

Register the Espressif Layer-2 receive callback and:

1. Validate the frame pointer and length.
2. Allocate an `NX_PACKET` from the receive pool.
3. Copy the complete Ethernet frame.
4. Release the Espressif RX buffer promptly.
5. Queue the NetX packet to a dedicated receive thread.
6. Parse EtherType and deliver to the appropriate NetX IP or ARP receive path.

Do not run substantial NetX processing in the vendor callback. A dedicated receive
thread provides clear ownership, prevents blocking Wi-Fi internals, and makes queue
overflow behavior observable.

### 13.5 Link-state manager

Maintain a native state machine:

```text
STOPPED
  -> STARTED
  -> SCANNING
  -> ASSOCIATING
  -> ASSOCIATED
  -> ADDRESSING
  -> ONLINE
```

On association:

- Mark the NetX interface link up.
- Start DHCP, or apply a configured static address during early testing.

On disconnection:

- Mark link down immediately.
- Stop or restart DHCP as appropriate.
- Invalidate stale address and DNS state.
- Schedule reconnection with bounded exponential backoff.

Application notifications run from the manager thread, never directly from the
vendor event callback.

## 14. Implementation phases and exit criteria

### Phase 0: Generic RV32 audit in ThreadX

Deliver:

- An audit of the generic RV32 GNU context, interrupt, timer, and stack-frame
  assumptions against the ESP32-C6 CPU.
- Generic RV32 fixes only where the problem applies beyond the ESP32-C6.
- Regression tests for every generic port change.

Exit criterion: the generic port continues to build and pass its existing QEMU and
other example tests, with new regression coverage for changed behavior.

Repository: ThreadX.

### Phase 1: Basic ESP32-C6 `example_build`

Deliver:

- `ports/risc-v32/gnu/example_build/esp32c6`.
- Standalone CMake/Ninja build instructions.
- ESP32-C6 startup, linker script, vectors, PLIC, console, and SYSTIMER files
  contained under the example directory.
- ELF, map file, application image, and flash instructions.
- ThreadX `tx_application_define` validation application.
- Context switching and preemption.
- 100 Hz kernel tick.
- Console, reset, GPIO, and a peripheral interrupt.

Exit criterion: sustained thread, timer, queue, mutex, and interrupt stress without
context corruption or timing stalls on an ESP32-C6 board. The example has no NetX
Duo, Wi-Fi, or SampleX application dependency.

Repository: ThreadX.

### Phase 2: SampleX ESP32-C6 skeleton

Deliver:

- `SampleX/Espressif/ESP32-C6` following the existing SampleX board layout.
- Updated SampleX ThreadX and NetX Duo submodule revisions.
- A pinned ESP-IDF acquisition mechanism.
- CMake/Ninja build, image generation, build/deploy scripts, README, and notice.
- Reuse of the basic ThreadX ESP32-C6 example support through `libs/threadx`.
- Pinned timer-source manifest and reproducible adaptation patch application.
- Shared SYSTIMER ownership with counter 1/alarm 0 for the ThreadX tick and
  counter 0/alarm 2 for the high-resolution timer.
- ThreadX-adapted IDF timer thread, event notification, interrupt backend,
  allocator, `esp_timer_get_time()`, and ETS compatibility API.

Exit criterion: the SampleX starter boots, runs ThreadX, and passes the same basic
kernel validation as the ThreadX example without carrying a fork of the generic
RV32 port. High-resolution one-shot, periodic, cancellation, rearm, and deferred
deletion tests pass concurrently with the 100 Hz ThreadX tick.

Repository: SampleX.

### Phase 3: SampleX Wi-Fi ABI closure

Deliver:

- Selected Wi-Fi/PHY archive manifest.
- Undefined-symbol classification report.
- Native OS adapter skeleton.
- Compile-time adapter version and layout checks.
- Verified `ETSTimer` size, field offsets, callback field, and timer wrapper
  signatures for the pinned Wi-Fi libraries.

Exit criterion: clean link with every required symbol assigned to a documented
implementation and no catch-all stubs.

Repository: SampleX.

### Phase 4: SampleX PHY and scanning

Deliver:

- Modem clock and RF/PHY initialization.
- Wi-Fi interrupts, threads, queues, mutexes, and timers.
- Station startup and access-point scan.
- SSID, channel, security, and RSSI reporting.

Exit criterion: repeated scans complete without deadlocks, leaks, timer loss, or
interrupt corruption. This is the primary feasibility gate.

Repository: SampleX.

### Phase 5: SampleX WPA2 association

Deliver:

- Minimal WPA2-Personal supplicant build.
- Association and disconnection handling.
- Native reconnect state machine.

Exit criterion: repeated successful association/reassociation cycles with stable
memory use and no leaked ThreadX objects.

Repository: SampleX.

### Phase 6: SampleX NetX Layer 2

Deliver:

- NetX Duo library and `nx_port.h`.
- ESP32-C6 Wi-Fi `NX_IP_DRIVER`.
- Static IPv4 configuration initially.
- ARP, ICMP echo, and bidirectional UDP.

Exit criterion: sustained ping and UDP traffic under deliberate packet loss and
reassociation.

Repository: SampleX.

### Phase 7: Complete SampleX network demo

Deliver:

- DHCP client.
- DNS client.
- TCP client and/or server.
- A representative application such as HTTP or MQTT.
- NetX Secure TLS if required by the demonstration.

Exit criterion: unattended operation across access-point restarts, DHCP renewal,
disconnects, and traffic stress.

Repository: SampleX.

### Phase 8: Refinement

Optional follow-on work:

- Flash-backed PHY calibration and configuration storage.
- WPA3/SAE.
- Wi-Fi power saving.
- Improved packet zero-copy paths.
- Watchdog integration.
- Crash dumps and ThreadX-aware debugger support.
- Secure boot and flash encryption.

Repository: primarily SampleX. Any issue discovered here should move into the
ThreadX repository only when it is demonstrated to be a generic RV32 port defect.

### Recommended change sequence

Keep reviews and repository histories focused by using separate changes:

1. ThreadX PR: generic RV32 corrections and regression tests, if any.
2. ThreadX PR: minimal ESP32-C6 `example_build` and hardware validation record.
3. SampleX PR: ESP32-C6 starter skeleton and pinned dependency setup.
4. SampleX PR: ThreadX-adapted IDF timer and shared SYSTIMER validation.
5. SampleX PR: PHY initialization and Wi-Fi scan with the native OS adapter.
6. SampleX PR: WPA2 station association.
7. SampleX PR: NetX Duo Layer-2 driver and static-IP traffic.
8. SampleX PR: DHCP, DNS, and the final application demo.

## 15. Testing strategy

Testing follows the repository boundary. ThreadX tests prove the generic RV32 port
and the minimal C6 example. SampleX tests prove the reusable board integration,
ESP-IDF archive adaptation, Wi-Fi behavior, and NetX Duo data path.

### 15.1 ThreadX generic-port and example tests

Run the existing generic RV32 builds and tests after every generic port change.
Add a regression test for each generic defect corrected. On the ESP32-C6 target,
the `example_build/esp32c6` validation should exercise:

- Thread create/delete and rapid preemption.
- Nested critical sections.
- ThreadX tick accuracy and wrap behavior.
- Simultaneous SYSTIMER and GPIO interrupts.
- ISR-to-thread semaphore/queue wakeups.
- Long-running stack-watermark checks.

These tests must not depend on SampleX, ESP-IDF Wi-Fi libraries, NetX Duo, or a
network connection.

### 15.2 SampleX host tests

Test RTOS-independent logic on the host where practical:

- Priority conversion.
- Timeout and tick conversion.
- Queue slot management and ordering.
- Recursive mutex ownership rules.
- Timer expiration ordering and periodic catch-up behavior using a host hardware
  backend or extracted timer-list harness.
- ETS timer initialization, callback, arm/disarm, and deferred-delete semantics.
- Wi-Fi event-state transitions.
- NetX packet-chain linearization.
- Ethernet frame validation and EtherType routing.
- Reconnect backoff.

### 15.3 SampleX on-target platform tests

Exercise:

- SampleX bootstrap using the pinned ThreadX revision.
- High-resolution time accuracy, monotonicity, long-range behavior, and conversion
  at the C6 SYSTIMER's 16 MHz rate.
- One-shot and periodic expiration, simultaneous expiration ordering, minimum
  period behavior, cancellation, rearm, and deferred deletion.
- Timer callbacks that stop, rearm, or delete their own timers.
- Coalesced ISR wakeups when multiple alarms become due before the timer thread
  runs.
- Concurrent ThreadX counter-1/alarm-0 tick and counter-0/alarm-2 timer traffic.
- Verification that neither timer initialization path resets the other consumer.
- Wi-Fi interrupt delivery alongside the ThreadX tick.
- SampleX service-thread wakeups and event delivery.
- Allocator exhaustion and recovery behavior.
- Repeated initialization and shutdown where the vendor API supports it.

### 15.4 SampleX Wi-Fi adapter contract tests

Before association, test every active OS adapter service independently:

- Queue full, empty, timeout, front, and ISR cases.
- Semaphore finite/infinite waits and ISR give.
- Mutex priority inheritance and recursion.
- Thread lifecycle and per-thread objects.
- Timer arm, rearm, cancel, and callback races.
- Timer creation/allocation failure and asynchronous deletion reclamation.
- Long-running callbacks and timer-thread priority inversion scenarios.
- Event payload lifetime.
- Internal-memory allocation failures.

### 15.5 SampleX network tests

Test:

- Scan and association repetition.
- Incorrect password and unavailable AP behavior.
- AP restart and channel changes.
- DHCP lease acquisition and renewal.
- RX burst exhaustion.
- TX allocation failure.
- UDP loss and reordering.
- TCP retransmission and reconnect.
- Long-duration traffic with memory and packet-pool telemetry.

## 16. Diagnostics and observability

Keep basic interrupt, exception, tick, and stack diagnostics in the ThreadX C6
example. Provide the fuller SampleX integration counters from the beginning for:

- Interrupts per source and unhandled interrupts.
- Kernel and high-resolution timer interrupts.
- Timer ISR-to-thread latency, event coalescing, active timer count, late
  expirations, callback duration, and allocation failures.
- Wi-Fi OS adapter object creation/deletion.
- Wi-Fi event queue overflows.
- RX frames, drops, allocation failures, and queue high-water mark.
- TX frames, errors, copies, and completion outcomes.
- NetX packet pool availability and low-water mark.
- Association attempts, failures, disconnect reasons, and reconnects.
- Heap current use, peak use, and failed allocations.

Early failures may occur with caches or normal logging unavailable, so retain a
minimal IRAM-safe diagnostic path and a reset-reason record.

## 17. Primary risks and mitigations

### Vendor ABI drift

Risk: private Wi-Fi APIs, OS adapter fields, and archive dependencies change.

Mitigation: pin ESP-IDF, check adapter version/layout at compile time, checksum
archives, and regenerate the undefined-symbol report in CI.

### Queue and timer semantic differences

Risk: subtle behavioral differences cause intermittent driver failures.

Mitigation: implement direct native adapters, test their contracts separately, and
instrument all timeout and overflow paths.

### SYSTIMER ownership and initialization ordering

Risk: the imported timer's early initialization resets the whole SYSTIMER after
the ThreadX tick has been configured, stopping the kernel tick or corrupting alarm
state.

Mitigation: make SampleX the sole peripheral owner, perform one early reset, assign
fixed nonoverlapping counters and alarms, and test both interrupt streams under
stress. Remove peripheral reset and IDF RCC acquisition from the adapted backend.

### Imported timer-source drift

Risk: an ESP-IDF update changes timer internals, `ETSTimer` layout, configuration
macros, or assumptions made by the Wi-Fi blobs.

Mitigation: pin the timer sources to the Wi-Fi archive revision, hash imported
files, require clean patch application, assert ABI layout at compile time, retain
license notices, and rerun timer contract tests before accepting an update.

### Interrupt-context incompatibility

Risk: mixing ESP-IDF vector assumptions with ThreadX frames corrupts context.

Mitigation: own the trap entry from the start and design it around the ThreadX RV32
port contract. Do not link ESP-IDF's FreeRTOS vector hooks unchanged.

### Private Layer-2 APIs

Risk: internal transmit/RX callback APIs are not stable public interfaces.

Mitigation: isolate them behind a small versioned adapter and keep NetX code unaware
of Espressif-private types.

### PHY/platform dependency expansion

Risk: enabling an apparently small feature pulls in PM, coexistence, NVS, or other
IDF frameworks.

Mitigation: begin with station-only, no-power-save configuration and classify the
binary symbol closure before implementing platform hooks.

## 18. Definition of success

There are two independently reviewable completion points.

### 18.1 ThreadX repository completion

The ThreadX work is successful when the generic RV32 port retains its existing
behavior and `ports/risc-v32/gnu/example_build/esp32c6`:

1. Builds without SampleX, NetX Duo, or Wi-Fi sources.
2. Boots through the stock Espressif bootloader and enters ThreadX.
3. Demonstrates stable context switching, preemption, synchronization objects,
   the ThreadX tick, and at least one non-timer peripheral interrupt.
4. Keeps all ESP32-C6-specific startup, linker, vector, interrupt-controller, and
   example code within the ESP32-C6 example directory unless a change is proven
   to be a generic RV32 port correction.
5. Documents the required toolchain, image generation, flashing, and supported
   ESP32-C6 board configuration.

This completion point is an architecture-port example, not a supported full BSP,
ESP-IDF integration layer, or networking sample.

### 18.2 SampleX demonstration completion

The SampleX demonstration is successful when one ESP32-C6, with no external
peripheral hardware:

1. Boots through the stock Espressif bootloader into ThreadX.
2. Runs all application and vendor-created threads under ThreadX.
3. Initializes the integrated PHY and scans for access points.
4. Associates with a WPA2-Personal network.
5. Exchanges raw Ethernet frames through a native NetX Duo driver.
6. Acquires an IPv4 address with NetX DHCP.
7. Resolves DNS and completes a TCP or UDP application transaction.
8. Recovers from access-point loss without rebooting.
9. Operates without FreeRTOS, lwIP, `esp_netif`, `esp_event`, or the stock
   ESP-IDF `esp_timer` runtime/component. Its pinned timer sources run entirely on
   native ThreadX and SampleX platform services.

This design keeps the hardware self-contained and establishes a clear ownership
boundary: ThreadX contains the generic RV32 port and its minimal C6 adaptation
example; SampleX contains the complete board/demo integration; Espressif supplies
the radio implementation and low-level silicon knowledge; ThreadX supplies all
operating-system services; and NetX Duo supplies all IP networking.
