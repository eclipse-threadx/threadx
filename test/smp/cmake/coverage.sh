#!/bin/bash

set -e

cd $(dirname $0)
threadx_smp=$(realpath ../../../common_smp/src)
mkdir -p coverage_report/$1
gcovr --object-directory=build/$1/threadx_smp/CMakeFiles/threadx_smp.dir/$threadx_smp -r build/$1 -f ../../../common_smp/src --xml-pretty --output coverage_report/$1.xml
gcovr --object-directory=build/$1/threadx_smp/CMakeFiles/threadx_smp.dir/$threadx_smp -r build/$1 -f ../../../common_smp/src --html --html-details --output coverage_report/$1/index.html
