/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines common network port modes.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port modes.
 */

#ifndef _SHR_PORTMODE_H
#define _SHR_PORTMODE_H

typedef unsigned int _shr_port_mode_t;

/*
 * Defines:
 *      _SHR_PM_*
 * Purpose:
 *      Defines for port modes.
 */

#define _SHR_PM_10MB_HD         (1U << 0)
#define _SHR_PM_10MB_FD         (1U << 1)
#define _SHR_PM_100MB_HD        (1U << 2)
#define _SHR_PM_100MB_FD        (1U << 3)
#define _SHR_PM_1000MB_HD       (1U << 4)
#define _SHR_PM_1000MB_FD       (1U << 5)
#define _SHR_PM_2500MB_HD       (1U << 6)
#define _SHR_PM_2500MB_FD       (1U << 7)
#define _SHR_PM_10GB_HD         (1U << 8)
#define _SHR_PM_10GB_FD         (1U << 9)
#define _SHR_PM_PAUSE_TX        (1U << 10)       /* TX pause capable */
#define _SHR_PM_PAUSE_RX        (1U << 11)       /* RX pause capable */
#define _SHR_PM_PAUSE_ASYMM     (1U << 12)       /* Asymm pause capable (R/O) */
#define _SHR_PM_TBI             (1U << 13)       /* TBI mode supported */
#define _SHR_PM_MII             (1U << 14)       /* MII mode supported */
#define _SHR_PM_GMII            (1U << 15)       /* GMII mode supported */
#define _SHR_PM_SGMII           (1U << 16)       /* SGMII mode supported */
#define _SHR_PM_XGMII           (1U << 17)       /* XGMII mode supported */
#define _SHR_PM_LB_MAC          (1U << 18)       /* MAC loopback supported */
#define _SHR_PM_LB_NONE         (1U << 19)       /* Useful for automated test */
#define _SHR_PM_LB_PHY          (1U << 20)       /* PHY loopback supported */
#define _SHR_PM_AN              (1U << 21)       /* Auto-negotiation */
#define _SHR_PM_3000MB_HD       (1U << 29)
#define _SHR_PM_3000MB_FD       (1U << 30)
#define _SHR_PM_12GB_HD         (1U << 22)
#define _SHR_PM_12GB_FD         (1U << 23)
#define _SHR_PM_13GB_HD         (1U << 24)
#define _SHR_PM_13GB_FD         (1U << 25) 
#define _SHR_PM_16GB_HD         (1U << 26)
#define _SHR_PM_16GB_FD         (1U << 27)
#define _SHR_PM_COMBO           (1U << 31) /* COMBO ports support both \
                                            * copper and fiber interfaces */

#define _SHR_PM_PAUSE           (_SHR_PM_PAUSE_TX  | _SHR_PM_PAUSE_RX)
#define _SHR_PM_10MB            (_SHR_PM_10MB_HD   | _SHR_PM_10MB_FD)
#define _SHR_PM_100MB           (_SHR_PM_100MB_HD  | _SHR_PM_100MB_FD)
#define _SHR_PM_1000MB          (_SHR_PM_1000MB_HD | _SHR_PM_1000MB_FD)
#define _SHR_PM_2500MB          (_SHR_PM_2500MB_HD | _SHR_PM_2500MB_FD)
#define _SHR_PM_3000MB          (_SHR_PM_3000MB_HD | _SHR_PM_3000MB_FD)
#define _SHR_PM_10GB            (_SHR_PM_10GB_HD   | _SHR_PM_10GB_FD)
#define _SHR_PM_12GB            (_SHR_PM_12GB_HD   | _SHR_PM_12GB_FD)
#define _SHR_PM_13GB            (_SHR_PM_13GB_HD   | _SHR_PM_13GB_FD)
#define _SHR_PM_16GB            (_SHR_PM_16GB_HD   | _SHR_PM_16GB_FD)

#define _SHR_PM_SPEED_ALL       (_SHR_PM_16GB |         \
                                 _SHR_PM_13GB |         \
                                 _SHR_PM_12GB |         \
                                 _SHR_PM_10GB |         \
                                 _SHR_PM_3000MB |       \
                                 _SHR_PM_2500MB |       \
                                 _SHR_PM_1000MB |       \
                                 _SHR_PM_100MB |        \
                                 _SHR_PM_10MB)

#define _SHR_PM_SPEED_MAX(m)    (((m) & _SHR_PM_16GB)   ? 16000 : \
                                 ((m) & _SHR_PM_13GB)   ? 13000 : \
                                 ((m) & _SHR_PM_12GB)   ? 12000 : \
                                 ((m) & _SHR_PM_10GB)   ? 10000 : \
                                 ((m) & _SHR_PM_3000MB) ? 3000 : \
                                 ((m) & _SHR_PM_2500MB) ? 2500 : \
                                 ((m) & _SHR_PM_1000MB) ? 1000 : \
                                 ((m) & _SHR_PM_100MB)  ? 100 : \
                                 ((m) & _SHR_PM_10MB)   ? 10 : 0)

#define _SHR_PM_SPEED(s)        ((16000 == (s)) ? _SHR_PM_16GB : \
                                 (13000 == (s)) ? _SHR_PM_13GB : \
                                 (12000 == (s)) ? _SHR_PM_12GB : \
                                 (10000 == (s)) ? _SHR_PM_10GB : \
                                 (3000  == (s)) ? _SHR_PM_3000MB : \
                                 (2500  == (s)) ? _SHR_PM_2500MB : \
                                 (1000  == (s)) ? _SHR_PM_1000MB : \
                                 (100   == (s)) ? _SHR_PM_100MB : \
                                 (10    == (s)) ? _SHR_PM_10MB : 0)

#define _SHR_PM_FD              (_SHR_PM_16GB_FD |    \
                                 _SHR_PM_13GB_FD |    \
                                 _SHR_PM_12GB_FD |    \
                                 _SHR_PM_10GB_FD |    \
                                 _SHR_PM_3000MB_FD |    \
                                 _SHR_PM_2500MB_FD |    \
                                 _SHR_PM_1000MB_FD |    \
                                 _SHR_PM_100MB_FD |     \
                                 _SHR_PM_10MB_FD)

#define _SHR_PM_HD              (_SHR_PM_16GB_HD |    \
                                 _SHR_PM_13GB_HD |    \
                                 _SHR_PM_12GB_HD |    \
                                 _SHR_PM_10GB_HD |    \
                                 _SHR_PM_3000MB_HD |    \
                                 _SHR_PM_2500MB_HD |    \
                                 _SHR_PM_1000MB_HD |    \
                                 _SHR_PM_100MB_HD |     \
                                 _SHR_PM_10MB_HD)

#define _SHR_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_0   (1 << 0)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_1   (1 << 1)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_2   (1 << 2)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_CUSTOM       (1 << 3)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_88640_TM     (1 << 4)
#define _SHR_PORT_CONTROL_FABRIC_HEADER_88640_RAW    (1 << 5)

/* Port link fault signalling. */
#define _SHR_PORT_FAULT_LOCAL               0x0001
#define _SHR_PORT_FAULT_REMOTE              0x0002

#endif  /* !_SHR_PORTMODE_H */
