#
#!/bin/bash
#
# This is a help script to build imx-boot[flash.bin] for maaxboard
#
# Created by ahnniu in Dec 2020
#


typeset -A config
config=(
    [board]="maaxboard"
    [tag]="rel_imx_5.4.24_2.1.0"
    [firmware]="firmware-imx-8.8.bin"
    [cc]="aarch64-poky-linux-"
    [ddr]="ddr4_imem_1d_201810.bin ddr4_dmem_1d_201810.bin ddr4_imem_2d_201810.bin ddr4_dmem_2d_201810.bin"
    [ddr_version]="_201810"
    [uboot_defconfig]="maaxboard_defconfig"
    [uboot_dtb]="maaxboard.dtb"
    [mkimage_ddr_type]="ddr4"
    [mkimage_board_type]="val"
    [seek]="33"
)

typeset -A config_mini
config_mini=(
    [board]="maaxboard-mini"
    [uboot_defconfig]="maaxboard_mini_defconfig"
    [uboot_dtb]="maaxboard-mini.dtb"
    [mkimage_ddr_type]="ddr4"
    [mkimage_board_type]="evk"
    [seek]="33"
)

typeset -A config_nano
config_nano=(
    [board]="maaxboard-nano"
    [uboot_defconfig]="maaxboard_nano_defconfig"
    [uboot_dtb]="maaxboard-nano-mipi.dtb"
    [mkimage_ddr_type]="ddr4"
    [mkimage_board_type]="evk"
    [seek]="32"
)

IMX_FIRMWARE_URL="https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/${config[firmware]}"
IMX_GIT_REMOTE=https://source.codeaurora.org/external/imx
IMX_MKIMAGE_NAME=imx-mkimage
IMX_ATF_NAME=imx-atf
IMX_MKIMAGE_REMOTE_REPO=$IMX_GIT_REMOTE/$IMX_MKIMAGE_NAME
IMX_ATF_REMOTE_REPO=$IMX_GIT_REMOTE/$IMX_ATF_NAME

UBOOT_PATH=$(dirname $(realpath $0))
IMX_BOOT_PATH=$(realpath $UBOOT_PATH/../imx-boot)
IMX_MKIMAGE_PATH=$(realpath $IMX_BOOT_PATH/$IMX_MKIMAGE_NAME)
IMX_ATF_PATH=$(realpath $IMX_BOOT_PATH/$IMX_ATF_NAME)
IMX_FIRMWARE_D_PATH="$IMX_BOOT_PATH/${config[firmware]%.*}"

PLATFORM="imx8mq"
SOC_TARGET="iMX8M"

mkdir -p $IMX_BOOT_PATH

function do_set_board_env() {
    local board="$1"

    case "$board" in
    "maaxboard" )
        PLATFORM="imx8mq"
        SOC_TARGET="iMX8M"
        ;;
    "maaxboard-mini" )
        PLATFORM="imx8mm"
        SOC_TARGET="iMX8MM"
        config[board]="${config_mini[board]}"
        config[uboot_defconfig]="${config_mini[uboot_defconfig]}"
        config[uboot_dtb]="${config_mini[uboot_dtb]}"
        config[mkimage_ddr_type]="${config_mini[mkimage_ddr_type]}"
        config[mkimage_board_type]="${config_mini[mkimage_board_type]}"
        config[seek]="${config_mini[seek]}"
        ;;
    "maaxboard-nano" )
        PLATFORM="imx8mn"
        SOC_TARGET="iMX8MN"

        config[board]="${config_nano[board]}"
        config[uboot_defconfig]="${config_nano[uboot_defconfig]}"
        config[uboot_dtb]="${config_nano[uboot_dtb]}"
        config[mkimage_ddr_type]="${config_nano[mkimage_ddr_type]}"
        config[mkimage_board_type]="${config_nano[mkimage_board_type]}"
        config[seek]="${config_nano[seek]}"
        ;;
    esac

    printf "board_env: set env according to board[$board] ... [OK]\n"
}

function do_repo_clone() {
    local remote=$1
    local local_path=$2
    local ret=0

    printf "repo: clone [$remote] to [$local_path] ... "
    proxychains4 git clone "$remote" "$local_path" > /dev/null 2>&1
    ret=$?
    if [ $ret -eq 0 ]; then
        printf "[OK]\n"
    else
        printf "[FAILED]\n"
    fi

    return $ret
}

function do_repo_checkout() {
    local local_path=$1
    local repo_name=$(basename $local_path)
    local new_branch=
    local ret=0

    if ! [ -d "$local_path" ]; then
        printf "repo: checkout: $local_path do not exist ... [FAILED]\n"
        return 1
    fi

    pushd "$local_path" > /dev/null 2>&1
    new_branch="${config[board]}-${config[tag]}"
    printf "repo[$repo_name]: checkout tag:[${config[tag]}] to new branch:[$new_branch] ... "
    git checkout -b "$new_branch" "${config[tag]}" > /dev/null 2>&1

    ret=$?
    if [ $ret -eq 0 ]; then
        printf "[OK]\n"
    else
        printf "[FAILED]\n"
    fi

    popd > /dev/null 2>&1
    return $ret
}

function do_repo() {
    local remote=$1
    local local_path=$2

    if [ -d "$local_path" ]; then
        printf "repo: $local_path exist, do not need to clone again ...\n"
        return 0
    fi

    if do_repo_clone $remote $local_path; then
        do_repo_checkout $local_path
    fi

    return $?
}

function do_firmware_download() {
    local dir="$IMX_BOOT_PATH"
    local filename="$dir/${config[firmware]}"
    local firmware_d_path="$dir/${config[firmware]%.*}"
    local ret=0

    if [ -e "$filename" ]; then
        printf "firmware: ${config[firmware]} has already downloaded to [$filename]\n"
    else
        printf "firmware: download from [$IMX_FIRMWARE_URL] to [$filename] ... "
        wget -O "$filename" $IMX_FIRMWARE_URL > /dev/null 2>&1
        ret=$?
        if [ $ret -eq 0 ]; then
            printf "[OK]\n"
        else
            printf "[FAILED]\n"
        fi
    fi

    pushd $dir > /dev/null 2>&1

    if [ -d $IMX_FIRMWARE_D_PATH ]; then
        printf "firmware: $(basename $filename) has already decompressed to [$IMX_FIRMWARE_D_PATH]\n"
    else
        printf "firmware: decompress to [$IMX_FIRMWARE_D_PATH]\n"
        sh "${config[firmware]}" --auto-accept --force  > /dev/null 2>&1
    fi

    popd > /dev/null 2>&1
}

function do_mkimage_cp_firmware() {
    local src="$IMX_FIRMWARE_D_PATH"
    local dst="$IMX_MKIMAGE_PATH/$SOC_TARGET"

    for ddr_firmware in ${config[ddr]}; do
        printf "firmware: cp ddr: [$ddr_firmware] to [$dst] ... "
        cp "$src/firmware/ddr/synopsys/${ddr_firmware}" "${dst}/"
        printf "[OK]\n"
    done

    printf "firmware: cp: hdmi: [signed_dp_imx8m.bin] to [$dst] ... "
    cp "$src/firmware/hdmi/cadence/signed_dp_imx8m.bin" "${dst}/"
    printf "[OK]\n"

    printf "firmware: cp: hdmi: [signed_hdmi_imx8m.bin] to [$dst] ... "
    cp "$src/firmware/hdmi/cadence/signed_hdmi_imx8m.bin" "${dst}/"
    printf "[OK]\n"
}

function do_uboot_make() {
    local dir="$UBOOT_PATH"

    pushd $dir > /dev/null 2>&1

    printf "uboot: defconfig: [${config[uboot_defconfig]}] ... "
    make "${config[uboot_defconfig]}" > /dev/null 2>&1
    printf "[OK]\n"
    printf "uboot: make ... "
    make -j8 > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        printf "[OK]\n"
    else
        printf "[FAILED]\n"
    fi

    popd > /dev/null 2>&1
}

function do_uboot_clean() {
    local dir="$UBOOT_PATH"

    pushd $dir > /dev/null 2>&1

    printf "uboot: distclean ... "
    make distclean > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        printf "[OK]\n"
    else
        printf "[FAILED]\n"
    fi

    popd > /dev/null 2>&1
}

function do_mkimage_cp_uboot() {
    local src="$UBOOT_PATH"
    local dst="$IMX_MKIMAGE_PATH/$SOC_TARGET"
    local dtb="${config[uboot_dtb]}"
    local dtb_dst="$dst/$PLATFORM-${config[mkimage_ddr_type]}-${config[mkimage_board_type]}.dtb"
    local spl_bin_dst="$dst/u-boot-spl-${config[mkimage_ddr_type]}.bin"

    cp "$src/arch/arm/dts/$dtb" "$dtb_dst"
    printf "uboot: cp: dtb: [$dtb] to [$dtb_dst] ... [OK]\n"

    cp "$src/spl/u-boot-spl.bin" "$spl_bin_dst"
    printf "uboot: cp: bin: [spl/u-boot-spl.bin] to [$spl_bin_dst] ... [OK]\n"

    cp "$src/u-boot-nodtb.bin" "$dst/"
    printf "uboot: cp: bin: [u-boot-nodtb.bin] to [$dst] ... [OK]\n"

    cp "$src/u-boot.bin" "$dst/"
    printf "uboot: cp: bin: [u-boot.bin] to [$dst] ... [OK]\n"

    cp "$src/tools/mkimage" "$dst/mkimage_uboot"
    printf "uboot: cp: tools: [mkimage] to [$dst/mkimage_uboot] ... [OK]\n"
}

function do_atf_make() {
    local dir="$IMX_ATF_PATH"

    pushd $dir > /dev/null 2>&1

    printf "imx-atf: make PLAT=${PLATFORM} ... "

    # Clear LDFLAGS to avoid the option -Wl recognize issue
    unset LDFLAGS

    make CROSS_COMPILE="${config[cc]}" PLAT="${PLATFORM}" -j8 > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        printf "[OK]\n"
    else
        printf "[FAILED]\n"
    fi

    popd > /dev/null 2>&1
}

function do_mkimage_cp_atf() {
    local src="$IMX_ATF_PATH/build/imx8mq/release/bl31.bin"
    local dst="$IMX_MKIMAGE_PATH/$SOC_TARGET"

    cp "$src" "$dst/"
    printf "imx-atf: cp: bin: [$(basename $src)] to [$dst] ... [OK]\n"
}

function do_mkimage_make() {
    local dir="$IMX_MKIMAGE_PATH"
    local dtb="${config[uboot_dtb]}"
    local target="flash_${config[mkimage_ddr_type]}_${config[mkimage_board_type]}"
    local flash_bin_filename="${dir}/${SOC_TARGET}/flash.bin"

    printf "imx-mkimage: make SOC=${SOC_TARGET} DDR_FW_VERSION=${config[ddr_version]} ${target} ... "
    pushd $dir > /dev/null 2>&1

    make clean > /dev/null 2>&1

    make SOC=${SOC_TARGET} DDR_FW_VERSION=${config[ddr_version]} ${target} > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        printf "[OK]\n"
    else
        printf "[FAILED]\n"
    fi
    popd > /dev/null 2>&1

    printf "imx-mkimage: flash.bin: generate at [ $flash_bin_filename ] ... [OK]\n"
    printf "dd-example: dd if=flash.bin of=/dev/sda bs=1k seek=${config[seek]} conv=fsync\n"
}

function usage() {
    echo "Usage:"
    echo ""
    echo "./$(basename $0)   clean        distclean uboot-imx"
    echo "./$(basename $0)   [board]      build flash.bin for the [board], support borads:"
    echo "                                   maaxboard / maaxboard-mini / maaxboard-nano"
    echo ""
    echo "Exmaple: Exec './$(basename $0) maaxboard' to build flash.bin for maaxboard"
    echo ""

    echo "Note:"
    echo "Before exec this script, you should setup the cross compiler env first."
    echo "Here we use the sdk for Yocto:"
    echo "source /opt/fsl-imx-wayland/5.4-zeus/environment-setup-aarch64-poky-linux"
    echo ""
}

if [[ $# -eq 0 ]] ; then
  usage
  exit 1
fi

BOARD_TO_BUILD="maaxboard"

case $1 in
"clean" )
    do_uboot_clean
    exit
    ;;
"maaxboard" | "maaxboard-mini" | "maaxboard-nano" )
    BOARD_TO_BUILD="$1"
    ;;
* )
    echo "error: arg[$1] do not support."
    usage
    exit 1
    ;;
esac

do_set_board_env $BOARD_TO_BUILD

do_repo "$IMX_MKIMAGE_REMOTE_REPO" "$IMX_MKIMAGE_PATH"
do_repo "$IMX_ATF_REMOTE_REPO" "$IMX_ATF_PATH"

do_firmware_download
do_mkimage_cp_firmware

do_uboot_make
do_mkimage_cp_uboot

do_atf_make
do_mkimage_cp_atf

do_mkimage_make
printf "Done.\n"




