/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/cosq.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident3.h>

#include <soc/trident3.h>

#if defined (INCLUDE_TCB) && defined (BCM_TCB_SUPPORT)
#include <bcm_int/esw/tcb.h>

#define _BCM_TD3_TCB_MAX_NUM    1
#define _BCM_TD3_TCB_PROFILE_MAX_NUM    8
#define _BCM_TD3_TCB_THRESHOLD_MAX      0x3ffff

#define _BCM_TD3_NUM_UCAST_QUEUE_PER_PORT 10

#define _TD3_NUM_PROFILE_COS 10

#define _TD3_TCB_UCQ_SCOPE_PIPE_OFFSET 10
#define _TD3_TCB_UCQ_SCOPE_QUEUE_MASK  0x3ff
#define _TD3_TCB_PORT_SCOPE_PIPE_OFFSET 7
#define _TD3_TCB_PORT_SCOPE_PORT_MASK   0x7f

STATIC soc_field_t _bcm_td3_cosq_tcb_threshold_profile_map_fields[] = {
    QUEUE_0f,
    QUEUE_1f,
    QUEUE_2f,
    QUEUE_3f,
    QUEUE_4f,
    QUEUE_5f,
    QUEUE_6f,
    QUEUE_7f,
    QUEUE_8f,
    QUEUE_9f
};


STATIC int
_bcm_td3_cosq_tcb_intr_enable_set(int unit, int enable)
{
    uint32 rval;

    if (SOC_REG_IS_VALID(unit, MMU_XCFG_XPE_CPU_INT_ENr)) {
        BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_ENr(unit, &rval));
        soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                          TCB_FREEZE_INT_ENf, enable);
        BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_ENr(unit, rval));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td3_cosq_tcb_intr_status_reset(int unit)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, &rval));
    soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_CLEARr, &rval,
                      TCB_FREEZE_INT_CLRf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_cosq_tcb_current_phase_get
 * Purpose:
 *      Get tcb buffer running status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      status - (OUT) TCB buffer status.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_cosq_tcb_current_phase_get(int unit, int *status)
{
    uint32 rval;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_current_phase_get: unit %d, buffer_id 0\n"),
                         unit));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, TCB_CURRENT_PHASEr, 0, 0, &rval));
    *status = soc_reg_field_get(unit,TCB_CURRENT_PHASEr, rval, CURRENT_PHASEf);

    return BCM_E_NONE;
}

STATIC int
_bcm_td3_cosq_tcb_hw_event_cb(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    bcm_cosq_buffer_id_t buffer_id;
    int status;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    if (!tcb_unit_info) {
        return 0;
    }

    _bcm_td3_cosq_tcb_intr_enable_set(unit, 0);
    _bcm_td3_cosq_tcb_intr_status_reset(unit);

    buffer_id = 0;
    /* Go over the enabled TCB to see which is freezed */
    if(tcb_unit_info->tcb_enabled_bmp & (1 << buffer_id)) {
        BCM_IF_ERROR_RETURN(
                _bcm_td3_cosq_tcb_current_phase_get(unit, &status));
        if (_TCB_FROZEN_PHASE == status) {
            if (tcb_unit_info->tcb_evt_cb) {
                tcb_unit_info->tcb_evt_cb(unit, buffer_id,
                                          tcb_unit_info->tcb_evt_user_data);
            }
        }
    }

    _bcm_td3_cosq_tcb_intr_enable_set(unit, 1);
    return BCM_E_NONE;
}

int
soc_td3_tcb_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(
            soc_td3_set_tcb_callback(unit, &_bcm_td3_cosq_tcb_hw_event_cb));
    return SOC_E_NONE;
}

STATIC int
_bcm_td3_cosq_tcb_buffer_ready_clear(int unit, bcm_cosq_buffer_id_t buffer_id)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, TCB_BUFFER_READYr, buffer_id, 0, &rval));
    soc_reg_field_set(unit, TCB_BUFFER_READYr, &rval, BUFFER_READYf, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, TCB_BUFFER_READYr, buffer_id, 0, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_td3_cosq_tcb_ext_freeze_set(int unit,
                                 bcm_cosq_buffer_id_t buffer_id, int enable)
{
    uint32 rval;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_ext_freeze_set: unit %d, buffer_id %d, enable %d\n"),
                         unit, buffer_id, enable));

    BCM_IF_ERROR_RETURN(READ_MMU_GCFG_TCB_FREEZEr(unit, &rval));

    if(enable) {
        soc_reg_field_set(unit,MMU_GCFG_TCB_FREEZEr, &rval, TCB_FREEZE_XPEf, 1);
    } else {
        soc_reg_field_set(unit,MMU_GCFG_TCB_FREEZEr, &rval, TCB_FREEZE_XPEf, 0);
    }

    BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_TCB_FREEZEr(unit, rval));

    return BCM_E_NONE;
}

int
bcm_td3_cosq_tcb_uc_queue_resolve(int unit, bcm_cosq_buffer_id_t buffer_id, int queue,
                                  bcm_gport_t *uc_gport)
{
    int pipe, id;
    bcm_port_t mmu_port, local_port, phy_port, local_mmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    pipe = queue >> _TD3_TCB_UCQ_SCOPE_PIPE_OFFSET;
    id = (queue & _TD3_TCB_UCQ_SCOPE_QUEUE_MASK) %
         _BCM_TD3_NUM_UCAST_QUEUE_PER_PORT;
    local_mmu_port = (queue & _TD3_TCB_UCQ_SCOPE_QUEUE_MASK) /
                     _BCM_TD3_NUM_UCAST_QUEUE_PER_PORT;
    mmu_port = pipe * (SOC_TD3_MMU_PORT_STRIDE + 1) + local_mmu_port;
    phy_port = si->port_m2p_mapping[mmu_port];
    local_port = si->port_p2l_mapping[phy_port];
    BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*uc_gport, local_port, id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_cosq_tcb_enable
 * Purpose:
 *      Enable or disable a TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      enable - (IN) enable or disable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_cosq_tcb_enable(int unit, bcm_cosq_buffer_id_t buffer_id, int enable)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    int mask, rv = BCM_E_NONE;
    uint64 rval64;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);
    mask = 1;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_enable: unit %d, buffer_id %d, enable %d\n"),
                         unit, buffer_id, enable));

    if (!(tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config of buffer %d is not initialized\n"),
                             buffer_id));
        return BCM_E_DISABLED;
    }

    if (enable) {
        if (tcb_unit_info->tcb_enabled_bmp & mask) {
            /* tcb is already enabled */
            rv = BCM_E_BUSY;
        } else { /* enable tcb */
            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0,
                                              &rval64));
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TCB_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0,
                                              rval64));

            if (tcb_unit_info->tcb_enabled_bmp == 0) {
                /* Enable interrupt */
                BCM_IF_ERROR_RETURN(_bcm_td3_cosq_tcb_intr_enable_set(unit, 1));
            }
            tcb_unit_info->tcb_enabled_bmp |= mask;
        }
    } else {
        if (!(tcb_unit_info->tcb_enabled_bmp & mask)) {
            /* tcb is already disabled */
            rv =  BCM_E_PARAM;
        } else { /* disable tcb */
            BCM_IF_ERROR_RETURN(_bcm_td3_cosq_tcb_ext_freeze_set(unit, buffer_id, 0));

            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0,
                                              &rval64));
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TCB_ENABLEf, 0);
            SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0,
                                              rval64));

            BCM_IF_ERROR_RETURN(_bcm_td3_cosq_tcb_buffer_ready_clear(unit, buffer_id));

            tcb_unit_info->tcb_enabled_bmp &= ~mask;
            if (tcb_unit_info->tcb_enabled_bmp == 0) {
                /* Disable interrupt */
                BCM_IF_ERROR_RETURN(_bcm_td3_cosq_tcb_intr_enable_set(unit, 0));
            }
        }
    }

    return rv;

}

/*
 * Function:
 *      bcm_td3_cosq_tcb_freeze
 * Purpose:
 *      External freeze a TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      enable - (IN) enable or disable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_cosq_tcb_freeze(int unit, bcm_cosq_buffer_id_t buffer_id, int enable)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_freeze: unit %d, buffer_id %d, enable %d\n"),
                         unit, buffer_id, enable));

    if (buffer_id == -1) {
        buffer_id = 0;
    }

    TCB_ID_CHECK(buffer_id);

    if (!tcb_unit_info->tcb_enabled_bmp) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is disabled, can not freeze\n"),
                             buffer_id));
        return BCM_E_DISABLED;
    }

    if (0 != enable) {
        BCM_IF_ERROR_RETURN(
                _bcm_td3_cosq_tcb_ext_freeze_set(unit, buffer_id, 1));
    } else {
        BCM_IF_ERROR_RETURN(
                _bcm_td3_cosq_tcb_ext_freeze_set(unit, buffer_id, 0));
        BCM_IF_ERROR_RETURN(
                _bcm_td3_cosq_tcb_buffer_ready_clear(unit, buffer_id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_cosq_tcb_deinit
 * Purpose:
 *      Deinitialize all TCB internal structure.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_cosq_tcb_deinit(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_info;
    int i = 0;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    if (tcb_unit_info == NULL) {
        return BCM_E_NONE;
    }

    if (NULL != tcb_unit_info->tcb_info[i]) {
        tcb_info = tcb_unit_info->tcb_info[i];
        if (NULL != tcb_info->profile_bmp) {
            sal_free(tcb_info->profile_bmp);
            tcb_info->profile_bmp = NULL;
        }
        sal_free(tcb_unit_info->tcb_info[i]);
        tcb_unit_info->tcb_info[i] = NULL;
    }

    if (NULL != tcb_unit_info->tcb_evt_cb) {
        tcb_unit_info->tcb_evt_cb = NULL;
    }

    if (NULL != tcb_unit_info->tcb_evt_user_data) {
        tcb_unit_info->tcb_evt_user_data = NULL;
    }

    if (NULL != tcb_unit_info->tcb_reslock) {
        sal_mutex_destroy(tcb_unit_info->tcb_reslock);
        tcb_unit_info->tcb_reslock = NULL;
    }

    sal_free(tcb_unit_info);

    TCB_UNIT_CONTROL(unit) = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_cosq_tcb_config_get
 * Purpose:
 *      Get TCB instance control status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_cosq_tcb_config_get(int unit, bcm_cosq_buffer_id_t buffer_id,
                                    bcm_cosq_tcb_config_t *config)
{
    uint64 rval64;
    uint32 trigger, timer;
    int type, entity_num, propability;
    bcm_port_t mmu_port, local_port, phy_port;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    soc_info_t *si;
    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_config_get: unit %d, buffer_id %d\n"),
                         unit, buffer_id));

    TCB_ID_CHECK(buffer_id);
    if (NULL == config) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "config is null!\n")));
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    /* check TCB instance status */
    if (!(tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
        config->scope_type = bcmCosqTcbScopeMaxCount;
        config->gport = BCM_GPORT_INVALID;
        config->cosq = -1;
        config->trigger_reason = BCM_COSQ_TCB_INGRESS_ADMIN_DROP |
                                 BCM_COSQ_TCB_EGRESS_ADMIN_DROP  |
                                 BCM_COSQ_TCB_CFAP_DROP          |
                                 BCM_COSQ_TCB_WRED_DROP;
        config->pre_freeze_capture_num = _TCB_CAP_NUM_DEFAULT;
        config->pre_freeze_capture_time = _TCB_CAP_TIM_DEFAULT * 10 / si->frequency;
        config->pre_sample_probability = _TCB_PRE_SAMP_RATE_DEFAULT + 1;
        config->post_sample_probability = _TCB_POST_SAMP_RATE_DEFAULT + 1;

        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0, &rval64));

    type = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, SCOPE_ENTITY_TYPEf);

    switch(type) {
        case _TCB_SCOPE_UCQ:
            config->scope_type = bcmCosqTcbScopeUcastQueue;
            entity_num = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                               SCOPE_ENTITYf);
            BCM_IF_ERROR_RETURN
                (_bcm_cosq_tcb_uc_queue_resolve(unit, buffer_id, entity_num,
                                                &config->gport));
            break;
        case _TCB_SCOPE_ING_PORT:
            config->scope_type = bcmCosqTcbScopeIngressPort;
            entity_num = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                               SCOPE_ENTITYf);
            mmu_port = entity_num;
            phy_port = si->port_m2p_mapping[mmu_port];
            local_port = si->port_p2l_mapping[phy_port];
            BCM_GPORT_LOCAL_SET(config->gport, local_port);
            break;
        case _TCB_SCOPE_EGR_PORT:
            config->scope_type = bcmCosqTcbScopeEgressPort;
            entity_num = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                               SCOPE_ENTITYf);
            mmu_port = entity_num;
            phy_port = si->port_m2p_mapping[mmu_port];
            local_port = si->port_p2l_mapping[phy_port];
            BCM_GPORT_LOCAL_SET(config->gport, local_port);
            break;
        case _TCB_SCOPE_ING_PIPE:
            config->scope_type = bcmCosqTcbScopeIngressPipe;
            config->pipe_id = soc_reg64_field32_get(
                unit, TCB_CONFIGr, rval64, SCOPE_ENTITYf);
            break;
        case _TCB_SCOPE_EGR_PIPE:
            config->scope_type = bcmCosqTcbScopeEgressPipe;
            config->pipe_id = soc_reg64_field32_get(
                unit, TCB_CONFIGr, rval64, SCOPE_ENTITYf);
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit,
                                  "scope type %d is illegal\n"),
                                  type));
            return BCM_E_PARAM;
    }

    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_IADMN_DROPf);
    config->trigger_reason = trigger << _SHR_TCB_TRACK_IADMN_DROP_SHIFT;
    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_EADMN_DROPf);
    config->trigger_reason |= trigger << _SHR_TCB_TRACK_EADMN_DROP_SHIFT;
    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_CFAP_DROPf);
    config->trigger_reason |= trigger << _SHR_TCB_TRACK_CFAP_DROP_SHIFT;
    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_WRED_DROPf);
    config->trigger_reason |= trigger << _SHR_TCB_TRACK_WRED_DROP_SHIFT;

    config->pre_freeze_capture_num = soc_reg64_field32_get(unit, TCB_CONFIGr,
                                                           rval64, REV_COUNTERf);
    timer = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                                WD_TIMER_PRESETf);
    config->pre_freeze_capture_time =  timer * 10 / si->frequency;
    propability = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                        PRE_TRIGGER_SAMPLE_RATEf);
    config->pre_sample_probability = propability + 1;
    propability = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                        POST_TRIGGER_SAMPLE_RATEf);
    config->post_sample_probability = propability + 1;

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_td3_cosq_tcb_config_set
 * Purpose:
 *      Set TCB instance config attributes.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (IN) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_cosq_tcb_config_set(int unit, bcm_cosq_buffer_id_t buffer_id,
                            bcm_cosq_tcb_config_t *config)
{
    uint64 rval64 = COMPILER_64_INIT(0,0);
    int timer;
    bcm_port_t local_port;
    bcm_gport_t gport;
    int mmu_port, phy_port, q_num, pipe, arg = 0, cosq, modid, trunk_id;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    soc_info_t *si;
    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_config_set: unit %d, buffer_id %d\n"),
                         unit, buffer_id));

    TCB_ID_CHECK(buffer_id);
    if (NULL == config) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "config is null!\n")));
        return BCM_E_PARAM;
    }

    if (tcb_unit_info->tcb_enabled_bmp) {
        /* tcb can only be configured in idle state */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is enabled\n"),
                             buffer_id));
        return BCM_E_BUSY;
    }

    gport = config->gport;
    si = &SOC_INFO(unit);

    /* Reset default value */
    if ((config->scope_type == bcmCosqTcbScopeMaxCount) &&
        (config->gport == BCM_GPORT_INVALID) &&
        (config->cosq == -1) &&
        (config->trigger_reason == (BCM_COSQ_TCB_INGRESS_ADMIN_DROP |
                                    BCM_COSQ_TCB_EGRESS_ADMIN_DROP  |
                                    BCM_COSQ_TCB_CFAP_DROP          |
                                    BCM_COSQ_TCB_WRED_DROP)) &&
        (config->pre_freeze_capture_num == _TCB_CAP_NUM_DEFAULT) &&
        (config->pre_freeze_capture_time ==
                                _TCB_CAP_TIM_DEFAULT * 10 / si->frequency) &&
        (config->pre_sample_probability == _TCB_PRE_SAMP_RATE_DEFAULT + 1) &&
        (config->post_sample_probability == _TCB_POST_SAMP_RATE_DEFAULT + 1)) {

        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0, rval64));
        /* update TCB instance status */
        if ((tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
            tcb_unit_info->tcb_config_init_bmp &= ~(1 << buffer_id);
        }
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0, &rval64));

    switch(config->scope_type) {
        case bcmCosqTcbScopeUcastQueue:
            if (!BCM_GPORT_IS_SET(gport)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PARAM;
            }
            if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(_bcm_td3_cosq_index_resolve(unit, gport, 0,
                                    _BCM_TD3_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                                    &cosq, NULL));
            BCM_IF_ERROR_RETURN(_bcm_td3_cosq_port_resolve(unit, gport, &modid,
                                    &local_port, &trunk_id, NULL, NULL));

            SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

            q_num = (pipe << _TD3_TCB_UCQ_SCOPE_PIPE_OFFSET) + cosq;

            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                                  _TCB_SCOPE_UCQ);
            arg = q_num;
            break;
        case bcmCosqTcbScopeIngressPort:
            if (BCM_GPORT_IS_SET(gport)){
                BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport,
                                                           &local_port));
            } else if (SOC_PORT_VALID(unit, gport)) {
                local_port = gport;
            } else {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            phy_port = si->port_l2p_mapping[local_port];
            mmu_port = si->port_p2m_mapping[phy_port];

            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                              _TCB_SCOPE_ING_PORT);
            arg = mmu_port;
            break;
        case bcmCosqTcbScopeEgressPort:
            if (BCM_GPORT_IS_SET(gport)){
                BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport,
                                                           &local_port));
            } else if (SOC_PORT_VALID(unit, gport)) {
                local_port = gport;
            } else {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            phy_port = si->port_l2p_mapping[local_port];
            mmu_port = si->port_p2m_mapping[phy_port];

            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                                  _TCB_SCOPE_EGR_PORT);
            arg = mmu_port;
            break;
        case bcmCosqTcbScopeIngressPipe:
            if ((config->pipe_id < 0) || (config->pipe_id >= si->num_pipe)) {
                return BCM_E_PARAM;
            }
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                                  _TCB_SCOPE_ING_PIPE);
            arg = config->pipe_id;
            break;
        case bcmCosqTcbScopeEgressPipe:
            if ((config->pipe_id < 0) || (config->pipe_id >= si->num_pipe)) {
                return BCM_E_PARAM;
            }
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                                  _TCB_SCOPE_EGR_PIPE);
            arg = config->pipe_id;
            break;
        default:
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb config set, scope type %d is illegal\n"),
                                 config->scope_type));
            return BCM_E_PARAM;
    }

    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITYf, arg);

    if (config->trigger_reason > _TCB_TRIGGER_MAX) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, trigger reason %d is illegal\n"),
                             config->trigger_reason));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_IADMN_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_INGRESS_ADMIN_DROP) ? 1:0);
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_EADMN_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_EGRESS_ADMIN_DROP) ? 1:0);
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_CFAP_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_CFAP_DROP) ? 1:0);
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_WRED_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_WRED_DROP) ? 1:0);

    /* rev_counter valid range is 2 - 1024 */
    if ((config->pre_freeze_capture_num < _TCB_CAP_NUM_MIN) ||
        (config->pre_freeze_capture_num > _TCB_CAP_NUM_MAX)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, pre freeze capture %d is illegal\n"),
                             config->pre_freeze_capture_num));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, REV_COUNTERf,
                          config->pre_freeze_capture_num);

    /* WD_TIMER_PRESET is a counter of clock cycles * 10, a value 1 means 10 clock cycles */
    timer = (config->pre_freeze_capture_time) * (si->frequency) / 10;
    if (timer > _TCB_CAP_TIM_MAX) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, pre freeze capture time %d is illegal\n"),
                             config->pre_freeze_capture_time));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, WD_TIMER_PRESETf,
                          timer);

    if (config->post_sample_probability > _TCB_POST_SAMP_RATE_MAX ||
        config->post_sample_probability <= 0) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, post sample probability %d is illegal\n"),
                             config->post_sample_probability));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, POST_TRIGGER_SAMPLE_RATEf,
                          config->post_sample_probability - 1);

    if (config->pre_sample_probability > _TCB_PRE_SAMP_RATE_MAX ||
        config->pre_sample_probability <= 0) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, pre sample probability %d is illegal\n"),
                             config->pre_sample_probability));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, PRE_TRIGGER_SAMPLE_RATEf,
                          config->pre_sample_probability - 1);

    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0, rval64));

    /* update TCB instance status */
    if (!(tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
        tcb_unit_info->tcb_config_init_bmp |= (1 << buffer_id);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_cosq_tcb_gport_threshold_mapping_set
 * Purpose:
 *      Set threshold profile for a UCQ or a port.
 * Parameters:
 *      unit - (IN) unit number
 *      gport - (IN) Ucq or a port
 *      cosq  - (IN) Reserved field
 *      buffer_id - (IN) TCB buffer id.
 *      index - (IN) Threshold profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When gport is a port, SDK will attach all Ucqs in this port to one threshold profile.
 */
int
bcm_td3_cosq_tcb_gport_threshold_mapping_set(int unit,
                                      bcm_gport_t gport, bcm_cos_queue_t cosq,
                                      bcm_cosq_buffer_id_t buffer_id,
                                      int index)
{
    soc_mem_t profile_mem;
    soc_field_t field;
    bcm_port_t local_port;
    int mmu_port, phy_port, pipe, i, port_index, old_profile;
    bcm_cos_t cos;
    soc_info_t *si;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    uint32 entry[SOC_MAX_MEM_WORDS];

    si = &SOC_INFO(unit);
    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_mapping_set: unit %d, gport 0x%x, "
                         "cosq %d, buffer_id %d, index %d\n"),
                         unit, gport, cosq, buffer_id, index));

    if (tcb_unit_info->tcb_enabled_bmp) {
        /* tcb can only be configured in idle state */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is enabled\n"),
                             buffer_id));
        return BCM_E_BUSY;
    }

    tcb_config = tcb_unit_info->tcb_info[buffer_id];
    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(_bcm_td3_cosq_index_resolve(unit, gport, 0,
                                    _BCM_TD3_COSQ_INDEX_STYLE_COS, &local_port,
                                    &cos, NULL));
        } else if (BCM_GPORT_IS_LOCAL(gport)) {
            local_port = BCM_GPORT_LOCAL_GET(gport);
            cos = -1;
        } else {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb_threshold_mapping_set, gport 0x%x is illegal\n"),
                                 gport));
            return BCM_E_PARAM;
        }
    } else {
        local_port = gport;
        cos = -1;
    }

    if (!SOC_PORT_VALID(unit, local_port) || IS_CPU_PORT(unit, local_port) ||
        IS_LB_PORT(unit, local_port)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb_threshold_mapping_set, gport 0x%x is illegal\n"),
                             gport));
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[local_port];
    port_index = mmu_port % 66 + pipe * 66;


    if (!SHR_BITGET(tcb_config->profile_bmp, index)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "profile %d is illegal\n"),
                              index));
        return BCM_E_NOT_FOUND;
    }

    if (-1 == cos) {
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                          port_index, entry));
        for (i = 0; i < _TD3_NUM_PROFILE_COS; i++) {
            field = _bcm_td3_cosq_tcb_threshold_profile_map_fields[i];
            old_profile = soc_mem_field32_get(unit, TCB_THRESHOLD_PROFILE_MAPm,
                                              entry, field);
            tcb_config->profile_ref_count[old_profile]--;
            soc_mem_field32_set(unit, TCB_THRESHOLD_PROFILE_MAPm, entry, field,
                                index);
            tcb_config->profile_ref_count[index]++;
        }
        BCM_IF_ERROR_RETURN (soc_mem_write(unit, profile_mem, MEM_BLOCK_ALL,
                                           port_index, entry));
    } else {
        field = _bcm_td3_cosq_tcb_threshold_profile_map_fields[cos];
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                          port_index, entry));
        old_profile = soc_mem_field32_get(unit, TCB_THRESHOLD_PROFILE_MAPm,
                                          entry, field);
        tcb_config->profile_ref_count[old_profile]--;

        soc_mem_field32_set(unit, TCB_THRESHOLD_PROFILE_MAPm, entry,
                            field, index);
        tcb_config->profile_ref_count[index]++;
        BCM_IF_ERROR_RETURN (soc_mem_write(unit, profile_mem, MEM_BLOCK_ALL,
                                           port_index, entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_cosq_tcb_gport_threshold_mapping_get
 * Purpose:
 *      Set threshold profile for a UCQ or a port.
 * Parameters:
 *      unit - (IN) unit number
 *      gport - (IN) Ucq or a port
 *      cosq  - (IN) Reserved field
 *      buffer_id - (IN) TCB buffer id.
 *      index - (OUT) Threshold profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When gport is a port, SDK will return the threshold profile index of the first Ucq in this
 *      port.
 */
int
bcm_td3_cosq_tcb_gport_threshold_mapping_get(int unit,
                                     bcm_gport_t gport, bcm_cos_queue_t cosq,
                                     bcm_cosq_buffer_id_t buffer_id, int *index)
{
    soc_mem_t profile_mem;
    soc_field_t field;
    bcm_port_t local_port;
    int mmu_port, phy_port, pipe, port_index, cos;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    soc_info_t *si;
    uint32 entry[SOC_MAX_MEM_WORDS];

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_mapping_get: unit %d, gport 0x%x, "
                         "cosq %d, buffer_id %d\n"),
                         unit, gport, cosq, buffer_id));

    TCB_ID_CHECK(buffer_id);
    if (NULL == index) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "index is null!\n")));
        return BCM_E_PARAM;
    }

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    si = &SOC_INFO(unit);

    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(_bcm_td3_cosq_index_resolve(unit, gport, 0,
                                    _BCM_TD3_COSQ_INDEX_STYLE_COS, &local_port,
                                    &cos, NULL));
        } else if (BCM_GPORT_IS_LOCAL(gport)) {
            local_port = BCM_GPORT_LOCAL_GET(gport);
            cos = 0;
        } else {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb_threshold_mapping_get, gport 0x%x is illegal\n"),
                                 gport));
            return BCM_E_PARAM;
        }
    } else {
        local_port = gport;
        cos = 0;
    }

    if (!SOC_PORT_VALID(unit, local_port) || IS_CPU_PORT(unit, local_port) ||
        IS_LB_PORT(unit, local_port)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb_threshold_mapping_get, gport 0x%x is illegal\n"),
                             gport));
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[local_port];
    port_index = mmu_port % 66 + pipe * 66;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                     port_index, entry));

    field = _bcm_td3_cosq_tcb_threshold_profile_map_fields[cos];
    *index = soc_mem_field32_get(unit,TCB_THRESHOLD_PROFILE_MAPm, entry, field);

    tcb_config = tcb_unit_info->tcb_info[buffer_id];
    if (!SHR_BITGET(tcb_config->profile_bmp, *index)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "profile %d is illegal\n"),
                              *index));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;

}

#ifdef BCM_WARM_BOOT_SUPPORT
int
bcm_td3_cosq_tcb_reinit(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    int i = 0, j, k, n, enable, index, start_threshold, stop_threshold;
    uint64 rval64;
    soc_mem_t profile_mem;
    soc_field_t field;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_INIT(unit);

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, i, 0, &rval64));
    enable = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TCB_ENABLEf);
    if (enable) {
        tcb_unit_info->tcb_enabled_bmp |= 1 << i;
    }

    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[i];
    tcb_config = tcb_unit_info->tcb_info[i];
    SHR_BITSET(tcb_config->profile_bmp, 0);

    for (n = 1; n < _BCM_TD3_TCB_PROFILE_MAX_NUM; n++) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_THRESHOLD_PROFILEr, i,
                                          n, &rval64));
        start_threshold = soc_reg_field_get(unit, TCB_THRESHOLD_PROFILEr,
                                            rval64, CAPTURE_START_THRESHOLDf);
        stop_threshold = soc_reg_field_get(unit, TCB_THRESHOLD_PROFILEr,
                                           rval64, CAPTURE_END_THRESHOLDf);
        if (start_threshold != 0 || stop_threshold != 0) {
            SHR_BITSET(tcb_config->profile_bmp, n);
        }

    }
    for (j = 0; j < soc_mem_index_count(unit, profile_mem); j++) {
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                          j, &rval64));
        for (k = 0; k < _TD3_NUM_PROFILE_COS; k++) {
            field = _bcm_td3_cosq_tcb_threshold_profile_map_fields[k];
            index = soc_mem_field32_get(unit,TCB_THRESHOLD_PROFILE_MAPm,
                                        &rval64, field);
            for (n = 0; n < _BCM_TD3_TCB_PROFILE_MAX_NUM; n++) {
                if (index == n) {
                    if(SHR_BITGET(tcb_config->profile_bmp, n)) {
                    tcb_config->profile_ref_count[n]++;
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_td3_cosq_tcb_init
 * Purpose:
 *      Initialize (clear) all TCB internal structure.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_cosq_tcb_init(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_info;
    bcm_cosq_buffer_id_t buffer_id = 0;
    soc_mem_t profile_mem;

    (void)_bcm_td3_cosq_tcb_deinit(unit);

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    if (NULL == tcb_unit_info) {
        tcb_unit_info = sal_alloc(sizeof(_bcm_cosq_tcb_unit_ctrl_t), "tcb_dev_info");
        if (!tcb_unit_info) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(tcb_unit_info, 0, sizeof(_bcm_cosq_tcb_unit_ctrl_t));

    TCB_UNIT_CONTROL(unit) = tcb_unit_info;

    tcb_unit_info->tcb_num = _BCM_TD3_TCB_MAX_NUM;
    tcb_unit_info->cell_bytes = _TD3_MMU_BYTES_PER_CELL;
    tcb_unit_info->threshold_max = _BCM_TD3_TCB_THRESHOLD_MAX;

    /* Create protection mutex. */
    if (tcb_unit_info->tcb_reslock == NULL) {
        tcb_unit_info->tcb_reslock = sal_mutex_create("tcb_resource_lock");
    }

    if (tcb_unit_info->tcb_reslock == NULL) {
        (void)_bcm_td3_cosq_tcb_deinit(unit);
        return BCM_E_MEMORY;
    }

    tcb_unit_info->tcb_info[buffer_id] =
            sal_alloc(sizeof(_bcm_cosq_tcb_info_t), "tcb info");
    if (NULL == tcb_unit_info->tcb_info[buffer_id]) {
        (void)_bcm_td3_cosq_tcb_deinit(unit);
        return BCM_E_MEMORY;
    }

    sal_memset(tcb_unit_info->tcb_info[buffer_id], 0, sizeof(_bcm_cosq_tcb_info_t));

    tcb_info = tcb_unit_info->tcb_info[buffer_id];
    tcb_info->profile_bmp =
            sal_alloc(SHR_BITALLOCSIZE(_BCM_TD3_TCB_PROFILE_MAX_NUM),
                      "profile_bitmap");
    if (NULL == tcb_info->profile_bmp) {
        (void)_bcm_td3_cosq_tcb_deinit(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(tcb_info->profile_bmp, 0,
               SHR_BITALLOCSIZE(_BCM_TD3_TCB_PROFILE_MAX_NUM));

    if (!SOC_WARM_BOOT(unit)) {
        SHR_BITSET(tcb_info->profile_bmp, 0);
        profile_mem =
            SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];
        tcb_info = tcb_unit_info->tcb_info[buffer_id];
        tcb_info->profile_ref_count[0] =
                    soc_mem_index_count(unit, profile_mem) * 10;
    }

    soc_td3_tcb_set_hw_intr_cb(unit);

    return BCM_E_NONE;
}

#endif /* INCLUDE_TCB && BCM_TCB_SUPPORT */
