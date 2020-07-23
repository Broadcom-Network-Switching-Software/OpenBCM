/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains PORT definitions internal to the BCM library.
 */

#ifndef _BCM_INT_PORT_H
#define _BCM_INT_PORT_H

#include <soc/ll.h>
#include <bcm_int/common/lock.h>
#include <bcm/port.h>
#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm/pkt.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#define BCM_PORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_PORT_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_PORT_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_PORT_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_PORT_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_PORT_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_PORT_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_PORT_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_PORT_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_PORT_WB_VERSION_1_9                SOC_SCACHE_VERSION(1,9)
#define BCM_PORT_WB_VERSION_1_10               SOC_SCACHE_VERSION(1,10)
#define BCM_PORT_WB_VERSION_1_11               SOC_SCACHE_VERSION(1,11)
#define BCM_PORT_WB_VERSION_1_12               SOC_SCACHE_VERSION(1,12)
#define BCM_PORT_WB_VERSION_1_13               SOC_SCACHE_VERSION(1,13)
#define BCM_PORT_WB_DEFAULT_VERSION            BCM_PORT_WB_VERSION_1_13

#define BCM_PORT_WB_SCACHE_PART_DEFAULT       (0)
#if defined(BCM_PREEMPTION_SUPPORT)
#define BCM_PORT_WB_SCACHE_PART_PREMMPTION    (1)
#define BCM_PORT_WB_SCACHE_PART_MIN  (BCM_PORT_WB_SCACHE_PART_DEFAULT)
#define BCM_PORT_WB_SCACHE_PART_MAX  (BCM_PORT_WB_SCACHE_PART_PREMMPTION)
#else
#define BCM_PORT_WB_SCACHE_PART_MIN  (BCM_PORT_WB_SCACHE_PART_DEFAULT)
#define BCM_PORT_WB_SCACHE_PART_MAX  (BCM_PORT_WB_SCACHE_PART_DEFAULT)
#endif
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Hardware values for different redirection configuration */
#define _BCM_REDIRECT_DEST_TYPE_NONE  0x0
#define _BCM_REDIRECT_DEST_TYPE_MTP   0x4
#define _BCM_REDIRECT_DEST_TYPE_PORT  0x5
#define _BCM_REDIRECT_DEST_TYPE_TRUNK 0x6
#define _BCM_REDIRECT_DEST_TYPE_MCAST 0x7

#define _BCM_REDIRECT_NONE            0x0
#define _BCM_REDIRECT_IF_NOT_DROPPED  0x1
#define _BCM_REDIRECT_IF_DROPPED      0x2
#define _BCM_REDIRECT_ALWAYS          0x3

#define _BCM_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION  0x1
#define _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE       0x2

#define _BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW    0x0
#define _BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM 0x1
#define _BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH   0x2

#define BCMI_MOD_PORT_MODID_GET(mod_port, modid) \
                  ((modid) = ((mod_port) >> 8) & 0xFF)

#define BCMI_MOD_PORT_PORTID_GET(mod_port, portid) \
                  ((portid) = (mod_port) & 0xFF)

#define BCMI_MOD_PORT_SET(mod_port, modid, portid) \
                  ((mod_port) = ((((modid) & 0xFF) << 8) | \
                                   ((portid) & 0xFF)))

/*
 * Port Function Vector Driver
 */
typedef struct bcm_esw_port_drv_s {
    /* Initalize Port Function Vector driver */
    int (*fn_drv_init)(int unit);

    /*
     * Port Resource APIs (FlexPort)
     */
    /* Modify the Port Resources (e.g. speed, lanes, etc.) for a port */
    int (*resource_set)(int unit, bcm_gport_t port,
                        bcm_port_resource_t *resource);
    /* Get Port Resource configuration for a port */
    int (*resource_get)(int unit, bcm_gport_t port,
                        bcm_port_resource_t *resource);
    /* Modify the Port Resources (e.g. speed, lanes, etc.) for a set of ports */
    int (*resource_multi_set)(int unit, int nport,
                              bcm_port_resource_t *resource);
    /* Modify the Port speed only, keep same number of lanes */
    int (*resource_speed_set)(int unit, bcm_port_t port, int speed);

    /* Iterate and get the Port Resource configuration for all ports */
    int (*resource_traverse)(int unit, bcm_port_resource_traverse_cb trav_fn,
                             void *user_data);
    /* Set the port redirection configuration */
    int (*port_redirect_set)(int unit, bcm_port_t port,
                          bcm_port_redirect_config_t *redirect_config);
    /* Get the port redirection configuration */
    int (*port_redirect_get)(int unit, bcm_port_t port,
                          bcm_port_redirect_config_t *redirect_config);

    /* Enable or disable a port */
    int (*port_enable_set)(int unit, bcm_port_t port, int enable);
    /* Check if the speed and encap mode match */
    int (*encap_speed_check)(int unit, bcm_port_t port, int encap, int speed);
    /* Set force loopback */
    int (*force_lb_set)(int unit);
    /* Check if port should be forced loopback */
    int (*force_lb_check)(int unit, int port, int loopback);
    /* Update port resources operational status after flex operation. */
    int (*resource_status_update)(int unit, int nport, bcm_port_resource_t *resource);

    /* Set the Speed information for several ports */
    int (*port_resource_speed_multi_set)(int unit, int nport,
            bcm_port_resource_t *resource);

    /* Check if the speed and lanes are supported on the device */
    int (*device_speed_check)(int unit, int speed, int lanes);
} bcm_esw_port_drv_t;

extern bcm_esw_port_drv_t      *bcm_esw_port_drv[BCM_MAX_NUM_UNITS];

#define BCM_ESW_PORT_DRV(_u)   (bcm_esw_port_drv[(_u)])



#define  BCM_PORT_L3_V4_ENABLE     (1 << 0)    /* Enable L3 IPv4 switching. */
#define  BCM_PORT_L3_V6_ENABLE     (1 << 1)    /* Enable L3 IPv6 switching. */

#define  BCM_PORT_JUMBO_MAXSZ    0x3FE8

#define BCM_EGR_PORT_TYPE_ETHERNET      0x0
#define BCM_EGR_PORT_TYPE_HIGIG         0x1
#define BCM_EGR_PORT_TYPE_LB            0x2
#define BCM_EGR_PORT_TYPE_EHG           0x3

#define BCM_EGR_PORT_HIGIG_PLUS         0x0
#define BCM_EGR_PORT_HIGIG2             0x1

/* For MEMORY PORT_TABLE.HG_TYPE EGR_PORT.HG_TYPE, EGR_ING_PORT.HG_TYPE */
#define BCM_PORT_HG_TYPE_HIGIGPLUS          0x00
#define BCM_PORT_HG_TYPE_HIGIG2_NORMAL      0x08
#define BCM_PORT_HG_TYPE_HGOE_TRANSPORT     0x09
#define BCM_PORT_HG_TYPE_HGOE_RAW           0x0A

#define _BCM_PORT_EHG_L2_HEADER_BUFFER_SZ       8    /* 24 bytes  = 6 words + 2 words padding*/
#define _BCM_PORT_EHG_IP_GRE_HEADER_BUFFER_SZ   12    /* 48 bytes  = 12 words */

#define _BCM_CPU_TABS_NONE        0
#define _BCM_CPU_TABS_ETHER     0x1
#define _BCM_CPU_TABS_HIGIG     0x2
#define _BCM_CPU_TABS_BOTH      0x3

#define _BCM_PORT_RATE_PPS_MODE  1  /* Rate Limiting in packet mode */
#define _BCM_PORT_RATE_BYTE_MODE 0  /* Rate Limiting in byte mode   */
#define _BCM_PORT_RATE_MIN_KILOBITS  8
#define _BCM_PORT_RATE_MAX_KILOBITS  106*1000*1000
#define _BCM_PORT_BURST_MIN_KILOBITS  2
#define _BCM_PORT_BURST_MAX_KILOBITS  1000*1000
/*
 * Define:
 *     LPORT_PROFILE_LPORT_TAB/LPORT_PROFILE_RTAG7_TAB
 * Purpose:
 *     Table indexes for LPORT Profile Table.
 */
#define LPORT_PROFILE_LPORT_TAB 0
#define LPORT_PROFILE_RTAG7_TAB 1

/*
 * Define:
 *      _BCM_PORT_VD_PBVL_ESIZE
 * Purpose:
 *      Number of bits per entry in vd_pbvl bitmap
 */

#define _BCM_PORT_VD_PBVL_ESIZE      8

/*
 * Define:
 *      _BCM_PORT_VD_PBVL_SET, _BCM_PORT_VD_PBVL_CLEAR
 * Purpose:
 *      Set/clear bit in vp_pbvl bitmap for unit, port, index
 */
#define _BCM_PORT_VD_PBVL_SET(_pinfo, _idx) \
        (_pinfo)->p_vd_pbvl[(_idx) / _BCM_PORT_VD_PBVL_ESIZE] |= \
                (1 << ((_idx) % _BCM_PORT_VD_PBVL_ESIZE))

#define _BCM_PORT_VD_PBVL_CLEAR(_pinfo, _idx) \
        (_pinfo)->p_vd_pbvl[(_idx)/ _BCM_PORT_VD_PBVL_ESIZE] &= \
                ~(1 << ((_idx) % _BCM_PORT_VD_PBVL_ESIZE))

/*
 * Define:
 *      PORT_VD_PBVL_IS_SET
 * Purpose:
 *      Test if bit in vp_pbvl bitmap is set for unit, port, index
 */
#define _BCM_PORT_VD_PBVL_IS_SET(_pinfo, _idx) \
        (((_pinfo)->p_vd_pbvl[(_idx) / _BCM_PORT_VD_PBVL_ESIZE] \
                >> ((_idx) % _BCM_PORT_VD_PBVL_ESIZE)) & 1)

#define _PORT_INFO_STOP_TX              1
#define _PORT_INFO_PROBE_RESET_SERDES   2

/*
 * Define:
 *      PORT_LOCK/PORT_UNLOCK
 * Purpose:
 *      Serialization Macros.
 *      Here the cmic memory lock also serves to protect the
 *      bcm_port_info structure and EPC_PFM registers.
 */

#define PORT_LOCK(unit) \
        BCM_LOCK(unit); \
        if (soc_mem_is_valid(unit, PORT_TABm)) \
        { soc_mem_lock(unit, PORT_TABm); }


#define PORT_UNLOCK(unit) \
        BCM_UNLOCK(unit); \
        if (soc_mem_is_valid(unit, PORT_TABm)) \
        { soc_mem_unlock(unit, PORT_TABm); }

typedef struct _bcm_port_metering_info_s {
    uint32 refresh;
    uint32 threshold;
    uint32 bucket;
    uint8  in_profile;
} _bcm_port_metering_info_t;

/*
 * The port structure and hardware PTABLE are protected by the PORT_LOCK.
 */
typedef struct _bcm_port_info_s {
    mac_driver_t       *p_mac;          /* Per port MAC driver */
    int                 p_ut_prio;      /* Untagged priority   */
    uint8               *p_vd_pbvl;     /* Proto-based VLAN_DATA state */
    int                 dtag_mode;      /* Double-tag mode */
    int                 vlan_prot_ptr;  /* VLAN_PROTOCOL_DATA index */
    int                 vp_count;       /* Number of VPs on a physical port */
    bcm_port_congestion_config_t *e2ecc_config;
                                        /* End-to-end congestion control configuration */
    uint32              cosmask;        /* Cached h/w value on a port stop */
    _bcm_port_metering_info_t m_info;   /* Cached h/w value on a port stop */
    uint8               flags;          /* Bits to indicate port stop etc */
    uint8               rx_los;         /* Cached PHY serdes rx_los status */
    int                 enable;         /* Cached port enable status */
    bcm_port_if_t       intf;           /* Cached port interface */
    int                 link_delay_offset; /* link delay offset */
    int                 port_dscp_map_set; /* dscp map set */
} _bcm_port_info_t;

/*
 * Internal port configuration enum.
 * Used internally by sdk to set port configuration from
 */
typedef enum _bcm_port_config_e {
   _bcmPortL3UrpfMode,          /* L3 unicast packet RPF check mode.  */
   _bcmPortL3UrpfDefaultRoute,  /* UPRF check accept default gateway. */
   _bcmPortVlanTranslate,       /* Enable Vlan translation on the port. */
   _bcmPortVlanPrecedence,      /* Set Vlan lookup precedence between
                                 * IP table and MAC table             */
   _bcmPortVTMissDrop,          /* Drop if VLAN translate miss.       */
   _bcmPortLookupMACEnable,     /* Enable MAC-based VLAN              */
   _bcmPortLookupIPEnable,      /* Enable subnet-based VLAN           */
   _bcmPortUseInnerPri,         /* Trust the inner vlan tag PRI bits  */
   _bcmPortUseOuterPri,         /* Trust the outer vlan tag PRI bits  */
   _bcmPortVerifyOuterTpid,     /* Verify outer TPID against VLAN TPID*/
   _bcmPortVTKeyTypeFirst,      /* First Vlan Translation Key Type    */
   _bcmPortVTKeyPortFirst,      /* First Per-port VT Key Type         */
   _bcmPortVTKeyTypeSecond,     /* Second Vlan Translation Key Type   */
   _bcmPortVTKeyPortSecond,     /* Second Per-port VT Key Type        */
   _bcmPortIpmcV4Enable,        /* Enable IPv4 IPMC processing        */
   _bcmPortIpmcV6Enable,        /* Enable IPv4 IPMC processing        */
   _bcmPortIpmcVlanKey,         /* Include VLAN in L3 host key for IPMC */
   _bcmPortCfiAsCng,            /* Configure CFI to CNG mapping       */
   _bcmPortNni,                 /* Set port as NNI                    */
   _bcmPortHigigTrunkId,        /* Higig trunk, -1 to disable         */
   _bcmPortModuleLoopback,      /* Allow loopback to same modid       */
   _bcmPortOuterTpidEnables,    /* Bitmap of outer TPID valid bits    */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
   _bcmPortSvcMeterIndex,       /* Service Meter(global meter) index  */
   _bcmPortSvcMeterOffsetMode,  /* Service Meter offset mode          */
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
   _bcmPortOamUseXlatedInnerVlan, /* Use Xlated C vlan for OAM lookup  */
#endif
   _bcmPortCount                /* Always last max value of the enum. */
} _bcm_port_config_t;

typedef enum bcmi_port_mac_type_e {
    bcmiPortMacTypePortmode = 0,
    bcmiPortMacTypeUnimac = 1,
    bcmiPortMacTypeXLmac = 2,
    bcmiPortMacTypeCLmac = 3,
    bcmiPortMacTypeCounts = 4
} bcmi_port_mac_type_t;

/*
 * Port configuration structure.
 * An abstraction of data in the PTABLE (strata) and other places.
 */

typedef struct bcm_port_cfg_s {
    int		pc_frame_type;
    int		pc_ether_type;

    int		pc_stp_state;	  /* spanning tree state of port */
    int		pc_cpu;		  /* CPU learning */
    int		pc_disc;	  /* discard state */
    int		pc_bpdu_disable;  /* Where is this in Draco? */
    int		pc_trunk;	  /* trunking on for this port */
    int		pc_tgid;	  /* trunk group id */
    int		pc_mirror_ing;	  /* mirror on ingress */
    int		pc_ptype;	  /* port type */
    int		pc_jumbo;
    int		pc_cml;		  /* CML bits */
    int     pc_cml_move;  /* CML move bits on supporting devices */

    bcm_pbmp_t	pc_pbm;		  /* port bitmaps for port based vlan */
    bcm_pbmp_t	pc_ut_pbm;
    bcm_vlan_t	pc_vlan;	  /* port based vlan tag */
    bcm_vlan_t	pc_ivlan;	  /* port based inner-tag vlan tag */
    int         pc_vlan_action;   /* port based vlan action profile pointer */

#if defined(BCM_STRATA2_SUPPORT)
    int		pc_stg;		  /* stg value */
    int		pc_rsvd1;	  /* Reserved field (must be ones) */
#endif

    int		pc_l3_flags;	  /* L3 flags. */

#if defined(BCM_STRATA2_SUPPORT) || defined(BCM_XGS_SWITCH_SUPPORT)
    int		pc_remap_pri_en;  /* remap priority enable */
#endif

    int	        pc_new_opri;      /* new outer packet priority */
    int	        pc_new_ocfi;      /* new outer cfi */
    int	        pc_new_ipri;      /* new inner packet priority */
    int	        pc_new_icfi;      /* new inner cfi */

    int		pc_dse_mode;	  /* DSCP mapping (off, or on/mode) */
    int		pc_dse_mode_ipv6;	  /* DSCP mapping for IPv6 (off, or on/mode) */
    int		pc_dscp;	  /* Resultant diffserv code point */

#if defined(BCM_XGS_SWITCH_SUPPORT)
    int         pc_en_ifilter;    /* Enable Ingress Filtering */
    int         pc_pfm;           /* In the port table for Draco */
    int         pc_dscp_prio;     /* For Draco15 & Tucana */
    int         pc_bridge_port;   /* FB/ER, allows egress=ingress */
    int         pc_nni_port;      /* FB, indicates non-customer port */
#endif

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int     pc_urpf_mode;         /* Unicast rpf lookup mode.      */
    int     pc_urpf_def_gw_check; /* Unicast rpf check default gw. */
    int     pc_pvlan_enable;      /* Private (force) vlan enable */
#endif

} bcm_port_cfg_t;

typedef struct _bcm_gport_dest_s {
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             mpls_id;
    int             mim_id;
    int             wlan_id;
    int             trill_id;
    int             l2gre_id;
    int             vxlan_id;
    int             flow_id;
    int             vlan_vp_id;
    int             niv_id;
    int             extender_id;
    int             subport_id;
    int             scheduker_id;
    uint32          gport_type;
} _bcm_gport_dest_t;

typedef struct _bcm_port_egr_dest_s {
    bcm_port_t      in_port;
    bcm_module_t    in_modid;
    bcm_port_t      out_min_port;
    bcm_module_t    out_min_modid;
    bcm_port_t      out_max_port;
    bcm_module_t    out_max_modid;
} _bcm_port_egr_dest_t;

#ifdef BCM_TRIUMPH3_SUPPORT
typedef struct _port_ibod_ctrl_s
{
    char taskname[16];
    sal_sem_t sema;
    int interval;
    int running;
    int stop;
    int unit;
    uint64 rrpkt[SOC_MAX_NUM_PORTS];
    uint64 rrbyt[SOC_MAX_NUM_PORTS];
    uint64 rrpkt_old[SOC_MAX_NUM_PORTS];
    uint64 rrbyt_old[SOC_MAX_NUM_PORTS];
    uint64 txerrpkt[SOC_MAX_NUM_PORTS];
    uint64 tuflpkt[SOC_MAX_NUM_PORTS];
    bcm_pkt_t pkt;
    sal_usecs_t start_time;
    uint32 event_count;
    uint32 port_avg_time;
    uint32 txerr_count;
    uint32 txerr_avg_time;
    uint32 mmu_war_count;
    uint32 mmu_avg_time;
    int drain_timeout_flag;
    int port_recovery_failed_flag;
    uint32 egress_drain_timeout_count;
    uint32 drain_timeout_count;
    uint32 port_recovery_failed_count;
} _port_ibod_ctrl_t;
#endif

enum NIV_VNTAG_ACTIONS {
    VNTAG_NOOP = 0,
    VNTAG_ADD = 1,
    VNTAG_REPLACE = 2,
    VNTAG_DELETE = 3,
    VNTAG_ACTION_COUNT
};

/*
 * Datastructure used to store the Port Force Forward
 * information. This is when the port flaps.
 */
typedef struct _bcm_port_force_forward_linkstate_{
    uint8 num_pbmp_entry;
    uint8 lock;
    bcm_pbmp_t *sw_disable_pbmp_linkstate;
} _bcm_port_force_forward_linkstate_t;

#ifdef BCM_TRIUMPH3_SUPPORT
#define _PORT_IBOD_FLAG_MMU_TRIGGER           1

extern void _bcm_esw_ibod_debug_flag_set(int unit, int flag);
extern int _bcm_esw_ibod_sync_recovery_start(int unit, sal_usecs_t interval);
extern int _bcm_esw_ibod_sync_recovery_stop(int unit);
extern int _bcm_esw_ibod_sync_recovery_running(int unit, _port_ibod_ctrl_t *ibod_ctrl);
#endif

#ifdef BCM_HIGIG2_SUPPORT
extern int _bcm_esw_port_higig2_mode_set(int unit, bcm_port_t port, int higig2_mode);
#endif

extern int _bcm_port_encap_xport_set(int unit, bcm_port_t port, int mode,
                                     int update_port_mode);
extern int _bcm_esw_port_mon_start(int unit);
extern int _bcm_esw_port_mon_stop(int unit);
extern int _bcm_esw_port_deinit(int unit);
extern int _bcm_esw_port_portmod_thread_stop(int unit);
extern int _bcm_port_probe(int unit, bcm_port_t p, int *okay);
extern int bcm_port_settings_init(int unit, bcm_port_t p);
extern int _bcm_port_mirror_enable_set(int unit, bcm_port_t port,
                                       int enable);
extern int _bcm_port_mirror_enable_get(int unit, bcm_port_t port,
                                       int *enable);
extern int _bcm_port_mirror_egress_true_enable_set(int unit, bcm_port_t port,
                                                   int enable);
extern int _bcm_port_mirror_egress_true_enable_get(int unit, bcm_port_t port,
                                                   int *enable);
extern int _bcm_port_link_get(int unit, bcm_port_t port, int hw, int *link);
extern int _bcm_port_pd10_handler(int unit, bcm_port_t port, int link_up);
extern int _bcm_port_untagged_vlan_get(int unit, bcm_port_t port,
				bcm_vlan_t *vid_ptr);
extern int _bcm_esw_port_config_set(int unit, bcm_port_t port,
                                    _bcm_port_config_t type, int value);
extern int _bcm_esw_port_config_get(int unit, bcm_port_t port,
                                    _bcm_port_config_t type, int *value);
extern void _bcm_gport_dest_t_init(_bcm_gport_dest_t *gport_dest);
extern int _bcm_gport_modport_hw2api_map(int, bcm_module_t, bcm_port_t,
                                         bcm_module_t *, bcm_port_t *);
extern int _bcm_esw_gport_construct(int unit, _bcm_gport_dest_t *gport_dest,
                                    bcm_gport_t *gport);
extern int _bcm_esw_gport_resolve(int unit, bcm_gport_t gport,
                                  bcm_module_t *modid, bcm_port_t *port,
                                  bcm_trunk_t *trunk_id, int *id);
extern int _bcm_esw_port_is_local_subport(int unit, bcm_gport_t gport,
                                          bcm_module_t modid, bcm_port_t port,
                                          int *is_local_subport,
                                          bcm_port_t *phy_port);
extern bcm_gport_t _bcm_esw_port_gport_get_from_modport(int unit, bcm_module_t modid,
                                                        bcm_port_t port);
extern int _bcm_esw_modid_is_local(int unit, bcm_module_t modid, int *result);
extern int _bcm_esw_port_link_delay_update(int unit, bcm_port_t port, int speed);
extern int _bcm_port_info_get(int unit, bcm_port_t port, _bcm_port_info_t **pinfo);
extern int _bcm_esw_port_gport_validate(int unit, bcm_port_t port_in,
                                        bcm_port_t *port_out);
extern int _bcm_esw_port_gport_phyn_validate(int unit, bcm_port_t port,
                                             bcm_port_t *local_port, int *phyn,
                                             int *phy_lane, int *sys_side);
extern int _bcm_port_encap_stport_set(int unit, bcm_port_t port, int mode);
extern int _bcm_port_ehg_header_write(int, bcm_port_t, uint32 *, uint32 *, uint32 *,
                                          int);
extern int _bcm_port_mode_setup(int unit, bcm_port_t port, int enable);
extern void _bcm_port_info_access(int unit, bcm_port_t port,
                                  _bcm_port_info_t **info);
extern unsigned _bcm_esw_valid_flex_port_controlling_port(int unit, bcm_port_t port);
extern int
_bcm_esw_port_tab_multi_set(int unit, bcm_port_t port, int cpu_tabs,
                            int field_count, soc_field_t *fields,
                            uint32 *values);
extern int _bcm_esw_port_tab_set(int unit, bcm_port_t port, int cpu_tabs,
                      soc_field_t field, int value);
extern int _bcm_esw_port_tab_set_without_portlock(int unit, bcm_port_t port,
                                                  int cpu_tabs,
                                                  soc_field_t field, int value);
extern int _bcm_esw_port_tab_get(int unit, bcm_port_t port,
                      soc_field_t field, int *value);
extern int _bcm_esw_egr_port_tab_set(int unit, bcm_port_t port,
                          soc_field_t field, uint32 value);
extern int _bcm_esw_egr_port_tab_get(int unit, bcm_port_t port,
                          soc_field_t field, int *value);


#define _BCM_ESW_PT_PORT        0
#define _BCM_ESW_PT_COE         1
#define _BCM_ESW_PT_HGPROXY     2
#define _BCM_ESW_PT_MACSEC_PORT 3

extern int bcm_esw_port_lport_prof_src_check(int unit, int port_type, int *);

extern int
bcm_esw_port_lport_fields_set(int unit, bcm_port_t port,
                              int table_id, int field_count,
                              soc_field_t *fields, uint32 *values);

extern int
bcm_esw_port_lport_fields_get(int unit, bcm_port_t port,
                              int table_id, int field_count,
                              soc_field_t *fields, uint32 *values);
extern int
bcm_esw_port_lport_field_set(int unit, bcm_port_t port,
                             int table_id, soc_field_t field, uint32 value);

extern int
bcm_esw_port_lport_field_get(int unit, bcm_port_t port,
                             int table_id, soc_field_t field, uint32 *value);
extern int
_bcm_esw_egr_port_tab_multi_set(int unit, bcm_port_t port, int field_count,
        soc_field_t *fields, uint32 *values);

extern int
bcm_esw_port_egr_lport_fields_set(int unit, bcm_port_t port,
                              soc_mem_t mem, int field_count,
                              soc_field_t *fields, uint32 *values);

extern int
bcm_esw_port_egr_lport_fields_clear(int unit, bcm_port_t port,
                                    soc_mem_t mem);
extern int
bcm_esw_port_egr_lport_fields_get(int unit, bcm_port_t port,
                              soc_mem_t mem, int field_count,
                              soc_field_t *fields, uint32 *values);

extern int
bcm_esw_port_egr_lport_field_set(int unit, bcm_port_t port,
                             soc_mem_t mem, soc_field_t field, uint32 value);

extern int
bcm_esw_port_egr_lport_field_get(int unit, bcm_port_t port,
                             soc_mem_t mem, soc_field_t field, uint32 *value);

extern int
_bcm_esw_link_force_for_disabled_loopback_port(int unit);

extern int _bcm_esw_port_mac_failover_notify(int,bcm_port_t);
extern int _bcm_esw_port_stat_get(int unit, int sync_mode,
                                  bcm_gport_t port, bcm_port_stat_t stat,
                                  uint64 *val);
extern int _bcm_esw_port_sw_info_display(int unit, bcm_port_t port);
extern int _bcm_esw_port_hw_info_display(int unit, bcm_port_t port);
extern void
_bcm_port_force_forward_linkscan_handler(int unit, bcm_port_t port,
                               bcm_port_info_t *info);

#if defined(INCLUDE_L3)
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
extern int _bcm_esw_port_ecmp_dlb_link_status_set(int unit, bcm_port_t port,
                                                  int status);
extern int _bcm_esw_port_ecmp_dlb_link_status_get(int unit, bcm_port_t port,
                                                  int *status);
#endif /* (BCM_TOMAHAWK2_SUPPORT) || (BCM_TRIDENT3_SUPPORT) */
#endif /* INCLUDE_L3 */

extern int
_bcm_esw_port_ingress_dest_enable(int unit, bcm_port_t port, int enable);
#if defined(BCM_SCORPION_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
extern int
_bcm_sc_tr_port_pfc_tx_set(int unit, bcm_port_t port, uint32 pri_bmp);
#endif

extern void _bcm_esw_port_mirror_lock(int unit);
extern void _bcm_esw_port_mirror_unlock(int unit);
extern int _bcm_esw_glp_construct(int unit, bcm_trunk_t trunk_id,
                                  bcm_module_t modid, bcm_port_t port,
                                  bcm_gport_t *glport);
extern int _bcm_esw_glp_resolve(int unit, bcm_trunk_t *is_trunk,
                                bcm_module_t *modid, bcm_port_t *port,
                                bcm_gport_t glport);
extern int _bcm_esw_gport_to_glp(int unit, bcm_gport_t gport,
                                 bcm_gport_t *glp);
extern int
bcmi_esw_port_eee_cfg_set(int unit, bcm_port_t port, int value);

extern int
bcmi_esw_port_egr_prof_ptr_set(int unit, bcm_port_t port, int value);

extern int bcmi_esw_port_mac_type_get(int unit, bcm_port_t lport,
                                      bcmi_port_mac_type_t *type);
extern int bcmi_esw_port_encap_set(int unit, bcm_port_t port, int mode);

extern int bcmi_port_force_lb_set(int unit);

#ifdef BCM_KATANA2_SUPPORT
void bcm_esw_kt2_port_lock(int unit);
void bcm_esw_kt2_port_unlock(int unit);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_port_sync(int unit);
extern int _bcm_port_cleanup(int unit);
#else
#define _bcm_port_cleanup(u)        (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_esw_flexport_sw_dump(int unit);
extern void _bcm_esw_flexport_sw_pipe_speed_group_dump(
        int unit,
        int pipe,
        int speed_id);
extern void _bcm_port_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#ifdef BCM_TRIUMPH3_SUPPORT
#define IBOD_SYNC_INTERVAL_MIN         100000

extern int _bcm_tr3_port_ur_chk(int unit, int num_ports, bcm_port_t port);
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int
bcm_esw_port_asf_mode_get(
    int           unit,
    bcm_port_t    port,
    int* const    mode);

extern int
bcm_esw_port_asf_mode_set(
    int           unit,
    bcm_port_t    port,
    int           mode);

extern int
bcm_esw_port_asf_show(
    int          unit,
    bcm_port_t   port);

extern int
bcm_esw_asf_diag(int unit);

extern int
bcm_esw_port_asf_diag(
    int          unit,
    bcm_port_t   port);
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
extern int
bcmi_gh2_port_e2efc_remote_module_enable_set(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);
extern int
bcmi_gh2_port_e2efc_remote_module_enable_get(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int *enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);
extern int
bcmi_gh2_port_e2efc_remote_port_init(int unit);
extern int
bcmi_gh2_port_e2efc_remote_port_add(
    int unit,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg,
    int *rport_handle_id);
extern int
bcmi_gh2_port_e2efc_remote_port_set(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);
extern int
bcmi_gh2_port_e2efc_remote_port_get(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);
extern int
bcmi_gh2_port_e2efc_remote_port_delete(
    int unit,
    int rport_handle_id);
extern int
bcmi_gh2_port_e2efc_remote_module_traverse(
    int unit,
    bcm_port_e2efc_remote_module_traverse_cb cb,
    void *user_data);
extern int
bcmi_gh2_port_e2efc_remote_port_traverse(
    int unit,
    bcm_port_e2efc_remote_port_traverse_cb cb,
    void *user_data);
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
extern int
bcmi_kt2_port_e2efc_remote_module_enable_set(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);
extern int
bcmi_kt2_port_e2efc_remote_module_enable_get(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int *enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg);
extern int
bcmi_kt2_port_e2efc_remote_port_init(int unit);
extern int
bcmi_kt2_port_e2efc_remote_port_add(
    int unit,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg,
    int *rport_handle_id);
extern int
bcmi_kt2_port_e2efc_remote_port_set(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);
extern int
bcmi_kt2_port_e2efc_remote_port_get(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg);
extern int
bcmi_kt2_port_e2efc_remote_port_delete(
    int unit,
    int rport_handle_id);
extern int
bcmi_kt2_port_e2efc_remote_module_traverse(
    int unit,
    bcm_port_e2efc_remote_module_traverse_cb cb,
    void *user_data);
extern int
bcmi_kt2_port_e2efc_remote_port_traverse(
    int unit,
    bcm_port_e2efc_remote_port_traverse_cb cb,
    void *user_data);
#endif /* BCM_KATANA2_SUPPORT */
#ifdef SW_AUTONEG_SUPPORT
extern int
_bcm_esw_port_notify_link_down_evt(int unit, bcm_port_t port);
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
extern int
_bcm_esw_is_multi_nexthop_mod_port(int unit, bcm_module_t modid,
                                   bcm_port_t port);
extern int
_bcm_esw_modport_local_get(int unit, bcm_gport_t modport,
                           bcm_port_t *local_port);
#endif

extern int
_bcm_esw_port_vp_gport_check(bcm_gport_t port, int vp);
extern int
bcm_esw_roe_tx_port_update(int unit, bcm_port_t port, int link);
extern int
bcm_esw_roe_rx_port_update(int unit, bcm_port_t port, int link);

#endif	/* !_BCM_INT_PORT_H */
