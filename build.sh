#!/bin/bash

###############################################################################
# To all DEV around the world via Mansi :)                                    #
#                                                                             #
# 1.) use the "bash"                                                          #
# chsh -s /bin/bash `whoami`                                                  #
#                                                                             #
# 2.) delete old files                                                        #
# rm -r out                                                                   #
#                                                                             #
# 3.) now you can build my kernel                                             #
# ./build.sh                                                                  #
#                                                                             #
# Have fun and update me if something nice can be added to my source.         #
###############################################################################

OUT_DIR="build-msm8916"
KERNEL_DIR=$PWD
ABOOT_IMG=${OUT_DIR}/emmc_appsboot.mbn
BUILD_START=$(date +"%s")

#Set Color
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'

###############################################################################
# Set configs
###############################################################################

# Name a kernel
export KBUILD_BUILD_USER="Mansi"
export KBUILD_BUILD_HOST="MSI"

# Type
export ARCH=arm
export SUBARCH=arm
export TARGET_BUILD_VARIANT=user

# gcc 4.8.3 (Linaro 2013.x)
export TOOLCHAIN_PREFIX=~/toolchains/arm-eabi-4.8/bin/arm-eabi-

# Other
export JAVA_HOME=/usr/lib/jvm/java-7-oracle
export USE_CCACHE=1

mkdir ${OUT_DIR}


###############################################################################
# Build system
###############################################################################
echo -e "$cyan Clean old files $nocol";
	rm ${ABOOT_IMG}
	rm ${KERNEL_DIR}/mansi/emmc_appsboot.mbn

echo -e "$cyan Build aboot $nocol";
	make O=${OUT_DIR} msm8916

if ! [ -a $ABOOT_IMG ]; then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol";
	exit 1
fi


###############################################################################
# Other configs, after build
###############################################################################

echo -e "$cyan Copy aboot $nocol";
	cp ${ABOOT_IMG}  ${KERNEL_DIR}/mansi/emmc_appsboot.mbn
	cd ${KERNEL_DIR}/mansi/

echo -e "$cyan Build flash file $nocol";
	zipfile="Mansi_ABOOT_M1_($(date +"%d-%m-%Y(%H.%M%p)")).zip"
	zip -r ${zipfile} emmc_appsboot.mbn META-INF -x *kernel/.gitignore*

	BUILD_END=$(date +"%s")
	DIFF=$(($BUILD_END - $BUILD_START))

echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol";

echo "Enjoy Mansi kernel";
