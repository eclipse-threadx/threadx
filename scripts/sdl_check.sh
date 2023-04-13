# !/bin/bash
dir_list="common common_smp common_modules ports ports_module ports_smp samples"
exclude_list="-path TX"
file_list=$(find $dir_list \( $exclude_list \) -prune -o -type f -name '*.[ch]' -print)
cd $(dirname `realpath $0`)/..
echo "Checking for unexpected usage of memcpy..."
echo ""
echo "Excluding:"
echo $exclude_list | grep -P "[^\s]*/[^\s]*" -o
echo ""
echo "Result:"
grep -i "memcpy(" -i $file_list -n | grep -i "use case of .* is verified" -v
if [ "$?" -eq "1" ];
then
    echo "CLEAN"
    exit 0
else
    exit 1
fi