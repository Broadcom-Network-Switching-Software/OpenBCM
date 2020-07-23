/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 */
#ifndef BLACKHAWK7_L8P2_UCODE_H
#define BLACKHAWK7_L8P2_UCODE_H

#include <phymod/phymod.h>

#define BLACKHAWK7_V1L8P2_UCODE_IMAGE_VERSION "D005_02"  /* matches the version number from microcode */
#define BLACKHAWK7_V1L8P2_UCODE_IMAGE_SIZE    119552
#define BLACKHAWK7_V1L8P2_UCODE_STACK_SIZE    0x9F2
#define BLACKHAWK7_V1L8P2_UCODE_IMAGE_CRC     0xBCA4

#define BLACKHAWK7_V2L8P2_UCODE_IMAGE_VERSION "D005_02"  /* matches the version number from microcode */
#define BLACKHAWK7_V2L8P2_UCODE_IMAGE_SIZE    119808
#define BLACKHAWK7_V2L8P2_UCODE_STACK_SIZE    0x9F2
#define BLACKHAWK7_V2L8P2_UCODE_IMAGE_CRC     0x55D6


extern unsigned char*  blackhawk7_l8p2_ucode_get(uint8_t afe_rev_a0);

#endif
