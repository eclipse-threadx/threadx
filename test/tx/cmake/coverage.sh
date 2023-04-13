#!/bin/bash

set -e

cd $(dirname $0)
mkdir -p coverage_report/$1
gcovr --object-directory=build/$1/threadx/CMakeFiles/threadx.dir/common/src -r build/$1 -f ../../../common/src --xml-pretty --output coverage_report/$1.xml
gcovr --object-directory=build/$1/threadx/CMakeFiles/threadx.dir/common/src -r build/$1 -f ../../../common/src --html --html-details --output coverage_report/$1/index.html
