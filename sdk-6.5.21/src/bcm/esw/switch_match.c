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

#if defined(BCM_SWITCH_MATCH_SUPPORT)

/*
 * Switch Match id encoding:
 * 4-bit bcm_switch_match_service_t (+encoding base) + 12-bit (reserved)
 * + 16-bit sw_idx
 */
#define BCMI_MATCH_TYPE_ENCODING_BASE    (1)
#define BCMI_MATCH_TYPE_SHIFT            (28)
#define BCMI_MATCH_TYPE_MASK             (0xF)
#define BCMI_MATCH_SW_IDX_MASK           (0xFFFF)

/*
 * Translate between Match_ID and sw_idx & Switch Match Type
 * It represents the entry is used or not in intf_bitmap[match_service]
 * with the MARCO MATCH_INTF_GET/SET/CLR
 * Note:
 * The sw_idx is not the hardware index of register/memory.
 * There is a hw_idx which is mapping in the chip specific layer
 * which is a real HW index
 */
#define MATCH_ID_TO_SW_IDX(_match_id_) \
        ((_match_id_) & BCMI_MATCH_SW_IDX_MASK)

#define SW_IDX_TO_MATCH_ID(_sw_idx_, _service_type_) \
        ((_sw_idx_) | \
        (_service_type_ + BCMI_MATCH_TYPE_ENCODING_BASE) << BCMI_MATCH_TYPE_SHIFT)

#define MATCH_TYPE_GET(_id_) \
        ((((_id_) >> BCMI_MATCH_TYPE_SHIFT) & BCMI_MATCH_TYPE_MASK) \
         - BCMI_MATCH_TYPE_ENCODING_BASE)

#define MATCH_ID_IS_TYPE(_id_, _service_type_) \
        (MATCH_TYPE_GET(_id_) == (_service_type_))

/*
 * Software book keeping for Switch Match related information
 */
typedef struct bcmi_match_bk_s {
    SHR_BITDCL  *intf_bitmap[bcmSwitchMatchService__Count];
    sal_mutex_t match_mutex;
    int initialized;
    /* Switch match service platform-specific information */
    const bcmi_switch_match_dev_info_t *dev_info;
} bcmi_match_bk_t;

STATIC bcmi_match_bk_t bcmi_match_bk_info[BCM_MAX_NUM_UNITS];

/* Flag to check first initialized status */
STATIC uint8 bcmi_match_initialized_bool = FALSE;

/* Switch match SW book keeping instance fetch/lock/unlock */
#define MATCH_BK_INFO(_u_) (&bcmi_match_bk_info[(_u_)])

#define MATCH_LOCK(_u_) \
        sal_mutex_take(MATCH_BK_INFO(_u_)->match_mutex, sal_mutex_FOREVER)

#define MATCH_UNLOCK(_u_) \
        sal_mutex_give(MATCH_BK_INFO(_u_)->match_mutex)

/*
 * Switch match usage bitmap operations
 */
#define MATCH_INTF_GET(_u_, _intf_, _service_) \
        (SHR_BITGET(MATCH_BK_INFO(_u_)->intf_bitmap[(_service_)], (_intf_)))
#define MATCH_INTF_SET(_u_, _intf_, _service_) \
        (SHR_BITSET(MATCH_BK_INFO(_u_)->intf_bitmap[(_service_)], (_intf_)))
#define MATCH_INTF_CLR(_u_, _intf_, _service_) \
        (SHR_BITCLR(MATCH_BK_INFO(_u_)->intf_bitmap[(_service_)], (_intf_)))

#define MATCH_DEVICE_INFO(_u_, _service_) \
        bcmi_match_bk_info[(_u_)].dev_info->service_info[(_service_)]

#define MATCH_CHECK_INIT(_u_, _service_)                                      \
    do {                                                                      \
        if (((_u_) < 0) || ((_u_) >= BCM_MAX_NUM_UNITS)) {                    \
            return BCM_E_UNIT;                                                \
        }                                                                     \
        if (!soc_feature(unit, soc_feature_switch_match)) {                   \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
        if (!bcmi_match_bk_info[(_u_)].initialized) {                         \
            LOG_ERROR(BSL_LS_BCM_SWITCH,                                      \
                      (BSL_META_U(unit,                                       \
                                  "Switch Match Error:"                       \
                                  " not initialized\n")));                    \
            return BCM_E_INIT;                                                \
        }                                                                     \
        if (!bcmi_match_bk_info[(_u_)].dev_info ||                            \
            !bcmi_match_bk_info[(_u_)].dev_info->service_info[(_service_)]) { \
            LOG_ERROR(BSL_LS_BCM_SWITCH,                                      \
                      (BSL_META_U(unit,                                       \
                                  "Switch Match Error:"                       \
                                  " not supported\n")));                      \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
    } while (0)

/* Parameter NULL error handling */
#define MATCH_CHECK_NULL_PARAMETER(_u_, _parameter_)                          \
    do {                                                                      \
        if (NULL == (_parameter_)) {                                          \
            LOG_ERROR(BSL_LS_BCM_SWITCH,                                      \
                      (BSL_META_U((_u_),                                      \
                                  "Error: NULL parameter\n")));               \
            return BCM_E_PARAM;                                               \
        }                                                                     \
    } while (0)

/* Device Information NULL error handling */
#define MATCH_CHECK_NULL_DEV_INSTANCE(_u_, _instance_)                        \
    do {                                                                      \
        if (NULL == (_instance_)) {                                           \
            LOG_ERROR(BSL_LS_BCM_SWITCH,                                      \
                      (BSL_META_U((_u_),                                      \
                                  "Error: NULL chip specific function \n"))); \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
    } while (0)

/* Function Enter/Leave Tracing */
#define MATCH_FUNCTION_TRACE(_u_, _str_)                                      \
    do {                                                                      \
        LOG_VERBOSE(BSL_LS_BCM_SWITCH,                                        \
                    (BSL_META_U((_u_),                                        \
                                "%s: " _str_ "\n"), FUNCTION_NAME()));        \
    } while (0)

/*
 * Function:
 *   bcmi_switch_match_dev_info_init
 * Purpose:
 *   register switch match chip specific information
 * Parameters:
 *   unit - (IN) Unit being initialized
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_switch_match_dev_info_init(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcm_switch_match_service_t match_service;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Chip specific initialization */
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        rv = bcmi_hr3_switch_match_dev_info_init(unit,
                &(bcmi_match_bk_info[unit].dev_info));
    } else
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_switch_match_dev_info_init(unit,
                &(bcmi_match_bk_info[unit].dev_info));
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }

    if (BCM_FAILURE(rv) || NULL == bcmi_match_bk_info[unit].dev_info) {
        /* No chip specific information initialized */
        LOG_WARN(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "Switch Match not supported:\n"
                             "dev_info_init failed.\n")));
        if (BCM_SUCCESS(rv)) {
            return BCM_E_UNAVAIL;
        }
        return rv;
    }

    /* Check if MATCH_DEVICE_INFO(unit, match_service) are all NULL */
    for (match_service = bcmSwitchMatchServiceMiml;
         match_service < bcmSwitchMatchService__Count;
         match_service++) {
        if (NULL != MATCH_DEVICE_INFO(unit, match_service)) {
            /* Found instance */
            return BCM_E_NONE;
        }
    }

    /* All MATCH_DEVICE_INFO(unit, match_service) are NULL */
    LOG_WARN(BSL_LS_BCM_SWITCH,
             (BSL_META_U(unit,
                         "Switch Match not supported:\n"
                         "platform initialized, but none "
                         "Switch Match services are available.\n")));

    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmi_switch_match_service_check
 * Purpose:
 *    Check the supported match service.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
bcmi_switch_match_service_check(
    int unit,
    bcm_switch_match_service_t match_service)
{
    MATCH_FUNCTION_TRACE(unit, "IN");

    if (!((match_service >= bcmSwitchMatchServiceMiml) &&
        (match_service < bcmSwitchMatchService__Count))) {
        LOG_WARN(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "Wrong Switch Match Service (%d)\n"),
                             match_service));
        return BCM_E_PARAM;
    }

    if ((match_service == bcmSwitchMatchServiceMiml) &&
        (soc_feature(unit, soc_feature_miml))) {
        return BCM_E_NONE;
    }

    if ((match_service == bcmSwitchMatchServiceCustomHeader) &&
        (soc_feature(unit, soc_feature_custom_header))) {
        return BCM_E_NONE;
    }

    if ((match_service == bcmSwitchMatchServicePrpHsrSupervision) &&
        ((soc_feature(unit, soc_feature_tsn_sr_hsr)) ||
        (soc_feature(unit, soc_feature_tsn_sr_prp)))) {
        return BCM_E_NONE;
    }

    if ((match_service == bcmSwitchMatchServiceDot1cbSupervision) &&
        (soc_feature(unit, soc_feature_tsn_sr_802_1cb))) {
        return BCM_E_NONE;
    }

    if ((match_service == bcmSwitchMatchServiceLinkLocal) &&
        (soc_feature(unit, soc_feature_tsn_sr))) {
        return BCM_E_NONE;
    }

    LOG_WARN(BSL_LS_BCM_SWITCH,
             (BSL_META_U(unit,
                         "Switch Match Service (%d) not supported\n"),
                         match_service));

    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmi_switch_match_control_check
 * Purpose:
 *    Check the supported match control type according to the match service.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    control_type - (IN) Match control type
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
bcmi_switch_match_control_check(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type)
{
    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    if (!((control_type >= bcmSwitchMatchControlPortEnable) &&
        (control_type < bcmSwitchMatchControl__Count))) {
        LOG_WARN(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "Wrong Switch Match Service Control Type (%d)\n"),
                             control_type));
        return BCM_E_PARAM;
    }

    if (match_service == bcmSwitchMatchServiceMiml) {
        if (control_type == bcmSwitchMatchControlPortEnable) {
            return BCM_E_NONE;
        }
    }

    if (match_service == bcmSwitchMatchServiceCustomHeader) {
        if (control_type == bcmSwitchMatchControlPortEnable ||
            control_type == bcmSwitchMatchControlMatchMask) {
            return BCM_E_NONE;
        }
    }
    LOG_WARN(BSL_LS_BCM_SWITCH,
             (BSL_META_U(unit,
                         "Wrong Switch Match Control\n")));

    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmi_switch_match_id_check
 * Purpose:
 *    Check match_id is valid
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    match_id - (IN) Match ID
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
bcmi_switch_match_id_check(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcmi_match_bk_t *match_bk_info = NULL;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    uint32 sw_idx = MATCH_ID_TO_SW_IDX(match_id);
    uint32 table_size;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    if (!MATCH_ID_IS_TYPE(match_id, match_service)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "match_id(0x%x) doesn't match "
                              "SwitchMatchService(%d)\n"),
                              match_id,
                              match_service));
        return BCM_E_PARAM;
    }

    /* Get chip specific assignment & bookkeeping structure */
    match_bk_info = MATCH_BK_INFO(unit);
    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_table_size_get);

    rv = sm_dev_info->switch_match_table_size_get(unit, &table_size);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Get table size failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    /* Check if sw_idx excess the table size or not */
    if (sw_idx >= table_size) {
        return BCM_E_PARAM;
    }

    /* Check if the valid bitmap is NULL or not */
    if (NULL == match_bk_info->intf_bitmap[match_service]) {
        return BCM_E_UNAVAIL;
    }

    /* Check if the sw_idx is valid or not */
    if (!MATCH_INTF_GET(unit, sw_idx, match_service)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "HW resource (sw_idx:%d) doesn't exist.\n"),
                              sw_idx));
        return BCM_E_NOT_FOUND;
    }
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_match_hw_id_get
 * Purpose:
 *      Get HW memory index with given map_id
 * Parameters:
 *      unit     - (IN) Unit number
 *      match_id - (IN) Match ID
 *      match_service - (OUT) Match service type
 *      hw_id - (OUT) Priority Map Type
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_switch_match_hw_id_get(
    int unit,
    int match_id,
    bcm_switch_match_service_t *match_service,
    uint32 *hw_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    uint32 sw_idx;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, match_service);
    MATCH_CHECK_NULL_PARAMETER(unit, hw_id);

    *match_service = MATCH_TYPE_GET(match_id);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, *match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, *match_service);

    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_id_check(unit, *match_service, match_id));

    sw_idx = MATCH_ID_TO_SW_IDX(match_id);

    sm_dev_info = MATCH_DEVICE_INFO(unit, *match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_hw_id_get);

    rv = sm_dev_info->switch_match_hw_id_get(unit, sw_idx, hw_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "HW operation failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_match_match_id_get
 * Purpose:
 *      Get map_id with given HW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      match_service - (IN) Priority Map Type
 *      hw_id - (IN) HW memory index
 *      match_id - (OUT) Match ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_switch_match_match_id_get(
    int unit,
    bcm_switch_match_service_t match_service,
    uint32 hw_id,
    int *match_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    uint32 sw_idx;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, match_id);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_sw_idx_get);

    rv = sm_dev_info->switch_match_sw_idx_get(unit, hw_id, &sw_idx);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "HW operation failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    *match_id = SW_IDX_TO_MATCH_ID(sw_idx, match_service);

    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_id_check(unit, match_service, *match_id));

    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_switch_match_free_resources
 * Purpose:
 *   Free match bookkeeping resources associated with a given unit. This
 *   happens either during an error initializing or during a detach operation.
 * Parameters:
 *   unit - (IN) Unit number
 * Returns:
 *   none
 */
STATIC void
bcmi_switch_match_free_resources(int unit)
{
    bcmi_match_bk_t *match_bk_info = MATCH_BK_INFO(unit);
    bcm_switch_match_service_t match_service;

    MATCH_FUNCTION_TRACE(unit, "IN");
    for (match_service = bcmSwitchMatchServiceMiml;
         match_service < bcmSwitchMatchService__Count;
         match_service++) {
        if (match_bk_info->intf_bitmap[match_service]) {
            sal_free(match_bk_info->intf_bitmap[match_service]);
            match_bk_info->intf_bitmap[match_service] = NULL;
        }
    }

    if (match_bk_info->match_mutex) {
        sal_mutex_destroy(match_bk_info->match_mutex);
        match_bk_info->match_mutex = NULL;
    }
    MATCH_FUNCTION_TRACE(unit, "OUT");
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *   bcmi_switch_match_reinit
 * Purpose:
 *   Re-initialize the resource. Restore bookkeeping information.
 * Parameters:
 *   unit - (IN) Unit being initialized
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_switch_match_reinit(int unit)
{
    int rv = BCM_E_UNAVAIL;
    uint32 table_size;
    uint32 sw_idx;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    bcm_switch_match_service_t match_service;

    MATCH_FUNCTION_TRACE(unit, "IN");

    for (match_service = bcmSwitchMatchServiceMiml;
         match_service < bcmSwitchMatchService__Count;
         match_service++) {

        sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

        if (NULL == sm_dev_info) {
            /* Need to check all match_service */
            continue;
        }

        /* Check if the chip specific function existed */
        MATCH_CHECK_NULL_DEV_INSTANCE(
            unit, sm_dev_info->switch_match_table_size_get);
        MATCH_CHECK_NULL_DEV_INSTANCE(
            unit, sm_dev_info->switch_match_wb_hw_existed);

        rv = sm_dev_info->switch_match_table_size_get(unit, &table_size);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SWITCH,
                      (BSL_META_U(unit,
                                  "Get table size failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

        /* Scan all sw_idx (0 ~ table_size) to check existed HW */
        for (sw_idx = 0; sw_idx < table_size; sw_idx++) {
            MATCH_INTF_CLR(unit, sw_idx, match_service);

            /* BCM_SUCCESS means HW(sw_idx) existed */
            rv = sm_dev_info->switch_match_wb_hw_existed(unit, sw_idx);
            if (BCM_SUCCESS(rv)) {
                MATCH_INTF_SET(unit, sw_idx, match_service);
            }
        }
    }
    MATCH_FUNCTION_TRACE(unit, "OUT");
    /* Return BCM_E_NONE means that WB successfully */
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *   bcmi_switch_match_coldboot_init
 * Purpose:
 *   Reset the HW to default status
 * Parameters:
 *   unit - (IN) unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_switch_match_coldboot_init(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    const bcmi_switch_match_service_info_t *sm_dev_info;
    bcm_switch_match_service_t match_service;
    uint32 table_size;
    uint32 sw_idx;

#if defined(BCM_WARM_BOOT_SUPPORT)
    /* Warm boot should be false in cold boot case */
    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Reset all type to default value */
    for (match_service = bcmSwitchMatchServiceMiml;
         match_service < bcmSwitchMatchService__Count;
         match_service++) {

        sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

        /* Check if the chip specific function existed */
        if (NULL == sm_dev_info) {
            /* Maybe some chips only have some services */
            continue;
        }
        /* Check if the chip specific function existed */
        MATCH_CHECK_NULL_DEV_INSTANCE(
            unit, sm_dev_info->switch_match_table_size_get);
        MATCH_CHECK_NULL_DEV_INSTANCE(
            unit, sm_dev_info->switch_match_config_delete);

        rv = sm_dev_info->switch_match_table_size_get(unit, &table_size);

        for (sw_idx = 0;
             sw_idx < table_size;
             sw_idx++) {
            /* Use delete for reset HW to default value */
            rv = sm_dev_info->switch_match_config_delete(unit, sw_idx);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SWITCH,
                          (BSL_META_U(unit,
                                      "coldboot fail(rv=%d) on "
                                      "Switch Match Service(%d)\n"),
                                      rv,
                                      match_service));
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_switch_match_detach
 * Purpose:
 *   Detach resource. Called when the module should de-initialize.
 * Parameters:
 *   unit - (IN) Unit being detached.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_switch_match_detach(int unit)
{
    MATCH_FUNCTION_TRACE(unit, "IN");
    if (bcmi_match_bk_info[unit].initialized) {
        bcmi_switch_match_free_resources(unit);
        bcmi_match_bk_info[unit].initialized = FALSE;
        bcmi_match_bk_info[unit].dev_info = NULL;
    }
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_switch_match_init
 * Purpose:
 *   Initialize the resource.  Allocate bookkeeping information.
 * Parameters:
 *   unit - (IN) Unit being initialized
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_switch_match_init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcmi_match_bk_t *match_bk_info = NULL;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    int i;
    uint32 table_size, shr_bitdcl_size;
    bcm_switch_match_service_t match_service;

    MATCH_FUNCTION_TRACE(unit, "IN");

    if (!soc_feature(unit, soc_feature_switch_match)) {
        return BCM_E_UNAVAIL;
    }

    /* Check if leading bits is enough for identifying different types*/
    assert((1 << (32 - BCMI_MATCH_TYPE_SHIFT - 1)) >= bcmSwitchMatchService__Count);

    /* Initial all unit with default value FALSE. */
    if (FALSE == bcmi_match_initialized_bool) {
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            bcmi_match_bk_info[unit].initialized = FALSE;
            bcmi_match_bk_info[unit].dev_info = NULL;
        }
        bcmi_match_initialized_bool = TRUE;
    }

    /*
     * Check whether the unit is initialized or not
     * if it's initialized, and then free resource for later initialization.
     */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_detach(unit));

    /* Initialize platform-specific functions related information */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_dev_info_init(unit));

    /* Check if chip specific information is NULL or not */
    if (NULL == bcmi_match_bk_info[unit].dev_info) {
        return BCM_E_UNAVAIL;
    }

    /* Get chip specific assignment & bookkeeping structure */
    match_bk_info = MATCH_BK_INFO(unit);

    /* Create all match_id list */
    for (match_service = bcmSwitchMatchServiceMiml;
         match_service < bcmSwitchMatchService__Count;
         match_service++) {

        sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

        /* Check if the chip specific function existed */
        if (NULL == sm_dev_info) {
            /* Maybe some chips only have some services */
            continue;
        }

        /* Check if the chip specific function existed */
        MATCH_CHECK_NULL_DEV_INSTANCE(
            unit, sm_dev_info->switch_match_table_size_get);

        rv = sm_dev_info->switch_match_table_size_get(unit, &table_size);
        if (BCM_FAILURE(rv) || !(table_size > 0)) {
            LOG_ERROR(BSL_LS_BCM_SWITCH,
                      (BSL_META_U(unit,
                                  "Get table size failed(rv = %d)\n"),
                                  rv));
            /* Table size should > 0 */
            assert(table_size > 0);
            return rv;
        }

        /* Allocate usage bitmap */
        shr_bitdcl_size = SHR_BITALLOCSIZE(table_size);
        match_bk_info->intf_bitmap[match_service] =
            sal_alloc(shr_bitdcl_size, "Switch Map Valid intf_bitmap");
        if (match_bk_info->intf_bitmap[match_service] == NULL) {
            bcmi_switch_match_free_resources(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(match_bk_info->intf_bitmap[match_service],
                   0, shr_bitdcl_size);
    }

    /* Create mutex */
    match_bk_info->match_mutex = sal_mutex_create("switch_match.lock");
    if (match_bk_info->match_mutex == NULL) {
        bcmi_switch_match_free_resources(unit);
        return BCM_E_MEMORY;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery */
        rv = bcmi_switch_match_reinit(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN
                (bcmi_switch_match_detach(unit));
            return rv;
        }
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        /* Cold Boot */
        rv = bcmi_switch_match_coldboot_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_switch_match_detach(unit);
            LOG_ERROR(BSL_LS_BCM_SWITCH,
                      (BSL_META_U(unit,
                                  "Cold boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }
    bcmi_match_bk_info[unit].initialized = TRUE;

    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_config_add
 * Purpose:
 *    Add a match config to hardware.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    config_info - (IN) Match configuration
 *    match_id - (OUT) Match ID
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_config_add(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_info_t *config_info,
    int *match_id)
{
    bcm_error_t rv = BCM_E_NONE;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    uint32 sw_idx;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, config_info);
    MATCH_CHECK_NULL_PARAMETER(unit, match_id);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    /* Get chip specific assignment & bookkeeping structure */
    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_table_size_get);
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_config_add);

    MATCH_LOCK(unit);

    /* Check if there is any empty sw_idx */
    rv = sm_dev_info->switch_match_config_add(unit,
                                              config_info,
                                              &sw_idx);
    if (BCM_FAILURE(rv)) {
        MATCH_UNLOCK(unit);
        if (BCM_E_EXISTS == rv) {
            *match_id = SW_IDX_TO_MATCH_ID(sw_idx, match_service);
            return BCM_E_EXISTS;
        }
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "add to HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    *match_id = SW_IDX_TO_MATCH_ID(sw_idx, match_service);
    /* Set current sw_idx to valid */
    MATCH_INTF_SET(unit, sw_idx, match_service);

    MATCH_UNLOCK(unit);

    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_config_delete
 * Purpose:
 *    Delete a match config with the given match_id by removing it from
 *    hardware and deleting the associated software state info.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    match_id - (IN) Match ID
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_config_delete(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id)
{
    int rv;
    uint32 sw_idx = MATCH_ID_TO_SW_IDX(match_id);
    const bcmi_switch_match_service_info_t *sm_dev_info;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_id_check(unit, match_service, match_id));

    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_config_delete);

    MATCH_LOCK(unit);
    rv = sm_dev_info->switch_match_config_delete(unit,
                                                 sw_idx);
    if (BCM_FAILURE(rv)) {
        MATCH_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "delete HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    /* Clear the ID in software table */
    MATCH_INTF_CLR(unit, sw_idx, match_service);

    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_config_delete_all
 * Purpose:
 *    Delete all match config with the given match_service.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_config_delete_all(
    int unit,
    bcm_switch_match_service_t match_service)
{
    bcm_error_t rv = BCM_E_NONE;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    uint32 table_size, sw_idx;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    /* Get chip specific assignment & bookkeeping structure */
    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_table_size_get);
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_config_delete);

    rv = sm_dev_info->switch_match_table_size_get(unit, &table_size);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Get table size failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    MATCH_LOCK(unit);

    for (sw_idx = 0; sw_idx < table_size; sw_idx++) {
        if (MATCH_INTF_GET(unit, sw_idx, match_service)) {

            rv = sm_dev_info->switch_match_config_delete(unit,
                                                         sw_idx);
            if (BCM_FAILURE(rv)) {
                MATCH_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_SWITCH,
                          (BSL_META_U(unit,
                                      "Delete HW failed(rv = %d)\n"),
                                      rv));
                return rv;
            }

            /* Clear the ID in software table */
            MATCH_INTF_CLR(unit, sw_idx, match_service);
        }
    }

    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_config_get
 * Purpose:
 *    Get the configuration of the match with the given match_id.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    match_id - (IN) Match ID
 *    config_info - (OUT) Match configuration
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_config_get(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t *config_info)
{
    int rv;
    const bcmi_switch_match_service_info_t *sm_dev_info;
    uint32 sw_idx;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_id_check(unit, match_service, match_id));

    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_config_get);

    sw_idx = MATCH_ID_TO_SW_IDX(match_id);

    bcm_switch_match_config_info_t_init(config_info);

    MATCH_LOCK(unit);
    rv = sm_dev_info->switch_match_config_get(unit,
                                              sw_idx,
                                              config_info);
    if (BCM_FAILURE(rv)) {
        MATCH_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Read from HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_config_set
 * Purpose:
 *    Set the configuration of the match with the given match_id.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    match_id - (IN) Match ID
 *    config_info - (IN) Match configuration
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_config_set(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t *config_info)
{
    int rv;
    const bcmi_switch_match_service_info_t *sm_dev_info;
    uint32 sw_idx;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, config_info);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_id_check(unit, match_service, match_id));

    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_config_set);

    sw_idx = MATCH_ID_TO_SW_IDX(match_id);

    MATCH_LOCK(unit);
    rv = sm_dev_info->switch_match_config_set(unit,
                                              sw_idx,
                                              config_info);
    if (BCM_FAILURE(rv)) {
        MATCH_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Write to HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}


/*
 * Function:
 *    bcmi_switch_match_config_traverse
 * Purpose:
 *    Traverse the created match config.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    cb_fn - (IN) Traverse call back function
 *    user_data - (IN) User data
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_config_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_traverse_cb cb_fn,
    void *user_data)
{
    bcm_error_t rv = BCM_E_NONE;
    const bcmi_switch_match_service_info_t *sm_dev_info = NULL;
    int sw_idx = 0;
    int match_id;
    uint32 table_size;
    bcm_switch_match_config_info_t match_config;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, cb_fn);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    /* Get chip specific assignment & bookkeeping structure */
    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);


    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_table_size_get);

    rv = sm_dev_info->switch_match_table_size_get(unit, &table_size);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Get table size failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    MATCH_LOCK(unit);

    for (sw_idx = 0; sw_idx < table_size; sw_idx++) {
        if (MATCH_INTF_GET(unit, sw_idx, match_service)) {
            match_id = SW_IDX_TO_MATCH_ID(sw_idx, match_service);
            /* Reset the config */
            bcm_switch_match_config_info_t_init(&match_config);

            /* Get the config */
            rv = bcmi_switch_match_config_get(unit,
                                              match_service,
                                              match_id,
                                              &match_config);

            /* Doing the user cb function with config */
            if (BCM_SUCCESS(rv)) {
                rv = cb_fn(unit, match_id, &match_config, user_data);
                if (BCM_FAILURE(rv)) {
                    MATCH_UNLOCK(unit);
                    return rv;
                }
            }
        }
    }
    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_control_get
 * Purpose:
 *    Get the match related control information with given control service
 *    and type.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    control_type - (IN) Match control type
 *    gport - (IN) gport number for port basis control.
 *                 Otherwise, BCM_GPORT_INVALID.
 *    control_info - (OUT) Match control information
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_control_get(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info)
{
    int rv;
    const bcmi_switch_match_service_info_t *sm_dev_info;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, control_info);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_control_check(unit, match_service, control_type));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);


    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_control_get);

    MATCH_LOCK(unit);
    bcm_switch_match_control_info_t_init(control_info);
    rv = sm_dev_info->switch_match_control_get(unit,
                                               control_type,
                                               gport,
                                               control_info);
    if (BCM_FAILURE(rv)) {
        MATCH_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Read from HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_control_set
 * Purpose:
 *    Set the match related control information with given control type.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    control_type - (IN) Match control type
 *    gport - (IN) gport number for port basis control.
 *                 Otherwise, BCM_GPORT_INVALID.
 *    control_info - (IN) Match control information
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_control_set(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info)
{
    int rv;
    const bcmi_switch_match_service_info_t *sm_dev_info;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, control_info);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_control_check(unit, match_service, control_type));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    sm_dev_info = MATCH_DEVICE_INFO(unit, match_service);

    /* Check if the chip specific function existed */
    MATCH_CHECK_NULL_DEV_INSTANCE(
        unit, sm_dev_info->switch_match_control_set);

    MATCH_LOCK(unit);
    rv = sm_dev_info->switch_match_control_set(unit,
                                               control_type,
                                               gport,
                                               control_info);
    if (BCM_FAILURE(rv)) {
        MATCH_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "Write to HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_switch_match_control_traverse
 * Purpose:
 *    Traverse the match control information.
 * Parameters:
 *    unit - (IN) Unit number
 *    match_service - (IN) Match service type
 *    cb_fn - (IN) Traverse call back function
 *    user_data - (IN) User data
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_switch_match_control_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_traverse_cb cb_fn,
    void *user_data)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_port_t port;
    bcm_switch_match_control_info_t match_ctrl;

    MATCH_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    MATCH_CHECK_NULL_PARAMETER(unit, cb_fn);

    /* Parameter validation check */
    BCM_IF_ERROR_RETURN
        (bcmi_switch_match_service_check(unit, match_service));

    /* Initialization check */
    MATCH_CHECK_INIT(unit, match_service);

    MATCH_LOCK(unit);

    if (match_service == bcmSwitchMatchServiceMiml ||
        match_service == bcmSwitchMatchServiceCustomHeader) {
        /* Traverse all logical port with bcmSwitchMatchControlPortEnable */
        PBMP_ALL_ITER(unit, port) {
            bcm_switch_match_control_info_t_init(&match_ctrl);

            rv = bcmi_switch_match_control_get(unit,
                                               match_service,
                                               bcmSwitchMatchControlPortEnable,
                                               port,
                                               &match_ctrl);
            if (BCM_SUCCESS(rv)) {
                rv = cb_fn(unit,
                           bcmSwitchMatchControlPortEnable,
                           port,
                           &match_ctrl, user_data);
                if (BCM_FAILURE(rv)) {
                    MATCH_UNLOCK(unit);
                    return rv;
                }
            }
        }
    }

    if (match_service == bcmSwitchMatchServiceCustomHeader) {

        /* Traverse bcmSwitchMatchControlMatchMask */
        bcm_switch_match_control_info_t_init(&match_ctrl);
        rv = bcmi_switch_match_control_get(unit,
                                           match_service,
                                           bcmSwitchMatchControlMatchMask,
                                           BCM_GPORT_INVALID,
                                           &match_ctrl);
        if (BCM_SUCCESS(rv)) {
            rv = cb_fn(unit,
                       bcmSwitchMatchControlMatchMask,
                       BCM_GPORT_INVALID,
                       &match_ctrl,
                       user_data);
            if (BCM_FAILURE(rv)) {
                MATCH_UNLOCK(unit);
                return rv;
            }
        }
    }
    MATCH_UNLOCK(unit);
    MATCH_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}
#endif /* BCM_SWITCH_MATCH_SUPPORT */

