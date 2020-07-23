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
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#if defined(BCM_SWITCH_MATCH_SUPPORT)

/* Parameter NULL error handling */
#define GH2_MATCH_CHECK_NULL_PARAMETER(_u_, _parameter_)                      \
    do {                                                                      \
        if (NULL == (_parameter_)) {                                          \
            LOG_ERROR(BSL_LS_BCM_SWITCH,                                      \
                      (BSL_META_U((_u_),                                      \
                                  "Error: NULL parameter\n")));               \
            return BCM_E_PARAM;                                               \
        }                                                                     \
    } while (0)

/* Function Enter/Leave Tracing */
#define GH2_MATCH_FUNCTION_TRACE(_u_, _str_)                                  \
    do {                                                                      \
        LOG_VERBOSE(BSL_LS_BCM_SWITCH,                                        \
                    (BSL_META_U((_u_),                                        \
                                "%s: " _str_ "\n"), FUNCTION_NAME()));        \
    } while (0)

#define GH2_SWITCH_MATCH_TABLE_SIZE_HSRPRP       (1)
#define GH2_SWITCH_MATCH_TABLE_SIZE_DOT1CB       (1)
#define GH2_SWITCH_MATCH_TABLE_SIZE_LINKLOCAL    (2)

/* default values for HsrPrp & dot1cb (reset Value) */
#define GH2_ETHERTYPE_EMPTY_VALUE         0
static const bcm_mac_t GH2_EMPTY_MACADDR =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const bcm_mac_t GH2_EMPTY_MACADDR_MASK =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_table_size_get
 * Purpose:
 *   Get the table size for HSRPRP
 * Parameters:
 *   unit - (IN) Unit number
 *   table_size - (OUT) table size
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_hsrprp_sup_match_table_size_get(
    int unit,
    uint32 *table_size)
{
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, table_size);

    *table_size = GH2_SWITCH_MATCH_TABLE_SIZE_HSRPRP;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_hsrprp_sup_match_hw_idx_get
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
bcmi_gh2_hsrprp_sup_match_hw_idx_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_idx)
{
    uint32 table_size;
    /*
     * The tables size of HSRPRP/DOT1CB are only with 1 hw entry. Further more
     * in HSRPRP/DOT1CB cases, sw_idx should be the same as hw_idx = 0 all
     * the time.
     */

    GH2_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, hw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_hsrprp_sup_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    *hw_idx = sw_idx;
    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "hw_idx %u\n"), *hw_idx));
    GH2_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_hsrprp_sup_match_sw_idx_get
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
bcmi_gh2_hsrprp_sup_match_sw_idx_get(
    int unit,
    uint32 hw_idx,
    uint32 *sw_idx)
{
    uint32 table_size;
    /*
     * The tables size of HSRPRP/DOT1CB are only with 1 hw entry. Further more
     * in HSRPRP/DOT1CB cases, sw_idx should be the same as hw_idx = 0 all
     * the time.
     */

    GH2_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_hsrprp_sup_match_table_size_get(unit, &table_size));

    if (hw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid hw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    *sw_idx = hw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "sw_idx %u\n"), *sw_idx));
    GH2_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_table_size_get
 * Purpose:
 *   Get the table size for DOT1CB
 * Parameters:
 *   unit - (IN) Unit number
 *   table_size - (OUT) table size
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_dot1cb_sup_match_table_size_get(
    int unit,
    uint32 *table_size)
{
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, table_size);

    *table_size = GH2_SWITCH_MATCH_TABLE_SIZE_DOT1CB;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_dot1cb_sup_match_hw_idx_get
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
bcmi_gh2_dot1cb_sup_match_hw_idx_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_idx)
{
    uint32 table_size;
    /*
     * The tables size of HSRPRP/DOT1CB are only with 1 hw entry. Further more
     * in HSRPRP/DOT1CB cases, sw_idx should be the same as hw_idx = 0 all
     * the time.
     */

    GH2_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, hw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_dot1cb_sup_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    *hw_idx = sw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "hw_idx %u\n"), *hw_idx));
    GH2_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_dot1cb_sup_match_sw_idx_get
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
bcmi_gh2_dot1cb_sup_match_sw_idx_get(
    int unit,
    uint32 hw_idx,
    uint32 *sw_idx)
{
    uint32 table_size;
    /*
     * The tables size of HSRPRP/DOT1CB are only with 1 hw entry. Further more
     * in HSRPRP/DOT1CB cases, sw_idx should be the same as hw_idx = 0 all
     * the time.
     */

    GH2_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_dot1cb_sup_match_table_size_get(unit, &table_size));

    if (hw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid hw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    *sw_idx = hw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "sw_idx %u\n"), *sw_idx));
    GH2_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_linklocal_match_table_size_get
 * Purpose:
 *   Get the table size for LINKLOCAL
 * Parameters:
 *   unit - (IN) Unit number
 *   table_size - (OUT) table size
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_linklocal_match_table_size_get(
    int unit,
    uint32 *table_size)
{
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, table_size);

    *table_size = GH2_SWITCH_MATCH_TABLE_SIZE_LINKLOCAL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_linklocal_match_hw_id_get
 * Purpose:
 *      Get HW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_id - (OUT) HW field entry # in memory index 0
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_linklocal_match_hw_id_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_id)
{
    uint32 table_size;

    /* Note:
     * For linklocal case, Since there is only one memory entry in
     * LINK_LOCAL_MAC_ADDRESSm, the hw_id here means the field
     * entries #1 (hw_id = 0) or #2 (hw_id = 1). The hw_id should equal to
     * sw_idx all the time.
     */

    GH2_MATCH_FUNCTION_TRACE(unit, "IN");
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, hw_id);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_linklocal_match_table_size_get(unit, &table_size));

    if (sw_idx >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    *hw_id = sw_idx;

    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "hw_id %u\n"), *hw_id));
    GH2_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_linklocal_match_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_id - (IN) HW field entry # in memory index 0
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_linklocal_match_sw_idx_get(
    int unit,
    uint32 hw_id,
    uint32 *sw_idx)
{
    uint32 table_size;

    /* Note:
     * For linklocal case, Since there is only one memory entry in
     * LINK_LOCAL_MAC_ADDRESSm, the hw_id here means the field
     * entries #1 (hw_id = 0) or #2 (hw_id = 1). The hw_id should equal to
     * sw_idx all the time.
     */

    GH2_MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    /* Parameter validation */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_linklocal_match_table_size_get(unit, &table_size));

    if (hw_id >= table_size) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid hw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    *sw_idx = hw_id;
    LOG_DEBUG(BSL_LS_BCM_SWITCH,
              (BSL_META_U(unit,
                          "sw_idx %u\n"), *sw_idx));
    GH2_MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_config_write
 * Purpose:
 *   Write the match configuration to hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   hw_idx - (IN) HW memory index
 *   config_info - (IN) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_hsrprp_sup_match_config_write(
    int unit,
    uint32 hw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    sr_ethertypes_entry_t sr_ethertypes_entry;
    egr_sr_ethertypes_entry_t egr_sr_ethertypes_entry;
    sr_supervisory_mac_address_entry_t sr_sup_mac_address_entry;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    sal_memset(&sr_ethertypes_entry, 0,
               sizeof(sr_ethertypes_entry_t));
    sal_memset(&egr_sr_ethertypes_entry, 0,
               sizeof(egr_sr_ethertypes_entry_t));
    sal_memset(&sr_sup_mac_address_entry, 0,
               sizeof(sr_supervisory_mac_address_entry_t));

    /* Write PRP/HSR_SUPERVISION_ETHERTYPE to SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &sr_ethertypes_entry));
    soc_mem_field32_set(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                        PRP_SUPERVISION_ETHERTYPEf, config_info->ethtype1);
    soc_mem_field32_set(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                        HSR_SUPERVISION_ETHERTYPEf, config_info->ethtype2);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &sr_ethertypes_entry));

    /* Write PRP/HSR_SUPERVISION_ETHERTYPE to EGR_SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &egr_sr_ethertypes_entry));
    soc_mem_field32_set(unit, EGR_SR_ETHERTYPESm, &egr_sr_ethertypes_entry,
                        PRP_SUPERVISION_ETHERTYPEf, config_info->ethtype1);
    soc_mem_field32_set(unit, EGR_SR_ETHERTYPESm, &egr_sr_ethertypes_entry,
                        HSR_SUPERVISION_ETHERTYPEf, config_info->ethtype2);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &egr_sr_ethertypes_entry));

    /* Write MAC_ADDRESS/ADDRESS_MASK to SR_SUPERVISORY_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_entry));

    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESSf,
                                                 config_info->dst_mac);
    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESS_MASKf,
                                                 config_info->dst_mac_mask);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_SUPERVISORY_MAC_ADDRESSm, MEM_BLOCK_ALL,
                       hw_idx, &sr_sup_mac_address_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_config_write
 * Purpose:
 *   Write the match configuration to hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   hw_idx - (IN) HW memory index
 *   config_info - (IN) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_dot1cb_sup_match_config_write(
    int unit,
    uint32 hw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    sr_ethertypes_entry_t sr_ethertypes_entry;
    egr_sr_ethertypes_entry_t egr_sr_ethertypes_entry;
    sr_supervisory_mac_address_dot1cb_entry_t sr_sup_mac_address_dot1cb_entry;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    sal_memset(&sr_ethertypes_entry, 0,
               sizeof(sr_ethertypes_entry_t));
    sal_memset(&egr_sr_ethertypes_entry, 0,
               sizeof(egr_sr_ethertypes_entry_t));
    sal_memset(&sr_sup_mac_address_dot1cb_entry, 0,
               sizeof(sr_supervisory_mac_address_dot1cb_entry_t));

    /* Write DOT1CB_SUPERVISION_ETHERTYPE to SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &sr_ethertypes_entry));
    soc_mem_field32_set(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                        DOT1CB_SUPERVISION_ETHERTYPEf, config_info->ethtype1);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &sr_ethertypes_entry));

    /* Write DOT1CB_SUPERVISION_ETHERTYPE to EGR_SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &egr_sr_ethertypes_entry));
    soc_mem_field32_set(unit, EGR_SR_ETHERTYPESm, &egr_sr_ethertypes_entry,
                        DOT1CB_SUPERVISION_ETHERTYPEf, config_info->ethtype1);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &egr_sr_ethertypes_entry));

    /* Write MAC_ADDRESS/ADDRESS_MASK to SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_dot1cb_entry));

    soc_mem_mac_addr_set(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry, MAC_ADDRESSf,
                         config_info->dst_mac);
    soc_mem_mac_addr_set(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry, MAC_ADDRESS_MASKf,
                         config_info->dst_mac_mask);


    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm, MEM_BLOCK_ALL,
                       hw_idx, &sr_sup_mac_address_dot1cb_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_linklocal_match_config_write
 * Purpose:
 *   Write the match configuration to hardware
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 *   config_info - (IN) Match configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_linklocal_match_config_write(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    /*
     * For linklocal case, the hardware index (hw_idx) is always 0 and there are
     * two entries for MACs/MAC_MASK . Thus, sw_idx could be 0 or 1, but the
     * hw_idx is 0 all the time.
     */
    uint32 hw_idx;
    link_local_mac_address_entry_t link_local_mac_addr_ent;
    egr_link_local_mac_address_entry_t egr_link_local_mac_addr_ent;
    soc_field_t mac_field, mac_mask_field;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    if (sw_idx == 0) {
        mac_field = MAC_ADDRESS_1f;
        mac_mask_field = MASK_1f;
    } else if (sw_idx == 1) {
        mac_field = MAC_ADDRESS_2f;
        mac_mask_field = MASK_2f;
    } else {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    sal_memset(&link_local_mac_addr_ent, 0,
               sizeof(link_local_mac_address_entry_t));
    sal_memset(&egr_link_local_mac_addr_ent, 0,
               sizeof(egr_link_local_mac_address_entry_t));

    /* The hw index for linklocal is always 0 */
    hw_idx = 0;

    /* Write MAC_ADDRESS/ADDRESS_MASK to LINK_LOCAL_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &link_local_mac_addr_ent));

    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                             &link_local_mac_addr_ent,
                                             mac_field,
                                             config_info->dst_mac);
    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                             &link_local_mac_addr_ent,
                                             mac_mask_field,
                                             config_info->dst_mac_mask);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ALL,
                       hw_idx, &link_local_mac_addr_ent));

    /* Write MAC_ADDRESS/ADDRESS_MASK to EGR_LINK_LOCAL_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &egr_link_local_mac_addr_ent));
    soc_EGR_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &egr_link_local_mac_addr_ent,
                                                 mac_field,
                                                 config_info->dst_mac);
    soc_EGR_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &egr_link_local_mac_addr_ent,
                                                 mac_mask_field,
                                                 config_info->dst_mac_mask);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ALL,
                       hw_idx, &egr_link_local_mac_addr_ent));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_config_add
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
bcmi_gh2_hsrprp_sup_match_config_add(
    int unit,
    bcm_switch_match_config_info_t *config_info,
    uint32 *sw_idx)
{
    /*
     * The tables size of HSRPRP/DOT1CB is only with 1 hw entry. So, forcing to
     * write 0 means to write the only entry. Further more in HSRPRP/DOT1CB
     * cases, sw_idx should be the same as hw_idx all the time.
     */
    uint32 hw_idx = 0;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_hsrprp_sup_match_config_write(unit,
                                                hw_idx,
                                                config_info));
    *sw_idx = hw_idx;
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_config_add
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
bcmi_gh2_dot1cb_sup_match_config_add(
    int unit,
    bcm_switch_match_config_info_t *config_info,
    uint32 *sw_idx)
{
    /*
     * The tables size of HSRPRP/DOT1CB is only with 1 hw entry. So, forcing to
     * write 0 means to write the only entry. Further more in HSRPRP/DOT1CB
     * cases, sw_idx should be the same as hw_idx all the time.
     */
    uint32 hw_idx = 0;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_dot1cb_sup_match_config_write(unit,
                                                hw_idx,
                                                config_info));
    *sw_idx = hw_idx;
    return BCM_E_NONE;
}


/*
 * Function:
 *   bcmi_gh2_linklocal_match_config_add
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
bcmi_gh2_linklocal_match_config_add(
    int unit,
    bcm_switch_match_config_info_t *config_info,
    uint32 *sw_idx)
{
    /*
     * For linklocal case, the hardware index (hw_idx) is always 0 and there are
     * two entries for MACs/MAC_MASK . Thus, sw_idx could be 0 or 1, but the
     * hw_idx is 0 all the time.
     * sw_idx == 0 is for MAC_ADDRESS_1f & MASK_1f
     * sw_idx == 1 is for MAC_ADDRESS_2f & MASK_2f
     */
    uint32 hw_idx = 0;
    link_local_mac_address_entry_t link_local_mac_addr_ent;
    egr_link_local_mac_address_entry_t egr_link_local_mac_addr_ent;
    bcm_mac_t tmp_mac_mask1, tmp_mac_mask2;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, sw_idx);

    sal_memset(&link_local_mac_addr_ent, 0,
               sizeof(link_local_mac_address_entry_t));
    sal_memset(&egr_link_local_mac_addr_ent, 0,
               sizeof(egr_link_local_mac_address_entry_t));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &link_local_mac_addr_ent));
    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_get(unit,
                                             &link_local_mac_addr_ent,
                                             MASK_1f,
                                             tmp_mac_mask1);
    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_get(unit,
                                             &link_local_mac_addr_ent,
                                             MASK_2f,
                                             tmp_mac_mask2);

    /* If the MASK_1f/2f == GH2_EMPTY_MACADDR_MASK, then it's empty */
    if (SAL_MAC_ADDR_EQUAL(tmp_mac_mask1, GH2_EMPTY_MACADDR_MASK)) {
        *sw_idx = 0;
    } else if (SAL_MAC_ADDR_EQUAL(tmp_mac_mask2, GH2_EMPTY_MACADDR_MASK)) {
        *sw_idx = 1;
    } else {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "bcm_gh2_linklocal_match_config_add failed: "
                              "No hardware resource.\n")));
        return BCM_E_RESOURCE;
    }

    /* linklocal's hw_idx is always 0 with MAC_ADDRESS_1f/2f & MASK_1f/2f */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_linklocal_match_config_write(unit,
                                               *sw_idx,
                                               config_info));
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_config_delete
 * Purpose:
 *   Destroy a match ID by removing it from hardware.
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_hsrprp_sup_match_config_delete(
    int unit,
    uint32 sw_idx)
{
    uint32 hw_idx = sw_idx;
    sr_ethertypes_entry_t sr_ethertypes_entry;
    egr_sr_ethertypes_entry_t egr_sr_ethertypes_entry;
    sr_supervisory_mac_address_entry_t sr_sup_mac_address_entry;

    sal_memset(&sr_ethertypes_entry, 0,
               sizeof(sr_ethertypes_entry_t));
    sal_memset(&egr_sr_ethertypes_entry, 0,
               sizeof(egr_sr_ethertypes_entry_t));
    sal_memset(&sr_sup_mac_address_entry, 0,
               sizeof(sr_supervisory_mac_address_entry_t));

    /* Write PRP/HSR_SUPERVISION_ETHERTYPE to SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &sr_ethertypes_entry));
    soc_mem_field32_set(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                        PRP_SUPERVISION_ETHERTYPEf,
                        GH2_ETHERTYPE_EMPTY_VALUE);
    soc_mem_field32_set(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                        HSR_SUPERVISION_ETHERTYPEf,
                        GH2_ETHERTYPE_EMPTY_VALUE);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &sr_ethertypes_entry));

    /* Write PRP/HSR_SUPERVISION_ETHERTYPE to EGR_SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &egr_sr_ethertypes_entry));
    soc_mem_field32_set(unit, EGR_SR_ETHERTYPESm, &egr_sr_ethertypes_entry,
                        PRP_SUPERVISION_ETHERTYPEf,
                        GH2_ETHERTYPE_EMPTY_VALUE);
    soc_mem_field32_set(unit, EGR_SR_ETHERTYPESm, &egr_sr_ethertypes_entry,
                        HSR_SUPERVISION_ETHERTYPEf,
                        GH2_ETHERTYPE_EMPTY_VALUE);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &egr_sr_ethertypes_entry));

    /* Write MAC_ADDRESS/ADDRESS_MASK to SR_SUPERVISORY_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_entry));
    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESSf,
                                                 GH2_EMPTY_MACADDR);
    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESS_MASKf,
                                                 GH2_EMPTY_MACADDR_MASK);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_SUPERVISORY_MAC_ADDRESSm, MEM_BLOCK_ALL,
                       hw_idx, &sr_sup_mac_address_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_config_delete
 * Purpose:
 *   Destroy a match ID by removing it from hardware.
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_dot1cb_sup_match_config_delete(
    int unit,
    uint32 sw_idx)
{
    uint32 hw_idx = sw_idx;
    sr_ethertypes_entry_t sr_ethertypes_entry;
    egr_sr_ethertypes_entry_t egr_sr_ethertypes_entry;
    sr_supervisory_mac_address_dot1cb_entry_t sr_sup_mac_address_dot1cb_entry;

    sal_memset(&sr_ethertypes_entry, 0,
               sizeof(sr_ethertypes_entry_t));
    sal_memset(&egr_sr_ethertypes_entry, 0,
               sizeof(egr_sr_ethertypes_entry_t));
    sal_memset(&sr_sup_mac_address_dot1cb_entry, 0,
               sizeof(sr_supervisory_mac_address_dot1cb_entry_t));

    /* Write DOT1CB_SUPERVISION_ETHERTYPE to SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &sr_ethertypes_entry));
    soc_mem_field32_set(unit, SR_ETHERTYPESm,
                        &sr_ethertypes_entry,
                        DOT1CB_SUPERVISION_ETHERTYPEf,
                        GH2_ETHERTYPE_EMPTY_VALUE);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &sr_ethertypes_entry));

    /* Write DOT1CB_SUPERVISION_ETHERTYPE to EGR_SR_ETHERTYPESm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &egr_sr_ethertypes_entry));
    soc_mem_field32_set(unit, EGR_SR_ETHERTYPESm,
                        &egr_sr_ethertypes_entry,
                        DOT1CB_SUPERVISION_ETHERTYPEf,
                        GH2_ETHERTYPE_EMPTY_VALUE);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_SR_ETHERTYPESm, MEM_BLOCK_ALL,
                       hw_idx, &egr_sr_ethertypes_entry));

    /* Write MAC_ADDRESS/ADDRESS_MASK to SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_dot1cb_entry));

    soc_mem_mac_addr_set(unit,
                         SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry,
                         MAC_ADDRESSf,
                         GH2_EMPTY_MACADDR);
    soc_mem_mac_addr_set(unit,
                         SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry,
                         MAC_ADDRESS_MASKf,
                         GH2_EMPTY_MACADDR_MASK);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm, MEM_BLOCK_ALL,
                       hw_idx, &sr_sup_mac_address_dot1cb_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_linklocal_match_config_delete
 * Purpose:
 *   Destroy a match ID by removing it from hardware.
 * Parameters:
 *   unit - (IN) Unit number
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_linklocal_match_config_delete(
    int unit,
    uint32 sw_idx)
{
    /*
     * For linklocal case, the hardware index (hw_idx) is always 0 and there are
     * two entries for MACs/MAC_MASK . Thus, sw_idx could be 0 or 1, but the
     * hw_idx is 0 all the time.
     */
    uint32 hw_idx = 0;
    link_local_mac_address_entry_t link_local_mac_addr_ent;
    egr_link_local_mac_address_entry_t egr_link_local_mac_addr_ent;
    soc_field_t mac_field, mac_mask_field;

    if (sw_idx == 0) {
        mac_field = MAC_ADDRESS_1f;
        mac_mask_field = MASK_1f;
    } else if (sw_idx == 1) {
        mac_field = MAC_ADDRESS_2f;
        mac_mask_field = MASK_2f;
    } else {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    sal_memset(&link_local_mac_addr_ent, 0,
               sizeof(link_local_mac_address_entry_t));
    sal_memset(&egr_link_local_mac_addr_ent, 0,
               sizeof(egr_link_local_mac_address_entry_t));

    /* Write MAC_ADDRESS/ADDRESS_MASK to LINK_LOCAL_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &link_local_mac_addr_ent));

    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                             &link_local_mac_addr_ent,
                                             mac_field,
                                             GH2_EMPTY_MACADDR);
    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                             &link_local_mac_addr_ent,
                                             mac_mask_field,
                                             GH2_EMPTY_MACADDR_MASK);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ALL,
                       hw_idx, &link_local_mac_addr_ent));

    /* Write MAC_ADDRESS/ADDRESS_MASK to EGR_LINK_LOCAL_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &egr_link_local_mac_addr_ent));
    soc_EGR_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &egr_link_local_mac_addr_ent,
                                                 mac_field,
                                                 GH2_EMPTY_MACADDR);
    soc_EGR_LINK_LOCAL_MAC_ADDRESSm_mac_addr_set(unit,
                                                 &egr_link_local_mac_addr_ent,
                                                 mac_mask_field,
                                                 GH2_EMPTY_MACADDR_MASK);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ALL,
                       hw_idx, &egr_link_local_mac_addr_ent));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_config_get
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
bcmi_gh2_hsrprp_sup_match_config_get(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 hw_idx = sw_idx;
    uint32 rval = 0;
    sr_ethertypes_entry_t sr_ethertypes_entry;
    sr_supervisory_mac_address_entry_t sr_sup_mac_address_entry;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    sal_memset(&sr_ethertypes_entry, 0,
               sizeof(sr_ethertypes_entry_t));
    sal_memset(&sr_sup_mac_address_entry, 0,
               sizeof(sr_supervisory_mac_address_entry_t));

    /*
     * Get PRP/HSR_SUPERVISION_ETHERTYPE from SR_ETHERTYPESm
     * Note:
     * Only retrieve Ethernet Type from SR_ETHERTYPES since both
     * SR_ETHERTYPES/EGR_SR_ETHERTYPES are filled with the same Ethernet Type
     */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &sr_ethertypes_entry));
    rval = soc_mem_field32_get(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                               PRP_SUPERVISION_ETHERTYPEf);
    config_info->ethtype1 = rval;
    rval = soc_mem_field32_get(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                               HSR_SUPERVISION_ETHERTYPEf);
    config_info->ethtype2 = rval;

    /* Get MAC_ADDRESS/ADDRESS_MASK from SR_SUPERVISORY_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_entry));

    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_get(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESSf,
                                                 config_info->dst_mac);
    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_get(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESS_MASKf,
                                                 config_info->dst_mac_mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_config_get
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
bcmi_gh2_dot1cb_sup_match_config_get(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 hw_idx = sw_idx;
    uint32 rval = 0;
    sr_ethertypes_entry_t sr_ethertypes_entry;
    sr_supervisory_mac_address_dot1cb_entry_t sr_sup_mac_address_dot1cb_entry;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    sal_memset(&sr_ethertypes_entry, 0,
               sizeof(sr_ethertypes_entry_t));
    sal_memset(&sr_sup_mac_address_dot1cb_entry, 0,
               sizeof(sr_supervisory_mac_address_dot1cb_entry_t));

    /*
     * Get DOT1CB_SUPERVISION_ETHERTYPE from SR_ETHERTYPESm
     * Note:
     * Only retrive Ethernet Type from SR_ETHERTYPES since both
     * SR_ETHERTYPES/EGR_SR_ETHERTYPES are filled with the same Ethernet Type
     */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY,
                      hw_idx, &sr_ethertypes_entry));
    rval = soc_mem_field32_get(unit, SR_ETHERTYPESm, &sr_ethertypes_entry,
                               DOT1CB_SUPERVISION_ETHERTYPEf);
    config_info->ethtype1 = rval;

    /* Get MAC_ADDRESS/ADDRESS_MASK from SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_dot1cb_entry));


    soc_mem_mac_addr_get(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry,
                         MAC_ADDRESSf, config_info->dst_mac);
    soc_mem_mac_addr_get(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry,
                         MAC_ADDRESS_MASKf, config_info->dst_mac_mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_linklocal_match_config_get
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
bcmi_gh2_linklocal_match_config_get(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    /*
     * For linklocal case, the hardware index (hw_idx) is always 0 and there are
     * two entries for MACs/MAC_MASK . Thus, sw_idx could be 0 or 1, but the
     * hw_idx is 0 all the time.
     */
    uint32 hw_idx = 0;
    link_local_mac_address_entry_t link_local_mac_addr_ent;
    soc_field_t mac_field, mac_mask_field;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    if (sw_idx == 0) {
        mac_field = MAC_ADDRESS_1f;
        mac_mask_field = MASK_1f;
    } else if (sw_idx == 1) {
        mac_field = MAC_ADDRESS_2f;
        mac_mask_field = MASK_2f;
    } else {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }
    sal_memset(&link_local_mac_addr_ent, 0,
               sizeof(link_local_mac_address_entry_t));

    /* Get MAC_ADDRESS/ADDRESS_MASK to LINK_LOCAL_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &link_local_mac_addr_ent));

    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_get(unit,
                                             &link_local_mac_addr_ent,
                                             mac_field,
                                             config_info->dst_mac);
    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_get(unit,
                                             &link_local_mac_addr_ent,
                                             mac_mask_field,
                                             config_info->dst_mac_mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_config_set
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
bcmi_gh2_hsrprp_sup_match_config_set(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 hw_idx = sw_idx;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_hsrprp_sup_match_config_write(unit,
                                                hw_idx,
                                                config_info));
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_config_set
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
bcmi_gh2_dot1cb_sup_match_config_set(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    uint32 hw_idx = sw_idx;

    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_dot1cb_sup_match_config_write(unit,
                                                hw_idx,
                                                config_info));
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_linklocal_match_config_set
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
bcmi_gh2_linklocal_match_config_set(
    int unit,
    uint32 sw_idx,
    bcm_switch_match_config_info_t *config_info)
{
    /* Parameter NULL error handling */
    GH2_MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    if ((sw_idx != 0) && (sw_idx != 1)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    /* linklocal's hw_idx is always 0 with MAC_ADDRESS_1f/2f & MASK_1f/2f */
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_linklocal_match_config_write(unit,
                                               sw_idx,
                                               config_info));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_hsrprp_sup_match_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   sw_idx - (IN) memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_gh2_hsrprp_sup_match_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    uint32 hw_idx = sw_idx;
    bcm_mac_t mac_mask;
    sr_supervisory_mac_address_entry_t sr_sup_mac_address_entry;

    sal_memset(&sr_sup_mac_address_entry, 0,
               sizeof(sr_supervisory_mac_address_entry_t));

    /* Get MAC_ADDRESS_MASK from SR_SUPERVISORY_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_entry));

    soc_SR_SUPERVISORY_MAC_ADDRESSm_mac_addr_get(unit,
                                                 &sr_sup_mac_address_entry,
                                                 MAC_ADDRESS_MASKf,
                                                 mac_mask);

    if (SAL_MAC_ADDR_EQUAL(mac_mask, GH2_EMPTY_MACADDR_MASK)) {
        /* Return BCM_E_NOT_FOUND means HW doesn't exist */
        return BCM_E_NOT_FOUND;
    }
    /* BCM_E_NONE means HW existed */
    return BCM_E_NONE;
#else /* BCM_WARM_BOOT_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

/*
 * Function:
 *   bcmi_gh2_dot1cb_sup_match_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   sw_idx - (IN) memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_gh2_dot1cb_sup_match_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    uint32 hw_idx = sw_idx;
    bcm_mac_t mac_mask;
    sr_supervisory_mac_address_dot1cb_entry_t sr_sup_mac_address_dot1cb_entry;

    sal_memset(&sr_sup_mac_address_dot1cb_entry, 0,
               sizeof(sr_supervisory_mac_address_dot1cb_entry_t));

    /* Get MAC_ADDRESS_MASK from SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm, MEM_BLOCK_ANY,
                      hw_idx, &sr_sup_mac_address_dot1cb_entry));

    soc_mem_mac_addr_get(unit, SR_SUPERVISORY_MAC_ADDRESS_DOT1CBm,
                         &sr_sup_mac_address_dot1cb_entry,
                         MAC_ADDRESS_MASKf, mac_mask);


    if (SAL_MAC_ADDR_EQUAL(mac_mask, GH2_EMPTY_MACADDR_MASK)) {
        /* Return BCM_E_NOT_FOUND means HW doesn't exist */
        return BCM_E_NOT_FOUND;
    }
    /* BCM_E_NONE means HW existed */
    return BCM_E_NONE;
#else /* BCM_WARM_BOOT_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

/*
 * Function:
 *   bcmi_gh2_linklocal_match_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   sw_idx - (IN) memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_gh2_linklocal_match_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    /* The hw index for linklocal is always 0 */
    uint32 hw_idx = 0;
    bcm_mac_t mac_mask;

    link_local_mac_address_entry_t link_local_mac_addr_ent;
    soc_field_t mac_mask_field;

    sal_memset(&link_local_mac_addr_ent, 0,
               sizeof(link_local_mac_address_entry_t));

    if (sw_idx == 0) {
        mac_mask_field = MASK_1f;
    } else if (sw_idx == 1) {
        mac_mask_field = MASK_2f;
    } else {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Invalid sw_idx.\n")));
        return BCM_E_RESOURCE;
    }

    /* The hw index for linklocal is 0 */
    hw_idx = 0;

    /* Get MAC_ADDRESS_MASK from LINK_LOCAL_MAC_ADDRESSm */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, LINK_LOCAL_MAC_ADDRESSm, MEM_BLOCK_ANY,
                      hw_idx, &link_local_mac_addr_ent));

    soc_LINK_LOCAL_MAC_ADDRESSm_mac_addr_get(unit,
                                             &link_local_mac_addr_ent,
                                             mac_mask_field,
                                             mac_mask);

    if (SAL_MAC_ADDR_EQUAL(mac_mask, GH2_EMPTY_MACADDR_MASK)) {
        /* Return BCM_E_NOT_FOUND means HW doesn't exist */
        return BCM_E_NOT_FOUND;
    }
    /* BCM_E_NONE means HW existed */
    return BCM_E_NONE;
#else /* BCM_WARM_BOOT_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

STATIC const
bcmi_switch_match_service_info_t bcmi_gh2_hsrprp_sup_match_info = {
    "gh2_hsrprp_sup_match",                   /* table_name */
    bcmi_gh2_hsrprp_sup_match_table_size_get, /* switch_match_table_size_get */
    bcmi_gh2_hsrprp_sup_match_hw_idx_get,     /* switch_match_hw_id_get */
    bcmi_gh2_hsrprp_sup_match_sw_idx_get,     /* switch_match_sw_idx_get */
    bcmi_gh2_hsrprp_sup_match_wb_hw_existed,  /* switch_match_wb_hw_existed */
    bcmi_gh2_hsrprp_sup_match_config_add,     /* switch_match_config_add */
    bcmi_gh2_hsrprp_sup_match_config_delete,  /* switch_match_config_delete */
    bcmi_gh2_hsrprp_sup_match_config_get,     /* switch_match_config_get */
    bcmi_gh2_hsrprp_sup_match_config_set,     /* switch_match_config_set */
    NULL,                                     /* switch_match_control_get */
    NULL                                      /* switch_match_control_set */
};

STATIC const
bcmi_switch_match_service_info_t bcmi_gh2_dot1cb_sup_match_info = {
    "gh2_dot1cb_sup_match",                   /* table_name */
    bcmi_gh2_dot1cb_sup_match_table_size_get, /* switch_match_table_size_get */
    bcmi_gh2_dot1cb_sup_match_hw_idx_get,     /* switch_match_hw_id_get */
    bcmi_gh2_dot1cb_sup_match_sw_idx_get,     /* switch_match_sw_idx_get */
    bcmi_gh2_dot1cb_sup_match_wb_hw_existed,  /* switch_match_wb_hw_existed */
    bcmi_gh2_dot1cb_sup_match_config_add,     /* switch_match_config_add */
    bcmi_gh2_dot1cb_sup_match_config_delete,  /* switch_match_config_delete */
    bcmi_gh2_dot1cb_sup_match_config_get,     /* switch_match_config_get */
    bcmi_gh2_dot1cb_sup_match_config_set,     /* switch_match_config_set */
    NULL,                                     /* switch_match_control_get */
    NULL                                      /* switch_match_control_set */
};

STATIC const
bcmi_switch_match_service_info_t bcmi_gh2_linklocal_match_info = {
    "gh2_linklocal_match",                    /* table_name */
    bcmi_gh2_linklocal_match_table_size_get,  /* switch_match_table_size_get */
    bcmi_gh2_linklocal_match_hw_id_get,       /* switch_match_hw_id_get */
    bcmi_gh2_linklocal_match_sw_idx_get,      /* switch_match_sw_idx_get */
    bcmi_gh2_linklocal_match_wb_hw_existed,   /* switch_match_wb_hw_existed */
    bcmi_gh2_linklocal_match_config_add,      /* switch_match_config_add */
    bcmi_gh2_linklocal_match_config_delete,   /* switch_match_config_delete */
    bcmi_gh2_linklocal_match_config_get,      /* switch_match_config_get */
    bcmi_gh2_linklocal_match_config_set,      /* switch_match_config_set */
    NULL,                                     /* switch_match_control_get */
    NULL                                      /* switch_match_control_set */
};

STATIC const bcmi_switch_match_dev_info_t bcmi_gh2_match_dev_info = {
    {   /* service_info[bcmSwitchMatchService__Count] */
        &bcmi_hr3_miml_match_info, /* Miml */
        &bcmi_hr3_custom_hdr_match_info, /* Customer Header */
        NULL, /* GTP */
        &bcmi_gh2_hsrprp_sup_match_info, /* HsrPrp */
        &bcmi_gh2_dot1cb_sup_match_info, /* Dot1cb */
        &bcmi_gh2_linklocal_match_info /* LinkLocal */
    }
};

/*
 * Function:
 *   bcmi_gh2_switch_match_info_init
 * Purpose:
 *   Setup the chip specific info.
 * Parameters:
 *   unit - (IN) Unit being initialized
 *   devinfo - (OUT) device info structure.
 * Returns:
 *   none
 */
int
bcmi_gh2_switch_match_dev_info_init(
    int unit,
    const bcmi_switch_match_dev_info_t **dev_info)
{
    if (dev_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Return the chip info structure */
    *dev_info = &bcmi_gh2_match_dev_info;
    return BCM_E_NONE;
}
#endif /* BCM_SWITCH_MATCH_SUPPORT */
#endif /* BCM_GREYHOUND2_SUPPORT */

