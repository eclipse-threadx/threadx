#!/bin/bash

# There is only one tx_port.h file that covers three architectures: M3/M4/M7 and four tools: ac5/ac6/gnu/iar.
# This file is in threadx/ports_module/armv7-m/inc. We are going to ignore GHS.

set -e

ports_folder="ports_arch/ARMv7-M/threadx"
ports_file_list="tx_thread_context_restore tx_thread_context_save tx_thread_interrupt_control tx_thread_interrupt_disable tx_thread_interrupt_restore tx_thread_stack_build tx_thread_system_return tx_timer_interrupt"
ports_module_folder="ports_module"
source_folder="ports_arch/ARMv7-M/threadx_modules"
target_mcu="cortex_m3 cortex_m4 cortex_m7"
source_inc_folder="$source_folder/inc"
target_inc_folder="inc"
target_ide="ac5 ac6 gnu iar"
source_common_folder="common/module_manager/src"
target_common_folder="module_manager"
source_string="Cortex-Mx"
target_string_pre="Cortex-M"


cd $(dirname `realpath $0`)/..

# Copy IDE specific files
for mcu in $target_mcu;
do
    for ide in $target_ide;
    do
        for d in $(ls $source_folder/$ide);
        do
            # Copy specific files
            source=$source_folder/$ide/$d
            target=$ports_module_folder/$mcu/$ide/$d
            echo "$source -> $target"
            rm -rf $target
            cp -rf $source $target

            # String replacement
            find $target -type f -exec sed -i "s/$source_string/$target_string_pre${mcu: -1}/g" {} \;
        done

        # copy common inc directory containing unified tx_port.h
        target=$ports_module_folder/$mcu/$ide/$target_inc_folder
        echo "$source_inc_folder -> $target"
        cp -rf $source_inc_folder/* $target
        find $target -type f -exec sed -i "s/$source_string/$target_string_pre${mcu: -1}/g" {} \;
        
        # Copy common files
        source=$source_folder/$source_common_folder;
        target=$ports_module_folder/$mcu/$ide/$target_common_folder
        echo "$source -> $target"
        cp -rf $source $target

        source=$ports_folder/$ide/src;
        echo "$source -> $target"
        for f in $ports_file_list;
        do
            cp $source/$f.* $target/src
        done

        # String replacement
        find $target -type f -exec sed -i "s/$source_string/$target_string_pre${mcu: -1}/g" {} \;
    done
done