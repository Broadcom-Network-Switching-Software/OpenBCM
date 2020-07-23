/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef FIREBOLT6_FLEXPORT_DEFINES_H
#define FIREBOLT6_FLEXPORT_DEFINES_H


#define FIREBOLT6_PHY_PORT_CPU               0
#define FIREBOLT6_PHY_PORT_LPBK0             77

#define FIREBOLT6_FORCE_SAF_TIMER_ENTRY_CNT  5
#define FIREBOLT6_OBM_PRIORITY_LOSSY_LO      0
#define FIREBOLT6_OBM_PRIORITY_LOSSY_HI      1
#define FIREBOLT6_OBM_PRIORITY_LOSSLESS0     2
#define FIREBOLT6_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define FIREBOLT6_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=128))

/* General Physical port */
#define FIREBOLT6_PORTS_PER_PBLK             4
#define FIREBOLT6_PBLKS_PER_PIPE             17
#define FIREBOLT6_PBLKS_PER_HPIPE            9
#define FIREBOLT6_PIPES_PER_DEV              1
#define FIREBOLT6_XPES_PER_DEV               1
#define FIREBOLT6_GPHY_PORTS_PER_PIPE        \
    (FIREBOLT6_PORTS_PER_PBLK * FIREBOLT6_PBLKS_PER_PIPE)
#define FIREBOLT6_PHY_PORTS_PER_PIPE         (FIREBOLT6_GPHY_PORTS_PER_PIPE + 3)
#define FIREBOLT6_PBLKS_PER_DEV              \
    (FIREBOLT6_PBLKS_PER_PIPE * FIREBOLT6_PIPES_PER_DEV)
#define FIREBOLT6_PHY_PORTS_PER_DEV          \
    (FIREBOLT6_PHY_PORTS_PER_PIPE * FIREBOLT6_PIPES_PER_DEV)


/* Device port
 * 64 General device port + 1 CPU/Mng + 1 Loopback*/
/*#define FIREBOLT6_GDEV_PORTS_PER_PIPE       71*/
#define FIREBOLT6_DEV_PORTS_PER_PIPE        72
#define FIREBOLT6_DEV_PORTS_PER_DEV          \
    (FIREBOLT6_DEV_PORTS_PER_PIPE * FIREBOLT6_PIPES_PER_DEV)

/* MMU port */
#define FIREBOLT6_MMU_PORTS_OFFSET_PER_PIPE  72
#define FIREBOLT6_MMU_PORTS_PER_DEV          72

/* TDM */
#define FIREBOLT6_TDM_LENGTH                 512
#define FIREBOLT6_OVS_GROUP_COUNT_PER_HPIPE  6
#define FIREBOLT6_OVS_GROUP_COUNT_PER_PIPE   12
#define FIREBOLT6_OVS_GROUP_TDM_LENGTH       12
#define FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE   2

/* IDB BUFFER COUNT */
/*
  Total buffer available in OBM is 72KB. 72 KB /16B = 4608
  Per lane 4608 / 4 = 1152
*/
#define FIREBOLT6_OBM_BUFFER_PER_LANE_FL    1152
#define FIREBOLT6_OBM_BUFFER_PER_LANE_EG    1152

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* FIREBOLT6_FLEXPORT_DEFINES_H */
