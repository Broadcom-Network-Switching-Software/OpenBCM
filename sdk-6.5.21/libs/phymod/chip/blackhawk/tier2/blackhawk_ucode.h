/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 */
#ifndef BLACKHAWK_UCODE_H
#define BLACKHAWK_UCODE_H

#include <phymod/phymod.h>


#define BLACKHAWK_TSC_UCODE_IMAGE_VERSION "D003_0C"  /* matches the version number from microcode */
#define BLACKHAWK_TSC_UCODE_IMAGE_SIZE    70260
#define BLACKHAWK_TSC_UCODE_STACK_SIZE    0x634
#define BLACKHAWK_TSC_UCODE_IMAGE_CRC     0x6D6E

#define BLACKHAWKB0_TSC_UCODE_IMAGE_VERSION "D100_0E"  /* matches the version number from microcode */
#define BLACKHAWKB0_TSC_UCODE_IMAGE_SIZE    84772
#define BLACKHAWKB0_TSC_UCODE_STACK_SIZE    0x8F4
#define BLACKHAWKB0_TSC_UCODE_IMAGE_CRC     0x4ED5


/*extern int blackhawkb0_ucode_get(unsigned char *blackhawkb0_ucode); */
extern unsigned char*  blackhawkb0_ucode_get(void);

/* extern int blackhawk_ucode_get(unsigned char *blackhawk_ucode); */

extern unsigned char*  blackhawk_ucode_get(void);

#endif
