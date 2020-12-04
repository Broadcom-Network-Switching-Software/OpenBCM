/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#ifndef _WCMOD_REGISTER_FIELDS_H_
#define _WCMOD_REGISTER_FIELDS_H_

/* AN advertisement 0 pause fields */

#define AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_pause_MASK   0x0800
#define AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_asm_dir_MASK 0x0400

/* AN advertisement 1 speed fields */

#define AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_100G_CR10_MASK  0x0400
#define AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_40G_CR4_MASK    0x0200
#define AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_40G_KR4_MASK    0x0100
#define AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_10G_KR_MASK     0x0080
#define AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_10G_KX4_MASK    0x0040
#define AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_1G_KX_MASK      0x0020


/* AN advertisement 2 fec fields */

#define AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_fec_requested_MASK 0x8000
#define AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_fec_ability_MASK 0x8000

#endif
