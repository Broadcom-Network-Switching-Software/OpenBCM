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
#include <bcm_int/esw/switch_match.h>
#include <bcm_int/esw/hurricane3.h>
#include <bcm_int/esw/xgs3.h>
#if defined(BCM_HURRICANE3_SUPPORT)
#if defined(BCM_SWITCH_MATCH_SUPPORT)

/* Parameter NULL error handling */
#define HR3_MATCH_CHECK_NULL_PARAMETER(_u_, _parameter_)                      \
    do {                                                                      \
        if (NULL == (_parameter_)) {                                          \
            LOG_ERROR(BSL_LS_BCM_SWITCH,                                      \
                      (BSL_META_U((_u_),                                      \
                                  "Error: NULL parameter\n")));               \
            return BCM_E_PARAM;                                               \
        }                                                                     \
    } while (0)

/* Function Enter/Leave Tracing */
#define HR3_MATCH_FUNCTION_TRACE(_u_, _str_)                                  \
    do {                                                                      \
        LOG_VERBOSE(BSL_LS_BCM_SWITCH,                                        \
                    (BSL_META_U((_u_),                                        \
                                "%s: " _str_ "\n"), FUNCTION_NAME()));        \
    } while (0)

/* Switch Match Miml table size */
#define HR3_SWITCH_MATCH_TABLE_SIZE_MIML       (1)

/* Check if the value exceed the field max acceptable value */
STATIC int
bcmi_hr3_swtich_match_valid_field_value(
    int unit,
    soc_mem_t mem,
    soc_field_t field,
    uint32 value)
{
    uint32 max_value;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Check that memory is a valid one for this unit. */
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return (BCM_E_INTERNAL);
    }

    /* Check that field is a valid one for the memory. */
    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_INTERNAL);
    }

    max_value = (uint32)(1 << soc_mem_field_length(unit, mem, field)) - 1;

    if (!(value <= max_value)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "value(%d) doesn't fit to %s of %s.\n"),
                              value,
                              SOC_FIELD_NAME(unit, field),
                              SOC_MEM_NAME(unit, mem)));
        return BCM_E_PARAM;
    }

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_miml_table_size_get
 * Purpose:
 *   Get the table size for MIML
 * Parameters:
 *   unit - (IN) Unit number
 *   table_size - (OUT) table size
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_table_size_get(
    int unit,
    uint32 *table_size)
{
    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, table_size);

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
        *table_size = HR3_SWITCH_MATCH_TABLE_SIZE_MIML;
    }
#endif /* BCM_HURRICANE3_SUPPORT */
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hr3_miml_match_hw_idx_get
 * Purpose:
 *      Get HW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_idx - (OUT) HW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_hw_idx_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_idx)
{
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, hw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    /* For Miml case, the hardware index and sw_idx are always 0 */
    *hw_idx = sw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "hw_idx %u\n"), *hw_idx));
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hr3_miml_match_match_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_idx - (IN) HW memory index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_sw_idx_get(
    int unit,
    uint32 hw_idx,
    uint32 *sw_idx)
{
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_table_size_get(unit, &table_size));

    if (hw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    /* For Miml case, the hardware index and sw_idx are always 0 */
    *sw_idx = hw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "sw_idx %u\n"), *sw_idx));
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_table_size_get
 * Purpose:
 *   Get the table size for Custom Header
 * Parameters:
 *   unit - (IN) Unit number
 *   table_size - (OUT) table size
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_table_size_get(
    int unit,
    uint32 *table_size)
{
    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, table_size);

    *table_size = soc_mem_index_count(unit, CUSTOM_HEADER_MATCHm);
    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "table_size %u\n"), *table_size));
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hr3_custom_hdr_match_hw_idx_get
 * Purpose:
 *      Get HW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_idx - (OUT) HW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_hw_idx_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_idx)
{
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, hw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_custom_hdr_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    /* For Customer Header, sw_idx = hw_idx */
    *hw_idx = sw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "hw_idx %u\n"), *hw_idx));
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_hr3_custom_hdr_match_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_idx - (IN) HW memory index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_sw_idx_get(
    int unit,
    uint32 hw_idx,
    uint32 *sw_idx)
{
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_custom_hdr_match_table_size_get(unit, &table_size));

    if (hw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    /* For Customer Header, sw_idx = hw_idx */
    *sw_idx = hw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "sw_idx %u\n"), *sw_idx));
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_config_write
 * Purpose:
 *   Write the match configuration to hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   config_info - (IN) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_config_write(
    int unit,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 rval = 0;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    if (config_info->outer_ethtype == 0 ||
        config_info->inner_ethtype == 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_MIML_TPIDr(unit, &rval));
    soc_reg_field_set(unit, MIML_TPIDr, &rval, TPIDf,
                      config_info->outer_ethtype);
    BCM_IF_ERROR_RETURN(WRITE_MIML_TPIDr(unit, rval));

    BCM_IF_ERROR_RETURN(READ_MIML_ETYPEr(unit, &rval));
    soc_reg_field_set(unit, MIML_ETYPEr, &rval, ETYPEf,
                      config_info->inner_ethtype);
    BCM_IF_ERROR_RETURN(WRITE_MIML_ETYPEr(unit, rval));

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_config_add
 * Purpose:
 *   Add a match config, return match_id after matched
 *   config created.
 * Parameters:
 *   unit - (IN) Unit number
 *   config_info - (IN) Match configuration
 *   sw_idx - (OUT) SW memory index
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_config_add(
    int unit,
    bcm_switch_match_config_info_t *config_info,
    uint32 *sw_idx)
{
    int rv = BCM_E_NONE;
    uint32 rval = 0;
    uint16 outer_ethtype, inner_ethtype;
    /* For Miml case, the hardware index (hw_idx) is always 0 */
    uint32 hw_idx = 0;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    if (config_info->outer_ethtype == 0 ||
        config_info->inner_ethtype == 0) {
        return BCM_E_PARAM;
    }
    /* Check if the config is the same or HW has been used */
    BCM_IF_ERROR_RETURN(READ_MIML_TPIDr(unit, &rval));
    outer_ethtype = (uint16)soc_reg_field_get(unit, MIML_TPIDr, rval, TPIDf);

    BCM_IF_ERROR_RETURN(READ_MIML_ETYPEr(unit, &rval));
    inner_ethtype = (uint16)soc_reg_field_get(unit, MIML_ETYPEr, rval, ETYPEf);

    if (outer_ethtype == config_info->outer_ethtype &&
        inner_ethtype == config_info->inner_ethtype) {
        /* the config is the same */
        return BCM_E_EXISTS;
    } else if (outer_ethtype != 0 && inner_ethtype != 0) {
        /* the only HW entry has been used */
        return BCM_E_RESOURCE;
    }

    /* No sw_idx paramter since there is only one entry in HR3 miml */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_config_write(unit,
                                          config_info));
    *sw_idx = hw_idx;
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_config_add
 * Purpose:
 *   Add a match config, return match_id after matched
 *   config created.
 * Parameters:
 *   unit - (IN) Unit number
 *   config_info - (IN) Match configuration
 *   sw_idx - (OUT) SW memory index
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_config_add(
    int unit,
    bcm_switch_match_config_info_t *config_info,
    uint32 *sw_idx)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx;
    int ref_count = 0;
    custom_header_match_entry_t custom_hdr_entry;
    custom_header_policy_table_entry_t policy_entry;
    void *entries[1];

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    sal_memset(&custom_hdr_entry, 0, sizeof(custom_header_match_entry_t));
    sal_memset(&policy_entry, 0, sizeof(custom_header_policy_table_entry_t));

    /* Add one entry in CUSTOM_HEADER_MATCHm */
    soc_mem_field32_set(unit, CUSTOM_HEADER_MATCHm, &custom_hdr_entry,
                        CUSTOM_HEADERf, config_info->value32);
    soc_mem_field32_set(unit, CUSTOM_HEADER_MATCHm, &custom_hdr_entry,
                        VALIDf, 1);
    entries[0] = &custom_hdr_entry;

    BCM_IF_ERROR_RETURN
        (_bcm_custom_header_match_entry_add(unit, entries, 1, &hw_idx));

    if (hw_idx >= soc_mem_index_count(unit, CUSTOM_HEADER_MATCHm)) {
        return BCM_E_INTERNAL;
    }

    /* Check if the entry is existed */
    BCM_IF_ERROR_RETURN
        (_bcm_custom_header_match_entry_ref_count_get(unit, hw_idx,
                                                      &ref_count));

    /* Decrease the reference count if existed, and return BCM_E_EXISTS */
    if (ref_count > 1) {
        _bcm_common_profile_mem_ref_cnt_update(unit, CUSTOM_HEADER_MATCHm,
                                               hw_idx, -1);
        *sw_idx = hw_idx;
        return BCM_E_EXISTS;
    }
    /* Check if the value fit the target field range */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_swtich_match_valid_field_value(
            unit, CUSTOM_HEADER_POLICY_TABLEm, INT_PRIf,
            config_info->int_pri));

    BCM_IF_ERROR_RETURN
        (bcmi_hr3_swtich_match_valid_field_value(
            unit, CUSTOM_HEADER_POLICY_TABLEm, CNGf,
            config_info->color));

    /* Write PRI/CNG to CUSTOM_HEADER_POLICY_TABLEm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ANY,
                      hw_idx, &policy_entry));
    soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm, &policy_entry,
                        INT_PRIf, config_info->int_pri);
    soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm, &policy_entry,
                        CNGf, _BCM_COLOR_ENCODING(unit, config_info->color));
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ALL,
                       hw_idx, &policy_entry));

    *sw_idx = hw_idx;
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_config_delete
 * Purpose:
 *   Destroy a match ID by removing it from hardware.
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_hr3_miml_match_config_delete(
    int unit,
    uint32 sw_idx)
{
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    BCM_IF_ERROR_RETURN(WRITE_MIML_TPIDr(unit, 0));
    BCM_IF_ERROR_RETURN(WRITE_MIML_ETYPEr(unit, 0));

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_config_delete
 * Purpose:
 *   Destroy a match ID by removing it from hardware.
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_hr3_custom_hdr_match_config_delete(
    int unit,
    uint32 sw_idx)
{
    int rv = BCM_E_UNAVAIL;
    custom_header_policy_table_entry_t policy_entry;
    uint32 hw_idx = sw_idx;
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_custom_hdr_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    /* Delete the entry from CUSTOM_HEADER_MATCHm */
    rv = _bcm_custom_header_match_entry_delete(unit, hw_idx);
    /* For cold boot case, delete return BCM_E_NOT_FOUND should be okay */
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the entry from CUSTOM_HEADER_POLICY_TABLEm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ANY,
                      hw_idx, &policy_entry));
    soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                        &policy_entry, INT_PRIf, 0);
    soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                        &policy_entry, CNGf, 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ALL,
                       hw_idx, &policy_entry));

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_config_get
 * Purpose:
 *   Get the match configuration from hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 *   config_info - (OUT) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_config_get(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 rval = 0;
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    BCM_IF_ERROR_RETURN(READ_MIML_TPIDr(unit, &rval));
    config_info->outer_ethtype =
        soc_reg_field_get(unit, MIML_TPIDr, rval, TPIDf);

    BCM_IF_ERROR_RETURN(READ_MIML_ETYPEr(unit, &rval));
    config_info->inner_ethtype =
        soc_reg_field_get(unit, MIML_ETYPEr, rval, ETYPEf);

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_config_get
 * Purpose:
 *   Get the match configuration from hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 *   config_info - (OUT) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_config_get(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 hw_idx = sw_idx;
    custom_header_match_entry_t custom_hdr_entry;
    custom_header_policy_table_entry_t policy_entry;
    void *entries[1];
    uint32 hw_color;
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    BCM_IF_ERROR_RETURN
        (bcmi_hr3_custom_hdr_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    sal_memset(&custom_hdr_entry, 0, sizeof(custom_header_match_entry_t));
    sal_memset(&policy_entry, 0, sizeof(custom_header_policy_table_entry_t));

    /* Get the entry from CUSTOM_HEADER_MATCHm */
    entries[0] = &custom_hdr_entry;
    BCM_IF_ERROR_RETURN
        (_bcm_custom_header_match_entry_get(unit, hw_idx, 1, entries));

    config_info->value32 = soc_mem_field32_get(unit,
                                               CUSTOM_HEADER_MATCHm,
                                               &custom_hdr_entry,
                                               CUSTOM_HEADERf);

    /* Get PRI/CNG from CUSTOM_HEADER_POLICY_TABLEm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ANY,
                      hw_idx, &policy_entry));
    config_info->int_pri = soc_mem_field32_get(unit,
                                               CUSTOM_HEADER_POLICY_TABLEm,
                                               &policy_entry, INT_PRIf);
    hw_color = soc_mem_field32_get(unit,
                                   CUSTOM_HEADER_POLICY_TABLEm,
                                   &policy_entry, CNGf);
    config_info->color = _BCM_COLOR_DECODING(unit, hw_color);

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_config_set
 * Purpose:
 *   Set the match configuration from hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 *   config_info - (IN) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_config_set(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    if (config_info->outer_ethtype == 0 ||
        config_info->inner_ethtype == 0) {
        return BCM_E_PARAM;
    }

    /* No sw_idx paramter since there is only one entry in HR3 miml */
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_config_write(unit,
                                          config_info));

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_config_set
 * Purpose:
 *   Set the match configuration from hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 *   config_info - (IN) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_config_set(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 hw_idx = sw_idx;
    custom_header_match_entry_t custom_hdr_entry;
    custom_header_policy_table_entry_t policy_entry;
    void *entries[1];
    uint32 table_size;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    BCM_IF_ERROR_RETURN
        (bcmi_hr3_custom_hdr_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    sal_memset(&custom_hdr_entry, 0, sizeof(custom_header_match_entry_t));
    sal_memset(&policy_entry, 0, sizeof(custom_header_policy_table_entry_t));

    /* Set the entry to CUSTOM_HEADER_MATCHm */
    soc_mem_field32_set(unit, CUSTOM_HEADER_MATCHm, &custom_hdr_entry,
                        CUSTOM_HEADERf, config_info->value32);
    soc_mem_field32_set(unit, CUSTOM_HEADER_MATCHm, &custom_hdr_entry,
                        VALIDf, 1);
    entries[0] = &custom_hdr_entry;
    BCM_IF_ERROR_RETURN
        (_bcm_custom_header_match_entry_update(unit, entries, hw_idx));

    /* Set PRI/CNG to CUSTOM_HEADER_POLICY_TABLEm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ANY,
                      hw_idx, &policy_entry));
    soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                        &policy_entry, INT_PRIf, config_info->int_pri);
    soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                        &policy_entry, CNGf,
                        _BCM_COLOR_ENCODING(unit, config_info->color));
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, CUSTOM_HEADER_POLICY_TABLEm, MEM_BLOCK_ALL,
                       hw_idx, &policy_entry));

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}


/*
 * Function:
 *   bcmi_hr3_miml_match_control_get
 * Purpose:
 *   Get the match related control information with given control type.
 * Parameters:
 *   unit - (IN) Unit number
 *   control_type - (IN) Match control type
 *   gport - (IN) gport number for port basis control.
 *                Otherwise, BCM_GPORT_INVALID.
 *   control_info - (OUT) Match control information
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_control_get(
    int unit,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info)
{
    int rv = BCM_E_UNAVAIL;
    int val;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, control_info);

    if (control_type == bcmSwitchMatchControlPortEnable) {
        rv = _bcm_esw_port_tab_get(unit, gport, MIML_ENABLEf, &val);

        if (BCM_SUCCESS(rv)) {
            control_info->port_enable = val ? 1 : 0;
        }
    }

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_control_get
 * Purpose:
 *   Get the match related control information with given control type.
 * Parameters:
 *   unit - (IN) Unit number
 *   control_type - (IN) Match control type
 *   gport - (IN) gport number for port basis control.
 *                Otherwise, BCM_GPORT_INVALID.
 *   control_info - (OUT) Match control information
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_custom_hdr_match_control_get(
    int unit,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info)
{
    int rv = BCM_E_UNAVAIL;
    int fval = 0;
    uint32 rval = 0;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, control_info);

    if (control_type == bcmSwitchMatchControlPortEnable) {
        rv = _bcm_esw_port_tab_get(unit, gport, CUSTOM_HEADER_ENABLEf, &fval);

        if (BCM_SUCCESS(rv)) {
            control_info->port_enable = fval ? 1 : 0;
        }
    } else if (control_type == bcmSwitchMatchControlMatchMask) {
        rv = READ_CUSTOM_HEADER_MASKr(unit, &rval);
        if (BCM_SUCCESS(rv)) {
            control_info->mask32 = soc_reg_field_get(unit, CUSTOM_HEADER_MASKr,
                                                     rval, MASKf);
        }
    }

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_control_set
 * Purpose:
 *   Set the match related control information with given control type.
 * Parameters:
 *   unit - (IN) Unit number
 *   control_type - (IN) Match control type
 *   gport - (IN) gport number for port basis control.
 *                Otherwise, BCM_GPORT_INVALID.
 *   control_info - (IN) Match control information
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_hr3_miml_match_control_set(
    int unit,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info)
{
    int rv = BCM_E_UNAVAIL;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, control_info);

    if (control_type == bcmSwitchMatchControlPortEnable) {
        rv = _bcm_esw_port_tab_set(unit, gport,
                                   _BCM_CPU_TABS_ETHER, MIML_ENABLEf,
                                   (control_info->port_enable) ? 1 : 0);
    }

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_control_set
 * Purpose:
 *   Set the match related control information with given control type.
 * Parameters:
 *   unit - (IN) Unit number
 *   control_type - (IN) Match control type
 *   gport - (IN) gport number for port basis control.
 *                Otherwise, BCM_GPORT_INVALID.
 *   control_info - (IN) Match control information
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_hr3_custom_hdr_match_control_set(
    int unit,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval = 0;

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    HR3_MATCH_CHECK_NULL_PARAMETER(unit, control_info);

    if (control_type == bcmSwitchMatchControlPortEnable) {
        rv = _bcm_esw_port_tab_set(unit, gport,
                                   _BCM_CPU_TABS_ETHER, CUSTOM_HEADER_ENABLEf,
                                   (control_info->port_enable) ? 1 : 0);
    } else if (control_type == bcmSwitchMatchControlMatchMask) {
        BCM_IF_ERROR_RETURN(READ_CUSTOM_HEADER_MASKr(unit, &rval));
        soc_reg_field_set(unit, CUSTOM_HEADER_MASKr, &rval,
                          MASKf, control_info->mask32);
        rv = WRITE_CUSTOM_HEADER_MASKr(unit, rval);
    }

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_hr3_miml_match_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   sw_idx - (IN) SW bookeeping memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_hr3_miml_match_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    uint32 rval = 0;
    int outer_ethtype = 0, inner_ethtype = 0;
    uint32 table_size;
#endif /* BCM_WARM_BOOT_SUPPORT */

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
#if defined(BCM_WARM_BOOT_SUPPORT)
    BCM_IF_ERROR_RETURN
        (bcmi_hr3_miml_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    BCM_IF_ERROR_RETURN(READ_MIML_TPIDr(unit, &rval));
    outer_ethtype = soc_reg_field_get(unit, MIML_TPIDr, rval, TPIDf);

    BCM_IF_ERROR_RETURN(READ_MIML_ETYPEr(unit, &rval));
    inner_ethtype = soc_reg_field_get(unit, MIML_ETYPEr, rval, ETYPEf);

    if (outer_ethtype && inner_ethtype) {
        /* BCM_E_NONE means HW existed */
        return BCM_E_NONE;
    }

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    /* Return BCM_E_NOT_FOUND means HW doesn't exist */
    return BCM_E_NOT_FOUND;
#else /* BCM_WARM_BOOT_SUPPORT */

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

/*
 * Function:
 *   bcmi_hr3_custom_hdr_match_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_hr3_custom_hdr_match_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    int is_valid = 0;
    custom_header_match_entry_t custom_hdr_entry;
    uint32 hw_idx = sw_idx;
#endif /* BCM_WARM_BOOT_SUPPORT */

    HR3_MATCH_FUNCTION_TRACE(unit, "IN");
#if defined(BCM_WARM_BOOT_SUPPORT)
    sal_memset(&custom_hdr_entry, 0, sizeof(custom_header_match_entry_t));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, CUSTOM_HEADER_MATCHm, MEM_BLOCK_ANY,
                      hw_idx, &custom_hdr_entry));

    is_valid = soc_mem_field32_get(unit, CUSTOM_HEADER_MATCHm,
                                   &custom_hdr_entry, VALIDf);
    if (is_valid) {
        _bcm_common_profile_mem_ref_cnt_update(unit, CUSTOM_HEADER_MATCHm,
                                               hw_idx, 1);
        /* BCM_E_NONE means HW existed */
        return BCM_E_NONE;
    }
    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    /* Return BCM_E_NOT_FOUND means HW doesn't exist */
    return BCM_E_NOT_FOUND;
#else /* BCM_WARM_BOOT_SUPPORT */

    HR3_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

const bcmi_switch_match_service_info_t bcmi_hr3_miml_match_info = {
    "hr3_miml_match",                         /* table_name */
    bcmi_hr3_miml_match_table_size_get,       /* switch_match_table_size_get */
    bcmi_hr3_miml_match_hw_idx_get,           /* switch_match_hw_id_get */
    bcmi_hr3_miml_match_sw_idx_get,           /* switch_match_sw_idx_get */
    bcmi_hr3_miml_match_wb_hw_existed,        /* switch_match_wb_hw_existed */
    bcmi_hr3_miml_match_config_add,           /* switch_match_config_add */
    bcmi_hr3_miml_match_config_delete,        /* switch_match_config_delete */
    bcmi_hr3_miml_match_config_get,           /* switch_match_config_get */
    bcmi_hr3_miml_match_config_set,           /* switch_match_config_set */
    bcmi_hr3_miml_match_control_get,          /* switch_match_control_get */
    bcmi_hr3_miml_match_control_set           /* switch_match_control_set */
};

const bcmi_switch_match_service_info_t bcmi_hr3_custom_hdr_match_info = {
    "hr3_custom_hdr_match",                   /* table_name */
    bcmi_hr3_custom_hdr_match_table_size_get, /* switch_match_table_size_get */
    bcmi_hr3_custom_hdr_match_hw_idx_get,     /* switch_match_hw_id_get */
    bcmi_hr3_custom_hdr_match_sw_idx_get,     /* switch_match_sw_idx_get */
    bcmi_hr3_custom_hdr_match_wb_hw_existed,  /* switch_match_wb_hw_existed */
    bcmi_hr3_custom_hdr_match_config_add,     /* switch_match_config_add */
    bcmi_hr3_custom_hdr_match_config_delete,  /* switch_match_config_delete */
    bcmi_hr3_custom_hdr_match_config_get,     /* switch_match_config_get */
    bcmi_hr3_custom_hdr_match_config_set,     /* switch_match_config_set */
    bcmi_hr3_custom_hdr_match_control_get,    /* switch_match_control_get */
    bcmi_hr3_custom_hdr_match_control_set     /* switch_match_control_set */
};

STATIC const bcmi_switch_match_dev_info_t bcmi_hr3_match_dev_info = {
    {   /* service_info[bcmSwitchMatchService__Count] */
        &bcmi_hr3_miml_match_info, /* Miml */
        &bcmi_hr3_custom_hdr_match_info, /* Custom Header */
        NULL, /* GTP */
        NULL, /* HsrPrp */
        NULL, /* Dot1cb */
        NULL /* LinkLocal */
    }
};

/*
 * Function:
 *   bcmi_hr3_switch_match_info_init
 * Purpose:
 *   Setup the chip specific info.
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   devinfo - (OUT) device info structure.
 * Returns:
 *   none
 */
int
bcmi_hr3_switch_match_dev_info_init(
    int unit,
    const bcmi_switch_match_dev_info_t **dev_info)
{
    if (dev_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Return the chip info structure */
    *dev_info = &bcmi_hr3_match_dev_info;
    return BCM_E_NONE;
}
#endif /* BCM_HURRICANE3_SUPPORT */
#endif /* BCM_SWITCH_MATCH_SUPPORT */
