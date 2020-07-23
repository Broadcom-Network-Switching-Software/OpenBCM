/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vlan.c
 * Purpose: Handle trident2plus specific vlan features:
 *             Manages VFI VLAN membership tables.
 */

#include <soc/defs.h>
#include <soc/field.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_TRIDENT2_SUPPORT)

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/ptable.h>
#include <soc/drv.h>
#include <soc/td2_td2p.h>
#include <soc/mcm/memacc.h>
#include <bcm/error.h>
#include <bcm/port.h>

#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/virtual.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif

#define BCMI_MAX_COE_MODULE_ID         255
#define BCMI_NUM_PORTS_PER_COE_MODULE  128

#define BCMI_VP_GROUP_VP_OFFSET    ((BCMI_MAX_COE_MODULE_ID * \
                                      BCMI_NUM_PORTS_PER_COE_MODULE) - 1)

#define BCMI_VP_GROUP_GLP_COUNT    ((BCMI_MAX_COE_MODULE_ID * \
                                      BCMI_NUM_PORTS_PER_COE_MODULE))

#define BCMI_VP_GROUP_GLP_MODID_SHIFT    7
#define BCMI_VP_GROUP_GLP_MODID_MASK     0xff
#define BCMI_VP_GROUP_GLP_PORT_SHIFT     0
#define BCMI_VP_GROUP_GLP_PORT_MASK      0x7f

#define BCMI_VP_GROUP_SUBPORT_MODID_GET(_gport)    \
        (((_gport) >> BCMI_VP_GROUP_GLP_MODID_SHIFT) & \
                    BCMI_VP_GROUP_GLP_MODID_MASK)

#define BCMI_VP_GROUP_SUBPORT_PORT_GET(_gport)     \
        (((_gport) >> BCMI_VP_GROUP_GLP_PORT_SHIFT) & \
                    BCMI_VP_GROUP_GLP_PORT_MASK)

#define VP_FILTER_DISABLE      0
#define VP_FILTER_VP_GRP_BMAP  1
#define VP_FILTER_HASH         2
#define VP_DO_NOT_CHECK        3

#define GPP_FILTER_DISABLE      0
#define GPP_FILTER_ING_PORT     1
#define GPP_FILTER_HASH         2
#define GPP_FILTER_VP_GRP_BMAP  3

#define _BCM_DEFAULT_STP_STATE        0 /* Disabled */
#define _BCM_DEFAULT_UNTAGGED_STATE   0

#define _BCM_INVALID_STG            -1
#define _BCM_INVALID_VP_GROUP       -1
#define _BCM_INVALID_VFI_PROFILE    -1

#define BCM_MN_VPN_MAX             2047
#define BCM_MN_VPN_COUNT           (BCM_MN_VPN_MAX - BCM_VPN_MIN + 1)

STATIC soc_field_t _bcm_vp_group_stp_field[64] = {
    SP_TREE_VP_GRP0f,
    SP_TREE_VP_GRP1f,
    SP_TREE_VP_GRP2f,
    SP_TREE_VP_GRP3f,
    SP_TREE_VP_GRP4f,
    SP_TREE_VP_GRP5f,
    SP_TREE_VP_GRP6f,
    SP_TREE_VP_GRP7f,
    SP_TREE_VP_GRP8f,
    SP_TREE_VP_GRP9f,
    SP_TREE_VP_GRP10f,
    SP_TREE_VP_GRP11f,
    SP_TREE_VP_GRP12f,
    SP_TREE_VP_GRP13f,
    SP_TREE_VP_GRP14f,
    SP_TREE_VP_GRP15f,
    SP_TREE_VP_GRP16f,
    SP_TREE_VP_GRP17f,
    SP_TREE_VP_GRP18f,
    SP_TREE_VP_GRP19f,
    SP_TREE_VP_GRP20f,
    SP_TREE_VP_GRP21f,
    SP_TREE_VP_GRP22f,
    SP_TREE_VP_GRP23f,
    SP_TREE_VP_GRP24f,
    SP_TREE_VP_GRP25f,
    SP_TREE_VP_GRP26f,
    SP_TREE_VP_GRP27f,
    SP_TREE_VP_GRP28f,
    SP_TREE_VP_GRP29f,
    SP_TREE_VP_GRP30f,
    SP_TREE_VP_GRP31f,
    SP_TREE_VP_GRP32f,
    SP_TREE_VP_GRP33f,
    SP_TREE_VP_GRP34f,
    SP_TREE_VP_GRP35f,
    SP_TREE_VP_GRP36f,
    SP_TREE_VP_GRP37f,
    SP_TREE_VP_GRP38f,
    SP_TREE_VP_GRP39f,
    SP_TREE_VP_GRP40f,
    SP_TREE_VP_GRP41f,
    SP_TREE_VP_GRP42f,
    SP_TREE_VP_GRP43f,
    SP_TREE_VP_GRP44f,
    SP_TREE_VP_GRP45f,
    SP_TREE_VP_GRP46f,
    SP_TREE_VP_GRP47f,
    SP_TREE_VP_GRP48f,
    SP_TREE_VP_GRP49f,
    SP_TREE_VP_GRP50f,
    SP_TREE_VP_GRP51f,
    SP_TREE_VP_GRP52f,
    SP_TREE_VP_GRP53f,
    SP_TREE_VP_GRP54f,
    SP_TREE_VP_GRP55f,
    SP_TREE_VP_GRP56f,
    SP_TREE_VP_GRP57f,
    SP_TREE_VP_GRP58f,
    SP_TREE_VP_GRP59f,
    SP_TREE_VP_GRP60f,
    SP_TREE_VP_GRP61f,
    SP_TREE_VP_GRP62f,
    SP_TREE_VP_GRP63f
};

/* Internal functions */
STATIC int _bcm_td2p_vlan_vfi_mbrship_profile_entry_set(int unit,
                                              bcm_vlan_t vlan_vpn,
                                              int egress,
                                              void *entry_data);
STATIC int _bcm_td2p_vlan_vpn_mbrship_profile_get(int unit,
                                            bcm_vlan_t vlan_vpn,
                                            int egress, 
                                            uint32 *prof_index);
STATIC int _bcm_td2p_vlan_vpn_mbrship_profile_set(int unit,
                                            bcm_vlan_t vlan_vpn,
                                            int egress, 
                                            int prof_index);

STATIC int bcm_td2p_vp_group_port_filter_get(int unit, int port,
                                            int egress, int *vp_group, int *filter);
STATIC int _bcm_td2p_gpp_hw_index_get(int unit, int port, int *hw_index);
STATIC int _bcm_td2p_vp_group_resolve_port_num(int unit, int port, int *port_offset);


/* --------------------------------------------------------
 * Software book keeping for virtual port group information
 * --------------------------------------------------------
 */

typedef struct _bcm_td2p_vp_group_s {
    int gpp_vp_count;          /* Number of VPs and GPPs that belong to this VP group */
    int stp_state;
    int ut_state;

    SHR_BITDCL *gpp_vp_bitmap; /* Bitmap of VPs and GPPs that belong to this VP group */
    SHR_BITDCL *vlan_vfi_bitmap;  /* VLANs and VFIs this VP group belongs to */
} _bcm_td2p_vp_group_t;


typedef struct _bcm_td2p_vp_group_bk_s {
    int vp_group_initialized; /* Flag to check initialized status */
    int num_ing_vp_group;     /* Number of ingress VP groups */
    _bcm_td2p_vp_group_t *ing_vp_group_array; /* Ingress VP group array */
    int num_eg_vp_group;      /* Number of egress VP groups */
    _bcm_td2p_vp_group_t *eg_vp_group_array; /* Egress VP group array */
} _bcm_td2p_vp_group_bk_t;

typedef struct {
    int ingress;
    int egress;
} _bcm_td2p_vp_group_unmanaged_t;

STATIC _bcm_td2p_vp_group_bk_t _bcm_td2p_vp_group_bk[BCM_MAX_NUM_UNITS];

STATIC _bcm_td2p_vp_group_unmanaged_t 
        _bcm_td2p_vp_group_unmanaged[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _bcm_vp_group_ing_mutex[BCM_MAX_NUM_UNITS] = {NULL};
STATIC sal_mutex_t _bcm_vp_group_egr_mutex[BCM_MAX_NUM_UNITS] = {NULL};


/* 
 * VP Group resource lock
 */
#define VP_GROUP_ING_LOCK(unit) \
        sal_mutex_take(_bcm_vp_group_ing_mutex[unit], sal_mutex_FOREVER); 

#define VP_GROUP_EGR_LOCK(unit) \
        sal_mutex_take(_bcm_vp_group_egr_mutex[unit], sal_mutex_FOREVER); 

#define VP_GROUP_ING_UNLOCK(unit) \
        sal_mutex_give(_bcm_vp_group_ing_mutex[unit]); 

#define VP_GROUP_EGR_UNLOCK(unit) \
        sal_mutex_give(_bcm_vp_group_egr_mutex[unit]); 

#define VP_GROUP_LOCK_MEMS(unit, egress) \
        if(!egress) { \
            VP_GROUP_ING_LOCK(unit); \
        } else { \
            VP_GROUP_EGR_LOCK(unit); \
        }

#define VP_GROUP_UNLOCK_MEMS(unit, egress) \
        if(!egress) { \
            VP_GROUP_ING_UNLOCK(unit); \
        } else { \
            VP_GROUP_EGR_UNLOCK(unit); \
        }

const char * _bcm_vp_group_access_type_strs[] = {
    "GLP",
    "VP",
    "STG"
};

const char * _bcm_vp_group_port_type_strs[] = {
    "Front-Panel",
    "GLP",
    "VP"
};

#define VP_GROUP_BK(unit) (&_bcm_td2p_vp_group_bk[unit])

#define VP_GROUP_INIT(unit)                              \
    do {                                                 \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) { \
            return BCM_E_UNIT;                           \
        }                                                \
        if (!VP_GROUP_BK(unit)->vp_group_initialized) {  \
            return BCM_E_INIT;                           \
        }                                                \
    } while (0)

#define ING_VP_GROUP(unit, vp_group) \
    (&VP_GROUP_BK(unit)->ing_vp_group_array[vp_group])

#define EG_VP_GROUP(unit, vp_group) \
    (&VP_GROUP_BK(unit)->eg_vp_group_array[vp_group])

#define ING_STG_GROUP_DATA(unit, stg) \
    (&VP_GROUP_BK(unit)->ing_vp_group_array[vp_group])

#define TD2P_EGR_DVP_ATTRIBUTE_FIELD(_type,_sf) \
        (_type) == 1 ? TRILL__##_sf: \
        (_type) == 2 ? VXLAN__##_sf: \
        (_type) == 3 ? L2GRE__##_sf: \
        COMMON__##_sf;

#define VP_GROUP_SP_TREE_FIELD(_vp_group) \
        SP_TREE_VP_GRP##_vp_group##f

#define VP_GROUP_CLEANUP(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) { goto cleanup; } } while(0)

#endif /* defined(BCM_TRIDENT2_SUPPORT) */

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_td2p_vp_group_free_resources
 * Purpose:
 *      Free VP group data structures. 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_td2p_vp_group_free_resources(int unit)
{
    int num_vp_groups, i;

    if (VP_GROUP_BK(unit)->ing_vp_group_array) {
        num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);
        for (i = 0; i < num_vp_groups; i++) {
            if (ING_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                sal_free(ING_VP_GROUP(unit, i)->gpp_vp_bitmap);
                ING_VP_GROUP(unit, i)->gpp_vp_bitmap = NULL;
            }
            if (ING_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                sal_free(ING_VP_GROUP(unit, i)->vlan_vfi_bitmap);
                ING_VP_GROUP(unit, i)->vlan_vfi_bitmap = NULL;
            }
        }
        sal_free(VP_GROUP_BK(unit)->ing_vp_group_array);
        VP_GROUP_BK(unit)->ing_vp_group_array = NULL;
    }

    if (_bcm_vp_group_ing_mutex[unit]) {
        sal_mutex_destroy(_bcm_vp_group_ing_mutex[unit]);
        _bcm_vp_group_ing_mutex[unit] = NULL;
    }

    if (VP_GROUP_BK(unit)->eg_vp_group_array) {
        num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);
        for (i = 0; i < num_vp_groups; i++) {
            if (EG_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                sal_free(EG_VP_GROUP(unit, i)->gpp_vp_bitmap);
                EG_VP_GROUP(unit, i)->gpp_vp_bitmap = NULL;
            }
            if (EG_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                sal_free(EG_VP_GROUP(unit, i)->vlan_vfi_bitmap);
                EG_VP_GROUP(unit, i)->vlan_vfi_bitmap = NULL;
            }
        }
        sal_free(VP_GROUP_BK(unit)->eg_vp_group_array);
        VP_GROUP_BK(unit)->eg_vp_group_array = NULL;
    }

    if (_bcm_vp_group_egr_mutex[unit]) {
        sal_mutex_destroy(_bcm_vp_group_egr_mutex[unit]);
        _bcm_vp_group_egr_mutex[unit] = NULL;
    }

    return;
}

/*
 * Function:
 *      _td2p_egr_dvp_attribute_field_name_get
 * Purpose:
 *      Retrieve the proper field name based the device and vp type.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_td2p_egr_dvp_attribute_field_name_get(int unit,
                         void *dvp_entry,
                         soc_field_t legacy_name,
                         soc_field_t *result_name)
{
    int vp_type;
    soc_field_t vpt_fld = DATA_TYPEf;

    if (soc_mem_field_valid(unit, EGR_DVP_ATTRIBUTEm, VP_TYPEf)) {
        vpt_fld = VP_TYPEf;
    }

    vp_type = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit,
                (egr_dvp_attribute_entry_t *)dvp_entry, vpt_fld);
    switch (legacy_name) {
        case EN_EFILTERf:
            *result_name = TD2P_EGR_DVP_ATTRIBUTE_FIELD(vp_type, EN_EFILTERf);
            break;
        case VLAN_MEMBERSHIP_PROFILEf:
            *result_name = TD2P_EGR_DVP_ATTRIBUTE_FIELD(vp_type,
                    VLAN_MEMBERSHIP_PROFILEf);
            break;
        default:
            return BCM_E_NOT_FOUND;
            break;
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_td2p_vp_group_reinit
 * Purpose:
 *      Recover the virtual port group data structures.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_reinit(int unit)
{
    

    int rv = BCM_E_NONE;
    uint8 *source_vp_buf = NULL;
    uint8 *vlan_buf = NULL;
    uint8 *egr_dvp_buf = NULL;
    uint8 *vlan_vfi_mbrship_buf = NULL;
    uint8 *vlan_mpls_buf = NULL;
    int profile_ptr = 0;
    uint8 *egr_vlan_buf = NULL;
    int index_min, index_max;
    int i, k;
    source_vp_entry_t *svp_entry;
    int vp_group;
    vlan_tab_entry_t *vlan_entry = NULL;
    vlan_mpls_entry_t *vlan_mpls_entry;
#if defined(BCM_TRIDENT3_SUPPORT)
    vlan_attrs_1_entry_t *vlan_attrs_entry;
#endif
    soc_mem_t vlan_mpls_mem = VLAN_MPLSm;
    uint32 fldbuf[2];
    egr_dvp_attribute_entry_t *egr_dvp_entry = NULL;
    egr_vlan_entry_t *egr_vlan_entry = NULL;
    ing_vlan_vfi_membership_entry_t *ing_vlan_vfi_mbrship = NULL;
    egr_vlan_vfi_membership_entry_t *egr_vlan_vfi_mbrship = NULL;
    soc_field_t en_efilter_f;
    soc_field_t vm_prof_f;
    uint8                *vfi_buf       = NULL;
    vfi_entry_t          *vfi_entry     = NULL;
    egr_vfi_entry_t      *egr_vfi_entry = NULL;
    uint8                *stg_buf       = NULL;
    stg_tab_entry_t      *stg_entry     = NULL;
    egr_vlan_stg_entry_t *egr_stg_entry = NULL;
    int                   stg           = BCM_STG_DEFAULT;
    uint32                ut_grp_bmap[SOC_PBMP_WORD_MAX] = {0};

    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {

        if(!bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
            /* Recover ingress VP group's virtual ports from SOURCE_VP table */

            source_vp_buf = soc_cm_salloc(unit,
                    SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm), "SOURCE_VP buffer");
            if (NULL == source_vp_buf) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            index_min = soc_mem_index_min(unit, SOURCE_VPm);
            index_max = soc_mem_index_max(unit, SOURCE_VPm);
            rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY,
                    index_min, index_max, source_vp_buf);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            for (i = index_min; i <= index_max; i++) {
                svp_entry = soc_mem_table_idx_to_pointer
                    (unit, SOURCE_VPm, source_vp_entry_t *, source_vp_buf, i);

                if (soc_SOURCE_VPm_field32_get(unit, svp_entry,
                                               ENABLE_IFILTERf) != 1) {
                    continue;
                }

                vp_group = soc_SOURCE_VPm_field32_get(unit, svp_entry,
                        VLAN_MEMBERSHIP_PROFILEf);
                SHR_BITSET(ING_VP_GROUP(unit, vp_group)->gpp_vp_bitmap,
                           BCMI_VP_GROUP_VP_OFFSET + i);
                ING_VP_GROUP(unit, vp_group)->gpp_vp_count++;
            }

            soc_cm_sfree(unit, source_vp_buf);
            source_vp_buf = NULL;
        }
        /* Recover ingress VP group's vlans from VLAN table */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            vlan_mpls_mem = VLAN_ATTRS_1m;
        }
#endif
        vlan_mpls_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, vlan_mpls_mem), "VLAN_MPLS buffer");
        if (NULL == vlan_mpls_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, vlan_mpls_mem);
        index_max = soc_mem_index_max(unit, vlan_mpls_mem);
        rv = soc_mem_read_range(unit, vlan_mpls_mem, MEM_BLOCK_ANY,
                index_min, index_max, vlan_mpls_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        vlan_vfi_mbrship_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, ING_VLAN_VFI_MEMBERSHIPm), 
                "ING_VLAN_VFI buffer");
        if (NULL == vlan_vfi_mbrship_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, ING_VLAN_VFI_MEMBERSHIPm);
        index_max = soc_mem_index_max(unit, ING_VLAN_VFI_MEMBERSHIPm);
        rv = soc_mem_read_range(unit, ING_VLAN_VFI_MEMBERSHIPm, 
                 MEM_BLOCK_ANY, index_min, index_max, vlan_vfi_mbrship_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        stg_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, STG_TABm),
                                "STG_TAB buffer");
        if (NULL == stg_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, STG_TABm);
        index_max = soc_mem_index_max(unit, STG_TABm);
        rv = soc_mem_read_range(unit, STG_TABm, MEM_BLOCK_ANY,
                                index_min, index_max, stg_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        vlan_buf = soc_cm_salloc(unit,
                                 SOC_MEM_TABLE_BYTES(unit, VLAN_TABm),
                                 "VLAN_TAB buffer");
        if (NULL == vlan_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, VLAN_TABm);
        index_max = soc_mem_index_max(unit, VLAN_TABm);
        rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
                                index_min, index_max, vlan_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            vlan_entry = soc_mem_table_idx_to_pointer
                (unit, VLAN_TABm, vlan_tab_entry_t *, vlan_buf, i);

            if (soc_VLAN_TABm_field32_get(unit, vlan_entry, VALIDf) == 0) {
                continue;
            }

            if (soc_mem_is_valid(unit, VLAN_MPLSm)) {
                vlan_mpls_entry = soc_mem_table_idx_to_pointer
                    (unit, vlan_mpls_mem, vlan_mpls_entry_t *, vlan_mpls_buf, i);
                profile_ptr = soc_mem_field32_get(unit, vlan_mpls_mem, vlan_mpls_entry,
                                           MEMBERSHIP_PROFILE_PTRf);
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            else if (soc_mem_is_valid(unit, VLAN_ATTRS_1m)) {
                vlan_attrs_entry = soc_mem_table_idx_to_pointer
                    (unit, vlan_mpls_mem, vlan_attrs_1_entry_t *, vlan_mpls_buf, i);
                profile_ptr = soc_mem_field32_get(unit, vlan_mpls_mem, vlan_attrs_entry,
                        MEMBERSHIP_PROFILE_PTRf);
            }
#endif

            rv = _bcm_vlan_vfi_membership_profile_mem_reference(
                     unit, profile_ptr, 1, 0);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            if (bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                continue;
            }

            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
               if (soc_VLAN_TABm_field32_get(unit, vlan_entry,
                           VIRTUAL_PORT_ENf) == 0) {
                   continue;
               }
            }

            stg = soc_VLAN_TABm_field32_get(unit, vlan_entry, STGf);

            ing_vlan_vfi_mbrship = soc_mem_table_idx_to_pointer(unit, 
                    ING_VLAN_VFI_MEMBERSHIPm, 
                    ing_vlan_vfi_membership_entry_t *,
                    vlan_vfi_mbrship_buf, profile_ptr);
            soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, ing_vlan_vfi_mbrship,
                    VP_GROUP_BITMAPf, fldbuf);

            for (k = 0; k < VP_GROUP_BK(unit)->num_ing_vp_group; k++) {
                if (fldbuf[k / 32] & (1 << (k % 32))) {
                    /* The bit in VP_GROUP_BITMAP that corresponds to
                     * VP group k is set.
                     */
                    SHR_BITSET(ING_VP_GROUP(unit, k)->vlan_vfi_bitmap, i);

                    if (stg != BCM_STG_DEFAULT) {
                        stg_entry =
                            soc_mem_table_idx_to_pointer(unit, STG_TABm,
                                                         stg_tab_entry_t *,
                                                         stg_buf, stg);
                        ING_VP_GROUP(unit, k)->stp_state =
                            soc_STG_TABm_field32_get(unit, stg_entry,
                                                     _bcm_vp_group_stp_field[k]);
                    }
                }
            }
        }

        soc_cm_sfree(unit, vlan_buf);
        soc_cm_sfree(unit, vlan_mpls_buf);
        vlan_buf = NULL;
        vlan_mpls_buf = NULL;

        vfi_buf =
            soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VFIm), "VFI buffer");
        if (NULL == vfi_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, VFIm);
        index_max = soc_mem_index_max(unit, VFIm);
        rv = soc_mem_read_range(unit, VFIm, MEM_BLOCK_ANY,
                                index_min, index_max, vfi_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            vfi_entry =
                soc_mem_table_idx_to_pointer(unit, VFIm, vfi_entry_t *,
                                             vfi_buf, i);
            profile_ptr =
                soc_VFIm_field32_get(unit, vfi_entry,
                                     MEMBERSHIP_PROFILE_PTRf);
            rv = _bcm_vlan_vfi_membership_profile_mem_reference(
                     unit, profile_ptr, 1, 0);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            if (bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                continue;
            }

            stg = soc_VFIm_field32_get(unit, vfi_entry, STGf);

            ing_vlan_vfi_mbrship =
                soc_mem_table_idx_to_pointer(unit,
                                             ING_VLAN_VFI_MEMBERSHIPm,
                                             ing_vlan_vfi_membership_entry_t *,
                                             vlan_vfi_mbrship_buf, profile_ptr);

            soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, ing_vlan_vfi_mbrship,
                                                   VP_GROUP_BITMAPf, fldbuf);
            for (k = 0; k < VP_GROUP_BK(unit)->num_ing_vp_group; k++) {
                if (fldbuf[k / 32] & (1 << (k % 32))) {
                    /* The bit in VP_GROUP_BITMAP that corresponds to
                     * VP group k is set.
                     */
                    SHR_BITSET(ING_VP_GROUP(unit, k)->vlan_vfi_bitmap,
                               BCM_VLAN_COUNT + i);

                    if (stg != BCM_STG_DEFAULT) {
                        stg_entry =
                            soc_mem_table_idx_to_pointer(unit, STG_TABm,
                                                         stg_tab_entry_t *,
                                                         stg_buf, stg);
                        ING_VP_GROUP(unit, k)->stp_state =
                            soc_STG_TABm_field32_get(unit, stg_entry,
                                _bcm_vp_group_stp_field[k]);
                    }
                }
            }
        }

        soc_cm_sfree(unit, vfi_buf);
        soc_cm_sfree(unit, vlan_vfi_mbrship_buf);
        soc_cm_sfree(unit, stg_buf);
        vlan_vfi_mbrship_buf = NULL;
        vfi_buf = NULL;
        stg_buf = NULL;
    }

    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        if (!bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
            /* Recover egress VP group's virtual ports from
             * EGR_DVP_ATTRIBUTE table
             */

            egr_dvp_buf = soc_cm_salloc(unit,
                    SOC_MEM_TABLE_BYTES(unit, EGR_DVP_ATTRIBUTEm),
                    "EGR_DVP buffer");
            if (NULL == egr_dvp_buf) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            index_min = soc_mem_index_min(unit, EGR_DVP_ATTRIBUTEm);
            index_max = soc_mem_index_max(unit, EGR_DVP_ATTRIBUTEm);
            rv = soc_mem_read_range(unit, EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ANY,
                    index_min, index_max, egr_dvp_buf);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            for (i = index_min; i <= index_max; i++) {
                egr_dvp_entry = soc_mem_table_idx_to_pointer
                    (unit, EGR_DVP_ATTRIBUTEm, egr_dvp_attribute_entry_t *,
                     egr_dvp_buf, i);

                BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                             egr_dvp_entry, EN_EFILTERf, &en_efilter_f));

                if (soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, egr_dvp_entry,
                                                       en_efilter_f) != 1) {
                    continue;
                }

                BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                             egr_dvp_entry, VLAN_MEMBERSHIP_PROFILEf,
                             &vm_prof_f));

                vp_group = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, egr_dvp_entry,
                        vm_prof_f);
                SHR_BITSET(EG_VP_GROUP(unit, vp_group)->gpp_vp_bitmap,
                           BCMI_VP_GROUP_VP_OFFSET + i);
                EG_VP_GROUP(unit, vp_group)->gpp_vp_count++;
            }

            soc_cm_sfree(unit, egr_dvp_buf);
            egr_dvp_buf = NULL;
        }
        /* Recover egress VP group's vlans from EGR_VLAN table */

        vlan_vfi_mbrship_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, EGR_VLAN_VFI_MEMBERSHIPm), 
                "EGR_VLAN_VFI buffer");
        if (NULL == vlan_vfi_mbrship_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, EGR_VLAN_VFI_MEMBERSHIPm);
        index_max = soc_mem_index_max(unit, EGR_VLAN_VFI_MEMBERSHIPm);
        rv = soc_mem_read_range(unit, EGR_VLAN_VFI_MEMBERSHIPm, 
                 MEM_BLOCK_ANY, index_min, index_max, vlan_vfi_mbrship_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        stg_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, EGR_VLAN_STGm),
                                "EGR_VLAN_STG buffer");
        if (NULL == stg_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, EGR_VLAN_STGm);
        index_max = soc_mem_index_max(unit, EGR_VLAN_STGm);
        rv = soc_mem_read_range(unit, EGR_VLAN_STGm, MEM_BLOCK_ANY,
                                index_min, index_max, stg_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        egr_vlan_buf = soc_cm_salloc(unit,
                                     SOC_MEM_TABLE_BYTES(unit, EGR_VLANm),
                                     "EGR_VLAN buffer");
        if (NULL == egr_vlan_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, EGR_VLANm);
        index_max = soc_mem_index_max(unit, EGR_VLANm);
        rv = soc_mem_read_range(unit, EGR_VLANm, MEM_BLOCK_ANY,
                                index_min, index_max, egr_vlan_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            egr_vlan_entry = soc_mem_table_idx_to_pointer
                (unit, EGR_VLANm, egr_vlan_entry_t *, egr_vlan_buf, i);

            if (soc_EGR_VLANm_field32_get(unit, egr_vlan_entry, VALIDf) == 0) {
                continue;
            }

            profile_ptr = soc_EGR_VLANm_field32_get(unit, egr_vlan_entry,
                    MEMBERSHIP_PROFILE_PTRf);
            rv =_bcm_vlan_vfi_membership_profile_mem_reference
                    (unit, profile_ptr, 1, 1);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            if (bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                continue;
            }

            stg = soc_EGR_VLANm_field32_get(unit, egr_vlan_entry, STGf);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
                uint32 prof_index;
                egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
                prof_index = soc_mem_field32_get(unit, EGR_VLANm,
                                egr_vlan_entry, UNTAG_PROFILE_PTRf);
                rv = READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index,
                                              &egr_vlan_vfi);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }
                soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
                                       UT_VP_GRP_BITMAPf, (void *)ut_grp_bmap);
                rv = _bcm_vlan_vfi_untag_profile_mem_reference(unit,
                        prof_index, 1);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }
            } else
#endif
            {
                soc_EGR_VLANm_field_get(unit, egr_vlan_entry,
                                        UT_VP_GRP_BITMAPf, ut_grp_bmap);
            }

            egr_vlan_vfi_mbrship = soc_mem_table_idx_to_pointer(unit,
                    EGR_VLAN_VFI_MEMBERSHIPm, 
                    egr_vlan_vfi_membership_entry_t *,
                    vlan_vfi_mbrship_buf, profile_ptr);
            soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, egr_vlan_vfi_mbrship,
                        VP_GROUP_BITMAPf, fldbuf);

            for (k = 0; k < VP_GROUP_BK(unit)->num_eg_vp_group; k++) {
                if (fldbuf[k / 32] & (1 << (k % 32))) {
                    /* The bit in VP_GROUP_BITMAP that corresponds to
                     * VP group k is set.
                     */
                    SHR_BITSET(EG_VP_GROUP(unit, k)->vlan_vfi_bitmap, i);

                    if (stg != BCM_STG_DEFAULT) {
                        egr_stg_entry =
                            soc_mem_table_idx_to_pointer(unit, EGR_VLAN_STGm,
                                                         egr_vlan_stg_entry_t *,
                                                         stg_buf, stg);
                        EG_VP_GROUP(unit, k)->stp_state =
                            soc_EGR_VLAN_STGm_field32_get(unit, egr_stg_entry,
                                _bcm_vp_group_stp_field[k]);
                    }

                    EG_VP_GROUP(unit, k)->ut_state =
                        (ut_grp_bmap[k / 32] & (1 << (k % 32)));

                }
            }
        }
        
        soc_cm_sfree(unit, egr_vlan_buf);
        egr_vlan_buf = NULL;

        vfi_buf =
            soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, EGR_VFIm),
                          "EGR_VFI buffer");
        if (NULL == vfi_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, EGR_VFIm);
        index_max = soc_mem_index_max(unit, EGR_VFIm);
        rv = soc_mem_read_range(unit, EGR_VFIm, MEM_BLOCK_ANY,
                                index_min, index_max, vfi_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            egr_vfi_entry =
                soc_mem_table_idx_to_pointer(unit, EGR_VFIm, egr_vfi_entry_t *,
                                             vfi_buf, i);
            profile_ptr =
                soc_EGR_VFIm_field32_get(unit, egr_vfi_entry,
                                         MEMBERSHIP_PROFILE_PTRf);
            rv = _bcm_vlan_vfi_membership_profile_mem_reference(
                     unit, profile_ptr, 1, 1);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            if (bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                continue;
            }

            stg = soc_EGR_VFIm_field32_get(unit, egr_vfi_entry, STGf);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
                uint32 prof_index;
                egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
                prof_index = soc_mem_field32_get(unit, EGR_VFIm,
                                egr_vfi_entry, UNTAG_PROFILE_PTRf);
                rv = READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index,
                                              &egr_vlan_vfi);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }
                soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
                                       UT_VP_GRP_BITMAPf, (void *)ut_grp_bmap);
                rv = _bcm_vlan_vfi_untag_profile_mem_reference(unit,
                        prof_index, 1);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }
            }
#endif
            egr_vlan_vfi_mbrship =
                soc_mem_table_idx_to_pointer(unit,
                                             EGR_VLAN_VFI_MEMBERSHIPm,
                                             egr_vlan_vfi_membership_entry_t *,
                                             vlan_vfi_mbrship_buf, profile_ptr);
            soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, egr_vlan_vfi_mbrship,
                                                   VP_GROUP_BITMAPf, fldbuf);

            for (k = 0; k < VP_GROUP_BK(unit)->num_eg_vp_group; k++) {
                if (fldbuf[k / 32] & (1 << (k % 32))) {
                    /* The bit in VP_GROUP_BITMAP that corresponds to
                     * VP group k is set.
                     */
                    SHR_BITSET(EG_VP_GROUP(unit, k)->vlan_vfi_bitmap,
                               BCM_VLAN_COUNT + i);

                    if (stg != BCM_STG_DEFAULT) {
                        egr_stg_entry =
                            soc_mem_table_idx_to_pointer(unit, EGR_VLAN_STGm,
                                                         egr_vlan_stg_entry_t *,
                                                         stg_buf, stg);
                        EG_VP_GROUP(unit, k)->stp_state =
                            soc_EGR_VLAN_STGm_field32_get(unit, egr_stg_entry,
                                _bcm_vp_group_stp_field[k]);
                    }
#if defined(BCM_TRIDENT3_SUPPORT)
                    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
                        EG_VP_GROUP(unit, k)->ut_state =
                            (ut_grp_bmap[k / 32] & (1 << (k % 32)));
                    }
#endif
                }
            }
        }

        soc_cm_sfree(unit, vfi_buf);
        soc_cm_sfree(unit, vlan_vfi_mbrship_buf);
        soc_cm_sfree(unit, stg_buf);
        vlan_vfi_mbrship_buf = NULL;
        vfi_buf = NULL;
        stg_buf = NULL;
    }

cleanup:
    if (source_vp_buf) {
        soc_cm_sfree(unit, source_vp_buf);
    }
    if (vlan_buf) {
        soc_cm_sfree(unit, vlan_buf);
    }
    if (vlan_mpls_buf) {
        soc_cm_sfree(unit, vlan_mpls_buf);
    }
    if (egr_dvp_buf) {
        soc_cm_sfree(unit, egr_dvp_buf);
    }
    if (egr_vlan_buf) {
        soc_cm_sfree(unit, egr_vlan_buf);
    }
    if (vlan_vfi_mbrship_buf) {
        soc_cm_sfree(unit, vlan_vfi_mbrship_buf);
    }
    if (vfi_buf) {
        soc_cm_sfree(unit, vfi_buf);
    }
    if (stg_buf) {
        soc_cm_sfree(unit, stg_buf);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_td2p_vp_group_sw_dump
 * Purpose:
 *     Displays VP group information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_td2p_vp_group_sw_dump(int unit)
{
    int i, k;
    int vlan_vfi_count;
    int gpp_vp_bit_size, num_gpp;
    int gpp_vp_count, stp_state, ut_state;

    vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);
    num_gpp = BCMI_MAX_COE_MODULE_ID * BCMI_NUM_PORTS_PER_COE_MODULE;
    gpp_vp_bit_size = soc_mem_index_count(unit, SOURCE_VPm) + num_gpp;

    cli_out("\nSW Information Ingress VP Group - Unit %d\n", unit);
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        gpp_vp_count = ING_VP_GROUP(unit, i)->gpp_vp_count;
        stp_state = ING_VP_GROUP(unit, i)->stp_state;
        ut_state = ING_VP_GROUP(unit, i)->ut_state;

        if (gpp_vp_count == 0 && stp_state == 0 &&
            ut_state == 0) {
            continue;
        }
        cli_out("\n  Ingress Group = %d, ", i);
        cli_out("GPP_VP Count = %d, ", gpp_vp_count);
        cli_out("STP State = %d, ", stp_state);
        cli_out("Untag State = %d\n", ut_state);

        cli_out("    GPP_VP List =");
        for (k = 0; k < gpp_vp_bit_size; k++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, i)->gpp_vp_bitmap, k)) {

                if(k < num_gpp) {
                    cli_out(" GPP (mod,port) (%d,%d)", (k % BCMI_MAX_COE_MODULE_ID),
                            (k - (k % BCMI_MAX_COE_MODULE_ID)));
                } else {
                    cli_out(" VP %d", k);
                }
            }
        }
        cli_out("\n");

        cli_out("    VLAN/VFI List =");
        for (k = 0; k < vlan_vfi_count; k++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, i)->vlan_vfi_bitmap, k)) {
                if(k <= BCM_VLAN_MAX) {
                    cli_out(" vlan %d", k);
                } else {
                    cli_out(" vfi %d", k);
                }
            }
        }
        cli_out("\n");
    }

    cli_out("\nSW Information Egress VP Group - Unit %d\n", unit);
    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        gpp_vp_count = EG_VP_GROUP(unit, i)->gpp_vp_count;
        stp_state = EG_VP_GROUP(unit, i)->stp_state;
        ut_state = EG_VP_GROUP(unit, i)->ut_state;

        if (gpp_vp_count == 0 && stp_state == 0 &&
            ut_state == 0) {
            continue;
        }
        cli_out("\n  Egress VP Group = %d, ", i);
        cli_out("GPP_VP Count = %d, ", gpp_vp_count);
        cli_out("STP State = %d, ", stp_state);
        cli_out("Untag State = %d\n", ut_state);

        cli_out("    GPP/VP List =");
        for (k = 0; k < gpp_vp_bit_size; k++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, i)->gpp_vp_bitmap, k)) {

                if(k < num_gpp) {
                    cli_out(" GPP (mod,port) (%d,%d)", (k % BCMI_MAX_COE_MODULE_ID),
                            (k - (k % BCMI_MAX_COE_MODULE_ID)));
                } else {
                    cli_out(" VP %d", k);
                }
            }
        }
        cli_out("\n");

        cli_out("    VLAN List =");
        for (k = 0; k < vlan_vfi_count; k++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, i)->vlan_vfi_bitmap, k)) {
                if(k <= BCM_VLAN_MAX) {
                    cli_out(" vlan %d", k);
                } else {
                    cli_out(" vfi %d", k);
                }
            }
        }
        cli_out("\n");
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      bcm_td2p_vp_group_init
 * Purpose:
 *      Initialize the virtual port group data structures.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_init(int unit)
{
    int vlan_vfi_count;
    int num_vp_groups = 0, num_vp = 0; 
    int num_gpp = 0, i = 0;
    int rv = BCM_E_NONE;
    uint32 reg_value = 0;

    _bcm_td2p_vp_group_free_resources(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
    } else
#endif
    {
        _bcm_td2p_vp_group_unmanaged[unit].ingress = FALSE;
        _bcm_td2p_vp_group_unmanaged[unit].egress  = FALSE;
    }
    sal_memset(VP_GROUP_BK(unit), 0, sizeof(_bcm_td2p_vp_group_bk_t));

    num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
            VP_GROUP_BITMAPf);
    VP_GROUP_BK(unit)->num_ing_vp_group = num_vp_groups;

    if (NULL == VP_GROUP_BK(unit)->ing_vp_group_array) {
        VP_GROUP_BK(unit)->ing_vp_group_array = 
            sal_alloc(sizeof(_bcm_td2p_vp_group_t) * num_vp_groups,
                    "ingress vp group array");
        if (NULL == VP_GROUP_BK(unit)->ing_vp_group_array) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(VP_GROUP_BK(unit)->ing_vp_group_array, 0,
            sizeof(_bcm_td2p_vp_group_t) * num_vp_groups);

    vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);
    num_gpp =  BCMI_MAX_COE_MODULE_ID * BCMI_NUM_PORTS_PER_COE_MODULE;
    num_vp   =  soc_mem_index_count(unit, SOURCE_VPm);

    for (i = 0; i < num_vp_groups; i++) {
        if (NULL == ING_VP_GROUP(unit, i)->gpp_vp_bitmap) {
            ING_VP_GROUP(unit, i)->gpp_vp_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_gpp + num_vp),
           "ingress vp group vp gpp bitmap");
            if (NULL == ING_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(ING_VP_GROUP(unit, i)->gpp_vp_bitmap, 0,
                   (SHR_BITALLOCSIZE(num_gpp + num_vp)));

        if (NULL == ING_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
            ING_VP_GROUP(unit, i)->vlan_vfi_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(vlan_vfi_count),
                  "ingress vp group vlan vfi bitmap");
            if (NULL == ING_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }

        sal_memset(ING_VP_GROUP(unit, i)->vlan_vfi_bitmap, 0,
                         SHR_BITALLOCSIZE(vlan_vfi_count));
    }

    /* Enable the STG checks on Ingress VP Groups */
    soc_reg_field_set(unit, VP_GROUP_CHECK_ENABLEr, &reg_value,
                      STG_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(WRITE_VP_GROUP_CHECK_ENABLEr(unit, reg_value));
    
    if (NULL == _bcm_vp_group_ing_mutex[unit]) {
        _bcm_vp_group_ing_mutex[unit] = sal_mutex_create("vp group ing mutex");
        if (_bcm_vp_group_ing_mutex[unit] == NULL) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
            VP_GROUP_BITMAPf);
    VP_GROUP_BK(unit)->num_eg_vp_group = num_vp_groups;

    if (NULL == VP_GROUP_BK(unit)->eg_vp_group_array) {
        VP_GROUP_BK(unit)->eg_vp_group_array = 
            sal_alloc(sizeof(_bcm_td2p_vp_group_t) * num_vp_groups,
                    "egress vp group array");
        if (NULL == VP_GROUP_BK(unit)->eg_vp_group_array) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    sal_memset(VP_GROUP_BK(unit)->eg_vp_group_array, 0,
            sizeof(_bcm_td2p_vp_group_t) * num_vp_groups);

    num_vp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);

    for (i = 0; i < num_vp_groups; i++) {
        if (NULL == EG_VP_GROUP(unit, i)->gpp_vp_bitmap) {
            EG_VP_GROUP(unit, i)->gpp_vp_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_gpp + num_vp),
                 "egress vp group vp gpp bitmap");
            if (NULL == EG_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }

        sal_memset(EG_VP_GROUP(unit, i)->gpp_vp_bitmap, 0,
                   (SHR_BITALLOCSIZE(num_gpp + num_vp)));

        if (NULL == EG_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
            EG_VP_GROUP(unit, i)->vlan_vfi_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(vlan_vfi_count),
                 "egress vp group vlan vfi bitmap");
            if (NULL == EG_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }

        sal_memset(EG_VP_GROUP(unit, i)->vlan_vfi_bitmap, 0,
                SHR_BITALLOCSIZE(vlan_vfi_count));
    }

    /* Enables the STG checks and UT deletion on Egress VP Groups */
    reg_value = 0;
    soc_reg_field_set(unit, EGR_VP_GROUP_CHECK_ENABLEr, &reg_value,
            STG_ENABLEf, 1);
    soc_reg_field_set(unit, EGR_VP_GROUP_CHECK_ENABLEr, &reg_value,
            UNTAG_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(WRITE_EGR_VP_GROUP_CHECK_ENABLEr(unit, reg_value)); 

    if (NULL == _bcm_vp_group_egr_mutex[unit]) {
        _bcm_vp_group_egr_mutex[unit] = sal_mutex_create("vp group egr mutex");
        if (_bcm_vp_group_egr_mutex[unit] == NULL) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    VP_GROUP_BK(unit)->vp_group_initialized = 1;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_td2p_vp_group_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_td2p_vp_group_free_resources(unit);
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_detach
 * Purpose:
 *      De-initialize the virtual port group data structures.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_detach(int unit)
{
    _bcm_td2p_vp_group_free_resources(unit);

    VP_GROUP_BK(unit)->vp_group_initialized = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vlan_vp_group_set
 * Purpose:
 *      set/clear the specified VP group in the vlan's vp group bitmap.
 * Parameters:
 *      unit            - (IN) Device Number
 *      vlan_vpn        - (IN) vlan/vpn ID
 *      egress          - (IN) TURE for egress, FALSE for ingress
 *      vp_group        - (IN) the specified VP group
 *      enable          - (IN) TRUE to set, FALSE to clear
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If vp_group == -1 and enable == false, clear the vp_group bitmap
 */

int
bcm_td2p_vlan_vp_group_set(int unit, int vlan_vpn, int egress, int vp_group, int enable)
{
    int rv = BCM_E_NONE;
    int field_len;
    int bit_exist;
    uint32 profile_idx;
    uint32 vp_group_bitmap[2];
    soc_mem_t mbrship_mem;
    uint32 mbrship_entry[SOC_MAX_MEM_WORDS];

    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                            unit, vlan_vpn, egress, &profile_idx));

    if (egress) {
        mbrship_mem = EGR_VLAN_VFI_MEMBERSHIPm;
    } else {
        mbrship_mem = ING_VLAN_VFI_MEMBERSHIPm;
    }

    /* safety check */
    field_len = soc_mem_field_length(unit, mbrship_mem, VP_GROUP_BITMAPf);
    if (vp_group >= 0) {
        if (vp_group >= field_len) {
            return BCM_E_PARAM;
        }
    } else if (enable) {
        return BCM_E_PARAM;
    }

    if (field_len > sizeof(vp_group_bitmap) * 8) {
        return BCM_E_INTERNAL;
    }

    soc_mem_lock(unit, mbrship_mem);
    if ((rv = soc_mem_read(unit, mbrship_mem, MEM_BLOCK_ANY, profile_idx,
                    mbrship_entry)) != BCM_E_NONE) {
        soc_mem_unlock(unit, mbrship_mem);
        return rv;
    }

    if (vp_group >= 0) {
        soc_mem_field_get(unit, mbrship_mem, mbrship_entry,
                            VP_GROUP_BITMAPf, vp_group_bitmap);
        bit_exist = vp_group_bitmap[vp_group/32] & (1 << (vp_group % 32));

        if (enable) {
            if (bit_exist) {
                soc_mem_unlock(unit, mbrship_mem);
                return BCM_E_NONE;
            }

            vp_group_bitmap[vp_group/32] |= 1 << (vp_group%32);
        } else {
            if (!bit_exist) {
                soc_mem_unlock(unit, mbrship_mem);
                return BCM_E_NONE;
            }

            /* clear the vp_group */
            vp_group_bitmap[vp_group/32] &= ~(1 << (vp_group%32));
        }
    } else {
        /* clear the VP group bitmap for this vlan */
        sal_memset(vp_group_bitmap, 0, sizeof(vp_group_bitmap));
    }

    soc_mem_field_set(unit, mbrship_mem, mbrship_entry,
                    VP_GROUP_BITMAPf, vp_group_bitmap);

    rv = _bcm_td2p_vlan_vfi_mbrship_profile_entry_set(unit,
                                vlan_vpn, egress, mbrship_entry);

    soc_mem_unlock(unit, mbrship_mem);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vlan_vp_group_get
 * Purpose:
 *      get the flags indicating the given vp group is set for
 *      ingress or/and egress.
 * Parameters:
 *      unit            - (IN) Device Number
 *      vlan_vpn        - (IN) vlan/vpn ID
 *      egress          - (IN) TURE for egress, FALSE for ingress
 *      vp_group        - (IN) the specified VP group
 *      enable          - (OUT) usage flag
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2p_vlan_vp_group_get(int unit, int vlan_vpn, int egress, int vp_group, int *enable)
{
    uint32 profile_idx;
    uint32 vp_group_bitmap[2];
    int field_len;
    soc_mem_t mbrship_mem;
    uint32 mbrship_entry[SOC_MAX_MEM_WORDS];

    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                            unit, vlan_vpn, egress, &profile_idx));

    if (egress) {
        mbrship_mem = EGR_VLAN_VFI_MEMBERSHIPm;
    } else {
        mbrship_mem = ING_VLAN_VFI_MEMBERSHIPm;
    }

    /* safety check */
    field_len = soc_mem_field_length(unit, mbrship_mem, VP_GROUP_BITMAPf);
    if (vp_group >= field_len) {
        return BCM_E_PARAM;
    }
    if (field_len > sizeof(vp_group_bitmap) * 8) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
        mbrship_mem, MEM_BLOCK_ANY, profile_idx, mbrship_entry));
    soc_mem_field_get(unit, mbrship_mem,
        (uint32 *)mbrship_entry, VP_GROUP_BITMAPf, vp_group_bitmap);

    *enable = 0;
    if (vp_group_bitmap[vp_group/32] & (1 << (vp_group % 32))) {
        *enable = TRUE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_local_ports_get
 * Purpose:
 *      Get the local ports on which the given VP resides.
 * Parameters:
 *      unit       - (IN) Device Number
 *      vp         - (IN) Virtual port number
 *      local_port_max   - (OUT) Size of local_port_array
 *      local_port_array - (OUT) Array of local ports
 *      local_port_count - (OUT) Number of local ports 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If local_port_max = 0 and local_port_array == NULL,
 *      the number of local ports will still be returned in
 *      local_port_count. 
 */

STATIC int
_bcm_td2p_vp_local_ports_get(int unit, int vp, int local_port_max,
        bcm_port_t *local_port_array, int *local_port_count)
{
    ing_dvp_table_entry_t dvp_entry;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_trunk_t trunk_id = -1;
    bcm_module_t modid = 0;
    bcm_port_t port = 0;
    int modid_local;
    bcm_gport_t tmp_gport;
    int is_ecmp = 0, i = 0, rv = 0;
    int intf_size = 0;
    bcm_if_t *intf_array = NULL;
    int intf_count = 0;
    bcm_l3_egress_ecmp_t ecmp_l3_egress;

    if (local_port_max < 0) {
        return BCM_E_PARAM;
    }

    if ((local_port_max == 0) && (NULL != local_port_array)) {
        return BCM_E_PARAM;
    }

    if ((local_port_max > 0) && (NULL == local_port_array)) {
        return BCM_E_PARAM;
    }

    if (NULL == local_port_count) {
        return BCM_E_PARAM;
    }

    *local_port_count = 0;

    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

    is_ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
            ECMPf);

    intf_size = soc_mem_index_max(unit, ING_L3_NEXT_HOPm) + 1;
    intf_array = sal_alloc(sizeof(bcm_if_t) * intf_size,
                           "_bcm_td2p_vp_local_ports_get");
    if (intf_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(intf_array, 0, (sizeof(bcm_if_t) * intf_size));

    if(is_ecmp) {
        sal_memset(&ecmp_l3_egress, 0, sizeof(bcm_l3_egress_ecmp_t));
        ecmp_l3_egress.ecmp_intf =
            soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, ECMP_PTRf);
        ecmp_l3_egress.ecmp_intf += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        if ((rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_l3_egress, intf_size,
                                    intf_array, &intf_count)) != BCM_E_NONE) {
            sal_free(intf_array);
            return rv;
        }
    } else {

        intf_array[0] = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                NEXT_HOP_INDEXf);
        intf_array[0] += BCM_XGS3_EGRESS_IDX_MIN(unit);
        intf_count = 1;
    }

    for( i = 0; i < intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            intf_array[i] -= BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            sal_free(intf_array);
            return SOC_E_NOT_FOUND;
        }        
        if ((rv = soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                    intf_array[i], &ing_nh)) != SOC_E_NONE) {
            sal_free(intf_array);
            return rv;
        }

        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dest_type = SOC_MEM_FIF_DEST_INVALID, dest = 0;
            dest = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh,
                                            DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_LAG) {
                trunk_id = (dest & SOC_MEM_FIF_DGPP_TGID_MASK);
            } else if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
                modid = (dest & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                          SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                port = dest & SOC_MEM_FIF_DGPP_PORT_MASK;
            }
        } else {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            } else {
                modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
                port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
            }
        }

        if (trunk_id != -1) {
            if ((rv = _bcm_esw_trunk_local_members_get(unit,
                                                 trunk_id,
                                                 local_port_max,
                                                 local_port_array,
                                                 local_port_count)) !=
                                                     BCM_E_NONE) {
                sal_free(intf_array);
                return rv;
            }
        } else {
            if ((rv = _bcm_esw_modid_is_local(unit, modid, &modid_local)) !=
                                                        BCM_E_NONE) {
                sal_free(intf_array);
                return rv;
            }

            if (!modid_local &&
                soc_feature(unit, soc_feature_multi_next_hops_on_port) &&
                _bcm_esw_is_multi_nexthop_mod_port(unit, modid, port)) {
                BCM_GPORT_MODPORT_SET(tmp_gport, modid, port);
                if ((rv = _bcm_esw_modport_local_get(unit, tmp_gport, &port) !=
                                              BCM_E_NONE)) {
                    sal_free(intf_array);
                    return rv;
                }
                modid_local = TRUE;
            }
        }

        (*local_port_count)++;
        if (NULL != local_port_array) {
            local_port_array[i] = port;
        }
    }

    sal_free(intf_array);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vlan_vp_group_get_all
 * Purpose:
 *      get the array of gports and flags related to the gport.
 * Parameters:
 *      unit       - (IN) Device Number
 *      vlan       - (IN) vlan mem entry index
 *      array_max  - (IN) max size of the array 
 *      bcm_gport_t *gport_array  - (IN) gport array
 *      flags_array -(IN) flags array related to gports
 *      port_cnt   - (IN/OUT) point to the first available array entry
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2p_vlan_vp_group_get_all(
      int unit,           /* unit number */
      int vlan,           /* index to the vlan  memory table */
      int array_max,      /* max size of the array */
      bcm_gport_t *gport_array,    /* gport array */
      int *flags_array,            /* flags array related to gports */
      int *port_cnt)      /* point to the first available array entry */
{
    int num_vp_groups;
    int vpg;
    int ing_enable;
    int egr_enable;
    int rv = BCM_E_NONE;

    num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
            VP_GROUP_BITMAPf);

    for (vpg = 0; vpg < num_vp_groups; vpg++) {
        ing_enable = 0;
        egr_enable = 0;
        if ((*port_cnt == array_max) && (gport_array || flags_array)) {
            break;
        }
        if (flags_array) {
            flags_array[*port_cnt] = 0;
        }
        if (soc_feature(unit,
                  soc_feature_vp_group_ingress_vlan_membership) &&
                  bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
            rv = bcm_td2p_vlan_vp_group_get(unit,vlan, FALSE, vpg, &ing_enable);
            BCM_IF_ERROR_RETURN(rv);
            if (ing_enable) {
                if (gport_array) {
                    BCM_GPORT_VP_GROUP_SET(gport_array[*port_cnt],vpg);
                }
                if (flags_array) {
                    flags_array[*port_cnt] = BCM_VLAN_PORT_INGRESS_ONLY;
                }
            }
        }
        if (soc_feature(unit,
                 soc_feature_vp_group_egress_vlan_membership) &&
                 bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
            rv = bcm_td2p_vlan_vp_group_get(unit,vlan, TRUE, vpg, &egr_enable);
            BCM_IF_ERROR_RETURN(rv);
            if (egr_enable) {
                if (gport_array) {
                    BCM_GPORT_VP_GROUP_SET(gport_array[*port_cnt],vpg);
                }
                if (flags_array) {
                    flags_array[*port_cnt] |= BCM_VLAN_PORT_EGRESS_ONLY;
                }
            }
        }
        if (ing_enable || egr_enable) {
            (*port_cnt)++;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_vlan_vfi_bitmap_get
 * Purpose:
 *      Get a bitmap of all the VLANs the given VP belongs to.
 * Parameters:
 *      unit        - (IN) BCM device number
 *      vp_gport    - (IN) VP gport ID 
 *      vlan_vfi_bitmap - (OUT) Bitmap of VLANs
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_vlan_vfi_bitmap_get(int unit, bcm_gport_t vp_gport, SHR_BITDCL *vlan_vfi_bitmap)
{
    int rv;
    int vp;
    int mc_type;
    source_vp_entry_t svp_entry;
    int vp_group;
    egr_dvp_attribute_entry_t egr_dvp_entry;
    bcm_port_t local_port;
    int local_port_count;
    int vlan_vfi_count;
    uint8 *vlan_tab_buf = NULL;
    uint8 *vfi_tab_buf = NULL;
    int if_max, if_count = 0;
    bcm_if_t *if_array = NULL;
    int i, j, k;
    vlan_tab_entry_t *vlan_tab_entry = NULL;
    vfi_entry_t *vfi_entry = NULL;
    int mc_index_array[3];
    int match_prev_mc_index = FALSE;
    bcm_multicast_t group;
    bcm_gport_t local_gport;
    bcm_if_t encap_id;
    int match = FALSE;
    bcm_trunk_member_t *member_array = NULL;
    int member_count, member_max;
    bcm_trunk_chip_info_t trunk_chip_info;
    SHR_BITDCL *vlan_vfi_bitmap_tmp = NULL;
    int vfi_index_max;

    if(vlan_vfi_bitmap == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_VLAN_PORT(vp_gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_VLAN;
    } else if (BCM_GPORT_IS_NIV_PORT(vp_gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_NIV;
    } else if (BCM_GPORT_IS_EXTENDER_PORT(vp_gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_EXTENDER;
    } else if (BCM_GPORT_IS_VXLAN_PORT(vp_gport)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_VXLAN;
    } else if (BCM_GPORT_IS_FLOW_PORT(vp_gport)) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_FLOW;
    } else if (BCM_GPORT_IS_L2GRE_PORT(vp_gport)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_L2GRE;
    } else if (BCM_GPORT_IS_MIM_PORT(vp_gport)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_MIM;
    } else if (BCM_GPORT_IS_MPLS_PORT(vp_gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_VPLS;
    } else if (BCM_GPORT_IS_TRUNK(vp_gport)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_tid_to_vp_lag_vp(
                    unit, BCM_GPORT_TRUNK_GET(vp_gport), &vp));
        /* there is no need to set mc_type for vplag */
        mc_type = 0;
    } else {
        return BCM_E_PARAM;
    }

    vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);

    /* Check if VP already belongs to a VP group.
     * If so, just return the VP group's VLAN bitmap.
     */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
        BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        if (VP_FILTER_VP_GRP_BMAP ==
            soc_SOURCE_VPm_field32_get(unit, &svp_entry, ENABLE_IFILTERf)) {
            vp_group = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                    VLAN_MEMBERSHIP_PROFILEf);
            sal_memcpy(vlan_vfi_bitmap, 
                       ING_VP_GROUP(unit, vp_group)->vlan_vfi_bitmap,
                       SHR_BITALLOCSIZE(vlan_vfi_count));
            return BCM_E_NONE;
        }
    }

    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        soc_field_t en_efilter_f;
        soc_field_t vm_prof_f;
        BCM_IF_ERROR_RETURN
            (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &egr_dvp_entry));

        BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                    &egr_dvp_entry, EN_EFILTERf, &en_efilter_f));

        if (VP_FILTER_VP_GRP_BMAP ==
                soc_EGR_DVP_ATTRIBUTEm_field32_get(
                unit, &egr_dvp_entry, en_efilter_f)) {
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                        &egr_dvp_entry, VLAN_MEMBERSHIP_PROFILEf, &vm_prof_f));
            vp_group = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &egr_dvp_entry,
                    vm_prof_f);
            sal_memcpy(vlan_vfi_bitmap, EG_VP_GROUP(unit, vp_group)->vlan_vfi_bitmap,
                    SHR_BITALLOCSIZE(vlan_vfi_count));
            return BCM_E_NONE;
        }
    }

    /* need a special process for vp-lag */
    if (BCM_GPORT_IS_TRUNK(vp_gport)) {
        /* 1 get vp-lag member count max */
        rv = bcm_esw_trunk_chip_info_get(unit, &trunk_chip_info);
        VP_GROUP_CLEANUP(rv);

        /* 2 alloc member array according to the member count */
        member_max = trunk_chip_info.vp_ports_max;
        member_array = sal_alloc(member_max * sizeof(bcm_trunk_member_t),
                                "member array");
        if (NULL == member_array) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        /* 3 get member array */
        rv = bcm_esw_trunk_get(unit, BCM_GPORT_TRUNK_GET(vp_gport), NULL,
                               member_max, member_array, &member_count);
        VP_GROUP_CLEANUP(rv);

        /* 4 alloc vlan bitmap tmp */
        vlan_vfi_bitmap_tmp = sal_alloc(SHR_BITALLOCSIZE(vlan_vfi_count),
                                    "vlan bitmap tmp");
        if (NULL == vlan_vfi_bitmap_tmp) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        /* 5 get each member vlan bitmap, then or them together */
        for (i = 0; i < member_count; i++) {
            sal_memset(vlan_vfi_bitmap_tmp, 0, SHR_BITALLOCSIZE(vlan_vfi_count));
            rv = _bcm_td2p_vp_vlan_vfi_bitmap_get(
                    unit, member_array[i].gport, vlan_vfi_bitmap_tmp);
            VP_GROUP_CLEANUP(rv);

            SHR_BITOR_RANGE(vlan_vfi_bitmap_tmp,
                vlan_vfi_bitmap, 0, vlan_vfi_count, vlan_vfi_bitmap);
        }

        return BCM_E_NONE;
    }

    /* VP does not belong to any VP group. Need to derive VLAN bitmap by
     * searching through each VLAN table entry's BC_IDX/UMC_IDX/UUC_IDX
     * multicast groups to see if VP belongs to their VP replication lists.
     */ 

    /* Get one local port on which the VP resides.
     * Even if the VP resides on a trunk group, only one trunk
     * member is needed since all members of a trunk group
     * have the same VP replication list.
     */
    BCM_IF_ERROR_RETURN(_bcm_td2p_vp_local_ports_get(unit,
                        vp, 1, &local_port, &local_port_count));
    if (local_port_count == 0) {
        return BCM_E_PORT; 
    }

    vlan_tab_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, VLAN_TABm), "VLAN_TAB buffer");
    if (NULL == vlan_tab_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
            0, BCM_VLAN_MAX, vlan_tab_buf);
    VP_GROUP_CLEANUP(rv);

    vfi_tab_buf = soc_cm_salloc(unit,
        SOC_MEM_TABLE_BYTES(unit, VFIm), "VFI buffer");
    if (NULL == vfi_tab_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    
    if (SOC_IS_MONTEREY(unit)) {
        rv = soc_mem_read_range(unit, VFIm, MEM_BLOCK_ANY,
                0, BCM_MN_VPN_MAX, vfi_tab_buf);
    } else {
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            vfi_index_max = soc_mem_index_max(unit, VFIm);
        } else
#endif
        {
            vfi_index_max = BCM_VPN_MAX;
        }
        rv = soc_mem_read_range(unit, VFIm, MEM_BLOCK_ANY,
                0, vfi_index_max, vfi_tab_buf);
    }
    VP_GROUP_CLEANUP(rv);

    if_max = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    if_array = sal_alloc(sizeof(bcm_if_t) * if_max, "if_array");
    if (NULL == if_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(if_array, 0, sizeof(bcm_if_t) * if_max);

    SHR_BITCLR_RANGE(vlan_vfi_bitmap, 0, vlan_vfi_count);

    for (i = 0; i < vlan_vfi_count; i++) {
        if (i < BCM_VLAN_COUNT) {
            vlan_tab_entry = soc_mem_table_idx_to_pointer
                (unit, VLAN_TABm, vlan_tab_entry_t *, vlan_tab_buf, i);

            if (0 == soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                        VALIDf)) {
                continue;
            }

            if (0 == soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                        VIRTUAL_PORT_ENf)) {
                continue;
            }

            mc_index_array[0] = _bcm_xgs3_vlan_mcast_idx_get(
                                   unit, vlan_tab_entry, BC_IDXf);
            mc_index_array[1] = _bcm_xgs3_vlan_mcast_idx_get(
                                    unit, vlan_tab_entry, UMC_IDXf);
            mc_index_array[2] = _bcm_xgs3_vlan_mcast_idx_get(
                                    unit, vlan_tab_entry, UUC_IDXf);
        } else {
            if (!_bcm_vfi_used_get(unit, (i-BCM_VLAN_COUNT), _bcmVfiTypeAny)) {
                continue;
            }

            vfi_entry = soc_mem_table_idx_to_pointer
                (unit, VFIm, vfi_entry_t *, vfi_tab_buf, (i-BCM_VLAN_COUNT));

            if (soc_VFIm_field32_get(unit, vfi_entry, PT2PT_ENf)){
                continue;
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dt;

                mc_index_array[0] = soc_mem_field32_dest_get(unit, VFIm,
                           vfi_entry, BC_DESTINATIONf, &dt);
                if (dt != SOC_MEM_FIF_DEST_IPMC) {
                    rv = BCM_E_INTERNAL;
                }
                mc_index_array[1] = soc_mem_field32_dest_get(unit, VFIm,
                            vfi_entry, UMC_DESTINATIONf, &dt);
                if (dt != SOC_MEM_FIF_DEST_IPMC) {
                    rv =  BCM_E_INTERNAL;
                }
                mc_index_array[2] = soc_mem_field32_dest_get(unit, VFIm,
                                vfi_entry, UUC_DESTINATIONf, &dt);
                if (dt != SOC_MEM_FIF_DEST_IPMC) {
                    rv = BCM_E_INTERNAL;
                }

                VP_GROUP_CLEANUP(rv);
            } else
#endif
            {
            mc_index_array[0] = soc_VFIm_field32_get(unit, vfi_entry, BC_INDEXf);
            mc_index_array[1] = soc_VFIm_field32_get(unit, vfi_entry, UMC_INDEXf);
            mc_index_array[2] = soc_VFIm_field32_get(unit, vfi_entry, UUC_INDEXf);
        }
        }

        for (j = 0; j < 3; j++) {

            /* Check if the same mc_index was already searched */
            match_prev_mc_index = FALSE;
            for (k = j - 1; k >= 0; k--) {
                if (mc_index_array[j] == mc_index_array[k]) {
                    match_prev_mc_index = TRUE;
                    break;
                }
            }
            if (match_prev_mc_index) {
                /* continue to next mc_index */
                continue;
            }

            /* Get VP replication list for (mc_index, local_port) */
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_index_array[j],
                    local_port, if_max, if_array, &if_count);
            VP_GROUP_CLEANUP(rv);

            /* Get VP's encap_id */
            _BCM_MULTICAST_GROUP_SET(group, mc_type, mc_index_array[j]);
            rv = bcm_esw_port_gport_get(unit, local_port, &local_gport);
            VP_GROUP_CLEANUP(rv);

            if (BCM_GPORT_IS_VLAN_PORT(vp_gport))  {
                rv = bcm_esw_multicast_vlan_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            } else if (BCM_GPORT_IS_NIV_PORT(vp_gport)) {
                rv = bcm_esw_multicast_niv_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            } else if (BCM_GPORT_IS_EXTENDER_PORT(vp_gport)) {
                rv = bcm_esw_multicast_extender_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            } else if (BCM_GPORT_IS_VXLAN_PORT(vp_gport)) {
                rv = bcm_esw_multicast_vxlan_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            }
#if 0
            else if (BCM_GPORT_IS_FLOW_PORT(vp_gport)) {
                rv = bcm_esw_multicast_flow_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            }
#endif
            else if (BCM_GPORT_IS_MIM_PORT(vp_gport)) {
                rv = bcm_esw_multicast_mim_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            }
            VP_GROUP_CLEANUP(rv);

            /* Search for VP's encap_id in if_array */
            match = FALSE;
            for (k = 0; k < if_count; k++) {
                if (encap_id == if_array[k]) {
                    match = TRUE;
                    break;
                }
            }
            if (match) {
                break;
            }
        }

        if (match) {
            /* VP belongs to this VLAN */
            SHR_BITSET(vlan_vfi_bitmap, i);
        }
    }

cleanup:
    if (vlan_tab_buf) {
        soc_cm_sfree(unit, vlan_tab_buf);
    }
    if (vfi_tab_buf) {
        soc_cm_sfree(unit, vfi_tab_buf);
    }
    if (if_array) {
        sal_free(if_array);
    }
    if (vlan_vfi_bitmap_tmp) {
        sal_free(vlan_vfi_bitmap_tmp);
    }
    if (member_array) {
        sal_free(member_array);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_id_set
 * Purpose:
 *      For a given port, update its vp_group id into relevant memory
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port in question
 *      vp_group - (IN) vp_group to update with
 *      egress - (IN) True if we are dealing with egress 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_id_set(int unit, int port, int vp_group, int egress)
{
    soc_mem_t                          port_mem;
    soc_field_t                        mbrship_field;
    int                                index;
    egr_dvp_attribute_entry_t          egr_dvp_entry;

    if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
        if (egress) {
            port_mem = EGR_GPP_ATTRIBUTESm;
        } else {
            port_mem = SOURCE_TRUNK_MAP_TABLEm;
        }
        mbrship_field = VLAN_MEMBERSHIP_PROFILEf;

        BCM_IF_ERROR_RETURN(_bcm_td2p_gpp_hw_index_get(unit, port, &index));
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &index));

        if (egress) {
            port_mem = EGR_DVP_ATTRIBUTEm;

            BCM_IF_ERROR_RETURN(READ_EGR_DVP_ATTRIBUTEm(
                unit, MEM_BLOCK_ANY, index, &egr_dvp_entry));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                    &egr_dvp_entry, VLAN_MEMBERSHIP_PROFILEf, &mbrship_field));
        } else {
            port_mem = SOURCE_VPm;
            mbrship_field = VLAN_MEMBERSHIP_PROFILEf;
        }
    }

    /* Modify the group into relevant memory */
    BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
        unit, port_mem, index, mbrship_field, vp_group));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_join
 * Purpose:
 *      Given a port and a vlan_vfi bmap, either join and existing
 *      vp group or create a new group.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port number passed in
 *      vlan_vfi_bmap - (IN) The VLAN/VFI bmap
 *      stp_state - (IN) STP status
 *      ut_state - (IN) Vlan untag status
 *      egress -  (IN) Direction, TRUE is egress
 *      vp_group -  (OUT) The alloted vp_group num, if all goes well
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2p_vp_group_join(int unit, int port, SHR_BITDCL *vlan_vfi_bmap,
                            int stp_state, int ut_state, int egress, int *vp_group)
{
    int rv = BCM_E_NONE;
    int i;
    int vpg;
    int gport;
    int port_offset = 0;
    int vlan_vfi_count;
    int num_grps = 0, vlan_count = 0;
    int stg, vlan_vfi, vlan_vpn;
    _bcm_td2p_vp_group_t *vp_grp_array = NULL;

    if (vlan_vfi_bmap == NULL)
    {
        return BCM_E_PARAM;
    }

    VP_GROUP_LOCK_MEMS(unit, egress);
    /* Get the data pertaining to Ingress/Egress */
    if(egress) {
        num_grps = VP_GROUP_BK(unit)->num_eg_vp_group;
        vp_grp_array = EG_VP_GROUP(unit, 0);
    } else {
        num_grps = VP_GROUP_BK(unit)->num_ing_vp_group;
        vp_grp_array = ING_VP_GROUP(unit, 0);
    }

    /* Set the vp_group_num to invalid to start with */
    vpg = _BCM_INVALID_VP_GROUP;
    SHR_BITCOUNT_RANGE(vlan_vfi_bmap, vlan_count, 0, BCM_VLAN_COUNT);
    vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
        SHR_BITCOUNT_RANGE(vlan_vfi_bmap, vlan_count, 0, vlan_vfi_count);
    }
#endif
    /* Run through all vp groups to get a matched/new one */
    for (i = 0; i < num_grps; i++) {
        if (vp_grp_array[i].gpp_vp_count > 0 &&
            SHR_BITEQ_RANGE(vlan_vfi_bmap,
            vp_grp_array[i].vlan_vfi_bitmap, 0, vlan_vfi_count)) {

            if (vp_grp_array[i].stp_state != stp_state) {
                continue;
            }
            if (egress && vlan_count) {
                if (vp_grp_array[i].ut_state != ut_state) {
                    continue;
                }
            }
            vpg = i;
            break;
        } else {
            if (vp_grp_array[i].gpp_vp_count == 0 &&
                vpg == _BCM_INVALID_VP_GROUP) {

                vpg = i;
            }
        }
    }

    *vp_group = vpg;

    if (vpg == _BCM_INVALID_VP_GROUP) {
        VP_GROUP_UNLOCK_MEMS(unit, egress);
        return BCM_E_RESOURCE;
    }

    if (vp_grp_array[vpg].gpp_vp_count == 0) {
        vp_grp_array[vpg].stp_state = stp_state;
        vp_grp_array[vpg].ut_state = ut_state;
        sal_memcpy(vp_grp_array[vpg].vlan_vfi_bitmap,
            vlan_vfi_bmap, SHR_BITALLOCSIZE(vlan_vfi_count));

        /* Update EGR_VLAN table's VP_GROUP_BITMAP field */
        SHR_BIT_ITER(vlan_vfi_bmap, vlan_vfi_count, vlan_vfi) {
            if(vlan_vfi < BCM_VLAN_COUNT) {
                vlan_vpn = vlan_vfi;

                if (egress && ut_state) {
                    rv = bcm_td2p_vp_group_ut_state_set(unit, vlan_vpn, vpg,
                                                        ut_state);
                    if (BCM_FAILURE(rv)) {
                        VP_GROUP_UNLOCK_MEMS(unit, egress);
                        return rv;
                    }
                }
            } else {
                _BCM_VPN_SET(vlan_vpn, _BCM_VPN_TYPE_VFI,
                             (vlan_vfi - BCM_VLAN_COUNT));
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm) &&
                                                      egress && ut_state) {
                    rv = bcm_td2p_vp_group_ut_state_set(unit, vlan_vpn, vpg,
                                                        ut_state);
                    if (BCM_FAILURE(rv)) {
                        VP_GROUP_UNLOCK_MEMS(unit, egress);
                        return rv;
                    }
                }
#endif
            }

            rv = bcm_td2p_vlan_vp_group_set(unit, vlan_vpn, egress, vpg, TRUE);
            if (BCM_FAILURE(rv)) {
                VP_GROUP_UNLOCK_MEMS(unit, egress);
                return rv;
            }

            rv = bcm_td2p_vp_group_vlan_vpn_stg_get(unit, vlan_vpn, egress,
                                                    &stg);
            if (BCM_FAILURE(rv)) {
                VP_GROUP_UNLOCK_MEMS(unit, egress);
                return rv;
            }

            if(stg != BCM_STG_DEFAULT) {
                BCM_GPORT_VP_GROUP_SET(gport, vpg);
                rv = bcm_td2p_vp_group_stp_state_set(unit, stg, gport, egress,
                                                     stp_state);
                if (BCM_FAILURE(rv)) {
                    VP_GROUP_UNLOCK_MEMS(unit, egress);
                    return rv;
            }
        }
    }
    }

    rv = _bcm_td2p_vp_group_resolve_port_num(unit, port, &port_offset);
    if (BCM_FAILURE(rv)) {
        VP_GROUP_UNLOCK_MEMS(unit, egress);
        return rv;
    }

    if (!SHR_BITGET(vp_grp_array[vpg].gpp_vp_bitmap, port_offset)) {
        SHR_BITSET(vp_grp_array[vpg].gpp_vp_bitmap, port_offset);
        vp_grp_array[vpg].gpp_vp_count++;
    }

    rv = _bcm_td2p_vp_group_id_set(unit, port, vpg, egress);
    if (BCM_FAILURE(rv)) {
        VP_GROUP_UNLOCK_MEMS(unit, egress);
        return rv;
    }

    VP_GROUP_UNLOCK_MEMS(unit, egress);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_leave
 * Purpose:
 *      Leave a given VP group
  * Parameters:
 *      unit - (IN) BCM device number
 *      port   - (IN) Front-panel/Gpp/VP port value
 *      vp_group - (IN) VP Group in question
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_leave(int unit, int port, int vp_group, int egress)
{
    int gport;
    int vlan_vfi_count;
    int stg = 0, port_offset = 0;
    int vlan_vfi = 0, vlan_vpn = 0;
    _bcm_td2p_vp_group_t *vp_grp = NULL;

    /* Get the data pertaining to Ingress/Egress */
    if(egress) {
        vp_grp = EG_VP_GROUP(unit, vp_group);
    } else {
        vp_grp = ING_VP_GROUP(unit, vp_group);
    }

    if (vp_grp->gpp_vp_count == 0) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_resolve_port_num(unit, port, &port_offset));

    vp_grp->gpp_vp_count--;
    SHR_BITCLR(vp_grp->gpp_vp_bitmap, port_offset);

    /* no port use this group, release it */
    if(0 == vp_grp->gpp_vp_count) {
        vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);

        /* Run through all VLAN/VFI's and unset this vp_group bit */
        SHR_BIT_ITER(vp_grp->vlan_vfi_bitmap, vlan_vfi_count, vlan_vfi) {
            if(vlan_vfi <= BCM_VLAN_MAX) {
                vlan_vpn = vlan_vfi;
                if (egress) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_ut_state_set(unit,
                            vlan_vpn, vp_group, _BCM_DEFAULT_UNTAGGED_STATE));
                }
            } else {
                _BCM_VPN_SET(vlan_vpn, _BCM_VPN_TYPE_VFI,
                             (vlan_vfi - (BCM_VLAN_MAX+1)));
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm) && egress) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_ut_state_set(unit,
                            vlan_vpn, vp_group, _BCM_DEFAULT_UNTAGGED_STATE));
                }
#endif
            }

            BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vp_group_set(
                        unit, vlan_vpn, egress, vp_group, FALSE));

            BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_vlan_vpn_stg_get(
                                        unit, vlan_vpn, egress, &stg));

            if(stg != BCM_STG_DEFAULT) {
                BCM_GPORT_VP_GROUP_SET(gport, vp_group);
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_stp_state_set(
                    unit, stg, gport, egress, _BCM_DEFAULT_STP_STATE));
            }
        }

        /* Clear out the vlan_vfi array */
        SHR_BITCLR_RANGE(vp_grp->vlan_vfi_bitmap, 0, vlan_vfi_count);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_port_move
 * Purpose:
 *      Move GPP/VP from one VP group to another due to add/remove
 *      of the GPP/VP to/from VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      gport   - (IN) Gpp/VP port value
 *      vlan_vpn - (IN) vlan/vpn ID
 *      add  - (IN) If TRUE, VP is added to VLAN, else removed from VLAN.
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      flags - (IN) Flags value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_port_move(int unit, int gport, bcm_vlan_t vlan_vpn,
                            int add, int egress, int flags)
{
    int rv = BCM_E_NONE;
    int index = 0, vlan_vfi_bit = 0;
    int old_vp_group = 0, new_vp_group = 0;
    int vlan_count, vlan_vfi_count;
    int ut_state = 0, filter = 0;
    SHR_BITDCL *vlan_vfi_bitmap = NULL;
    _bcm_td2p_vp_group_t *cur_vp_grp = NULL;

    VP_GROUP_LOCK_MEMS(unit, egress);

    /* Get the current vp group associated with this port */
    rv = bcm_td2p_vp_group_port_filter_get(
            unit, gport, egress, &old_vp_group, &filter);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if(BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        if (GPP_FILTER_VP_GRP_BMAP != filter) {
            rv = BCM_E_DISABLED;
            goto cleanup;
        }
    } else {
        if (VP_FILTER_VP_GRP_BMAP != filter) {
            rv = BCM_E_DISABLED;
            goto cleanup;
        }
    }

    vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);
    vlan_vfi_bitmap = sal_alloc(SHR_BITALLOCSIZE(vlan_vfi_count), "vlan bitmap");
    if (NULL == vlan_vfi_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(vlan_vfi_bitmap, 0, SHR_BITALLOCSIZE(vlan_vfi_count));

    /* Get the current vlan_vfi bitmap associated with this old_group */
    if (egress) {
        cur_vp_grp = EG_VP_GROUP(unit, old_vp_group);
    } else {
        cur_vp_grp = ING_VP_GROUP(unit, old_vp_group);
    }
    sal_memcpy(vlan_vfi_bitmap, cur_vp_grp->vlan_vfi_bitmap,
                                SHR_BITALLOCSIZE(vlan_vfi_count));

    if(_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
            if ((flags & BCM_VLAN_GPORT_ADD_UNTAGGED) && egress) {
                ut_state = 1;
            } else {
                ut_state = _BCM_DEFAULT_UNTAGGED_STATE;
            }
        }
#endif
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vpn);
        index += BCM_VLAN_COUNT;
    } else {
        if ((flags & BCM_VLAN_GPORT_ADD_UNTAGGED) && egress) {
            ut_state = 1;
        } else {
            ut_state = _BCM_DEFAULT_UNTAGGED_STATE;
        }

        index = vlan_vpn;
    }

    /* Get changing status of the relevant bit*/
    vlan_vfi_bit = SHR_BITGET(vlan_vfi_bitmap, index);
    if(!vlan_vfi_bit && !add) { /* VP does not exist in vlan/vfi */
        rv = BCM_E_NOT_FOUND;
        goto cleanup;
    }

    SHR_BITCOUNT_RANGE(vlan_vfi_bitmap, vlan_count, 0, BCM_VLAN_COUNT);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
        SHR_BITCOUNT_RANGE(vlan_vfi_bitmap, vlan_count, 0, vlan_vfi_count);
    }
#endif
    if (vlan_vfi_bit && add) { /* vp already exist in vlan/vfi */
        if (egress && vlan_count) { /* check ut status for egress vlan */
            if (ut_state == cur_vp_grp->ut_state) {
                goto cleanup;
            }
        } else {
            goto cleanup;
        }
    }

    /* Set or clear the VLAN/VFI */
    if (add) {
        SHR_BITSET(vlan_vfi_bitmap, index);
    } else {
        SHR_BITCLR(vlan_vfi_bitmap, index);
    }

    rv = _bcm_td2p_vp_group_join(unit, gport, vlan_vfi_bitmap,
            cur_vp_grp->stp_state, ut_state, egress, &new_vp_group);
    VP_GROUP_CLEANUP(rv);

    /* Now leave the previous group, if different from the old one */
    if (old_vp_group != new_vp_group) {
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, egress);
    }

cleanup:
    if (vlan_vfi_bitmap) {
        sal_free(vlan_vfi_bitmap);
        vlan_vfi_bitmap = NULL;
    }

    VP_GROUP_UNLOCK_MEMS(unit, egress);

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_gpp_hw_index_get
 * Purpose:
 *      Given a port and port_type, pass back the absolute value to
 *      be used within a vp_group in addition to the offset.
 *      bmap from the group this port is a member of.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port number
 *      egress - (IN) True for egress
 *      hw_index - (OUT) The hardware index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_gpp_hw_index_get(int unit, int port, int *hw_index)
{
    bcm_trunk_t trunk_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    int id;

    /* Resolve the port */
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(
        unit, port, &mod_out, &port_out, &trunk_id, &id));

    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(
                            unit, mod_out, port_out, hw_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_resolve_port_num
 * Purpose:
 *      Get absolute offset of port for gpp_vp_bitmap updatation
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port number passed in .
 *      port_offset -  (OUT) The offset to be used for VP group bmap
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_resolve_port_num(int unit, int port, int *port_offset)
{
    int             vp;

    if(BCM_GPORT_IS_SUBPORT_PORT(port)) {
        BCM_IF_ERROR_RETURN(_bcm_td2p_gpp_hw_index_get(unit, port, port_offset));
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &vp));

        /* VP's follow the GPP's, add the offset */
        *port_offset = BCMI_VP_GROUP_VP_OFFSET + vp;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_port_filter_get
 * Purpose:
 *      Given a access_type and the port value,
 *      return the current vp_group the entity belongs to.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port   - (IN) Gpp/VP port value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      vp_group - (OUT) The vp_group associated with the entity
 *      ifilter - (OUT) The ifilter value 
 *      
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_td2p_vp_group_port_filter_get(int unit, int port, int egress, int *vp_group, int *filter)
{
    int             port_index;
    soc_mem_t       port_mem;
    soc_field_t     mbrship_field;
    soc_field_t     filter_field;
    uint32          entry[SOC_MAX_MEM_WORDS];

    if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
        if (egress) {
            port_mem = EGR_GPP_ATTRIBUTESm;
            filter_field = EN_EFILTERf;
            mbrship_field = VLAN_MEMBERSHIP_PROFILEf;
        } else {
            port_mem = SOURCE_TRUNK_MAP_TABLEm;
            filter_field = EN_IFILTERf;
            mbrship_field = VLAN_MEMBERSHIP_PROFILEf;
        }

        BCM_IF_ERROR_RETURN(_bcm_td2p_gpp_hw_index_get(unit, port, &port_index));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, port_mem, MEM_BLOCK_ANY, port_index, &entry));

        *vp_group = soc_mem_field32_get(unit, port_mem, &entry, mbrship_field);

        if(egress) {
            /* Currently '_bcm_esw_egr_port_tab_set' is defined only for KT2,
               when the port handling gets cleaned up, we need to make it
               generic and call this LPORT config from there.
            */
            BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_get(
                            unit, port, EGR_LPORT_PROFILEm,
                            filter_field, (uint32*)filter));
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_get(unit, port, filter_field, filter));
        }
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port_index));

        if (egress) {
            port_mem = EGR_DVP_ATTRIBUTEm;

            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, port_mem, MEM_BLOCK_ANY, port_index, &entry));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(
                                unit, &entry, EN_EFILTERf, &filter_field));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(
                    unit, &entry, VLAN_MEMBERSHIP_PROFILEf, &mbrship_field));
        } else {
            port_mem = SOURCE_VPm;
            filter_field = ENABLE_IFILTERf;
            mbrship_field = VLAN_MEMBERSHIP_PROFILEf;

            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, port_mem, MEM_BLOCK_ANY, port_index, &entry));
        }

        *vp_group = soc_mem_field32_get(unit, port_mem, &entry, mbrship_field);
        *filter = soc_mem_field32_get(unit, port_mem, &entry, filter_field);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_port_filter_set
 * Purpose:
 *      Given a access_type and the port value,
 *      return the current vp_group the entity belongs to.
 * Parameters:
 *      unit - (IN) BCM device number
 *      access_type - (IN) The data_type which can be FP, GPP/VP or STG
 *      port   - (IN) Gpp/VP port value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      ifilter - (IN) The ifilter value to set
 *      
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_td2p_vp_group_port_filter_set(int unit, int port, int egress, int filter)
{
    int             port_index;
    soc_mem_t       port_mem;
    soc_field_t     filter_field;
    uint32          entry[SOC_MAX_MEM_WORDS];

    if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
        if (egress) {
            filter_field = EN_EFILTERf;
        } else {
            filter_field = EN_IFILTERf;
        }

        if (egress) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit, port,
                            EGR_LPORT_PROFILEm, filter_field, filter));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_NONE, filter_field, filter));
        }
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port_index));

        if (egress) {
            port_mem = EGR_DVP_ATTRIBUTEm;

            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, port_mem, MEM_BLOCK_ANY, port_index, &entry));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(
                                unit, &entry, EN_EFILTERf, &filter_field));
        } else {
            port_mem = SOURCE_VPm;
            filter_field = ENABLE_IFILTERf;

            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, port_mem, MEM_BLOCK_ANY, port_index, &entry));
        }

        soc_mem_field32_set(unit, port_mem, &entry, filter_field, filter);
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, port_mem, MEM_BLOCK_ALL, port_index, &entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_ut_state_set
 * Purpose:
 *      Given an "EGR-VLAN" and a group, set the UT state.
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      egr_vlan - (IN) Egress VLAN
 *      vp_group - (IN) - STP state that the value needs to be set to
 *      ut_state - (IN) The UT state for the given group on this egr_vlan
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_ut_state_set(int unit, int egr_vlan, int vp_group, int ut_state)
{
    uint32              ut_grp_bmap[2];
    egr_vlan_entry_t    egr_vlan_entry;
#if defined(BCM_TRIDENT3_SUPPORT)
    egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
#endif
    int                 rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
    sal_memset(&egr_vlan_vfi, 0, sizeof(egr_vlan_vfi));
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
        BCM_IF_ERROR_RETURN(_bcm_td3_vlan_vfi_profile_entry_get(unit,
                                                egr_vlan, &egr_vlan_vfi));
        soc_mem_field_get(unit, EGR_VLAN_VFI_UNTAGm, (uint32*)&egr_vlan_vfi,
                UT_VP_GRP_BITMAPf, ut_grp_bmap);
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY,
                                            egr_vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry,
            UT_VP_GRP_BITMAPf, ut_grp_bmap);
    }

    /* Set or clear the relevant group */
    if(ut_state) {
        ut_grp_bmap[vp_group / 32] |= (1 << (vp_group % 32));
    } else {
        ut_grp_bmap[vp_group / 32] &= ~(1 << (vp_group % 32));
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
        soc_mem_field_set(unit, EGR_VLAN_VFI_UNTAGm, (uint32*)&egr_vlan_vfi,
                UT_VP_GRP_BITMAPf, ut_grp_bmap);
        _bcm_td3_vlan_vfi_profile_entry_set(unit, egr_vlan, &egr_vlan_vfi);
    } else
#endif
    {
    soc_EGR_VLANm_field_set(unit, &egr_vlan_entry,
            UT_VP_GRP_BITMAPf, ut_grp_bmap);
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLANm(unit, MEM_BLOCK_ALL, egr_vlan, &egr_vlan_entry));
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_ut_state_get
 * Purpose:
 *      Given an "VLAN" and a subport, get the port's states.
 *
 * Parameters:
 *      unit  - (IN) BCM device number
 *      gport - (IN) Subport port
 *      vlan  - (IN) - vlan
 *      flags - (OUT) Subport state
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_ut_state_get(int unit, bcm_port_t gport, bcm_vlan_t vlan, int *ut_state)
{
    int vp_group = 0, filter = 0;
    uint32              ut_grp_bmap[2];
    egr_vlan_entry_t    egr_vlan_entry;
    egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
    uint32              prof_index = 0;

    if ((vlan > BCM_VLAN_MAX) && (!SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm))) {
        return BCM_E_PARAM;
    }

    if(BCM_GPORT_IS_VP_GROUP(gport)) {
        vp_group = BCM_GPORT_VP_GROUP_GET(gport);
    } else {
        /* Get vp_group associated with this port */
        BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
                        unit, gport, TRUE, &vp_group, &filter));

        /* Make sure the filter values are set to vp_group */
        if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
            if (filter != GPP_FILTER_VP_GRP_BMAP) {
                return BCM_E_PARAM;
            }
        } else {
            if (filter != VP_FILTER_VP_GRP_BMAP) {
                return BCM_E_PARAM;
            }
        }
    }

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
        /* Get the profile_idx associated with the VLAN/VFI */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_td3_vlan_vfi_profile_entry_get(unit,
                                                    vlan, &egr_vlan_vfi));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vlan_entry));
            prof_index = soc_mem_field32_get(unit, EGR_VLANm,
                                             &egr_vlan_entry, UNTAG_PROFILE_PTRf);
            BCM_IF_ERROR_RETURN
                (READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index, &egr_vlan_vfi));
        }
        soc_EGR_VLAN_VFI_UNTAGm_field_get(unit, &egr_vlan_vfi, UT_VP_GRP_BITMAPf, ut_grp_bmap);
    } else {
    BCM_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry, UT_VP_GRP_BITMAPf, ut_grp_bmap);
    }

    *ut_state = (ut_grp_bmap[vp_group / 32] & (1 << (vp_group % 32)));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_vlan_vpn_stg_set
 * Purpose:
 *      Attach a STG group to vlan/vpn.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vpn - (IN) vlan/vpn ID
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      stg - (OUT) The STG for the VLAN
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_vlan_vpn_stg_set(int unit, int vlan_vpn, int egress, int stg)
{
    int                rv = 0;
    soc_mem_t          mem;
    uint32             index = 0;
    bcm_vlan_info_t *vi = &vlan_info[unit];
    _bcm_virtual_bookkeeping_t *virtual_info = &_bcm_virtual_bk_info[unit];

    /* Depending on VLAN/VFI and direction, note the memory to use */
    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vpn);
        if (virtual_info->vfi_bitmap != NULL &&
            !_bcm_vfi_used_get(unit, index, _bcmVfiTypeAny)) {
            return BCM_E_NOT_FOUND;
        }

        if (!egress) {
            mem = VFIm;
        } else {
            mem = EGR_VFIm;
        }
    } else {
        index = vlan_vpn;
        if (vi->init && !SHR_BITGET(vi->bmp.w, vlan_vpn)) {
            return BCM_E_NOT_FOUND;
        }

        if (!egress) {
            mem = VLAN_TABm;
        } else {
            mem = EGR_VLANm;
        }
    }

    rv = soc_mem_field32_modify(unit, mem, index, STGf, stg);
    if (BCM_SUCCESS(rv) &&
        !egress && mem == VLAN_TABm &&
        SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
        rv = soc_mem_field32_modify(unit, VLAN_ATTRS_1m, index, STGf, stg);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_vlan_vpn_stg_get
 * Purpose:
 *      Get stg group id of given vlan/vpn per direction.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vpn - (IN) vlan/vpn ID
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      stg - (OUT) The STG for the VLAN
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_vlan_vpn_stg_get(int unit, int vlan_vpn, int egress, int *stg)
{
    soc_mem_t          mem;
    uint32             index = 0;
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read memory entry. */
    bcm_vlan_info_t *vi = &vlan_info[unit];
    _bcm_virtual_bookkeeping_t *virtual_info = &_bcm_virtual_bk_info[unit];

    /* Depending on VLAN/VFI and direction, note the memory to use */
    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vpn);
        if (virtual_info->vfi_bitmap != NULL &&
            !_bcm_vfi_used_get(unit, index, _bcmVfiTypeAny)) {
            return BCM_E_NOT_FOUND;
        }

        if (egress) {
            mem = EGR_VFIm;
        } else {
            mem = VFIm;
        }
    } else {
        index = vlan_vpn;
        if (vi->init && !SHR_BITGET(vi->bmp.w, vlan_vpn)) {
            return BCM_E_NOT_FOUND;
        }

        if (egress) {
            mem = EGR_VLANm;
        } else {
            mem = VLAN_TABm;
        }
    }

    /* Read the memory location */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, buf));
    /* Assign the stp_state */
    *stg = soc_mem_field32_get(unit, mem, buf, STGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_stg_move
 * Purpose:
 *      Move FP/GPP/VP from one VP group to another due to add/remove
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Gpp/VP port value
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      stp_state - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stg_move(int unit, int stg, int gport, int stp_state, int egress)
{
    int rv;
    int filter = 0;
    int vlan_vfi_count;
    int old_vp_group = 0, new_vp_grp = 0;
    _bcm_td2p_vp_group_t *cur_vp_grp = NULL;
    SHR_BITDCL *vlan_vfi_bmap = NULL;

    VP_GROUP_LOCK_MEMS(unit, egress);

    /* Get the current vp group associated with this STG group */
    rv = bcm_td2p_vp_group_port_filter_get(
            unit, gport, egress, &old_vp_group, &filter);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if(BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        if (GPP_FILTER_VP_GRP_BMAP != filter) {
            rv = BCM_E_DISABLED;
            goto cleanup;
        }
    } else {
        if (VP_FILTER_VP_GRP_BMAP != filter) {
            rv = BCM_E_DISABLED;
            goto cleanup;
        }
    }

    if (egress) {
        cur_vp_grp = EG_VP_GROUP(unit, old_vp_group);
    } else {
        cur_vp_grp = ING_VP_GROUP(unit, old_vp_group);
    }

    if(stp_state == cur_vp_grp->stp_state) {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);
    vlan_vfi_bmap = sal_alloc(SHR_BITALLOCSIZE(vlan_vfi_count), "vlan bitmap");
    if (NULL == vlan_vfi_bmap) {
        rv =  BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(vlan_vfi_bmap, 0, SHR_BITALLOCSIZE(vlan_vfi_count));

    if (SHR_BITEQ_RANGE(vlan_vfi_bmap,
            cur_vp_grp->vlan_vfi_bitmap, 0, vlan_vfi_count)) {
        rv = BCM_E_PARAM;
        goto cleanup;
    } else {
        sal_memcpy(vlan_vfi_bmap, cur_vp_grp->vlan_vfi_bitmap,
                            SHR_BITALLOCSIZE(vlan_vfi_count));
    }

    /* Try and find a group we can re-use */
    rv = _bcm_td2p_vp_group_join(unit, gport, vlan_vfi_bmap,
            stp_state, cur_vp_grp->ut_state, egress, &new_vp_grp);
    VP_GROUP_CLEANUP(rv);

    /* Now leave the previous group, if different from new group */
    if (old_vp_group != new_vp_grp){
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, egress);
    }

cleanup:
    if(vlan_vfi_bmap) {
        sal_free(vlan_vfi_bmap);
        vlan_vfi_bmap = NULL;
    }
    VP_GROUP_UNLOCK_MEMS(unit, egress);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_stp_init
 * Purpose:
 *      Init all vp group stp state for the given stg group.
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      stp_state - (IN) - STP state the stg-port belongs to
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stp_init(int unit, int stg, int egress, int stp_state)
{
    int rv = BCM_E_NONE;
    int vp_group = 0;
    int num_grps = 0;
    soc_mem_t mem;
    uint32 stg_entry[SOC_MAX_MEM_WORDS];

    if (!soc_feature(unit, soc_feature_vlan_vfi_membership)){
        return BCM_E_UNAVAIL;
    }

    if (egress) {
        mem = EGR_VLAN_STGm;
        num_grps = soc_mem_field_length(unit,
            EGR_VLAN_VFI_MEMBERSHIPm, VP_GROUP_BITMAPf);
    } else {
        mem = STG_TABm;
        num_grps = soc_mem_field_length(unit,
            ING_VLAN_VFI_MEMBERSHIPm, VP_GROUP_BITMAPf);
    }

    soc_mem_lock(unit, mem);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stg, &stg_entry);
    VP_GROUP_CLEANUP(rv);

    for (vp_group = 0; vp_group < num_grps; vp_group++) {
        soc_mem_field32_set(unit, mem, &stg_entry,
            _bcm_vp_group_stp_field[vp_group], stp_state);
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, stg, &stg_entry);

cleanup:
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_stp_state_set
 * Purpose:
 *      Given a port and a stg, sets it state
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      port - (IN) Port number
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      stp_state - (IN) - STP state the stg-port belongs to
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stp_state_set(int unit, int stg, int gport, int egress, int stp_state)
{
    int rv = BCM_E_NONE;
    int vp_group = 0, filter = 0;
    soc_mem_t mem;

    if (!soc_feature(unit, soc_feature_vlan_vfi_membership)){
        return BCM_E_UNAVAIL;
    }

    if (egress) {
        mem = EGR_VLAN_STGm;
    } else {
        mem = STG_TABm;
    }

    if(BCM_GPORT_IS_VP_GROUP(gport)) {
        vp_group = BCM_GPORT_VP_GROUP_GET(gport);
    } else {
        BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
                unit, gport, egress, &vp_group, &filter));
    }

    soc_mem_lock(unit, mem);

    rv = soc_mem_field32_modify(unit, mem, stg,
            _bcm_vp_group_stp_field[vp_group], stp_state);

    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_stp_state_get
 * Purpose:
 *      Given a port and a stg, gets its state
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      port - (IN) Port number
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress 
 *      stp_state - (OUT) - STP state the stg-port belongs to
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stp_state_get(int unit, int stg, int gport, int egress, int *stp_state)
{
    int rv = BCM_E_NONE;
    int vp_group = 0, filter = 0;
    soc_mem_t mem;
    uint32 stg_entry[SOC_MAX_MEM_WORDS];

    if(BCM_GPORT_IS_VP_GROUP(gport)) {
        vp_group = BCM_GPORT_VP_GROUP_GET(gport);
    } else {
        /* Get vp_group associated with this port */
        BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
                unit, gport, egress, &vp_group, &filter));
    }

    if (egress) {
        mem = EGR_VLAN_STGm;
    } else {
        mem = STG_TABm;
    }

    soc_mem_lock(unit, STG_TABm);
    /* Read the relevant entry */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stg, &stg_entry);
    soc_mem_unlock(unit, STG_TABm);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the STP state */
    *stp_state = soc_mem_field32_get(unit, mem,&stg_entry,
                            _bcm_vp_group_stp_field[vp_group]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_vlan_get_all
 * Purpose:
 *      Get all VPs from the VP groups belonging to the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 *      gpp_vp_bitmap - (OUT) Bitmap of VPs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *gpp_vp_bitmap)
{
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i;
    uint32 profile_idx;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
        vlan_attrs_1_entry_t vlan_tab;
        SOC_IF_ERROR_RETURN(
                READ_VLAN_ATTRS_1m(unit, MEM_BLOCK_ANY, vlan, &vlan_tab));
        profile_idx = soc_VLAN_ATTRS_1m_field32_get(unit, &vlan_tab,
                MEMBERSHIP_PROFILE_PTRf);
    } else
#endif
    {
        SOC_IF_ERROR_RETURN
        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vlan, &vlan_mpls_entry));
    soc_VLAN_MPLSm_field_get(unit, &vlan_mpls_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);
    }

    BCM_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx,
                                       &vlan_vfi_entry));
    soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry, 
            VP_GROUP_BITMAPf, fldbuf);
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Get all VPs in VP group i.
             */
            SHR_BITOR_RANGE(gpp_vp_bitmap, ING_VP_GROUP(unit, i)->gpp_vp_bitmap,
                    0, (BCMI_VP_GROUP_GLP_COUNT + 
                        soc_mem_index_count(unit, SOURCE_VPm)), 
                        gpp_vp_bitmap);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_eg_vp_group_vlan_get_all
 * Purpose:
 *      Get all VPs from the VP groups belonging to the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 *      gpp_vp_bitmap - (OUT) Bitmap of VPs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *gpp_vp_bitmap)
{
    egr_vlan_entry_t egr_vlan_entry;
    egr_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i;
    uint32 profile_idx;

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx, 
                                       &vlan_vfi_entry));
    soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
            VP_GROUP_BITMAPf, fldbuf);

    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Get all VPs in VP group i.
             */
            SHR_BITOR_RANGE(gpp_vp_bitmap, EG_VP_GROUP(unit, i)->gpp_vp_bitmap,
                    0, (BCMI_VP_GROUP_GLP_COUNT + 
                        soc_mem_index_count(unit, SOURCE_VPm)), 
                        gpp_vp_bitmap);
        }
    }

    return BCM_E_NONE;
}

#if defined(BCM_TRX_SUPPORT)

/*
 * Function:
 *      bcm_td2p_vp_group_delete_all
 * Purpose:
 *      Delete all VP groups from the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID
 *      egress - (IN) TRUE for egress, FALSE for ingress
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_delete_all(int unit, bcm_vlan_t vlan_vpn, int egress)
{
    uint32 profile_idx;
    uint32 fldbuf[2];
    int mod, port, gport;
    int grp;
    int grp_num;
    int gpp_vp;
    int gpp_vp_num;
    soc_mem_t mbrship_mem;
    uint32 mbrship_entry[SOC_MAX_MEM_WORDS];
    _bcm_td2p_vp_group_t *vp_grp = NULL;

    if (egress) {
        mbrship_mem = EGR_VLAN_VFI_MEMBERSHIPm;
        grp_num = VP_GROUP_BK(unit)->num_eg_vp_group;
    } else {
        mbrship_mem = ING_VLAN_VFI_MEMBERSHIPm;
        grp_num = VP_GROUP_BK(unit)->num_ing_vp_group;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                                unit, vlan_vpn, egress, &profile_idx));

    if (profile_idx == 0 || profile_idx == 1) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mbrship_mem,
                        MEM_BLOCK_ANY, profile_idx, mbrship_entry));
    soc_mem_field_get(unit, mbrship_mem, mbrship_entry, VP_GROUP_BITMAPf, fldbuf);

    gpp_vp_num =  BCMI_VP_GROUP_GLP_COUNT + soc_mem_index_count(unit, SOURCE_VPm);

    SHR_BIT_ITER(fldbuf, grp_num, grp) { /* find vp group used by this VLAN_VFI */
        if (egress) {
            vp_grp = EG_VP_GROUP(unit, grp);
        } else {
            vp_grp = ING_VP_GROUP(unit, grp);
        }

        /* find GPP/VP used by this vp group */
        SHR_BIT_ITER(vp_grp->gpp_vp_bitmap, gpp_vp_num, gpp_vp) {
            if(gpp_vp < BCMI_VP_GROUP_GLP_COUNT) {
                /* Compute the (mod, port) for this GPP,
                   based on the location */
                mod = gpp_vp % 255;
                port = gpp_vp - mod;
                BCM_GPORT_SUBPORT_PORT_SET(gport,
                                           (((mod & 0xFF) << 0x7) | port ));
            } else {
                if(_bcm_vp_encode_gport(unit, (gpp_vp-BCMI_VP_GROUP_VP_OFFSET),
                                        &gport) != BCM_E_NONE) {
                    return BCM_E_INTERNAL;
                }
            }

            BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_move(unit,
                                    gport, vlan_vpn, FALSE, egress, 0));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_vlan_member_set
 * Purpose:
 *      Set virtual port ingress and egress filter modes.
 * Parameters:
 *      unit  - (IN) BCM device number
 *      gport - (IN) VP gport ID 
 *      flags - (IN) Ingress and egress filter modes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_vlan_member_set(int unit, bcm_gport_t gport, uint32 flags)
{
    int rv = BCM_E_NONE;
    int vp = -1;
    int vlan_vfi_count;
    int ing_vpg_managed, egr_vpg_managed;
    int old_ifilter = 0, old_efilter = 0;
    SHR_BITDCL *vlan_vfi_bitmap = NULL;
    int old_ing_vpg = 0, old_egr_vpg = 0, vp_group = 0;
    int filter_disable, filter_hash_table, filter_vp_grp;
    int filter_vp_not_check = -1;

    if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        filter_disable = GPP_FILTER_DISABLE;
        filter_hash_table = GPP_FILTER_HASH;
        filter_vp_grp = GPP_FILTER_VP_GRP_BMAP;
    } else {
        filter_disable = VP_FILTER_DISABLE;
        filter_hash_table = VP_FILTER_HASH;
        filter_vp_grp = VP_FILTER_VP_GRP_BMAP;
        filter_vp_not_check = VP_DO_NOT_CHECK;

        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport, &vp));

        if (!BCM_GPORT_IS_TRUNK(gport)) {
            int local_port_num;

            BCM_IF_ERROR_RETURN(_bcm_td2p_vp_local_ports_get(
                            unit, vp, 0, NULL, &local_port_num));
            if ((local_port_num == 0) && 
                (flags & (BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS))) {
                return BCM_E_PORT;
            }
        }
    }

    /* Get current filter mode */
    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
            unit, gport, FALSE, &old_ing_vpg, &old_ifilter));
    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
            unit, gport, TRUE, &old_egr_vpg, &old_efilter));

    if (flags & BCM_PORT_VLAN_MEMBER_VP_DO_NOT_CHECK) {
        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) { 
            if (old_ifilter != filter_vp_not_check) {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                     unit, gport, FALSE, filter_vp_not_check));
            }
        } else {
            /* only set to default if enabled by the same mode */
            if (old_ifilter == filter_vp_not_check) {
                /* Disable checks */
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                    unit, gport, FALSE, filter_disable));
            }
        }

        if(flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (old_efilter != filter_vp_not_check) {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                     unit, gport, TRUE, filter_vp_not_check));
            }
        } else {
            if (old_efilter == filter_vp_not_check) {
                /* Disable checks */
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                    unit, gport, TRUE, filter_disable));
            }
        }
    } else if (flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP) { /* hash table */
        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
            if (old_ifilter != filter_hash_table) {
                /* enable hash table */
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                unit, gport, FALSE, filter_hash_table));
            }
        } else {
            if (old_ifilter == filter_hash_table) {
                /* Disable checks */
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                    unit, gport, FALSE, filter_disable));
            }
        }

        if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (old_efilter != filter_hash_table) {
                /* enable hash table */
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                    unit, gport, TRUE, filter_hash_table));
            }
        } else {
            if (old_efilter == filter_hash_table) {
                /* Disable checks */
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_set(
                                    unit, gport, TRUE, filter_disable));
            }
        }
    } else { /* vp group */
        ing_vpg_managed = !bcm_td2p_ing_vp_group_unmanaged_get(unit);
        egr_vpg_managed = !bcm_td2p_egr_vp_group_unmanaged_get(unit);

        if (((flags & BCM_PORT_VLAN_MEMBER_INGRESS) &&
                ing_vpg_managed && (old_ifilter != filter_vp_grp)) ||
                ((flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
                egr_vpg_managed && (old_efilter != filter_vp_grp))) {
            vlan_vfi_count = BCM_VLAN_COUNT + soc_mem_index_count(unit, VFIm);

            vlan_vfi_bitmap = sal_alloc(
                SHR_BITALLOCSIZE(vlan_vfi_count), "vlan bitmap");
            if (NULL == vlan_vfi_bitmap) {
                return BCM_E_MEMORY;
            }
            sal_memset(vlan_vfi_bitmap, 0,
                       SHR_BITALLOCSIZE(vlan_vfi_count));

            if(!BCM_GPORT_IS_SUBPORT_PORT(gport)) {
                rv = _bcm_td2p_vp_vlan_vfi_bitmap_get(
                            unit, gport, vlan_vfi_bitmap);
                VP_GROUP_CLEANUP(rv);
            }
        }

        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
            if (old_ifilter != filter_vp_grp) {
                if (ing_vpg_managed) {
                    rv = _bcm_td2p_vp_group_join(unit, gport, vlan_vfi_bitmap,
                            _BCM_DEFAULT_STP_STATE, _BCM_DEFAULT_UNTAGGED_STATE,
                            FALSE, &vp_group);
                    VP_GROUP_CLEANUP(rv);
                }

                /* Enable ingress VP group filtering */
                rv = bcm_td2p_vp_group_port_filter_set(
                        unit, gport, FALSE, filter_vp_grp);
                VP_GROUP_CLEANUP(rv);
            }
        } else {
            if (old_ifilter == filter_vp_grp) {
                /* Disable ingress filtering */
                rv = bcm_td2p_vp_group_port_filter_set(
                        unit, gport, FALSE, filter_disable);
                VP_GROUP_CLEANUP(rv);
            }

            if (filter_vp_grp == old_ifilter) {
                if (!bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                    /* Remove VP from VP group */
                    rv = _bcm_td2p_vp_group_leave(unit, gport, old_ing_vpg, FALSE);
                    VP_GROUP_CLEANUP(rv);

                    rv = _bcm_td2p_vp_group_id_set(unit, gport, 0, FALSE);
                    VP_GROUP_CLEANUP(rv);
                }
            }
        }

        if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (old_efilter != filter_vp_grp) {
                if (egr_vpg_managed) {
                    rv = _bcm_td2p_vp_group_join(unit, gport, vlan_vfi_bitmap,
                            _BCM_DEFAULT_STP_STATE, _BCM_DEFAULT_UNTAGGED_STATE,
                            TRUE, &vp_group);
                    VP_GROUP_CLEANUP(rv);
                }

                /* Enable ingress VP group filtering */
                rv = bcm_td2p_vp_group_port_filter_set(
                        unit, gport, TRUE, filter_vp_grp);
                VP_GROUP_CLEANUP(rv);
            }
        } else {
            if (old_efilter == filter_vp_grp) {
                /* Disable ingress filtering */
                rv = bcm_td2p_vp_group_port_filter_set(
                        unit, gport, TRUE, filter_disable);
                VP_GROUP_CLEANUP(rv);
            }

            if (filter_vp_grp == old_efilter) {
                if (!bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                    /* Remove VP from VP group */
                    rv = _bcm_td2p_vp_group_leave(unit, gport, old_egr_vpg, TRUE);
                    VP_GROUP_CLEANUP(rv);

                    rv = _bcm_td2p_vp_group_id_set(unit, gport, 0, TRUE);
                    VP_GROUP_CLEANUP(rv);
                }
            }
        }
    }

cleanup:
    if (vlan_vfi_bitmap) {
        sal_free(vlan_vfi_bitmap);
    }

    return rv;
}

#endif

/*
 * Function:
 *      bcm_td2p_vp_vlan_member_get
 * Purpose:
 *      Get virtual port ingress and egress filter modes.
 * Parameters:
 *      unit  - (IN) BCM device number
 *      gport - (IN) VP gport id
 *      ingress - (IN) direction: 1/ingress, 0/egress, -1/both
 *      flags - (OUT) Ingress and egress filter modes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_vlan_member_get(int unit, bcm_gport_t gport, int ingress, uint32 *flags)
{
    int cur_vp_group = 0;
    int filter = 0, filter_hash_table, filter_vp_grp, filter_vp_do_not_check=-1;

    if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        filter_hash_table = GPP_FILTER_HASH;
        filter_vp_grp = GPP_FILTER_VP_GRP_BMAP;
    } else {
        filter_hash_table = VP_FILTER_HASH;
        filter_vp_grp = VP_FILTER_VP_GRP_BMAP;
        filter_vp_do_not_check = VP_DO_NOT_CHECK;
    }

    *flags = 0;
    if (ingress != 0) {
        BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
                    unit, gport, FALSE, &cur_vp_group, &filter));
        if (filter == filter_vp_grp) {
            *flags = BCM_PORT_VLAN_MEMBER_INGRESS;
        } else if (filter == filter_hash_table) {
            *flags = BCM_PORT_VLAN_MEMBER_INGRESS |
                        BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP;
        } else if (filter == filter_vp_do_not_check) {
            *flags = BCM_PORT_VLAN_MEMBER_INGRESS |
                        BCM_PORT_VLAN_MEMBER_VP_DO_NOT_CHECK;
        }
        if (ingress == 1) return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_filter_get(
                unit, gport, TRUE, &cur_vp_group, &filter));
    if (filter == filter_vp_grp) {
        *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
    } else if (filter == filter_hash_table) {
        *flags |= BCM_PORT_VLAN_MEMBER_EGRESS |
                    BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP;
    } else if (filter == filter_vp_do_not_check) {
        *flags |= BCM_PORT_VLAN_MEMBER_EGRESS |
                    BCM_PORT_VLAN_MEMBER_VP_DO_NOT_CHECK;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_unmanaged_get
 * Purpose:
 *      Check if VP group resource is managed by user. If so, user will be
 *      resposible to assign a VP group to a VP and add the VP group to the
 *      vlan's VP group bitmap in order to establish the vlan membership.
 * Parameters:
 *      unit  - (IN) BCM device number
 * Returns:
 *      TRUE/FALSE
 */
int
bcm_td2p_ing_vp_group_unmanaged_get(int unit)
{
    return _bcm_td2p_vp_group_unmanaged[unit].ingress;
}

int
bcm_td2p_egr_vp_group_unmanaged_get(int unit)
{
    return _bcm_td2p_vp_group_unmanaged[unit].egress;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_unmanaged_set
 * Purpose:
 *      Set whether the VP group resource is managed by user.
 * Parameters:
 *      unit  - (IN) BCM device number
        unmanaged - (IN) TRUE/FALSE
 * Returns: BCM_E_XXX 
 */
int
bcm_td2p_ing_vp_group_unmanaged_set(int unit, int flag)
{
    int i;
    int gpp_vp_count = 0;

    /* check if to set the same value */
    if (_bcm_td2p_vp_group_unmanaged[unit].ingress == flag) {
        return BCM_E_NONE;
    }

    /* make sure auto method hasn't kicked in yet */
    if (VP_GROUP_BK(unit)->vp_group_initialized) {
        for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
            if (0 == ING_VP_GROUP(unit, i)->gpp_vp_count){
                    gpp_vp_count++;
            }
        }

        if (gpp_vp_count != VP_GROUP_BK(unit)->num_ing_vp_group) {
            LOG_INFO(BSL_LS_BCM_VLAN,
                     (BSL_META_U(unit, "Unmanaged mode set fails:"
                            "VP group auto method already started\n")));
            return BCM_E_EXISTS;
        }
    }
    _bcm_td2p_vp_group_unmanaged[unit].ingress = flag;
    return BCM_E_NONE;
}

int
bcm_td2p_egr_vp_group_unmanaged_set(int unit, int flag)
{
    int i;
    int gpp_vp_count = 0;

    /* check if to set the same value */
    if (_bcm_td2p_vp_group_unmanaged[unit].egress == flag) {
        return BCM_E_NONE;
    }

    /* make sure auto method hasn't kicked in yet */
    if (VP_GROUP_BK(unit)->vp_group_initialized) {
        for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
            if (0 == EG_VP_GROUP(unit, i)->gpp_vp_count) {
                    gpp_vp_count++;
            }
        }

        if (gpp_vp_count != VP_GROUP_BK(unit)->num_eg_vp_group) {
            LOG_INFO(BSL_LS_BCM_VLAN,
                     (BSL_META_U(unit, "Unmanaged mode set fails:"
                            "VP group auto method already started\n")));

            return BCM_E_EXISTS;
        }
    }
    _bcm_td2p_vp_group_unmanaged[unit].egress = flag;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_dvp_group_validate
 * Purpose:
 *      Check dvp group id valid or not
 */
int
bcm_td2p_dvp_group_validate(int unit, int dvp_group_id)
{
    int max_group_id;     

    max_group_id = (1 << soc_mem_field_length(unit, EGR_DVP_ATTRIBUTEm, 
                                              VT_DVP_GROUP_IDf)) - 1;
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (SOC_MEM_FIELD_VALID(unit,EGR_DVP_ATTRIBUTEm,COMMON__VT_DVP_GROUP_IDf)) {
            max_group_id = (1 << soc_mem_field_length(unit, EGR_DVP_ATTRIBUTEm,
                                              COMMON__VT_DVP_GROUP_IDf)) - 1;
        } else {
            return BCM_E_UNAVAIL;
        }
    }
#endif
    if ((dvp_group_id < 0) || (dvp_group_id > max_group_id)) {
        return BCM_E_PARAM;
    } 
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_params_check
 *
 * Purpose:
 *      Verifies if input parameters are valid for the operation on the device
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id. 
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_PARAM for errorneous inputs
 *      BCM_E_NONE  if all memebers in the structure are legal
 */
STATIC int
bcmi_td2p_vfi_control_params_check(int unit, bcm_vlan_t vid,
                                   uint32 valid_fields,
                                   bcm_vlan_control_vlan_t *control)
{
    uint32 supported_flags = 0;
    uint32 supported_flags2 = 0;
    uint32 supported_fields = 0;

    /*
     * VFI_PROFILE table in TD2Plus supports only the following fields:
     *
     *  - IPMCV4_L2_ENABLE  : Enable IGMP-based L2 lookups for IPMCv4 packets
     *  - IPMCV6_L2_ENABLE  : Enable IGMP-based L2 lookups for IPMCv6 packets
     *  - L2_NON_UCAST_DROP : Drop all broadcast and multicast packets
     *  - L2_NON_UCAST_TOCPU: Copy all broadcast and multicast packets to CPU
     *  - L2_MISS_DROP      : Drop unknown unicast traffic, Do not flood.
     *  - L2_MISS_TOCPU     : Copy unknown unicast traffic to the CPU
     *  - LEARN_DISABLE     : Disable learning.
     *  - EN_IFILTER        : Enable membership checks.
     */

    /*
     * VFI table can be programmed with the following fields:
     *
     *  - VRF           : Virtual Routing and Forwarding Instance
     *  - VFI_CLASS_ID  : VFI Class ID for IFP lookup key
     *  - L3_IIF        : Layer3 Incoming Interface to be assigned to the packet
     *  - UUC_INDEX     : Default unknown unicast group for this VFI
     *  - UMC_INDEX     : Default unknown multicast group for this VFI
     *  - BC_INDEX      : Default Multicast group for broadcasting to this VFI
     */
    supported_fields = (BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK |
                        BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK |
                        BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_VRF_MASK);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
        supported_fields |= BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK;
    }
#endif
    supported_flags =  (BCM_VLAN_IP4_MCAST_L2_DISABLE |
                        BCM_VLAN_IP6_MCAST_L2_DISABLE |
                        BCM_VLAN_UNKNOWN_UCAST_TOCPU |
                        BCM_VLAN_UNKNOWN_UCAST_DROP |
                        BCM_VLAN_NON_UCAST_TOCPU |
                        BCM_VLAN_NON_UCAST_DROP |
                        BCM_VLAN_LEARN_DISABLE |
                        BCM_VLAN_L2_CLASS_ID_ONLY 
                        );

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        supported_flags |= BCM_VLAN_MPLS_DISABLE;
    }
#endif
     
    supported_flags2 = (BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE  |
                        BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE
                        );

#if defined(BCM_HELIX5_SUPPORT)
    supported_flags2 |= (BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT |
                        BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT 
                        );
#endif

    if (valid_fields != BCM_VLAN_CONTROL_VLAN_ALL_MASK) {
        if ((~supported_fields) & valid_fields) {
            return BCM_E_PARAM;
        }
    }

    if ((~supported_flags) & control->flags) {
        return BCM_E_PARAM;
    }

    if ((~supported_flags2) & control->flags2) {
        return BCM_E_PARAM;
    }

    if (control->flags & BCM_VLAN_L2_CLASS_ID_ONLY) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
            if ((control->if_class < 0) ||
                (control->if_class > SOC_INTF_CLASS_MAX(unit))) {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_VRF_MASK) {
        if ((control->vrf < 0) ||
            (control->vrf > SOC_VRF_MAX(unit))) {
            return BCM_E_PARAM;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_intf_vlan_vfi_deoverlay)) {
        if (control->egress_vlan != BCM_VLAN_NONE) {
            return BCM_E_PARAM;
        }
    }
#endif

    VLAN_CHK_ID(unit, control->egress_vlan);


    /*
     * There is no provision to implicitly map L3 IIFs to VFIs.
     * Hence, assigning L3 IIF class id through this API is not allowed.
     * Users should use bcm_l3_ingress_create() to create L3 ingress interfaces
     * and assign class ids through the same API.
     */

    if (!soc_feature(unit, soc_feature_vp_sharing)) {
        if (control->egress_vlan != BCM_VLAN_NONE) {
            return BCM_E_PARAM;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
        VLAN_CHK_ID(unit, control->forwarding_vlan);
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_profile_add
 *
 * Purpose:
 *      Programs VFI_PROFILE table based on input bcm_vlan_control_vlan_t
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (IN) Configuration.
 *     profile_index - (OUT) Profile index allocated by soc_mem_profile_add
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_profile_add(int unit, uint32 valid_fields,
                          bcm_vlan_control_vlan_t *vfi_control,
                          int old_profile_index, int *profile_index)
{
    int i;
    soc_field_t field;
    int field_num = 0;
    uint32 index;
    int value;
    vfi_profile_entry_t vfi_profile_entry;
    vfi_profile_2_entry_t vfi_profile_2_entry;
    soc_mem_t vfi_profile_mem = VFI_PROFILEm;
    void *entries[3];
    int vfi_profile_flags_map[][4] = {
        /* field,                            flag,            bool_invert,          0: flags; 1: flags2 */
        {L2_NON_UCAST_DROPf,   BCM_VLAN_NON_UCAST_DROP,         0,        0},
        {L2_NON_UCAST_TOCPUf,  BCM_VLAN_NON_UCAST_TOCPU,        0,        0},
        {L2_MISS_DROPf,        BCM_VLAN_UNKNOWN_UCAST_DROP,     0,        0},
        {L2_MISS_TOCPUf,       BCM_VLAN_UNKNOWN_UCAST_TOCPU,    0,        0},
        {IPMCV4_L2_ENABLEf,    BCM_VLAN_IP4_MCAST_L2_DISABLE,   1,        0},
        {IPMCV6_L2_ENABLEf,    BCM_VLAN_IP6_MCAST_L2_DISABLE,   1,        0},
        {LEARN_DISABLEf,       BCM_VLAN_LEARN_DISABLE,          0,        0},
        {EN_IFILTERf,          BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE, 1, 1},
        {MPLS_ENABLEf,         BCM_VLAN_MPLS_DISABLE,           1,        0},
            {INVALIDf,             0,                               0}
        };

    entries[0] = &vfi_profile_entry;
    if (soc_feature(unit, soc_feature_vfi_combo_profile)) {
        entries[1] = &vfi_profile_2_entry;
    }
    if (_BCM_INVALID_VFI_PROFILE != old_profile_index) {
        BCM_IF_ERROR_RETURN(
            _bcm_vfi_profile_entry_get(unit, old_profile_index, 1, entries));
    } else {
        sal_memset(&vfi_profile_entry, 0, sizeof(vfi_profile_entry_t));
        sal_memset(&vfi_profile_2_entry, 0, sizeof(vfi_profile_2_entry_t));
    }

    field_num = COUNTOF(vfi_profile_flags_map);
    for (i = 0; i < field_num; i++) {
        field = vfi_profile_flags_map[i][0];
        if (field == INVALIDf) {
            continue;
        }

        if (vfi_profile_flags_map[i][3] == 0) {
        value = ((vfi_profile_flags_map[i][2]) ?
                 (!(vfi_control->flags & vfi_profile_flags_map[i][1])):
                 ((vfi_control->flags & vfi_profile_flags_map[i][1])));
        } else {
            /* flags2 */
            value = ((vfi_profile_flags_map[i][2]) ?
                     (!(vfi_control->flags2 & vfi_profile_flags_map[i][1])):
                     ((vfi_control->flags2 & vfi_profile_flags_map[i][1])));
        }

        value = !!value;
        if (soc_mem_field_valid(unit, vfi_profile_mem, field)) {
            soc_mem_field32_set(unit, vfi_profile_mem, &vfi_profile_entry, field, value);
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_vfi_profile_entry_add(unit, entries, 1, &index));

    *profile_index = index;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_profile_get
 *
 * Purpose:
 *      Reads VFI_PROFILE table and fills vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     profile_index - (IN) Profile index.
 *     vfi_control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_profile_get(int unit, uint8 profile_index, bcm_vlan_control_vlan_t *vfi_control)
{
    int rv;
    int i;
    soc_field_t field;
    int field_num = 0;
    int value;
    vfi_profile_entry_t vfi_profile_entry;
    vfi_profile_2_entry_t vfi_profile_2_entry;
    soc_mem_t vfi_profile_mem = VFI_PROFILEm;
    void *entries[2];
    int vfi_profile_flags_map[][4] = {
        /* field,                         flag,                        bool_invert,  0: flags; 1: flags2 */
        {L2_NON_UCAST_DROPf,   BCM_VLAN_NON_UCAST_DROP,         0,        0},
        {L2_NON_UCAST_TOCPUf,  BCM_VLAN_NON_UCAST_TOCPU,        0,        0},
        {L2_MISS_DROPf,        BCM_VLAN_UNKNOWN_UCAST_DROP,     0,        0},
        {L2_MISS_TOCPUf,       BCM_VLAN_UNKNOWN_UCAST_TOCPU,    0,        0},
        {IPMCV4_L2_ENABLEf,    BCM_VLAN_IP4_MCAST_L2_DISABLE,   1,        0},
        {IPMCV6_L2_ENABLEf,    BCM_VLAN_IP6_MCAST_L2_DISABLE,   1,        0},
        {LEARN_DISABLEf,       BCM_VLAN_LEARN_DISABLE,          0,        0},
        {EN_IFILTERf,          BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE, 1, 1},
        {MPLS_ENABLEf,         BCM_VLAN_MPLS_DISABLE,           1,        0},
            {INVALIDf,             0,                               0}
        };

    entries[0] = &vfi_profile_entry;
    if (soc_feature(unit, soc_feature_vfi_combo_profile)) {
        entries[1] = &vfi_profile_2_entry;
    }

    rv = _bcm_vfi_profile_entry_get(unit, profile_index, 1, entries);
    BCM_IF_ERROR_RETURN(rv);

    field_num = COUNTOF(vfi_profile_flags_map);
    for (i = 0; i < field_num; i++) {
        field = vfi_profile_flags_map[i][0];
        if (field == INVALIDf) {
            continue;
        }
        if (!soc_mem_field_valid(unit, vfi_profile_mem, field)) {
            continue;
        }
        value = soc_mem_field32_get(unit, vfi_profile_mem, &vfi_profile_entry, field);
        if (vfi_profile_flags_map[i][3] == 0) {
        vfi_control->flags |= ((vfi_profile_flags_map[i][2]) ?
                                  ((!value)  ? vfi_profile_flags_map[i][1] : 0) :
                                  ((value) ? vfi_profile_flags_map[i][1] : 0));
        } else {
            /* flags2 */
            vfi_control->flags2 |= ((vfi_profile_flags_map[i][2]) ?
                                       ((!value)  ? vfi_profile_flags_map[i][1] : 0) :
                                       ((value) ? vfi_profile_flags_map[i][1] : 0));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td2p_vfi_control_ingress_set
 *
 * Purpose:
 *      Programs VFI table according to vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_ingress_set(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv;
    int i;
    soc_field_t field;
    int field_num = 0;
    int value;
    int profile_index;
    int old_profile_index;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;
    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        int    *vlan_control_obj;
    } vfi_field_values[] = {
            {VFI_CLASS_IDf, BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK},
            {L3_IIFf,       BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK},
            {UUC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK},
            {UMC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK},
            {BC_INDEXf,     BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK},
            {VRFf,          BCM_VLAN_CONTROL_VLAN_VRF_MASK},
#if defined(BCM_TRIDENT3_SUPPORT)
            {FID_IDf,       BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK}, /*td3 only*/
#endif
            {INVALIDf,      0, 0}
    };

    vfi_field_values[0].vlan_control_obj = &(control->if_class);
    vfi_field_values[1].vlan_control_obj = &(control->ingress_if);
    vfi_field_values[2].vlan_control_obj = &(control->unknown_unicast_group);
    vfi_field_values[3].vlan_control_obj = &(control->unknown_multicast_group);
    vfi_field_values[4].vlan_control_obj = &(control->broadcast_group);
    vfi_field_values[5].vlan_control_obj = &(control->vrf);
#if defined(BCM_TRIDENT3_SUPPORT)
    vfi_field_values[6].vlan_control_obj = (int*)&(control->forwarding_vlan);

    if (!soc_feature(unit, soc_feature_pvlan_on_vfi)) {
        valid_fields &= ~BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK;
    }

    if (soc_feature(unit, soc_feature_generic_dest)) {
        vfi_field_values[2].field = UUC_DESTINATIONf;
        vfi_field_values[3].field = UMC_DESTINATIONf;
        vfi_field_values[4].field = BC_DESTINATIONf;
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_generic_dest)) {
        vfi_field_values[2].field = UUC_DESTINATIONf;
        vfi_field_values[3].field = UMC_DESTINATIONf;
        vfi_field_values[4].field = BC_DESTINATIONf;
    }
#endif

    /*
     * VFI table can be programmed with the following fields:
     *
     *  - VRF           : Virtual Routing and Forwarding Instance
     *  - VFI_CLASS_ID  : VFI Class ID for IFP lookup key
     *  - L3_IIF        : Layer3 Incoming Interface to be assigned to the packet
     *  - UUC_INDEX     : Default unknown unicast group for this VFI
     *  - UMC_INDEX     : Default unknown multicast group for this VFI
     *  - BC_INDEX      : Default Multicast group for broadcasting to this VFI
     */

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    old_profile_index = soc_mem_field32_get(unit, mem,
                                            &vfi_entry, VFI_PROFILE_PTRf);

    field_num = COUNTOF(vfi_field_values);
    for (i = 0; i < field_num; i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf)) {
            continue;
        }
        value = *(vfi_field_values[i].vlan_control_obj);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (field == FID_IDf) {
            value = *((uint16 *)vfi_field_values[i].vlan_control_obj);
        }

        if (soc_feature(unit, soc_feature_generic_dest) &&
                    ((field == UUC_DESTINATIONf) || (field == UMC_DESTINATIONf) ||
                    (field == BC_DESTINATIONf))) {
            soc_mem_field32_dest_set(unit, mem, &vfi_entry, field,
                                                    SOC_MEM_FIF_DEST_IPMC,value);
        } else
#endif
        {
        soc_mem_field32_set(unit, mem, &vfi_entry, field, value);
    }
    }

    if (control->vrf == 0) {
        soc_mem_field32_set(unit, mem, &vfi_entry, VRF_VALIDf, 0);
    } else {
        soc_mem_field32_set(unit, mem, &vfi_entry, VRF_VALIDf, 1);
    }

#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_wirelss_traffic_select)) {
        if (control->flags2 & BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT &&
            control->flags2 & BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT) {
            return BCM_E_PARAM;
        }

        if (control->flags2 & BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT) {
            soc_mem_field32_set(unit, mem, &vfi_entry, WIRED_WIRELESSf, 0x1);
        }

        if (control->flags2 & BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT) {
            soc_mem_field32_set(unit, mem, &vfi_entry, WIRED_WIRELESSf, 0x2);
        }
    }
#endif


    /* Add profile object */
    rv = bcmi_td2p_vfi_profile_add(unit, valid_fields,
            control, old_profile_index, &profile_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Overwrite the the profile pointer in VFI with the new profile index */
    soc_mem_field32_set(unit, mem, &vfi_entry, VFI_PROFILE_PTRf, profile_index);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete the old profile object */
    if (old_profile_index > 0) {
        rv = _bcm_vfi_profile_entry_delete(unit, old_profile_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_egress_set
 *
 * Purpose:
 *      Programs EGR_VFI table according to vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_egress_set(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{

    int rv;
    int i;
    soc_field_t field;
    int field_num = 0;
    int value;
    egr_vfi_entry_t vfi_entry;
    soc_mem_t mem = EGR_VFIm;
    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        uint16    *vlan_control_obj;
    } vfi_field_values[] = {
            {OVIDf, BCM_VLAN_CONTROL_VLAN_ALL_MASK},
            {INVALIDf, 0, NULL}
    };

    vfi_field_values[0].vlan_control_obj = (uint16 *)&(control->egress_vlan);

    /* Read the memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    field_num = COUNTOF(vfi_field_values);
    for (i = 0; i < field_num; i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf) ||
            (soc_feature(unit, soc_feature_egr_intf_vlan_vfi_deoverlay) && (field == OVIDf))) {
            continue;
        }
        value = *(vfi_field_values[i].vlan_control_obj);
        /* update each field */
        if (soc_mem_field_valid(unit, mem, field)) {
        soc_mem_field32_set(unit, mem, &vfi_entry, field, value);
    }
    }

    if (soc_mem_field_valid(unit, mem, EN_EFILTERf)) {
        /* EN_EFILTERf is enabled by default during the VPN creation */
        if (control->flags2 & BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE) {
            soc_mem_field32_set(unit, mem, &vfi_entry, EN_EFILTERf, 0);
        } else {
            soc_mem_field32_set(unit, mem, &vfi_entry, EN_EFILTERf, 1);
        }
    }

    /* Write the memory back */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_ingress_get
 *
 * Purpose:
 *      Reads VFI table and fills vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (OUT) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_ingress_get(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv;
    int i;
    soc_field_t field;
    int field_num = 0;
    int value;
    uint8 profile_index;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;
    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        int    *vlan_control_obj;
    } vfi_field_values[] = {
            {VFI_CLASS_IDf, BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK},
            {L3_IIFf,       BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK},
            {UUC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK},
            {UMC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK},
            {BC_INDEXf,     BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK},
            {VRFf,          BCM_VLAN_CONTROL_VLAN_VRF_MASK},
#if defined(BCM_TRIDENT3_SUPPORT)
            {FID_IDf,       BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK},
#endif
            {INVALIDf,      0, 0}
    };

    vfi_field_values[0].vlan_control_obj = &(control->if_class);
    vfi_field_values[1].vlan_control_obj = &(control->ingress_if);
    vfi_field_values[2].vlan_control_obj = &(control->unknown_unicast_group);
    vfi_field_values[3].vlan_control_obj = &(control->unknown_multicast_group);
    vfi_field_values[4].vlan_control_obj = &(control->broadcast_group);
    vfi_field_values[5].vlan_control_obj = &(control->vrf);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (!soc_feature(unit, soc_feature_pvlan_on_vfi)) {
        valid_fields &= ~BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK;
    }

    if (soc_feature(unit, soc_feature_generic_dest)) {
        vfi_field_values[2].field = UUC_DESTINATIONf;
        vfi_field_values[3].field = UMC_DESTINATIONf;
        vfi_field_values[4].field = BC_DESTINATIONf;
    }
#endif
 
    /* Read the memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    field_num = COUNTOF(vfi_field_values);
    for (i = 0; i < field_num; i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf)) {
            continue;
        }
        /* read each field */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest) &&
            ((field == UUC_DESTINATIONf) || (field == UMC_DESTINATIONf) ||
                    (field == BC_DESTINATIONf))) {
            uint32 dest_type = 0;
            value = soc_mem_field32_dest_get(unit, mem, &vfi_entry, field,
                                            &dest_type);
        } else
#endif
        {
        value = soc_mem_field32_get(unit, mem, &vfi_entry, field);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (field == FID_IDf) {
            control->forwarding_vlan = value & 0xffff;
        } else
#endif
        {
            *(vfi_field_values[i].vlan_control_obj) = value;
        }
    }

    control->flags |= (control->if_class) ? BCM_VLAN_L2_CLASS_ID_ONLY : 0;

#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_wirelss_traffic_select)) {
        value = soc_mem_field32_get(unit, mem, &vfi_entry, WIRED_WIRELESSf);
        if (value == 0x1) {
            control->flags2 |= BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT;
        }
        if (value == 0x2) {
            control->flags2 |= BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT;
        }
    }
#endif

    profile_index = soc_mem_field32_get(unit, mem, &vfi_entry, VFI_PROFILE_PTRf);
    BCM_IF_ERROR_RETURN(bcmi_td2p_vfi_profile_get(unit, profile_index, control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_egress_get
 *
 * Purpose:
 *      Reads EGR_VFI table and fills vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (OUT) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_egress_get(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{

    int rv;
    int i;
    int value;
    soc_field_t field;
    int field_num = 0;
    egr_vfi_entry_t vfi_entry;
    soc_mem_t mem = EGR_VFIm;

    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        uint16    *vlan_control_obj;
    } vfi_field_values[] = {
            {OVIDf, BCM_VLAN_CONTROL_VLAN_ALL_MASK},
            {INVALIDf, 0, NULL}
    };

    vfi_field_values[0].vlan_control_obj = (uint16 *)&(control->egress_vlan);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    field_num = COUNTOF(vfi_field_values);
    for (i = 0; i < field_num; i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf) ||
            (soc_feature(unit, soc_feature_egr_intf_vlan_vfi_deoverlay) && (field == OVIDf))) {
            continue;
        }
        if (soc_mem_field_valid(unit, mem, field)) {
        value = soc_mem_field32_get(unit, mem, &vfi_entry, field);
        *(vfi_field_values[i].vlan_control_obj) = value;
    }
    }

    if (soc_mem_field_valid(unit, mem, EN_EFILTERf)) {
        if (soc_mem_field32_get(unit, mem, &vfi_entry, EN_EFILTERf) == 0) {
            control->flags2 |= BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE;
        } else {
            control->flags2 &= ~BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE;
        }
    }

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}


/*
 * Function :
 *    bcmi_td2p_vlan_control_vpn_set
 *
 * Purpose  :
 *    Configures various controls on the VFI.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) Configuration structure.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vlan_control_vpn_set(int unit, bcm_vlan_t vid, uint32 valid_fields,
                               bcm_vlan_control_vlan_t *control)
{

    int rv;
    int vfi;

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    rv = bcmi_td2p_vfi_control_params_check(unit, vid, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    /* Update VFI & VI_PROFILE tables per vlan_control INPUT */
    rv = bcmi_td2p_vfi_control_ingress_set(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    /* Update EGR_VFI table */
    rv = bcmi_td2p_vfi_control_egress_set(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function :
 *    bcmi_td2p_vlan_control_vpn_get
 *
 * Purpose  :
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     control - (OUT) Configuration structure.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vlan_control_vpn_get(int unit, bcm_vlan_t vid, uint32 valid_fields,
                               bcm_vlan_control_vlan_t *control)
{
    int rv;
    int vfi;

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    rv = bcmi_td2p_vfi_control_ingress_get(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_td2p_vfi_control_egress_get(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_dvp_vlan_xlate_key_set
 * Description:
 *      Set egress vlan translation key per VXLAN gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_id         - (IN) vxlan gport 
 *      key_type      - (IN) Hardware egress vlan translation key type.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_td2p_dvp_vlan_xlate_key_set(int unit, bcm_gport_t port_id, int key_type)
{
    int vp = -1, egr_key_type = 0, rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int network_port = 0;
    source_vp_entry_t svp;
    egr_dvp_attribute_entry_t egr_dvp_attribute;
    _bcm_vp_info_t vp_info;

    if (BCM_GPORT_IS_VXLAN_PORT(port_id)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_MIM_PORT(port_id)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_MPLS_PORT(port_id)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_L2GRE_PORT(port_id)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
        }
    }

    /* Check for Network-Port */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port = TRUE;
    } else {
        network_port = FALSE;
    }

    sal_memset(&egr_dvp_attribute, 0, 
               sizeof(egr_dvp_attribute_entry_t));
    /* coverity[negative_returns] */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if ((network_port) && (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan))) {
        egr_key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                           VXLAN__EVXLT_KEY_SELf);
    } else if ((network_port) && (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre))) {
        egr_key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        L2GRE__EVXLT_KEY_SELf);
    } else {
        egr_key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                           COMMON__EVXLT_KEY_SELf);
    }
    if (egr_key_type != key_type) { 
        if ((network_port) && (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan))) {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute, 
                                VXLAN__EVXLT_KEY_SELf, key_type);
        } else if ((network_port) && (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre))) {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                                L2GRE__EVXLT_KEY_SELf, key_type);
        } else {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute, 
                                COMMON__EVXLT_KEY_SELf, key_type);
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, 
                           vp, &egr_dvp_attribute);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_dvp_vlan_xlate_key_get
 * Description:
 *      Get egress vlan translation key per gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_id         - (IN) vxlan gport 
 *      key_type      - (IN) Hardware egress vlan translation key type.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_td2p_dvp_vlan_xlate_key_get(int unit, bcm_gport_t port_id, int *key_type)
{
    int vp = -1, rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int network_port = 0;
    source_vp_entry_t svp;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    _bcm_vp_info_t vp_info;

    if (key_type == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_VXLAN_PORT(port_id)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_MIM_PORT(port_id)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_MPLS_PORT(port_id)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_L2GRE_PORT(port_id)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_NOT_FOUND;
        }
    }

    /* Check for Network-Port */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port = TRUE;
    } else {
        network_port = FALSE;
    }
 
    sal_memset(&egr_dvp_attribute, 0, 
               sizeof(egr_dvp_attribute_entry_t));
    /* coverity[negative_returns] */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if ((network_port) && (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan))) {
        *key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                        VXLAN__EVXLT_KEY_SELf);
    } else if ((network_port) && (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre))) {
        *key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        L2GRE__EVXLT_KEY_SELf);
    } else {
        *key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                        COMMON__EVXLT_KEY_SELf);
    }

    return rv;
}

int
bcm_td2p_ing_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *buf = NULL;
    int i, entry_index_min, entry_index_max;
    uint32 *buf_entry;
    int vp;
    int sp_state;
    int vfi = -1;
    _bcm_vp_vlan_mbrship_key_type_e key_type;
    soc_field_t vld_fld = VALIDf;

    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vlan);
    }

    if (NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }
    if (arr_size != soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    if (NULL == flags_arr) {
        return BCM_E_PARAM;
    }

    mem = ING_VP_VLAN_MEMBERSHIPm;

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = sizeof(ing_vp_vlan_membership_entry_t) * chunk_size;
    buf = soc_cm_salloc(unit, alloc_size, "ING_VP_VLAN_MEMBERSHIP buffer");
    if (NULL == buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk to extract VP and flags */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            buf_entry = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, buf, i);

            key_type = soc_mem_field32_get(unit, mem, buf_entry, KEY_TYPEf);

            if (soc_mem_field32_get(unit, mem, buf_entry, vld_fld) == 0) {
                continue;
            }

            if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
                if (soc_mem_field32_get(unit, mem, buf_entry, VFIf) != vfi) {
                    continue;
                }
            } else {
                if (soc_mem_field32_get(unit, mem, buf_entry, VLANf) != vlan) {
                    continue;
                }
            }

            vp = soc_mem_field32_get(unit, mem, buf_entry, VPf);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                continue;
            }
            SHR_BITSET(vp_bitmap, vp);

            sp_state = soc_mem_field32_get(unit, mem, buf_entry, SP_TREEf);
            if (sp_state == PVP_STP_DISABLED) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_DISABLE;
            } else if (sp_state == PVP_STP_LEARNING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_LEARN;
            } else if (sp_state == PVP_STP_BLOCKING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_BLOCK;
            } else {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_FORWARD;
            }
        }
    }

cleanup:
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return rv;
}

int
bcm_td2p_egr_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *buf = NULL;
    int i, entry_index_min, entry_index_max;
    uint32 *buf_entry;
    int vp;
    int sp_state;
    int untag;
    int vfi = -1;
    _bcm_vp_vlan_mbrship_key_type_e key_type;
    soc_field_t vld_fld = VALIDf;

    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vlan);
    }

    if (NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }
    if (arr_size != soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    if (NULL == flags_arr) {
        return BCM_E_PARAM;
    }

    mem = EGR_VP_VLAN_MEMBERSHIPm;
    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = sizeof(egr_vp_vlan_membership_entry_t) * chunk_size;
    buf = soc_cm_salloc(unit, alloc_size, "EGR_VP_VLAN_MEMBERSHIP buffer");
    if (NULL == buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk to extract VP and flags */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            buf_entry = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, buf, i);

            key_type = soc_mem_field32_get(unit, mem, buf_entry, KEY_TYPEf);

            if (soc_mem_field32_get(unit, mem, buf_entry, vld_fld) == 0) {
                continue;
            }

            if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
                if (soc_mem_field32_get(unit, mem, buf_entry, VFIf) != vfi) {
                    continue;
                }
            } else {
                if (soc_mem_field32_get(unit, mem, buf_entry, VLANf) != vlan) {
                    continue;
                }
            }

            vp = soc_mem_field32_get(unit, mem, buf_entry, VPf);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                continue;
            }
            SHR_BITSET(vp_bitmap, vp);

            sp_state = soc_mem_field32_get(unit, mem, buf_entry, SP_TREEf);
            if (sp_state == PVP_STP_DISABLED) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_DISABLE;
            } else if (sp_state == PVP_STP_LEARNING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_LEARN;
            } else if (sp_state == PVP_STP_BLOCKING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_BLOCK;
            } else {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_FORWARD;
            }

            untag = soc_mem_field32_get(unit, mem, buf_entry, UNTAGf);
            if (untag) {
                flags_arr[vp] |= BCM_VLAN_PORT_UNTAGGED;
            }
        }
    }

cleanup:
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return rv;
}

int
bcm_td2p_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan, int array_max,
                                    bcm_gport_t *gport_array, int *flags_array,
                                    int *array_size)
{
    int i = 0;
    int rv = 0;
    int num_vp = 0;
    SHR_BITDCL *ing_vp_bitmap = NULL;
    int *ing_flags_arr = NULL;
    SHR_BITDCL *egr_vp_bitmap = NULL;
    int *egr_flags_arr = NULL;
    int flags = 0;

    if ((array_max > 0) && (NULL == gport_array)) {
        return BCM_E_PARAM;
    }

    if ((array_max > 0) && (*array_size >= array_max)) {
        return BCM_E_NONE;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    ing_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "ing_vp_bitmap");
    if (NULL == ing_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    ing_flags_arr = sal_alloc(num_vp * sizeof(int), "ing_flags_arr");
    if (NULL == ing_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_flags_arr, 0, num_vp * sizeof(int));

    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        rv = bcm_td2p_ing_vp_vlan_membership_get_all(unit, vlan, ing_vp_bitmap,
                num_vp, ing_flags_arr);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    egr_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "egr_vp_bitmap");
    if (NULL == egr_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    egr_flags_arr = sal_alloc(num_vp * sizeof(int), "egr_flags_arr");
    if (NULL == egr_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_flags_arr, 0, num_vp * sizeof(int));

    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        rv = bcm_td2p_egr_vp_vlan_membership_get_all(unit, vlan, egr_vp_bitmap,
                num_vp, egr_flags_arr);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    for (i = 0; i < num_vp; i++) {
        if (!SHR_BITGET(ing_vp_bitmap, i) &&
            !SHR_BITGET(egr_vp_bitmap, i)) {
            continue;
        }

        if (0 == array_max) {
            (*array_size)++;
        } else {
            if (_bcm_vp_used_get(unit, i, _bcmVpTypeVlan)) {
                BCM_GPORT_VLAN_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
                BCM_GPORT_NIV_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeExtender)) {
                BCM_GPORT_EXTENDER_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeVxlan)) {
                BCM_GPORT_VXLAN_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeMpls)) {
                BCM_GPORT_MPLS_PORT_ID_SET(gport_array[*array_size], i);
            } else {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            if (NULL != flags_array) {
                flags_array[*array_size] = ing_flags_arr[i] |
                                           egr_flags_arr[i] |
                                           BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;

                if (SHR_BITGET(ing_vp_bitmap, i) && !SHR_BITGET(egr_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
                }

                if (!SHR_BITGET(ing_vp_bitmap, i) && SHR_BITGET(egr_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
                }

                rv = bcm_tr2_vlan_gport_get(unit, vlan, gport_array[*array_size], &flags);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                flags_array[*array_size] |= flags;

            }

            (*array_size)++;
            if (*array_size == array_max) {
                break;
            }
        }
    }

cleanup:
    if (NULL != ing_vp_bitmap) {
        sal_free(ing_vp_bitmap);
    }
    if (NULL != ing_flags_arr) {
        sal_free(ing_flags_arr);
    }
    if (NULL != egr_vp_bitmap) {
        sal_free(egr_vp_bitmap);
    }
    if (NULL != egr_flags_arr) {
        sal_free(egr_flags_arr);
    }

    return rv;

}

/*
 * Function :
 *    bcmi_td2p_vfi_block_update
 *
 * Purpose  :
 *    Update blocking profile on VFI.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     block   - (IN) Block configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vfi_block_update(int unit, int vfi, _trx_vlan_block_t *block)
{
    int rv;                 /* Operation return status. */
    soc_mem_t mem;          /* Vlan profile table memory. */
    int old_profile_idx;    /* Profile index.           */
    void *entries[3], *entry;
    vfi_profile_entry_t vfi_profile_entry;
    vfi_profile_2_entry_t vfi_profile_2_entry;
    uint32 profile_index;
    vfi_entry_t vfi_entry;

    /* Input parameters check. */
    if (NULL == block) {
        return (BCM_E_PARAM);
    }

    /* Get original vlan profile. */
    rv = soc_mem_read(unit, VFIm, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    old_profile_idx = soc_mem_field32_get(unit, VFIm,
                                          &vfi_entry, VFI_PROFILE_PTRf);

    mem = VFI_PROFILE_2m;
    entries[0] = &vfi_profile_entry;
    entries[1] = &vfi_profile_2_entry;
    BCM_IF_ERROR_RETURN(
        _bcm_vfi_profile_entry_get(unit, old_profile_idx, 1, entries));

    entry = &vfi_profile_2_entry;
    /* Bitmap A  */
    soc_mem_pbmp_field_set(unit, mem, entry, BLOCK_MASK_Af,
                           &block->first_mask);

    /* Bitmap B  */
    soc_mem_pbmp_field_set(unit, mem, entry, BLOCK_MASK_Bf,
                           &block->second_mask);

    /* Broadcast mask select. */
    soc_mem_field32_set(unit, mem, entry, BCAST_MASK_SELf,
                        block->broadcast_mask_sel);

    /* Unknown unicast mask select. */
    if (soc_mem_field_valid(unit, mem,
                            UNKNOWN_UCAST_MASK_SELf)) {
        soc_mem_field32_set(unit, mem, entry, UNKNOWN_UCAST_MASK_SELf,
                        block->unknown_unicast_mask_sel);
    }

    /* Unknown multicast mask select. */
    if (soc_mem_field_valid(unit, mem,
                            UNKNOWN_MCAST_MASK_SELf)) {
        soc_mem_field32_set(unit, mem, entry, UNKNOWN_MCAST_MASK_SELf,
                        block->unknown_mulitcast_mask_sel);
    }

    /* Unknown multicast mask select. */
    if (soc_mem_field_valid(unit, mem, KNOWN_MCAST_MASK_SELf)) {
        soc_mem_field32_set(unit, mem, entry, KNOWN_MCAST_MASK_SELf,
                        block->known_mulitcast_mask_sel);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_vfi_profile_entry_add(unit, entries, 1, &profile_index));

    /* Overwrite the the profile pointer in VFI with the new profile index */
    soc_mem_field32_set(unit, VFIm, &vfi_entry, VFI_PROFILE_PTRf,
                        profile_index);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, VFIm, MEM_BLOCK_ALL, vfi, &vfi_entry));

    /* Delete the old profile object */
    if (old_profile_idx > 0) {
        BCM_IF_ERROR_RETURN(
            _bcm_vfi_profile_entry_delete(unit, old_profile_idx));
    }

    return (BCM_E_NONE);
}

/*
 * Function :
 *    bcmi_td2p_vlan_block_set
 *
 * Purpose  :
 *    Configures VFI specific traffic blocking parameters.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     block   - (IN) Block configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    bcm_pbmp_t         array[_BCM_VLAN_BLOCK_LOOKUP_TYPES];
    _trx_vlan_block_t  hw_block;       /* HW block structure.      */
    int                rv;             /* Operation return status. */
    int vfi;

    /* Input parameters check. */
    if (NULL == block) {
        return (BCM_E_PARAM);
    }

    /* Store block mask in a array. */
    BCM_PBMP_ASSIGN(array[0], block->broadcast);
    BCM_PBMP_ASSIGN(array[1], block->unknown_unicast);
    BCM_PBMP_ASSIGN(array[2], block->unknown_multicast);
    BCM_PBMP_ASSIGN(array[3], block->known_multicast);

    /* Select first  shortest mask. */

    /* Coverity
     * The index coverity thinks outside of boundary is actually for
     * the field pbits of the pbmp structure.
     */
    /* coverity[overrun-buffer-val: FALSE] */
    rv = _vlan_block_bitmap_min(unit, array,
                                _BCM_VLAN_BLOCK_LOOKUP_TYPES,
                                &hw_block.first_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear those masks in the array that exactly match the shortest mask. */
    /* coverity[overrun-buffer-val: FALSE] */
    rv = _vlan_block_bitmap_clear(unit, array,
                                  _BCM_VLAN_BLOCK_LOOKUP_TYPES,
                                  hw_block.first_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Select second shortest mask. */
    /* coverity[overrun-buffer-val: FALSE] */
    rv = _vlan_block_bitmap_min(unit, array,
                                _BCM_VLAN_BLOCK_LOOKUP_TYPES,
                                &hw_block.second_mask);
    BCM_IF_ERROR_RETURN(rv);


    /* Get mask selector for each lookup type. */
    rv = _vlan_block_select_get(unit, block->broadcast, hw_block.first_mask,
                                hw_block.second_mask,
                                &hw_block.broadcast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_select_get(unit, block->unknown_unicast,
                                hw_block.first_mask, hw_block.second_mask,
                                &hw_block.unknown_unicast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_select_get(unit, block->unknown_multicast,
                                hw_block.first_mask, hw_block.second_mask,
                                &hw_block.unknown_mulitcast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_select_get(unit, block->known_multicast,
                                hw_block.first_mask, hw_block.second_mask,
                                &hw_block.known_mulitcast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    BCM_IF_ERROR_RETURN(
        bcmi_td2p_vfi_block_update(unit, vfi, &hw_block));

    return (BCM_E_NONE);
}

/*
 * Function :
 *    bcmi_td2p_vlan_block_get
 *
 * Purpose  :
 *    Get VFI specific traffic blocking parameters
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     block   - (IN) Block configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vlan_block_get (int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    _trx_vlan_block_t  hw_block; /* Vlan block profile.      */
    int                rv;        /* Operation return status. */
    soc_mem_t mem;
    int profile_index;
    vfi_entry_t vfi_entry;
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.     */
    int vfi;

    /* Input parameters check. */
    if (NULL == block) {
        return (BCM_E_PARAM);
    }

    /* Get original vlan profile. */
    mem = VFIm;
    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    profile_index = soc_mem_field32_get(unit, mem,
                                        &vfi_entry, VFI_PROFILE_PTRf);

    /* READ VFI_PROFILE_2 portion. */
    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    mem = VFI_PROFILE_2m;

    /* READ vfi profile 2 table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, profile_index, buf);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Bitmap A  */
    soc_mem_pbmp_field_get(unit, mem, buf, BLOCK_MASK_Af,
                           &hw_block.first_mask);

    /* Bitmap B  */
    soc_mem_pbmp_field_get(unit, mem, buf, BLOCK_MASK_Bf,
                           &hw_block.second_mask);

    /* Broadcast mask select. */
    hw_block.broadcast_mask_sel =
        soc_mem_field32_get(unit, mem, buf, BCAST_MASK_SELf);

    /* Unknown unicast mask select. */
    hw_block.unknown_unicast_mask_sel =
        soc_mem_field32_get(unit, mem, buf, UNKNOWN_UCAST_MASK_SELf);

    /* Unknown multicast mask select. */
    hw_block.unknown_mulitcast_mask_sel =
        soc_mem_field32_get(unit, mem, buf, UNKNOWN_MCAST_MASK_SELf);

    /* Unknown multicast mask select. */
    hw_block.known_mulitcast_mask_sel =
        soc_mem_field32_get(unit, mem, buf, KNOWN_MCAST_MASK_SELf);

    rv = _vlan_block_mask_create (unit, hw_block.first_mask,
                                  hw_block.second_mask,
                                  hw_block.broadcast_mask_sel,
                                  &block->broadcast);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_mask_create (unit, hw_block.first_mask,
                                  hw_block.second_mask,
                                  hw_block.unknown_unicast_mask_sel,
                                  &block->unknown_unicast);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_mask_create (unit,
                                  hw_block.first_mask,
                                  hw_block.second_mask,
                                  hw_block.unknown_mulitcast_mask_sel,
                                  &block->unknown_multicast);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_mask_create (unit,
                                  hw_block.first_mask,
                                  hw_block.second_mask,
                                  hw_block.known_mulitcast_mask_sel,
                                  &block->known_multicast);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT2_SUPPORT)

/*
 * Function:
 *     bcm_td2p_vlan_vfi_mbrship_port_get
 * Purpose:
 *      Read the ING_PORT_BITMAP from a vlan entry.
 */
int
bcm_td2p_vlan_vfi_mbrship_port_get(int unit,
                    bcm_vlan_t vlan_vpn, int egress, pbmp_t *pbmp)
{
    uint32 profile_idx;
    soc_mem_t mbrship_mem;
    soc_field_t port_field;
    uint32 mbrship_entry[SOC_MAX_MEM_WORDS];

    if (egress) {
        mbrship_mem = EGR_VLAN_VFI_MEMBERSHIPm;
        port_field = PORT_BITMAPf;
    } else {
        mbrship_mem = ING_VLAN_VFI_MEMBERSHIPm;
        port_field = ING_PORT_BITMAPf;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                                unit, vlan_vpn, egress, &profile_idx));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mbrship_mem,
                MEM_BLOCK_ANY, profile_idx, &mbrship_entry));

    soc_mem_pbmp_field_get(unit, mbrship_mem, &mbrship_entry, port_field, pbmp);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vlan_vfi_mbrship_profile_entry_set
 * Purpose:
 *      Given a VLAN/VFI value and a direction (Igr/Egr), set the
 *      data associated with a profile entry and update the VLAN/VFI
 *      with a new profile index
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vpn - (IN) VLAN/VPN ID
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      entry_data - (IN) Entry to write
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vlan_vfi_mbrship_profile_entry_set(int unit,
                                              bcm_vlan_t vlan_vpn,
                                              int egress,
                                              void *entry_data)
{
    uint32 cur_prof_index = 0, new_prof_index = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                        unit, vlan_vpn, egress, &cur_prof_index));

    BCM_IF_ERROR_RETURN(_bcm_vlan_vfi_membership_profile_entry_op(
                unit, &entry_data, 1, TRUE, egress, &new_prof_index));
    /* Write the new index into the VALN/VFI tables */
    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_set(
                            unit, vlan_vpn, egress, new_prof_index));

    /*
     * If the old-profile-index entry is not default, delete it, note that if the
     * same index is reallocated, the profile-mgmt would have incremented the
     * ref-count anyway.
     */
    if(cur_prof_index != 0 && cur_prof_index != 1) {
        BCM_IF_ERROR_RETURN(_bcm_vlan_vfi_membership_profile_entry_op(
                            unit, NULL, 1, FALSE, egress, &cur_prof_index));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vlan_vpn_mbrship_profile_get
 * Purpose:
 *      Per direction get membership profile of the given vlan/vpn.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vpn   - (IN) VLAN/VPN ID
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      prof_index - (OUT) The associated profile index if all goes well
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vlan_vpn_mbrship_profile_get(int unit,
                                            bcm_vlan_t vlan_vpn,
                                            int egress,
                                            uint32 *prof_index)
{
    int index;
    soc_mem_t mem;
    uint32 vlan_vfi_entry[SOC_MAX_MEM_WORDS];

    if(prof_index == NULL) {
        return BCM_E_PARAM;
    }

    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        if (egress) {
            mem = EGR_VFIm;
        } else {
            mem = VFIm;
        }

        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vpn);

    } else {
        if (egress) {
            mem = EGR_VLANm;
        } else if (SOC_MEM_FIELD_VALID(unit, VLAN_ATTRS_1m,
                                       MEMBERSHIP_PROFILE_PTRf)) {
            mem = VLAN_ATTRS_1m;
        } else {
            mem = VLAN_MPLSm;
        }

        index = vlan_vpn;
    }

    /* Read the memory and extract the field in question */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                MEM_BLOCK_ANY, index, vlan_vfi_entry));

    /* Assign the profile index */
    *prof_index = soc_mem_field32_get(unit, mem, vlan_vfi_entry,
                                      MEMBERSHIP_PROFILE_PTRf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vlan_vpn_mbrship_profile_set
 * Purpose:
 *      Per direction set membership profile of the given vlan/vpn.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vpn   - (IN) VLAN/VPN ID
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      prof_index - (OUT) The associated profile index if all goes well
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vlan_vpn_mbrship_profile_set(int unit,
                                            bcm_vlan_t vlan_vpn,
                                            int egress,
                                            int prof_index)
{
    int                 index = -1;
    soc_mem_t           mem;

    /* Depending on VLAN/VFI, write to relevant memories */
    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        if (egress) {
            mem = EGR_VFIm;
        } else {
            mem = VFIm;
        }

        /* Index in case of a VFI is the encoded value */
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vpn);

    } else {
        if (egress) {
            mem = EGR_VLANm;
        } else if (SOC_MEM_FIELD_VALID(unit, VLAN_ATTRS_1m,
                                       MEMBERSHIP_PROFILE_PTRf)) {
            mem = VLAN_ATTRS_1m;
        } else {
            mem = VLAN_MPLSm;
        }

        index = vlan_vpn;
    }

    BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
        mem, index, MEMBERSHIP_PROFILE_PTRf, prof_index));

    if (mem == VLAN_ATTRS_1m &&
        soc_mem_field_valid(unit, VLAN_TABm, MEMBERSHIP_PROFILE_PTRf)) {
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
            VLAN_TABm, index, MEMBERSHIP_PROFILE_PTRf, prof_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vlan_vfi_mbrship_port_set
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td2p_vlan_vfi_mbrship_port_set(int unit, bcm_vlan_t vlan_vpn,
                                                    int egress, int add, pbmp_t pbmp)
{
    uint32 profile_idx;
    pbmp_t cur_pbmp;
    pbmp_t tmp_pbmp;
    soc_mem_t mbrship_mem;
    soc_field_t port_field;
    uint32 mbrship_entry[SOC_MAX_MEM_WORDS];

    if (egress) {
        mbrship_mem = EGR_VLAN_VFI_MEMBERSHIPm;
        port_field = PORT_BITMAPf;
    } else {
        mbrship_mem = ING_VLAN_VFI_MEMBERSHIPm;
        port_field = ING_PORT_BITMAPf;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                                unit, vlan_vpn, egress, &profile_idx));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mbrship_mem,
                MEM_BLOCK_ANY, profile_idx, &mbrship_entry));

    soc_mem_pbmp_field_get(unit, mbrship_mem, &mbrship_entry, port_field, &cur_pbmp);
    BCM_PBMP_ASSIGN(tmp_pbmp, cur_pbmp);

    if (add) {
        BCM_PBMP_OR(cur_pbmp, pbmp);
    } else {
        BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    }

    if (BCM_PBMP_NEQ(tmp_pbmp, cur_pbmp)) {
        soc_mem_pbmp_field_set(unit, mbrship_mem,
                &mbrship_entry, port_field, &cur_pbmp);
        BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vfi_mbrship_profile_entry_set(
                                    unit, vlan_vpn, egress, &mbrship_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vlan_vpn_membership_delete
 * Purpose:
 *      Delete the VLAN Vfi membership entry.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      vlan_vpn  - (IN) VLAN/VPN ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vlan_vpn_membership_delete(int unit, bcm_vlan_t vlan_vpn)
{
    uint32 profile_ptr = 0;

    /* Delete ingress vlan_vfi_membership profile */
    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                            unit, vlan_vpn, FALSE, &profile_ptr));

    if(profile_ptr != 0 && profile_ptr != 1) {
        BCM_IF_ERROR_RETURN(_bcm_vlan_vfi_membership_profile_entry_op(
                                unit, NULL, 1, FALSE, FALSE, &profile_ptr));
    }

    /* Delete egress vlan_vfi_membership profile */
    BCM_IF_ERROR_RETURN(_bcm_td2p_vlan_vpn_mbrship_profile_get(
                            unit, vlan_vpn, TRUE, &profile_ptr));

    if(profile_ptr != 0 && profile_ptr != 1) {
        BCM_IF_ERROR_RETURN(_bcm_vlan_vfi_membership_profile_entry_op(
                                unit, NULL, 1, FALSE, TRUE, &profile_ptr));
    }

    return BCM_E_NONE;
}

#endif /* defined(BCM_TRIDENT2_SUPPORT) */
