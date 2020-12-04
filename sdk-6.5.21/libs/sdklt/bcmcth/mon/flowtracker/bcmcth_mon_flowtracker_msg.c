/*! \file bcmcth_mon_flowtracker_msg.c
 *
 * Flowtracker message send and receive functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_ha.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd_mcs.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmbd/bcmbd_mcs.h>
#include <shr/shr_util_pack.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_int.h>
#include <bcmcth/bcmcth_mon_flowtracker_pack.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/* Get the upper 32b of a 64b */
#define INT_64_HI(_u64_) (((_u64_) >> 32) & 0xFFFFFFFF)

/* Get the lower 32b of a 64b */
#define INT_64_LO(_u64_) ((_u64_) & 0xFFFFFFFF)

/*******************************************************************************
 * Private Functions
 */
 static int
bcmcth_mon_ft_convert_uc_error(uint32_t uc_rv)
{
    int rv = SHR_E_NONE;

    switch (uc_rv) {
        case MCS_SHR_FT_UC_E_NONE:
            rv = SHR_E_NONE;
            break;
        case MCS_SHR_FT_UC_E_INTERNAL:
            rv = SHR_E_INTERNAL;
            break;
        case MCS_SHR_FT_UC_E_MEMORY:
            rv = SHR_E_MEMORY;
            break;
        case MCS_SHR_FT_UC_E_UNIT:
            rv = SHR_E_UNIT;
            break;
        case MCS_SHR_FT_UC_E_PARAM:
            rv = SHR_E_PARAM;
            break;
        case MCS_SHR_FT_UC_E_EMPTY:
            rv = SHR_E_EMPTY;
            break;
        case MCS_SHR_FT_UC_E_FULL:
            rv = SHR_E_FULL;
            break;
        case MCS_SHR_FT_UC_E_NOT_FOUND:
            rv = SHR_E_NOT_FOUND;
            break;
        case MCS_SHR_FT_UC_E_EXISTS:
            rv = SHR_E_EXISTS;
            break;
        case MCS_SHR_FT_UC_E_TIMEOUT:
            rv = SHR_E_TIMEOUT;
            break;
        case MCS_SHR_FT_UC_E_BUSY:
            rv = SHR_E_BUSY;
            break;
        case MCS_SHR_FT_UC_E_FAIL:
            rv = SHR_E_FAIL;
            break;
        case MCS_SHR_FT_UC_E_DISABLED:
            rv = SHR_E_DISABLED;
            break;
        case MCS_SHR_FT_UC_E_BADID:
            rv = SHR_E_BADID;
            break;
        case MCS_SHR_FT_UC_E_RESOURCE:
            rv = SHR_E_RESOURCE;
            break;
        case MCS_SHR_FT_UC_E_CONFIG:
            rv = SHR_E_CONFIG;
            break;
        case MCS_SHR_FT_UC_E_UNAVAIL:
            rv = SHR_E_UNAVAIL;
            break;
        case MCS_SHR_FT_UC_E_INIT:
            rv = SHR_E_INIT;
            break;
        case MCS_SHR_FT_UC_E_PORT:
            rv = SHR_E_PORT;
            break;
        default:
            rv = SHR_E_INTERNAL;
            break;
    }
    return rv;
}

static int
ft_txrx(int unit, uint8_t s_subclass, uint16_t s_len, uint64_t s_data,
        uint8_t r_subclass, uint16_t *r_len, sal_usecs_t timeout)
{
    int rv;
    mcs_msg_data_t send, reply;
    uint32_t uc_rv;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    MCS_MSG_MCLASS_SET(send,MCS_MSG_CLASS_FT);
    MCS_MSG_SUBCLASS_SET(send,s_subclass);
    MCS_MSG_LEN_SET(send,s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MCS_MSG_DMA_MSG(s_subclass) || MCS_MSG_DMA_MSG(r_subclass)) {
        MCS_MSG_DATA_SET(send, INT_64_LO(ft_info->dma_buffer_p_addr));
        MCS_MSG_DATA1_SET(send, INT_64_HI(ft_info->dma_buffer_p_addr));
    } else {
        MCS_MSG_DATA_SET(send, INT_64_LO(s_data));
        MCS_MSG_DATA1_SET(send, INT_64_HI(s_data));
    }

    rv = bcmbd_mcs_msg_send_receive(unit, ft_info->ha->uc_num,
                                    &send, &reply,
                                    timeout);
    if (rv != SHR_E_NONE){
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Convert FT uController error code to BCM */
    uc_rv  = MCS_MSG_DATA_GET(reply);
    rv = bcmcth_mon_ft_convert_uc_error(uc_rv);

    *r_len = MCS_MSG_LEN_GET(reply);

    /*Check reply class and subclass*/
    if ((rv == SHR_E_NONE) &&
        ((MCS_MSG_MCLASS_GET(reply) != MCS_MSG_CLASS_FT) ||
         (MCS_MSG_SUBCLASS_GET(reply) != r_subclass))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }


exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_uc_appl_init(int unit, int uc, uint32_t *uc_msg_version)
{
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_APPL_INIT);
    MCS_MSG_LEN_SET(send, MCS_MSG_CLASS_FT);
    MCS_MSG_DATA_SET(send, FT_MSG_VERSION);


    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        return SHR_E_INTERNAL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_FT,
                               &rcv, MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *uc_msg_version = MCS_MSG_DATA_GET(rcv);

 exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_shutdown(int unit, int uc_num)
{
    bcmbd_ukernel_info_t ukernel_info;
    uint16_t reply_len = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_uc_status(unit, uc_num, &ukernel_info));

    if (sal_strcmp(ukernel_info.status, "OK") == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ft_txrx(unit,
                     MCS_MSG_SUBCLASS_FT_SHUTDOWN,
                     0, 0,
                     MCS_MSG_SUBCLASS_FT_SHUTDOWN_REPLY,
                     &reply_len,
                     MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

        if (reply_len != 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_observation_domain_update(int unit, uint32_t obs_domain) {
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_OBS_DOM_UPDATE, 0, obs_domain,
                 MCS_MSG_SUBCLASS_FT_OBS_DOM_UPDATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_features_exchange(
    int unit,
    mcs_ft_msg_ctrl_sdk_features_t *send_msg)
{
    mcs_ft_msg_ctrl_uc_features_t  rcv_msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&rcv_msg, 0, sizeof(rcv_msg));
    ft_info = bcmcth_mon_ft_info_get(unit);
    /* Exchange supported features with the FW */
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_sdk_features_pack(buffer_req,
                                                          send_msg);
    buffer_len = buffer_ptr - buffer_req;

    rv = ft_txrx(unit,  MCS_MSG_SUBCLASS_FT_FEATURES_EXCHANGE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_FEATURES_EXCHANGE_REPLY,
                 &reply_len, MCS_SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv == SHR_E_UNAVAIL) {
        ft_info->uc_features = (MCS_SHR_FT_UC_FEATURE_IPFIX_EXPORT |
                                MCS_SHR_FT_UC_FEATURE_ELEPH);
        if (ft_info->cfg_mode != MCS_SHR_FT_CFG_MODE_V1) {
            /* Only FTv1 is supported when working with older FW */
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        SHR_EXIT();
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_uc_features_unpack(buffer_req,
                                                           &rcv_msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    ft_info->uc_features = rcv_msg.features;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_ctrl_init(int unit,
                             mcs_ft_msg_ctrl_init_t *ctrl_init_msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_init_pack(ft_info->uc_msg_version,
                                                  buffer_req, ctrl_init_msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_INIT, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_INIT_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_rxmpd_flex_format_set(int unit,
                             mcs_ft_msg_ctrl_rxpmd_flex_format_t *msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_rxpmd_flex_format_set_pack(
                                                  buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_RXPMD_FLEX_FORMAT_SET, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_RXPMD_FLEX_FORMAT_SET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_hw_learn_opt_init(int unit,
                                mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_hw_learn_opt_msg_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_HW_LEARN_OPT, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_HW_LEARN_OPT_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_em_key_format(
    int unit,
    mcs_ft_msg_ctrl_em_key_format_t *em_key_format)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_em_key_format_pack(buffer_req,
                                                           em_key_format);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_EM_KEY_FORMAT, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_EM_KEY_FORMAT_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_em_group_create(
    int unit,
    mcs_ft_msg_ctrl_em_group_create_t *msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_em_group_create_pack(buffer_req,
                                                             msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_EM_GROUP_CREATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_EM_GROUP_CREATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_flex_em_group_create(
    int unit,
    mcs_ft_msg_ctrl_flex_em_group_create_t *msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_flex_em_group_create_pack(ft_info->uc_msg_version,
                                                                  buffer_req,
                                                                  msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_FLEX_EM_GROUP_CREATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_FLEX_EM_GROUP_CREATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_em_group_delete(int unit,
                                       int em_group_id)
{
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_EM_GROUP_DELETE, em_group_id, 0,
                 MCS_MSG_SUBCLASS_FT_EM_GROUP_DELETE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_flow_group_create(
    int unit,
    mcs_ft_msg_ctrl_group_create_t *group_create_msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_group_create_pack(buffer_req,
                                                          group_create_msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_CREATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_CREATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_flow_group_delete(
    int unit,
    bcmcth_mon_flowtracker_group_t flow_group_id)
{
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_DELETE, flow_group_id, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_DELETE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_flow_group_update(
    int unit,
    mcs_ft_msg_ctrl_group_update_t *group_update_msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_group_update_pack(buffer_req,
                                                          group_update_msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_UPDATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_UPDATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
   SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_flow_group_get(
    int unit,
    bcmcth_mon_flowtracker_group_t flow_group_id,
    uint32_t *flow_count)
{
    mcs_ft_msg_ctrl_group_get_t group_get_msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&group_get_msg, 0, sizeof(group_get_msg));

    /* Do a group get for flow_group_id */
    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_GET, flow_group_id, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_GET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_group_get_unpack(buffer_req,
                                                         &group_get_msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return SHR_E_INTERNAL;
    }

    *flow_count = group_get_msg.flow_count;

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_template_create(
    int unit,
    mcs_ft_msg_ctrl_template_create_t *template_create_msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_template_create_pack(buffer_req,
            template_create_msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_TEMPLATE_CREATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_TEMPLATE_CREATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_template_delete(
    int unit,
    bcmcth_mon_flowtracker_export_template_t template_id)
{
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_TEMPLATE_DELETE, template_id, 0,
                 MCS_MSG_SUBCLASS_FT_TEMPLATE_DELETE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_collector_create(
    int unit,
    mcs_ft_msg_ctrl_collector_create_t *collector_create_msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_collector_create_pack(buffer_req,
            collector_create_msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_COLLECTOR_CREATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_COLLECTOR_CREATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_collector_delete(int unit,
                                        uint16_t collector_id)
{
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_COLLECTOR_DELETE, collector_id, 0,
                 MCS_MSG_SUBCLASS_FT_COLLECTOR_DELETE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_export_interval_update(
    int unit,
    uint32_t export_interval)
{
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_EXPORT_INTERVAL_UPDATE, 0,
                 export_interval,
                 MCS_MSG_SUBCLASS_FT_EXPORT_INTERVAL_UPDATE_REPLY,
                 &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_template_xmit_start(
    int unit,
    mcs_ft_msg_ctrl_template_xmit_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_template_xmit_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_TEMPLATE_XMIT, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_TEMPLATE_XMIT_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_group_actions_set(
    int unit,
    mcs_ft_msg_ctrl_group_actions_set_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_group_actions_set_pack(buffer_req,
                                                               msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_ACTIONS_SET, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_ACTIONS_SET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_get_flow_data(
    int unit,
    mcs_ft_msg_ctrl_group_flow_data_get_t *msg,
    mcs_ft_msg_ctrl_group_flow_data_get_reply_t *reply_msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_group_flow_data_get_pack(ft_info->uc_msg_version,
                                                                 buffer_req,
                                                                 msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_group_flow_data_get_reply_unpack
                 (buffer_req, reply_msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_mon_ft_eapp_get_first_flow_data(
    int unit,
    mcs_ft_msg_ctrl_flow_data_get_first_reply_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GET_FIRST_FLOW, 0, 0,
                 MCS_MSG_SUBCLASS_FT_GET_FIRST_FLOW_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_flow_data_get_first_reply_unpack(ft_info->uc_msg_version,
                                                                buffer_req,
                                                                msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_get_next_flow_data(
    int unit,
    uint16_t flow_index,
    mcs_ft_msg_ctrl_flow_data_get_next_reply_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GET_NEXT_FLOW, flow_index, 0,
                 MCS_MSG_SUBCLASS_FT_GET_NEXT_FLOW_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_flow_data_get_next_reply_unpack(ft_info->uc_msg_version,
                                                               buffer_req,
                                                               msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_get_first_static_flow(
    int unit,
    mcs_ft_msg_ctrl_static_flow_get_first_reply_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GET_FIRST_STATIC_FLOW, 0, 0,
                 MCS_MSG_SUBCLASS_FT_GET_FIRST_STATIC_FLOW_REPLY,
                 &reply_len, MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_static_flow_get_first_reply_unpack(ft_info->uc_msg_version,
                                                                  buffer_req,
                                                                  msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_get_next_static_flow(
    int unit,
    uint16_t flow_index,
    mcs_ft_msg_ctrl_static_flow_get_next_reply_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = NULL;
    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GET_NEXT_STATIC_FLOW, flow_index, 0,
                 MCS_MSG_SUBCLASS_FT_GET_NEXT_STATIC_FLOW_REPLY,
                 &reply_len, MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_static_flow_get_next_reply_unpack(ft_info->uc_msg_version,
                                                                 buffer_req,
                                                                 msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_eapp_group_flow_data_set(
    int unit,
    mcs_ft_msg_ctrl_group_flow_data_set_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_group_flow_data_set_pack(ft_info->uc_msg_version,
                                                        buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    /* NOTE : add a validation during LT udpate that data must be 0 */
    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_SET, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_SET_REPLY,
                 &reply_len, MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_static_flow_delete(int unit,
                                       mcs_ft_msg_ctrl_static_flow_set_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_static_flow_set_pack(ft_info->uc_msg_version,
                                                             buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_DELETE_STATIC_FLOW, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_DELETE_STATIC_FLOW_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}


int bcmcth_mon_ft_eapp_static_flow_set(int unit,
                                       mcs_ft_msg_ctrl_static_flow_set_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_static_flow_set_pack(ft_info->uc_msg_version,
                                                             buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    /* NOTE : add a validation during LT udpate that data must be 0 */
    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_SET_STATIC_FLOW, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_SET_STATIC_FLOW_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_uft_bank_info(int unit,
                                     mcs_ft_msg_ctrl_em_bank_uft_info_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_em_bank_uft_info_send_msg_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_UFT_BANK_INFO, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_UFT_BANK_INFO_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_get_learn_stats(int unit,
                                       mcs_ft_msg_ctrl_stats_learn_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_STATS_LEARN_GET, 0, 0,
                 MCS_MSG_SUBCLASS_FT_STATS_LEARN_GET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_stats_learn_get_unpack(buffer_req,
                                                               msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_get_export_stats(int unit,
                                        mcs_ft_msg_ctrl_stats_export_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_STATS_EXPORT_GET, 0, 0,
                 MCS_MSG_SUBCLASS_FT_STATS_EXPORT_GET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_stats_export_get_unpack(buffer_req,
                                                                msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_get_age_stats(int unit,
                                     mcs_ft_msg_ctrl_stats_age_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_STATS_AGE_GET, 0, 0,
                 MCS_MSG_SUBCLASS_FT_STATS_AGE_GET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_stats_age_get_unpack(buffer_req,
                                                             msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_get_elph_stats(int unit,
                                      mcs_ft_msg_ctrl_stats_elph_t *msg)
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint8_t *buffer_req = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_STATS_ELPH_GET, 0, 0,
                 MCS_MSG_SUBCLASS_FT_STATS_ELPH_GET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_stats_elph_get_unpack(buffer_req,
                                                              msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_elph_profile_create(
        int unit,
        mcs_ft_msg_ctrl_elph_profile_create_t *msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_elph_profile_create_pack(buffer_req,
            msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_CREATE, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_CREATE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    if (reply_len!= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_eapp_eleph_profile_delete(int unit, int id)
{
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_DELETE, id, 0,
                 MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_DELETE_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    if (reply_len!= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_mon_ft_match_id_set(int unit, mcs_ft_msg_ctrl_match_id_set_t *msg)
{
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_ft_info_t *ft_info = NULL;

    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = bcmcth_mon_ft_msg_ctrl_match_id_set_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    SHR_IF_ERR_EXIT
        (ft_txrx(unit, MCS_MSG_SUBCLASS_FT_MATCH_ID_SET, buffer_len, 0,
                 MCS_MSG_SUBCLASS_FT_MATCH_ID_SET_REPLY, &reply_len,
                 MCS_SHR_FT_MAX_UC_MSG_TIMEOUT));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
