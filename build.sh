#!/bin/sh
TOOLCHAIN_PATH=/home/eric/imx8m/gcc-linaro-7.3.1_aarch64/bin

export ARCH=arm64

###
export PATH=$TOOLCHAIN_PATH:$PATH
###
export CROSS_COMPILE=aarch64-linux-gnu-

make distclean 

make em_sbc_imx8m_defconfig

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
cp -f arch/arm/dts/em-sbc-imx8m.dtb tools/imx-boot/iMX8M/

cd tools/imx-boot/
make clean
make SOC=iMX8M flash_ddr4_em
cd ../../

cp -f ./tools/imx-boot/iMX8M/flash.bin  u-boot.imx

