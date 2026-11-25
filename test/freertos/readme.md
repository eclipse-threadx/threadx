# FreeRTOS compatibility layer regression tests

Regression tests for `utility/rtos_compatibility_layers/FreeRTOS/tx_freertos.c`,
built against the real ThreadX Linux port.

## Running them

```bash
./scripts/build_freertos.sh
./scripts/test_freertos.sh
```

Or directly, which is the same thing:

```bash
test/freertos/cmake/run.sh build all
test/freertos/cmake/run.sh test all
```

CI runs both scripts through `.github/workflows/regression_test.yml`.

## What they cover

Every function in the layer that creates an object takes one or two byte pool
allocations for its bookkeeping and then creates one or more ThreadX kernel
objects. When one of those kernel objects cannot be created, the function
returns `NULL`, or `pdFAIL` for `xTaskCreate()`, and the caller is left with no
handle and therefore no way to call the matching delete function. Anything the
layer fails to release on the way out is lost until the system restarts.

That makes these error paths invisible from the outside: a leaking version and
a correct version return exactly the same thing to the caller. The suite
therefore counts the ThreadX primitives the layer reaches for, and checks that
each error path gives back precisely what it took.

| Test | Covers |
| --- | --- |
| `txfr_queue_create_test` | `xQueueCreate`, `xQueueCreateStatic`, `vQueueDelete` |
| `txfr_task_create_test` | `xTaskCreate`, `xTaskCreateStatic` |
| `txfr_sync_create_test` | semaphores, mutexes, event groups and timers |

## How the fault injection works

A test asks the harness to fail a chosen kernel creation call, then reads back
how many allocations, releases, object creations and object deletions the layer
performed. The interception is done with the linker's `--wrap` option, so
`tx_freertos.c` is compiled exactly as it ships, with no test hooks in it.

Two things are worth knowing before adding tests:

- `tx_api.h` maps the public API onto the error checking entry points, so the
  symbols that exist at link time are the `_txe_` variants, and those are what
  the wrap list in `cmake/regression/CMakeLists.txt` names. A `--wrap` for a
  name that does not resolve is silently ignored, so a typo there produces a
  test that quietly never injects anything.
- `txfr_malloc()` and `txfr_free()` cannot be wrapped, because they are defined
  in `tx_freertos.c` and called from within it, so the compiler resolves those
  calls internally. The byte pool counts stand in for them.

Because `--wrap` is a GNU ld and lld feature with no MSVC equivalent, this
suite is Linux only. The CMake configuration stops with a clear message rather
than failing later with confusing link errors.

## Fixtures

`fixtures/FreeRTOSConfig.h` configures the layer for the tests. Two of its
settings are not arbitrary:

- `configASSERT()` and `TX_FREERTOS_ASSERT_FAIL()` are empty, since the tests
  drive error paths deliberately and neither may halt the run.
- `portDISABLE_INTERRUPTS()` and `portENABLE_INTERRUPTS()` are defined up front.
  `FreeRTOS.h` picks those by compiler rather than by target, so a GNU build
  otherwise resolves them to the bare metal `__disable_interrupts()` intrinsic,
  which does not exist when the layer is hosted on Linux.

`fixtures/tx_user.h` supplies `TX_THREAD_USER_EXTENSION`, which the layer
requires, as documented in the layer's own `readme.md`.

## Why the build is 32 bit

The Linux port defines `ULONG` as `unsigned int` on x86_64, while the layer
passes pointers through `ULONG` arguments, such as the task argument and the
timer identifier. A 64 bit build truncates those pointers, which the compiler
reports as `-Wpointer-to-int-cast` and which crashes the timer callback
wrapper. The ThreadX and SMP suites build 32 bit for their own reasons; this
suite has to.
