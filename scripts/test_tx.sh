#!/bin/bash

CTEST_PARALLEL_LEVEL=4 $(dirname `realpath $0`)/../test/tx/cmake/run.sh test all
