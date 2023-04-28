@ECHO OFF
ECHO INFO: Cleaning %CD%...
DEL *.o
DEL tx.a
DEL sample_threadx.map
DEL sample_threadx.out
ECHO INFO: Cleaning completed successfully!
