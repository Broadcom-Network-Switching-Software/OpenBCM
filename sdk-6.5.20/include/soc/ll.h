/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ll.h
 * Purpose:     Link Layer interface (MAC and PHY)
 */

#ifndef _SOC_LL_H
#define _SOC_LL_H

#include <shared/port.h>
#include <shared/phyconfig.h>

#include <soc/defs.h>
#include <soc/macipadr.h>
#include <soc/portmode.h>
#include <soc/port_ability.h>
#include <soc/phyctrl.h>

/* Device specific MAC control settings */
typedef enum soc_mac_control_e {
    SOC_MAC_CONTROL_RX_SET,
    SOC_MAC_CONTROL_TX_SET,
    SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
    SOC_MAC_CONTROL_SW_RESET,
    SOC_MAC_CONTROL_TIMESTAMP_TRANSMIT,
    SOC_MAC_CONTROL_DISABLE_PHY,
    SOC_MAC_PASS_CONTROL_FRAME,
    SOC_MAC_CONTROL_PFC_TYPE,
    SOC_MAC_CONTROL_PFC_OPCODE,
    SOC_MAC_CONTROL_PFC_CLASSES,
    SOC_MAC_CONTROL_PFC_MAC_DA_OUI,
    SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI,
    SOC_MAC_CONTROL_PFC_RX_PASS,
    SOC_MAC_CONTROL_PFC_RX_ENABLE,
    SOC_MAC_CONTROL_PFC_TX_ENABLE,
    SOC_MAC_CONTROL_PFC_FORCE_XON,
    SOC_MAC_CONTROL_PFC_STATS_ENABLE,
    SOC_MAC_CONTROL_PFC_REFRESH_TIME,
    SOC_MAC_CONTROL_PFC_XOFF_TIME,
    SOC_MAC_CONTROL_LLFC_RX_ENABLE,
    SOC_MAC_CONTROL_LLFC_TX_ENABLE,
    SOC_MAC_CONTROL_EEE_ENABLE,
    SOC_MAC_CONTROL_EEE_TX_IDLE_TIME,
    SOC_MAC_CONTROL_EEE_TX_WAKE_TIME,
    SOC_MAC_CONTROL_EEE_LINK_ACTIVE_DURATION,
    SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE,
    SOC_MAC_CONTROL_FAULT_LOCAL_STATUS,
    SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE,
    SOC_MAC_CONTROL_FAULT_REMOTE_STATUS,
    SOC_MAC_CONTROL_PAD_SIZE,
    SOC_MAC_CONTROL_FAILOVER_RX_SET,
    SOC_MAC_CONTROL_FAULT_REMOTE_TX_ENABLE,
    SOC_MAC_CONTROL_EGRESS_DRAIN,
    SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID,
    SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID,
    SOC_MAC_CONTROL_EXPECTED_RX_LATENCY,
    SOC_MAC_CONTROL_RX_RUNT_THRESHOLD,
    SOC_MAC_CONTROL_COUNT            /* always last */
} soc_mac_control_t;

typedef struct soc_port_timesync_tx_info_s {
    uint32 timestamps_in_fifo; /**< low 32bit of Timestamp in Fifo */
    uint32 timestamps_in_fifo_hi; /**< high 32 bit of timestamp in Fifo */
    uint32 sequence_id; /**< sequence id of sent ptp packet */
} soc_port_timesync_tx_info_t;

/*
 * Typedef:
 *      mac_driver_t
 * Purpose:
 *      Maps out entry points into MAC drivers.
 * Notes:
 *      Supplied functions are:
 *
 *      md_init       - Initialize the MAC to a known good state.
 *      md_enable_set - Enable MAC or disable and quiesce MAC.
 *      md_enable_get - Return current MAC enable state.
 *      md_duplex_set - Set the current operating duplex mode for the MAC.
 *      md_duplex_get - Return the current operating duplex mode for the MAC.
 *      md_speed_set  - Set the current operating speed for the MAC
 *      md_speed_get  - Return the current operating speed for the MAC.
 *      md_pause_set  - Set current flow control mode.
 *      md_pause_get  - Return current flow control mode.
 *      md_pause_addr_set - Set current flow control address.
 *      md_pause_addr_get - Return current flow control address.
 *      md_lb_set     - Set current Loopback mode (enable/disable) for MAC.
 *      md_lb_get     - Return current Loopback mode for MAC.
 *      md_interface_set - set MAC-PHY interface type (SOC_PORT_IF_*)
 *      md_interface_get - retrieve MAC-PHY interface type
 *      md_ability    - Return mask of MAC abilities (from portmode.h)
 *      md_frame_max_set - Set the maximum receive frame size
 *      md_frame_max_get - Return the current maximum receive frame size
 *      md_ifg_set    - set the inter-frame gap for the specified speed/duplex
 *      md_ifg_get    - get the inter-frame gap for the specified speed/duplex
 *      md_encap_set  - select port encapsulation mode (RAW/IEEE/HG/HG2)
 *      md_encap_get  - get current port encapsulation mode
 *      md_control_set- set device specific controls
 *      md_control_get- get device specific controls
 *      md_timesync_tx_info_get - get timestamp and sequence id for two step timesync
 *
 * All routines return SOC_E_XXX values.
 * Null pointers for routines will cause SOC_E_UNAVAIL to be returned.
 *
 * Macros are provided to access the fields.
 */
typedef struct mac_driver_s {
    char        *drv_name;
    int         (*md_init)(int, soc_port_t);
    int         (*md_enable_set)(int, soc_port_t, int);
    int         (*md_enable_get)(int, soc_port_t, int *);
    int         (*md_duplex_set)(int, soc_port_t, int);
    int         (*md_duplex_get)(int, soc_port_t, int *);
    int         (*md_speed_set)(int, soc_port_t, int);
    int         (*md_speed_get)(int, soc_port_t, int *);
    int         (*md_pause_set)(int, soc_port_t, int, int);
    int         (*md_pause_get)(int, soc_port_t, int *, int *);
    int         (*md_pause_addr_set)(int, soc_port_t, sal_mac_addr_t);
    int         (*md_pause_addr_get)(int, soc_port_t, sal_mac_addr_t);
    int         (*md_lb_set)(int, soc_port_t, int);
    int         (*md_lb_get)(int, soc_port_t, int *);
    int         (*md_interface_set)(int, soc_port_t, soc_port_if_t);
    int         (*md_interface_get)(int, soc_port_t, soc_port_if_t *);
    int         (*md_ability_get)(int, soc_port_t, soc_port_mode_t *);
    int         (*md_frame_max_set)(int, soc_port_t, int);
    int         (*md_frame_max_get)(int, soc_port_t, int *);
    int         (*md_ifg_set)(int, soc_port_t, int, soc_port_duplex_t, int);
    int         (*md_ifg_get)(int, soc_port_t, int, soc_port_duplex_t, int *);
    int         (*md_encap_set)(int, soc_port_t, int);
    int         (*md_encap_get)(int, soc_port_t, int *);
    int         (*md_control_set)(int, soc_port_t, soc_mac_control_t, int);
    int         (*md_control_get)(int, soc_port_t, soc_mac_control_t, int *);
    int         (*md_ability_local_get)(int, soc_port_t, soc_port_ability_t *);
    int         (*md_timesync_tx_info_get)(int, soc_port_t, soc_port_timesync_tx_info_t *);
} mac_driver_t;

#define _MAC_CALL(_md, _mf, _ma) \
        ((_md) == 0 ? SOC_E_PARAM : \
         ((_md)->_mf == 0 ? SOC_E_UNAVAIL : (_md)->_mf _ma))

#define MAC_INIT(_md, _u, _p) \
        _MAC_CALL((_md), md_init, ((_u), (_p)))

#define MAC_ENABLE_SET(_md, _u, _p, _e) \
        _MAC_CALL((_md), md_enable_set, ((_u), (_p), (_e)))

#define MAC_ENABLE_GET(_md, _u, _p, _e) \
        _MAC_CALL((_md), md_enable_get, ((_u), (_p), (_e)))

#define MAC_DUPLEX_SET(_md, _u, _p, _d) \
        _MAC_CALL((_md), md_duplex_set, ((_u), (_p), (_d)))

#define MAC_DUPLEX_GET(_md, _u, _p, _d) \
        _MAC_CALL((_md), md_duplex_get, ((_u), (_p), (_d)))

#define MAC_SPEED_SET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_speed_set, ((_u), (_p), (_s)))

#define MAC_SPEED_GET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_speed_get, ((_u), (_p), (_s)))

#define MAC_PAUSE_SET(_md, _u, _p, _tx, _rx) \
        _MAC_CALL((_md), md_pause_set, ((_u), (_p), (_tx), (_rx)))

#define MAC_PAUSE_GET(_md, _u, _p, _tx, _rx) \
        _MAC_CALL((_md), md_pause_get, ((_u), (_p), (_tx), (_rx)))

#define MAC_PAUSE_ADDR_SET(_md, _u, _p, _m) \
        _MAC_CALL((_md), md_pause_addr_set, ((_u), (_p), (_m)))

#define MAC_PAUSE_ADDR_GET(_md, _u, _p, _m) \
        _MAC_CALL((_md), md_pause_addr_get, ((_u), (_p), (_m)))

#define MAC_LOOPBACK_SET(_md, _u, _p, _l) \
        _MAC_CALL((_md), md_lb_set, ((_u), (_p), (_l)))

#define MAC_LOOPBACK_GET(_md, _u, _p, _l) \
        _MAC_CALL((_md), md_lb_get, ((_u), (_p), (_l)))

#define MAC_INTERFACE_SET(_md, _u, _p, _i) \
        _MAC_CALL((_md), md_interface_set, ((_u), (_p), (_i)))

#define MAC_INTERFACE_GET(_md, _u, _p, _i) \
        _MAC_CALL((_md), md_interface_get, ((_u), (_p), (_i)))

#define MAC_ABILITY_GET(_md, _u, _p, _a) \
        _MAC_CALL((_md), md_ability_get, ((_u), (_p), (_a)))

#define MAC_ABILITY_LOCAL_GET(_md, _u, _p, _a) \
        _MAC_CALL((_md), md_ability_local_get, ((_u), (_p), (_a)))

#define MAC_FRAME_MAX_SET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_frame_max_set, ((_u), (_p), (_s)))

#define MAC_FRAME_MAX_GET(_md, _u, _p, _s) \
        _MAC_CALL((_md), md_frame_max_get, ((_u), (_p), (_s)))

#define MAC_IFG_SET(_md, _u, _p, _s, _d, _b) \
        _MAC_CALL((_md), md_ifg_set, ((_u), (_p), (_s), (_d), (_b)))

#define MAC_IFG_GET(_md, _u, _p, _s, _d, _b) \
        _MAC_CALL((_md), md_ifg_get, ((_u), (_p), (_s), (_d), (_b)))

#define MAC_ENCAP_SET(_md, _u, _p, _m) \
        _MAC_CALL((_md), md_encap_set, ((_u), (_p), (_m)))

#define MAC_ENCAP_GET(_md, _u, _p, _m) \
        _MAC_CALL((_md), md_encap_get, ((_u), (_p), (_m)))

#define MAC_CONTROL_SET(_md, _u, _p, _t, _v) \
        _MAC_CALL((_md), md_control_set, ((_u), (_p), (_t), (_v)))

#define MAC_CONTROL_GET(_md, _u, _p, _t, _v) \
        _MAC_CALL((_md), md_control_get, ((_u), (_p), (_t), (_v)))

#define MAC_TIMESYNC_TX_INFO_GET(_md, _u, _p, _info) \
        _MAC_CALL((_md), md_timesync_tx_info_get, ((_u), (_p), (_info)))

extern mac_driver_t soc_mac_ge;   /* GEMAC */
extern mac_driver_t soc_mac_fe;   /* FEMAC */
extern mac_driver_t soc_mac_big;  /* BigMAC */
extern mac_driver_t soc_mac_uni;  /* UniMAC */
extern mac_driver_t soc_mac_x;    /* XMAC */
extern mac_driver_t soc_mac_xl;   /* XLMAC */
extern mac_driver_t soc_mac_c;    /* CMAC */
extern mac_driver_t soc_mac_cl;   /* CLMAC */
extern mac_driver_t soc_mac_gx;   /* 12G/10G/2.5G/1G combo MAC */
extern mac_driver_t soc_mac_combo; /* generic combo MAC */
extern mac_driver_t soc_mac_il;   /* Interlaken */
extern mac_driver_t soc_mac_ilkn; /* C3 Interlaken */

/*
 * Functions for selecting the MAC, and determining speed.
 */

extern  int     soc_mac_probe(int unit, soc_port_t port, mac_driver_t **);

typedef enum soc_mac_mode_e {
    SOC_MAC_MODE_10_100,                /* 10/100 Mb/s MAC selected */
    SOC_MAC_MODE_10,                    /* 10/100 Mb/s MAC in 10Mbps mode */
    SOC_MAC_MODE_1000_T,                /* 1000/TURBO MAC selected */
    SOC_MAC_MODE_10000,                 /* 10G MAC selected */
    SOC_MAC_MODE_100000                 /* 100G MAC selected */
} soc_mac_mode_t;
extern  int     soc_mac_mode_set(int unit, soc_port_t port,
                                 soc_mac_mode_t mode);
extern  int     soc_mac_mode_get(int unit, soc_port_t port,
                                 soc_mac_mode_t *mode);
extern  int     soc_mac_speed_get(int unit, soc_port_t port, int *speed);

/* Media Access Controller Ethernet Frame Encapsulation mode */
typedef _shr_port_encap_t soc_encap_mode_t;

#define SOC_ENCAP_IEEE                 _SHR_PORT_ENCAP_IEEE
#define SOC_ENCAP_HIGIG                _SHR_PORT_ENCAP_HIGIG
#define SOC_ENCAP_B5632                _SHR_PORT_ENCAP_B5632
#define SOC_ENCAP_HIGIG2               _SHR_PORT_ENCAP_HIGIG2
#define SOC_ENCAP_HIGIG2_LITE          _SHR_PORT_ENCAP_HIGIG2_LITE
#define SOC_ENCAP_HIGIG2_L2            _SHR_PORT_ENCAP_HIGIG2_L2
#define SOC_ENCAP_HIGIG2_IP_GRE        _SHR_PORT_ENCAP_HIGIG2_IP_GRE
#define SOC_ENCAP_OBSOLETE             _SHR_PORT_ENCAP_OBSOLETE
#define SOC_ENCAP_SOP_ONLY             _SHR_PORT_ENCAP_PREAMBLE_SOP_ONLY 
#define SOC_ENCAP_CPRI                 _SHR_PORT_ENCAP_CPRI
#define SOC_ENCAP_RSVD4                _SHR_PORT_ENCAP_RSVD4
#define SOC_ENCAP_COUNT                _SHR_PORT_ENCAP_COUNT

#define SOC_ENCAP_MODE_NAMES_INITIALIZER _SHR_PORT_ENCAP_NAMES_INITIALIZER

extern  int     soc_autoz_set(int unit, soc_port_t port, int enable);
extern  int     soc_autoz_get(int unit, soc_port_t port, int *enable);

#endif  /* !_SOC_LL_H */
