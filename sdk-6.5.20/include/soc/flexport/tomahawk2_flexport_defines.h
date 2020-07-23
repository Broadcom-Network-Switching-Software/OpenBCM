/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_flexport_defines.h
*/


#ifndef TOMAHAWK2_FLEXPORT_DEFINES_H
#define TOMAHAWK2_FLEXPORT_DEFINES_H


#define _TH2_PHY_PORT_CPU               0
#define _TH2_PHY_PORT_MNG0              257
#define _TH2_PHY_PORT_MNG1              259
#define _TH2_LOG_PORT_MNG0              66
#define _TH2_LOG_PORT_MNG1              100
#define _TH2_PHY_PORT_LPBK0             260
#define _TH2_PHY_PORT_LPBK1             261
#define _TH2_PHY_PORT_LPBK2             262
#define _TH2_PHY_PORT_LPBK3             263
#define _TH2_FORCE_SAF_TIMER_ENTRY_CNT  5
#define _TH2_OBM_PRIORITY_LOSSY_LO      0
#define _TH2_OBM_PRIORITY_LOSSY_HI      1
#define _TH2_OBM_PRIORITY_LOSSLESS0     2
#define _TH2_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define _TH2_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=256))

/* General Physical port */
#define _TH2_PORTS_PER_PBLK             4
#define _TH2_PBLKS_PER_PIPE             16
#define _TH2_PBLKS_PER_HPIPE            8
#define _TH2_PIPES_PER_DEV              4
#define _TH2_XPES_PER_DEV               4
#define _TH2_GPHY_PORTS_PER_PIPE        \
    (_TH2_PORTS_PER_PBLK * _TH2_PBLKS_PER_PIPE)
#define _TH2_PHY_PORTS_PER_PIPE         (_TH2_GPHY_PORTS_PER_PIPE + 2)
#define _TH2_PBLKS_PER_DEV              \
    (_TH2_PBLKS_PER_PIPE * _TH2_PIPES_PER_DEV)
#define _TH2_PHY_PORTS_PER_DEV          \
    (_TH2_PHY_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)


/* Device port
 * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH2_GDEV_PORTS_PER_PIPE        32
#define _TH2_DEV_PORTS_PER_PIPE         (_TH2_GDEV_PORTS_PER_PIPE + 2)
#define _TH2_DEV_PORTS_PER_DEV          \
    (_TH2_DEV_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)

/* MMU port */
#define _TH2_MMU_PORTS_OFFSET_PER_PIPE  64
#define _TH2_MMU_PORTS_PER_DEV          (64 * 4)

/* TDM */
#define _TH2_TDM_LENGTH                 512
#define _TH2_OVS_GROUP_COUNT_PER_HPIPE  6
#define _TH2_OVS_GROUP_COUNT_PER_PIPE   12
#define _TH2_OVS_GROUP_TDM_LENGTH       12
#define _TH2_OVS_HPIPE_COUNT_PER_PIPE   2

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* TOMAHAWK2_FLEXPORT_DEFINES_H */
