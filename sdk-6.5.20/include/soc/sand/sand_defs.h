/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* This file contains structure and routine declarations for the
* Switch-on-a-Chip Driver.
*
* This file also includes the more common include files so the
* individual driver files don't have to include as much.
*/

#ifndef _SOC_SAND_DEFS_H
#define _SOC_SAND_DEFS_H

#define SOC_SAND_NOF_LINKS_IN_MAC 4


#define SOC_SAND_RESET_ACTION_IN_RESET          (0x1)
#define SOC_SAND_RESET_ACTION_OUT_RESET         (0x2)
#define SOC_SAND_RESET_ACTION_INOUT_RESET       (0x4)

#define SOC_SAND_RESET_MODE_HARD_RESET                              (0x1)
#define SOC_SAND_RESET_MODE_BLOCKS_RESET                            (0x2)
#define SOC_SAND_RESET_MODE_BLOCKS_SOFT_RESET                       (0x4)
#define SOC_SAND_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET               (0x8)
#define SOC_SAND_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET                (0x10)
#define SOC_SAND_RESET_MODE_INIT_RESET                              (0x20)
#define SOC_SAND_RESET_MODE_REG_ACCESS                              (0x40)
#define SOC_SAND_RESET_MODE_ENABLE_TRAFFIC                          (0x80)
#define SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET            (0x100)
#define SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET    (0x200)
#define SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET     (0x400)
#define SOC_SAND_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT                (0x800)

#endif
