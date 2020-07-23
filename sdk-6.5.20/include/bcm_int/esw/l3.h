/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains L3 definitions internal to the BCM library.
 */

#ifndef _BCM_INT_L3_H
#define _BCM_INT_L3_H

#include <shared/bitop.h>

#define INVALID_IP_ADDR		0xffffffff

#define BCM_L3_IP6_MAX_NETLEN       128
#define BCM_L3_IP6_MAX_NETLEN_FBER  64
#define BCM_L3_IP4_MAX_NETLEN       32



#define _BCM_DEFIP_IPMC_EXPECTED_L3IIF_MAX   \
    (SOC_IS_MONTEREY(unit) ? 0x3FF : \
     (SOC_IS_HURRICANE4(unit) ? 0x3FF : (SOC_IS_HELIX5(unit) ? 0xFFF : 0x2FFF)))
#define _BCM_DEFIP_IPMC_RP_ID_BASE           \
    (SOC_IS_MONTEREY(unit) ? 0x400 : \
     (SOC_IS_HURRICANE4(unit) ? 0x400 : (SOC_IS_HELIX5(unit) ? 0x1000 : 0x3000)))
#define _BCM_DEFIP_IPMC_RP_SET(rp)           \
    (soc_feature(unit, soc_feature_l3defip_rp_l3iif_resolve) ? (rp) : \
     ((rp) | _BCM_DEFIP_IPMC_RP_ID_BASE))
#define _BCM_DEFIP_IPMC_RP_GET(rp)           (rp & (~ _BCM_DEFIP_IPMC_RP_ID_BASE))
#define _BCM_DEFIP_IPMC_RP_IS_SET(val)       ((val) > _BCM_DEFIP_IPMC_EXPECTED_L3IIF_MAX)

extern void _bcm_xgs3_nh_ref_cnt_incr(int unit, unsigned idx);
extern void _bcm_xgs3_nh_ref_cnt_get(int unit, uint32 idx, int ecmp,
                                     uint32 *ref_count);
extern int _bcm_xgs3_ecmp_max_grp_size_get(int unit, int ecmp_group_id,
                                           int *grp_sz);
extern int _bcm_esw_l3_egress_reference_get(int unit, bcm_if_t mpintf,
                                            int ecmp, uint32 *ref_count);
extern int _bcm_esw_l3_egress_ref_dlb_update(int unit, int index,
                                            uint32 * ref_count);
extern int bcmi_esw_l3_egress_ref_count_update(int unit, bcm_if_t intf,
                                    uint32 flags, int nh_ecmp_id, int incr);
extern int _bcm_esw_l3_gport_construct(int unit, bcm_port_t port,
        bcm_module_t modid, bcm_trunk_t tgid, uint32 flags, bcm_port_t *gport);

extern void bcmi_esw_l3_dependent_cleanup(int unit);
extern int bcmi_l3_init_check(int unit);
/* DGLP Calculation defines */

/* Shift module value by 7 bits, since port number
 * occupies least significant 7 bits.
 */
#define BCM_L3_DGLP_MODULE_SHIFT_BITS    7
/* Port number is 7 bits. Maximum port number is 128 */
#define BCM_L3_DGLP_PORT_MASK         0x7f
/* Module id is 8 bits. Maximum modid is 255 */
#define BCM_L3_DGLP_MODULE_ID_MASK       0xff

#define BCM_L3_DGLP_GET(mod, port) \
                 (((mod & BCM_L3_DGLP_MODULE_ID_MASK) \
                 << BCM_L3_DGLP_MODULE_SHIFT_BITS) | \
                 (port & BCM_L3_DGLP_PORT_MASK))
#define BCM_L3_DGLP_PORT_GET(dglp) \
                 (dglp & BCM_L3_DGLP_PORT_MASK)
#define BCM_L3_DGLP_MODULE_ID_GET(dglp) \
                 ((dglp >> BCM_L3_DGLP_MODULE_SHIFT_BITS) & \
                  BCM_L3_DGLP_MODULE_ID_MASK)


#ifdef INCLUDE_L3

extern int _bcm_esw_l3_vrf_stat_get(int unit, int sync_mode,bcm_vrf_t vrf, 
                                   bcm_l3_vrf_stat_t stat, uint64 *val);
#ifdef FB_LPM_DEBUG
extern int lpm128_state_verify(int u);
#endif

#define LPM_L3_ADDR		0xffffffff

#define BCM_L3_NO_IP6_SUPPORT(_unit_, _flags_) \
    (((_flags_) & BCM_L3_IP6) ?           \
     (soc_feature(_unit_, soc_feature_l3_ip6) ? FALSE : TRUE) : FALSE)

#define BCM_L3_RPE_SET(_flags_) \
    (((_flags_) & BCM_L3_RPE) ? TRUE : FALSE)


/* 5674 L3 interface defines */
#define BCM_LYNX_L3_INTF_NUM_HI_SHIFT      9
#define BCM_LYNX_L3_INTF_NUM_L0_MASK       0x1ff

#define BCM_LYNX_LPM_NEXT_PTR_HI_SHIFT      12
#define BCM_LYNX_LPM_NEXT_PTR_L0_MASK       0xfff

#define _BCM_TR_L3_TRUST_DSCP_PTR_BIT_SIZE 6
#define _BCM_TD_L3_TRUST_DSCP_PTR_BIT_SIZE 7
#define _BCM_FB6_L3_TRUST_DSCP_PTR_BIT_SIZE 9

/* Generic types. */
#define BCM_L3_CMP_GREATER       (1)
#define BCM_L3_CMP_EQUAL         (0)
#define BCM_L3_CMP_LESS         (-1)
#define BCM_L3_CMP_NOT_EQUAL     (2)

/* ECMP member operations */
#define BCM_L3_ECMP_MEMBER_OP_SET     0
#define BCM_L3_ECMP_MEMBER_OP_ADD     1
#define BCM_L3_ECMP_MEMBER_OP_DELETE  2
#define BCM_L3_ECMP_MEMBER_OP_REPLACE 3

/* L3 IP options Default Profile */
#define BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX 0x00

/* L3 subsystem Bookkeeping flags */
#define BCM_L3_BK_DISABLE_ADD_TO_ARL             (1 << 0)
#define BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE       (1 << 1)

#define BCM_L3_INTER_FLAGS_PRE_FILTER      (1 << 0)
#define BCM_L3_INTER_FLAGS_PRE_FILTER_SRC  (1 << 1)
/*
 * Defines L3 interface
 */
typedef struct _bcm_l3_intf_cfg_s {
    uint32       l3i_flags;         /* create or replace */
    bcm_vrf_t    l3i_vrf;           /* Virtual Router Instance (EZ only) */
    int          l3i_index;         /* interface number */
    bcm_mac_t    l3i_mac_addr;      /* MAC address for this interface */
    bcm_vlan_t   l3i_vid;           /* VLAN ID this interface is for */
    bcm_vlan_t   l3i_inner_vlan;    /* Inner VLAN ID for double tagged packets*/
    int          l3i_tunnel_idx;    /* tunnel index */
    int          l3i_ttl;           /* TTL threshold */
    int          l3i_mtu;           /* MTU */
    bcm_if_group_t  l3i_group;      /* Interface group number */
    bcm_l3_intf_qos_t  vlan_qos;
    bcm_l3_intf_qos_t  inner_vlan_qos;
    bcm_l3_intf_qos_t  dscp_qos;
#ifdef BCM_TRIUMPH3_SUPPORT
    int l3i_class;                  /* Classification class ID */
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    int l3i_ip4_options_profile_id; /* IP4 Options handling Profile ID */
    int l3i_nat_realm_id;           /* Interface realm for NAT */
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int l3i_intf_flags;             /* L3 intf flags */
#endif /* BCM_TRIDENT2_SUPPORT || BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_RIOT_SUPPORT
    bcm_gport_t l3i_source_vp;      /* svp for routing in 
                                       VFI domain over HG flow */
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 flow_handle;             /* FLOW handle for flex entries. */
    uint32 flow_option_handle;      /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex entries */
    uint32 num_of_fields;           /* number of logical fields. */
#endif /* BCM_TRIDENT3_SUPPORT */
} _bcm_l3_intf_cfg_t;

/*
 * Covers all info needed to config L3 table
 */
typedef struct _bcm_l3_cfg_s {
    uint32        l3c_flags;        /* See <bcm/l3.h> BCM_L3_* */
    uint32        l3c_flags2;       /* See <bcm/l3.h> BCM_L3_* */
    bcm_vrf_t     l3c_vrf;          /* Virtual Router Instance (EZ only) */
    int           l3c_hw_index;     /* L3 table index */
    bcm_ip_t      l3c_ip_addr;      /* IPv4 address */
    bcm_ip_t      l3c_src_ip_addr;  /* Source IPv4 address (for IPMC) */
    bcm_ip_t      l3c_ip_mask;      /* used by delete by prefix */
    bcm_ip6_t     l3c_ip6;          /* IPv6 address */
    bcm_ip6_t     l3c_sip6;         /* IPv6 Source address (for IPMC) */
    bcm_ip6_t     l3c_ip6_mask;     /* IPv6 delete by prefix */
    bcm_vlan_t    l3c_vid;          /* VLAN ID (for IPMC 5695 only) */
    bcm_mac_t     l3c_mac_addr;     /* MAC address */
    bcm_if_t      l3c_intf;         /* L3 interface number */
    bcm_if_t      l3c_ing_intf;     /* L3 ingress Interface associated with this entry*/
    bcm_port_t    l3c_port_tgid;    /* port/TGID */
    bcm_port_t    l3c_stack_port;   /* Strata stack port if mod not local */
    int           l3c_modid;        /* MODID */
    int           l3c_tunnel;       /* Tunnel */
    int           l3c_prio;         /* Priority */
    int           l3c_ipmc_ptr;     /* 5690 index to IPMC table */
    int           l3c_lookup_class; /* Classification lookup class id. */
    int           l3c_ecmp;         /* Set this to 1 to indicate ECMP route */
    int           l3c_ecmp_count;   /* ECMP count */
    int           l3c_rp_id;        /* PIM-BIDIR Rendezvous point ID */
    int           l3c_eh_q_tag_type;/* EH queue tag type */
    int           l3c_eh_q_tag;     /* EH queue tag */
    bcm_if_t      l3c_encap_id;     /* Encapsulation index */
    int           l3c_flex_ctr_base_id; /* Base flex counter index */
    int           l3c_flex_ctr_mode;    /* Flex counter offset mode */
    int           l3c_flex_ctr_pool;    /* Flex counter pool number */
    int           l3c_bfd_enable;       /* BFD enable */
#define l3c_ipmc_group   l3c_ip_addr
#ifdef BCM_TRIDENT3_SUPPORT
    uint32        l3c_flow_handle;        /* FLOW handle for flex entries. */
    uint32        l3c_flow_option_handle; /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t l3c_logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex entries */
    uint32        l3c_num_of_fields;      /* number of logical fields. */
#endif /* BCM_TRIDENT3_SUPPORT */
    int           l3c_ipmc_ptr_l2;  /* L3MC_INDEX_FOR_L2_DISTRIBUTION on TTL/RPF check failure */
} _bcm_l3_cfg_t;

/*
 * defip entry type used in _bcm_defip_cfg_t
 */
typedef enum _bcm_defip_entry_type_e {
    bcmDefipEntryTypeIp = 0,
    bcmDefipEntryTypeFcoe = 1
} _bcm_defip_entry_type;

/*
 * All info needed to configure an IP route.
 */
typedef struct _bcm_defip_cfg_s {
    uint32        defip_flags;       /* See <bcm/l3.h> BCM_L3_* */
    uint32        defip_flags2;      /* See <bcm/l3.h> BCM_L3_* */
    uint32        defip_inter_flags; /* See BCM_L3_INTER_* */
    uint32        defip_ipmc_flags;  /* MC-ROUTE: See BCM_IPMC_xxx flag definitions. */
    bcm_vrf_t     defip_vrf;         /* Virtual Router Instance (EZ only) */
    bcm_ip_t      defip_ip_addr;     /* IPv4 subnet address */
    bcm_ip6_t     defip_ip6_addr;    /* IPv6 address */
    int           defip_sub_len;     /* Subnet prefix length */
    int           defip_index;       /* DEFIP table index (Strata only) */
    bcm_mac_t     defip_mac_addr;    /* Next hop MAC address (DA) */
    bcm_ip_t      defip_nexthop_ip;  /* Next hop IPv4 address */
    bcm_ip6_t     defip_nexthop_ip6; /* Next hop IPv6 address */
    int           defip_tunnel;      /* Tunnel */
    int           defip_prio;        /* Priority */
    bcm_if_t      defip_intf;        /* Hardware redirection index.
                                      * For Draco, this is an index into
                                      * the L3_INTF table.  For Tucana,
                                      * this is an index into the L3 table. */
    bcm_port_t    defip_port_tgid;   /* destination port */
    bcm_port_t    defip_stack_port;  /* Strata stack port if mod not local */
    int           defip_modid;       /* destination MODID */
    bcm_vlan_t    defip_vid;         /* For BCM5695 per VLAN default route only */
    int           defip_ecmp;        /* Set this to 1 to indicate ECMP route */
    int           defip_ecmp_count;  /* ECMP count */
    int           defip_ecmp_index;  /* ECMP table index (BCM5695 only) */
    int           defip_l3hw_index;  /* DEFIP route's L3 table index */
    uint32        defip_tunnel_option; /* Tunnel option value. */
    bcm_mpls_label_t defip_mpls_label; /* MPLS label.          */
    int           defip_lookup_class;/* Classification lookup class id. */
    _bcm_defip_entry_type defip_entry_type; /* Entry type, IP or FCOE */
    int           defip_fcoe_d_id;   /* D_ID for FCOE entries */
    int           defip_fcoe_d_id_mask;/* D_ID mask for FCOE entries */
    /* Used to indicate if the entry is in paired tcam or in unpaired TCAM */
    int           defip_flags_high; 
    int           defip_alpm_cookie; /* returns lookup result to be reused in
                                      * insert or delete for alpm */
    bcm_if_t      defip_expected_intf; /* MC-ROUTE: Expected L3 Interface used for
                                        * multicast RPF check */
    int           defip_l3a_rp;      /* MC-ROUTE: Rendezvous point ID */
    int           defip_source_port_tgid; /* Expected source port/trunk */
    int           defip_source_modid;     /* Expected source module */
    int           defip_source_ts;        /* Expected source is trunk? */
    bcm_multicast_t defip_mc_group;  /* MC-ROUTE: L3 Multicast group index */
#ifdef BCM_TRIDENT3_SUPPORT
    uint32        defip_flow_handle;        /* FLOW handle for flex entries. */
    uint32        defip_flow_option_handle; /* FLOW option handle for flex entries. */
    uint32        defip_num_of_fields;      /* number of logical fields. */
    bcm_flow_logical_field_t defip_logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex entries */
#endif /* BCM_TRIDENT3_SUPPORT */
    int           defip_flex_ctr_base_id; /* Base flex counter index */
    int           defip_flex_ctr_mode;    /* Flex counter offset mode */
    int           defip_flex_ctr_pool;    /* Flex counter pool number */
    int           defip_acl_class_id;     /* Pre-filter class id. */

#ifdef ALPM_ENABLE
    /* Used by ALPM */
    uint32        user_data[5];      /* Currently used by ALPM */
    void          *l1_pvt_node;
    void          *pvt_node;
    void          *bkt_node;
    void          *bkt_info;
    uint32        bkt_kshift;       /* carry down ACB pvt_key_len */

    void          *spl_opn;
    void          *spl_npn;
    void          *spl_pn;
    void          *tcam_write;      /* carry _alpm_tcam_write_t info for L1 pvt add during L2 bkt split */
    uint32        spl_key[5];
    int           spl_key_len;
    int           default_miss;     /* Used for pivot's default_miss in ALPM_TCAM_ZONED */
    int           fast_delete_all;  /* used only in TH3 fast delete_all */
#endif
} _bcm_defip_cfg_t;

/*
 * Information of DEF_IP binary table (Strata only)
 */
typedef struct _bcm_strata_defip_s {
    ip_addr_t 	defip_ip_addr;
    ip_addr_t 	defip_sub_len;
    int		defip_interface;
} _bcm_strata_defip_t;

/*
 * Node for hashed version of SW L3 table (XGS Switch)
 */
typedef struct _bcm_l3hash_s {
    ip_addr_t l3h_ip_addr;
    ip_addr_t l3h_sip_addr;
    uint16    l3h_vid;       /* for 5695 IPMC only */
    uint16    l3h_flags;
    uint32    l3h_use_count;
#define L3H_HOST      1
#define L3H_LPM       2
#define L3H_INVALID   4
} _bcm_l3hash_t;

/*
 * Information about the L3 table (XGS Switch)
 */
typedef struct _bcm_l3_table_s {
    _bcm_l3hash_t   *l3_hash;    /* hashed version of SW L3 table */
    uint32          l3_min;      /* Min index */
    uint32          l3_max;      /* Total number of L3 table entries */
    uint32          l3_count;    /* Number of entries currently used */
} _bcm_l3_table_t;

/*
 * Definition of linked list of backup routes (XGS Switch)
 */
typedef struct _bcm_backup_route_s {
    struct _bcm_backup_route_s  *next;
    uint16    l3_intf;               /* L3 interface number */
    uint16    l3_index;              /* index in L3 table */
    ip_addr_t ip_addr;               /* IP subnet address */
    unsigned  subnet_len:6;          /* subnet IP prefix len */
    unsigned  ecmp:1;                /* ecmp bit */
    unsigned  ecmp_count:6;          /* # of ECMP routes */
    unsigned  ecmp_index:11;         /* ECMP table index as in BCM5695 */
} _bcm_backup_route_t;

/*
 * Information of one LPM table entry (XGS Switch)
 */
typedef struct _bcm_lpm_entry_s {
    _bcm_backup_route_t *backup_routes; /* Backup routes (shorter prefix) */
    uint16      forward;                /* index of forward LPM _block_ */
    uint16      parent;                 /* index of parent LPM _entry_ */
    uint16      l3_index;               /* L3 table index for this route */
    uint16      l3_intf;                /* L3 interface num for this route */
    ip_addr_t   ip_addr;                /* subnet IP address */
    unsigned    subnet_len:6;           /* subnet IP prefix len */
    unsigned    valid:1;                /* valid bit */
    unsigned    final:1;                /* stop bit */
    unsigned    dirty:1;                /* needs to be written to h/w tbl */
    unsigned    ecmp:1;                 /* ECMP route bit */
    unsigned    ecmp_count:6;           /* # of ECMP routes */
    unsigned    ecmp_index:11;          /* ECMP table index as in BCM5695 */
} _bcm_lpm_entry_t;

/*
 * Node for LPM update list
 */
typedef struct _bcm_update_list_s {
    struct _bcm_update_list_s *next;
    uint16  block;
    uint32  offsets;
} _bcm_update_list_t;

/*
 * Per VLAN default route (Draco 1.5)
 */
typedef struct _bcm_per_vlan_def_route_s {
    struct _bcm_per_vlan_def_route_s *next;
    bcm_vlan_t vid;
    uint16 l3_index;
} _bcm_per_vlan_def_route_t;

/*
 * Informatin about the LPM table (XGS Switch only)
 */
typedef struct _bcm_l3_defip_table_s {
    _bcm_lpm_entry_t *l3_lpm;     /* SW shadow copy of LPM table */
    int def_route_index;          /* def route is last in l3_lpm table */
    _bcm_per_vlan_def_route_t *pv_def_route; /* Per VLAN def route (5695 only) */
    int global_def_route_set;     /* VLAN table based global default route */
    int lpm_based_def_route_set;  /* LPM based def route set */

    SHR_BITDCL *lpm_blk_used;     /* LPM bock usage bit mask */
    soc_mem_t lpm_mem;            /* name of the DEFIP memory */
    soc_mem_t lpmhit_mem;         /* name of the DEFIP HIT memory */
    int  lpm_min;                 /* min LPM index */
    int  lpm_max;                 /* max LPM index */
    int  lpm_block_max;           /* Max number of blocks in LPM table */
    int  lpm_block_used;          /* number of blocks used */
    int  lpm_entry_used;          /* number of entries used */
    int  ecmp_max_paths;          /* maximum number of ECMP paths allowed */
    int  ecmp_inuse;              /* in use, ecmp_max_paths cannot be changed */

    /*
     * These linked lists contain the entries need to be
     * written to HW LPM table after each insertion/deletion
     */
    _bcm_update_list_t *update_head;  /* head of linked list */
    _bcm_update_list_t *update_tail;  /* tail used for inverse the list */
} _bcm_l3_defip_tbl_t;

/*
 * define structure for per Group ECMP info
 */

typedef struct _bcm_l3_ecmp_group_info_s {
    int max_paths;         /* Maximum number of ECMP paths per group. */
    int alternate_cnt;
    uint32 ecmp_flags;     /* Ecmp group flags */
} _bcm_l3_ecmp_group_info_t;

/*
 * define structure for per Group ECMP buffer
 */
typedef struct _bcm_l3_ecmp_group_buffer_s {
    int        *group_buffer;      /* Ecmp group buffer */
    int        *dlb_group_buffer;  /* DLB group buffer */
} _bcm_l3_ecmp_group_buffer_t;

/*
 * define structure for user supplied data in DEFIP traverse
 */
typedef struct _bcm_l3_defip_traverse_s {
    bcm_l3_route_traverse_cb trav_cb;
    void *user_data;
    int trav_start;
    int trav_end;
    int trav_index;
    bcm_l3_route_t *route_info;
} _bcm_l3_defip_traverse_t;

typedef int (*route_trav_cb_fn)(int unit, _bcm_lpm_entry_t *lpm_entry, void *data);

/*
 * Software book keeping for L3 related information
 */
typedef struct _bcm_l3_bookkeeping_s {
    uint8       l3_initialized;  /* Is L3 module init'ed ? */
    SHR_BITDCL *l3_intf_used;    /* L3 outgoing interface usage bit mask */
    SHR_BITDCL *add_to_arl;      /* L3 intf is added to ARL bit mask */
    int         l3_intf_table_size; /* max number of intf supported by chip */
    uint16      l3intf_count;    /* number of intf created */
    SHR_BITDCL *l3_iif_used;     /* L3 incoiming interface usage bit mask. */
    int         l3_iif_table_size;/* max number of ingress interfaces  */
    uint16      l3iif_count;     /* number of ingress interfaces created */
    _bcm_strata_defip_t *strata_defip_info;
                                 /* SW copy of DEF_IP table (Strata) */
    int      defip_table_size;   /* max DEF_IP table entries for chip */
    int      defip_count;        /* IP routes added, note each route takes
                                    more than one entries in LPM table */
    int      defip_ip6_count;    /* IPv6 routes added */
    int      l3_table_size;      /* chip L3 table size */
    int      l3_ip4_added;       /* IPv4 hosts added */
    int      l3_ip6_added;       /* IPv6 hosts added */
    int      l3_ipmc4_added;     /* IPv4 IPMC added */
    int      l3_ipmc6_added;     /* IPv6 IPMC added */
    int      l3_nh_added;        /* Egress Next hops added */
    int      l3_ecmp_grp_added;  /* ECMP groups added */
    int      ecmp_table_size;    /* max ECMP table entries (D1.5 only) */
    int      nh_table_size;      /* max next hop table */
    int      tunnel_table_size;  /* max tunnel table entries */
    int      dscp_map_size;      /* DSCP-map size */
    SHR_BITDCL *dscp_map_used;   /* DSCP-map usage bit mask */
    int      num_dscp_maps;      /* Number of DSCP maps available */
#ifdef BCM_TRIDENT_SUPPORT
    int     *l3_trunk_nh_store; /* Store for Trunk nextHop */
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    int      l3_max_ecmp_mode;   /* ECMP grp scalability control */
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    SHR_BITDCL *ip4_options_bitmap;   /* IP options usage bitmap*/
    uint32     *ip4_profiles_hw_idx;   /* IP options usage bitmap*/
#endif /* BCM_TRIDENT2_SUPPORT */
    uint32    l3_ip6_max_128b_entries;  /* Max allowed 128b V6 entries */
    uint32    flags;             /* bookkeeping flags for L3 subsystem */
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    uint32    l3_ecmp_levels;
    uint32    l3_ecmp_overlay_entries;
    uint32    l3_ecmp_rh_overlay_entries;
#endif
#ifdef BCM_RIOT_SUPPORT
    uint32    l3_intf_overlay_entries;
    uint32    l3_nh_overlay_entries;
    uint32    riot_enable;
#endif
#if defined (BCM_TOMAHAWK_SUPPORT) || defined (BCM_TRIDENT2PLUS_SUPPORT)
    uint8     mc_per_trunk_repl_mode; /* per trunk replication mode */
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_RIOT_SUPPORT
    uint32    l3_intf_overlay_alloc_mode;
    uint32    l3_nh_overlay_alloc_mode;
#endif
    uint32   *l3_nh_multi_count;
    uint32    l3_nh_reserve_for_ecmp;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32    l3_ecmp_group_first_lkup_mem_size;
    uint32    l3_ecmp_member_first_lkup_mem_size;
#endif
} _bcm_l3_bookkeeping_t;

extern _bcm_l3_bookkeeping_t  _bcm_l3_bk_info[BCM_MAX_NUM_UNITS];


/*
 * L3 interface table usage bit mask operations
 */
#define BCM_L3_INTF_USED_GET(_u_, _if_)                           \
        SHR_BITGET((_bcm_l3_bk_info[unit]).l3_intf_used, (_if_))
#define BCM_L3_INTF_USED_SET(_u_, _if_)                           \
    {                                                             \
        SHR_BITSET((_bcm_l3_bk_info[unit]).l3_intf_used, (_if_)); \
        (_bcm_l3_bk_info[unit]).l3intf_count++;                   \
    }
#define BCM_L3_INTF_USED_CLR(_u_, _if_)                           \
    {                                                             \
        SHR_BITCLR((_bcm_l3_bk_info[unit]).l3_intf_used, (_if_)); \
        (_bcm_l3_bk_info[unit]).l3intf_count--;                   \
    }
#define BCM_L3_IIF_USED_GET(_u_, _if_)                           \
        SHR_BITGET((_bcm_l3_bk_info[unit]).l3_iif_used, (_if_))
#define BCM_L3_IIF_USED_SET(_u_, _if_)                           \
    {                                                             \
        SHR_BITSET((_bcm_l3_bk_info[unit]).l3_iif_used, (_if_)); \
        (_bcm_l3_bk_info[unit]).l3iif_count++;                   \
    }
#define BCM_L3_IIF_USED_CLR(_u_, _if_)                           \
    {                                                             \
        SHR_BITCLR((_bcm_l3_bk_info[unit]).l3_iif_used, (_if_)); \
        (_bcm_l3_bk_info[unit]).l3iif_count--;                   \
    }
#define BCM_L3_INTF_ARL_GET(_u_, _if_) \
        SHR_BITGET((_bcm_l3_bk_info[unit]).add_to_arl, (_if_))
#define BCM_L3_INTF_ARL_SET(_u_, _if_) \
        SHR_BITSET((_bcm_l3_bk_info[unit]).add_to_arl, (_if_))
#define BCM_L3_INTF_ARL_CLR(_u_, _if_) \
        SHR_BITCLR((_bcm_l3_bk_info[unit]).add_to_arl, (_if_))

#define BCM_L3_DSCP_MAP_USED_GET(_u_, _idx_) \
        SHR_BITGET((_bcm_l3_bk_info[unit]).dscp_map_used, (_idx_))
#define BCM_L3_DSCP_MAP_USED_SET(_u_, _idx_) \
        SHR_BITSET((_bcm_l3_bk_info[unit]).dscp_map_used, (_idx_))
#define BCM_L3_DSCP_MAP_USED_CLR(_u_, _idx_) \
        SHR_BITCLR((_bcm_l3_bk_info[unit]).dscp_map_used, (_idx_))

/* Get and Set macros for bookkeeping FLAGs */
#define BCM_L3_BK_FLAG_SET(_u_, _flag_)       \
        ((_bcm_l3_bk_info[unit]).flags) |= (_flag_)

#define BCM_L3_BK_FLAG_GET(_u_, _flag_)       \
        (((_bcm_l3_bk_info[unit]).flags) & (_flag_) ? 1 : 0)

/* L3X ROUTINES TO BE REMOVED FROM PUBLIC API */
extern int bcm_l3_defip_traverse(int unit, bcm_l3_route_traverse_cb trav_fn,
                      uint32 start, uint32 end);

extern int _bcm_esw_l3_egress_ecmp_get(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                       int intf_size, bcm_if_t *intf_array,
                                       int *intf_count, int rh_retrieve);
extern int _bcm_esw_l3_ecmp_create(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                   int intf_count, bcm_if_t *intf_array, int op,
                                   int count, bcm_if_t *intf, int primary_count,
                                   SHR_BITDCL *dlb_member_bitmap);

#define _BCM_EGR_IP_TUNNEL_MPLS_DOUBLE_WIDE(_u_, _tnl_type_)   \
       (((soc_feature((_u_), soc_feature_egr_ip_tnl_mpls_double_wide)) \
        || (soc_feature((_u_), soc_feature_th3_style_simple_mpls))) \
           && (_tnl_type_ == bcmTunnelTypeMpls))

#define _BCM_EGR_IP_TUNNEL_DOUBLE_WIDE_ENTRY(unit, _tnl_type_)      \
       ((_BCM_EGR_IP_TUNNEL_MPLS_DOUBLE_WIDE(unit, _tnl_type_))  || \
       (_BCM_TUNNEL_OUTER_HEADER_IPV6(_tnl_type_)))


#define _BCM_TUNNEL_OUTER_HEADER_IPV6(_tnl_type_)              \
       ((((_tnl_type_) == bcmTunnelTypeIp4In6)              || \
         ((_tnl_type_) == bcmTunnelTypeIp6In6)              || \
         ((_tnl_type_) == bcmTunnelTypeAutoMulticast6)      || \
         ((_tnl_type_) == bcmTunnelTypeIpAnyIn6)            || \
         ((_tnl_type_) == bcmTunnelTypeGre4In6)             || \
         ((_tnl_type_) == bcmTunnelTypeGre6In6)             || \
         ((_tnl_type_) == bcmTunnelTypeGreAnyIn6)           || \
         ((_tnl_type_) == bcmTunnelTypePim6SmDr1)           || \
         ((_tnl_type_) == bcmTunnelTypePim6SmDr2)           || \
         ((_tnl_type_) == bcmTunnelTypeRoeIn6)              || \
         ((_tnl_type_) == bcmTunnelTypeWlanWtpToAc6         || \
         ((_tnl_type_) == bcmTunnelTypeWlanAcToAc6))        || \
         ((_tnl_type_) == bcmTunnelTypeVxlan6)) ? TRUE : FALSE)

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_l3_cleanup(int unit);
#else
#define    _bcm_l3_cleanup(u)   (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_l3_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_l3_sw_dump(int unit);
extern void _bcm_tr_ext_lpm_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

typedef struct bcmi_l3_egress_multi_info_s {
    uint32 flags;           /* See BCM_L3_EGRESS_MULTI_XXX flag definitions. */
    int number_of_elements; /* number of egress objects to allocate. */
} bcmi_l3_egress_multi_info_t;

#define BCMI_L3_EGRESS_SOURCE_MULTI_ADD         1
#define BCMI_L3_EGRESS_SOURCE_SINGLE_ADD        2


#define BCMI_L3_EGRESS_SOURCE_MASK              0xff 
#define BCMI_L3_EGRESS_SOURCE_SHIFT             24
#define BCMI_L3_EGRESS_SOURCE_VAL_MASK          0xffffff

#define BCMI_L3_EGRESS_SOURCE_SET(type, val)    \
            ((((type) << BCMI_L3_EGRESS_SOURCE_SHIFT) | val))

#define BCMI_L3_EGRESS_SOURCE_GET(val)    \
            ((((val) >> BCMI_L3_EGRESS_SOURCE_SHIFT) & BCMI_L3_EGRESS_SOURCE_MASK))


/*
 * L3 l3table show flags
 */
#define _BCM_L3_SHOW_INDEX    BCM_L3_VXLAN_ONLY

#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)

/* ECMP Single level */
#define BCMI_L3_ECMP_SINGLE_LEVEL         1

/* Overlay validity checks */
#define BCMI_L3_ECMP_IS_MULTI_LEVEL(_u) \
    (((&_bcm_l3_bk_info[unit])->l3_ecmp_levels) > BCMI_L3_ECMP_SINGLE_LEVEL)

#define BCMI_L3_ECMP_OVERLAY_ENTRIES(_u) \
    ((&_bcm_l3_bk_info[_u])->l3_ecmp_overlay_entries)

#define BCMI_L3_INTF_OVERLAY_ENTRIES(_u) \
    ((&_bcm_l3_bk_info[_u])->l3_intf_overlay_entries)

/* ECMP Macro flow overlay check */
#define BCM_MACROFLOW_IS_OVERLAY(type) \
            ((type == bcmSwitchMacroFlowHashOverlayMinOffset) || \
             (type == bcmSwitchMacroFlowHashOverlayMaxOffset) || \
             (type == bcmSwitchMacroFlowHashOverlayStrideOffset))

/* RioT Overlay validity checks */
#define BCMI_RIOT_IS_ENABLED(_u) \
    ((&_bcm_l3_bk_info[_u])->riot_enable)

#define BCMI_L3_NH_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_nh_overlay_entries) > 0))

#define BCMI_L3_INTF_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_intf_overlay_entries) > 0))

#define BCMI_L3_INTF_MEM_ALLOC_MODE(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    ((_bcm_l3_bk_info[_u].l3_intf_overlay_alloc_mode) == 1))

#define BCMI_RIOT_VPN_VFI_IS_SET(_u, _vid) \
    ((BCMI_RIOT_IS_ENABLED(_u)) && (_BCM_VPN_VFI_IS_SET((_vid))))

#define BCMI_RIOT_VPN_VFI_IS_NOT_SET(_u, _vid) \
    ((!BCMI_RIOT_IS_ENABLED(_u)) || (!(_BCM_VPN_VFI_IS_SET((_vid)))))

#define BCMI_GPORT_TYPE_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)

#endif
#endif	/* INCLUDE_L3 */

#endif	/* !_BCM_INT_L3_H */
