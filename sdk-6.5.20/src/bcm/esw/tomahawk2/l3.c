/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Tomahawk2 L3 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(INCLUDE_L3) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT))
#include <soc/drv.h>
#include <soc/tomahawk2.h>
#include <soc/profile_mem.h>

#include <bcm/vlan.h>
#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/tomahawk2.h>
#include <bcm_int/esw/tomahawk3.h>
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
#include <bcm_int/esw/subport.h>
#endif /*BCM_CHANNELIZED_SWITCHING_SUPPORT */

_th2_ecmp_dlb_bookkeeping_t *_th2_ecmp_dlb_bk[BCM_MAX_NUM_UNITS];

/* per ecmp group enhanced hashing enabled info  */
#ifdef BCM_TRIDENT3_SUPPORT
extern uint8  ecmp_grp_enhanced_hashing[BCM_MAX_NUM_UNITS][4096];
#endif
#define ECMP_DLB_INFO(_unit_) (_th2_ecmp_dlb_bk[_unit_])
#define ECMP_DLB_PORT_INFO(_unit_, _idx_) \
           (_th2_ecmp_dlb_bk[_unit_]->ecmp_dlb_port_info[_idx_])
#define ECMP_DLB_PORT_LIST(_unit_, _idx_) \
            ECMP_DLB_PORT_INFO(unit, _idx_).port_membership_list

#define _BCM_ECMP_DLB_ID_USED_GET(_u_, _idx_) \
    SHR_BITGET(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_used_bitmap, _idx_)
#define _BCM_ECMP_DLB_ID_USED_SET(_u_, _idx_) \
    SHR_BITSET(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_used_bitmap, _idx_)
#define _BCM_ECMP_DLB_ID_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_used_bitmap, _idx_)

#define _BCM_ECMP_DLB_ID_DGM_GET(_u_, _idx_) \
    SHR_BITGET(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_dgm_bitmap, _idx_)
#define _BCM_ECMP_DLB_ID_DGM_SET(_u_, _idx_) \
    SHR_BITSET(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_dgm_bitmap, _idx_)
#define _BCM_ECMP_DLB_ID_DGM_CLR(_u_, _idx_) \
    SHR_BITCLR(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_dgm_bitmap, _idx_)


#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_GET(_u_, _idx_) \
    SHR_BITGET(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET(_u_, _idx_) \
    SHR_BITSET(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(_u_, _idx_, _count_) \
    SHR_BITSET_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(_u_, _idx_, _count_) \
    SHR_BITCLR_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_TEST_RANGE(_u_, _idx_, _count_, _result_) \
    SHR_BITTEST_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_, _result_)

#define _BCM_ECMP_DLB_CELL_BYTES 208
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT      8
#define _BCM_ECMP_DLB_DEFAULT_SCALING_FACTOR        10
#define _BCM_ECMP_DLB_DEFAULT_PORT_LOAD_WEIGHT      100
#define _BCM_ECMP_DLB_DEFAULT_QUEUE_SIZE_WEIGHT     0



/*----- ECMP DLB FUNCS ----- */


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_quality_assign
 * Purpose:
 *      Assign quality based on loading, total queue size and physical queue size.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      tx_load_percent - (IN) Percent weight of loading in determing quality.
 *      qsize_percent   - (IN) Percent weight of total queue size in determing
 *                             quality.
 *                             The remaining percentage is the weight of physical
 *                             queue size.
 *      entry_arr       - (IN) Array of 512 quality map table entries.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_quality_assign(int unit, int tx_load_percent,
                                 int qsize_percent, uint32 *entry_arr)
{
    int quantized_tx_load;
    int quantized_qsize;
    int quantized_physical_qsize;
    int quality;
    int entry_index;
    int physical_qsize_percent = 100 - tx_load_percent - qsize_percent;

    if (entry_arr == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        for (quantized_tx_load = 0; quantized_tx_load < 8; quantized_tx_load++) {
            for (quantized_qsize = 0; quantized_qsize < 8; quantized_qsize++) {
                quality = (quantized_tx_load * tx_load_percent +
                            quantized_qsize * qsize_percent) / 100;
                entry_index = (quantized_tx_load << 6) +
                                (quantized_qsize << 3);
                soc_DLB_ECMP_PORT_QUALITY_MAPPINGm_field32_set(unit,
                        &entry_arr[entry_index], ASSIGNED_QUALITYf, quality);
            }
        }
    } else
#endif
    {
        for (quantized_tx_load = 0; quantized_tx_load < 8; quantized_tx_load++) {
            for (quantized_physical_qsize = 0; quantized_physical_qsize < 8;
                 quantized_physical_qsize++) {
                for (quantized_qsize = 0; quantized_qsize < 8; quantized_qsize++) {
                    quality = (quantized_tx_load * tx_load_percent +
                               quantized_qsize * qsize_percent +
                               quantized_physical_qsize * physical_qsize_percent) /
                               100;
                    entry_index = (quantized_tx_load << 6) +
                                  (quantized_physical_qsize << 3) +
                                  quantized_qsize;
                    soc_DLB_ECMP_PORT_QUALITY_MAPPINGm_field32_set(unit,
                            &entry_arr[entry_index], ASSIGNED_QUALITYf, quality);
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_port_quality_map_set
 * Purpose:
 *      Set per-member ECMP DLB quality mapping table.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      port            - (IN) Port, not Gport.
 *      tx_load_percent - (IN) Percent weight of loading in determing quality.
 *      qsize_percent   - (IN) Percent weight of total queue size in determing
 *                             quality.
 *                             The remaining percentage is the weight of physical
 *                             queue size.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_port_quality_map_set(int unit, int port,
                                       int tx_load_percent, int qsize_percent)
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_bytes;
    int entries_per_profile;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    uint32 old_base_index;
    dlb_ecmp_quantize_control_entry_t quantize_control_entry;
    int num_quality_map_profiles;

    profile = ECMP_DLB_INFO(unit)->ecmp_dlb_quality_map_profile;
    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    entries_per_profile =
        soc_mem_index_count(unit, DLB_ECMP_PORT_QUALITY_MAPPINGm) /
        num_quality_map_profiles;

    entry_bytes = sizeof(dlb_ecmp_quality_mapping_entry_t);
    alloc_size = entries_per_profile * entry_bytes;
    entry_arr = sal_alloc(alloc_size, "ECMP DLB Quality Map entries");
    if (entry_arr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_arr, 0, alloc_size);

    /* Assign quality in the entry array */
    rv = _bcm_th2_ecmp_dlb_quality_assign(unit, tx_load_percent,
                                          qsize_percent, entry_arr);
    if (BCM_FAILURE(rv)) {
        sal_free(entry_arr);
        return rv;
    }

    mem = DLB_ECMP_PORT_QUALITY_MAPPINGm;
    soc_mem_lock(unit, mem);

    /* Add profile */
    entries = entry_arr;
    rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
                             &base_index);
    sal_free(entry_arr);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Update member profile pointer */
    rv = READ_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY, port,
            &quantize_control_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    old_base_index = entries_per_profile * soc_mem_field32_get(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, &quantize_control_entry,
            PORT_QUALITY_MAPPING_PROFILE_PTRf);
    soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit, &quantize_control_entry,
            PORT_QUALITY_MAPPING_PROFILE_PTRf, base_index / entries_per_profile);
    rv = WRITE_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ALL, port,
            &quantize_control_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Delete old profile */
    rv = soc_profile_mem_delete(unit, profile, old_base_index);
    soc_mem_unlock(unit, mem);

    ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[base_index/entries_per_profile] =
        tx_load_percent;
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_weight[base_index/entries_per_profile] =
        qsize_percent;

    return rv;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_scaling_factor_encode
 * Purpose:
 *      Check valid scaling factors, and encode them.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      scaling_factor  - (IN) Threshold scaling factor.
 * Returns:
 *      1: check success
 *      0: check fail
 */
STATIC int
_bcm_th2_ecmp_dlb_scaling_factor_encode(int unit, int scaling_factor,
                                        int *scaling_factor_hw)
{
    int bcm_factors[] = {10, 25, 40, 50, 75, 100};
    int hw_factors[]  = { 0,  1,  2,  3,  4,   5};
    int i;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        return _bcm_th3_l3_ecmp_dlb_scaling_factor_encode(unit, scaling_factor,
                                                       scaling_factor_hw);
    }
#endif

    for (i = 0; i < COUNTOF(bcm_factors); i++ ) {
        if (scaling_factor == bcm_factors[i]) {
            break;
        }
    }

    if (i < COUNTOF(hw_factors)) {
        if (scaling_factor_hw) {
            *scaling_factor_hw = hw_factors[i];
        }
        return 1;
    }
    return 0;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_scaling_factor_decode
 * Purpose:
 *      Decode scaling factors.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      scaling_factor  - (IN) Threshold scaling factor.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_scaling_factor_decode(int unit, int scaling_factor_hw,
                                        int *scaling_factor)
{
    int bcm_factors[] = {10, 25, 40, 50, 75, 100};
    int hw_factors[]  = { 0,  1,  2,  3,  4,   5};
    int i;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        return _bcm_th3_l3_ecmp_dlb_scaling_factor_decode(unit, scaling_factor_hw,
                                                       scaling_factor);
    }
#endif
    for (i = 0; i < COUNTOF(hw_factors); i++ ) {
        if (scaling_factor_hw == hw_factors[i]) {
            break;
        }
    }

    if (i == COUNTOF(hw_factors)) {
        return BCM_E_INTERNAL;
    }

    *scaling_factor = bcm_factors[i];
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_port_attr_set
 * Purpose:
 *      Set ECMP dynamic load balancing attributes for a member.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      port            - (IN) Port, not Gport.
 *      nh_index        - (IN) Next hop index.
 *      scaling_factor  - (IN) Threshold scaling factor.
 *      load_weight     - (IN) Weighting of load in determining member quality.
 *      qsize_weight    - (IN) Weighting of total qsize in determining member quality.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_port_attr_set(int unit, bcm_port_t port,
        int scaling_factor, int load_weight, int qsize_weight)
{
    dlb_ecmp_quantize_control_entry_t quantize_control_entry;
    int scaling_factor_hw;
    soc_field_t qsf_field = XPE_PORT_QSIZE_THRESHOLD_SCALING_FACTORf;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        qsf_field = ITM_PORT_QSIZE_THRESHOLD_SCALING_FACTORf;
    }
#endif

    /* Set load weight, qsize weight */
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_port_quality_map_set(unit,
                                                               port,
                                                               load_weight,
                                                               qsize_weight));

    if (!_bcm_th2_ecmp_dlb_scaling_factor_encode(unit,
            scaling_factor, &scaling_factor_hw)) {
        return BCM_E_PARAM;
    }
    /* Set scaling factor */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                port, &quantize_control_entry));

    soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
            &quantize_control_entry,
            PORT_LOADING_THRESHOLD_SCALING_FACTORf,
            scaling_factor_hw);
    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
                &quantize_control_entry,
                TOTAL_PORT_QSIZE_THRESHOLD_SCALING_FACTORf,
                scaling_factor_hw);
    }
    soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
            &quantize_control_entry,
            qsf_field,
            scaling_factor_hw);
    BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ALL,
                port, &quantize_control_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_port_attr_get
 * Purpose:
 *      Get ECMP dynamic load balancing attributes for a member.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      port            - (IN) Port, not Gport.
 *      scaling_factor  - (OUT) Threshold scaling factor.
 *      load_weight     - (OUT) Weighting of load in determining member quality.
 *      qsize_weight    - (OUT) Weighting of total qsize in determining member quality.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_port_attr_get(int unit, bcm_port_t port, int *scaling_factor,
        int *load_weight, int *qsize_weight)
{
    dlb_ecmp_quantize_control_entry_t quantize_control_entry;
    int profile_ptr;
    int scaling_factor_hw;

    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                port, &quantize_control_entry));

    /* Get load weight, qsize weight*/
    profile_ptr = soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_get(unit,
            &quantize_control_entry, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    *load_weight = ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[profile_ptr];
    *qsize_weight = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_weight[profile_ptr];

    /* Get scaling factor */
    scaling_factor_hw = soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_get(unit,
            &quantize_control_entry, PORT_LOADING_THRESHOLD_SCALING_FACTORf);

    BCM_IF_ERROR_RETURN
        (_bcm_th2_ecmp_dlb_scaling_factor_decode(unit, scaling_factor_hw,
                                                 scaling_factor));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_l3_egress_dlb_attr_set
 * Purpose:
 *      Set ECMP dynamic load balancing attributes for a next hop index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 *      egr      - (IN) L3 egress info.
 *      old_egr  - (IN) Old L3 egress info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_dlb_attr_set(int unit, int nh_index,
                               bcm_l3_egress_t *egr, bcm_l3_egress_t *old_egr)
{
    int scaling_factor_valid = 1;
    int load_weight_valid = 1;
    int qsize_weight_valid = 1;
    int sum_valid;
    _th2_ecmp_dlb_port_membership_t *membership = NULL;
    int new_mod, new_port; /* not GPORT */
    int mod, port;         /* not GPORT */
    int is_local;
    int move_dlb_count;
    int search_old_qualified;
    _th2_ecmp_dlb_port_membership_t *current_ptr, *prev_ptr;
    _th2_ecmp_dlb_port_membership_t *current_next;
    _th2_ecmp_dlb_port_membership_t *first_ptr, *prev_first_ptr;
    _th2_ecmp_dlb_port_membership_t *last_ptr;
    _th2_ecmp_dlb_port_membership_t *record_ptr;



#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        bcm_gport_t subport, modport_gport;
        int rv=BCM_E_NONE;

        BCM_GPORT_MODPORT_SET(modport_gport, egr->module, egr->port);
        rv = _bcm_coe_subtag_subport_port_get_by_modport(unit,
                 modport_gport, &subport);
        if (rv == BCM_E_NONE) {
            if (BCM_GPORT_IS_SET(subport) &&
                (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport))){
                /* DLB is not supported on subports. */
                return BCM_E_NONE;
            }

        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


    if (egr->dynamic_scaling_factor ==
             BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) {
       scaling_factor_valid = 0;
    }
    if (egr->dynamic_load_weight ==
             BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID) {
        load_weight_valid = 0;
    }
    if (egr->dynamic_queue_size_weight ==
             BCM_L3_ECMP_DYNAMIC_QUEUE_SIZE_WEIGHT_INVALID) {
        qsize_weight_valid = 0;
    }
    sum_valid = scaling_factor_valid +
        load_weight_valid + qsize_weight_valid;

    if ((sum_valid != 0) && (sum_valid != 3)) {
        /* The scaling factor, load weight and qsize weight must
         * be all invalid or all valid.
         */
        return BCM_E_PARAM;
    }


    /* Verify DLB parameters */
    if (sum_valid == 3) {
        if (!_bcm_th2_ecmp_dlb_scaling_factor_encode(unit,
                egr->dynamic_scaling_factor, NULL)) {
            return BCM_E_PARAM;
        }
        if (egr->dynamic_load_weight < 0 ||
            egr->dynamic_load_weight > 100) {
            return BCM_E_PARAM;
        }
        if (egr->dynamic_queue_size_weight < 0 ||
            egr->dynamic_queue_size_weight > 100) {
            return BCM_E_PARAM;
        }
        if ((egr->dynamic_load_weight + egr->dynamic_queue_size_weight) < 0 ||
            (egr->dynamic_load_weight + egr->dynamic_queue_size_weight) > 100) {
            return BCM_E_PARAM;
        }
        /* ECMP DLB members shouldn't be LAG */
        if (egr->flags & BCM_L3_TGID) {
            return BCM_E_PARAM;
        } else {
            /* ECMP DLB members should be local */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                         egr->module, egr->port,
                                         &new_mod, &new_port));
            if (new_port < 0 ||
                new_port >= ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size) {
                return BCM_E_PARAM;
            }

            (void) _bcm_esw_modid_is_local(unit, new_mod, &is_local);
            if (!is_local) {
                return BCM_E_PARAM;
            }
        }
    }


    search_old_qualified = 0;
    if (old_egr &&
        !(old_egr->flags & BCM_L3_TGID)) {
        BCM_IF_ERROR_RETURN
        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                 old_egr->module, old_egr->port,
                                 &mod, &port));
        (void) _bcm_esw_modid_is_local(unit, mod, &is_local);
        if (port < 0 ||
            port >= ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size) {
            return BCM_E_PARAM;
        }

        search_old_qualified = is_local;
    }

    current_ptr = NULL;
    prev_ptr    = NULL;
    if (search_old_qualified) {
        current_ptr = ECMP_DLB_PORT_LIST(unit, port);
        while (current_ptr) {
            if (nh_index == current_ptr->nh_index) {
                break;
            } else {
                prev_ptr = current_ptr;
            }
            current_ptr = current_ptr->next;
        }
    }

    if (current_ptr) {
        /* This Nexthop existed with DLB attributes, which means it
         * must have DLB attributes before.
         */

        record_ptr = current_ptr;
        if (egr->dynamic_scaling_factor ==
                BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) {
            /* Now remove its DLB attributes. This node should be removed
             * from port list.
             */

            /* Check that the given next hop index is currently not a member of
             * any ECMP DLB group.
             */
            while (current_ptr) {
                if (nh_index == current_ptr->nh_index &&
                    -1 != current_ptr->dlb_id) {
                    return BCM_E_BUSY;
                }
                current_ptr = current_ptr->next;
            }

            current_ptr = record_ptr;
            while (current_ptr) {
                current_next = current_ptr->next;
                if (nh_index == current_ptr->nh_index) {
                    if (prev_ptr == NULL) {
                        ECMP_DLB_PORT_LIST(unit, port) = current_next;
                    } else {
                        prev_ptr->next = current_next;
                    }
                    sal_free(current_ptr);
                } else {
                    break;
                }
                current_ptr = current_next;
            }
            /* must do and only do once */
            ECMP_DLB_PORT_INFO(unit, port).nh_count--;

            /* Reset the DLB attributes */
            if (ECMP_DLB_PORT_INFO(unit, port).nh_count == 0) {
                /* Verify nh_count consistent with port_list and dlb_count */
                if (ECMP_DLB_PORT_LIST(unit, port) != NULL ||
                    ECMP_DLB_PORT_INFO(unit, port).dlb_count != 0) {
                    return BCM_E_INTERNAL;
                }

                BCM_IF_ERROR_RETURN
                (_bcm_th2_ecmp_dlb_port_attr_set(unit, port,
                    _BCM_ECMP_DLB_DEFAULT_SCALING_FACTOR,
                    _BCM_ECMP_DLB_DEFAULT_PORT_LOAD_WEIGHT,
                    _BCM_ECMP_DLB_DEFAULT_QUEUE_SIZE_WEIGHT));
            }
        } else {
            /* Set nexthop with new DLB attributes.
             * There're 2 cases:
             * 1, the new port equal to old port
             * 2, the new port not equal to old port
 */
            if (port == new_port) {
                /* 1, the new port is equal to old port. */
                if (ECMP_DLB_PORT_INFO(unit, port).nh_count == 0) {
                    return BCM_E_INTERNAL;
                }

                if (ECMP_DLB_PORT_INFO(unit, port).nh_count == 1 ||
                    ECMP_DLB_INFO(unit)->ecmp_dlb_property_force_set) {
                    /* This nexthop is the only nexthop pointing to old port
                     * Set with new DLB attributes.
                     */
                    BCM_IF_ERROR_RETURN
                    (_bcm_th2_ecmp_dlb_port_attr_set(unit,
                                                     new_port,
                                                     egr->dynamic_scaling_factor,
                                                     egr->dynamic_load_weight,
                                                     egr->dynamic_queue_size_weight));
                } else {
                    /* This nexthop is not the only nexthop pointing to old port
                     * Reject/alert if new DLB attributes are not equal
                     * to old DLB attributes
                     */
                    if (egr->dynamic_load_weight       != old_egr->dynamic_load_weight ||
                        egr->dynamic_queue_size_weight != old_egr->dynamic_queue_size_weight ||
                        egr->dynamic_scaling_factor    != old_egr->dynamic_scaling_factor) {
                        return BCM_E_BUSY;
                    }
                }

            } else {
               /* 2, the new port is not equal to old port.
                *  move this nexthop's node or nodes to new port list
                */

                /* Check that the move is feasible */
                if (!ECMP_DLB_INFO(unit)->ecmp_dlb_property_force_set &&
                    ECMP_DLB_PORT_INFO(unit, new_port).nh_count >= 1) {
                    int dynamic_scaling_factor;
                    int dynamic_load_weight;
                    int dynamic_qsize_weight;
                    BCM_IF_ERROR_RETURN
                    (_bcm_th2_ecmp_dlb_port_attr_get(unit,
                                                     new_port,
                                                     &dynamic_scaling_factor,
                                                     &dynamic_load_weight,
                                                     &dynamic_qsize_weight));
                    if (egr->dynamic_load_weight       != dynamic_load_weight ||
                        egr->dynamic_queue_size_weight != dynamic_qsize_weight||
                        egr->dynamic_scaling_factor    != dynamic_scaling_factor){
                        return BCM_E_BUSY;
                    }
                }

                /* a, first remove this nexthop from old port list */
                first_ptr = NULL;
                last_ptr = NULL;
                move_dlb_count = 0;
                prev_ptr = ECMP_DLB_PORT_LIST(unit, port);
                prev_first_ptr = NULL;
                while (current_ptr) {
                    if (nh_index == current_ptr->nh_index) {
                        if (first_ptr == NULL) {
                            first_ptr = current_ptr;
                            prev_first_ptr = prev_ptr;
                        }
                        if (current_ptr->dlb_id != -1) {
                            move_dlb_count++;
                        }
                        last_ptr = current_ptr;
                    } else {
                        /* Given the fact nodes with same nh_index are always
                         * bundled together.
                         */
                        if (first_ptr) {
                            break;
                        }
                    }
                    prev_ptr = current_ptr;
                    current_ptr = current_ptr->next;
                }
                /* must do and only do once */
                ECMP_DLB_PORT_INFO(unit, port).nh_count--;

                if (last_ptr == NULL ||
                    first_ptr == NULL ||
                    prev_first_ptr == NULL) {
                    return BCM_E_INTERNAL;
                }
                if (prev_first_ptr == ECMP_DLB_PORT_LIST(unit, port)) {
                    ECMP_DLB_PORT_LIST(unit, port) = last_ptr->next;
                } else {
                    prev_first_ptr->next = last_ptr->next;
                }

                ECMP_DLB_PORT_INFO(unit, port).dlb_count -= move_dlb_count;

                /* Reset the DLB attributes */
                if (ECMP_DLB_PORT_INFO(unit, port).nh_count == 0) {
                    if (ECMP_DLB_PORT_LIST(unit, port) != NULL ||
                        ECMP_DLB_PORT_INFO(unit, port).dlb_count != 0) {
                        return BCM_E_INTERNAL;
                    }
                    BCM_IF_ERROR_RETURN
                        (_bcm_th2_ecmp_dlb_port_attr_set(unit, port,
                            _BCM_ECMP_DLB_DEFAULT_SCALING_FACTOR,
                            _BCM_ECMP_DLB_DEFAULT_PORT_LOAD_WEIGHT,
                            _BCM_ECMP_DLB_DEFAULT_QUEUE_SIZE_WEIGHT));
                }

                /* Set the DLB attributes */
                BCM_IF_ERROR_RETURN(
                _bcm_th2_ecmp_dlb_port_attr_set(unit,
                                                new_port,
                                                egr->dynamic_scaling_factor,
                                                egr->dynamic_load_weight,
                                                egr->dynamic_queue_size_weight));
                /*  Add into list */
                last_ptr->next = ECMP_DLB_PORT_LIST(unit, new_port);
                ECMP_DLB_PORT_LIST(unit, new_port) = first_ptr;
                ECMP_DLB_PORT_INFO(unit, new_port).nh_count++;
                ECMP_DLB_PORT_INFO(unit, new_port).dlb_count += move_dlb_count;

            }
        }
    } else {
        /* Either the Nexthop did not exist or it existed without DLB attributes. */
        if (egr->dynamic_scaling_factor ==
            BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) {
            /* still without DLB porperties */
            return BCM_E_NONE;
        } else {
            /* With DLB porperties */

            /* Check that DLB attributes setting is feasible */
            if (!ECMP_DLB_INFO(unit)->ecmp_dlb_property_force_set &&
                ECMP_DLB_PORT_INFO(unit, new_port).nh_count >= 1) {
                int dynamic_scaling_factor;
                int dynamic_load_weight;
                int dynamic_qsize_weight;
                BCM_IF_ERROR_RETURN
                (_bcm_th2_ecmp_dlb_port_attr_get(unit,
                                                 new_port,
                                                 &dynamic_scaling_factor,
                                                 &dynamic_load_weight,
                                                 &dynamic_qsize_weight));
                if (egr->dynamic_load_weight != dynamic_load_weight ||
                    egr->dynamic_queue_size_weight != dynamic_qsize_weight ||
                    egr->dynamic_scaling_factor != dynamic_scaling_factor){
                    return BCM_E_BUSY;
                }
            }

            BCM_IF_ERROR_RETURN
            (_bcm_th2_ecmp_dlb_port_attr_set(unit,
                                             new_port,
                                             egr->dynamic_scaling_factor,
                                             egr->dynamic_load_weight,
                                             egr->dynamic_queue_size_weight));

            membership = sal_alloc(sizeof(_th2_ecmp_dlb_port_membership_t),
                    "ecmp dlb nh membership");
            if (NULL == membership) {
                return BCM_E_MEMORY;
            }
            sal_memset(membership, 0, sizeof(_th2_ecmp_dlb_port_membership_t));
            membership->nh_index = nh_index;
            membership->ref_count = 0;
            membership->dlb_id    = -1;
            membership->next = ECMP_DLB_PORT_LIST(unit, new_port);
            ECMP_DLB_PORT_LIST(unit, new_port) = membership;
            ECMP_DLB_PORT_INFO(unit, new_port).nh_count++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_l3_egress_dlb_attr_destroy
 * Purpose:
 *      Destroy ECMP dynamic load balancing attributes for a next hop index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_dlb_attr_destroy(int unit, int nh_index, bcm_l3_egress_t *egr)
{
    int mod, port;
    int is_local;
    _th2_ecmp_dlb_port_membership_t *record_ptr = NULL;
    _th2_ecmp_dlb_port_membership_t *current_next, *prev_ptr = NULL;
    _th2_ecmp_dlb_port_membership_t *current_ptr = NULL;


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        bcm_gport_t subport, modport_gport;
        int rv=BCM_E_NONE;

        BCM_GPORT_MODPORT_SET(modport_gport, egr->module, egr->port);
        rv = _bcm_coe_subtag_subport_port_get_by_modport(unit,
                 modport_gport, &subport);
        if (rv == BCM_E_NONE) {
            if (BCM_GPORT_IS_SET(subport) &&
                (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport))){
                /* No LAB support on subport. */
                return BCM_E_NONE;

            }
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


    /* ECMP DLB members shouldn't be LAG */
    if (egr->flags & BCM_L3_TGID) {
        return BCM_E_NONE;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                     egr->module, egr->port,
                                     &mod, &port));
        if (port < 0 ||
            port >= ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size) {
            if (egr->flags & BCM_L3_DST_DISCARD) {
                return BCM_E_NONE;
            }
            return BCM_E_PARAM;
        }

        (void) _bcm_esw_modid_is_local(unit, mod, &is_local);
        if (!is_local) {
            return BCM_E_NONE;
        }
    }

    /* Check that the given next hop index is currently not a member of
     * any ECMP DLB group.
     */
    current_ptr = ECMP_DLB_PORT_LIST(unit, port);
    while (current_ptr) {
        if (nh_index == current_ptr->nh_index) {
            if (record_ptr == NULL) {
                record_ptr = current_ptr;
            }
            if (current_ptr->dlb_id != -1) {
                return BCM_E_BUSY;
            }
        } else {
            if (record_ptr) {
                break;
            } else {
                prev_ptr = current_ptr;
            }
        }
        current_ptr = current_ptr->next;
    }

    if (record_ptr == NULL) {
        return BCM_E_NONE;
    }

    current_ptr = record_ptr;

    while (current_ptr) {
        current_next = current_ptr->next;
        if (nh_index == current_ptr->nh_index) {
            if (prev_ptr == NULL) {
                ECMP_DLB_PORT_LIST(unit, port) = current_next;
            } else {
                prev_ptr->next = current_next;
            }
            sal_free(current_ptr);
        } else {
            break;
        }
        current_ptr = current_next;
    }
    /* must do and only do once */
    ECMP_DLB_PORT_INFO(unit, port).nh_count--;

    /* Reset the DLB attributes */
    if (ECMP_DLB_PORT_INFO(unit, port).nh_count == 0) {
        /* Verify nh_count consistent with port_list and dlb_count */
        if (ECMP_DLB_PORT_LIST(unit, port) != NULL ||
            ECMP_DLB_PORT_INFO(unit, port).dlb_count != 0) {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN
        (_bcm_th2_ecmp_dlb_port_attr_set(unit, port,
            _BCM_ECMP_DLB_DEFAULT_SCALING_FACTOR,
            _BCM_ECMP_DLB_DEFAULT_PORT_LOAD_WEIGHT,
            _BCM_ECMP_DLB_DEFAULT_QUEUE_SIZE_WEIGHT));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_l3_egress_dlb_attr_get
 * Purpose:
 *      Get ECMP dynamic load balancing attributes for a next hop index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 *      egr      - (OUT) L3 egress info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_dlb_attr_get(int unit, int nh_index, bcm_l3_egress_t *egr)
{
    _th2_ecmp_dlb_port_membership_t *current_ptr;
    int port;


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        bcm_gport_t subport, modport_gport;
        int rv=BCM_E_NONE;

        BCM_GPORT_MODPORT_SET(modport_gport, egr->module, egr->port);
        rv = _bcm_coe_subtag_subport_port_get_by_modport(unit,
                 modport_gport, &subport);
        if (rv == BCM_E_NONE) {
            if (BCM_GPORT_IS_SET(subport) &&
                (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport))){
                egr->dynamic_scaling_factor = \
                    BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID;
                egr->dynamic_load_weight = \
                    BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID;
                egr->dynamic_queue_size_weight = \
                    BCM_L3_ECMP_DYNAMIC_QUEUE_SIZE_WEIGHT_INVALID;

                return BCM_E_NONE;

            }
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    /* ECMP DLB members shouldn't be LAG */
    if (egr->flags & BCM_L3_TGID) {
        return BCM_E_NONE;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                     egr->module, egr->port,
                                     NULL, &port));
        if (port < 0 ||
            port >= ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size) {
            if (egr->flags & BCM_L3_DST_DISCARD) {
                egr->dynamic_scaling_factor = \
                          BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID;
                egr->dynamic_load_weight = \
                          BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID;
                egr->dynamic_queue_size_weight = \
                          BCM_L3_ECMP_DYNAMIC_QUEUE_SIZE_WEIGHT_INVALID;
                return BCM_E_NONE;
            }
            return BCM_E_PARAM;
        }
    }

    current_ptr = ECMP_DLB_PORT_LIST(unit, port);
    while (current_ptr) {
        if (nh_index == current_ptr->nh_index) {
            break;
        }
        current_ptr = current_ptr->next;
    }

    if (current_ptr) {
        /* Get the DLB attributes */
        BCM_IF_ERROR_RETURN
        (_bcm_th2_ecmp_dlb_port_attr_get(unit,
                                         port,
                                         &egr->dynamic_scaling_factor,
                                         &egr->dynamic_load_weight,
                                         &egr->dynamic_queue_size_weight));
    } else {
        egr->dynamic_scaling_factor = BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID;
        egr->dynamic_load_weight = BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID;
        egr->dynamic_queue_size_weight = BCM_L3_ECMP_DYNAMIC_QUEUE_SIZE_WEIGHT_INVALID;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_id_alloc
 * Purpose:
 *      Get a free ECMP DLB ID and mark it used.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      dynamic_mode - (IN) Dynamic mode
 *      dlb_id  - (OUT) Allocated DLB ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_id_alloc(int unit, bcm_l3_egress_ecmp_t *ecmp, int *dlb_id)
{
    int i;
    uint32 dynamic_mode = ecmp->dynamic_mode;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        *dlb_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp->ecmp_intf
                               - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit));
        _BCM_ECMP_DLB_ID_USED_SET(unit, *dlb_id);
        if (dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            _BCM_ECMP_DLB_ID_DGM_SET(unit, *dlb_id);
        } else {
            _BCM_ECMP_DLB_ID_DGM_CLR(unit, *dlb_id);
        }
        return BCM_E_NONE;
    }
#endif
    for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm); i++) {
        if (!_BCM_ECMP_DLB_ID_USED_GET(unit, i)) {
            _BCM_ECMP_DLB_ID_USED_SET(unit, i);
            if (dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
                _BCM_ECMP_DLB_ID_DGM_SET(unit, i);
            } else {
                _BCM_ECMP_DLB_ID_DGM_CLR(unit, i);
            }
            *dlb_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_id_free
 * Purpose:
 *      Free an ECMP DLB ID.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      dlb_id  - (IN) DLB ID to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_id_free(int unit, int dlb_id)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint64 dlb_en_bitmap;
#endif
    if (dlb_id < 0 ||
        dlb_id > soc_mem_index_max(unit, DLB_ECMP_GROUP_CONTROLm)) {
        return BCM_E_PARAM;
    }

    _BCM_ECMP_DLB_ID_USED_CLR(unit, dlb_id);
    _BCM_ECMP_DLB_ID_DGM_CLR(unit, dlb_id);

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((soc_feature(unit, soc_feature_th3_style_dlb))) {
            if (dlb_id < soc_reg_field_length(unit,
                         DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
                SOC_IF_ERROR_RETURN( \
                  READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
                COMPILER_64_BITCLR(dlb_en_bitmap, dlb_id);
                SOC_IF_ERROR_RETURN( \
                  WRITE_DLB_ID_0_TO_63_ENABLEr(unit, dlb_en_bitmap));
            } else {
                SOC_IF_ERROR_RETURN( \
                  READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap));
                COMPILER_64_BITCLR(dlb_en_bitmap, (dlb_id -
                                 (soc_reg_field_length(unit,
                                 DLB_ID_0_TO_63_ENABLEr, BITMAPf))));
                SOC_IF_ERROR_RETURN( \
                  WRITE_DLB_ID_64_TO_127_ENABLEr(unit, dlb_en_bitmap));
            }
        }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_dynamic_size_encode
 * Purpose:
 *      Encode ECMP dynamic load balancing flow set size.
 * Parameters:
 *      dynamic_size    - (IN) Number of flow sets.
 *      encoded_value   - (OUT) Encoded value.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_dynamic_size_encode(int dynamic_size,
        int *encoded_value)
{
    switch (dynamic_size) {
        case 256:
            *encoded_value = 1;
            break;
        case 512:
            *encoded_value = 2;
            break;
        case 1024:
            *encoded_value = 3;
            break;
        case 2048:
            *encoded_value = 4;
            break;
        case 4096:
            *encoded_value = 5;
            break;
        case 8192:
            *encoded_value = 6;
            break;
        case 16384:
            *encoded_value = 7;
            break;
        case 32768:
            *encoded_value = 8;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_dynamic_size_decode
 * Purpose:
 *      Decode ECMP dynamic load balancing flow set size.
 * Parameters:
 *      encoded_value   - (IN) Encoded value.
 *      dynamic_size    - (OUT) Number of flow sets.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_dynamic_size_decode(int encoded_value,
        int *dynamic_size)
{
    switch (encoded_value) {
        case 1:
            *dynamic_size = 256;
            break;
        case 2:
            *dynamic_size = 512;
            break;
        case 3:
            *dynamic_size = 1024;
            break;
        case 4:
            *dynamic_size = 2048;
            break;
        case 5:
            *dynamic_size = 4096;
            break;
        case 6:
            *dynamic_size = 8192;
            break;
        case 7:
            *dynamic_size = 16384;
            break;
        case 8:
            *dynamic_size = 32768;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_free_resource
 * Purpose:
 *      Free resources for an ECMP dynamic load balancing group.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      ecmp_group_idx  - (IN) ECMP group index.
 *      dlb_id          - (IN) DLB id.
 *                         -1: free all DLB resources,
 *                         others: will not disable DLB and free flowset.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_free_resource(int unit, int ecmp_group_idx, int dlb_id,
                                int cleanup_flowset)
{
    int rv = BCM_E_NONE;
    ecmp_dlb_control_entry_t ecmp_dlb_control_entry;
    int dlb_enable;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int block_base_ptr;
    int membership_pointer;
    int port2member_base = 0;
    int num_blocks;
    dlb_ecmp_group_membership_entry_t dlb_ecmp_group_membership_entry;
    int port_map_width;
    int alloc_size;
    SHR_BITDCL *port_map = NULL, *port_map2 = NULL;
    SHR_BITDCL *port_map_remove = NULL;
    int port_map_remove_set = 0;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int port;
    dlb_ecmp_link_control_entry_t link_control_entry;
    _th2_ecmp_dlb_port_membership_t *current_ptr, *prev_ptr, *current_next;
    int duplicate = 0;
    dlb_ecmp_group_port_to_member_entry_t *entry_arr = NULL;
    int entry_size;
    int entries_per_profile;
    int index_max;
#ifdef BCM_TRIDENT3_SUPPORT
    ecmp_count_entry_t ecmp_count_entry;
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint64 dlb_en_bitmap;
#endif
    int delete_all_flag = TRUE;

    delete_all_flag = (dlb_id == -1) ? TRUE: FALSE;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        /* Make sure the ECMP group is in DLB range */
        if ( ecmp_group_idx < BCM_TH3_L3_ECMP_DLB_START(unit)) {
            return BCM_E_NONE;
        }

        dlb_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp_group_idx);
        /* Enable bits for the first 64 groups are in
           DLB_ID_0_TO_63_ENABLE register and the next 64 are in
           DLB_ID_64_TO_127_ENABLE. */
        if (dlb_id < soc_reg_field_length(unit,
                     DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
            SOC_IF_ERROR_RETURN(READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
            if (!(COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id))) {
                return BCM_E_NONE;
            }
        } else {
            SOC_IF_ERROR_RETURN( \
                READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap));
            if (!(COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id -
                soc_reg_field_length(unit, DLB_ID_0_TO_63_ENABLEr, BITMAPf)))) {
                return BCM_E_NONE;
            }
        }
        delete_all_flag = cleanup_flowset;
    }
#endif

    if (delete_all_flag) {
        if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
            BCM_IF_ERROR_RETURN(
                READ_ECMP_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                                       ecmp_group_idx, &ecmp_dlb_control_entry));
            dlb_enable =
                soc_ECMP_DLB_CONTROLm_field32_get(unit, &ecmp_dlb_control_entry,
                                                  GROUP_ENABLEf);
            dlb_id =
                soc_ECMP_DLB_CONTROLm_field32_get(unit,
                                              &ecmp_dlb_control_entry, DLB_IDf);
            if (!dlb_enable) {
                return BCM_E_NONE;
            }
            /* Clear DLB fields in ECMP group table */
            soc_ECMP_DLB_CONTROLm_field32_set(unit, &ecmp_dlb_control_entry,
                                              GROUP_ENABLEf, 0);
            soc_ECMP_DLB_CONTROLm_field32_set(unit, &ecmp_dlb_control_entry,
                                              DLB_IDf, 0);
            BCM_IF_ERROR_RETURN(
                WRITE_ECMP_DLB_CONTROLm(unit, MEM_BLOCK_ALL,
                                    ecmp_group_idx, &ecmp_dlb_control_entry));
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            BCM_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group_idx, &ecmp_count_entry));
            soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry, LB_MODEf, 0x0);
            BCM_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group_idx, &ecmp_count_entry));
            ecmp_grp_enhanced_hashing[unit][ecmp_group_idx] = 0;
        }
#endif
    }

    /* Free flow set table resources */
    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id,
                                      &dlb_ecmp_group_control_entry));
    entry_base_ptr = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, FLOW_SET_BASEf);
    flow_set_size = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, FLOW_SET_SIZEf);
    membership_pointer = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, GROUP_MEMBERSHIP_POINTERf);
    if (!soc_feature(unit, soc_feature_th3_style_dlb)) {
        port2member_base = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &dlb_ecmp_group_control_entry, GROUP_PORT_TO_MEMBER_BASE_ADDRf);
    }

    if (delete_all_flag) {
        BCM_IF_ERROR_RETURN(
            _bcm_th2_ecmp_dlb_dynamic_size_decode(flow_set_size, &num_entries));
        block_base_ptr =
            entry_base_ptr >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        num_blocks =
            num_entries >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;

        _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(unit, block_base_ptr,
                                                   num_blocks);
    }

    /* Clear ECMP DLB group control */
    if (cleanup_flowset) {
        SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_GROUP_CONTROLm(unit, \
                    MEM_BLOCK_ALL, dlb_id, \
                    soc_mem_entry_null(unit, DLB_ECMP_GROUP_CONTROLm)));
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((soc_feature(unit, soc_feature_th3_style_dlb))) {
            if (dlb_id < soc_reg_field_length(unit,
                         DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
                SOC_IF_ERROR_RETURN( \
                  READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
                COMPILER_64_BITCLR(dlb_en_bitmap, dlb_id);
                SOC_IF_ERROR_RETURN( \
                  WRITE_DLB_ID_0_TO_63_ENABLEr(unit, dlb_en_bitmap));
            } else {
                SOC_IF_ERROR_RETURN( \
                  READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap));
                COMPILER_64_BITCLR(dlb_en_bitmap, (dlb_id -
                                 (soc_reg_field_length(unit,
                                 DLB_ID_0_TO_63_ENABLEr, BITMAPf))));
                SOC_IF_ERROR_RETURN( \
                  WRITE_DLB_ID_64_TO_127_ENABLEr(unit, dlb_en_bitmap));
            }
        }
#endif
    }

    /* Allocate port map */
    port_map_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size = SHR_BITALLOCSIZE(port_map_width);
    port_map = sal_alloc(alloc_size, "DLB ECMP port map");
    if (NULL == port_map) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map, 0, alloc_size);

    port_map2 = sal_alloc(alloc_size, "DLB ECMP port map2");
    if (NULL == port_map2) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map2, 0, alloc_size);

    rv = READ_DLB_ECMP_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY, membership_pointer,
            &dlb_ecmp_group_membership_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }
    soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
            &dlb_ecmp_group_membership_entry, PORT_MAPf, port_map);
    soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
            &dlb_ecmp_group_membership_entry, PORT_MAP_ALTERNATEf, port_map2);

    /* Allocate port map remove */
    port_map_remove = sal_alloc(alloc_size, "DLB ECMP port map remove");
    if (NULL == port_map_remove) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map_remove, 0, alloc_size);

    SHR_BITOR_RANGE(port_map, port_map2, 0, port_map_width, port_map);
    for (port = 0; port < port_map_width; port++) {
        if (!SHR_BITGET(port_map, port)) {
            continue;
        }
        current_ptr = ECMP_DLB_PORT_LIST(unit, port);

        /*
         * Traverse the port list and then delete all duplicate nodes, if any,
         * in given DLB group.
         */

        prev_ptr = NULL;
        while (current_ptr) {
            current_next = current_ptr->next;
            if (current_ptr->dlb_id == dlb_id) {
                /* Duplicate nodes are always bundled together.
                 * Keep the first one if duplicate notes exist.
                 */
                duplicate = 0;
                if (prev_ptr &&
                    prev_ptr->nh_index == current_ptr->nh_index) {
                    duplicate = 1;
                }

                if (duplicate) {
                    prev_ptr->next = current_next;
                    sal_free(current_ptr);
                    current_ptr = NULL;
                } else {
                    current_ptr->ref_count = 0;
                    current_ptr->dlb_id = -1;
                }
                if (ECMP_DLB_PORT_INFO(unit, port).dlb_count <= 0) {
                    rv = BCM_E_INTERNAL;
                    goto error;
                }
                ECMP_DLB_PORT_INFO(unit, port).dlb_count--;
            }
            if (current_ptr != NULL) {
                prev_ptr = current_ptr;
            }
            current_ptr = current_next;
        }

        if (ECMP_DLB_PORT_INFO(unit, port).dlb_count == 0) {
            /* This port is now removed from all DLB Groups.
             * So it's safe to remove it from link_control.
             */
            SHR_BITSET(port_map_remove, port);
            port_map_remove_set = 1;
        }
    }

    if (port_map_remove_set) {
        rv = READ_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                    &link_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        /* Clear software status bitmap */
        status_bitmap = sal_alloc(alloc_size, "DLB ECMP member status bitmap");
        if (NULL == status_bitmap) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(status_bitmap, 0, alloc_size);
        soc_DLB_ECMP_LINK_CONTROLm_field_get(unit,
                &link_control_entry, SW_PORT_STATEf, status_bitmap);
        SHR_BITREMOVE_RANGE(status_bitmap, port_map_remove, 0, port_map_width,
                status_bitmap);
        soc_DLB_ECMP_LINK_CONTROLm_field_set(unit,
                &link_control_entry, SW_PORT_STATEf, status_bitmap);

        /* Clear software override bitmap */
        override_bitmap = sal_alloc(alloc_size, "DLB ECMP member override bitmap");
        if (NULL == override_bitmap) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(override_bitmap, 0, alloc_size);
        soc_DLB_ECMP_LINK_CONTROLm_field_get(unit,
                &link_control_entry, SW_OVERRIDE_PORT_MAPf,
                override_bitmap);
        SHR_BITREMOVE_RANGE(override_bitmap, port_map_remove, 0, port_map_width,
                override_bitmap);
        soc_DLB_ECMP_LINK_CONTROLm_field_set(unit,
                &link_control_entry, SW_OVERRIDE_PORT_MAPf,
                override_bitmap);
        rv = WRITE_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                    &link_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }


    /* Clear group membership */
    rv = WRITE_DLB_ECMP_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY, membership_pointer,
            soc_mem_entry_null(unit, DLB_ECMP_GROUP_MEMBERSHIPm));
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Clear group port2member */
    entry_size = sizeof(dlb_ecmp_group_port_to_member_entry_t);
    entries_per_profile = ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size;
    alloc_size = entries_per_profile * entry_size;
    entry_arr = soc_cm_salloc(unit, alloc_size,
                              "ECMP DLB Group Port To Member entries");
    if (entry_arr == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(entry_arr, 0, alloc_size);

    if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
        index_max = port2member_base + entries_per_profile - 1;
        rv = soc_mem_write_range(unit, DLB_ECMP_GROUP_PORT_TO_MEMBERm, MEM_BLOCK_ALL,
                                 port2member_base, index_max, entry_arr);
    }

    if (BCM_FAILURE(rv)) {
        goto error;
    }

    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    sal_free(port_map);
    sal_free(port_map2);
    sal_free(port_map_remove);
    soc_cm_sfree(unit, entry_arr);

    /* Free DLB ID */
    if ((soc_feature(unit, soc_feature_th3_style_dlb))) {
        if (delete_all_flag) {
            BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_id_free(unit, dlb_id));
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_id_free(unit, dlb_id));
    }

    return BCM_E_NONE;

error:
    if (NULL != port_map) {
        sal_free(port_map);
    }
    if (NULL != port_map2) {
        sal_free(port_map2);
    }
    if (NULL != port_map_remove) {
        sal_free(port_map_remove);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    if (NULL != entry_arr) {
        soc_cm_sfree(unit, entry_arr);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_port_array_get
 * Purpose:
 *      Get ports for each ECMP member.
 * Parameters:
 *      unit                - (IN) SOC unit number.
 *      ecmp                - (IN) ECMP group info.
 *      dlb_id              - (IN) DLB group ID.
 *      primary_count       - (IN) ECMP primary count..
 *      intf_count          - (IN) Number of elements in intf_array.
 *      intf_array          - (IN) Array of Egress forwarding objects.
 *      port_array          - (OUT) Array of ports.
 *      portmap_first_use   - (OUT) Port Bitmap indicates correspoinding port
 *                                  in port_array being attached to ECMP Group
 *                                  the first time.
 *      ecmp_member_bitmap  - (IN) ECMP DLB members.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_port_array_get(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                 int dlb_id, int primary_count, int intf_count,
                                 bcm_if_t *intf_array, int *port_array,
                                 SHR_BITDCL *portmap_first_use,
                                 SHR_BITDCL *ecmp_member_bitmap)
{
    int rv = BCM_E_NONE;
    int i;
    _th2_ecmp_dlb_port_membership_t *current_ptr, *first_ptr;
    _th2_ecmp_dlb_port_membership_t *membership = NULL;
    ing_l3_next_hop_entry_t ing_nh_entry;
    int mod = 0, port;
    int is_local;
    int nh_index;
    int location_offset = 0;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 dt, dv;
#endif

    /* Only alternate members exist , start from 0 instead of max_paths / 2 */
    if ((ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) &&
        (primary_count == 0))  {
        location_offset = ecmp->max_paths / 2;
    }

    for (i = 0; i < intf_count; i++) {
        /* Ignore filler members */
        if (!SHR_BITGET(ecmp_member_bitmap, i + location_offset)) {
            continue;
        }

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            nh_index =  intf_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf_array[i])) {
            nh_index = intf_array[i] - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            return BCM_E_PARAM;
        }

        /* Derive port from nh_index */
        rv = (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                    nh_index, &ing_nh_entry));
        if (BCM_FAILURE(rv)) {
            return rv;
        }
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_generic_dest)) {
            dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh_entry,
                    DESTINATIONf, &dt);
            if (dt == SOC_MEM_FIF_DEST_LAG) {
                /* Next hop correspond to Trunk group */
                return BCM_E_CONFIG;
            } else if (dt == SOC_MEM_FIF_DEST_DGPP) {
                mod = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                    SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
            } else {
                return BCM_E_PARAM;
            }
        } else
#endif
        {
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
                /* Next hop should correspond to a single physical port. */
                return BCM_E_CONFIG;
            }
            if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
                mod = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                        MODULE_IDf);
            }
            port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                    PORT_NUMf);
        }
        (void) _bcm_esw_modid_is_local(unit, mod, &is_local);
        if (!is_local) {
            return BCM_E_PARAM;
        }
        port_array[i] = port;

        /* Check if DLB attributes have been set for next hop index */
        current_ptr = ECMP_DLB_PORT_LIST(unit, port);
        while (current_ptr) {
            if (current_ptr->nh_index == nh_index) {
                break;
            }
            current_ptr = current_ptr->next;
        }

        if (current_ptr == NULL) {
            /* User did not configure DLB attributes for this next hop index */
            return BCM_E_CONFIG;
        }

        /* When the port is first referenced by a DLB Group, its link control
         * state will be cleard. This is consitent to free_resource where
         * the port is removed from all DLB Groups.
         */
        if (ECMP_DLB_PORT_INFO(unit, port).dlb_count == 0) {
            SHR_BITSET(portmap_first_use, port);
        }

        /* Continue traverse */
        first_ptr = NULL;
        while (current_ptr) {
            if (current_ptr->nh_index != nh_index) {
                current_ptr = current_ptr->next;
                continue;
            }
            /* There're 3 cases when nh_index equals:
             * 1) this nexthop never be used. dlb_id = -1.
             * 2) this nexthop is used, and now will be used again in same dlb_id.
             * 3) this nexthop is used, but now will be used in different dlb_id.
 */

            if (-1 == current_ptr->dlb_id) {
                /* first case, set ref_count, dlb_id, and dlb_count.
                 * And break.
                 */
                current_ptr->ref_count = 1;
                current_ptr->dlb_id = dlb_id;
                ECMP_DLB_PORT_INFO(unit, port).dlb_count++;
                break;
            } else if (dlb_id == current_ptr->dlb_id) {
                /* second case, just increase ref_count. And break. */
                current_ptr->ref_count++;
                break;
            }
            if (first_ptr == NULL) {
                /* third case, record first same nexthop node, but don't break
                 * since there's still chance this dlb_id used later.
                 */
                first_ptr = current_ptr;
            }
            current_ptr = current_ptr->next;
        }

        /* deal with third case: same nexthop find but dlb_id is different.
         * It's certain that first_ptr is find. We have got this covered above.
         */
        if (current_ptr == NULL) {
            /* did not find an available node or find an existing node
             * however, we record the first node with same nexthop.
             * rember the rule is to bundle nodes with same nexthop.
             */

            if (first_ptr == NULL) {
                /* there must exist a node with same nexthop*/
                return BCM_E_INTERNAL;
            }

            membership = sal_alloc(sizeof(_th2_ecmp_dlb_port_membership_t),
                    "ecmp dlb nh membership");
            if (NULL == membership) {
                return BCM_E_MEMORY;
            }
            sal_memset(membership, 0, sizeof(_th2_ecmp_dlb_port_membership_t));

            /* this duplicate node will be placed after first one */
            membership->nh_index = nh_index;
            membership->ref_count = 1;
            membership->dlb_id = dlb_id;
            membership->next = first_ptr->next;
            first_ptr->next = membership;
            ECMP_DLB_PORT_INFO(unit, port).dlb_count++;
        }

        if (ECMP_DLB_PORT_INFO(unit, port).dlb_count < 1) {
            return BCM_E_INTERNAL;
        }
    }
    return BCM_E_NONE;
}

#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *      _bcm_th3_cmp_int
 * Purpose:
 *      Compare two integers.
 * Parameters:
 *      b - first compared integer
 *      a - second compared integer
 * Returns:
 *      a<=>b
 */
static INLINE int
_bcm_th3_cmp_int(void *a, void *b)
{
    int first;                  /* First compared integer. */
    int second;                 /* Second compared integer. */

    first = *(int *)a;
    second = *(int *)b;

    if (first < second) {
        return (BCM_L3_CMP_LESS);
    } else if (first > second) {
        return (BCM_L3_CMP_GREATER);
    }
    return (BCM_L3_CMP_EQUAL);
}
#endif

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_set
 * Purpose:
 *      Configure an ECMP dynamic load balancing group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      primary    - (IN) ECMP primary count.
 *      dlb_id     - (IN) DLB id. -1:  new create, others: replace.
 *      ecmp_member_bitmap   - (IN) ECMP DLB members.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_set(int unit, bcm_l3_egress_ecmp_t *ecmp, int intf_count,
                      bcm_if_t *intf_array, int primary_count, int dlb_id,
                      int *old_id, SHR_BITDCL *ecmp_member_bitmap)
{
    int rv = BCM_E_NONE;
    int *port_array = NULL;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int *ports = NULL;
#endif
    soc_mem_t mem;
    int flowset_entry_size;
    int mflowset_entry_size;
    dlb_ecmp_flowset_entry_t *flowset_entry;
    dlb_ecmp_flowset_member_entry_t *mflowset_entry;
    int num_blocks;
    int total_blocks;
    int max_block_base_ptr;
    int block_base_ptr;
    int occupied;
    int entry_base_ptr;
    int num_entries_per_block;
    int port_map_width;
    int member_bitmap_width;
    int member_bitmap_bitdclsize = 0;
    int alloc_size;
    uint32 *block_ptr = NULL;
    uint32 *mblock_ptr = NULL;
    int i = 0, k = 0, j = 0;
    int index_min, index_max;
    int flow_set_size;
    int dlb_mode, primary_size = 0, primary_thd = 0;
    int cost = 0, bias = 0;
    SHR_BITDCL *port_map = NULL;
    SHR_BITDCL *port_map2 = NULL;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *port_map_first_use = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int ecmp_group_idx;
    int entry_size;
    int entries_per_profile;
    int port;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int unique_intf_count = 0;
#endif
    uint32 membership_pointer;
    uint32 base_addr = 0;
    ecmp_dlb_control_entry_t ecmp_dlb_control_entry;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    dlb_ecmp_group_membership_entry_t dlb_ecmp_group_membership_entry;
    dlb_ecmp_link_control_entry_t link_control_entry;
#ifdef BCM_TRIDENT3_SUPPORT
    ecmp_count_entry_t ecmp_count_entry;
#endif
    dlb_ecmp_group_port_to_member_entry_t *entry_arr = NULL;
    int *member_offset = NULL;
    int dlb_member_count = 0;  /* Exclude filler member */
    int location_offset = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint64 dlb_en_bitmap;
    int unique_primary_intf_count = 0;
#endif

    if (ecmp_member_bitmap == NULL) {
        return BCM_E_PARAM;
    }

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
        primary_size = primary_count;
    } else {
        primary_size = 0;
    }

    if (dlb_id == -1) {
        /* Allocate a DLB ID */
        BCM_IF_ERROR_RETURN(
            _bcm_th2_ecmp_dlb_id_alloc(unit, ecmp, &dlb_id));
    }

    /* Allocate port array */
    port_array = sal_alloc(sizeof(int) * intf_count,
                           "ECMP DLB Ports");
    if (NULL == port_array) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_array, 0, sizeof(int) * intf_count);

    /* Allocate portmap first use */
    port_map_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size = SHR_BITALLOCSIZE(port_map_width);
    port_map_first_use = sal_alloc(alloc_size, "DLB ECMP port map first use");
    if (NULL == port_map_first_use) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map_first_use, 0, alloc_size);

    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

    rv = _bcm_th2_ecmp_dlb_port_array_get(unit, ecmp, dlb_id,
                                          primary_count, intf_count,
                                          intf_array, port_array,
                                          port_map_first_use, ecmp_member_bitmap);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    if (!SHR_BITNULL_RANGE(port_map_first_use, 0, port_map_width)) {
        rv = READ_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                    &link_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        /* Clear software status bitmap */
        status_bitmap = sal_alloc(alloc_size, "DLB ECMP member status bitmap");
        if (NULL == status_bitmap) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(status_bitmap, 0, alloc_size);
        soc_DLB_ECMP_LINK_CONTROLm_field_get(unit,
                &link_control_entry, SW_PORT_STATEf, status_bitmap);
        SHR_BITREMOVE_RANGE(status_bitmap, port_map_first_use, 0, port_map_width,
                status_bitmap);
        soc_DLB_ECMP_LINK_CONTROLm_field_set(unit,
                &link_control_entry, SW_PORT_STATEf, status_bitmap);

        /* Clear software override bitmap */
        override_bitmap = sal_alloc(alloc_size, "DLB ECMP member override bitmap");
        if (NULL == override_bitmap) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(override_bitmap, 0, alloc_size);
        soc_DLB_ECMP_LINK_CONTROLm_field_get(unit,
                &link_control_entry, SW_OVERRIDE_PORT_MAPf,
                override_bitmap);
        SHR_BITREMOVE_RANGE(override_bitmap, port_map_first_use, 0, port_map_width,
                override_bitmap);
        soc_DLB_ECMP_LINK_CONTROLm_field_set(unit,
                &link_control_entry, SW_OVERRIDE_PORT_MAPf,
                override_bitmap);
        rv = WRITE_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                    &link_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Allocate ports */
    ports = sal_alloc(sizeof(int) * intf_count,
                           "ECMP DLB Ports");
    if (NULL == ports) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    sal_memset(ports, 0, sizeof(int) * intf_count);
#endif

    /* Allocate port map */
    port_map_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size = SHR_BITALLOCSIZE(port_map_width);
    port_map = sal_alloc(alloc_size, "DLB ECMP port map");
    if (NULL == port_map) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map, 0, alloc_size);

    port_map2 = sal_alloc(alloc_size, "DLB ECMP port map 2");
    if (NULL == port_map2) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(port_map2, 0, alloc_size);

    member_offset = sal_alloc(intf_count * sizeof(int),
                              "dlb member offset array");
    if (NULL == member_offset) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    sal_memset(member_offset, 0, intf_count * sizeof(int));

    /* Only alternate members exist , start from 0 instead of max_paths / 2 */
    if ((ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) &&
        (primary_count == 0))  {
        location_offset = ecmp->max_paths / 2;
    }

    /* Allocate member bitmap */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        int index = 0;
        bcm_if_t *intf_array_copy;

        intf_array_copy = sal_alloc(intf_count * sizeof(int),
                                  "dlb member offset array");

        if (NULL == intf_array_copy) {
            rv = BCM_E_MEMORY;
            goto error;
        }

        if (dlb_id < soc_reg_field_length(unit,
                     DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
            rv = READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap);
            if (rv < 0) {
                goto error;
            }
            COMPILER_64_BITSET(dlb_en_bitmap, dlb_id);
            rv = WRITE_DLB_ID_0_TO_63_ENABLEr(unit, dlb_en_bitmap);
            if (rv < 0) {
                goto error;
            }
        } else {
            rv = READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap);
            if (rv < 0) {
                goto error;
            }
            COMPILER_64_BITSET(dlb_en_bitmap, (dlb_id -
                                (soc_reg_field_length(unit,
                                DLB_ID_0_TO_63_ENABLEr, BITMAPf))));
            rv = WRITE_DLB_ID_64_TO_127_ENABLEr(unit, dlb_en_bitmap);
            if (rv < 0) {
                goto error;
            }
        }
        for (i = 0; i < intf_count; i++) {
            intf_array_copy[i] = intf_array[i];
            if (!SHR_BITGET(ecmp_member_bitmap, i + location_offset)) {
                continue;
            }
            /* Construct port_map */
            port = port_array[i];
            if ((ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM) ||
                (i < primary_size)) {
                SHR_BITSET(port_map, port);
                /* Construct member bitmap */
            } else {
                SHR_BITSET(port_map2, port);
                /* Construct member bitmap */
            }
            member_offset[j++] = i;
        }

        _shr_sort(intf_array_copy, intf_count, sizeof(int),
                  _bcm_th3_cmp_int);

        index = 0;
        for (i = 1; i < intf_count; i++) {
            if (intf_array_copy[i] != intf_array_copy[index]) {
                index++;
                intf_array_copy[index]  = intf_array_copy[i];
            }
        }

        rv = _bcm_th3_l3_ecmp_dlb_port_get(unit, ecmp, dlb_id,
                                          primary_count, intf_count,
                                          intf_array_copy, ports);

        if (rv != BCM_E_NONE) {
            goto error;
        }
        for (i = 0; i < intf_count; i++) {
            if (!SHR_BITGET(ecmp_member_bitmap, i + location_offset)) {
                continue;
            }
            if ((ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM) ||
                (i < primary_size)) {
                unique_primary_intf_count++;
            }
            unique_intf_count++;
        }
        if ((ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM) &&
            (unique_intf_count > primary_count)) {
            unique_intf_count = primary_count;
        }
        _bcm_th3_l3_ecmp_dlb_port_member_set(unit, ports, unique_primary_intf_count, unique_intf_count, dlb_id);
        _bcm_th3_l3_ecmp_dlb_nhi_member_set(unit, intf_array_copy, unique_primary_intf_count, unique_intf_count, dlb_id);
    } else
#endif
    {
        member_bitmap_width = soc_mem_field_length(unit,
                DLB_ECMP_GROUP_PORT_TO_MEMBERm, ECMP_MEMBER_BITMAPf);
        member_bitmap_bitdclsize = _SHR_BITDCLSIZE(member_bitmap_width);

        entries_per_profile = ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size;
        alloc_size = SHR_BITALLOCSIZE(member_bitmap_width)  * entries_per_profile;
        member_bitmap = sal_alloc(alloc_size, "DLB ECMP member bitmap");
        if (NULL == member_bitmap) {
            rv = BCM_E_MEMORY;
            goto error;
        }

        sal_memset(member_bitmap, 0, alloc_size);

        /* Fulfill member bitmaps */
        for (i = 0; i < intf_count; i++) {
            if (!SHR_BITGET(ecmp_member_bitmap, i + location_offset)) {
                continue;
            }
            member_offset[j++] = i;
            /* Construct port_map */
            port = port_array[i];
            if (ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM || i < primary_size) {
                SHR_BITSET(port_map, port);
                /* Construct member bitmap */
                SHR_BITSET(&member_bitmap[member_bitmap_bitdclsize * port], i);
            } else {
                SHR_BITSET(port_map2, port);
                /* Construct member bitmap */
                SHR_BITSET(&member_bitmap[member_bitmap_bitdclsize * port],
                           (i - primary_size));
            }
        }
    }

    SHR_BITAND_RANGE(port_map, port_map2, 0, port_map_width,
                     port_map_first_use);
    if (!SHR_BITNULL_RANGE(port_map_first_use, 0, port_map_width)) {
        /* the priamray and alternate port map should not overlap */
        rv = BCM_E_PARAM;
        goto error;
    }

    if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
        /* Allocate entry array */
        mem = DLB_ECMP_GROUP_PORT_TO_MEMBERm;
        entry_size = sizeof(dlb_ecmp_group_port_to_member_entry_t);
        entries_per_profile = ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size;
        alloc_size = entries_per_profile * entry_size;
        entry_arr = soc_cm_salloc(unit, alloc_size,
                                  "ECMP DLB Group Port To Member entries");
        if (entry_arr == NULL) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(entry_arr, 0, alloc_size);

        /* Construct Port To Member entries */
        SHR_BITOR_RANGE(port_map, port_map2, 0, port_map_width,
                        port_map_first_use);

        for (port = 0; port < entries_per_profile; port++) {
            if (SHR_BITGET(port_map_first_use, port)) {
                soc_DLB_ECMP_GROUP_PORT_TO_MEMBERm_field_set(unit,
                        &entry_arr[port],
                        ECMP_MEMBER_BITMAPf,
                        &member_bitmap[member_bitmap_bitdclsize * port]);

                /* dlb_count only count different DLBs, a ECMP group could have
                 * 2 identical nexthops (same port, 1 node),
                 * 1 different nexthop (same port, 1 node)
                 * however, the 3 nexthops only contributes 2 count.
                 */
            } else {
                sal_memcpy(&entry_arr[port],
                   soc_mem_entry_null(unit, mem), entry_size);
            }
        }

        base_addr = dlb_id * entries_per_profile;
        index_max = base_addr + entries_per_profile - 1;
        rv = soc_mem_write_range(unit, DLB_ECMP_GROUP_PORT_TO_MEMBERm, MEM_BLOCK_ALL,
                                 base_addr, index_max, entry_arr);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }

    /* Construct membership entry */
    membership_pointer = dlb_id;
    sal_memset(&dlb_ecmp_group_membership_entry, 0,
               sizeof(dlb_ecmp_group_membership_entry_t));
    soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_set(unit,
                                             &dlb_ecmp_group_membership_entry,
                                             PORT_MAPf,
                                             port_map);
    soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_set(unit,
                                             &dlb_ecmp_group_membership_entry,
                                             PORT_MAP_ALTERNATEf,
                                             port_map2);
    rv = WRITE_DLB_ECMP_GROUP_MEMBERSHIPm(unit,
                                          MEM_BLOCK_ALL,
                                          membership_pointer,
                                          &dlb_ecmp_group_membership_entry);

    if (BCM_FAILURE(rv)) {
        goto error;
    }

    if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
        rv = READ_ECMP_DLB_CONTROLm(unit, MEM_BLOCK_ANY, ecmp_group_idx,
                &ecmp_dlb_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }

    /* Set ECMP DLB group control table */
    if (old_id == NULL) { /* not replace */
        sal_memset(&dlb_ecmp_group_control_entry, 0,
                sizeof(dlb_ecmp_group_control_entry_t));
    } else {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((soc_feature(unit, soc_feature_th3_style_dlb))) {
            *old_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp_group_idx);
        } else
#endif
        {
            *old_id = soc_ECMP_DLB_CONTROLm_field32_get
                                    (unit, &ecmp_dlb_control_entry, DLB_IDf);
        }
        rv = READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, *old_id,
                                      &dlb_ecmp_group_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }
    if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
        soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            GROUP_PORT_TO_MEMBER_BASE_ADDRf, base_addr);
    }

    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
        GROUP_MEMBERSHIP_POINTERf, membership_pointer);

    if (old_id == NULL) { /* not replace */
        /* Find a contiguous region in flow set table.
         * A better practice would be: make this 2 sub-routies: alloc and free.
         * And do the alloc ahead of time, to make sure the resource is available.
         */

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)){
            if (soc_mem_index_count(unit, DLB_ECMP_FLOWSETm)) {
                rv = soc_reg_field32_modify(
                        unit, ENHANCED_HASHING_CONTROLr, REG_PORT_ANY,
                        RH_DLB_SELECTIONf, 0);
                if (BCM_FAILURE(rv)) {
                    goto error;
                }
            }
        }
#endif
        flowset_entry_size = sizeof(dlb_ecmp_flowset_entry_t);
        mflowset_entry_size = sizeof(dlb_ecmp_flowset_member_entry_t);
        num_blocks = ecmp->dynamic_size >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        total_blocks = soc_mem_index_count(unit, DLB_ECMP_FLOWSETm) >>
                       _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        max_block_base_ptr = total_blocks - num_blocks;
        for (block_base_ptr = 0;
                block_base_ptr <= max_block_base_ptr;
                block_base_ptr++) {
            /* Check if the contiguous region of flow set table from
             * block_base_ptr to (block_base_ptr + num_blocks - 1) is free.
             */
            _BCM_ECMP_DLB_FLOWSET_BLOCK_TEST_RANGE(unit, block_base_ptr, num_blocks,
                    occupied);
            if (!occupied) {
                break;
            }
        }
        if (block_base_ptr > max_block_base_ptr) {
            /* A contiguous region of the desired size could not be found in
             * flow set table.
             */
            rv = BCM_E_RESOURCE;
            goto error;
        }

        /* Set DLB flow set table */
        entry_base_ptr = block_base_ptr << _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        num_entries_per_block = 1 << _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        alloc_size = num_entries_per_block * flowset_entry_size;
        block_ptr = soc_cm_salloc(unit, alloc_size,
                "Block of DLB_ECMP_FLOWSET entries");
        if (NULL == block_ptr) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(block_ptr, 0, alloc_size);

        alloc_size = num_entries_per_block * mflowset_entry_size;
        mblock_ptr = soc_cm_salloc(unit, alloc_size,
                "Block of DLB_ECMP_FLOWSET_MEMBER entries");
        if (NULL == mblock_ptr) {
            rv = BCM_E_MEMORY;
            goto error;
        }
        sal_memset(mblock_ptr, 0, alloc_size);

        /* Reset ECMP count excluding filler members */
        SHR_BITCOUNT_RANGE(ecmp_member_bitmap, dlb_member_count,
                           0, ecmp->max_paths);

        for (i = 0; i < num_blocks; i++) {
            for (k = 0; k < num_entries_per_block; k++) {
                flowset_entry = soc_mem_table_idx_to_pointer(unit,
                        DLB_ECMP_FLOWSETm, dlb_ecmp_flowset_entry_t *,
                        block_ptr, k);
                mflowset_entry = soc_mem_table_idx_to_pointer(unit,
                        DLB_ECMP_FLOWSET_MEMBERm, dlb_ecmp_flowset_member_entry_t *,
                        mblock_ptr, k);
                if (dlb_member_count > 0) {
                    soc_DLB_ECMP_FLOWSETm_field32_set(unit, flowset_entry,
                            VALIDf, 1);
                    j = (i * num_entries_per_block + k) % dlb_member_count;
                    soc_DLB_ECMP_FLOWSETm_field32_set(unit, flowset_entry,
                            PORT_MEMBER_ASSIGNMENTf,
                            port_array[member_offset[j]]);
                    if (member_offset[j] < primary_count) {
                        soc_DLB_ECMP_FLOWSET_MEMBERm_field32_set(
                            unit, mflowset_entry,
                            ECMP_MEMBER_ASSIGNMENTf, member_offset[j]);
                        soc_DLB_ECMP_FLOWSET_MEMBERm_field32_set(
                            unit, mflowset_entry,
                            ECMP_MEMBER_IS_ALTERNATEf, 0);
                    } else {
                        soc_DLB_ECMP_FLOWSET_MEMBERm_field32_set(
                            unit, mflowset_entry, ECMP_MEMBER_ASSIGNMENTf,
                            member_offset[j] - primary_count);
                        soc_DLB_ECMP_FLOWSET_MEMBERm_field32_set(
                            unit, mflowset_entry,
                            ECMP_MEMBER_IS_ALTERNATEf, 1);
                    }
                } else {
                    soc_DLB_ECMP_FLOWSETm_field32_set(unit, flowset_entry,
                            VALIDf, 0);
                }
            }
            index_min = entry_base_ptr + i * num_entries_per_block;
            index_max = index_min + num_entries_per_block - 1;
            rv = soc_mem_write_range(unit, DLB_ECMP_FLOWSETm, MEM_BLOCK_ALL,
                    index_min, index_max, block_ptr);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
            rv = soc_mem_write_range(unit, DLB_ECMP_FLOWSET_MEMBERm, MEM_BLOCK_ALL,
                    index_min, index_max, mblock_ptr);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
        }
        _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr, num_blocks);

        soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
                ENABLE_OPTIMAL_CANDIDATE_UPDATEf, 1);
        soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
                FLOW_SET_BASEf, entry_base_ptr);
        rv = _bcm_th2_ecmp_dlb_dynamic_size_encode(ecmp->dynamic_size,
                &flow_set_size);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
                FLOW_SET_SIZEf, flow_set_size);
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            ecmp_grp_enhanced_hashing[unit][ecmp_group_idx] = 1;
        }
#endif
    }
    switch (ecmp->dynamic_mode) {
        case BCM_L3_ECMP_DYNAMIC_MODE_DGM:
            primary_thd = ecmp->dgm.threshold;
            cost = ecmp->dgm.cost;
            bias = ecmp->dgm.bias;
            dlb_mode = 0;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_NORMAL:
            dlb_mode = 0;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED:
            dlb_mode = 1;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL:
            dlb_mode = 2;
            break;
        default:
            rv = BCM_E_PARAM;
            goto error;
    }
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            PORT_ASSIGNMENT_MODEf, dlb_mode);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            PRIMARY_GROUP_SIZEf, primary_size);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            PRIMARY_PATH_THRESHOLDf, primary_thd);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            ALTERNATE_PATH_COSTf, cost);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            ALTERNATE_PATH_BIASf, bias);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            INACTIVITY_DURATIONf, ecmp->dynamic_age);
    rv = WRITE_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ALL, dlb_id,
            &dlb_ecmp_group_control_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Update ECMP DLB control table */
    if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
        soc_ECMP_DLB_CONTROLm_field32_set(unit, &ecmp_dlb_control_entry, GROUP_ENABLEf, 1);
        soc_ECMP_DLB_CONTROLm_field32_set(unit, &ecmp_dlb_control_entry, DLB_IDf, dlb_id);
        rv = WRITE_ECMP_DLB_CONTROLm(unit, MEM_BLOCK_ALL, ecmp_group_idx,
                &ecmp_dlb_control_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group_idx, &ecmp_count_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_count_entry, LB_MODEf, 0x4);
        rv = WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group_idx, &ecmp_count_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    }
#endif
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    sal_free(port_array);
    sal_free(port_map);
    sal_free(port_map2);
    sal_free(member_offset);
    if (member_bitmap) {
        sal_free(member_bitmap);
    }
    if (entry_arr) {
        soc_cm_sfree(unit, entry_arr);
    }
    sal_free(port_map_first_use);
    if (block_ptr) {
        soc_cm_sfree(unit, block_ptr);
    }
    if (mblock_ptr) {
        soc_cm_sfree(unit, mblock_ptr);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (ports) {
        sal_free(ports);
    }
#endif
    return BCM_E_NONE;

error:
    if (dlb_id != -1) {
        _bcm_th2_ecmp_dlb_id_free(unit, dlb_id);
    }
    if (NULL != port_array) {
        sal_free(port_array);
    }
    if (NULL != port_map_first_use) {
        sal_free(port_map_first_use);
    }
    if (NULL != block_ptr) {
        soc_cm_sfree(unit, block_ptr);
    }
    if (NULL != mblock_ptr) {
        soc_cm_sfree(unit, mblock_ptr);
    }
    if (NULL != port_map) {
        sal_free(port_map);
    }
    if (NULL != port_map2) {
        sal_free(port_map2);
    }
    if (NULL != member_bitmap) {
        sal_free(member_bitmap);
    }
    if (NULL != member_offset) {
        sal_free(member_offset);
    }
    if (NULL != entry_arr) {
        soc_cm_sfree(unit, entry_arr);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (NULL != ports) {
        sal_free(ports);
    }
#endif
    return rv;
}
/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_dlb_create
 * Purpose:
 *      Create ECMP DLB group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      ecmp_member_bitmap   - (IN) ECMP DLB members.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_ecmp_dlb_create(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                  int intf_count, bcm_if_t *intf_array,
                                  int primary_count,
                                  SHR_BITDCL *dlb_member_bitmap)
{
    int dlb_enable;
    int dynamic_size_encode;
    int ecmp_group_idx;
    uint32 hw_limit;
    SHR_BITDCL *group_bitmap_ptr = NULL;

    bcm_l3_egress_ecmp_t dlb_ecmp;
    int dlb_ecmp_count = 0;

    if (BCM_TH2_ECMP_IS_DLB_OR_DGM(unit, ecmp->dynamic_mode)) {
        dlb_enable = 1;
    } else {
        dlb_enable = 0;
    }

    if (dlb_enable) {
        /* Verify dynamic_size */
        BCM_IF_ERROR_RETURN
            (_bcm_th2_ecmp_dlb_dynamic_size_encode(ecmp->dynamic_size,
                                                   &dynamic_size_encode));

        /* Verify dynamic_age */
        hw_limit = (1 << soc_mem_field_length(unit, DLB_ECMP_GROUP_CONTROLm,
                                              INACTIVITY_DURATIONf)) - 1;
        if (ecmp->dynamic_age < 16 ||
            ecmp->dynamic_age > hw_limit) {
            /* Hardware limits */
            if ((!(soc_feature(unit, soc_feature_th3_style_dlb)) &&
                !(soc_feature(unit, soc_feature_td3_dlb))) ||
                (ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED)) {
                return BCM_E_PARAM;
            }
        }

        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            if (primary_count > intf_count) {
                return BCM_E_PARAM;
            }
            hw_limit = (1 << soc_mem_field_length(unit, DLB_ECMP_GROUP_CONTROLm,
                                                  PRIMARY_PATH_THRESHOLDf)) - 1;

            if (ecmp->dgm.threshold > hw_limit) {
                return BCM_E_PARAM;
            }
            hw_limit = (1 << soc_mem_field_length(unit, DLB_ECMP_GROUP_CONTROLm,
                                                  ALTERNATE_PATH_COSTf)) - 1;

            if (ecmp->dgm.cost > hw_limit ||
                ecmp->dgm.bias > hw_limit) {
                return BCM_E_PARAM;
            }
        }
    }

    /* Free resources associated with old DLB group, if exists */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

    if (ecmp->flags & BCM_L3_REPLACE) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((!dlb_enable) && (soc_feature(unit, soc_feature_th3_style_dlb))) {
            return BCM_E_NONE;
        }
#endif

        bcm_l3_egress_ecmp_t_init(&dlb_ecmp);
        dlb_ecmp.ecmp_intf = ecmp->ecmp_intf;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_l3_egress_ecmp_get(unit, &dlb_ecmp, 0, NULL,
                                        &dlb_ecmp_count, TRUE));
        if (dlb_enable /* also indicates it was created before */
            && dlb_ecmp.dynamic_mode == ecmp->dynamic_mode
            && dlb_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED
            && dlb_ecmp.dynamic_size == ecmp->dynamic_size) {
            int old_id = 0, dlb_id;

            /* Do new style replace only if free resources left */
            if (BCM_E_NONE == _bcm_th2_ecmp_dlb_id_alloc(
                                  unit, ecmp, &dlb_id)) {
                if ((soc_feature(unit, soc_feature_th3_style_dlb))) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_th2_ecmp_dlb_free_resource(unit, ecmp_group_idx,
                                                        old_id, FALSE));
                }
                BCM_IF_ERROR_RETURN(
                    _bcm_th2_ecmp_dlb_set(unit, ecmp, intf_count, intf_array,
                                          primary_count, dlb_id,
                                          &old_id, dlb_member_bitmap));
                if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_th2_ecmp_dlb_free_resource(unit, ecmp_group_idx,
                                                        old_id, TRUE));
                }
                return BCM_E_NONE;
            }
        }
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_th3_style_dlb) && 
            (ecmp->flags & BCM_L3_REPLACE)) {
        int dlb_num, rv = 0;
        dlb_num = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp->ecmp_intf
                                   - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit));

        /* If the dynamic size for the new dlb id is greater than the one
           being replaced and also greater than the FLOWSET block size (256),
           we need to check if we have enough free resources
           for the extra size. If we do have free resources, we go ahead
           and clean up the old dlb id. Otherwise, we do not clean up and
           this way we do not destroy the old dlb id when replace fails
        */
        if (ecmp->dynamic_size > dlb_ecmp.dynamic_size &&
            ecmp->dynamic_size > (1 << _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT)) {
            rv = _bcm_th3_l3_egress_ecmp_dlb_resource_validate(unit, ecmp,
                                          dlb_num);
        }

        if ((rv == BCM_E_NONE)) {
            BCM_IF_ERROR_RETURN(
                _bcm_th2_ecmp_dlb_free_resource(unit, ecmp_group_idx, -1, TRUE));
        } else {
            return rv;  
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(
            _bcm_th2_ecmp_dlb_free_resource(unit, ecmp_group_idx, -1, TRUE));
    }

    if (dlb_enable) {
        /* Set new DLB group */
        BCM_IF_ERROR_RETURN(
            _bcm_th2_ecmp_dlb_set(unit, ecmp, intf_count, intf_array,
                                  primary_count, -1, NULL, dlb_member_bitmap));
    } else {
        /*  Clear DLB bitmap */
        group_bitmap_ptr =
            BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
        SHR_BITCLR_RANGE(group_bitmap_ptr, 0,
                         BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_dlb_destroy
 * Purpose:
 *      Destroy an ECMP DLB group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      mpintf     - (IN) L3 interface id pointing to Egress multipath object.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_ecmp_dlb_destroy(int unit, bcm_if_t mpintf)
{
    int ecmp_group_idx;
    SHR_BITDCL *group_bitmap_ptr = NULL;

    /* Free resources associated with old DLB group, if exists */
    ecmp_group_idx = mpintf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_free_resource(unit,
                                  ecmp_group_idx, -1, TRUE));
    /*  Clear DLB bitmap */
    group_bitmap_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
    SHR_BITCLR_RANGE(group_bitmap_ptr, 0,
                     BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_dlb_get
 * Purpose:
 *      Get info about an ECMP DLB group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN/OUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_ecmp_dlb_get(int unit, bcm_l3_egress_ecmp_t *ecmp)
{
    int ecmp_group_idx;
    ecmp_dlb_control_entry_t ecmp_group_entry;
    int group_enable, dlb_id;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    int dlb_mode, primary_size, primary_thd, cost, bias;
    int flow_set_size;

    /* Get DLB ID */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

    ecmp->dynamic_mode = 0;
    ecmp->dynamic_size = 0;
    ecmp->dynamic_age = 0;
    ecmp->dgm.threshold = 0;
    ecmp->dgm.cost = 0;
    ecmp->dgm.bias = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        if (ecmp_group_idx < BCM_TH3_L3_ECMP_DLB_START(unit)) {
            return BCM_E_NONE;
        }
        dlb_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp_group_idx);
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_ECMP_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                    ecmp_group_idx, &ecmp_group_entry));
        group_enable = soc_ECMP_DLB_CONTROLm_field32_get(unit,
                                            &ecmp_group_entry,
                                            GROUP_ENABLEf);
        if (!group_enable) {
            return BCM_E_NONE;
        }
        dlb_id = soc_ECMP_DLB_CONTROLm_field32_get(unit,
                            &ecmp_group_entry, DLB_IDf);
    }

    /* Get dynamic_mode */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY,
                dlb_id, &dlb_ecmp_group_control_entry));
    dlb_mode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, PORT_ASSIGNMENT_MODEf);
    primary_size = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, PRIMARY_GROUP_SIZEf);
    primary_thd = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, PRIMARY_PATH_THRESHOLDf);
    cost = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, ALTERNATE_PATH_COSTf);
    bias = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, ALTERNATE_PATH_BIASf);
    switch (dlb_mode) {
        case 0:
            if (primary_size == 0 &&
                !_BCM_ECMP_DLB_ID_DGM_GET(unit, dlb_id)) {
                ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_NORMAL;
            } else {
                ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
                ecmp->dgm.threshold = primary_thd;
                ecmp->dgm.cost      = cost;
                ecmp->dgm.bias      = bias;
            }
            /* In L1 warmboot, empty group's dgm mode cannot be recovered */
#ifdef BCM_WARM_BOOT_SUPPORT
            if (SOC_WARM_BOOT(unit)) {
                if (!ECMP_DLB_INFO(unit)->recovered_from_scache &&
                    ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
                    /* If already recovered from scache, do nothing. */
                    _BCM_ECMP_DLB_ID_DGM_SET(unit, dlb_id);
                }
            }
#endif /* BCM_WARM_BOOT_SUPPORT */

            break;
        case 1:
            ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED;
            break;
        case 2:
            ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    /* Get dynamic_size */
    flow_set_size = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_th2_ecmp_dlb_dynamic_size_decode(flow_set_size,
                                               (int *)&ecmp->dynamic_size));

    /* Get dynamic_age */
    ecmp->dynamic_age = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, INACTIVITY_DURATIONf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_l3_egress_ecmp_port_status_set
 * Purpose:
 *      Set ECMP DLB port status.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      port    - (IN) Port, not Gport.
 *      status  - (IN) ECMP port status.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th2_l3_egress_ecmp_port_status_set(int unit, bcm_port_t port, int status)
{
    dlb_ecmp_link_control_entry_t link_control_entry;
    int status_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int rv = BCM_E_NONE;

    soc_mem_lock(unit, DLB_ECMP_LINK_CONTROLm);
    /* Get status bitmap */
    rv = READ_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                &link_control_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, DLB_ECMP_LINK_CONTROLm);
        return rv;
    }
    status_width = soc_mem_field_length(unit, DLB_ECMP_LINK_CONTROLm,
            SW_PORT_STATEf);
    alloc_size = SHR_BITALLOCSIZE(status_width);
    status_bitmap = sal_alloc(alloc_size, "DLB ECMP port status bitmap");
    if (NULL == status_bitmap) {
        soc_mem_unlock(unit, DLB_ECMP_LINK_CONTROLm);
        return BCM_E_MEMORY;
    }
    sal_memset(status_bitmap, 0, alloc_size);
    soc_DLB_ECMP_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_PORT_STATEf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB ECMP port override bitmap");
    if (NULL == override_bitmap) {
        soc_mem_unlock(unit, DLB_ECMP_LINK_CONTROLm);
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    sal_memset(override_bitmap, 0, alloc_size);
    soc_DLB_ECMP_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_OVERRIDE_PORT_MAPf, override_bitmap);

    /* Update status and override bitmaps*/

    switch (status) {
        case BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_DOWN:
            SHR_BITSET(override_bitmap, port);
            SHR_BITCLR(status_bitmap, port);
            break;
        case BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_UP:
            SHR_BITSET(override_bitmap, port);
            SHR_BITSET(status_bitmap, port);
            break;
        case BCM_L3_ECMP_DYNAMIC_MEMBER_HW:
            SHR_BITCLR(override_bitmap, port);
            SHR_BITCLR(status_bitmap, port);
            break;
        default:
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            soc_mem_unlock(unit, DLB_ECMP_LINK_CONTROLm);
            return BCM_E_PARAM;
    }

    /* Write status and override bitmaps to hardware */
    soc_DLB_ECMP_LINK_CONTROLm_field_set(unit, &link_control_entry,
            SW_PORT_STATEf, status_bitmap);
    soc_DLB_ECMP_LINK_CONTROLm_field_set(unit, &link_control_entry,
            SW_OVERRIDE_PORT_MAPf, override_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);
    rv = WRITE_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                         &link_control_entry);

    soc_mem_unlock(unit, DLB_ECMP_LINK_CONTROLm);
    return rv;
}

/*
 * Function:
 *      _bcm_th2_l3_egress_ecmp_port_status_get
 * Purpose:
 *      Get ECMP DLB port status.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      port    - (IN) Port, not Gport.
 *      status  - (OUT) ECMP port status.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th2_l3_egress_ecmp_port_status_get(int unit, bcm_port_t port, int *status)
{
    int rv = BCM_E_NONE;
    dlb_ecmp_link_control_entry_t link_control_entry;
    int status_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    SHR_BITDCL *hw_status_bitmap = NULL;
    dlb_ecmp_port_state_entry_t hw_state;

    /* Get status bitmap */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_LINK_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                &link_control_entry));
    status_width = soc_mem_field_length(unit, DLB_ECMP_LINK_CONTROLm,
            SW_PORT_STATEf);
    alloc_size = SHR_BITALLOCSIZE(status_width);
    status_bitmap = sal_alloc(alloc_size, "DLB ECMP port status bitmap");
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(status_bitmap, 0, alloc_size);
    soc_DLB_ECMP_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_PORT_STATEf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB ECMP port override bitmap");
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    sal_memset(override_bitmap, 0, alloc_size);
    soc_DLB_ECMP_LINK_CONTROLm_field_get(unit, &link_control_entry,
            SW_OVERRIDE_PORT_MAPf, override_bitmap);

    /* Derive status from software override and status bitmaps or
     * hardware state bitmap.
     */
    if (SHR_BITGET(override_bitmap, port)) {
        if (SHR_BITGET(status_bitmap, port)) {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_UP;
        } else {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_DOWN;
        }
    } else {
        /* Get hardware bitmap */
        hw_status_bitmap = sal_alloc(alloc_size, "DLB ECMP member hw status bitmap");
        if (NULL == hw_status_bitmap) {
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            return BCM_E_MEMORY;
        }
        sal_memset(hw_status_bitmap, 0, alloc_size);

        rv = READ_DLB_ECMP_PORT_STATEm(unit, MEM_BLOCK_ANY, 0, &hw_state);
        if (BCM_FAILURE(rv)) {
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            sal_free(hw_status_bitmap);
            return rv;
        }
        soc_DLB_ECMP_PORT_STATEm_field_get(unit, &hw_state,
                BITMAPf, hw_status_bitmap);
        if (SHR_BITGET(hw_status_bitmap, port)) {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_HW_UP;
        } else {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_HW_DOWN;
        }
    }

    /* Cleanup */
    sal_free(status_bitmap);
    sal_free(override_bitmap);
    if (NULL != hw_status_bitmap) {
        sal_free(hw_status_bitmap);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_member_status_set
 * Purpose:
 *      Set ECMP DLB member status.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      intf    - (IN) L3 Interface ID pointing to an Egress forwarding object.
 *      status  - (IN) ECMP member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_l3_egress_ecmp_member_status_set(int unit, bcm_if_t intf, int status)
{
    int nh_index;
    int old_status;
    int mod = 0, port;
    int is_local;
    ing_l3_next_hop_entry_t ing_nh_entry;
    _th2_ecmp_dlb_port_membership_t *current_ptr;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 dt, dv;
#endif

    /* Derive next hop index */
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    /* Derive port from nh_index */
    BCM_IF_ERROR_RETURN(READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                nh_index, &ing_nh_entry));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_generic_dest)) {
        dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh_entry,
                DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_LAG) {
            /* Next hop correspond to Trunk group */
            return BCM_E_CONFIG;
        } else if (dt == SOC_MEM_FIF_DEST_DGPP) {
            mod = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
        } else {
            return BCM_E_PARAM;
        }
    } else
#endif
        {
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
                /* Next hop should correspond to a single physical port. */
                return BCM_E_CONFIG;
            }
            if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
                mod = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                        MODULE_IDf);
            }
            port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                    PORT_NUMf);
        }
    if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
        (void) _bcm_esw_modid_is_local(unit, mod, &is_local);
        if (!is_local) {
            return BCM_E_PARAM;
        }
    }

    /* Verify that the nh_index is with DLB arrtibutes. */
    current_ptr = ECMP_DLB_PORT_LIST(unit, port);
    while (current_ptr) {
        if (nh_index == current_ptr->nh_index &&
            current_ptr->dlb_id != -1) {
            break;
        }
        current_ptr = current_ptr->next;
    }

    if (current_ptr == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (!ECMP_DLB_INFO(unit)->ecmp_dlb_property_force_set &&
        ECMP_DLB_PORT_INFO(unit, port).nh_count > 1) {
        BCM_IF_ERROR_RETURN
        (_bcm_th2_l3_egress_ecmp_port_status_get(unit, port, &old_status));

        if (old_status == BCM_L3_ECMP_DYNAMIC_MEMBER_HW_UP ||
            old_status == BCM_L3_ECMP_DYNAMIC_MEMBER_HW_DOWN) {
            old_status = BCM_L3_ECMP_DYNAMIC_MEMBER_HW;
        }

        if (status != old_status) {
            return BCM_E_BUSY;
        } else {
            return BCM_E_NONE;
        }
    }

    return _bcm_th2_l3_egress_ecmp_port_status_set(unit, port, status);
}

/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_member_status_get
 * Purpose:
 *      Get ECMP DLB member status.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      intf    - (IN) L3 Interface ID pointing to an Egress forwarding object.
 *      status  - (OUT) ECMP member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th2_l3_egress_ecmp_member_status_get(int unit, bcm_if_t intf, int *status)
{
    int nh_index;
    int port = 0;
    ing_l3_next_hop_entry_t ing_nh_entry;
    _th2_ecmp_dlb_port_membership_t *current_ptr;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 dt, dv;
#endif

    if (NULL == status) {
        return BCM_E_PARAM;
    }

    /* Derive next hop index */
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    /* Derive port from nh_index */
    BCM_IF_ERROR_RETURN(READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                nh_index, &ing_nh_entry));
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_generic_dest)) {
        dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh_entry,
                DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_DGPP) {
            port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
        } else {
            return BCM_E_PARAM;
        }

    } else
#endif
    {
        port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                PORT_NUMf);
    }
    /* Verify that the nh_index is with DLB arrtibutes. */
    current_ptr = ECMP_DLB_PORT_LIST(unit, port);
    while (current_ptr) {
        if (nh_index == current_ptr->nh_index &&
            current_ptr->dlb_id != -1) {
            break;
        }
        current_ptr = current_ptr->next;
    }

    if (current_ptr == NULL) {
        return BCM_E_NOT_FOUND;
    }

    return _bcm_th2_l3_egress_ecmp_port_status_get(unit, port, status);
}
/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_dlb_ethertype_set
 * Purpose:
 *      Set the Ethertypes that are eligible or ineligible for
 *      ECMP dynamic load balancing.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      flags           - (IN) BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_count - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (IN) Array of Ethertypes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_ecmp_dlb_ethertype_set(
    int unit,
    uint32 flags,
    int ethertype_count,
    int *ethertype_array)
{
    int i, j;
    dlb_ecmp_eem_configuration_entry_t eem_entry;
    dlb_ecmp_ethertype_eligibility_map_entry_t ethertype_entry;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 reg_val = 0;
#endif

    /* Input check */
    if ((ethertype_count > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (ethertype_count > soc_mem_index_count(unit,
                DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm)) {
        return BCM_E_RESOURCE;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if ((soc_feature(unit, soc_feature_th3_style_dlb)) &&
        (flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER)) {
        return BCM_E_PARAM;
    }
#endif
    /* Update quality measure control register */
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    &reg_val));
        soc_reg_field_set(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                &reg_val, CONFIGURATIONf,
                flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
        soc_reg_field_set(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                &reg_val, INNER_OUTER_ETHERTYPE_SELECTIONf,
                flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
        BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    reg_val));
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ANY, 0, &eem_entry));
        soc_DLB_ECMP_EEM_CONFIGURATIONm_field32_set(unit,
                &eem_entry, CONFIGURATIONf,
                flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
        if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
            soc_DLB_ECMP_EEM_CONFIGURATIONm_field32_set(unit,
                    &eem_entry, INNER_OUTER_ETHERTYPE_SELECTIONf,
                    flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
        }
        BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ALL, 0, &eem_entry));
    }
    /* Update Ethertype eligibility map table */
    for (i = 0; i < ethertype_count; i++) {
        sal_memset(&ethertype_entry, 0,
                sizeof(dlb_ecmp_ethertype_eligibility_map_entry_t));
        soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, VALIDf, 1);
        soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, ETHERTYPEf, ethertype_array[i] & 0xFFFF);
        BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, i, &ethertype_entry));
    }

    /* Zero out remaining entries of Ethertype eligibility map table */
    for (j = i; j < soc_mem_index_count(unit,
                DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm); j++) {
        BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, j, soc_mem_entry_null(unit,
                        DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_l3_egress_ecmp_dlb_ethertype_get
 * Purpose:
 *      Get the Ethertypes that are eligible or ineligible for
 *      ECMP dynamic load balancing.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      flags           - (IN/OUT) BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_max   - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (OUT) Array of Ethertypes.
 *      ethertype_count - (OUT) Number of elements returned in ethertype_array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_egress_ecmp_dlb_ethertype_get(
    int unit,
    uint32 *flags,
    int ethertype_max,
    int *ethertype_array,
    int *ethertype_count)
{
    int i;
    int ethertype;
    dlb_ecmp_ethertype_eligibility_map_entry_t ethertype_entry;
    dlb_ecmp_eem_configuration_entry_t eem_entry;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 reg_val = 0;
#endif

    /* Input check */
    if (NULL == flags) {
        return BCM_E_PARAM;
    }
    if ((ethertype_max > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (NULL == ethertype_count) {
        return BCM_E_PARAM;
    }

    *ethertype_count = 0;

    /* Get flags */
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_dlb)) {
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    &reg_val));
        if (soc_reg_field_get(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                    reg_val, CONFIGURATIONf)) {
            *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE;
        }
        if (soc_reg_field_get(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                    reg_val, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
            *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER;
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ANY, 0, &eem_entry));
        if (soc_DLB_ECMP_EEM_CONFIGURATIONm_field32_get(unit,
                    &eem_entry, CONFIGURATIONf)) {
            *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE;
        }
        if (!soc_feature(unit, soc_feature_th3_style_dlb)) {
            if (soc_DLB_ECMP_EEM_CONFIGURATIONm_field32_get(unit,
                        &eem_entry, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
                *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER;
            }
        }
    }
    /* Get Ethertypes */
    for (i = 0; i < soc_mem_index_count(unit,
                DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm); i++) {
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ANY, i, &ethertype_entry));
        if (soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, VALIDf)) {
            ethertype = soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, ETHERTYPEf);
            if (NULL != ethertype_array) {
                ethertype_array[*ethertype_count] = ethertype;
            }
            (*ethertype_count)++;
            if ((ethertype_max > 0) && (*ethertype_count == ethertype_max)) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_flowset_reset
 * Purpose:
 *      Invalidate matched  flowset entry when deleting ecmp member
 *      so that they can be given new assignments by DLB
 * Parameters:
 *      unit               - (IN) bcm device.
 *      ecmp               - (IN) ECMP group info.
 *      ecmp_member_bitmap - (IN/OUT) Offset of deleted ecmp member.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_l3_ecmp_dlb_flowset_reset(int unit,
                                  bcm_l3_egress_ecmp_t *ecmp,
                                  SHR_BITDCL *ecmp_member_bitmap)
{
    int       rv = BCM_E_NONE;
    int       ecmp_group_idx = 0;
    int       i = 0;
    int       dlb_id = 0;
    int       group_enable = 0;
    int       entry_base_ptr = 0;
    int       alloc_size = 0;
    int       index_min = 0;
    int       index_max = 0;
    int       assigned_member = 0;
    int       pipe = 0;
    dlb_ecmp_group_control_entry_t   dlb_ecmp_group_control_entry;
    dlb_ecmp_flowset_entry_t         flowset_entry;
    ecmp_dlb_control_entry_t         ecmp_group_entry;
    dlb_ecmp_flowset_member_entry_t *flowset_member_ptr = NULL;
    uint32                          *flowset_member_buf = NULL;
    soc_mem_t                        ecmp_flowset_member;
    soc_mem_t                        ecmp_flowset;

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) {
        return rv;
    }

    sal_memcpy(&flowset_entry, soc_mem_entry_null(unit, DLB_ECMP_FLOWSETm),
               sizeof(dlb_ecmp_flowset_entry_t));

    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        uint64 dlb_en_bitmap;
        dlb_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp_group_idx);
        if (dlb_id < soc_reg_field_length(unit,
                     DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
            SOC_IF_ERROR_RETURN( \
              READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
            group_enable = COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id);
        } else {
            rv = READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap);
            if (rv < 0) {
                goto error;
            }
            group_enable = COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id -
                                   (soc_reg_field_length(unit,
                                   DLB_ID_0_TO_63_ENABLEr, BITMAPf)));
        }
    } else
#endif
    {
        SOC_IF_ERROR_RETURN( \
            READ_ECMP_DLB_CONTROLm(unit, MEM_BLOCK_ANY,
                                   ecmp_group_idx, &ecmp_group_entry));
        group_enable =
            soc_ECMP_DLB_CONTROLm_field32_get(unit,
                                              &ecmp_group_entry, GROUP_ENABLEf);
        dlb_id =
            soc_ECMP_DLB_CONTROLm_field32_get(unit, &ecmp_group_entry, DLB_IDf);
    }
    if (!group_enable) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(
        READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY,
                                     dlb_id, &dlb_ecmp_group_control_entry));

    entry_base_ptr =
        soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                                                &dlb_ecmp_group_control_entry,
                                                FLOW_SET_BASEf);

    alloc_size = sizeof(dlb_ecmp_flowset_member_entry_t) * (ecmp->dynamic_size);
    flowset_member_buf = soc_cm_salloc(unit, alloc_size,
                                       "ECMP flowset member entries");
    if (NULL == flowset_member_buf) {
        rv = BCM_E_MEMORY;
        goto error;
    }


    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {

        ecmp_flowset_member =
            SOC_MEM_UNIQUE_ACC(unit, DLB_ECMP_FLOWSET_MEMBERm)[pipe];

        ecmp_flowset =
            SOC_MEM_UNIQUE_ACC(unit, DLB_ECMP_FLOWSETm)[pipe];

        index_min = entry_base_ptr;
        index_max = index_min + ecmp->dynamic_size - 1;
        rv = soc_mem_read_range(unit, ecmp_flowset_member, MEM_BLOCK_ANY,
                                index_min, index_max, flowset_member_buf);
        if (BCM_FAILURE(rv)) {
            goto error;
        }

        /*  Invalidate matched  flowset entry */
        for (i = 0; i < ecmp->dynamic_size; i++) {
            flowset_member_ptr =
                soc_mem_table_idx_to_pointer(unit, ecmp_flowset_member,
                                             dlb_ecmp_flowset_member_entry_t *,
                                             flowset_member_buf, i);
            assigned_member =
                soc_mem_field32_get(unit, ecmp_flowset_member,
                                    (uint32 *)flowset_member_ptr,
                                    ECMP_MEMBER_ASSIGNMENTf);
            if (soc_mem_field32_get(unit, ecmp_flowset_member,
                                    (uint32 *)flowset_member_ptr,
                                    ECMP_MEMBER_IS_ALTERNATEf)) {
                assigned_member = assigned_member + ecmp->max_paths / 2;
            }
            if (SHR_BITGET(ecmp_member_bitmap, assigned_member)) {

                rv = soc_mem_write(unit, ecmp_flowset,
                                   MEM_BLOCK_ALL, (i + entry_base_ptr),
                                   &flowset_entry);
                if (BCM_FAILURE(rv)) {
                    goto error;
                }
            }
        }

    }

    soc_cm_sfree(unit, flowset_member_buf);

    return BCM_E_NONE;

error:

    if (flowset_member_buf) {
        soc_cm_sfree(unit, flowset_member_buf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_member_delete
 * Purpose:
 *      Delete an Egress forwarding object from an DLB ECMP object
 * Parameters:
 *      unit         - (IN) bcm device.
 *      ecmp         - (IN) ECMP group info.
 *      intf         - (IN) Deleted L3 interface id.
 *      intf_array   - (IN/OUT) Array of Egress forwarding objects.
 *      intf_count   - (IN/OUT) Number of entries of intf_count actually filled in.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_member_delete(int unit,
                                  bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf,
                                  bcm_if_t *intf_array, int *intf_count)
{
    int       rv = BCM_E_NONE;
    int       ecmp_group_idx = 0;
    int       remove_member_found = FALSE;
    bcm_if_t *dlb_intf_array = NULL;
    int       i = 0, j = 0;
    int       dlb_member_count = 0; /* Exclude filler */
    int       member_bitmap_width = 0;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *delete_member_bitmap = NULL;
    SHR_BITDCL *group_member_ptr = NULL;


    /* Check count*/
    if (*intf_count == 0) {
        rv = BCM_E_NOT_FOUND;
        goto clean_up;
    } else if (*intf_count != ecmp->max_paths) {
        /* Should always has max_paths members on expanded DLB group */
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    /* Allocate DLB member bitmap */
    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;
    member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                              "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate Deleted DLB member bitmap */
    delete_member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                                     "Deleted DLB ECMP member bitmap");
    if (NULL == delete_member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(delete_member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate DLB member array */
    dlb_intf_array = sal_alloc((*intf_count) * sizeof(bcm_if_t),
                               "ecmp member array");
    if (NULL == dlb_intf_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    /* Get currrent group bitmap */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    group_member_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
    sal_memcpy(member_bitmap, group_member_ptr,
               SHR_BITALLOCSIZE(member_bitmap_width));

    /*
        * Get the DLB members (excluding deleted member and filler members)
        * and Clear DLB member bitmap
        */
    for (i = 0; i < ecmp->max_paths; i++) {
         if (SHR_BITGET(member_bitmap, i)) {
             if (!remove_member_found && (intf_array[i] == intf)) {
                 SHR_BITCLR(member_bitmap, i);
                 SHR_BITSET(delete_member_bitmap, i);
                 remove_member_found = TRUE;
             } else {
                 dlb_intf_array[dlb_member_count++] = intf_array[i];
             }
         }
    }

    if (!remove_member_found) {
        rv = BCM_E_NOT_FOUND;
        goto clean_up;
    }

    /* Refill the location of deleted members with existing members randomly */
    for (i = 0; i < ecmp->max_paths; i++) {
        if (dlb_member_count > 0) {
             /* Just reset filler*/
            if (!SHR_BITGET(member_bitmap, i)) {
                intf_array[i] =
                        dlb_intf_array[j % dlb_member_count];
                j++;
            }
        } else {
            /* All the DLB members are deleted */
            intf_array[i] = 0;
            (*intf_count)--;
        }
    }

    /* Update ECMP group */
    ecmp->flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;
    rv = _bcm_esw_l3_ecmp_create(unit, ecmp, *intf_count, intf_array,
                                 BCM_L3_ECMP_MEMBER_OP_DELETE,
                                 1, &intf, *intf_count, member_bitmap);
    if (BCM_SUCCESS(rv)) {
        sal_memcpy(group_member_ptr, member_bitmap,
                   SHR_BITALLOCSIZE(member_bitmap_width));
        rv = bcm_th2_l3_ecmp_dlb_flowset_reset(unit, ecmp,
                                               delete_member_bitmap);
    }

clean_up:

    if (member_bitmap) {
        sal_free(member_bitmap);
    }

    if (delete_member_bitmap) {
        sal_free(delete_member_bitmap);
    }

    if (dlb_intf_array) {
        sal_free(dlb_intf_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_dgm_member_delete
 * Purpose:
 *      Delete an Egress forwarding object from an DLB/DGM ECMP object
 * Parameters:
 *      unit                - (IN) bcm device.
 *      ecmp                - (IN) ECMP group info.
 *      ecmp_member         - (IN) Deleted L3 interface id pointing to Egress forwarding object.
 *      ecmp_member_array   - (IN/OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count   - (IN/OUT) Number of members of ecmp_member_count actually filled in.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_dgm_member_delete(int unit,
                                      bcm_l3_egress_ecmp_t *ecmp,
                                      bcm_l3_ecmp_member_t *ecmp_member,
                                      bcm_l3_ecmp_member_t *ecmp_member_array,
                                      int *ecmp_member_count)
{
    int rv = BCM_E_NONE;
    int alloc_size = 0;
    bcm_if_t *intf_array = 0;
    int ecmp_group_idx = 0;
    int start = 0, end = 0;
    int i = 0, j = 0;
    int location_offset = 0;
    int copy_offset = 0;
    int remove_member_found = FALSE;
    int alternate_count = 0;   /* Include filler */
    int primary_count = 0;     /* Include filler */
    int dlb_member_count = 0;  /* Exclude filler */
    int member_bitmap_width = 0;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *delete_member_bitmap = NULL;
    SHR_BITDCL *group_member_ptr = NULL;
    bcm_l3_ecmp_member_t *dlb_member_array = NULL; /* Exclude filler */


    /* Allocate DLB member bitmap */
    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;
    member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                               "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
       rv = BCM_E_MEMORY;
       goto clean_up;
    }

    sal_memset(member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate Deleted DLB member bitmap */
    delete_member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                                     "Deleted DLB ECMP member bitmap");
    if (NULL == delete_member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(delete_member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate DLB member array */
    dlb_member_array = sal_alloc((*ecmp_member_count) *
                                 sizeof(bcm_l3_ecmp_member_t),
                                 "ecmp member array");
    if (NULL == dlb_member_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;

    }

    /* Get currrent group bitmap */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    group_member_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
    sal_memcpy(member_bitmap, group_member_ptr,
               SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate interface array */
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    intf_array = sal_alloc(alloc_size, "intf array");
    if (NULL == intf_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;

    }
    sal_memset(intf_array, 0, alloc_size);

    /*
     * Index of ecmp_member_array
     * - Both primary and alternate members exist: Primary members start
     *    from 0 and Alternate members start from max_paths / 2
     * - Only primary members exist: Primary members start from 0
     * - Only alternate members exist: Alternate members start from 0
     * Offset of DLB members bitmap
     * - Primary members always start from 0
     * - Alternate members always start from max_paths / 2
     * Member count (including filler)
     * - DLB count: 0 or max_paths if any
     * - DGM primary count: 0 or max_paths/2 if any
     *    DGM alternate count: 0 or max_paths/2 if any
     */
    if ((ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) &&
        (ecmp_member->flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE)) {
        /* Delete alternate member */
        start = ecmp->max_paths / 2;
        end = start * 2;

        /* Get the count of  alternate members  (without filler) */
        SHR_BITCOUNT_RANGE(group_member_ptr, dlb_member_count,
                           start, end - start);

        if (dlb_member_count == 0) {
            rv = BCM_E_NOT_FOUND;
            goto clean_up;
        } else {
            alternate_count = ecmp->max_paths / 2;
            primary_count = (*ecmp_member_count) - alternate_count;
        }

        /*
         *  Only alternate members exist:
         *  Alternate members start from 0, offset should be max_paths / 2.
         */
        if (0 == primary_count) {
            location_offset = ecmp->max_paths / 2;
        }

        /* Internal check */
        if (end - location_offset > (*ecmp_member_count)) {
            rv = BCM_E_INTERNAL;
            goto clean_up;
        }
    } else {
        /* Delete DGM primary member or DLB member */
        start = 0;
        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            end = ecmp->max_paths / 2;
        } else {
            end = ecmp->max_paths;
        }

        /* Get the count of  DGM primary members or DLB members (without filler) */
        SHR_BITCOUNT_RANGE(group_member_ptr, dlb_member_count, start, end);

        if (dlb_member_count == 0) {
            rv = BCM_E_NOT_FOUND;
            goto clean_up;
        }

        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            primary_count = ecmp->max_paths / 2;
            alternate_count = (*ecmp_member_count) - primary_count;
            /*
             * If delete the lastest primary member, alternate members shoud be
             * moved,  and start from 0 instead of max_paths / 2.
             */
            if (alternate_count > 0) {
                copy_offset =  ecmp->max_paths / 2;
            }
        } else {
            primary_count = ecmp->max_paths;
        }

        /* Internal check */
        if (end + copy_offset > (*ecmp_member_count)) {
            rv = BCM_E_INTERNAL;
            goto clean_up;
        }
    }

    /*
     * Get the DLB members (excluding deleted member and filler members)
     * and Clear DLB member bitmap
     */
    dlb_member_count = 0;
    for (i = start - location_offset; i < end - location_offset; i++) {

         if (SHR_BITGET(member_bitmap, i + location_offset)) {
             if (!remove_member_found &&
                 (ecmp_member_array[i].egress_if == ecmp_member->egress_if)) {

                 SHR_BITCLR(member_bitmap, i + location_offset);
                 SHR_BITSET(delete_member_bitmap, i + location_offset);
                 remove_member_found  = TRUE;
             } else {
                 sal_memcpy(&dlb_member_array[dlb_member_count++],
                            &ecmp_member_array[i],
                            sizeof(bcm_l3_ecmp_member_t));
             }
         }
    }

    if (!remove_member_found) {
        rv = BCM_E_NOT_FOUND;
        goto clean_up;
    }

    /* Refill the location of deleted members with existing members randomly */
    for (i = start - location_offset; i < end - location_offset; i++) {
        if (dlb_member_count > 0) {
            if (!SHR_BITGET(member_bitmap, i + location_offset)) {
                sal_memcpy(&ecmp_member_array[i],
                           &dlb_member_array[j % dlb_member_count],
                           sizeof(bcm_l3_ecmp_member_t));
                j++;
            }
        } else {
            /*
             * 1. All the DLB members are deleted
             * 2. or all the DGM primary members are deleted
             * 3. or all the DGM alternate members are deleted
             */
            if (copy_offset) {
                /*
                 * DGM - if delete the last primary member, alternate members
                 * shoud be moved,  and start from 0 instead of max_paths / 2
                 */
                sal_memcpy(&ecmp_member_array[i],
                           &ecmp_member_array[i + copy_offset],
                           sizeof(bcm_l3_ecmp_member_t));
                sal_memset(&ecmp_member_array[i + copy_offset], 0,
                           sizeof(bcm_l3_ecmp_member_t));
            } else {
                sal_memset(&ecmp_member_array[i], 0,
                           sizeof(bcm_l3_ecmp_member_t));
            }
            (*ecmp_member_count)--;
            if (!(ecmp_member->flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE)) {
                primary_count--;
            }
        }
    }

    for (i = 0; i < (*ecmp_member_count); i++) {
        intf_array[i] = ecmp_member_array[i].egress_if;
    }

    /* Update ECMP group */
    ecmp->flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;
    rv = _bcm_esw_l3_ecmp_create(unit, ecmp, *ecmp_member_count,
                                 intf_array, BCM_L3_ECMP_MEMBER_OP_DELETE,
                                 1, &(ecmp_member->egress_if),
                                 primary_count, member_bitmap);

    if (BCM_SUCCESS(rv)) {
        sal_memcpy(group_member_ptr, member_bitmap,
                   SHR_BITALLOCSIZE(member_bitmap_width));
        rv = bcm_th2_l3_ecmp_dlb_flowset_reset(unit, ecmp,
                                               delete_member_bitmap);
    }

clean_up:

    if (member_bitmap) {
        sal_free(member_bitmap);
    }

    if (delete_member_bitmap) {
        sal_free(delete_member_bitmap);
    }

    if (dlb_member_array) {
        sal_free(dlb_member_array);
    }

    if (intf_array) {
        sal_free(intf_array);
    }

    return rv;

}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_member_create
 * Purpose:
 *      Create or modify an Egress DLB ECMP forwarding object.
 * Parameters:
 *      unit         - (IN) bcm device.
 *      ecmp         - (IN) ECMP group info.
 *      intf_array   - (IN/OUT) Array of Egress forwarding objects.
 *      intf_count   - (IN/OUT) Number of entries of intf_count actually filled in.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_member_create(int unit,
                                  bcm_l3_egress_ecmp_t *ecmp,
                                  bcm_if_t *intf_array,
                                  int intf_count)
{
    int       rv = BCM_E_NONE;
    int       i = 0, j = 0;
    int       alloc_size = 0;
    int       ecmp_group_idx = 0;
    bcm_if_t *dlb_intf_array = NULL;   /* Include filler*/
    bcm_if_t *dlb_intf_array_2 = NULL; /* Exclude filler */
    int       dlb_intf_count = 0;
    int       dlb_intf_count_2 = 0;
    int       member_chg = TRUE;
    bcm_l3_egress_ecmp_t  dlb_ecmp;
    int                   member_bitmap_width = 0;
    SHR_BITDCL           *member_bitmap = NULL;
    SHR_BITDCL           *group_member_ptr = NULL;

    /* Check max_paths */
    if ((ecmp->max_paths > BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit)) ||
        (ecmp->max_paths < 2)) {
        return BCM_E_PARAM;
    } else if (ecmp->max_paths == 0) {
        ecmp->max_paths = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit);
    }

    if (intf_count > ecmp->max_paths) {
        return BCM_E_RESOURCE;
    }

    /* Allocate interface array */
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    dlb_intf_array = sal_alloc(alloc_size, "dlb intf array");
    if (NULL == dlb_intf_array) {
        return BCM_E_MEMORY;
    }

    sal_memset(dlb_intf_array, 0, alloc_size);

    /* Allocate DLB member bitmap */
    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;
    member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                              "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    if (ecmp->flags & BCM_L3_REPLACE) {
        if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
            rv = BCM_E_PARAM;
            goto clean_up;
        }

        /* Get currrent group bitmap */
        ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        group_member_ptr =
            BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);

        /* Get DLB group's existing interface array including filler */
        bcm_l3_egress_ecmp_t_init(&dlb_ecmp);
        dlb_ecmp.ecmp_intf = ecmp->ecmp_intf;
        rv = _bcm_esw_l3_egress_ecmp_get(unit, &dlb_ecmp,
                                         BCM_XGS3_L3_ECMP_MAX(unit),
                                         dlb_intf_array, &dlb_intf_count, TRUE);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        if (dlb_ecmp.dynamic_mode == ecmp->dynamic_mode) {

            if (dlb_ecmp.max_paths != ecmp->max_paths) {
                rv = BCM_E_PARAM;
                goto clean_up;
            }

            /* Get DLB group's existing interface array excluding filler */
            alloc_size = sizeof(bcm_if_t) * dlb_intf_count;
            dlb_intf_array_2 = sal_alloc(alloc_size, "dlb intf array_2");
            if (NULL == dlb_intf_array_2) {
                rv = BCM_E_MEMORY;
                goto clean_up;
            }
            sal_memset(dlb_intf_array_2, 0, alloc_size);
            /* Exclude filler members of expanded DLB group */
            for (i = 0; i < dlb_intf_count; i++) {
                if (SHR_BITGET(group_member_ptr, i)) {
                    dlb_intf_array_2[dlb_intf_count_2++] = dlb_intf_array[i];
                }
            }

            /* Compare new members(intf_array) with exisiting members(dlb_intf_array_2) */
            if ((intf_count == dlb_intf_count_2) &&
                !sal_memcmp((void *)intf_array, (void *)dlb_intf_array_2,
                             (intf_count * sizeof(bcm_if_t)))) {
                member_chg = FALSE;
            }
        }
    }

    /*
     * To keep the traffic resiliency, the offset of DLB members in L3 ECMP tables should
     * not be changed when an DLB ECMP member is deleted or added.
     * - An expanded ecmp group will be used for DLB. SDK always allocates member entries
     *   based on the configuration of max_paths. i.e. DLB(max_paths entries).
     *   Up to max_paths next hops can be supported in DLB group. The DLB bitmap
     *   will always reflect the TRUE DLB membership(specified by SDK API),
     *   and the remaining entries will contain fillers which won't be selected by DLB.
     * - When an DLB ECMP member is deleted, the space previously occupied by the deleted
     *   member will be filled as a duplicate of any existing member.
     * - When an DLB ECMP member is added, SDK will choose the space used by a filler
     *   for the new member.
     */
    if (member_chg) {
        sal_memset(dlb_intf_array, 0,
                   sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit));
        if (intf_count) {
            for (i = 0; i < ecmp->max_paths; i++) {
                j = i % intf_count;
                dlb_intf_array[i] = intf_array[j];
                if (i < intf_count) {
                    SHR_BITSET(member_bitmap, i);
                }
            }
            /*  Should be expanded to max_paths with filler */
            dlb_intf_count = ecmp->max_paths;
        } else {
            dlb_intf_count = 0;
        }
    } else {
        /* No member change */
        sal_memcpy(member_bitmap, group_member_ptr,
                   SHR_BITALLOCSIZE(member_bitmap_width));
    }

    rv = _bcm_esw_l3_ecmp_create(unit, ecmp, dlb_intf_count,
                                 dlb_intf_array, BCM_L3_ECMP_MEMBER_OP_SET,
                                 0, NULL, dlb_intf_count, member_bitmap);
    if (member_chg && BCM_SUCCESS(rv)) {
        ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        group_member_ptr =
            BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
        sal_memcpy(group_member_ptr, member_bitmap,
                   SHR_BITALLOCSIZE(member_bitmap_width));
    }

clean_up:

    if (dlb_intf_array) {
        sal_free(dlb_intf_array);
    }

    if (member_bitmap) {
        sal_free(member_bitmap);
    }

    if (dlb_intf_array_2) {
        sal_free(dlb_intf_array_2);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_dgm_member_create
 * Purpose:
 *      Create or modify an Egress DLB/DGM ECMP forwarding object.
 * Parameters:
 *      unit              - (IN) bcm device.
 *      ecmp              - (IN) ECMP group info.
 *      ecmp_member_array - (IN) Member array of Egress forwarding objects.
 *      ecmp_member_count - (IN) Number of elements in ecmp_member_array.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_dgm_member_create(int unit,
                                      bcm_l3_egress_ecmp_t *ecmp,
                                      bcm_l3_ecmp_member_t *ecmp_member_array,
                                      int ecmp_member_count)
{
    int         rv = BCM_E_NONE;
    int         i = 0, j = 0;
    int         alloc_size = 0;
    int         ecmp_group_idx = 0;
    bcm_if_t   *dlb_intf_array = NULL;   /* Include filler */
    int         dlb_intf_count = 0;
    bcm_if_t   *dlb_intf_array_2 = NULL; /* Exclude filler */
    int         dlb_intf_count_2 = 0;
    int         grand_max_paths = 0;
    int         dlb_primary_count = 0;   /* Include filler */
    int         dlb_primary_count_2 = 0; /* Exclude filler */
    int         location_offset = 0;
    int         member_bitmap_width = 0;
    int         member_chg = TRUE;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *group_member_ptr = NULL;
    bcm_if_t   *intf_array = NULL;
    int         primary_count = 0;
    int         intf_count = 0;
    int         dlb_member_count = 0;
    bcm_l3_ecmp_member_t *dlb_member_array = NULL; /* Include filler */
    bcm_l3_egress_ecmp_t  dlb_ecmp;

    grand_max_paths =
        (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) ?
                               BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2 :
                               BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit);
    /* Check max_paths */
    if ((ecmp->max_paths > grand_max_paths) || (ecmp->max_paths < 2)) {
        return BCM_E_PARAM;
    } else if (ecmp->max_paths == 0) {
        ecmp->max_paths = grand_max_paths;
    }

    if (ecmp_member_count > ecmp->max_paths) {
        return BCM_E_RESOURCE;
    }

    /* Convert the ecmp_member_array to intf_array */
    intf_count = ecmp_member_count;
    alloc_size = BCM_XGS3_L3_ECMP_MAX(unit) * sizeof(bcm_if_t);
    intf_array = sal_alloc(alloc_size, "L3 ECMP intf_array");
    if (intf_array == NULL) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    sal_memset(intf_array, 0, alloc_size);

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
        for (i = 0; i < intf_count; i++) {
            if (!(ecmp_member_array[i].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE)) {
                intf_array[j++] = ecmp_member_array[i].egress_if;
                primary_count++;
            }
        }
        for (i = 0; i < intf_count; i++) {
            if (ecmp_member_array[i].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                intf_array[j++] = ecmp_member_array[i].egress_if;
            }
        }
        /* Either primary or alternate cannot exceed half  max paths */
        if (primary_count > (ecmp->max_paths >> 1) ||
            (intf_count - primary_count) > (ecmp->max_paths >> 1)) {
            rv = BCM_E_RESOURCE;
            goto clean_up;
        }
    } else {
        for (i = 0; i < intf_count; i++) {
            intf_array[i] = ecmp_member_array[i].egress_if;
        }
        primary_count = intf_count;
    }

    /* Allocate interface array */
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    dlb_intf_array = sal_alloc(alloc_size, "dlb intf array");
    if (NULL == dlb_intf_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    sal_memset(dlb_intf_array, 0, alloc_size);

    /* Allocate DLB member bitmap */
    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;
    member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                              "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    if (ecmp->flags & BCM_L3_REPLACE) {
        if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
            rv = BCM_E_PARAM;
            goto clean_up;
        }
        /* Get currrent group bitmap */
        ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        group_member_ptr =
            BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);

        /* Get DLB group's existing member array including filler  */
        bcm_l3_egress_ecmp_t_init(&dlb_ecmp);
        dlb_ecmp.ecmp_intf = ecmp->ecmp_intf;

        alloc_size = sizeof(bcm_l3_ecmp_member_t) * BCM_XGS3_L3_ECMP_MAX(unit);
        dlb_member_array = sal_alloc(alloc_size, "dlb intf array");
        if (NULL == dlb_member_array) {
            rv = BCM_E_MEMORY;
            goto clean_up;
        }
        sal_memset(dlb_member_array, 0, alloc_size);
        rv = bcm_xgs3_l3_ecmp_get(unit, &dlb_ecmp, BCM_XGS3_L3_ECMP_MAX(unit),
                                  dlb_member_array, &dlb_member_count);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        if (dlb_ecmp.dynamic_mode == ecmp->dynamic_mode) {
            /* Max_paths can not be changed for DLB/DGM group*/
            if (dlb_ecmp.max_paths != ecmp->max_paths) {
                rv = BCM_E_PARAM;
                goto clean_up;
            }

            /* Get DLB group's existing interface array including filler */
            for (i = 0; i < dlb_member_count; i++) {
                dlb_intf_array[i] = dlb_member_array[i].egress_if;
            }

            dlb_intf_count = dlb_member_count;

            /* Get DLB group's existing interface array excluding filler */
            alloc_size = sizeof(bcm_if_t) * dlb_member_count;

            dlb_intf_array_2 = sal_alloc(alloc_size, "dlb intf array_2");
            if (NULL == dlb_intf_array_2) {
                rv = BCM_E_MEMORY;
                goto clean_up;
            }

            sal_memset(dlb_intf_array_2, 0, alloc_size);

            if (dlb_ecmp.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
                SHR_BITCOUNT_RANGE(group_member_ptr, dlb_primary_count_2,
                                   0, dlb_ecmp.max_paths / 2);
                if (dlb_primary_count_2) {
                    /*  Should be expanded to max_paths / 2 with filler */
                    dlb_primary_count = dlb_ecmp.max_paths / 2;
                } else {
                    /* Only alternate members exist , start from 0 instead of max_paths / 2 */
                    location_offset = dlb_ecmp.max_paths / 2;
                }
            } else {
                SHR_BITCOUNT_RANGE(group_member_ptr, dlb_primary_count_2,
                                   0, dlb_ecmp.max_paths);
                if (dlb_primary_count_2) {
                     /*  Should be expanded to max_paths with filler */
                    dlb_primary_count = dlb_ecmp.max_paths;
                }
            }

            /* Exclude filler members of expanded DLB group */
            for (i = 0; i < dlb_member_count; i++) {
                if (SHR_BITGET(group_member_ptr, i + location_offset)) {
                    dlb_intf_array_2[dlb_intf_count_2++] =
                        dlb_member_array[i].egress_if;
                }
            }

            /* Compare new members(intf_array) with exisiting members(dlb_intf_array_2) */
            if ((intf_count == dlb_intf_count_2) &&
                (primary_count == dlb_primary_count_2) &&
                !sal_memcmp((void *)intf_array, (void *)dlb_intf_array_2,
                             (intf_count * sizeof(bcm_if_t)))) {
                member_chg = FALSE;
            }
        }
    }

    /*
     * To keep the traffic resiliency, the offset of DLB members in L3 ECMP tables should
     * not be changed when an DLB ECMP member is deleted or added.
     * - An expanded ecmp group will be used for DLB. SDK always allocates member entries
     *   based on the configuration of max_paths. i.e. DLB(max_paths entries) or
     *   DGM (max_paths/2 primary entries , max_paths/2 alternate entries).
     *   Up to max_paths next hops can be supported in DLB group.
     *   With DGM, it will support the groups with up to max_paths/2 primary
     *   next hops and up to max_paths/2 alternate next hops. The DLB bitmap will always
     *   reflect the TRUE DLB membership(specified by SDK API),
     *   and the remaining entries will contain fillers which won't be selected by DLB.
     * - When an DLB ECMP member is deleted, the space previously occupied by the deleted
     *   member will be filled as a duplicate of any existing member.
     * - When an DLB ECMP member is added, SDK will choose the space used by a filler
     *   for the new member.
     */
    if (member_chg) {
        sal_memset(dlb_intf_array, 0,
                   sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit));
        if (intf_count) {
            if ((0 == primary_count) &&
                (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
                for (i = 0; i < ecmp->max_paths / 2; i++) {
                    j = i % intf_count;
                    dlb_intf_array[i] = intf_array[j];
                    if (i < intf_count) {
                        SHR_BITSET(member_bitmap, i + ecmp->max_paths / 2);
                    }
                }
                /*  If only alternate exist, expand ecmp group to max_paths / 2 with filler */
                dlb_intf_count = ecmp->max_paths / 2;
                dlb_primary_count = 0;
            } else if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {

                for (i = 0; i < ecmp->max_paths / 2; i++) {
                    j = i % primary_count;
                    dlb_intf_array[i] = intf_array[j];
                    if (i < primary_count) {
                        SHR_BITSET(member_bitmap, i);
                    }
                }

                /*  If primary exist, expand ecmp group to max_paths / 2 with filler */
                dlb_intf_count = ecmp->max_paths / 2;
                dlb_primary_count = ecmp->max_paths / 2;

                if (intf_count - primary_count) {
                    for (i = 0; i < ecmp->max_paths / 2; i++) {
                        j = primary_count + i % (intf_count - primary_count);
                        dlb_intf_array[i + ecmp->max_paths / 2] = intf_array[j];
                        if (i < (intf_count - primary_count)) {
                            SHR_BITSET(member_bitmap, i + ecmp->max_paths / 2);
                        }
                    }

                    /*  If alternate also exist, expand ecmp group to max_paths with filler */
                    dlb_intf_count = dlb_intf_count + ecmp->max_paths / 2;
                }
            } else {
                for (i = 0; i < ecmp->max_paths; i++) {
                    j = i % intf_count;
                    dlb_intf_array[i] = intf_array[j];
                    if (i < intf_count) {
                        SHR_BITSET(member_bitmap, i);
                    }
                }

                /*  expand ecmp group  to max_paths with filler */
                dlb_intf_count = ecmp->max_paths;
                dlb_primary_count = ecmp->max_paths;
            }
        } else {
            dlb_intf_count = 0;
            dlb_primary_count = 0;
        }
    } else {
        sal_memcpy(member_bitmap, group_member_ptr,
                   SHR_BITALLOCSIZE(member_bitmap_width));
    }

    rv = _bcm_esw_l3_ecmp_create(unit, ecmp, dlb_intf_count,
                                 dlb_intf_array, BCM_L3_ECMP_MEMBER_OP_SET,
                                 0, NULL, dlb_primary_count, member_bitmap);
    if (member_chg && BCM_SUCCESS(rv)) {
        ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        group_member_ptr =
            BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
        sal_memcpy(group_member_ptr, member_bitmap,
                   SHR_BITALLOCSIZE(member_bitmap_width));
    }

clean_up:

    if (intf_array) {
        sal_free(intf_array);
    }

    if (dlb_intf_array) {
        sal_free(dlb_intf_array);
    }

    if (member_bitmap) {
        sal_free(member_bitmap);
    }

    if (dlb_member_array) {
        sal_free(dlb_member_array);
    }

    if (dlb_intf_array_2) {
        sal_free(dlb_intf_array_2);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_member_add
 * Purpose:
 *      Add an Egress forwarding object to an Egress DLB ECMP forwarding object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      ecmp        - (IN) ECMP group info.
 *      intf        - (IN) Deleted L3 interface id.
 *      intf_array  - (IN/OUT) Array of Egress forwarding objects.
 *      intf_count  - (IN/OUT) Number of entries of intf_count actually filled in.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_member_add(int unit,
                                  bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf,
                                  bcm_if_t *intf_array, int *intf_count)

{
    int rv = BCM_E_NONE;
    int ecmp_group_idx = 0;
    int i = 0, j = 0;
    int dlb_member_add_flag = FALSE;
    int dlb_member_count = 0;   /* Exclude filler */
    int member_bitmap_width = 0;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *group_member_ptr = NULL;
    bcm_if_t   *dlb_intf_array = NULL; /* Exclude filler */

    /* Allocate DLB member bitmap */
    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;
    member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                              "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    /* Get currrent group bitmap */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    group_member_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);

    if ((*intf_count != ecmp->max_paths) && (*intf_count != 0)) {
        /* Should always has max_paths or 0 members on expanded DLB group */
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    sal_memcpy(member_bitmap, group_member_ptr,
               SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate DLB intf array */
    dlb_intf_array = sal_alloc(BCM_XGS3_L3_ECMP_MAX(unit) *
                               sizeof(bcm_if_t),
                               "ecmp intf array");
    if (NULL == dlb_intf_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    /* Get the DLB intf (including added intf) without filler */
    dlb_member_count = 0;
    for (i = 0; i < ecmp->max_paths; i++) {
        if (SHR_BITGET(member_bitmap, i)) {
            dlb_intf_array[dlb_member_count++] = intf_array[i];
        } else {
            if(!dlb_member_add_flag) {
                dlb_intf_array[dlb_member_count++] = intf;
                dlb_member_add_flag = TRUE;
            }
        }
    }

    if (!dlb_member_add_flag) {
        rv = BCM_E_FULL;
        goto clean_up;
    }

    /* Refill the members of expanded DLB group */
    dlb_member_add_flag = FALSE;
    for (i = 0; i < ecmp->max_paths; i++) {
        if (dlb_member_count > 1) {
            if (!SHR_BITGET(member_bitmap, i)) {
                if (dlb_member_add_flag) {
                    intf_array[i] = dlb_intf_array[j % dlb_member_count];
                    j++;
                } else {
                    SHR_BITSET(member_bitmap, i);
                    intf_array[i] = intf;
                    dlb_member_add_flag = TRUE;
                }
            }
        } else {
            /* Add first primary member into DLB group */
            intf_array[i] = intf;
            if (!dlb_member_add_flag) {
                SHR_BITSET(member_bitmap, i);
                dlb_member_add_flag = TRUE;
            }
            (*intf_count)++;
        }
    }

    if (!dlb_member_add_flag) {
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    /* Update ECMP group */
    ecmp->flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;
    rv = _bcm_esw_l3_ecmp_create(unit, ecmp, *intf_count,
                                 intf_array, BCM_L3_ECMP_MEMBER_OP_ADD,
                                 1, &intf, *intf_count, member_bitmap);
    if (BCM_SUCCESS(rv)) {
        sal_memcpy(group_member_ptr, member_bitmap,
                   SHR_BITALLOCSIZE(member_bitmap_width));
    }

clean_up:

    if (member_bitmap) {
        sal_free(member_bitmap);
    }

    if (dlb_intf_array) {
        sal_free(dlb_intf_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_dgm_member_add
 * Purpose:
 *      Add an Egress forwarding object to an Egress DLB/DGM ECMP forwarding object.
 * Parameters:
 *      unit              - (IN) bcm device.
 *      ecmp              - (IN) ECMP group info.
 *      ecmp_member       - (IN) Deleted L3 interface id pointing to Egress forwarding object.
 *      ecmp_member_array - (IN/OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (IN/OUT) Number of members of ecmp_member_count actually  filled in.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_dgm_member_add(int unit,
                                   bcm_l3_egress_ecmp_t *ecmp,
                                   bcm_l3_ecmp_member_t *ecmp_member,
                                   bcm_l3_ecmp_member_t *ecmp_member_array,
                                   int *ecmp_member_count)
{
    int rv = BCM_E_NONE;
    int alloc_size = 0;
    bcm_if_t *intf_array = 0;      /* Include filler */
    bcm_l3_ecmp_member_t *dlb_member_array = 0; /* Exclude filler */
    int ecmp_group_idx = 0;
    int dlb_member_add_flag = FALSE;
    int dlb_member_count = 0;   /* Exclude filler */
    int alternate_count = 0;   /* Include filler */
    int primary_count = 0;     /* Include filler */
    int start = 0, end = 0;
    int i = 0, j = 0;
    int location_offset = 0, copy_offset = 0;
    int member_bitmap_width = 0;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *group_member_ptr = NULL;

    /* Allocate DLB member bitmap */
    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;
    member_bitmap = sal_alloc(SHR_BITALLOCSIZE(member_bitmap_width),
                              "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    sal_memset(member_bitmap, 0, SHR_BITALLOCSIZE(member_bitmap_width));

    /* Get currrent group bitmap */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    group_member_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);
    SHR_BITCOUNT_RANGE(group_member_ptr, dlb_member_count, 0, ecmp->max_paths);

    sal_memcpy(member_bitmap, group_member_ptr,
               SHR_BITALLOCSIZE(member_bitmap_width));

    /* Allocate interface array */
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    intf_array = sal_alloc(alloc_size, "intf array");
    if (NULL == intf_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;

    }
    sal_memset(intf_array, 0, alloc_size);

    /*
     * Index of ecmp_member_array
     * - Both primary and alternate members exist: Primary members start
     *    from 0 and Alternate members start from max_paths / 2
     * - Only primary members exist: Primary members start from 0
     * - Only alternate members exist: Alternate members start from 0
     * Offset of DLB members bitmap
     * - Primary members always start from 0
     * - Alternate members always start from max_paths / 2
     * Member count (including filler)
     * - DLB count: 0 or max_paths if exist
     * - DGM primary count: 0 or max_paths/2 if exsit
     *   DGM alternate count: 0 or max_paths/2 if exist
     */
    if ((ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) &&
        (ecmp_member->flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE)) {
        /* Add alternate member */
        start = ecmp->max_paths / 2;
        end = start * 2;

        /* Get the count of  alternate members (without filler) */
        SHR_BITCOUNT_RANGE(group_member_ptr, dlb_member_count,
                           start, end - start);
        alternate_count =
            dlb_member_count ? ecmp->max_paths / 2 : 0;
        primary_count = (*ecmp_member_count) - alternate_count;

        /*
         *  Only alternate members exist:
         *  Alternate members start from 0, there should a location offset.
         */
        if (0 == primary_count) {
            location_offset = ecmp->max_paths / 2;
        }

    } else {
        /* Add primary member */
        start = 0;
        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            end = ecmp->max_paths / 2;
        } else {
            end = ecmp->max_paths;
        }

        /* Get the count of  primary members (without filler) */
        SHR_BITCOUNT_RANGE(group_member_ptr, dlb_member_count, start, end);

        primary_count = dlb_member_count ? end : 0;
        alternate_count = (*ecmp_member_count) - primary_count;

        if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            /*
             * Add first primary member - Alternate members shoud be moved,
             * and start from max_paths / 2 if any.
             */
            if (alternate_count > 0) {
                copy_offset =  ecmp->max_paths / 2;
            }
        }
    }

    /* Allocate DLB member array */
    dlb_member_array = sal_alloc(BCM_XGS3_L3_ECMP_MAX(unit) *
                                 sizeof(bcm_l3_ecmp_member_t),
                                 "ecmp member array");
    if (NULL == dlb_member_array) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    /* Get the DLB members (including added member) without filler */
    dlb_member_count = 0;
    for (i = start - location_offset; i < end - location_offset; i++) {
        if (SHR_BITGET(member_bitmap, i + location_offset)) {
            sal_memcpy(&dlb_member_array[dlb_member_count++],
                       &ecmp_member_array[i],
                       sizeof(bcm_l3_ecmp_member_t));
        } else {
            if(!dlb_member_add_flag) {
                sal_memcpy(&dlb_member_array[dlb_member_count++],
                           ecmp_member,
                           sizeof(bcm_l3_ecmp_member_t));
                dlb_member_add_flag = TRUE;
            }
        }
    }

    if (!dlb_member_add_flag) {
        rv = BCM_E_FULL;
        goto clean_up;
    }

    /* Refill the members of expanded DLB group */
    dlb_member_add_flag = FALSE;
    for (i = start - location_offset; i < end - location_offset; i++) {
        if (dlb_member_count > 1) {
            if (!SHR_BITGET(member_bitmap, i + location_offset)) {
                if (dlb_member_add_flag) {
                    sal_memcpy(&ecmp_member_array[i],
                               &dlb_member_array[j % dlb_member_count],
                               sizeof(bcm_l3_ecmp_member_t));
                    j++;
                } else {
                    SHR_BITSET(member_bitmap, i + location_offset);
                    sal_memcpy(&ecmp_member_array[i], ecmp_member,
                               sizeof(bcm_l3_ecmp_member_t));
                    dlb_member_add_flag = TRUE;
                }
            }
        } else {
            /*
             * 1. Add first DLB member
             * 2. or Add first DGM primary member
             * 3. or Add first DGM alternate member
             */
            if (copy_offset) {
               /*
                * DGM - Add the first primary member -
                * Alternate members shoud be moved,
                * and start from max_paths / 2 instead of 0.
                */
                sal_memcpy(&ecmp_member_array[i + copy_offset],
                           &ecmp_member_array[i],
                           sizeof(bcm_l3_ecmp_member_t));
            }
            sal_memcpy(&ecmp_member_array[i], ecmp_member,
                       sizeof(bcm_l3_ecmp_member_t));

            if (!dlb_member_add_flag) {
                SHR_BITSET(member_bitmap, i + location_offset);
                dlb_member_add_flag = TRUE;
            }

            (*ecmp_member_count)++;
            if (!(ecmp_member->flags &
                BCM_L3_ECMP_MEMBER_DGM_ALTERNATE)) {
                primary_count++;
            }
        }
    }

    for (i = 0; i < (*ecmp_member_count); i++) {
        intf_array[i] = ecmp_member_array[i].egress_if;
    }

    /* Update ECMP group */
    ecmp->flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;
    rv = _bcm_esw_l3_ecmp_create(unit, ecmp, *ecmp_member_count,
                                 intf_array, BCM_L3_ECMP_MEMBER_OP_ADD,
                                 1, &(ecmp_member->egress_if),
                                 primary_count, member_bitmap);
    if (BCM_SUCCESS(rv)) {
        sal_memcpy(group_member_ptr, member_bitmap,
                   SHR_BITALLOCSIZE(member_bitmap_width));
    }

clean_up:

     if (member_bitmap) {
         sal_free(member_bitmap);
     }

     if (intf_array) {
         sal_free(intf_array);
     }

     if (dlb_member_array) {
         sal_free(dlb_member_array);
     }

     return rv;
}

/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_member_get
 * Purpose:
 *      Get an Egress DLB ECMP forwarding object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_size  - (IN) Size of allocated entries in intf_array.
 *      intf_array - (OUT) Array of Egress forwarding objects.
 *      intf_count - (OUT) Number of entries of intf_count actually filled in.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_member_get(int unit,
                               bcm_l3_egress_ecmp_t *ecmp, int intf_size,
                               bcm_if_t *intf_array, int *intf_count)
{
    int         rv = BCM_E_NONE;
    bcm_if_t   *dlb_intf_array = NULL;  /* Include filler */
    int         dlb_intf_count = 0;     /* Include filler */
    int         dlb_intf_size = 0;
    int         ecmp_group_idx = 0;
    int         i = 0, j = 0;
    SHR_BITDCL *group_bitmap_ptr = NULL;

    /* input check*/
    if ((NULL == intf_count) ||
        ((NULL == intf_array) && (0 != intf_size))) {
        rv = BCM_E_PARAM;
        goto clean_up;
    }

    if (intf_size) {
        dlb_intf_array = sal_alloc(sizeof(bcm_if_t) *
                                   BCM_XGS3_L3_ECMP_MAX(unit),
                                   "ecmp intf array");
        if (NULL == dlb_intf_array) {
            rv = BCM_E_MEMORY;
            goto clean_up;
        }
        dlb_intf_size = BCM_XGS3_L3_ECMP_MAX(unit);
    }

    /* Get the ECMP member's information including filler  */
    rv = _bcm_esw_l3_egress_ecmp_get(unit, ecmp, dlb_intf_size,
                                     dlb_intf_array, &dlb_intf_count, TRUE);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    group_bitmap_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);

    /* Exclude filler members from expanded DLB group */
    if (intf_size) {
        for (i = 0; i < dlb_intf_count; i++) {
            if (SHR_BITGET(group_bitmap_ptr, i)) {
                intf_array[j++] = dlb_intf_array[i];
            }
            if (j == intf_size) {
                break;
            }
        }
        *intf_count = j;
    } else {
        SHR_BITCOUNT_RANGE(group_bitmap_ptr, *intf_count, 0, ecmp->max_paths);
    }

clean_up:

    if (dlb_intf_array) {
        sal_free(dlb_intf_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_th2_l3_ecmp_dlb_dgm_member_get
 * Purpose:
 *      Get an Egress DLB/DGM ECMP forwarding object.
 * Parameters:
 *      unit              - (IN) bcm device.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_size  - (IN) Size of allocated entries in ecmp_member_array.
 *      ecmp_member_array - (OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (OUT) Number of members of ecmp_member_count actually
 *                                filled in.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_l3_ecmp_dlb_dgm_member_get(int unit,
                                   bcm_l3_egress_ecmp_t *ecmp_info,
                                   int ecmp_member_size,
                                   bcm_l3_ecmp_member_t *ecmp_member_array,
                                   int *ecmp_member_count)
{
    int         rv = BCM_E_NONE;
    int         ecmp_group_idx = 0;
    int         location_offset = 0;
    int         primary_count = 0;
    int         i = 0, j = 0;
    SHR_BITDCL *group_bitmap_ptr = NULL;
    int         dlb_member_count = 0;     /* Include filler */
    int         dlb_member_size = 0;
    bcm_l3_ecmp_member_t *dlb_member_array = NULL;  /* Include filler */

    /* Input check */
    if ((NULL == ecmp_member_count) ||
        ((NULL == ecmp_member_array) && (0 != ecmp_member_size))) {
        rv = BCM_E_PARAM;
        goto clean_up;
    }

    if (ecmp_member_size) {
        dlb_member_array = sal_alloc(sizeof(bcm_l3_ecmp_member_t) *
                                     BCM_XGS3_L3_ECMP_MAX(unit),
                                     "ecmp member array");
        if (NULL == dlb_member_array) {
            rv = BCM_E_MEMORY;
            goto clean_up;
        }
        sal_memset(dlb_member_array, 0, sizeof(bcm_l3_ecmp_member_t) *
                   BCM_XGS3_L3_ECMP_MAX(unit));
        dlb_member_size = BCM_XGS3_L3_ECMP_MAX(unit);
    }

    /* Get the ECMP member's information including filler  */
    rv = _bcm_xgs3_l3_ecmp_member_get(
             unit, ecmp_info, dlb_member_size,
             dlb_member_array, &dlb_member_count);

    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    ecmp_group_idx = ecmp_info->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    group_bitmap_ptr =
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, ecmp_group_idx);

    if (ecmp_member_size) {
        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {

            SHR_BITCOUNT_RANGE(group_bitmap_ptr, primary_count,
                               0, ecmp_info->max_paths / 2);

            /* Only alternate members exist */
            if (0 == primary_count) {
                location_offset = ecmp_info->max_paths / 2;
            }
        }

        /* Exclude filler members from expanded DLB group */
        for (i = 0; i < dlb_member_count; i++) {
            if (SHR_BITGET(group_bitmap_ptr, i + location_offset)) {
                sal_memcpy(&ecmp_member_array[j++],
                           &dlb_member_array[i],
                           sizeof(bcm_l3_ecmp_member_t));
            }
            if (j == ecmp_member_size) {
                break;
            }
        }
        *ecmp_member_count = j;
    } else {
        SHR_BITCOUNT_RANGE(group_bitmap_ptr, *ecmp_member_count,
                           0, ecmp_info->max_paths);
    }

clean_up:

    if (dlb_member_array) {
        sal_free(dlb_member_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_random_seed_set
 * Purpose:
 *      Set ECMP DLB random seed.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      seed - (IN) Random seed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_random_seed_set(int unit, int seed)
{

    uint32 rval;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_ECMP_RANDOM_SELECTION_CONTROLr,
                                          SEEDf)) - 1;
    if (seed < 0 ||
        seed > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_RANDOM_SELECTION_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, DLB_ECMP_RANDOM_SELECTION_CONTROLr, &rval, SEEDf,
                      seed);

    BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_RANDOM_SELECTION_CONTROLr(unit, rval));
    return BCM_E_NONE;
}




/*
 * Function:
 *      _bcm_th2_ecmp_dlb_random_seed_get
 * Purpose:
 *      Set ECMP DLB random seed.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 *      seed - (OUT) Random seed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_random_seed_get(int unit, int *seed)
{

    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_RANDOM_SELECTION_CONTROLr(unit, &rval));
    *seed = soc_reg_field_get(unit, DLB_ECMP_RANDOM_SELECTION_CONTROLr, rval,
                        SEEDf);
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_sample_rate_thresholds_set
 * Purpose:
 *      Set ECMP DLB sample period and physical link accounting thresholds.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 *      min_th      - (IN) Minimum port load threshold, in mbps.
 *      max_th      - (IN) Maximum port load threshold, in mbps.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_sample_rate_thresholds_set(int unit, int sample_rate,
                                             int min_th, int max_th)
{
    int num_time_units;
    uint32 measure_control_reg;
    int max_th_bytes;
    int th_increment;
    dlb_ecmp_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_bytes[7];
    uint32 hw_limit;

    if (sample_rate <= 0 || min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Compute sampling period in units of 1us:
     *     number of 1us time units = 10^6 / sample_rate
     */
    num_time_units = 1000000 / sample_rate;
    hw_limit = (1 << soc_reg_field_length(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                                            SAMPLING_PERIODf)) - 1;
    if (num_time_units < 1 || num_time_units > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    /* Compute threshold in bytes per sampling period:
     * bytes per 1us = (million bits per sec) * 10^6 / 8 * 10^(-6),
     * bytes per sampling period = (bytes per 1us) * (number of 1us
     *                             time units in sampling period)
     *                           = mbps * num_time_units / 8
     */
    max_th_bytes = max_th * num_time_units / 8;
    hw_limit = (1 << soc_mem_field_length(unit, DLB_ECMP_GLB_QUANTIZE_THRESHOLDm,
                                            PORT_LOADING_THRESHOLDf)) - 1;
    if (max_th_bytes > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, SAMPLING_PERIODf, num_time_units);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = sample_rate;

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_bytes[i] = (min_th + i * th_increment) * num_time_units / 8;
        soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, PORT_LOADING_THRESHOLDf,
             th_bytes[i]);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th = min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_sample_rate_set
 * Purpose:
 *      Set ECMP dynamic load balancing sample rate.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_sample_rate_set(int unit, int sample_rate)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_sample_rate_thresholds_set(unit,
                sample_rate, ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_tx_load_min_th_set
 * Purpose:
 *      Set ECMP DLB port load minimum threshold.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      min_th  - (IN) Minimum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_tx_load_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_sample_rate_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate, min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_ecmp_dlb_tx_load_max_th_set
 * Purpose:
 *      Set ECMP DLB port load maximum threshold.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      max_th  - (IN) Maximum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_tx_load_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_sample_rate_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate,
                ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th,
                max_th));
    return BCM_E_NONE;
}




/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_thresholds_set
 * Purpose:
 *      Set ECMP DLB queue size thresholds.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      min_th  - (IN) Minimum queue size threshold, in bytes.
 *      max_th  - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_thresholds_set(int unit, int min_th, int max_th)
{
    int max_th_cells;
    int th_increment;
    dlb_ecmp_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_cells[7];
    uint32 hw_limit;

    if (min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Convert threshold to number of cells */
    max_th_cells = max_th / _BCM_ECMP_DLB_CELL_BYTES;
    hw_limit = (1 << soc_mem_field_length(unit, DLB_ECMP_GLB_QUANTIZE_THRESHOLDm,
                                           TOTAL_PORT_QSIZE_THRESHOLDf)) - 1;
    if (max_th_cells > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_cells[i] = (min_th + i * th_increment) / _BCM_ECMP_DLB_CELL_BYTES;
        soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, TOTAL_PORT_QSIZE_THRESHOLDf,
             th_cells[i]);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th = min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_min_th_set
 * Purpose:
 *      Set ECMP DLB queue size minimum threshold.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      min_th  - (IN) Minimum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_qsize_thresholds_set(unit,
                min_th, ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_max_th_set
 * Purpose:
 *      Set ECMP DLB queue size maximum threshold.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      max_th  - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_qsize_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th, max_th));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_thresholds_set
 * Purpose:
 *      Set ECMP DLB physical queue size thresholds.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      min_th  - (IN) Minimum physical queue size threshold, in bytes.
 *      max_th  - (IN) Maximum physical queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_thresholds_set(int unit, int min_th, int max_th)
{
    int max_th_cells;
    int th_increment;
    dlb_ecmp_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_cells[7];
    uint32 hw_limit;
    soc_field_t qth_field = XPE_PORT_QSIZE_THRESHOLDf;
    if (min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        qth_field = ITM_PORT_QSIZE_THRESHOLDf;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Convert threshold to number of cells */
    max_th_cells = max_th / _BCM_ECMP_DLB_CELL_BYTES;
    hw_limit = (1 << soc_mem_field_length(unit, DLB_ECMP_GLB_QUANTIZE_THRESHOLDm,
                                           qth_field)) - 1;
    if (max_th_cells > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        BCM_IF_ERROR_RETURN
            (READ_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_cells[i] = (min_th + i * th_increment) / _BCM_ECMP_DLB_CELL_BYTES;
        soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, qth_field,
             th_cells[i]);
        BCM_IF_ERROR_RETURN
            (WRITE_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th = min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th = max_th;

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_min_th_set
 * Purpose:
 *      Set ECMP DLB physical queue size minimum threshold.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      min_th  - (IN) Minimum physical queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_physical_qsize_thresholds_set(unit,
                min_th, ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_max_th_set
 * Purpose:
 *      Set ECMP DLB physical queue size maximum threshold.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      max_th  - (IN) Maximum physical queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_physical_qsize_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th, max_th));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_tx_load_weight_set
 * Purpose:
 *      Set ECMP DLB port load weight.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      weight  - (IN) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_tx_load_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                                          PORT_LOADING_WEIGHTf)) - 1;
    if (weight < 0 ||
        weight > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PORT_LOADING_WEIGHTf, weight);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_tx_load_weight_get
 * Purpose:
 *      Get ECMP DLB port load weight.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      weight  - (OUT) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_tx_load_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PORT_LOADING_WEIGHTf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_weight_set
 * Purpose:
 *      Set ECMP DLB qsize weight.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      weight  - (IN) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;
    uint32 hw_limit;

    hw_limit = (1 << soc_reg_field_length(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                                           TOTAL_QSIZE_WEIGHTf)) - 1;
    if (weight < 0 ||
        weight > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, TOTAL_QSIZE_WEIGHTf, weight);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_weight_get
 * Purpose:
 *      Get ECMP DLB qsize weight.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      weight  - (OUT) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, TOTAL_QSIZE_WEIGHTf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_weight_set
 * Purpose:
 *      Set ECMP DLB physical qsize weight.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      weight  - (IN) Physical Queue size weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;
    soc_field_t qweight_field  = XPE_PORT_QSIZE_WEIGHTf;
    uint32 hw_limit;

    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        qweight_field = ITM_PORT_QSIZE_WEIGHTf;
    }
    hw_limit = (1 << soc_reg_field_length(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                                           qweight_field)) - 1;
    if (weight < 0 ||
        weight > hw_limit) {
        /* Hardware limits */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, qweight_field, weight);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_weight_get
 * Purpose:
 *      Get ECMP DLB physical qsize weight.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      weight  - (OUT) Physical Queue size weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        *weight = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, ITM_PORT_QSIZE_WEIGHTf);
    } else
#endif
    {
        *weight = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, XPE_PORT_QSIZE_WEIGHTf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_tx_load_cap_set
 * Purpose:
 *      Set ECMP DLB port load cap control.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      cap     - (IN) Port load cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_tx_load_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 ||
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, CAP_PORT_LOADING_AVERAGEf, cap);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_tx_load_cap_get
 * Purpose:
 *      Get ECMP DLB port load cap control.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      cap     - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_tx_load_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, CAP_PORT_LOADING_AVERAGEf);

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_cap_set
 * Purpose:
 *      Set ECMP DLB queue size cap control.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      cap     - (IN) Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 ||
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, CAP_TOTAL_PORT_QSIZE_AVERAGEf, cap);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_qsize_cap_get
 * Purpose:
 *      Get ECMP DLB queue size cap control.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      cap     - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_qsize_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, CAP_TOTAL_PORT_QSIZE_AVERAGEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_cap_set
 * Purpose:
 *      Set ECMP DLB physical queue size cap control.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      cap     - (IN) Physical Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;
    soc_field_t qsizeavg_field  = CAP_XPE_PORT_QSIZE_AVERAGEf;

    if (cap < 0 ||
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        qsizeavg_field = CAP_ITM_PORT_QSIZE_AVERAGEf;
    }
#endif

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, qsizeavg_field, cap);
    BCM_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_physical_qsize_cap_get
 * Purpose:
 *      Get ECMP DLB physical queue size cap control.
 * Parameters:
 *      unit    - (IN) SOC unit number.
 *      cap     - (OUT) Physical Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th2_ecmp_dlb_physical_qsize_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    BCM_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        *cap = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, CAP_ITM_PORT_QSIZE_AVERAGEf);
    } else
#endif
    {
        *cap = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, CAP_XPE_PORT_QSIZE_AVERAGEf);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_config_set
 * Purpose:
 *      Set per-chip ECMP dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (IN) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th2_ecmp_dlb_config_set(int unit, bcm_switch_control_t type, int arg)
{
    switch (type) {
        case bcmSwitchEcmpDynamicSampleRate:
            return _bcm_th2_ecmp_dlb_sample_rate_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesExponent:
            return _bcm_th2_ecmp_dlb_tx_load_weight_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesExponent:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_ecmp_dlb_qsize_weight_set(unit, arg);
            }
        case bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent:
            return _bcm_th2_ecmp_dlb_physical_qsize_weight_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesDecreaseReset:
            return _bcm_th2_ecmp_dlb_tx_load_cap_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesDecreaseReset:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_ecmp_dlb_qsize_cap_set(unit, arg);
            }
        case bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset:
            return _bcm_th2_ecmp_dlb_physical_qsize_cap_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesMinThreshold:
            return _bcm_th2_ecmp_dlb_tx_load_min_th_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesMaxThreshold:
            return _bcm_th2_ecmp_dlb_tx_load_max_th_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesMinThreshold:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_ecmp_dlb_qsize_min_th_set(unit, arg);
            }
        case bcmSwitchEcmpDynamicQueuedBytesMaxThreshold:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_ecmp_dlb_qsize_max_th_set(unit, arg);
            }
        case bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold:
            return _bcm_th2_ecmp_dlb_physical_qsize_min_th_set(unit, arg);

        case bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold:
            return _bcm_th2_ecmp_dlb_physical_qsize_max_th_set(unit, arg);

        case bcmSwitchEcmpDynamicRandomSeed:
            return _bcm_th2_ecmp_dlb_random_seed_set(unit, arg);

        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_config_get
 * Purpose:
 *      Get per-chip ECMP dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (OUT) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th2_ecmp_dlb_config_get(int unit, bcm_switch_control_t type, int *arg)
{
    switch (type) {
        case bcmSwitchEcmpDynamicSampleRate:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate;
            break;

        case bcmSwitchEcmpDynamicEgressBytesExponent:
            return _bcm_th2_ecmp_dlb_tx_load_weight_get(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesExponent:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_ecmp_dlb_qsize_weight_get(unit, arg);
            }
        case bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent:
            return _bcm_th2_ecmp_dlb_physical_qsize_weight_get(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesDecreaseReset:
            return _bcm_th2_ecmp_dlb_tx_load_cap_get(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesDecreaseReset:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                return _bcm_th2_ecmp_dlb_qsize_cap_get(unit, arg);
            }
        case bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset:
            return _bcm_th2_ecmp_dlb_physical_qsize_cap_get(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesMinThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th;
            break;

        case bcmSwitchEcmpDynamicEgressBytesMaxThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th;
            break;

        case bcmSwitchEcmpDynamicQueuedBytesMinThreshold:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th;
                break;
            }

        case bcmSwitchEcmpDynamicQueuedBytesMaxThreshold:
            if (soc_feature(unit, soc_feature_td3_dlb)) {
                return BCM_E_UNAVAIL;
            } else {
                *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th;
                break;
            }

        case bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th;
            break;

        case bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th;
            break;

        case bcmSwitchEcmpDynamicRandomSeed:
            return _bcm_th2_ecmp_dlb_random_seed_get(unit, arg);

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_quality_map_profile_init
 * Purpose:
 *      Initialize ECMP DLB quality mapping profile.
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_quality_map_profile_init(int unit)
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_words;
    int entries_per_profile;
    int tx_load_percent;
    int qsize_percent;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    int i;
    int port;
    dlb_ecmp_quantize_control_entry_t quantize_control_entry;
    int num_quality_map_profiles;
    int field_len;
    void *mask_p[1];
    uint32 ent[1] = {0};

    if (NULL == _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile =
            sal_alloc(sizeof(soc_profile_mem_t),
                      "ECMP DLB Quality Map Profile Mem");
        if (NULL == _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile) {
            return BCM_E_MEMORY;
        }
    } else {
        soc_profile_mem_destroy(unit,
                _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile);
    }
    profile = _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile;
    soc_profile_mem_t_init(profile);

    mem = DLB_ECMP_PORT_QUALITY_MAPPINGm;
    entry_words = sizeof(dlb_ecmp_quality_mapping_entry_t) / sizeof(uint32);

    /* Get quality mask */
    field_len = soc_mem_field_length(unit, mem, ASSIGNED_QUALITYf);
    soc_mem_field32_set(unit, mem, ent,
                        ASSIGNED_QUALITYf, (1 << field_len) - 1);
    mask_p[0] = ent;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_index_create(unit, &mem, &entry_words,
                                      NULL, NULL, mask_p, 1, profile));

    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    entries_per_profile =
        soc_mem_index_count(unit, DLB_ECMP_PORT_QUALITY_MAPPINGm) /
        num_quality_map_profiles;

    if (!SOC_WARM_BOOT(unit)) {
        alloc_size = entries_per_profile * entry_words * sizeof(uint32);
        entry_arr = sal_alloc(alloc_size, "ECMP DLB Quality Map entries");
        if (entry_arr == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(entry_arr, 0, alloc_size);

        /* Assign quality in the entry array, with 100% weight for port load,
         * 0% for queue size.
         */
        tx_load_percent = 100;
        qsize_percent   = 0;
        rv = _bcm_th2_ecmp_dlb_quality_assign(unit, tx_load_percent,
                qsize_percent, entry_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_arr);
            return rv;
        }

        entries = entry_arr;
        rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
                                 &base_index);
        sal_free(entry_arr);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        for (port = 0;
             port < soc_mem_index_count(unit, DLB_ECMP_QUANTIZE_CONTROLm);
             port++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                                                 port,
                                                 &quantize_control_entry));
            soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
                    &quantize_control_entry, PORT_QUALITY_MAPPING_PROFILE_PTRf,
                    base_index / entries_per_profile);
            BCM_IF_ERROR_RETURN
                (WRITE_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ALL,
                                                  port,
                                                  &quantize_control_entry));
        }

        for (i = 0; i < entries_per_profile; i++) {
            SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i,
                                      port - 1);
        }

        ECMP_DLB_INFO(unit)->
            ecmp_dlb_load_weight[base_index/entries_per_profile] =
                tx_load_percent;
        ECMP_DLB_INFO(unit)->
            ecmp_dlb_qsize_weight[base_index/entries_per_profile] =
                qsize_percent;

    } else { /* Warm boot, recover reference counts and entries_per_set */

        for (port = 0;
             port < soc_mem_index_count(unit, DLB_ECMP_QUANTIZE_CONTROLm);
             port++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                                                 port,
                                                 &quantize_control_entry));
            base_index = soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_get(unit,
                    &quantize_control_entry, PORT_QUALITY_MAPPING_PROFILE_PTRf);
            base_index *= entries_per_profile;
            for (i = 0; i < entries_per_profile; i++) {
                SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, profile,
                        base_index + i, entries_per_profile);
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th2_ecmp_dlb_deinit
 * Purpose:
 *      Deallocate _th2_ecmp_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      None
 */
void
_bcm_th2_ecmp_dlb_deinit(int unit)
{
    if (_th2_ecmp_dlb_bk[unit]) {
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info) {
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info = NULL;
        }
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap) {
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap = NULL;
        }
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap) {
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap = NULL;
        }
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap) {
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap = NULL;
        }
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight) {
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight = NULL;
        }
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight) {
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight = NULL;
        }
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile) {
            soc_profile_mem_destroy(unit,
                    _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile);
            sal_free(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile);
            _th2_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile = NULL;
        }

        sal_free(_th2_ecmp_dlb_bk[unit]);
        _th2_ecmp_dlb_bk[unit] = NULL;
    }

}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_init
 * Purpose:
 *      Allocate and initialize _th2_ecmp_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_th2_ecmp_dlb_init(int unit)
{
    int rv = BCM_E_NONE;
    int num_ecmp_dlb_id;
    int flowset_tbl_size;
    int total_num_blocks;
    int num_quality_map_profiles;
    int port;
    dlb_ecmp_quantize_control_entry_t quantize_control_entry;
    dlb_ecmp_port_quality_update_measure_control_entry_t measure_entry;
    dlb_ecmp_eem_configuration_entry_t eem_entry;
    int max_ecmp_dlb_ports;
    int sample_rate;
    int qsize_sel[4] = {1, 0, 0, 1};
    soc_reg_t reg;
    int pipe;
    uint32 rval;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 reg_val = 0;
#endif
    soc_field_t qsf_field = XPE_PORT_QSIZE_THRESHOLD_SCALING_FACTORf;

    _bcm_th2_ecmp_dlb_deinit(unit);

    if (_th2_ecmp_dlb_bk[unit] == NULL) {
        _th2_ecmp_dlb_bk[unit] = sal_alloc(
                sizeof(_th2_ecmp_dlb_bookkeeping_t), "_th2_ecmp_dlb_bk");
        if (_th2_ecmp_dlb_bk[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit], 0, sizeof(_th2_ecmp_dlb_bookkeeping_t));

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        qsf_field = ITM_PORT_QSIZE_THRESHOLD_SCALING_FACTORf;
    }
#endif

    _th2_ecmp_dlb_bk[unit]->ecmp_dlb_property_force_set =
        soc_feature(unit, soc_feature_ecmp_dlb_property_force_set);

    max_ecmp_dlb_ports = soc_mem_index_count(unit,
            DLB_ECMP_QUANTIZE_CONTROLm);
    if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info == NULL) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info =
            sal_alloc(sizeof(_th2_ecmp_dlb_port_info_t) * max_ecmp_dlb_ports,
                    "ecmp_dlb_port_info");
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info == NULL) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info, 0,
            sizeof(_th2_ecmp_dlb_port_info_t) * max_ecmp_dlb_ports);
    _th2_ecmp_dlb_bk[unit]->ecmp_dlb_port_info_size = max_ecmp_dlb_ports;


    num_ecmp_dlb_id = soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm);
    if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap == NULL) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_ecmp_dlb_id),
                    "ecmp_dlb_id_used_bitmap");
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap == NULL) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap, 0,
            SHR_BITALLOCSIZE(num_ecmp_dlb_id));

    if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap == NULL) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_ecmp_dlb_id),
                    "ecmp_dlb_id_dgm_bitmap");
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap == NULL) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap, 0,
            SHR_BITALLOCSIZE(num_ecmp_dlb_id));

    flowset_tbl_size = soc_mem_index_count(unit, DLB_ECMP_FLOWSETm);
    /* Each bit in ecmp_dlb_flowset_block_bitmap corresponds to a block of 256
     * flow set table entries.
     */
    total_num_blocks = flowset_tbl_size >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap == NULL) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(total_num_blocks),
                    "ecmp_dlb_flowset_block_bitmap");
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap == NULL) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap, 0,
            SHR_BITALLOCSIZE(total_num_blocks));

    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight == NULL) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight =
            sal_alloc(num_quality_map_profiles * sizeof(uint8),
                    "ecmp_dlb_load_weight");
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight == NULL) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight, 0,
            num_quality_map_profiles * sizeof(uint8));


    if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight == NULL) {
        _th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight =
            sal_alloc(num_quality_map_profiles * sizeof(uint8),
                    "ecmp_dlb_qsize_weight");
        if (_th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight == NULL) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_qsize_weight, 0,
            num_quality_map_profiles * sizeof(uint8));


    /* Initialize port quality mapping profile */
    rv = _bcm_th2_ecmp_dlb_quality_map_profile_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_th2_ecmp_dlb_deinit(unit);
        return rv;
    }

    if (!SOC_WARM_BOOT(unit)) {

        /* Set DLB sampling period and thresholds */
        sample_rate = 62500;
        rv = _bcm_th2_ecmp_dlb_sample_rate_thresholds_set(unit, sample_rate,
                                                          65536, (65536 * 7));
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure ECMP DLB load EWMA weight */
        rv = _bcm_th2_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicEgressBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure ECMP DLB queue size EWMA weight */
        if (!soc_feature(unit, soc_feature_td3_dlb)) {
            rv = _bcm_th2_ecmp_dlb_config_set(unit,
                    bcmSwitchEcmpDynamicQueuedBytesExponent, 0);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }
        /* Configure ECMP DLB physical queue size EWMA weight */
        rv = _bcm_th2_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure ECMP DLB load cap control */
        rv = _bcm_th2_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicEgressBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure ECMP DLB queue size cap control */
        if (!soc_feature(unit, soc_feature_td3_dlb)) {
            rv = _bcm_th2_ecmp_dlb_config_set(unit,
                    bcmSwitchEcmpDynamicQueuedBytesDecreaseReset, 0);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }

        /* Configure ECMP DLB physical queue size cap control */
        rv = _bcm_th2_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure ECMP DLB queue size thresholds */
        if (!soc_feature(unit, soc_feature_td3_dlb)) {
            rv = _bcm_th2_ecmp_dlb_qsize_thresholds_set(unit, 0, 0);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }
        /* Configure ECMP DLB physical queue size thresholds */
        rv = _bcm_th2_ecmp_dlb_physical_qsize_thresholds_set(unit, 0, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Disable link status override by software */
        rv = soc_mem_clear(unit, DLB_ECMP_LINK_CONTROLm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Clear group membership */
        rv = soc_mem_clear(unit, DLB_ECMP_GROUP_MEMBERSHIPm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (soc_feature(unit, soc_feature_th3_style_dlb)) {
            uint64 dlb_en_bitmap;
            rv = READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap);
            if (BCM_SUCCESS(rv)) {
                uint64 my_zero;
                COMPILER_64_ZERO(my_zero);
                soc_reg64_field_set(unit, DLB_ID_0_TO_63_ENABLEr, &dlb_en_bitmap, 
                                    BITMAPf, my_zero);
                soc_reg64_field_set(unit, DLB_ID_64_TO_127_ENABLEr, 
                                    &dlb_en_bitmap, BITMAPf, my_zero);
                BCM_IF_ERROR_RETURN(WRITE_DLB_ID_0_TO_63_ENABLEr(unit, 
                                        dlb_en_bitmap));
                BCM_IF_ERROR_RETURN(WRITE_DLB_ID_64_TO_127_ENABLEr(unit, 
                                        dlb_en_bitmap));
            }
        }
#endif
        if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
            /* Clear group port to member tables */
            rv = soc_mem_clear(unit, DLB_ECMP_GROUP_PORT_TO_MEMBERm,
                    MEM_BLOCK_ALL, 0);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }

        /* Configure per member quality measure update control. */
        for (port = 0;
             port < soc_mem_index_count(unit,
                     DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm);
             port++) {
            rv = READ_DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm(unit,
                    MEM_BLOCK_ANY, port, &measure_entry);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
            soc_DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm_field32_set(unit,
                    &measure_entry, ENABLE_MEASURE_AVERAGE_CALCULATIONf, 1);
            soc_DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm_field32_set(unit,
                    &measure_entry, ENABLE_PORT_QUALITY_UPDATEf, 1);
            soc_DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm_field32_set(unit,
                    &measure_entry, ENABLE_CREDIT_COLLECTIONf, 1);
            rv = WRITE_DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm(unit,
                    MEM_BLOCK_ALL, port, &measure_entry);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }

        /* Configure per member threshold scaling factor */
        for (port = 0;
             port < soc_mem_index_count(unit, DLB_ECMP_QUANTIZE_CONTROLm);
             port++) {
            rv = READ_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ANY,
                    port, &quantize_control_entry);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
            /* default 10G */
            soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
                    &quantize_control_entry,
                    PORT_LOADING_THRESHOLD_SCALING_FACTORf,
                    0);
            if (!soc_feature(unit, soc_feature_td3_dlb)) {
                soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
                        &quantize_control_entry,
                        TOTAL_PORT_QSIZE_THRESHOLD_SCALING_FACTORf,
                        2);
            }
            soc_DLB_ECMP_QUANTIZE_CONTROLm_field32_set(unit,
                    &quantize_control_entry,
                    qsf_field,
                    0);
            rv = WRITE_DLB_ECMP_QUANTIZE_CONTROLm(unit, MEM_BLOCK_ALL,
                    port, &quantize_control_entry);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }

        /* Configure Ethertype eligibility */
        rv = soc_mem_clear(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm,
                MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_ecmp_dlb_deinit(unit);
            return rv;
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            rv = READ_DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                    &reg_val);
            if (BCM_SUCCESS(rv)) {
                soc_reg_field_set(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                        &reg_val, CONFIGURATIONf, 0);
                soc_reg_field_set(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr,
                        &reg_val, INNER_OUTER_ETHERTYPE_SELECTIONf, 0);
                rv = WRITE_DLB_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr(unit,
                        reg_val);
            }
        } else
#endif
        {
            rv = READ_DLB_ECMP_EEM_CONFIGURATIONm(unit,
                    MEM_BLOCK_ANY, 0, &eem_entry);
            if (BCM_SUCCESS(rv)) {
                soc_DLB_ECMP_EEM_CONFIGURATIONm_field32_set(unit,
                        &eem_entry,
                        CONFIGURATIONf, 0);
                if (!(soc_feature(unit, soc_feature_th3_style_dlb))) {
                    soc_DLB_ECMP_EEM_CONFIGURATIONm_field32_set(unit,
                            &eem_entry,
                            INNER_OUTER_ETHERTYPE_SELECTIONf, 0);
                }
                rv = WRITE_DLB_ECMP_EEM_CONFIGURATIONm(unit,
                        MEM_BLOCK_ALL, 0, &eem_entry);
            }
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Clear all DLB flow monitoring confuration */
        if (SOC_IS_TOMAHAWK3(unit)) {
            int seed = 1; /* Initialize seed to a non-zero value */

            if (soc_property_get(unit, spn_DLB_FLOW_MONITOR_EN, 0)) {
                rv = soc_mem_clear(unit, DLB_ECMP_GROUP_MONITOR_CONTROLm,
                        MEM_BLOCK_ALL, 0);

                if (BCM_FAILURE(rv)) {
                    _bcm_th2_ecmp_dlb_deinit(unit);
                    return rv;
                }
            }

            /* Program DLB monitor's random selection seed */
            rv = WRITE_DLB_ECMP_MONITOR_CONTROLr(unit, seed);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }

            /* Program DLB monitor's random selection seed for IFP */
            rv = WRITE_DLB_ECMP_MONITOR_IFP_CONTROLr(unit, seed);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }
#endif
    }
    if (SOC_REG_IS_VALID(unit, DLB_ECMP_PORT_XPE_A_B_SELECTORr)) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe ++) {
            reg = SOC_REG_UNIQUE_ACC(unit, DLB_ECMP_PORT_XPE_A_B_SELECTORr)[pipe];
            rv = soc_reg_field32_modify(unit, reg,
                    REG_PORT_ANY, CAP_SINGLE_QSIZE_SELECTORf, qsize_sel[pipe]);
            if (BCM_FAILURE(rv)) {
                _bcm_th2_ecmp_dlb_deinit(unit);
                return rv;
            }
        }
    }

    READ_AUX_ARB_CONTROLr(unit, &rval);
    if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                DLB_HGT_256NS_REFRESH_ENABLEf)) {
        soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval,
                DLB_HGT_256NS_REFRESH_ENABLEf, 1);
    }
    WRITE_AUX_ARB_CONTROLr(unit, rval);

    return rv;
}


#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_th2_ecmp_dlb_wb_alloc_size_get
 * Purpose:
 *      Get level 2 warm boot scache size for ECMP DLB.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      size - (OUT) Allocation size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_ecmp_dlb_wb_alloc_size_get(int unit, int *size)
{
    int alloc_size = 0;
    int num_elements;

    /* Allocate size for the following ECMP DLB parameters:
     * int ecmp_dlb_sample_rate;
     * int ecmp_dlb_tx_load_min_th;
     * int ecmp_dlb_tx_load_max_th;
     * int ecmp_dlb_qsize_min_th;
     * int ecmp_dlb_qsize_max_th;
     * int ecmp_dlb_physical_qsize_min_th;
     * int ecmp_dlb_physical_qsize_max_th;
     *     ecmp_dlb_id_dgm_bitmap
     * For TD3, these HW thresholds are absent
     * ecmp_dlb_qsize_min_th & ecmp_dlb_qsize_max_th
     * For TH3, bitmap for Egr objects are also saved.
     */
    if(soc_feature(unit, soc_feature_td3_dlb)) {
        alloc_size += sizeof(int) * 5;
    } else {
        alloc_size += sizeof(int) * 7;
    }
    /* Allocate size of ecmp_dlb_load_weight & ecmp_dlb_qsize_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    alloc_size += sizeof(uint8) * 2 * num_elements;

    num_elements = soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm);
    alloc_size += SHR_BITALLOCSIZE(num_elements);

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        /* For TH3, add DLB EGR objects which are not a member of the group.
         * Save in a form of bitmap, as this way it always be 4K bytes in size,
         * else if IDs are saved directly it might take upto 64K bytes in worst
         * case scenario (where all EGR Objects not a part of ECMP groups), as
         * there can be max 16K DLB Egr objects.
         */
        num_elements = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
        alloc_size += SHR_BITALLOCSIZE(num_elements);

        /* For TH3, save the member bitmap for the DLB ECMP group */
        alloc_size += BCM_TH3_L3_ECMP_MAX_DLB_GROUPS *
                SHR_BITALLOCSIZE(BCM_TH3_L3_ECMP_DLB_MAX_PATHS * 2);
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
    {
        num_elements = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
        alloc_size += SHR_BITALLOCSIZE(num_elements);
    }
    *size = alloc_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_ecmp_dlb_member_sync
 * Purpose:
 *      Sync ECMP DLB member info for those members which do not belong
 *      to any DLB ECMP group yet.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_ecmp_dlb_member_sync(int unit, uint8 **scache_ptr)
{
    int         port;

    SHR_BITDCL *bitmap = (SHR_BITDCL *)(*scache_ptr);
    _th2_ecmp_dlb_port_membership_t *current_ptr = NULL;

    /* Save the standalone NHOPs from the DLP port based list */
    for (port=0 ;port < ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size ;port++) {
        current_ptr = ECMP_DLB_PORT_LIST(unit, port);
        while (current_ptr) {
            if (-1 == current_ptr->dlb_id) {
                if (current_ptr->nh_index < BCM_XGS3_L3_NH_TBL_SIZE(unit)) {
                    SHR_BITSET(bitmap, current_ptr->nh_index);
                }
            }
            current_ptr = current_ptr->next;
        }
    }
    (*scache_ptr) += SHR_BITALLOCSIZE(BCM_XGS3_L3_NH_TBL_SIZE(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_ecmp_dlb_member_scache_recover
 * Purpose:
 *      Recover ECMP DLB member info for those members which do not belong
 *      to any DLB ECMP group yet.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_ecmp_dlb_member_scache_recover(int unit, uint8 **scache_ptr)
{
    int         rv = BCM_E_NONE;

    int         nh_index;
    char        *l3_tbl_ptr    = NULL;  /* Table dma pointer. */
    uint32      *ing_entry_ptr = NULL;
    uint32      port;
    uint32      hw_entry[SOC_MAX_MEM_WORDS];  /* Buffer to fill nh entry. */
    SHR_BITDCL  *bitmap = (SHR_BITDCL *)(*scache_ptr);
    _th2_ecmp_dlb_port_membership_t *membership;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
#endif

    rv = bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, nh),
                             BCM_XGS3_L3_ENT_SZ(unit, nh),
                             "l3_tbl", &l3_tbl_ptr, NULL);
    if (BCM_FAILURE(rv)) {
        rv = BCM_E_NONE;
        l3_tbl_ptr = NULL;
        ing_entry_ptr = &hw_entry[0];
    }

    /* Create the DLB port based list */
    SHR_BIT_ITER(bitmap, BCM_XGS3_L3_NH_TBL_SIZE(unit), nh_index) {
        /* Read ingress next hop entry. */
        if (l3_tbl_ptr) {
            ing_entry_ptr =
                soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, nh),
                                             uint32 *, l3_tbl_ptr, nh_index);
        } else {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_MEM_READ(
                     unit, BCM_XGS3_L3_MEM(unit, nh), nh_index, ing_entry_ptr);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            port = soc_mem_field32_dest_get(unit, BCM_XGS3_L3_MEM(unit, nh),
                                            ing_entry_ptr, DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
                port = (port & SOC_MEM_FIF_DGPP_PORT_MASK);
            } else {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(
                    unit, BCM_XGS3_L3_MEM(unit, nh), ing_entry_ptr, Tf)) {
                /* DLB Egr object cannot be a trunk, Skip the entry */
                continue;
            }
            port = soc_mem_field32_get(
                       unit, BCM_XGS3_L3_MEM(unit, nh), ing_entry_ptr, PORT_NUMf);
        }

        membership =
            sal_alloc(sizeof(_th2_ecmp_dlb_port_membership_t),
            "ecmp dlb nh membership");
        if (NULL == membership) {
            rv = BCM_E_MEMORY;
            break;
        }
        sal_memset(membership, 0, sizeof(_th2_ecmp_dlb_port_membership_t));
        membership->nh_index = nh_index;
        membership->dlb_id   = -1;
        membership->next = ECMP_DLB_PORT_LIST(unit, port);
        ECMP_DLB_PORT_LIST(unit, port) = membership;
        ECMP_DLB_PORT_INFO(unit, port).nh_count++;
    }

    if (l3_tbl_ptr) {
        soc_cm_sfree(unit, l3_tbl_ptr);
        l3_tbl_ptr = NULL;
    }
    (*scache_ptr) += SHR_BITALLOCSIZE(BCM_XGS3_L3_NH_TBL_SIZE(unit));

    return rv;
}
#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *      _bcm_th3_ecmp_dlb_member_sync
 * Purpose:
 *      Sync ECMP DLB member info for those members which do not belong
 *      to any DLB ECMP group yet.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_th3_ecmp_dlb_member_sync(int unit, uint8 **scache_ptr)
{
    int         port;
    uint32      bytes;
    SHR_BITDCL *bitmap = (SHR_BITDCL *)(*scache_ptr);
    _th2_ecmp_dlb_port_membership_t *current_ptr = NULL;

    /* Save the standalone NHOPs from the DLP port based list */
    for (port=0 ;port < ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size ;port++) {
        current_ptr = ECMP_DLB_PORT_LIST(unit, port);
        while (current_ptr) {
            if (-1 == current_ptr->dlb_id) {
                if (current_ptr->nh_index < BCM_XGS3_L3_NH_TBL_SIZE(unit)) {
                    SHR_BITSET(bitmap, current_ptr->nh_index);
                }
            }
            current_ptr = current_ptr->next;
        }
    }
    (*scache_ptr) += SHR_BITALLOCSIZE(BCM_XGS3_L3_NH_TBL_SIZE(unit));

    /* Save the member bitmap information for all DLB ECMP groups */
    bitmap = (SHR_BITDCL *)
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(
            unit, BCM_TH3_L3_ECMP_DLB_START(unit));
    bytes = BCM_TH3_L3_ECMP_MAX_DLB_GROUPS *
                SHR_BITALLOCSIZE(BCM_TH3_L3_ECMP_DLB_MAX_PATHS * 2);
    sal_memcpy((*scache_ptr), bitmap, bytes);
    (*scache_ptr) += bytes;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ecmp_dlb_scache_recover
 * Purpose:
 *      Recover ECMP DLB member info for those members which do not belong
 *      to any DLB ECMP group yet.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_th3_ecmp_dlb_scache_recover(int unit, uint8 **scache_ptr)
{
    int         rv = BCM_E_NONE;
    int         bytes;
    int         nh_index;
    char        *l3_tbl_ptr    = NULL;  /* Table dma pointer. */
    uint32      *ing_entry_ptr = NULL;
    uint32      port;
    uint32      hw_entry[SOC_MAX_MEM_WORDS];  /* Buffer to fill nh entry. */
    SHR_BITDCL  *bitmap = (SHR_BITDCL *)(*scache_ptr);
    _th2_ecmp_dlb_port_membership_t *membership;

    rv = bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, nh),
                                  BCM_XGS3_L3_ENT_SZ(unit, nh),
                                  "l3_tbl", &l3_tbl_ptr, NULL);
    if (BCM_FAILURE(rv)) {
        rv = BCM_E_NONE;
        l3_tbl_ptr = NULL;
        ing_entry_ptr = &hw_entry[0];
    }

    /* Create the DLB port based list */
    SHR_BIT_ITER(bitmap, BCM_XGS3_L3_NH_TBL_SIZE(unit), nh_index) {
        /* Read ingress next hop entry. */
        if (l3_tbl_ptr) {
            ing_entry_ptr =
                soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, nh),
                                             uint32 *, l3_tbl_ptr, nh_index);
        } else {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_MEM_READ(
                    unit, BCM_XGS3_L3_MEM(unit, nh), nh_index, ing_entry_ptr);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (soc_mem_field32_get(
                unit, BCM_XGS3_L3_MEM(unit, nh), ing_entry_ptr, Tf)) {
            /* DLB Egr object cannot be a trunk, Skip the entry */
            continue;
        }
        port = soc_mem_field32_get(
                    unit, BCM_XGS3_L3_MEM(unit, nh), ing_entry_ptr, PORT_NUMf);

        membership = sal_alloc(
            sizeof(_th2_ecmp_dlb_port_membership_t), "ecmp dlb nh membership");
        if (NULL == membership) {
            rv = BCM_E_MEMORY;
            break;
        }
        sal_memset(membership, 0, sizeof(_th2_ecmp_dlb_port_membership_t));
        membership->nh_index = nh_index;
        membership->dlb_id   = -1;
        membership->next = ECMP_DLB_PORT_LIST(unit, port);
        ECMP_DLB_PORT_LIST(unit, port) = membership;
        ECMP_DLB_PORT_INFO(unit, port).nh_count++;
    }

    if (l3_tbl_ptr) {
        soc_cm_sfree(unit, l3_tbl_ptr);
        l3_tbl_ptr = NULL;
    }
    (*scache_ptr) += SHR_BITALLOCSIZE(BCM_XGS3_L3_NH_TBL_SIZE(unit));
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Retrieve the member bitmap for DLB ECMP groups */
    bitmap = (SHR_BITDCL *)
        BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(
            unit, BCM_TH3_L3_ECMP_DLB_START(unit));
    bytes = BCM_TH3_L3_ECMP_MAX_DLB_GROUPS *
                SHR_BITALLOCSIZE(BCM_TH3_L3_ECMP_DLB_MAX_PATHS * 2);
    sal_memcpy(bitmap, (*scache_ptr), bytes);
    (*scache_ptr) += bytes;
    return rv;
}
#endif
/*
 * Function:
 *      bcm_th2_ecmp_dlb_sync
 * Purpose:
 *      Store ECMP DLB parameters into scache.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      scache_ptr  - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_ecmp_dlb_sync(int unit, uint8 **scache_ptr)
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;
    uint8 qsize_weight;

    /* Store the following ECMP DLB parameters:
     * int ecmp_dlb_sample_rate;
     * int ecmp_dlb_tx_load_min_th;
     * int ecmp_dlb_tx_load_max_th;
     * int ecmp_dlb_qsize_min_th;
     * int ecmp_dlb_qsize_max_th;
     * int ecmp_dlb_physical_qsize_min_th;
     * int ecmp_dlb_physical_qsize_max_th;
     *     ecmp_dlb_id_dgm_bitmap
     */
    value = ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        value = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th;
        sal_memcpy((*scache_ptr), &value, sizeof(int));
        (*scache_ptr) += sizeof(int);

        value = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th;
        sal_memcpy((*scache_ptr), &value, sizeof(int));
        (*scache_ptr) += sizeof(int);
    }

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    /* Store ecmp_dlb_load_weight & ecmp_dlb_qsize_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        load_weight = ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[i];
        qsize_weight = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_weight[i];
        sal_memcpy((*scache_ptr), &load_weight, sizeof(uint8));
        (*scache_ptr) += sizeof(uint8);
        sal_memcpy((*scache_ptr), &qsize_weight, sizeof(uint8));
        (*scache_ptr) += sizeof(uint8);
    }

    num_elements = soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm);
    num_elements = SHR_BITALLOCSIZE(num_elements);
    sal_memcpy((*scache_ptr), _th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap,
               num_elements);
    (*scache_ptr) += num_elements;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        /* For TH3, add DLB EGR objects which are not a member of the group.
         * Save in a form of bitmap, as this way it always be 4K bytes in size,
         * else if IDs are saved directly it might take upto 64K bytes in worst
         * case scenario (where all EGR Objects not a part of ECMP groups), as
         * there can be max 16K DLB Egr objects.
         */
        _bcm_th3_ecmp_dlb_member_sync(unit, scache_ptr);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_ecmp_dlb_scache_recover
 * Purpose:
 *      Recover ECMP DLB parameters from scache.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      scache_ptr  - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_ecmp_dlb_scache_recover(int unit, uint8 **scache_ptr)
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;
    uint8 qsize_weight;

    /* Recover the following ECMP DLB parameters:
     * int ecmp_dlb_sample_rate;
     * int ecmp_dlb_tx_load_min_th;
     * int ecmp_dlb_tx_load_max_th;
     * int ecmp_dlb_qsize_min_th;
     * int ecmp_dlb_qsize_max_th;
     * int ecmp_dlb_physical_qsize_min_th;
     * int ecmp_dlb_physical_qsize_max_th;
     *     ecmp_dlb_id_dgm_bitmap
     */
    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th = value;
    (*scache_ptr) += sizeof(int);

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        sal_memcpy(&value, (*scache_ptr), sizeof(int));
        ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th = value;
        (*scache_ptr) += sizeof(int);

        sal_memcpy(&value, (*scache_ptr), sizeof(int));
        ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th = value;
        (*scache_ptr) += sizeof(int);
    }

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th = value;
    (*scache_ptr) += sizeof(int);


    /* Recover ecmp_dlb_load_weight & ecmp_dlb_qsize_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUANTIZE_CONTROLm, PORT_QUALITY_MAPPING_PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        sal_memcpy(&load_weight, (*scache_ptr), sizeof(uint8));
        ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[i] = load_weight;
        (*scache_ptr) += sizeof(uint8);
        sal_memcpy(&qsize_weight, (*scache_ptr), sizeof(uint8));
        ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_weight[i] = qsize_weight;
        (*scache_ptr) += sizeof(uint8);
    }

    num_elements = soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm);
    num_elements = SHR_BITALLOCSIZE(num_elements);
    sal_memcpy(_th2_ecmp_dlb_bk[unit]->ecmp_dlb_id_dgm_bitmap, (*scache_ptr), num_elements);
    (*scache_ptr) += num_elements;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {
        _bcm_th3_ecmp_dlb_scache_recover(unit, scache_ptr);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    ECMP_DLB_INFO(unit)->recovered_from_scache = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_member_recover
 * Purpose:
 *      Recover ECMP DLB ports usage and the next hop indices associated
 *      with the ports.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_member_recover(int unit)
{
    int rv = BCM_E_NONE;
    int port_map_width;
    int alloc_size_port_map;
    int alloc_size_member_bitmap;
    SHR_BITDCL *port_map = NULL;
    SHR_BITDCL *port_map2 = NULL;
    SHR_BITDCL *member_bitmap = NULL;
    int i, j, k;
    int port;
    int dlb_id;
    dlb_ecmp_group_membership_entry_t group_membership_entry;
    dlb_ecmp_group_port_to_member_entry_t port2member_entry;
    ecmp_dlb_control_entry_t *ecmp_group_entry;
    _th2_ecmp_dlb_port_membership_t *membership;
    int ecmp_count = 0;           /* Next hop count in the group.        */
    bcm_if_t *ecmp_member = NULL; /* Ecmp group from hw.                 */
    dlb_ecmp_group_control_entry_t group_control_entry;
    _th2_ecmp_dlb_port_membership_t *current_ptr, *first_ptr;
    int membership_pointer;
    int base_addr;
    int group_enable;
    int member_bitmap_width;
    int offset;
    int count;
    int nh_index;
    bcm_l3_egress_ecmp_t tmp_ecmp;
    int primary_size = 0;
    SHR_BITDCL *group_member_ptr = NULL;
    int index_min, index_max,buf_size;
    uint8 *ecmp_dlb_buf = NULL;
    soc_mem_t mem = ECMP_DLB_CONTROLm;

    member_bitmap_width = BCM_XGS3_L3_ECMP_DLB_MAX_PATHS(unit) * 2;

    /* Allocate port map */
    port_map_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size_port_map = SHR_BITALLOCSIZE(port_map_width);
    port_map = sal_alloc(alloc_size_port_map, "DLB ECMP port map");
    if (NULL == port_map) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    port_map2 = sal_alloc(alloc_size_port_map, "DLB ECMP port map 2");
    if (NULL == port_map2) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Allocate member bitmap */
    alloc_size_member_bitmap = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size_member_bitmap, "DLB ECMP member bitmap");
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Allocate ECMP group members */
    ecmp_member = sal_alloc(member_bitmap_width * sizeof(bcm_if_t),
            "ecmp member array");
    if (NULL == ecmp_member) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    ecmp_dlb_buf = soc_cm_salloc(unit, buf_size, "ECMP_DLB_CONTROL buffer");
    if (NULL == ecmp_dlb_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, ecmp_dlb_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = index_min; i <= index_max; i++) {
        ecmp_group_entry = (ecmp_dlb_control_entry_t *)
                               soc_mem_table_idx_to_pointer
                                  (unit, mem, void *, ecmp_dlb_buf, i);

        group_enable = soc_ECMP_DLB_CONTROLm_field32_get(unit,
                                                         ecmp_group_entry,
                                                         GROUP_ENABLEf);
        if (0 == group_enable) {
            /* DLB not enabled */
            continue;
        }

        /* Get DLB group ID */
        dlb_id = soc_ECMP_DLB_CONTROLm_field32_get(unit, ecmp_group_entry,
                                                   DLB_IDf);

        rv = READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id,
                                          &group_control_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        membership_pointer = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, GROUP_MEMBERSHIP_POINTERf);

        base_addr = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, GROUP_PORT_TO_MEMBER_BASE_ADDRf);

        primary_size = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, PRIMARY_GROUP_SIZEf);

        rv = READ_DLB_ECMP_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                                             membership_pointer,
                                             &group_membership_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* We have the DLB_ID here,
         * now there are 2 ways to recover these info from hw to sw:
         * 1) Use the DLB_ID to get port_map,
         *    the port map will indicate how many port2member entries are set.
         *    traverse the valid port2member entries via port map.
         *    Each port2member entry can be parsed to get the valid bits and then
         *    the nexthops. Got the nexthop, the DLB_ID, the port. It's enough
         *    to recover the sw state.
         * 2) The first method has a side effect, which is traverse all valid
         *    port2member entries, and then traverse the valid bits of each.
         *    That's portnumber * 64 loops in total.
         *    An alternative way is to get the ecmp members (64 members at most)
         *    Parse the entries and then get the ports. Same as above, we got
         *    all the needed info: nexthop, DLB_ID, port.
         *
         *    Actually, the port2member entries are not necessary, if DLB can
         *    traverse the ecmp member table and get all nexthops pointing
         *    to the chosen port, it's done. In fact, port2member is just another
         *    angle of ecmp member table.
         */

        /* Get port map */
        sal_memset(port_map, 0, alloc_size_port_map);
        soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
                &group_membership_entry, PORT_MAPf, port_map);

        sal_memset(port_map2, 0, alloc_size_port_map);
        soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
                &group_membership_entry, PORT_MAP_ALTERNATEf, port_map2);

        /* Get ECMP members */
        /* Check if the ECMP group is DLB enabled. */
        bcm_l3_egress_ecmp_t_init(&tmp_ecmp);
        tmp_ecmp.ecmp_intf = i + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        /*
         * Increase entry reference count to avoid the failure of calling
         * ecmp_get since the reference count of ecmp group is recovered along
         * with hash value after the recovery of DLB info.
         */
        BCM_XGS3_L3_ENT_REF_CNT_INC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), i, _BCM_SINGLE_WIDE);
        rv = _bcm_esw_l3_egress_ecmp_get(unit, &tmp_ecmp, 0,
                                         NULL, &ecmp_count, TRUE);
        BCM_XGS3_L3_ENT_REF_CNT_DEC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), i, _BCM_SINGLE_WIDE);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_NORMAL &&
            tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED &&
            tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL &&
            tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        if (ecmp_count > member_bitmap_width) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        sal_memset(ecmp_member, 0, member_bitmap_width * sizeof(bcm_if_t));
        /*
         * Increase entry reference count to avoid the failure of calling
         * ecmp_get since the reference count of ecmp group is recovered along
         * with hash value after the recovery of DLB info.
         */
        BCM_XGS3_L3_ENT_REF_CNT_INC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), i, _BCM_SINGLE_WIDE);
        rv = _bcm_esw_l3_egress_ecmp_get(unit, &tmp_ecmp, ecmp_count,
                                         ecmp_member, &count, TRUE);
        BCM_XGS3_L3_ENT_REF_CNT_DEC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), i, _BCM_SINGLE_WIDE);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        for (j = 0; j < ecmp_count; j++) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, ecmp_member[j])) {
                offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, ecmp_member[j])) {
                offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            } else {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            ecmp_member[j] -= offset;
        }

        SHR_BITOR_RANGE(port_map, port_map2, 0, port_map_width, port_map);
        group_member_ptr =
            BCM_XGS3_L3_ECMP_DLB_MEMBER_GROUP_PTR(unit, i);
        for (port = 0; port < port_map_width; port++) {
            if (!SHR_BITGET(port_map, port)){
                continue;
            }

            rv = (READ_DLB_ECMP_GROUP_PORT_TO_MEMBERm(unit, MEM_BLOCK_ANY,
                                               base_addr + port,
                                               &port2member_entry));
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* Get member bitmap */
            sal_memset(member_bitmap, 0, alloc_size_member_bitmap);
            soc_DLB_ECMP_GROUP_PORT_TO_MEMBERm_field_get(unit,
                    &port2member_entry, ECMP_MEMBER_BITMAPf, member_bitmap);

            for (k = 0; k < member_bitmap_width; k++) {
                if (!SHR_BITGET(member_bitmap, k)) {
                    continue;
                }
                if (k >= ecmp_count) {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
                if (!SHR_BITGET(port_map2, port)) {
                    nh_index = ecmp_member[k];
                    SHR_BITSET(group_member_ptr, k);
                } else {  /* an alternate port */
                    nh_index = ecmp_member[k + primary_size];
                    SHR_BITSET(group_member_ptr, k + tmp_ecmp.max_paths / 2);
                }

                /* now have nh_index (ecmp_member[k]), port (port), dlb_id  */
                /* find it nexthop exists in specific port list*/
                current_ptr = ECMP_DLB_PORT_LIST(unit, port);
                first_ptr = NULL;
                while (current_ptr) {
                    if (current_ptr->nh_index != nh_index) {
                        current_ptr = current_ptr->next;
                        continue;
                    }

                    if (dlb_id == current_ptr->dlb_id) {
                        current_ptr->ref_count++;
                        break;
                    }
                    if (first_ptr == NULL) {
                        first_ptr = current_ptr;
                    }

                    current_ptr = current_ptr->next;
                }

                if (current_ptr == NULL) {
                    /* Insert member ID and group to the next hop's linked list of member
                     * IDs and groups.
                     */
                    membership = sal_alloc(sizeof(_th2_ecmp_dlb_port_membership_t),
                            "ecmp dlb nh membership");
                    if (NULL == membership) {
                        rv = BCM_E_MEMORY;
                        goto cleanup;
                    }
                    sal_memset(membership, 0, sizeof(_th2_ecmp_dlb_port_membership_t));
                    membership->nh_index = nh_index;
                    membership->ref_count = 1;
                    membership->dlb_id    = dlb_id;

                    if (first_ptr != NULL) {
                    /* didnt find exact same node(nh_index, dlb_id) ,
                     * but find same nh_index (A).
                     * create B after A.
                     * first_ptr points to A.
                     */
                        membership->next = first_ptr->next;
                        first_ptr->next = membership;
                        ECMP_DLB_PORT_INFO(unit, port).dlb_count++;
                    } else {
                    /* didnt find same nh_index*/
                    /* this is new nh_index, insert it at head */
                        membership->next = ECMP_DLB_PORT_LIST(unit, port);
                        ECMP_DLB_PORT_LIST(unit, port) = membership;
                        ECMP_DLB_PORT_INFO(unit, port).nh_count++;
                        ECMP_DLB_PORT_INFO(unit, port).dlb_count++;
                    }
                }
            }
        }
    }

cleanup:
    if (ecmp_dlb_buf) {
        soc_cm_sfree(unit, ecmp_dlb_buf);
    }
    if (port_map) {
        sal_free(port_map);
    }
    if (port_map2) {
        sal_free(port_map2);
    }
    if (member_bitmap) {
        sal_free(member_bitmap);
    }
    if (ecmp_member) {
        sal_free(ecmp_member);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_group_recover
 * Purpose:
 *      Recover ECMP DLB group usage and flowset table usage.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_group_recover(int unit)
{
    int i;
    ecmp_dlb_control_entry_t *ecmp_dlb_control;
    int group_enable, dlb_id;
    dlb_ecmp_group_control_entry_t group_control_entry;
    int entry_base_ptr, dynamic_size_encode, dynamic_size;
    int block_base_ptr, num_blocks;
    uint32 rval;
    int index_min, index_max,buf_size;
    uint8 *ecmp_dlb_buf = NULL;
    soc_mem_t mem = ECMP_DLB_CONTROLm;
    int rv = BCM_E_NONE;

    if (soc_reg_field_valid(unit, ENHANCED_HASHING_CONTROLr, RH_DLB_SELECTIONf)) {
        SOC_IF_ERROR_RETURN(READ_ENHANCED_HASHING_CONTROLr(unit, &rval));
        if (1 == soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,
                RH_DLB_SELECTIONf)) {
           /* ECMP DLB  hashing is not enabled chip-wide. */
            return BCM_E_NONE;
        }
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    ecmp_dlb_buf = soc_cm_salloc(unit, buf_size, "ECMP_DLB_CONTROL buffer for group");
    if (NULL == ecmp_dlb_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, ecmp_dlb_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = index_min; i <= index_max; i++) {
        ecmp_dlb_control = (ecmp_dlb_control_entry_t *)
                               soc_mem_table_idx_to_pointer
                                  (unit, mem, void *, ecmp_dlb_buf, i);
        group_enable = soc_ECMP_DLB_CONTROLm_field32_get(unit, ecmp_dlb_control,
                GROUP_ENABLEf);
        if (0 == group_enable) {
            /* DLB not enabled */
            continue;
        }

        /* Get DLB group ID and marked it as used */
        dlb_id = soc_ECMP_DLB_CONTROLm_field32_get(unit, ecmp_dlb_control,
                DLB_IDf);
        _BCM_ECMP_DLB_ID_USED_SET(unit, dlb_id);

        /* Get flow set table usage */
        rv = READ_DLB_ECMP_GROUP_CONTROLm(unit,
                    MEM_BLOCK_ANY, dlb_id, &group_control_entry);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        entry_base_ptr = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, FLOW_SET_BASEf);
        dynamic_size_encode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, FLOW_SET_SIZEf);
        rv = _bcm_th2_ecmp_dlb_dynamic_size_decode(dynamic_size_encode,
                                                   &dynamic_size);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        block_base_ptr = entry_base_ptr >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        num_blocks = dynamic_size >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr,
                num_blocks);
#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit, soc_feature_td3_dlb)) {
            ecmp_grp_enhanced_hashing[unit][i] = 1;
        }
#endif
    }

cleanup:
    if (ecmp_dlb_buf) {
        soc_cm_sfree(unit, ecmp_dlb_buf);
    }

    return rv;
}

#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *      _bcm_th3_ecmp_dlb_group_recover
 * Purpose:
 *      Recover ECMP DLB group usage and flowset table usage.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_ecmp_dlb_group_recover(int unit)
{
    int dlb_id;
    uint64 dlb_en_bitmap;
    dlb_ecmp_group_control_entry_t group_control_entry;
    int entry_base_ptr, dynamic_size_encode, dynamic_size;
    int block_base_ptr, num_blocks;
    uint32 rval;

    if (soc_reg_field_valid(unit, ENHANCED_HASHING_CONTROLr, RH_DLB_SELECTIONf)) {
        SOC_IF_ERROR_RETURN(READ_ENHANCED_HASHING_CONTROLr(unit, &rval));
        if (1 == soc_reg_field_get(unit, ENHANCED_HASHING_CONTROLr, rval,
                RH_DLB_SELECTIONf)) {
           /* ECMP DLB  hashing is not enabled chip-wide. */
            return BCM_E_NONE;
        }
    }

    for (dlb_id = 0; dlb_id < 128; dlb_id++) {
        /* Enable bits for the first 64 groups are in
           DLB_ID_0_TO_63_ENABLE register and the next 64 are in
           DLB_ID_64_TO_127_ENABLE. */
        if (dlb_id < soc_reg_field_length(unit,
                     DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
            SOC_IF_ERROR_RETURN( \
              READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
            if (!(COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id))) {
                continue;
            }
        } else {
            SOC_IF_ERROR_RETURN( \
              READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap));
            if (!(COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id -
                soc_reg_field_length(unit, DLB_ID_0_TO_63_ENABLEr, BITMAPf)))) {
                continue;
            }
        }

        /* Get DLB group ID and marked it as used */
        _BCM_ECMP_DLB_ID_USED_SET(unit, dlb_id);

        /* Get flow set table usage */
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_CONTROLm(unit,
                    MEM_BLOCK_ANY, dlb_id, &group_control_entry));
        entry_base_ptr = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, FLOW_SET_BASEf);
        dynamic_size_encode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, FLOW_SET_SIZEf);
        BCM_IF_ERROR_RETURN
            (_bcm_th2_ecmp_dlb_dynamic_size_decode(dynamic_size_encode,
                                                   &dynamic_size));
        block_base_ptr = entry_base_ptr >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        num_blocks = dynamic_size >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
        _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr,
                num_blocks);
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_quality_parameters_recover
 * Purpose:
 *      Recover ECMP DLB parameters used to derive a member's quality.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_ecmp_dlb_quality_parameters_recover(int unit)
{
    uint32 measure_control_reg;
    int num_time_units;
    dlb_ecmp_glb_quantize_threshold_entry_t quantize_threshold_entry;
    int min_th_bytes, min_th_cells;
    int max_th_bytes, max_th_cells;
    int entries_per_profile;
    int profile_ptr, base_index;
    dlb_ecmp_quality_mapping_entry_t quality_mapping_entry;
    int quality;
    int num_profile;

    if (ECMP_DLB_INFO(unit)->recovered_from_scache) {
        /* If already recovered from scache, do nothing. */
        return BCM_E_NONE;
    }

    /* Recover sampling rate */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit,
                &measure_control_reg));
    num_time_units = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, SAMPLING_PERIODf);
    if (num_time_units > 0) {
        ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = 1000000 / num_time_units;
    }

    /* Recover min and max load, queue size and physical queue size thresholds:
     * Load threshold (in mbps)
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (sampling period in seconds) / 10^6
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (num_time_units * 1 us per time unit * 10^-6) / 10^6
     *     = (bytes per sampling period) * 8 / num_time_units
     */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 0, &quantize_threshold_entry));
    min_th_bytes = soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, PORT_LOADING_THRESHOLDf);
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th = (min_th_bytes << 3) /
        num_time_units;

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        min_th_cells = soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_get
            (unit, &quantize_threshold_entry, TOTAL_PORT_QSIZE_THRESHOLDf);
        ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th =
            min_th_cells * _BCM_ECMP_DLB_CELL_BYTES;
    }

    min_th_cells = soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, XPE_PORT_QSIZE_THRESHOLDf);
    ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th =
            min_th_cells * _BCM_ECMP_DLB_CELL_BYTES;

    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, soc_mem_index_max(unit,
                    DLB_ECMP_GLB_QUANTIZE_THRESHOLDm),
                &quantize_threshold_entry));
    max_th_bytes = soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, PORT_LOADING_THRESHOLDf);
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th = (max_th_bytes << 3) /
        num_time_units;

    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        max_th_cells = soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_get
            (unit, &quantize_threshold_entry, TOTAL_PORT_QSIZE_THRESHOLDf);
        ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th =
            max_th_cells * _BCM_ECMP_DLB_CELL_BYTES;
    }

    max_th_cells = soc_DLB_ECMP_GLB_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, XPE_PORT_QSIZE_THRESHOLDf);
    ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th =
            max_th_cells * _BCM_ECMP_DLB_CELL_BYTES;

    /* Recover load weights and qsize weights */
    num_profile = 1 << soc_mem_field_length(unit, DLB_ECMP_QUANTIZE_CONTROLm,
                                            PORT_QUALITY_MAPPING_PROFILE_PTRf);
    entries_per_profile =
        soc_mem_index_count(unit, DLB_ECMP_PORT_QUALITY_MAPPINGm) / num_profile;
    for (profile_ptr = 0; profile_ptr < num_profile; profile_ptr++) {
        base_index = profile_ptr * entries_per_profile;
        /* quality = (quantized_tx_load * tx_load_percent +
         *            quantized_qsize * qsize_percent +
         *            quantized_physical_qsize *
         *            (100 - tx_load_percent - qsize_percent)) / 100;
         * The index to DLB_ECMP_PORT_QUALITY_MAPPING table consists of
         * base_index + quantized_tx_load * 64 +
         * quantized_qsize * 8 + quantized_physical_qsize,
         * where quantized_tx_load, quantized_qsize and quantized_physical_qsize
         * range from 0 to 7.
         *
         * With quantized_tx_load = 7, quantized_qsize = quantized_physical_qsize = 0,
         * the index to DLB_ECMP_PORT_QUALITY_MAPPING table is
         * base_index + 448, and the tx_load_percent expression
         * simplifies to quality * 100 / 7;
         *
         * With quantized_qsize = 7, quantized_tx_load = quantized_physical_qsize = 0,
         * the index to DLB_ECMP_PORT_QUALITY_MAPPING table is
         * base_index + 56, and the qsize_percent expression
         * simplifies to quality * 100 / 7;
         */
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_PORT_QUALITY_MAPPINGm(unit,
                    MEM_BLOCK_ANY, base_index + 448,
                    &quality_mapping_entry));
        quality = soc_DLB_ECMP_PORT_QUALITY_MAPPINGm_field32_get(unit,
                &quality_mapping_entry, ASSIGNED_QUALITYf);
        ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[profile_ptr] =
            quality * 100 / 7;
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_PORT_QUALITY_MAPPINGm(unit,
                    MEM_BLOCK_ANY, base_index + 56,
                    &quality_mapping_entry));
        quality = soc_DLB_ECMP_PORT_QUALITY_MAPPINGm_field32_get(unit,
                &quality_mapping_entry, ASSIGNED_QUALITYf);
        ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_weight[profile_ptr] =
            quality * 100 / 7;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_ecmp_dlb_hw_recover
 * Purpose:
 *      Recover ECMP DLB internal state from hardware.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      scache_ptr  - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_ecmp_dlb_hw_recover(int unit)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_dlb)) {

        BCM_IF_ERROR_RETURN(_bcm_th3_ecmp_dlb_member_recover(unit));
        BCM_IF_ERROR_RETURN(_bcm_th3_ecmp_dlb_group_recover(unit));
    } else
#endif
    {

        BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_member_recover(unit));
        BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_group_recover(unit));
    }
    BCM_IF_ERROR_RETURN(_bcm_th2_ecmp_dlb_quality_parameters_recover(unit));
    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */


#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_th2_ecmp_dlb_sw_dump
 * Purpose:
 *     Displays ECMP DLB information maintained by software.
 * Parameters:
 *     unit    - (IN) Device unit number.
 * Returns:
 *     None
 */
void
bcm_th2_ecmp_dlb_sw_dump(int unit)
{
    int i;
    _th2_ecmp_dlb_port_membership_t *current_ptr;
    int num_entries_per_profile;
    int num_profiles;
    int rv;
    int ref_count;

    LOG_CLI((BSL_META_U(unit,
                        "  ECMP DLB Info -\n")));

    /* Print ECMP DLB next hop info */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Port Info -\n")));
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_port_info_size; i++) {
        if (ECMP_DLB_PORT_INFO(unit, i).nh_count > 0) {
            LOG_CLI((BSL_META_U(unit,
                                "      Port Number %d output by "),
                     i));
            current_ptr = ECMP_DLB_PORT_LIST(unit, i);
            while (NULL != current_ptr) {
                LOG_CLI((BSL_META_U(unit,
                                    "nexthop %d in DLB ID %d Ref_count %d"),
                         current_ptr->nh_index, current_ptr->dlb_id, current_ptr->ref_count));
                current_ptr = current_ptr->next;
                if (NULL == current_ptr) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        ", ")));
                }
            }
        }
    }

    /* Print ECMP DLB group usage */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Groups Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm); i++) {
        if (_BCM_ECMP_DLB_ID_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print ECMP DLB flowset table usage */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Flowset Table Blocks Used:")));
    for (i = 0;
         i < soc_mem_index_count(unit, DLB_ECMP_FLOWSETm) >>
                            _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
         i++) {
        if (_BCM_ECMP_DLB_FLOWSET_BLOCK_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print sample rate and threshold parameters */
    LOG_CLI((BSL_META_U(unit,
                        "    Sample rate: %d per second\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load min threshold: %d mbps\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load max threshold: %d mbps\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th));
    if (!soc_feature(unit, soc_feature_td3_dlb)) {
        LOG_CLI((BSL_META_U(unit,
                        "    Total Queue size min threshold: %d cells\n"),
                    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th));
        LOG_CLI((BSL_META_U(unit,
                        "    Total Queue size max threshold: %d cells\n"),
                    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th));
    }
    LOG_CLI((BSL_META_U(unit,
                        "    Physical Queue size min threshold: %d cells\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Physical Queue size max threshold: %d cells\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_physical_qsize_max_th));

    /* Print quality mapping profiles */
    LOG_CLI((BSL_META_U(unit,
                        "    Quality mapping profiles:\n")));
    num_entries_per_profile = soc_mem_index_count(unit, DLB_ECMP_PORT_QUALITY_MAPPINGm)/
        (1 << soc_mem_field_length(unit, DLB_ECMP_QUANTIZE_CONTROLm,
                                   PORT_QUALITY_MAPPING_PROFILE_PTRf));
    num_profiles = soc_mem_index_count(unit, DLB_ECMP_PORT_QUALITY_MAPPINGm) /
                    num_entries_per_profile;
    for (i = 0; i < num_profiles; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "      Profile %d: load weight %d percent, "\
                            "qsize weight %d percent, "),
                 i, ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[i],
                 ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_weight[i]));
        rv = soc_profile_mem_ref_count_get(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_quality_map_profile,
                i * num_entries_per_profile, &ref_count);
        if (BCM_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "ref count %d\n"), ref_count));
    }

    return;
}

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
void
bcm_th2_dgm_sw_dump(int unit)
{
    int i;
    LOG_CLI((BSL_META_U(unit,
                        "  DGM Info -\n")));

    LOG_CLI((BSL_META_U(unit,
                        "    DGM Alternate Count:(ECMP Group- Alternate Count)")));
    for (i = 0; i < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); i++) {
        if (BCM_XGS3_L3_ALTERNATE_CNT_PTR(unit)[i] != 0) {
            LOG_CLI((BSL_META_U(unit,
                                " %d-%d"), i,
                     BCM_XGS3_L3_ALTERNATE_CNT_PTR(unit)[i]));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    if (soc_mem_is_valid(unit, DLB_ECMP_GROUP_CONTROLm)) {
        /* Print ECMP DGM group usage */
        LOG_CLI((BSL_META_U(unit,
                            "    DGM Groups Used:")));
        for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm); i++) {
            if (_BCM_ECMP_DLB_ID_DGM_GET(unit, i)) {
                LOG_CLI((BSL_META_U(unit,
                                    " %d"), i));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
}
#endif
#endif /* ifndef BCM_SW_STATE_DUMP_DISABLE*/

#endif /* INCLUDE_L3 && BCM_TOMAHAWK2_SUPPORT  */

