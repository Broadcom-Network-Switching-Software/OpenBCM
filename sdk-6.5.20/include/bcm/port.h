/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PORT_H__
#define __BCM_PORT_H__

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>
#include <shared/phyreg.h>
#include <shared/switch.h>
#include <bcm/types.h>
#include <bcm/stat.h>
#include <shared/port_ability.h>

#define BCM_PIPES_MAX           _SHR_SWITCH_MAX_PIPES 

#define BCM_PORT_WIDE_DATA_WIDTH    57         

#define BCM_PORT_WIDE_DATA_STRENGTH_SET(_data, _strength)  \
	  COMPILER_64_SET(_data,( (_strength ) ? ( COMPILER_64_HI(_data) | (1 << ((BCM_PORT_WIDE_DATA_WIDTH -1) -32))) :  (COMPILER_64_HI(_data) & ~( 1 << ((BCM_PORT_WIDE_DATA_WIDTH -1) -32)))) ,COMPILER_64_LO(_data)) 

#define BCM_PORT_WIDE_DATA_STRENGTH_GET(_data)  \
    (COMPILER_64_HI(_data)  >> ((BCM_PORT_WIDE_DATA_WIDTH -1) -32) ) 

/* Port Configuration structure. */
typedef struct bcm_port_config_s {
    bcm_pbmp_t fe;                      /* Mask of FE ports. */
    bcm_pbmp_t ge;                      /* Mask of GE ports. */
    bcm_pbmp_t xe;                      /* Mask of 10gig ports. */
    bcm_pbmp_t ce;                      /* Mask of 100gig ports. */
    bcm_pbmp_t le;                      /* Mask of 50gig ports. */
    bcm_pbmp_t cc;                      /* Mask of 200gig ports. */
    bcm_pbmp_t cd;                      /* Mask of 400gig ports. */
    bcm_pbmp_t e;                       /* Mask of eth ports. */
    bcm_pbmp_t hg;                      /* Mask of HiGig ports. */
    bcm_pbmp_t sci;                     /* Mask of SCI ports. */
    bcm_pbmp_t sfi;                     /* Mask of SFI ports. */
    bcm_pbmp_t spi;                     /* Mask of SPI ports. */
    bcm_pbmp_t spi_subport;             /* Mask of SPI subports. */
    bcm_pbmp_t port;                    /* Mask of all front panel ports. */
    bcm_pbmp_t cpu;                     /* Mask of CPU ports. */
    bcm_pbmp_t all;                     /* Mask of all ports. */
    bcm_pbmp_t stack_int;               /* Deprecated - unused. */
    bcm_pbmp_t stack_ext;               /* Mask of Stack ports. */
    bcm_pbmp_t tdm;                     /* Mask of TDM ports. */
    bcm_pbmp_t pon;                     /* Mask of PON ports. */
    bcm_pbmp_t llid;                    /* Mask of LLID ports. */
    bcm_pbmp_t il;                      /* Mask of ILKN ports. */
    bcm_pbmp_t xl;                      /* Mask of XLAUI ports. */
    bcm_pbmp_t rcy;                     /* Mask of RECYCLE ports. */
    bcm_pbmp_t sat;                     /* Mask of SAT ports. */
    bcm_pbmp_t ipsec;                   /* Mask of IPSEC ports. */
    bcm_pbmp_t per_pipe[BCM_PIPES_MAX]; /* Mask of ports per pipe. The number of
                                           pipes per device can be obtained via
                                           num_pipes field of bcm_info_t. */
    bcm_pbmp_t nif;                     /* Mask of Network Interfaces ports. */
    bcm_pbmp_t control;                 /* Mask of hot swap controlling ports. */
    bcm_pbmp_t eventor;                 /* Mask of Eventor ports. */
    bcm_pbmp_t olp;                     /* Mask of OLP ports. */
    bcm_pbmp_t oamp;                    /* Mask of OAMP ports. */
    bcm_pbmp_t erp;                     /* Mask of ERP ports. */
    bcm_pbmp_t roe;                     /* Mask of ROE ports. */
    bcm_pbmp_t rcy_mirror;              /* Mask of mirror recycle ports (sniff) */
    bcm_pbmp_t per_pp_pipe[BCM_PP_PIPES_MAX]; /* Mask of ports per PP pipe. The number
                                           of PP pipes per device can be
                                           obtained via num_pp_pipes field of
                                           bcm_info_t. */
    bcm_pbmp_t stat;                    /* Mask of stat ports. */
    bcm_pbmp_t crps;                    /* Mask of crps ports. */
} bcm_port_config_t;

/* Port destination info structure. */
typedef struct bcm_port_dest_info_s {
    uint32 flags;       /* Future expansion. */
    bcm_gport_t gport;  /* e.g. sysport gport, unicast gport, multicast gport,
                           etc... */
    bcm_cos_t priority; /* Traffic class (0-7). */
    bcm_color_t dp;     /* Drop precedence (0-3). */
} bcm_port_dest_info_t;

/* Port ability */
typedef _shr_port_ability_t bcm_port_ability_t;

#define BCM_PORT_ABILITY_10MB               _SHR_PA_SPEED_10MB 
#define BCM_PORT_ABILITY_100MB              _SHR_PA_SPEED_100MB 
#define BCM_PORT_ABILITY_1000MB             _SHR_PA_SPEED_1000MB 
#define BCM_PORT_ABILITY_2500MB             _SHR_PA_SPEED_2500MB 
#define BCM_PORT_ABILITY_3000MB             _SHR_PA_SPEED_3000MB 
#define BCM_PORT_ABILITY_5000MB             _SHR_PA_SPEED_5000MB 
#define BCM_PORT_ABILITY_6000MB             _SHR_PA_SPEED_6000MB 
#define BCM_PORT_ABILITY_10GB               _SHR_PA_SPEED_10GB 
#define BCM_PORT_ABILITY_11GB               _SHR_PA_SPEED_11GB 
#define BCM_PORT_ABILITY_12GB               _SHR_PA_SPEED_12GB 
#define BCM_PORT_ABILITY_12P5GB             _SHR_PA_SPEED_12P5GB 
#define BCM_PORT_ABILITY_13GB               _SHR_PA_SPEED_13GB 
#define BCM_PORT_ABILITY_15GB               _SHR_PA_SPEED_15GB 
#define BCM_PORT_ABILITY_16GB               _SHR_PA_SPEED_16GB 
#define BCM_PORT_ABILITY_20GB               _SHR_PA_SPEED_20GB 
#define BCM_PORT_ABILITY_21GB               _SHR_PA_SPEED_21GB 
#define BCM_PORT_ABILITY_23GB               _SHR_PA_SPEED_23GB 
#define BCM_PORT_ABILITY_24GB               _SHR_PA_SPEED_24GB 
#define BCM_PORT_ABILITY_25GB               _SHR_PA_SPEED_25GB 
#define BCM_PORT_ABILITY_27GB               _SHR_PA_SPEED_27GB 
#define BCM_PORT_ABILITY_30GB               _SHR_PA_SPEED_30GB 
#define BCM_PORT_ABILITY_32GB               _SHR_PA_SPEED_32GB 
#define BCM_PORT_ABILITY_40GB               _SHR_PA_SPEED_40GB 
#define BCM_PORT_ABILITY_42GB               _SHR_PA_SPEED_42GB 
#define BCM_PORT_ABILITY_48GB               _SHR_PA_SPEED_48GB 
#define BCM_PORT_ABILITY_50GB               _SHR_PA_SPEED_50GB 
#define BCM_PORT_ABILITY_53GB               _SHR_PA_SPEED_53GB 
#define BCM_PORT_ABILITY_100GB              _SHR_PA_SPEED_100GB 
#define BCM_PORT_ABILITY_106GB              _SHR_PA_SPEED_106GB 
#define BCM_PORT_ABILITY_120GB              _SHR_PA_SPEED_120GB 
#define BCM_PORT_ABILITY_127GB              _SHR_PA_SPEED_127GB 
#define BCM_PORT_ABILITY_INTERFACE_TBI      _SHR_PA_INTF_TBI 
#define BCM_PORT_ABILITY_INTERFACE_MII      _SHR_PA_INTF_MII 
#define BCM_PORT_ABILITY_INTERFACE_GMII     _SHR_PA_INTF_GMII 
#define BCM_PORT_ABILITY_INTERFACE_SGMII    _SHR_PA_INTF_SGMII 
#define BCM_PORT_ABILITY_INTERFACE_XGMII    _SHR_PA_INTF_XGMII 
#define BCM_PORT_ABILITY_INTERFACE_QSGMII   _SHR_PA_INTF_QSGMII 
#define BCM_PORT_ABILITY_INTERFACE_CGMII    _SHR_PA_INTF_CGMII 
#define BCM_PORT_ABILITY_MEDIUM_COPPER      _SHR_PA_MEDIUM_COPPER 
#define BCM_PORT_ABILITY_MEDIUM_FIBER       _SHR_PA_MEDIUM_FIBER 
#define BCM_PORT_ABILITY_MEDIUM_BACKPLANE   _SHR_PA_MEDIUM_BACKPLANE 
#define BCM_PORT_ABILITY_LB_NONE            _SHR_PA_LB_NONE 
#define BCM_PORT_ABILITY_LB_MAC             _SHR_PA_LB_MAC 
#define BCM_PORT_ABILITY_LB_PHY             _SHR_PA_LB_PHY 
#define BCM_PORT_ABILITY_LB_LINE            _SHR_PA_LB_LINE 
#define BCM_PORT_ABILITY_AUTONEG            _SHR_PA_AUTONEG 
#define BCM_PORT_ABILITY_COMBO              _SHR_PA_COMBO 
#define BCM_PORT_ABILITY_PAUSE_TX           _SHR_PA_PAUSE_TX 
#define BCM_PORT_ABILITY_PAUSE_RX           _SHR_PA_PAUSE_RX 
#define BCM_PORT_ABILITY_PAUSE              _SHR_PA_PAUSE /* Both TX and RX. */
#define BCM_PORT_ABILITY_PAUSE_ASYMM        _SHR_PA_PAUSE_ASYMM /* The following is used
                                                          only by
                                                          bcm_port_ability_get,
                                                          and indicates that a
                                                          port can support
                                                          having PAUSE_TX be
                                                          different than
                                                          PAUSE_RX. */
#define BCM_PORT_ABILITY_FEC_NONE           _SHR_PA_FEC_NONE /* No FEC request */
#define BCM_PORT_ABILITY_FEC_CL74           _SHR_PA_FEC_CL74 /* FEC CL74  request. */
#define BCM_PORT_ABILITY_FEC_CL91           _SHR_PA_FEC_CL91 /* FEC CL91  request. */
#define BCM_PORT_ABILITY_EEE_100MB_BASETX   _SHR_PA_EEE_100MB_BASETX /* EEE ability at
                                                          100M-BaseTX. */
#define BCM_PORT_ABILITY_EEE_1GB_BASET      _SHR_PA_EEE_1GB_BASET /* EEE ability at
                                                          1G-BaseT. */
#define BCM_PORT_ABILITY_EEE_10GB_BASET     _SHR_PA_EEE_10GB_BASET /* EEE ability at
                                                          10G-BaseT. */
#define BCM_PORT_ABILITY_EEE_10GB_KX        _SHR_PA_EEE_10GB_KX /* EEE ability at
                                                          10GB-KX. */
#define BCM_PORT_ABILITY_EEE_10GB_KX4       _SHR_PA_EEE_10GB_KX4 /* EEE ability at
                                                          10GB-KX4. */
#define BCM_PORT_ABILITY_EEE_10GB_KR        _SHR_PA_EEE_10GB_KR /* EEE ability at
                                                          10GB-KR. */
#define BCM_PORT_ABILITY_CHANNEL_LONG       _SHR_PA_CHANNEL_LONG /* port is long channel. */
#define BCM_PORT_ABILITY_CHANNEL_SHORT      _SHR_PA_CHANNEL_SHORT /* port is short channel. */

/* 
 * Port ability mask.
 * 
 * The following flags are used to indicate which set of capabilities are
 * provided by a PHY or MAC when retrieving the ability of a port,
 * setting or getting the local advertisement, getting the remote
 * advertisement, or setting the MAC encapsulation and/or CRC modes.
 */
typedef _shr_port_mode_t bcm_port_abil_t;

#define BCM_PORT_ABIL_10MB_HD       _SHR_PM_10MB_HD 
#define BCM_PORT_ABIL_10MB_FD       _SHR_PM_10MB_FD 
#define BCM_PORT_ABIL_100MB_HD      _SHR_PM_100MB_HD 
#define BCM_PORT_ABIL_100MB_FD      _SHR_PM_100MB_FD 
#define BCM_PORT_ABIL_1000MB_HD     _SHR_PM_1000MB_HD 
#define BCM_PORT_ABIL_1000MB_FD     _SHR_PM_1000MB_FD 
#define BCM_PORT_ABIL_2500MB_HD     _SHR_PM_2500MB_HD 
#define BCM_PORT_ABIL_2500MB_FD     _SHR_PM_2500MB_FD 
#define BCM_PORT_ABIL_3000MB_HD     _SHR_PM_3000MB_HD 
#define BCM_PORT_ABIL_3000MB_FD     _SHR_PM_3000MB_FD 
#define BCM_PORT_ABIL_10GB_HD       _SHR_PM_10GB_HD 
#define BCM_PORT_ABIL_10GB_FD       _SHR_PM_10GB_FD 
#define BCM_PORT_ABIL_12GB_HD       _SHR_PM_12GB_HD 
#define BCM_PORT_ABIL_12GB_FD       _SHR_PM_12GB_FD 
#define BCM_PORT_ABIL_13GB_HD       _SHR_PM_13GB_HD 
#define BCM_PORT_ABIL_13GB_FD       _SHR_PM_13GB_FD 
#define BCM_PORT_ABIL_16GB_HD       _SHR_PM_16GB_HD 
#define BCM_PORT_ABIL_16GB_FD       _SHR_PM_16GB_FD 
#define BCM_PORT_ABIL_TBI           _SHR_PM_TBI 
#define BCM_PORT_ABIL_MII           _SHR_PM_MII 
#define BCM_PORT_ABIL_GMII          _SHR_PM_GMII 
#define BCM_PORT_ABIL_SGMII         _SHR_PM_SGMII 
#define BCM_PORT_ABIL_XGMII         _SHR_PM_XGMII 
#define BCM_PORT_ABIL_LB_MAC        _SHR_PM_LB_MAC 
#define BCM_PORT_ABIL_LB_PHY        _SHR_PM_LB_PHY 
#define BCM_PORT_ABIL_LB_NONE       _SHR_PM_LB_NONE 
#define BCM_PORT_ABIL_AN            _SHR_PM_AN 
#define BCM_PORT_ABIL_COMBO         _SHR_PM_COMBO 
#define BCM_PORT_ABIL_PAUSE_TX      _SHR_PM_PAUSE_TX 
#define BCM_PORT_ABIL_PAUSE_RX      _SHR_PM_PAUSE_RX 
#define BCM_PORT_ABIL_PAUSE         _SHR_PM_PAUSE /* Both TX and RX. */
#define BCM_PORT_ABIL_10B           BCM_PORT_ABIL_TBI /* Deprecated */
#define BCM_PORT_ABIL_PAUSE_ASYMM   _SHR_PM_PAUSE_ASYMM /* The following is used only by
                                                  bcm_port_ability_get, and
                                                  indicates that a port can
                                                  support having PAUSE_TX be
                                                  different than PAUSE_RX. */
#define BCM_PORT_ABIL_10MB          _SHR_PM_10MB 
#define BCM_PORT_ABIL_100MB         _SHR_PM_100MB 
#define BCM_PORT_ABIL_1000MB        _SHR_PM_1000MB 
#define BCM_PORT_ABIL_2500MB        _SHR_PM_2500MB 
#define BCM_PORT_ABIL_3000MB        _SHR_PM_3000MB 
#define BCM_PORT_ABIL_10GB          _SHR_PM_10GB 
#define BCM_PORT_ABIL_12GB          _SHR_PM_12GB 
#define BCM_PORT_ABIL_13GB          _SHR_PM_13GB 
#define BCM_PORT_ABIL_16GB          _SHR_PM_16GB 
#define BCM_PORT_ABIL_HD            _SHR_PM_HD 
#define BCM_PORT_ABIL_FD            _SHR_PM_FD 
#define BCM_PORT_ABIL_SPD_ANY       _SHR_PM_SPEED_ALL 

#define BCM_PORT_ABIL_SPD_MAX(abil)  \
    _SHR_PM_SPEED_MAX(abil) 

#define BCM_PORT_ABILITY_SPEED_MAX(abil)  _SHR_PA_SPEED_MAX(abil) 

#define BCM_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_0 _SHR_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_0 
#define BCM_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_1 _SHR_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_1 
#define BCM_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_2 _SHR_PORT_CONTROL_FABRIC_HEADER_OBSOLETE_2 
#define BCM_PORT_CONTROL_FABRIC_HEADER_CUSTOM _SHR_PORT_CONTROL_FABRIC_HEADER_CUSTOM 
#define BCM_PORT_CONTROL_FABRIC_HEADER_88640_TM _SHR_PORT_CONTROL_FABRIC_HEADER_88640_TM 
#define BCM_PORT_CONTROL_FABRIC_HEADER_88640_RAW _SHR_PORT_CONTROL_FABRIC_HEADER_88640_RAW 

/* Port Management packet Classification flags. */
#define BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE 0x01       /* If set, ether type is
                                                          valid in the
                                                          configuration
                                                          structure. */
#define BCM_PORT_MANAGEMENT_PACKET_DEST_MAC 0x02       /* If set, Destination
                                                          MAC is valid in the
                                                          configuration
                                                          structure. */
#define BCM_PORT_MANAGEMENT_PACKET_CLEAR    0x80       /* If set, Destination
                                                          MAC and or EtherType
                                                          to be cleared from
                                                          Management packet
                                                          classification. */

/* Port Management Packet Classification Configuration structure. */
typedef struct bcm_port_management_packet_config_s {
    uint32 flags;               /* See BCM_PORT_MANAGEMENT_XXX flag definitions. */
    bcm_ethertype_t ethertype;  /* Ether Type for packet classification. */
    bcm_mac_t dest_mac;         /* Destination mac address for packet
                                   classification */
} bcm_port_management_packet_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the port subsystem. */
extern int bcm_port_init(
    int unit);

/* 
 * Initialize the port subsystem without affecting the current state of
 * stack ports.
 */
extern int bcm_port_clear(
    int unit);

/* Probe the port to determine the proper MAC and PHY drivers. */
extern int bcm_port_probe(
    int unit, 
    bcm_pbmp_t pbmp, 
    bcm_pbmp_t *okay_pbmp);

/* Detach ports from the BCM API. */
extern int bcm_port_detach(
    int unit, 
    bcm_pbmp_t pbmp, 
    bcm_pbmp_t *detached);

/* Retrieved the port configuration for the specified device. */
extern int bcm_port_config_get(
    int unit, 
    bcm_port_config_t *config);

/* Enable or disable a port. */
extern int bcm_port_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* Enable or disable a port. */
extern int bcm_port_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

/* Set or retrieve autonegotiation settings for a port. */
extern int bcm_port_advert_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_abil_t ability_mask);

/* Set or retrieve auto-negotiation abilities for a port. */
extern int bcm_port_ability_advert_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_ability_t *ability_mask);

/* Set or retrieve autonegotiation settings for a port. */
extern int bcm_port_advert_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_abil_t *ability_mask);

/* Set or retrieve auto-negotiation abilities for a port. */
extern int bcm_port_ability_advert_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_ability_t *ability_mask);

/* Set or retrieve autonegotiation settings for a port. */
extern int bcm_port_advert_remote_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_abil_t *ability_mask);

/* Retrieve the valid abilities of a remote port. */
extern int bcm_port_ability_remote_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_ability_t *ability_mask);

/* 
 * Retrieve the valid operating modes of a port including speed and
 * duplex.
 */
extern int bcm_port_ability_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_abil_t *local_ability_mask);

/* Retrieve the valid abilities of a local port. */
extern int bcm_port_ability_local_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_ability_t *local_ability_mask);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Backward compatibility. */
#define bcm_port_remote_advert_get  bcm_port_advert_remote_get 

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the management packet classification properties. */
extern int bcm_port_management_packet_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_management_packet_config_t *config);

/* Get or set the management packet classification properties. */
extern int bcm_port_management_packet_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_management_packet_config_t *config_array, 
    int max_config, 
    int *config_count);

/* Get or set the default VLAN for packets that ingress untagged. */
extern int bcm_port_untagged_vlan_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vid);

/* Get or set the default VLAN for packets that ingress untagged. */
extern int bcm_port_untagged_vlan_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t *vid_ptr);

/* Get or set the default priority for packets that ingress untagged. */
extern int bcm_port_untagged_priority_set(
    int unit, 
    bcm_port_t port, 
    int priority);

/* Get or set the default priority for packets that ingress untagged. */
extern int bcm_port_untagged_priority_get(
    int unit, 
    bcm_port_t port, 
    int *priority);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_DSCP_MAP_NONE          0          /* Disable DSCP mapping */
#define BCM_PORT_DSCP_MAP_ZERO          1          /* Map only if incoming DSCP
                                                      = 0. */
#define BCM_PORT_DSCP_MAP_ALL           2          /* Map all. */
#define BCM_PORT_DSCP_MAP_UNTAGGED_ONLY 3          /* Map only when packet is
                                                      untagged. */
#define BCM_PORT_DSCP_MAP_DEFAULT       4          /* Map according to default
                                                      mapping,Not looking at PCP
                                                      or TOS. */
#define BCM_PORT_DSCP_MAP_IPV4_ONLY     (1 << 3)   /* Map DSCP only for IPv4
                                                      packets. */
#define BCM_PORT_DSCP_MAP_IPV6_ONLY     (1 << 4)   /* Map DSCP only for IPv6
                                                      packets. */
#define BCM_PORT_DSCP_MAP_IPV4_NONE     (1 << 5)   /* Disable DSCP mapping for
                                                      IPv4 packets. */
#define BCM_PORT_DSCP_MAP_IPV6_NONE     (1 << 6)   /* Disable DSCP mapping for
                                                      IPv6 packets. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Control mapping of Differentiated Services Code Points (DSCP). */
extern int bcm_port_dscp_map_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* Control mapping of Differentiated Services Code Points (DSCP). */
extern int bcm_port_dscp_map_mode_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

/* Control mapping of Differentiated Services Code Points (DSCP) */
extern int bcm_port_dscp_map_set(
    int unit, 
    bcm_port_t port, 
    int srccp, 
    int mapcp, 
    int prio);

/* Control mapping of Differentiated Services Code Points (DSCP) */
extern int bcm_port_dscp_map_get(
    int unit, 
    bcm_port_t port, 
    int srccp, 
    int *mapcp, 
    int *prio);

/* Get or set the current linkscan mode for the specified port. */
extern int bcm_port_linkscan_set(
    int unit, 
    bcm_port_t port, 
    int linkscan);

/* Get or set the current linkscan mode for the specified port. */
extern int bcm_port_linkscan_get(
    int unit, 
    bcm_port_t port, 
    int *linkscan);

/* 
 * Configure or retrieve the current auto-negotiation settings for a
 * port,
 * or restart auto-negotiation if already enabled.
 */
extern int bcm_port_autoneg_set(
    int unit, 
    bcm_port_t port, 
    int autoneg);

/* 
 * Configure or retrieve the current auto-negotiation settings for a
 * port,
 * or restart auto-negotiation if already enabled.
 */
extern int bcm_port_autoneg_get(
    int unit, 
    bcm_port_t port, 
    int *autoneg);

/* Get or set the current operating speed of a port. */
extern int bcm_port_speed_max(
    int unit, 
    bcm_port_t port, 
    int *speed);

/* Get or set the current operating speed of a port. */
extern int bcm_port_speed_set(
    int unit, 
    bcm_port_t port, 
    int speed);

/* Get or set the current operating speed of a port. */
extern int bcm_port_speed_get(
    int unit, 
    bcm_port_t port, 
    int *speed);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_ms_t */
typedef _shr_port_ms_t bcm_port_ms_t;

#define BCM_PORT_MS_SLAVE       _SHR_PORT_MS_SLAVE 
#define BCM_PORT_MS_MASTER      _SHR_PORT_MS_MASTER 
#define BCM_PORT_MS_AUTO        _SHR_PORT_MS_AUTO 
#define BCM_PORT_MS_NONE        _SHR_PORT_MS_NONE 
#define BCM_PORT_MS_COUNT       _SHR_PORT_MS_COUNT 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or get the Master mode on the PHY attached to the specified port. */
extern int bcm_port_master_set(
    int unit, 
    bcm_port_t port, 
    int ms);

/* Set or get the Master mode on the PHY attached to the specified port. */
extern int bcm_port_master_get(
    int unit, 
    bcm_port_t port, 
    int *ms);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_if_t */
typedef _shr_port_if_t bcm_port_if_t;

/* Port properties structure. */
typedef struct bcm_port_interface_config_s {
    uint32 flags;               /* flags of port. */
    uint32 channel;             /* channel of port. */
    uint32 phy_port;            /* related phy port. */
    bcm_port_if_t interface;    /* interface of port. */
} bcm_port_interface_config_t;

/* physical port properties structure. */
typedef struct bcm_port_interface_info_s {
    uint32 phy_port;            /* related phy port. */
    bcm_port_if_t interface;    /* interface of port. */
    uint32 num_lanes;           /* number of lanes the interface uses. */
    bcm_pbmp_t phy_pbmp;        /* bitmap of phy lanes the interface occupies. */
    uint32 interface_id;        /* should be indicated in case the id can't be
                                   derived from phy_port. */
    int max_speed;              /* The supported port max speed via legacy flex
                                   port operations, i.e. the max speed
                                   provisioned through soc property port_map. */
} bcm_port_interface_info_t;

/* port mapping properties structure. */
typedef struct bcm_port_mapping_info_s {
    uint32 channel;             /* logical port channel. */
    int core;                   /* core id */
    uint32 tm_port;             /* tm port id */
    uint32 pp_port;             /* pp port id */
    uint32 base_q_pair;         /* base queue pair id */
    uint32 num_priorities;      /* number of port priorities */
    uint32 base_hr;             /* base hr id */
    uint32 num_sch_priorities;  /* number of sch port priorities */
    uint32 base_uc_queue;       /* base uc queue */
    uint32 num_uc_queue;        /* number of uc queue */
    uint32 base_mc_queue;       /* base mc queue */
    uint32 num_mc_queue;        /* number of mc queue */
} bcm_port_mapping_info_t;

/* Initialize structure bcm_port_mapping_info_t */
extern void bcm_port_mapping_info_t_init(
    bcm_port_mapping_info_t *mapping);

/* Port link state structure. */
typedef struct bcm_port_link_state_s {
    int status;     /* link status, one of BCM_PORT_LINK_STATUS_* */
    int latch_down; /* indicates whether link latch has been down since last
                       call to API. */
} bcm_port_link_state_t;

#define BCM_PORT_IF_NOCXN                   _SHR_PORT_IF_NOCXN 
#define BCM_PORT_IF_NULL                    _SHR_PORT_IF_NULL 
#define BCM_PORT_IF_MII                     _SHR_PORT_IF_MII /* MII Operating mode,
                                                          not supported on all
                                                          ports */
#define BCM_PORT_IF_GMII                    _SHR_PORT_IF_GMII /* GMII Operating mode,
                                                          not supported on all
                                                          ports */
#define BCM_PORT_IF_SGMII                   _SHR_PORT_IF_SGMII 
#define BCM_PORT_IF_TBI                     _SHR_PORT_IF_TBI /* TBI interface
                                                          supported on Gigabit
                                                          Ethernet ports
                                                          operating at gigabit
                                                          speeds */
#define BCM_PORT_IF_XGMII                   _SHR_PORT_IF_XGMII /* 10-Gigabit interface */
#define BCM_PORT_IF_RGMII                   _SHR_PORT_IF_RGMII /* Reduced Gigabit Media
                                                          Independent Interface */
#define BCM_PORT_IF_SFI                     _SHR_PORT_IF_SFI /* SFI 10-Gigabit serial
                                                          electrical interface */
#define BCM_PORT_IF_XFI                     _SHR_PORT_IF_XFI /* XFI 10-Gigabit serial
                                                          electrical interface */
#define BCM_PORT_IF_KR                      _SHR_PORT_IF_KR /* Backplane 10 GbE
                                                          64B/66B interface */
#define BCM_PORT_IF_KR2                     _SHR_PORT_IF_KR2 /* Backplane 2x10GbE
                                                          64B/66B interface */
#define BCM_PORT_IF_KR4                     _SHR_PORT_IF_KR4 /* Backplane 4x10 GbE
                                                          64B/66B interface */
#define BCM_PORT_IF_CR                      _SHR_PORT_IF_CR /* Copper 10 GbE 64B/66B
                                                          interface */
#define BCM_PORT_IF_CR2                     _SHR_PORT_IF_CR2 /* Copper 2x10GbE 64B/66B
                                                          interface */
#define BCM_PORT_IF_CR4                     _SHR_PORT_IF_CR4 /* Copper 4x10 GbE
                                                          64B/66B interface. */
#define BCM_PORT_IF_XLAUI                   _SHR_PORT_IF_XLAUI /* 40 Gigabit Attachment
                                                          Unit Interface */
#define BCM_PORT_IF_XLAUI2                  _SHR_PORT_IF_XLAUI2 /* 40 Gigabit Attachment
                                                          Unit Interface over 2
                                                          lanes */
#define BCM_PORT_IF_RXAUI                   _SHR_PORT_IF_RXAUI 
#define BCM_PORT_IF_XAUI                    _SHR_PORT_IF_XAUI 
#define BCM_PORT_IF_SPAUI                   _SHR_PORT_IF_SPAUI 
#define BCM_PORT_IF_QSGMII                  _SHR_PORT_IF_QSGMII 
#define BCM_PORT_IF_ILKN                    _SHR_PORT_IF_ILKN 
#define BCM_PORT_IF_RCY                     _SHR_PORT_IF_RCY 
#define BCM_PORT_IF_FAT_PIPE                _SHR_PORT_IF_FAT_PIPE 
#define BCM_PORT_IF_SR                      _SHR_PORT_IF_SR /* Fiber SR/LR 64B/66B
                                                          interface */
#define BCM_PORT_IF_SR2                     _SHR_PORT_IF_SR2 /* Fiber 2x10GbE 64B/66B
                                                          interface. */
#define BCM_PORT_IF_CAUI                    _SHR_PORT_IF_CAUI 
#define BCM_PORT_IF_LR                      _SHR_PORT_IF_LR /* Fiber LR 64B/66B
                                                          interface */
#define BCM_PORT_IF_LR4                     _SHR_PORT_IF_LR4 /* Fiber LR4 64B/66B
                                                          interface */
#define BCM_PORT_IF_SR4                     _SHR_PORT_IF_SR4 /* Fiber SR4 64B/66B
                                                          interface */
#define BCM_PORT_IF_KX                      _SHR_PORT_IF_KX 
#define BCM_PORT_IF_ZR                      _SHR_PORT_IF_ZR /* Fiber ZR 64B/66B
                                                          interface */
#define BCM_PORT_IF_SR10                    _SHR_PORT_IF_SR10 /* Fiber SR10 64B/66B
                                                          interface. */
#define BCM_PORT_IF_CR10                    _SHR_PORT_IF_CR10 /* Copper CR10 64B/66B
                                                          interface. */
#define BCM_PORT_IF_KR10                    _SHR_PORT_IF_KR10 /* Backplane KR10 64B/66B
                                                          interface. */
#define BCM_PORT_IF_LR10                    _SHR_PORT_IF_LR10 /* Fiber LR10 64B/66B
                                                          interface. */
#define BCM_PORT_IF_OTL                     _SHR_PORT_IF_OTL /* Fiber 4x25 GbE OTL
                                                          interface */
#define BCM_PORT_IF_CPU                     _SHR_PORT_IF_CPU 
#define BCM_PORT_IF_ER                      _SHR_PORT_IF_ER /* Fiber ER 64B/66B
                                                          interface */
#define BCM_PORT_IF_ER2                     _SHR_PORT_IF_ER2 /* Fiber ER2 2x10G
                                                          64B/66B interface */
#define BCM_PORT_IF_ER4                     _SHR_PORT_IF_ER4 /* Fiber ER4 4x10G
                                                          64B/66B interface */
#define BCM_PORT_IF_CX                      _SHR_PORT_IF_CX /* Copper 10G 10B/8B
                                                          interface */
#define BCM_PORT_IF_CX2                     _SHR_PORT_IF_CX2 /* Copper 2x10G 10B/8B
                                                          interface. */
#define BCM_PORT_IF_CX4                     _SHR_PORT_IF_CX4 /* Copper 4x10G 10B/8B
                                                          interface */
#define BCM_PORT_IF_CAUI_C2C                _SHR_PORT_IF_CAUI_C2C /* CAUI 100G C2C
                                                          interface */
#define BCM_PORT_IF_CAUI_C2M                _SHR_PORT_IF_CAUI_C2M /* CAUI 100G C2M
                                                          interface */
#define BCM_PORT_IF_VSR                     _SHR_PORT_IF_VSR /* Fiber VSR 10G 64B/66B
                                                          interface */
#define BCM_PORT_IF_LR2                     _SHR_PORT_IF_LR2 /* Fiber 2x10Gbe LR
                                                          64B/66B interface */
#define BCM_PORT_IF_LRM                     _SHR_PORT_IF_LRM /* Fiber LRM multipoint
                                                          64B/66B interface */
#define BCM_PORT_IF_XLPPI                   _SHR_PORT_IF_XLPPI /* 40G parallel physical
                                                          interface */
#define BCM_PORT_IF_LBG                     _SHR_PORT_IF_LBG /* Link bonding interface */
#define BCM_PORT_IF_CAUI4                   _SHR_PORT_IF_CAUI4 /* CAUI4 100G interface */
#define BCM_PORT_IF_OAMP                    _SHR_PORT_IF_OAMP 
#define BCM_PORT_IF_OLP                     _SHR_PORT_IF_OLP 
#define BCM_PORT_IF_ERP                     _SHR_PORT_IF_ERP 
#define BCM_PORT_IF_SAT                     _SHR_PORT_IF_SAT 
#define BCM_PORT_IF_RCY_MIRROR              _SHR_PORT_IF_RCY_MIRROR 
#define BCM_PORT_IF_EVENTOR                 _SHR_PORT_IF_EVENTOR 
#define BCM_PORT_IF_NIF_ETH                 _SHR_PORT_IF_NIF_ETH /* NIF interface */
#define BCM_PORT_IF_FLEXE_CLIENT            _SHR_PORT_IF_FLEXE_CLIENT /* FlexE client */
#define BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT    _SHR_PORT_IF_VIRTUAL_FLEXE_CLIENT /* Virtual FlexE client */
#define BCM_PORT_IF_SCH                     _SHR_PORT_IF_SCH 
#define BCM_PORT_IF_CRPS                    _SHR_PORT_IF_CRPS 
#define BCM_PORT_IF_COUNT                   _SHR_PORT_IF_COUNT 

#define BCM_PORT_IF_10B         BCM_PORT_IF_TBI /* Deprecated */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure the physical interface between the MAC and the PHY for
 * the specified port.
 */
extern int bcm_port_interface_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_if_t intf);

/* 
 * Configure the physical interface between the MAC and the PHY for
 * the specified port.
 */
extern int bcm_port_interface_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_if_t *intf);

/* Configure or retrieve port interface configuration */
extern int bcm_port_interface_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_interface_config_t *config);

/* Configure or retrieve port interface configuration */
extern int bcm_port_interface_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_interface_config_t *config);

/* add/remove/get port */
extern int bcm_port_add(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_port_interface_info_t *interface_info, 
    bcm_port_mapping_info_t *mapping_info);

/* add/remove/get port */
extern int bcm_port_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags, 
    bcm_port_interface_info_t *interface_info, 
    bcm_port_mapping_info_t *mapping_info);

/* add/remove/get port */
extern int bcm_port_remove(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port add flags. */
#define BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID    0x0001     /* Static base queue pair
                                                          configuration, should
                                                          be passed in API */
#define BCM_PORT_ADD_CONFIG_CHANNELIZED     0x0002     /* Indicate a channelized
                                                          interface */
#define BCM_PORT_ADD_CGM_AUTO_ADJUST_DISABLE 0x0004     /* Disable CGM
                                                          adjustments */
#define BCM_PORT_ADD_HIGH_GIG               0x0008     /* Indicate a High Gig
                                                          port */
#define BCM_PORT_ADD_DONT_PROBE             0x0010     /* Don't initialize the
                                                          physical port */
#define BCM_PORT_ADD_USE_PHY_PBMP           0x0020     /* The phy pots occupided
                                                          by the interface are
                                                          indicated by phy_pbmp
                                                          in interface_info
                                                          struct */
#define BCM_PORT_ADD_TDM_QUEUING_ON         0x0040     /* TDM queuing is on for
                                                          this port, it's
                                                          handled as a TDM
                                                          interleaved port */
#define BCM_PORT_ADD_TDM_PORT               0x0080     /* Indicate a new TDM
                                                          port */
#define BCM_PORT_ADD_KBP_PORT               0x0100     /* Indicate it is a port
                                                          for KBP */
#define BCM_PORT_ADD_STIF_PORT              0x0200     /* Indicate it is a port
                                                          for statistics
                                                          interface */
#define BCM_PORT_ADD_MODEM_PORT             0x0400     /* Indicate it is a port
                                                          used for Link bonding
                                                          modem */
#define BCM_PORT_ADD_BICAST_PORT            0x0800     /* Indicate it is a port
                                                          used for Bicast */
#define BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT  0x1000     /* Indicate it is an over
                                                          fabric port */
#define BCM_PORT_ADD_CROSS_CONNECT          0x2000     /* Indicate it is a port
                                                          for cross connect */
#define BCM_PORT_ADD_FLEXE_PHY              0x4000     /* Indicate it is a port
                                                          for FlexE physical
                                                          interface */
#define BCM_PORT_ADD_FLEXE_MAC              0x8000     /* Indicate it is a port
                                                          for FlexE MAC channel */
#define BCM_PORT_ADD_FLEXE_SAR              0x10000    /* Indicate it is a port
                                                          for FlexE SAR channel */
#define BCM_PORT_ADD_EGRESS_INTERLEAVING_ENABLE 0x20000    /* Indicates egress
                                                          interleaving enable */
#define BCM_PORT_ADD_IF_TDM_ONLY            0x40000    /* Indicate if the
                                                          interface will have
                                                          only TDM ports */
#define BCM_PORT_ADD_IF_TDM_HYBRID          0x80000    /* Indicates if the
                                                          interface will have
                                                          some TDM ports and
                                                          some NON-TDM ports */
#define BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN 0x100000   /* Enable scheduler
                                                          priority propagation
                                                          on the port */
#define BCM_PORT_ADD_SCH_ONLY               0x200000   /* Add scheduler to the
                                                          existing port */
#define BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE 0x400000   /* Indicates ingress
                                                          interface interleaving
                                                          enable */
#define BCM_PORT_ADD_STIF_AND_DATA_PORT     0x800000   /* Indicate it is a
                                                          statistics interface
                                                          port with data traffic
                                                          capabilities */

/* Port remove flags. */
#define BCM_PORT_REMOVE_SCH_ONLY    0x1        /* Remove scheduler only from the
                                                  port */

/* bcm_port_duplex_t */
typedef _shr_port_duplex_t bcm_port_duplex_t;

#define BCM_PORT_DUPLEX_HALF    _SHR_PORT_DUPLEX_HALF 
#define BCM_PORT_DUPLEX_FULL    _SHR_PORT_DUPLEX_FULL 
#define BCM_PORT_DUPLEX_COUNT   _SHR_PORT_DUPLEX_COUNT 

/* Port pause modes. */
typedef enum bcm_port_pause_e {
    BCM_PORT_PAUSE_NONE    = 0, 
    BCM_PORT_PAUSE_ASYM_RX = 1, 
    BCM_PORT_PAUSE_ASYM_TX = 2, 
    BCM_PORT_PAUSE_SYM     = 3 
} bcm_port_pause_t;

/* Port PCS modes */
typedef enum bcm_port_pcs_e {
    bcmPortPCS8b9bLFec  = 0, 
    bcmPortPCS8b10b     = 1, 
    bcmPortPCS64b66bFec = 2, 
    bcmPortPCS64b66bBec = 3, 
    bcmPortPCS64b66b    = 4, 
    bcmPortPCS64b66bRsFec = 5, 
    bcmPortPCS64b66bLowLatencyRsFec = 6, 
    bcmPortPCS64b66b15TRsFec = 7, 
    bcmPortPCS64b66b15TLowLatencyRsFec = 8 
} bcm_port_pcs_t;

/* Set NIV Port type. */
typedef enum bcm_port_niv_type_e {
    BCM_PORT_NIV_TYPE_UPLINK = 0,       /* NIV Uplink port */
    BCM_PORT_NIV_TYPE_RESERVED_1 = 1,   /* Reserved */
    BCM_PORT_NIV_TYPE_RESERVED_2 = 2,   /* Reserved */
    BCM_PORT_NIV_TYPE_UPLINK_ACCESS = BCM_PORT_NIV_TYPE_UPLINK, /* NIV uplink port */
    BCM_PORT_NIV_TYPE_UPLINK_TRANSIT = BCM_PORT_NIV_TYPE_UPLINK, /* NIV uplink port */
    BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS = 3, /* NIV Downlink access port */
    BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT = 4, /* NIV downlink transit port */
    BCM_PORT_NIV_TYPE_SWITCH = 5,       /* NIV switch port */
    BCM_PORT_NIV_TYPE_NONE = 6 
} bcm_port_niv_type_t;

/* Set Extender Port type. */
typedef enum bcm_port_extender_type_e {
    BCM_PORT_EXTENDER_TYPE_UPLINK          = 0, 
    BCM_PORT_EXTENDER_TYPE_DOWNLINK_ACCESS = 1, 
    BCM_PORT_EXTENDER_TYPE_DOWNLINK_TRANSIT= 2, 
    BCM_PORT_EXTENDER_TYPE_SWITCH          = 3, 
    BCM_PORT_EXTENDER_TYPE_NONE 
} bcm_port_extender_type_t;

/* Set Egress Port QoS Resolution Type. */
typedef enum bcm_qos_resolution_type_e {
    BCM_PORT_QOS_SERIAL_RESOLUTION          = 0, 
    BCM_PORT_QOS_LAYERED_RESOLUTION         = 1, 
    BCM_PORT_QOS_RESOLUTION_TYPE_NONE 
} bcm_qos_resolution_type_t;

/* Port COS remap Selection. */
typedef enum bcm_port_cos_remap_e {
    BCM_PORT_COS_REMAP_VLAN_PRI = 0, 
    BCM_PORT_COS_REMAP_DSCP     = 1, 
    BCM_PORT_COS_REMAP_EXP      = 2 
} bcm_port_cos_remap_t;

/* Port TPID source Selection. */
typedef enum bcm_port_tpid_src_e {
    bcmPortTpidSrcVp = 0, 
    bcmPortTpidSrcVpn = 1, 
    bcmPortTpidSrcPort = 2, 
    bcmPortTpidSrcTrillPayload = 3, 
    bcmPortTpidSrcCount = 4 
} bcm_port_tpid_src_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the current duplex mode of a port. */
extern int bcm_port_duplex_set(
    int unit, 
    bcm_port_t port, 
    int duplex);

/* Get or set the current duplex mode of a port. */
extern int bcm_port_duplex_get(
    int unit, 
    bcm_port_t port, 
    int *duplex);

/* 
 * Enable or disable transmission of pause frames and honoring received
 * pause frames on a port.
 */
extern int bcm_port_pause_set(
    int unit, 
    bcm_port_t port, 
    int pause_tx, 
    int pause_rx);

/* 
 * Enable or disable transmission of pause frames and honoring received
 * pause frames on a port.
 */
extern int bcm_port_pause_get(
    int unit, 
    bcm_port_t port, 
    int *pause_tx, 
    int *pause_rx);

/* 
 * Get or set the source MAC address transmitted in MAC control pause
 * frames.
 */
extern int bcm_port_pause_addr_set(
    int unit, 
    bcm_port_t port, 
    bcm_mac_t mac);

/* 
 * Get or set the source MAC address transmitted in MAC control pause
 * frames.
 */
extern int bcm_port_pause_addr_get(
    int unit, 
    bcm_port_t port, 
    bcm_mac_t mac);

/* Configure or retrieve asymmetric pause setting for a port. */
extern int bcm_port_pause_sym_set(
    int unit, 
    bcm_port_t port, 
    int pause);

/* Configure or retrieve asymmetric pause setting for a port. */
extern int bcm_port_pause_sym_get(
    int unit, 
    bcm_port_t port, 
    int *pause);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Backward compatibility. */
#define bcm_port_sym_pause_set  bcm_port_pause_sym_set 
#define bcm_port_sym_pause_get  bcm_port_pause_sym_get 

#ifndef BCM_HIDE_DISPATCHABLE

/* Process a link change event and perform required device configuration. */
extern int bcm_port_update(
    int unit, 
    bcm_port_t port, 
    int link);

/* Set or retrieve the current maximum packet size permitted on a port. */
extern int bcm_port_frame_max_set(
    int unit, 
    bcm_port_t port, 
    int size);

/* Set or retrieve the current maximum packet size permitted on a port. */
extern int bcm_port_frame_max_get(
    int unit, 
    bcm_port_t port, 
    int *size);

/* 
 * Set or retrieve the current maximum L3 packet size permitted on a
 * port.
 */
extern int bcm_port_l3_mtu_set(
    int unit, 
    bcm_port_t port, 
    int size);

/* 
 * Set or retrieve the current maximum L3 packet size permitted on a
 * port.
 */
extern int bcm_port_l3_mtu_get(
    int unit, 
    bcm_port_t port, 
    int *size);

/* 
 * Set or retrieve the current maximum encapsulated L3 packet size
 * permitted on a port (Link-Layer MTU).
 */
extern int bcm_port_l3_encapsulated_mtu_set(
    int unit, 
    bcm_port_t port, 
    int size);

/* 
 * Set or retrieve the current maximum encapsulated L3 packet size
 * permitted on a port (Link-Layer MTU).
 */
extern int bcm_port_l3_encapsulated_mtu_get(
    int unit, 
    bcm_port_t port, 
    int *size);

/* Get or set half-duplex flow control (jam) for a port. */
extern int bcm_port_jam_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* Get or set half-duplex flow control (jam) for a port. */
extern int bcm_port_jam_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

/* Get or set the interframe gap settings for the specified port. */
extern int bcm_port_ifg_set(
    int unit, 
    bcm_port_t port, 
    int speed, 
    bcm_port_duplex_t duplex, 
    int bit_times);

/* Get or set the interframe gap settings for the specified port. */
extern int bcm_port_ifg_get(
    int unit, 
    bcm_port_t port, 
    int speed, 
    bcm_port_duplex_t duplex, 
    int *bit_times);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Values for flags parameter of bcm_port_force_vlan_set/get. */
#define BCM_PORT_FORCE_VLAN_ENABLE          0x01       /* Enable force VLAN on
                                                          this port. */
#define BCM_PORT_FORCE_VLAN_UNTAG           0x02       /* Egress untagged when
                                                          force VLAN is enabled
                                                          on this port. */
#define BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT 0x04       /* Promiscuous port type. */
#define BCM_PORT_FORCE_VLAN_ISOLATED_PORT   0x08       /* isolated port type. */
#define BCM_PORT_FORCE_VLAN_COMMUNITY_PORT  0x0c       /* community port type. */
#define BCM_PORT_FORCE_VLAN_PORT_TYPE_MASK  0x0c       /* port type mask. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get force VLAN attribute of a port. */
extern int bcm_port_force_vlan_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    int pkt_prio, 
    uint32 flags);

/* Set/Get force VLAN attribute of a port. */
extern int bcm_port_force_vlan_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t *vlan, 
    int *pkt_prio, 
    uint32 *flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for direct PHY register access. */
#define BCM_PORT_PHY_INTERNAL           _SHR_PORT_PHY_INTERNAL 
#define BCM_PORT_PHY_NOMAP              _SHR_PORT_PHY_NOMAP 
#define BCM_PORT_PHY_CLAUSE45           _SHR_PORT_PHY_CLAUSE45 
#define BCM_PORT_PHY_I2C_DATA8          _SHR_PORT_PHY_I2C_DATA8 
#define BCM_PORT_PHY_I2C_DATA16         _SHR_PORT_PHY_I2C_DATA16 
#define BCM_PORT_PHY_PVT_DATA           _SHR_PORT_PHY_PVT_DATA 
#define BCM_PORT_PHY_BROADCAST_WRITE    _SHR_PORT_PHY_BROADCAST_WRITE 
#define BCM_PORT_PHY_REG_RDB            _SHR_PORT_PHY_REG_RDB 
#define BCM_PORT_PHY_REG_MACSEC         _SHR_PORT_PHY_REG_MACSEC 
#define BCM_PORT_PHY_REG64_DATA_HI32    _SHR_PORT_PHY_REG64_DATA_HI32 

#define BCM_PORT_PHY_CLAUSE45_ADDR(_devad, _regad)  \
    _SHR_PORT_PHY_CLAUSE45_ADDR(_devad, _regad) 

#define BCM_PORT_PHY_CLAUSE45_DEVAD(_reg_addr)  \
    _SHR_PORT_PHY_CLAUSE45_DEVAD(_reg_addr) 

#define BCM_PORT_PHY_CLAUSE45_REGAD(_reg_addr)  \
    _SHR_PORT_PHY_CLAUSE45_REGAD(_reg_addr) 

#define BCM_PORT_PHY_I2C_ADDR(_devad, _regad)  \
    _SHR_PORT_PHY_I2C_ADDR(_devad, _regad) 

#define BCM_PORT_PHY_I2C_DEVAD(_reg_addr)  \
    _SHR_PORT_PHY_I2C_DEVAD(_reg_addr) 

#define BCM_PORT_PHY_I2C_REGAD(_reg_addr)  \
    _SHR_PORT_PHY_I2C_REGAD(_reg_addr) 

/* Flags for indirect PHY register access. */
#define BCM_PORT_PHY_REG_1000X  _SHR_PORT_PHY_REG_1000X 

#define BCM_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_type, _reg_selector)  \
    _SHR_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_type, _reg_selector) 

#define BCM_PORT_PHY_REG_FLAGS(_reg_addr)  _SHR_PORT_PHY_REG_FLAGS(_reg_addr) 

#define BCM_PORT_PHY_REG_BANK(_reg_addr)  _SHR_PORT_PHY_REG_BANK(_reg_addr) 

#define BCM_PORT_PHY_REG_ADDR(_reg_addr)  _SHR_PORT_PHY_REG_ADDR(_reg_addr) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Read or write PHY registers associated with a port. */
extern int bcm_port_phy_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 phy_reg_addr, 
    uint32 phy_data);

/* Read or write PHY registers associated with a port. */
extern int bcm_port_phy_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 phy_reg_addr, 
    uint32 *phy_data);

/* Read or write PHY registers associated with a port. */
extern int bcm_port_phy_modify(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 phy_reg_addr, 
    uint32 phy_data, 
    uint32 phy_mask);

/* 
 * Read data from a slave device connected to a PHY associated with a
 * port.
 */
extern int bcm_port_phy_multi_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 dev_addr, 
    uint32 offset, 
    int max_size, 
    uint8 *data, 
    int *actual_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* PHY reset callback function prototype. */
typedef _shr_port_phy_reset_cb_t bcm_port_phy_reset_cb_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Register or unregister a callout to post process PHY resets. */
extern int bcm_port_phy_reset_register(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_reset_cb_t callback, 
    void *user_data);

/* Register or unregister a callout to post process PHY resets. */
extern int bcm_port_phy_reset_unregister(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_reset_cb_t callback, 
    void *user_data);

/* Perform low level PHY reset. */
extern int bcm_port_phy_reset(
    int unit, 
    bcm_port_t port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MDI crossover control. */
typedef _shr_port_mdix_t bcm_port_mdix_t;

/* MDI crossover control values. */
#define BCM_PORT_MDIX_AUTO          _SHR_PORT_MDIX_AUTO 
#define BCM_PORT_MDIX_FORCE_AUTO    _SHR_PORT_MDIX_FORCE_AUTO 
#define BCM_PORT_MDIX_NORMAL        _SHR_PORT_MDIX_NORMAL 
#define BCM_PORT_MDIX_XOVER         _SHR_PORT_MDIX_XOVER 
#define BCM_PORT_MDIX_COUNT         _SHR_PORT_MDIX_COUNT 

/* MDI crossover status. */
typedef _shr_port_mdix_status_t bcm_port_mdix_status_t;

/* MDI crossover status values. */
#define BCM_PORT_MDIX_STATUS_NORMAL _SHR_PORT_MDIX_STATUS_NORMAL 
#define BCM_PORT_MDIX_STATUS_XOVER  _SHR_PORT_MDIX_STATUS_XOVER 
#define BCM_PORT_MDIX_STATUS_COUNT  _SHR_PORT_MDIX_STATUS_COUNT 

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the Auto MDIX mode for a port. */
extern int bcm_port_mdix_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_mdix_t mode);

/* Get or set the Auto MDIX mode for a port. */
extern int bcm_port_mdix_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_mdix_t *mode);

/* Get or set the Auto MDIX mode for a port. */
extern int bcm_port_mdix_status_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_mdix_status_t *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Combo port control/status. */
typedef _shr_port_medium_t bcm_port_medium_t;

/* Combo port control/status values. */
#define BCM_PORT_MEDIUM_NONE        _SHR_PORT_MEDIUM_NONE 
#define BCM_PORT_MEDIUM_COPPER      _SHR_PORT_MEDIUM_COPPER 
#define BCM_PORT_MEDIUM_FIBER       _SHR_PORT_MEDIUM_FIBER 
#define BCM_PORT_MEDIUM_BACKPLANE   _SHR_PORT_MEDIUM_BACKPLANE 
#define BCM_PORT_MEDIUM_ALL         _SHR_PORT_MEDIUM_ALL 
#define BCM_PORT_MEDIUM_COUNT       _SHR_PORT_MEDIUM_COUNT 

/* Call back function prototype for media status changes. */
typedef _shr_port_medium_status_cb_t bcm_port_medium_status_cb_t;

/* bcm_phy_config_t */
typedef _shr_phy_config_t bcm_phy_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set medium preferences for combination ports. */
extern int bcm_port_medium_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_medium_t medium, 
    bcm_phy_config_t *config);

/* Get or set medium preferences for combination ports. */
extern int bcm_port_medium_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_medium_t medium, 
    bcm_phy_config_t *config);

/* Get or set medium preferences for combination ports. */
extern int bcm_port_medium_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_medium_t *medium);

/* Register or unregister a callback function on media status changes. */
extern int bcm_port_medium_status_register(
    int unit, 
    bcm_port_t port, 
    bcm_port_medium_status_cb_t callback, 
    void *user_data);

/* Register or unregister a callback function on media status changes. */
extern int bcm_port_medium_status_unregister(
    int unit, 
    bcm_port_t port, 
    bcm_port_medium_status_cb_t callback, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port loopback modes. */
typedef enum bcm_port_loopback_e {
    BCM_PORT_LOOPBACK_NONE = 0, 
    BCM_PORT_LOOPBACK_MAC  = 1, 
    BCM_PORT_LOOPBACK_PHY  = 2, 
    BCM_PORT_LOOPBACK_PHY_REMOTE = 3, 
    BCM_PORT_LOOPBACK_MAC_REMOTE = 4, 
    BCM_PORT_LOOPBACK_COUNT = 5 
} bcm_port_loopback_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or retrieve the current loopback mode of a port. */
extern int bcm_port_loopback_set(
    int unit, 
    bcm_port_t port, 
    int loopback);

/* Set or retrieve the current loopback mode of a port. */
extern int bcm_port_loopback_get(
    int unit, 
    bcm_port_t port, 
    int *loopback);

/* 
 * Set the spanning tree state for a port (single instance spanning tree
 * only).
 */
extern int bcm_port_stp_set(
    int unit, 
    bcm_port_t port, 
    int state);

/* 
 * Set the spanning tree state for a port (single instance spanning tree
 * only).
 */
extern int bcm_port_stp_get(
    int unit, 
    bcm_port_t port, 
    int *state);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_discard_e */
typedef enum bcm_port_discard_e {
    BCM_PORT_DISCARD_NONE  = 0, 
    BCM_PORT_DISCARD_ALL   = 1, 
    BCM_PORT_DISCARD_UNTAG = 2, 
    BCM_PORT_DISCARD_TAG   = 3, 
    BCM_PORT_DISCARD_INGRESS = 4, 
    BCM_PORT_DISCARD_EGRESS = 5, 
    BCM_PORT_DISCARD_COUNT = 6 
} bcm_port_discard_t;

#define BCM_PORT_DISCARD_MODE_INGRESS_ONLY  (1 << 15)  /* Discard state should
                                                          be set for ingress
                                                          only */
#define BCM_PORT_DISCARD_MODE_EGRESS_ONLY   (1 << 14)  /* Discard state should
                                                          be set for egress only */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure port discard mode for packets ingressing tagged and
 * untagged.
 */
extern int bcm_port_discard_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* 
 * Configure port discard mode for packets ingressing tagged and
 * untagged.
 */
extern int bcm_port_discard_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Flags for learn mode
 * 
 * This call takes flags to turn on and off mutually-independent actions
 * that should be taken when a packet is received with an unknown source
 * address, or source lookup failure (SLF).
 * 
 * The set call returns BCM_E_UNAVAIL for flag combinations that are not
 * supported by the hardware.
 */
#define BCM_PORT_LEARN_ARL      0x01       /* Learn SLF address. */
#define BCM_PORT_LEARN_CPU      0x02       /* Copy SLF packet to CPU. */
#define BCM_PORT_LEARN_FWD      0x04       /* Forward SLF packet */
#define BCM_PORT_LEARN_PENDING  0x08       /* Mark learned SLF as pending */
#define BCM_PORT_LEARN_ON       0x01       /* Deprecated name */
#define BCM_PORT_LEARN_SWITCH   0x04       /* Deprecated name */

#ifndef BCM_HIDE_DISPATCHABLE

/* Control the hardware and software learning support on a port. */
extern int bcm_port_learn_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

/* Control the hardware and software learning support on a port. */
extern int bcm_port_learn_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags);

/* Control the hardware and software learning support on a port. */
extern int bcm_port_learn_modify(
    int unit, 
    bcm_port_t port, 
    uint32 add, 
    uint32 remove);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_LINK_STATUS_DOWN           0          
#define BCM_PORT_LINK_STATUS_UP             1          
#define BCM_PORT_LINK_STATUS_FAILED         2          
#define BCM_PORT_LINK_STATUS_REMOTE_FAULT   3          
#define BCM_PORT_LINK_STATUS_LOCAL_FAULT    4          
#define BCM_PORT_LINK_STATUS_LOCAL_AND_REMOTE_FAULT 5          

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve the current link status of a port. */
extern int bcm_port_link_status_get(
    int unit, 
    bcm_port_t port, 
    int *status);

/* Retrieve the current link state of a port. */
extern int bcm_port_link_state_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_port_link_state_t *state);

/* Clear failed link status from a port which has undergone LAG failover. */
extern int bcm_port_link_failed_clear(
    int unit, 
    bcm_port_t port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Modes for VLAN input filtering.
 * 
 * Superceded APIs: use bcm_port_vlan_member_[get|set] instead.
 */
#define BCM_PORT_IFILTER_OFF    0          
#define BCM_PORT_IFILTER_ON     1          

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set or retrieve current behavior of tagged packets arriving on a port
 * not a member of the specified VLAN.
 */
extern int bcm_port_ifilter_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* 
 * Set or retrieve current behavior of tagged packets arriving on a port
 * not a member of the specified VLAN.
 */
extern int bcm_port_ifilter_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Modes for VLAN ingress/egress filtering. */
#define BCM_PORT_VLAN_MEMBER_INGRESS    0x00000001 
#define BCM_PORT_VLAN_MEMBER_EGRESS     0x00000002 

/* Table method for VLAN ingress/egress filtering. */
#define BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP 0x00000004 

/* 
 * Do not perform a VFI membership check on VP, instead do a VLAN check
 * on physical port.
 */
#define BCM_PORT_VLAN_MEMBER_VP_DO_NOT_CHECK 0x00000008 

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set or retrieve current behavior of tagged packets arriving/leaving on
 * a port not a member of the specified VLAN.
 */
extern int bcm_port_vlan_member_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

/* 
 * Set or retrieve current behavior of tagged packets arriving/leaving on
 * a port not a member of the specified VLAN.
 */
extern int bcm_port_vlan_member_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags);

/* Enable or disable BPDU processing on a port. */
extern int bcm_port_bpdu_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* Enable or disable BPDU processing on a port. */
extern int bcm_port_bpdu_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

/* Enable or disable l3 switching on an ingress port. */
extern int bcm_port_l3_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* Enable or disable l3 switching on an ingress port. */
extern int bcm_port_l3_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

/* bcm_port_tgid_set */
extern int bcm_port_tgid_set(
    int unit, 
    bcm_port_t port, 
    int tgid, 
    int psc);

/* bcm_port_tgid_get */
extern int bcm_port_tgid_get(
    int unit, 
    bcm_port_t port, 
    int *tgid, 
    int *psc);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_mcast_flood_t */
typedef _shr_port_mcast_flood_t bcm_port_mcast_flood_t;

#define BCM_PORT_MCAST_FLOOD_ALL        _SHR_PORT_MCAST_FLOOD_ALL 
#define BCM_PORT_MCAST_FLOOD_UNKNOWN    _SHR_PORT_MCAST_FLOOD_UNKNOWN 
#define BCM_PORT_MCAST_FLOOD_NONE       _SHR_PORT_MCAST_FLOOD_NONE 
#define BCM_PORT_MCAST_FLOOD_COUNT      _SHR_PORT_MCAST_FLOOD_COUNT 

/* Backward compatibility. */
#define BCM_PORT_PFM_MODEA      BCM_PORT_MCAST_FLOOD_ALL 
#define BCM_PORT_PFM_MODEB      BCM_PORT_MCAST_FLOOD_UNKNOWN 
#define BCM_PORT_PFM_MODEC      BCM_PORT_MCAST_FLOOD_NONE 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or retrieve the current unknown multicast behavior. */
extern int bcm_port_pfm_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* Set or retrieve the current unknown multicast behavior. */
extern int bcm_port_pfm_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Port Encapsulation modes
 * 
 * Members of the XGS family of switches support multiple frame
 * (link-level) encapsulation modes (HIGIG, BCM5632, Ethernet).
 */
typedef _shr_port_encap_t bcm_port_encap_t;

#define BCM_PORT_ENCAP_IEEE                 _SHR_PORT_ENCAP_IEEE 
#define BCM_PORT_ENCAP_HIGIG                _SHR_PORT_ENCAP_HIGIG 
#define BCM_PORT_ENCAP_B5632                _SHR_PORT_ENCAP_B5632 
#define BCM_PORT_ENCAP_HIGIG2               _SHR_PORT_ENCAP_HIGIG2 
#define BCM_PORT_ENCAP_HIGIG2_LITE          _SHR_PORT_ENCAP_HIGIG2_LITE 
#define BCM_PORT_ENCAP_HIGIG2_L2            _SHR_PORT_ENCAP_HIGIG2_L2 
#define BCM_PORT_ENCAP_HIGIG2_IP_GRE        _SHR_PORT_ENCAP_HIGIG2_IP_GRE 
#define BCM_PORT_ENCAP_OBSOLETE             _SHR_PORT_ENCAP_OBSOLETE 
#define BCM_PORT_ENCAP_PREAMBLE_SOP_ONLY    _SHR_PORT_ENCAP_PREAMBLE_SOP_ONLY  
#define BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET  _SHR_PORT_ENCAP_HIGIG_OVER_ETHERNET 
#define BCM_PORT_ENCAP_CPRI                 _SHR_PORT_ENCAP_CPRI 
#define BCM_PORT_ENCAP_RSVD4                _SHR_PORT_ENCAP_RSVD4 
#define BCM_PORT_ENCAP_HIGIG3               _SHR_PORT_ENCAP_HIGIG3 
#define BCM_PORT_ENCAP_COUNT                _SHR_PORT_ENCAP_COUNT 

/* Port Encapsulation abilities */
typedef _shr_pa_encap_t bcm_pa_encap_t;

#define BCM_PA_ENCAP_IEEE                   _SHR_PA_ENCAP_IEEE 
#define BCM_PA_ENCAP_HIGIG                  _SHR_PA_ENCAP_HIGIG 
#define BCM_PA_ENCAP_B5632                  _SHR_PA_ENCAP_B5632 
#define BCM_PA_ENCAP_HIGIG2                 _SHR_PA_ENCAP_HIGIG2 
#define BCM_PA_ENCAP_HIGIG2_LITE            _SHR_PA_ENCAP_HIGIG2_LITE 
#define BCM_PA_ENCAP_HIGIG2_L2              _SHR_PA_ENCAP_HIGIG2_L2 
#define BCM_PA_ENCAP_HIGIG2_IP_GRE          _SHR_PA_ENCAP_HIGIG2_IP_GRE 
#define BCM_PA_ENCAP_OBSOLETE               _SHR_PA_ENCAP_OBSOLETE 
#define BCM_PA_ENCAP_HIGIG_OVER_ETHERNET    _SHR_PA_ENCAP_HIGIG_OVER_ETHERNET 
#define BCM_PA_ENCAP_CPRI                   _SHR_PA_ENCAP_CPRI 
#define BCM_PA_ENCAP_RSVD4                  _SHR_PA_ENCAP_RSVD4 

/* bcm_port_encap_config_s */
typedef struct bcm_port_encap_config_s {
    bcm_port_encap_t encap; /* Port encapsulation one of the BCM_PORT_ENCAP_* */
    uint16 oui_ethertype;   /* Vendor specific ether type. */
    uint16 higig_ethertype; /* HiGig ether type */
    uint8 oui[3];           /* Vendor specific value */
    uint16 hgoe_reserved;   /* Value for reserved field in HGoE header */
    bcm_mac_t dst_mac;      /* Destination MAC address */
    bcm_mac_t src_mac;      /* Source MAC address */
    uint16 tpid;            /* Outer L2 header TPID */
    uint16 vlan;            /* Outer L2 header VLAN tag */
    uint8 ttl;              /* Hop limit. */
    uint8 tos;              /* Traffic Class/Tos byte */
    bcm_ip_t src_addr;      /* Tunnel source ip address (IPv4) */
    bcm_ip_t dst_addr;      /* Tunnel destination ip address (IPv4) */
} bcm_port_encap_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or get the link level encapsulation configuration. */
extern int bcm_port_encap_config_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_encap_config_t *encap_config);

/* Set or get the link level encapsulation configuration. */
extern int bcm_port_encap_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_encap_config_t *encap_config);

/* Set or get the link level encapsulation mode. */
extern int bcm_port_encap_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* Set or get the link level encapsulation mode. */
extern int bcm_port_encap_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

/* 
 * Get the current count of cells or packets queued on a port for
 * transmission.
 */
extern int bcm_port_queued_count_get(
    int unit, 
    bcm_port_t port, 
    uint32 *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_frametype_t */
typedef int bcm_port_frametype_t;

/* Protocol based VLAN support. */
#define BCM_PORT_FRAMETYPE_ETHER2       0x1        
#define BCM_PORT_FRAMETYPE_8023         0x2        
#define BCM_PORT_FRAMETYPE_SNAP         BCM_PORT_FRAMETYPE_8023 
#define BCM_PORT_FRAMETYPE_LLC          0x4        
#define BCM_PORT_FRAMETYPE_SNAP_PRIVATE 0x8        

/* bcm_port_ethertype_t */
typedef bcm_ethertype_t bcm_port_ethertype_t;

/* Backplane multiplexing type */
typedef enum bcm_port_control_ability_e {
    BCM_PORT_ABILITY_SFI = 1, 
    BCM_PORT_ABILITY_DUAL_SFI = 2, 
    BCM_PORT_ABILITY_SCI = 3, 
    BCM_PORT_ABILITY_SFI_SCI = 4, 
    BCM_PORT_ABILITY_DUAL_SFI_LOCAL = 5, 
    BCM_PORT_ABILITY_DUAL_SFI_LOOPBACK=BCM_PORT_ABILITY_DUAL_SFI_LOCAL, 
    BCM_PORT_ABILITY_SFI_LOOPBACK = 6 
} bcm_port_control_ability_t;

/* PRBS Polynomial type */
typedef _shr_port_prbs_polynomial_t bcm_port_prbs_t;

#define BCM_PORT_PRBS_POLYNOMIAL_X7_X6_1    _SHR_PORT_PRBS_POLYNOMIAL_X7_X6_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X15_X14_1  _SHR_PORT_PRBS_POLYNOMIAL_X15_X14_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X23_X18_1  _SHR_PORT_PRBS_POLYNOMIAL_X23_X18_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1  _SHR_PORT_PRBS_POLYNOMIAL_X31_X28_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X9_X5_1    _SHR_PORT_PRBS_POLYNOMIAL_X9_X5_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X11_X9_1   _SHR_PORT_PRBS_POLYNOMIAL_X11_X9_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X58_X31_1  _SHR_PORT_PRBS_POLYNOMIAL_X58_X31_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X49_X40_1  _SHR_PORT_PRBS_POLYNOMIAL_X49_X40_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X20_X3_1   _SHR_PORT_PRBS_POLYNOMIAL_X20_X3_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X13_X12_X2_1 _SHR_PORT_PRBS_POLYNOMIAL_X13_X12_X2_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X20_X3_1   _SHR_PORT_PRBS_POLYNOMIAL_X20_X3_1 
#define BCM_PORT_PRBS_POLYNOMIAL_X10_X7_1   _SHR_PORT_PRBS_POLYNOMIAL_X10_X7_1 
#define BCM_PORT_PRBS_POLYNOMIAL_PAM4_13Q   _SHR_PORT_PRBS_POLYNOMIAL_PAM4_13Q 

/* PHY PCS mode */
typedef _shr_port_phy_pcs_mode_t bcm_port_phy_pcs_mode_t;

#define BCM_PORT_PHY_CONTROL_PCS_MODE_IEEE  _SHR_PORT_PHY_CONTROL_PCS_MODE_IEEE 
#define BCM_PORT_PHY_CONTROL_PCS_MODE_BRCM  _SHR_PORT_PHY_CONTROL_PCS_MODE_BRCM 

/* Backward compatibility. */
#define bcm_port_untagged_protocol_vlan_add \
    bcm_port_protocol_vlan_add 
#define bcm_port_untagged_protocol_vlan_delete \
    bcm_port_protocol_vlan_delete 
#define bcm_port_untagged_protocol_vlan_delete_all \
    bcm_port_protocol_vlan_delete_all 

#ifndef BCM_HIDE_DISPATCHABLE

/* Add or delete a protocol-based VLAN. */
extern int bcm_port_protocol_vlan_add(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether, 
    bcm_vlan_t vid);

/* Add or delete a protocol-based VLAN. */
extern int bcm_port_protocol_vlan_delete(
    int unit, 
    bcm_port_t port, 
    bcm_port_frametype_t frame, 
    bcm_port_ethertype_t ether);

/* Add or delete a protocol-based VLAN. */
extern int bcm_port_protocol_vlan_delete_all(
    int unit, 
    bcm_port_t port);

/* Configure ports to block or allow packets from a given ingress port. */
extern int bcm_port_egress_set(
    int unit, 
    bcm_port_t port, 
    int modid, 
    bcm_pbmp_t pbmp);

/* Configure ports to block or allow packets from a given ingress port. */
extern int bcm_port_egress_get(
    int unit, 
    bcm_port_t port, 
    int modid, 
    bcm_pbmp_t *pbmp);

/* 
 * Configure which egress ports a stacking port is permitted to forward
 * packets to.
 */
extern int bcm_port_modid_enable_set(
    int unit, 
    bcm_port_t port, 
    int modid, 
    int enable);

/* 
 * Configure which egress ports a stacking port is permitted to forward
 * packets to.
 */
extern int bcm_port_modid_enable_get(
    int unit, 
    bcm_port_t port, 
    int modid, 
    int *enable);

/* 
 * Configure which egress ports a stacking port is permitted to forward
 * packets to.
 */
extern int bcm_port_modid_egress_set(
    int unit, 
    bcm_port_t port, 
    bcm_module_t modid, 
    bcm_pbmp_t pbmp);

/* 
 * Configure which egress ports a stacking port is permitted to forward
 * packets to.
 */
extern int bcm_port_modid_egress_get(
    int unit, 
    bcm_port_t port, 
    bcm_module_t modid, 
    bcm_pbmp_t *pbmp);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flood blocking modes. */
#define BCM_PORT_FLOOD_BLOCK_BCAST          0x1        
#define BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST  0x2        
#define BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST  0x4        
#define BCM_PORT_FLOOD_BLOCK_ALL            0x8        
#define BCM_PORT_FLOOD_BLOCK_UNKNOWN_IP_MCAST 0x10       
#define BCM_PORT_FLOOD_BLOCK_UNKNOWN_NONIP_MCAST 0x20       
#define BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST    0x40       
#define BCM_PORT_FLOOD_BLOCK_KNOWN_UCAST    0x80       

#ifndef BCM_HIDE_DISPATCHABLE

/* Selectively block flooding traffic. */
extern int bcm_port_flood_block_set(
    int unit, 
    bcm_port_t ingress_port, 
    bcm_port_t egress_port, 
    uint32 flags);

/* Selectively block flooding traffic. */
extern int bcm_port_flood_block_get(
    int unit, 
    bcm_port_t ingress_port, 
    bcm_port_t egress_port, 
    uint32 *flags);

/* Configure a port for egress rate shaping. */
extern int bcm_port_rate_egress_set(
    int unit, 
    bcm_port_t port, 
    uint32 kbits_sec, 
    uint32 kbits_burst);

/* Configure a port for egress rate shaping in packet mode. */
extern int bcm_port_rate_egress_pps_set(
    int unit, 
    bcm_port_t port, 
    uint32 pps, 
    uint32 burst);

/* Configure a port for egress rate shaping. */
extern int bcm_port_rate_egress_get(
    int unit, 
    bcm_port_t port, 
    uint32 *kbits_sec, 
    uint32 *kbits_burst);

/* Configure a port for egress rate shaping in packet mode. */
extern int bcm_port_rate_egress_pps_get(
    int unit, 
    bcm_port_t port, 
    uint32 *pps, 
    uint32 *burst);

/* Configure a port for ingress rate policing. */
extern int bcm_port_rate_ingress_set(
    int unit, 
    bcm_port_t port, 
    uint32 kbits_sec, 
    uint32 kbits_burst);

/* Configure a port for ingress rate policing. */
extern int bcm_port_rate_ingress_get(
    int unit, 
    bcm_port_t port, 
    uint32 *kbits_sec, 
    uint32 *kbits_burst);

/* 
 * Configure a port's ingress rate limiting pause frame control
 * parameters.
 */
extern int bcm_port_rate_pause_set(
    int unit, 
    bcm_port_t port, 
    uint32 kbits_pause, 
    uint32 kbits_resume);

/* 
 * Configure a port's ingress rate limiting pause frame control
 * parameters.
 */
extern int bcm_port_rate_pause_get(
    int unit, 
    bcm_port_t port, 
    uint32 *kbits_pause, 
    uint32 *kbits_resume);

/* Control the sampling of packets ingressing or egressing a port. */
extern int bcm_port_sample_rate_set(
    int unit, 
    bcm_port_t port, 
    int ingress_rate, 
    int egress_rate);

/* Control the sampling of packets ingressing or egressing a port. */
extern int bcm_port_sample_rate_get(
    int unit, 
    bcm_port_t port, 
    int *ingress_rate, 
    int *egress_rate);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Double tagging modes. */
#define BCM_PORT_DTAG_MODE_NONE             0          /* No double tagging. */
#define BCM_PORT_DTAG_MODE_INTERNAL         1          /* Service Provider port. */
#define BCM_PORT_DTAG_MODE_EXTERNAL         2          /* Customer port. */
#define BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG   4          /* Remove customer tag. */
#define BCM_PORT_DTAG_ADD_EXTERNAL_TAG      8          /* Add customer tag. */
#define BCM_PORT_DTAG_MODE_TRANSPARENT      16         /* Transparent port, all
                                                          packets assume port's
                                                          default VID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or retrieve the current double tagging mode for a port. */
extern int bcm_port_dtag_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode);

/* Set or retrieve the current double tagging mode for a port. */
extern int bcm_port_dtag_mode_get(
    int unit, 
    bcm_port_t port, 
    int *mode);

/* Set the default tag protocol ID (TPID) for the specified port. */
extern int bcm_port_tpid_set(
    int unit, 
    bcm_port_t port, 
    uint16 tpid);

/* Set the default tag protocol ID (TPID) for the specified port. */
extern int bcm_port_tpid_get(
    int unit, 
    bcm_port_t port, 
    uint16 *tpid);

/* Add an allowed outer tag protocol ID (TPID) for the specified port. */
extern int bcm_port_tpid_add(
    int unit, 
    bcm_port_t port, 
    uint16 tpid, 
    int color_select);

/* Delete allowed outer tag protocol ID (TPID) for the specified port. */
extern int bcm_port_tpid_delete(
    int unit, 
    bcm_port_t port, 
    uint16 tpid);

/* Delete allowed outer tag protocol ID (TPID) for the specified port. */
extern int bcm_port_tpid_delete_all(
    int unit, 
    bcm_port_t port);

/* 
 * Set the default L2 tunnel payload tag protocol ID (TPID) for the
 * specified port.
 */
extern int bcm_port_l2_tunnel_payload_tpid_set(
    int unit, 
    bcm_port_t port, 
    uint16 tpid);

/* 
 * Set the default L2 tunnel payload tag protocol ID (TPID) for the
 * specified port.
 */
extern int bcm_port_l2_tunnel_payload_tpid_get(
    int unit, 
    bcm_port_t port, 
    uint16 *tpid);

/* 
 * Add an allowed L2 tunnel payload outer tag protocol ID (TPID) for the
 * specified port.
 */
extern int bcm_port_l2_tunnel_payload_tpid_add(
    int unit, 
    bcm_port_t port, 
    uint16 tpid, 
    int color_select);

/* 
 * Delete allowed L2 tunnel payload outer tag protocol ID (TPID) for the
 * specified port.
 */
extern int bcm_port_l2_tunnel_payload_tpid_delete(
    int unit, 
    bcm_port_t port, 
    uint16 tpid);

/* 
 * Delete allowed L2 tunnel payload outer tag protocol ID (TPID) for the
 * specified port.
 */
extern int bcm_port_l2_tunnel_payload_tpid_delete_all(
    int unit, 
    bcm_port_t port);

/* Set the default tag protocol ID (TPID) for the specified port. */
extern int bcm_port_inner_tpid_set(
    int unit, 
    bcm_port_t port, 
    uint16 tpid);

/* Set the default tag protocol ID (TPID) for the specified port. */
extern int bcm_port_inner_tpid_get(
    int unit, 
    bcm_port_t port, 
    uint16 *tpid);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port TPID class flags. */
#define BCM_PORT_TPID_CLASS_OUTER_IS_PRIO   0x0001     /* Outer identified TPID
                                                          is priority i.e, VLAN
                                                          ID equals to zero */
#define BCM_PORT_TPID_CLASS_DISCARD         0x0002     /* Discard packet */
#define BCM_PORT_TPID_CLASS_OUTER_C         0x0004     /* Outer tag is customer
                                                          tag */
#define BCM_PORT_TPID_CLASS_INNER_C         0x0008     /* Inner tag is customer
                                                          tag */
#define BCM_PORT_TPID_CLASS_INGRESS_ONLY    0x0010     /* Ingress only */
#define BCM_PORT_TPID_CLASS_EGRESS_ONLY     0x0020     /* Egress only */
#define BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO  0x0040     /* Outer identified TPID
                                                          is not priority i.e,
                                                          VLAN ID different than
                                                          zero */
#define BCM_PORT_TPID_CLASS_NATIVE_EVE      0x0080     /* Native Egress vlan
                                                          classification. */
#define BCM_PORT_TPID_CLASS_NATIVE_IVE      0x0100     /* Native Ingress vlan
                                                          classification. */
#define BCM_PORT_TPID_CLASS_WITH_ID         0x0200     /* Create specified
                                                          TAG-STRUCT SW-ID. */
#define BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL 0x0400     /* Egress IP Tunnel vlan
                                                          classification. */

/* Special TPID values for Port TPID class configuration */
#define BCM_PORT_TPID_CLASS_TPID_INVALID    0xFFFFFFFF /* Value for invalid TPID
                                                          in port TPID class
                                                          configuration */
#define BCM_PORT_TPID_CLASS_TPID_ANY        0xFFFFFFFE /* Value for any TPID in
                                                          port TPID class
                                                          configuration */

/* bcm_port_tag_format_class_t */
typedef uint16 bcm_port_tag_format_class_t;

/* VLAN tag structure type */
typedef enum bcm_port_tag_struct_type_e {
    bcmTagStructTypeUntag = 0,      /* Type Untagged */
    bcmTagStructTypeSTag = 1,       /* Type S tag */
    bcmTagStructTypeCTag = 2,       /* Type C tag */
    bcmTagStructTypeSCTag = 3,      /* Type S C tag */
    bcmTagStructTypeSPrioCTag = 4,  /* Type S prio C tag */
    bcmTagStructTypeSSTag = 5,      /* Type S S tag */
    bcmTagStructTypeCCTag = 6,      /* Type C C tag */
    bcmNofTagStructType = 7         /* Number of tag structure types */
} bcm_port_tag_struct_type_t;

typedef enum bcm_port_control_recycle_application_e {
    bcmPortControlRecycleAppDefault = 0, /* Default port settings according to
                                           header-type. */
    bcmPortControlRecycleAppDropAndContinue = 1, /* Drop and Continue. */
    bcmPortControlRecycleAppExtendedEncap = 2, /* Extended Encapsulation. */
    bcmPortControlRecycleAppExtendedTerm = 3, /* Extended Termination. */
    bcmPortControlRecycleAppReflector = 4, /* Reflector Application. */
    bcmPortControlRecycleAppSbfdReflector = 5, /* SBFD Reflector Application. */
    bcmPortControlRecycleAppBridgeFallback = 6, /* Bridge fallback. */
    bcmPortControlRecycleAppExtendedEncapUncollapse = 7 /* Uncollapsed Extended Encapsulation,
                                           forwarding decision is done in 2nd
                                           pass */
} bcm_port_control_recycle_application_t;

/* bcm_port_tpid_class_s */
typedef struct bcm_port_tpid_class_s {
    uint32 flags;                       /* BCM_PORT_TPID_CLASS_XXX */
    bcm_gport_t port;                   /* Port ID */
    uint32 tpid1;                       /* First TPID value, -1 for invalid */
    uint32 tpid2;                       /* Second TPID value, -1 for invalid */
    bcm_port_tag_format_class_t tag_format_class_id; /* VLAN tag format ID */
    int vlan_translation_action_id;     /* Default translation action ID */
    uint32 vlan_translation_qos_map_id; /* Default translation QoS map ID */
    uint32 tpid_class_id;               /* TPID profile. */
} bcm_port_tpid_class_t;

/* 
 * Initialize a structure that holds both the key and the info for Port
 * TPID class configuration.
 */
extern void bcm_port_tpid_class_t_init(
    bcm_port_tpid_class_t *tpid_class);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set VLAN tag classification for a port. Set tag-classifier for given
 * identified TPIDs on packet.
 */
extern int bcm_port_tpid_class_set(
    int unit, 
    bcm_port_tpid_class_t *tpid_class);

/* Get VLAN tag classification for a port. */
extern int bcm_port_tpid_class_get(
    int unit, 
    bcm_port_tpid_class_t *tpid_class);

/* 
 * Create/destroy a VLAN tag classification ID used to aggregate a group
 * of ports for further processing such as VLAN translation and field
 * processing.
 */
extern int bcm_port_tpid_class_create(
    int unit, 
    uint32 flags, 
    bcm_port_tag_struct_type_t tag_struct_type, 
    bcm_port_tag_format_class_t *tag_format_class_id);

/* 
 * Create/destroy a VLAN tag classification ID used to aggregate a group
 * of ports for further processing such as VLAN translation and field
 * processing.
 */
extern int bcm_port_tpid_class_destroy(
    int unit, 
    uint32 flags, 
    bcm_port_tag_format_class_t tag_format_class_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_cable_state_t */
typedef _shr_port_cable_state_t bcm_port_cable_state_t;

#define BCM_PORT_CABLE_STATE_OK         _SHR_PORT_CABLE_STATE_OK 
#define BCM_PORT_CABLE_STATE_OPEN       _SHR_PORT_CABLE_STATE_OPEN 
#define BCM_PORT_CABLE_STATE_SHORT      _SHR_PORT_CABLE_STATE_SHORT 
#define BCM_PORT_CABLE_STATE_OPENSHORT  _SHR_PORT_CABLE_STATE_OPENSHORT 
#define BCM_PORT_CABLE_STATE_CROSSTALK  _SHR_PORT_CABLE_STATE_CROSSTALK 
#define BCM_PORT_CABLE_STATE_UNKNOWN    _SHR_PORT_CABLE_STATE_UNKNOWN 
#define BCM_PORT_CABLE_STATE_COUNT      _SHR_PORT_CABLE_STATE_COUNT 

/* bcm_port_cable_diag_t */
typedef _shr_port_cable_diag_t bcm_port_cable_diag_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Perform cable diagnostics on the specified port. */
extern int bcm_port_cable_diag(
    int unit, 
    bcm_port_t port, 
    bcm_port_cable_diag_t *status);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_L3_MODIFY_NO_SRCMAC    0x0001     
#define BCM_PORT_L3_MODIFY_NO_DSTMAC    0x0002     
#define BCM_PORT_L3_MODIFY_NO_TTL       0x0004     
#define BCM_PORT_L3_MODIFY_NO_VLAN      0x0008     

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the L3 unicast packet modification operations of a port. */
extern int bcm_port_l3_modify_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

/* Get or set the L3 unicast packet modification operations of a port. */
extern int bcm_port_l3_modify_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_IPMC_MODIFY_NO_SRCMAC  0x0001     
#define BCM_PORT_IPMC_MODIFY_NO_TTL     0x0004     
#define BCM_PORT_IPMC_CHECK_NO_TTL      0x0008     

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set the L3 multicast packet modification operations of a port. */
extern int bcm_port_ipmc_modify_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

/* Get or set the L3 multicast packet modification operations of a port. */
extern int bcm_port_ipmc_modify_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Port information valid fields
 * 
 * Each field in the bcm_port_info_t structure has a corresponding mask
 * bit to control whether to get or set that value during the execution
 * of the bcm_port_selective_get/_set functions. The OR of all requested
 * ATTR masks should be stored in the action_mask field and the OR of all
 * requested ATTR2 masks should be stored in the action_mask2 field of
 * the bcm_port_info_t before calling the functions.
 */
#define BCM_PORT_ATTR_ENABLE_MASK           0x00000001 
#define BCM_PORT_ATTR_LINKSTAT_MASK         0x00000002 /* Get only. */
#define BCM_PORT_ATTR_AUTONEG_MASK          0x00000004 
#define BCM_PORT_ATTR_SPEED_MASK            0x00000008 
#define BCM_PORT_ATTR_DUPLEX_MASK           0x00000010 
#define BCM_PORT_ATTR_LINKSCAN_MASK         0x00000020 
#define BCM_PORT_ATTR_LEARN_MASK            0x00000040 
#define BCM_PORT_ATTR_DISCARD_MASK          0x00000080 
#define BCM_PORT_ATTR_VLANFILTER_MASK       0x00000100 
#define BCM_PORT_ATTR_UNTAG_PRI_MASK        0x00000200 
#define BCM_PORT_ATTR_UNTAG_VLAN_MASK       0x00000400 
#define BCM_PORT_ATTR_STP_STATE_MASK        0x00000800 
#define BCM_PORT_ATTR_PFM_MASK              0x00001000 
#define BCM_PORT_ATTR_LOOPBACK_MASK         0x00002000 
#define BCM_PORT_ATTR_PHY_MASTER_MASK       0x00004000 
#define BCM_PORT_ATTR_INTERFACE_MASK        0x00008000 
#define BCM_PORT_ATTR_PAUSE_TX_MASK         0x00010000 
#define BCM_PORT_ATTR_PAUSE_RX_MASK         0x00020000 
#define BCM_PORT_ATTR_PAUSE_MAC_MASK        0x00040000 
#define BCM_PORT_ATTR_LOCAL_ADVERT_MASK     0x00080000 
#define BCM_PORT_ATTR_REMOTE_ADVERT_MASK    0x00100000 /* Get only. */
#define BCM_PORT_ATTR_ENCAP_MASK            0x00200000 
#define BCM_PORT_ATTR_RATE_MCAST_MASK       0x00400000 
#define BCM_PORT_ATTR_RATE_BCAST_MASK       0x00800000 
#define BCM_PORT_ATTR_RATE_DLFBC_MASK       0x01000000 
#define BCM_PORT_ATTR_SPEED_MAX_MASK        0x02000000 /* Get only. */
#define BCM_PORT_ATTR_ABILITY_MASK          0x04000000 /* Get only. */
#define BCM_PORT_ATTR_FRAME_MAX_MASK        0x08000000 
#define BCM_PORT_ATTR_MDIX_MASK             0x10000000 
#define BCM_PORT_ATTR_MDIX_STATUS_MASK      0x20000000 
#define BCM_PORT_ATTR_MEDIUM_MASK           0x40000000 
#define BCM_PORT_ATTR_FAULT_MASK            0x80000000 /* Get only. */
#define BCM_PORT_ATTR2_PORT_ABILITY         0x00000001 

/* Backward compatibility. */
#define BCM_PORT_ATTR_SPEED_MAX BCM_PORT_ATTR_SPEED_MAX_MASK 
#define BCM_PORT_ATTR_ABILITY   BCM_PORT_ATTR_ABILITY_MASK 

#define BCM_PORT_ATTR_ALL_MASK      0xffffffff 
#define BCM_PORT_ATTR_PAUSE_MASK    \
    (BCM_PORT_ATTR_PAUSE_TX_MASK    | \
     BCM_PORT_ATTR_PAUSE_RX_MASK) 
#define BCM_PORT_ATTR_RATE_MASK     \
    (BCM_PORT_ATTR_RATE_MCAST_MASK  | \
     BCM_PORT_ATTR_RATE_BCAST_MASK  | \
     BCM_PORT_ATTR_RATE_DLFBC_MASK) 

/* Attributes that can be controlled on BCM5670/75. */
#define BCM_PORT_HERC_ATTRS     \
    (BCM_PORT_ATTR_ENABLE_MASK      | \
     BCM_PORT_ATTR_LINKSTAT_MASK    | \
     BCM_PORT_ATTR_SPEED_MASK       | \
     BCM_PORT_ATTR_DUPLEX_MASK      | \
     BCM_PORT_ATTR_LINKSCAN_MASK    | \
     BCM_PORT_ATTR_INTERFACE_MASK   | \
     BCM_PORT_ATTR_LOOPBACK_MASK    | \
     BCM_PORT_ATTR_PAUSE_TX_MASK    | \
     BCM_PORT_ATTR_PAUSE_RX_MASK    | \
     BCM_PORT_ATTR_PAUSE_MAC_MASK   | \
     BCM_PORT_ATTR_FRAME_MAX_MASK   | \
     BCM_PORT_ATTR_ENCAP_MASK) 

/* Attributes specific to XGS devices. */
#define BCM_PORT_XGS_ATTRS      (BCM_PORT_ATTR_ENCAP_MASK) 

/* Auto-negotiated values. */
#define BCM_PORT_AN_ATTRS       \
    (BCM_PORT_ATTR_SPEED_MASK       | \
     BCM_PORT_ATTR_DUPLEX_MASK      | \
     BCM_PORT_ATTR_PAUSE_MASK) 

/* bcm_port_info_s */
typedef struct bcm_port_info_s {
    uint32 action_mask;                 /* BCM_PORT_ATTR_xxx. */
    uint32 action_mask2;                /* BCM_PORT_ATTR2_xxx. */
    int enable; 
    int linkstatus; 
    int autoneg; 
    int speed; 
    int duplex; 
    int linkscan; 
    uint32 learn; 
    int discard; 
    uint32 vlanfilter; 
    int untagged_priority; 
    bcm_vlan_t untagged_vlan; 
    int stp_state; 
    int pfm; 
    int loopback; 
    int phy_master; 
    bcm_port_if_t interface; 
    int pause_tx; 
    int pause_rx; 
    int encap_mode; 
    bcm_mac_t pause_mac; 
    bcm_port_abil_t local_advert; 
    bcm_port_ability_t local_ability; 
    int remote_advert_valid; 
    bcm_port_abil_t remote_advert; 
    bcm_port_ability_t remote_ability; 
    int mcast_limit; 
    int mcast_limit_enable; 
    int bcast_limit; 
    int bcast_limit_enable; 
    int dlfbc_limit; 
    int dlfbc_limit_enable; 
    int speed_max; 
    bcm_port_abil_t ability; 
    bcm_port_ability_t port_ability; 
    int frame_max; 
    bcm_port_mdix_t mdix; 
    bcm_port_mdix_status_t mdix_status; 
    bcm_port_medium_t medium; 
    uint32 fault; 
} bcm_port_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set multiple port characteristics. */
extern int bcm_port_info_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* Get or set multiple port characteristics. */
extern int bcm_port_info_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* Get or set multiple port characteristics. */
extern int bcm_port_selective_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* Get or set multiple port characteristics. */
extern int bcm_port_selective_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* Get or set multiple port characteristics. */
extern int bcm_port_info_restore(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* Get or set multiple port characteristics. */
extern int bcm_port_info_save(
    int unit, 
    bcm_port_t port, 
    bcm_port_info_t *info);

/* Gets the physical driver name. */
extern int bcm_port_phy_drv_name_get(
    int unit, 
    bcm_port_t port, 
    char *name, 
    int len);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Values for EPON port DBA Polling Intervals. */
#define BCM_PORT_PON_POLLING_INTERVALS  8          /* Number of DBA Polling
                                                      Intervals. */

/* bcm_port_pon_info_s */
typedef struct bcm_port_pon_info_s {
    int llid; 
    int enable; 
    int linkstatus; 
    int oam_discovery_status; 
    int loopback; 
    bcm_mac_t olt_mac_addr; 
    int polling_intervals[BCM_PORT_PON_POLLING_INTERVALS]; 
} bcm_port_pon_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set multiple port characteristics for PON and link port. */
extern int bcm_port_pon_info_get(
    int unit, 
    bcm_port_t pon_port, 
    bcm_port_pon_info_t *info);

/* Get or set multiple port characteristics for PON and link port. */
extern int bcm_port_pon_info_set(
    int unit, 
    bcm_port_t pon_port, 
    bcm_port_pon_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port link fault signalling. */
#define BCM_PORT_FAULT_LOCAL    _SHR_PORT_FAULT_LOCAL 
#define BCM_PORT_FAULT_REMOTE   _SHR_PORT_FAULT_REMOTE 

/* 
 * Port Rate Egress Traffic identifiers for QE FIFO Selectors into
 * Shapers.
 */
#define BCM_PORT_RATE_TRAFFIC_UC_EF         0x0001     
#define BCM_PORT_RATE_TRAFFIC_UC_NON_EF     0x0002     
#define BCM_PORT_RATE_TRAFFIC_NON_UC_EF     0x0200     
#define BCM_PORT_RATE_TRAFFIC_NON_UC_NON_EF 0x0100     
#define BCM_PORT_RATE_TRAFFIC_ALL           0x0303     

#ifndef BCM_HIDE_DISPATCHABLE

/* Get link fault type. */
extern int bcm_port_fault_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags);

/* 
 * Set or get port index of a trunk for ingress port that is used to
 * select the egress port in the trunk.
 */
extern int bcm_port_trunk_index_set(
    int unit, 
    bcm_port_t port, 
    int port_index);

/* 
 * Set or get port index of a trunk for ingress port that is used to
 * select the egress port in the trunk.
 */
extern int bcm_port_trunk_index_get(
    int unit, 
    bcm_port_t port, 
    int *port_index);

/* Set or retrieve color assignment for a given port and priority. */
extern int bcm_port_priority_color_set(
    int unit, 
    bcm_port_t port, 
    int prio, 
    bcm_color_t color);

/* Set or retrieve color assignment for a given port and priority. */
extern int bcm_port_priority_color_get(
    int unit, 
    bcm_port_t port, 
    int prio, 
    bcm_color_t *color);

/* 
 * Set or retrieve color assignment for a given port and Canonical Format
 * Indicator (CFI).
 */
extern int bcm_port_cfi_color_set(
    int unit, 
    bcm_port_t port, 
    int cfi, 
    bcm_color_t color);

/* 
 * Set or retrieve color assignment for a given port and Canonical Format
 * Indicator (CFI).
 */
extern int bcm_port_cfi_color_get(
    int unit, 
    bcm_port_t port, 
    int cfi, 
    bcm_color_t *color);

/* 
 * Map the incoming packet priority and CFI to internal priority and
 * color.
 */
extern int bcm_port_vlan_priority_map_set(
    int unit, 
    bcm_port_t port, 
    int pkt_pri, 
    int cfi, 
    int internal_pri, 
    bcm_color_t color);

/* 
 * Map the incoming packet priority and CFI to internal priority and
 * color.
 */
extern int bcm_port_vlan_priority_map_get(
    int unit, 
    bcm_port_t port, 
    int pkt_pri, 
    int cfi, 
    int *internal_pri, 
    bcm_color_t *color);

/* 
 * Map the internal priority and color to outgoing packet priority and
 * Canonical Format Indicator (CFI).
 */
extern int bcm_port_vlan_priority_unmap_set(
    int unit, 
    bcm_port_t port, 
    int internal_pri, 
    bcm_color_t color, 
    int pkt_pri, 
    int cfi);

/* 
 * Map the internal priority and color to outgoing packet priority and
 * Canonical Format Indicator (CFI).
 */
extern int bcm_port_vlan_priority_unmap_get(
    int unit, 
    bcm_port_t port, 
    int internal_pri, 
    bcm_color_t color, 
    int *pkt_pri, 
    int *cfi);

/* 
 * Map the incoming packet priority and CFI to internal priority and
 * color on a per-port per-vlan basis.
 */
extern int bcm_port_vlan_pri_map_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    int pkt_pri, 
    int cfi, 
    int internal_pri, 
    bcm_color_t color);

/* 
 * Get the mapping of incoming packet priority and CFI to internal
 * priority and color.
 */
extern int bcm_port_vlan_pri_map_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    int pkt_pri, 
    int cfi, 
    int *internal_pri, 
    bcm_color_t *color);

/* Map the incoming IP packet DSCP to internal priority and color. */
extern int bcm_port_vlan_dscp_map_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    int dscp, 
    int internal_pri, 
    bcm_color_t color);

/* Map the incoming IP packet DSCP to internal priority and color. */
extern int bcm_port_vlan_dscp_map_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    int dscp, 
    int *internal_pri, 
    bcm_color_t *color);

/* Map the internal priority and color to outgoing packet DSCP. */
extern int bcm_port_dscp_unmap_set(
    int unit, 
    bcm_port_t port, 
    int internal_pri, 
    bcm_color_t color, 
    int pkt_dscp);

/* Map the internal priority and color to outgoing packet DSCP. */
extern int bcm_port_dscp_unmap_get(
    int unit, 
    bcm_port_t port, 
    int internal_pri, 
    bcm_color_t color, 
    int *pkt_dscp);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a Priority Mapping structure. */
extern void bcm_priority_mapping_t_init(
    bcm_priority_mapping_t *pri_map);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set the mapping of wire/packet PRI and CFI to internal priority and
 * color.
 */
extern int bcm_port_vlan_priority_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vid, 
    int pkt_pri, 
    int cfi, 
    bcm_priority_mapping_t *pri_map);

/* 
 * Get the mapping of wire/packet PRI and CFI to internal priority and
 * color.
 */
extern int bcm_port_vlan_priority_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vid, 
    int pkt_pri, 
    int cfi, 
    bcm_priority_mapping_t *pri_map);

/* Set the inner tag value to be added to the outgoing packet. */
extern int bcm_port_vlan_inner_tag_set(
    int unit, 
    bcm_port_t port, 
    uint16 inner_tag);

/* Set the inner tag value to be added to the outgoing packet. */
extern int bcm_port_vlan_inner_tag_get(
    int unit, 
    bcm_port_t port, 
    uint16 *inner_tag);

/* Set or get egress override port. */
extern int bcm_port_force_forward_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_t egr_port, 
    int enable);

/* Set or get egress override port. */
extern int bcm_port_force_forward_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_t *egr_port, 
    int *enabled);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_FORCE_FORWARD_DISABLE  0x0        /* Normal switching */
#define BCM_PORT_FORCE_FORWARD_ALL      0x1        /* Redirect all ingress port
                                                      traffic to specified port */
#define BCM_PORT_FORCE_FORWARD_LOCAL    0x2        /* Redirect all ingress port
                                                      traffic which would
                                                      otherwise go to a local
                                                      port to the specified port */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or get egress override port mode. */
extern int bcm_port_force_forward_mode_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_t egr_port, 
    uint32 flags);

/* Set or get egress override port mode. */
extern int bcm_port_force_forward_mode_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_t *egr_port, 
    uint32 *flags);

/* 
 * Get or set the destination of a packet (recycled-path) for the second
 * pass.
 */
extern int bcm_port_force_dest_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_dest_info_t *dest_info);

/* 
 * Get or set the destination of a packet (recycled-path) for the second
 * pass.
 */
extern int bcm_port_force_dest_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_dest_info_t *dest_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_class_e */
typedef enum bcm_port_class_e {
    bcmPortClassFieldLookup = 0,        /* Class for field stage Lookup */
    bcmPortClassFieldIngress = 1,       /* Class for field stage Ingress */
    bcmPortClassFieldEgress = 2,        /* Class for field stage Egress */
    bcmPortClassVlanTranslateEgress = 3, /* Class for egress VLAN translation */
    bcmPortClassIngress = 4,            /* Class for ingress basis grouping */
    bcmPortClassId = 5,                 /* Class for mapping local-port to
                                           vlan-domain. vlan domain is used in
                                           mapping packet VID to Vlan */
    bcmPortClassFieldIngressPacketProcessing = 6, /* Packet processing port Class lower 32
                                           bits for field stage Ingress */
    bcmPortClassFieldEgressPacketProcessing = 7, /* Packet processing port Class for
                                           field stage Egress */
    bcmPortClassL2Lookup = 8,           /* Class for L2 lookup */
    bcmPortClassForwardIngress = 9,     /* Class for ingress forwarding */
    bcmPortClassForwardEgress = 10,     /* Class for egress forwarding */
    bcmPortClassFieldIngressVlanTranslation = 11, /* Class for field stage Ingress Vlan
                                           Translation */
    bcmPortClassFieldIngressTunnelTerminated = 12, /* Class for field stage Ingress Tunnel
                                           Terminated */
    bcmPortClassProgramEditorEgressPacketProcessing = 13, /* Class for Egress Programmable Editor
                                           properties */
    bcmPortClassEgress = 14,            /* EGR_PORT class ID field for field
                                           stage Egress */
    bcmPortClassFieldIngressGroupSel = 15, /* Per port Class ID for field stage
                                           Ingress group selection */
    bcmPortClassFieldIngressSystemPort = 16, /* virtual port instance of SGLP in IFP
                                           IPBM key */
    bcmPortClassFieldIngressSourceGport = 17, /* virtual port instance of SVP in IFP
                                           IPBM key */
    bcmPortClassFieldIngressDevicePort = 18, /* virtual port instance of device port
                                           in IFP IPBM key */
    bcmPortClassVlanTranslateIngress = 19, /* Class for ingress VLAN translation */
    bcmPortClassProgramEditorEgressTrafficManagement = 20, /* Traffic management port Class for
                                           field stage Egress */
    bcmPortClassFieldIngressPacketProcessingHigh = 21, /* Packet processing port Class high 32
                                           bits for field stage Ingress */
    bcmPortClassVlanMember = 22,        /* Class for VLAN Membership */
    bcmPortClassFieldIngressVport = 23, /* Class for LIF encoded as Gport in the
                                           Field Ingres stages */
    bcmPortClassFieldEgressVport = 24,  /* Class for LIF encoded as Gport in the
                                           Field stage ePMF */
    bcmPortClassFieldIngressPMF1PacketProcessingPort = 25, /* Class for packet processing port in
                                           Field stage iPMF1 */
    bcmPortClassFieldIngressPMF1PacketProcessingPortCs = 26, /* Context selection class for packet
                                           processing port in Field stage iPMF1 */
    bcmPortClassFieldIngressPMF3PacketProcessingPort = 27, /* Class for packet processing port in
                                           Field stage iPMF3 */
    bcmPortClassFieldIngressPMF3PacketProcessingPortCs = 28, /* Context selection class for packet
                                           processing port in Field stage iPMF3 */
    bcmPortClassFieldEgressPacketProcessingPort = 29, /* Class for packet processing port in
                                           Field stage ePMF */
    bcmPortClassFieldEgressPacketProcessingPortCs = 30, /* Context selection class for packet
                                           processing port in Field stage ePMF */
    bcmPortClassFieldExternalPacketProcessingPortCs = 31, /* Context selection class for packet
                                           processing port in Field stage
                                           external */
    bcmPortClassFieldIngressPMF1TrafficManagementPort = 32, /* Class for traffic management port in
                                           Field stage iPMF1 */
    bcmPortClassFieldIngressPMF1TrafficManagementPortCs = 33, /* Context selection class for traffic
                                           management port in Field stage iPMF1 */
    bcmPortClassFieldIngressPMF3TrafficManagementPort = 34, /* Class for traffic management port in
                                           Field stage iPMF3 */
    bcmPortClassFieldIngressPMF3TrafficManagementPortCs = 35, /* Context selection class for traffic
                                           management port in Field stage iPMF3 */
    bcmPortClassFieldEgressTrafficManagementPort = 36, /* Class for traffic management
                                           processing port in Field stage ePMF */
    bcmPortClassFieldExternalTrafficManagementPortCs = 37, /* Context selection class for traffic
                                           management port in Field stage
                                           external */
    bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData = 38, /* Extra constant to assign to a packet
                                           processing port, up to 32lsb. Field
                                           stage iPMF1. */
    bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh = 39, /* Extra constant to assign to a packet
                                           processing port, beyond the first 32
                                           bits. Field stage iPMF1. */
    bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData = 40, /* Extra constant to assign to a packet
                                           processing port, first 32 bits. Field
                                           stage iPMF3. */
    bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh = 41, /* Extra constant to assign to a packet
                                           processing port, beyond the first 32
                                           bits. Field stage iPMF3. */
    bcmPortClassOpaqueCtrlId = 42,      /* Opaque control ID available for IFP
                                           key selection */
    bcmPortClassEgressOpaqueCtrlId = 43, /* Opaque control ID available for EFP
                                           key selection */
    bcmPortClassOpaqueCtrlId1 = 44,     /* Opaque control ID available for VFP
                                           key selection */
    bcmPortClassOpaqueCtrlId2 = 45,     /* Opaque control ID A for flex digest
                                           and VFP key selection. */
    bcmPortClassOpaqueCtrlId3 = 46,     /* Opaque control ID B for flex digest
                                           and VFP key selection. */
    bcmPortClassCount = 47              /* Last enum value. */
} bcm_port_class_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set or get port classification ID to aggregate a group of ports for
 * further processing such as VLAN translation and field processing.
 */
extern int bcm_port_class_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_class_t pclass, 
    uint32 class_id);

/* 
 * Set or get port classification ID to aggregate a group of ports for
 * further processing such as VLAN translation and field processing.
 */
extern int bcm_port_class_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_class_t pclass, 
    uint32 *class_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Features that can be controlled on a per-port basis. */
typedef enum bcm_port_control_e {
    bcmPortControlBridge = 0,           /* Enable local port bridging. */
    bcmPortControlTrunkHashSet = 1,     /* Select one of the two sets of hash
                                           bits selection configuration for
                                           trunking. */
    bcmPortControlFabricTrunkHashSet = 2, /* Select one of the two sets of hash
                                           bits selection configuration for
                                           fabric trunking. */
    bcmPortControlECMPHashSet = 3,      /* Select one of the two sets of hash
                                           bits selection configuration for
                                           ECMP. */
    bcmPortControlLoadBalanceHashSet = 4, /* Select one of the two sets of hash
                                           bits selection configuration for LBID
                                           to be populated in HiGig2 header. */
    bcmPortControlLoadBalancingNumber = 5, /* Assigns the Load Balancing Number of
                                           this port for HiGig2 headers. */
    bcmPortControlErrorSymbolDetect = 6, /* Enable or disable XAUI |E| monitoring
                                           feature  for port's XAUI lanes. */
    bcmPortControlErrorSymbolCount = 7, /* Get the number of XAUI |E| symbol
                                           occured since last read. Setting any
                                           value to the count will set it to
                                           zero. */
    bcmPortControlIP4 = 8,              /* Enable IPv4 Routing on port. */
    bcmPortControlIP6 = 9,              /* Enable IPv6 Routing on port. */
    bcmPortControlIP4Mcast = 10,        /* Enable IPv4 Multicast on port. */
    bcmPortControlIP6Mcast = 11,        /* Enable IPv6 Multicast on port. */
    bcmPortControlIP4McastL2 = 12,      /* Enable L2 switching on IPv4 IPMC
                                           packets based on (SG,*G) instead of
                                           (DMAC,VLAN). */
    bcmPortControlIP6McastL2 = 13,      /* Enable L2 switching on IPv6 IPMC
                                           packets based on (SG,*G) instead of
                                           (DMAC,VLAN). */
    bcmPortControlMpls = 14,            /* Enable MPLS on the port ingress. */
    bcmPortControlPassControlFrames = 15, /* Enable controls frames on port. */
    bcmPortControlFilterLookup = 16,    /* Enable port lookup Field Processor. */
    bcmPortControlFilterIngress = 17,   /* Enable ingress Field Processor on
                                           port. */
    bcmPortControlFilterEgress = 18,    /* Enable egress Field Processor on
                                           port. */
    bcmPortControlFrameSpacingStretch = 19, /* One byte of inter packet gap is added
                                           for specified number of packet data
                                           bytes. If the value is 10, one byte
                                           of IPG is added for each 10 byte of
                                           packet data. */
    bcmPortControlPreservePacketPriority = 20, /* Preserve packet original IEEE 802.1p
                                           priority. */
    bcmPortControlLearnClassEnable = 21, /* Enable class based station movement
                                           checks. */
    bcmPortControlTrustIncomingVlan = 22, /* Trust incoming packet's Vlan tag */
    bcmPortControlDoNotCheckVlan = 23,  /* Enable/disable spanning tree and Vlan
                                           membership checks on ingress Ethernet
                                           and higig packets. For CPU port
                                           configuration is applied only for
                                           higig packets. To configure these
                                           settings for Ethernet packets
                                           bcmPortControlDoNotCheckVlanFromCpu
                                           port control can be used. */
    bcmPortControlDoNotCheckVlanFromCpu = 24, /* Enable/disable spanning tree and Vlan
                                           membership checks on Ethernet packets
                                           that are ingressed from CPU port. */
    bcmPortControlIEEE8021ASEnableIngress = 25, /* Enable/disable ingress processing of
                                           IEEE 802.1AS protocol packets. */
    bcmPortControlIEEE8021ASEnableEgress = 26, /* Enable/disable egress processing of
                                           IEEE 802.1AS protocol packets. */
    bcmPortControlPrbsMode = 27,        /* PRBS location - 0=>Phy PRBS 1=>MAC/SI
                                           Port */
    bcmPortControlPrbsPolynomial = 28,  /* Assigns PRBS polynomial,
                                           BCM_PORT_PRBS_POLYNOMIAL_ */
    bcmPortControlPrbsTxInvertData = 29, /* Configure inversion of Tx data */
    bcmPortControlPrbsForceTxError = 30, /* Configure insertion of Tx errors */
    bcmPortControlPrbsTxEnable = 31,    /* Enable Tx PRBS */
    bcmPortControlPrbsRxEnable = 32,    /* Enable Rx PRBS */
    bcmPortControlPrbsRxStatus = 33,    /* PRBS Rx status */
    bcmPortControlSerdesDriverStrength = 34, /* Assigns Driver Strength */
    bcmPortControlSerdesDriverEqualization = 35, /* Assigns Driver Equalization */
    bcmPortControlSerdesDriverEqualizationFarEnd = 36, /* Informs software of far end transmit
                                           driver equalization setting for
                                           tuning */
    bcmPortControlSerdesDriverTune = 37, /* Start hardware receive equalization
                                           tuning */
    bcmPortControlSerdesDriverEqualizationTuneStatusFarEnd = 38, /* Get results of tune operation which
                                           include suggested far end
                                           equalization update */
    bcmPortControlAbility = 39,         /* Assigns Port ability,
                                           BCM_PORT_ABILITY_ */
    bcmPortControlMacInMac = 40,        /* Enable/Disable MiM frame processing */
    bcmPortControlSrcBmacMissToCpu = 41, /* Mac-in-Mac Source BMAC lookup miss,
                                           Send to CPU (value = TRUE/FALSE) */
    bcmPortControlOamLoopback = 42,     /* Enable/Disable 802.3 Clause 57 Oam
                                           Loopback */
    bcmPortControlEgressVlanPriUsesPktPri = 43, /* If set, outgoing packets derive their
                                           priority from the incoming priority */
    bcmPortControlPacketFlowMode = 45,  /* Enable/Disable requeue for hybrid
                                           mode */
    bcmPortControlTrain = 46,           /* Start/stop training sequence */
    bcmPortControlRxEnable = 47,        /* Rx enable/disable */
    bcmPortControlTxEnable = 48,        /* Tx enable/disable */
    bcmPortControlRxLink = 49,          /* Retreives Rx Status */
    bcmPortControlTxLink = 50,          /* Retreives Tx Status */
    bcmPortControlIpfixRate = 51,       /* Set IPFIX flow rate meter id */
    bcmPortControlOAMEnable = 52,       /* Enable reception of OAM messages */
    bcmPortControlCustomerQueuing = 53, /* Enable customer domain queuing */
    bcmPortControlFabricKnockoutId = 54, /* sets port identifier, used for source
                                           knockout functionality */
    bcmPortControlLanes = 55,           /* Sets the number of active lanes for a
                                           port that can be dynamically
                                           hot-swapped. */
    bcmPortControlEgressNonUnicastLossless = 56, /* Enable/Disable multicast Lossless
                                           operation */
    bcmPortControlPFCEthertype = 57,    /* Priority Flow Control packet
                                           EtherType */
    bcmPortControlPFCOpcode = 58,       /* Priority Flow Control packet opcode */
    bcmPortControlPFCReceive = 59,      /* Priority Flow Control packet receive
                                           enable */
    bcmPortControlPFCTransmit = 60,     /* Priority Flow Control packet transmit
                                           enable */
    bcmPortControlPFCClasses = 61,      /* Priority Flow Control number of
                                           classes */
    bcmPortControlPFCPassFrames = 62,   /* Allow Priority Flow Control packets
                                           into switch device */
    bcmPortControlPFCDestMacOui = 63,   /* Priority Flow Control packet
                                           destination MAC address, upper three
                                           bytes */
    bcmPortControlPFCDestMacNonOui = 64, /* Priority Flow Control packet
                                           destination MAC address, lower three
                                           bytes */
    bcmPortControlPFCRefreshTime = 65,  /* Priority Flow Control refresh time */
    bcmPortControlVrf = 66,             /* Enable per-port VRF to override
                                           per-Vlan VRF */
    bcmPortControlL3Ingress = 67,       /* Enable per-port L3-IIF for IP traffic */
    bcmPortControlL2Learn = 69,         /* Configure L2 learning behavior using
                                           BCM_PORT_LEARN_xxx flags */
    bcmPortControlL2Move = 70,          /* Configure L2 station movement
                                           behavior using BCM_PORT_LEARN_xxx
                                           flags */
    bcmPortControlForwardStaticL2MovePkt = 71, /* Allow forwarding L2 packet even if L2
                                           movement is detected against an
                                           existing static entry in L2 table */
    bcmPortControlLinkFaultLocalEnable = 72, /* Enable local LSS message processing */
    bcmPortControlLinkFaultRemoteEnable = 73, /* Enable remote LSS message processing */
    bcmPortControlLinkFaultLocal = 74,  /* Read local LSS message processing
                                           status */
    bcmPortControlLinkFaultRemote = 75, /* Read remote LSS message processing
                                           status */
    bcmPortControlTimestampTransmit = 76, /* Read the timestamp value assigned to
                                           TX packet */
    bcmPortControlTimestampEnable = 77, /* Set the port to be capable of
                                           timestamping TX packet */
    bcmPortControlTCPriority = 78,      /* Indicates the default Traffic Class
                                           attributes assoiated with the port */
    bcmPortControlDropPrecedence = 79,  /* Indicates the default Drop Precedence
                                           attributes assoiated with the port */
    bcmPortControlFabricQueue = 80,     /* Enable ext header on HiGig port. */
    bcmPortControlLinkdownTransmit = 81, /* Enable transmission when linkdown */
    bcmPortControlEEETransmitWakeTime = 82, /* Time(in microsecs) to wait before
                                           transmitter can begin transmitting
                                           leaving Low Power Idle State */
    bcmPortControlEEEReceiveWakeTime = 83, /* Time(in microsecs) Receiver counts
                                           for SLEEP or IDLE symbols to move to
                                           Active from Low Power Idle State */
    bcmPortControlEEETransmitSleepTime = 84, /* Time(in microsecs) Tranmitter sends
                                           SLEEP symbols before going to QUIET
                                           state */
    bcmPortControlEEEReceiveSleepTime = 85, /* Time(in microsecs) Receiver is in
                                           SLEEP state before entering QUIET
                                           state */
    bcmPortControlEEETransmitQuietTime = 86, /* Time(in microsecs) transmit stays in
                                           Quiet state before entering REFRESH
                                           state */
    bcmPortControlEEEReceiveQuietTime = 87, /* Time(in microsecs) Receiver stays in
                                           Quiet state before it expects REFRESH
                                           signal */
    bcmPortControlEEETransmitRefreshTime = 88, /* Time(in microsecs) transmitter stays
                                           in Refresh state */
    bcmPortControlSerdesTuneMarginMax = 89, /* SerDes max tune margin value */
    bcmPortControlSerdesTuneMarginMode = 90, /* SerDes in tune margin mode */
    bcmPortControlSerdesTuneMarginValue = 91, /* Value used to measure tune margin */
    bcmPortControlStatOversize = 92,    /* Threshold above which packet will be
                                           counted as oversized */
    bcmPortControlStatOversizeIsError = 93, /* Oversize Threshold flag to control
                                           oversize packets as error counts */
    bcmPortControlEEEEnable = 94,       /* Enable/Disable Energy Efficient
                                           Ethernet */
    bcmPortControlEEEStatisticsClear = 95, /* Clear Energy Efficient Ethernet Low
                                           Power Idle Statistics */
    bcmPortControlEEETransmitIdleTime = 96, /* Time (in microsecs) for which
                                           condition to move to LPI state is
                                           satisfied, at the end of which MAC
                                           transitions to LPI state */
    bcmPortControlEEETransmitEventCount = 97, /* Number of time MAC TX enters LPI
                                           state for a given measurement
                                           interval */
    bcmPortControlEEETransmitDuration = 98, /* Time (in microseconds) for which MAC
                                           TX is in LPI state during a
                                           measurement interval */
    bcmPortControlEEEReceiveEventCount = 99, /* Number of time MAC RX enters LPI
                                           state for a given measurement
                                           interval */
    bcmPortControlEEEReceiveDuration = 100, /* Time (in microseconds) for which MAC
                                           RX is in LPI state during a
                                           measurement interval */
    bcmPortControlEEETransmitIdleTimeHund = 101, /* The same as
                                           PortControlEEETransmitIdleTime for
                                           the link speed at 100MB data rate */
    bcmPortControlEEETransmitWakeTimeHund = 102, /* The same as
                                           PortControlEEETransmitWakeTime for
                                           the link speed at 100MB data rate */
    bcmPortControlEEETransmitMinLPITime = 103, /* Time (in microseconds) MAC stay in
                                           LPI state before leaving it */
    bcmPortControlEEETransmitMinLPITimeHund = 104, /* The same as
                                           PortControlEEETransmitMinLPITime for
                                           the link speed at 100MB data rate */
    bcmPortControlNivAccessVirtualInterfaceId = 105, /* Source virtual interface identifier
                                           (value range 0-0xfff) */
    bcmPortControlNivNonVntagDrop = 106, /* Set Ingress drop control, if VNTAG is
                                           not present  (value = TRUE/FALSE) */
    bcmPortControlNivVntagDrop = 107,   /* Set Ingress drop control, if VNTAG is
                                           present (value = TRUE/FALSE) */
    bcmPortControlNivNonVntagAdd = 108, /* Set ADD tag action, when VNTAG is not
                                           present(value = TRUE/FALSE) */
    bcmPortControlNivVntagIngressReplace = 109, /* Set REPLACE Ingress tag action, when
                                           VNTAG is present (value = TRUE/FALSE) */
    bcmPortControlNivVntagEgressReplace = 110, /* Set REPLACE Egress tag action, when
                                           VNTAG is present (value = TRUE/FALSE) */
    bcmPortControlNivVntagIngressDelete = 111, /* Set DELETE Ingress tag action, when
                                           VNTAG is present (value = TRUE/FALSE) */
    bcmPortControlNivVntagEgressDelete = 112, /* Set DELETE Egress tag action, when
                                           VNTAG is present (value = TRUE/FALSE) */
    bcmPortControlNivRpfCheck = 113,    /* Set incoming packet VNTAG.SRC_VIF RPF
                                           check control (value = TRUE/FALSE) */
    bcmPortControlNivSourceKnockout = 114, /* Set outgoing packet NIV pruning check
                                           control (value = TRUE/FALSE) */
    bcmPortControlNivForwardPort = 115, /* Set destination port for forwarding -
                                           mainly for upstream traffic only, but
                                           use -1 as special to flag the port as
                                           uplink port (value = local port
                                           number or -1) */
    bcmPortControlNivType = 116,        /* Set NIV port type (enumerated value
                                           within bcm_port_niv_type_t) */
    bcmPortControlNivNameSpace = 117,   /* Set namespace value (value range
                                           0-0xfff) */
    bcmPortControlRemoteCpuEnable = 118, /* Enables the port to accept packets
                                           from Remote CPU */
    bcmPortControlRemoteCpuTcMapping = 119, /* Enable/Disable the mapping between TC
                                           and COS queue for received ToCPU
                                           packets */
    bcmPortControlArbiterActive = 120,  /* Indicates if the SCI link is active
                                           and its arbiter association. -1
                                           indicates disabled, 0 indicates
                                           active and associted with Arbiter 0,
                                           1 indicates active and associated
                                           with Arbiter 1 */
    bcmPortControlArbiterForceDown = 121, /* Status to be indicated to arbiter.
                                           TRUE forces Down, FALSE does not
                                           force down */
    bcmPortControlTrill = 123,          /* Enable/Disable TRILL frame processing
                                           (value = TRUE/FALSE) */
    bcmPortControlTrillAllow = 124,     /* Allow TRILL frames (value =
                                           TRUE/FALSE) */
    bcmPortControlNonTrillAllow = 125,  /* Allow Non-TRILL frames (value =
                                           TRUE/FALSE) */
    bcmPortControlTrillCoreISISToCPU = 126, /* Core IS-IS frames received, Send to
                                           CPU (value = TRUE/FALSE) */
    bcmPortControlTrillHashSelect = 127, /* Select one of 2 sets of configuration
                                           for TRILL ECMP hashing (value = 1/0 ) */
    bcmPortControlFabricHeaderFormat = 128, /* Select appropriate fabric header
                                           type. */
    bcmPortControlFabricSourceKnockout = 130, /* Disables packet forwarding to
                                           non-trunk port on local modid */
    bcmPortControlFaultResponseLocalDataAndIdle = 131, /* If Local Fault indicated, continue
                                           sending data, send Idles */
    bcmPortControlFaultResponseLocalDataAndRf = 132, /* If Local Fault indicated, continue
                                           sending data, send Remote Fault
                                           Indication */
    bcmPortControlFaultResponseLocalDataAndLf = 133, /* If Local Fault indicated, continue
                                           sending data, send Local Fault
                                           Indication */
    bcmPortControlFaultResponseLocalNoDataIdle = 134, /* If Local Fault indicated, stop
                                           sending data, send Idles */
    bcmPortControlFaultResponseLocalNoDataRf = 135, /* If Local Fault indicated, stop
                                           sending data, send Remote Fault
                                           Indication */
    bcmPortControlFaultResponseLocalNoDataLf = 136, /* If Local Fault indicated, stop
                                           sending data, send Local Fault
                                           Indication */
    bcmPortControlFaultResponseRemoteDataAndIdle = 137, /* If Remote Fault indicated, continue
                                           sending data, send Idles */
    bcmPortControlFaultResponseRemoteDataAndRf = 138, /* If Remote Fault indicated, continue
                                           sending data, send Remote Fault
                                           Indication */
    bcmPortControlFaultResponseRemoteDataAndLf = 139, /* If Remote Fault indicated, continue
                                           sending data, send Remote Fault
                                           Indication */
    bcmPortControlFaultResponseRemoteNoDataIdle = 140, /* If Remote Fault indicated, stop
                                           sending data, send Idles */
    bcmPortControlFaultResponseRemoteNoDataRf = 141, /* If Remote Fault indicated, stop
                                           sending data, send Remote Fault
                                           Indication */
    bcmPortControlFaultResponseRemoteNoDataLf = 142, /* If Remote Fault indicated, stop
                                           sending data, send Remote Fault
                                           Indication */
    bcmPortControlPCS = 143,            /* The Physical Coding Sub-layer of the
                                           port */
    bcmPortControlLinkDownPowerOn = 144, /* 0: The Link is disabled on the MAC
                                           level, the SerDes power-state is not
                                           changed 1: Activate the link on the
                                           MAC level, the SerDes power-state is
                                           not changed */
    bcmPortControlRegex = 145,          /* Enable/Disable tracking of IPv4 flows */
    bcmPortControlRegexTcp = 146,       /* Enable/Disable tracking of IPv6 flows */
    bcmPortControlRegexUdp = 147,       /* Enable/Disable tracking of IPv4 UDP
                                           flows */
    bcmPortControlPonEncryptKeyExpiryTime = 148, /* Set timeout value for encryption keys
                                           (s) */
    bcmPortControlPonHoldoverState = 149, /* Enable holdover state */
    bcmPortControlPonHoldoverTime = 150, /* Set holdover time (ms) */
    bcmPortControlPonMultiLlid = 151,   /* Set LLID number */
    bcmPortControlPonFecMode = 152,     /* Set FEC mode on Rx and Tx together,
                                           Rx/Tx off (value = 0), Rx on and Tx
                                           off (value = 1), Rx off and Tx on
                                           (value = 2), Rx/Tx on (value = 3) or
                                           FEC is settable per link (value = 4) */
    bcmPortControlPonUserTraffic = 153, /* Enable user traffic */
    bcmPortControlEthPortAutoNegFailureAlarmState = 154, /* Alarm on/off state of the
                                           EthPortAutoNegFailure */
    bcmPortControlEthPortLosAlarmState = 155, /* Alarm on/off state of the EthPortLos */
    bcmPortControlEthPortFailureAlarmState = 156, /* Alarm on/off state of the
                                           EthPortFailure */
    bcmPortControlEthPortLoopbackAlarmState = 157, /* Alarm on/off state of the
                                           EthPortLoopback */
    bcmPortControlEthPortCongestionAlarmState = 158, /* Alarm on/off state of the
                                           EthPortCongestion */
    bcmPortControlMplsEntropyHashSet = 159, /* Select one of 2 sets of configuration
                                           for MPLS Entropy hashing (value =
                                           1/0) */
    bcmPortControlStripCRC = 160,       /* If set no cell CRC is calculated,
                                           else 8-bit CRC will be added to each
                                           data cell. Relevant for VSC256 mode
                                           only. */
    bcmPortControlPrivateVlanIsolate = 161, /* Argument = 1/0. Enable, disable
                                           system port being port isolate type.
                                           By default port will be community. */
    bcmPortControlPrivateVlanModeSet = bcmPortControlPrivateVlanIsolate, /* Argument = 2/1/0. Set system port
                                           private vlan mode (0 : community, 1 :
                                           isolate,  2 : promiscuous). By
                                           default it is community. */
    bcmPortControlFloodUnknownUcastGroup = 162, /* Override the per-vsi settings. */
    bcmPortControlFloodUnknownMcastGroup = 163, /* Override the per-vsi settings. */
    bcmPortControlFloodBroadcastGroup = 164, /* Override the per-vsi settings. */
    bcmPortControlDefaultQueue = 165,   /* Specify default queue ID for use with
                                           the port */
    bcmPortControlL2GreEnable = 166,    /* Set per port enable for L2-GRE
                                           (Value=TRUE/FALSE) */
    bcmPortControlL2GreTunnelbasedVpnId = 167, /* Set per port VPNID lookup key
                                           (Value=TRUE/FALSE) */
    bcmPortControlL2GreDefaultTunnelEnable = 168, /* Set per port dafult-SVP for Tunnel
                                           lookup failure(Value=TRUE/FALSE) */
    bcmPortControlL2SaAuthenticaion = 169, /* to enable SA authentication check on
                                           a port */
    bcmPortControlUnknownMacDaAction = 170, /* Per port configures set of actions
                                           that are taken by the device when an
                                           unknown destination address is
                                           recognized on an ingressing packet */
    bcmPortControlDiscardMacSaAction = 171, /* Per port configures set of actions
                                           that are taken by the device upon SA
                                           lookup when L2 entry flag with
                                           SA-discard */
    bcmPortControlManagementPacketIp6Reserved = 172, /* Classify Packets with IPV6 multicast
                                           reserved address FF0X:0:0:0:0:0:0:0
                                           on a port as management frames */
    bcmPortControlManagementPacketIp4Reserved = 173, /* Classify Packets with IPV4 multicast
                                           reserved address 224.0.0.x on a port
                                           as management frames */
    bcmPortControlManagementPacketLinkConstrained = 174, /* Classify Packets with
                                           link-constrained multicast range
                                           DMAC=01-80-C2-00-00-0x as management
                                           frames */
    bcmPortControlManagementPacketGarp = 175, /* Classify Packets with DA range
                                           0x0180c2000020 through 0x0180c200002f
                                           as management frames */
    bcmPortControlManagementPacketCos = 176, /* COS assignment for Management Packets */
    bcmPortControlManagementPacketColor = 177, /* Drop Precedence for Management
                                           Packets */
    bcmPortControlTaggedL3PacketPktPri = 178, /* Set packet Priority source for tagged
                                           L3 packets (enumerated value within
                                           bcm_port_cos_remap_t) */
    bcmPortControlTaggedMPLSPacketPktPri = 179, /* Set packet Priority source for tagged
                                           MPLS packets (enumerated value within
                                           bcm_port_cos_remap_t) */
    bcmPortControlTaggedMPLSL3PacketPktPri = 180, /* Set packet Priority source for tagged
                                           MPLS L3 packets (enumerated value
                                           within bcm_port_cos_remap_t) */
    bcmPortControlEEEEventCountSymmetric = 181, /* Operate EEE counter in symmetric
                                           mode, where TX and RX counters
                                           increment together */
    bcmPortControlEEELinkActiveDuration = 182, /* Time(in microsecs) to wait before EEE
                                           transitions to Active state after
                                           link status becomes active */
    bcmPortControlEsmEligibility = 183, /* Indicates if ESM search is enabled
                                           for packets coming in on this port */
    bcmPortControlTrillDesignatedVlan = 184, /* VLAN which carries Trill packets
                                           between RBridges */
    bcmPortControlMplsFRREnable = 185,  /* Enable or disable FRR on MPLS
                                           packets. */
    bcmPortControlMplsContextSpecificLabelEnable = 186, /* Enable or disable Upstream assignment
                                           label on MPLS packets. */
    bcmPortControlVxlanEnable = 187,    /* Set per port enable for VXLAN
                                           (Value=TRUE/FALSE) */
    bcmPortControlVxlanTunnelbasedVnId = 188, /* Set per port VNID lookup key
                                           (Value=TRUE/FALSE) */
    bcmPortControlVxlanDefaultTunnelEnable = 189, /* Set per port dafult-SVP for Tunnel
                                           lookup failure(Value=TRUE/FALSE) */
    bcmPortControlFieldEgressClassSelect = 190, /* Select Class Id to pass to EFP */
    bcmPortControlPonMpcpDiscoveryRate = 191, /* EPON Port MPCP Discovery rate in
                                           miliseconds. */
    bcmPortControlPonMpcpDiscoveryGrantLength = 192, /* EPON Port MPCP Discovery Grant length
                                           in bytes. */
    bcmPortControlPonMpcpOneRegistrationPerGrant = 193, /* Allow only one registration per grant */
    bcmPortControlPonOamHeartbeatRate = 194, /* EPON port OAM heartbeat rate in
                                           seconds */
    bcmPortControlPonMaxLinks = 195,    /* EPON port maximum number of links */
    bcmPortControlPonRamanMitigationMode = 196, /* EPON port Raman Mitigation mode */
    bcmPortControlPonLoopTimeMinPropDelay = 197, /* EPON Loop Timing Minimum Propagation
                                           Delay */
    bcmPortControlPonLoopTimeMaxPropDelay = 198, /* EPON Loop Timing Maximum Propagation
                                           Delay */
    bcmPortControlPonLoopTimeOnuDelay = 199, /* EPON Loop Timing Onu Delay */
    bcmPortControlPonLoopTimeNullGrantSize = 200, /* EPON Loop Timing Null Grant size */
    bcmPortControlPonLoopTimeOltDelay = 201, /* EPON Loop Timing OLT delay */
    bcmPortControlPonSfecAllowedBitErrors = 202, /* EPON SFEC Allowed bit errors */
    bcmPortControlPonTfecAllowedBitErrors = 203, /* EPON TFEC Allowed bit errors */
    bcmPortControlPonFecRxBypass = 204, /* Rx FEC Bypass mode */
    bcmPortControlPonSyncTime = 205,    /* EPON Sync Time in 16ns(TQ) unit */
    bcmPortControlPonEncryptionMode = 206, /* EPON encryption mode (0 : None, 1 :
                                           AE, 2 : CTC, 3 : Zero-overhead AE). */
    bcmPortControlPonEncryptionDirection = 207, /* EPON encryption direction (1 :
                                           downstream only, 2 : bi-direction). */
    bcmPortControlPonOamKeyExchangeMsgBypass = 208, /* Bypass processing Key Exchange OAM
                                           from Pon port */
    bcmPortControlTunnelIdIngressSource = 209, /* Move outermost VLAN tag to Tunnel ID
                                           on ingress */
    bcmPortControlTunnelIdEgressAdd = 210, /* Add outermost VLAN tag as Tunnel ID
                                           on egress */
    bcmPortControlPonOamHeartbeatTimeout = 211, /* EPON Oam heartbeat timeout in sec
                                           unit */
    bcmPortControlPonMpcpReportTimeout = 212, /* EPON report timeout in ms unit */
    bcmPortControlPon1GDiscoveryWindow = 213, /* Enable 1G discovery window */
    bcmPortControlPon10GDiscoveryWindow = 214, /* Enable 10G discovery window */
    bcmPortControlExtendedPortVid = 215, /* Extended port VID (value range
                                           0-0xfff) */
    bcmPortControlNonEtagDrop = 216,    /* Set Ingress drop control, if ETAG is
                                           not present (value = TRUE/FALSE) */
    bcmPortControlEtagDrop = 217,       /* Set Ingress drop control, if ETAG is
                                           present (value = TRUE/FALSE) */
    bcmPortControlNonEtagAdd = 218,     /* Set ADD tag action, when ETAG is not
                                           present (value = TRUE/FALSE) */
    bcmPortControlEtagIngressReplace = 219, /* Set REPLACE Ingress tag action, when
                                           ETAG is present (value = TRUE/FALSE) */
    bcmPortControlEtagEgressReplace = 220, /* Set REPLACE Egress tag action, when
                                           ETAG is present (value = TRUE/FALSE) */
    bcmPortControlEtagIngressDelete = 221, /* Set DELETE Ingress tag action, when
                                           ETAG is present (value = TRUE/FALSE) */
    bcmPortControlEtagEgressDelete = 222, /* Set DELETE Egress tag action, when
                                           ETAG is present (value = TRUE/FALSE) */
    bcmPortControlExtenderRpfCheck = 223, /* Set incoming packet ETAG.SVID RPF
                                           check control (value = TRUE/FALSE) */
    bcmPortControlExtenderSourceKnockout = 224, /* Set source extended port VID pruning
                                           control (value = TRUE/FALSE) */
    bcmPortControlExtenderForwardPort = 225, /* Set destination port for upstream
                                           Extender forwarding (value = local
                                           port number) */
    bcmPortControlExtenderType = 226,   /* Set Extender port type (enumerated
                                           value within
                                           bcm_port_extender_type_t) */
    bcmPortControlExtenderNameSpace = 227, /* Set namespace value (value range
                                           0-0xfff) */
    bcmPortControlEtagPcpDeSelect = 228, /* Set source of PCP and DE fields when
                                           adding an ETAG to an ingress packet
                                           (value =
                                           bcm_extender_pcp_de_select_t) */
    bcmPortControlEtagPcp = 229,        /* Set default PCP field when adding an
                                           ETAG to an ingress packet (value =
                                           0-7) */
    bcmPortControlEtagDe = 230,         /* Set default DE field when adding an
                                           ETAG to an ingress packet (value =
                                           0-1) */
    bcmPortControlUseEtagPri = 231,     /* Use ETAG PCP and DE fields to derive
                                           internal priority and color */
    bcmPortControlProhibitedDot1p = 232, /* Set prohibited 802.1p values */
    bcmPortControlEgressFilterDisable = 233, /* Enable/ disable Egress Filtering */
    bcmPortControlLLFCReceive = 234,    /* Link Level Flow Control packet
                                           receive enable */
    bcmPortControlLLFCTransmit = 235,   /* Link Level Flow Control packet
                                           transmit enable */
    bcmPortControlSAFCReceive = 236,    /* Service Aware Flow Control packet
                                           receive enable */
    bcmPortControlSAFCTransmit = 237,   /* Service Aware Flow Control packet
                                           transmit enable */
    bcmPortControlMacSwap = 238,        /* Enable or Disable MAC swapping on the
                                           port for incoming packet */
    bcmPortControlOverlayRecycle = 241, /* Set port as recycle port for overlay
                                           packets second pass */
    bcmPortControlEvbType = 242,        /* Set EVB port type (enumerated value
                                           within bcm_port_evb_type_t) */
    bcmPortControlOamLookupWithDvp = 243, /* Use DVP as one of the keys for OAM
                                           lookup */
    bcmPortControlSubportTagEnable = 244, /* Enable/disable 'third VLAN tag'
                                           (subport tag) channelization over
                                           Ethernet */
    bcmPortControlVMac = 247,           /* Enable/Disable VMAC function. */
    bcmPortControlEgressModifyDscp = 248, /* Enable DSCP/EXP marking. In simple
                                           mode enable on port. In advanced mode
                                           enable on inlif profile. */
    bcmPortControlFcoeRouteEnable = 249, /* Enables FCOE routing */
    bcmPortControlFcoeVftEnable = 250,  /* Controls VFT header expected on port */
    bcmPortControlFcoeZoneCheckEnable = 251, /* Enables zone check */
    bcmPortControlFcoeSourceBindCheckEnable = 252, /* Enables source bind check */
    bcmPortControlFcoeFpmaPrefixCheckEnable = 253, /* Enables FPMA prefix check */
    bcmPortControlFcoeDoNotLearn = 254, /* Disables learning for packets with
                                           FCOE EtherType */
    bcmPortControlFcoeNetworkPort = 255, /* Sets whether the port is network
                                           (uplink) or not (downfacing) */
    bcmPortControlFcoeFabricSel = 256,  /* Selects the source of fabric Id and
                                           priority */
    bcmPortControlFcoeFabricId = 257,   /* Sets the port default 12-bit fabric
                                           Id */
    bcmPortControlFcoeFabricPri = 258,  /* Sets the port default fabric priority */
    bcmPortControlFcoeFcCrcAction = 259, /* Sets the FC CRC action to take */
    bcmPortControlFcoeFcCrcRecompute = 260, /* Sets the re-compute CRC options */
    bcmPortControlFcoeFcEofIgnore = 261, /* Sets the control to Ignore EOF */
    bcmPortControlMcastGroupRemap = 262, /* Enable remapping of multicast group
                                           index. */
    bcmPortControlLocalSwitching = 263, /* to enable Local route switching on a
                                           port */
    bcmPortControlFecErrorDetectEnable = 264, /* Enables marking of failed error
                                           correction on the cells */
    bcmPortControlLowLatencyLLFCEnable = 265, /* Enables an improvment in latency on
                                           the expense of bandwidth */
    bcmPortControlErspanEnable = 266,   /* Enable/Disable ERSPAN mirroring */
    bcmPortControlPFCXOffTime = 267,    /* Priority Flow Control xoff time */
    bcmPortControlOamDefaultProfile = 268, /* OAM default profile */
    bcmPortControlSubportPktTagDrop = 269, /* Set Ingress drop control, if LLTAG is
                                           present  (value = TRUE/FALSE) */
    bcmPortControlNoSubportPktTagDrop = 270, /* Set Ingress drop control, if LLTAG is
                                           not present  (value = TRUE/FALSE) */
    bcmPortControlUseSubportPktTagPri = 271, /* Use LLTAG PCP and DE fields to derive
                                           internal priority and color */
    bcmPortControlVlanVpGroupEgress = 272, /* VP group for egress vlan membership */
    bcmPortControlVlanVpGroupIngress = 273, /* VP group for ingress vlan membership */
    bcmPortControlEEEMode = 274,        /* Select Energy Efficient Ethernet mode */
    bcmPortControlProbeSerdesReset = 275, /* Request to reset SerDes on next probe
                                           to the SerDes */
    bcmPortControlFcoeVsanSel = bcmPortControlFcoeFabricSel, /* Selects the source of VSAN ID and
                                           priority */
    bcmPortControlFcoeVsanId = bcmPortControlFcoeFabricId, /* Sets the port default 12-bit VSAN ID */
    bcmPortControlFcoeVsanPri = bcmPortControlFcoeFabricPri, /* Sets the port default VSAN priority */
    bcmPortControlIPMC4Ucast = 276,     /* Enable IPMCv4 Unicast MACDA feature
                                           on port. */
    bcmPortControlIPMC6Ucast = 277,     /* Enable IPMCv6 Unicast MACDA feature
                                           on port. */
    bcmPortControlWredDropCountUpdateEnableQueueMask = 278, /* Set the queue mask to enable the WRED
                                           dropped counter increase. */
    bcmPortControlWredDropCountUpdateEnableColorMask = 279, /* Set the color mask to enable the WRED
                                           dropped counter increase. */
    bcmPortControlReserved280 = 280,    /* Reserved. */
    bcmPortControlReserved281 = 281,    /* Reserved. */
    bcmPortControlRxFastLOS = 282,      /* Enable/disable accelerated linkscan
                                           capability on port */
    bcmPortControlFcoeTranslateKeyFirst = 283, /* The key to use for the first VLAN
                                           translate search if the EtherType is
                                           FCOE */
    bcmPortControlFcoeTranslateKeySecond = 284, /* The key to use for the second VLAN
                                           translate search if the EtherType is
                                           FCOE */
    bcmPortControlMmuDrain = 285,       /* Draining the cells of the port */
    bcmPortControlMmuTrafficEnable = 286, /* Enable/disable enqueing the packets
                                           to the port */
    bcmPortControlStatCollectionEnable = 287, /* Enable/disable the gathering of MIB
                                           statistics on the port */
    bcmPortControlLlfcCellsCongestionIndEnable = 288, /* EnableDisable extracting cells
                                           congestion indication from LLFC cells */
    bcmPortControlTrunkLoadBalancingRandomizer = 289, /* Assigns the Load Balancing
                                           Randomizer-value for LAGs(value =
                                           integer between 0 and 15) */
    bcmPortControlECMPLevel1LoadBalancingRandomizer = 290, /* Assigns the Load Balancing
                                           Randomizer-value for ECMP (at Level 1
                                           in Two level ECMP)(value = integer
                                           between 0 and 15) */
    bcmPortControlECMPLevel2LoadBalancingRandomizer = 291, /* Assigns the Load Balancing
                                           Randomizer-value for ECMP at Level 2
                                           in Two Level ECMP(value = integer
                                           between 0 and 15) */
    bcmPortControlMplsExplicitNullEnable = 292, /* Enable or disable Explicit NULL
                                           termination database */
    bcmPortControlExtenderEnable = 293, /* Enable/disable port extender
                                           capability on the port */
    bcmPortControlSubportTagSize = 294, /* Set the CoE egress subport tag size.
                                           When set inserts a wide subport tag,
                                           else inserts a regular subport tag */
    bcmPortControlSystemPortOverride = 295, /* When set, port group id used for
                                           egress VLAN translation is replaced
                                           with destination global logical port */
    bcmPortControlOamDefaultProfileEgress = 296, /* OAM default profile for Egress */
    bcmPortControlPadToSize = 297,      /* Min packet size in bytes, packets
                                           smaller than this value are padded
                                           with '0' */
    bcmPortControlVxlanVpnAssignmentCriteria = 298, /* VxLAN vpn assignment criteria */
    bcmPortControlVxlanTerminationMatchCriteria = 299, /* VxLAN termination match criteria */
    bcmPortControlVxlanGportAssignmentCriteria = 300, /* VxLAN virtual port assignment
                                           criteria */
    bcmPortControlOOBFCTxReportIngEnable = 301, /* OOB FC Tx Ingress Congestion
                                           Notification Enable */
    bcmPortControlOOBFCTxReportEgrEnable = 302, /* OOB FC Tx Egress Congestion
                                           Notification Enable */
    bcmPortControlMplsEncapsulateExtendedLabel = 303, /* Encapsulate additional MPLS tunnel on
                                           a given outlif */
    bcmPortControlSampleIngressDest = 304, /* Set Ingress sFlow sample destination */
    bcmPortControlSampleFlexDest = 305, /* Set Flex sFlow sample destination */
    bcmPortControlSampleFlexRate = 306, /* Set Flex sFlow sampling rate */
    bcmPortControlExternalBufferEnable = 307, /* Enable/disable the MMU External
                                           Buffer on the port */
    bcmPortControlTrustMimlPri = 308,   /* If set, use packet priority from MiML
                                           header for internal priority mapping */
    bcmPortControlTrustCustomHeaderPri = 309, /* If set, determine internal Priority
                                           based on custom header */
    bcmPortControlCustomHeaderEncapEnable = 310, /* If set, enable the capability of
                                           custom header encapsulation */
    bcmPortControlLinkPhyEnable = 311,  /* Enable LinkPhy on a Port */
    bcmPortControlOlpEnable = 312,      /* Enable/disable OLP
                                           encapsulation/Decapsulation on a
                                           given port */
    bcmPortControlPortMacOui = 313,     /* OUI field of PORT specific MAC. Used
                                           for OAM and SAT */
    bcmPortControlPortMacNonOui = 314,  /* NON OUI field of PORT specific MAC.
                                           Used for OAM and SAT */
    bcmPortControlSatEnable = 315,      /* Enable/Disable SAT feature. */
    bcmPortControl1588P2PDelay = 316,   /* Update path delay in 1588 p2p
                                           mechanism. */
    bcmPortControlSampleIngressEnable = 317, /* Enable/Disable ingress port-based
                                           sflow. */
    bcmPortControlSampleEgressEnable = 318, /* Enable/Disable egress port-based
                                           sflow. */
    bcmPortControlSampleIngressRate = 319, /* Set Ingress sFlow sample rate. */
    bcmPortControlSampleEgressRate = 320, /* Set Egress sFlow sample rate. */
    bcmPortControlMplsSpeculativeParse = 321, /* Enable the speculative parsing of
                                           headers over MPLS. */
    bcmPortControlFieldHiGigClassSelect = 322, /* Control classid selection into HG
                                           header. */
    bcmPortControlDvpGroupId = 323,     /* Configure DVP group value which will
                                           be used to compare against SVP for
                                           pruning decision */
    bcmPortControlDvpGroupKnockoutEnable = 324, /* Enable/Disable DVP group based
                                           pruning. Supported values are
                                           0-Disable, 1-Enable */
    bcmPortControlUntaggedVlanMember = 325, /* Set the VLAN-Membership state per
                                           port for packets classified as
                                           untagged */
    bcmPortControlMPLSEncapsulateAdditionalLabel = 326, /* Set additional label enable on
                                           outgoing LIF */
    bcmPortControlPreserveDscpIngress = 327, /* Preserve DSCP after routing, ingress
                                           logical port configuration */
    bcmPortControlPreserveDscpEgress = 328, /* Preserve DSCP after routing, egress
                                           logical port configuration */
    bcmPortControlPWETerminationPortModeEnable = 329, /* Enable PWE termination port mode on a
                                           port, if enabled, PWE label
                                           termination lookup key will be
                                           <port+label> */
    bcmPortControlOuterPolicerRemark = 330, /* set/get in-lif profile or map DP
                                           profile mapping in-lif profile to
                                           remark outer PCP-DEI. */
    bcmPortControlInnerPolicerRemark = 331, /* set/get in-lif profile or map DP
                                           profile mapping in-lif profile to
                                           remark inner PCP-DEI. */
    bcmPortControlAllowTtlInheritance = 332, /* Enabling Ingress inheritance of TTL
                                           per Outgoing Logical interface
                                           (consume Out-LIF profile) */
    bcmPortControlAllowQosInheritance = 333, /* Enabling Ingress inheritance DSCP-EXP
                                           per Outgoing Logical interface
                                           (consume Out-LIF profile) */
    bcmPortControlPacketTimeStampInsertRx = 334, /* Insert RX packet timestamp for
                                           packets arriving on the port. */
    bcmPortControlPacketTimeStampInsertTx = 335, /* Insert TX packet timestamp for
                                           packets exiting the port. */
    bcmPortControlPacketTimeStampSrcPortInsertCancel = 336, /* Cancel/override both RX and TX time
                                           stamp insertions for the packets
                                           arriving on the port. */
    bcmPortControlPacketTimeStampDestPortInsertCancel = 337, /* Cancel/override both RX and TX time
                                           stamp insertions for the packets
                                           exiting the port. */
    bcmPortControlPacketTimeStampRxId = 338, /* Set Source identifier of RX (Ingress)
                                           Packet Time Stamp record header. */
    bcmPortControlPacketTimeStampTxId = 339, /* Set Source identifier of TX (Egress)
                                           Packet Time Stamp record header. */
    bcmPortControlPacketTimeStampByteMeterAdjust = 340, /* Set if need to adjust meter for
                                           packet timestamp. */
    bcmPortControlDoNotCheckVlanEgress = 341, /* Enable/Disable egress VLAN checks for
                                           VP port. */
    bcmPortControlE2EFCTransmitEnable = 342, /* Enable E2EFC message transmit. */
    bcmPortControlE2EFCReceiveEnable = 343, /* Enable E2EFC message receive. */
    bcmPortControlE2EFCCopyToCpuEnable = 344, /* Enable E2EFC message copy-to-CPU. */
    bcmPortControlE2EFCDestMacOui = 345, /* E2EFC message destination MAC upper 3
                                           bytes, only for E2EFC TX. */
    bcmPortControlE2EFCDestMacNonOui = 346, /* E2EFC message destination MAC lower 3
                                           bytes, only for E2EFC TX. */
    bcmPortControlE2EFCSrcMacOui = 347, /* E2EFC message source MAC upper 3
                                           bytes, only for E2EFC TX. */
    bcmPortControlE2EFCSrcMacNonOui = 348, /* E2EFC message source MAC lower 3
                                           bytes, only for E2EFC TX. */
    bcmPortControlE2EFCEtherType = 349, /* E2EFC message EtherType/length, only
                                           for E2EFC TX. */
    bcmPortControlE2EFCOpcode = 350,    /* E2EFC message opcode, only for E2EFC
                                           TX. */
    bcmPortControlForceBridgeForwarding = 351, /* Set port to Force Bridge Forwarding. */
    bcmPortControlIlknRxPathEnable = 352, /* Enable/Disable rx path on Ilkn port.
                                           Only use with no linkscan */
    bcmPortControlEgressAclTwoPassEditing = 353, /* Enable/Disable egress ACL two pass
                                           editing on a recycle port */
    bcmPortControlLLFCAfterFecEnable = 354, /* Enable/Disable FEC on LLFC and
                                           congestion indication */
    bcmPortControlResetSerdes = 355,    /* Apply reset and relesase reset to
                                           SerDes core. Hard resets PCS and PMD */
    bcmPortControlPONEnable = 356,      /*  Enable or disable pon capability on
                                           the port */
    bcmPortControlLogicalInterfaceSameFilter = 357, /* Set or unset Same-interface filtering
                                           according to logical-interface ID
                                           while incoming and outgoing port-ID
                                           will be ignored. */
    bcmPortControlIPv6TerminationCascadedModeEnable = 358, /* Enable per port cascading lookup of
                                           IPv6 tunnel termination */
    bcmPortControlMimUcastTermEnable = 359, /* Enable/Disable MiM UC Termination
                                           Mode */
    bcmPortControlMimMcastTermEnable = 360, /* Enable/Disable MiM MC and BC
                                           Termination Mode */
    bcmPortControlTXPISDMOverride = 361, /* Override TXPI SDM value */
    bcmPortControlTXPISDMStatus = 362,  /* The final result value of TXPI SDM */
    bcmPortControlBicastPort = 363,     /* Sets the Bicast mapping for port */
    bcmPortControlObmClassifierPriority = 364, /* Indicates the default OBM Traffic
                                           class for port */
    bcmPortControlObmClassifierEnableDscp = 365, /* Enable OBM DSCP Classifier on port */
    bcmPortControlObmClassifierEnableMpls = 366, /* Enable OBM MPLS Classifier on port */
    bcmPortControlObmClassifierEnableEtag = 367, /* Enable OBM ETAG Classifier on port */
    bcmPortControlObmClassifierEtagEthertype = 368, /* Configure EtherType for OBM ETAG
                                           Classifier */
    bcmPortControlObmClassifierVntagEthertype = 369, /* Configure EtherType for OBM VNTAG
                                           Classifier */
    bcmPortControlControlCellsFecBypassEnable = 370, /* Determine if FEC is bypassed by two
                                           control cells types: credit cells and
                                           flow status cells.
                                                                          
                                           Enable FEC bypass means to NOT do FEC
                                           on these controls cells.Disable FEC
                                           bypass means to do FEC on these
                                           controls cells.
                                                                           This
                                           configuration is relevant for RS FEC
                                           types only. */
    bcmPortControlPrbsRxInvertData = 371, /* Configure inversion of Rx data */
    bcmPortControlFcoeTranslateKeyFirstEgress = 372, /* The key to use for the first Egress
                                           VLAN translate search if the
                                           EtherType is FCOE */
    bcmPortControlFcoeTranslateKeySecondEgress = 373, /* The key to use for the second Egress
                                           VLAN translate search if the
                                           EtherType is FCOE */
    bcmPortControlIntEnable = 374,      /* Enable INT at port level */
    bcmPortControlIntPortMetaData = 375, /* Enable per port Metadata inserstion */
    bcmPortControlIntTurnaround = 376,  /* Program a egress port as INT
                                           turnaround port */
    bcmPortControlQosRemarkResolution = 377, /* Program a egress port QoS
                                           resolution(enumerated value within
                                           bcm_port_qos_resolution_type_t) */
    bcmPortControlObmClassifierInternal = 378, /* Used to enable/disable internal OBM
                                           classification. By default external
                                           OBM classification is enabled. Once
                                           internal OBM classification is
                                           disabled, external OBM classification
                                           becomes active. */
    bcmPortControlWirelessCosMapSelect = 379, /* Used to select the wireless cos map
                                           for the traffic for a port. */
    bcmPortControlHiGigCosSelect = 380, /* Used to select the HiGig cos for a
                                           port. */
    bcmPortControlTpidSrc = 381,        /* Used to configure TPID source on VP */
    bcmPortControlInternalLoopback = 382, /* Enable/Disable the port's internal
                                           loopback mode */
    bcmPortConrolEgressVlanPortArpPointed = 383, /* Used to configure the index to vlan
                                           port per ARP. */
    bcmPortControlVrrpScalingPort = 384, /* Used to enable/disable VRRP scaling
                                           per port. */
    bcmPortControlObmInterruptTypeDropPreempt = 385, /* Used to enable obm interrupt for
                                           preempt drop. */
    bcmPortControlObmInterruptTypeDropExpress = 386, /* Used to enable obm interrupt for
                                           express drop. */
    bcmPortControlObmInterruptTypeOverflowPreempt = 387, /* Used to enable interrupt for preempt
                                           overflow */
    bcmPortControlObmInterruptTypeOverflowExpress = 388, /* Used to enable interrupt for express
                                           overflow. */
    bcmPortControlForwardNetworkGroup = 389, /* Enable/Disable Forward network group
                                           functionality. */
    bcmPortControlIngressQosModelRemark = 390, /* Used to configure Ingress remark QOS
                                           model */
    bcmPortControlIngressQosModelPhb = 391, /* Used to configure Ingress PHB QOS
                                           model */
    bcmPortControlIngressQosModelTtl = 392, /* Used to configure Ingress TTL QOS
                                           model */
    bcmPortControlMetadata = 393,       /* Used to configure egress port
                                           metadata */
    bcmPortControlFirstHeaderSize = 394, /* Used to configure the size of the
                                           first header to skip. */
    bcmPortControlDistributeLearnMessageMode = 395, /* Used to configure distribute learn
                                           message mode. */
    bcmPortControlDiscardEgress = 396,  /* enable/disable to discard all traffic
                                           on that outgoing port. */
    bcmPortControlIngressEnable = 397,  /* enable/disable single ingress port. */
    bcmPortControlRuntThreshold = 398,  /* configure rxrunt threshold to mac. */
    bcmPortControlReflectorL2 = 399,    /* configure port to internal reflector
                                           without system header. */
    bcmPortControlRxPriority = 400,     /* configure port Rx priority. */
    bcmPortControlIP4TunnelTermEnable = 401, /* Enable or disable IPv4 tunnel
                                           termination. */
    bcmPortControlIP6TunnelTermEnable = 402, /* Enable or disable IPv6 tunnel
                                           termination. */
    bcmPortControlQoSRemarkEgressEnable = 403, /* Enable or disable egress port-based
                                           QoS remarking. */
    bcmPortControlBridgeEgress = 404,   /* Enable or disable egress port-based
                                           bridging. */
    bcmPortControlLatencyEcnEnable = 405, /* Enable or disable latency based ECN
                                           on egress port. */
    bcmPortControlModEnable = 406,      /* Enable or disable port-based MOD. */
    bcmPortControlModProfileId = 407,   /* Designate a MOD profile id to a port. */
    bcmPortControlLinkFaultLocalClear = 408, /* Clear the sticky local_fault_status
                                           bit. */
    bcmPortControlLinkFaultRemoteClear = 409, /* Clear the sticky remote_fault_status
                                           bit. */
    bcmPortControlIngressSampleProfile = 410, /* Ingress sample profile id. */
    bcmPortControlPFCFromLLFCEnable = 411, /* Enable/disable LLFC RMC threshold to
                                           generate PFC on the specified port. */
    bcmPortControlFirstHeaderSizeAfterInjected = 412, /* Used to configure the size of the
                                           first header to skip after injected. */
    bcmPortControlFlexeDoubleNbTdmSlotEnable = 413, /* Control the number of slots for FlexE
                                           NB TDM calendar for 50G NRZ port with
                                           FEC enabled. */
    bcmPortControlRecycleApp = 414,     /* Configure Port Recycle application. */
    bcmPortControlSystemPortInjectedMap = 415, /* Enable mapping system port to
                                           local-port for injected traffic
                                           scenarios. */
    bcmPortControlPacketTimeStampIngressInsertTx = 416, /* Enable to insert TX timestamp on
                                           transmitted packets when they are
                                           arriving on the ingress port. */
    bcmPortControlPacketTimeStampIngressInsertTxCancel = 417, /* Enable to skip TX timestamp insertion
                                           on transmitted packets when they are
                                           arriving on the ingress port. */
    bcmPortControlEgressVlanPriSrc = 418, /* Select the source of egress VLAN
                                           priority. */
    bcmPortControlTxStall = 419,        /* Stall dequeuing packets. */
    bcmPortControlTxByteState = 420,    /* Enable TX byte state to update TX
                                           byte count that would be inserted
                                           into IFA metadata. */
    bcmPortControlPacketProcssingPort = 421, /* Set the packet processing port
                                           number. */
    bcmPortControlIfaEnable = 422,      /* Enable IFA. */
    bcmPortControlIoamEnable = 423,     /* Enable IOAM. */
    bcmPortControlPFCStatus = 424,      /* Get port's current received PFCs
                                           status. */
    bcmPortControlRoeLinkdownTransmit = 425, /* For the given ROE port tx traffic is
                                           enabled even if link is downt. */
    bcmPortControlRoeLinkdownReceive = 426, /* For the given ROE port rx traffic is
                                           enabled even if link is down. */
    bcmPortControlEgressPktPriCfiCopyIngressEnable = 427, /* Enable or disable egress packet
                                           priority and CFI copy on ingress
                                           port. */
    bcmPortControlEgressTosCopyIngressEnable = 428, /* Enable or disable egress TOS copy on
                                           ingress port. */
    bcmPortControlEgressL2OpaqueTag = 429 /* Set egress L2 opaque tag. */
} bcm_port_control_t;

/* EEE modes. */
typedef enum bcm_port_eee_mode_e {
    bcmPortEEENativeMode = 0,       /* EEE native mode */
    bcmPortEEEBurstAndBatchMode = 1 /* EEE burst and batch mode */
} bcm_port_eee_mode_t;

/* Types of Tunnel ID. */
typedef enum bcm_port_tunnel_id_e {
    BCM_PORT_TUNNEL_ID_NONE = 0,    /* No Tunnel ID */
    BCM_PORT_TUNNEL_ID_VLAN = 1     /* VLAN tag as Tunnel ID */
} bcm_port_tunnel_id_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set various features at the port level. */
extern int bcm_port_control_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_t type, 
    int value);

/* Get or set various features at the port level. */
extern int bcm_port_control_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_t type, 
    int *value);

/* 
 * Get the list of ancillary/flex ports belonging to the same port block
 * as the
 * controlling port.
 */
extern int bcm_port_subsidiary_ports_get(
    int unit, 
    bcm_port_t port, 
    bcm_pbmp_t *pbmp);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initializes the bcm_port_info_t structure. */
extern void bcm_port_info_t_init(
    bcm_port_info_t *info);

/* Initialize a Port Ability structure. */
extern void bcm_port_ability_t_init(
    bcm_port_ability_t *ability);

/* Initializes the bcm_phy_config_t structure. */
extern void bcm_phy_config_t_init(
    bcm_phy_config_t *config);

/* Initializes the bcm_port_interface_config_t structure. */
extern void bcm_port_interface_config_t_init(
    bcm_port_interface_config_t *config);

/* Initialize a Port Configuration structure. */
extern void bcm_port_config_t_init(
    bcm_port_config_t *pconfig);

/* Initialize a Port Encapsulation Configuration structure. */
extern void bcm_port_encap_config_t_init(
    bcm_port_encap_config_t *encap_config);

#define BCM_PORT_CONGESTION_CONFIG_E2ECC    0x00000001 
#define BCM_PORT_CONGESTION_CONFIG_HCFC     0x00000002 
#define BCM_PORT_CONGESTION_CONFIG_DESTMOD_FLOW_CONTROL 0x00000004 
#define BCM_PORT_CONGESTION_CONFIG_MAX_SPLIT4 0x00000008 
#define BCM_PORT_CONGESTION_CONFIG_TX       0x00000010 
#define BCM_PORT_CONGESTION_CONFIG_RX       0x00000020 
#define BCM_PORT_CONGESTION_CONFIG_SAFC     0x00000040 
#define BCM_PORT_CONGESTION_CONFIG_QCN      0x00000080 
#define BCM_PORT_CONGESTION_CONFIG_DEST_PORT_VALID 0x00000100 
#define BCM_PORT_CONGESTION_CONFIG_DESTMODPORT_FLOW_CONTROL 0x00000200 
#define BCM_PORT_CONGESTION_CONFIG_E2ECC_COE_STRICT_MODE 0x00000400 
#define BCM_PORT_CONGESTION_CONFIG_E2ECC_COE_FLEX_MODE 0x00000800 
#define BCM_PORT_CONGESTION_CONFIG_VLAN_PAUSE_COE 0x00001000 
#define BCM_PORT_CONGESTION_CONFIG_COPY_TO_CPU 0x00002000 

/* bcm_port_congestion_config_s */
typedef struct bcm_port_congestion_config_s {
    uint32 flags;               /* congestion mode. */
    uint8 port_bits;            /* HiGig format for port status */
    int packets_per_sec;        /* frequency of sending congestion messages */
    bcm_gport_t src_port;       /* gport of type mmodid:port, wildcard on
                                   receive */
    uint16 multicast_id;        /* Multicast Identifier */
    bcm_cos_t traffic_class;    /* Traffic Class */
    bcm_color_t color;          /* Color */
    bcm_vlan_t vlan;            /* Vlan */
    uint8 pri;                  /* Packet priority */
    uint8 cfi;                  /* CFI */
    bcm_mac_t src_mac;          /* Source MAC address */
    bcm_mac_t dest_mac;         /* Destination MAC address */
    uint16 ethertype;           /* Ethernet Type */
    uint16 opcode;              /* Opcode */
    uint8 version;              /* QCN PDU version */
    uint8 reserved_v;           /* QCN PDU reserved_v */
    bcm_gport_t dst_port;       /* gport of type mmodid:port */
} bcm_port_congestion_config_t;

/* Initialize a Port Congestion Configuration structure. */
extern void bcm_port_congestion_config_t_init(
    bcm_port_congestion_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure/retrieve flow control protocol mode. */
extern int bcm_port_congestion_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_congestion_config_t *config);

/* Configure/retrieve flow control protocol mode. */
extern int bcm_port_congestion_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_congestion_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_phy_control_t */
typedef _shr_port_phy_control_t bcm_port_phy_control_t;

/* 
 * PHY interrupts sources for use with PORT_PHY_CONTROL_INTR_MASK and
 * PORT_PHY_CONTROL_INTR_STATUS.
 */
#define BCM_PORT_PHY_INTR_TIMESYNC_FRAMESYNC _SHR_PORT_PHY_INTR_TIMESYNC_FRAMESYNC 
#define BCM_PORT_PHY_INTR_TIMESYNC_TIMESTAMP _SHR_PORT_PHY_INTR_TIMESYNC_TIMESTAMP 
#define BCM_PORT_PHY_INTR_LINK_EVENT        _SHR_PORT_PHY_INTR_LINK_EVENT 
#define BCM_PORT_PHY_INTR_AUTONEG_EVENT     _SHR_PORT_PHY_INTR_AUTONEG_EVENT 
#define BCM_PORT_PHY_INTR_PLL_EVENT         _SHR_PORT_PHY_INTR_PLL_EVENT 
#define BCM_PORT_PHY_INTR_UC_EVENT          _SHR_PORT_PHY_INTR_UC_EVENT 
#define BCM_PORT_PHY_INTR_EMON_EVENT        _SHR_PORT_PHY_INTR_EMON_EVENT 
#define BCM_PORT_PHY_INTR_AUX_EVENT         _SHR_PORT_PHY_INTR_AUX_EVENT 

/* Controls for changing PHY settings. */
#define BCM_PORT_PHY_CONTROL_WAN            _SHR_PORT_PHY_CONTROL_WAN  
#define BCM_PORT_PHY_CONTROL_PREEMPHASIS    _SHR_PORT_PHY_CONTROL_PREEMPHASIS 
#define BCM_PORT_PHY_CONTROL_PREEMPHASIS_LANE0 _SHR_PORT_PHY_CONTROL_PREEMPHASIS_LANE0 
#define BCM_PORT_PHY_CONTROL_PREEMPHASIS_LANE1 _SHR_PORT_PHY_CONTROL_PREEMPHASIS_LANE1 
#define BCM_PORT_PHY_CONTROL_PREEMPHASIS_LANE2 _SHR_PORT_PHY_CONTROL_PREEMPHASIS_LANE2 
#define BCM_PORT_PHY_CONTROL_PREEMPHASIS_LANE3 _SHR_PORT_PHY_CONTROL_PREEMPHASIS_LANE3 
#define BCM_PORT_PHY_CONTROL_DRIVER_CURRENT _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT 
#define BCM_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE0 _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE0 
#define BCM_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE1 _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE1 
#define BCM_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE2 _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE2 
#define BCM_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE3 _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE3 
#define BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT 
#define BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0 _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0 
#define BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1 _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1 
#define BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2 _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2 
#define BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3 _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3 
#define BCM_PORT_PHY_CONTROL_EQUALIZER_BOOST _SHR_PORT_PHY_CONTROL_EQUALIZER_BOOST 
#define BCM_PORT_PHY_CONTROL_INTERFACE      _SHR_PORT_PHY_CONTROL_INTERFACE 
#define BCM_PORT_PHY_CONTROL_MACSEC_ENABLE  _SHR_PORT_PHY_CONTROL_MACSEC_ENABLE 
#define BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED _SHR_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED 
#define BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED _SHR_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED 
#define BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX _SHR_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX 
#define BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE _SHR_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE 
#define BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD _SHR_PORT_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD 
#define BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD _SHR_PORT_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD 
#define BCM_PORT_PHY_CONTROL_MACSEC_LINE_IPG _SHR_PORT_PHY_CONTROL_MACSEC_LINE_IPG 
#define BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_IPG _SHR_PORT_PHY_CONTROL_MACSEC_SWITCH_IPG 
#define BCM_PORT_PHY_CONTROL_LONGREACH_SPEED _SHR_PORT_PHY_CONTROL_LONGREACH_SPEED 
#define BCM_PORT_PHY_CONTROL_LONGREACH_PAIRS _SHR_PORT_PHY_CONTROL_LONGREACH_PAIRS 
#define BCM_PORT_PHY_CONTROL_LONGREACH_GAIN _SHR_PORT_PHY_CONTROL_LONGREACH_GAIN 
#define BCM_PORT_PHY_CONTROL_LONGREACH_AUTONEG _SHR_PORT_PHY_CONTROL_LONGREACH_AUTONEG 
#define BCM_PORT_PHY_CONTROL_LONGREACH_LOCAL_ABILITY _SHR_PORT_PHY_CONTROL_LONGREACH_LOCAL_ABILITY 
#define BCM_PORT_PHY_CONTROL_LONGREACH_REMOTE_ABILITY _SHR_PORT_PHY_CONTROL_LONGREACH_REMOTE_ABILITY 
#define BCM_PORT_PHY_CONTROL_LONGREACH_CURRENT_ABILITY _SHR_PORT_PHY_CONTROL_LONGREACH_CURRENT_ABILITY 
#define BCM_PORT_PHY_CONTROL_LONGREACH_MASTER _SHR_PORT_PHY_CONTROL_LONGREACH_MASTER 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ACTIVE _SHR_PORT_PHY_CONTROL_LONGREACH_ACTIVE 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ENABLE _SHR_PORT_PHY_CONTROL_LONGREACH_ENABLE 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_NONE _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_NONE 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX 
#define BCM_PORT_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX _SHR_PORT_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX 
#define BCM_PORT_PHY_CONTROL_POWER          _SHR_PORT_PHY_CONTROL_POWER 
#define BCM_PORT_PHY_CONTROL_POWER_FULL     _SHR_PORT_PHY_CONTROL_POWER_FULL 
#define BCM_PORT_PHY_CONTROL_POWER_LOW      _SHR_PORT_PHY_CONTROL_POWER_LOW 
#define BCM_PORT_PHY_CONTROL_POWER_AUTO     _SHR_PORT_PHY_CONTROL_POWER_AUTO 
#define BCM_PORT_PHY_CONTROL_POWER_AUTO_DISABLE _SHR_PORT_PHY_CONTROL_POWER_AUTO_DISABLE 
#define BCM_PORT_PHY_CONTROL_POWER_AUTO_FULL _SHR_PORT_PHY_CONTROL_POWER_AUTO_FULL 
#define BCM_PORT_PHY_CONTROL_POWER_AUTO_LOW _SHR_PORT_PHY_CONTROL_POWER_AUTO_LOW 
#define BCM_PORT_PHY_CONTROL_POWER_AUTO_SLEEP_TIME _SHR_PORT_PHY_CONTROL_POWER_AUTO_SLEEP_TIME 
#define BCM_PORT_PHY_CONTROL_POWER_AUTO_WAKE_TIME _SHR_PORT_PHY_CONTROL_POWER_AUTO_WAKE_TIME 
#define BCM_PORT_PHY_CONTROL_LOOPBACK_EXTERNAL _SHR_PORT_PHY_CONTROL_LOOPBACK_EXTERNAL 
#define BCM_PORT_PHY_CONTROL_EDC_CONFIG_NONE _SHR_PORT_PHY_CONTROL_EDC_CONFIG_NONE 
#define BCM_PORT_PHY_CONTROL_EDC_CONFIG_HARDWARE _SHR_PORT_PHY_CONTROL_EDC_CONFIG_HARDWARE 
#define BCM_PORT_PHY_CONTROL_EDC_CONFIG_SOFTWARE _SHR_PORT_PHY_CONTROL_EDC_CONFIG_SOFTWARE 
#define BCM_PORT_PHY_CONTROL_EDC_MODE       _SHR_PORT_PHY_CONTROL_EDC_MODE 
#define BCM_PORT_PHY_CONTROL_CLOCK_ENABLE   _SHR_PORT_PHY_CONTROL_CLOCK_ENABLE 
#define BCM_PORT_PHY_CONTROL_CLOCK_SECONDARY_ENABLE _SHR_PORT_PHY_CONTROL_CLOCK_SECONDARY_ENABLE 
#define BCM_PORT_PHY_CONTROL_CLOCK_FREQUENCY _SHR_PORT_PHY_CONTROL_CLOCK_FREQUENCY 
#define BCM_PORT_PHY_CONTROL_CLOCK_MODE_AUTO _SHR_PORT_PHY_CONTROL_CLOCK_MODE_AUTO 
#define BCM_PORT_PHY_CONTROL_CLOCK_AUTO_SECONDARY _SHR_PORT_PHY_CONTROL_CLOCK_AUTO_SECONDARY 
#define BCM_PORT_PHY_CONTROL_CLOCK_SOURCE   _SHR_PORT_PHY_CONTROL_CLOCK_SOURCE 
#define BCM_PORT_PHY_CONTROL_PORT_PRIMARY   _SHR_PORT_PHY_CONTROL_PORT_PRIMARY 
#define BCM_PORT_PHY_CONTROL_PORT_OFFSET    _SHR_PORT_PHY_CONTROL_PORT_OFFSET 
#define BCM_PORT_PHY_CONTROL_LINKDOWN_TRANSMIT _SHR_PORT_PHY_CONTROL_LINKDOWN_TRANSMIT 
#define BCM_PORT_PHY_CONTROL_EDC_CONFIG     _SHR_PORT_PHY_CONTROL_EDC_CONFIG 
#define BCM_PORT_PHY_CONTROL_SUPER_ISOLATE  _SHR_PORT_PHY_CONTROL_SUPER_ISOLATE 
#define BCM_PORT_PHY_CONTROL_SOFT_RESET     _SHR_PORT_PHY_CONTROL_SOFT_RESET 
#define BCM_PORT_PHY_CONTROL_EEE            _SHR_PORT_PHY_CONTROL_EEE 
#define BCM_PORT_PHY_CONTROL_EEE_AUTO       _SHR_PORT_PHY_CONTROL_EEE_AUTO 
#define BCM_PORT_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD _SHR_PORT_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_EEE_AUTO_BUFFER_LIMIT _SHR_PORT_PHY_CONTROL_EEE_AUTO_BUFFER_LIMIT 
#define BCM_PORT_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY _SHR_PORT_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY 
#define BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_WAKE_TIME _SHR_PORT_PHY_CONTROL_EEE_TRANSMIT_WAKE_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_RECEIVE_WAKE_TIME _SHR_PORT_PHY_CONTROL_EEE_RECEIVE_WAKE_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_SLEEP_TIME _SHR_PORT_PHY_CONTROL_EEE_TRANSMIT_SLEEP_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_RECEIVE_SLEEP_TIME _SHR_PORT_PHY_CONTROL_EEE_RECEIVE_SLEEP_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_QUIET_TIME _SHR_PORT_PHY_CONTROL_EEE_TRANSMIT_QUIET_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_RECEIVE_QUIET_TIME _SHR_PORT_PHY_CONTROL_EEE_RECEIVE_QUIET_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_REFRESH_TIME _SHR_PORT_PHY_CONTROL_EEE_TRANSMIT_REFRESH_TIME 
#define BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_EVENTS _SHR_PORT_PHY_CONTROL_EEE_TRANSMIT_EVENTS 
#define BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_DURATION _SHR_PORT_PHY_CONTROL_EEE_TRANSMIT_DURATION 
#define BCM_PORT_PHY_CONTROL_EEE_RECEIVE_EVENTS _SHR_PORT_PHY_CONTROL_EEE_RECEIVE_EVENTS 
#define BCM_PORT_PHY_CONTROL_EEE_RECEIVE_DURATION _SHR_PORT_PHY_CONTROL_EEE_RECEIVE_DURATION 
#define BCM_PORT_PHY_CONTROL_EEE_STATISTICS_CLEAR _SHR_PORT_PHY_CONTROL_EEE_STATISTICS_CLEAR 
#define BCM_PORT_PHY_CONTROL_SERDES_TUNE_MARGIN_MAX _SHR_PORT_PHY_CONTROL_SERDES_TUNE_MARGIN_MAX 
#define BCM_PORT_PHY_CONTROL_SERDES_TUNE_MARGIN_MODE _SHR_PORT_PHY_CONTROL_SERDES_TUNE_MARGIN_MODE 
#define BCM_PORT_PHY_CONTROL_SERDES_TUNE_MARGIN_VALUE _SHR_PORT_PHY_CONTROL_SERDES_TUNE_MARGIN_VALUE 
#define BCM_PORT_PHY_CONTROL_PARALLEL_DETECTION _SHR_PORT_PHY_CONTROL_PARALLEL_DETECTION 
#define BCM_PORT_PHY_CONTROL_PARALLEL_DETECTION_10G _SHR_PORT_PHY_CONTROL_PARALLEL_DETECTION_10G 
#define BCM_PORT_PHY_CONTROL_LOOPBACK_INTERNAL _SHR_PORT_PHY_CONTROL_LOOPBACK_INTERNAL 
#define BCM_PORT_PHY_CONTROL_LOOPBACK_REMOTE _SHR_PORT_PHY_CONTROL_LOOPBACK_REMOTE 
#define BCM_PORT_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS _SHR_PORT_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS 
#define BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION _SHR_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION 
#define BCM_PORT_PHY_CONTROL_FEC_OFF        _SHR_PORT_PHY_CONTROL_FEC_OFF 
#define BCM_PORT_PHY_CONTROL_FEC_ON         _SHR_PORT_PHY_CONTROL_FEC_ON 
#define BCM_PORT_PHY_CONTROL_FEC_AUTO       _SHR_PORT_PHY_CONTROL_FEC_AUTO 
#define BCM_PORT_PHY_CONTROL_SCRAMBLER      _SHR_PORT_PHY_CONTROL_SCRAMBLER 
#define BCM_PORT_PHY_CONTROL_CUSTOM1        _SHR_PORT_PHY_CONTROL_CUSTOM1 
#define BCM_PORT_PHY_CONTROL_BERT_PATTERN   _SHR_PORT_PHY_CONTROL_BERT_PATTERN 
#define BCM_PORT_PHY_CONTROL_BERT_RUN       _SHR_PORT_PHY_CONTROL_BERT_RUN 
#define BCM_PORT_PHY_CONTROL_BERT_CRPAT     _SHR_PORT_PHY_CONTROL_BERT_CRPAT 
#define BCM_PORT_PHY_CONTROL_BERT_CJPAT     _SHR_PORT_PHY_CONTROL_BERT_CJPAT 
#define BCM_PORT_PHY_CONTROL_BERT_PACKET_SIZE _SHR_PORT_PHY_CONTROL_BERT_PACKET_SIZE 
#define BCM_PORT_PHY_CONTROL_BERT_IPG       _SHR_PORT_PHY_CONTROL_BERT_IPG 
#define BCM_PORT_PHY_CONTROL_BERT_TX_PACKETS _SHR_PORT_PHY_CONTROL_BERT_TX_PACKETS 
#define BCM_PORT_PHY_CONTROL_BERT_RX_PACKETS _SHR_PORT_PHY_CONTROL_BERT_RX_PACKETS 
#define BCM_PORT_PHY_CONTROL_BERT_RX_ERROR_BITS _SHR_PORT_PHY_CONTROL_BERT_RX_ERROR_BITS 
#define BCM_PORT_PHY_CONTROL_BERT_RX_ERROR_BYTES _SHR_PORT_PHY_CONTROL_BERT_RX_ERROR_BYTES 
#define BCM_PORT_PHY_CONTROL_BERT_RX_ERROR_PACKETS _SHR_PORT_PHY_CONTROL_BERT_RX_ERROR_PACKETS 
#define BCM_PORT_PHY_CONTROL_PON_LASER_TX_POWER_TIME _SHR_PORT_PHY_CONTROL_PON_LASER_TX_POWER_TIME 
#define BCM_PORT_PHY_CONTROL_PON_LASER_TX_POWER_MODE _SHR_PORT_PHY_CONTROL_PON_LASER_TX_POWER_MODE 
#define BCM_PORT_PHY_CONTROL_PON_LASER_TX_STATUS _SHR_PORT_PHY_CONTROL_PON_LASER_TX_STATUS  
#define BCM_PORT_PHY_CONTROL_PON_LASER_RX_STATE _SHR_PORT_PHY_CONTROL_PON_LASER_RX_STATE 
#define BCM_PORT_PHY_CONTROL_PON_LASER_TRANCEIVER_TEMP _SHR_PORT_PHY_CONTROL_PON_LASER_TRANCEIVER_TEMP 
#define BCM_PORT_PHY_CONTROL_PON_LASER_SUPPLY_VOLTAGE _SHR_PORT_PHY_CONTROL_PON_LASER_SUPPLY_VOLTAGE 
#define BCM_PORT_PHY_CONTROL_PON_LASER_TX_BIAS _SHR_PORT_PHY_CONTROL_PON_LASER_TX_BIAS 
#define BCM_PORT_PHY_CONTROL_PON_LASER_TX_POWER _SHR_PORT_PHY_CONTROL_PON_LASER_TX_POWER 
#define BCM_PORT_PHY_CONTROL_PON_LASER_RX_POWER _SHR_PORT_PHY_CONTROL_PON_LASER_RX_POWER 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_LOW_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_RX_POWER_LOW_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_LOW_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_LOW_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_LOW_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_LOW_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_LOW_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_TX_POWER_LOW_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_LOW_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_LOW_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_LOW_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_LOW_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_HIGH_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_VCC_HIGH_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_VCC_HIGH_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_HIGH_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_HIGH_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_HIGH_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_LOW_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_VCC_LOW_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_VCC_LOW_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_LOW_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_LOW_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_LOW_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_HIGH_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_TEMP_HIGH_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_HIGH_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_HIGH_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_HIGH_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_HIGH_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_LOW_ALARM_STATE _SHR_PORT_PHY_CONTROL_PON_TEMP_LOW_ALARM_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_LOW_ALARM_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_LOW_ALARM_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_LOW_ALARM_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_LOW_ALARM_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_HIGH_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_LOW_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_RX_POWER_LOW_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_LOW_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_LOW_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_RX_POWER_LOW_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_RX_POWER_LOW_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_HIGH_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_LOW_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_TX_POWER_LOW_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_LOW_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_LOW_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_POWER_LOW_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_POWER_LOW_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_HIGH_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TX_BIAS_LOW_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_HIGH_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_VCC_HIGH_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_VCC_HIGH_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_HIGH_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_HIGH_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_HIGH_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_LOW_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_VCC_LOW_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_VCC_LOW_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_LOW_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_VCC_LOW_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_VCC_LOW_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_HIGH_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_TEMP_HIGH_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_HIGH_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_HIGH_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_HIGH_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_HIGH_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_LOW_WARNING_STATE _SHR_PORT_PHY_CONTROL_PON_TEMP_LOW_WARNING_STATE 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_LOW_WARNING_REPORT_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_LOW_WARNING_REPORT_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_PON_TEMP_LOW_WARNING_CLEAR_THRESHOLD _SHR_PORT_PHY_CONTROL_PON_TEMP_LOW_WARNING_CLEAR_THRESHOLD 
#define BCM_PORT_PHY_CONTROL_DLL_POWER_AUTO _SHR_PORT_PHY_CONTROL_DLL_POWER_AUTO 
#define BCM_PORT_PHY_CONTROL_TX_LANE_SQUELCH _SHR_PORT_PHY_CONTROL_TX_LANE_SQUELCH 
#define BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH _SHR_PORT_PHY_CONTROL_RX_LANE_SQUELCH 
#define BCM_PORT_PHY_CONTROL_DRIVER_POST2_CURRENT _SHR_PORT_PHY_CONTROL_DRIVER_POST2_CURRENT 
#define BCM_PORT_PHY_CONTROL_RX_PPM         _SHR_PORT_PHY_CONTROL_RX_PPM 
#define BCM_PORT_PHY_CONTROL_RX_POLARITY    _SHR_PORT_PHY_CONTROL_RX_POLARITY 
#define BCM_PORT_PHY_CONTROL_TX_POLARITY    _SHR_PORT_PHY_CONTROL_TX_POLARITY 
#define BCM_PORT_PHY_CONTROL_RX_RESET       _SHR_PORT_PHY_CONTROL_RX_RESET 
#define BCM_PORT_PHY_CONTROL_TX_RESET       _SHR_PORT_PHY_CONTROL_TX_RESET 
#define BCM_PORT_PHY_CONTROL_CL72           _SHR_PORT_PHY_CONTROL_CL72 
#define BCM_PORT_PHY_CONTROL_CL72_STATUS    _SHR_PORT_PHY_CONTROL_CL72_STATUS 
#define BCM_PORT_PHY_CONTROL_LANE_SWAP      _SHR_PORT_PHY_CONTROL_LANE_SWAP 
#define BCM_PORT_PHY_CONTROL_RX_PEAK_FILTER _SHR_PORT_PHY_CONTROL_RX_PEAK_FILTER 
#define BCM_PORT_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER _SHR_PORT_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER 
#define BCM_PORT_PHY_CONTROL_RX_VGA         _SHR_PORT_PHY_CONTROL_RX_VGA 
#define BCM_PORT_PHY_CONTROL_RX_TAP1        _SHR_PORT_PHY_CONTROL_RX_TAP1 
#define BCM_PORT_PHY_CONTROL_RX_TAP2        _SHR_PORT_PHY_CONTROL_RX_TAP2 
#define BCM_PORT_PHY_CONTROL_RX_TAP3        _SHR_PORT_PHY_CONTROL_RX_TAP3 
#define BCM_PORT_PHY_CONTROL_RX_TAP4        _SHR_PORT_PHY_CONTROL_RX_TAP4 
#define BCM_PORT_PHY_CONTROL_RX_TAP5        _SHR_PORT_PHY_CONTROL_RX_TAP5 
#define BCM_PORT_PHY_CONTROL_RX_TAP6        _SHR_PORT_PHY_CONTROL_RX_TAP6 
#define BCM_PORT_PHY_CONTROL_RX_VGA_RELEASE _SHR_PORT_PHY_CONTROL_RX_VGA_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_TAP1_RELEASE _SHR_PORT_PHY_CONTROL_RX_TAP1_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_TAP2_RELEASE _SHR_PORT_PHY_CONTROL_RX_TAP2_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_TAP3_RELEASE _SHR_PORT_PHY_CONTROL_RX_TAP3_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_TAP4_RELEASE _SHR_PORT_PHY_CONTROL_RX_TAP4_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_TAP5_RELEASE _SHR_PORT_PHY_CONTROL_RX_TAP5_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_TAP6_RELEASE _SHR_PORT_PHY_CONTROL_RX_TAP6_RELEASE 
#define BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME _SHR_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME 
#define BCM_PORT_PHY_CONTROL_RX_PLUS1_SLICER _SHR_PORT_PHY_CONTROL_RX_PLUS1_SLICER 
#define BCM_PORT_PHY_CONTROL_RX_MINUS1_SLICER _SHR_PORT_PHY_CONTROL_RX_MINUS1_SLICER 
#define BCM_PORT_PHY_CONTROL_RX_D_SLICER    _SHR_PORT_PHY_CONTROL_RX_D_SLICER 
#define BCM_PORT_PHY_CONTROL_RX_SIGNAL_DETECT _SHR_PORT_PHY_CONTROL_RX_SIGNAL_DETECT  
#define BCM_PORT_PHY_CONTROL_RX_SEQ_DONE    _SHR_PORT_PHY_CONTROL_RX_SEQ_DONE 
#define BCM_PORT_PHY_CONTROL_RX_SEQ_TOGGLE  _SHR_PORT_PHY_CONTROL_RX_SEQ_TOGGLE 
#define BCM_PORT_PHY_CONTROL_DUMP           _SHR_PORT_PHY_CONTROL_DUMP 
#define BCM_PORT_PHY_CONTROL_FIRMWARE_MODE  _SHR_PORT_PHY_CONTROL_FIRMWARE_MODE 
#define BCM_PORT_PHY_CONTROL_DFE            _SHR_PORT_PHY_CONTROL_DFE 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_20BIT _SHR_PORT_PHY_CONTROL_TX_PATTERN_20BIT 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_256BIT _SHR_PORT_PHY_CONTROL_TX_PATTERN_256BIT 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA0 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA0 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA1 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA1 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA2 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA2 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA3 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA3 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA4 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA4 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA5 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA5 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA6 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA6 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_DATA7 _SHR_PORT_PHY_CONTROL_TX_PATTERN_DATA7 
#define BCM_PORT_PHY_CONTROL_PHASE_INTERP   _SHR_PORT_PHY_CONTROL_PHASE_INTERP 
#define BCM_PORT_PHY_CONTROL_TX_PPM_ADJUST  _SHR_PORT_PHY_CONTROL_TX_PPM_ADJUST 
#define BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS _SHR_PORT_PHY_CONTROL_SOFTWARE_RX_LOS 
#define BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS_LINK_WAIT_TIMER_US _SHR_PORT_PHY_CONTROL_SOFTWARE_RX_LOS_LINK_WAIT_TIMER_US 
#define BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS_RESTART_TIMER_US _SHR_PORT_PHY_CONTROL_SOFTWARE_RX_LOS_RESTART_TIMER_US 
#define BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE _SHR_PORT_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE 
#define BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA _SHR_PORT_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA 
#define BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL _SHR_PORT_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL 
#define BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE _SHR_PORT_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE 
#define BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA _SHR_PORT_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA 
#define BCM_PORT_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL _SHR_PORT_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL 
#define BCM_PORT_PHY_CONTROL_RX_PEAK_FILTER_TEMP_COMP _SHR_PORT_PHY_CONTROL_RX_PEAK_FILTER_TEMP_COMP 
#define BCM_PORT_PHY_CONTROL_CL73_FSM_AUTO_RECOVER _SHR_PORT_PHY_CONTROL_CL73_FSM_AUTO_RECOVER 
#define BCM_PORT_PHY_CONTROL_DRIVER_SUPPLY  _SHR_PORT_PHY_CONTROL_DRIVER_SUPPLY 
#define BCM_PORT_PHY_CONTROL_DRIVER_SUPPLY_1_5V_1_0V _SHR_PORT_PHY_CONTROL_DRIVER_SUPPLY_1_5V_1_0V 
#define BCM_PORT_PHY_CONTROL_DRIVER_SUPPLY_1_0V_700MV _SHR_PORT_PHY_CONTROL_DRIVER_SUPPLY_1_0V_700MV 
#define BCM_PORT_PHY_CONTROL_DRIVER_SUPPLY_1_5V_700MV _SHR_PORT_PHY_CONTROL_DRIVER_SUPPLY_1_5V_700MV 
#define BCM_PORT_PHY_CONTROL_DIGITAL_TEMP   _SHR_PORT_PHY_CONTROL_DIGITAL_TEMP 
#define BCM_PORT_PHY_CONTROL_ANALOG_TEMP    _SHR_PORT_PHY_CONTROL_ANALOG_TEMP 
#define BCM_PORT_PHY_CONTROL_MGBASET_802P3BZ_PRIORITY _SHR_PORT_PHY_CONTROL_MGBASET_802P3BZ_PRIORITY 
#define BCM_PORT_PHY_CONTROL_RX_LOS_NONE    _SHR_PORT_PHY_CONTROL_RX_LOS_NONE 
#define BCM_PORT_PHY_CONTROL_RX_LOS_SOFTWARE _SHR_PORT_PHY_CONTROL_RX_LOS_SOFTWARE 
#define BCM_PORT_PHY_CONTROL_RX_LOS_FIRMWARE _SHR_PORT_PHY_CONTROL_RX_LOS_FIRMWARE 
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE   _SHR_PORT_PHY_CONTROL_AUTONEG_MODE 
#define BCM_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE _SHR_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE 
#define BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE _SHR_PORT_PHY_CONTROL_PRBS_TX_ENABLE 
#define BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA _SHR_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA 
#define BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL _SHR_PORT_PHY_CONTROL_PRBS_POLYNOMIAL 
#define BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS _SHR_PORT_PHY_CONTROL_PRBS_RX_STATUS 
#define BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE _SHR_PORT_PHY_CONTROL_PRBS_RX_ENABLE 
#define BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA _SHR_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA 
#define BCM_PORT_PHY_CONTROL_PRBS_CHECK_MODE _SHR_PORT_PHY_CONTROL_PRBS_CHECK_MODE 
#define BCM_PORT_PHY_CONTROL_MOD_AUTO_DETECT_ENABLE _SHR_PORT_PHY_CONTROL_MOD_AUTO_DETECT_ENABLE 
#define BCM_PORT_PHY_CONTROL_TX_FIR_PRE     _SHR_PORT_PHY_CONTROL_TX_FIR_PRE 
#define BCM_PORT_PHY_CONTROL_TX_FIR_MAIN    _SHR_PORT_PHY_CONTROL_TX_FIR_MAIN 
#define BCM_PORT_PHY_CONTROL_TX_FIR_POST    _SHR_PORT_PHY_CONTROL_TX_FIR_POST 
#define BCM_PORT_PHY_CONTROL_TX_FIR_POST2   _SHR_PORT_PHY_CONTROL_TX_FIR_POST2 
#define BCM_PORT_PHY_CONTROL_TX_FIR_POST3   _SHR_PORT_PHY_CONTROL_TX_FIR_POST3 
#define BCM_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE _SHR_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_PMA_OS _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_PMA_OS 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_SCR_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_SCR_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_ENCODE_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_ENCODE_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_CL48_CHECK_END _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_CL48_CHECK_END 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_BLK_SYNC_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_BLK_SYNC_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_REORDER_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_REORDER_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_CL36_EN _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_CL36_EN 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESCR1_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESCR1_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_DEC1_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_DEC1_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESKEW_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESKEW_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESC2_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESC2_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESC2_BYTE_DEL _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_DESC2_BYTE_DEL 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_BRCM64B66_DESCR _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_BRCM64B66_DESCR 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_SGMII_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_SGMII_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT0 _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT0 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT1 _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_CLKCNT1 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_LPCNT0 _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_LPCNT0 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_LPCNT1 _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_LPCNT1 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_MAC_CGC _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_MAC_CGC 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_REPCNT _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_REPCNT 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_CRDTEN _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_CREDTEN 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_CLKCNT _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_CLKCNT 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_CGC _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_PCS_CGC 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_CL72_EN _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_CL72_EN 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_PLL_DIVIDER _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_PLL_DIVIDER 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_HTO_CLEAR _SHR_PORT_PHY_CONTROL_PCS_SPEED_HTO_CLEAR 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_ENTRY _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_ENTRY 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_HCD _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_HCD 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_PMA_OS _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_PMA_OS 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_SCR_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_SCR_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_ENCODE_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_ENCODE_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_CL48_CHECK_END _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_CL48_CHECK_END 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_BLK_SYNC_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_BLK_SYNC_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_REORDER_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_REORDER_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_CL36_EN _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_CL36_EN 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_DESCR1_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_DESCR1_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_DEC1_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_DEC1_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_DESKEW_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_DESKEW_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_DESC2_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_DESC2_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_DESC2_BYTE_DEL _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_DESC2_BYTE_DEL 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_BRCM64B66_DESCR _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_BRCM64B66_DESCR 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_SGMII_MODE _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_SGMII_MODE 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_CLKCNT0 _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_CLKCNT0 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_CLKCNT1 _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_CLKCNT1 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_LPCNT0 _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_LPCNT0 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_LPCNT1 _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_LPCNT1 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_MAC_CGC _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_MAC_CGC 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_REPCNT _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_REPCNT 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_CRDTEN _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_CREDTEN 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_CLKCNT _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_CLKCNT 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_CGC _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_PCS_CGC 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_CL72_EN _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_CL72_EN 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_NUM_OF_LANES _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_NUM_OF_LANES 
#define BCM_PORT_PHY_CONTROL_PCS_SPEED_ST_PLL_DIVIDER _SHR_PORT_PHY_CONTROL_PCS_SPEED_ST_PLL_DIVIDER 
#define BCM_PORT_PHY_CONTROL_LOOPBACK_PMD   _SHR_PORT_PHY_CONTROL_LOOPBACK_PMD 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_GEN_ENABLE _SHR_PORT_PHY_CONTROL_TX_PATTERN_GEN_ENABLE 
#define BCM_PORT_PHY_CONTROL_TX_PATTERN_LENGTH _SHR_PORT_PHY_CONTROL_TX_PATTERN_LENGTH 
#define BCM_PORT_PHY_CONTROL_INTR_MASK      _SHR_PORT_PHY_CONTROL_INTR_MASK /* PHY interrupt enable
                                                          mask (a bit set to 1
                                                          means interrupt
                                                          enabled) */
#define BCM_PORT_PHY_CONTROL_INTR_STATUS    _SHR_PORT_PHY_CONTROL_INTR_STATUS /* PHY interrupt control
                                                          mask (write 1 to
                                                          clear) */
#define BCM_PORT_PHY_CONTROL_XSW_LANE_MAP   _SHR_PORT_PHY_CONTROL_XSW_LANE_MAP /* Cross-switch lane map.
                                                          Bits[3:0] map Rx lane
                                                          0, bits [7:4] map Rx
                                                          lane 1, etc. Default
                                                          mapping is 0x76543210. */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE /* enable digital
                                                          feedback equalization */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE /* enable digital
                                                          feedback equalization */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE /* enable digital
                                                          feedback equalization */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE /* enable cl72 restart
                                                          timeout */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_AN_CL72_TX_INIT_SKIP_ON_RESTART _SHR_PORT_PHY_CONTROL_FIRMWARE_AN_CL72_TX_INIT_SKIP_ON_RESTART /* skip the TXFIR
                                                          initialization in a
                                                          restart event during
                                                          the AN link training */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE /* enable auto polarity
                                                          for cl72 training */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE /* enable firmware lane
                                                          config rx forced
                                                          extended reach */
#define BCM_PORT_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE _SHR_PORT_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE /* enable firmware lane
                                                          config rx forced
                                                          normal reach */
#define BCM_PORT_PHY_CONTROL_GPIO_CONFIG    _SHR_PORT_PHY_CONTROL_GPIO_CONFIG /* Config GPIO PINs. Bits
                                                          [3:0] controls GPIO
                                                          pin 0, bits [7:4]
                                                          controls GPIO pin 1,
                                                          etc */
#define BCM_PORT_PHY_CONTROL_GPIO_VALUE     _SHR_PORT_PHY_CONTROL_GPIO_VALUE /* Set value for GPIO
                                                          PINs. Bits [3:0]
                                                          controls GPIO pin 0,
                                                          bits [7:4] controls
                                                          GPIO pin 1, etc */
#define BCM_PORT_PHY_CONTROL_GPIO_CONFIG_DISABLED _SHR_PORT_PHY_CONTROL_GPIO_CONFIG_DISABLED 
#define BCM_PORT_PHY_CONTROL_GPIO_CONFIG_OUTPUT _SHR_PORT_PHY_CONTROL_GPIO_CONFIG_OUTPUT 
#define BCM_PORT_PHY_CONTROL_GPIO_CONFIG_INPUT _SHR_PORT_PHY_CONTROL_GPIO_CONFIG_INPUT 
#define BCM_PORT_PHY_CONTROL_PCS_MODE       _SHR_PORT_PHY_CONTROL_PCS_MODE 
#define BCM_PORT_PHY_CONTROL_AUTONEG_ALLOW_PLL_CHANGE _SHR_PORT_PHY_CONTROL_AUTONEG_ALLOW_PLL_CHANGE 
#define BCM_PORT_PHY_CONTROL_MEDIUM_TYPE    _SHR_PORT_PHY_CONTROL_MEDIUM_TYPE 
#define BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91 _SHR_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91 
#define BCM_PORT_PHY_CONTROL_AUTONEG_FEC_OVERRIDE _SHR_PORT_PHY_CONTROL_AUTONEG_FEC_OVERRIDE /* FEC override for 100G
                                                          AN noFEC mode */
#define BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108 _SHR_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108 
#define BCM_PORT_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE _SHR_PORT_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE 
#define BCM_PORT_PHY_CONTROL_UNRELIABLE_LOS _SHR_PORT_PHY_CONTROL_UNRELIABLE_LOS 
#define BCM_PORT_PHY_CONTROL_FORCED_SPEED_LINE_SIDE _SHR_PORT_PHY_CONTROL_FORCED_SPEED_LINE_SIDE /* Set speed on line side
                                                          of external PHY in
                                                          case of EBE mode only */
#define BCM_PORT_PHY_CONTROL_EGRESS_FLOW_CONTROL_MODE _SHR_PORT_PHY_EGRESS_FLOW_CONTROL_MODE /* Supported flow modes
                                                          as defined in
                                                          bcm_port_phy_control_flow_control_mode_t. */
#define BCM_PORT_PHY_CONTROL_INGRESS_FLOW_CONTROL_MODE _SHR_PORT_PHY_INGRESS_FLOW_CONTROL_MODE /* Supported flow modes
                                                          as defined in
                                                          bcm_port_phy_control_flow_control_mode_t */
#define BCM_PORT_PHY_CONTROL_PSM_COS_BMP    _SHR_PORT_PHY_CONTROL_PSM_COS_BMP 
#define BCM_PORT_PHY_CONTROL_AUTONEG_OUI    _SHR_PORT_PHY_CONTROL_AUTONEG_OUI 
#define BCM_PORT_PHY_CONTROL_AUTONEG_LINE_SIDE _SHR_PORT_PHY_CONTROL_AUTONEG_LINE_SIDE /* Valid in case of EBE
                                                          mode of MT2 PHY device
                                                          and can be applied to
                                                          enable Autoneg only on
                                                          line side of MT2
                                                          ports. */
#define BCM_PORT_PHY_CONTROL_FEC_CORRECTED_BLOCK_COUNT _SHR_PORT_PHY_CONTROL_FEC_CORRECTED_BLOCK_COUNT /* CL74 FEC corrected
                                                          block count. */
#define BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_BLOCK_COUNT _SHR_PORT_PHY_CONTROL_FEC_UNCORRECTED_BLOCK_COUNT /* CL74 FEC uncorrected
                                                          block count. */
#define BCM_PORT_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT _SHR_PORT_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT /* CL91 FEC corrected
                                                          code word count. */
#define BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT _SHR_PORT_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT /* CL91 FEC uncorrected
                                                          code word count. */
#define BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE _SHR_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE /* TX pam4 precoder
                                                          enable */
#define BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE _SHR_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE /* indicate link partner
                                                          tx has precoder
                                                          enabled */
#define BCM_PORT_PHY_CONTROL_PAM4_TX_PATTERN_ENABLE _SHR_PORT_PHY_CONTROL_PAM4_TX_PATTERN_ENABLE  /* PAM4 test pattern
                                                          enable */
#define BCM_PORT_PHY_CONTROL_FEC_SYMBOL_ERROR_COUNT _SHR_PORT_PHY_CONTROL_FEC_SYMBOL_ERROR_COUNT /* fec symbol error count */
#define BCM_PORT_PHY_CONTROL_TX_FIR_MODE    _SHR_PORT_PHY_CONTROL_TX_FIR_MODE /* tx fir mode */
#define BCM_PORT_PHY_CONTROL_TX_FIR_PRE2    _SHR_PORT_PHY_CONTROL_TX_FIR_PRE2 /* tx fir pre2 tap */
#define BCM_PORT_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER _SHR_PORT_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER /* rx high freq peaking
                                                          filter */
#define BCM_PORT_PHY_CONTROL_FAST_BER_PROJ  _SHR_PORT_PHY_CONTROL_FAST_BER_PROJ /* get the BER projection
                                                          using PRBS */
#define BCM_PORT_PHY_CONTROL_CHANNEL_LOSS_HINT _SHR_PORT_PHY_CONTROL_CHANNEL_LOSS_HINT /* specify the channel db
                                                          loss */
#define BCM_PORT_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST _SHR_PORT_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST /* set the tx fir post
                                                          initial value in uC
                                                          RAM for link training */
#define BCM_PORT_PHY_CONTROL_PCS_RESET      _SHR_PORT_PHY_CONTROL_PCS_RESET  /* toggle speed change */
#define BCM_PORT_PHY_CONTROL_PFC_USE_IP_COS _SHR_PORT_PHY_CONTROL_PFC_USE_IP_COS /* Use IP COS mapping in
                                                          case of PFC mode */
#define BCM_PORT_PHY_CONTROL_BASE_R_FEC_ECC_INTR_ENABLE _SHR_PORT_PHY_CONTROL_BASE_R_FEC_ECC_INTR_ENABLE  /* enable/disable base R
                                                          fec ECC intetrrupt */
#define BCM_PORT_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL _SHR_PORT_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL /* set the PMD debug log
                                                          level (valid value 0
                                                          to 6, default is 2),
                                                          user should call port
                                                          disable first, then
                                                          change the debug
                                                          level, followed with
                                                          port enable call */
#define BCM_PORT_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE _SHR_PORT_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE /* configure the AM
                                                          spacing to 20K for 50G
                                                          (2x25G) without FEC on
                                                          FLEXE */
#define BCM_PORT_PHY_CONTROL_FEC_CORRUPTION_PERIOD _SHR_PORT_PHY_CONTROL_FEC_CORRUPTION_PERIOD  /* config FEC corruption
                                                          period */
#define BCM_PORT_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE _SHR_PORT_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE /* enabling Force
                                                          Extended Reach in
                                                          AutoNeg mode */
#define BCM_PORT_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE _SHR_PORT_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE /* enabling Force Normal
                                                          Reach in AutoNeg mode */

#define BCM_PORT_PHY_FIRMWARE_DEFAULT       _SHR_PORT_PHY_FIRMWARE_DEFAULT /* firmware mode control
                                                          default */
#define BCM_PORT_PHY_FIRMWARE_SFP_OPT_SR4   _SHR_PORT_PHY_FIRMWARE_SFP_OPT_SR4 /* optical short range */
#define BCM_PORT_PHY_FIRMWARE_SFP_DAC       _SHR_PORT_PHY_FIRMWARE_SFP_DAC /* direct attach copper */
#define BCM_PORT_PHY_FIRMWARE_XLAUI         _SHR_PORT_PHY_FIRMWARE_XLAUI /* 40G XLAUI mode */
#define BCM_PORT_PHY_FIRMWARE_FORCE_OSDFE   _SHR_PORT_PHY_FIRMWARE_FORCE_OSDFE /* force over sample
                                                          digital feedback
                                                          equalization */
#define BCM_PORT_PHY_FIRMWARE_FORCE_BRDFE   _SHR_PORT_PHY_FIRMWARE_FORCE_BRDFE /* force baud rate
                                                          digital feedback
                                                          equalization */
#define BCM_PORT_PHY_FIRMWARE_SW_CL72       _SHR_PORT_PHY_FIRMWARE_SW_CL72 /* software cl72 with AN
                                                          on */
#define BCM_PORT_PHY_FIRMWARE_CL72_WITHOUT_AN _SHR_PORT_PHY_FIRMWARE_CL72_WITHOUT_AN /* cl72 without AN */

#define BCM_PORT_PHY_DFE_AUTO   _SHR_PORT_PHY_DFE_AUTO /* Auto control of DFE by firmware */
#define BCM_PORT_PHY_DFE_FREEZE _SHR_PORT_PHY_DFE_FREEZE /* Freeze DFE to current values */

#define BCM_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_DEFAULT _SHR_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_DEFAULT /* default drivermode */
#define BCM_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_NOT_SUPPORTED _SHR_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_NOT_SUPPORTED /* not supported
                                                          drivermode */
#define BCM_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_HALF_AMP _SHR_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_HALF_AMP /* half amp drivermode */
#define BCM_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_HALF_AMP_HI_IMPED _SHR_PORT_PHY_CONTROL_TX_FIR_DRIVERMODE_HALF_AMP_HI_IMPED /* high impedence half
                                                          amp drivermode */

#define BCM_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_OFF _SHR_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_OFF /* hg2 codec off */
#define BCM_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_8BYTE_IPG _SHR_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_8BYTE_IPG /* 8-byte IPG setting */
#define BCM_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_9BYTE_IPG _SHR_PORT_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_9BYTE_IPG /* 9-byte IPG setting */

#define BCM_PORT_PHY_CONTROL_TX_FIR_MODE_INVALID _SHR_PORT_PHY_TX_FIR_MODE_INVALID /* Invalid TX mode */
#define BCM_PORT_PHY_CONTROL_TX_FIR_3TAP_MODE _SHR_PORT_PHY_TX_FIR_3TAP_MODE /* TX  fir 3 tap mode */
#define BCM_PORT_PHY_CONTROL_TX_FIR_6TAP_MODE _SHR_PORT_PHY_TX_FIR_6TAP_MODE /* tx fir 6 tap mode */

#define BCM_PORT_PHY_CONTROL_PAM4_TX_PATTERN_INVALID _SHR_PORT_PHY_PAM4_TX_PATTERN_INVALID /* Invalid PAM4 test
                                                          pattern */
#define BCM_PORT_PHY_CONTROL_PAM4_TX_PATTERN_OFF _SHR_PORT_PHY_PAM4_TX_PATTERN_OFF /* PAM4 tx test pattern
                                                          off */
#define BCM_PORT_PHY_CONTROL_PAM4_TX_PATTERN_JP03B _SHR_PORT_PHY_PAM4_TX_PATTERN_JP03B /* PAM4 tx pattern JP03B */
#define BCM_PORT_PHY_CONTROL_PAM4_TX_PATTERN_LINEAR _SHR_PORT_PHY_PAM4_TX_PATTERN_LINEAR /* PAM4 tx pattern linear */

#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL37 _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37 /* Autoneg mode CL37 */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM /* Autoneg mode CL37 &
                                                          BAM */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL73 _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL73 /* Autoneg mode CL73 */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM /* Autoneg mode CL73 &
                                                          BAM */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_CL73 _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_CL73 /* Autoneg mode CL37 &
                                                          CL73 */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM /* Autoneg mode CL37 &
                                                          CL73 BAM */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73 _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73 /* Autoneg mode CL37 BAM
                                                          & CL73 */
#define BCM_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73_BAM _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73_BAM /* Autoneg mode CL37 BAM
                                                          & CL73 BAM */

#define BCM_PORT_PHY_CONTROL_AUTONEG_OUI_DEFAULT _SHR_PORT_PHY_CONTROL_AUTONEG_OUI_DEFAULT /* Autoneg OUI default */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get PHY specific configurations. */
extern int bcm_port_phy_control_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_control_t type, 
    uint32 value);

/* Set/Get PHY specific configurations. */
extern int bcm_port_phy_control_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_control_t type, 
    uint32 *value);

/* Set the given firmware to the PHY's non-volatile storage device. */
extern int bcm_port_phy_firmware_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int offset, 
    uint8 *array, 
    int length);

/* Get the GPORT ID for the specified local port number. */
extern int bcm_port_gport_get(
    int unit, 
    bcm_port_t port, 
    bcm_gport_t *gport);

/* 
 * Request if the given virtual port gport value is available on the
 * device.
 */
extern int bcm_port_gport_is_free(
    int unit, 
    bcm_gport_t gport);

/* 
 * Retrieve the minimum and maximum unallocated gport values for
 *  a given gport type.
 */
extern int bcm_port_gport_free_range_get(
    int unit, 
    uint32 gport_type, 
    bcm_gport_t *gport_min, 
    bcm_gport_t *gport_max);

/* Get local port number encoded within a GPORT ID. */
extern int bcm_port_local_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_t *local_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* flags for retreiving internal ports */
#define BCM_PORT_INTERNAL_EGRESS_REPLICATION 0x0001     
#define BCM_PORT_INTERNAL_OLP               0x0002     
#define BCM_PORT_INTERNAL_RECYCLE           0x0004     
#define BCM_PORT_INTERNAL_OAMP              0x0008     
#define BCM_PORT_INTERNAL_CONF_SCOPE_CORE0  0x0010     
#define BCM_PORT_INTERNAL_CONF_SCOPE_CORE1  0x0020     

#ifndef BCM_HIDE_DISPATCHABLE

/* Get handles to internal ports. */
extern int bcm_port_internal_get(
    int unit, 
    uint32 flags, 
    int internal_ports_max, 
    bcm_gport_t *internal_gport, 
    int *internal_ports_count);

/* 
 * Update the specified logical port (GPORT) with the given range(s) of
 * VIDs.
 */
extern int bcm_port_vlan_vector_set(
    int unit, 
    bcm_gport_t port_id, 
    bcm_vlan_vector_t vlan_vec);

/* Get the range(s) of VIDs for the specified logical port (GPORT). */
extern int bcm_port_vlan_vector_get(
    int unit, 
    bcm_gport_t port_id, 
    bcm_vlan_vector_t vlan_vec);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Backward compatibility. */
typedef _shr_port_stp_t bcm_port_stp_t;

/* Backward compatibility. */
#define BCM_PORT_STP_DISABLE    _SHR_PORT_STP_DISABLE 
#define BCM_PORT_STP_BLOCK      _SHR_PORT_STP_BLOCK 
#define BCM_PORT_STP_LISTEN     _SHR_PORT_STP_LISTEN 
#define BCM_PORT_STP_LEARN      _SHR_PORT_STP_LEARN 
#define BCM_PORT_STP_FORWARD    _SHR_PORT_STP_FORWARD 
#define BCM_PORT_STP_COUNT      _SHR_PORT_STP_COUNT 

/* Backward compatibility. */
#define bcm_port_vlan_set(u, p, vid)  \
    bcm_port_untagged_vlan_set(u, p, vid) 

/* Backward compatibility. */
#define bcm_port_vlan_get(u, p, vdata)  \
    bcm_port_untagged_vlan_get((u), (p), &((vdata)->vlan_tag)), \
    bcm_vlan_port_get((u), (vdata)->vlan_tag, \
                      &((vdata)->port_bitmap), &((vdata)->ut_port_bitmap)) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve the Policer ID associated for the specified physical port. */
extern int bcm_port_policer_get(
    int unit, 
    bcm_port_t port, 
    bcm_policer_t *policer_id);

/* Set the Policer ID associated for the specified physical port. */
extern int bcm_port_policer_set(
    int unit, 
    bcm_port_t port, 
    bcm_policer_t policer_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of statistics that are maintained per gport. */
typedef enum bcm_port_stat_e {
    bcmPortStatIngressPackets = 0,      /* Packets that ingress on the gport */
    bcmPortStatIngressBytes = 1,        /* Bytes that ingress on the gport */
    bcmPortStatEgressPackets = 2,       /* Packets that egress on the gport */
    bcmPortStatEgressBytes = 3,         /* Bytes that egress on the gport */
    bcmPortStatIngressPreemptPackets = 4, /* Preemptable packets that ingress on
                                           the gport */
    bcmPortStatIngressPreemptBytes = 5, /* Bytes of preemptable packets that
                                           ingress on the gport */
    bcmPortStatEgressPreemptPackets = 6, /* Preemptable packets that egress on
                                           the gport */
    bcmPortStatEgressPreemptBytes = 7   /* Bytes of preemptable packets that
                                           egress on the gport */
} bcm_port_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable/disable packet and byte counters for the selected gport. */
extern int bcm_port_stat_enable_set(
    int unit, 
    bcm_gport_t port, 
    int enable);

/* Get port counter value for specified port statistic type. */
extern int bcm_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and retrieve port counter value for
 * specified port statistic type.
 */
extern int bcm_port_stat_sync_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint64 *val);

/* Get stat counter ID associated with given gport. */
extern int bcm_port_stat_id_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 *stat_counter_id);

/* Get port counter value for specified port statistic type. */
extern int bcm_port_stat_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and retrieve port counter value for
 * specified port statistic type.
 */
extern int bcm_port_stat_sync_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 *val);

/* Set port counter value for specified port statistic type. */
extern int bcm_port_stat_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint64 val);

/* Set port counter value for specified port statistic type. */
extern int bcm_port_stat_set32(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 val);

/* Get 64-bit/32-bit counter value for multiple port statistic types. */
extern int bcm_port_stat_multi_get(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint64 *value_arr);

/* Get 64-bit/32-bit counter value for multiple port statistic types. */
extern int bcm_port_stat_multi_get32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint32 *value_arr);

/* Set 64-bit/32-bit counter value for multiple port statistic types. */
extern int bcm_port_stat_multi_set(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint64 *value_arr);

/* Set 64-bit/32-bit counter value for multiple port statistic types. */
extern int bcm_port_stat_multi_set32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint32 *value_arr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Generic port match criteria */
typedef enum bcm_port_match_e {
    BCM_PORT_MATCH_INVALID = 0,         /* Illegal */
    BCM_PORT_MATCH_NONE = 1,            /* No matching criteria */
    BCM_PORT_MATCH_PORT = 2,            /* Match on module/port or trunk */
    BCM_PORT_MATCH_PORT_VLAN = 3,       /* Match on module/port or trunk + outer
                                           VLAN or SVLAN */
    BCM_PORT_MATCH_PORT_VLAN_RANGE = 4, /* Match on module/port or trunk + VLAN
                                           range */
    BCM_PORT_MATCH_PORT_VLAN_STACKED = 5, /* Match on module/port or trunk +
                                           inner/outer VLAN */
    BCM_PORT_MATCH_TUNNEL_VLAN_SRCMAC = 6, /* Match on MiM B-VLAN and source MAC */
    BCM_PORT_MATCH_LABEL = 7,           /* Match on MPLS label */
    BCM_PORT_MATCH_LABEL_PORT = 8,      /* Match on MPLS label + module/port or
                                           trunk */
    BCM_PORT_MATCH_LABEL_VLAN = 9,      /* Match on MPLS label + VLAN */
    BCM_PORT_MATCH_PORT_PCP_VLAN = 10,  /*  Mod/port/trunk + outer VLAN + outer
                                           PCP. */
    BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED = 11, /*  Mod/port/trunk + outer/inner VLAN +
                                           outer/inner PCP. */
    BCM_PORT_MATCH_PORT_INITIAL_VLAN = 12, /*  Mod/port/trunk + Initial-VID */
    BCM_PORT_MATCH_PORT_PON_TUNNEL = 13, /*  Mod/port/trunk + PON Tunnel Value. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN = 14, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer VLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED = 15, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer VLAN + inner VLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN = 16, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer PCP + outer VLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN_STACKED = 17, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer PCP + outer VLAN + inner VLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN_ETHERTYPE = 18, /*  Mod/port/trunk + PON Tunnel Value +
                                           Outer PCP + outer VLAN + EtherType. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE = 19, /*  Mod/port/trunk + PON Tunnel Value +
                                           Outer PCP + outer VLAN + Inner VLAN +
                                           EtherType. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_ETHERTYPE = 20, /*  Mod/port/trunk + PON Tunnel Value +
                                           EtherType. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_ETHERTYPE = 21, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer VLAN + EtherType. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED_ETHERTYPE = 22, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer VLAN + inner VLAN + EtherType. */
    BCM_PORT_MATCH_PORT_VLAN16 = 23,    /* Mod/port/trunk + 16-bit outer VLAN
                                           tag. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_PCP = 24, /* Mod/port/trunk + PON Tunnel Value +
                                           outer PCP. */
    BCM_PORT_MATCH_PORT_UNTAGGED = 25,  /* Mod/port/trunk for untagged packets. */
    BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID = 26, /* Mod/port/trunk + Extender port VID */
    BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_VLAN = 27, /* Mod/port/trunk + Extender port VID +
                                           VLAN */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN = 28, /* Mod/port/trunk + LLVID [11:0] of
                                           LLTAG + I-VID[11:0] */
    BCM_PORT_MATCH_PORT_NIV_PORT_VIF = 29, /* Mod/port/trunk + NIV port VIF */
    BCM_PORT_MATCH_PORT_NIV_PORT_VIF_VLAN = 30, /* Mod/port/trunk + NIV port VIF + VLAN */
    BCM_PORT_MATCH_PORT_INNER_VLAN = 31, /* Mod/port/trunk + inner VLAN */
    BCM_PORT_MATCH_PORT_VPN = 32,       /* Logical Port + VPN */
    BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_INITIAL_VLAN = 33, /* Mod/port/trunk + Extender port VID +
                                           initial VLAN */
    BCM_PORT_MATCH_PORT_INNER_VLAN_RANGE = 34, /* Match on module/port or trunk + inner
                                           VLAN range */
    BCM_PORT_MATCH_PORT_VLAN_STACKED_RANGE = 35, /* Match on module/port or trunk + inner
                                           VLAN range + outer VLAN range */
    BCM_PORT_MATCH_PORT_INNER_VLAN16 = 36, /* Mod/port/trunk + 16-bit Inner VLAN
                                           tag */
    BCM_PORT_MATCH_PORT_I2E_CLASS = 37, /* port + Ingress to Egress(I2E) class
                                           id vlan type */
    BCM_PORT_MATCH_PORT_CVLAN = 38,     /* Mod/port/trunk + CVLAN */
    BCM_PORT_MATCH_MAC_PORT = 39,       /* Match on mac address + Mod/port/trunk
                                           or virtual port */
    BCM_PORT_MATCH_MAC_PORT_CLASS = 40, /* Match on mac address + port class id */
    BCM_PORT_MATCH_PORT_SVLAN_STACKED = 41, /* Match on module/port or trunk +
                                           inner/outer both SVLAN */
    BCM_PORT_MATCH_PORT_CVLAN_STACKED = 42, /* Match on module/port or trunk +
                                           inner/outer both CVLAN */
    BCM_PORT_MATCH_PORT_I2E_CLASS_IFP_TYPE = 43, /* port + Ingress to Egress(I2E) class
                                           id IFP type */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN = 44, /*  Mod/port/trunk + PON Tunnel Value +
                                           outer CVLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN_STACKED = 45, /*  Mod/port/trunk + PON Tunnel Value +
                                           inner/outer both CVLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_SVLAN_STACKED = 46, /*  Mod/port/trunk + PON Tunnel Value +
                                           inner/outer both SVLAN. */
    BCM_PORT_MATCH_PORT_PON_TUNNEL_UNTAGGED = 47, /*  Mod/port/trunk + PON Tunnel Value +
                                           untagged packets. */
    BCM_PORT_MATCH_PORT_GROUP_VLAN = 48, /*  Match on port group + outer VLAN. */
    BCM_PORT_MATCH_PORT_GROUP_INNER_VLAN = 49, /*  Match on port group + inner VLAN. */
    BCM_PORT_MATCH_PORT_GROUP_VLAN_STACKED = 50, /*  Match on port group + inner and
                                           outer VLAN. */
    BCM_PORT_MATCH_COUNT = 51           /* Must be last */
} bcm_port_match_t;

/* Flags for bcm_port_match_info_t */
#define BCM_PORT_MATCH_INGRESS_ONLY     0x00000001 /* Indicates Ingress settings */
#define BCM_PORT_MATCH_EGRESS_ONLY      0x00000002 /* Indicates Egress settings */
#define BCM_PORT_MATCH_NATIVE           0x00000004 /* Indicates Native Ethernet
                                                      settings */
#define BCM_PORT_MATCH_MIM              0x00000008 /* ISEM entry added contains
                                                      an ISID that points to a
                                                      MiM LIF. */
#define BCM_PORT_MATCH_MIM_SERVICE_SMAC 0x00000010 /* Match contains a BSA
                                                      nickname (in addition to
                                                      ISID). */

/* Generic port match attribute structure */
typedef struct bcm_port_match_info_s {
    bcm_port_match_t match;             /* Match criteria */
    bcm_gport_t port;                   /* Match port */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match */
    bcm_vlan_t match_vlan_max;          /* Maximum VLAN ID in range to match */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match */
    bcm_vlan_t match_inner_vlan_max;    /* Maximum Inner VLAN ID in range to
                                           match */
    bcm_vlan_t match_tunnel_vlan;       /* B-domain VID */
    bcm_mac_t match_tunnel_srcmac;      /* B-domain source MAC address */
    bcm_mpls_label_t match_label;       /* MPLS label */
    uint32 flags;                       /* BCM_PORT_MATCH_XXX flags */
    bcm_tunnel_id_t match_pon_tunnel;   /* PON Tunnel value to match. */
    bcm_port_ethertype_t match_ethertype; /* Ethernet type value to match */
    int match_pcp;                      /* Outer PCP ID to match */
    bcm_vlan_action_set_t *action;      /* Match action */
    uint16 extended_port_vid;           /* Extender port VID */
    bcm_vpn_t vpn;                      /* VPN ID */
    uint16 niv_port_vif;                /* NIV port VIF */
    uint32 isid; 
    bcm_mac_t src_mac;                  /* Source Mac Address */
    uint32 port_class;                  /* Port Class Id */
} bcm_port_match_info_t;

/* Initialize a Port Match Information structure. */
extern void bcm_port_match_info_t_init(
    bcm_port_match_info_t *port_match_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Add a match to an existing virtual port, or add multiple matches to a
 * shared virtual port and also gports such as Mirror destination ID.
 */
extern int bcm_port_match_add(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *match);

/* Remove a match from an existing port. */
extern int bcm_port_match_delete(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *match);

/* Replace an old match with a new one for an existing port. */
extern int bcm_port_match_replace(
    int unit, 
    bcm_gport_t port, 
    bcm_port_match_info_t *old_match, 
    bcm_port_match_info_t *new_match);

/* Get all the matches for an existing port. */
extern int bcm_port_match_multi_get(
    int unit, 
    bcm_gport_t port, 
    int size, 
    bcm_port_match_info_t *match_array, 
    int *count);

/* Assign a set of matches to an existing port. */
extern int bcm_port_match_set(
    int unit, 
    bcm_gport_t port, 
    int size, 
    bcm_port_match_info_t *match_array);

/* Delete all matches for an existing port. */
extern int bcm_port_match_delete_all(
    int unit, 
    bcm_gport_t port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Timesync Configuration flags. */
#define BCM_PORT_TIMESYNC_DEFAULT           0x00000001 /* Port Timesync
                                                          configuration */
#define BCM_PORT_TIMESYNC_MPLS              0x00000002 /* Port Timesync MPLS
                                                          configuration */
#define BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP 0x00000004 /* Port one step
                                                          timestamp
                                                          configuration */
#define BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP 0x00000008 /* Port two step
                                                          timestamp
                                                          configuration */
#define BCM_PORT_TIMESYNC_TIMESTAMP_CFUPDATE_ALL 0x00000010 /* All 1588 packets will
                                                          be egress timestamped.
                                                          By default, only
                                                          ingress timestamped
                                                          packets are egress
                                                          timestamped */
#define BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE 0x00000020 /* enable external mac
                                                          for the port */
#define BCM_PORT_PHY_CONTROL_FAIL_OVER_MODE _SHR_PORT_PHY_CONTROL_FAIL_OVER_MODE 
#define BCM_PORT_PHY_CONTROL_SHORT_CHANNEL_MODE _SHR_PORT_PHY_SHORT_CHANNEL_MODE 
#define BCM_PORT_PHY_CONTROL_SHORT_CHANNEL_MODE_STATUS _SHR_PORT_PHY_CONTROL_SHORT_CHANNEL_MODE_STATUS 
#define BCM_PORT_PHY_CONTROL_EYE_VAL_HZ_L   _SHR_PORT_PHY_CONTROL_EYE_VAL_HZ_L /* Eye scan value,
                                                          Horizantal left */
#define BCM_PORT_PHY_CONTROL_EYE_VAL_HZ_R   _SHR_PORT_PHY_CONTROL_EYE_VAL_HZ_R /* Eye scan value,
                                                          Horizantal right */
#define BCM_PORT_PHY_CONTROL_EYE_VAL_VT_U   _SHR_PORT_PHY_CONTROL_EYE_VAL_VT_U /* Eye scan value,
                                                          vertical up */
#define BCM_PORT_PHY_CONTROL_EYE_VAL_VT_D   _SHR_PORT_PHY_CONTROL_EYE_VAL_VT_D /* Eye scan value,
                                                          vertical down */

/* Timesync Packet type bitmaps. */
#define BCM_PORT_TIMESYNC_PKT_SYNC          0x00000001 /* Sync Event Packet type */
#define BCM_PORT_TIMESYNC_PKT_DELAY_REQ     0x00000002 /* Delay Request Event
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_PDELAY_REQ    0x00000004 /* Pdelay Request Event
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_PDELAY_RESP   0x00000008 /* Pdelay Response Event
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_FOLLOWUP      0x00000100 /* Followup General
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_DELAY_RESP    0x00000200 /* Delay_resp General
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP 0x00000400 /* Pdelay_resp_followup
                                                          General Packet type */
#define BCM_PORT_TIMESYNC_PKT_ANNOUNCE      0x00000800 /* Announce General
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_SIGNALLING    0x00001000 /* Signalling General
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_MANAGMENT     0x00002000 /* Management General
                                                          Packet type */
#define BCM_PORT_TIMESYNC_PKT_INVALID       0x00004000 /* Invalid Packet type */

/* Timesync configuration. */
typedef struct bcm_port_timesync_config_s {
    uint32 flags;           /* Timesync Configuration flags */
    uint32 pkt_drop;        /* Pakcet Drop bitmap of event and general  packet
                               types */
    uint32 pkt_tocpu;       /* CPU forward bitmap of event and general  packet
                               types */
    int mpls_min_label;     /* Timesync over MPLS min label */
    int mpls_max_label;     /* Timesync over MPLS max label */
    bcm_mac_t src_mac_addr; /* Source mac address for one-step timestamp update */
    int user_trap_id;       /* 1588 user trap id */
} bcm_port_timesync_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set timesync configurations for the port. */
extern int bcm_port_timesync_config_set(
    int unit, 
    bcm_port_t port, 
    int config_count, 
    bcm_port_timesync_config_t *config_array);

/* Get timesync configurations for the port. */
extern int bcm_port_timesync_config_get(
    int unit, 
    bcm_port_t port, 
    int array_size, 
    bcm_port_timesync_config_t *config_array, 
    int *array_count);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_PHY_TIMESYNC_CAP_MPLS      _SHR_PORT_PHY_TIMESYNC_CAP_MPLS /* PHY is 1588 over MPLS
                                                          capable */
#define BCM_PORT_PHY_TIMESYNC_CAP_ENHANCED_TSFIFO _SHR_PORT_PHY_TIMESYNC_CAP_ENHANCED_TSFIFO /* PHY has enhanced
                                                          TSFIFO */
#define BCM_PORT_PHY_TIMESYNC_CAP_INBAND_TS _SHR_PORT_PHY_TIMESYNC_CAP_INBAND_TS /* PHY supports inband
                                                          timestamping */
#define BCM_PORT_PHY_TIMESYNC_CAP_FOLLOW_UP_ASSIST _SHR_PORT_PHY_TIMESYNC_CAP_FOLLOW_UP_ASSIST /* PHY supports follow up
                                                          assist */
#define BCM_PORT_PHY_TIMESYNC_CAP_DELAY_RESP_ASSIST _SHR_PORT_PHY_TIMESYNC_CAP_DELAY_RESP_ASSIST /* PHY supports delay
                                                          response assist */
#define BCM_PORT_PHY_TIMESYNC_CAP_CAPTURE_TIMESTAMP_MSG _SHR_PORT_PHY_TIMESYNC_CAP_CAPTURE_TIMESTAMP_MSG /* PHY supports selective
                                                          timestamp capture of
                                                          1588 messages */
#define BCM_PORT_PHY_TIMESYNC_VALID_FLAGS   _SHR_PORT_PHY_TIMESYNC_VALID_FLAGS /* Flags field is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_ITPID   _SHR_PORT_PHY_TIMESYNC_VALID_ITPID /* itpid is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_OTPID   _SHR_PORT_PHY_TIMESYNC_VALID_OTPID /* otpid is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_OTPID2  _SHR_PORT_PHY_TIMESYNC_VALID_OTPID2 /* otpid2 is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_GMODE   _SHR_PORT_PHY_TIMESYNC_VALID_GMODE /* gmode is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE _SHR_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE /* framesync mode is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE _SHR_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE /* syncout mode is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER _SHR_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER /* ts divider mode is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE _SHR_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE /* original timecode is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET _SHR_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET /* tx timestamp offset is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET _SHR_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET /* rx timestamp offset is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE /* tx sync mode is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE /* tx delay request mode
                                                          is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE /* tx pdelay request mode
                                                          is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE /* tx pdelay response
                                                          mode is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE /* rx sync mode is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE /* rx delay request mode
                                                          is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE /* rx pdelay request mode
                                                          is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE /* rx pdelay response
                                                          mode is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL _SHR_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL /* mpls control is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY _SHR_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY /* rx link delay is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_SYNC_FREQ _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_SYNC_FREQ /* sync freq. is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 /* phy 1588 dpll k1 is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 /* phy 1588 dpll k2 is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3 _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3 /* phy 1588 dpll k3 is
                                                          valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER /* phy 1588 dpll loop
                                                          filter is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE /* phy 1588 dpll ref
                                                          phase is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA /* phy 1588 dpll ref
                                                          phase delta is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_TIMER_ADJUSTMENT _SHR_PORT_PHY_TIMESYNC_VALID_TIMER_ADJUSTMENT /* phy 1588 direct timer
                                                          adjustment is valid */
#define BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL /* phy 1588 inband ts
                                                          control is valid */
#define BCM_PORT_PHY_TIMESYNC_ENABLE        _SHR_PORT_PHY_TIMESYNC_ENABLE /* Enable Time
                                                          Synchronization
                                                          Interface */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE _SHR_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE /* Enable Packet
                                                          Timestamping */
#define BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE _SHR_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE /* Enable Heartbeat
                                                          Timestamping */
#define BCM_PORT_PHY_TIMESYNC_RX_CRC_ENABLE _SHR_PORT_PHY_TIMESYNC_RX_CRC_ENABLE /* Enable CRC checking */
#define BCM_PORT_PHY_TIMESYNC_8021AS_ENABLE _SHR_PORT_PHY_TIMESYNC_8021AS_ENABLE /* Enable IEEE802.1AS
                                                          mode */
#define BCM_PORT_PHY_TIMESYNC_L2_ENABLE     _SHR_PORT_PHY_TIMESYNC_L2_ENABLE /* IEEE1588 over Layer 2 */
#define BCM_PORT_PHY_TIMESYNC_IP4_ENABLE    _SHR_PORT_PHY_TIMESYNC_IP4_ENABLE /* IEEE1588 over IPV4 UDP */
#define BCM_PORT_PHY_TIMESYNC_IP6_ENABLE    _SHR_PORT_PHY_TIMESYNC_IP6_ENABLE /* IEEE1588 over IPV6 UDP */
#define BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT _SHR_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT /* Use external clock
                                                          source (DPLL) */
#define BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT_MODE _SHR_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT_MODE /* DPLL mode, 0 - phase
                                                          lock, 1 - frequency
                                                          lock */
#define BCM_PORT_PHY_TIMESYNC_1588_ENCRYPTED_MODE _SHR_PORT_PHY_TIMESYNC_1588_ENCRYPTED_MODE /* Encrypted mode, 0 -
                                                          disable, 1 - enable */
#define BCM_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE _SHR_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE /* Enable follow up
                                                          assistant */
#define BCM_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE _SHR_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE /* Enable delay response
                                                          assistant */
#define BCM_PORT_PHY_TIMESYNC_64BIT_TIMESTAMP_ENABLE _SHR_PORT_PHY_TIMESYNC_64BIT_TIMESTAMP_ENABLE /* Enable 64bit
                                                          timestamping */
#define BCM_PORT_PHY_TIMESYNC_1588_OVER_HSR_ENABLE _SHR_PORT_PHY_TIMESYNC_1588_OVER_HSR_ENABLE /* Enable 1588 over HSR */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_SYNC _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_SYNC /* Capture timestamp of
                                                          Tx Sync packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_DELAY_REQ _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_DELAY_REQ /* Capture timestamp of
                                                          Tx Delay Request
                                                          packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_REQ _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_REQ /* Capture timestamp of
                                                          Tx PDelay Request
                                                          packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_RESP _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_RESP /* Capture timestamp of
                                                          Tx PDelay Response
                                                          packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_SYNC _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_SYNC /* Capture timestamp of
                                                          Rx Sync packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_DELAY_REQ _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_DELAY_REQ /* Capture timestamp of
                                                          Rx Delay Request
                                                          packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_REQ _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_REQ /* Capture timestamp of
                                                          Rx PDelay Request
                                                          packets */
#define BCM_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_RESP _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_RESP /* Capture timestamp of
                                                          Rx PDelay Response
                                                          packets */
#define BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK _SHR_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK /* Enable inband resv0 id
                                                          check */
#define BCM_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE _SHR_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE /* Enable inband
                                                          timestamping for Sync */
#define BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE _SHR_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE /* Enable inband
                                                          timestamping for Delay
                                                          Request */
#define BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE _SHR_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE /* Enable inband
                                                          timestamping for
                                                          PDelay Request */
#define BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE _SHR_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE /* Enable inband
                                                          timestamping for
                                                          PDelay Response */
#define BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE _SHR_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE /* Enables update of
                                                          resv0 field in the
                                                          ingress packet with
                                                          the resv0_id */
#define BCM_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST _SHR_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST /* Enable inband
                                                          Follow_Up (T1) assist */
#define BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST _SHR_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST /* Enable inband
                                                          Delay_Response (T3)
                                                          assist */
#define BCM_PORT_PHY_TIMESYNC_INBAND_TIMER_MODE_SELECT _SHR_PORT_PHY_TIMESYNC_INBAND_TIMER_MODE_SELECT /* Select 80- or 48-bit
                                                          counter for inband PTP
                                                          messages */
#define BCM_PORT_PHY_TIMESYNC_INBAND_CAP_SRC_PORT_CLK_ID _SHR_PORT_PHY_TIMESYNC_INBAND_CAP_SRC_PORT_CLK_ID /* By default, MAC DA is
                                                          captured and saved in
                                                          the memory/FIFO. To
                                                          capture source port
                                                          clock ID instead, this
                                                          flag can be used. Only
                                                          one of them can be
                                                          selected at a time. */
#define BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID /* Enable the comparison
                                                          of the VLAN ID field
                                                          during delay
                                                          response(T3) and
                                                          follow up packet. */
#define BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM /* Enable the comparison
                                                          of the source port
                                                          number during delay
                                                          response(T3) and
                                                          follow up packet. */
#define BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR /* Enable the comparison
                                                          of the MAC address
                                                          during delay
                                                          response(T3) and
                                                          follow up packet. */
#define BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR /* Enable the comparison
                                                          of the IP address
                                                          during delay
                                                          response(T3) and
                                                          follow up packet. */
#define BCM_PORT_PHY_TIMESYNC_MPLS_LABEL_IN _SHR_PORT_PHY_TIMESYNC_MPLS_LABEL_IN /* MPLS label applies to
                                                          ingress packets */
#define BCM_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT _SHR_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT /* MPLS label applies to
                                                          egress packets */
#define BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE   _SHR_PORT_PHY_TIMESYNC_MPLS_ENABLE /* Enable processing of
                                                          MPLS 1588 packets */
#define BCM_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE _SHR_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE /* Enable MPLS entropy on
                                                          1588 packets */
#define BCM_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE _SHR_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE /* Enable MPLS special
                                                          label on 1588 packets */
#define BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE _SHR_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE /* Enable MPLS control
                                                          word on 1588 packets */
#define BCM_PORT_PHY_TIMESYNC_TN_LOAD       _SHR_PORT_PHY_TIMESYNC_TN_LOAD /* Load Tn upon framesync */
#define BCM_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD /* Always load Tn upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD _SHR_PORT_PHY_TIMESYNC_TIMECODE_LOAD /* Load timecode upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD /* Always load timecode
                                                          upon framesync */
#define BCM_PORT_PHY_TIMESYNC_SYNCOUT_LOAD  _SHR_PORT_PHY_TIMESYNC_SYNCOUT_LOAD /* Load syncout upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD /* Always load syncout
                                                          upon framesync */
#define BCM_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD /* Load NCO divider upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD /* Always load NCO
                                                          divider upon framesync */
#define BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD _SHR_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD /* Load local time upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD /* Always load local time
                                                          upon framesync */
#define BCM_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD /* Load NCO addend upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD /* Always load NCO addend
                                                          upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD /* Load DPLL loop filter
                                                          upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD /* Always load DPLL loop
                                                          filter upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD /* Load DPLL ref phase
                                                          upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD /* Always load DPLL ref
                                                          phase upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD /* Load DPLL ref phase
                                                          delta upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD /* Always load DPLL ref
                                                          phase delta upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD  _SHR_PORT_PHY_TIMESYNC_DPLL_K3_LOAD /* Load DPLL coefficient
                                                          K3 upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD /* Always load DPLL
                                                          coefficient K3 upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD  _SHR_PORT_PHY_TIMESYNC_DPLL_K2_LOAD /* Load DPLL coefficient
                                                          K2 upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD /* Always load DPLL
                                                          coefficient K2 upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD  _SHR_PORT_PHY_TIMESYNC_DPLL_K1_LOAD /* Load DPLL coefficient
                                                          K1 upon framesync */
#define BCM_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD /* Always load DPLL
                                                          coefficient K1 upon
                                                          framesync */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT /* Packet timestamp
                                                          interrupt */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_0 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_0 /* Packet timestamp
                                                          interrupt from port 0 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_1 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_1 /* Packet timestamp
                                                          interrupt from port 1 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_2 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_2 /* Packet timestamp
                                                          interrupt from port 2 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_3 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_3 /* Packet timestamp
                                                          interrupt from port 3 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_4 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_4 /* Packet timestamp
                                                          interrupt from port 4 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_5 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_5 /* Packet timestamp
                                                          interrupt from port 5 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_6 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_6 /* Packet timestamp
                                                          interrupt from port 6 */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_7 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_7 /* Packet timestamp
                                                          interrupt from port 7 */
#define BCM_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT /* Framesync interrupt */
#define BCM_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK /* Packet timestamp
                                                          interrupt mask */
#define BCM_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK /* Framesync interrupt
                                                          mask */
#define BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE _SHR_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE /* Enable One-Step
                                                          timestamping */

/* Actions on Egress event messages */
typedef enum bcm_port_phy_timesync_event_message_egress_mode_e {
    bcmPortPhyTimesyncEventMessageEgressModeNone = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE, /* Do nothing */
    bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD, /* Update correction field */
    bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN, /* Replace correction field + original
                                           timecode */
    bcmPortPhyTimesyncEventMessageEgressModeCaptureTimestamp = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP /* Capture TX timestamp */
} bcm_port_phy_timesync_event_message_egress_mode_t;

/* Actions on ingress event messages */
typedef enum bcm_port_phy_timesync_event_message_ingress_mode_e {
    bcmPortPhyTimesyncEventMessageIngressModeNone = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE, /* Do nothing */
    bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD, /* Update correction field */
    bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP, /* Insert timestamp */
    bcmPortPhyTimesyncEventMessageIngressModeInsertDelaytime = _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME /* Insert delay */
} bcm_port_phy_timesync_event_message_ingress_mode_t;

/* Global mode actions */
typedef enum bcm_port_phy_timesync_global_mode_e {
    bcmPortPhyTimesyncModeFree = _SHR_PORT_PHY_TIMESYNC_MODE_FREE, /* Do nothing */
    bcmPortPhyTimesyncModeSyncin = _SHR_PORT_PHY_TIMESYNC_MODE_SYNCIN, /* Use syncin as the source */
    bcmPortPhyTimesyncModeCpu = _SHR_PORT_PHY_TIMESYNC_MODE_CPU /* CPU based framesync */
} bcm_port_phy_timesync_global_mode_t;

/* Fast call actions */
typedef enum bcm_port_control_phy_timesync_e {
    bcmPortControlPhyTimesyncCaptureTimestamp = _SHR_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP, /* Captured TS from packets */
    bcmPortControlPhyTimesyncHeartbeatTimestamp = _SHR_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP, /* Heartbeat TS at framesync */
    bcmPortControlPhyTimesyncNCOAddend = _SHR_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND, /* NCO addend (for non DPLL mode) */
    bcmPortControlPhyTimesyncFrameSync = _SHR_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC, /* Cause immediate framesync */
    bcmPortControlPhyTimesyncLocalTime = _SHR_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME, /* Set local time (44 bits) */
    bcmPortControlPhyTimesyncLoadControl = _SHR_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL, /* Load control and sticky bits */
    bcmPortControlPhyTimesyncInterrupt = _SHR_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT, /* 1588 PHY interrupt status */
    bcmPortControlPhyTimesyncInterruptMask = _SHR_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK, /* 1588 PHY interrupt mask */
    bcmPortControlPhyTimesyncTxTimestampOffset = _SHR_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET, /* TX AFE delay */
    bcmPortControlPhyTimesyncRxTimestampOffset = _SHR_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET, /* RX AFE delay */
    bcmPortControlPhyTimesyncNSETimeOffset = _SHR_PORT_CONTROL_PHY_TIMESYNC_NSE_TIME_OFFSET, /* En_adj_80 << 15 | En_adj_48 << 14 |
                                           14 bit 2's comp value */
    bcmPortControlPhyTimesyncTimestampOffset = _SHR_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_OFFSET, /* the delay from CMIC to the PCS */
    bcmPortControlPhyTimesyncTimestampAdjust = _SHR_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_ADJUST, /* the adjustment of TS */
    bcmPortControlPhyTimesyncOneStepEnable = _SHR_PORT_CONTROL_PHY_TIMESYNC_ONE_STEP_ENABLE /* One step proecssing mode for TS */
} bcm_port_control_phy_timesync_t;

/* AM norm modes for aggregated port speeds */
typedef enum bcm_port_phy_timesync_compensation_mode_e {
    bcmPortPhyTimesyncCompensationModeNone = _SHR_PORT_PHY_TIMESYNC_COMPENSATION_MODE_NONE, /* No compensation */
    bcmPortPhyTimesyncCompensationModeEarliestLane = _SHR_PORT_PHY_TIMESYNC_COMPENSATION_MODE_EARLIEST_LANE, /* Compensate based on the earliest lane */
    bcmPortPhyTimesyncCompensationModeLatestlane = _SHR_PORT_PHY_TIMESYNC_COMPENSATION_MODE_LATEST_LANE /* Compensate based on the latest lane. */
} bcm_port_phy_timesync_compensation_mode_t;

/* SA-Discard actions */
#define BCM_PORT_CONTROL_DISCARD_MACSA_NONE 0x1        /* Packets with drop-sa
                                                          are switched by the
                                                          device */
#define BCM_PORT_CONTROL_DISCARD_MACSA_DROP 0x2        /* Packets with drop-sa
                                                          are dropped */
#define BCM_PORT_CONTROL_DISCARD_MACSA_TRAP 0x4        /* Packets with drop-sa
                                                          are copied to the CPU */

/* Unknown DA actions */
#define BCM_PORT_CONTROL_UNKNOWN_MACDA_NONE 0x1        /* Packets with
                                                          unknown-da are
                                                          switched by the device */
#define BCM_PORT_CONTROL_UNKNOWN_MACDA_DROP 0x2        /* Packets with
                                                          unknown-da are dropped */
#define BCM_PORT_CONTROL_UNKNOWN_MACDA_TRAP 0x4        /* Packets with
                                                          unknown-da are copied
                                                          to the CPU */

/* Per Port Filter enable/disable control */
#define BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC 0x1        /* disable egress filter
                                                          for unknown Unicast
                                                          packets */
#define BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC 0x2        /* disable egress filter
                                                          for unknown Multicast
                                                          packets */
#define BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC 0x4        /* disable egress filter
                                                          for Broadcast packets */
#define BCM_PORT_CONTROL_FILTER_DISABLE_ALL 0xFFFFFFFF /* disable all egress
                                                          filter */

/* sFlow packet sample destiantion flags */
#define BCM_PORT_CONTROL_SAMPLE_DEST_CPU    0x1        /* Copy packet samples to
                                                          CPU */
#define BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR 0x2        /* Copy packet samples to
                                                          all mirror
                                                          destinations created
                                                          with
                                                          BCM_MIRROR_PORT_SFLOW
                                                          flag */

/* Set EVB Port type. */
typedef enum bcm_port_evb_type_e {
    bcmPortEvbTypeNone = 0, 
    bcmPortEvbTypeUplinkAccess = 1, 
    bcmPortEvbTypeCount = 2         /* Must be last */
} bcm_port_evb_type_t;

/* SyncE recovered clock source */
typedef enum bcm_port_phy_clock_source_e {
    bcmPortPhyClockSourcePrimary = _SHR_PORT_PHY_CLOCK_SOURCE_PRIMARY, /* Recovery clock is being derived from
                                           primary port's recovered clock */
    bcmPortPhyClockSourceSecondary = _SHR_PORT_PHY_CLOCK_SOURCE_SECONDARY /* Recovery clock is being derived from
                                           the secondary port's recovered clock */
} bcm_port_phy_clock_source_t;

/* FrameSync mode */
typedef enum bcm_port_phy_timesync_framesync_mode_e {
    bcmPortPhyTimesyncFramesyncNone = _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_NONE, /* Do nothing */
    bcmPortPhyTimesyncFramesyncSyncin0 = _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0, /* Use long pulse on SyncIn0 for
                                           FrameSync */
    bcmPortPhyTimesyncFramesyncSyncin1 = _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1, /* Use SyncIn1 as FrameSync */
    bcmPortPhyTimesyncFramesyncSyncout = _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT, /* Use internal SyncOut as FrameSync */
    bcmPortPhyTimesyncFramesyncCpu = _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_CPU /* CPU based immediate FrameSync */
} bcm_port_phy_timesync_framesync_mode_t;

/* FrameSync configuration type. */
typedef struct bcm_port_phy_timesync_framesync_s {
    bcm_port_phy_timesync_framesync_mode_t mode; /* framesync mode */
    uint32 length_threshold;            /* syncin threshold for framesync */
    uint32 event_offset;                /* event offset for framesync */
} bcm_port_phy_timesync_framesync_t;

/* SyncOut mode */
typedef enum bcm_port_phy_timesync_syncout_mode_e {
    bcmPortPhyTimesyncSyncoutDisable = _SHR_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE, /* Do nothing */
    bcmPortPhyTimesyncSyncoutOneTime = _SHR_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME, /* One time SYNC pulse on a match with
                                           the timesync heartbeat */
    bcmPortPhyTimesyncSyncoutPulseTrain = _SHR_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN, /* Pulse train on SyncOut */
    bcmPortPhyTimesyncSyncoutPulseTrainWithSync = _SHR_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC /* Combination of pulse train and one
                                           time SYNC */
} bcm_port_phy_timesync_syncout_mode_t;

/* Event Message Type */
typedef enum bcm_port_phy_timesync_msg_e {
    bcmPortPhyTimesyncMsgSync = _SHR_PORT_PHY_TIMESYNC_MSG_SYNC, /* Sync */
    bcmPortPhyTimesyncMsgDelayReq = _SHR_PORT_PHY_TIMESYNC_MSG_DELAY_REQ, /* Delay Request */
    bcmPortPhyTimesyncMsgPdelayReq = _SHR_PORT_PHY_TIMESYNC_MSG_PDELAY_REQ, /* PDelay Request */
    bcmPortPhyTimesyncMsgPdelayResp = _SHR_PORT_PHY_TIMESYNC_MSG_PDELAY_RESP /* PDelay Response */
} bcm_port_phy_timesync_msg_t;

/* Protocol Type */
typedef enum bcm_port_phy_timesync_prot_e {
    bcmPortPhyTimesyncProtLayer2 = _SHR_PORT_PHY_TIMESYNC_PROT_LAYER2, /* 1588 over Layer 2 */
    bcmPortPhyTimesyncProtIpv4Udp = _SHR_PORT_PHY_TIMESYNC_PROT_IPV4_UDP, /* 1588 over IPv4 UDP */
    bcmbcmPortPhyTimesyncProtIpv6Udp = _SHR_PORT_PHY_TIMESYNC_PROT_IPV6_UDP /* 1588 over IPv6 UDP */
} bcm_port_phy_timesync_prot_t;

/* Direction Type */
typedef enum bcm_port_phy_timesync_dir_e {
    bcmPortPhyTimesyncDirEgress = _SHR_PORT_PHY_TIMESYNC_DIR_EGRESS, /* Egress Packet */
    bcmPortPhyTimesyncDirIngress = _SHR_PORT_PHY_TIMESYNC_DIR_INGRESS /* Igress Packet */
} bcm_port_phy_timesync_dir_t;

/* TimeSync timer mode */
typedef enum bcm_port_phy_timesync_timer_mode_e {
    bcmPortPhyTimesyncTimerModeNone = _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_NONE, /* No TimeSync timer */
    bcmPortPhyTimesyncTimerModeDefault = _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_DEFAULT, /* Use the default TimeSync timer */
    bcmPortPhyTimesyncTimerMode32bit = _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_32BIT, /* Use 32-bit TimeSync timer */
    bcmPortPhyTimesyncTimerMode48bit = _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT, /* Use 48-bit TimeSync timer */
    bcmPortPhyTimesyncTimerMode64bit = _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_64BIT, /* Use 64-bit TimeSync timer */
    bcmPortPhyTimesyncTimerMode80bit = _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT /* Use 80-bit TimeSync timer */
} bcm_port_phy_timesync_timer_mode_t;

/* Flow control modes */
typedef enum bcm_port_phy_control_flow_control_mode_e {
    bcmPortPhyControlFlowCtrlModeNone = _SHR_PORT_PHY_CONTROL_FLOW_CTRL_MODE_NONE, /* No flow control mode (Relay mode) */
    bcmPortPhyControlFlowCtrlLegacyPauseEn = _SHR_PORT_PHY_CONTROL_FLOW_CTRL_LEGACY_PAUSE_EN, /* Lagacy Pause flow control mode */
    bcmPortPhyControlFlowCtrlPfcPause = _SHR_PORT_PHY_CONTROL_FLOW_CTRL_PFC_PAUSE, /* PFC flow control mode */
    bcmPortPhyControlFlowCtrlLlfc = _SHR_PORT_PHY_CONTROL_FLOW_CTRL_LLFC /* LLFC flow control mode */
} bcm_port_phy_control_flow_control_mode_t;

/* Parameter for bcmPortControlFieldEgressClassSelect */
typedef enum bcm_port_field_egress_class_select_e {
    bcmPortEgressClassSelectNone = 0,   /* Class Id Not Assigned. */
    bcmPortEgressClassSelectPort = 1,   /* Class Id from Port Interface. */
    bcmPortEgressClassSelectSvp = 2,    /* Class Id from SVP. */
    bcmPortEgressClassSelectL3Interface = 3, /* Class Id from L3 IIF Interface. */
    bcmPortEgressClassSelectFieldSrc = 4, /* Class Id from VFP HI. */
    bcmPortEgressClassSelectFieldDst = 5, /* Class Id from VFP LOW. */
    bcmPortEgressClassSelectL2Src = 6,  /* Class Id from L2 SRC. */
    bcmPortEgressClassSelectL2Dst = 7,  /* Class Id from L2 DST. */
    bcmPortEgressClassSelectL3Src = 8,  /* Class Id from L3 SRC. */
    bcmPortEgressClassSelectL3Dst = 9,  /* Class Id from L3 DST. */
    bcmPortEgressClassSelectVlan = 10,  /* Class Id from VLAN Interface. */
    bcmPortEgressClassSelectVrf = 11,   /* Class Id from VRF. */
    bcmPortEgressClassSelectFieldIngress = 12, /* Class Id from IFP Policy Table. */
    bcmPortEgressClassSelectCount = 13  /* Always Last. Not a usable value. */
} bcm_port_field_egress_class_select_t;

/* Parameter for bcmPortControlFieldHiGigClassSelect */
typedef enum bcm_port_field_higig_class_select_e {
    bcmPortFieldHiGigClassSelectNone = 0, /* Class Id Not Assigned. */
    bcmPortFieldHiGigClassSelectIngressToEgress = 1, /* Class Id passed from ingress to
                                           egress. */
    bcmPortFieldHiGigClassSelectL3Egress = 2, /* Class Id from L3 egress object. */
    bcmPortFieldHiGigClassSelectL3EgressIntf = 3, /* Class Id from L3 egress interface. */
    bcmPortFieldHiGigClassSelectEgressGport = 4, /* Class Id from egress dvp. */
    bcmPortFieldHiGigClassSelectCount = 5 /* Always Last. Not a usable value. */
} bcm_port_field_higig_class_select_t;

/* syncout configuration type. */
typedef struct bcm_port_phy_timesync_syncout_s {
    bcm_port_phy_timesync_syncout_mode_t mode; /* syncout mode */
    uint16 pulse_1_length;              /* pulse 1 length in ns */
    uint16 pulse_2_length;              /* pulse 2 length in ns */
    uint32 interval;                    /* interval in ns */
    uint64 syncout_ts;                  /* syncout timestamp */
} bcm_port_phy_timesync_syncout_t;

/* MPLS label type. */
typedef struct bcm_port_phy_timesync_mpls_label_s {
    bcm_mpls_label_t value; /* label value */
    bcm_mpls_label_t mask;  /* label mask */
    uint32 flags;           /* MPLS label flags */
} bcm_port_phy_timesync_mpls_label_t;

/* MPLS control type. */
typedef struct bcm_port_phy_timesync_mpls_control_s {
    uint32 flags;                       /* MPLS control flags */
    bcm_mpls_label_t special_label;     /* special label value */
    bcm_port_phy_timesync_mpls_label_t labels[10]; /* Timesync MPLS labels */
    int size;                           /* Number of valid elements in label
                                           array */
} bcm_port_phy_timesync_mpls_control_t;

/* Timer adjustment type. */
typedef struct bcm_port_phy_timesync_timer_adjust_s {
    bcm_port_phy_timesync_timer_mode_t mode; /* Timer adjustment mode */
    int delta;                          /* Timer adjustment delta value */
} bcm_port_phy_timesync_timer_adjust_t;

/* Inband TS control type. */
typedef struct bcm_port_phy_timesync_inband_control_s {
    uint32 flags;                       /* Inband TS control flags */
    uint32 resv0_id;                    /* Resv0 ID used */
    bcm_port_phy_timesync_timer_mode_t timer_mode; /* Timer mode */
} bcm_port_phy_timesync_inband_control_t;

/* Enhanced capture type. */
typedef struct bcm_port_phy_timesync_enhanced_capture_s {
    bcm_port_phy_timesync_msg_t msg_type; /* Message type */
    bcm_port_phy_timesync_prot_t protocol; /* Protocol type */
    bcm_port_phy_timesync_dir_t direction; /* Direction type */
    uint32 seq_id;                      /* Sequence ID */
    uint32 domain;                      /* Domain */
    uint64 timestamp;                   /* Timestamp */
    uint64 source_port_identity;        /* Source port identity */
    uint16 source_port;                 /* Source port */
    uint16 vlan_id;                     /* Vlan ID */
} bcm_port_phy_timesync_enhanced_capture_t;

/* Base timesync configuration type. */
typedef struct bcm_port_phy_timesync_config_s {
    uint32 capabilities;                /* Flags BCM_PORT_PHY_TIMESYNC_CAP_* */
    uint32 validity_mask;               /* Flags BCM_PORT_PHY_TIMESYNC_VALID_* */
    uint32 flags;                       /* Flags BCM_PORT_PHY_TIMESYNC_* */
    uint16 itpid;                       /* 1588 inner tag */
    uint16 otpid;                       /* 1588 outer tag */
    uint16 otpid2;                      /* 1588 outer tag2 */
    bcm_port_phy_timesync_timer_adjust_t timer_adjust; /* Direct timer adjustment */
    bcm_port_phy_timesync_inband_control_t inband_control; /* Inband TS control */
    bcm_port_phy_timesync_global_mode_t gmode; /* Global mode */
    bcm_port_phy_timesync_framesync_t framesync; /* Framesync */
    bcm_port_phy_timesync_syncout_t syncout; /* Syncout */
    uint16 ts_divider;                  /* TS divider */
    bcm_time_spec_t original_timecode;  /* Original timecode to be inserted */
    uint32 tx_timestamp_offset;         /* TX AFE delay in ns - per port */
    uint32 rx_timestamp_offset;         /* RX AFE delay in ns - per port */
    uint32 rx_link_delay;               /* RX link delay */
    bcm_port_phy_timesync_event_message_egress_mode_t tx_sync_mode; /* sync */
    bcm_port_phy_timesync_event_message_egress_mode_t tx_delay_request_mode; /* delay request */
    bcm_port_phy_timesync_event_message_egress_mode_t tx_pdelay_request_mode; /* pdelay request */
    bcm_port_phy_timesync_event_message_egress_mode_t tx_pdelay_response_mode; /* pdelay response */
    bcm_port_phy_timesync_event_message_ingress_mode_t rx_sync_mode; /* sync */
    bcm_port_phy_timesync_event_message_ingress_mode_t rx_delay_request_mode; /* delay request */
    bcm_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_request_mode; /* pdelay request */
    bcm_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_response_mode; /* pdelay response */
    bcm_port_phy_timesync_mpls_control_t mpls_control; /* MPLS control */
    uint32 sync_freq;                   /* DPLL sync freq. */
    uint16 phy_1588_dpll_k1;            /* DPLL K1 */
    uint16 phy_1588_dpll_k2;            /* DPLL K2 */
    uint16 phy_1588_dpll_k3;            /* DPLL K3 */
    uint64 phy_1588_dpll_loop_filter;   /* DPLL loop filter */
    uint64 phy_1588_dpll_ref_phase;     /* DPLL ref phase */
    uint32 phy_1588_dpll_ref_phase_delta; /* DPLL ref phase delta */
} bcm_port_phy_timesync_config_t;

/* Initialize a Port Configuration structure. */
extern void bcm_port_timesync_config_t_init(
    bcm_port_timesync_config_t *port_timesync_config);

/* Initialize a Port Configuration structure. */
extern void bcm_port_phy_timesync_config_t_init(
    bcm_port_phy_timesync_config_t *conf);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get the port configuration for the specified device. */
extern int bcm_port_phy_timesync_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_timesync_config_t *conf);

/* Set/get the port configuration for the specified device. */
extern int bcm_port_phy_timesync_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_timesync_config_t *conf);

/* Retrieved the port configuration for the specified device. */
extern int bcm_port_control_phy_timesync_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_phy_timesync_t type, 
    uint64 value);

/* Retrieved the port configuration for the specified device. */
extern int bcm_port_control_phy_timesync_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_phy_timesync_t type, 
    uint64 *value);

/* Retrieved the port configuration for the specified device. */
extern int bcm_port_phy_timesync_enhanced_capture_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_timesync_enhanced_capture_t *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE _SHR_PORT_PHY_OAM_MAC_CHECK_ENABLE /* Enable MAC check for
                                                          Delay Measurement */
#define BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE _SHR_PORT_PHY_OAM_CONTROL_WORD_ENABLE /* Enable Control Word
                                                          for Delay Measurement */
#define BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE  _SHR_PORT_PHY_OAM_DM_ENTROPY_ENABLE /* Enable entropy for
                                                          Delay Measurement */
#define BCM_PORT_PHY_OAM_DM_TS_FORMAT       _SHR_PORT_PHY_OAM_DM_TS_FORMAT /* Select timestamp
                                                          format PTP(0)/NTP(1)
                                                          for Delay Measurement */

/* Port controls for phy OAM */
typedef enum bcm_port_control_phy_oam_e {
    bcmPortControlPhyOamDmTxEthertype = _SHR_PORT_CONTROL_PHY_OAM_DM_TX_ETHERTYPE, /* OAM Delay measurement Tx EtherType */
    bcmPortControlPhyOamDmRxEthertype = _SHR_PORT_CONTROL_PHY_OAM_DM_RX_ETHERTYPE, /* OAM Delay measurement Rx EtherType */
    bcmPortControlPhyOamDmTxPortMacAddressIndex = _SHR_PORT_CONTROL_PHY_OAM_DM_TX_PORT_MAC_ADDRESS_INDEX, /* Index to select the MAC address. Use
                                           index of 1 to select MacAddress1,
                                           index of 2 for MacAddress2 and so
                                           forth. */
    bcmPortControlPhyOamDmRxPortMacAddressIndex = _SHR_PORT_CONTROL_PHY_OAM_DM_RX_PORT_MAC_ADDRESS_INDEX, /* Index to select the MAC address. Use
                                           index of 1 to select MacAddress1,
                                           index of 2 for MacAddress2 and so
                                           forth. */
    bcmPortControlPhyOamDmMacAddress1 = _SHR_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_1, /* OAM Delay measurement MAC address 1.
                                           There can be multiple MAC address to
                                           choose from in a PHY depending upon
                                           the design */
    bcmPortControlPhyOamDmMacAddress2 = _SHR_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_2, /* OAM Delay measurement MAC address 2.
                                           There can be multiple MAC address to
                                           choose from in a PHY depending upon
                                           the design */
    bcmPortControlPhyOamDmMacAddress3 = _SHR_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_3 /* OAM Delay measurement MAC address 3.
                                           There can be multiple MAC address to
                                           choose from in a PHY depending upon
                                           the design */
} bcm_port_control_phy_oam_t;

/* Phy OAM Delay Measurement Modes */
typedef enum bcm_port_config_phy_oam_dm_mode_e {
    bcmPortConfigPhyOamDmModeY1731 = _SHR_PORT_CONFIG_PHY_OAM_DM_Y1731, /* OAM Delay measurement Y.7131 Mode */
    bcmPortConfigPhyOamDmModeBhh = _SHR_PORT_CONFIG_PHY_OAM_DM_BHH, /* OAM Delay measurement BHH Mode */
    bcmPortConfigPhyOamDmModeIetf = _SHR_PORT_CONFIG_PHY_OAM_DM_IETF /* OAM Delay measurement IETF Mode */
} bcm_port_config_phy_oam_dm_mode_t;

/* Phy OAM Delay measurement config type. */
typedef struct bcm_port_config_phy_oam_dm_s {
    uint32 flags;                       /* OAM Delay Measurement config flags */
    bcm_port_config_phy_oam_dm_mode_t mode; /* OAM Delay Measurement mode Y.1731,
                                           BHH or IETF */
} bcm_port_config_phy_oam_dm_t;

/* Phy OAM config type. */
typedef struct bcm_port_config_phy_oam_s {
    bcm_port_config_phy_oam_dm_t tx_dm_config; /* OAM delay measurement config for Tx */
    bcm_port_config_phy_oam_dm_t rx_dm_config; /* OAM delay measurement config for Rx */
} bcm_port_config_phy_oam_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set OAM configurations for the PHY. */
extern int bcm_port_config_phy_oam_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_config_phy_oam_t *conf);

/* Gets OAM configurations for the PHY. */
extern int bcm_port_config_phy_oam_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_config_phy_oam_t *conf);

/* Set the value for a particular OAM control type. */
extern int bcm_port_control_phy_oam_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_phy_oam_t type, 
    uint64 value);

/* Get the value for a  particular OAM control type. */
extern int bcm_port_control_phy_oam_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_phy_oam_t type, 
    uint64 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_CONGESTION_MAPPING_CLEAR   _SHR_PORT_CONGESTION_MAPPING_CLEAR /* Clear mapping */
#define BCM_PORT_CONGESTION_MAPPING_RX      _SHR_PORT_CONGESTION_MAPPING_RX /* Mapping for Rx
                                                          Direction */
#define BCM_PORT_CONGESTION_MAPPING_TX      _SHR_PORT_CONGESTION_MAPPING_TX /* Mapping for Tx
                                                          Direction */

#define BCM_PORT_CHANNEL_ID_SET(_channel_id, _port_base, _port_ch)  \
    _SHR_PORT_CHANNEL_ID_SET(_channel_id, _port_base, _port_ch) 

#define BCM_PORT_CHANNEL_ID_PORT_BASE_GET(_channel_id)  \
    _SHR_PORT_CHANNEL_ID_PORT_BASE_GET(_channel_id) 

#define BCM_PORT_CHANNEL_ID_PORT_CH_GET(_channel_id)  \
    _SHR_PORT_CHANNEL_ID_PORT_CH_GET(_channel_id) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE 
#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE 
#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_BR_DFE_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_BR_DFE_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_BR_DFE_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_BR_DFE_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_BR_DFE_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_BR_DFE_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(lane_config, medium_type)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(lane_config, medium_type) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_PAM4_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_PAM4_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_PAM4_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_PAM4_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NRZ_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NRZ_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NRZ_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NRZ_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(lane_config, channel_loss)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(lane_config, channel_loss) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_CLEAR(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_SET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_SET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_GET(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_GET(lane_config) 

#define BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(lane_config)  \
    _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(lane_config) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set mapping for port to  HCFC port(channel_id) */
extern int bcm_port_congestion_set(
    int unit, 
    bcm_gport_t congestion_port, 
    bcm_gport_t port, 
    uint32 flags, 
    int channel_id);

/* retreive mapping for port to  HCFC port(channel_id) */
extern int bcm_port_congestion_get(
    int unit, 
    bcm_gport_t congestion_port, 
    bcm_gport_t port, 
    uint32 flags, 
    int *channel_id);

/* Attach  counter entries to the given gport. */
extern int bcm_port_stat_attach(
    int unit, 
    bcm_gport_t port, 
    uint32 stat_counter_id);

/* Detach counter entries to the given gport. */
extern int bcm_port_stat_detach(
    int unit, 
    bcm_gport_t port);

/* 
 * Detach counter entries to the given gport with a given stat counter
 * id.
 */
extern int bcm_port_stat_detach_with_id(
    int unit, 
    bcm_gport_t port, 
    uint32 stat_counter_id);

/* Get counter values for specified gport based on port statistic type. */
extern int bcm_port_stat_counter_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter values for
 * specified gport based on port statistic type.
 */
extern int bcm_port_stat_counter_sync_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set   counter values for specified gport based on port statistic type. */
extern int bcm_port_stat_counter_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Attach flex state action to the given gport. */
extern int bcm_port_flexstate_attach(
    int unit, 
    bcm_gport_t port, 
    uint32 action_id);

/* 
 * Detach flex state action to the given gport with a given flex state
 * id.
 */
extern int bcm_port_flexstate_detach_with_id(
    int unit, 
    bcm_gport_t port, 
    uint32 action_id);

/* Set the encapsulation to port mapping from encap_id to port. */
extern int bcm_port_encap_map_set(
    int unit, 
    uint32 flags, 
    bcm_if_t encap_id, 
    bcm_gport_t port);

/* Get the encapsulation to port mapping from encap_id. */
extern int bcm_port_encap_map_get(
    int unit, 
    uint32 flags, 
    bcm_if_t encap_id, 
    bcm_gport_t *port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Allow remapping an encapsulation to a port on a different core. */
#define BCM_PORT_ENCAP_MAP_ALLOW_CORE_CHANGE 1          

#define BCM_PORT_PON_TUNNEL_WITH_ID 0x01       

#ifndef BCM_HIDE_DISPATCHABLE

/* Add service classifications(tunnel-port-ids) per pon port. */
extern int bcm_port_pon_tunnel_add(
    int unit, 
    bcm_gport_t pon_port, 
    uint32 flags, 
    bcm_gport_t *tunnel_port_id);

/* Remove service classifications(tunnel-port-ids) per pon port. */
extern int bcm_port_pon_tunnel_remove(
    int unit, 
    bcm_gport_t tunnel_port_id);

/* Set the tunnel id to the service class mapping */
extern int bcm_port_pon_tunnel_map_set(
    int unit, 
    bcm_gport_t pon_port, 
    bcm_tunnel_id_t tunnel_id, 
    bcm_gport_t tunnel_port_id);

/* Get the tunnel id to the service class mapping */
extern int bcm_port_pon_tunnel_map_get(
    int unit, 
    bcm_gport_t pon_port, 
    bcm_tunnel_id_t tunnel_id, 
    bcm_gport_t *tunnel_port_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Statistics threshold flag. */
#define BCM_STAT_THRESHOLD_FLAGS_NOT    (1 << 0)   

/* threshold type */
typedef enum bcm_stat_threshold_type_e {
    bcmStatThresholdDisabled = 0, 
    bcmStatThresholdSingle = 1, 
    bcmStatThresholdRange = 2, 
    bcmStatThresholdZero = 3, 
    bcmStatThresholdSigned = 4 
} bcm_stat_threshold_type_t;

/* 
 * Statistics threshold information.
 * Interpretations of arg1 and arg2:
 *     single:
 *         stat > arg1 raises alarm; stat <= arg2 clears alarm
 *     range:
 *         !(arg1 <= stat <= arg2) raises alarm; inverse clears alarm
 *     zero:
 *         stat == 0 raises alarm; stat != 0 clears alarm. arg1/2 ignored
 *     signed:
 *         as range, except arg1/2 are interpreted as signed (int32_t)
 */
typedef struct bcm_stat_threshold_s {
    bcm_stat_threshold_type_t type; /* threshold type, per-type data follows */
    uint32 flags;                   /* threshold flag */
    uint32 arg1;                    /* threshold parameter 1 */
    uint32 arg2;                    /* threshold parameter 2 */
} bcm_stat_threshold_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Statistics threshold set */
extern int bcm_stat_threshold_set(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t stat, 
    bcm_stat_threshold_t *threshold);

/* Statistics threshold get */
extern int bcm_stat_threshold_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t stat, 
    bcm_stat_threshold_t *threshold);

/* Get all allowed outer tag protocol IDs (TPIDs) for the specified port. */
extern int bcm_port_tpid_get_all(
    int unit, 
    bcm_port_t port, 
    int size, 
    uint16 *tpid_array, 
    int *color_array, 
    int *count);

/* 
 * Get all allowed L2 tunnel payload outer tag protocol IDs (TPIDs) for
 * the specified port.
 */
extern int bcm_port_l2_tunnel_payload_tpid_get_all(
    int unit, 
    bcm_port_t port, 
    int size, 
    uint16 *tpid_array, 
    int *color_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FCOE Fabric select. Used with bcmPortControlFcoeFabricSel */
typedef enum bcm_port_fcoe_vsan_select_e {
    bcmPortFcoeVsanSelectVft = 0,       /* use VFT header */
    bcmPortFcoeVsanSelectPort = 1,      /* use Port Table value */
    bcmPortFcoeVsanSelectInnerVlan = 2, /* use incoming inner VID/COS */
    bcmPortFcoeVsanSelectOuterVlan = 3  /* use incoming outer VID/COS */
} bcm_port_fcoe_vsan_select_t;

/* 
 * BCM56960 Port ASF (Cut-thru) Modes. Used with
 * bcmSwitchAlternateStoreForward
 */
typedef enum bcm_port_asf_mode_e {
    bcmPortAsfModeStoreAndForward = 0,  /* Store and Forward (SAF) forwarding
                                           mode */
    bcmPortAsfModeSameSpeed = 1,        /* Cut-thru forwarding between same
                                           speed ports */
    bcmPortAsfModeSlowToFast = 2,       /* Cut-thru forwarding between slower to
                                           faster speed ports */
    bcmPortAsfModeFastToSlow = 3        /* Cut-thru forwarding between faster to
                                           slower speed ports */
} bcm_port_asf_mode_t;

/* Define port mapping type */
typedef enum bcm_port_extender_mapping_type_e {
    bcmPortExtenderMappingTypePonTunnel = 0, /* Map information includes PON port and
                                           Tunnel-ID */
    bcmPortExtenderMappingTypePortVlan = 1, /* Map information includes Phy Port and
                                           VLAN-ID */
    bcmPortExtenderMappingTypeUserDefineValue = 2 /* Map information includes user define
                                           fields */
} bcm_port_extender_mapping_type_t;

#define BCM_PORT_EXTENDER_MAPPING_INGRESS   0x0001     /* Map information to
                                                          Incoming local PP port */
#define BCM_PORT_EXTENDER_MAPPING_EGRESS    0x0002     /* Map information to
                                                          Outgoing local PP port */

/* port mapping information */
typedef struct bcm_port_extender_mapping_info_s {
    bcm_gport_t pp_port;        /* Mapped Local PP port */
    bcm_tunnel_id_t tunnel_id;  /* Tunnel-ID */
    bcm_gport_t phy_port;       /* Physical Port connected to interface x
                                   channel */
    bcm_vlan_t vlan;            /* VLAN-ID */
    uint32 user_define_value;   /* User define value from header */
} bcm_port_extender_mapping_info_t;

/* Initialize a port extender mapping struct. */
extern void bcm_port_extender_mapping_info_t_init(
    bcm_port_extender_mapping_info_t *mapping_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure/retrieve port extender mapping from different inputs to
 * Local pp port.
 */
extern int bcm_port_extender_mapping_info_set(
    int unit, 
    uint32 flags, 
    bcm_port_extender_mapping_type_t type, 
    bcm_port_extender_mapping_info_t *mapping_info);

/* 
 * Configure/retrieve port extender mapping from different inputs to
 * Local pp port.
 */
extern int bcm_port_extender_mapping_info_get(
    int unit, 
    uint32 flags, 
    bcm_port_extender_mapping_type_t type, 
    bcm_port_extender_mapping_info_t *mapping_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Define vxlan vpn assignment criteria type */
typedef enum bcm_port_vxlan_vpn_assignment_criteria_e {
    bcmPortVxlanVpnAssignOnVnid = 0,    /* use VNID. */
    bcmPortVxlanVpnAssignOnVnidSrcIp = 1, /* use VNID and SIP. */
    bcmPortVxlanVpnAssignOnVnidOuterVlan = 2, /* use VNID and OVID. */
    bcmPortVxlanVpnAssignOnVnidOuterVlanSrcIp = 3 /* use VNID , OVID and SIP. */
} bcm_port_vxlan_vpn_assignment_criteria_t;

/* Define vxlan termination match criteria type */
typedef enum bcm_port_vxlan_termination_match_criteria_e {
    bcmPortVxlanTerminationMatchDstIp = 0, /* Match destination IP. */
    bcmPortVxlanTerminationMatchOuterVlanDstIp = 1, /* Match outer VLAN ID and destination
                                           IP. */
    bcmPortVxlanTerminationMatchSrcDstIp = 2, /* Match source & destination IP. */
    bcmPortVxlanTerminationMatchOuterVlanSrcDstIp = 3 /* Match outer VLAN ID and source &
                                           destination IP. */
} bcm_port_vxlan_termination_match_criteria_t;

/* Define vxlan gport assignment criteria type */
typedef enum bcm_port_vxlan_gport_assignment_criteria_e {
    bcmPortVxlanGportAssignOnSrcIp = 0, /* use SIP. */
    bcmPortVxlanGportAssignOnOuterVlanSrcIp = 1 /* use OVID and SIP. */
} bcm_port_vxlan_gport_assignment_criteria_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a GPORT ID for the specified physical port. */
extern int bcm_port_gport_add(
    int unit, 
    bcm_port_t local_port, 
    bcm_gport_t modport);

/* Del the GPORT ID for the specified physical port. */
extern int bcm_port_gport_delete(
    int unit, 
    bcm_port_t local_port, 
    bcm_gport_t modport);

/* Delete all the GPORT IDs for the specified physical port. */
extern int bcm_port_gport_delete_all(
    int unit, 
    bcm_port_t local_port);

/* Get all the GPORT ID for the specified physical port. */
extern int bcm_port_gport_get_all(
    int unit, 
    bcm_port_t local_port, 
    int size, 
    bcm_gport_t *gport_array, 
    int *count);

/* Configure/retrieve additional data for a wide LIF. */
extern int bcm_port_wide_data_set(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    uint64 data);

/* Configure/retrieve additional data for a wide LIF. */
extern int bcm_port_wide_data_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    uint64 *data);

/* Get local Encap id (local LIF id) per Gport (Global LIF id) */
extern int bcm_port_encap_local_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    int *encap_local_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port wide data set/get flags. */
#define BCM_PORT_WIDE_DATA_INGRESS  0x1        /* If set, wide data related to
                                                  ingress port (lif) extension. */
#define BCM_PORT_WIDE_DATA_EGRESS   0x2        /* If set, wide data related to
                                                  egress port (lif) extension. */

/* Port encap local id get flags. */
#define BCM_PORT_ENCAP_LOCAL_INGRESS    0x1        /* If set, encap local id
                                                      related to ingress port
                                                      (lif). */
#define BCM_PORT_ENCAP_LOCAL_EGRESS     0x2        /* If set, encap local id
                                                      related to egress port
                                                      (lif). */

/* serdes channel supported */
typedef enum bcm_port_phy_channel_e {
    bcmPortPhyChannelShort = _SHR_PORT_PHY_CHANNEL_SHORT, /* short channel */
    bcmPortPhyChannelLong = _SHR_PORT_PHY_CHANNEL_LONG, /* long channel */
    bcmPortPhyChannelAll = _SHR_PORT_PHY_CHANNEL_ALL, /* this is only used by
                                           bcm_port_apeed_ability_local_get */
    bcmPortPhyChannelCount = _SHR_PORT_PHY_CHANNEL_COUNT /* channel count */
} bcm_port_phy_channel_t;

/* serdes pause supported */
typedef enum bcm_port_phy_pause_e {
    bcmPortPhyPauseNone = _SHR_PORT_PHY_PAUSE_NONE, /* no pause */
    bcmPortPhyPauseTx = _SHR_PORT_PHY_PAUSE_TX, /* tx pause */
    bcmPortPhyPauseRx = _SHR_PORT_PHY_PAUSE_RX, /* rx pause */
    bcmPortPhyPauseSymm = _SHR_PORT_PHY_PAUSE_SYMM, /* Symm pause type */
    bcmPortPhyPauseALL = _SHR_PORT_PHY_PAUSE_ALL, /* this type is used only by
                                           bcm_port_speed_ability_local get */
    bcmPortPhyPauseCount = _SHR_PORT_PHY_PAUSE_COUNT /* pause types count */
} bcm_port_phy_pause_t;

/* antoneg mode supported */
typedef enum bcm_port_autoneg_mode_e {
    bcmPortAnModeCL37 = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37, /* cl37 autoneg mode */
    bcmPortAnModeCL37BAM = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM, /* CL37 BAM mode */
    bcmPortAnModeCL73 = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL73, /* CL73 an mode */
    bcmPortAnModeCL73BAM = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_BAM, /* CL73 BAM  an mode */
    bcmPortAnModeCL37_CL73 = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_CL73, /* autoneg CL37 and CL73 */
    bcmPortAnModeCL37_CL73BAM = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM, /* autoneg CL37 and CL73BAM */
    bcmPortAnModeCL37BAM_CL73BAM = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73_BAM, /* autoneg CL37BAM and CL73BAM */
    bcmPortAnModeCL73MSA = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_CL73_MSA, /* autoneg cl73 msa mode */
    bcmPortAnModeNone = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_NONE, /* this is only used by
                                           bcm_port_apeed_ability_local_get for
                                           forced speed */
    bcmPortAnModeCount = _SHR_PORT_PHY_CONTROL_AUTONEG_MODE_COUNT /* an mode count */
} bcm_port_autoneg_mode_t;

/* FEC types supported */
typedef enum bcm_port_phy_fec_e {
    bcmPortPhyFecDefaultRequest = _SHR_PORT_PHY_FEC_DEFAULT_REQUEST, /* Set this value to request default
                                           fec_type when calling
                                           bcm_port_resource_default_get */
    bcmPortPhyFecInvalid = _SHR_PORT_PHY_FEC_INVALID, /* invalid */
    bcmPortPhyFecNone = _SHR_PORT_PHY_FEC_NONE, /* no fec */
    bcmPortPhyFecBaseR = _SHR_PORT_PHY_FEC_BASE_R, /* CL74/Base-R. 64/66b KR FEC for fabric */
    bcmPortPhyFecRsFec = _SHR_PORT_PHY_FEC_RS_FEC, /* CL91/RS-FEC */
    bcmPortPhyFecRs544 = _SHR_PORT_PHY_FEC_RS_544, /* Rs544, using 1xN RS FEC architecture */
    bcmPortPhyFecRs272 = _SHR_PORT_PHY_FEC_RS_272, /* Rs272, using 1xN RS FEC architecture */
    bcmPortPhyFecRs206 = _SHR_PORT_PHY_FEC_RS_206, /* Rs206. 64/66b 5T RS FEC for fabric */
    bcmPortPhyFecRs108 = _SHR_PORT_PHY_FEC_RS_108, /* Rs108. 64/66b 5T low latency RS FEC
                                           for fabric */
    bcmPortPhyFecRs545 = _SHR_PORT_PHY_FEC_RS_545, /* Rs545. 64/66b 15T RS FEC for fabric */
    bcmPortPhyFecRs304 = _SHR_PORT_PHY_FEC_RS_304, /* Rs304. 64/66b 15T low latency RS FEC
                                           for fabric */
    bcmPortPhyFecRs544_2xN = _SHR_PORT_PHY_FEC_RS_544_2XN, /* Rs544, using 2xN RS FEC architecture */
    bcmPortPhyFecRs272_2xN = _SHR_PORT_PHY_FEC_RS_272_2XN, /* Rs272, using 2xN RS FEC architecture */
    bcmPortPhyFecCount = _SHR_PORT_PHY_FEC_COUNT /* Fec type count */
} bcm_port_phy_fec_t;

/* Port resource flags. */
#define BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR 0x1        /* If set, 25G port will
                                                          use 50G calendar. */
#define BCM_PORT_RESOURCE_SPEED_ONLY        0x2        /* If set, retain port
                                                          configuration for
                                                          flexport operations
                                                          where port mapping,
                                                          lanes and encap mode
                                                          are unchanged. */
#define BCM_PORT_RESOURCE_ASYMMETRICAL      0x4        /* If set, indicate the
                                                          Rx and Tx resources
                                                          are asymmetric. */
#define BCM_PORT_RESOURCE_TX                0x8        /* If set, only allocate
                                                          TX resource. */
#define BCM_PORT_RESOURCE_CASCADED          0x10       /* If set, the port is a
                                                          cascaded port. */
#define BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G 0x20       /* If set, prioritize
                                                          using 26.562G TVCO
                                                          instead of 25.781G. */

/* speed_ability supported by a port */
typedef struct bcm_port_speed_ability_s {
    uint32 speed;                       /* port speed in mbps */
    uint32 resolved_num_lanes;          /* autoneg resolved speed num lane */
    uint32 parallel_detect_en;          /* parallel detect */
    uint32 link_training;               /* link training */
    bcm_port_phy_fec_t fec_type;        /* fec_type for port */
    bcm_port_medium_t medium;           /* medium type of a port */
    bcm_port_phy_pause_t pause;         /* pause */
    bcm_port_phy_channel_t channel;     /* short/long channel for a port */
    bcm_port_autoneg_mode_t an_mode;    /* autoneg mode */
} bcm_port_speed_ability_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or get  a port advertised speed on BCM56980 */
extern int bcm_port_autoneg_ability_advert_set(
    int unit, 
    bcm_port_t port, 
    int num_ability, 
    bcm_port_speed_ability_t *abilities_array);

/* Set or get  a port advertised speed on BCM56980 */
extern int bcm_port_autoneg_ability_advert_get(
    int unit, 
    bcm_port_t port, 
    int max_num_ability, 
    bcm_port_speed_ability_t *abilities_array, 
    int *actual_num_ability);

/* Get a port link partner advertised speed on BCM56980 */
extern int bcm_port_autoneg_ability_remote_get(
    int unit, 
    bcm_port_t port, 
    int max_num_ability, 
    bcm_port_speed_ability_t *abilities_array, 
    int *actual_num_ability);

/* Get a local port supported speed ability on BCM56980 */
extern int bcm_port_speed_ability_local_get(
    int unit, 
    bcm_port_t port, 
    int max_num_ability, 
    bcm_port_speed_ability_t *abilities_array, 
    int *actual_num_ability);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port Resource Configuration */
typedef struct bcm_port_resource_s {
    uint32 flags;                   /* BCM_PORT_RESOURCE_XXX */
    bcm_gport_t port;               /* Local logical port number to connect to
                                       the given physical port */
    int physical_port;              /* Local physical port, -1 if the logical to
                                       physical mapping is to be deleted */
    int speed;                      /* Initial speed after FlexPort operation */
    int rx_speed;                   /* Indicate the Rx speed, valid only
                                       BCM_PORT_RESOURCE_ASYMMETRICAL flags is
                                       set */
    int lanes;                      /* Number of PHY lanes for this physical
                                       port */
    bcm_port_encap_t encap;         /* Encapsulation mode for port */
    bcm_port_phy_fec_t fec_type;    /* fec_type for port */
    int phy_lane_config;            /* serdes pmd lane config for port */
    int link_training;              /* link training on or off */
    int roe_compression;            /* enable roe_compression */
    int num_subports;               /* Number of subports if this is cascaded
                                       port. */
    int line_card;                  /* Line card number for the cascaded port */
    int base_flexe_instance;        /* Base FlexE instance ID. "-1" means the
                                       base instance ID is allocated by SW. For
                                       50G FlexE PHY, base instance ID can be
                                       0~7; For 100G FlexE PHY, valid values are
                                       0/2/4/6; For 200G FlexE PHY, valid values
                                       are 0/4; For 400G FlexE PHY, valid value
                                       is 0 */
} bcm_port_resource_t;

/* Initialize a Port Resource Configuration structure. */
extern void bcm_port_resource_t_init(
    bcm_port_resource_t *resource);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Perform a FlexPort operation changing the port resources for a given
 * logical port.
 */
extern int bcm_port_resource_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t *resource);

/* Get the port resource configuration. */
extern int bcm_port_resource_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t *resource);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_RESOURCE_DEFAULT_REQUEST   _SHR_PORT_RESOURCE_DEFAULT_REQUEST /* For
                                                          bcm_port_resource_default_get
                                                          - set this value to
                                                          all struct members for
                                                          which to get default
                                                          value from the API. */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get suggested default resource values prior to calling
 * bcm_port_resource_set.
 */
extern int bcm_port_resource_default_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    bcm_port_resource_t *resource);

/* 
 * Performs a FlexPort operation changing the port resources for a set of
 * ports.
 */
extern int bcm_port_resource_multi_set(
    int unit, 
    int nport, 
    bcm_port_resource_t *resource);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port Resource traverse callback. */
typedef int (*bcm_port_resource_traverse_cb)(
    int unit, 
    bcm_port_resource_t *resource, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over the port resource configurations on a given unit and call
 * user-provided callback for every entry.
 */
extern int bcm_port_resource_traverse(
    int unit, 
    bcm_port_resource_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Validate a port's speed configuration on the BCM56980 by fetching the
 * affected ports.
 */
extern int bcm_port_resource_speed_config_validate(
    int unit, 
    bcm_port_resource_t *resource, 
    bcm_pbmp_t *pbmp);

/* Get/Modify the speed for a given port. */
extern int bcm_port_resource_speed_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t *resource);

/* Get/Modify the speed for a given port. */
extern int bcm_port_resource_speed_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t *resource);

/* Modify the Port Speeds for a set of ports in a single Port Macro. */
extern int bcm_port_resource_speed_multi_set(
    int unit, 
    int nport, 
    bcm_port_resource_t *resource);

/* Enable traffic after Fast Reboot and reconfiguration has completed */
extern int bcm_port_fast_reboot_traffic_enable(
    int unit);

/* Set the list of enabled TDM ports. */
extern int bcm_port_ingress_tdm_failover_set(
    int unit, 
    int flag, 
    bcm_pbmp_t tdm_enable_pbmp);

/* Get the list of enabled TDM ports. */
extern int bcm_port_ingress_tdm_failover_get(
    int unit, 
    int flag, 
    bcm_pbmp_t *tdm_enable_pbmp);

/* Assign/retrieve the Priority Group mapped to the input priority. */
extern int bcm_port_priority_group_mapping_set(
    int unit, 
    bcm_gport_t gport, 
    int prio, 
    int priority_group );

/* Assign/retrieve the Priority Group mapped to the input priority. */
extern int bcm_port_priority_group_mapping_get(
    int unit, 
    bcm_gport_t gport, 
    int prio, 
    int *priority_group);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Priority Group attributes */
typedef struct bcm_port_priority_group_config_s {
    int shared_pool_xoff_enable;        /* enable/disable of triggering PFC
                                           messages of the Priority Group when
                                           exceeds egress shared pool xoff
                                           threshold */
    int shared_pool_discard_enable;     /* enable/disable of dropping packets of
                                           the Priority Group when exceeds
                                           egress shared pool discard threshold */
    int pfc_transmit_enable;            /* enable/disable of transmitting PFC
                                           message even in IBP state */
    uint16 priority_enable_vector_mask; /* Priority enable vector mask that
                                           mapped to PriorityEnableVector field
                                           in the PFC message */
} bcm_port_priority_group_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get the port priority group configuration. */
extern int bcm_port_priority_group_config_set(
    int unit, 
    bcm_gport_t gport, 
    int priority_group, 
    bcm_port_priority_group_config_t *prigrp_config);

/* Set/get the port priority group configuration. */
extern int bcm_port_priority_group_config_get(
    int unit, 
    bcm_gport_t gport, 
    int priority_group, 
    bcm_port_priority_group_config_t *prigrp_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a port priority group configuration struct. */
extern void bcm_port_priority_group_config_t_init(
    bcm_port_priority_group_config_t *prigrp_config);

/* Flags for port based packet redirection. */
#define BCM_PORT_REDIRECT_TRUNCATE          0x00000001 /* If set, redirected
                                                          packets will be
                                                          truncated. */
#define BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE 0x00000002 /* If set, the original
                                                          source port is used as
                                                          source of redirected
                                                          packets. */
#define BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION 0x00000004 /* If set, the original
                                                          destination port is
                                                          used as source of
                                                          redirected packets. */
#define BCM_PORT_REDIRECT_DROPPED_PACKET    0x00000008 /* If set, only packets
                                                          dropped will be
                                                          rediredcted. */
#define BCM_PORT_REDIRECT_NOT_DROPPED_PACKET 0x00000010 /* If set, only packets
                                                          not dropped will be
                                                          rediredcted. */
#define BCM_PORT_REDIRECT_DESTINATION_PORT  0x00000020 /* If set, the
                                                          destination for
                                                          redirected ports will
                                                          be set as a port. */
#define BCM_PORT_REDIRECT_DESTINATION_TRUNK 0x00000040 /* If set, the
                                                          destination for
                                                          redirected ports will
                                                          be set as a trunk. */
#define BCM_PORT_REDIRECT_DESTINATION_MCAST 0x00000080 /* If set, the
                                                          destination for
                                                          redirected ports will
                                                          be set as a L2
                                                          Multicast group. */

/* Flags used to select the buffer priority for redirected packets. */
#define BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW 0x00000001 /* If set, a low buffer
                                                          priority will be used
                                                          for redirected
                                                          packets. */
#define BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM 0x00000002 /* If set, a medium
                                                          buffer priority will
                                                          be used for redirected
                                                          packets. */
#define BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH 0x00000004 /* If set, a high buffer
                                                          priority will be used
                                                          for redirected
                                                          packets. */

/* New redirection packet color */
typedef enum bcm_port_redirect_color_e {
    bcmPortRedirectColorNone = 0, 
    bcmPortRedirectColorGreen = 1, 
    bcmPortRedirectColorYellow = 2, 
    bcmPortRedirectColorRed = 3 
} bcm_port_redirect_color_t;

/* Preemption control */
typedef enum bcm_port_preempt_control_e {
    bcmPortPreemptControlPreemptionSupport = 0, /* Support preemption operation. */
    bcmPortPreemptControlQueueBitmap = 1, /* Specify the preemptable queue in
                                           bitmap. */
    bcmPortPreemptControlEnableTx = 2,  /* Enable preemption in transmit
                                           direction. */
    bcmPortPreemptControlVerifyEnable = 3, /* Enable preemption verify operation. */
    bcmPortPreemptControlVerifyTime = 4, /* The wait time in ms between
                                           verification attempts. */
    bcmPortPreemptControlVerifyAttempts = 5, /* The number of verification attempts. */
    bcmPortPreemptControlNonFinalFragSizeTx = 6, /* The minimum size (in Bytes) of
                                           non-final fragment of a packet in
                                           transmit direction. */
    bcmPortPreemptControlFinalFragSizeTx = 7, /* The minimum size (in Bytes) of final
                                           fragment of a packet in transmit
                                           direction. */
    bcmPortPreemptControlNonFinalFragSizeRx = 8, /* The minimum size (in Bytes) of
                                           non-final fragment of a packet in
                                           receive direction. */
    bcmPortPreemptControlFinalFragSizeRx = 9, /* The minimum size (in Bytes) of final
                                           fragment of a packet in receive
                                           direction. */
    bcmPortPreemptControlHoldReqCntMode = 10, /* The MMU preemption request count mode
                                           for snmpBcmMacMergeHoldCount */
    bcmPortPreemptControlCount = 11     /* always the last one */
} bcm_port_preempt_control_t;

/* Preemption status */
typedef enum bcm_port_preempt_status_e {
    bcmPortPreemptStatusTx = 0,     /* Preemption status in the transmit
                                       direction. */
    bcmPortPreemptStatusVerify = 1  /* Preemption verify operation status . */
} bcm_port_preempt_status_t;

/* Preemption TX status */
typedef enum bcm_port_preempt_status_tx_e {
    bcmPortPreemptStatusTxInactive = 0, /* Verify is enabled but its status is
                                           either failed or not initiated. Or
                                           Preemption is not enabled. */
    bcmPortPreemptStatusTxVerifying = 1, /* Preemption is enabled and proceeding
                                           the verify operation. */
    bcmPortPreemptStatusTxActive = 2    /* Preemption is activated. */
} bcm_port_preempt_status_tx_t;

/* Preemption Verify status */
typedef enum bcm_port_preempt_status_verify_e {
    bcmPortPreemptStatusVerifyDisabled = 0, /* Verify operation is not enabled. */
    bcmPortPreemptStatusVerifyInitial = 1, /* Verify operation is enabled but the
                                           not started yet. */
    bcmPortPreemptStatusVerifyInProgress = 2, /* Verify operation is in progress. */
    bcmPortPreemptStatusVerifySucceeded = 3, /* Verify operation is succeeded. */
    bcmPortPreemptStatusVerifyFailed = 4 /* Verify operation is failed. */
} bcm_port_preempt_status_verify_t;

/* Port redirection attributes */
typedef struct bcm_port_redirect_config_s {
    int flags;                          /* Flags to pass with configuration data */
    int strength;                       /* Strength of the entry */
    int buffer_priority;                /* Buffering priority of the redirected
                                           packet */
    int destination;                    /* Destination of the redirected packets */
    int priority;                       /* Priority to be assigned for
                                           redirected packets */
    bcm_port_redirect_color_t color;    /* Color to be assigned to redirected
                                           packets */
} bcm_port_redirect_config_t;

/* Initialize a port redirection structure. */
extern void bcm_port_redirect_config_t_init(
    bcm_port_redirect_config_t *redirect_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure/retrieve port redirection data */
extern int bcm_port_redirect_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_redirect_config_t *redirect_config);

/* Configure/retrieve port redirection data */
extern int bcm_port_redirect_config_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_redirect_config_t *redirect_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port redirection attributes */
typedef struct bcm_port_nif_prio_s {
    int priority_level; /* NIF Priority level to set/get */
} bcm_port_nif_prio_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get port NIF priority. */
extern int bcm_port_nif_priority_set(
    int unit, 
    bcm_gport_t local_port, 
    uint32 flags, 
    bcm_port_nif_prio_t *priority, 
    bcm_pbmp_t *affected_ports);

/* Set/Get port NIF priority. */
extern int bcm_port_nif_priority_get(
    int unit, 
    bcm_gport_t local_port, 
    uint32 flags, 
    bcm_port_nif_prio_t *priority, 
    bcm_pbmp_t *affected_ports);

/* Get or set various configurations for Preemption */
extern int bcm_port_preemption_control_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_preempt_control_t type, 
    uint32 arg);

/* Get or set various configurations for Preemption */
extern int bcm_port_preemption_control_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_preempt_control_t type, 
    uint32 *arg);

/* Get the specified preemption status */
extern int bcm_port_preemption_status_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_port_preempt_status_t type, 
    uint32 *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* E2EFC modes */
typedef enum bcm_port_e2efc_mode_e {
    bcmPortE2efcModeTx = 0,     /* E2EFC frame transmit mode */
    bcmPortE2efcModeRx = 1,     /* E2EFC frame receive mode */
    bcmPortE2efcModeCount = 2   /* Must be last. Not a usable value */
} bcm_port_e2efc_mode_t;

/* Configuration of remote module */
typedef struct bcm_port_e2efc_remote_module_config_s {
    bcm_gport_t local_port; /* Local port for E2EFC message transaction, only
                               for E2EFC TX */
    int pkt_per_second;     /* Frequency of sending E2EFC messages, only for
                               E2EFC TX */
} bcm_port_e2efc_remote_module_config_t;

/* Initialize a bcm_port_e2efc_remote_module_config_t structure. */
extern void bcm_port_e2efc_remote_module_config_t_init(
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get the remote module that applies E2EFC mechanism. */
extern int bcm_port_e2efc_remote_module_enable_set(
    int unit, 
    bcm_module_t remote_module, 
    bcm_port_e2efc_mode_t mode, 
    int enable, 
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);

/* Set/get the remote module that applies E2EFC mechanism. */
extern int bcm_port_e2efc_remote_module_enable_get(
    int unit, 
    bcm_module_t remote_module, 
    bcm_port_e2efc_mode_t mode, 
    int *enable, 
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_e2efc_remote_module_traverse_cb */
typedef int (*bcm_port_e2efc_remote_module_traverse_cb)(
    int unit, 
    bcm_module_t remote_module, 
    bcm_port_e2efc_mode_t mode, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over the E2EFC remote module configuration on a given unit and
 * call user-provided callback for every entry.
 */
extern int bcm_port_e2efc_remote_module_traverse(
    int unit, 
    bcm_port_e2efc_remote_module_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Resource type would be used to configure set, reset and discard
 * thresholds
 */
typedef enum bcm_port_e2efc_resource_type_e {
    BCM_E2EFC_RESOURCE_TYPE_NONE = 0,   /* Resource type not needed */
    BCM_E2EFC_RESOURCE_TYPE_INT_MEM = 1, /* Resource type internal Memory
                                           configured in number of bytes */
    BCM_E2EFC_RESOURCE_TYPE_QEN = 2,    /* Resource type queue entries
                                           configured in number of packets */
    BCM_E2EFC_RESOURCE_TYPE_RQE = 3,    /* Resource type replication queue
                                           engine entries configured in number
                                           of packets */
    BCM_E2EFC_RESOURCE_TYPE_EXT_MEM = 4, /* Resource type external Memory
                                           configured in number of bytes */
    BCM_E2EFC_RESOURCE_TYPE_EMA = 5,    /* Resource type external Memory access
                                           configured in number of bytes */
    BCM_E2EFC_RESOURCE_TYPE_COUNT = 6   /* Number of E2EFC Resource type */
} bcm_port_e2efc_resource_type_t;

/* Configuration for remote port */
typedef struct bcm_port_e2efc_remote_port_config_s {
    bcm_module_t remote_module;         /* Remote module id that E2EFC applied */
    bcm_port_t remote_port;             /* Remote port id to be configured */
    int xoff_threshold_bytes;           /* Assert backpressure when number of
                                           buffers used is at or above this
                                           threshold in bytes */
    int xoff_threshold_packets;         /* Assert backpressure when number of
                                           buffers used is at or above this
                                           threshold in packets */
    int xon_threshold_bytes;            /* Remove backpressure when number of
                                           buffers used drop below this
                                           threshold in bytes */
    int xon_threshold_packets;          /* Remove backpressure when number of
                                           buffers used drop below this
                                           threshold in packets */
    int drop_threshold_bytes;           /* Drop packets when number of buffers
                                           used hit this threshold in bytes */
    int drop_threshold_packets;         /* Drop packets when number of buffers
                                           used hit this threshold in packets */
    bcm_port_e2efc_resource_type_t e2efc_resource_type; /* Type of resource being
                                           configured.Internal Memory, External
                                           Memory and EMA shall be configured in
                                           terms of bytes and Queue entry and
                                           RQE shall be configured in terms of
                                           packets */
} bcm_port_e2efc_remote_port_config_t;

/* Initialize a bcm_port_e2efc_remote_port_config_t structure. */
extern void bcm_port_e2efc_remote_port_config_t_init(
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add remote module-port and set configuration. */
extern int bcm_port_e2efc_remote_port_add(
    int unit, 
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg, 
    int *rport_handle_id);

/* Set/get the configuration of the remote port. */
extern int bcm_port_e2efc_remote_port_set(
    int unit, 
    int rport_handle_id, 
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);

/* Set/get the configuration of the remote port. */
extern int bcm_port_e2efc_remote_port_get(
    int unit, 
    int rport_handle_id, 
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);

/* Delete remote port. */
extern int bcm_port_e2efc_remote_port_delete(
    int unit, 
    int rport_handle_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_e2efc_remote_port_traverse_cb */
typedef int (*bcm_port_e2efc_remote_port_traverse_cb)(
    int unit, 
    int rport_handle_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterate over the E2EFC remote port configuration on a given unit and
 * call user-provided callback for every entry.
 */
extern int bcm_port_e2efc_remote_port_traverse(
    int unit, 
    bcm_port_e2efc_remote_port_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_timesync_tx_info */
typedef struct bcm_port_timesync_tx_info_s {
    uint64 timestamp;   /* transmit timestamp of ptp packet */
    uint32 sequence_id; /* sequence id of transmitted ptp packet */
    uint32 sub_ns;      /* timestamp sub-nanosecond */
} bcm_port_timesync_tx_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get 1588 packet's transmit information form the port. */
extern int bcm_port_timesync_tx_info_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_timesync_tx_info_t *tx_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Timestamp adjust */
typedef _shr_port_timestamp_adjust_t bcm_port_timestamp_adjust_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get or set timestamp OSTS/TSTS adjust. */
extern int bcm_port_timestamp_adjust_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_timestamp_adjust_t *ts_adjust);

/* Get or set timestamp OSTS/TSTS adjust. */
extern int bcm_port_timestamp_adjust_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_timestamp_adjust_t *ts_adjust);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Port max priority groups. */
#define BCM_PORT_MAX_NOF_PRIORITY_GROUPS    3          

/* Port priority */
#define BCM_PORT_F_PRIORITY_0   (1<<1)     /* User defined priority 0. */
#define BCM_PORT_F_PRIORITY_1   (1<<2)     /* User defined priority 1. */
#define BCM_PORT_F_PRIORITY_2   (1<<3)     /* User defined priority 2. */
#define BCM_PORT_F_PRIORITY_3   (1<<4)     /* User defined priority 3. */
#define BCM_PORT_F_PRIORITY_TDM (1<<16)    /* User defined priority TDM */

/* Priority Scheduler. */
typedef enum bcm_port_nif_scheduler_e {
    bcmPortNifSchedulerLow = 0,     /* Low Priority Scheduler */
    bcmPortNifSchedulerHigh = 1,    /* High Priority Scheduler */
    bcmPortNifSchedulerTDM = 2      /* TDM Priority Scheduler */
} bcm_port_nif_scheduler_t;

/* bcm_port_prio_group_config_t */
typedef struct bcm_port_prio_group_config_s {
    uint32 source_priority;             /* see BCM_PORT_F_PRIORITY_* */
    bcm_port_nif_scheduler_t sch_priority; /* see bcm_port_nif_scheduler_t */
    int num_of_entries;                 /* Indicate how many memory entries each
                                           priority group takes. Set to -1 for
                                           equal division between all
                                           priorities; otherwise indicate number
                                           of entries up to 128 per lane */
} bcm_port_prio_group_config_t;

/* bcm_port_prio_config_t */
typedef struct bcm_port_prio_config_s {
    int nof_priority_groups;            /* see BCM_PORT_F_PRIORITY */
    bcm_port_prio_group_config_t priority_groups[BCM_PORT_MAX_NOF_PRIORITY_GROUPS]; 
} bcm_port_prio_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure priority groups for the port. Priority group is meant to
 * connect one or more user priorities (see BCM_PORT_F_PRIORITY_*) to a
 * scheduler priority (High, Low, TDM) Each priority group acts as a
 * logical FIFO. The size of the FIFO can be dynamically specified, in
 * entries.
 */
extern int bcm_port_priority_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_prio_config_t *port_priority_config);

/* Retrieve the port priority group configuration. */
extern int bcm_port_priority_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_prio_config_t *priority_config);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_LANE_TO_SERDES_NOT_MAPPED  -1         /* A lane is unmapped for
                                                          both Tx and Rx.
                                                          Unmapped lanes can't
                                                          be assigned to a port. */

/* 
 * This struct holds serdes information (rx and tx) related to a specific
 * lane.
 */
typedef struct bcm_port_lane_to_serdes_map_s {
    int serdes_rx_id; 
    int serdes_tx_id; 
} bcm_port_lane_to_serdes_map_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Map lanes to serdeses. */
extern int bcm_port_lane_to_serdes_map_set(
    int unit, 
    int flags, 
    int map_size, 
    bcm_port_lane_to_serdes_map_t *serdes_map);

/* Map lanes to serdeses. */
extern int bcm_port_lane_to_serdes_map_get(
    int unit, 
    int flags, 
    int map_size, 
    bcm_port_lane_to_serdes_map_t *serdes_map);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE 0x1        /* Update lane to serdes
                                                          mapping on fabric
                                                          side. */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * This API is used to configure the order of the ILKN lanes. Each index
 * in lanes[] array stands for ILKN lane ID. The values of lanes[] array
 * is the NIF/Fabric lanes mapped to ILKN logical lane.
 */
extern int bcm_port_ilkn_lane_map_set(
    int unit, 
    int flags, 
    bcm_port_t port, 
    int num_of_lanes, 
    int *lanes);

/* 
 * Get the order of the ILKN lanes for ILKN ports. Each index in lanes[]
 * array stands for ILKN lane ID. The values of lanes[] array is the
 * NIF/Fabric lanes mapped to ILKN logical lane.
 */
extern int bcm_port_ilkn_lane_map_get(
    int unit, 
    int flags, 
    bcm_port_t port, 
    int num_of_lanes, 
    int *lanes);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Per port and LIF flooding destination information. */
typedef struct bcm_port_flood_group_s {
    bcm_gport_t unknown_unicast_group;  /* Destination for unknown unicast
                                           packets. */
    bcm_gport_t unknown_multicast_group; /* Destination for unknown multicast
                                           packets. */
    bcm_gport_t broadcast_group;        /* Destination for broadcast packets. */
} bcm_port_flood_group_t;

/* Initialize a port flooding group structure. */
extern void bcm_port_flood_group_t_init(
    bcm_port_flood_group_t *flood_groups);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the flooding groups for ports or LIFs. */
extern int bcm_port_flood_group_set(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    bcm_port_flood_group_t *flood_groups);

/* Set/Get the flooding groups for ports or LIFs. */
extern int bcm_port_flood_group_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    bcm_port_flood_group_t *flood_groups);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_port_phy_tx_tap_mode_e {
    bcmPortPhyTxTapMode3Tap = _SHR_PORT_PHY_TX_TAP_MODE_3_TAP, 
    bcmPortPhyTxTapMode6Tap = _SHR_PORT_PHY_TX_TAP_MODE_6_TAP 
} bcm_port_phy_tx_tap_mode_t;

typedef enum bcm_port_phy_signalling_mode_e {
    bcmPortPhySignallingModeNRZ = _SHR_PORT_PHY_SIGNALLING_MODE_NRZ, 
    bcmPortPhySignallingModePAM4 = _SHR_PORT_PHY_SIGNALLING_MODE_PAM4 
} bcm_port_phy_signalling_mode_t;

typedef struct bcm_port_phy_tx_s {
    int pre2; 
    int pre; 
    int main; 
    int post; 
    int post2; 
    int post3; 
    int amp; 
    int drv_mode; 
    bcm_port_phy_tx_tap_mode_t tx_tap_mode; 
    bcm_port_phy_signalling_mode_t signalling_mode; 
} bcm_port_phy_tx_t;

/* Initialize the bcm_port_phy_tx_t structure. */
extern void bcm_port_phy_tx_t_init(
    bcm_port_phy_tx_t *tx);

#ifndef BCM_HIDE_DISPATCHABLE

/* Modify TX FIR parameters for a given port */
extern int bcm_port_phy_tx_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_tx_t *tx);

/* Get TX FIR parameters for a given port */
extern int bcm_port_phy_tx_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_tx_t *tx);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_port_rlm_config_s {
    int is_initiator; 
    uint32 active_lane_bit_map; /* input only valid for initator */
} bcm_port_rlm_config_t;

/* Initialize a Port Reduced Lane Mode Configuration structure. */
extern void bcm_port_rlm_config_t_init(
    bcm_port_rlm_config_t *rlm_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set and get the configuration for Reduced Lane Mode (RLM) on a port. */
extern int bcm_port_rlm_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_rlm_config_t *rlm_config, 
    int enable);

/* Set and get the configuration for Reduced Lane Mode (RLM) on a port. */
extern int bcm_port_rlm_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_rlm_config_t *rlm_config, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_port_rlm_status_e {
    bcmPortRlmDisabled = _SHR_PORT_RLM_DISABLED, 
    bcmPortRlmBusy = _SHR_PORT_RLM_BUSY, 
    bcmPortRlmDone = _SHR_PORT_RLM_DONE, 
    bcmPortRlmFailed = _SHR_PORT_RLM_FAILED, 
    bcmPortRlmCount = _SHR_PORT_RLM_COUNT 
} bcm_port_rlm_status_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Check the Reduced Lane Mode (RLM) status of a port */
extern int bcm_port_rlm_status_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_rlm_status_t *rlm_status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE PHY info structure. */
typedef struct bcm_port_flexe_group_phy_info_s {
    bcm_port_t port;    /* Logical port ID for FlexE physical port */
    int logical_phy_id; /* Logical PHY ID for FlexE PHY */
} bcm_port_flexe_group_phy_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an FlexE group and add related PHYs. */
extern int bcm_port_flexe_group_create(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    int nof_pcs, 
    bcm_port_flexe_group_phy_info_t *phy_info);

/* Get FlexE group info. */
extern int bcm_port_flexe_group_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    int max_nof_pcs, 
    bcm_port_flexe_group_phy_info_t *phy_info, 
    int *actual_nof_pcs);

/* Add one FlexE physical port into one group. */
extern int bcm_port_flexe_group_phy_add(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_group_phy_info_t *phy_info);

/* Remove one FlexE physical port from one group. */
extern int bcm_port_flexe_group_phy_remove(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_group_phy_info_t *phy_info);

/* Destroy one FlexE group. */
extern int bcm_port_flexe_group_destroy(
    int unit, 
    bcm_gport_t gport);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE calendar types. */
typedef enum bcm_port_flexe_group_cal_e {
    bcmPortFlexeGroupCalA = 0,      /* FlexE calendar A */
    bcmPortFlexeGroupCalB = 1,      /* FlexE calendar B */
    bcmPortFlexeGroupCalCount = 2   /* FlexE calendar count */
} bcm_port_flexe_group_cal_t;

/* Port FlexE flags. */
#define BCM_PORT_FLEXE_RX       0x1        /* Indicate it is for FlexE RX
                                              direction only */
#define BCM_PORT_FLEXE_TX       0x2        /* Indicate it is for FlexE TX
                                              direction only */
#define BCM_PORT_FLEXE_OVERHEAD 0x4        /* Indicate it is for FlexE overhead
                                              only */
#define BCM_PORT_FLEXE_BYPASS   0x8        /* Indicate some FlexE logics are
                                              bypassed in FlexE core, such as
                                              demux */

#ifndef BCM_HIDE_DISPATCHABLE

/* FlexE group calendar slots configuration. */
extern int bcm_port_flexe_group_cal_slots_set(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_group_cal_t calendar_id, 
    int nof_slots, 
    int *calendar_slots);

/* GET FlexE group calendar slots configuration. */
extern int bcm_port_flexe_group_cal_slots_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_group_cal_t calendar_id, 
    int max_nof_slots, 
    int *calendar_slots, 
    int *actual_nof_slots);

/* Set the active calendar in FlexE Mux/Demux. */
extern int bcm_port_flexe_group_cal_active_set(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_group_cal_t active_cal);

/* Get the active calendar in FlexE Mux/Demux. */
extern int bcm_port_flexe_group_cal_active_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_group_cal_t *active_cal);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE Overhead types. */
typedef enum bcm_port_flexe_oh_type_e {
    bcmPortFlexeOhGroupID = 0,      /* Group ID in FlexE overhead */
    bcmPortFlexeOhLogicalPhyID = 1, /* Logical PHY ID in FlexE overhead */
    bcmPortFlexeOhCalInUse = 2,     /* Calendar in use bit in FlexE overhead */
    bcmPortFlexeOhCalRequest = 3,   /* Calendar request bit in FlexE overhead */
    bcmPortFlexeOhCalAck = 4,       /* Calendar ack bit in FlexE overhead */
    bcmPortFlexeOhSyncConfig = 5,   /* Synchronization configuration bit in
                                       FlexE overhead */
    bcmPortFlexeOhCount = 6         /* Number of overhead types */
} bcm_port_flexe_oh_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure the overhead according to the overhead type. */
extern int bcm_port_flexe_oh_set(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_oh_type_t type, 
    int val);

/* Get the overhead according to the overhead type. */
extern int bcm_port_flexe_oh_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_oh_type_t type, 
    int *val);

/* Configure FlexE traffic flow for single direction. */
extern int bcm_port_flexe_flow_set(
    int unit, 
    uint32 flags, 
    bcm_port_t src_port, 
    int channel, 
    bcm_port_t dest_port);

/* Clear FlexE traffic flow for single direction. */
extern int bcm_port_flexe_flow_clear(
    int unit, 
    bcm_port_t src_port, 
    int channel, 
    bcm_port_t dest_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE PHY info structure. */
typedef struct bcm_port_flexe_oh_alarm_s {
    uint8 alarm_active;                 /* Indicate if there is active alarm */
    uint8 phymap_mismatch;              /* Phymap mismatch alarm */
    bcm_port_flexe_time_slot_t cal_a_mismatch; /* The client ID in calendar A mismatch,
                                           return value is time slots */
    bcm_port_flexe_time_slot_t cal_b_mismatch; /* The client ID in calendar B mismatch,
                                           return value is time slots */
    uint8 group_deskew_alarm;           /* FlexE group deskew alarm */
    uint8 group_id_mismatch;            /* Group ID mismatch alarm */
    uint8 phy_id_mismatch;              /* Phy ID mismatch alarm */
    uint8 lost_of_frame;                /* This alarm will be raised if out of
                                           frame status lasts for a certain time */
    uint8 lost_of_multiframe;           /* This alarm will be raised if out of
                                           multiframe status lasts for a certain
                                           time */
    uint8 oh_block_1_alarm;             /* Error happens in the first overhead
                                           block */
    uint8 c_bit_alarm;                  /* The alarm for C bit in overhead */
    uint8 crc_error;                    /* There is CRC16 error in overhead */
    uint8 rpf_alarm;                    /* RPF alarm in overhead */
    uint8 sc_mismatch;                  /* Sync config mismatch alarm in
                                           overhead */
} bcm_port_flexe_oh_alarm_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get overhead alarms. */
extern int bcm_port_flexe_oh_alarm_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_port_flexe_oh_alarm_t *alarms);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE PHY info structure. */
typedef struct bcm_port_flexe_oam_alarm_s {
    uint8 alarm_active;             /* Indicate if there is active alarm */
    uint8 rx_base_csf_lpi;          /* Rx base OAM client signal LPI fault alarm */
    uint8 rx_base_cs_lf;            /* Rx base OAM client signal Local Fault
                                       alarm */
    uint8 rx_base_cs_rf;            /* Rx base OAM client signal Remote Fault
                                       alarm */
    uint8 base_oam_los;             /* Base OAM no receive alarm */
    uint8 rx_sdbip;                 /* Rx SDBIP alarm */
    uint8 rx_base_crc;              /* Rx OAM base CRC error alarm */
    uint8 rx_base_rdi;              /* Rx OAM base RDI alarm */
    uint8 rx_base_period_mismatch;  /* Rx base OAM period mismatch alarm */
    uint8 local_fault;              /* Local fault alarm in Rx direction */
    uint8 remote_fault;             /* Remote fault alarm in Rx direction */
    uint8 sdrei;                    /* Signal defect REI alarm */
    uint8 sfbip;                    /* Signal fail BIP alarm */
    uint8 sfrei;                    /* Signal fail REI alarm */
} bcm_port_flexe_oam_alarm_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get OAM alarms. */
extern int bcm_port_flexe_oam_alarm_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_port_flexe_oam_alarm_t *alarms);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE OAM control type */
typedef enum bcm_port_flexe_oam_control_type_e {
    bcmPortFlexeOamControlRxBasePeriod = 0, /* Rx OAM base period */
    bcmPortFlexeOamControlTxBasePeriod = 1, /* Tx OAM base period */
    bcmPortFlexeOamControlTxBaseEnable = 2, /* Enable the Tx base OAM */
    bcmPortFlexeOamControlRxBypassEnable = 3, /* Enable Rx OAM bypass */
    bcmPortFlexeOamControlTxBypassEnable = 4, /* Enable Tx OAM bypass */
    bcmPortFlexeOamControlInsertLocalFault = 5, /* Insert local fault to client signal */
    bcmPortFlexeOamControlInsertRemoteFault = 6, /* Insert remote fault to client signal */
    bcmPortFlexeOamControlSdBip8BlockNum = 7, /* Block number for SD BIP8 checking */
    bcmPortFlexeOamControlSdBip8SetThreshold = 8, /* Threshold for setting SD BIP8 */
    bcmPortFlexeOamControlSdBip8ClearThreshold = 9, /* Threshold for clearing SD BIP8 */
    bcmPortFlexeOamControlSfBip8BlockNum = 10, /* Block number for SF BIP8 checking */
    bcmPortFlexeOamControlSfBip8SetThreshold = 11, /* Threshold for setting SF BIP8 */
    bcmPortFlexeOamControlSfBip8ClearThreshold = 12, /* Threshold for clearing SF BIP8 */
    bcmPortFlexeOamControlSdBeiBlockNum = 13, /* Block number for SD Bei checking */
    bcmPortFlexeOamControlSdBeiSetThreshold = 14, /* Threshold for setting SD Bei */
    bcmPortFlexeOamControlSdBeiClearThreshold = 15, /* Threshold for clearing SD Bei */
    bcmPortFlexeOamControlSfBeiBlockNum = 16, /* Block number for SF Bei checking */
    bcmPortFlexeOamControlSfBeiSetThreshold = 17, /* Threshold for setting SF Bei */
    bcmPortFlexeOamControlSfBeiClearThreshold = 18, /* Threshold for clearing SF Bei */
    bcmPortSarOamControlIlkn2SarBypassEnable = 19, /* Enable SAR OAM bypass, direction:
                                           ILKN-to-SAR */
    bcmPortSarOamControlSar2IlknBypassEnable = 20 /* Enable SAR OAM bypass, direction:
                                           SAR-to-ILKN */
} bcm_port_flexe_oam_control_type_t;

/* FlexE OAM base period type */
typedef enum bcm_port_flexe_oam_base_period_e {
    bcmPortFlexeOamBasePeriod16K = 0,   /* OAM base period 16K */
    bcmPortFlexeOamBasePeriod32K = 1,   /* OAM base period 32K */
    bcmPortFlexeOamBasePeriod64K = 2,   /* OAM base period 64K */
    bcmPortFlexeOamBasePeriod512K = 3   /* OAM base period 512K */
} bcm_port_flexe_oam_base_period_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get FlexE OAM configurations. */
extern int bcm_port_flexe_oam_control_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_port_flexe_oam_control_type_t type, 
    uint32 *val);

/* Set FlexE OAM configurations. */
extern int bcm_port_flexe_oam_control_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_port_flexe_oam_control_type_t type, 
    uint32 val);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FlexE OAM base period type */
typedef enum bcm_port_flexe_oam_stat_e {
    bcmbcmPortFlexeOamStatBip8 = 0,     /* OAM BIP8 error counter */
    bcmPortFlexeOamStatBei = 1,         /* OAM BEI error counter */
    bcmPortFlexeOamPacketCount = 2,     /* OAM packet counter */
    bcmPortFlexeBaseOamPacketCount = 3  /* OAM base packet counter */
} bcm_port_flexe_oam_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get OAM stat counters. */
extern int bcm_port_flexe_oam_stat_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_port_flexe_oam_stat_t stat, 
    uint64 *val);

/* Get the rxcdr and txpi clock in Hz for given port. */
extern int bcm_port_txpi_rxcdr_clk_get(
    int unit, 
    bcm_port_t port, 
    uint32 *txpi_clk, 
    uint32 *rxcdr_clk);

#endif /* BCM_HIDE_DISPATCHABLE */

/* POST-FEC BER projection input */
typedef struct bcm_port_ber_proj_params_s {
    int hist_errcnt_thresh;         /* Histogram error count threshold */
    int interval_second;            /* Measurement time that PRBS errors are
                                       accumulated */
    bcm_port_phy_fec_t fec_type;    /* fec_type for port */
} bcm_port_ber_proj_params_t;

/* POST-FEC BER projection input */
typedef struct bcm_port_ber_proj_analyzer_errcnt_s {
    int start_hist_errcnt_thresh;       /* Histogram error count threshold */
    int max_errcnt_thresh;              /* PRBS error count threshold */
    uint32 proj_ber;                    /* projected BER */
    uint32 ber_proj_prbs_errcnt[16];    /* Array which stores prbs error count */
} bcm_port_ber_proj_analyzer_errcnt_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get POST FEC Projection info. */
extern int bcm_port_post_ber_proj_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_ber_proj_params_t *ber_proj, 
    int max_errcnt, 
    bcm_port_ber_proj_analyzer_errcnt_t *errcnt_array, 
    int *actual_errcnt);

/* Configure PCS lane map for the entire PM. */
extern int bcm_port_pcs_lane_map_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int num_map, 
    bcm_port_lane_to_serdes_map_t *serdes_map);

/* Get the L2 interface ID of a gport. */
extern int bcm_port_l2_interface_id_get(
    int unit, 
    bcm_gport_t port, 
    int *interface_id);

/* Get the GPORT of a L2 interface ID. */
extern int bcm_port_l2_interface_gport_get(
    int unit, 
    int interface_id, 
    bcm_gport_t *port);

/* Get the port bitmap of a given pipe. */
extern int bcm_port_pipe_pbmp_get(
    int unit, 
    int pipe, 
    bcm_pbmp_t *pbmp);

/* Get the port btimap of a given packet buffer. */
extern int bcm_port_buffer_pbmp_get(
    int unit, 
    int buffer_id, 
    bcm_pbmp_t *pbmp);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Source of egress VLAN priority. */
typedef enum bcm_port_egress_vlan_pri_src_e {
    bcmPortEgressVlanPriSrcIntPri = 0,  /* The source is the internal priority
                                           map. */
    bcmPortEgressVlanPriSrcPktPri = 1,  /* Keep the packet priority. */
    bcmPortEgressVlanPriSrcVpn = 2      /* The source is egress VPN or egress
                                           VLAN translation by looking up both
                                           VPN and port group (higher priority
                                           than egress VPN). */
} bcm_port_egress_vlan_pri_src_t;

/* Port group type. */
typedef enum bcm_port_group_type_e {
    bcmPortGroupEgressL2Interface = 0,  /* L2 OIF. */
    bcmPortGroupDvp = 1,                /* DVP. */
    bcmPortGroupCount = 2               /* Number of port group type. */
} bcm_port_group_type_t;

/* Port group range. */
typedef struct bcm_port_group_range_s {
    int min;    /* Lower limit of the range. */
    int max;    /* Upper limit of the range. */
} bcm_port_group_range_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get port group range based on port group type. */
extern int bcm_port_group_range_get(
    int unit, 
    bcm_port_group_type_t type, 
    bcm_port_group_range_t *range);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_port_fdr_symb_err_window_size_e {
    bcmPortFdrSymbErrWindowCw128 = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_128, 
    bcmPortFdrSymbErrWindowCw1K = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_1K, 
    bcmPortFdrSymbErrWindowCw8K = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_8K, 
    bcmPortFdrSymbErrWindowCw64K = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_64K, 
    bcmPortFdrSymbErrWindowCw512K = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_512K, 
    bcmPortFdrSymbErrWindowCw4M = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_4M, 
    bcmPortFdrSymbErrWindowCw32M = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_32M, 
    bcmPortFdrSymbErrWindowCw256M = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_256M, 
    bcmPortFdrSymbErrWindowCwCount = _SHR_PORT_FDR_SYMB_ERR_WINDOW_CW_COUNT 
} bcm_port_fdr_symb_err_window_size_t;

typedef struct bcm_port_fdr_config_s {
    int fdr_enable;                     /* enable Flight Data Recorder (FDR) */
    bcm_port_fdr_symb_err_window_size_t window_size; /* indicate a window size for FDR symbol
                                           error */
    uint32 symb_err_threshold;          /* indicate a threshold value for FDR
                                           symbol error */
    int symb_err_stats_sel;             /* To select the stats range of symbol
                                           errors in bcm_port_fdr_stats_t, in
                                           which cw_sN_errs indicates count of
                                           code words with S+N symbol errors.
                                           symb_err_stats_sel = 0 indicates S is
                                           0. symb_err_stats_sel = 1 indicates S
                                           = 9 or S = 8 based on the chip. The
                                           cw_s8_errs is 0 when the
                                           chip-dependent value of S is 8. */
    int intr_enable;                    /* enable Flight Data Recorder (FDR)
                                           interrupt */
} bcm_port_fdr_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the configuration for Flight Data Recorder (FDR) on a port. */
extern int bcm_port_fdr_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_fdr_config_t *fdr_config);

/* Get the configuration for Flight Data Recorder (FDR) on a port. */
extern int bcm_port_fdr_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_fdr_config_t *fdr_config);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_port_fdr_stats_s {
    uint64 start_time;          /* indicate the time when Flight Data Recorder
                                   (FDR) start to collect data */
    uint64 end_time;            /* indicate the time when the last statistics
                                   are collected */
    uint32 num_uncorrect_cws;   /* indicate the total number of uncorrectable
                                   code words */
    uint32 num_cws;             /* total number of code words used for the
                                   statistics collection */
    uint32 num_symb_errs;       /* total number of symbol errors detected */
    uint32 cw_s0_errs;          /* Number of Code Words with S + 0 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s1_errs;          /* Number of Code Words with S + 1 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s2_errs;          /* Number of Code Words with S + 2 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s3_errs;          /* Number of Code Words with S + 3 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s4_errs;          /* Number of Code Words with S + 4 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s5_errs;          /* Number of Code Words with S + 5 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s6_errs;          /* Number of Code Words with S + 6 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s7_errs;          /* Number of Code Words with S + 7 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
    uint32 cw_s8_errs;          /* Number of Code Words with S + 8 symbol errors
                                   where S is the selected value via
                                   symb_err_stats_sel in bcm_port_fdr_config_t.
                                   If symb_err_stats_sel is 0, S is 0. If
                                   symb_err_stats_sel is 1, S = 9 or S = 8 based
                                   on the chip. */
} bcm_port_fdr_stats_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the statistics of Flight Data Recorder (FDR) on a port */
extern int bcm_port_fdr_stats_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_fdr_stats_t *fdr_stats);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_port_fdr_handler_t */
typedef void (*bcm_port_fdr_handler_t)(
    int unit, 
    bcm_port_t port, 
    bcm_port_fdr_stats_t *fdr_stats, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Register port Flight Data Recorder (FDR) interrupt notification
 * callbacks
 */
extern int bcm_port_fdr_callback_register(
    int unit, 
    bcm_port_fdr_handler_t f, 
    void *user_data);

/* 
 * Unregister port Flight Data Recorder (FDR) interrupt notification
 * callbacks
 */
extern int bcm_port_fdr_callback_unregister(
    int unit, 
    bcm_port_fdr_handler_t f, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Bit error mask to generate FEC error. */
typedef struct bcm_port_phy_fec_error_mask_s {
    uint32 error_mask_bit_31_0;     /* Error mask bit 31-0. */
    uint32 error_mask_bit_63_32;    /* Error mask bit 63-32. */
    uint16 error_mask_bit_79_64;    /* Error mask bit 79-64. */
} bcm_port_phy_fec_error_mask_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set one-time fec error injection. */
extern int bcm_port_phy_fec_error_inject_set(
    int unit, 
    bcm_port_t port, 
    uint16 error_control_map, 
    bcm_port_phy_fec_error_mask_t bit_error_mask);

/* Get configuration of one-time fec error injection. */
extern int bcm_port_phy_fec_error_inject_get(
    int unit, 
    bcm_port_t port, 
    uint16 *error_control_map, 
    bcm_port_phy_fec_error_mask_t *bit_error_mask);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_PORT_H__ */
