/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <bcm/switch.h>
#include <bcm/error.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/hurricane3.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>

#ifdef BCM_HURRICANE3_SUPPORT

/*
 * Software book keeping for Switch Encap related information
 */
typedef struct _bcm_hr3_switch_encap_bookkeeping_s {
#ifdef INCLUDE_L3
    SHR_BITDCL  *intf_bitmap;                           /* L3 interfaces used */
    uint32  *miml_encap;                                  /* MiML encap id */
#endif /* INCLUDE_L3 */
    uint32  *custom_header_encap;                  /* Custom Header encap id */
} _bcm_hr3_switch_encap_bookkeeping_t;

_bcm_hr3_switch_encap_bookkeeping_t
    _bcm_hr3_switch_encap_bk_info[BCM_MAX_NUM_UNITS];

/* Flag to check initialized status */
STATIC int _bcm_hr3_encap_initialized[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _hr3_encap_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define HR3_ENCAP_INFO(_unit_) (&_bcm_hr3_switch_encap_bk_info[_unit_])

#define HR3_ENCAP_CONTINUOUS_ENTRIES_MAX 128

#define HR3_ENCAP_INIT(unit)                              \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!_bcm_hr3_encap_initialized[unit]) {          \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/* Function Enter/Leave Tracing */
#define HR3_ENCAP_FUNCTION_TRACE(_u_, _str_)                                  \
    do {                                                                      \
        LOG_VERBOSE(BSL_LS_BCM_SWITCH,                                        \
                    (BSL_META_U((_u_),                                        \
                                "%s: " _str_ "\n"), FUNCTION_NAME()));        \
    } while (0)

/*
 * Switch Encap lock
 */
#define HR3_ENCAP_LOCK(unit) \
        sal_mutex_take(_hr3_encap_mutex[unit], sal_mutex_FOREVER);

#define HR3_ENCAP_UNLOCK(unit) \
        sal_mutex_give(_hr3_encap_mutex[unit]);

#ifdef INCLUDE_L3
#define HR3_L3_INFO(unit)    (&_bcm_l3_bk_info[unit])
/*
 * L3 interface usage bitmap operations
 */
#define HR3_MIML_INTF_USED_GET(_u_, _intf_) \
    SHR_BITGET(HR3_ENCAP_INFO(_u_)->intf_bitmap, (_intf_))
#define HR3_MIML_INTF_USED_SET(_u_, _intf_) \
    SHR_BITSET(HR3_ENCAP_INFO((_u_))->intf_bitmap, (_intf_))
#define HR3_MIML_INTF_USED_CLR(_u_, _intf_) \
    SHR_BITCLR(HR3_ENCAP_INFO((_u_))->intf_bitmap, (_intf_))

typedef struct _bcm_hr3_ing_nh_info_s {
    int      port;
    int      module;
    int      trunk;
} _bcm_hr3_ing_nh_info_t;

typedef struct _bcm_hr3_egr_nh_info_s {
    uint8    entry_type;
    int      intf_num;
    int      macda_index;
    int      pri;
    int      cfi;
} _bcm_hr3_egr_nh_info_t;
#endif /* INCLUDE_L3 */

/* Function:
*	   _bcm_hr3_switch_encap_idx2id
* Purpose:
*	   Translate hardware table index into encap ID used by API
* Parameters:
* Returns:
*	   BCM_E_XXX
*/
int
_bcm_hr3_switch_encap_idx2id
    (int unit, int type, int index_count, int *index_array,  bcm_if_t *encap_id)
{
    int idx, id = 0;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");

    if (index_array == NULL) {
        return BCM_E_PARAM;
    }

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case _BCM_SWITCH_ENCAP_TYPE_MIML:
            if (index_count < 2) {
                return BCM_E_PARAM;
            }

            for (idx = 0; idx < index_count; idx++) {
                if (idx == 0) {
                    id = index_array[idx];
                 } else if (idx == 1) {
                    id |= (index_array[idx] <<
                        _BCM_SWITCH_ENCAP_MIML_EGR_HEADER_ENCAP_SHIFT);
                }
            }
            break;
        case _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER:
            if (index_count < 1) {
                return BCM_E_PARAM;
            }

            for (idx = 0; idx < index_count; idx++) {
                if (idx == 0) {
                    id = index_array[idx];
                }
            }
            break;
        default:
            return BCM_E_NOT_FOUND;
    }

    *encap_id = id  | (type << _BCM_SWITCH_ENCAP_SHIFT);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/* Function:
*	   _bcm_hr3_switch_encap_id2idx
* Purpose:
*	   Translate encap ID into hardware table index used by API
* Parameters:
* Returns:
*	   BCM_E_XXX
*/
int
_bcm_hr3_switch_encap_id2idx
    (int unit, bcm_if_t encap_id, int index_count, int *index_array)
{
    int idx, id = 0;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    if (index_array == NULL) {
        return BCM_E_PARAM;
    }

    id = encap_id & _BCM_SWITCH_ENCAP_TYPE_MASK;

    switch (encap_id >> _BCM_SWITCH_ENCAP_SHIFT) {
        case _BCM_SWITCH_ENCAP_TYPE_MIML:
            if (index_count < 2) {
                return BCM_E_PARAM;
            }

            for (idx = 0; idx < index_count; idx++) {
                if (idx == 0) {
                    index_array[idx] = id & _BCM_SWITCH_ENCAP_MIML_NHI_MASK;
                } else if (idx == 1) {
                    index_array[idx] =
                        (id >> _BCM_SWITCH_ENCAP_MIML_EGR_HEADER_ENCAP_SHIFT) &
                        _BCM_SWITCH_ENCAP_MIML_EGR_HEADER_ENCAP_MASK;
                } else {
                    index_array[idx] = 0;
                }
            }
            break;
        case _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER:
            if (index_count < 1) {
                return BCM_E_PARAM;
            }

            for (idx = 0; idx < index_count; idx++) {
                if (idx == 0) {
                    index_array[idx] =
                        (id & _BCM_SWITCH_ENCAP_CUSTOM_HEADER_EGR_HEADER_ENCAP_MASK);
                } else {
                    index_array[idx] = 0;
                }
            }
            break;
        default:
            return BCM_E_NOT_FOUND;
    }

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

#ifdef INCLUDE_L3
STATIC int
_bcm_hr3_miml_l3_intf_add(int unit, _bcm_l3_intf_cfg_t *if_info)
{
    int idx, num_intf;
    egr_l3_intf_entry_t egr_intf;
    bcm_mac_t hw_mac;
    bcm_vlan_t hw_ivid;
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    for (idx = 0; idx < num_intf; idx++) {
        if (HR3_MIML_INTF_USED_GET(unit, idx)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm,
                MEM_BLOCK_ANY, idx, &egr_intf));
            soc_mem_mac_addr_get(unit, EGR_L3_INTFm,
                &egr_intf, MAC_ADDRESSf, hw_mac);
            hw_ivid = soc_mem_field32_get(unit, EGR_L3_INTFm, &egr_intf, IVIDf);
            if ((SAL_MAC_ADDR_EQUAL(hw_mac, if_info->l3i_mac_addr)) &&
                (hw_ivid == if_info->l3i_inner_vlan)) {
                if_info->l3i_index = idx;
                return BCM_E_NONE;
            }
        }
    }
    /* Create an interface */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_intf_create(unit, if_info));
    HR3_MIML_INTF_USED_SET(unit, if_info->l3i_index);

    return BCM_E_NONE;
}

STATIC int
_bcm_hr3_miml_l3_nh_info_add(int unit,
    bcm_switch_encap_info_t *encap_info, int *nh_index, int replace)
{
    int rv = BCM_E_NONE;
    int hw_idx = 0;
    egr_l3_next_hop_entry_t egr_nh_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    egr_mac_da_profile_entry_t macda;
    _bcm_hr3_ing_nh_info_t ing_nh_info;
    _bcm_hr3_egr_nh_info_t egr_nh_info;
    bcm_l3_egress_t nh_info;
    _bcm_l3_intf_cfg_t if_info;
    uint32 nh_flags;
    int gport_id, is_local;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int old_macda_idx = -1;
    uint64 temp_mac;
    void *entries[1];

    /* Initialize values */
    is_local = -1;
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    /* Parameter checking */
    if (encap_info->encap_service != BCM_SWITCH_ENCAP_SERVICE_MIML) {
        return BCM_E_PARAM;
    }

    /* Resolve the gport */
    rv = _bcm_esw_gport_resolve(unit, encap_info->port, &mod_out,
              &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Note: encap_info->port is must set for MiML to indicate the destination port */
    if (BCM_GPORT_IS_TRUNK(encap_info->port)) {
        ing_nh_info.module = -1;
        ing_nh_info.port = -1;
        ing_nh_info.trunk = trunk_id;
    } else {
        ing_nh_info.module = mod_out;
        ing_nh_info.port = port_out;
        ing_nh_info.trunk = -1;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, &is_local));
    }

    if (replace) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
            (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ANY, *nh_index, &egr_nh_entry);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /*
         * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
         * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
         * allocated but nothing is written to hardware. The "nh_info"
         * in this case is not used, so just set to all zeros.
         */
         bcm_l3_egress_t_init(&nh_info);

        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* ENTRY_TYPE is MiML */
    if (replace) {
        /* Be sure that the existing entry is programmed to MiML */
        egr_nh_info.entry_type =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry, ENTRY_TYPEf);
        if (egr_nh_info.entry_type != 0x3) { /* != MiML */
            return BCM_E_PARAM;
        }
        /* Remember old MAC DA profile index */
        old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh_entry, MIML__MAC_DA_PROFILE_INDEXf);
    }
    egr_nh_info.entry_type = 0x3;

    /* MIML-DA: Add MAC DA profile */
    sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
    soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm,
        &macda, MAC_ADDRESSf, encap_info->egress_dstmac);
    entries[0] = &macda;
    rv = _bcm_mac_da_profile_entry_add(unit,
              entries, 1, (uint32 *) &egr_nh_info.macda_index);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* MIML-SA: Add MAC SA to an L3 interface entry - ref count if exists */
    if (!BCM_VLAN_VALID(encap_info->egress_vlan)) {
        rv = BCM_E_PARAM;
        goto cleanup;
    }
    sal_memset(&if_info, 0, sizeof(_bcm_l3_intf_cfg_t));
    SAL_MAC_ADDR_TO_UINT64(encap_info->egress_srcmac, temp_mac);
    SAL_MAC_ADDR_FROM_UINT64(if_info.l3i_mac_addr, temp_mac);

    /* MIML-VID */
    if_info.l3i_inner_vlan = encap_info->egress_vlan;

    /* HiGig Header OVID */
    if (encap_info->flags & BCM_SWITCH_ENCAP_MIML_OVERRIDE_HG_HEADER_VID) {
        if (!BCM_VLAN_VALID(encap_info->higig_vlan)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        if (is_local && !IS_HG_PORT(unit, port_out)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        if_info.l3i_vid = encap_info->higig_vlan;
    } else {
        /* For non-HiGig port, the OVID must be dropped by configuring VLAN as untagged set */
        if_info.l3i_vid = encap_info->egress_vlan;
    }

    rv = _bcm_hr3_miml_l3_intf_add(unit, &if_info);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Populate the fields of MIML::EGR_l3_NEXT_HOP */
    if (!replace) {
        sal_memset(&egr_nh_entry, 0, sizeof(egr_l3_next_hop_entry_t));
    }

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
        &egr_nh_entry, ENTRY_TYPEf, egr_nh_info.entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
        &egr_nh_entry, MIML__INTF_NUMf, if_info.l3i_index);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
        &egr_nh_entry, MIML__HG_HDR_SELf, 1); /* HG 2 */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
        &egr_nh_entry, MIML__MAC_DA_PROFILE_INDEXf, egr_nh_info.macda_index);

    /* encap priority selection */
    if (encap_info->flags & BCM_SWITCH_ENCAP_MIML_QOS_MAP_INT_PRI) {
        /* USE_MAPPING */
        rv = _bcm_tr2_qos_id2idx(unit, encap_info->qos_map_id, &hw_idx);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            &egr_nh_entry, MIML__DOT1P_PRI_SELECTf, 0x1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            &egr_nh_entry, MIML__DOT1P_MAPPING_PTRf, hw_idx);
    } else {
        /* USE_FIXED */
        if ((encap_info->pkt_pri > 7) || (encap_info->pkt_cfi > 1)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        /* MIML-NEW_PCP/NEW_CFI */
        egr_nh_info.pri = encap_info->pkt_pri;
        egr_nh_info.cfi = encap_info->pkt_cfi;

        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            &egr_nh_entry, MIML__DOT1P_PRI_SELECTf, 0x0);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            &egr_nh_entry, MIML__NEW_PCPf, egr_nh_info.pri);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            &egr_nh_entry, MIML__NEW_CFIf, egr_nh_info.cfi);
    }

    /* Write EGR_L3_NEXT_HOP entry */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
               MEM_BLOCK_ALL, *nh_index, &egr_nh_entry);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh_entry, 0, sizeof(ing_l3_next_hop_entry_t));
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh_entry, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh_entry, MODULE_IDf, ing_nh_info.module);
    } else {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh_entry, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh_entry, TGIDf, ing_nh_info.trunk);
    }
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
               MEM_BLOCK_ALL, *nh_index, &ing_nh_entry);
    if (rv < 0) {
        goto cleanup;
    }

    /* Delete MAC_DA indexes */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    return rv;

cleanup:
    if (!replace) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    if (egr_nh_info.macda_index != -1) {
        (void) _bcm_mac_da_profile_entry_delete(unit, egr_nh_info.macda_index);
    }

    return rv;
}

STATIC int
_bcm_hr3_miml_l3_nh_info_delete(int unit, int nh_index)
{
    int rv, old_macda_idx = -1;
    egr_l3_next_hop_entry_t egr_nh_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm,
        MEM_BLOCK_ANY, nh_index, &egr_nh_entry));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm,
        MEM_BLOCK_ANY, nh_index, &ing_nh_entry));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry, ENTRY_TYPEf) == 0x3) {
        old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh_entry, MIML__MAC_DA_PROFILE_INDEXf);
    } else {
        return BCM_E_NOT_FOUND;
    }

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh_entry, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
        MEM_BLOCK_ALL, nh_index, &egr_nh_entry));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh_entry, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
        MEM_BLOCK_ALL, nh_index, &ing_nh_entry));

    /* Delete old MAC profile reference */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);

    return rv;
}

STATIC int
_bcm_hr3_miml_l3_nh_info_get(int unit,
    int nh_index, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    egr_l3_next_hop_entry_t egr_nh_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    egr_l3_intf_entry_t egr_intf;
    egr_mac_da_profile_entry_t macda;
    int intf_num = -1, macda_idx = -1;
    int hw_idx = 0;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;
    int is_local = 0;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm,
        MEM_BLOCK_ANY, nh_index, &egr_nh_entry));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm,
        MEM_BLOCK_ANY, nh_index, &ing_nh_entry));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
        trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, TGIDf);
        BCM_GPORT_TRUNK_SET(encap_info->port, trunk_id);
    } else {
        mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, MODULE_IDf);
        port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, PORT_NUMf);
        SOC_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit,
            BCM_STK_MODMAP_GET, mod_in, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(encap_info->port, mod_out, port_out);

        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, &is_local));
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry, ENTRY_TYPEf) == 0x3) {
        /* Get the MiML encap attributes */
        intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh_entry, MIML__INTF_NUMf);
        macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh_entry, MIML__MAC_DA_PROFILE_INDEXf);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm,
            MEM_BLOCK_ANY, intf_num, &egr_intf));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_MAC_DA_PROFILEm,
            MEM_BLOCK_ANY, macda_idx, &macda));

        soc_mem_mac_addr_get(unit, EGR_L3_INTFm,
            &egr_intf, MAC_ADDRESSf, encap_info->egress_srcmac);
        encap_info->egress_vlan =
            soc_mem_field32_get(unit, EGR_L3_INTFm, &egr_intf, IVIDf);

        encap_info->higig_vlan =
            soc_mem_field32_get(unit, EGR_L3_INTFm, &egr_intf, OVIDf);
        /* HiGig Header OVID */
        if (!is_local || (is_local && IS_HG_PORT(unit, port_out))) {
            encap_info->flags |= BCM_SWITCH_ENCAP_MIML_OVERRIDE_HG_HEADER_VID;
        }

        soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm,
            &macda, MAC_ADDRESSf, encap_info->egress_dstmac);

        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh_entry, MIML__DOT1P_PRI_SELECTf) == 0x1) {
            /* USE_MAPPING */
            hw_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh_entry, MIML__DOT1P_MAPPING_PTRf);

            rv = _bcm_tr2_qos_idx2id(unit, hw_idx,
                _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, &encap_info->qos_map_id);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            encap_info->flags |= BCM_SWITCH_ENCAP_MIML_QOS_MAP_INT_PRI;
        } else {
            /* USE_FIXED */
            encap_info->pkt_pri = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh_entry, MIML__NEW_PCPf);
            encap_info->pkt_cfi = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh_entry, MIML__NEW_CFIf);
        }

    } else {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}
#endif /* INCLUDE_L3 */

STATIC int
bcmi_hr3_egr_header_encap_data_fill_content(
    int unit,
    bcm_switch_encap_info_t *encap_info,
    int value_offset,
    egr_header_encap_data_entry_t *egr_header_encap_data)
{
    int rv = BCM_E_NONE;
    uint32 value32;
    int hw_idx = 0;

    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }
    if (egr_header_encap_data == NULL) {
        return BCM_E_PARAM;
    }

    if (encap_info->encap_service == BCM_SWITCH_ENCAP_SERVICE_MIML) {
        value32 =
            (encap_info->length & _BCM_SWITCH_ENCAP_MIML_LENGTH_MASK)
            << _BCM_SWITCH_ENCAP_MIML_LENGTH_SHIFT;
        value32 |=
            (encap_info->iid & _BCM_SWITCH_ENCAP_MIML_IID_MASK)
            << _BCM_SWITCH_ENCAP_MIML_IID_SHIFT;
        value32 |=
            (encap_info->pkt_type &
             _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_MASK)
            << _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_SHIFT;

        soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
            egr_header_encap_data, CUSTOM_PACKET_HEADERf, value32);
    } else if (encap_info->encap_service ==
        BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER) {
        /* Set 32-bit data value for encapsulation */
        value32 = encap_info->value32 + value_offset;

        soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
            egr_header_encap_data, CUSTOM_PACKET_HEADERf, value32);

        /* encap priority selection */
        if (encap_info->flags &
            BCM_SWITCH_ENCAP_CUSTOM_HEADER_QOS_MAP_INT_PRI) {
            /* USE_MAPPING */
            rv = _bcm_tr2_qos_id2idx(
                    unit, encap_info->qos_map_id, &hw_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
                egr_header_encap_data, CUSTOM_PRI_SELECTf, 0x1);
            soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
                egr_header_encap_data, CUSTOM_PRI_MAPPING_PTRf,
                hw_idx);
        } else {
            /* USE_FIXED */
            soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
                egr_header_encap_data, CUSTOM_PRI_SELECTf, 0x0);
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_hr3_egr_header_encap_data_add_one_entry(
    int unit,
    bcm_switch_encap_info_t *encap_info,
    int value_offset_base,
    uint32 *eh_idx)
{
    int rv = BCM_E_NONE;

    void *entries[1];
    int entries_per_set;
    int value_offset = value_offset_base;
    egr_header_encap_data_entry_t egr_header_encap_data;
    egr_header_encap_data_entry_t egr_header_encap_data_war[4];

    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }
    if (eh_idx == NULL) {
        return BCM_E_PARAM;
    }

    while (1) {
        sal_memset(&egr_header_encap_data, 0,
            sizeof(egr_header_encap_data_entry_t));
        sal_memset(egr_header_encap_data_war, 0,
            sizeof(egr_header_encap_data_war));
        BCM_IF_ERROR_RETURN(
            bcmi_hr3_egr_header_encap_data_fill_content(
                unit, encap_info, value_offset, &egr_header_encap_data));

        /* Add EGR_HEADER_ENCAP_DATA profile */
        if (soc_feature(unit,
                        soc_feature_hr3_switch_encap_index_shift2_war)) {
            /* WAR for A0: The index of */
            /* EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
            sal_memcpy(&egr_header_encap_data_war[0],
                &egr_header_encap_data, sizeof(egr_header_encap_data));
            entries[0] = &egr_header_encap_data_war;
            entries_per_set = 4;
        } else {
            entries[0] = &egr_header_encap_data;
            entries_per_set = 1;
        }

        rv = _bcm_egr_header_encap_data_entry_search(
                unit, entries, entries_per_set, eh_idx);
        if (value_offset == 0 && rv == BCM_E_EXISTS) {
            /* First entry value already existed */
            return BCM_E_EXISTS;
        } else if (rv == BCM_E_EXISTS) {
            value_offset++;
            continue;
        }
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_egr_header_encap_data_entry_add(
                unit, entries, entries_per_set, eh_idx);
        BCM_IF_ERROR_RETURN(rv);
        break;
    }
    return BCM_E_NONE;
}

STATIC int
bcmi_hr3_egr_header_encap_data_update_one_entry(
    int unit,
    bcm_switch_encap_info_t *encap_info,
    int value_offset,
    uint32 eh_idx)
{
    int rv = BCM_E_NONE;

    void *entries[1];
    egr_header_encap_data_entry_t egr_header_encap_data;
    egr_header_encap_data_entry_t egr_header_encap_data_war[4];

    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }
    sal_memset(&egr_header_encap_data, 0,
        sizeof(egr_header_encap_data_entry_t));
    sal_memset(egr_header_encap_data_war, 0,
        sizeof(egr_header_encap_data_war));
    BCM_IF_ERROR_RETURN(
        bcmi_hr3_egr_header_encap_data_fill_content(
            unit, encap_info, value_offset, &egr_header_encap_data));

    /* Add EGR_HEADER_ENCAP_DATA profile */
    if (soc_feature(unit,
                    soc_feature_hr3_switch_encap_index_shift2_war)) {
        /* WAR for A0: The index of */
        /* EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
        sal_memcpy(&egr_header_encap_data_war[0],
            &egr_header_encap_data, sizeof(egr_header_encap_data));
        entries[0] = &egr_header_encap_data_war;
    } else {
        entries[0] = &egr_header_encap_data;
    }
    rv = _bcm_egr_header_encap_data_entry_update(
            unit, entries, eh_idx);
    BCM_IF_ERROR_RETURN(rv);
    return BCM_E_NONE;
}

STATIC int
_bcm_hr3_egr_header_encap_data_add(int unit,
    bcm_switch_encap_info_t *encap_info,
    uint32 count,
    int *eh_index)
{
    int rv = BCM_E_NONE;
    uint32 eh_idx = 0;
    uint32 continue_cnt = 0;
    uint32 eh_idx_arry[HR3_ENCAP_CONTINUOUS_ENTRIES_MAX];
    int idx_first, idx_next, idx;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");

    /* MIML doesn't support continued N encap entries */
    if (count != 1 &&
        encap_info->encap_service == BCM_SWITCH_ENCAP_SERVICE_MIML) {
        return BCM_E_UNAVAIL;
    }

    assert(count <= HR3_ENCAP_CONTINUOUS_ENTRIES_MAX);

    /*
     * Since the utility soc_profile_mem_add cannot add the same entry data content
     * , for loop here is to create continuous encap custom data by increase
     * CUSTOM_PACKET_HEADERf
     * i.e
     * CUSTOM_PACKET_HEADER=0x99990000
     * CUSTOM_PACKET_HEADER=0x99990001
     * ...
     * CUSTOM_PACKET_HEADER=0x99990000 + count -1
 */

    /* 1. Add 1st entry */
    rv = bcmi_hr3_egr_header_encap_data_add_one_entry
             (unit, encap_info, 0, &eh_idx);
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            *eh_index = eh_idx;
        }
        return rv;
    }
    continue_cnt = 1;
    idx_first = 0;
    idx_next = idx_first + 1;
    eh_idx_arry[idx_first] = eh_idx;

    /* 2. keep adding 1 entry until reaching continues "count" entries */
    while (1) {
        if (continue_cnt == count) {
            break;
        }
        BCM_IF_ERROR_RETURN(
            bcmi_hr3_egr_header_encap_data_add_one_entry(
                unit, encap_info, 1, &eh_idx));
        eh_idx_arry[idx_next] = eh_idx;
        if (eh_idx_arry[idx_next - 1] == eh_idx_arry[idx_next] - 1) {
            continue_cnt++;
        } else {
            continue_cnt = 1;
            idx_first = idx_next;
        }
        idx_next++;
        if (idx_next >= HR3_ENCAP_CONTINUOUS_ENTRIES_MAX) {
            return BCM_E_RESOURCE;
        }
    }

    /* 3. Delete entries from 0 to idx_first -1 */
    for (idx = 0; idx < idx_first; idx++) {
        rv = _bcm_egr_header_encap_data_entry_delete(unit, eh_idx_arry[idx]);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* 4. refill value for 1st entry */
    BCM_IF_ERROR_RETURN(
        bcmi_hr3_egr_header_encap_data_update_one_entry(
            unit, encap_info, 0, eh_idx_arry[idx_first]));

    /* Add EGR_HEADER_ENCAP_DATA profile */
    if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
        /* Shift >> 2 bits for software index */
        *eh_index = (eh_idx_arry[idx_first] >> 2);
    } else {
        *eh_index = eh_idx_arry[idx_first];
    }

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

STATIC int
_bcm_hr3_egr_header_encap_data_delete(int unit, int eh_index)
{
    int rv = BCM_E_NONE;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
        /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
        /* Shift << 2 bits for hardware index */
        eh_index = (eh_index << 2);
    }

    /* Delete existing EGR_HEADER_ENCAP_DATA profile reference */
    rv = _bcm_egr_header_encap_data_entry_delete(unit, eh_index);
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

STATIC int
_bcm_hr3_egr_header_encap_data_update(int unit, bcm_if_t encap_id,
    int eh_index, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    int hw_idx;
    uint32 value32;
    egr_header_encap_data_entry_t egr_header_encap_data;
    egr_header_encap_data_entry_t egr_header_encap_data_war[4];
    void *entries[1];

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");

    if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
        /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
        /* Shift << 2 bits for hardware index */
        eh_index = (eh_index << 2);
    }

    if ((eh_index > soc_mem_index_max(unit, EGR_HEADER_ENCAP_DATAm)) ||
        (eh_index < soc_mem_index_min(unit, EGR_HEADER_ENCAP_DATAm)))  {
        return BCM_E_PARAM;
    }

    sal_memset(&egr_header_encap_data, 0, sizeof(egr_header_encap_data_entry_t));
    sal_memset(egr_header_encap_data_war, 0, sizeof(egr_header_encap_data_war));

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) == _BCM_SWITCH_ENCAP_TYPE_MIML) {
        /* Set MiML Packet Type, IID and Length: use the same field as Custom Header */
        value32 =
            (encap_info->length & _BCM_SWITCH_ENCAP_MIML_LENGTH_MASK) <<
            _BCM_SWITCH_ENCAP_MIML_LENGTH_SHIFT;
        value32 |=
            (encap_info->iid & _BCM_SWITCH_ENCAP_MIML_IID_MASK) <<
            _BCM_SWITCH_ENCAP_MIML_IID_SHIFT;
        value32 |=
            (encap_info->pkt_type & _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_MASK) <<
            _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_SHIFT ;

        soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
            &egr_header_encap_data, CUSTOM_PACKET_HEADERf, value32);
    } else if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
        _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
        /* Set 32-bit data value for encapsulation */
        value32 = encap_info->value32;

        soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
            &egr_header_encap_data, CUSTOM_PACKET_HEADERf, value32);

        /* encap priority selection */
        if (encap_info->flags & BCM_SWITCH_ENCAP_CUSTOM_HEADER_QOS_MAP_INT_PRI) {
            /* USE_MAPPING */
            rv = _bcm_tr2_qos_id2idx(unit, encap_info->qos_map_id, &hw_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
                &egr_header_encap_data, CUSTOM_PRI_SELECTf, 0x1);
            soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
                &egr_header_encap_data, CUSTOM_PRI_MAPPING_PTRf, hw_idx);
        } else {
            /* USE_FIXED */
            soc_mem_field32_set(unit, EGR_HEADER_ENCAP_DATAm,
                &egr_header_encap_data, CUSTOM_PRI_SELECTf, 0x0);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Update EGR_HEADER_ENCAP_DATA profile */
    if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
        /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
        sal_memcpy(&egr_header_encap_data_war[0],
            &egr_header_encap_data, sizeof(egr_header_encap_data));
        /* entries_per_set = 4 */
        entries[0] = &egr_header_encap_data_war;
    } else {
        entries[0] = &egr_header_encap_data;
    }

    rv = _bcm_egr_header_encap_data_entry_update(unit, entries, (uint32)eh_index);
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

STATIC int
_bcm_hr3_egr_header_encap_data_get(int unit, bcm_if_t encap_id,
    int eh_index, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    int hw_idx = 0;
    uint32 value32;
    egr_header_encap_data_entry_t egr_header_encap_data;

    if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
        /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
        /* Shift << 2 bits for hardware index */
        eh_index = (eh_index << 2);
    }

    if ((eh_index > soc_mem_index_max(unit, EGR_HEADER_ENCAP_DATAm)) ||
        (eh_index < soc_mem_index_min(unit, EGR_HEADER_ENCAP_DATAm)))  {
        return BCM_E_PARAM;
    }

    /* Read the existing egress next_hop entry */
    rv = soc_mem_read(unit, EGR_HEADER_ENCAP_DATAm,
        MEM_BLOCK_ANY, eh_index, &egr_header_encap_data);

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) == _BCM_SWITCH_ENCAP_TYPE_MIML) {
        /* Get MiML Packet Type, IID and Length: use the same field as Custom Header */
        value32 = soc_mem_field32_get(unit, EGR_HEADER_ENCAP_DATAm,
            &egr_header_encap_data, CUSTOM_PACKET_HEADERf);

        encap_info->length =
            (value32 >> _BCM_SWITCH_ENCAP_MIML_LENGTH_SHIFT) &
            _BCM_SWITCH_ENCAP_MIML_LENGTH_MASK;
        encap_info->iid =
            (value32 >> _BCM_SWITCH_ENCAP_MIML_IID_SHIFT) &
            _BCM_SWITCH_ENCAP_MIML_IID_MASK;
        encap_info->pkt_type =
            (value32 >> _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_SHIFT) &
            _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_MASK;
    } else if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
        _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
        /* Get 32-bit data value for encapsulation */
        value32 = soc_mem_field32_get(unit, EGR_HEADER_ENCAP_DATAm,
            &egr_header_encap_data, CUSTOM_PACKET_HEADERf);
        encap_info->value32 = value32;

        encap_info->flags = 0;
        /* encap priority selection */
        if (soc_EGR_HEADER_ENCAP_DATAm_field32_get(unit,
            &egr_header_encap_data, CUSTOM_PRI_SELECTf) == 0x1) {
            /* USE_MAPPING */
            hw_idx = soc_mem_field32_get(unit, EGR_HEADER_ENCAP_DATAm,
                &egr_header_encap_data, CUSTOM_PRI_MAPPING_PTRf);

            rv = _bcm_tr2_qos_idx2id(unit, hw_idx,
                _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, &encap_info->qos_map_id);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            encap_info->flags |= BCM_SWITCH_ENCAP_CUSTOM_HEADER_QOS_MAP_INT_PRI;
        }
    } else {
        return BCM_E_PARAM;
    }

    return rv;
}

#ifdef INCLUDE_L3
STATIC int
_bcm_hr3_miml_switch_encap_delete(int unit, int nh_index, int eh_index)
{
    int rv = BCM_E_NONE;

    rv = _bcm_hr3_miml_l3_nh_info_delete(unit, nh_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_hr3_egr_header_encap_data_delete(unit, eh_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return rv;
}

STATIC int
_bcm_hr3_miml_switch_encap_create(int unit,
    bcm_switch_encap_info_t *encap_info, bcm_if_t *encap_id)
{
    int rv = BCM_E_NONE;
    int nh_index, eh_index, index_array[2];
    int idx, num_miml, free_set;

    num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);  /* Depend on IFP */

    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }
    if (encap_info->encap_service != BCM_SWITCH_ENCAP_SERVICE_MIML) {
        return BCM_E_PARAM;
    }

    rv = _bcm_hr3_miml_l3_nh_info_add(unit, encap_info, &nh_index, FALSE);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_hr3_egr_header_encap_data_add(unit, encap_info, 1, &eh_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    index_array[0] = nh_index;
    index_array[1] = eh_index;
    rv = _bcm_hr3_switch_encap_idx2id(unit,
        _BCM_SWITCH_ENCAP_TYPE_MIML, 2, index_array, encap_id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Check if created encap entry is already existed */
    free_set = -1;
    for (idx = 0; idx < num_miml; idx++) {
        if (HR3_ENCAP_INFO(unit)->miml_encap[idx] == 0x0) {
            if (free_set == -1) {
                free_set = idx;
            }
        }

        if (HR3_ENCAP_INFO(unit)->miml_encap[idx] == *encap_id) {
            rv = _bcm_hr3_miml_switch_encap_delete(unit, nh_index, eh_index);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            return BCM_E_EXISTS;
        }
    }

    if (free_set != -1) {
        HR3_ENCAP_INFO(unit)->miml_encap[free_set] = *encap_id;
    } else {
        /* Delete previous added nh_info and egr_header_encap_data */
        rv = _bcm_hr3_miml_switch_encap_delete(unit, nh_index, eh_index);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = BCM_E_FULL;
    }

    return rv;
}

STATIC int
_bcm_hr3_miml_switch_encap_set(int unit,
    bcm_if_t encap_id, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    int nh_index, eh_index, index_array[2];

    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) != _BCM_SWITCH_ENCAP_TYPE_MIML) {
        return BCM_E_PARAM;
    }

    rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, 2, index_array);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    nh_index = index_array[0];
    eh_index = index_array[1];

    rv = _bcm_hr3_miml_l3_nh_info_add(unit, encap_info, &nh_index, TRUE);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_hr3_egr_header_encap_data_update(unit,
        encap_id, eh_index, encap_info);

    return rv;
}

STATIC int
_bcm_hr3_miml_switch_encap_get(int unit,
    bcm_if_t encap_id, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    int nh_index, eh_index, index_array[2];

    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) != _BCM_SWITCH_ENCAP_TYPE_MIML) {
        return BCM_E_PARAM;
    }

    rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, 2, index_array);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    nh_index = index_array[0];
    eh_index = index_array[1];

    rv = _bcm_hr3_miml_l3_nh_info_get(unit, nh_index, encap_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_hr3_egr_header_encap_data_get(unit, encap_id, eh_index, encap_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    encap_info->encap_service = BCM_SWITCH_ENCAP_SERVICE_MIML;

    return rv;
}
#endif /* INCLUDE_L3 */

/* create N Customer header encap objects */
STATIC int
_bcm_hr3_custom_header_switch_encap_create(int unit,
    bcm_switch_encap_info_t *encap_info,
    uint32 count,
    bcm_if_t *encap_id)
{
    int rv = BCM_E_NONE;
    int eh_index, base_eh_index, index_array[1];
    int ref_count = 0;
    int i;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }
    if (encap_info->encap_service != BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER) {
        return BCM_E_PARAM;
    }

    rv = _bcm_hr3_egr_header_encap_data_add(unit,
            encap_info, count, &base_eh_index);
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            index_array[0] = base_eh_index;
            rv = _bcm_hr3_switch_encap_idx2id
                     (unit, _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER, 1,
                      index_array, encap_id);
            BCM_IF_ERROR_RETURN(rv);
            return BCM_E_EXISTS;
        } else {
            return rv;
        }
    }

    for (i = count - 1; i >= 0; i--) {
        eh_index = base_eh_index + i;
        /* Check if created encap entry is existed */
        if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
            /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
            rv = _bcm_egr_header_encap_data_entry_ref_count_get(unit,
                      ((uint32)eh_index << 2), &ref_count);
        } else {
            rv = _bcm_egr_header_encap_data_entry_ref_count_get(unit,
                      (uint32)eh_index, &ref_count);
        }
        BCM_IF_ERROR_RETURN(rv);

        if (ref_count >= 2) {
            /* Delete previous added egr_header_encap_data entry */
            rv = _bcm_hr3_egr_header_encap_data_delete(unit, eh_index);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            return BCM_E_EXISTS;
        }

        index_array[0] = eh_index;
        rv = _bcm_hr3_switch_encap_idx2id(unit,
                  _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER, 1, index_array, encap_id);
        BCM_IF_ERROR_RETURN(rv);

        HR3_ENCAP_INFO(unit)->custom_header_encap[eh_index] = *encap_id;
    }

    return rv;
}

STATIC int
_bcm_hr3_custom_header_switch_encap_set(int unit,
    bcm_if_t encap_id, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    int eh_index, index_array[1];

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) !=
        _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
        return BCM_E_PARAM;
    }

    rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, 1, index_array);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    eh_index = index_array[0];

    rv = _bcm_hr3_egr_header_encap_data_update(unit,
        encap_id, eh_index, encap_info);
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

STATIC int
_bcm_hr3_custom_header_switch_encap_get(int unit,
    bcm_if_t encap_id, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_NONE;
    int eh_index, index_array[1];

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    /* Parameter checking */
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) !=
        _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
        return BCM_E_PARAM;
    }

    rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, 1, index_array);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    eh_index = index_array[0];

    rv = _bcm_hr3_egr_header_encap_data_get(unit,
        encap_id, eh_index, encap_info);
    BCM_IF_ERROR_RETURN(rv);

    encap_info->encap_service = BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER;

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      _bcm_hr3_switch_encap_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_hr3_switch_encap_free_resources(int unit)
{
    _bcm_hr3_switch_encap_bookkeeping_t *sw_encap_info = HR3_ENCAP_INFO(unit);

    if (_hr3_encap_mutex[unit]) {
        sal_mutex_destroy(_hr3_encap_mutex[unit]);
        _hr3_encap_mutex[unit] = NULL;
    }
#ifdef INCLUDE_L3
    if (sw_encap_info->intf_bitmap) {
        sal_free(sw_encap_info->intf_bitmap);
        sw_encap_info->intf_bitmap = NULL;
    }

    if (sw_encap_info->miml_encap) {
        sal_free(sw_encap_info->miml_encap);
        sw_encap_info->miml_encap = NULL;
    }
#endif /* INCLUDE_L3 */
    if (sw_encap_info->custom_header_encap) {
        sal_free(sw_encap_info->custom_header_encap);
        sw_encap_info->custom_header_encap = NULL;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_hr3_switch_encap_wb_scache_size_get
 * Purpose:
 *      Helper utility to determine scache details.
 * Parameters:
 *      unit        : (IN) Device Unit Number
 *      scache_len  : (OUT) Total required scache length
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_hr3_switch_encap_wb_scache_size_get(int unit,
    int *req_scache_size)
{
   int alloc_size = 0;
#ifdef INCLUDE_L3
    int num_miml = 0;
#endif /* INCLUDE_L3 */
    int num_custom_header = 0;

#ifdef INCLUDE_L3
    /* Depend on IFP */
    num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);
    alloc_size += (num_miml * sizeof(uint32));
#endif /* INCLUDE_L3 */
    num_custom_header = soc_mem_index_count(unit, EGR_HEADER_ENCAP_DATAm);
    alloc_size += (num_custom_header * sizeof(uint32));

    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}

STATIC int
_bcm_hr3_switch_encap_associated_data_recover(int unit,
    bcm_if_t encap_id)
{
    int rv = BCM_E_NONE;
    int index_array[2];
    int eh_index;
#ifdef INCLUDE_L3
    int nh_index, macda_idx, intf_num;
    uint32 nh_flags;
    egr_l3_next_hop_entry_t egr_nh_entry;
    bcm_l3_egress_t nh_info;
#endif /* INCLUDE_L3 */

    rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, 2, index_array);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
         _BCM_SWITCH_ENCAP_TYPE_MIML) {
#ifdef INCLUDE_L3
        nh_index = index_array[0];
        eh_index = index_array[1];

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ANY, nh_index, &egr_nh_entry));

        bcm_l3_egress_t_init(&nh_info);

        nh_flags = _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WRITE_DISABLE |
            _BCM_L3_SHR_WITH_ID;
        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index);
        BCM_IF_ERROR_RETURN(rv);

        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry, ENTRY_TYPEf) == 0x3) {
            /* MIML view - recover MACDA profile and interface reference counts */
            macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh_entry, MIML__MAC_DA_PROFILE_INDEXf);
            _bcm_common_profile_mem_ref_cnt_update(unit,
                EGR_MAC_DA_PROFILEm, macda_idx, 1);
            intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh_entry, MIML__INTF_NUMf);

            HR3_MIML_INTF_USED_SET(unit, intf_num);
            BCM_L3_INTF_USED_SET(unit, intf_num);

            if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
                /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
                _bcm_common_profile_mem_ref_cnt_update(unit,
                    EGR_HEADER_ENCAP_DATAm, (eh_index << 2), 1);
            } else {
                _bcm_common_profile_mem_ref_cnt_update(unit,
                    EGR_HEADER_ENCAP_DATAm, eh_index, 1);
            }
        }
#else
        rv = BCM_E_UNAVAIL;
#endif /* INCLUDE_L3 */
    } else if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
        _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
        eh_index = index_array[0];

        if (soc_feature(unit, soc_feature_hr3_switch_encap_index_shift2_war)) {
            /* WAR for A0: The index of EGR_HEADER_ENCAP_DATA is shifted << 2 bits. */
            _bcm_common_profile_mem_ref_cnt_update(unit,
                EGR_HEADER_ENCAP_DATAm, (eh_index << 2), 1);
        } else {
            _bcm_common_profile_mem_ref_cnt_update(unit,
                EGR_HEADER_ENCAP_DATAm, eh_index, 1);
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_hr3_switch_encap_reinit
 * Purpose:
 *      Warm boot recovery for the Switch Encap software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_hr3_switch_encap_reinit(int unit, uint8 **scache_ptr)
{
    uint32 *u32_scache_p;
    uint32 encap_id;
    int idx, num_custom_header = 0;
#ifdef INCLUDE_L3
    int num_miml = 0;
#endif /* INCLUDE_L3 */

#ifdef INCLUDE_L3
    num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);  /* Depend on IFP */
#endif /* INCLUDE_L3 */
    num_custom_header = soc_mem_index_count(unit, EGR_HEADER_ENCAP_DATAm);

    u32_scache_p = (uint32 *)(*scache_ptr);

    /* recover from scache into book-keeping structs */
#ifdef INCLUDE_L3
    for (idx = 0; idx < num_miml; idx++) {
        encap_id = *u32_scache_p;
        u32_scache_p++;
        if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
            _BCM_SWITCH_ENCAP_TYPE_MIML) {
            HR3_ENCAP_INFO(unit)->miml_encap[idx] = encap_id;

            BCM_IF_ERROR_RETURN(
                _bcm_hr3_switch_encap_associated_data_recover(unit, encap_id));
        }
    }
#endif /* INCLUDE_L3 */
    for (idx = 0; idx < num_custom_header; idx++) {
        encap_id = *u32_scache_p;
        u32_scache_p++;
        if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
            _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
            HR3_ENCAP_INFO(unit)->custom_header_encap[idx] = encap_id;

            BCM_IF_ERROR_RETURN(
                _bcm_hr3_switch_encap_associated_data_recover(unit, encap_id));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hr3_switch_encap_sync
 * Purpose:
 *      This routine extracts the state that needs to be stored from the
 *      book-keeping structs and stores it in the allocated scache location
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_hr3_switch_encap_sync(int unit, uint8 **scache_ptr)
{
    uint32 *u32_scache_p;
    uint32 encap_id;
    int idx, num_custom_header = 0;
#ifdef INCLUDE_L3
    int num_miml = 0;
#endif /* INCLUDE_L3 */

#ifdef INCLUDE_L3
    num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);  /* Depend on IFP */
#endif /* INCLUDE_L3 */
    num_custom_header = soc_mem_index_count(unit, EGR_HEADER_ENCAP_DATAm);

    u32_scache_p = (uint32 *)(*scache_ptr);

    /* now store the state into the compressed format */
#ifdef INCLUDE_L3
    for (idx = 0; idx < num_miml; idx++) {
        if ((HR3_ENCAP_INFO(unit)->miml_encap[idx] >>
            _BCM_SWITCH_ENCAP_SHIFT) == _BCM_SWITCH_ENCAP_TYPE_MIML) {
            encap_id = HR3_ENCAP_INFO(unit)->miml_encap[idx];
        } else {
            encap_id = 0x0;
        }
        *u32_scache_p = encap_id;
        u32_scache_p++;
    }
#endif /* INCLUDE_L3 */
    for (idx = 0; idx < num_custom_header; idx++) {
        if ((HR3_ENCAP_INFO(unit)->custom_header_encap[idx] >>
            _BCM_SWITCH_ENCAP_SHIFT) == _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
            encap_id = HR3_ENCAP_INFO(unit)->custom_header_encap[idx];
        } else {
            encap_id = 0x0;
        }
        *u32_scache_p = encap_id;
        u32_scache_p++;
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_hr3_switch_encap_detach
 * Purpose:
 *      Detach the Switch Encap software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hr3_switch_encap_detach(int unit)
{
    int rv = BCM_E_NONE;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    if (_bcm_hr3_encap_initialized[unit] == FALSE) {
        return BCM_E_NONE;
    }

    _bcm_hr3_switch_encap_free_resources(unit);
    _bcm_hr3_encap_initialized[unit] = FALSE;
    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_init
 * Description:
 *      Initialize switch encap.
 * Parameters:
 *      unit        - Device unit number
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_hr3_switch_encap_init(int unit)
{
    int rv = BCM_E_NONE;
#ifdef INCLUDE_L3
    int num_intf = 0, num_miml = 0;
#endif /* INCLUDE_L3 */
    int num_custom_header = 0;
    _bcm_hr3_switch_encap_bookkeeping_t *sw_encap_info = HR3_ENCAP_INFO(unit);

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");

    if (!(soc_feature(unit, soc_feature_miml) ||
           soc_feature(unit, soc_feature_custom_header))) {
        return BCM_E_UNAVAIL;
    }

#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_miml) && !HR3_L3_INFO(unit)->l3_initialized) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 module must be initialized first\n")));
        return BCM_E_NONE;
    }
#endif /* INCLUDE_L3 */

    if (_bcm_hr3_encap_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_hr3_switch_encap_detach(unit));
    }
#ifdef INCLUDE_L3
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);  /* Depend on IFP */
#endif /* INCLUDE_L3 */
    num_custom_header = soc_mem_index_count(unit, EGR_HEADER_ENCAP_DATAm);

    sal_memset(sw_encap_info, 0, sizeof(_bcm_hr3_switch_encap_bookkeeping_t));

#ifdef INCLUDE_L3
    /* Allocate MiML encap L3 intf usage bitmap */
    if (sw_encap_info->intf_bitmap == NULL) {
        sw_encap_info->intf_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_intf), "intf_bitmap");
        if (sw_encap_info->intf_bitmap == NULL) {
            _bcm_hr3_switch_encap_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(sw_encap_info->intf_bitmap, 0, SHR_BITALLOCSIZE(num_intf));

    /* Allocate MiML encap id */
    if (sw_encap_info->miml_encap == NULL) {
        sw_encap_info->miml_encap =
            sal_alloc((sizeof(uint32) * num_miml), "miml_encap");
        if (sw_encap_info->miml_encap == NULL) {
            _bcm_hr3_switch_encap_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(sw_encap_info->miml_encap, 0, (sizeof(uint32) * num_miml));
#endif /* INCLUDE_L3 */

    /* Allocate Custom Header encap id */
    if (sw_encap_info->custom_header_encap == NULL) {
        sw_encap_info->custom_header_encap =
            sal_alloc((sizeof(uint32) * num_custom_header), "custom_header_encap");
        if (sw_encap_info->custom_header_encap == NULL) {
            _bcm_hr3_switch_encap_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(sw_encap_info->custom_header_encap, 0,
        (sizeof(uint32) * num_custom_header));

    if (_hr3_encap_mutex[unit] == NULL) {
        _hr3_encap_mutex[unit] = sal_mutex_create("sw_encap mutex");
        if (_hr3_encap_mutex[unit] == NULL) {
            _bcm_hr3_switch_encap_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    _bcm_hr3_encap_initialized[unit] = TRUE;
    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   _bcm_hr3_switch_encap_create
 * Purpose:
 *   Create N continuous encapsulation objects, base_encap_id will be returned
 *   after the N (base_encap_id ~ base_encap_id + count - 1) encapsulation
 *   objects are created.
 * Parameters:
 *   unit - (IN) Unit number.
 *   encap_info - (IN) Encapsulation info data structure.
 *   count - (IN) Number of encapsulation objects to create
 *   base_encap_id - (OUT) Encapsulation object id.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
int
_bcm_hr3_switch_encap_create(int unit,
    bcm_switch_encap_info_t *encap_info,
    uint32 count,
    bcm_if_t *base_encap_id)
{
    int rv = BCM_E_UNAVAIL;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    HR3_ENCAP_INIT(unit);

    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }
    if (base_encap_id == NULL) {
        return BCM_E_PARAM;
    }

    switch (encap_info->encap_service) {
        case BCM_SWITCH_ENCAP_SERVICE_MIML:
            if (!soc_feature(unit, soc_feature_miml)) {
                return BCM_E_UNAVAIL;
            }
            if (count != 1) {
                return BCM_E_PARAM;
            }
#ifdef INCLUDE_L3
            HR3_ENCAP_LOCK(unit);
            rv = _bcm_hr3_miml_switch_encap_create(unit,
                      encap_info, base_encap_id);
            HR3_ENCAP_UNLOCK(unit);
#endif /* INCLUDE_L3 */
            break;
        case BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER:
            if (!soc_feature(unit, soc_feature_custom_header)) {
                return BCM_E_UNAVAIL;
            }
            HR3_ENCAP_LOCK(unit);
            rv = _bcm_hr3_custom_header_switch_encap_create(unit,
                       encap_info, count, base_encap_id);
            HR3_ENCAP_UNLOCK(unit);
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_create
 * Purpose:
 *      Create an encapuslation object, encap_id will be returned after the
 *      encapsulation object created. The encap_id will be used by FP.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_info - (IN) Encapsulation info data structure.
 *      encap_id - (OUT) Encapsulation object id.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_hr3_switch_encap_create(int unit,
    bcm_switch_encap_info_t *encap_info, bcm_if_t *encap_id)
{
    int rv;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

    /* create 1 encap_id */
    rv = _bcm_hr3_switch_encap_create(unit, encap_info, 1, encap_id);
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_destroy
 * Purpose:
 *      Destroy the encap object with the given encap_id.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) Encapsulation object id.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_hr3_switch_encap_destroy(int unit, bcm_if_t encap_id)
{
    int rv = BCM_E_UNAVAIL;
    int idx, index_count, index_array[2];
#ifdef INCLUDE_L3
    int num_miml;
#endif /* INCLUDE_L3 */

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    HR3_ENCAP_INIT(unit);

    switch (encap_id >> _BCM_SWITCH_ENCAP_SHIFT) {
        case _BCM_SWITCH_ENCAP_TYPE_MIML:
            if (!soc_feature(unit, soc_feature_miml)) {
                return BCM_E_UNAVAIL;
            }
#ifdef INCLUDE_L3
            HR3_ENCAP_LOCK(unit);
            index_count = 2;
            rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, index_count, index_array);
            if (BCM_FAILURE(rv)) {
                HR3_ENCAP_UNLOCK(unit);
                return rv;
            }

            rv = _bcm_hr3_miml_switch_encap_delete(unit,
                      index_array[0], index_array[1]);
            if (BCM_FAILURE(rv)) {
                HR3_ENCAP_UNLOCK(unit);
                return rv;
            }

            num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);  /* Depend on IFP */
            for (idx = 0; idx < num_miml; idx++) {
                if (HR3_ENCAP_INFO(unit)->miml_encap[idx] == encap_id) {
                    HR3_ENCAP_INFO(unit)->miml_encap[idx] = 0x0;
                }
            }
            HR3_ENCAP_UNLOCK(unit);
#endif /* INCLUDE_L3 */
            break;
        case _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER:
            if (!soc_feature(unit, soc_feature_custom_header)) {
                return BCM_E_UNAVAIL;
            }

            HR3_ENCAP_LOCK(unit);
            index_count = 1;
            rv = _bcm_hr3_switch_encap_id2idx(unit, encap_id, index_count, index_array);
            if (BCM_FAILURE(rv)) {
                HR3_ENCAP_UNLOCK(unit);
                return rv;
            }

            idx = index_count - 1;
            rv = _bcm_hr3_egr_header_encap_data_delete(unit, index_array[idx]);
            if (BCM_FAILURE(rv)) {
                HR3_ENCAP_UNLOCK(unit);
                return rv;
            }

            HR3_ENCAP_INFO(unit)->custom_header_encap[index_array[idx]] = 0x0;

            HR3_ENCAP_UNLOCK(unit);
            break;
        default:
            return BCM_E_NOT_FOUND;
    }

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_destroy_all
 * Purpose:
 *      Destroy all encap objects.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_hr3_switch_encap_destroy_all(int unit)
{
    int rv = BCM_E_NONE;
    int idx, num_custom_header = 0;
#ifdef INCLUDE_L3
    int num_miml = 0;
#endif /* INCLUDE_L3 */
    bcm_if_t encap_id;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    HR3_ENCAP_INIT(unit);

#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_miml)) {
        /* Depend on IFP */
        num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);
    }
#endif /* INCLUDE_L3 */
    if (soc_feature(unit, soc_feature_custom_header)) {
        num_custom_header = soc_mem_index_count(unit, EGR_HEADER_ENCAP_DATAm);
    }

#ifdef INCLUDE_L3
    /* Destroy all created encap id for MiML */
    for (idx = 0; idx < num_miml; idx++) {
        encap_id = HR3_ENCAP_INFO(unit)->miml_encap[idx];
        if (encap_id != 0x0) {
            rv = bcm_hr3_switch_encap_destroy(unit, encap_id);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* INCLUDE_L3 */

    /* Destroy all created encap id for Custom Header */
    for (idx = 0; idx < num_custom_header; idx++) {
        encap_id = HR3_ENCAP_INFO(unit)->custom_header_encap[idx];
        if (encap_id != 0x0) {
            rv = bcm_hr3_switch_encap_destroy(unit, encap_id);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_set
 * Purpose:
 *     Modify the configuration of the encap object with the given encap_id.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) Encapsulation object id.
 *      encap_info - (IN) Encapsulation info data structure.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_hr3_switch_encap_set(int unit,
    bcm_if_t encap_id, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_UNAVAIL;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    HR3_ENCAP_INIT(unit);

    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    switch (encap_id >> _BCM_SWITCH_ENCAP_SHIFT) {
        case _BCM_SWITCH_ENCAP_TYPE_MIML:
            if (!soc_feature(unit, soc_feature_miml)) {
                return BCM_E_UNAVAIL;
            }
#ifdef INCLUDE_L3
            HR3_ENCAP_LOCK(unit);
            rv = _bcm_hr3_miml_switch_encap_set(unit,
                      encap_id, encap_info);
            HR3_ENCAP_UNLOCK(unit);
#endif /* INCLUDE_L3 */
            break;
        case _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER:
            if (!soc_feature(unit, soc_feature_custom_header)) {
                return BCM_E_UNAVAIL;
            }
            HR3_ENCAP_LOCK(unit);
            rv = _bcm_hr3_custom_header_switch_encap_set(unit,
                       encap_id, encap_info);
            HR3_ENCAP_UNLOCK(unit);
            break;
        default:
            return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_get
 * Purpose:
 *     Get the configuration of the encap object with the given encap_id.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) Encapsulation object id.
 *      encap_info - (OUT) Encapsulation info data structure.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_hr3_switch_encap_get(int unit,
    bcm_if_t encap_id, bcm_switch_encap_info_t *encap_info)
{
    int rv = BCM_E_UNAVAIL;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    HR3_ENCAP_INIT(unit);

    if (encap_info == NULL) {
        return BCM_E_PARAM;
    }

    bcm_switch_encap_info_t_init(encap_info);

    switch (encap_id >> _BCM_SWITCH_ENCAP_SHIFT) {
        case _BCM_SWITCH_ENCAP_TYPE_MIML:
            if (!soc_feature(unit, soc_feature_miml)) {
                return BCM_E_UNAVAIL;
            }
#ifdef INCLUDE_L3
            HR3_ENCAP_LOCK(unit);
            rv = _bcm_hr3_miml_switch_encap_get(unit,
                      encap_id, encap_info);
            HR3_ENCAP_UNLOCK(unit);
#endif /* INCLUDE_L3 */
            break;
        case _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER:
            if (!soc_feature(unit, soc_feature_custom_header)) {
                return BCM_E_UNAVAIL;
            }
            HR3_ENCAP_LOCK(unit);
            rv = _bcm_hr3_custom_header_switch_encap_get(unit,
                       encap_id, encap_info);
            HR3_ENCAP_UNLOCK(unit);
            break;
        default:
            return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(rv);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *      bcm_hr3_switch_encap_traverse
 * Purpose:
 *     To traverse all created encapsulation entries and call provided callback function
 *     with matched entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb_fn - (IN) User specified callback function.
 *      user_data - (IN) User specified cookie.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_hr3_switch_encap_traverse(int unit,
    bcm_switch_encap_traverse_cb cb_fn, void *user_data)
{
    int rv = BCM_E_NONE;
    int idx, num_custom_header = 0;
#ifdef INCLUDE_L3
    int num_miml = 0;
#endif /* INCLUDE_L3 */
    bcm_if_t encap_id;
    bcm_switch_encap_info_t encap_info;

    HR3_ENCAP_FUNCTION_TRACE(unit, "IN");
    HR3_ENCAP_INIT(unit);

#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_miml)) {
        /* Depend on IFP */
        num_miml = soc_mem_index_count(unit, FP_POLICY_TABLEm);
    }
#endif /* INCLUDE_L3 */
    if (soc_feature(unit, soc_feature_custom_header)) {
        num_custom_header = soc_mem_index_count(unit, EGR_HEADER_ENCAP_DATAm);
    }

    bcm_switch_encap_info_t_init(&encap_info);

    HR3_ENCAP_LOCK(unit);
#ifdef INCLUDE_L3
    /* Traverse all created encap id for MiML */
    for (idx = 0; idx < num_miml; idx++) {
        encap_id = HR3_ENCAP_INFO(unit)->miml_encap[idx];
        if (encap_id != 0x0) {
            /* Get the configuration of the encap info */
            rv = bcm_hr3_switch_encap_get(unit, encap_id, &encap_info);
            if (BCM_FAILURE(rv)) {
                HR3_ENCAP_UNLOCK(unit);
                return rv;
            }

            /* Call application call-back */
            rv = cb_fn(unit, encap_id, &encap_info, user_data);
            if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
                if (SOC_CB_ABORT_ON_ERR(unit)) {
                    HR3_ENCAP_UNLOCK(unit);
                    return rv;
                }
#endif
            }
        }
    }
#endif /* INCLUDE_L3 */

    /* Traverse all created encap id for Custom Header */
    for (idx = 0; idx < num_custom_header; idx++) {
        encap_id = HR3_ENCAP_INFO(unit)->custom_header_encap[idx];
        if (encap_id != 0x0) {
            /* Get the configuration of the encap info */
            rv = bcm_hr3_switch_encap_get(unit, encap_id, &encap_info);
            if (BCM_FAILURE(rv)) {
                HR3_ENCAP_UNLOCK(unit);
                return rv;
            }
            /* Call application call-back */
            rv = cb_fn(unit, encap_id, &encap_info, user_data);
            if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
                if (SOC_CB_ABORT_ON_ERR(unit)) {
                    HR3_ENCAP_UNLOCK(unit);
                    return rv;
                }
#endif
            }
        }
    }

    HR3_ENCAP_UNLOCK(unit);

    HR3_ENCAP_FUNCTION_TRACE(unit, "OUT");
    return rv;
}
#endif /* BCM_HURRICANE3_SUPPORT */

