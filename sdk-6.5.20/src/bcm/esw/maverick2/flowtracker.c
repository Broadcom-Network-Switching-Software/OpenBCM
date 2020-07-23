/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Flowtracker - Flow Tracking embedded APP interface
 * Purpose: API to configure flowtracker embedded app interface.
 */
#if defined(INCLUDE_FLOWTRACKER)

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <soc/tomahawk.h>
#include <shared/alloc.h>
#include <shared/idxres_fl.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/flowtracker.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/maverick2.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/switch.h>

/*
 * Function:
 *  _bcm_mv2_flowtracker_config_init
 * Purpose:
 *  Validate config variables.
 *  Program FLOW_TRACKER_CONFIG register as per config variables.
 *  Program HASH_CONTROL register to enable FT in hw.
 *  Program FT_GROUP_TABLE_CONFIG index 0 with thresold value 0.
 * Parameters:
 *  unit - (IN) BCM device number
 * Returns:
 *  BCM_E_XXX - BCM error code.
 * Notes:
 *  This api should be called only if flow tracker is enabled
 *  using config variable "flowtracker_enable". Check is done
 *  before calling api.
 */
int
_bcm_mv2_flowtracker_config_init(int unit, _bcm_int_ft_info_t *ft_info)
{
    uint32 hash_control = 0;
    ft_group_table_config_entry_t ft_grp_entry;
    uint32 ft_config = 0;
    uint32 ft_max_flows = 0;
    uint32 ft_max_counters_per_flow = 0;
    uint8  ft_table_mode = 0;
    uint8  ft_cntr_alloc_mode = 0;

    /* Validate input variable */
    if (ft_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Initialise variables */
    sal_memset(&ft_grp_entry, 0, sizeof(ft_grp_entry));

    /* Step 1: Validate config variables "flowtracker_max_flows"
     * and "flowtracker_max_counters_per_flow"
     */
    ft_max_flows = soc_property_get(unit,spn_FLOWTRACKER_MAX_FLOWS,
                                          BCM_INT_FT_DEF_MAX_FLOWS);
    ft_max_counters_per_flow = soc_property_get(unit,spn_FLOWTRACKER_MAX_COUNTERS_PER_FLOW,
                                                      BCM_INT_FT_DEF_MAX_COUNTERS_PER_FLOW);
    if (!(ft_max_counters_per_flow == 1 ||
          ft_max_counters_per_flow == 2 ||
          ft_max_counters_per_flow == 4)) {
        return BCM_E_CONFIG;
    }

    if (!(ft_max_flows == 16384 ||
          ft_max_flows == 32768)) {
        return BCM_E_CONFIG;
    }

    if (ft_max_flows == 32768 && ft_max_counters_per_flow == 1) {
        ft_info->ft_config_mode = MV2_FT_CONFIG_MODE_MIXED_MODE1_ID;
    } else if (ft_max_flows == 32768 && ft_max_counters_per_flow == 2) {
        ft_info->ft_config_mode = MV2_FT_CONFIG_MODE_MIXED_MODE2_ID;
    } else if (ft_max_flows == 16384 && ft_max_counters_per_flow == 1) {
        ft_info->ft_config_mode = MV2_FT_CONFIG_MODE_QW_MODE1_ID;
    } else if (ft_max_flows == 16384 && ft_max_counters_per_flow == 2) {
        ft_info->ft_config_mode = MV2_FT_CONFIG_MODE_QW_MODE2_ID;
    } else if (ft_max_flows == 16384 && ft_max_counters_per_flow == 4) {
        ft_info->ft_config_mode = MV2_FT_CONFIG_MODE_QW_MODE3_ID;
    } else {
        return (BCM_E_CONFIG);
    }

    /* Step 2: Configure FLOW_TRACKER_CONFIG register as per mode
     * decided in step 1 */
    if (ft_info->ft_config_mode == MV2_FT_CONFIG_MODE_MIXED_MODE1_ID) {
        ft_table_mode = 0;
        ft_cntr_alloc_mode = 0;
    } else if (ft_info->ft_config_mode == MV2_FT_CONFIG_MODE_MIXED_MODE2_ID) {
        ft_table_mode = 0;
        ft_cntr_alloc_mode = 1;
    } else if (ft_info->ft_config_mode == MV2_FT_CONFIG_MODE_QW_MODE1_ID) {
        ft_table_mode = 1;
        ft_cntr_alloc_mode = 0;
    } else if (ft_info->ft_config_mode == MV2_FT_CONFIG_MODE_QW_MODE2_ID) {
        ft_table_mode = 1;
        ft_cntr_alloc_mode = 1;
    } else if (ft_info->ft_config_mode == MV2_FT_CONFIG_MODE_QW_MODE3_ID) {
        ft_table_mode = 1;
        ft_cntr_alloc_mode = 2;
    } else {
        return (BCM_E_CONFIG);
    }

    soc_reg_field_set(unit, FLOW_TRACKER_CONFIGr,
                        &ft_config, FLOW_TABLE_MODEf, ft_table_mode);
    soc_reg_field_set(unit, FLOW_TRACKER_CONFIGr,
                        &ft_config, FLEX_CNTR_ALLOC_MODf, ft_cntr_alloc_mode);
    BCM_IF_ERROR_RETURN(WRITE_FLOW_TRACKER_CONFIGr(unit, ft_config));

    /* Step 3: Enable flow tracking using exact match table */
    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));

    soc_reg_field_set(unit, HASH_CONTROLr,
            &hash_control, FLOW_TRACKER_ENABLEf, 1);

    BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));

    /* Step 4: Reserve GROUP ID 0 and program it with threshold value 0 */
    BCM_IF_ERROR_RETURN(WRITE_FT_GROUP_TABLE_CONFIGm(unit, MEM_BLOCK_ALL,
                                                       0, &ft_grp_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_config_detach
 * Purpose:
 *  Disable flow tracker module in hardware.
 * Parameters:
 *  unit - (IN) BCM device number
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_config_detach(int unit)
{
    uint32 hash_control = 0;
    uint32 ft_config = 0;

    BCM_IF_ERROR_RETURN(WRITE_FLOW_TRACKER_CONFIGr(unit, ft_config));

    /* Disable flow tracking using exact match table */
    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));

    soc_reg_field_set(unit, HASH_CONTROLr,
            &hash_control, FLOW_TRACKER_ENABLEf, 0);

    BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_config_mode_pools_get
 * Purpose:
 *  Get number of stats reserved for provided flow tracker config mode.
 * Parameters:
 *  unit           - (IN) BCM device number
 *  ft_config_mode - (IN) Flow Tracker Config Mode
 *  no_of_stats    - (OUT) Number of flex counters reserved
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_config_mode_stats_get(int unit, 
                                           uint8 ft_config_mode, 
                                           uint32 *no_of_stats)
{
    /* Validate input variable */
    if (no_of_stats == NULL) {
        return (BCM_E_PARAM);
    }

    if (ft_config_mode == MV2_FT_CONFIG_MODE_MIXED_MODE1_ID) {
        *no_of_stats = MV2_FT_CONFIG_MODE_MIXED_MODE1_FLEX_COUNTERS;
    } else if (ft_config_mode == MV2_FT_CONFIG_MODE_MIXED_MODE2_ID) {
        *no_of_stats = MV2_FT_CONFIG_MODE_MIXED_MODE2_FLEX_COUNTERS;
    } else if (ft_config_mode == MV2_FT_CONFIG_MODE_QW_MODE1_ID) {
        *no_of_stats = MV2_FT_CONFIG_MODE_QW_MODE1_FLEX_COUNTERS;
    } else if (ft_config_mode == MV2_FT_CONFIG_MODE_QW_MODE2_ID) {
        *no_of_stats = MV2_FT_CONFIG_MODE_QW_MODE2_FLEX_COUNTERS;
    } else if (ft_config_mode == MV2_FT_CONFIG_MODE_QW_MODE3_ID) {
        *no_of_stats = MV2_FT_CONFIG_MODE_QW_MODE3_FLEX_COUNTERS;
    } else {
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_mv2_ft_group_flex_policy_data_set
 * Purpose:
 *  Set data ub entry buffer at specified offset and given width.
 * Parameters:
 *  p_fn_data      - (IN/OUT) Update with p_data value
 *  p_data         - (IN) data
 *  offset         - (IN) Offset in p_fn_data
 *  width          - (IN) Width of data
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
STATIC int
_bcm_mv2_ft_group_flex_policy_data_set(uint32 *p_fn_data,
                                       uint32 *p_data,
                                       uint32 offset,
                                       uint32 width)
{
    uint32 u32_mask;
    int idx, wp, bp, len;

    if (NULL == p_fn_data || NULL == p_data) {
        LOG_VERBOSE(BSL_LS_BCM_FLOWTRACKER, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    wp = offset / 32;
    bp = offset & (32 - 1);
    idx = 0;

    for (len = width; len > 0; len -= 32) {
        if (bp) {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                if ((p_data[idx] & ~u32_mask) != 0) {
                    return (BCM_E_PARAM);
                }
            } else {
                u32_mask = 0xffffffff;
            }

            p_fn_data[wp] &= ~(u32_mask << bp);
            p_fn_data[wp++] |= p_data[idx] << bp;
            p_fn_data[wp] &= ~(u32_mask >> (32 - bp));
            p_fn_data[wp] |= p_data[idx] >> (32 - bp) & ((1 << bp) - 1);

        } else {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                if ((p_data[idx] & ~u32_mask) != 0) {
                    return (BCM_E_PARAM);
                }
                p_fn_data[wp] &= ~u32_mask;
                p_fn_data[wp++] |= p_data[idx];
            } else {
                p_fn_data[wp++] = p_data[idx];
            }
        }

        idx++;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_default_config_set
 * Purpose:
 *  Sets Default Flowtracker group properties when created.
 * Parameters:
 *  unit       - (IN) BCM device number
 *  id         - (IN) Flowtracker Group Id
 * Returns:
 *  BCM_E_XXX - BCM error code.
 * Notes:
 *  When Flow Tracker Group is created, certain properties like
 *  flex stat mode and enable_disable control need to be programmed
 *  by default. This api will program certain flow tracker group
 *  level properties.
 */

int
_bcm_mv2_flowtracker_flow_group_default_config_set(int unit,
                                                   bcm_flowtracker_group_t id)
{
    uint32 fmt_len = 0;
    uint32 meter_data_buf = 0;
    soc_field_info_t *field_info = NULL;
    ft_group_table_config_entry_t ft_grp_config_entry;

    /* Initialise Flow Group Entry */
    sal_memset(&ft_grp_config_entry, 0, sizeof(ft_grp_config_entry));

    /* Read entry at given ft group index */
    BCM_IF_ERROR_RETURN(READ_FT_GROUP_TABLE_CONFIGm(unit,
                MEM_BLOCK_ANY, id, &ft_grp_config_entry));

    /* Enable Flex Stat on Flow Group by default */
    soc_FT_GROUP_TABLE_CONFIGm_field32_set(unit,
                                           &ft_grp_config_entry,
                                           FIXED_ASSOCIATED_DATAf,
                                           1 << MV2_FT_FLEX_STAT_ENABLE_BIT);

    /* Set default flex stat mode */

    /* METER_PAIR_MODE_MODIFIER must be set for EM to see the incoming color.
     * This is needed for EM to apply ColorAware actions in ETRAP mode. However
     * setting this bit can be done unconditionally as it does not do anything
     * if no actions are applied.
     */
    field_info = soc_mem_fieldinfo_get(unit,
                    FT_GROUP_TABLE_CONFIGm, FLEX_POLICY_DATAf);
    if (field_info != NULL) {
        /* Set Meter_pair_mode_modifier */
        soc_format_field32_set(unit, IFP_METER_SETfmt,
                &meter_data_buf,
                METER_PAIR_MODE_MODIFIERf, 1);

        /* Get length of format */
        fmt_len = soc_format_field_length(unit, IFP_METER_SETfmt, METERf);

        /* Set Flex policy Data in FT_GROUP_TABLE_CONFIGm */
        BCM_IF_ERROR_RETURN(_bcm_mv2_ft_group_flex_policy_data_set(
                    (uint32 *)&ft_grp_config_entry,
                    &meter_data_buf, field_info->bp, fmt_len));
    }

    /* Set default flow tracker group property */
    BCM_IF_ERROR_RETURN(WRITE_FT_GROUP_TABLE_CONFIGm(unit, MEM_BLOCK_ALL,
                                                     id, &ft_grp_config_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_config_clear
 * Purpose:
 *  Clears Flowtracker group properties.
 * Parameters:
 *  unit       - (IN) BCM device number
 *  id         - (IN) Flowtracker Group Id
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_flow_group_config_clear(int unit,
                                             bcm_flowtracker_group_t id)
{
    soc_mem_t mem = FT_GROUP_TABLE_CONFIGm;
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                        id, soc_mem_entry_null(unit, mem)));
    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_flow_limit_set
 * Purpose:
 *  Set Flow Threshold limit associated to a Flowtracker group.
 *  Determines number of maximum flow allowed to be learnt on a flow.
 * Parameters:
 *  unit       - (IN) BCM device number
 *  id         - (IN) Flowtracker Group Id
 *  flow_limit - (IN) Threshold of flow allowed to learn in Flowtracker group.
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_flow_group_flow_limit_set(int unit,
                                               bcm_flowtracker_group_t id,
                                               uint32 flow_limit)
{
    ft_group_table_config_entry_t ft_grp_config_entry;

    /* Initialise Flow Group Entry */
    sal_memset(&ft_grp_config_entry, 0, sizeof(ft_grp_config_entry));

    /* Read entry at given ft group index */
    BCM_IF_ERROR_RETURN(READ_FT_GROUP_TABLE_CONFIGm(unit,
                                    MEM_BLOCK_ANY, id, &ft_grp_config_entry));

    /* Set Flow Thresold Limit in entry buffer */
    soc_FT_GROUP_TABLE_CONFIGm_field32_set(unit,
                                           &ft_grp_config_entry,
                                           FLOW_THRESHOLDf, flow_limit);

    /* Set FLow Threshold Limit in a flow tracker group */
    BCM_IF_ERROR_RETURN(WRITE_FT_GROUP_TABLE_CONFIGm(unit, MEM_BLOCK_ALL,
                                                    id, &ft_grp_config_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_flow_limit_get
 * Purpose:
 *  Get Flow Threshold limit associated to a Flowtracker group.
 *  Determines number of maximum flow allowed to be learnt on a flow.
 * Parameters:
 *  unit       - (IN) BCM device number
 *  id         - (IN) Flowtracker Group Id
 *  flow_limit - (OUT) Threshold of flow allowed to learn in Flowtracker group.
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_flow_group_flow_limit_get(int unit,
                                               bcm_flowtracker_group_t id,
                                               uint32 *flow_limit)
{
    ft_group_table_config_entry_t ft_grp_config_entry;

    /* Initialise Flow Group Entry */
    sal_memset(&ft_grp_config_entry, 0, sizeof(ft_grp_config_entry));

    /* Read entry at given ft group index */
    BCM_IF_ERROR_RETURN(READ_FT_GROUP_TABLE_CONFIGm(unit,
                                      MEM_BLOCK_ANY, id, &ft_grp_config_entry));

    *flow_limit = soc_FT_GROUP_TABLE_CONFIGm_field32_get(unit,
                                        &ft_grp_config_entry, FLOW_THRESHOLDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_stat_modeid_set
 * Purpose:
 *  Set Stat Modeid to a Flowtracker group.
 *  Determines offset for counters to increment based on packet attributes.
 * Parameters:
 *  unit        - (IN) BCM device number
 *  id          - (IN) Flowtracker Group Id
 *  stat_modeid - (IN) Stat Modeid to be associated to a group
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_flow_group_stat_modeid_set(int unit,
                                                bcm_flowtracker_group_t id,
                                                uint32 stat_modeid)
{
    ft_group_table_config_entry_t ft_grp_config_entry;
    uint32 flex_counter_enable = 0;
    uint32 flex_stat_modeid = 0;
    uint32 fixed_assoc_data = 0;

    /* Initialise Flow Group Entry */
    sal_memset(&ft_grp_config_entry, 0, sizeof(ft_grp_config_entry));

    /* Read entry at given ft group index */
    BCM_IF_ERROR_RETURN(READ_FT_GROUP_TABLE_CONFIGm(unit,
                MEM_BLOCK_ANY, id, &ft_grp_config_entry));

    flex_counter_enable = 1 << MV2_FT_FLEX_STAT_ENABLE_BIT;
    flex_stat_modeid = stat_modeid << MV2_FT_FLEX_STAT_MODEID_BIT;
    fixed_assoc_data = flex_counter_enable | flex_stat_modeid;

    /* Set Stat Modeid in entry buffer */
    soc_FT_GROUP_TABLE_CONFIGm_field32_set(unit,
                                           &ft_grp_config_entry,
                                           FIXED_ASSOCIATED_DATAf,
                                           fixed_assoc_data);

    /* Set Stat Modeid in a flow tracker group */
    BCM_IF_ERROR_RETURN(WRITE_FT_GROUP_TABLE_CONFIGm(unit, MEM_BLOCK_ALL,
                                                     id, &ft_grp_config_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_stat_modeid_get
 * Purpose:
 *  Get Stat Modeid associated to a Flowtracker group.
 *  Determines offset for counters to increment based on packet attributes.
 * Parameters:
 *  unit        - (IN) BCM device number
 *  id          - (IN) Flowtracker Group Id
 *  stat_modeid - (OUT) Stat Modeid associated to a group
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_flow_group_stat_modeid_get(int unit,
                                                bcm_flowtracker_group_t id,
                                                uint32 *stat_modeid)
{
    ft_group_table_config_entry_t ft_grp_config_entry;
    uint32 fixed_assoc_data = 0;

    /* Initialise Flow Group Entry */
    sal_memset(&ft_grp_config_entry, 0, sizeof(ft_grp_config_entry));

    /* Read entry at given ft group index */
    BCM_IF_ERROR_RETURN(READ_FT_GROUP_TABLE_CONFIGm(unit,
                                                    MEM_BLOCK_ANY,
                                                    id, &ft_grp_config_entry));

    fixed_assoc_data = soc_FT_GROUP_TABLE_CONFIGm_field32_get(unit,
                        &ft_grp_config_entry, FIXED_ASSOCIATED_DATAf);

    *stat_modeid = fixed_assoc_data >> MV2_FT_FLEX_STAT_MODEID_BIT & 0x3;

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_mv2_flowtracker_flow_group_flow_count_get
 * Purpose:
 *  Retreive Flow Count associated to a Flowtracker group.
 *  Determines number of flow learnt in a group.
 * Parameters:
 *  unit       - (IN) BCM device number
 *  id         - (IN) Flowtracker Group Id
 *  flow_count - (OUT) No. of flows learnt in Flowtracker group.
 * Returns:
 *  BCM_E_XXX - BCM error code.
 */
int
_bcm_mv2_flowtracker_flow_group_flow_count_get(int unit,
                                               bcm_flowtracker_group_t id,
                                               uint32 *flow_count)
{
    ft_group_table_cnt_entry_t ft_grp_cnt_entry;

    /* Initialise Flow Group Entry */
    sal_memset(&ft_grp_cnt_entry, 0, sizeof(ft_grp_cnt_entry));

    /* Read entry at given ft group index */
    BCM_IF_ERROR_RETURN(READ_FT_GROUP_TABLE_CNTm(unit,
                                                 MEM_BLOCK_ANY, id, &ft_grp_cnt_entry));

    /* Retreive Flow Count for a flow tracker group */
    *flow_count = soc_FT_GROUP_TABLE_CNTm_field32_get(unit,
                                                      &ft_grp_cnt_entry, FLOW_CNTf);

    return BCM_E_NONE;
}

#else /* INCLUDE_FLOWTRACKER */
typedef int _bcm_mv2_ft_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_FLOWTRACKER */
