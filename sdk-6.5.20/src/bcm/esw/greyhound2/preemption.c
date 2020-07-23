/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/drv.h>
#include <soc/greyhound2.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <shared/bsl.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_GREYHOUND2_SUPPORT)
STATIC int
bcmi_gh2_preemption_port_check(int unit, bcm_port_t port)
{
    if (!IS_XL_PORT(unit, port) && !IS_GE_PORT(unit, port)) {
        return BCM_E_UNAVAIL;
    } else {
        return BCM_E_NONE;
    }
}

int
bcmi_gh2_preemption_capability_set(int unit, bcm_port_t port,
                                   uint32 arg)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    int link_up;

    BCM_IF_ERROR_RETURN(
        bcm_esw_port_link_status_get(unit, port, &link_up));
    if (1 == link_up) {
        LOG_ERROR(BSL_LS_BCMAPI_PORT,
                  (BSL_META_U(unit,
                              "This needs to be setup during link down\n")));
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit,
                               MMU_PORT_PREEMPTION_CONFIGr,
                               port,
                               PREEMPTION_MODEf,
                               arg ? 1 : 0));
    BCM_IF_ERROR_RETURN(
        soc_mem_field32_modify(unit,
                               EGR_EDB_XMIT_CTRLm,
                               phy_port,
                               EP_PORT_PORT_PREEMPT_MODEf,
                               arg ? 1 : 0));
    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_queue_bitmap_set(int unit, bcm_port_t port,
                                     uint32 arg)
{
    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        soc_reg_field_validate(unit,
                               MMU_PORT_PREEMPTION_CONFIGr,
                               PREEMPTION_Q_BITMAPf,
                               arg));
    BCM_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit,
                               MMU_PORT_PREEMPTION_CONFIGr,
                               port,
                               PREEMPTION_Q_BITMAPf,
                               arg));
    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_hold_request_mode_set(int unit, bcm_port_t port,
                                          uint32 arg)
{
    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit,
                               MMU_PORT_PREEMPTION_CONFIGr,
                               port,
                               HOLD_REQ_CNT_MODEf,
                               arg ? 1 : 0));
    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_frag_config_tx_set(int unit, bcm_port_t port,
                                       int is_final,
                                       uint32 value)
{
    uint32 hw_value;
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];

    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));

    if(value > 512 || value < 64) {
        return BCM_E_PARAM;
    }
    if (IS_GE_PORT(unit, port)) {
        if (value % 16) {
           return BCM_E_PARAM;
        }
    } else if (IS_XL_PORT(unit, port)) {
        if (value % 32) {
           return BCM_E_PARAM;
        }
    } else {
        return BCM_E_INTERNAL; /* should not happen */
    }

    if (512 == value) {
        /* The size for is max(32N, hw_value*4)
         * because the there is only 7 bits in hw_value
         * set hw_value to 127 for the size 512
         */
        hw_value = 127;
    } else {
        hw_value = value / 4;
    }
    return soc_mem_field32_modify(unit, EGR_EDB_XMIT_CTRLm, phy_port,
                                  (is_final ? MIN_FINAL_FRAG_SIZEf:
                                              MIN_NON_FINAL_FRAG_SIZEf),
                                  hw_value);
}


int
bcmi_gh2_preemption_capability_get(int unit, bcm_port_t port,
                                   uint32* arg)
{
    uint32 regval;

    if (NULL == arg) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        READ_MMU_PORT_PREEMPTION_CONFIGr(unit, port,
                                         &regval));
    *arg = soc_reg_field_get(unit,
                             MMU_PORT_PREEMPTION_CONFIGr,
                             regval, PREEMPTION_MODEf);
    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_queue_bitmap_get(int unit, bcm_port_t port,
                                     uint32* arg)
{
    uint32 regval;

    if (NULL == arg) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        READ_MMU_PORT_PREEMPTION_CONFIGr(unit, port,
                                         &regval));
    *arg = soc_reg_field_get(unit,
                             MMU_PORT_PREEMPTION_CONFIGr,
                             regval, PREEMPTION_Q_BITMAPf);
    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_hold_request_mode_get(int unit, bcm_port_t port,
                                          uint32* arg)
{
    uint32 regval;

    if (NULL == arg) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        READ_MMU_PORT_PREEMPTION_CONFIGr(unit, port,
                                         &regval));
    *arg = soc_reg_field_get(unit,
                             MMU_PORT_PREEMPTION_CONFIGr,
                             regval, HOLD_REQ_CNT_MODEf);
    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_frag_config_tx_get(int unit, bcm_port_t port,
                                       int is_final,
                                       uint32* value)
{
    egr_edb_xmit_ctrl_entry_t entry;
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    uint32 hw_value, remainder, base;

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_gh2_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, EGR_EDB_XMIT_CTRLm, MEM_BLOCK_ANY,
                     phy_port, &entry));
    if (IS_GE_PORT(unit, port)) {
        base = 16;
    } else if (IS_XL_PORT(unit, port)) {
        base = 32;
    } else {
        return BCM_E_INTERNAL; /* should not happen */
    }

    /* For GE port :
     *    size is the max(16N, hw_value*4),
     *    so need to round up as multiple of 16
     * For XL port :
     *    size is the max(32N, hw_value*4),
     *    so need to round up as multiple of 32
     */
    hw_value = soc_mem_field32_get(unit,
                                   EGR_EDB_XMIT_CTRLm,
                                   &entry,
                                   (is_final ? MIN_FINAL_FRAG_SIZEf:
                                               MIN_NON_FINAL_FRAG_SIZEf));

    remainder = (hw_value * 4) % base;
    if (remainder == 0) {
        *value = hw_value * 4;
    } else {
        *value = (hw_value * 4) + base - remainder;
    }

    return BCM_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT */
