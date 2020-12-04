/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 */

#ifndef blackhawk_v1l8p1_ucode_H_
#define blackhawk_v1l8p1_ucode_H_ 

#include <phymod/phymod.h>

#define BLACKHAWK7_V1L8P1_UCODE_IMAGE_VERSION "D005_0B"  /* matches the version number from microcode */
#define BLACKHAWK7_V1L8P1_UCODE_IMAGE_SIZE    121808
#define BLACKHAWK7_V1L8P1_UCODE_STACK_SIZE    0x9F2
#define BLACKHAWK7_V1L8P1_UCODE_IMAGE_CRC     0xFE01

extern unsigned char*  blackhawk7_v1l8p1_ucode_get(void);

#endif


