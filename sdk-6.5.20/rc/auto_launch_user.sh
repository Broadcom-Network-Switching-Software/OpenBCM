#!/bin/sh
# $Id: auto_launch_user.sh,v 1.13 2013/09/16 14:59:30 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.

IS_GTO=0
IPROC_CHIPID_NS="CF12"
IPROC_CHIPID_CS="CF1A"
IPROC_CHIPID_NSP="CF1E"
IPROC_CHIPID_READ=
# Define Broadcom Switch device ID range
# The PCI Device ID is a 16-bit value equal to the part number in hexadecimal.
# For the 5-digit part number Broadcom switch devices, the last three digits
# of the part number match the last three digits of the hexadecimal device ID,
# with the first hexadecimal digit set to B (= 5 + 6). Thus, for the BCM56524
# device, the PCI Device ID is 0xb524.
# Listed the device ID ranges of Broadcom switch devices below. Ranges are
# 0x8000-0x8fff, 0xa000-0xafff, 0xb000-0xbfff, 0xc000-0xcfff.
# For string comparison operation, plus 1 for MAX boundary value and minor 1
# for MIN boundary value.
BRCM_SWITCH_DEVID_MAX1=9000
BRCM_SWITCH_DEVID_MIN1=7fff
BRCM_SWITCH_DEVID_MAX2=d000
BRCM_SWITCH_DEVID_MIN2=bfff
BRCM_SWITCH_DEVID_MAX3=c000
BRCM_SWITCH_DEVID_MIN3=afff
BRCM_SWITCH_DEVID_MAX4=b000
BRCM_SWITCH_DEVID_MIN4=9fff

cd /broadcom

grep -q BCM9562 /proc/cpuinfo
if [ $? == 0 ] ; then
    mount -t vfat /dev/mtdblock0 /mnt
else
    grep -q BCM5300 /proc/cpuinfo
    if [ $? == 0 ] ; then
        mount -t vfat /dev/mtdblock2 /mnt
    else
        grep -q BCM98548 /proc/cpuinfo
        if [ $? == 0 ] ; then
            IS_GTO=1
            grep -q "Linux version 2.6" /proc/version
            if [ $? == 0 ] ; then
                mount -t vfat /dev/mtdblock1 /mnt
            fi
        else
            grep -q iProc /proc/cpuinfo
            if [ $? == 0 ] ; then
                IPROC_CHIPID_READ=`devmem 0x18000000 16|grep '0x'|cut -d'x' -f2`
                # Get Broadcom PCI device ID list
                #     qualification : Ethernet Controller, Class 0200
                #                     Broadcom PCI vendor ID, 14e4
                PCIE_DEVICE_LIST=`lspci | grep 'Class 0200' | grep '14e4' |cut -d':' -f4`
                IS_EHOST=0
                # Identify if the Broadcom PCI devices are switch device.
                # If any Broadcom switch device is found on the PCI bus, consider the CPU is in eHost mode.
                for DEVID in $PCIE_DEVICE_LIST
                do
                    if [ "$DEVID" \< "$BRCM_SWITCH_DEVID_MAX1" ] && [ "$DEVID" \> "$BRCM_SWITCH_DEVID_MIN1" ]; then
                        IS_EHOST=1
                    fi
                    if [ "$DEVID" \< "$BRCM_SWITCH_DEVID_MAX2" ] && [ "$DEVID" \> "$BRCM_SWITCH_DEVID_MIN2" ]; then
                        IS_EHOST=1
                    fi
                    if [ "$DEVID" \< "$BRCM_SWITCH_DEVID_MAX3" ] && [ "$DEVID" \> "$BRCM_SWITCH_DEVID_MIN3" ]; then
                        IS_EHOST=1
                    fi
                    if [ "$DEVID" \< "$BRCM_SWITCH_DEVID_MAX4" ] && [ "$DEVID" \> "$BRCM_SWITCH_DEVID_MIN4" ]; then
                        IS_EHOST=1
                    fi
                done
            fi
        fi
    fi
fi

if [ -f /mnt/config.bcm ] ; then
    export BCM_CONFIG_FILE=/mnt/config.bcm
else
    # See if Petra co-processor (vendor 1172, device 4) / ARAD / FE1600
    # is present. on the PCI bus. If it is, then this is a Dune board.
    #
    egrep -q "11720004|14e48650|14e48750|14e48660" /proc/bus/pci/devices
    if [ $? == 0 ] ; then
        export BCM_CONFIG_FILE=/broadcom/config-sand.bcm
    elif [ "$IPROC_CHIPID_READ" == "$IPROC_CHIPID_NSP" ] && [ $IS_EHOST == 0 ]; then
        export BCM_CONFIG_FILE=/broadcom/config/BCM953022K.bcm
    elif [ "$IPROC_CHIPID_READ" == "$IPROC_CHIPID_CS" ]; then
        export BCM_CONFIG_FILE=/broadcom/config/BCM953012K.bcm
    elif [ "$IPROC_CHIPID_READ" == "$IPROC_CHIPID_NS" ]; then
        export BCM_CONFIG_FILE=/broadcom/config/BCM953012K.bcm
    else
        # Not XCore, not SAND. Must be an XGS board...
        # Load the default configuration file.
        #
        export BCM_CONFIG_FILE=/broadcom/config.bcm
    fi
fi

./bcm.user

