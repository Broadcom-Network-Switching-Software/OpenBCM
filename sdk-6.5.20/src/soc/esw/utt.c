
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC UTT Memory (Table) Utilities
 */
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#ifdef BCM_UTT_SUPPORT
#include <soc/esw/utt.h>
#endif
#ifdef BCM_HURRICANE4_SUPPORT
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
#include <soc/lpm.h>
#endif

#ifdef BCM_UTT_SUPPORT
/************************************************************************
 *********************** MACROS DEFINITION ******************************
 */
#define _SOC_UTT_IF_ERROR_GOTO_CLEANUP(_rv_)                \
    do {                                                    \
        if (SOC_FAILURE(_rv_)) {                            \
            goto cleanup;                                   \
        }                                                   \
    } while (0)

/************************************************************************
 *********************** GLOBAL VARAIABLES ******************************
 */
soc_utt_control_t *soc_utt_control[SOC_MAX_NUM_DEVICES];

/************************************************************************
 ********************* IFP related UTT Functions ************************
 */
/*
 * @name    - _soc_utt_ifp_mem_config
 * @param   - unit: unit number
 *
 * @purpose - Update index_max of IFP tables views as per tcams units allocated.
 * @returns - SOC_E_XXXX
 */
static int
_soc_utt_ifp_mem_config(int unit)
{
    int num_tcams;
    soc_persist_t *sop;

    num_tcams = SOC_UTT_AGENT_NUM_TCAMS(unit, SOC_UTT_AGENT_IFP);

    sop = SOC_PERSIST(unit);

    sop->memState[IFP_TCAM_WIDEm].index_max =
        num_tcams * SOC_UTT_TCAM_DEPTH(unit) - 1;

    sop->memState[IFP_TCAM_WIDE_PIPE0m].index_max =
        sop->memState[IFP_TCAM_WIDEm].index_max;

    sop->memState[IFP_TCAMm].index_max =
        num_tcams * 2 * SOC_UTT_TCAM_DEPTH(unit) - 1;

    sop->memState[IFP_TCAM_PIPE0m].index_max =
        sop->memState[IFP_TCAMm].index_max;

    if (soc_feature(unit, soc_feature_utt_ver1_5)) {
        sop->memState[IFP_POLICY_TABLEm].index_max =
        sop->memState[IFP_POLICY_TABLE_PIPE0m].index_max =
        sop->memState[IFP_TCAMm].index_max;

        sop->memState[IFP_POLICY_TABLE_WIDEm].index_max =
        sop->memState[IFP_POLICY_TABLE_WIDE_PIPE0m].index_max =
        sop->memState[IFP_TCAM_WIDEm].index_max;
    }
    return SOC_E_NONE;
}

/*
 * @name    - _soc_utt_ifp_feature_reset
 * @param   - unit: unit number
 * @param   - lookups: lookup/slice count
 *
 * @purpose - Sets slice soc feature based on the number of lookups allocated.
 * @returns - SOC_E_XXXX
 */
static int
_soc_utt_ifp_feature_reset(int unit, int lookups)
{
    int idx = 0;
    soc_utt_agent_t aid = SOC_UTT_AGENT_IFP;

    SOC_FEATURE_CLEAR(unit, soc_feature_field_slices18);

    switch (lookups) {
    case 8:
        SOC_FEATURE_SET(unit, soc_feature_field_slices8);
        break;
    case 9:
        SOC_FEATURE_SET(unit, soc_feature_field_slices9);
        break;
    case 10:
        SOC_FEATURE_SET(unit, soc_feature_field_slices10);
        break;
    case 11:
        SOC_FEATURE_SET(unit, soc_feature_field_slices11);
        break;
    case 12:
        SOC_FEATURE_SET(unit, soc_feature_field_slices12);
        break;
    case 13:
        SOC_FEATURE_SET(unit, soc_feature_field_slices13);
        break;
    case 14:
        SOC_FEATURE_SET(unit, soc_feature_field_slices14);
        break;
    case 15:
        SOC_FEATURE_SET(unit, soc_feature_field_slices15);
        break;
    case 16:
        SOC_FEATURE_SET(unit, soc_feature_field_slices16);
        break;
    case 17:
        SOC_FEATURE_SET(unit, soc_feature_field_slices17);
        break;
    case 18:
        SOC_FEATURE_SET(unit, soc_feature_field_slices18);
        break;
    default:
        return SOC_E_PARAM;
    }

    if(soc_feature(unit, soc_feature_utt_ver1_5)) {
        for(idx = 0; idx < lookups; idx++) {
            /* Check whether all slices have same depth. If not enable Two slice Types feature */
            if ((idx) &&
                (SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) * SOC_UTT_TCAM_DEPTH(unit) !=
                SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, (idx - 1)) * SOC_UTT_TCAM_DEPTH(unit))) {
                SOC_FEATURE_SET(unit, soc_feature_field_ingress_two_slice_types);
                break;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * @name    - _soc_utt_ifp_mem_process_config_var
 * @param   - unit: unit number
 * @param   - ifp_tcams: Number of IFP Tcams allocated
 * @param   - ifp_mem_size: Total mem size allocated to IFP
 *
 * @purpose - Process IFP related config variables
 * @returns - SOC_E_XXXX
 */
static int
_soc_utt_ifp_mem_process_config_var(int unit, int ifp_tcams, int ifp_mem_size)
{
    int rv;
    int idx;
    int idx1;
    int value;
    int count;
    int depth[2] = {0};
    int ifp_slices = 0;
    int tcam_count = 0;
    soc_utt_agent_t aid;
    int *tcam_size_arr = NULL;

    aid = SOC_UTT_AGENT_IFP;
    rv = SOC_E_NONE;

    /* Validate min and max count */
    if ((ifp_tcams > SOC_UTT_AGENT_MAX_TCAMS(unit, aid)) ||
        (ifp_tcams < SOC_UTT_AGENT_MIN_TCAMS(unit, aid))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "IFP TCAM[%d] not in range Min[%d] Max[%d]\n"),
                   ifp_tcams, SOC_UTT_AGENT_MIN_TCAMS(unit, aid),
                   SOC_UTT_AGENT_MAX_TCAMS(unit, aid)));
        return SOC_E_PARAM;
    }

    SOC_UTT_AGENT_NUM_TCAMS(unit, aid) = ifp_tcams;
    SOC_UTT_AGENT_BANK_START(unit, aid) = SOC_UTT_IFP_TCAM_START;

    if (ifp_tcams <= SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid)) {
        ifp_slices = ifp_tcams;
    } else {
        ifp_slices = SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid);
    }

    /*
     * UTT 1.5: Slices to be in multiples of 3
     * Normalize default slice count
     */
    if (soc_feature(unit, soc_feature_utt_ver1_5) && (ifp_slices % 3)) {
        /*
         * Try increasing num slices
         * Ensure Max slice count is not exceed and
         * each slice can get atleast one TCAM unit
         */
        if ((ifp_tcams >= (((ifp_slices / 3 ) + 1) * 3)) &&
            (SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid) >=
            ((ifp_slices / 3) + 1) * 3)) {
            ifp_slices = ((ifp_slices / 3) + 1) * 3;
        } else
        /*
         * Try decreasing num slices
         * Ensure Min slice count is maintained and
         * each slice max tcam unit is not exceeded
         */
        if ((ifp_tcams < (((ifp_slices / 3 ) * 3) *
             SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid))) &&
            (SOC_UTT_AGENT_MIN_TCAMS(unit, aid) <= ((ifp_slices / 3 ) * 3))) {
            ifp_slices = (ifp_slices / 3) * 3;
        } else {
            return SOC_E_PARAM;
        }
    }

    /*
     * Get Number of IFP lookups required.
     */
    ifp_slices = soc_property_get(unit, spn_IFP_NUM_LOOKUPS, ifp_slices);
    if (ifp_slices > SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "IFP Lookups[%d] > MAX[%d]\n"),
                   ifp_slices, SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid)));
        return SOC_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_utt_ver1_5) &&
        (ifp_slices < SOC_UTT_AGENT_MIN_TCAMS(unit, aid))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "IFP Lookups[%d] < MIN[%d]\n"),
                   ifp_slices, SOC_UTT_AGENT_MIN_TCAMS(unit, aid)));
        return SOC_E_PARAM;
    }
    if (soc_feature(unit, soc_feature_utt_ver1_5) && (ifp_slices % 3)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "IFP Lookups[%d] not in multiples of 3\n"),
                   ifp_slices));
        return SOC_E_PARAM;
    }
    SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid) = ifp_slices;

    SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) =
        sal_alloc(sizeof(soc_utt_agent_lookup_info_t) * ifp_slices,
                  "soc_utt_agent_lookup_info_t");
    if (SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid), 0,
               (sizeof(soc_utt_agent_lookup_info_t) * ifp_slices));

    tcam_size_arr = sal_alloc(sizeof(int) * ifp_slices * 2, "temp slice array");
    if (tcam_size_arr == NULL) {
        return SOC_E_MEMORY;
    }

    count = 0;
    for (idx = 0 ; idx < ifp_slices ; idx++) {
        /*
         * Get indivdual size of each IFP lookup
         */
        value = soc_property_suffix_num_get(unit, idx,
                                            spn_IFP_LOOKUP_DEPTH, "", 0);
        if ((value % SOC_UTT_TCAM_DEPTH(unit)) || (value < 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "IFP lookup depth not multiples of [%d]\n"),
                      SOC_UTT_TCAM_DEPTH(unit)));
            rv = SOC_E_PARAM;
            _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
        }

        value = value / SOC_UTT_TCAM_DEPTH(unit);

        if (value > SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "IFP lookup depth[%d] more than max[%d]\n"),
                       value * SOC_UTT_TCAM_DEPTH(unit),
                       (SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid) *
                        SOC_UTT_TCAM_DEPTH(unit))));
            rv = SOC_E_PARAM;
            _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
        }

        if (value == 0) {
            count++;
        }

        tcam_size_arr[idx] = value;
        tcam_count += value;
    }

    if (tcam_count > ifp_tcams) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "IFP lookup depth exceeds total mem size\n")));
        rv = SOC_E_PARAM;
        _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
    }

    /* Unreserved TCAM count */
    tcam_count = ifp_tcams - tcam_count;

    /* Distribute remaining TCAM's among non-configured IFP slices */
    if (count) {
        if ((count * SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid)) < tcam_count) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Remaining lookup's MAX depth[%d] less"
                                  " than configured TCAMS[%d]\n"),
                       count * SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid) *
                       SOC_UTT_TCAM_DEPTH(unit),
                       (tcam_count * SOC_UTT_TCAM_DEPTH(unit))));
            rv = SOC_E_PARAM;
            _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
        }
        value = tcam_count / count;
        if (value == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "IFP not enough TCAMs left for lookups\n")));
            rv = SOC_E_PARAM;
            _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
        }

        for (idx = 0 ; idx < ifp_slices ; idx++) {
            if (tcam_size_arr[idx] == 0) {
                tcam_size_arr[ifp_slices + idx] = value;
                tcam_count -= value;
            }
        }

        for (idx = 0 ; (idx < ifp_slices) && (tcam_count > 0) ; idx++) {
            if (tcam_size_arr[idx] == 0) {
                tcam_size_arr[ifp_slices + idx]++;
                tcam_count--;
            }
        }
    }
    if (tcam_count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "IFP TCAM's count is more invalid config!!!\n")));
        rv = SOC_E_PARAM;
        _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
    }
    for (idx = 0 ; idx < ifp_slices ; idx++) {
        if (tcam_size_arr[idx] == 0) {
            value = tcam_size_arr[ifp_slices + idx];
        } else {
            value = tcam_size_arr[idx];
        }
        if (value == 0) {
            rv = SOC_E_INTERNAL;
            _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
        }
        SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) = value;
        if (idx > 0) {
            SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx) =
                SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx - 1) +
                SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx - 1);
        } else {
            SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx) = 0;
        }
    }
    SOC_UTT_AGENT_BANK_COUNT(unit, aid) = ifp_slices;
    /*
     * Banks required will depend on MAX number
     * of TCAM's in each super MACRO
     */
    if(soc_feature(unit, soc_feature_utt_ver1_5)) {
        idx = 0;
        /* Validate types of ifp_lookup_depths */
        depth[0] = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) * SOC_UTT_TCAM_DEPTH(unit);
        for(idx = 0; idx < ifp_slices; idx++) {
            if (SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) * SOC_UTT_TCAM_DEPTH(unit) \
                != depth[0]) {
                if (0 == depth[1]) {
                    depth[1] = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) \
                                  * SOC_UTT_TCAM_DEPTH(unit);
                } else if (SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) \
                        * SOC_UTT_TCAM_DEPTH(unit) != depth[1]) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Only 2 types of IFP lookup depths are allowed"
                                          " at a time!!!\n")));
                    rv = SOC_E_CONFIG;
                    _SOC_UTT_IF_ERROR_GOTO_CLEANUP(rv);
                }
            }
        }
        idx = 0;
        count = 0;
        while (idx < ifp_slices) {
            value = 0;
            for (idx1 = 0 ; idx1 < 6 && (idx + idx1) < ifp_slices; idx1++) {
                if (value < SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, (idx + idx1))) {
                    value = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, (idx + idx1));
                }
            }
            idx += idx1;
            count += value;
        }
        SOC_UTT_AGENT_BANK_COUNT(unit, aid) = count;
    }

cleanup:
    if (tcam_size_arr) {
        sal_free(tcam_size_arr);
        tcam_size_arr = NULL;
    }
    return rv;
}

/*
 * @name    - soc_utt_ifp_bank_config
 * @param   - unit: unit number
 *
 * @purpose - Programs IFP related UTT memories
 * @returns - SOC_E_XXXX
 */
int
soc_utt_ifp_bank_config(int unit)
{
    soc_mem_t mem;
    int num_lookups;
    int bank_start;
    soc_utt_agent_t aid;
    int idx, idx_max, bank;
    utt_bank_decode_entry_t lookup_entry;
    utt_logical_lookup_to_tcam_map_entry_t map_entry;
    ifp_policy_utt_addr_xlate_entry_t entry;

    aid = SOC_UTT_AGENT_IFP;
    num_lookups = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid);

    /* soc features are reinitialized during do_init */
    SOC_IF_ERROR_RETURN
        (_soc_utt_ifp_feature_reset(unit, num_lookups));

    mem = UTT_BANK_DECODEm;

    bank_start = SOC_UTT_AGENT_BANK_START(unit, aid);
    if (soc_feature(unit, soc_feature_utt_ver1_5)) {
        int total_tcams, num_banks, tcams_per_bank;
        int tcam_id, phy_tcam_id, idx2, bank_unit;
        int sw_id = 0, banks_per_lkup = 0, incr_step =0;
        int sv=0, sj=0, rem_bank = 0, lookup_sel=0;

        total_tcams = SOC_UTT_AGENT_NUM_TCAMS(unit, aid);
        num_banks = SOC_UTT_AGENT_BANK_COUNT(unit, aid);
        tcams_per_bank = total_tcams/num_banks;

        /* There are 18 IFP lookups in FB6 and 6 TCAMS per bank.
         * As a result, when assigning SW IDs for all the tcams
         *  we will repeat the assignment every 18/6 = 3 banks
         * Now, For different modes, SW IDs increment at step of:
         * Total_banks in that mode / 3 */
        bank_unit = num_lookups / tcams_per_bank;
        banks_per_lkup = num_banks / bank_unit;
        rem_bank = num_banks % bank_unit;

        /* Program the individual banks based on config */
        for (sv=0; sv<num_banks; sv++) {
            sal_memset(&lookup_entry, 0 , sizeof(lookup_entry));
            soc_mem_field32_set(unit, mem, &lookup_entry, AGENT_IDf,
                    SOC_UTT_AGENT_HW_ID(unit, aid));
            soc_mem_field32_set(unit, mem, &lookup_entry, VALIDf, 0x3f);
            /* Starting from first bank, start filling SW bank ID in
             * increments of the step derived earlier.
             * For unused bank, add additional TCAMS from remaining
             * bank into first bank and susequent banks at the same
             * interval */
            if ((sv % bank_unit) == 0) {
                incr_step = banks_per_lkup + rem_bank;
                lookup_sel = 0;
            } else {
                incr_step = banks_per_lkup;
            }

            for (sj=0; sj<tcams_per_bank; sj++) {
                soc_mem_field32_set(unit, mem, &lookup_entry, SW_BANK_ID_0f + sj, sw_id);
                sw_id += incr_step;

                if (sw_id >= total_tcams) {
                    sw_id -= total_tcams;
                    sw_id += 1;
                }
            }
            soc_mem_field32_set(unit, mem, &lookup_entry,
                    LOOKUP_SEL_0f, lookup_sel);
            lookup_sel++;
            soc_mem_field32_set(unit, mem, &lookup_entry,
                    LOOKUP_SEL_1f, lookup_sel);
            lookup_sel++;
            SOC_IF_ERROR_RETURN
                (WRITE_UTT_BANK_DECODEm(unit, MEM_BLOCK_ALL, sv,
                                        &lookup_entry));
        }
        bank = sv;
        sal_memset(&entry, 0 , sizeof(entry));
        tcam_id = phy_tcam_id = 0;
        for (idx = 0; idx < num_lookups; idx++) {
            /* Each IFP lookup is at a boundary of 4 physical TCAMs which also corresponds to the SRAM. 
             * Run a loop only at an interval 1,2,3 or all 4 constituent TCAMS
             * that are be active depending on the mode used to remap the idx
             * corresponding to the SRAM */
            for (idx2 = 0; idx2 < SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx); idx2++) {
                soc_mem_field32_set(unit, IFP_POLICY_UTT_ADDR_XLATEm, &entry,
                                    SBUS_TABLE_INDEX_MSBf, phy_tcam_id++);
                SOC_IF_ERROR_RETURN
                    (WRITE_IFP_POLICY_UTT_ADDR_XLATEm(unit, MEM_BLOCK_ALL,
                                                      tcam_id++, &entry));
            }
            phy_tcam_id += SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid) - idx2;
        }
    } else {
        idx = bank_start;
        idx_max = bank_start + num_lookups;
        bank = 0;
        for ( ; idx < idx_max ; idx++, bank++) {
            sal_memset(&lookup_entry, 0 , sizeof(lookup_entry));
            soc_mem_field32_set(unit, mem, &lookup_entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, &lookup_entry, AGENT_IDf,
                                SOC_UTT_AGENT_HW_ID(unit, aid));
            soc_mem_field32_set(unit, mem, &lookup_entry, BANK_IDf, bank);
            SOC_IF_ERROR_RETURN
                (WRITE_UTT_BANK_DECODEm(unit, MEM_BLOCK_ALL, idx,
                                        &lookup_entry));
        }

        idx = SOC_UTT_AGENT_LOOKUP_BASE(unit, aid);
        idx_max = idx + num_lookups;

        /* Lookup to TCAM map */
        mem = UTT_LOGICAL_LOOKUP_TO_TCAM_MAPm;
        bank = 0;
        for ( ; idx < idx_max ; idx++, bank++) {
            sal_memset(&map_entry, 0 , sizeof(map_entry));
            soc_mem_field32_set(unit, mem, &map_entry, BITMAPf, 1 << bank);
            SOC_IF_ERROR_RETURN
                (WRITE_UTT_LOGICAL_LOOKUP_TO_TCAM_MAPm(unit, MEM_BLOCK_ALL, idx,
                                                       &map_entry));
        }
    }
    if (bank != SOC_UTT_AGENT_BANK_COUNT(unit, aid)) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_ifp_lookup_depth_get
 * @param   - unit: unit number
 * @param   - num_lookups: Number of IFP lookups
 * @param   - depth_arr: Slice depth of IFP lookups
 *
 * @purpose - Provides IFP slice depth details that can be used at FP module
 * @returns - SOC_E_XXXX
 */
int
soc_utt_ifp_lookup_depth_get(int unit, int num_lookups, int *depth_arr)
{
    int idx;
    soc_utt_agent_t aid = SOC_UTT_AGENT_IFP;

    if ((depth_arr == NULL) ||
        (num_lookups != SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid))) {
        return SOC_E_PARAM;
    }
    /* Fill slice depth */
    for (idx = 0 ; idx < num_lookups ; idx++) {
        depth_arr[idx] =
            SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) *
            SOC_UTT_TCAM_DEPTH(unit);
    }
    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_ifp_index_get
 * @param   - unit: unit number
 * @param   - wide:(in) if index is of wide or narrow view
 * @param   - index:(in/out) index of wide or narrow view
 * @param   - index2:(out) second index of narrow view
 *
 * @purpose - Converts index of one view to another
 * @returns - SOC_E_XXXX
 */
void
soc_utt_ifp_index_get(int unit, int wide, int *index, int *index2, int m)
{
    int i, depth_arr[18], count = 0;
    soc_utt_agent_t aid = SOC_UTT_AGENT_IFP;

    for (i = 0 ; i < SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid) ; i++) {
        depth_arr[i] =
            SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, i) *
            SOC_UTT_TCAM_DEPTH(unit);
        count += depth_arr[i];
        if (wide) {
            /* *index is wide */
            if (*index < count - 1) {
                if (m == 0) {
                    *index += count - depth_arr[i];
                    *index2 += count;
                } else if (m == 1) {
                    *index += count;
                    *index2 += count - depth_arr[i];
                }
                break;
            }
        } else {
            /* *index is narrow */
            if (*index < (count * 2) - 1) {
                *index -= count - depth_arr[i];
                break;
            }
        }
    }
}

/************************************************************************
 ********************* LPM related UTT Functions ************************
 */
/*
 * @name    - _soc_utt_lpm_mem_config
 * @param   - unit: unit number
 *
 * @purpose - Configures LPM related memory sizes
 * @returns - SOC_E_XXXX
 */
static int
_soc_utt_lpm_mem_config(int unit)
{
    int lpm_tcams;
    soc_persist_t *sop;
    int defip_config = 0;
    int config_v6 = 0;
    int num_v6_128b = 0;

    if (!soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        return SOC_E_NONE;
    }

    if (!soc_feature(unit, soc_feature_utt)) {
        return SOC_E_NONE;
    }

    lpm_tcams = SOC_UTT_AGENT_NUM_TCAMS(unit, SOC_UTT_AGENT_LPM);

    SOC_CONTROL(unit)->l3_defip_max_tcams = lpm_tcams;

    SOC_CONTROL(unit)->l3_defip_utt_max_tcams =
        SOC_UTT_AGENT_MAX_TCAMS(unit, SOC_UTT_AGENT_LPM);

    if (soc_feature(unit, soc_feature_utt_ver1_5)) {
        SOC_CONTROL(unit)->l3_defip_utt_tcams_per_lookup =
            lpm_tcams / SOC_UTT_AGENT_MAX_LOOKUPS(unit, SOC_UTT_AGENT_LPM);
        if (SOC_CONTROL(unit)->l3_defip_utt_tcams_per_lookup % 2 != 0) {
            SOC_CONTROL(unit)->l3_defip_utt_tcams_per_lookup++;
        }
    } else {
        SOC_CONTROL(unit)->l3_defip_utt_tcams_per_lookup =
            SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, SOC_UTT_AGENT_LPM);
    }

    SOC_CONTROL(unit)->l3_defip_tcam_size = SOC_UTT_TCAM_DEPTH(unit);

    sop = SOC_PERSIST(unit);

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

    num_v6_128b = soc_property_get(unit,
                                   spn_NUM_IPV6_LPM_128B_ENTRIES,
                                   (defip_config ? (lpm_tcams / 4) * SOC_UTT_TCAM_DEPTH(unit) : 0));

    num_v6_128b = num_v6_128b + (num_v6_128b % 2);

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        int alpm_mode = _soc_alpm_mode[unit];

        /* Ensure that given config can support pre-filter
         */
        if (alpm_mode == 2) {
            num_v6_128b = (num_v6_128b + 3) / 4 * 4;
        }
    }
#endif

    config_v6 = num_v6_128b;
    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        num_v6_128b = 0;
        if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
            config_v6 = ((config_v6 / SOC_CONTROL(unit)->l3_defip_tcam_size) +
                         ((config_v6 % SOC_CONTROL(unit)->l3_defip_tcam_size)
                          ? 1 : 0)) * SOC_CONTROL(unit)->l3_defip_tcam_size;
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_utt_lpm_mem_config_validate(unit, config_v6));

    SOC_IF_ERROR_RETURN
        (soc_utt_lpm_paired_tcams(unit, config_v6,
                                  &(SOC_CONTROL(unit)->l3_defip_utt_paired_tcams)));

    sop->memState[L3_DEFIP_PAIR_128m].index_max = num_v6_128b - 1;
    sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = num_v6_128b - 1;
    sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = num_v6_128b - 1;
    sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = num_v6_128b - 1;

    sop->memState[L3_DEFIPm].index_max =
        (SOC_CONTROL(unit)->l3_defip_max_tcams *
         SOC_CONTROL(unit)->l3_defip_tcam_size) -
        (num_v6_128b * 2) - 1;

    sop->memState[L3_DEFIP_ONLYm].index_max =
        sop->memState[L3_DEFIPm].index_max;
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
        sop->memState[L3_DEFIPm].index_max;
    sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
        sop->memState[L3_DEFIPm].index_max;
    sop->memState[L3_DEFIP_AUX_TABLEm].index_max =
        sop->memState[L3_DEFIPm].index_max;

    SOC_CONTROL(unit)->l3_defip_index_remap = num_v6_128b;
    SOC_IF_ERROR_RETURN(soc_l3_defip_indexes_init(unit, config_v6));

    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_lpm_bank_config
 * @param   - unit: unit number
 * @param   - urpf: urpf status
 * @param   - tcam_pair: TCAM pair count
 *
 * @purpose - Programs LPM related UTT memories
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_bank_config(int unit, int urpf, int tcam_pair)
{
    soc_mem_t mem;
    int lpm_banks;
    int bank_start;
    int bit_map, bit_map1;
    int i, idx, idx_max, bank;
    int tcam_map_idx, tcam_count;
    int lkp_idx, lkp_base, bank_decode_idx;
    int tcams_per_lkp, tcam_pairs_per_lkp;
    soc_utt_agent_t aid = SOC_UTT_AGENT_LPM;
    utt_bank_decode_entry_t lookup_entry;
    utt_logical_lookup_to_tcam_map_entry_t map_entry;
    utt_logical_lookup_to_tcam_map_entry_t map_entry_1;

    if (!soc_property_get(unit, "l3_defip_sizing", TRUE) ||
        !soc_feature(unit, soc_feature_utt)) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_utt_ver1_5)) {
        lpm_banks = SOC_UTT_AGENT_NUM_TCAMS(unit, aid) / 6;
        if (SOC_UTT_AGENT_NUM_TCAMS(unit, aid) % 6) {
            lpm_banks++;
        }
    } else {
        lpm_banks = SOC_UTT_AGENT_NUM_TCAMS(unit, aid);
    }
    bank_start = SOC_UTT_AGENT_BANK_START(unit, aid);
    idx_max = bank_start + lpm_banks;

    if (idx_max > soc_mem_index_count(unit, UTT_BANK_DECODEm)) {
        return SOC_E_INTERNAL;
    }
    /* Reset Memory */
    mem = UTT_BANK_DECODEm;
    for (idx = bank_start; idx < idx_max; idx++) {
        SOC_IF_ERROR_RETURN
            (WRITE_UTT_BANK_DECODEm(unit, MEM_BLOCK_ALL,
                                    idx, soc_mem_entry_null(unit, mem)));
    }

if (soc_feature(unit, soc_feature_utt_ver1_5)) {
    int sw_tcam_id, dw_tcam_id, phy_tcam_id, lpm_tcams, tcam_count[4] = {0};
    defip_data_utt_addr_xlate_entry_t entry;
    lkp_idx = -1;
    sw_tcam_id = dw_tcam_id = lpm_tcams = 0;

    for (idx = bank_start; idx < idx_max; idx++) {
        sal_memset(&lookup_entry, 0 , sizeof(lookup_entry));
        for (i = 0; i < 6 && lpm_tcams < SOC_UTT_AGENT_NUM_TCAMS(unit, aid); i++) {
            soc_mem_field32_set(unit, mem, &lookup_entry, AGENT_IDf,
                                SOC_UTT_AGENT_HW_ID(unit, aid));
            soc_mem_field32_set(unit, mem, &lookup_entry, INPUT_MUX_MODEf, 1);

            soc_mem_field32_set(unit, mem, &lookup_entry, SW_BANK_ID_0f + i, sw_tcam_id);
            soc_mem_field32_set(unit, mem, &lookup_entry, VALIDf,
                                (2 << i) - 1);
            if (i % 2 == 0) {
                lkp_idx++;
                soc_mem_field32_set(unit, mem, &lookup_entry, LOOKUP_SEL_0f + i / 2,
                                    lkp_idx % 4);
                soc_mem_field32_set(unit, mem, &lookup_entry, DW_BANK_ID_0f + i / 2, dw_tcam_id);

                lkp_idx = (lkp_idx % 4) + (urpf ? 4 : 0);
                dw_tcam_id += SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, lkp_idx) / 2;
            } else {
                if (++tcam_count[lkp_idx % 4] >
                    SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, lkp_idx) / 2) {
                    i -= 2;
                    lpm_tcams -= 2;
                }
            }
            sw_tcam_id += SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, lkp_idx) / 2;
            if (sw_tcam_id >= SOC_UTT_AGENT_NUM_TCAMS(unit, aid)) {
                sw_tcam_id = (sw_tcam_id % SOC_UTT_AGENT_NUM_TCAMS(unit, aid)) + 1;
                dw_tcam_id = (dw_tcam_id % (SOC_UTT_AGENT_NUM_TCAMS(unit, aid) / 2)) + 1;
            }
            lpm_tcams++;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_UTT_BANK_DECODEm(unit, MEM_BLOCK_ALL,
                                    idx, &lookup_entry));
    }
    sal_memset(&entry, 0 , sizeof(entry));
    sw_tcam_id = phy_tcam_id = 0;
    for (idx = 0; idx < SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid) * 2; idx++) {
        for (i = 0; i < SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, urpf ? idx/2 + 4 : idx/2) / 2; i++) {
            soc_mem_field32_set(unit, L3_DEFIP_DATA_UTT_ADDR_XLATEm, &entry,
                                XLATE_ADDRf, phy_tcam_id++);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_DEFIP_AUX_UTT_ADDR_XLATEm(unit, MEM_BLOCK_ALL,
                                                    sw_tcam_id, &entry));
            SOC_IF_ERROR_RETURN
                (WRITE_L3_DEFIP_HIT_UTT_ADDR_XLATEm(unit, MEM_BLOCK_ALL,
                                                    sw_tcam_id, &entry));
            SOC_IF_ERROR_RETURN
                (WRITE_L3_DEFIP_DATA_UTT_ADDR_XLATEm(unit, MEM_BLOCK_ALL,
                                                     sw_tcam_id++, &entry));
        }

        phy_tcam_id += (SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid) / 2) - i;
    }

    dw_tcam_id = phy_tcam_id = 0;
    for (idx = 0; idx < SOC_UTT_AGENT_MAX_LOOKUPS(unit, aid); idx++) {
        for (i = 0; i < SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, urpf ? idx + 4 : idx) / 2; i++) {
            soc_mem_field32_set(unit, L3_DEFIP_PAIR_128_HIT_UTT_ADDR_XLATEm, &entry,
                                XLATE_ADDRf, phy_tcam_id++);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_DEFIP_PAIR_128_HIT_UTT_ADDR_XLATEm(unit, MEM_BLOCK_ALL,
                                                             dw_tcam_id, &entry));
            SOC_IF_ERROR_RETURN
                (WRITE_L3_DEFIP_PAIR_128_DATA_UTT_ADDR_XLATEm(unit, MEM_BLOCK_ALL,
                                                              dw_tcam_id++, &entry));
        }

        phy_tcam_id += (SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid) / 2) - i;
    }
} else {

    tcams_per_lkp = SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(unit, aid);
    tcam_pairs_per_lkp = tcams_per_lkp / 2;

    lkp_base = SOC_UTT_AGENT_LOOKUP_BASE(unit, aid);

    /*
     * UTT BANK MAP
     * Maps UTT bank to a logical L3_DEFIP bank
     */
    mem = UTT_BANK_DECODEm;
    bank_decode_idx = bank_start;

    idx = idx_max = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid);
    if (urpf) {
        idx = idx / 2;
    } else {
        idx = 0;
        idx_max = idx_max / 2;
    }
    for (; idx < idx_max; idx++) {
        if (urpf) {
            bank = (idx - SOC_UTT_AGENT_DEF_NUM_LOOKUPS(unit, aid));
        } else {
            bank = idx;
        }
        bank *= tcams_per_lkp;
        tcam_count = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);
        for (i = 0; i < tcam_count; i++, bank++, bank_decode_idx++) {
            sal_memset(&lookup_entry, 0 , sizeof(lookup_entry));
            soc_mem_field32_set(unit, mem, &lookup_entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, &lookup_entry, AGENT_IDf,
                                SOC_UTT_AGENT_HW_ID(unit, aid));
            soc_mem_field32_set(unit, mem, &lookup_entry, BANK_IDf, bank);
            SOC_IF_ERROR_RETURN
                (WRITE_UTT_BANK_DECODEm(unit, MEM_BLOCK_ALL,
                                        bank_decode_idx, &lookup_entry));

            /* Paired TCAM */
            if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx)) {
                soc_mem_field32_set(unit, mem, &lookup_entry, BANK_IDf,
                                    bank + tcam_pairs_per_lkp);
                SOC_IF_ERROR_RETURN
                    (WRITE_UTT_BANK_DECODEm(unit, MEM_BLOCK_ALL,
                                            (bank_decode_idx + 1),
                                            &lookup_entry));
                i++;
                bank_decode_idx++;
            }
        }
    }

    /*
     * Lookup to UTT TCAM map.
     * 2 index's per lookup (even & odd)
     * For paired lookup's bitmap needs to be
     * split for even and odd indexes.
     * For non-paired lookup's even index bitmap should be fine.
     */
    mem = UTT_LOGICAL_LOOKUP_TO_TCAM_MAPm;
    bank = bank_start;
    tcam_map_idx = 0;
    lkp_idx = lkp_base;

    idx = idx_max = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid);
    if (urpf) {
        idx = idx / 2;
    } else {
        idx = 0;
        idx_max = idx_max / 2;
    }

    for (; idx < idx_max; idx++, lkp_idx += 2) {
        sal_memset(&map_entry, 0 , sizeof(map_entry));
        sal_memset(&map_entry_1, 0 , sizeof(map_entry_1));
        bit_map = bit_map1 = 0;
        tcam_count = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);
        /* Paired TCAM */
        if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx)) {
            for (i = 0; i < tcam_count; i++, bank++, tcam_map_idx++) {
                bit_map |= (1 << bank);
                bank++;
                i++;
                bit_map1 |= (1 << bank);
            }
        } else {
            /*
             * UnPaired TCAM, but need both lookups to be setup.
             * eg: If lookup has 4 TCAM, LO as 0x3 and L1 as 0xC.
             */
            for (i = 0; i < tcam_count; i++, bank++, tcam_map_idx++) {
                if (i < tcam_pairs_per_lkp) {
                    bit_map |= (1 << bank);
                } else {
                    bit_map1 |= (1 << bank);
                }
            }
        }
        soc_mem_field32_set(unit, mem, &map_entry, BITMAPf, bit_map);
        SOC_IF_ERROR_RETURN
            (WRITE_UTT_LOGICAL_LOOKUP_TO_TCAM_MAPm(unit, MEM_BLOCK_ALL,
                                                   lkp_idx,
                                                   &map_entry));

        soc_mem_field32_set(unit, mem, &map_entry_1, BITMAPf, bit_map1);
        SOC_IF_ERROR_RETURN
            (WRITE_UTT_LOGICAL_LOOKUP_TO_TCAM_MAPm(unit, MEM_BLOCK_ALL,
                                                   (lkp_idx + 1),
                                                   &map_entry_1));
    }
}

    return SOC_E_NONE;
}

/************************************************************************
 ********************* Common UTT Functions *****************************
 */
/*
 * @name    - _soc_utt_mem_process_config_var
 * @param   - unit: unit number
 *
 * @purpose - Process UTT relatd config variables
 * @returns - SOC_E_XXXX
 */
static int
_soc_utt_mem_process_config_var(int unit)
{
    int ifp_tcams = 0;
    int lpm_tcams = 0;
    int ifp_def_size = 0;
    int lpm_def_size = 0;
    int ifp_mem_size = 0;
    int lpm_mem_size = 0;
    soc_utt_agent_t aid;

    /* Default table size */
    ifp_def_size = SOC_UTT_AGENT_DEF_TABLE_SIZE(unit, SOC_UTT_AGENT_IFP);
    lpm_def_size = SOC_UTT_AGENT_DEF_TABLE_SIZE(unit, SOC_UTT_AGENT_LPM);

    /*
     * Get configured IFP mem size.
     */
    ifp_mem_size = soc_property_get(unit, spn_IFP_MEM_SIZE, 0);

    /*
     * Get configured LPM mem size.
     */
    lpm_mem_size = soc_property_get(unit, spn_LPM_MEM_SIZE, 0);

    /* Normalize to multiple of TCAM size */
    ifp_mem_size = (((ifp_mem_size + SOC_UTT_TCAM_DEPTH(unit) - 1)
                     / SOC_UTT_TCAM_DEPTH(unit))
                    * SOC_UTT_TCAM_DEPTH(unit));

    /* Normalize to multiple of TCAM size */
    lpm_mem_size = (((lpm_mem_size + SOC_UTT_TCAM_DEPTH(unit) - 1)
                     / SOC_UTT_TCAM_DEPTH(unit))
                    * SOC_UTT_TCAM_DEPTH(unit));

    if (lpm_mem_size != 0) {
        lpm_tcams = lpm_mem_size / SOC_UTT_TCAM_DEPTH(unit);
        /* Validate lpm size
         * UTT1.0: TCAMs to be multiple of 2
         * UTT1.5: TCAMs to be multiple of 4
         */
        if ((soc_feature(unit, soc_feature_utt_ver1_5) && (lpm_tcams & 0x3)) ||
            (lpm_tcams & 0x1)) {
            return SOC_E_CONFIG;
        }
    }

    /* Validate config data */
    if (ifp_mem_size == 0 && lpm_mem_size == 0) {
        ifp_mem_size = ifp_def_size;
        lpm_mem_size = lpm_def_size;
    } else if (ifp_mem_size == 0) {
        ifp_mem_size = ((SOC_UTT_MAX_TCAMS(unit) * SOC_UTT_TCAM_DEPTH(unit))
                        - lpm_mem_size);
        if (ifp_mem_size >
            (SOC_UTT_AGENT_MAX_TCAMS(unit, SOC_UTT_AGENT_IFP)
             *  SOC_UTT_TCAM_DEPTH(unit))) {
            ifp_mem_size = (SOC_UTT_AGENT_MAX_TCAMS(unit, SOC_UTT_AGENT_IFP) *
                            SOC_UTT_TCAM_DEPTH(unit));
        }
    } else if (lpm_mem_size == 0) {
        lpm_mem_size = ((SOC_UTT_MAX_TCAMS(unit) * SOC_UTT_TCAM_DEPTH(unit))
                        - ifp_mem_size);
        if (lpm_mem_size >
            (SOC_UTT_AGENT_MAX_TCAMS(unit, SOC_UTT_AGENT_LPM)
             *  SOC_UTT_TCAM_DEPTH(unit))) {
            lpm_mem_size = (SOC_UTT_AGENT_MAX_TCAMS(unit, SOC_UTT_AGENT_LPM) *
                            SOC_UTT_TCAM_DEPTH(unit));
        }
        lpm_tcams = lpm_mem_size / SOC_UTT_TCAM_DEPTH(unit);
        if (soc_feature(unit, soc_feature_utt_ver1_5)) {
            lpm_tcams &= (~0x3);
        } else {
            lpm_tcams &= (~0x1);
        }
        lpm_mem_size = lpm_tcams * SOC_UTT_TCAM_DEPTH(unit);
    }

    ifp_tcams = ifp_mem_size / SOC_UTT_TCAM_DEPTH(unit);
    lpm_tcams = lpm_mem_size / SOC_UTT_TCAM_DEPTH(unit);

    if ((ifp_tcams + lpm_tcams) > SOC_UTT_MAX_TCAMS(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Total TCAM requested[%d] > MAX %d\n"),
                   ifp_tcams + lpm_tcams, SOC_UTT_MAX_TCAMS(unit)));
        return SOC_E_PARAM;
    }

    /* IFP */
    SOC_IF_ERROR_RETURN
        (_soc_utt_ifp_mem_process_config_var(unit, ifp_tcams, ifp_mem_size));

    /* LPM */
    aid = SOC_UTT_AGENT_LPM;

    /* Validate min and max count */
    if ((lpm_tcams > SOC_UTT_AGENT_MAX_TCAMS(unit, aid)) ||
        (lpm_tcams < SOC_UTT_AGENT_MIN_TCAMS(unit, aid))) {
        return SOC_E_PARAM;
    }

    SOC_UTT_AGENT_NUM_TCAMS(unit, aid) = lpm_tcams;
    SOC_UTT_AGENT_BANK_START(unit, aid) =
        SOC_UTT_AGENT_BANK_COUNT(unit, SOC_UTT_AGENT_IFP);

    /* First half for normal mode & Second Half for urpf mode */
    SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid) =
        SOC_UTT_AGENT_DEF_NUM_LOOKUPS(unit, aid) * 2;

    SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) =
        sal_alloc(sizeof(soc_utt_agent_lookup_info_t) *
                  SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid),
                  "soc_utt_agent_lookup_info_t");
    if (SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid), 0,
               (sizeof(soc_utt_agent_lookup_info_t) *
               SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid)));

    LOG_DEBUG(BSL_LS_SOC_LPM,
              (BSL_META_U(unit, "IFP tcams[%d] lookups[%d] LPM tcams[%d]\n"),
               SOC_UTT_AGENT_NUM_TCAMS(unit, SOC_UTT_AGENT_IFP),
               SOC_UTT_AGENT_NUM_LOOKUPS(unit, SOC_UTT_AGENT_IFP),
               SOC_UTT_AGENT_NUM_TCAMS(unit, SOC_UTT_AGENT_LPM)));

    return SOC_E_NONE;
}

/*
 * @name    - _soc_utt_mem_config
 * @param   - unit: unit number
 *
 * @purpose - Read config varaible and allocate mem for IFP and LPM.
 * @returns - SOC_E_XXXX
 */
static int
_soc_utt_mem_config(int unit)
{
    /* Config variables read, validate and populate metadata */
    SOC_IF_ERROR_RETURN(_soc_utt_mem_process_config_var(unit));
    /* IFP */
    SOC_IF_ERROR_RETURN(_soc_utt_ifp_mem_config(unit));
    /* LPM */
    SOC_IF_ERROR_RETURN(_soc_utt_lpm_mem_config(unit));

    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_init
 * @param   - unit: unit number
 *
 * @purpose - UTT module init
 * @returns - SOC_E_XXXX
 */
int
soc_utt_init(int unit)
{
    if (SOC_UTT_CONTROL(unit) != NULL) {
        SOC_IF_ERROR_RETURN(soc_utt_deinit(unit));
        if (SOC_UTT_CONTROL(unit) != NULL) {
            return SOC_E_INTERNAL;
        }
    }

    SOC_UTT_CONTROL(unit) =
        sal_alloc(sizeof (soc_utt_control_t), "soc_utt_control_t");
    if (SOC_UTT_CONTROL(unit) == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(SOC_UTT_CONTROL(unit), 0, sizeof (soc_utt_control_t));

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        SOC_IF_ERROR_RETURN(soc_hr4_utt_init(unit));
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        SOC_IF_ERROR_RETURN(soc_fb6_utt_init(unit));
    }
#endif

    SOC_IF_ERROR_RETURN(_soc_utt_mem_config(unit));

    return SOC_E_NONE;
}


/*
 * @name    - soc_utt_deinit
 * @param   - unit: unit number
 *
 * @purpose - UTT module deinit
 * @returns - SOC_E_XXXX
 */
int
soc_utt_deinit(int unit)
{
    int idx;

    if (SOC_UTT_CONTROL(unit) != NULL) {
        for (idx = 0 ; idx < SOC_UTT_AGENT_MAX; idx++) {
            if (SOC_UTT_AGENT_LOOKUP_INFO(unit, idx)) {
                sal_free(SOC_UTT_AGENT_LOOKUP_INFO(unit, idx));
                SOC_UTT_AGENT_LOOKUP_INFO(unit, idx) = NULL;
            }
        }
        sal_free(SOC_UTT_CONTROL(unit));
        SOC_UTT_CONTROL(unit) = NULL;
    }
    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_lpm_mem_config_validate
 * @param   - unit: unit number
 * @param   - config_v6: number of v6 entries requested
 *
 * @purpose - LPM V6 entries config validate
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_mem_config_validate(int unit, int config_v6)
{
    int lpm_tcams;
    int lpm_tcam_size;
    int lpm_tcams_per_lookup;

    if (!soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        return SOC_E_NONE;
    }

    if (!soc_feature(unit, soc_feature_utt)) {
        return SOC_E_NONE;
    }

    lpm_tcams =  SOC_CONTROL(unit)->l3_defip_max_tcams;
    lpm_tcam_size = SOC_CONTROL(unit)->l3_defip_tcam_size;
    lpm_tcams_per_lookup = SOC_CONTROL(unit)->l3_defip_utt_tcams_per_lookup;

    if ((config_v6 * 2 ) > (lpm_tcams * lpm_tcam_size)) {
        return SOC_E_PARAM;
    }

    /* Ensure that given config can support dynamic URPF enable/disable */
    if ((config_v6 > ((lpm_tcams / 2) * 2 * (lpm_tcam_size / 2))) ||
        ((config_v6 / 2) >
         ((((lpm_tcams / lpm_tcams_per_lookup) * lpm_tcams_per_lookup) / 4) * 2
          * (lpm_tcam_size / 2)))) {
        return SOC_E_PARAM;
    }

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        int alpm_mode = _soc_alpm_mode[unit];

        /* Ensure that given config can support dynamic URPF enable/disable
         * in ALPM parallel mode
         */
        if((alpm_mode == 1 || alpm_mode == 3) &&
           (lpm_tcams_per_lookup * 4 > lpm_tcams)) {
            return SOC_E_PARAM;
        }
    }
#endif

    return SOC_E_NONE;
}

/************************************************************************
 ************************* LPM Helper Functions *************************
 */
/*
 * @name    - soc_utt_lpm_lookup_info_update
 * @param   - unit: unit number
 *
 * @purpose - Updats Lookup information based on Paired TCAM units.
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_lookup_info_update(int unit)
{
    int idx;
    int value;
    int max_idx;
    int offset;
    uint8 is_paired;
    int tcams_count;
    int paired_tcams;
    int num_lookups = 0;
    int half_tcams_per_lookup = 0;
    soc_utt_agent_t aid = SOC_UTT_AGENT_LPM;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(unit);
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    if (SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) == NULL) {
        return SOC_E_INTERNAL;
    }

    half_tcams_per_lookup = tcams_per_lookup / 2;
    num_lookups = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid);
    /* Reset existing Lookup info */
    sal_memset(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid), 0,
               (sizeof(soc_utt_agent_lookup_info_t) * num_lookups));
    num_lookups = num_lookups / 2;

    /* Non-URPF part */
    tcams_count = 0;
    max_idx = num_lookups;
    paired_tcams = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit);
    for (idx = 0; (idx < max_idx) && (tcams_count < max_tcams); idx++) {
        value = 0;
        is_paired = 0;
        if (paired_tcams) {
            if (paired_tcams > half_tcams_per_lookup) {
                value = half_tcams_per_lookup;
            } else {
                value = paired_tcams;
            }
            paired_tcams -= value;
            value *= 2;
            is_paired = 1;
        } else {
            if ((max_tcams - tcams_count) > tcams_per_lookup) {
                value = tcams_per_lookup;
            } else {
                value = max_tcams - tcams_count;
            }
        }
        tcams_count += value;
        SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) = value;
        SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx) = is_paired;
    }
    if (tcams_count > max_tcams) {
        return SOC_E_INTERNAL;
    }

    /* First lookup start idx is zero */
    SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, 0) = 0;
    /* Populate TCAM start index for each lookup */
    for (idx = 1; idx < max_idx; idx++) {
        SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx) =
            SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx - 1) +
            SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx - 1);
    }

    /* URPF part */
    tcams_count = 0;
    offset = num_lookups / 2;
    max_tcams = max_tcams / 2;
    max_idx = num_lookups + offset;
    paired_tcams = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
    for (idx = num_lookups; (idx < max_idx) && (tcams_count < max_tcams); idx++) {
        value = 0;
        is_paired = 0;
        if (paired_tcams) {
            if (paired_tcams > half_tcams_per_lookup) {
                value = half_tcams_per_lookup;
            } else {
                value = paired_tcams;
            }
            paired_tcams -= value;
            value *= 2;
            is_paired = 1;
        } else {
            if ((max_tcams - tcams_count) > tcams_per_lookup) {
                value = tcams_per_lookup;
            } else {
                value = max_tcams - tcams_count;
            }
        }
        tcams_count += value;
        SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx) = value;
        SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx) = is_paired;
        SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, (idx  + offset)) = value;
        SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, (idx + offset)) = is_paired;
    }

    if (tcams_count > max_tcams) {
        return SOC_E_INTERNAL;
    }

    /* First lookup start idx is zero */
    SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, num_lookups) = 0;
    /* Populate TCAM start index for each lookup */
    max_idx = num_lookups * 2;
    for (idx = num_lookups + 1; idx < max_idx; idx++) {
        SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx) =
            SOC_UTT_AGENT_LOOKUP_INFO_TCAM_START_IDX(unit, aid, idx - 1) +
            SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx - 1);
    }

    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_lpm_paired_tcams
 * @param   - unit: unit number
 * @param   - config_v6: number of v6 entries requested
 * @param   - p_tcam: Number TCAMs needed to be paired
 *                    to support config_v6 entries.
 *
 * @purpose - Computes number of paired tcams required
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_paired_tcams(int unit, int config_ipv6, int *p_tcam)
{
    int total_lookups;
    int paired_lookups;
    int paired_tcams;
    int unpaired_tcams;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(unit);
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int max_utt_tcams = SOC_L3_DEFIP_UTT_MAX_TCAMS_GET(unit);
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    *p_tcam = 0;

    paired_tcams = (config_ipv6 / tcam_size);
    if (config_ipv6 % tcam_size) {
        paired_tcams++;
    }

    paired_tcams *= 2;
    paired_lookups = ((paired_tcams + tcams_per_lookup - 1) / tcams_per_lookup);
    total_lookups = max_utt_tcams / tcams_per_lookup;

    unpaired_tcams = (total_lookups - paired_lookups) * tcams_per_lookup;

    if (soc_feature(unit, soc_feature_utt_ver1_5)) {
        paired_tcams = paired_lookups * tcams_per_lookup;
    }

    if ((max_tcams - paired_tcams) > unpaired_tcams) {
        paired_tcams += (max_tcams - paired_tcams - unpaired_tcams);
    }
    if (paired_tcams & 0x1) {
        paired_tcams++;
    }
    *p_tcam = (paired_tcams / 2);

    LOG_DEBUG(BSL_LS_SOC_LPM,
              (BSL_META_U(unit, "Paired TCAM %d v6 %d\n"),
               *p_tcam, config_ipv6));

    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_lpm_urpf_paired_tcams
 * @param   - unit: unit number
 * @param   - config_v6: number of v6 entries requested
 * @param   - p_tcam: Number TCAMs needed to be paired
 *
 * @purpose - Computes number of paired tcams required when urpf is enabled
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_urpf_paired_tcams(int unit, int *p_tcam)
{
    int total_lookups;
    int paired_lookups;
    int paired_tcams;
    int unpaired_tcams;
    int soc_defip_index_remap;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(unit);
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int max_utt_tcams = SOC_L3_DEFIP_UTT_MAX_TCAMS_GET(unit);
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        soc_defip_index_remap = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    }

    max_tcams = max_tcams / 2;
    soc_defip_index_remap = soc_defip_index_remap / 2;
    max_utt_tcams = max_utt_tcams / 2;

    paired_tcams = (soc_defip_index_remap / tcam_size);
    if (soc_defip_index_remap % tcam_size) {
        paired_tcams++;
    }

    paired_tcams *= 2;
    paired_lookups = ((paired_tcams + tcams_per_lookup - 1) / tcams_per_lookup);
    total_lookups = max_utt_tcams / tcams_per_lookup;

    unpaired_tcams = (total_lookups - paired_lookups) * tcams_per_lookup;

    if (soc_feature(unit, soc_feature_utt_ver1_5)) {
        paired_tcams = paired_lookups * tcams_per_lookup;
    }

    if ((max_tcams - paired_tcams) > unpaired_tcams) {
        paired_tcams += (max_tcams - paired_tcams - unpaired_tcams);
    }

    if (paired_tcams & 0x1) {
        paired_tcams++;
    }

    *p_tcam = (paired_tcams / 2);

    LOG_DEBUG(BSL_LS_SOC_LPM,
              (BSL_META_U(unit, "Paired TCAM %d v6 %d\n"),
               *p_tcam, soc_defip_index_remap));

    return SOC_E_NONE;
}

/************************************************************************
 **************** LPM INDEX MAP/REMAP Related Functions *****************
 */
/*
 * @name   - soc_utt_l3_defip_index_map
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Logical index
 *
 * @purpose - Map logical index to physical index
 */
int
soc_utt_l3_defip_index_map(int unit, int wide, int index)
{
    int tcam_idx = 0;
    int new_index = 0;
    int curr_lookup_num = 0;
    int num_paired_lookups = 0;
    int half_tcams_per_lookup = 0;
    int v6_entries_in_curr_lookup = 0;
    int num_paired_tcams_in_curr_lookup = 0;
    int entries_left_in_paired_partition = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit);
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    half_tcams_per_lookup = tcams_per_lookup / 2;

    assert(tcam_size);
    assert((tcams_per_lookup >= 2));
    assert(half_tcams_per_lookup);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    if (wide == 0) {
        /* PAIRED TCAM count can be more than actual required in some cases
         * Eg: Assume max LPM connfig: LPM has 16 TCAMs units.
         *     Each Lookup has max 4 TCAM units.
         *     User had requestd only 1 V6 entry.
         *     Then eventhough one TCAM BANK PAIR is enough to accomidate V6,
         *     SDK need to reserve 2 TCAM units pairs
         *     so that remaining TCAMs can fit in other lookups.
         */
        entries_left_in_paired_partition = ((num_paired_tcams * tcam_size) -
                                           soc_defip_index_remap);

        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
            (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
             SOC_UTT_LPM_FIXED_DATA_ADDRESS)) {
            /* Entry in paired partition */
            if (index < (2 * entries_left_in_paired_partition)) {
                tcam_idx = index / tcam_size;
                curr_lookup_num = tcam_idx / tcams_per_lookup;
                if (((curr_lookup_num + 1) * tcams_per_lookup) >
                    (num_paired_tcams * 2)) {
                    num_paired_tcams_in_curr_lookup =
                        num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                } else {
                    num_paired_tcams_in_curr_lookup = half_tcams_per_lookup;
                }

                /*
                 * Index in full paired lookup or
                 * first half of partial paired lookup
                 */
                if ((num_paired_tcams_in_curr_lookup == half_tcams_per_lookup) ||
                    ((tcam_idx % tcams_per_lookup) <
                     num_paired_tcams_in_curr_lookup)) {
                    new_index = index;
                } else {
                    /*
                     * Index in second half of partial paired lookup
                     */
                    new_index = (index +
                                 ((half_tcams_per_lookup -
                                   num_paired_tcams_in_curr_lookup) * tcam_size));
                }
            } else {
                /*
                 * Pairing partition is filled, compute number of lookups
                 * dedicated for pairing.
                 */
                num_paired_lookups =
                    ((num_paired_tcams + half_tcams_per_lookup - 1) /
                     half_tcams_per_lookup);

                new_index = (index +
                             (num_paired_lookups * tcams_per_lookup * tcam_size) -
                             (entries_left_in_paired_partition * 2));
            }
        } else if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
                   (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)) {
            new_index = index;
        } else {
            /* NON-LPM SCALING mode */
            /* Entry in paired partition */
            if (index < (2 * entries_left_in_paired_partition)) {
                /* Calculate last lookup number */
                if (num_paired_tcams) {
                    curr_lookup_num = (num_paired_tcams - 1) / half_tcams_per_lookup;
                }
                /* Num paired tcams in last lookup */
                num_paired_tcams_in_curr_lookup =
                    num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                /* Num v6 entries in last lookup */
                v6_entries_in_curr_lookup =
                    soc_defip_index_remap % (tcam_size * half_tcams_per_lookup);

                /*
                 * In first half of paired lookup
                 * If its 4 tcam per lookup case,
                 * Phy TCAMs 0 & 2, 1 & 3 will form the pairs.
                 * But v4 entries priority will be phy TCAM 0, 1, 2, 3
                 * with 0 being the highest priority.
                 */
                if (index < entries_left_in_paired_partition) {
                    new_index = ((curr_lookup_num * tcams_per_lookup * tcam_size) +
                                 v6_entries_in_curr_lookup + index);
                } else {
                    /* Entry in second half of paired lookup
                     * First 'n' entry would be occupied by v6 skip them
                     */
                    new_index = ((curr_lookup_num * tcams_per_lookup * tcam_size) +
                                 v6_entries_in_curr_lookup + index -
                                 entries_left_in_paired_partition);
                    /* Each lookup starts at a fixed address */
                    if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                        SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
                        new_index += (half_tcams_per_lookup * tcam_size);
                    } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                               SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
                        new_index += (num_paired_tcams_in_curr_lookup * tcam_size);
                    } else {
                        return -1;
                    }
                }
            } else {
                /*
                 * Unpaired partition.
                 * Unpaired partition start address computation.
                 * For fixed data addressing scheme, start address is fixed.
                 * For variable data addressing scheme, start address is
                 * actual paired tcams size.
                 */
                if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
                    /* Compute number of paired lookups */
                    num_paired_lookups =
                        (num_paired_tcams + half_tcams_per_lookup - 1) / half_tcams_per_lookup;
                    new_index = (num_paired_lookups * tcams_per_lookup *
                                 tcam_size);
                } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                           SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
                    new_index = num_paired_tcams * 2 * tcam_size;
                } else {
                    return -1;
                }
                new_index += index - (entries_left_in_paired_partition * 2);
            }
        }
        return new_index;
    }

    return index;
}

/*
 * @name   - soc_utt_l3_defip_index_remap
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Physical index
 *
 * @purpose - Remap physical index to logical index
 */

int
soc_utt_l3_defip_index_remap(int unit, int wide, int index)
{
    int tcam_idx = 0;
    int new_index = 0;
    int curr_lookup_num = 0;
    int num_paired_lookups = 0;
    int curr_lookup_half_idx = 0;
    int half_tcams_per_lookup = 0;
    int paired_partition_end_idx = 0;
    int v6_entries_in_curr_lookup = 0;
    int num_paired_tcams_in_curr_lookup = 0;
    int entries_left_in_paired_partition = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit);
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    half_tcams_per_lookup = tcams_per_lookup / 2;

    assert(tcam_size);
    assert((tcams_per_lookup >= 2));
    assert(half_tcams_per_lookup);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    if (wide == 0) {
        /* PAIRED TCAM count can be more than actual required in some cases
         * Eg: Assume max LPM connfig: LPM has 16 TCAMs units.
         *     Each Lookup has max 4 TCAM units.
         *     User had requestd only 1 V6 entry.
         *     Then eventhough one TCAM BANK PAIR is enough to accomidate V6,
         *     SDK need to reserve 2 TCAM units pairs
         *     so that remaining TCAMs can fit in other lookups.
         */
        entries_left_in_paired_partition = ((num_paired_tcams * tcam_size) -
                                           soc_defip_index_remap);

        /*
         * If Pairing is used, compute number of lookups
         * dedicated for pairing.
         */
        num_paired_lookups =
            (num_paired_tcams + half_tcams_per_lookup - 1) / half_tcams_per_lookup;

        /* Compute paired partition end index */
        if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
            paired_partition_end_idx =
                num_paired_lookups * tcams_per_lookup * tcam_size;
        } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                   SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
            paired_partition_end_idx = num_paired_tcams *  2 * tcam_size;
        } else {
            return -1;
        }

        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
            (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
             SOC_UTT_LPM_FIXED_DATA_ADDRESS)) {
            /* Entry in paired partition */
            if (index < paired_partition_end_idx) {
                tcam_idx = index / tcam_size;
                curr_lookup_num = tcam_idx / tcams_per_lookup;
                if (((curr_lookup_num + 1) * tcams_per_lookup) >
                    (num_paired_tcams * 2)) {
                    num_paired_tcams_in_curr_lookup =
                        num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                } else {
                    num_paired_tcams_in_curr_lookup = half_tcams_per_lookup;
                }

                /*
                 * Index in full paired lookup or
                 * first half of partial paired lookup
                 */
                if ((num_paired_tcams_in_curr_lookup == half_tcams_per_lookup) ||
                    ((tcam_idx % tcams_per_lookup) < half_tcams_per_lookup)) {
                    new_index = index;
                } else {
                    /*
                     * Index in second half of partial paired lookup
                     */
                    new_index = (index -
                                 ((half_tcams_per_lookup -
                                   num_paired_tcams_in_curr_lookup) * tcam_size));
                }
            } else {
                /* Unpaired partition */
                new_index = (index + (entries_left_in_paired_partition * 2) -
                             paired_partition_end_idx);
            }
        } else if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
                   (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)) {
            new_index = index;
        } else {
            /* NON-LPM SCALING mode */
            /* Entry in paired partition */
            if (index < paired_partition_end_idx) {
                /* Calculate last lookup number */
                if (num_paired_tcams) {
                    curr_lookup_num = (num_paired_tcams - 1) / half_tcams_per_lookup;
                }
                /* Num paired tcams in last lookup */
                num_paired_tcams_in_curr_lookup =
                    num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                /* Num v6 entries in last lookup */
                v6_entries_in_curr_lookup =
                    soc_defip_index_remap % (tcam_size * half_tcams_per_lookup);

                if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
                    curr_lookup_half_idx = ((curr_lookup_num * tcams_per_lookup) +
                                            half_tcams_per_lookup) * tcam_size;
                } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                           SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
                    curr_lookup_half_idx = ((curr_lookup_num * tcams_per_lookup) +
                                            num_paired_tcams_in_curr_lookup) * tcam_size;
                }

                if (index < curr_lookup_half_idx) {
                    /*
                     * In first half of paired lookup
                     */
                    new_index = (index -
                               ((curr_lookup_num * tcams_per_lookup) * tcam_size) -
                               v6_entries_in_curr_lookup);
                } else {
                    /*
                     * Entry in second half of paired lookup
                     */
                    new_index = (index -
                                 curr_lookup_half_idx -
                                 v6_entries_in_curr_lookup +
                                 entries_left_in_paired_partition);
                }
            } else {
                /* Unpaired partition */
                new_index = (index + (entries_left_in_paired_partition * 2) -
                             paired_partition_end_idx);
            }
        }
        return new_index;
    }
    return index;
}

/*
 * @name   - soc_utt_l3_defip_urpf_index_map
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Logical index
 *
 * @purpose - Map logical index to physical index with urpf enabled
 */
int
soc_utt_l3_defip_urpf_index_map(int unit, int wide, int index)
{
    int tcam_idx = 0;
    int new_index = 0;
    int curr_lookup_num = 0;
    int half_defip_size = 0;
    int half_defip_index = 0;
    int is_next_half_part = 0;
    int num_paired_lookups = 0;
    int half_tcams_per_lookup = 0;
    int v6_entries_in_curr_lookup = 0;
    int num_paired_tcams_in_curr_lookup = 0;
    int entries_left_in_paired_partition = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 2;
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    half_tcams_per_lookup = tcams_per_lookup / 2;

    assert(tcam_size);
    assert((tcams_per_lookup >= 2));
    assert(half_tcams_per_lookup);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    half_defip_size = soc_mem_index_count(unit, L3_DEFIPm) / 2;
    if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
        SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
        half_defip_index = (SOC_L3_DEFIP_UTT_MAX_TCAMS_GET(unit) * tcam_size) / 2;
    } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
               SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
        half_defip_index = (SOC_L3_DEFIP_MAX_TCAMS_GET(unit) * tcam_size) / 2;
    } else {
        return -1;
    }

    if (wide == 0) {
        /* Determine entry is in second half */
        if (index >= half_defip_size) {
            is_next_half_part = 1;
            index -= half_defip_size;
        }
        /* PAIRED TCAM can be more than actual required in some cases
         * Eg: Assune 16 TCAMs units are allocated for LPM.
         *     Lookup has 4 TCAM units and
         *     V6 entries requested is 1
         *     Then eventhough one TCAM BANK PAIR is enough,
         *     SDK need to 2 TCAM units pairs so that full lookup is occupied.
         */
        entries_left_in_paired_partition = ((num_paired_tcams * tcam_size) -
                                           soc_defip_index_remap);

        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
            (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
             SOC_UTT_LPM_FIXED_DATA_ADDRESS)) {
            /* Entry in paired partition */
            if (index < (2 * entries_left_in_paired_partition)) {
                tcam_idx = index / tcam_size;
                curr_lookup_num = tcam_idx / tcams_per_lookup;
                if (((curr_lookup_num + 1) * tcams_per_lookup) >
                    (num_paired_tcams * 2)) {
                    num_paired_tcams_in_curr_lookup =
                        num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                } else {
                    num_paired_tcams_in_curr_lookup = half_tcams_per_lookup;
                }

                /*
                 * Index in full paired lookup or
                 * first half of partial paired lookup
                 */
                if ((num_paired_tcams_in_curr_lookup == half_tcams_per_lookup) ||
                    ((tcam_idx % tcams_per_lookup) <
                     num_paired_tcams_in_curr_lookup)) {
                    new_index = index;
                } else {
                    /*
                     * Index in second half of partial paired lookup
                     */
                    new_index = (index +
                                 ((half_tcams_per_lookup -
                                   num_paired_tcams_in_curr_lookup) * tcam_size));
                }
            } else {
                /*
                 * Pairing partition is filled, compute number of lookups
                 * dedicated for pairing.
                 */
                num_paired_lookups =
                    ((num_paired_tcams + half_tcams_per_lookup - 1) /
                     half_tcams_per_lookup);

                new_index = (index +
                             (num_paired_lookups * tcams_per_lookup * tcam_size) -
                             (entries_left_in_paired_partition * 2));
            }
        } else if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
                   (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)) {
                new_index = index;
        } else {
            /* NON-LPM SCALING mode */
            /* Entry in paired partition */
            if (index < (2 * entries_left_in_paired_partition)) {
                /* Calculate last lookup number */
                if (num_paired_tcams) {
                    curr_lookup_num = (num_paired_tcams - 1) / half_tcams_per_lookup;
                }
                /* Num paired tcams in last lookup */
                num_paired_tcams_in_curr_lookup =
                    num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                /* Num v6 entries in last lookup */
                v6_entries_in_curr_lookup =
                    soc_defip_index_remap % (tcam_size * half_tcams_per_lookup);

                /*
                 * In first half of paired lookup
                 * If its 4 tcam per lookup case,
                 * Phy TCAMs 0 & 2, 1 & 3 will form the pairs.
                 * But v4 entries priority will be phy TCAM 0, 1, 2, 3
                 * with 0 being the highest priority.
                 */
                if (index < entries_left_in_paired_partition) {
                    new_index = ((curr_lookup_num * tcams_per_lookup * tcam_size) +
                                 v6_entries_in_curr_lookup + index);
                } else {
                    /* Entry in second half of paired lookup
                     * First 'n' entry would be occupied by v6 skip them
                     */
                    new_index = ((curr_lookup_num * tcams_per_lookup * tcam_size) +
                                 v6_entries_in_curr_lookup + index -
                                 entries_left_in_paired_partition);
                    /* Each lookup starts at a fixed address */
                    if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                        SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
                        new_index += (half_tcams_per_lookup * tcam_size);
                    } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                               SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
                        new_index += (num_paired_tcams_in_curr_lookup * tcam_size);
                    } else {
                        return -1;
                    }
                }
            } else {
                /*
                 * Unpaired partition.
                 * Unpaired partition start address computation.
                 * For fixed data addressing scheme, start address is fixed.
                 * For variable data addressing scheme, start address is
                 * actual paired tcams size.
                 */
                if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
                    /* Compute number of paired lookups */
                    num_paired_lookups =
                        (num_paired_tcams + half_tcams_per_lookup - 1) / half_tcams_per_lookup;
                    new_index = (num_paired_lookups * tcams_per_lookup *
                                 tcam_size);
                } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                           SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
                    new_index = num_paired_tcams * 2 * tcam_size;
                } else {
                    return -1;
                }
                new_index += index - (entries_left_in_paired_partition * 2);
            }
        }
        if (is_next_half_part) {
            new_index += half_defip_index;
        }
        return new_index;
    }

    new_index = index;
    if (index >= soc_defip_index_remap) {
        new_index = index + (half_defip_index / 2) - soc_defip_index_remap;
    }
    return new_index;
}

/*
 * @name   - soc_utt_l3_defip_urpf_index_remap
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Physical index
 *
 * @purpose - Remap physical index to logical index with urpf enabled
 */
int
soc_utt_l3_defip_urpf_index_remap(int unit, int wide, int index)
{
    int tcam_idx = 0;
    int new_index = 0;
    int curr_lookup_num = 0;
    int half_defip_size = 0;
    int half_defip_index = 0;
    int is_next_half_part = 0;
    int num_paired_lookups = 0;
    int curr_lookup_half_idx = 0;
    int half_tcams_per_lookup = 0;
    int paired_partition_end_idx = 0;
    int v6_entries_in_curr_lookup = 0;
    int num_paired_tcams_in_curr_lookup = 0;
    int entries_left_in_paired_partition = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 2;
    int tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit);

    half_tcams_per_lookup = tcams_per_lookup / 2;

    assert(tcam_size);
    assert((tcams_per_lookup >= 2));
    assert(half_tcams_per_lookup);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    half_defip_size = soc_mem_index_count(unit, L3_DEFIPm) / 2;
    if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
        SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
        half_defip_index = (SOC_L3_DEFIP_UTT_MAX_TCAMS_GET(unit) * tcam_size) / 2;
    } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
               SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
        half_defip_index = (SOC_L3_DEFIP_MAX_TCAMS_GET(unit) * tcam_size) / 2;
    } else {
        return -1;
    }


    if (wide == 0) {
        /* Determine entry is in second half */
        if (index >= half_defip_index) {
            is_next_half_part = 1;
            index -= half_defip_index;
        }
        /* PAIRED TCAM can be more than actual required in some cases
         * Eg: Assune 16 TCAMs units are allocated for LPM.
         *     Lookup has 4 TCAM units and
         *     V6 entries requested is 1
         *     Then eventhough one TCAM BANK PAIR is enough,
         *     SDK need to 2 TCAM units pairs so that full lookup is occupied.
         */
        entries_left_in_paired_partition = ((num_paired_tcams * tcam_size) -
                                           soc_defip_index_remap);

        /*
         * If Pairing is used, compute number of lookups
         * dedicated for pairing.
         */
        num_paired_lookups =
            (num_paired_tcams + half_tcams_per_lookup - 1) / half_tcams_per_lookup;

        /* Compute paired partition end index */
        if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
            paired_partition_end_idx =
                num_paired_lookups * tcams_per_lookup * tcam_size;
        } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                   SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
            paired_partition_end_idx = num_paired_tcams *  2 * tcam_size;
        } else {
            return -1;
        }

        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
            (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
             SOC_UTT_LPM_FIXED_DATA_ADDRESS)) {
            /* Entry in paired partition */
            if (index < paired_partition_end_idx) {
                tcam_idx = index / tcam_size;
                curr_lookup_num = tcam_idx / tcams_per_lookup;
                if (((curr_lookup_num + 1) * tcams_per_lookup) >
                    (num_paired_tcams * 2)) {
                    num_paired_tcams_in_curr_lookup =
                        num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                } else {
                    num_paired_tcams_in_curr_lookup = half_tcams_per_lookup;
                }

                /*
                 * Index in full paired lookup or
                 * first half of partial paired lookup
                 */
                if ((num_paired_tcams_in_curr_lookup == half_tcams_per_lookup) ||
                    ((tcam_idx % tcams_per_lookup) < half_tcams_per_lookup)) {
                    new_index = index;
                } else {
                    /*
                     * Index in second half of partial paired lookup
                     */
                    new_index = (index -
                                 ((half_tcams_per_lookup -
                                   num_paired_tcams_in_curr_lookup) * tcam_size));
                }
            } else {
                /* Unpaired partition */
                new_index = (index + (entries_left_in_paired_partition * 2) -
                             paired_partition_end_idx);
            }
        } else if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable) &&
                   (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)) {
            new_index = index;
        } else {
            /* NON-LPM SCALING mode */
            /* Entry in paired partition */
            if (index < paired_partition_end_idx) {
                /* Calculate last lookup number */
                if (num_paired_tcams) {
                    curr_lookup_num = (num_paired_tcams - 1) / half_tcams_per_lookup;
                }
                /* Num paired tcams in last lookup */
                num_paired_tcams_in_curr_lookup =
                    num_paired_tcams - (curr_lookup_num * half_tcams_per_lookup);
                /* Num v6 entries in last lookup */
                v6_entries_in_curr_lookup =
                    soc_defip_index_remap % (tcam_size * half_tcams_per_lookup);

                if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                    SOC_UTT_LPM_FIXED_DATA_ADDRESS) {
                    curr_lookup_half_idx = ((curr_lookup_num * tcams_per_lookup) +
                                            half_tcams_per_lookup) * tcam_size;
                } else if (SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
                           SOC_UTT_LPM_VARIABLE_DATA_ADDRESS) {
                    curr_lookup_half_idx = ((curr_lookup_num * tcams_per_lookup) +
                                            num_paired_tcams_in_curr_lookup) * tcam_size;
                }

                if (index < curr_lookup_half_idx) {
                    /*
                     * In first half of paired lookup
                     */
                    new_index = (index -
                               ((curr_lookup_num * tcams_per_lookup) * tcam_size) -
                               v6_entries_in_curr_lookup);
                } else {
                    /*
                     * Entry in second half of paired lookup
                     */
                    new_index = (index -
                                 curr_lookup_half_idx -
                                 v6_entries_in_curr_lookup +
                                 entries_left_in_paired_partition);
                }
            } else {
                new_index = (index + (entries_left_in_paired_partition * 2) -
                             paired_partition_end_idx);
            }
        }
        if (is_next_half_part) {
            new_index += half_defip_size;
        }
        return new_index;
    }
    new_index = index;
    if (index >= (half_defip_index / 2)) {
        new_index = index - (half_defip_index / 2) + soc_defip_index_remap;
    }
    return new_index;
}

/*
 * @name   - soc_utt_l3_defip_alpm_urpf_index_map
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Logical index
 *
 * @purpose - Map logical index to physical index with urpf enabled
 */
int
soc_utt_l3_defip_alpm_urpf_index_map(int unit, int wide, int index)
{
    int new_index = 0;
    soc_mem_t mem;
    int half_defip_size = 0, half_half_defip_size = 0;
    int is_next_half_part = 0, is_next_half_half_part = 0;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(unit);
    int entries_left_in_paired_partition = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2;
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 4;

    /* assert tcams per lookup are same */
    assert(tcam_size);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    if (wide == 0) {
        mem = L3_DEFIPm;
    } else {
        mem = L3_DEFIP_PAIR_128m;
    }

    half_defip_size = soc_mem_index_count(unit, mem) / 2;
    half_half_defip_size = half_defip_size / 2;

    /* Determine entry is in second half */
    if (index >= half_defip_size) {
        is_next_half_part = 1;
        index -= half_defip_size;
    }

    if (index >= half_half_defip_size) {
        is_next_half_half_part = 1;
        index -= half_half_defip_size;
    }

    if (wide == 0) {
        /* PAIRED TCAM can be more than actual required in some cases
         * Eg: Assune 16 TCAMs units are allocated for LPM.
         *     Lookup has 4 TCAM units and
         *     V6 entries requested is 1
         *     Then eventhough one TCAM BANK PAIR is enough,
         *     SDK need to 2 TCAM units pairs so that full lookup is occupied.
         */
        entries_left_in_paired_partition = ((num_paired_tcams * tcam_size) -
                                           soc_defip_index_remap);

        /*
         * In first half of paired lookup
         * If its 4 tcam per lookup case,
         * Phy TCAMs 0 & 2, 1 & 3 will form the pairs.
         * But v4 entries priority will be phy TCAM 0, 1, 2, 3
         * with 0 being the highest priority.
         */
        if (index < entries_left_in_paired_partition) {
            new_index = index + soc_defip_index_remap;
        } else {
            new_index = index + soc_defip_index_remap * 2;
        }

        if (is_next_half_half_part) {
            new_index += max_tcams * tcam_size / 4;
        }

        if (is_next_half_part) {
            new_index += max_tcams * tcam_size / 2;
        }
    } else {
        new_index = index;

        if (is_next_half_half_part) {
            new_index += (max_tcams / 2) * tcam_size / 4;
        }

        if (is_next_half_part) {
            new_index += (max_tcams / 2) * tcam_size / 2;
        }
    }

    return new_index;
}

/*
 * @name   - soc_utt_l3_defip_alpm_urpf_index_remap
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Physical index
 *
 * @purpose - Remap physical index to logical index with urpf enabled
 */
int
soc_utt_l3_defip_alpm_urpf_index_remap(int unit, int wide, int index)
{
    int new_index = 0;
    int half_tcam_size = 0, half_half_tcam_size = 0;
    int is_next_half_part = 0, is_next_half_half_part = 0;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(unit);
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2;
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 4;

    /* assert tcams per lookup are same */
    assert(tcam_size);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    half_tcam_size = max_tcams * tcam_size / 2;
    half_half_tcam_size = half_tcam_size / 2;

    /* Determine entry is in second half */
    if (wide == 0) {
        if (index >= half_tcam_size) {
            is_next_half_part = 1;
            index -= half_tcam_size;
        }

        if (index >= half_half_tcam_size) {
            is_next_half_half_part = 1;
            index -= half_half_tcam_size;
        }

        /*
         * In first half of paired lookup
         * If its 4 tcam per lookup case,
         * Phy TCAMs 0 & 2, 1 & 3 will form the pairs.
         * But v4 entries priority will be phy TCAM 0, 1, 2, 3
         * with 0 being the highest priority.
         */
        if (index < num_paired_tcams * tcam_size) {
            index -= soc_defip_index_remap;
        } else {
            index -= soc_defip_index_remap * 2;
        }
    } else {
        if (index >= half_tcam_size / 2) {
            is_next_half_part = 1;
            index -= half_tcam_size / 2;
        }

        if (index >= half_half_tcam_size / 2) {
            is_next_half_half_part = 1;
            index -= half_half_tcam_size / 2;
        }
    }

    if (wide == 0) {
        new_index = index;

        if (is_next_half_half_part) {
            new_index += soc_mem_index_count(unit, L3_DEFIPm) / 4;
        }

        if (is_next_half_part) {
            new_index += soc_mem_index_count(unit, L3_DEFIPm) / 2;
        }
    } else {
        new_index = index;

        if (is_next_half_half_part) {
            new_index += soc_defip_index_remap;
        }

        if (is_next_half_part) {
            new_index += soc_defip_index_remap * 2;
        }
    }

    return new_index;
}

/*
 * @name   - soc_utt_l3_defip_aacl_index_map
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Logical index
 *
 * @purpose - Map logical index to physical index with aacl enabled
 */
int
soc_utt_l3_defip_aacl_index_map(int unit, int wide, int urpf, int index)
{
    int new_index = 0;
    soc_mem_t mem;
    int half_defip_size = 0, half_half_defip_size = 0;
    int entries_left_in_paired_partition = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2;
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);
    int paired_tcam_size = num_paired_tcams * tcam_size;
    int num_full_paired_lookup;
    int lookup;

    /* assert tcams per lookup are same */
    assert(tcam_size);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    /*
     *                       -------------- 0
     * L3_DEFIP_PAIR_128     |  DB0 (v6)  |     BLOCK_0 (TCAM0/1)
     *                       |------------| Forwarding
     *      L3_DEFIP         |  DB0 (v4)  |     BLOCK_1 (TCAM2/3)
     *                       |------------| ACL Dst
     * L3_DEFIP_PAIR_128     |  DB1 (v6)  |     BLOCK_2 (TCAM4/5)
     *                       |------------| ACL Src
     * L3_DEFIP_PAIR_128     |  DB2 (v6)  |     BLOCK_3 (TCAM6/7)
     *                       -------------- Max
     */
    if (wide == 0) {
        mem = L3_DEFIPm;
    } else {
        mem = L3_DEFIP_PAIR_128m;
    }

    lookup = 0;

    half_defip_size = soc_mem_index_count(unit, mem) / 2;
    half_half_defip_size = half_defip_size / 2;

    if (index >= half_defip_size) { /* ACL DB */
        /* Now index is the offset in lookup 2 */
        index -= half_defip_size;
        lookup = 2;

        if (index >= half_half_defip_size) {
            /* Now index is the offset in lookup 3 */
            index -= half_half_defip_size;
            lookup = 3;
        }
    } else { /* Forwarding DB */
        /* Now index is the offset in lookup 0 */
        lookup = 0;

        if (urpf && index >= half_half_defip_size) {
            /* Now index is the offset in lookup 3 */
            index -= half_half_defip_size;
            lookup = 1;
        }
    }

    if (wide == 0) {
        if (lookup == 2 || lookup == 3) {
            /* 1/4 V6 entries are for ACL Dst or Src */
            soc_defip_index_remap /= 4;
        } else {
            if (!urpf) {
                /* 1/2 V6 entries are for forwarding */
                soc_defip_index_remap /= 2;
            } else {
                /* 1/4 V6 entries are for forwarding + urpf */
                soc_defip_index_remap /= 4;
            }
        }

        num_full_paired_lookup = soc_defip_index_remap / paired_tcam_size;

        entries_left_in_paired_partition =
            paired_tcam_size - (soc_defip_index_remap % paired_tcam_size);

        if (index < entries_left_in_paired_partition) {
            new_index = index + soc_defip_index_remap % paired_tcam_size;
        } else {
            new_index = index + (soc_defip_index_remap % paired_tcam_size) * 2;
        }

        new_index += num_full_paired_lookup * paired_tcam_size * 2;

        new_index += lookup * paired_tcam_size * 2;
    } else {
        new_index = index + lookup * paired_tcam_size;
    }

    return new_index;
}

/*
 * @name   - soc_utt_l3_defip_aacl_index_remap
 * @param  - unit: unit number
 * @param  - wide: 0 - L3_DEFIP, 1 - L3_DEFIP_PAIR_128
 * @param  - index: Physical index
 *
 * @purpose - Remap physical index to logical index with aacl enabled
 */
int
soc_utt_l3_defip_aacl_index_remap(int unit, int wide, int urpf, int index)
{
    int new_index = 0;
    int half_tcam_size = 0, half_half_tcam_size = 0;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(unit);
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_paired_tcams = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2;
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);
    int paired_tcam_size = num_paired_tcams * tcam_size;
    int num_full_paired_lookup;
    int lookup;

    /* assert tcams per lookup are same */
    assert(tcam_size);
    assert((SOC_UTT_AGENT_FLAGS(unit, SOC_UTT_AGENT_LPM) &
            (SOC_UTT_LPM_FIXED_DATA_ADDRESS | SOC_UTT_LPM_VARIABLE_DATA_ADDRESS)));

    if (wide == 0) {
        half_tcam_size = max_tcams * tcam_size / 2;
        half_half_tcam_size = half_tcam_size / 2;
    } else {
        half_tcam_size = max_tcams * tcam_size / 4;
        half_half_tcam_size = half_tcam_size / 2;
    }

    if (index >= half_tcam_size) { /* ACL DB */
        /* Now index is the offset in lookup 2 */
        index -= half_tcam_size;
        lookup = 2;

        if (index >= half_half_tcam_size) {
            /* Now index is the offset in lookup 3 */
            index -= half_half_tcam_size;
            lookup = 3;
        }
    } else { /* Forwarding DB */
        /* Now index is the offset in lookup 0 */
        lookup = 0;

        if (urpf && index >= half_half_tcam_size) {
            /* Now index is the offset in lookup 3 */
            index -= half_half_tcam_size;
            lookup = 1;
        }
    }

    if (wide == 0) {
        if (lookup == 2 || lookup == 3) {
            /* 1/4 V6 entries are for ACL Dst or Src */
            soc_defip_index_remap /= 4;
        } else {
            if (!urpf) {
                /* 1/2 V6 entries are for forwarding */
                soc_defip_index_remap /= 2;
            } else {
                /* 1/4 V6 entries are for forwarding + urpf */
                soc_defip_index_remap /= 4;
            }
        }

        num_full_paired_lookup = soc_defip_index_remap / paired_tcam_size;

        if (index < num_full_paired_lookup * paired_tcam_size * 2 + paired_tcam_size) {
            index -= soc_defip_index_remap % paired_tcam_size;
        } else {
            index -= (soc_defip_index_remap % paired_tcam_size) * 2;
        }

        index -= num_full_paired_lookup * paired_tcam_size * 2;

        new_index = index;

        if (lookup == 2 || lookup == 3) {
            /* 1/4 V6 entries are for ACL Dst or Src */
            new_index += lookup * (soc_mem_index_count(unit, L3_DEFIPm) / 4);
        } else {
            if (!urpf) {
                /* 1/2 V6 entries are for forwarding */
                new_index += lookup * (soc_mem_index_count(unit, L3_DEFIPm) / 2);
            } else {
                /* 1/4 V6 entries are for forwarding + urpf */
                new_index += lookup * (soc_mem_index_count(unit, L3_DEFIPm) / 4);
            }
        }
    } else {
        new_index = index;

        if (lookup == 2 || lookup == 3) {
            /* 1/4 V6 entries are for ACL Dst or Src */
            new_index += lookup * (soc_defip_index_remap / 4);
        } else {
            if (!urpf) {
                /* 1/2 V6 entries are for forwarding */
                new_index += lookup * (soc_defip_index_remap / 2);
            } else {
                /* 1/4 V6 entries are for forwarding + urpf */
                new_index += lookup * (soc_defip_index_remap / 4);
            }
        }
    }

    return new_index;
}

#if defined(INCLUDE_L3)
/************************************************************************
 ********************* LPM ALGO Related Functions ***********************
 */
/*
 * @name  - soc_utt_lpm_entry_in_paired_tcam
 * @param - unit - unit number
 * @param - index - logical index into L3_DEFIP
 *
 * @purpose - Check if the physical index corresponding to the logical index
 *            is in paired tcam
 * @returns - TRUE/FALSE
 */
int
soc_utt_lpm_entry_in_paired_tcam(int unit, int index)
{
    int num_tcam_pair = 0;
    int rem_entries_in_pair_lkup = 0;
    soc_mem_t mem = L3_DEFIPm;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int num_ipv6_128b_entries = SOC_L3_DEFIP_INDEX_REMAP_GET(unit);

    if (!SOC_MEM_IS_VALID(unit, L3_DEFIPm)) {
        mem = L3_DEFIP_LEVEL1m;
    }

    if (index >= soc_mem_index_count(unit, mem)) {
        LOG_DEBUG(BSL_LS_SOC_LPM,
                  (BSL_META_U(unit, "index %d Result %d\n"), index, FALSE));
        return FALSE;
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        num_ipv6_128b_entries >>= 1;
        num_tcam_pair = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
    } else {
        num_tcam_pair = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit);
    }

    rem_entries_in_pair_lkup =
        ((num_tcam_pair * tcam_size) - num_ipv6_128b_entries) * 2 ;

    if (index < rem_entries_in_pair_lkup) {
        LOG_DEBUG(BSL_LS_SOC_LPM,
                  (BSL_META_U(unit, "index %d Rem %d Result %d\n"),
                  index, rem_entries_in_pair_lkup, TRUE));
        return TRUE;
    }

    LOG_DEBUG(BSL_LS_SOC_LPM,
              (BSL_META_U(unit, "index %d Rem %d Result %d\n"),
              index, rem_entries_in_pair_lkup, FALSE));
    return FALSE;

}

/*
 * @name    - soc_utt_lpm_max_64bv6_route_get
 * @param   - unit: unit number
 * @param   - paired: number of TCAMs paired
 * @param   - entries: Max V6-64b count
 *
 * @purpose - Computes max v6-64b routes
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_max_64bv6_route_get(int u, int paired, uint16 *entries)
{
    int is_reserved = 0;
    int tcam_pair_count = 0;
    int defip_table_size = 0;
    int tcam_unpair_count = 0;
    int paired_table_size = 0;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(u);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int max_v6_entries = SOC_L3_DEFIP_MAX_128B_ENTRIES(u);

    if (SOC_URPF_STATUS_GET(u)) {
        tcam_pair_count = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(u);
    } else {
        tcam_pair_count = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(u);
    }
    if (!paired) {
        if (SOC_URPF_STATUS_GET(u)) {
            tcam_unpair_count = (max_tcams - (tcam_pair_count << 2));
            tcam_unpair_count  >>= 1;
        } else {
            tcam_unpair_count = (max_tcams - (tcam_pair_count << 1));
        }
        *entries = tcam_unpair_count * tcam_depth;
    } else if (paired && soc_feature(u, soc_feature_l3_lpm_scaling_enable)) {

        if (soc_feature(u, soc_feature_l3_lpm_128b_entries_reserved)) {
            is_reserved = 1;
        }

        SOC_IF_ERROR_RETURN(soc_fb_lpm_table_sizes_get(u, &paired_table_size,
                                                       &defip_table_size));
        LOG_DEBUG(BSL_LS_SOC_LPM,
                  (BSL_META_U(u,"Paired_Sz %d Un_Paired_Sz %d Rsv %d v6 %d\n"),
                   paired_table_size, defip_table_size,
                   is_reserved, max_v6_entries));

        if (SOC_URPF_STATUS_GET(u)) {
            max_v6_entries >>= 1;
        }
        /*
         * v6-64b entry will be converted to v6-128b entry.
         * so count will be same as v6-128b.
         */
        *entries = (paired_table_size >> 1);
        if (is_reserved) {
            *entries -= (max_v6_entries);
        }
    } else {
        *entries = 0;
    }
    LOG_DEBUG(BSL_LS_SOC_LPM,
              (BSL_META_U(u, "Max64b Paired %d urpf %d MaxTcams %d "
                          "TcamPairCount %d Entries %d\n"),
               paired, SOC_URPF_STATUS_GET(u), max_tcams, tcam_pair_count,
               *entries));
    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_lpm_unreserved_entry_count
 * @param   - unit: unit number
 * @param   - count: number of unreserved entry in Paired TCAMs
 *                   that can be used for v4.
 *
 * @purpose - Provides the entry count in the paired parition
 *            that can be used for v4. Used in non-lpm scaling algo.
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_unreserved_entry_count(int unit, int *count)
{
    int v6_128b_ent = 0;
    int paired_tcams = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    v6_128b_ent = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    if (SOC_URPF_STATUS_GET(unit)) {
        paired_tcams = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
        v6_128b_ent >>= 1;
    } else {
        paired_tcams = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit);
    }

    *count = ((paired_tcams * tcam_depth) - v6_128b_ent) * 2;

    return SOC_E_NONE;
}

/************************************************************************
 ****************** LPM SCALING ALGO Related Functions ******************
 */
/*
 * LPM functionality has 4 HW lookups to perform any of the below lookups
 *          V4/V6-128B DIP
 *          V4/V6-64B  DIP
 *          V4/V6-128B SIP
 *          V4/V6-64B  SIP
 * NOTE:
 * 1) Legcay devices always had 2 TCAM units per LPM lookup.
 *    Moving across parts of lookup could be done by adding TCAM depth.
 *
 *                   LOOKUP in Legacy devices
 *                  --------------------------
 *                  |                        |
 *                  |   -------    -------   |
 *                  |   |     |    |     |   |
 *                  |   |  0  |    |  1  |   |
 *                  |   |     |    |     |   |
 *                  |   -------    -------   |
 *                  |                        |
 *                  --------------------------
 * 2) With UTT feature, each LPM lookup can have more than 2 TCAM units.
 *    This number changes based on chip and number of v6 entries layout.
 *    To make movement across lookups and minimize the changes in the
 *    scaling algo, each lookup will be visualized as 2 logical TCAMs.
 *    Logical TCAM's size in a lookup is same
 *    but can be different across lookups.
 *    Layout for logical Lookup 0 with "n" TCAMs and
 *    logical lookup 1 with "m" TCAMs is shown below.
 *    Logical tcam numbers will still be in range [0-7].
 *
 *                          LOGICAL LOOKUP's in UTT devices
 *                -------------------------------------------------
 *                |                                               |
 *                |     LOGICAL TCAM 0        LOGICAL TCAM 1      |
 *                |  --------------------  ---------------------  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |      0      | |  |  |    (n/2)    |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |      1      | |  |  |   (n/2+1)   |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |         .        |  |         .         |  |
 *                |  |         .        |  |         .         |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |   (n/2-1)   | |  |  |     n-1     |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  --------------------  ---------------------  |
 *                |                                               |
 *                -------------------------------------------------
 *
 *                -------------------------------------------------
 *                |                                               |
 *                |     LOGICAL TCAM 2        LOGICAL TCAM 3      |
 *                |  --------------------  ---------------------  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |      n      | |  |  |   (n+m/2)   |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |     n+1     | |  |  |  (n+m/2+1)  |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |         .        |  |         .         |  |
 *                |  |         .        |  |         .         |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  |  (n+m/2-1)  | |  |  |    n+m-1    |  |  |
 *                |  |  |             | |  |  |             |  |  |
 *                |  |  --------------- |  |  ---------------  |  |
 *                |  |                  |  |                   |  |
 *                |  --------------------  ---------------------  |
 *                |                                               |
 *                -------------------------------------------------
 */
/*
 * @name    - soc_utt_lpm128_lookup_num_get
 * @param   - unit: unit number
 * @param   - index: Logical Memory Index
 *
 * @purpose - Computes lookup num for a given index.
 * @returns - lookup number.
 *            [0-3] for non-urpf & [4-7] for urpf
 */
static int
soc_utt_lpm128_lookup_num_get(int unit, int index)
{
    int idx;
    int max_idx;
    int total_tcams = 0;
    int lookup_num_tcams = 0;
    int tcam_depth = SOC_UTT_TCAM_DEPTH(unit);
    soc_utt_agent_t aid = SOC_UTT_AGENT_LPM;

    /* Valid only for LPM SCALING ALGO */
    assert(soc_feature(unit, soc_feature_l3_lpm_scaling_enable));
    assert(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) != NULL);

    idx = max_idx = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid);
    if (SOC_URPF_STATUS_GET(unit)) {
        idx = idx / 2;
    } else {
        idx = 0;
        max_idx = max_idx / 2;
    }

    for (; idx < max_idx; idx++) {
        lookup_num_tcams = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);
        if (((total_tcams + lookup_num_tcams) * tcam_depth) > index) {
            return idx;
        }
        total_tcams += lookup_num_tcams;
    }

    assert(idx < max_idx);
    return -1;
}


/*
 * @name    - soc_utt_lpm128_lookup_depth
 * @param   - unit: unit number
 * @param   - lkup_num: Logical lookup number
 *
 * @purpose - Computes lookup size.
 * @returns - Logical lookup size.
 */
static int
soc_utt_lpm128_lookup_depth(int unit, int lkup_num)
{
    int lookup_num_tcams = 0;
    int depth = SOC_UTT_TCAM_DEPTH(unit);
    soc_utt_agent_t aid = SOC_UTT_AGENT_LPM;

    /* Valid only for LPM SCALING ALGO */
    assert(soc_feature(unit, soc_feature_l3_lpm_scaling_enable));
    assert(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) != NULL);
    assert(lkup_num < SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid));

    lookup_num_tcams = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, lkup_num);

    assert(lookup_num_tcams);
    if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, lkup_num)) {
        assert((lookup_num_tcams & 0x1) == 0);
    }

    return (lookup_num_tcams * depth);
}

/*
 * @name    - soc_utt_lpm128_logical_tcam_start_index
 * @param   - unit: unit number
 * @param   - lkup_num: Logical lookup number
 *
 * @purpose - Computes Logical TCAM for and given index and
 *            returns start index of logical TCAM
 * @returns - Logical TCAM start index
 */
int
soc_utt_lpm128_logical_tcam_start_index(int unit, int index)
{
    int idx;
    int max_idx;
    int start_addr = 0;
    int total_tcams = 0;
    int lookup_num_tcams = 0;
    int depth = SOC_UTT_TCAM_DEPTH(unit);
    soc_utt_agent_t aid = SOC_UTT_AGENT_LPM;

    /* Valid only for LPM SCALING ALGO */
    assert(soc_feature(unit, soc_feature_l3_lpm_scaling_enable));
    assert(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) != NULL);

    /*
     * To handle when only V6 partition exists and free index is in the middle.
     * New pfx needs to be created at end, then next_index will be set to
     * MAX + 1 which will be an invalid index.
     */
    max_idx = SOC_UTT_AGENT_NUM_TCAMS(unit, aid) * depth;
    if (SOC_URPF_STATUS_GET(unit) && (index >= (max_idx / 2))) {
        return max_idx / 2;
    } else if (index >= max_idx) {
        return max_idx;
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        idx = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid) / 2;
    } else {
        idx = 0;
    }
    max_idx = soc_utt_lpm128_lookup_num_get(unit, index);

    for (; idx < max_idx; idx++) {
        lookup_num_tcams = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);
        total_tcams += lookup_num_tcams;
    }
    lookup_num_tcams = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);

    assert(lookup_num_tcams);
    if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx)) {
        assert((lookup_num_tcams & 0x1) == 0);
    }

    index -= (total_tcams * depth);

    start_addr = total_tcams * depth;

    /* Index in second half of the lookup */
    if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx) &&
        (index >= ((lookup_num_tcams * depth) / 2))) {
        start_addr += ((lookup_num_tcams * depth) / 2);
    }

    return start_addr;
}

/*
 * @name    - soc_utt_lpm128_logical_tcam_num
 * @param   - unit: unit number
 * @param   - index: Logical memory index
 *
 * @purpose - Computes Logical TCAM number for a given index.
 * @returns - Logical TCAM Number
 */
int
soc_utt_lpm128_logical_tcam_num(int unit, int index)
{
    int idx;
    int max_idx;
    int tcam_num = 0;
    int total_tcams = 0;
    int lookup_num_tcams = 0;
    int depth = SOC_UTT_TCAM_DEPTH(unit);
    soc_utt_agent_t aid = SOC_UTT_AGENT_LPM;

    /* Valid only for LPM SCALING ALGO */
    assert(soc_feature(unit, soc_feature_l3_lpm_scaling_enable));
    assert(SOC_UTT_AGENT_LOOKUP_INFO(unit, aid) != NULL);

    /*
     * To handle when only V6 partition exists and free index is in the middle.
     * New pfx needs to be created at end, then next_index will be set to
     * MAX + 1 which will be an invalid index.
     * So logical TCAM index will num_lookups * 2 or num_lookups (urpf)
     */
    max_idx = SOC_UTT_AGENT_NUM_TCAMS(unit, aid) * depth;
    if (SOC_URPF_STATUS_GET(unit) && (index >= (max_idx / 2))) {
        return SOC_UTT_AGENT_DEF_NUM_LOOKUPS(unit, aid);
    } else if (index >= max_idx) {
        return (SOC_UTT_AGENT_DEF_NUM_LOOKUPS(unit, aid) * 2);
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        idx = SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid) / 2;
    } else {
        idx = 0;
    }
    max_idx = soc_utt_lpm128_lookup_num_get(unit, index);

    for (; idx < max_idx; idx++) {
        lookup_num_tcams = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);
        total_tcams += lookup_num_tcams;
    }
    lookup_num_tcams = SOC_UTT_AGENT_LOOKUP_INFO_NUM_TCAMS(unit, aid, idx);

    assert(lookup_num_tcams);
    if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx)) {
        assert((lookup_num_tcams & 0x1) == 0);
    }

    index -= (total_tcams * depth);

    if (SOC_URPF_STATUS_GET(unit)) {
        tcam_num = (max_idx - (SOC_UTT_AGENT_NUM_LOOKUPS(unit, aid) / 2)) * 2;
    } else {
        tcam_num = max_idx * 2;
    }

    /* Index in second half of the lookup */
    if (SOC_UTT_AGENT_LOOKUP_INFO_IS_PAIRED(unit, aid, idx) &&
        (index >= ((lookup_num_tcams * depth) / 2))) {
        tcam_num++;
    }

    return tcam_num;
}

/*
 * @name    - soc_utt_lpm128_is_logical_tcam_first_entry
 * @param   - unit: unit number
 * @param   - index: Logical memory index
 *
 * @purpose - Computes if given index is the first entry
 *            in Logical TCAM number for a given index.
 * @returns - TRUE/FALSE
 */
int
soc_utt_lpm128_is_logical_tcam_first_entry(int unit, int index)
{
    int start_addr;

    start_addr = soc_utt_lpm128_logical_tcam_start_index(unit, index);

    if (start_addr == index) {
        return TRUE;
    }

    return FALSE;
}

/*
 * @name    - soc_utt_lpm128_paired_offset
 * @param   - unit: unit number
 * @param   - index: Logical memory index
 *
 * @purpose - Computes logical lookup offset required for the paired part
 *            for a given index.
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm128_paired_offset(int unit, int index)
{
    int lkup_num;

    lkup_num = soc_utt_lpm128_lookup_num_get(unit, index);
    return (soc_utt_lpm128_lookup_depth(unit, lkup_num) / 2);
}

/*
 * @name    - soc_utt_lpm128_move_v4_down_for_v6_get_fent
 * @param   - unit: unit number
 * @param   - v6_fent_start: Logical start index for the v6 pfx
 * @param   - v6_fent: Free entries in v6 pfx
 * @param   - fent_for_move: Free entry count in the last logical lookup.
 *
 * @purpose - Computes Entry count for move
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm128_move_v4_down_for_v6_get_fent(int unit, int v6_fent_start,
                                            int v6_fent, int *fent_for_move)
{
    int offset = 0;
    int l_tcam_start_index = 0;
    int rem_v6_fent = 0;
    int next_index = 0;

    *fent_for_move = 0;

    offset = soc_utt_lpm128_paired_offset(unit, v6_fent_start);

    /* Start index of logical TCAM */
    l_tcam_start_index  = soc_utt_lpm128_logical_tcam_start_index(unit,
                                                                  v6_fent_start);
    /* Calculate number of free entries outside first lookup */
    rem_v6_fent = ((v6_fent >> 1) -
                   (offset - (v6_fent_start - l_tcam_start_index)));
    /* Move to start of next lookup */
    next_index = l_tcam_start_index + offset;

    /* EVEN TCAM */
    if ((soc_utt_lpm128_lookup_num_get(unit, l_tcam_start_index) & 1) == 0) {
        next_index += offset;
    }

    offset = 0;
    while (rem_v6_fent > offset) {
        rem_v6_fent -= offset;
        /* Move to start of next lookup */
        next_index = next_index + (offset << 1);

        offset = soc_utt_lpm128_paired_offset(unit, next_index);
    }

    *fent_for_move = rem_v6_fent;

    return SOC_E_NONE;
}

/*
 * @name    - soc_utt_lpm128_multiple_lookup_v4_domain_odd_start
 * @param   - unit: unit number
 * @param   - start: Logical start index for the v6 pfx free space
 * @param   - fent: Free entries in v6 pfx
 * @param   - rem: Remaining free entry count in the logical lookup where free index start exists.
 * @param   - v4_domain_odd_start: Corresponding V4 domain odd start
 *
 * @purpose - Computes V4 domain odd start index
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm128_multiple_lookup_v4_domain_odd_start(int unit, int start,
                                                   int fent, int rem ,
                                                   int *v4_domain_odd_start)
{
    int offset = 0;
    int next_index = 0;

    offset = soc_utt_lpm128_paired_offset(unit, start);
    next_index = start + offset + rem;
    fent -= (rem << 1);

    offset = 0;
    while (fent > (2 * offset)) {
        fent -= (offset << 1);
        /* Move to start of next lookup */
        next_index = next_index + (offset << 1);
        offset = soc_utt_lpm128_paired_offset(unit, next_index);
    }
    next_index += (fent >> 1);
    offset = soc_utt_lpm128_paired_offset(unit, next_index);
    *v4_domain_odd_start = next_index + offset;

    assert(soc_utt_lpm128_logical_tcam_num(unit, *v4_domain_odd_start) & 1);

    return SOC_E_NONE;
}


/*
 * @name    - soc_utt_lpm_scaled_unreserved_start_index
 * @param   - unit: unit number
 * @param   - start: Logical start index
 *
 * @purpose - Computes start address for the unreserved space
 * @returns - SOC_E_XXXX
 */
int
soc_utt_lpm_scaled_unreserved_start_index(int unit, int *start)
{
    int v6_128b_ent = 0;
    int paired_tcams = 0;
    int last_lookup_num = 0;
    int half_tcams_per_lookup = 0;
    int v6_ent_in_last_lookup = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    v6_128b_ent = SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    if (SOC_URPF_STATUS_GET(unit)) {
        paired_tcams = SOC_L3_DEFIP_UTT_URPF_PAIRED_TCAMS_GET(unit);
        v6_128b_ent >>= 1;
    } else {
        paired_tcams = SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit);
    }

    half_tcams_per_lookup = SOC_L3_DEFIP_UTT_TCAMS_PER_LOOKUP_GET(unit) / 2;

    assert(half_tcams_per_lookup);

    last_lookup_num = 0;

    /* Calculate last lookup number */
    if (paired_tcams) {
        last_lookup_num = (paired_tcams - 1) / half_tcams_per_lookup;
    }

    /* Num v6 entries in last lookup */
    v6_ent_in_last_lookup =
        v6_128b_ent % (tcam_depth * half_tcams_per_lookup);

    *start = v6_ent_in_last_lookup +
        (last_lookup_num * half_tcams_per_lookup * tcam_depth * 2);

    return SOC_E_NONE;
}
#endif /* INCLUDE_L3 */


#endif /* BCM_UTT_SUPPORT */

