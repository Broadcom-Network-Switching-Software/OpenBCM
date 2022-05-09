/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 */
#ifndef BLACKHAWK7_L2P2_UCODE_H
#define BLACKHAWK7_L2P2_UCODE_H

#include <phymod/phymod.h>

#define BLACKHAWK7_V1L2P2_UCODE_IMAGE_VERSION "D005_14"  /* matches the version number from microcode */
#define BLACKHAWK7_V1L2P2_UCODE_IMAGE_SIZE    120048
#define BLACKHAWK7_V1L2P2_UCODE_STACK_SIZE    0x13C8
#define BLACKHAWK7_V1L2P2_UCODE_IMAGE_CRC     0xB6DD

extern unsigned char*  blackhawk7_l2p2_ucode_get(void);

#endif
