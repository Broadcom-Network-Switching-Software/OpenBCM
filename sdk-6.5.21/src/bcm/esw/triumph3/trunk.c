/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Triumph3 LAG dynamic load balancing
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#ifdef BCM_TRIUMPH3_SUPPORT 

#include <soc/mem.h>
#include <soc/profile_mem.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw_dispatch.h>

/* ------------------------------------------------- */
/* Bookkeeping info for LAG dynamic load balancing */
/* ------------------------------------------------- */

typedef struct _tr3_lag_dlb_bookkeeping_s {
    SHR_BITDCL *lag_dlb_id_used_bitmap;
    SHR_BITDCL *lag_dlb_flowset_block_bitmap; /* Each block corresponds to
                                                 512 entries */
    SHR_BITDCL *lag_dlb_member_id_used_bitmap;
    int lag_dlb_sample_rate;
    int lag_dlb_tx_load_min_th;
    int lag_dlb_tx_load_max_th;
    int lag_dlb_qsize_min_th;
    int lag_dlb_qsize_max_th;
    int lag_dlb_exp_load_min_th;
    int lag_dlb_exp_load_max_th;
    int lag_dlb_imbalance_min_th;
    int lag_dlb_imbalance_max_th;
    uint8 *lag_dlb_load_weight;
    soc_profile_mem_t *lag_dlb_quality_map_profile;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8 recovered_from_scache; /* Indicates if the following members of this
                                    structure have been recovered from scached:
                                    lag_dlb_sample_rate,
                                    lag_dlb_tx_load_min_th,
                                    lag_dlb_tx_load_max_th,
                                    lag_dlb_tx_qsize_min_th,
                                    lag_dlb_tx_qsize_max_th,
                                    lag_dlb_exp_load_min_th,
                                    lag_dlb_exp_load_max_th,
                                    lag_dlb_imbalance_min_th,
                                    lag_dlb_imbalance_max_th,
                                    lag_dlb_load_weight */
#endif /* BCM_WARM_BOOT_SUPPORT */
} _tr3_lag_dlb_bookkeeping_t;

static _tr3_lag_dlb_bookkeeping_t *_tr3_lag_dlb_bk[BCM_MAX_NUM_UNITS];

#define LAG_DLB_INFO(_unit_) (_tr3_lag_dlb_bk[_unit_])

#define _BCM_LAG_DLB_ID_USED_GET(_u_, _idx_) \
    SHR_BITGET(LAG_DLB_INFO(_u_)->lag_dlb_id_used_bitmap, _idx_)
#define _BCM_LAG_DLB_ID_USED_SET(_u_, _idx_) \
    SHR_BITSET(LAG_DLB_INFO(_u_)->lag_dlb_id_used_bitmap, _idx_)
#define _BCM_LAG_DLB_ID_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(LAG_DLB_INFO(_u_)->lag_dlb_id_used_bitmap, _idx_)

#define _BCM_LAG_DLB_FLOWSET_BLOCK_USED_GET(_u_, _idx_) \
    SHR_BITGET(LAG_DLB_INFO(_u_)->lag_dlb_flowset_block_bitmap, _idx_)
#define _BCM_LAG_DLB_FLOWSET_BLOCK_USED_SET(_u_, _idx_) \
    SHR_BITSET(LAG_DLB_INFO(_u_)->lag_dlb_flowset_block_bitmap, _idx_)
#define _BCM_LAG_DLB_FLOWSET_BLOCK_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(LAG_DLB_INFO(_u_)->lag_dlb_flowset_block_bitmap, _idx_)
#define _BCM_LAG_DLB_FLOWSET_BLOCK_USED_SET_RANGE(_u_, _idx_, _count_) \
    SHR_BITSET_RANGE(LAG_DLB_INFO(_u_)->lag_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_LAG_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(_u_, _idx_, _count_) \
    SHR_BITCLR_RANGE(LAG_DLB_INFO(_u_)->lag_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_LAG_DLB_FLOWSET_BLOCK_TEST_RANGE(_u_, _idx_, _count_, _result_) \
    SHR_BITTEST_RANGE(LAG_DLB_INFO(_u_)->lag_dlb_flowset_block_bitmap, _idx_, _count_, _result_)

#define _BCM_LAG_DLB_MEMBER_ID_USED_GET(_u_, _idx_) \
    SHR_BITGET(LAG_DLB_INFO(_u_)->lag_dlb_member_id_used_bitmap, _idx_)
#define _BCM_LAG_DLB_MEMBER_ID_USED_SET(_u_, _idx_) \
    SHR_BITSET(LAG_DLB_INFO(_u_)->lag_dlb_member_id_used_bitmap, _idx_)
#define _BCM_LAG_DLB_MEMBER_ID_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(LAG_DLB_INFO(_u_)->lag_dlb_member_id_used_bitmap, _idx_)

/* Function prototypes */

STATIC int _bcm_tr3_lag_dlb_quality_assign(int unit, int tx_load_percent,
        uint32 *entry_arr);
STATIC int _bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set(int unit,
        int sample_rate, int min_th, int max_th);
STATIC int _bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(int unit,
        int sample_rate, int exp_load_min_th, int exp_load_max_th,
        int imbalance_min_th, int imbalance_max_th);
STATIC int _bcm_tr3_lag_dlb_qsize_thresholds_set(int unit, int min_th,
        int max_th);


/*
 * Function:
 *      _bcm_tr3_lag_dlb_quality_map_profile_init
 * Purpose:
 *      Initialize LAG DLB quality mapping profile.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_tr3_lag_dlb_quality_map_profile_init(int unit) 
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_words;
    int entries_per_profile;
    int tx_load_percent;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    int i;
    int member_id;
    dlb_lag_quality_control_entry_t quality_control_entry;

    if (NULL == _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile) {
        _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile =
            sal_alloc(sizeof(soc_profile_mem_t),
                      "LAG DLB Quality Map Profile Mem");
        if (NULL == _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile) {
            return BCM_E_MEMORY;
        }
    } else {
        soc_profile_mem_destroy(unit,
                _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile);
    }
    profile = _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile;
    soc_profile_mem_t_init(profile);

    mem = DLB_LAG_QUALITY_MAPPINGm;
    entry_words = sizeof(dlb_lag_quality_mapping_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1, profile));

    entries_per_profile = 64;
    if (!SOC_WARM_BOOT(unit)) {
        alloc_size = entries_per_profile * entry_words * sizeof(uint32);
        entry_arr = sal_alloc(alloc_size, "LAG DLB Quality Map entries");
        if (entry_arr == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(entry_arr, 0, alloc_size);

        /* Assign quality in the entry array, with 100% weight for port load,
         * 0% for queue size.
         */
        tx_load_percent = 100;
        rv = _bcm_tr3_lag_dlb_quality_assign(unit, tx_load_percent,
                entry_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_arr);
            return rv;
        }

        entries = entry_arr;
        rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
                &base_index);
        sal_free(entry_arr);
        if (SOC_FAILURE(rv)) {
            return rv;
        }

        for (member_id = 0;
                member_id < soc_mem_index_count(unit,
                    DLB_LAG_QUALITY_CONTROLm);
                member_id++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                                               member_id,
                                               &quality_control_entry));
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, PROFILE_PTRf,
                    base_index / entries_per_profile);
            BCM_IF_ERROR_RETURN
                (WRITE_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL,
                                                member_id,
                                                &quality_control_entry));
        }  

        for (i = 0; i < entries_per_profile; i++) {
            SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i,
                    member_id - 1);
        }

        LAG_DLB_INFO(unit)->
            lag_dlb_load_weight[base_index/entries_per_profile] = 
                tx_load_percent;

    } else { /* Warm boot, recover reference counts and entries_per_set */

        for (member_id = 0;
                member_id < soc_mem_index_count(unit,
                    DLB_LAG_QUALITY_CONTROLm);
                member_id++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                                               member_id,
                                               &quality_control_entry));
            base_index = soc_DLB_LAG_QUALITY_CONTROLm_field32_get(unit,
                    &quality_control_entry, PROFILE_PTRf);
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
 *      bcm_tr3_lag_dlb_deinit
 * Purpose:
 *      Deallocate _tr3_lag_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
void
bcm_tr3_lag_dlb_deinit(int unit) 
{
    int rv;
    uint32 rsel2_hw_control_reg;

    if (_tr3_lag_dlb_bk[unit]) {
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap) {
            sal_free(_tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap);
            _tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap = NULL;
        }
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap) {
            sal_free(_tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap);
            _tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap = NULL;
        }
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap) {
            sal_free(_tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap);
            _tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap = NULL;
        }
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_load_weight) {
            sal_free(_tr3_lag_dlb_bk[unit]->lag_dlb_load_weight);
            _tr3_lag_dlb_bk[unit]->lag_dlb_load_weight = NULL;
        }
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile) {
            soc_profile_mem_destroy(unit,
                    _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile);
            sal_free(_tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile);
            _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile = NULL;
        }

        sal_free(_tr3_lag_dlb_bk[unit]);
        _tr3_lag_dlb_bk[unit] = NULL;
    }

    rv = READ_RSEL2_HW_CONTROLr(unit, &rsel2_hw_control_reg);
    if (SOC_SUCCESS(rv)) {
        soc_reg_field_set(unit, RSEL2_HW_CONTROLr, &rsel2_hw_control_reg,
                ENABLE_LAG_DLBf, 0);
        (void)WRITE_RSEL2_HW_CONTROLr(unit, rsel2_hw_control_reg);
    }
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_init
 * Purpose:
 *      Allocate and initialize _tr3_lag_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_tr3_lag_dlb_init(int unit) 
{
    int rv = BCM_E_NONE;
    int num_lag_dlb_id;
    int lag_flowset_tbl_size;
    int total_num_blocks;
    int num_lag_dlb_member_id;
    int num_quality_map_profiles;
    int member_id;
    dlb_lag_quality_control_entry_t quality_control_entry;
    uint32 rsel2_hw_control_reg;
    uint32 measure_control_reg;
    int sample_rate;

    if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
        return BCM_E_NONE;
    } 

    if (_tr3_lag_dlb_bk[unit] == NULL) {
        _tr3_lag_dlb_bk[unit] = sal_alloc(
                sizeof(_tr3_lag_dlb_bookkeeping_t), "_tr3_lag_dlb_bk");
        if (_tr3_lag_dlb_bk[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_lag_dlb_bk[unit], 0, sizeof(_tr3_lag_dlb_bookkeeping_t));

    num_lag_dlb_id = soc_mem_index_count(unit, DLB_LAG_GROUP_CONTROLm);
    if (_tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap == NULL) {
        _tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(num_lag_dlb_id),
                    "lag_dlb_id_used_bitmap");
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap == NULL) {
            bcm_tr3_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_lag_dlb_bk[unit]->lag_dlb_id_used_bitmap, 0,
            SHR_BITALLOCSIZE(num_lag_dlb_id));

    lag_flowset_tbl_size = soc_mem_index_count(unit, DLB_LAG_FLOWSETm);
    /* Each bit in lag_dlb_flowset_block_bitmap corresponds to a block of 512
     * flow set table entries.
     */
    total_num_blocks = lag_flowset_tbl_size / 512;
    if (_tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap == NULL) {
        _tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(total_num_blocks),
                    "lag_dlb_flowset_block_bitmap");
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap == NULL) {
            bcm_tr3_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_lag_dlb_bk[unit]->lag_dlb_flowset_block_bitmap, 0,
            SHR_BITALLOCSIZE(total_num_blocks));

    num_lag_dlb_member_id = soc_mem_index_count(unit,
            DLB_LAG_MEMBER_ATTRIBUTEm);
    if (_tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap == NULL) {
        _tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(num_lag_dlb_member_id),
                    "lag_dlb_member_id_used_bitmap");
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap == NULL) {
            bcm_tr3_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_lag_dlb_bk[unit]->lag_dlb_member_id_used_bitmap, 0,
                SHR_BITALLOCSIZE(num_lag_dlb_member_id));

    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
            DLB_LAG_QUALITY_CONTROLm, PROFILE_PTRf);
    if (_tr3_lag_dlb_bk[unit]->lag_dlb_load_weight == NULL) {
        _tr3_lag_dlb_bk[unit]->lag_dlb_load_weight = 
            sal_alloc(num_quality_map_profiles * sizeof(uint8),
                    "lag_dlb_load_weight");
        if (_tr3_lag_dlb_bk[unit]->lag_dlb_load_weight == NULL) {
            bcm_tr3_lag_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_lag_dlb_bk[unit]->lag_dlb_load_weight, 0,
            num_quality_map_profiles * sizeof(uint8));

    /* Initialize port quality mapping profile */
    rv = _bcm_tr3_lag_dlb_quality_map_profile_init(unit);
    if (BCM_FAILURE(rv)) {
        bcm_tr3_lag_dlb_deinit(unit);
        return rv;
    }

    if (!SOC_WARM_BOOT(unit)) {

        /* Configure accounting mode to physical link accounting */
        rv = bcm_tr3_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicAccountingSelect, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Set DLB sampling period and load thresholds */
        sample_rate = 1000;
        rv = _bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set(unit, sample_rate,
                125, 875);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }
        rv = _bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(unit, sample_rate,
                125, 875, -75, 75);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB load EWMA weight */
        rv = bcm_tr3_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicEgressBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB queue size EWMA weight */
        rv = bcm_tr3_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicQueuedBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB load cap control */
        rv = bcm_tr3_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicEgressBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB queue size cap control */
        rv = bcm_tr3_lag_dlb_config_set(unit,
                bcmSwitchTrunkDynamicQueuedBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB queue size thresholds */
        rv = _bcm_tr3_lag_dlb_qsize_thresholds_set(unit, 0, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Disable link status override by software */
        rv = soc_mem_clear(unit, DLB_LAG_MEMBER_SW_STATEm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Clear group membership */
        rv = soc_mem_clear(unit, DLB_LAG_GROUP_MEMBERSHIPm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Clear port<->member mapping tables */
        rv = soc_mem_clear(unit, DLB_LAG_MEMBERSHIP_REVERSE_MAPm,
                MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }
        rv = soc_mem_clear(unit, DLB_LAG_MEMBER_ATTRIBUTEm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure per member threshold scaling factor and 
         * quality measure update control.
         */
        for (member_id = 0;
                member_id < soc_mem_index_count(unit,
                    DLB_LAG_QUALITY_CONTROLm);
                member_id++) {
            rv = READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                    member_id, &quality_control_entry);
            if (SOC_FAILURE(rv)) {
                bcm_tr3_lag_dlb_deinit(unit);
                return rv;
            }
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, ENABLE_AVG_CALCf, 1);
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, ENABLE_QUALITY_UPDATEf, 1);
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, ENABLE_CREDIT_COLLECTIONf, 1);
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, LOADING_SCALING_FACTORf, 1);
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, QSIZE_SCALING_FACTORf, 1);
            rv = WRITE_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL,
                    member_id, &quality_control_entry);
            if (SOC_FAILURE(rv)) {
                bcm_tr3_lag_dlb_deinit(unit);
                return rv;
            }
        }

        /* Enable DLB HGT refresh */

        rv = READ_RSEL2_HW_CONTROLr(unit, &rsel2_hw_control_reg);
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, RSEL2_HW_CONTROLr, &rsel2_hw_control_reg,
                    ENABLE_LAG_DLBf, 1);
            rv = WRITE_RSEL2_HW_CONTROLr(unit, rsel2_hw_control_reg);
        }
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

        /* Configure Ethertype eligibility */

        rv = soc_mem_clear(unit, DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm,
                MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }
        rv = READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit,
                &measure_control_reg);
        if (SOC_SUCCESS(rv)) {
            soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
                    &measure_control_reg,
                    ETHERTYPE_ELIGIBILITY_CONFIGf, 0);
            soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
                    &measure_control_reg,
                    INNER_OUTER_ETHERTYPE_SELECTIONf, 0);
            rv = WRITE_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit,
                    measure_control_reg);
        }
        if (BCM_FAILURE(rv)) {
            bcm_tr3_lag_dlb_deinit(unit);
            return rv;
        }

    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_accounting_set
 * Purpose:
 *      Set LAG DLB accounting mode.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      accounting_mode - (IN) Accounting mode.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_accounting_set(int unit, int accounting_mode)
{
    uint32 measure_control_reg;
    int old_accounting_mode;
    SHR_BITDCL active;

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    old_accounting_mode = soc_reg_field_get(unit,
            DLB_LAG_QUALITY_MEASURE_CONTROLr, measure_control_reg,
            ACCOUNTING_SELf);
    if (old_accounting_mode == accounting_mode) {
        return BCM_E_NONE;
    }

    SHR_BITTEST_RANGE(LAG_DLB_INFO(unit)->lag_dlb_id_used_bitmap,
            0, soc_mem_index_count(unit, DLB_LAG_GROUP_CONTROLm), active);
    if (active) {
        /* Accounting mode cannot be changed while there are active
         * DLB groups.
         */
        return BCM_E_BUSY;
    }

    soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, ACCOUNTING_SELf, accounting_mode);
    if (accounting_mode) { /* virtual link accounting */
        soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
                &measure_control_reg, INST_METRIC_UPDATE_INTERVALf, 1);
    } else { /* physical link accounting */
        soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
                &measure_control_reg, INST_METRIC_UPDATE_INTERVALf, 0);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_accounting_get
 * Purpose:
 *      Get LAG DLB accounting mode.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      accounting_mode - (OUT) Accounting mode.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_accounting_get(int unit, int *accounting_mode)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *accounting_mode = soc_reg_field_get(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, ACCOUNTING_SELf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set
 * Purpose:
 *      Set LAG DLB sample period and physical link accounting thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 *      min_th      - (IN) Minimum port load threshold, in mbps.
 *      max_th      - (IN) Maximum port load threshold, in mbps.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set(int unit, int sample_rate,
        int min_th, int max_th)
{
    int num_time_units;
    uint32 measure_control_reg;
    int max_th_bytes;
    int th_increment;
    dlb_lag_pla_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_bytes[7];

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
    if (num_time_units < 1 || num_time_units > 0x3fff) {
        /* Hardware limits the sampling period to 14 bits */ 
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, HISTORICAL_SAMPLING_PERIODf, num_time_units);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    LAG_DLB_INFO(unit)->lag_dlb_sample_rate = sample_rate;

    /* Compute threshold in bytes per sampling period:
     * bytes per 1us = (million bits per sec) * 10^6 / 8 * 10^(-6),
     * bytes per sampling period = (bytes per 1us) * (number of 1us 
     *                             time units in sampling period)
     *                           = mbps * num_time_units / 8
     */
    max_th_bytes = max_th * num_time_units / 8;

    if (max_th_bytes > 0x3fffffff) {
        /* Hardware limits port load threshold to 30 bits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        SOC_IF_ERROR_RETURN
            (READ_DLB_LAG_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_bytes[i] = (min_th + i * th_increment) * num_time_units / 8;
        soc_DLB_LAG_PLA_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, THRESHOLD_HIST_LOADf,
             th_bytes[i]);
        SOC_IF_ERROR_RETURN
            (WRITE_DLB_LAG_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th = min_th;
    LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set
 * Purpose:
 *      Set LAG DLB sample period and virtual link accounting thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 *      exp_load_min_th  - (IN) Minimum expected load threshold, in mbps.
 *      exp_load_max_th  - (IN) Maximum expected load threshold, in mbps.
 *      imbalance_min_th - (IN) Minimum member imbalance threshold,
 *                              in percentage.
 *      imbalance_max_th - (IN) Maximum member imbalance threshold.
 *                              in percentage.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(int unit, int sample_rate,
        int exp_load_min_th, int exp_load_max_th,
        int imbalance_min_th, int imbalance_max_th)
{
    int num_time_units;
    uint32 measure_control_reg;
    int exp_load_max_th_kbytes;
    int num_exp_load_th;
    int exp_load_th_increment;
    dlb_lag_vla_expected_loading_threshold_entry_t exp_load_threshold_entry;
    int exp_load_th_kbytes;
    int num_imbalance_th;
    int imbalance_th_increment;
    int imbalance_max_th_kbytes;
    int imbalance_th_kbytes;
    dlb_lag_vla_member_imbalance_threshold_entry_t imbalance_threshold_entry;
    int i, j;
    int imbalance_th_fields[7] = {THRESHOLD_0f, THRESHOLD_1f, THRESHOLD_2f,
                                  THRESHOLD_3f, THRESHOLD_4f, THRESHOLD_5f,
                                  THRESHOLD_6f};

    if (sample_rate <= 0 || exp_load_min_th < 0 || exp_load_max_th < 0) {
        return BCM_E_PARAM;
    }

    if (exp_load_min_th > exp_load_max_th) {
        exp_load_max_th = exp_load_min_th;
    }

    if (imbalance_min_th > imbalance_max_th) {
        imbalance_max_th = imbalance_min_th;
    }

    /* Compute sampling period in units of 1us:
     *     number of 1us time units = 10^6 / sample_rate
     */ 
    num_time_units = 1000000 / sample_rate;
    if (num_time_units < 1 || num_time_units > 0x3fff) {
        /* Hardware limits the sampling period to 14 bits */ 
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, HISTORICAL_SAMPLING_PERIODf, num_time_units);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    LAG_DLB_INFO(unit)->lag_dlb_sample_rate = sample_rate;

    /* Compute expected loading threshold in kbytes per sampling period:
     * bytes per 1us = (million bits per sec) * 10^6 / 8 * 10^(-6)
     *               = mbps / 8.
     * bytes per sampling period = (bytes per 1us) * (number of 1us 
     *                             time units in sampling period)
     *                           = mbps * num_time_units / 8.
     * kbytes per sampling period =  mbps * num_times_units / 8000.
     */
    exp_load_max_th_kbytes = exp_load_max_th * num_time_units / 8000;
    if (exp_load_max_th_kbytes > 0xfffff) {
        /* Hardware limits expected loading threshold in kbytes to 20 bits */
        return BCM_E_PARAM;
    }

    /* Use expected load min threshold as threshold 0, and max threshold as
     * threshold 14. Thresholds 1 to 13 will be evenly spread out between them.
     */
    num_exp_load_th = soc_mem_index_count(unit,
            DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm);
    exp_load_th_increment = (exp_load_max_th - exp_load_min_th) /
                            (num_exp_load_th - 1);
    for (i = 0; i < num_exp_load_th; i++) {
        SOC_IF_ERROR_RETURN(READ_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                    MEM_BLOCK_ANY, i, &exp_load_threshold_entry));
        exp_load_th_kbytes = (exp_load_min_th + i * exp_load_th_increment) *
                             num_time_units / 8000;
        soc_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm_field32_set
            (unit, &exp_load_threshold_entry, THRESHOLDf, exp_load_th_kbytes);
        SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                    MEM_BLOCK_ALL, i, &exp_load_threshold_entry));
    }

    LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th = exp_load_min_th;
    LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th = exp_load_max_th;

    /* Use member imbalance min threshold as threshold 0, and max threshold as
     * threshold 6. Thresholds 1 to 5 will be evenly spread out between them.
     */
    num_imbalance_th = 7;
    imbalance_th_increment = (imbalance_max_th - imbalance_min_th) /
        (num_imbalance_th - 1);
    for (i = 0;
         i < soc_mem_index_count(unit,
             DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm);
         i++) {
        /* There are 16 sets of member imbalance thresholds.
         * For sets 0 to 14, use expected loading thresholds 0 to 14 from
         * DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLD table as the
         * expected load. For set 15, extrapolate the expected load.
         */
        if (i < num_exp_load_th) {
            SOC_IF_ERROR_RETURN
                (READ_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                        MEM_BLOCK_ANY, i, &exp_load_threshold_entry));
            exp_load_th_kbytes =
                soc_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm_field32_get(unit,
                        &exp_load_threshold_entry, THRESHOLDf);
        } else {
            exp_load_th_kbytes =
                (exp_load_min_th + num_exp_load_th * exp_load_th_increment) *
                num_time_units / 8000;
        }

        /* Compute member imbalance thresholds */
        imbalance_max_th_kbytes = exp_load_th_kbytes * imbalance_max_th / 100;
        if (imbalance_max_th_kbytes > 0xfffff) {
            /* Hardware limits imbalance threshold in kbytes to 1 sign bit
             * plus 20 bits.
             */
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(READ_DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm(unit,
                    MEM_BLOCK_ANY, i, &imbalance_threshold_entry));
        for (j = 0; j < num_imbalance_th; j++) {
            imbalance_th_kbytes = exp_load_th_kbytes *
                (imbalance_min_th + j * imbalance_th_increment) / 100;
            if (imbalance_th_kbytes < 0) {
                /* Convert to two's complement representation */
                imbalance_th_kbytes = -1 * imbalance_th_kbytes;
                imbalance_th_kbytes = (~imbalance_th_kbytes) + 1;
            } 
            soc_DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm_field32_set(unit,
                    &imbalance_threshold_entry, imbalance_th_fields[j],
                    imbalance_th_kbytes & 0x1fffff);
        }
        SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm(unit,
                    MEM_BLOCK_ALL, i, &imbalance_threshold_entry));
    }

    LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th = imbalance_min_th;
    LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th = imbalance_max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_sample_rate_set
 * Purpose:
 *      Set LAG dynamic load balancing sample rate.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_sample_rate_set(int unit, int sample_rate)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set(unit,
                sample_rate, LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th));

    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(unit,
                sample_rate, LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_tx_load_min_th_set
 * Purpose:
 *      Set LAG DLB port load minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_tx_load_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_sample_rate, min_th,
                LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_lag_dlb_tx_load_max_th_set
 * Purpose:
 *      Set LAG DLB port load maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_tx_load_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_pla_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_sample_rate,
                LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th,
                max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_exp_load_min_th_set
 * Purpose:
 *      Set LAG DLB expected load minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum expected load threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_exp_load_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_sample_rate,
                min_th,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_exp_load_max_th_set
 * Purpose:
 *      Set LAG DLB expected load maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum expected load threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_exp_load_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_sample_rate,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th,
                max_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_imbalance_min_th_set
 * Purpose:
 *      Set LAG DLB member imbalance minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum member imbalance threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_imbalance_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_sample_rate,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th,
                min_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_imbalance_max_th_set
 * Purpose:
 *      Set LAG DLB member imbalance maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum member imbalance threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_imbalance_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_sample_rate_vla_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_sample_rate,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th,
                LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th,
                LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th,
                max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_thresholds_set
 * Purpose:
 *      Set LAG DLB queue size thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum queue size threshold, in bytes.
 *      max_th - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_thresholds_set(int unit, int min_th, int max_th)
{
    int max_th_cells;
    int th_increment;
    dlb_hgt_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_cells[7];

    if (min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Convert threshold to number of cells */
    max_th_cells = max_th / 208;
    if (max_th_cells > 0xffff) {
        /* Hardware limits queue size threshold to 16 bits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        SOC_IF_ERROR_RETURN
            (READ_DLB_LAG_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_cells[i] = (min_th + i * th_increment) / 208;
        soc_DLB_LAG_PLA_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, THRESHOLD_HIST_QSIZEf,
             th_cells[i]);
        SOC_IF_ERROR_RETURN
            (WRITE_DLB_LAG_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th = min_th;
    LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_min_th_set
 * Purpose:
 *      Set LAG DLB queue size minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_qsize_thresholds_set(unit,
                min_th, LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_max_th_set
 * Purpose:
 *      Set LAG DLB queue size maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_qsize_thresholds_set(unit,
                LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th, max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_tx_load_weight_set
 * Purpose:
 *      Set LAG DLB port load weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_tx_load_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;

    if (weight < 0 || 
        weight > 0xf) {
        /* Hardware limits port load weight to 4 bits */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_WEIGHT_LOADINGf, weight);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_tx_load_weight_get
 * Purpose:
 *      Get LAG DLB port load weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_tx_load_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_WEIGHT_LOADINGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_weight_set
 * Purpose:
 *      Set LAG DLB qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;

    if (weight < 0 || 
        weight > 0xf) {
        /* Hardware limits qsize weight to 4 bits */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_WEIGHT_QSIZEf, weight);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_weight_get
 * Purpose:
 *      Get LAG DLB qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_WEIGHT_QSIZEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_tx_load_cap_set
 * Purpose:
 *      Set LAG DLB port load cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Port load cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_tx_load_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 || 
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_CAP_LOADING_AVGf, cap);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_tx_load_cap_get
 * Purpose:
 *      Get LAG DLB port load cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_tx_load_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_CAP_LOADING_AVGf);

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_cap_set
 * Purpose:
 *      Set LAG DLB queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 || 
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_CAP_QSIZE_AVGf, cap);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_qsize_cap_get
 * Purpose:
 *      Get LAG DLB queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_qsize_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_LAG_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_CAP_QSIZE_AVGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_config_set
 * Purpose:
 *      Set per-chip LAG dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (IN) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_config_set(int unit, bcm_switch_control_t type, int arg)
{
    switch (type) {
        case bcmSwitchTrunkDynamicSampleRate:
            return _bcm_tr3_lag_dlb_sample_rate_set(unit, arg);

        case bcmSwitchTrunkDynamicAccountingSelect:
            return _bcm_tr3_lag_dlb_accounting_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesExponent:
            return _bcm_tr3_lag_dlb_tx_load_weight_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesExponent:
            return _bcm_tr3_lag_dlb_qsize_weight_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesDecreaseReset:
            return _bcm_tr3_lag_dlb_tx_load_cap_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesDecreaseReset:
            return _bcm_tr3_lag_dlb_qsize_cap_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesMinThreshold:
            return _bcm_tr3_lag_dlb_tx_load_min_th_set(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesMaxThreshold:
            return _bcm_tr3_lag_dlb_tx_load_max_th_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesMinThreshold:
            return _bcm_tr3_lag_dlb_qsize_min_th_set(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesMaxThreshold:
            return _bcm_tr3_lag_dlb_qsize_max_th_set(unit, arg);

        case bcmSwitchTrunkDynamicExpectedLoadMinThreshold:
            return _bcm_tr3_lag_dlb_exp_load_min_th_set(unit, arg);

        case bcmSwitchTrunkDynamicExpectedLoadMaxThreshold:
            return _bcm_tr3_lag_dlb_exp_load_max_th_set(unit, arg);

        case bcmSwitchTrunkDynamicImbalanceMinThreshold:
            return _bcm_tr3_lag_dlb_imbalance_min_th_set(unit, arg);

        case bcmSwitchTrunkDynamicImbalanceMaxThreshold:
            return _bcm_tr3_lag_dlb_imbalance_max_th_set(unit, arg);

        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_config_get
 * Purpose:
 *      Get per-chip LAG dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (OUT) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_config_get(int unit, bcm_switch_control_t type, int *arg)
{
    switch (type) {
        case bcmSwitchTrunkDynamicSampleRate:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_sample_rate;
            break;

        case bcmSwitchTrunkDynamicAccountingSelect:
            return _bcm_tr3_lag_dlb_accounting_get(unit, arg);
            break;

        case bcmSwitchTrunkDynamicEgressBytesExponent:
            return _bcm_tr3_lag_dlb_tx_load_weight_get(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesExponent:
            return _bcm_tr3_lag_dlb_qsize_weight_get(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesDecreaseReset:
            return _bcm_tr3_lag_dlb_tx_load_cap_get(unit, arg);

        case bcmSwitchTrunkDynamicQueuedBytesDecreaseReset:
            return _bcm_tr3_lag_dlb_qsize_cap_get(unit, arg);

        case bcmSwitchTrunkDynamicEgressBytesMinThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th;
            break;

        case bcmSwitchTrunkDynamicEgressBytesMaxThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th;
            break;

        case bcmSwitchTrunkDynamicQueuedBytesMinThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th;
            break;

        case bcmSwitchTrunkDynamicQueuedBytesMaxThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th;
            break;

        case bcmSwitchTrunkDynamicExpectedLoadMinThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th;
            break;

        case bcmSwitchTrunkDynamicExpectedLoadMaxThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th;
            break;

        case bcmSwitchTrunkDynamicImbalanceMinThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th;
            break;

        case bcmSwitchTrunkDynamicImbalanceMaxThreshold:
            *arg = LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th;
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_dynamic_size_encode
 * Purpose:
 *      Encode LAG dynamic load balancing flow set size.
 * Parameters:
 *      dynamic_size - (IN) Number of flow sets.
 *      encoded_value - (OUT) Encoded value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_dynamic_size_encode(int dynamic_size,
        int *encoded_value)
{   
    switch (dynamic_size) {
        case 512:
            *encoded_value = 1;
            break;
        case 1024:
            *encoded_value = 2;
            break;
        case 2048:
            *encoded_value = 3;
            break;
        case 4096:
            *encoded_value = 4;
            break;
        case 8192:
            *encoded_value = 5;
            break;
        case 16384:
            *encoded_value = 6;
            break;
        case 32768:
            *encoded_value = 7;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_dynamic_size_decode
 * Purpose:
 *      Decode LAG trunk dynamic load balancing flow set size.
 * Parameters:
 *      encoded_value - (IN) Encoded value.
 *      dynamic_size - (OUT) Number of flow sets.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_dynamic_size_decode(int encoded_value,
        int *dynamic_size)
{   
    switch (encoded_value) {
        case 1:
            *dynamic_size = 512;
            break;
        case 2:
            *dynamic_size = 1024;
            break;
        case 3:
            *dynamic_size = 2048;
            break;
        case 4:
            *dynamic_size = 4096;
            break;
        case 5:
            *dynamic_size = 8192;
            break;
        case 6:
            *dynamic_size = 16384;
            break;
        case 7:
            *dynamic_size = 32768;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_id_alloc
 * Purpose:
 *      Get a free LAG DLB ID and mark it used.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      dlb_id - (OUT) Allocated DLB ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_id_alloc(int unit, int *dlb_id)
{
    int i;

    for (i = 0; i < soc_mem_index_count(unit, DLB_LAG_GROUP_CONTROLm); i++) {
        if (!_BCM_LAG_DLB_ID_USED_GET(unit, i)) {
            _BCM_LAG_DLB_ID_USED_SET(unit, i);
            *dlb_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_id_free
 * Purpose:
 *      Free a LAG DLB ID.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      dlb_id - (IN) DLB ID to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_id_free(int unit, int dlb_id)
{
    if (dlb_id < 0 ||
            dlb_id > soc_mem_index_max(unit, DLB_LAG_GROUP_CONTROLm)) {
        return BCM_E_PARAM;
    }

    _BCM_LAG_DLB_ID_USED_CLR(unit, dlb_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_member_id_alloc
 * Purpose:
 *      Get a free LAG DLB Member ID and mark it used.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      member_id - (OUT) Allocated Member ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_member_id_alloc(int unit, int *member_id)
{
    int vla;
    int i;
    int max_members;

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                        bcmSwitchTrunkDynamicAccountingSelect, &vla));
    if (vla) {
        max_members = soc_mem_index_count(unit, DLB_LAG_MEMBER_ATTRIBUTEm);
    } else {
        /* In physical link accounting, the number of members is limited to
         * the number of physical ports.
         */
        max_members = soc_reg_field_length(unit, DLB_LAG_MEMBER_HW_STATE_64r,
                BITMAPf);
    }

    for (i = 0; i < max_members; i++) {
        if (!_BCM_LAG_DLB_MEMBER_ID_USED_GET(unit, i)) {
            _BCM_LAG_DLB_MEMBER_ID_USED_SET(unit, i);
            *member_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_member_id_free
 * Purpose:
 *      Free a LAG DLB Member ID.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      member_id - (IN) Member ID to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_member_id_free(int unit, int member_id)
{
    if (member_id < 0 || member_id > soc_mem_index_max(unit,
                DLB_LAG_MEMBER_ATTRIBUTEm)) {
        return BCM_E_PARAM;
    }

    _BCM_LAG_DLB_MEMBER_ID_USED_CLR(unit, member_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_quality_assign
 * Purpose:
 *      Assign quality based on port loading and queue size.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      tx_load_percent - (IN) Percent weight of port loading in determing
 *                             port quality. The remaining percentage is
 *                             the weight of queue size.
 *      entry_arr - (IN) Array of 64 quality map table entries.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_tr3_lag_dlb_quality_assign(int unit, int tx_load_percent,
        uint32 *entry_arr)
{
    int quantized_tx_load;
    int quantized_qsize;
    int quality;
    int entry_index;

    if (entry_arr == NULL) {
        return BCM_E_PARAM;
    }

    for (quantized_tx_load = 0; quantized_tx_load < 8; quantized_tx_load++) {
        for (quantized_qsize = 0; quantized_qsize < 8; quantized_qsize++) {
            quality = (quantized_tx_load * tx_load_percent +
                    quantized_qsize * (100 - tx_load_percent)) / 100;
            entry_index = (quantized_tx_load << 3) + quantized_qsize;
            soc_DLB_LAG_QUALITY_MAPPINGm_field32_set(unit,
                    &entry_arr[entry_index], QUALITYf, quality);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_member_quality_map_set
 * Purpose:
 *      Set per-member LAG DLB quality mapping table.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      member_id - (IN) Member ID.
 *      tx_load_percent - (IN) Percent weight of port loading in determing
 *                             port quality. The remaining percentage is
 *                             the weight of queue size.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_member_quality_map_set(int unit, int member_id,
        int tx_load_percent)
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
    dlb_lag_quality_control_entry_t quality_control_entry;

    profile = _tr3_lag_dlb_bk[unit]->lag_dlb_quality_map_profile;

    entries_per_profile = 64;
    entry_bytes = sizeof(dlb_lag_quality_mapping_entry_t);
    alloc_size = entries_per_profile * entry_bytes;
    entry_arr = sal_alloc(alloc_size, "LAG DLB Quality Map entries");
    if (entry_arr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_arr, 0, alloc_size);

    /* Assign quality in the entry array */
    rv = _bcm_tr3_lag_dlb_quality_assign(unit, tx_load_percent, entry_arr);
    if (BCM_FAILURE(rv)) {
        sal_free(entry_arr);
        return rv;
    }

    mem = DLB_LAG_QUALITY_MAPPINGm;
    soc_mem_lock(unit, mem);

    /* Add profile */
    entries = entry_arr;
    rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
            &base_index);
    sal_free(entry_arr);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Update member profile pointer */
    rv = READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY, member_id,
            &quality_control_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    old_base_index = entries_per_profile * soc_mem_field32_get(unit,
            DLB_LAG_QUALITY_CONTROLm, &quality_control_entry,
            PROFILE_PTRf);
    soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit, &quality_control_entry, 
            PROFILE_PTRf, base_index / entries_per_profile);
    rv = WRITE_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL, member_id,
            &quality_control_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Delete old profile */
    rv = soc_profile_mem_delete(unit, profile, old_base_index);
    soc_mem_unlock(unit, mem);

    LAG_DLB_INFO(unit)->lag_dlb_load_weight[base_index/entries_per_profile] =
        tx_load_percent;

    return rv;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_free_resource
 * Purpose:
 *      Free resources for a LAG dynamic load balancing group.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      dlb_id - (IN) LAG DLB group ID.
 *      member_info - (IN) Trunk member info.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_free_resource(int unit, int tid)
{
    int rv = BCM_E_NONE;
    trunk_group_entry_t tg_entry;
    int dlb_enable;
    int dlb_id;
    dlb_lag_group_control_entry_t dlb_lag_group_control_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int block_base_ptr;
    int num_blocks;
    int vla;
    dlb_lag_group_membership_entry_t dlb_lag_group_membership_entry;
    int member_bitmap_width;
    int alloc_size;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int i;
    int reverse_map_index;
    dlb_lag_member_attribute_entry_t dlb_lag_member_attribute_entry;
    bcm_module_t mod;
    bcm_port_t port;
    int is_local;
    dlb_lag_quality_control_entry_t dlb_lag_quality_control_entry;
    dlb_lag_member_sw_state_entry_t dlb_lag_member_sw_state_entry;

    SOC_IF_ERROR_RETURN
        (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
    dlb_enable = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
            GROUP_ENABLEf);
    dlb_id = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
            DLB_IDf);

    if (!dlb_enable) {
        return BCM_E_NONE;
    }

    /* Clear DLB fields in TRUNK_GROUP table */

    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, GROUP_ENABLEf, 0);
    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, DLB_IDf, 0);
    SOC_IF_ERROR_RETURN
        (WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ALL, tid, &tg_entry));

    /* Clear VLA quality measure control */

    SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit,
                    DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm)));

    /* Free flow set table resources */

    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id,
                                     &dlb_lag_group_control_entry));
    entry_base_ptr = soc_DLB_LAG_GROUP_CONTROLm_field32_get(unit,
            &dlb_lag_group_control_entry, FLOW_SET_BASEf);
    flow_set_size = soc_DLB_LAG_GROUP_CONTROLm_field32_get(unit,
            &dlb_lag_group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_lag_dlb_dynamic_size_decode(flow_set_size, &num_entries));
    block_base_ptr = entry_base_ptr >> 9;
    num_blocks = num_entries >> 9; 
    _BCM_LAG_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(unit, block_base_ptr, num_blocks);

    /* Clear LAG DLB group control */

    SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_GROUP_CONTROLm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit, DLB_LAG_GROUP_CONTROLm)));

    /* Free Member IDs */

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchTrunkDynamicAccountingSelect, &vla));

    SOC_IF_ERROR_RETURN(READ_DLB_LAG_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                dlb_id, &dlb_lag_group_membership_entry));
    member_bitmap_width = soc_mem_field_length(unit,
            DLB_LAG_GROUP_MEMBERSHIPm, MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size, "DLB LAG member bitmap"); 
    if (NULL == member_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(member_bitmap, 0, alloc_size);
    soc_DLB_LAG_GROUP_MEMBERSHIPm_field_get(unit,
            &dlb_lag_group_membership_entry, MEMBER_BITMAPf, member_bitmap);

    for (i = 0; i < member_bitmap_width; i++) {
        if (SHR_BITGET(member_bitmap, i)) {

            /* Clear membership reverse map */

            if (vla) {
                reverse_map_index = i;
            } else {
                rv = READ_DLB_LAG_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ANY, i,
                        &dlb_lag_member_attribute_entry); 
                if (BCM_FAILURE(rv)) {
                    goto error;
                }
                mod = soc_DLB_LAG_MEMBER_ATTRIBUTEm_field32_get(unit,
                        &dlb_lag_member_attribute_entry, MODULE_IDf);
                port = soc_DLB_LAG_MEMBER_ATTRIBUTEm_field32_get(unit,
                        &dlb_lag_member_attribute_entry, PORT_NUMf);
                rv = _bcm_esw_modid_is_local(unit, mod, &is_local);
                if (BCM_FAILURE(rv)) {
                    goto error;
                }
                if (!is_local) {
                    rv = BCM_E_INTERNAL;
                    goto error;
                }
                reverse_map_index = port;
            }
            rv = WRITE_DLB_LAG_MEMBERSHIP_REVERSE_MAPm(unit, MEM_BLOCK_ALL,
                    reverse_map_index, soc_mem_entry_null(unit,
                        DLB_LAG_MEMBERSHIP_REVERSE_MAPm));
            if (BCM_FAILURE(rv)) {
                goto error;
            }

            /* Clear member attribute */

            rv = WRITE_DLB_LAG_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ALL, i,
                    soc_mem_entry_null(unit, DLB_LAG_MEMBER_ATTRIBUTEm));
            if (BCM_FAILURE(rv)) {
                goto error;
            }

            /* Revert member quality mapping profile to default */

            rv = _bcm_tr3_lag_dlb_member_quality_map_set(unit, i, 100);
            if (BCM_FAILURE(rv)) {
                goto error;
            }

            /* Clear threshold scaling factors */

            rv = READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY, i,
                    &dlb_lag_quality_control_entry);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &dlb_lag_quality_control_entry, LOADING_SCALING_FACTORf, 0);
            soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                    &dlb_lag_quality_control_entry, QSIZE_SCALING_FACTORf, 0);
            rv = WRITE_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL, i,
                    &dlb_lag_quality_control_entry);
            if (BCM_FAILURE(rv)) {
                goto error;
            }

            /* Free member ID */

            rv = _bcm_tr3_lag_dlb_member_id_free(unit, i);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
        }
    }

    /* Clear member software state */

    rv = READ_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &dlb_lag_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    status_bitmap = sal_alloc(alloc_size, "DLB LAG member status bitmap"); 
    if (NULL == status_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit,
            &dlb_lag_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);
    SHR_BITREMOVE_RANGE(status_bitmap, member_bitmap, 0, member_bitmap_width,
            status_bitmap);
    soc_DLB_LAG_MEMBER_SW_STATEm_field_set(unit,
            &dlb_lag_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);

    override_bitmap = sal_alloc(alloc_size, "DLB LAG member override bitmap"); 
    if (NULL == override_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit,
            &dlb_lag_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf, override_bitmap);
    SHR_BITREMOVE_RANGE(override_bitmap, member_bitmap, 0, member_bitmap_width,
            override_bitmap);
    soc_DLB_LAG_MEMBER_SW_STATEm_field_set(unit,
            &dlb_lag_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf, override_bitmap);

    rv = WRITE_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ALL, 0,
                &dlb_lag_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    sal_free(member_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);

    /* Clear group membership */

    SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_GROUP_MEMBERSHIPm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit, DLB_LAG_GROUP_MEMBERSHIPm)));

    /* Free DLB ID */

    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_id_free(unit, dlb_id));

    return rv;

error:
    if (NULL != member_bitmap) {
        sal_free(member_bitmap);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_member_id_array_get
 * Purpose:
 *      Get member IDs for each LAG member.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      dlb_id - (IN) DLB group ID.
 *      num_dlb_ports  - (IN) Number of DLB ports.
 *      mod_array  - (IN) Array of module IDs.
 *      port_array - (IN) Array of port numbers.
 *      scaling_factor_array - (IN) Array of scaling factors.
 *      load_weight_array    - (IN) Array of load weights.
 *      member_id_array - (OUT) Array of member IDs.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_member_id_array_get(int unit, int dlb_id, int num_dlb_ports,
        bcm_module_t *mod_array, bcm_port_t *port_array,
        int *scaling_factor_array, int *load_weight_array,
        int *member_id_array)
{
    int rv = BCM_E_NONE;
    int vla;
    int i;
    dlb_lag_membership_reverse_map_entry_t member_reverse_map_entry;
    dlb_lag_member_attribute_entry_t member_attribute_entry;
    int is_local;
    dlb_lag_quality_control_entry_t dlb_lag_quality_control_entry;
    port_tab_entry_t ptab;
    int hw_mask;

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                        bcmSwitchTrunkDynamicAccountingSelect, &vla));

    for (i = 0; i < num_dlb_ports; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_lag_dlb_member_id_alloc(unit, &member_id_array[i]));

        /* Set member reverse map */

        sal_memset(&member_reverse_map_entry, 0,
                sizeof(dlb_lag_membership_reverse_map_entry_t));
        if (vla) { /* Virtual link accounting */
            soc_DLB_LAG_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, VALIDf, 1);
            soc_DLB_LAG_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, GROUP_IDf, dlb_id);
            soc_DLB_LAG_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, MEMBER_COUNTf,
                    num_dlb_ports - 1);
            SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_MEMBERSHIP_REVERSE_MAPm(unit,
                    MEM_BLOCK_ALL, member_id_array[i],
                    &member_reverse_map_entry));
        } else { /* Physical link accounting */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_modid_is_local(unit, mod_array[i], &is_local));
            if (!is_local) {
                /* In physical link accounting mode, LAG members should be
                 * local.
                 */
                return BCM_E_PARAM;
            }
            soc_DLB_LAG_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, VALIDf, 1);
            soc_DLB_LAG_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry,
                    MEMBER_IDf, member_id_array[i]);
            SOC_IF_ERROR_RETURN
                (READ_PORT_TABm(unit, MEM_BLOCK_ANY, port_array[i], &ptab));
            if (soc_PORT_TABm_field32_get(unit, &ptab, PORT_TYPEf) == 3) {
                /* Port is an embedded Higig port */
                soc_DLB_LAG_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                        &member_reverse_map_entry, EMBEDDED_HGf, 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_MEMBERSHIP_REVERSE_MAPm(unit,
                    MEM_BLOCK_ALL, port_array[i],
                    &member_reverse_map_entry));
        }

        /* Set member attribute */

        sal_memset(&member_attribute_entry, 0,
                sizeof(dlb_lag_member_attribute_entry_t));
        soc_DLB_LAG_MEMBER_ATTRIBUTEm_field32_set(unit,
                &member_attribute_entry, MODULE_IDf, mod_array[i]);
        soc_DLB_LAG_MEMBER_ATTRIBUTEm_field32_set(unit,
                &member_attribute_entry, PORT_NUMf, port_array[i]);
        SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_MEMBER_ATTRIBUTEm(unit,
                    MEM_BLOCK_ALL, member_id_array[i],
                    &member_attribute_entry));

        /* Set member quality mapping profile */

        BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_member_quality_map_set(unit,
                    member_id_array[i], load_weight_array[i]));

        /* Set threshold scaling factors */

        SOC_IF_ERROR_RETURN(READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                    member_id_array[i], &dlb_lag_quality_control_entry));
        hw_mask = (1 << soc_mem_field_length(unit, DLB_LAG_QUALITY_CONTROLm,
                                      LOADING_SCALING_FACTORf)) - 1;
        soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                &dlb_lag_quality_control_entry, LOADING_SCALING_FACTORf,
                scaling_factor_array[i] & hw_mask);
        hw_mask = (1 << soc_mem_field_length(unit, DLB_LAG_QUALITY_CONTROLm,
                                      QSIZE_SCALING_FACTORf)) - 1;
        soc_DLB_LAG_QUALITY_CONTROLm_field32_set(unit,
                &dlb_lag_quality_control_entry, QSIZE_SCALING_FACTORf,
                scaling_factor_array[i] & hw_mask);
        SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL,
                    member_id_array[i], &dlb_lag_quality_control_entry));
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_set
 * Purpose:
 *      Configure a LAG dynamic load balancing group.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      tid        - (IN) Trunk group ID.
 *      add_info   - (IN) Pointer to trunk add info structure.
 *      num_dlb_ports  - (IN) Number of trunk member ports eligible for
 *                        LAG DLB
 *      mod_array  - (IN) Array of module IDs
 *      port_array - (IN) Array of port IDs
 *      scaling_factor_array - (IN) Array of scaling factors.
 *      load_weight_array    - (IN) Array of load weights.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_set(int unit, int tid, _esw_trunk_add_info_t *add_info,
        int num_dlb_ports, int *mod_array, int *port_array,
        int *scaling_factor_array, int *load_weight_array)
{
    int rv = BCM_E_NONE;
    int dlb_enable;
    int dlb_id;
    int *member_id_array = NULL;
    soc_mem_t flowset_mem;
    int flowset_entry_size;
    dlb_lag_flowset_entry_t *flowset_entry;
    int num_blocks;
    int total_blocks;
    int max_block_base_ptr;
    int block_base_ptr;
    SHR_BITDCL occupied;
    int entry_base_ptr;
    int num_entries_per_block;
    int alloc_size;
    uint32 *block_ptr = NULL;
    int i, k;
    int index_min, index_max;
    dlb_lag_group_control_entry_t dlb_lag_group_control_entry;
    int flow_set_size;
    int dlb_mode;
    dlb_lag_group_membership_entry_t dlb_lag_group_membership_entry;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    dlb_lag_member_sw_state_entry_t dlb_lag_member_sw_state_entry;
    int vla;
    dlb_lag_vla_quality_measure_control_entry_t vla_quality_measure_control_entry;
    trunk_group_entry_t tg_entry;
    int member_bitmap_width;

    dlb_enable = (add_info->psc == BCM_TRUNK_PSC_DYNAMIC) ||
        (add_info->psc == BCM_TRUNK_PSC_DYNAMIC_ASSIGNED) ||
        (add_info->psc == BCM_TRUNK_PSC_DYNAMIC_OPTIMAL);

    if (!dlb_enable) {
        return BCM_E_NONE;
    }

    /* Allocate a DLB ID */

    BCM_IF_ERROR_RETURN(_bcm_tr3_lag_dlb_id_alloc(unit, &dlb_id));

    /* Allocate member IDs */

    member_id_array = sal_alloc(sizeof(int) * num_dlb_ports,
            "LAG DLB Member IDs");
    if (NULL == member_id_array) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    rv = _bcm_tr3_lag_dlb_member_id_array_get(unit, dlb_id, num_dlb_ports,
            mod_array, port_array, scaling_factor_array, load_weight_array,
            member_id_array);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Set group membership table */

    member_bitmap_width = soc_mem_field_length(unit,
            DLB_LAG_GROUP_MEMBERSHIPm, MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size, "DLB LAG member bitmap"); 
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(member_bitmap, 0, alloc_size);
    for (i = 0; i < num_dlb_ports; i++) {
        SHR_BITSET(member_bitmap, member_id_array[i]);
    }

    sal_memset(&dlb_lag_group_membership_entry, 0,
            sizeof(dlb_lag_group_membership_entry_t));
    soc_DLB_LAG_GROUP_MEMBERSHIPm_field_set(unit,
            &dlb_lag_group_membership_entry, MEMBER_BITMAPf, member_bitmap);
    rv = WRITE_DLB_LAG_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ALL, dlb_id,
            &dlb_lag_group_membership_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    /* Set member software state */

    rv = bcm_esw_switch_control_get(unit,
            bcmSwitchTrunkDynamicAccountingSelect, &vla);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    rv = READ_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &dlb_lag_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    status_bitmap = sal_alloc(alloc_size, "DLB LAG member status bitmap"); 
    if (NULL == status_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit,
            &dlb_lag_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);

    if (vla) {
        SHR_BITOR_RANGE(status_bitmap, member_bitmap, 0, member_bitmap_width,
                status_bitmap);
    } else {
        SHR_BITREMOVE_RANGE(status_bitmap, member_bitmap, 0, member_bitmap_width,
                status_bitmap);
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_set(unit,
            &dlb_lag_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);

    override_bitmap = sal_alloc(alloc_size, "DLB LAG member override bitmap"); 
    if (NULL == override_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit,
            &dlb_lag_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf,
            override_bitmap);

    if (vla) {
        SHR_BITOR_RANGE(override_bitmap, member_bitmap, 0, member_bitmap_width,
                override_bitmap);
    } else {
        SHR_BITREMOVE_RANGE(override_bitmap, member_bitmap, 0, member_bitmap_width,
                override_bitmap);
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_set(unit,
            &dlb_lag_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf, override_bitmap);

    rv = WRITE_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ALL, 0,
                &dlb_lag_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Set DLB flow set table */

    flowset_mem = DLB_LAG_FLOWSETm;
    flowset_entry_size = sizeof(dlb_lag_flowset_entry_t);
    num_blocks = add_info->dynamic_size >> 9;
    total_blocks = soc_mem_index_count(unit, flowset_mem) >> 9;
    max_block_base_ptr = total_blocks - num_blocks;
    for (block_base_ptr = 0;
            block_base_ptr <= max_block_base_ptr;
            block_base_ptr++) {
        /* Check if the contiguous region of flow set table from
         * block_base_ptr to (block_base_ptr + num_blocks - 1) is free. 
         */
        _BCM_LAG_DLB_FLOWSET_BLOCK_TEST_RANGE(unit, block_base_ptr, num_blocks,
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

    entry_base_ptr = block_base_ptr << 9;
    num_entries_per_block = 512;
    alloc_size = num_entries_per_block * flowset_entry_size;
    block_ptr = soc_cm_salloc(unit, alloc_size,
            "Block of DLB_LAG_FLOWSET entries");
    if (NULL == block_ptr) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(block_ptr, 0, alloc_size);
    for (i = 0; i < num_blocks; i++) {
        for (k = 0; k < num_entries_per_block; k++) {
            flowset_entry = soc_mem_table_idx_to_pointer(unit,
                    DLB_LAG_FLOWSETm, dlb_lag_flowset_entry_t *,
                    block_ptr, k);
            soc_DLB_LAG_FLOWSETm_field32_set(unit, flowset_entry,
                    VALIDf, 1);
            soc_DLB_LAG_FLOWSETm_field32_set(unit, flowset_entry,
                    MEMBER_IDf,
                    member_id_array[(i * num_entries_per_block + k)
                    % num_dlb_ports]);
        }
        index_min = entry_base_ptr + i * num_entries_per_block;
        index_max = index_min + num_entries_per_block - 1;
        rv = soc_mem_write_range(unit, flowset_mem, MEM_BLOCK_ALL,
                index_min, index_max, block_ptr);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }

    _BCM_LAG_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr, num_blocks);

    /* Set LAG DLB group control table */

    sal_memset(&dlb_lag_group_control_entry, 0,
            sizeof(dlb_lag_group_control_entry_t));
    soc_DLB_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_lag_group_control_entry,
            ENABLE_OPTIMAL_CANDIDATE_UPDATEf, 1);
    soc_DLB_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_lag_group_control_entry,
            FLOW_SET_BASEf, entry_base_ptr);

    rv = bcm_tr3_lag_dlb_dynamic_size_encode(add_info->dynamic_size,
            &flow_set_size);
    if (SOC_FAILURE(rv)) {
        goto error;
    }
    soc_DLB_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_lag_group_control_entry,
            FLOW_SET_SIZEf, flow_set_size);

    switch (add_info->psc) {
        case BCM_TRUNK_PSC_DYNAMIC:
            dlb_mode = 0;
            break;
        case BCM_TRUNK_PSC_DYNAMIC_ASSIGNED:
            dlb_mode = 1;
            break;
        case BCM_TRUNK_PSC_DYNAMIC_OPTIMAL:
            dlb_mode = 2;
            break;
        default:
            rv = BCM_E_PARAM;
            goto error;
    }
    soc_DLB_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_lag_group_control_entry,
            MEMBER_ASSIGNMENT_MODEf, dlb_mode);

    soc_DLB_LAG_GROUP_CONTROLm_field32_set(unit, &dlb_lag_group_control_entry,
            INACTIVITY_DURATIONf, add_info->dynamic_age);

    rv = WRITE_DLB_LAG_GROUP_CONTROLm(unit, MEM_BLOCK_ALL, dlb_id,
            &dlb_lag_group_control_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    /* Set VLA quality measure control */

    if (vla) {
        sal_memset(&vla_quality_measure_control_entry, 0,
                sizeof(dlb_lag_vla_quality_measure_control_entry_t));
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                ENABLE_GROUP_AVG_CALCf, 1);
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_WEIGHT_LOADINGf, add_info->dynamic_load_exponent);
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_WEIGHT_EXPECTED_LOADINGf,
                add_info->dynamic_expected_load_exponent);
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_CAP_LOADING_AVGf, (add_info->flags &
                    BCM_TRUNK_FLAG_DYNAMIC_LOAD_DECREASE_RESET) ? 1 : 0);
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_CAP_EXPECTED_LOADINGf, (add_info->flags &
                    BCM_TRUNK_FLAG_DYNAMIC_EXPECTED_LOAD_DECREASE_RESET) ?
                1 : 0);
        rv = WRITE_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm(unit, MEM_BLOCK_ALL,
                dlb_id, &vla_quality_measure_control_entry);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }

    /* Update trunk group table */

    rv = READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }
    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, GROUP_ENABLEf, 1);
    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, DLB_IDf, dlb_id);
    rv = WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ALL, tid, &tg_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    sal_free(member_id_array);
    soc_cm_sfree(unit, block_ptr);
    sal_free(member_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);

    return rv;

error:
    if (NULL != member_id_array) { 
        sal_free(member_id_array);
    }
    if (NULL != block_ptr) { 
        soc_cm_sfree(unit, block_ptr);
    }
    if (NULL != member_bitmap) { 
        sal_free(member_bitmap);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_lag_dlb_member_id_get
 * Purpose:
 *      Get LAG DLB member ID based on module ID and port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      mod  - (IN) Module ID of the member.
 *      port - (IN) Port number of the member.
 *      member_id - (OUT) Member ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_lag_dlb_member_id_get(int unit, bcm_module_t mod, bcm_port_t port,
        int *member_id)
{
    int match;
    int i;
    dlb_lag_member_attribute_entry_t member_attr_entry;

    match = FALSE;
    for (i = 0; i < soc_mem_index_count(unit, DLB_LAG_MEMBER_ATTRIBUTEm); i++) {
        SOC_IF_ERROR_RETURN(READ_DLB_LAG_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ANY,
                    i, &member_attr_entry));
        if ((mod == soc_DLB_LAG_MEMBER_ATTRIBUTEm_field32_get(unit,
                        &member_attr_entry, MODULE_IDf)) &&
            (port == soc_DLB_LAG_MEMBER_ATTRIBUTEm_field32_get(unit,
                        &member_attr_entry, PORT_NUMf))) {
            match = TRUE;
            *member_id = i;
            break;
        }
    }
    if (!match) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_tr3_lag_dlb_member_attr_get
 * Purpose:
 *      Get attributes of a LAG DLB member.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      mod  - (IN) Module ID of the member.
 *      port - (IN) Port number of the member.
 *      scaling_factor - (OUT) Member's scaling factor.
 *      load_weight    - (OUT) Member's load weight.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_member_attr_get(int unit, bcm_module_t mod, bcm_port_t port,
        int *scaling_factor, int *load_weight)
{
    int member_id;
    dlb_lag_quality_control_entry_t quality_control_entry;
    int profile_ptr;

    /* Find member ID of the given module and port */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_lag_dlb_member_id_get(unit, mod, port, &member_id));

    /* Get member's scaling factor and load weight */
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                member_id, &quality_control_entry));
    *scaling_factor = soc_DLB_LAG_QUALITY_CONTROLm_field32_get(unit,
            &quality_control_entry, LOADING_SCALING_FACTORf);
    profile_ptr = soc_DLB_LAG_QUALITY_CONTROLm_field32_get(unit,
            &quality_control_entry, PROFILE_PTRf);
    *load_weight = LAG_DLB_INFO(unit)->lag_dlb_load_weight[profile_ptr];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_member_status_set
 * Purpose:
 *      Set LAG DLB member status.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      mod    - (IN) Member module ID.
 *      port   - (IN) Member port number.
 *      status - (IN) Member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_member_status_set(int unit, bcm_module_t mod, bcm_port_t port,
        int status)
{
    int member_id;
    dlb_lag_member_sw_state_entry_t sw_state_entry;
    int member_bitmap_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;

    BCM_IF_ERROR_RETURN
        (_bcm_tr3_lag_dlb_member_id_get(unit, mod, port, &member_id));

    /* Get status bitmap */
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &sw_state_entry));
    member_bitmap_width = soc_mem_field_length(unit, DLB_LAG_MEMBER_SW_STATEm,
            MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    status_bitmap = sal_alloc(alloc_size, "DLB LAG member status bitmap"); 
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            MEMBER_BITMAPf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB LAG member override bitmap"); 
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            OVERRIDE_MEMBER_BITMAPf, override_bitmap);

    /* Update status and override bitmaps */
    switch (status) {
        case BCM_TRUNK_DYNAMIC_MEMBER_FORCE_DOWN:
            SHR_BITSET(override_bitmap, member_id);
            SHR_BITCLR(status_bitmap, member_id);
            break;
        case BCM_TRUNK_DYNAMIC_MEMBER_FORCE_UP:
            SHR_BITSET(override_bitmap, member_id);
            SHR_BITSET(status_bitmap, member_id);
            break;
        case BCM_TRUNK_DYNAMIC_MEMBER_HW:
            SHR_BITCLR(override_bitmap, member_id);
            SHR_BITCLR(status_bitmap, member_id);
            break;
        default:
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            return BCM_E_PARAM;
    }

    /* Write status and override bitmaps to hardware */
    soc_DLB_LAG_MEMBER_SW_STATEm_field_set(unit, &sw_state_entry,
            MEMBER_BITMAPf, status_bitmap);
    soc_DLB_LAG_MEMBER_SW_STATEm_field_set(unit, &sw_state_entry,
            OVERRIDE_MEMBER_BITMAPf, override_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);
    SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ALL, 0,
                &sw_state_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_member_status_get
 * Purpose:
 *      Get LAG DLB member status.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      mod    - (IN) Member module ID.
 *      port   - (IN) Member port number.
 *      status - (OUT) Member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_lag_dlb_member_status_get(int unit, bcm_module_t mod, bcm_port_t port,
        int *status)
{
    int member_id;
    dlb_lag_member_sw_state_entry_t sw_state_entry;
    int member_bitmap_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    uint64 hw_state, hw_status_bitmap;
    uint32 bitmap32;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (_bcm_tr3_lag_dlb_member_id_get(unit, mod, port, &member_id));

    /* Get status bitmap */
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &sw_state_entry));
    member_bitmap_width = soc_mem_field_length(unit, DLB_LAG_MEMBER_SW_STATEm,
            MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    status_bitmap = sal_alloc(alloc_size, "DLB LAG member status bitmap"); 
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            MEMBER_BITMAPf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB LAG member override bitmap"); 
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    soc_DLB_LAG_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            OVERRIDE_MEMBER_BITMAPf, override_bitmap);

    /* Derive status from software override and status bitmaps or
     * hardware state bitmap.
     */
    if (SHR_BITGET(override_bitmap, member_id)) {
        if (SHR_BITGET(status_bitmap, member_id)) {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_FORCE_UP;
        } else {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_FORCE_DOWN;
        }
    } else {
        rv = READ_DLB_LAG_MEMBER_HW_STATE_64r(unit, &hw_state); 
        if (SOC_FAILURE(rv)) {
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            return rv;
        }
        hw_status_bitmap = soc_reg64_field_get(unit,
                DLB_LAG_MEMBER_HW_STATE_64r, hw_state, BITMAPf);
        if (member_id < 32) {
            bitmap32 = COMPILER_64_LO(hw_status_bitmap);
        } else {
            bitmap32 = COMPILER_64_HI(hw_status_bitmap);
            member_id -= 32;
        }
        if ((1 << member_id) & bitmap32) {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_HW_UP;
        } else {
            *status = BCM_TRUNK_DYNAMIC_MEMBER_HW_DOWN;
        }
    }

    sal_free(status_bitmap);
    sal_free(override_bitmap);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_ethertype_set
 * Purpose:
 *      Set the Ethertypes that are eligible or ineligible for
 *      LAG dynamic load balancing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_TRUNK_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_count - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (IN) Array of Ethertypes.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_lag_dlb_ethertype_set(
    int unit, 
    uint32 flags, 
    int ethertype_count, 
    int *ethertype_array)
{
    uint32 measure_control_reg;
    int i, j;
    dlb_lag_ethertype_eligibility_map_entry_t ethertype_entry;

    /* Input check */
    if (ethertype_count > soc_mem_index_count(unit,
                DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm)) {
        return BCM_E_RESOURCE;
    }

    /* Update quality measure control register */
    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, ETHERTYPE_ELIGIBILITY_CONFIGf,
            flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
    soc_reg_field_set(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, INNER_OUTER_ETHERTYPE_SELECTIONf,
            flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));

    /* Update Ethertype eligibility map table */
    for (i = 0; i < ethertype_count; i++) {
        sal_memset(&ethertype_entry, 0,
                sizeof(dlb_lag_ethertype_eligibility_map_entry_t));
        soc_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, VALIDf, 1);
        soc_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, ETHERTYPEf, ethertype_array[i]);
        SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, i, &ethertype_entry));
    }

    /* Zero out remaining entries of Ethertype eligibility map table */
    for (j = i; j < soc_mem_index_count(unit,
                DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm); j++) {
        SOC_IF_ERROR_RETURN(WRITE_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, j, soc_mem_entry_null(unit,
                        DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_ethertype_get
 * Purpose:
 *      Get the Ethertypes that are eligible or ineligible for
 *      LAG dynamic load balancing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (INOUT) BCM_TRUNK_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_max - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (OUT) Array of Ethertypes.
 *      ethertype_count - (OUT) Number of elements returned in ethertype_array.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_lag_dlb_ethertype_get(
    int unit, 
    uint32 *flags, 
    int ethertype_max, 
    int *ethertype_array, 
    int *ethertype_count)
{
    uint32 measure_control_reg;
    int i;
    int ethertype;
    dlb_lag_ethertype_eligibility_map_entry_t ethertype_entry;

    *ethertype_count = 0;

    /* Get flags */
    SOC_IF_ERROR_RETURN
        (READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    if (soc_reg_field_get(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
                measure_control_reg, ETHERTYPE_ELIGIBILITY_CONFIGf)) {
        *flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_ELIGIBLE;
    }
    if (soc_reg_field_get(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
                measure_control_reg, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
        *flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_INNER;
    }

    /* Get Ethertypes */
    for (i = 0; i < soc_mem_index_count(unit,
                DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm); i++) {
        SOC_IF_ERROR_RETURN(READ_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ANY, i, &ethertype_entry));
        if (soc_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, VALIDf)) {
            ethertype = soc_DLB_LAG_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
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

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_tr3_lag_dlb_wb_alloc_size_get
 * Purpose:
 *      Get level 2 warm boot scache size for LAG DLB.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      size - (OUT) Allocation size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_lag_dlb_wb_alloc_size_get(int unit, int *size) 
{
    int alloc_size = 0;
    int num_elements;

    /* Allocate size for the following LAG DLB parameters: 
     * int lag_dlb_sample_rate;
     * int lag_dlb_tx_load_min_th;
     * int lag_dlb_tx_load_max_th;
     * int lag_dlb_qsize_min_th;
     * int lag_dlb_qsize_max_th;
     * int lag_dlb_exp_load_min_th;
     * int lag_dlb_exp_load_max_th;
     * int lag_dlb_imbalance_min_th;
     * int lag_dlb_imbalance_max_th;
     */
    alloc_size += sizeof(int) * 9;

    /* Allocate size of lag_dlb_load_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_LAG_QUALITY_CONTROLm, PROFILE_PTRf);
    alloc_size += sizeof(uint8) * num_elements;

    *size = alloc_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_sync
 * Purpose:
 *      Store LAG DLB parameters into scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_lag_dlb_sync(int unit, uint8 **scache_ptr) 
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;

    /* Store the following LAG DLB parameters: 
     * int lag_dlb_sample_rate;
     * int lag_dlb_tx_load_min_th;
     * int lag_dlb_tx_load_max_th;
     * int lag_dlb_qsize_min_th;
     * int lag_dlb_qsize_max_th;
     * int lag_dlb_exp_load_min_th;
     * int lag_dlb_exp_load_max_th;
     * int lag_dlb_imbalance_min_th;
     * int lag_dlb_imbalance_max_th;
     */
    value = LAG_DLB_INFO(unit)->lag_dlb_sample_rate;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    /* Store lag_dlb_load_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_LAG_QUALITY_CONTROLm, PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        load_weight = LAG_DLB_INFO(unit)->lag_dlb_load_weight[i];
        sal_memcpy((*scache_ptr), &load_weight, sizeof(uint8));
        (*scache_ptr) += sizeof(uint8);
    }
        
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_scache_recover
 * Purpose:
 *      Recover LAG DLB parameters from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_lag_dlb_scache_recover(int unit, uint8 **scache_ptr) 
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;

    /* Recover the following LAG DLB parameters: 
     * int lag_dlb_sample_rate;
     * int lag_dlb_tx_load_min_th;
     * int lag_dlb_tx_load_max_th;
     * int lag_dlb_qsize_min_th;
     * int lag_dlb_qsize_max_th;
     * int lag_dlb_exp_load_min_th;
     * int lag_dlb_exp_load_max_th;
     * int lag_dlb_imbalance_min_th;
     * int lag_dlb_imbalance_max_th;
     */
    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_sample_rate = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th = value;
    (*scache_ptr) += sizeof(int);

    /* Recover lag_dlb_load_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_LAG_QUALITY_CONTROLm, PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        sal_memcpy(&load_weight, (*scache_ptr), sizeof(uint8));
        LAG_DLB_INFO(unit)->lag_dlb_load_weight[i] = load_weight;
        (*scache_ptr) += sizeof(uint8);
    }
        
    LAG_DLB_INFO(unit)->recovered_from_scache = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_member_recover
 * Purpose:
 *      Recover LAG DLB member ID usage.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_lag_dlb_member_recover(int unit) 
{
    int rv = BCM_E_NONE;
    int member_bitmap_width;
    int alloc_size;
    SHR_BITDCL *member_bitmap = NULL;
    int dlb_id;
    int i;
    dlb_lag_group_membership_entry_t group_membership_entry;

    member_bitmap_width = soc_mem_field_length(unit,
            DLB_LAG_GROUP_MEMBERSHIPm, MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size, "DLB LAG member bitmap"); 
    if (NULL == member_bitmap) {
        return BCM_E_MEMORY;
    }

    for (dlb_id = 0;
            dlb_id < soc_mem_index_count(unit, DLB_LAG_GROUP_MEMBERSHIPm);
            dlb_id++) {
        rv = READ_DLB_LAG_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                dlb_id, &group_membership_entry);
        if (SOC_FAILURE(rv)) {
            sal_free(member_bitmap);
            return rv;
        }
        sal_memset(member_bitmap, 0, alloc_size);
        soc_DLB_LAG_GROUP_MEMBERSHIPm_field_get(unit,
                &group_membership_entry, MEMBER_BITMAPf, member_bitmap);
        for (i = 0; i < member_bitmap_width; i++) {
            if (SHR_BITGET(member_bitmap, i)) {
                _BCM_LAG_DLB_MEMBER_ID_USED_SET(unit, i);
            }
        }
    }

    sal_free(member_bitmap);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_group_recover
 * Purpose:
 *      Recover LAG DLB group usage and flowset table usage.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      tid  - (IN) Trunk group ID.
 *      trunk_info - (OUT) Per trunk group DLB info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_lag_dlb_group_recover(int unit, bcm_trunk_t tid,
        trunk_private_t *trunk_info) 
{
    trunk_group_entry_t tg_entry;
    int group_enable, dlb_id;
    dlb_lag_group_control_entry_t group_control_entry;
    int dlb_mode;
    int dynamic_size_encode, dynamic_size;
    int entry_base_ptr;
    int block_base_ptr, num_blocks;
    dlb_lag_vla_quality_measure_control_entry_t vla_quality_measure_control_entry;

    SOC_IF_ERROR_RETURN
        (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
    group_enable = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
            GROUP_ENABLEf);
    if (0 == group_enable) {
        /* DLB not enabled */
        return BCM_E_NONE;
    }

    /* Get DLB group ID and marked it as used */
    dlb_id = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry, DLB_IDf);
    _BCM_LAG_DLB_ID_USED_SET(unit, dlb_id);

    SOC_IF_ERROR_RETURN(READ_DLB_LAG_GROUP_CONTROLm(unit,
                MEM_BLOCK_ANY, dlb_id, &group_control_entry));

    /* Recover DLB mode */
    dlb_mode = soc_DLB_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, MEMBER_ASSIGNMENT_MODEf);
    switch (dlb_mode) {
        case 0:
            trunk_info->psc = BCM_TRUNK_PSC_DYNAMIC;
            break;
        case 1:
            trunk_info->psc = BCM_TRUNK_PSC_DYNAMIC_ASSIGNED;
            break;
        case 2:
            trunk_info->psc = BCM_TRUNK_PSC_DYNAMIC_OPTIMAL;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    /* Recover dynamic_size */
    dynamic_size_encode = soc_DLB_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_lag_dlb_dynamic_size_decode(dynamic_size_encode,
                                              &dynamic_size));
    trunk_info->dynamic_size = dynamic_size;

    /* Recover dynamic_age */
    trunk_info->dynamic_age = soc_DLB_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, INACTIVITY_DURATIONf);

    /* Recover flow set table usage */
    entry_base_ptr = soc_DLB_LAG_GROUP_CONTROLm_field32_get(unit,
            &group_control_entry, FLOW_SET_BASEf);
    block_base_ptr = entry_base_ptr >> 9;
    num_blocks = dynamic_size >> 9; 
    _BCM_LAG_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr,
            num_blocks);

    SOC_IF_ERROR_RETURN(READ_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm(unit,
                MEM_BLOCK_ANY, dlb_id, &vla_quality_measure_control_entry));

    /* Recover BCM_TRUNK_FLAG_DYNAMIC_LOAD_DECREASE_RESET flag */
    if (soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
        &vla_quality_measure_control_entry, VLA_CAP_LOADING_AVGf)) {
        trunk_info->flags |= BCM_TRUNK_FLAG_DYNAMIC_LOAD_DECREASE_RESET;
    }
    
    /* Recover BCM_TRUNK_FLAG_DYNAMIC_EXPECTED_LOAD_DECREASE_RESET flag */
    if (soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
        &vla_quality_measure_control_entry, VLA_CAP_EXPECTED_LOADINGf)) {
        trunk_info->flags |= BCM_TRUNK_FLAG_DYNAMIC_EXPECTED_LOAD_DECREASE_RESET;
    }

    /* Recover dynamic_load_exponent */
    trunk_info->dynamic_load_exponent =
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
                &vla_quality_measure_control_entry, VLA_WEIGHT_LOADINGf);

    /* Recover dynamic_expected_load_exponent */
    trunk_info->dynamic_expected_load_exponent =
        soc_DLB_LAG_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
                &vla_quality_measure_control_entry, VLA_WEIGHT_EXPECTED_LOADINGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_lag_dlb_quality_parameters_recover
 * Purpose:
 *      Recover LAG DLB parameters used to derive a member's quality.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_lag_dlb_quality_parameters_recover(int unit) 
{
    uint32 measure_control_reg;
    int num_time_units;
    dlb_lag_pla_quantize_threshold_entry_t quantize_threshold_entry;
    int min_th_bytes, min_th_cells;
    int max_th_bytes, max_th_cells;
    dlb_lag_vla_expected_loading_threshold_entry_t expected_loading_threshold_entry;
    int exp_load_min_th_kbytes, exp_load_max_th_kbytes;
    dlb_lag_vla_member_imbalance_threshold_entry_t imbalance_threshold_entry;
    int imbalance_min_th_kbytes, imbalance_max_th_kbytes;
    int entries_per_profile;
    int profile_ptr, base_index;
    dlb_lag_quality_mapping_entry_t quality_mapping_entry;
    int quality;

    if (LAG_DLB_INFO(unit)->recovered_from_scache) {
        /* If already recovered from scache, do nothing. */ 
        return BCM_E_NONE;
    }

    /* Recover sampling rate */
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_QUALITY_MEASURE_CONTROLr(unit,
                &measure_control_reg));
    num_time_units = soc_reg_field_get(unit, DLB_LAG_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, HISTORICAL_SAMPLING_PERIODf);
    if (num_time_units > 0) {
        LAG_DLB_INFO(unit)->lag_dlb_sample_rate = 1000000 / num_time_units;
    }

    /* Recover min and max load and queue size thresholds: 
     * Load threshold (in mbps)
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (sampling period in seconds) / 10^6
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (num_time_units * 1 us per time unit * 10^-6) / 10^6
     *     = (bytes per sampling period) * 8 / num_time_units 
     */ 
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_PLA_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 0, &quantize_threshold_entry));
    min_th_bytes = soc_DLB_LAG_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_LOADf); 
    LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th = (min_th_bytes << 3) /
        num_time_units;
    min_th_cells = soc_DLB_LAG_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_QSIZEf); 
    LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th = min_th_cells * 208;

    SOC_IF_ERROR_RETURN(READ_DLB_LAG_PLA_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, soc_mem_index_max(unit,
                    DLB_LAG_PLA_QUANTIZE_THRESHOLDm),
                &quantize_threshold_entry));
    max_th_bytes = soc_DLB_LAG_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_LOADf); 
    LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th = (max_th_bytes << 3) /
        num_time_units;
    max_th_cells = soc_DLB_LAG_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_QSIZEf); 
    LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th = max_th_cells * 208;

    /* Recover min and max expected load thresholds: 
     * Load threshold (in mbps)
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (sampling period in seconds) / 10^6
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (num_time_units * 1 us per time unit * 10^-6) / 10^6
     *     = (bytes per sampling period) * 8 / num_time_units 
     *     = (kilobytes per sampling period) * 8000 / num_time_units 
     */ 
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 0, &expected_loading_threshold_entry));
    exp_load_min_th_kbytes =
        soc_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm_field32_get
        (unit, &expected_loading_threshold_entry, THRESHOLDf); 
    LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th =
        (exp_load_min_th_kbytes << 3) * 1000 / num_time_units;

    SOC_IF_ERROR_RETURN(READ_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 14, &expected_loading_threshold_entry));
    exp_load_max_th_kbytes =
        soc_DLB_LAG_VLA_EXPECTED_LOADING_THRESHOLDm_field32_get
        (unit, &expected_loading_threshold_entry, THRESHOLDf); 
    LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th =
        (exp_load_max_th_kbytes << 3) * 1000 / num_time_units;

    /* Recover min and max imbalance thresholds: 
     * Imbalance threshold (in percentage)
     *     = (imbalance threshold in kbytes) / 
     *       (expected load threshold in kbytes) * 100
     */
    SOC_IF_ERROR_RETURN(READ_DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 14, &imbalance_threshold_entry));
    imbalance_min_th_kbytes =
        soc_DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm_field32_get
        (unit, &imbalance_threshold_entry, THRESHOLD_0f); 
    if ((imbalance_min_th_kbytes >> 20) & 0x1) {
        /* Imbalance threshold is negative. Take absolute value. */
        imbalance_min_th_kbytes = ((~imbalance_min_th_kbytes) + 1) & 0x1fffff; 
        LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th =
            imbalance_min_th_kbytes * 100 / (exp_load_max_th_kbytes) * -1;
    } else {
        LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th =
            imbalance_min_th_kbytes * 100 / (exp_load_max_th_kbytes);
    }

    imbalance_max_th_kbytes =
        soc_DLB_LAG_VLA_MEMBER_IMBALANCE_THRESHOLDm_field32_get
        (unit, &imbalance_threshold_entry, THRESHOLD_6f); 
    if ((imbalance_max_th_kbytes >> 20) & 0x1) {
        /* Imbalance threshold is negative. Take absolute value. */
        imbalance_max_th_kbytes = ((~imbalance_max_th_kbytes) + 1) & 0x1fffff; 
        LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th =
            imbalance_max_th_kbytes * 100 / (exp_load_max_th_kbytes) * -1;
    } else {
        LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th =
            imbalance_max_th_kbytes * 100 / (exp_load_max_th_kbytes);
    }

    /* Recover load weights */
    entries_per_profile = 64;
    for (profile_ptr = 0; profile_ptr < (1 << soc_mem_field_length(unit,
                    DLB_LAG_QUALITY_CONTROLm, PROFILE_PTRf)); profile_ptr++) {
        base_index = profile_ptr * entries_per_profile;
        /* quality = (quantized_tx_load * tx_load_percent +
         *            quantized_qsize * (100 - tx_load_percent)) /
         *           100;
         * quality = ((quantized_tx_load - quantized_qsize) *
         *            tx_load_percent + quantized_qsize * 100) /
         *           100;
         * quality = (quantized_tx_load - quantized_qsize) *
         *           tx_load_percent / 100 + quantized_qsize;
         * tx_load_percent = (quality - quantized_qsize) * 100 /
         *                   (quantized_tx_load - quantized_qsize); 
         *
         * The index to DLB_LAG_QUALITY_MAPPING table consists of
         * base_index + quantized_tx_load * 8 + quantized_qsize,
         * where quantized_tx_load and quantized_qsize range from
         * 0 to 7.
         *
         * With quantized_tx_load = 7 and quantized_qsize = 0, the 
         * index to DLB_LAG_QUALITY_MAPPING table is
         * base_index + 56, and the tx_load_percent expression
         * simplifies to quality * 100 / 7;
         */
        BCM_IF_ERROR_RETURN(READ_DLB_LAG_QUALITY_MAPPINGm(unit,
                    MEM_BLOCK_ANY, base_index + 56,
                    &quality_mapping_entry));
        quality = soc_DLB_LAG_QUALITY_MAPPINGm_field32_get(unit,
                &quality_mapping_entry, QUALITYf);
        LAG_DLB_INFO(unit)->lag_dlb_load_weight[profile_ptr] =
            quality * 100 / 7;
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE

/*
 * Function:
 *     bcm_tr3_lag_dlb_sw_dump
 * Purpose:
 *     Displays LAG DLB information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr3_lag_dlb_sw_dump(int unit)
{
    int i;
    int num_entries_per_profile;
    int num_profiles;
    int rv;
    int ref_count;

    LOG_CLI((BSL_META_U(unit,
                        "LAG DLB Info -\n")));

    /* Print LAG DLB group usage */
    LOG_CLI((BSL_META_U(unit,
                        "    LAG DLB Groups Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_LAG_GROUP_CONTROLm); i++) {
        if (_BCM_LAG_DLB_ID_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print LAG DLB flowset table usage */
    LOG_CLI((BSL_META_U(unit,
                        "    LAG DLB Flowset Table Blocks Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_LAG_FLOWSETm) >> 9; i++) {
        if (_BCM_LAG_DLB_FLOWSET_BLOCK_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print LAG DLB member ID usage */
    LOG_CLI((BSL_META_U(unit,
                        "    LAG DLB Member IDs Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_LAG_MEMBER_ATTRIBUTEm); i++) {
        if (_BCM_LAG_DLB_MEMBER_ID_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print sample rate and threshold parameters */
    LOG_CLI((BSL_META_U(unit,
                        "    Sample rate: %d per second\n"),
             LAG_DLB_INFO(unit)->lag_dlb_sample_rate));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load min threshold: %d mbps\n"),
             LAG_DLB_INFO(unit)->lag_dlb_tx_load_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load max threshold: %d mbps\n"),
             LAG_DLB_INFO(unit)->lag_dlb_tx_load_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue size min threshold: %d cells\n"),
             LAG_DLB_INFO(unit)->lag_dlb_qsize_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue size max threshold: %d cells\n"),
             LAG_DLB_INFO(unit)->lag_dlb_qsize_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Expected load min threshold: %d mbps\n"),
             LAG_DLB_INFO(unit)->lag_dlb_exp_load_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Expected load max threshold: %d mbps\n"),
             LAG_DLB_INFO(unit)->lag_dlb_exp_load_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Imbalance min threshold: %d percent\n"),
             LAG_DLB_INFO(unit)->lag_dlb_imbalance_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Imbalance max threshold: %d percent\n"),
             LAG_DLB_INFO(unit)->lag_dlb_imbalance_max_th));

    /* Print quality mapping profiles */
    LOG_CLI((BSL_META_U(unit,
                        "    Quality mapping profiles:\n")));
    num_entries_per_profile = 64;
    num_profiles = soc_mem_index_count(unit, DLB_LAG_QUALITY_MAPPINGm) /
        num_entries_per_profile;
    for (i = 0; i < num_profiles; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "      Profile %d: load weight %d percent, "),
                 i, LAG_DLB_INFO(unit)->lag_dlb_load_weight[i]));
        rv = soc_profile_mem_ref_count_get(unit, 
                LAG_DLB_INFO(unit)->lag_dlb_quality_map_profile,
                i * num_entries_per_profile, &ref_count);
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "ref count %d\n"), ref_count));
    }

    return;
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */


#endif /* BCM_TRIUMPH3_SUPPORT */
