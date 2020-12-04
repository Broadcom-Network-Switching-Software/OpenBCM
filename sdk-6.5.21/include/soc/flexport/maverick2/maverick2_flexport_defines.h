/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  maverick2_flexport_defines.h
*/


#ifndef MAVERICK2_FLEXPORT_DEFINES_H
#define MAVERICK2_FLEXPORT_DEFINES_H


#define MAVERICK2_PHY_PORT_CPU               0
#define MAVERICK2_PHY_PORT_MNG0              81
#define MAVERICK2_PHY_PORT_LPBK0             82
#define MAVERICK2_PHY_PORT_LPBK1             999 /* unused */

#define MAVERICK2_LOG_PORT_MNG0              64
#define MAVERICK2_FORCE_SAF_TIMER_ENTRY_CNT  5
#define MAVERICK2_OBM_PRIORITY_LOSSY_LO      0
#define MAVERICK2_OBM_PRIORITY_LOSSY_HI      1
#define MAVERICK2_OBM_PRIORITY_LOSSLESS0     2
#define MAVERICK2_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define MAVERICK2_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=80))

/* General Physical port */
#define MAVERICK2_PORTS_PER_PBLK             4
#define MAVERICK2_PBLKS_PER_PIPE             20
#define MAVERICK2_PBLKS_PER_HPIPE            10
#define MAVERICK2_PIPES_PER_DEV              1
#define MAVERICK2_XPES_PER_DEV               1
#define MAVERICK2_GPHY_PORTS_PER_PIPE        \
    (MAVERICK2_PORTS_PER_PBLK * MAVERICK2_PBLKS_PER_PIPE)
#define MAVERICK2_PHY_PORTS_PER_PIPE         (MAVERICK2_GPHY_PORTS_PER_PIPE + 2)
#define MAVERICK2_PBLKS_PER_DEV              \
    (MAVERICK2_PBLKS_PER_PIPE * MAVERICK2_PIPES_PER_DEV)
#define MAVERICK2_PHY_PORTS_PER_DEV          \
    (MAVERICK2_PHY_PORTS_PER_PIPE * MAVERICK2_PIPES_PER_DEV)


/* Device port
 * 64 General device port + 1 CPU/Mng + 1 Loopback*/
#define MAVERICK2_GDEV_PORTS_PER_PIPE        80
#define MAVERICK2_DEV_PORTS_PER_PIPE         (MAVERICK2_GDEV_PORTS_PER_PIPE + 2)
#define MAVERICK2_DEV_PORTS_PER_DEV          \
    (MAVERICK2_DEV_PORTS_PER_PIPE * MAVERICK2_PIPES_PER_DEV)

/* MMU port */
#define MAVERICK2_MMU_PORTS_OFFSET_PER_PIPE  128
#define MAVERICK2_MMU_PORTS_PER_DEV          (80 * 1)

/* TDM */
#define MAVERICK2_TDM_LENGTH                 512
#define MAVERICK2_OVS_GROUP_COUNT_PER_HPIPE  6
#define MAVERICK2_OVS_GROUP_COUNT_PER_PIPE   12
#define MAVERICK2_OVS_GROUP_TDM_LENGTH       12
#define MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE   2

/* IDB BUFFER COUNT */
/*
  Total buffer available in OBM is 72KB. 72 KB /16B = 4608 
  Per lane 4608 / 4 = 1152
*/
#define MAVERICK2_OBM_BUFFER_PER_LANE    1152

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* MAVERICK2_FLEXPORT_DEFINES_H */
