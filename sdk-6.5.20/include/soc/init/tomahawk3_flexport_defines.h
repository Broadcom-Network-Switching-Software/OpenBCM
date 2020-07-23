/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk3_flexport_defines.h
*/

#ifndef TOMAHAWK3_FLEXPORT_DEFINES_H
#define TOMAHAWK3_FLEXPORT_DEFINES_H

#define _TH3_CA_BUFFER_SIZE             64
#define _TH3_NUM_OF_TSCBH               32
#define _TH3_PHY_PORT_CPU               0
#define _TH3_PHY_PORT_MNG0              257
#define _TH3_PHY_PORT_MNG1              258
#define _TH3_LOG_PORT_MNG0              38
#define _TH3_LOG_PORT_MNG1              118
#define _TH3_PHY_PORT_LPBK0             259
#define _TH3_PHY_PORT_LPBK1             260
#define _TH3_PHY_PORT_LPBK2             261
#define _TH3_PHY_PORT_LPBK3             262
#define _TH3_PHY_PORT_LPBK4             263
#define _TH3_PHY_PORT_LPBK5             264
#define _TH3_PHY_PORT_LPBK6             265
#define _TH3_PHY_PORT_LPBK7             266
#define _TH3_FORCE_SAF_TIMER_ENTRY_CNT  5
#define _TH3_OBM_PRIORITY_LOSSY_LO      0
#define _TH3_OBM_PRIORITY_LOSSY_HI      1
#define _TH3_OBM_PRIORITY_LOSSLESS0     2
#define _TH3_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define _TH3_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=256))

/* General Physical port */
#define _TH3_PORTS_PER_PBLK             8
#define _TH3_PBLKS_PER_PIPE             4
/*#define _TH3_PBLKS_PER_HPIPE            0*/
#define _TH3_PIPES_PER_DEV              8
#define _TH3_XPES_PER_DEV               2
#define _TH3_GPHY_PORTS_PER_PIPE        \
    (_TH3_PORTS_PER_PBLK * _TH3_PBLKS_PER_PIPE)
#define _TH3_PHY_PORTS_PER_PIPE         (_TH3_GPHY_PORTS_PER_PIPE + 2)
#define _TH3_PBLKS_PER_DEV              \
    (_TH3_PBLKS_PER_PIPE * _TH3_PIPES_PER_DEV)
#define _TH3_PHY_PORTS_PER_DEV          \
    (_TH3_PHY_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)


/* Device port
 * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH3_GDEV_PORTS_PER_PIPE        18
#define _TH3_DEV_PORTS_PER_PIPE         (_TH3_GDEV_PORTS_PER_PIPE + 2)
#define _TH3_DEV_PORTS_PER_DEV          \
    (_TH3_DEV_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)

/* MMU port */
#define _TH3_MMU_PORTS_OFFSET_PER_PIPE  32
#define _TH3_MMU_PORTS_PER_DEV          (32 * 8)

/* TDM */
#define _TH3_TDM_LENGTH                 33
#define _TH3_TDM_SCHED_SPECIAL_SLOT     18

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* TOMAHAWK3_FLEXPORT_DEFINES_H */
