#!/bin/bash

CTEST_PARALLEL_LEVEL=4 $(dirname `realpath $0`)/../test/smp/cmake/run.sh test all
