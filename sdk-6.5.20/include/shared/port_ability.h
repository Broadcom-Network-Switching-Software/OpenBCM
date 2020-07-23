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

#ifndef _SHR_PORTABILITY_H
#define _SHR_PORTABILITY_H

typedef unsigned int    _shr_pa_encap_t;

typedef struct _shr_port_ability_s {
    _shr_port_mode_t speed_half_duplex;
    _shr_port_mode_t speed_full_duplex;
    _shr_port_mode_t pause;
    _shr_port_mode_t interface;
    _shr_port_mode_t medium;
    _shr_port_mode_t loopback;
    _shr_port_mode_t flags;
    _shr_port_mode_t eee;
    _shr_port_mode_t rsvd;
    _shr_pa_encap_t  encap;
    _shr_port_mode_t fec;
    _shr_port_mode_t channel;
} _shr_port_ability_t;

#define _SHR_PA_ABILITY_ALL     (0xffffffff)
#define _SHR_PA_ABILITY_NONE    (0)

/*
 * Defines:
 *      _SHR_PA_ENCAP_*
 * Purpose:
 *      Defines for port encapsulations.
 */

#define _SHR_PA_ENCAP_IEEE              (1 << 0)
#define _SHR_PA_ENCAP_HIGIG             (1 << 1)
#define _SHR_PA_ENCAP_B5632             (1 << 2)
#define _SHR_PA_ENCAP_HIGIG2            (1 << 3)
#define _SHR_PA_ENCAP_HIGIG2_L2         (1 << 4)
#define _SHR_PA_ENCAP_HIGIG2_IP_GRE     (1 << 5)
#define _SHR_PA_ENCAP_OBSOLETE          (1 << 6)
#define _SHR_PA_ENCAP_HIGIG2_LITE       (1 << 7)
#define _SHR_PA_ENCAP_HIGIG_OVER_ETHERNET (1 << 8)
#define _SHR_PA_ENCAP_CPRI              (1 << 9)
#define _SHR_PA_ENCAP_RSVD4             (1 << 10)

#define _SHR_PA_ENCAP_STRING            {"IEEE", \
                                         "HIGIG", \
                                         "B5632", \
                                         "HIGIG2", \
                                         "HIGIG2_L2", \
                                         "HIGIG2_IP_GRE", \
                                         "OBSOLETE", \
                                         "HIGIG2_LITE", \
                                         "HIGIG_OVER_ETHERNET",\
                                         "CPRI",\
                                         "RSVD4"}


/*
 * Defines:
 *      _SHR_PA_SPEED_*
 * Purpose:
 *      Defines for half duplex port speeds.
 */
#define _SHR_PA_SPEED_10MB         (1U << 0)
#define _SHR_PA_SPEED_EXTENDED     (1U << 1)
#define _SHR_PA_SPEED_27GB         (1U << 2)
#define _SHR_PA_SPEED_50GB         (1U << 3)
#define _SHR_PA_SPEED_53GB         (1U << 4)
#define _SHR_PA_SPEED_100MB        (1U << 5)
#define _SHR_PA_SPEED_1000MB       (1U << 6)
#define _SHR_PA_SPEED_2500MB       (1U << 7)
#define _SHR_PA_SPEED_3000MB       (1U << 8)
#define _SHR_PA_SPEED_5000MB       (1U << 9)
#define _SHR_PA_SPEED_6000MB       (1U << 10)
#define _SHR_PA_SPEED_10GB         (1U << 11)
#define _SHR_PA_SPEED_11GB         (1U << 12)
#define _SHR_PA_SPEED_12GB         (1U << 13)
#define _SHR_PA_SPEED_12P5GB       (1U << 14)
#define _SHR_PA_SPEED_13GB         (1U << 15)
#define _SHR_PA_SPEED_15GB         (1U << 16)
#define _SHR_PA_SPEED_16GB         (1U << 17)
#define _SHR_PA_SPEED_20GB         (1U << 18)
#define _SHR_PA_SPEED_21GB         (1U << 19)
#define _SHR_PA_SPEED_23GB         (1U << 20)
#define _SHR_PA_SPEED_24GB         (1U << 21)
#define _SHR_PA_SPEED_25GB         (1U << 22)
#define _SHR_PA_SPEED_30GB         (1U << 23)
#define _SHR_PA_SPEED_40GB         (1U << 24)
#define _SHR_PA_SPEED_42GB         (1U << 25)
#define _SHR_PA_SPEED_100GB        (1U << 26)
#define _SHR_PA_SPEED_120GB        (1U << 27)
#define _SHR_PA_SPEED_127GB        (1U << 28)
#define _SHR_PA_SPEED_106GB        (1U << 29)
#define _SHR_PA_SPEED_48GB         (1U << 30)
#define _SHR_PA_SPEED_32GB         (1U << 31)

#define _SHR_PA_SPEED_STRING       {"10MB", \
                                 "", \
                                 "27GB", \
                                 "50GB", \
                                 "53GB", \
                                 "100MB", \
                                 "1000MB", \
                                 "2500MB", \
                                 "3000MB", \
                                 "5000MB", \
                                 "6000MB", \
                                 "10GB", \
                                 "11GB", \
                                 "12GB", \
                                 "12P5GB", \
                                 "13GB", \
                                 "15GB", \
                                 "16GB", \
                                 "20GB", \
                                 "21GB", \
                                 "23GB", \
                                 "24GB", \
                                 "25GB", \
                                 "30GB", \
                                 "40GB", \
                                 "42GB", \
                                 "100GB", \
                                 "120GB", \
                                 "127GB", \
                                 "106GB", \
                                 "48GB", \
                                 "32GB" }
/*
 * Defines:
 *      _SHR_PA_PAUSE_*
 * Purpose:
 *      Defines for flow control abilities.
 */
#define _SHR_PA_PAUSE_TX        (1 << 0)       /* TX pause capable */
#define _SHR_PA_PAUSE_RX        (1 << 1)       /* RX pause capable */
#define _SHR_PA_PAUSE_ASYMM     (1 << 2)       /* Asymm pause capable (R/O) */

#define _SHR_PA_PAUSE_STRING    {"PAUSE_TX", \
                                 "PAUSE_RX", \
                                 "PAUSE_ASYMM"}

/*
 * Defines:
 *      _SHR_PA_FEC_*
 * Purpose:
 *      Defines for FEC abilities.
 */

#define _SHR_PA_FEC_NONE     (1 << 0)       /* FEC is not requested  */
#define _SHR_PA_FEC_CL74     (1 << 1)       /* FEC CL74 ability request */
#define _SHR_PA_FEC_CL91     (1 << 2)       /* FEC Cl91 ability request */
#define _SHR_PA_FEC_STRING            {"FEC_NONE", \
                                       "FEC_CL74", \
                                       "FEC_CL91"}


/*
 * Defines:
 *      _SHR_PA_INTF_*
 * Purpose:
 *      Defines for port interfaces supported.
 */
#define _SHR_PA_INTF_TBI        (1 << 0)       /* TBI mode supported */
#define _SHR_PA_INTF_MII        (1 << 1)       /* MII mode supported */
#define _SHR_PA_INTF_GMII       (1 << 2)       /* GMII mode supported */
#define _SHR_PA_INTF_RGMII      (1 << 3)       /* RGMII mode supported */
#define _SHR_PA_INTF_SGMII      (1 << 4)       /* SGMII mode supported */
#define _SHR_PA_INTF_XGMII      (1 << 5)       /* XGMII mode supported */
#define _SHR_PA_INTF_QSGMII     (1 << 6)       /* QSGMII mode supported */
#define _SHR_PA_INTF_CGMII      (1 << 7)       /* CGMII mode supported */

#define _SHR_PA_INTF_STRING     {"TBI", \
                                 "MII", \
                                 "GMII", \
                                 "RGMII", \
                                 "SGMII", \
                                 "XGMII", \
                                 "QSGMII", \
                                 "CGMII"}

/*
 * Defines:
 *      _SHR_PA_CHANNEL_*
 * Purpose:
 *      Defines for CHANNEL abilities.
 */

#define _SHR_PA_CHANNEL_LONG     (1 << 0)       /* Channel is long   */
#define _SHR_PA_CHANNEL_SHORT    (1 << 1)       /* Channel is short  */
#define _SHR_PA_CHANNEL_STRING        {"CHANNEL_LONG", \
                                       "CHANNEL_SHORT" }

/*
 * Defines:
 *      _SHR_PA_MEDIUM_*
 * Purpose:
 *      Defines for port medium modes.
 */
#define _SHR_PA_MEDIUM_COPPER   (1 << 0)
#define _SHR_PA_MEDIUM_FIBER    (1 << 1)
#define _SHR_PA_MEDIUM_BACKPLANE (1 << 2)

#define _SHR_PA_MEDIUM_STRING   {"COPPER", \
                                 "FIBER", \
                                 "BACKPLANE"}
/*
 * Defines:
 *      _SHR_PA_LOOPBACK_*
 * Purpose:
 *      Defines for port loopback modes.
 */
#define _SHR_PA_LB_NONE         (1 << 0)       /* Useful for automated test */
#define _SHR_PA_LB_MAC          (1 << 1)       /* MAC loopback supported */
#define _SHR_PA_LB_PHY          (1 << 2)       /* PHY loopback supported */
#define _SHR_PA_LB_LINE         (1 << 3)       /* PHY lineside loopback */

#define _SHR_PA_LB_STRING       {"LB_NONE", \
                                 "LB_MAC", \
                                 "LB_PHY", \
                                 "LB_LINE"}

/*
 * Defines:
 *      _SHR_PA_FLAGS_*
 * Purpose:
 *      Defines for the reest of port ability flags.
 */
#define _SHR_PA_AUTONEG         (1 << 0)       /* Auto-negotiation */
#define _SHR_PA_COMBO           (1 << 1)       /* COMBO ports support both
                                                * copper and fiber interfaces */
#define _SHR_PA_FLAGS_STRING    {"AUTONEG", \
                                 "COMBO"}

#define _SHR_PA_PAUSE           (_SHR_PA_PAUSE_TX  | _SHR_PA_PAUSE_RX)

#define _SHR_PA_SPEED_ALL    (_SHR_PA_SPEED_EXTENDED |     \
                              _SHR_PA_SPEED_127GB |        \
                              _SHR_PA_SPEED_120GB |        \
                              _SHR_PA_SPEED_106GB |        \
                              _SHR_PA_SPEED_100GB |        \
                              _SHR_PA_SPEED_53GB |         \
                              _SHR_PA_SPEED_50GB |         \
                              _SHR_PA_SPEED_42GB |         \
                              _SHR_PA_SPEED_40GB |         \
                              _SHR_PA_SPEED_32GB |         \
                              _SHR_PA_SPEED_30GB |         \
                              _SHR_PA_SPEED_27GB |         \
                              _SHR_PA_SPEED_25GB |         \
                              _SHR_PA_SPEED_24GB |         \
                              _SHR_PA_SPEED_23GB |         \
                              _SHR_PA_SPEED_21GB |         \
                              _SHR_PA_SPEED_20GB |         \
                              _SHR_PA_SPEED_16GB |         \
                              _SHR_PA_SPEED_15GB |         \
                              _SHR_PA_SPEED_13GB |         \
                              _SHR_PA_SPEED_12P5GB |       \
                              _SHR_PA_SPEED_12GB |         \
                              _SHR_PA_SPEED_11GB |         \
                              _SHR_PA_SPEED_10GB |         \
                              _SHR_PA_SPEED_6000MB |       \
                              _SHR_PA_SPEED_5000MB |       \
                              _SHR_PA_SPEED_3000MB |       \
                              _SHR_PA_SPEED_2500MB |       \
                              _SHR_PA_SPEED_1000MB |       \
                              _SHR_PA_SPEED_100MB |        \
                              _SHR_PA_SPEED_10MB)


#define _SHR_PA_SPEED_MAX(m) (((m) & _SHR_PA_SPEED_127GB)    ? 127000 : \
                              ((m) & _SHR_PA_SPEED_120GB)    ? 120000 : \
                              ((m) & _SHR_PA_SPEED_106GB)    ? 106000 : \
                              ((m) & _SHR_PA_SPEED_100GB)    ? 100000 : \
                              ((m) & _SHR_PA_SPEED_53GB)     ? 53000 : \
                              ((m) & _SHR_PA_SPEED_50GB)     ? 50000 : \
                              ((m) & _SHR_PA_SPEED_42GB)     ? 42000 : \
                              ((m) & _SHR_PA_SPEED_40GB)     ? 40000 : \
                              ((m) & _SHR_PA_SPEED_32GB)     ? 32000 : \
                              ((m) & _SHR_PA_SPEED_30GB)     ? 30000 : \
                              ((m) & _SHR_PA_SPEED_27GB)     ? 27000 : \
                              ((m) & _SHR_PA_SPEED_25GB)     ? 25000 : \
                              ((m) & _SHR_PA_SPEED_24GB)     ? 24000 : \
                              ((m) & _SHR_PA_SPEED_23GB)     ? 23000 : \
                              ((m) & _SHR_PA_SPEED_21GB)     ? 21000 : \
                              ((m) & _SHR_PA_SPEED_20GB)     ? 20000 : \
                              ((m) & _SHR_PA_SPEED_16GB)     ? 16000 : \
                              ((m) & _SHR_PA_SPEED_15GB)     ? 15000 : \
                              ((m) & _SHR_PA_SPEED_13GB)     ? 13000 : \
                              ((m) & _SHR_PA_SPEED_12P5GB)   ? 12500 : \
                              ((m) & _SHR_PA_SPEED_12GB)     ? 12000 : \
                              ((m) & _SHR_PA_SPEED_11GB)     ? 11000 : \
                              ((m) & _SHR_PA_SPEED_10GB)     ? 10000 : \
                              ((m) & _SHR_PA_SPEED_6000MB)   ? 6000 : \
                              ((m) & _SHR_PA_SPEED_5000MB)   ? 5000 : \
                              ((m) & _SHR_PA_SPEED_3000MB)   ? 3000 : \
                              ((m) & _SHR_PA_SPEED_2500MB)   ? 2500 : \
                              ((m) & _SHR_PA_SPEED_1000MB)   ? 1000 : \
                              ((m) & _SHR_PA_SPEED_100MB)    ? 100 : \
                              ((m) & _SHR_PA_SPEED_10MB)     ? 10 : 0)


#define _SHR_PA_SPEED(s)     ((127000 == (s)) ? _SHR_PA_SPEED_127GB : \
                              (120000 == (s)) ? _SHR_PA_SPEED_120GB : \
                              (106000 == (s)) ? _SHR_PA_SPEED_106GB : \
                              (100000 == (s)) ? _SHR_PA_SPEED_100GB : \
                              (53000 == (s)) ? _SHR_PA_SPEED_53GB : \
                              (50000 == (s)) ? _SHR_PA_SPEED_50GB : \
                              (42000 == (s)) ? _SHR_PA_SPEED_42GB : \
                              (40000 == (s)) ? _SHR_PA_SPEED_40GB : \
                              (32000 == (s)) ? _SHR_PA_SPEED_32GB : \
                              (30000 == (s)) ? _SHR_PA_SPEED_30GB : \
                              (27000 == (s)) ? _SHR_PA_SPEED_27GB : \
                              (25000 == (s)) ? _SHR_PA_SPEED_25GB : \
                              (24000 == (s)) ? _SHR_PA_SPEED_24GB : \
                              (23000 == (s)) ? _SHR_PA_SPEED_23GB : \
                              (21000 == (s)) ? _SHR_PA_SPEED_21GB : \
                              (20000 == (s)) ? _SHR_PA_SPEED_20GB : \
                              (16000 == (s)) ? _SHR_PA_SPEED_16GB : \
                              (15000 == (s)) ? _SHR_PA_SPEED_15GB : \
                              (13000 == (s)) ? _SHR_PA_SPEED_13GB : \
                              (12500 == (s)) ? _SHR_PA_SPEED_12P5GB : \
                              (12000 == (s)) ? _SHR_PA_SPEED_12GB : \
                              (11000 == (s)) ? _SHR_PA_SPEED_11GB : \
                              (10000 == (s)) ? _SHR_PA_SPEED_10GB : \
                              (6000  == (s)) ? _SHR_PA_SPEED_6000MB : \
                              (5000  == (s)) ? _SHR_PA_SPEED_5000MB : \
                              (3000  == (s)) ? _SHR_PA_SPEED_3000MB : \
                              (2500  == (s)) ? _SHR_PA_SPEED_2500MB : \
                              (1000  == (s)) ? _SHR_PA_SPEED_1000MB : \
                              (100   == (s)) ? _SHR_PA_SPEED_100MB : \
                              (10    == (s)) ? _SHR_PA_SPEED_10MB : 0)

#define _SHR_PA_IS_HG_SPEED(m)   ((m) & (_SHR_PA_SPEED_127GB |        \
                                         _SHR_PA_SPEED_106GB |        \
                                         _SHR_PA_SPEED_53GB |         \
                                         _SHR_PA_SPEED_42GB |         \
                                         _SHR_PA_SPEED_32GB |         \
                                         _SHR_PA_SPEED_27GB |         \
                                         _SHR_PA_SPEED_24GB |         \
                                         _SHR_PA_SPEED_23GB |         \
                                         _SHR_PA_SPEED_21GB |         \
                                         _SHR_PA_SPEED_16GB |         \
                                         _SHR_PA_SPEED_15GB |         \
                                         _SHR_PA_SPEED_13GB |         \
                                         _SHR_PA_SPEED_12P5GB |       \
                                         _SHR_PA_SPEED_12GB |         \
                                         _SHR_PA_SPEED_11GB))

/*
 * Defines:
 *      _SHR_PA_EEE_*
 * Purpose:
 *      Defines for EEE
 */
#define _SHR_PA_EEE_100MB_BASETX       (1 << 0)    /* EEE for 100M-BaseTX */
#define _SHR_PA_EEE_1GB_BASET          (1 << 1)    /* EEE for 1G-BaseT */
#define _SHR_PA_EEE_10GB_BASET         (1 << 2)    /* EEE for 10G-BaseT */
#define _SHR_PA_EEE_10GB_KX            (1 << 3)    /* EEE for 10G-KX */
#define _SHR_PA_EEE_10GB_KX4           (1 << 4)    /* EEE for 10G-KX4 */
#define _SHR_PA_EEE_10GB_KR            (1 << 5)    /* EEE for 10G-KR */

#define _SHR_PA_EEE_STRING       {"100M-BaseTX", \
                                 "1G-BaseT", \
                                 "10G-BaseT", \
                                 "10G-KX", \
                                 "10G-KX4", \
                                 "10G-KR"}

/* Backplane multiplexing type Polaris/Sirius */
#define  _SHR_PORT_ABILITY_SFI      1
#define  _SHR_PORT_ABILITY_DUAL_SFI 2
#define  _SHR_PORT_ABILITY_SCI      3
#define  _SHR_PORT_ABILITY_SFI_SCI  4


#endif  /* !_SHR_PORTABILITY_H */
