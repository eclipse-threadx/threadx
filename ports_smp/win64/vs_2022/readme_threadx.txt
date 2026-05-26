This port provides a Windows-hosted SMP simulation environment for ThreadX.

The implementation models virtual ThreadX cores on top of Win64 host threads.
It is intended for validation and regression testing only. It is not a
production runtime port for deploying ThreadX applications on Windows.

Key characteristics:
- ThreadX cores are virtual scheduler-owned mappings, not Windows CPU numbers.
- One Windows host thread is created for each ThreadX thread.
- A dedicated scheduler thread and a dedicated timer/ISR thread coordinate the
  simulation.
- SuspendThread and ResumeThread are used only to emulate asynchronous
  preemption in the simulator.
