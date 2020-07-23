/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/preemption.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif
#include <bcm_int/esw/portctrl.h>
#if defined(BCM_MONTEREY_SUPPORT)
#include <bcm_int/esw/monterey.h>
#endif


#if defined(BCM_WARM_BOOT_SUPPORT)
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_PREEMPTION_SUPPORT)
soc_pbmp_t pbmp_verify_enable_cache[SOC_MAX_NUM_DEVICES];
sal_mutex_t verify_enable_cache_mutex[SOC_MAX_NUM_DEVICES];

#define PREEMPTION_LOCK(_unit_) \
        sal_mutex_take(verify_enable_cache_mutex[(_unit_)], sal_mutex_FOREVER);
#define PREEMPTION_UNLOCK(_unit_) \
        sal_mutex_give(verify_enable_cache_mutex[(_unit_)]);

/*
 * Function:
 *      bcmi_esw_preemption_unimac_enable_tx_set
 * Purpose:
 *      set preemption tx enable or not on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_enable_tx_set(int unit, bcm_port_t port,
                                         uint32 value)
{
    return soc_reg_field32_modify(unit, MAC_MERGE_CONFIGr,
                                  port, PREEMPT_ENABLEf,
                                  value ? 1 : 0);
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_enable_tx_set
 * Purpose:
 *      set preemption tx enable or not on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_enable_tx_set(int unit, bcm_port_t port,
                                        uint32 value)
{
    return soc_reg_field32_modify(unit, XLMAC_MERGE_CONFIGr,
                                  port, PREEMPT_ENABLEf,
                                  value ? 1 : 0);
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_enable_verify_set
 * Purpose:
 *      set preemption verify enable or not on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_enable_verify_set(int unit, bcm_port_t port,
                                         uint32 value)
{
    return soc_reg_field32_modify(unit, MAC_MERGE_CONFIGr,
                                  port, VERIFY_ENABLEf,
                                  value ? 1 : 0);
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_enable_verify_set
 * Purpose:
 *      set preemption verify enable or not on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_enable_verify_set(int unit, bcm_port_t port,
                                        uint32 value)
{
    return soc_reg_field32_modify(unit, XLMAC_MERGE_CONFIGr,
                                  port, VERIFY_ENABLEf,
                                  value ? 1 : 0);
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_enable_tx_get
 * Purpose:
 *      get preemption tx enable or not on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_enable_tx_get(int unit, bcm_port_t port,
                                         uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, MAC_MERGE_CONFIGr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, MAC_MERGE_CONFIGr,
                                   regval64, PREEMPT_ENABLEf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_enable_tx_get
 * Purpose:
 *      get preemption tx enable or not on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_enable_tx_get(int unit, bcm_port_t port,
                                        uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, XLMAC_MERGE_CONFIGr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                   regval64, PREEMPT_ENABLEf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_mode_set
 * Purpose:
 *      set preemption mode on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_mode_set(int unit, bcm_port_t port,
                                    uint32 value)
{
    return soc_reg_field32_modify(unit, MAC_MERGE_CONFIGr,
                                  port, PREEMPTION_MODEf,
                                  value ? 1 : 0);
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_mode_set
 * Purpose:
 *      set preemption mode on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_mode_set(int unit, bcm_port_t port,
                                   uint32 value)
{
    return soc_reg_field32_modify(unit, XLMAC_MERGE_CONFIGr,
                                  port, PREEMPTION_MODEf,
                                  value ? 1 : 0);
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_mode_get
 * Purpose:
 *      get preemption mode on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_mode_get(int unit, bcm_port_t port,
                                    uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, MAC_MERGE_CONFIGr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, MAC_MERGE_CONFIGr,
                                   regval64, PREEMPTION_MODEf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_mode_get
 * Purpose:
 *      get preemption mode on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_mode_get(int unit, bcm_port_t port,
                                   uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, XLMAC_MERGE_CONFIGr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                   regval64, PREEMPTION_MODEf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_verify_config_set
 * Purpose:
 *      set preemption verify config on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_verify_config_set(int unit, bcm_port_t port,
                                             bcm_port_preempt_control_t type,
                                             uint32 value)

{
    uint32 hw_value;
    soc_field_t field;

    switch(type) {
        case bcmPortPreemptControlVerifyTime:
            field = VERIFY_TIMEf;
            break;
        case bcmPortPreemptControlVerifyAttempts:
            field = VERIFY_ATTEMPT_LIMITf;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (value < 1) {
        return BCM_E_PARAM;
    }
    if (value > (1 << soc_reg_field_length(unit, MAC_MERGE_CONFIGr,
                                          field))) {
        return BCM_E_PARAM;
    }
    hw_value = value - 1;
    return soc_reg_field32_modify(unit, MAC_MERGE_CONFIGr, port,
                                  field, hw_value);
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_verify_config_set
 * Purpose:
 *      set preemption verify config on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_verify_config_set(int unit, bcm_port_t port,
                                            bcm_port_preempt_control_t type,
                                            uint32 value)

{
    uint32 hw_value;
    soc_field_t field;

    switch(type) {
        case bcmPortPreemptControlVerifyTime:
            field = VERIFY_TIMEf;
            break;
        case bcmPortPreemptControlVerifyAttempts:
            field = VERIFY_ATTEMPT_LIMITf;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (value < 1) {
        return BCM_E_PARAM;
    }
    if (value > (1 << soc_reg_field_length(unit, XLMAC_MERGE_CONFIGr,
                                          field))) {
        return BCM_E_PARAM;
    }
    hw_value = value - 1;
    return soc_reg_field32_modify(unit, XLMAC_MERGE_CONFIGr, port,
                                  field, hw_value);
}


/*
 * Function:
 *      bcmi_esw_preemption_unimac_verify_config_get
 * Purpose:
 *      get preemption verify config on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_verify_config_get(int unit, bcm_port_t port,
                                             bcm_port_preempt_control_t type,
                                             uint32* value)

{
    uint64 regval64;
    soc_field_t field;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    switch(type) {
        case bcmPortPreemptControlVerifyTime:
            field = VERIFY_TIMEf;
            break;
        case bcmPortPreemptControlVerifyAttempts:
            field = VERIFY_ATTEMPT_LIMITf;
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, MAC_MERGE_CONFIGr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, MAC_MERGE_CONFIGr,
                                   regval64, field) + 1;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_verify_config_get
 * Purpose:
 *      get preemption verify config on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_verify_config_get(int unit, bcm_port_t port,
                                            bcm_port_preempt_control_t type,
                                            uint32* value)

{
    uint64 regval64;
    soc_field_t field;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    switch(type) {
        case bcmPortPreemptControlVerifyTime:
            field = VERIFY_TIMEf;
            break;
        case bcmPortPreemptControlVerifyAttempts:
            field = VERIFY_ATTEMPT_LIMITf;
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, XLMAC_MERGE_CONFIGr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                   regval64, field) + 1;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_frag_config_rx_set
 * Purpose:
 *      set preemption fragment config on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_frag_config_rx_set(int unit, bcm_port_t port,
                                              bcm_port_preempt_control_t type,
                                              uint32 value)
{
    uint32 hw_value;
    bcm_port_t phy_port = port;
    int i,pindex = -1;
    int found = 0;
    int is_final = 1;
    soc_reg_t reg[8]={GE0_MIN_FRAG_SIZEr,
                      GE1_MIN_FRAG_SIZEr,
                      GE2_MIN_FRAG_SIZEr,
                      GE3_MIN_FRAG_SIZEr,
                      GE4_MIN_FRAG_SIZEr,
                      GE5_MIN_FRAG_SIZEr,
                      GE6_MIN_FRAG_SIZEr,
                      GE7_MIN_FRAG_SIZEr};

    switch (type) {
        case bcmPortPreemptControlNonFinalFragSizeRx:
            is_final = 0;
            /* coverity[fallthrough:FALSE] */
        case bcmPortPreemptControlFinalFragSizeRx:
            if (value > 512 || value < 64 || value % 8) {
                return BCM_E_PARAM;
            }
            hw_value = value / 8;
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            }
            for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
                int block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                if (SOC_BLK_GPORT == SOC_BLOCK_TYPE(unit, block)) {
                    pindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                    found = 1;
                    break;
                }
            }
            if (0 == found || pindex < 0 || pindex > 7) {
                return BCM_E_INTERNAL;
            }
            return soc_reg_field32_modify(unit, reg[pindex], port,
                                          (is_final ? FINAL_FRAGf:
                                                      NON_FINAL_FRAGf),
                                           hw_value);
        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_frag_config_rx_set
 * Purpose:
 *      set preemption fragment config on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_frag_config_rx_set(int unit, bcm_port_t port,
                                             bcm_port_preempt_control_t type,
                                             uint32 value)
{
    uint32 hw_value;
    int is_final = 1;

    switch (type) {
        case bcmPortPreemptControlNonFinalFragSizeRx:
            is_final = 0;
            /* coverity[fallthrough:FALSE] */
        case bcmPortPreemptControlFinalFragSizeRx:
            if (value > 512 || value < 64 || value % 8) {
                return BCM_E_PARAM;
            }
            hw_value = value / 8;
            return soc_reg_field32_modify(unit, XLPORT_MIB_MIN_FRAG_SIZEr, port,
                                          (is_final ? MIN_FINAL_FRAG_SIZEf:
                                                      MIN_NON_FINAL_FRAG_SIZEf),
                                          hw_value);
        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_frag_config_rx_get
 * Purpose:
 *      get preemption fragment config on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_frag_config_rx_get(int unit, bcm_port_t port,
                                              bcm_port_preempt_control_t type,
                                              uint32* value)
{
    bcm_port_t phy_port = port;
    int i,pindex = -1;
    int found = 0;
    uint64 regval64;
    soc_reg_t reg[8]={GE0_MIN_FRAG_SIZEr,
                      GE1_MIN_FRAG_SIZEr,
                      GE2_MIN_FRAG_SIZEr,
                      GE3_MIN_FRAG_SIZEr,
                      GE4_MIN_FRAG_SIZEr,
                      GE5_MIN_FRAG_SIZEr,
                      GE6_MIN_FRAG_SIZEr,
                      GE7_MIN_FRAG_SIZEr};
    int is_final = 1;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    switch (type) {
        case bcmPortPreemptControlNonFinalFragSizeRx:
            is_final = 0;
            /* coverity[fallthrough:FALSE] */
        case bcmPortPreemptControlFinalFragSizeRx:
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            }
            for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
                int block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                if (SOC_BLK_GPORT == SOC_BLOCK_TYPE(unit, block)) {
                    pindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                    found = 1;
                    break;
                }
            }
            if (0 == found || pindex < 0 || pindex > 7) {
                return BCM_E_INTERNAL;
            }
            BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg[pindex],
                                            port, 0, &regval64));
            *value = soc_reg64_field32_get(unit, reg[pindex], regval64,
                                           (is_final ? FINAL_FRAGf:
                                                       NON_FINAL_FRAGf)) * 8;
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_frag_config_rx_get
 * Purpose:
 *      get preemption fragment config on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_frag_config_rx_get(int unit, bcm_port_t port,
                                             bcm_port_preempt_control_t type,
                                             uint32* value)
{
    int is_final = 1;
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    switch (type) {
        case bcmPortPreemptControlNonFinalFragSizeRx:
            is_final = 0;
            /* coverity[fallthrough:FALSE] */
        case bcmPortPreemptControlFinalFragSizeRx:
            BCM_IF_ERROR_RETURN(soc_reg_get(unit, XLPORT_MIB_MIN_FRAG_SIZEr,
                                            port, 0, &regval64));
            *value = soc_reg64_field32_get(unit,
                                           XLPORT_MIB_MIN_FRAG_SIZEr,
                                           regval64,
                                           (is_final ? MIN_FINAL_FRAG_SIZEf:
                                                       MIN_NON_FINAL_FRAG_SIZEf)) * 8;
             break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_status_tx_get
 * Purpose:
 *      get preemption tx status on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_status_tx_get(int unit, bcm_port_t port,
                                         uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, MAC_MERGE_VERIFY_STATEr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, MAC_MERGE_VERIFY_STATEr,
                                   regval64, PREEMPTION_TX_STATUSf);
    if (*value > bcmPortPreemptStatusTxActive) {
        return BCM_E_INTERNAL;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_status_tx_get
 * Purpose:
 *      get preemption tx status on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_status_tx_get(int unit, bcm_port_t port,
                                        uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, XLMAC_MERGE_VERIFY_STATEr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, XLMAC_MERGE_VERIFY_STATEr,
                                   regval64, PREEMPTION_TX_STATUSf);
    if (*value > bcmPortPreemptStatusTxActive) {
        return BCM_E_INTERNAL;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_unimac_status_verify_get
 * Purpose:
 *      get preemption verify status on unimac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_unimac_status_verify_get(int unit, bcm_port_t port,
                                             uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, MAC_MERGE_VERIFY_STATEr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, MAC_MERGE_VERIFY_STATEr,
                                   regval64, PREEMPTION_VERIFY_STATUSf);
    if (*value > bcmPortPreemptStatusVerifyFailed) {
        return BCM_E_INTERNAL;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_xlmac_status_verify_get
 * Purpose:
 *      get preemption verify status on xlmac
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) bcmPortPreemptControlXXXX
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_xlmac_status_verify_get(int unit, bcm_port_t port,
                                             uint32* value)
{
    uint64 regval64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, XLMAC_MERGE_VERIFY_STATEr, port,
                                    0, &regval64));
    *value = soc_reg64_field32_get(unit, XLMAC_MERGE_VERIFY_STATEr,
                                   regval64, PREEMPTION_VERIFY_STATUSf);
    if (*value > bcmPortPreemptStatusVerifyFailed) {
        return BCM_E_INTERNAL;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_enable_tx_set
 * Purpose:
 *      set preemption tx enable or not
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_enable_tx_set(int unit, bcm_port_t port, uint32 value)
{
    bcmi_port_mac_type_t mac_type;
    
    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));

    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_enable_tx_set(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
       return  bcmi_esw_preemption_unimac_enable_tx_set(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      _bcmi_esw_preemption_enable_verify_set
 * Purpose:
 *      helper function to set preemption verify enable or not in hw
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcmi_esw_preemption_enable_verify_set(int unit, bcm_port_t port, uint32 value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_enable_verify_set(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
       return  bcmi_esw_preemption_unimac_enable_verify_set(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_enable_verify_set
 * Purpose:
 *      set preemption verify enable or not
 *      write sw cache first, then write hw only when port link up
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_enable_verify_set(int unit, bcm_port_t port, uint32 value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_port_mac_type_get(unit, port, &mac_type));

    if (bcmiPortMacTypeXLmac == mac_type || bcmiPortMacTypeUnimac == mac_type) {
        int link_up;

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_link_status_get(unit, port, &link_up));

        if (link_up) {
            /* write it into hw only when port is link up */
            BCM_IF_ERROR_RETURN(
                _bcmi_esw_preemption_enable_verify_set(unit, port, value));
        }

        PREEMPTION_LOCK(unit);
        if (value) {
            SOC_PBMP_PORT_ADD(pbmp_verify_enable_cache[unit], port);
        } else {
            SOC_PBMP_PORT_REMOVE(pbmp_verify_enable_cache[unit], port);
        }
        PREEMPTION_UNLOCK(unit);
    } else {
       return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_mode_set
 * Purpose:
 *      set preemption mode
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_mode_set(int unit, bcm_port_t port, uint32 value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_mode_set(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_mode_set(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_verify_config_set
 * Purpose:
 *      set preemption verify config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_verify_config_set(int unit, bcm_port_t port,
                                      bcm_port_preempt_control_t type,
                                      uint32 value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_verify_config_set(unit, port, type, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_verify_config_set(unit, port, type, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_frag_config_rx_set
 * Purpose:
 *      set preemption fragment config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_frag_config_rx_set(int unit, bcm_port_t port,
                                       bcm_port_preempt_control_t type,
                                       uint32 value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_frag_config_rx_set(unit, port, type, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_frag_config_rx_set(unit, port, type, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_enable_tx_get
 * Purpose:
 *      get preemption tx enable or not
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_enable_tx_get(int unit, bcm_port_t port, uint32* value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_enable_tx_get(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_enable_tx_get(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_enable_verify_get
 * Purpose:
 *      get preemption verify enable or not
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_enable_verify_get(int unit, bcm_port_t port, uint32* value)
{
    bcmi_port_mac_type_t mac_type;

    if (NULL == value) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));

    if (bcmiPortMacTypeXLmac == mac_type || bcmiPortMacTypeUnimac == mac_type) {
        PREEMPTION_LOCK(unit);
        if (SOC_PBMP_MEMBER(pbmp_verify_enable_cache[unit], port)) {
            *value = 1;
        } else {
            *value = 0;
        }
        PREEMPTION_UNLOCK(unit);
    } else {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_mode_get
 * Purpose:
 *      get preemption mode
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_mode_get(int unit, bcm_port_t port, uint32* value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_mode_get(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_mode_get(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_verify_config_get
 * Purpose:
 *      get preemption verify config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) Preemption control type
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_verify_config_get(int unit, bcm_port_t port,
                                      bcm_port_preempt_control_t type,
                                      uint32* value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_verify_config_get(unit, port, type, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_verify_config_get(unit, port, type, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_frag_config_rx_get
 * Purpose:
 *      get preemption fragment config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) Preemption control type
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_preemption_frag_config_rx_get(int unit, bcm_port_t port,
                                       bcm_port_preempt_control_t type,
                                       uint32* value)
{
    bcmi_port_mac_type_t mac_type;

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_frag_config_rx_get(unit, port, type, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_frag_config_rx_get(unit, port, type, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_status_tx_get
 * Purpose:
 *      get preemption tx status
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_status_tx_get(int unit, bcm_port_t port,
                                  uint32* value)
{
    bcmi_port_mac_type_t mac_type;

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_preemption_status_tx_get
                            (unit, port, value)); 
        return BCM_E_NONE; 
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_status_tx_get(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_status_tx_get(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_status_verify_get
 * Purpose:
 *      get preemption verify status
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      value      - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_status_verify_get(int unit, bcm_port_t port,
                                      uint32* value)
{
    bcmi_port_mac_type_t mac_type;
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_preemption_status_verify_get
                            (unit, port, value)); 
        return BCM_E_NONE; 
    }

    BCM_IF_ERROR_RETURN(bcmi_esw_port_mac_type_get(unit, port,
                                                   &mac_type));
    if (bcmiPortMacTypeXLmac == mac_type) {
        return bcmi_esw_preemption_xlmac_status_verify_get(unit, port, value);
    } else if (bcmiPortMacTypeUnimac == mac_type) {
        return bcmi_esw_preemption_unimac_status_verify_get(unit, port, value);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_mac_config_set
 * Purpose:
 *      set preemption mac config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) Preemption control type
*      arg        - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_mac_config_set(int unit, bcm_port_t port,
                                   bcm_port_preempt_control_t type,
                                   uint32 arg)
{

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
#if defined(BCM_MONTEREY_SUPPORT)
           if (SOC_IS_MONTEREY(unit) &&
           ((type == bcmPortPreemptControlFinalFragSizeRx) ||
           (type == bcmPortPreemptControlNonFinalFragSizeRx))) {
            if ((arg < 64) || (arg > 512)) {
                return BCM_E_PARAM;
            }
            arg = (arg + 7) / 8;
        }
#endif
        BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_preemption_mac_config_set
                            (unit, port ,type, arg));
        return BCM_E_NONE;  
    }
    switch (type) {
        case bcmPortPreemptControlPreemptionSupport:
            return bcmi_esw_preemption_mode_set(unit, port, arg);

        case bcmPortPreemptControlEnableTx:
            return bcmi_esw_preemption_enable_tx_set(unit, port, arg);

        case bcmPortPreemptControlVerifyEnable:
            return bcmi_esw_preemption_enable_verify_set(unit, port, arg);

        case bcmPortPreemptControlVerifyTime:
        case bcmPortPreemptControlVerifyAttempts:
            return bcmi_esw_preemption_verify_config_set(unit, port,
                                                         type, arg);
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
            return bcmi_esw_preemption_frag_config_rx_set(unit, port,
                                                          type, arg);
        default:
            return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_mac_config_get
 * Purpose:
 *      get preemption mac config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      type       - (IN) Preemption control type
 *      arg        - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_mac_config_get(int unit, bcm_port_t port,
                                   bcm_port_preempt_control_t type,
                                   uint32* arg)
{
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_preemption_mac_config_get
                            (unit, port ,type, arg));
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit) &&
           ((type == bcmPortPreemptControlFinalFragSizeRx) ||
           (type == bcmPortPreemptControlNonFinalFragSizeRx))) {
            *arg = *arg * 8;
        }
#endif
        return BCM_E_NONE; 
    }
    switch (type) {
        case bcmPortPreemptControlPreemptionSupport:
            return bcmi_esw_preemption_mode_get(unit, port, arg);

        case bcmPortPreemptControlEnableTx:
            return bcmi_esw_preemption_enable_tx_get(unit, port, arg);

        case bcmPortPreemptControlVerifyEnable:
            return bcmi_esw_preemption_enable_verify_get(unit, port, arg);

        case bcmPortPreemptControlVerifyTime:
        case bcmPortPreemptControlVerifyAttempts:
            return bcmi_esw_preemption_verify_config_get(unit, port,
                                                         type, arg);
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
            return bcmi_esw_preemption_frag_config_rx_get(unit, port,
                                                          type, arg);
        default:
            return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_hold_request_mode_set
 * Purpose:
 *      set preemption hold request mode
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      arg        - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_hold_request_mode_set(int unit, bcm_port_t port,
                                    uint32 arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_hold_request_mode_set(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_capability_set
 * Purpose:
 *      set preemption capability
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      arg        - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_capability_set(int unit, bcm_port_t port,
                                   uint32 arg)
{
    int rv;

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_preemption_capability_set(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        rv = bcmi_mn_preemption_capability_set(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_preemption_mac_config_set(
                 unit, port,
                 bcmPortPreemptControlPreemptionSupport,
                 arg);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_preemption_frag_config_tx_set
 * Purpose:
 *      set preemption fragmentation config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      is_final   - (IN) 1: final 0:non-final
 *      arg        - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_frag_config_tx_set(int unit, bcm_port_t port,
                                       int is_final, uint32 arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_frag_config_tx_set(unit, port, is_final, arg);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        return bcmi_mn_preemption_frag_config_tx_set(unit, port, is_final, arg);
    }
#endif /* BCM_MONTEREY_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_queue_bitmap_set
 * Purpose:
 *      set preemption queue bitmap
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      arg        - (IN) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_queue_bitmap_set(int unit, bcm_port_t port,
                                     uint32 arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_queue_bitmap_set(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        return bcmi_mn_preemption_queue_bitmap_set(unit, port, arg);
    } else
#endif /* BCM_MONTEREY_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_hold_request_mode_get
 * Purpose:
 *      get preemption hold request mode
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      arg        - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_hold_request_mode_get(int unit, bcm_port_t port,
                                          uint32* arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_hold_request_mode_get(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_capability_get
 * Purpose:
 *      get preemption capability
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      arg        - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_capability_get(int unit, bcm_port_t port,
                                   uint32* arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_capability_get(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        return bcmi_mn_preemption_capability_get(unit, port, arg);
    } else
#endif /* BCM_MONTEREY_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_queue_bitmap_get
 * Purpose:
 *      get preemption queue bitmap
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      arg        - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_queue_bitmap_get(int unit, bcm_port_t port,
                                     uint32* arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_queue_bitmap_get(unit, port, arg);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        return bcmi_mn_preemption_queue_bitmap_get(unit, port, arg);
    } else
#endif /* BCM_MONTEREY_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcmi_esw_preemption_frag_config_tx_get
 * Purpose:
 *      get preemption tx fragmentation config
 * Parameters:
 *      unit       - (IN) unit number.
 *      port       - (IN) port number
 *      is_final   - (IN) 1: final 0:non-final
 *      arg        - (OUT) value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_frag_config_tx_get(int unit, bcm_port_t port,
                                       int is_final, uint32* arg)
{
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_preemption_frag_config_tx_get(unit, port, is_final, arg);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        return bcmi_mn_preemption_frag_config_tx_get(unit, port, is_final, arg);
    }
#endif /* BCM_MONTEREY_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}


void bcmi_esw_preemption_linkscan_update(int unit,
                                         bcm_port_t port,
                                         bcm_port_info_t *portInfo)
{
    int enable_in_cache;

    PREEMPTION_LOCK(unit);
    enable_in_cache = SOC_PBMP_MEMBER(pbmp_verify_enable_cache[unit], port);
    PREEMPTION_UNLOCK(unit);

     if (portInfo->linkstatus == BCM_PORT_LINK_STATUS_DOWN) {
        if (enable_in_cache) {
            int rv = _bcmi_esw_preemption_enable_verify_set(unit, port, 0);
            if (BCM_SUCCESS(rv)) {
                LOG_VERBOSE(
                    BSL_LS_BCM_PORT,
                    (BSL_META("Disable verify on port %d when link down\n"),
                    port));
            }
        }
     } else if (portInfo->linkstatus == BCM_PORT_LINK_STATUS_UP) {
        if (enable_in_cache) {
            int rv = _bcmi_esw_preemption_enable_verify_set(unit, port, 1);
            if (BCM_SUCCESS(rv)) {
                LOG_VERBOSE(
                    BSL_LS_BCM_PORT,
                    (BSL_META("re-enable verify on port %d when link up\n"),
                    port));
            }
        }
     }
}

void bcmi_esw_preemption_cleanup(int unit)
{
    if (NULL != verify_enable_cache_mutex[unit]) {
        sal_mutex_destroy(verify_enable_cache_mutex[unit]);
        verify_enable_cache_mutex[unit] = NULL;
    }
}

int bcmi_esw_preemption_init(int unit)
{
#ifdef BCM_MONTEREY_SUPPORT
    if(SOC_IS_MONTEREY(unit)) {
       BCM_IF_ERROR_RETURN(
       bcmi_mn_preemption_init(unit));
    }
#endif

    SOC_PBMP_CLEAR(pbmp_verify_enable_cache[unit]);

    verify_enable_cache_mutex[unit] = sal_mutex_create("preempt_mutex");
    if (NULL == verify_enable_cache_mutex[unit]) {
        bcmi_esw_preemption_cleanup(unit);
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int bcmi_esw_preemption_sync(int unit,
                             soc_scache_handle_t scache_handle)
{
    uint8 *scache_ptr = NULL;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, sizeof(soc_pbmp_t),
                                &scache_ptr,
                                BCM_PORT_WB_DEFAULT_VERSION, NULL));

    if (NULL == scache_ptr) {
        return BCM_E_INTERNAL;
    }
    PREEMPTION_LOCK(unit);
    sal_memcpy(scache_ptr, &pbmp_verify_enable_cache[unit], sizeof(soc_pbmp_t));
    PREEMPTION_UNLOCK(unit);

    return BCM_E_NONE;
}

int bcmi_esw_preemption_wb_alloc(int unit,
                             soc_scache_handle_t scache_handle)
{
    int rv;
    uint8 *scache_ptr = NULL;

    rv = _bcm_esw_scache_ptr_get(
            unit, scache_handle, TRUE, sizeof(soc_pbmp_t),
            &scache_ptr, BCM_PORT_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Maybe proceed with Level 1 Warm Boot */
        return BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {
        if (NULL == scache_ptr) {
            return BCM_E_INTERNAL;
        }
    }
    return rv;
}

int bcmi_esw_preemption_recover(int unit,
                                soc_scache_handle_t scache_handle)
{
    int rv;
    uint8 *scache_ptr = NULL;

    rv = _bcm_esw_scache_ptr_get(
            unit, scache_handle, FALSE, sizeof(soc_pbmp_t),
            &scache_ptr, BCM_PORT_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Maybe proceed with Level 1 Warm Boot */
        return BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {
        if (NULL == scache_ptr) {
            rv = BCM_E_INTERNAL;
        } else {
            PREEMPTION_LOCK(unit);
            sal_memcpy(&pbmp_verify_enable_cache[unit], scache_ptr,
                       sizeof(soc_pbmp_t));
            PREEMPTION_UNLOCK(unit);
        }
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


#endif /* BCM_PREEMPTION_SUPPORT */

