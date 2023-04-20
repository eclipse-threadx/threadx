#!/bin/bash
# run this script to generate cmsis-pack in this folder

set -o pipefail

# copy gen_pack.sh, pack description file .pdsc and examples to source file folder
cp -r ./examples ../

cp ./gen_pack.sh ../

cp ./*.pdsc ../

cd ../

# run gen_pack.sh
./gen_pack.sh

# remove copied gen_pack.sh, .pdsc file and examples
rm ./gen_pack.sh

rm *.pdsc

rm -rf ./examples

cd cmsis_pack
