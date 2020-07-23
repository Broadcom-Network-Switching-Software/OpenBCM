/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        virtual.h
 * Purpose:     Function declarations for VP / VFI resource management
 */

#ifndef _BCM_INT_RESOURCE_H_
#define _BCM_INT_RESOURCE_H_

typedef struct _bcm_virtual_bookkeeping_s {
    SHR_BITDCL  *vfi_bitmap;         /* Global VFI bitmap */
    SHR_BITDCL  *vp_bitmap;          /* Global Virtual Port bitmap */
    SHR_BITDCL  *mpls_vfi_bitmap;    /* MPLS VFI bitmap */
    SHR_BITDCL  *mpls_vp_bitmap;     /* MPLS Virtual Port bitmap */
    SHR_BITDCL  *mim_vfi_bitmap;     /* MIM VFI bitmap */
    SHR_BITDCL  *mim_vp_bitmap;      /* MIM Virtual Port bitmap */
    SHR_BITDCL  *l2gre_vfi_bitmap;   /* L2GRE VFI bitmap */
    SHR_BITDCL  *l2gre_vp_bitmap;    /* L2GRE Virtual Port bitmap */
    SHR_BITDCL  *vxlan_vfi_bitmap;   /* VXLAN VFI bitmap */
    SHR_BITDCL  *vxlan_vp_bitmap;    /* VXLAN Virtual Port bitmap */
    SHR_BITDCL  *flow_vfi_bitmap;    /* FLOW VFI bitmap */
    SHR_BITDCL  *flow_vp_bitmap;     /* FLOW Virtual Port bitmap */
    SHR_BITDCL  *subport_vp_bitmap;  /* Subport Virtual Port bitmap */
    SHR_BITDCL  *wlan_vp_bitmap;     /* WLAN Virtual Port bitmap */
    SHR_BITDCL  *trill_vp_bitmap;    /* TRILL Virtual Port bitmap */
    SHR_BITDCL  *vlan_vp_bitmap;     /* VLAN Virtual Port bitmap */
    SHR_BITDCL  *niv_vp_bitmap;      /* NIV Virtual Port bitmap */
    SHR_BITDCL  *extender_vp_bitmap; /* Extender Virtual Port bitmap */
    SHR_BITDCL  *vp_lag_vp_bitmap;   /* VP LAG Virtual Port bitmap */
    SHR_BITDCL  *vp_shared_vp_bitmap;   /* Shared Virtual Port bitmap */
    SHR_BITDCL  *vp_network_vp_bitmap;  /* Network Virtual Port bitmap */
    SHR_BITDCL  *vlan_vfi_bitmap;    /*VLAN VFI BITMAP*/
    uint16      *vp_to_vpn; /* VP-to-VPN cache */
} _bcm_virtual_bookkeeping_t;

extern _bcm_virtual_bookkeeping_t  _bcm_virtual_bk_info[BCM_MAX_NUM_UNITS];

#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/mbcm.h>

/****************************************************************
 *
 * Virtual resource management functions
 */
#if defined(INCLUDE_L3)
typedef enum _bcm_vp_type_s {
    _bcmVpTypeMpls,      /* MPLS VP */
    _bcmVpTypeMim,       /* MIM VP */
    _bcmVpTypeSubport,   /* Subport VP */
    _bcmVpTypeWlan,      /* WLAN VP */
    _bcmVpTypeTrill,     /* TRILL VP */
    _bcmVpTypeVlan,      /* VLAN VP */
    _bcmVpTypeNiv,       /* NIV VP */
    _bcmVpTypeL2Gre,     /* L2GRE VP */
    _bcmVpTypeVxlan,     /* VXLAN VP */
    _bcmVpTypeExtender,  /* Extender VP */
    _bcmVpTypeVpLag,     /* VP LAG VP */
    _bcmVpTypeFlow,      /* FLOW VP */
    _bcmVpTypeAny        /* Any VP */
} _bcm_vp_type_e;

#define _BCM_VP_INFO_NETWORK_PORT 0x00000001 /* Network side port for VP applications */
#define _BCM_VP_INFO_SHARED_PORT  0x00000002 /* Shared virtual port for VP applications */

typedef struct _bcm_vp_info_s {
    _bcm_vp_type_e vp_type;
    uint32 flags;
} _bcm_vp_info_t;

typedef enum _bcm_vfi_type_s {
    _bcmVfiTypeMpls,     /* MPLS VFI */
    _bcmVfiTypeMim,      /* MIM VFI */
    _bcmVfiTypeL2Gre,   /* L2GRE VFI */
    _bcmVfiTypeVxlan,   /* VXLAN VFI */
    _bcmVfiTypeVlan,    /* VLAN VFI */
    _bcmVfiTypeFlow,    /* FLEX FLOW VFI */
    _bcmVfiTypeAny       /* Any VFI */
} _bcm_vfi_type_e;

typedef enum _bcm_vp_ing_dvp_config_op_e {
    _bcmVpIngDvpConfigClear,  /* Clear ING_DVP_* tables                    */
    _bcmVpIngDvpConfigSet,    /* Set input data in ING_DVP_* tables        */
    _bcmVpIngDvpConfigUpdate  /* Update the input data in ING_DVP_* tables */
} _bcm_vp_ing_dvp_config_op_t;

typedef enum _bcm_vp_ing_dvp_vp_type_e {
    _bcmVpIngDvpVpTypeMPLS_MIM_NIV,  /* MPLS/MIM/NIV */
    _bcmVpIngDvpVpTypeTRILL,    /* TRILL */
    _bcmVpIngDvpVpTypeL2GRE,  /* L2GRE */
    _bcmVpIngDvpVpTypeVXLAN,  /* VXLAN */
    _bcmVpIngDvpVpTypeCount
} _bcm_vp_ing_dvp_vp_type_t;

typedef enum _bcm_vp_ing_dvp_port_type_e {
    _bcmVpIngDvpVpPortTypeDefault,  /* vp is  not network port */
    _bcmVpIngDvpPortTypeNetwork,    /* vp is network port */
    _bcmVpIngDvpPortTypeCount
} _bcm_vp_ing_dvp_port_type_t;


#define ING_DVP_CONFIG_INVALID_INTF_ID    -1   /* invalid intf id */
#define ING_DVP_CONFIG_INVALID_PORT_TYPE  -1   /* invalid port type */
#define ING_DVP_CONFIG_INVALID_VP_TYPE    -1   /* invalid vp type */

/*
 * Split Horizon Network Group Id Validation
 */
#define _BCM_SWITCH_NETWORK_GROUP_ID_VALID(unit, gid)    \
                    (((gid) >= 0) && ((gid) <= 7))


#define _BCM_SWITCH_RESERVED_NETWORK_GROUP_IDS(unit, gid)    \
                    (((gid) >= 0) && ((gid) <= 1))


extern int _bcm_virtual_init(int unit, soc_mem_t vp_mem, soc_mem_t vfi_mem);
extern int _bcm_virtual_deinit(int unit);
extern void _bcm_vp_info_init (_bcm_vp_info_t *vp_info);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_virtual_sync(int unit);
#endif
extern int _bcm_vp_alloc(int unit, int start, int end, int count, soc_mem_t vp_mem, 
                         _bcm_vp_info_t type, int *base_vp);
extern int _bcm_vp_free(int unit, _bcm_vp_type_e type, int count, int base_vp);
extern int _bcm_vp_used_set(int unit, int vp, _bcm_vp_info_t vp_info);
extern int _bcm_vp_used_get(int unit, int vp, _bcm_vp_type_e type);
extern int _bcm_vp_info_get(int unit, int vp, _bcm_vp_info_t *vp_info);
extern int _bcm_vp_gport_dest_fill(int unit, int vp,
                                        _bcm_gport_dest_t *gport_dest);

extern int _bcm_vp_default_cml_mode_get (int unit, int *cml_default_enable, int *cml_new, int *cml_move);
extern int _bcm_vfi_alloc(int unit, soc_mem_t vp_mem, _bcm_vfi_type_e type, int *vfi);
extern int _bcm_vfi_alloc_with_id(int unit, soc_mem_t vfi_mem, _bcm_vfi_type_e type, int vfi);
extern int _bcm_vfi_free(int unit, _bcm_vfi_type_e type, int vfi);
extern int _bcm_vfi_used_get(int unit, int vfi, _bcm_vfi_type_e type);
extern int _bcm_vfi_flex_stat_index_set(int unit, int vfi,
                               _bcm_vfi_type_e type, int fs_idx,uint32 flags);
extern int _bcm_vp_ing_dvp_config(int unit, _bcm_vp_ing_dvp_config_op_t op, 
                                           int vp,int vp_type, bcm_if_t intf, 
                                           int network_port);
extern int _bcm_vp_ing_dvp_to_ing_dvp2(int unit, ing_dvp_table_entry_t *dvp,
        int vp);
extern int _bcm_vp_ing_dvp_to_ing_dvp2_2(int unit,
        ing_dvp_table_entry_t *dvp, int vp, int network_group_id);
extern int
_bcm_validate_splithorizon_network_group(int unit,int nw_flag,
        int *network_group);
extern int _bcm_vp_encode_gport(int unit, int vp, int *gport);
extern int _bcm_vp_is_vfi_type(int unit, bcm_gport_t gport);
extern int _bcm_vp_vfi_type_vp_get(int unit, bcm_gport_t gport, int *vp);
extern int _bcm_vp_lag_member_is_vfi_type(int unit, bcm_gport_t gport);
extern void _bcm_vp_vfi_set(int unit, int vp, int vfi);
extern int _bcm_vp_vfi_get(int unit, int vp);
#endif /* INCLUDE_L3 */
extern int
_bcm_switch_network_group_pruning(int unit,
     bcm_switch_network_group_t source_network_group_id,
     bcm_switch_network_group_config_t *config);


#endif /* BCM_TRX_SUPPORT */
#endif  /* !_BCM_INT_RESOURCE_H_ */


