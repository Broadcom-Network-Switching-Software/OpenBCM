/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File:        hurricane4_flexport_defines.h
 *  Purpose:
 *  Requires:
 */

#ifndef HURRICANE4_FLEXPORT_DEFINES_H
#define HURRICANE4_FLEXPORT_DEFINES_H


#define HURRICANE4_PHY_PORT_CPU               0
#define HURRICANE4_PHY_PORT_LPBK0             69

#define HURRICANE4_FORCE_SAF_TIMER_ENTRY_CNT  5
#define HURRICANE4_OBM_PRIORITY_LOSSY_LO      0
#define HURRICANE4_OBM_PRIORITY_LOSSY_HI      1
#define HURRICANE4_OBM_PRIORITY_LOSSLESS0     2
#define HURRICANE4_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define HURRICANE4_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=64))

/* General Physical port */
#define HURRICANE4_PORTS_PER_PBLK             4
#define HURRICANE4_PBLKS_PER_PIPE             16
#define HURRICANE4_PBLKS_PER_HPIPE            8
#define HURRICANE4_PIPES_PER_DEV              1
#define HURRICANE4_XPES_PER_DEV               1
#define HURRICANE4_GPHY_PORTS_PER_PIPE        \
    (HURRICANE4_PORTS_PER_PBLK * HURRICANE4_PBLKS_PER_PIPE)
#define HURRICANE4_PHY_PORTS_PER_PIPE         (HURRICANE4_GPHY_PORTS_PER_PIPE + 7 )
#define HURRICANE4_PBLKS_PER_DEV              \
    (HURRICANE4_PBLKS_PER_PIPE * HURRICANE4_PIPES_PER_DEV)
#define HURRICANE4_PHY_PORTS_PER_DEV          \
    (HURRICANE4_PHY_PORTS_PER_PIPE * HURRICANE4_PIPES_PER_DEV)


/* Device port
 * 64 General device port + 1 CPU/Mng + 1 Loopback*/
#define HURRICANE4_GDEV_PORTS_PER_PIPE        68
#define HURRICANE4_DEV_PORTS_PER_PIPE         (HURRICANE4_GDEV_PORTS_PER_PIPE + 3 )
#define HURRICANE4_DEV_PORTS_PER_DEV          \
    (HURRICANE4_DEV_PORTS_PER_PIPE * HURRICANE4_PIPES_PER_DEV)

/* MMU port */
#define HURRICANE4_MMU_PORTS_OFFSET_PER_PIPE  72
#define HURRICANE4_MMU_PORTS_PER_DEV          72

/* TDM */
#define HURRICANE4_TDM_LENGTH                 512
#define HURRICANE4_OVS_GROUP_COUNT_PER_HPIPE  6
#define HURRICANE4_OVS_GROUP_COUNT_PER_PIPE   12
#define HURRICANE4_OVS_GROUP_TDM_LENGTH       12
#define HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE   2

/* IDB BUFFER COUNT */
/*
  Total buffer available in OBM is 72KB. 72 KB /16B = 4608
  Per lane 4608 / 4 = 1152
*/
#define HURRICANE4_OBM_BUFFER_PER_LANE_FL    384
#define HURRICANE4_OBM_BUFFER_PER_LANE_EG    1152

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* HURRICANE4_FLEXPORT_DEFINES_H */
