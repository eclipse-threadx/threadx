#!/bin/bash

# There is only one tx_port.h file that covers three architectures: M3/M4/M7 and four tools: ac5/ac6/gnu/iar.
# This file is in threadx/ports/armv7-m/inc. We are going to ignore GHS for now, but I’d like to get GHS unified as well.

set -e

ports_folder="ports"
source_folder="ports_arch/ARMv7-M/threadx"
target_mcu="cortex_m3 cortex_m4 cortex_m7"
source_inc_folder="$source_folder/inc"
target_inc_mcu=$target_mcu
target_inc_ide="ac5 ac6 gnu iar keil"
target_inc_folder="inc"
source_src_ide="ac5 ac6 gnu iar"
target_src_mcu=$target_mcu
src_folder="src"
readme="readme_threadx.txt"
source_string="Cortex-Mx"
target_string_pre="Cortex-M"

cd $(dirname `realpath $0`)/..

# Copy port files
for mcu in $target_mcu;
do
    for ide in $target_inc_ide;
    do
        # Copy files
        target=$ports_folder/$mcu/$ide/$target_inc_folder
        [ -d "$target" ] || continue
        echo "$source_inc_folder -> $target"
        rm -rf $target
        cp -rf $source_inc_folder $target

        # String replacement
        mcu_string="$target_string_pre${mcu: -1}"
        if [ "$ide" == "keil" ];
        then
            ide_string="Keil"
            mcu_ide_string="$mcu_string\/$ide_string"
        else
            ide_string="${ide^^}"
            mcu_ide_string="$mcu_string\/$ide_string "
        fi
        sed -i "s/ $source_string    /$mcu_ide_string/g" $target/*
        sed -i "s/ThreadX $source_string /ThreadX $mcu_string\/$ide_string /g" $target/*
        sed -i "s/$source_string/$mcu_string/g" $target/*
    done
done
echo ""

# Copy source files
for ide in $source_src_ide;
do
    for mcu in $target_src_mcu;
    do
        # Copy source files
        src=$source_folder/$ide/$src_folder
        target=$ports_folder/$mcu/$ide/$src_folder
        #[ -d "$target" ] || continue
        echo "$src -> $target"
        rm -rf $target
        cp -rf $src $target

        # String replacement
        sed -i "s/$source_string/$target_string_pre${mcu: -1}/g" $target/*

        # Copy readme files
        src=$source_folder/$ide/$readme
        target=$ports_folder/$mcu/$ide/$readme
        #[ -f "$target" ] || continue
        echo "$src -> $target"
        rm -f $target
        cp -f $src $target

        # String replacement
        sed -i "s/$source_string/$target_string_pre${mcu: -1}/g" $target
    done
done