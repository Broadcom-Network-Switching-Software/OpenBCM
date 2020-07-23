/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IFA - In-band Flow Analyzer Monitoring Embedded Application APP interface
 * Purpose: API to configure IFA embedded app interface.
 */

#if defined(INCLUDE_IFA)

#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/debug.h>
#include <soc/scache.h>
#include <soc/trident3.h>
#include <soc/profile_mem.h>
#include <soc/uc.h>

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/idxres_fl.h>

#include <bcm/module.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/ifa.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/ifa_feature.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_td3_ifa_scache_alloc(int unit, int create);
STATIC int _bcm_td3_ifa_wb_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Initialize the IFA app. */
int _bcm_td3_ifa_init(int unit, uint8 *success)
{
    /* Init the embedded app */
    int rv = BCM_E_NONE;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    int ifa_enable = 0;

    *success = 0;
    ifa_enable = soc_property_get(unit, spn_IFA_ENABLE, 0);

    if (!ifa_enable) {
        /* Silently return since ifa is not enabled */
        return BCM_E_NONE;
    }

    if (ifa_info != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_td3_ifa_detach(unit));
        ifa_info = NULL;
    }

    _BCM_IFA_ALLOC(ifa_info, _bcm_int_ifa_info_t,
            sizeof(_bcm_int_ifa_info_t), "IFA INFO");
    if (ifa_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Failed to allocate memory")));
        return BCM_E_MEMORY;
    }
    ifa_global_info[unit] = ifa_info;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_td3_ifa_wb_recover(unit);
        if (!(ifa_info->status)) {
            sal_free(ifa_info);
            ifa_global_info[unit] = NULL;
            return BCM_E_NONE;
        }
    } else {
        rv = _bcm_td3_ifa_scache_alloc(unit, 1);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    ifa_info->dma_buffer_len = sizeof(shr_ifa_msg_ctrl_t);
    ifa_info->dma_buffer = soc_cm_salloc(unit, ifa_info->dma_buffer_len,
            "IFA DMA buffer allocate");
    if (!ifa_info->dma_buffer) {
        BCM_IF_ERROR_RETURN(bcm_xgs5_ifa_eapp_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(ifa_info->dma_buffer, 0, ifa_info->dma_buffer_len);

    rv = bcm_xgs5_ifa_eapp_init(unit);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    *success = ifa_info->status;
    return BCM_E_NONE;
}

/* Shut down the IFA app. */
int _bcm_td3_ifa_detach(int unit)
{
    /* De-init the embedded app */
    int rv = BCM_E_NONE;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_NONE;
    }

    rv = bcm_xgs5_ifa_eapp_detach(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Failed to Detach")));
        return rv;
    }

    sal_free(ifa_info);
    ifa_global_info[unit] = NULL;

    return rv;
}

/*
 * Function:
 *      _bcm_td3_ifa_collector_info_validate
 * Purpose:
 *      Validate the collector params
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      collector  - (IN)  Collector data structure
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_td3_ifa_collector_info_validate(int unit,
        bcm_ifa_collector_info_t *collector)
{

    /* Only IPv4 over UDP is supported */
    if ((collector->transport_type != bcmIfaCollectorTransportTypeIpv4Udp) &&
        (collector->transport_type != bcmIfaCollectorTransportTypeIpv6Udp)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Invalid Collector Transport Type")));
        return BCM_E_UNAVAIL;
    }

    /* Validate the export length */
    if (collector->mtu < BCM_IFA_MIN_COLLECTOR_ENCAP_LENGTH ||
        collector->mtu > SHR_IFA_MAX_COLLECTOR_ENCAP_LENGTH) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Invalid Collector Encap length configuration")));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ifa_collector_set
 * Purpose:
 *      Create a ifa collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector modify options
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_td3_ifa_collector_set(int unit,
                               uint32 options,
                               bcm_ifa_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    bcm_ifa_collector_info_t old_collector_int_info;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_INIT;
    }

    if (options & BCM_IFA_COLLECTOR_DETACH) {
        /* Detach the collector. */
        sal_memset(collector_info, 0, sizeof(bcm_ifa_collector_info_t));

        /* Inform IFA EAPP to detach collector */
        rv = bcm_xgs5_ifa_eapp_collector_set_msg(unit,
                                                 collector_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "IFA: collector detach uc message communication failed\n")));
            return rv;
        }
        return BCM_E_NONE;
    }

    /*
     * Also verify that a collector with such an ID
     * exists, else return BCM_E_NOT_FOUND
     */
    if ((options & BCM_IFA_COLLECTOR_MODIFY) &&
       !(options & BCM_IFA_COLLECTOR_ADD)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Invalid options set both ADD and MODIFY\n")));
        return BCM_E_PARAM;
    }

    /* Validate collector_info for any unsupported encap or
     * export length etc.
     */
    rv = _bcm_td3_ifa_collector_info_validate(unit, collector_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if ((options & BCM_IFA_COLLECTOR_ADD) &&
        (options & BCM_IFA_COLLECTOR_MODIFY)) {
        sal_memset(&old_collector_int_info, 0, sizeof(old_collector_int_info));
        rv = bcm_xgs5_ifa_collector_get(unit,
                                        &old_collector_int_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "IFA: Collector get failed\n")));
            return rv;
        }
    }

    /* ADD and Modify */
    if ((options & BCM_IFA_COLLECTOR_ADD)  &&
        (options & BCM_IFA_COLLECTOR_MODIFY)) {
        /* Modify the collector info in the list with ID. */
        rv = bcm_xgs5_ifa_collector_modify(unit,
                                           collector_info);
    } else {
        rv = bcm_xgs5_ifa_collector_set(unit,
                                        collector_info);
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: collector set functionality failed \n")));
        return rv;
    }

    /* Inform IFA EAPP of the change/addition in/of collector information */
    rv = bcm_xgs5_ifa_eapp_collector_set_msg(unit,
                                             collector_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Collector set uc message communication failed\n")));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ifa_collector_get
 * Purpose:
 *      Get ifa collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_td3_ifa_collector_get(int unit,
        bcm_ifa_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure collector_info is not NULL */
    if (collector_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the collector info */
    rv = bcm_xgs5_ifa_collector_get(unit, collector_info);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_ifa_config_set
 * Purpose:
 *      Create a ifa collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Configuration information modify options
 *      config_info     - (IN)  Configuration information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_td3_ifa_config_set(int unit,
                            uint32 options,
                            bcm_ifa_config_info_t *config_info)
{
    int rv = BCM_E_NONE, reg_value;
    uint16       max_len;
    bcm_ifa_config_info_t old_config_info;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_INIT;
    }

    if (options & BCM_IFA_CONFIG_CLEAR) {
        /* Clear the configuration info. */
        sal_memset(config_info, 0, sizeof(bcm_ifa_config_info_t));

        /* Inform IFA EAPP of the reset configuration information */
        rv = bcm_xgs5_ifa_eapp_config_info_msg(unit, config_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "IFA config clear uc message communication failed \n")));
            return rv;
        }
        return BCM_E_NONE;
    }

    if (config_info->hop_limit < 1) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Invalid Hoplimit: %d\n"), config_info->hop_limit));
        return BCM_E_PARAM;
    }

    if (config_info->max_payload_length < BCM_IFA_METADATA_HEADER_LENGTH) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Invalid max payload len: %d\n"), config_info->max_payload_length));
        return BCM_E_PARAM;
    }

    max_len = (config_info->hop_limit * BCM_IFA_METADATA_HEADER_LENGTH);
    if (config_info->max_payload_length < max_len) {
        max_len = config_info->max_payload_length;
    }
    max_len += BCM_IFA_MIN_RX_PACKET_LENGTH;
    max_len += config_info->rx_packet_payload_length;

    if (max_len > BCM_IFA_MAX_RX_PACKET_LENGTH) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: support maximum rx packet length %d\n"),
                  BCM_IFA_MAX_RX_PACKET_LENGTH));
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Hoplimit: %d max payload len: %d rx packet payload len: %d\n"),
                   config_info->hop_limit, config_info->max_payload_length,
                   config_info->rx_packet_payload_length));
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3(unit)) {
        if (((config_info->lb_port_1 > 66) && (config_info->lb_port_2 > 66)) ||
            ((config_info->lb_port_1 < 65) && (config_info->lb_port_2 < 65))) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "IFA App should have one loopback port per Pipe\n")));
            return BCM_E_PARAM;
        }
    }
#endif  /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_MAVERICK2_SUPPORT)
    if (SOC_IS_MAVERICK2(unit)) {
        if (((config_info->lb_port_1 > 62) && (config_info->lb_port_2 > 62))) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "IFA App should have one loopback port per Pipe\n")));
            return BCM_E_PARAM;
        }
    }
#endif  /* BCM_MAVERICK2_SUPPORT */

    /* CANCUN INT header fields validation */
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchIntMaxPayloadLength, &reg_value));
    if (reg_value != config_info->max_payload_length) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "INT HDR Maximum payload length field mismatch\n" \
                  "Set bcmSwitchIntMaxPayloadLength to %d current value: %d\n"),
                  config_info->max_payload_length, reg_value));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchIntProbeMarker1, &reg_value));
    if (reg_value != config_info->probemarker_1) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "INT HDR ProbeMarker1 field mismatch\n" \
                  "Set bcmSwitchIntProbeMarker1 to %d current value: %d\n"),
                  config_info->probemarker_1, reg_value));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchIntProbeMarker2, &reg_value));
    if (reg_value != config_info->probemarker_2) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "INT HDR ProbeMarker2 field mismatch\n" \
                  "Set bcmSwitchIntProbeMarker2 to %d current value: %d\n"),
                  config_info->probemarker_2, reg_value));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchIntL4DestPortEnable, &reg_value));
    if (reg_value) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "Disable UDP dst port based INT packet parse\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchIntProbeMarkerEnable, &reg_value));
    if (!reg_value) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "Enable prober_marker(UDF) based INT packet parse\n")));
        return BCM_E_PARAM;
    }

    /*
     * Also verify that a collector with such an ID
     * exists, else return BCM_E_NOT_FOUND
     */
    if ((options & BCM_IFA_CONFIG_MODIFY) &&
       !(options & BCM_IFA_CONFIG_ADD)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Invalid options set both ADD and MODIFY\n")));
        return BCM_E_PARAM;
    }

    if ((options & BCM_IFA_CONFIG_ADD) &&
        (options & BCM_IFA_CONFIG_MODIFY)) {
        sal_memset(&old_config_info, 0, sizeof(old_config_info));
        rv = bcm_xgs5_ifa_config_get(unit,
                                     &old_config_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "IFA: Configuration get failed\n")));
            return rv;
        }
    }

    /* ADD and Modify. */
    if ((options & BCM_IFA_CONFIG_ADD)  &&
        (options & BCM_IFA_CONFIG_MODIFY)) {
        /* Modify the configuration info in the list with ID. */
        rv = bcm_xgs5_ifa_config_modify(unit,
                                        config_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Modify config set functionality failed \n")));
            return rv;
        }
    } else {
        rv = bcm_xgs5_ifa_config_set(unit,
                                     config_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA: Add config set functionality failed \n")));
            return rv;
        }
    }

    /* Inform IFA EAPP of the change/addition in/of configuration information */
    rv = bcm_xgs5_ifa_eapp_config_info_msg(unit, config_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA App Config set uc message communication failed \n")));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ifa_config_get
 * Purpose:
 *      Get ifa configuration information
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      config_info      - (OUT)  Configuration information info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_td3_ifa_config_get(int unit,
                            bcm_ifa_config_info_t *config_info)
{
    int rv = BCM_E_NONE;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure collector_info is not NULL */
    if (config_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the collector info */
    rv = bcm_xgs5_ifa_config_get(unit, config_info);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_ifa_stat_get
 * Purpose:
 *      Get ifa statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_td3_ifa_stat_get(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    int rv = BCM_E_NONE;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure stat_data is not NULL */
    if (stat_data == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the Stat info */
    rv = bcm_xgs5_ifa_eapp_stat_info_get_msg(unit, stat_data);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_ifa_stat_set
 * Purpose:
 *      Get ifa statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_td3_ifa_stat_set(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    int rv = BCM_E_NONE;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure stat_data is not NULL */
    if (stat_data == NULL) {
        return BCM_E_PARAM;
    }

    /* Set the Stat info */
    rv = bcm_xgs5_ifa_eapp_stat_info_set_msg(unit, stat_data);
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_IFA_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_IFA_DEFAULT_VERSION  BCM_WB_IFA_VERSION_1_0

#define IFA_SCACHE_WRITE(_scache, _val, _type)                    \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define IFA_SCACHE_READ(_scache, _var, _type)                     \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)

/*
 * Function:
 *     _bcm_td3_ifa_scache_v0_size_get
 * Purpose:
 *     Get the size required to sync IFA data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_td3_ifa_scache_v0_size_get(int unit)
{
    uint32 size = 0;

    /* IFA status */
    size += sizeof(uint32);

    return size;
}

/*
 * Function:
 *     _bcm_td3_ifa_scache_v0_sync
 * Purpose:
 *     Sync IFA data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_td3_ifa_scache_v0_sync(int unit, uint8 **scache)
{
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    if (ifa_info == NULL) {
        IFA_SCACHE_WRITE(*scache, 0, uint32);
        return;
    }

    /* IFA status */
    IFA_SCACHE_WRITE(*scache, ifa_info->status, uint32);
}

/*
 * Function:
 *     _bcm_td3_ifa_scache_v0_recover
 * Purpose:
 *     Recover IFA data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td3_ifa_scache_v0_recover(int unit, uint8 **scache)
{
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    /* IFA status */
    IFA_SCACHE_READ(*scache, ifa_info->status, uint32);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ifa_scache_alloc
 * Purpose:
 *      IFA WB scache alloc
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_td3_ifa_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_td3_ifa_scache_v0_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IFA, _bcmIfaWbSequence);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                &scache, BCM_WB_IFA_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ifa_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_td3_ifa_sync(int unit)
{
    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    /* Get IFA module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IFA, _bcmIfaWbSequence);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    /* Sync IFA scache v0 */
    _bcm_td3_ifa_scache_v0_sync(unit, &scache);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ifa_wb_recover
 * Purpose:
 *      IFA WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_td3_ifa_wb_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IFA, _bcmIfaWbSequence);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                &scache, BCM_WB_IFA_DEFAULT_VERSION,
                &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_td3_ifa_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_td3_ifa_scache_v0_recover(unit, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_td3_ifa_scache_alloc(unit, 0));

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#else
typedef int bcm_make_iso_compilers_happy;
#endif /* INCLUDE_IFA */
