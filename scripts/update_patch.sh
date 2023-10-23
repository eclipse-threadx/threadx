#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 \"<version>\" \"<date>\""
    exit 1
fi

cd $(dirname `realpath $0`)/..
current_version="6\\.x"
source_dirs="common common_modules common_smp ports ports_module ports_smp ports_arch utility"
file_list=$(find $source_dirs -type f \( -name "*.[chsS]" -o -name "*.arm" -o -name "*.src" -o -name "*readme*" -o -name "*.mip" \)  -print)

sed -i "s/$current_version  /$1/g" $file_list
sed -i "s/$current_version/$1/g" $file_list
sed -i "s/xx-xx-xxxx/$2/g" $file_list

# Update version in port files
source_dirs="ports ports_module ports_smp ports_arch"
file_list=$(find $source_dirs -type f -name "*.h" -print)
sed -i "/\"Copyright/,/[0-9]\.[0-9]/s/[0-9]\.[0-9\.]\+/$1/" $file_list
