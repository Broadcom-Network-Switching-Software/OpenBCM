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

#define BLACKHAWK7_L2P2_UCODE_IMAGE_VERSION "D005_1D"  /* matches the version number from microcode */
#define BLACKHAWK7_L2P2_UCODE_IMAGE_SIZE    125232
#define BLACKHAWK7_L2P2_UCODE_STACK_SIZE    0x10C8
#define BLACKHAWK7_L2P2_UCODE_DR_CODE_SIZE_KB    0x3
#define BLACKHAWK7_L2P2_UCODE_IMAGE_CRC     0xE066

#define BLACKHAWK7_L2P2_FLR_UCODE_IMAGE_VERSION "D005_1D"  /* matches the version number from microcode */
#define BLACKHAWK7_L2P2_FLR_UCODE_IMAGE_SIZE    122192
#define BLACKHAWK7_L2P2_FLR_UCODE_STACK_SIZE    0x10C8
#define BLACKHAWK7_L2P2_FLR_UCODE_DR_CODE_SIZE_KB    0x3
#define BLACKHAWK7_L2P2_FLR_UCODE_IMAGE_CRC     0x9E35

extern unsigned char* blackhawk7_l2p2_ucode_get(uint8_t flr_ucode);

#endif
