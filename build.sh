#!/bin/bash

###
# TOOLCHAIN_PATH=/home/nick/toolchain/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin
# export PATH=$TOOLCHAIN_PATH:$PATH
###
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-


# check required applications are installed
command -v ${CROSS_COMPILE}gcc >/dev/null 2>&1
if [ $? -ne 0 ]; then
echo "ERROR: ${CROSS_COMPILE}gcc not found,"
echo ""
echo "please install the toolchain first "
echo "and export the enviroment like \"export PATH=\$PATH:your_toolchain_path\" "
echo ""
exit
fi

#------------------------------------------------------------------------------
help() {
bn=`basename $0`
cat << EOF

usage :  $bn <option>

options:
  -h		display this help and exit
  -mx8m  	build u-boot.imx for the EM-MC-SBC-IMX8M baord
  -mini 	build u-boot.imx for the MaaXBoard mini baord

Example:
	./$bn -mx8m
	./$bn -mini

EOF
}

SOC_TYPE="mx8m"

[ $# -eq 0 ] && help && exit
while [ $# -gt 0 ]; do
    case $1 in
        -h) help; exit ;;
        -mx8m) echo ${SOC_TYPE};;
        -mini) SOC_TYPE="mx8m_mini"; echo ${SOC_TYPE};;

        *)  echo "-- invalid option -- "; help; exit;;
    esac
    shift
done

make distclean 

if [ "${SOC_TYPE}" == "mx8m_mini" ] ; then
    echo "maaxboard_mini_defconfig"
    make maaxboard_mini_defconfig
else
    echo "maaxboard_defconfig"
    make maaxboard_defconfig
fi

make -j4 2>&1 | tee  ./build_log.txt

echo
MCOMPLETE=`grep -E 'failed|Error' ./build_log.txt | grep -o 'make' `
if [ -z "$MCOMPLETE" ];then
    echo "create u-boot.imx ... "
else
    echo "make u-boot Error !!!"
    exit 2
fi
echo

cp -f spl/u-boot-spl.bin tools/imx-boot/iMX8M/
cp -f u-boot-nodtb.bin tools/imx-boot/iMX8M/
if [ "${SOC_TYPE}" == "mx8m_mini" ] ; then
    cp -f arch/arm/dts/maaxboard-mini.dtb  tools/imx-boot/iMX8M/
else
    cp -f arch/arm/dts/maaxboard.dtb tools/imx-boot/iMX8M/
fi

cd tools/imx-boot/
make clean
[ ${SOC_TYPE} == "mx8m" ] && make SOC=iMX8M flash_ddr4_em
[ "${SOC_TYPE}" == "mx8m_mini" ] && make SOC=iMX8MM flash_ddr4_em
cd ../../

cp -f ./tools/imx-boot/iMX8M/flash.bin  u-boot.imx
exit
