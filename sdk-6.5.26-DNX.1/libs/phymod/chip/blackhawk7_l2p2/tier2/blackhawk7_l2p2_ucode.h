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

#define BLACKHAWK7_L2P2_UCODE_IMAGE_VERSION "D005_1B"  /* matches the version number from microcode */
#define BLACKHAWK7_L2P2_UCODE_IMAGE_SIZE    123748
#define BLACKHAWK7_L2P2_UCODE_STACK_SIZE    0x12C8
#define BLACKHAWK7_L2P2_UCODE_DR_CODE_SIZE_KB    0x1
#define BLACKHAWK7_L2P2_UCODE_IMAGE_CRC     0xBB19

#define BLACKHAWK7_L2P2_FLR_UCODE_IMAGE_VERSION "D005_1B"  /* matches the version number from microcode */
#define BLACKHAWK7_L2P2_FLR_UCODE_IMAGE_SIZE    120708
#define BLACKHAWK7_L2P2_FLR_UCODE_STACK_SIZE    0x12C8
#define BLACKHAWK7_L2P2_FLR_UCODE_DR_CODE_SIZE_KB    0x1
#define BLACKHAWK7_L2P2_FLR_UCODE_IMAGE_CRC     0x8C1F

extern unsigned char* blackhawk7_l2p2_ucode_get(uint8_t flr_ucode);

#endif
