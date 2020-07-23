/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix5_flexport_defines.h
 * Purpose:
 * Requires:
 */
 
#ifndef HELIX5_FLEXPORT_DEFINES_H
#define HELIX5_FLEXPORT_DEFINES_H


#define HELIX5_PHY_PORT_CPU               0
#define HELIX5_PHY_PORT_LPBK0             77

#define HELIX5_FORCE_SAF_TIMER_ENTRY_CNT  5
#define HELIX5_OBM_PRIORITY_LOSSY_LO      0
#define HELIX5_OBM_PRIORITY_LOSSY_HI      1
#define HELIX5_OBM_PRIORITY_LOSSLESS0     2
#define HELIX5_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define HELIX5_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=76))

/* General Physical port */
#define HELIX5_PORTS_PER_PBLK             4
#define HELIX5_PBLKS_PER_PIPE             19
#define HELIX5_PBLKS_PER_HPIPE            9
#define HELIX5_PIPES_PER_DEV              1
#define HELIX5_XPES_PER_DEV               1
#define HELIX5_GPHY_PORTS_PER_PIPE        \
    (HELIX5_PORTS_PER_PBLK * HELIX5_PBLKS_PER_PIPE)
#define HELIX5_PHY_PORTS_PER_PIPE         (HELIX5_GPHY_PORTS_PER_PIPE + 3)
#define HELIX5_PBLKS_PER_DEV              \
    (HELIX5_PBLKS_PER_PIPE * HELIX5_PIPES_PER_DEV)
#define HELIX5_PHY_PORTS_PER_DEV          \
    (HELIX5_PHY_PORTS_PER_PIPE * HELIX5_PIPES_PER_DEV)


/* Device port
 * 64 General device port + 1 CPU/Mng + 1 Loopback*/
#define HELIX5_GDEV_PORTS_PER_PIPE        69
#define HELIX5_DEV_PORTS_PER_PIPE         (HELIX5_GDEV_PORTS_PER_PIPE + 3)
#define HELIX5_DEV_PORTS_PER_DEV          \
    (HELIX5_DEV_PORTS_PER_PIPE * HELIX5_PIPES_PER_DEV)

/* MMU port */
#define HELIX5_MMU_PORTS_OFFSET_PER_PIPE  72
#define HELIX5_MMU_PORTS_PER_DEV          72 

/* TDM */
#define HELIX5_TDM_LENGTH                 512
#define HELIX5_OVS_GROUP_COUNT_PER_HPIPE  6
#define HELIX5_OVS_GROUP_COUNT_PER_PIPE   12
#define HELIX5_OVS_GROUP_TDM_LENGTH       12
#define HELIX5_OVS_HPIPE_COUNT_PER_PIPE   2

/* IDB BUFFER COUNT */
/*
  Total buffer available in OBM is 72KB. 72 KB /16B = 4608 
  Per lane 4608 / 4 = 1152
*/
#define HELIX5_OBM_BUFFER_PER_LANE_FL    384
#define HELIX5_OBM_BUFFER_PER_LANE_EG    1152

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* HELIX5_FLEXPORT_DEFINES_H */
