/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  trident3_flexport_defines.h
*/


#ifndef TRIDENT3_FLEXPORT_DEFINES_H
#define TRIDENT3_FLEXPORT_DEFINES_H


#define TRIDENT3_PHY_PORT_CPU               0
#define TRIDENT3_PHY_PORT_MNG0              129
#define TRIDENT3_PHY_PORT_MNG1              128
#define TRIDENT3_PHY_PORT_LPBK0             130
#define TRIDENT3_PHY_PORT_LPBK1             131

#define TRIDENT3_LOG_PORT_MNG0              66
#define TRIDENT3_LOG_PORT_MNG1              130
#define TRIDENT3_FORCE_SAF_TIMER_ENTRY_CNT  5
#define TRIDENT3_OBM_PRIORITY_LOSSY_LO      0
#define TRIDENT3_OBM_PRIORITY_LOSSY_HI      1
#define TRIDENT3_OBM_PRIORITY_LOSSLESS0     2
#define TRIDENT3_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define TRIDENT3_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=128))

/* General Physical port */
#define TRIDENT3_PORTS_PER_PBLK             4
#define TRIDENT3_PBLKS_PER_PIPE             16
#define TRIDENT3_PBLKS_PER_HPIPE            8
#define TRIDENT3_PIPES_PER_DEV              2
#define TRIDENT3_XPES_PER_DEV               2
#define TRIDENT3_GPHY_PORTS_PER_PIPE        \
    (TRIDENT3_PORTS_PER_PBLK * TRIDENT3_PBLKS_PER_PIPE)
#define TRIDENT3_PHY_PORTS_PER_PIPE         (TRIDENT3_GPHY_PORTS_PER_PIPE + 2)
#define TRIDENT3_PBLKS_PER_DEV              \
    (TRIDENT3_PBLKS_PER_PIPE * TRIDENT3_PIPES_PER_DEV)
#define TRIDENT3_PHY_PORTS_PER_DEV          \
    (TRIDENT3_PHY_PORTS_PER_PIPE * TRIDENT3_PIPES_PER_DEV)


/* Device port
 * 64 General device port + 1 CPU/Mng + 1 Loopback*/
#define TRIDENT3_GDEV_PORTS_PER_PIPE        64
#define TRIDENT3_DEV_PORTS_PER_PIPE         (TRIDENT3_GDEV_PORTS_PER_PIPE + 2)
#define TRIDENT3_DEV_PORTS_PER_DEV          \
    (TRIDENT3_DEV_PORTS_PER_PIPE * TRIDENT3_PIPES_PER_DEV)

/* MMU port */
#define TRIDENT3_MMU_PORTS_OFFSET_PER_PIPE  128
#define TRIDENT3_MMU_PORTS_PER_PIPE         64
#define TRIDENT3_MMU_PORTS_PER_DEV          \
      (TRIDENT3_MMU_PORTS_PER_PIPE * TRIDENT3_PIPES_PER_DEV)

/* TDM */
#define TRIDENT3_TDM_LENGTH                 512
#define TRIDENT3_OVS_GROUP_COUNT_PER_HPIPE  6
#define TRIDENT3_OVS_GROUP_COUNT_PER_PIPE   12
#define TRIDENT3_OVS_GROUP_TDM_LENGTH       12
#define TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE   2
#define TRIDENT3_PKT_SCH_LENGTH             160

/* IDB BUFFER COUNT */
/*
  Total buffer available in OBM is 72KB. 72 KB /16B = 4608 
  Per lane 4608 / 4 = 1152
*/
#define TRIDENT3_OBM_BUFFER_PER_LANE    1152

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* TRIDENT3_FLEXPORT_DEFINES_H */
