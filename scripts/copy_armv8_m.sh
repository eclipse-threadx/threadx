#!/bin/bash

# There are two files tx_port.h and tx_secure_interface.h that cover three architectures: M33/M55/M85 and three tools: ac6/gnu/iar.
# These files are in threadx/ports/armv8-m/inc.

set -e

ports_folder="ports"
source_folder="ports_arch/ARMv8-M/threadx"
target_mcu="cortex_m33 cortex_m55 cortex_m85"
source_inc_folder="$source_folder/inc"
target_inc_mcu=$target_mcu
target_inc_ide="ac6 gnu iar"
target_inc_folder="inc"
source_src_ide="ac6 gnu iar"
target_src_mcu=$target_mcu
src_folder="src"
readme="readme_threadx.txt"
cmake="CMakeLists.txt"
source_string="Cortex-Mxx"
target_string_pre="Cortex-M"

cd $(dirname `realpath $0`)/..

# Copy port files
for mcu in $target_mcu;
do
    for ide in $target_inc_ide;
    do
        # Copy files
        target=$ports_folder/$mcu/$ide/$target_inc_folder
        [ -d "$target" ] || mkdir -p $target
        echo "$source_inc_folder -> $target"
        rm -rf $target
        cp -rf $source_inc_folder $target

        # String replacement
        mcu_string="$target_string_pre${mcu: -2}"
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
        sed -i "s/$source_string/$target_string_pre${mcu: -2}/g" $target/*

        # Copy readme files
        src=$source_folder/$ide/$readme
        target=$ports_folder/$mcu/$ide/$readme
        #[ -f "$target" ] || continue
        echo "$src -> $target"
        rm -f $target
        cp -f $src $target

        # String replacement
        sed -i "s/$source_string/$target_string_pre${mcu: -2}/g" $target
    done
done

# Copy CMakeLists files
for ide in $source_src_ide;
do
    for mcu in $target_src_mcu;
    do
        # Copy CMakeList files ac6
        src=$source_folder/ac6/$cmake
        target=$ports_folder/$mcu/ac6/$cmake
        #[ -f "$target" ] || continue
        echo "$src -> $target"
        rm -f $target
        cp -f $src $target

        # Copy CMakeList files gnu
        src=$source_folder/gnu/$cmake
        target=$ports_folder/$mcu/gnu/$cmake
        #[ -f "$target" ] || continue
        echo "$src -> $target"
        rm -f $target
        cp -f $src $target
    done
done