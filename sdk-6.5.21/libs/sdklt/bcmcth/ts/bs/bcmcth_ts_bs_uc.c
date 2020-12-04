/*! \file bcmcth_ts_bs_uc.c
 *
 * BS uC interface APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_util_pack.h>
#include <shr/shr_ha.h>
#include <shr/shr_types.h>
#include <shr/shr_util.h>

#include <bcmbd/bcmbd_mcs.h>
#include <bcmdrd/bcmdrd.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltd/chip/bcmltd_device_constants.h>

#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_ts_bs.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/* Get the upper 32b of a 64b */
#define BS_64_HI(_u64_) (((_u64_) >> 32) & 0xFFFFFFFF)

/* Get the lower 32b of a 64b */
#define BS_64_LO(_u64_) ((_u64_) & 0xFFFFFFFF)

/* Join 2 32b to 1 64b */
#define BS_64_GET(_hi_, _lo_) ((((uint64_t)(_hi_)) << 32) | (_lo_))

/* Current messaging version of the SDK */
#define BS_SDK_MSG_VERSION (0)

#define BS_MAX_LOG_SIZE_BYTES            (1024)

/* Time to wait for the log thread to stop. */
#define BS_LOG_THREAD_STOP_TIMEOUT_USECS (5 * 1000 * 1000)

/* Macros for passing unit + warmboot indication to the log thread */
#define BS_ENCODE_UNIT_WARM(_unit, _wb) ((void *) (uintptr_t) ((_unit << 16) | _wb))
#define BS_DECODE_UNIT(_val)       ((int) (((uintptr_t) (_val)) >> 16))
#define BS_DECODE_WB(_val)         (((uintptr_t) (_val)) & 0xffff)

#define BS_MAX_INTERFACE_COUNT (2)

static int bs_log_thread_start(int unit, bool warm);
static int bs_log_thread_stop(int unit);
static int bs_uc_msg_dbg_cfg(int unit, bool warm);
static int bs_uc_msg_sdk_deinit_notify(int unit);
static bcmcth_ts_bs_info_t *g_bs_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * uC alloc and free messages from the SYSM
 */
int
bcmcth_ts_bs_uc_alloc(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    bcmcth_ts_bs_info_t *bs_info = NULL;
    int rv;
    uint32_t log_buf_info_size=0;
    size_t num_fields;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(g_bs_info[unit], sizeof(bcmcth_ts_bs_info_t), "bcmcthTsBsInfo");
    bs_info = g_bs_info[unit];
    sal_memset(bs_info, 0, sizeof(bcmcth_ts_bs_info_t));

    /* BS control HA memory allocation. */
    ha_req_size = sizeof(bcmcth_ts_bs_control_ha_t);
    ha_alloc_size = ha_req_size;

    bs_info->ctrl = shr_ha_mem_alloc(unit, BCMMGMT_TS_COMP_ID,
            BCMTS_BS_CTRL_SUB_COMP_ID,
            "tsBsCtrl",
            &ha_alloc_size);
    SHR_NULL_CHECK(bs_info->ctrl, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(bs_info->ctrl, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_TS_COMP_ID,
                                        BCMTS_BS_CTRL_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_TS_BS_CONTROL_HA_T_ID));
    }

    if (bcmdrd_feature_is_sim(unit) == false) {

         /* BS logdata HA memory allocation. */
        ha_req_size = sizeof(bcmcth_ts_bs_log_data_ha_t);
        ha_alloc_size = ha_req_size;

        bs_info->log_data = shr_ha_mem_alloc(unit, BCMMGMT_TS_COMP_ID,
                                          BCMTS_BS_LOG_DATA_SUB_COMP_ID,
                                          "tsBsLogdata",
                                          &ha_alloc_size);
        SHR_NULL_CHECK(bs_info->log_data, SHR_E_MEMORY);

        SHR_IF_ERR_VERBOSE_EXIT
            ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

        if (!warm) {
            sal_memset(bs_info->log_data, 0, ha_alloc_size);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmissu_struct_info_report(unit, BCMMGMT_TS_COMP_ID,
                                            BCMTS_BS_LOG_DATA_SUB_COMP_ID, 0,
                                            ha_req_size, 1,
                                            BCMCTH_TS_BS_LOG_DATA_HA_T_ID));
        }

        /* Allocate DMA buffer */
        bs_info->dma_buffer = bcmdrd_hal_dma_alloc(unit,
                                                    MAX_BS_DMA_MSG_SIZE_BYTES,
                                                    "BS DMA buffer",
                                                    &(bs_info->dma_buffer_p_addr));
        SHR_NULL_CHECK(bs_info->dma_buffer, SHR_E_MEMORY);

        /* Allocate log buffer */
        log_buf_info_size = sizeof(mcs_bs_log_info_t) - sizeof(bs_info->log_buf_info->buf)
                             + BS_MAX_LOG_SIZE_BYTES;

        bs_info->log_buf_info = bcmdrd_hal_dma_alloc(unit,
                                                    log_buf_info_size,
                                                    "BS log buffer",
                                                    &(bs_info->log_buf_p_addr));
        SHR_NULL_CHECK(bs_info->log_buf_info, SHR_E_MEMORY);

        bs_info->log_buf_info->size = shr_htonl(BS_MAX_LOG_SIZE_BYTES);
        bs_info->log_buf_info->head = 0;
        bs_info->log_buf_info->head = 0;

        rv = bcmlrd_map_table_attr_get(unit, DEVICE_TS_BROADSYNC_LOGt,
                                       BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                       &(bs_info->log_table_size));
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "bcmlrd_map_table_attr_get returned %d tbl size:%d\n"),
                     rv, bs_info->log_table_size));
        }

        /* Allocate fields for log data. */
        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, DEVICE_TS_BROADSYNC_LOGt, &num_fields));
        num_fields += (DEVICE_TS_BROADSYNC_LOG_DATA_MAX_SIZE-1);
        bs_info->log_num_fields = num_fields;

        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_fields_alloc(unit, num_fields, &(bs_info->log_fields)));

        num_fields = 1;
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_fields_alloc(unit, num_fields, &(bs_info->log_status_fields)));

        if (bs_info->ctrl->init == true) {
            SHR_IF_ERR_VERBOSE_EXIT(bs_log_thread_start(unit, warm));
        }
    }

exit:

    if (SHR_FUNC_ERR()) {
        if (g_bs_info[unit] != NULL) {
            if (g_bs_info[unit]->ctrl != NULL) {
                shr_ha_mem_free(unit, g_bs_info[unit]->ctrl);
                g_bs_info[unit]->ctrl = NULL;
            }

            if (g_bs_info[unit]->log_data != NULL) {
                shr_ha_mem_free(unit, g_bs_info[unit]->log_data);
                g_bs_info[unit]->log_data = NULL;
            }

            if (g_bs_info[unit]->dma_buffer != NULL) {
                bcmdrd_hal_dma_free(unit,
                                    MAX_BS_DMA_MSG_SIZE_BYTES,
                                    g_bs_info[unit]->dma_buffer,
                                    g_bs_info[unit]->dma_buffer_p_addr);
                g_bs_info[unit]->dma_buffer = NULL;
                g_bs_info[unit]->dma_buffer_p_addr = 0;
            }

            if (g_bs_info[unit]->log_buf_info != NULL) {
                bcmdrd_hal_dma_free(unit, log_buf_info_size,
                                    g_bs_info[unit]->log_buf_info,
                                    g_bs_info[unit]->log_buf_p_addr);
                g_bs_info[unit]->log_buf_info = NULL;
                g_bs_info[unit]->log_buf_p_addr = 0;
            }

            SHR_FREE(g_bs_info[unit]);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_uc_free(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = NULL;
    uint32_t log_buf_info_size;

    SHR_FUNC_ENTER(unit);

    bs_info = g_bs_info[unit];

    if (g_bs_info[unit] == NULL) {
        SHR_EXIT();
    }

    if ((bs_info->ctrl != NULL) &&
        (bs_info->ctrl->init == true)) {
        /* Inform f/w of WB prep. */
        bs_uc_msg_sdk_deinit_notify(unit);

        /* Stop the log thread. */
        bs_log_thread_stop(unit);
    }

    bcmcth_ts_bs_fields_free(unit, bs_info->log_num_fields, &(bs_info->log_fields));
    bcmcth_ts_bs_fields_free(unit, 1, &(bs_info->log_status_fields));

    if (g_bs_info[unit]->dma_buffer != NULL) {
        bcmdrd_hal_dma_free(unit,
                MAX_BS_DMA_MSG_SIZE_BYTES,
                g_bs_info[unit]->dma_buffer,
                g_bs_info[unit]->dma_buffer_p_addr);
    }

    log_buf_info_size = sizeof(mcs_bs_log_info_t) - sizeof(bs_info->log_buf_info->buf)
                             + BS_MAX_LOG_SIZE_BYTES;

    if (g_bs_info[unit]->log_buf_info != NULL) {
        bcmdrd_hal_dma_free(unit, log_buf_info_size,
                g_bs_info[unit]->log_buf_info,
                g_bs_info[unit]->log_buf_p_addr);
    }

    SHR_FREE(g_bs_info[unit]);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BS EApp messaging private functions.
 */

static int
bs_uc_msg_appl_init(int unit, int uc)
{
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_APPL_INIT);
    MCS_MSG_LEN_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_DATA_SET(send, BS_SDK_MSG_VERSION);

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_dbg_cfg(int unit, bool warm)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    uc = bs_info->ctrl->uc_num;

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_DBG_CFG);
    MCS_MSG_LEN_SET(send, warm);
    MCS_MSG_DATA_SET(send, BS_64_LO(bs_info->log_buf_p_addr));
    MCS_MSG_DATA1_SET(send, BS_64_HI(bs_info->log_buf_p_addr));

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_sdk_deinit_notify(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;

    SHR_FUNC_ENTER(unit);

    uc = bs_info->ctrl->uc_num;

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_DEINIT_ENTRY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_adjust_cfg(int unit, time_spec_t adjust, uint32_t fid)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    uint8_t *dma_buffer = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;
    int offset=0;

    SHR_FUNC_ENTER(unit);

    uc = bs_info->ctrl->uc_num;

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    dma_buffer = bs_info->dma_buffer;
    buffer_ptr = bs_info->dma_buffer;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);

    switch (fid) {
        case DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SECf:
            SHR_UTIL_PACK_U32(buffer_ptr, (uint32_t)adjust.sign);
            SHR_UTIL_PACK_U64(buffer_ptr, adjust.sec);
            SHR_UTIL_PACK_U32(buffer_ptr, adjust.nsec);
            buffer_len = (buffer_ptr - dma_buffer);
            MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_TIME_SET);
            MCS_MSG_LEN_SET(send, buffer_len);
            MCS_MSG_DATA_SET(send, BS_64_LO(bs_info->dma_buffer_p_addr));
            MCS_MSG_DATA1_SET(send, BS_64_HI(bs_info->dma_buffer_p_addr));
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SECf:
            SHR_UTIL_PACK_U32(buffer_ptr, (uint32_t)adjust.sign);
            SHR_UTIL_PACK_U64(buffer_ptr, adjust.sec);
            SHR_UTIL_PACK_U32(buffer_ptr, adjust.nsec);
            buffer_len = (buffer_ptr - dma_buffer);
            MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_PHASE_ADJUST_SET);
            MCS_MSG_LEN_SET(send, buffer_len);
            MCS_MSG_DATA_SET(send, BS_64_LO(bs_info->dma_buffer_p_addr));
            MCS_MSG_DATA1_SET(send, BS_64_HI(bs_info->dma_buffer_p_addr));
            break;

        case DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SECf:
            MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_FREQ_ADJUST_SET);
            MCS_MSG_LEN_SET(send, 0);
            offset = (adjust.nsec*1000);
            if (adjust.sign) {
                offset = -offset;
            }
            MCS_MSG_DATA_SET(send, shr_htonl(offset));
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bcm_uc_msg_signal_cfg(int unit,
    bcmcth_ts_bs_signal_t *entry)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;

    SHR_FUNC_ENTER(unit);

    uc = bs_info->ctrl->uc_num;

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_SIGNAL_SET);
    MCS_MSG_LEN_SET(send, 0);
    MCS_MSG_DATA_SET(send, entry->signal_id);
    MCS_MSG_DATA1_SET(send, entry->frequency);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_interface_cfg(int unit, bcmcth_ts_bs_interface_t *entry)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    uint8_t *dma_buffer = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;
    int fref=0;

    SHR_FUNC_ENTER(unit);

    uc = bs_info->ctrl->uc_num;

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    dma_buffer = bs_info->dma_buffer;
    buffer_ptr = bs_info->dma_buffer;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    SHR_UTIL_PACK_U32(buffer_ptr, entry->intf_id);
    SHR_UTIL_PACK_U32(buffer_ptr, fref);
    SHR_UTIL_PACK_U32(buffer_ptr, entry->mode);
    SHR_UTIL_PACK_U32(buffer_ptr, entry->bitclk_hz);
    SHR_UTIL_PACK_U32(buffer_ptr, entry->hb_hz);
    buffer_len = (buffer_ptr - dma_buffer);

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_INTERFACE_CFG);
    MCS_MSG_LEN_SET(send, buffer_len);
    MCS_MSG_DATA_SET(send, BS_64_LO(bs_info->dma_buffer_p_addr));
    MCS_MSG_DATA1_SET(send, BS_64_HI(bs_info->dma_buffer_p_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_log_ctrl_cfg(int unit, bcmcth_ts_bs_logctrl_t *log_ctrl)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    uint8_t *dma_buffer = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0;
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    uc = bs_info->ctrl->uc_num;

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    dma_buffer = bs_info->dma_buffer;
    buffer_ptr = bs_info->dma_buffer;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    SHR_UTIL_PACK_U32(buffer_ptr, log_ctrl->debug_mask);
    SHR_UTIL_PACK_U64(buffer_ptr, log_ctrl->udp_log_mask);
    for (idx = 0; idx < SHR_MAC_ADDR_LEN; idx++) {
        SHR_UTIL_PACK_U8(buffer_ptr, log_ctrl->src_mac[idx]);
    }
    for (idx = 0; idx < SHR_MAC_ADDR_LEN; idx++) {
        SHR_UTIL_PACK_U8(buffer_ptr, log_ctrl->dst_mac[idx]);
    }
    SHR_UTIL_PACK_U16(buffer_ptr, log_ctrl->tpid);
    SHR_UTIL_PACK_U16(buffer_ptr, log_ctrl->vlan_id);
    SHR_UTIL_PACK_U8(buffer_ptr, log_ctrl->ttl);
    SHR_UTIL_PACK_U32(buffer_ptr, log_ctrl->src_ip);
    SHR_UTIL_PACK_U32(buffer_ptr, log_ctrl->dst_ip);
    SHR_UTIL_PACK_U16(buffer_ptr, log_ctrl->src_port);
    SHR_UTIL_PACK_U16(buffer_ptr, log_ctrl->dst_port);
    buffer_len = (buffer_ptr - dma_buffer);

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_LOG_SET);
    MCS_MSG_LEN_SET(send, buffer_len);
    MCS_MSG_DATA_SET(send, BS_64_LO(bs_info->dma_buffer_p_addr));
    MCS_MSG_DATA1_SET(send, BS_64_HI(bs_info->dma_buffer_p_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();

}

static int
bs_uc_msg_interface_time_get(int unit, uint32_t intf_id,
                               bcmcth_ts_bs_interface_t *entry)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    uint8_t *buffer_ptr = NULL;
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;

    SHR_FUNC_ENTER(unit);

    uc = bs_info->ctrl->uc_num;

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        entry->intf_id = intf_id;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    buffer_ptr = bs_info->dma_buffer;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_TIME_GET);
    MCS_MSG_LEN_SET(send, 0);
    MCS_MSG_DATA_SET(send, BS_64_LO(bs_info->dma_buffer_p_addr));
    MCS_MSG_DATA1_SET(send, BS_64_HI(bs_info->dma_buffer_p_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    entry->intf_id = intf_id;
    entry->time.sign = 0;

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_UTIL_UNPACK_U64(buffer_ptr, entry->time.sec);
        SHR_UTIL_UNPACK_U32(buffer_ptr, entry->time.nsec);
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_interface_status_get(int unit, uint32_t intf_id,
                               bcmcth_ts_bs_interface_status_t *entry)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;
    int uc;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    uc = bs_info->ctrl->uc_num;

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BS);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_BS_STATE_GET);
    MCS_MSG_LEN_SET(send, 0);
    MCS_MSG_DATA_SET(send, BS_64_LO(intf_id));

    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        entry->intf_id = intf_id;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BS_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BS,
                               &rcv, MCS_BS_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    entry->intf_id = intf_id;
    entry->status = MCS_MSG_DATA_GET(rcv);

 exit:
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_shutdown(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

static int
bs_uc_msg_init(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    int uc, num_uc;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (bcmdrd_feature_is_sim(unit) == true) {
        SHR_EXIT();
    }

    if (bs_info->ctrl->init) {
        SHR_IF_ERR_CONT(bs_uc_msg_shutdown(unit));
        bs_info->ctrl->init = false;
    }

    /* Init the app, determine which uC is running the app by chooosing
     * the first uC that returns sucessfully to the APPL_INIT message.
     */
    num_uc = bcmbd_mcs_num_uc(unit);
    for (uc = 0; uc < num_uc ; uc++) {
        rv = bs_uc_msg_appl_init(unit, uc);
        if (rv == SHR_E_NONE) {
            break;
        }
    }
    if (uc == num_uc) {
        /* None of the uC's responded.
         * Donot set the init flag. Just return.
         */
        SHR_EXIT();
    }

    bs_info->ctrl->uc_num = uc;
    bs_info->ctrl->init = true;
    LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, " init:%d\n"), bs_info->ctrl->init));

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BS log thread
 */

/* Log thread entry point. */
static void
bs_log_thread(shr_thread_t tc, void *arg)
{
    int unit = BS_DECODE_UNIT(arg);
    bool warm = BS_DECODE_WB(arg);
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];
    mcs_bs_log_info_t *bs_log;
    bcmltd_fields_t *fields;
    int rv=0;
    uint32_t head = 0;
    uint32_t size = 0;
    uint32_t byte_count = 0;
    uint32_t i=0;
    uint8_t ch[DEVICE_TS_BROADSYNC_LOG_DATA_MAX_SIZE] = {0};
    uint16_t last_log_entry_id;
    uint32_t gu32_retry_count=0;

    bs_log = bs_info->log_buf_info;

    while (1) {
        if (warm) {
            rv = bs_uc_msg_dbg_cfg(unit, warm);
            if (rv == SHR_E_INIT) {
                ++gu32_retry_count;
                continue;
            }
            else {
                LOG_WARN(BSL_LOG_MODULE,
                            (BSL_META_U(unit, " bs_log_thread: uc dbg cfg notified: %u. Move with log thread\n"), gu32_retry_count));
            }
            warm = false;
        }

        if (!bs_log) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, " bs_log_thread: bs_log NULL...Exiting\n")));
            break;
        }

        if (shr_thread_stopping(tc)) {
            break;
        }

        size = shr_ntohl(bs_log->size);
        head = shr_ntohl(bs_log->head);

        byte_count = 0;
        if (bs_log->tail == head) {
            continue;
        }

        while (bs_log->tail != head) {
            ch[byte_count++] = bs_log->buf[bs_log->tail++];
            if (bs_log->tail == size) {
                bs_log->tail = 0;
            }
            if (byte_count == DEVICE_TS_BROADSYNC_LOG_DATA_MAX_SIZE) {
                break;
            }
        }

        fields = &(bs_info->log_fields);

        /* Writing one entry at a time. */
        fields->field[0]->id = DEVICE_TS_BROADSYNC_LOGt_DEVICE_TS_BROADSYNC_LOG_IDf;
        fields->field[0]->data = bs_info->log_data->log_id;

        fields->field[1]->id = DEVICE_TS_BROADSYNC_LOGt_LOG_DATA_CNTf;
        fields->field[1]->data = byte_count;

        for (i=0; i<byte_count; i++)
        {
            fields->field[2+i]->id = DEVICE_TS_BROADSYNC_LOGt_LOG_DATAf;
            fields->field[2+i]->data = ch[i];
            fields->field[2+i]->idx  = i;
        }

        if (bs_info->log_data->log_id_wraparound) {
            rv = bcmimm_entry_update(unit, true, DEVICE_TS_BROADSYNC_LOGt, fields);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Entry update to DEVICE_TS_BROADSYNC_LOG tbl fail:%d \n"),
                         rv));
                return;
            }
        } else {
            rv = bcmimm_entry_insert(unit, DEVICE_TS_BROADSYNC_LOGt, fields);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Entry insert to DEVICE_TS_BROADSYNC_LOG tbl fail:%d \n"),
                         rv));
                return;
            }
        }

        last_log_entry_id = bs_info->log_data->log_id;
        bs_info->log_data->log_id = (bs_info->log_data->log_id+1) % (bs_info->log_table_size);
        if (bs_info->log_data->log_id == 0) {
            bs_info->log_data->log_id_wraparound = true;
        }

        fields = &(bs_info->log_status_fields);
        fields->field[0]->id = DEVICE_TS_BROADSYNC_LOG_STATUSt_DEVICE_TS_BROADSYNC_LOG_IDf;
        fields->field[0]->data = last_log_entry_id;

        if (bs_info->log_data->log_status_inserted == FALSE) {
            rv = bcmimm_entry_insert(unit, DEVICE_TS_BROADSYNC_LOG_STATUSt, fields);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Entry insert to DEVICE_TS_BROADSYNC_LOG_STATUS tbl fail:%d \n"),
                         rv));
                return;
            }
            bs_info->log_data->log_status_inserted = TRUE;
        } else {
            rv = bcmimm_entry_update(unit, true, DEVICE_TS_BROADSYNC_LOG_STATUSt, fields);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Entry update to DEVICE_TS_BROADSYNC_LOG_STATUS tbl fail:%d \n"),
                         rv));
                return;
            }
        }
    }
    return;
}

static int
bs_log_thread_start(int unit, bool warm)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    /* Spawn the event thread. */
    bs_info->log_thread = shr_thread_start("bcmcthTsBsLog",
                                                   SAL_THREAD_PRIO_DEFAULT,
                                                   bs_log_thread,
                                                   BS_ENCODE_UNIT_WARM(unit, warm));
    if (bs_info->log_thread == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bs_log_thread_stop(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bs_info->log_thread == NULL) {
        /* Thread is not running. */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (shr_thread_stop(bs_info->log_thread,
                         BS_LOG_THREAD_STOP_TIMEOUT_USECS));

    bs_info->log_thread = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BS LT handler APIs
 */
int
bcmcth_ts_bs_control_insert(int unit,
    bcmcth_ts_bs_control_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, " bs:%d init:%d\n"), entry->broadsync, bs_info->ctrl->init));

    if (entry->broadsync == true) {
        /* Initialize the app. */
        SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_init(unit));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, " init:%d\n"), bs_info->ctrl->init));

    /* Set the operational state. */
    if (bs_info->ctrl->init == true) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_dbg_cfg(unit, false));

        /* Spawn the log thread. */
        SHR_IF_ERR_VERBOSE_EXIT(bs_log_thread_start(unit, false));
        *state = OPERATIONAL_STATE(SUCCESS);
    } else {
        *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_control_update(int unit,
              bcmcth_ts_bs_control_t *cur,
              bcmcth_ts_bs_control_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    *state = cur->state;

    LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, " bs: %d->%d\n"), prev->broadsync, cur->broadsync));
    if (cur->broadsync != prev->broadsync) {
        if (cur->broadsync == true) {
            /* Initialize the app. */
            SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_init(unit));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, " init:%d\n"), bs_info->ctrl->init));
            if (bs_info->ctrl->init == true) {
                SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_dbg_cfg(unit, false));

                /* Spawn the log thread. */
                SHR_IF_ERR_VERBOSE_EXIT(bs_log_thread_start(unit, false));

                /* Set the operational state. */
                *state = OPERATIONAL_STATE(SUCCESS);
            } else {
                /* Set the operational state. */
                *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
            }
        } else {
            /* Shutdown requested. */
            SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_shutdown(unit));

            /* Set the operational state. */
            *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
        }

    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_control_delete(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

    /* Inform f/w of WB prep. */
    SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_sdk_deinit_notify(unit));

    /* Stop the log thread. */
    SHR_IF_ERR_VERBOSE_EXIT(bs_log_thread_stop(unit));

    /* Shutdown the app */
    SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_shutdown(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_interface_insert(int unit,
    bcmcth_ts_bs_interface_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

   /* Set the operational state. */
   *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_interface_cfg(unit, entry));

    /* Set the operational state. */
    *state = OPERATIONAL_STATE(SUCCESS);
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_interface_update(int unit,
              bcmcth_ts_bs_interface_t *cur,
              bcmcth_ts_bs_interface_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       /* Set the operational state. */
       *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
       SHR_EXIT();
    }

    if (SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SIGNf) ||
        SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SECf) ||
        SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_NSECf)) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_adjust_cfg(unit, cur->time,
                                DEVICE_TS_BROADSYNC_INTERFACEt_TIMEu_SECf));
    }

    if (SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SIGNf) ||
        SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SECf) ||
        SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_NSECf)) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_adjust_cfg(unit, cur->phase_adj,
                            DEVICE_TS_BROADSYNC_INTERFACEt_PHASE_ADJUSTu_SECf));
    }

    if (SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SIGNf) ||
        SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SECf) ||
        SHR_BITGET(cur->fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_NSECf)) {
        SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_adjust_cfg(unit, cur->freq_adj,
                            DEVICE_TS_BROADSYNC_INTERFACEt_FREQ_ADJUSTu_SECf));
    }

    /* Set the operational state. */
    *state = OPERATIONAL_STATE(SUCCESS);
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_interface_lookup(int unit,
                           uint32_t intf_id,
                           bcmcth_ts_bs_interface_t *entry)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bs_uc_msg_interface_time_get(unit, intf_id, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_interface_delete(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_signal_insert(int unit,
    bcmcth_ts_bs_signal_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
        *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_uc_msg_signal_cfg(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_signal_update(int unit,
              bcmcth_ts_bs_signal_t *cur,
              bcmcth_ts_bs_signal_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
        *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_uc_msg_signal_cfg(unit, cur));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_signal_delete(int unit, uint32_t signal)
{
    bcmcth_ts_bs_signal_t entry;
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

    entry.signal_id = signal;
    entry.frequency = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_uc_msg_signal_cfg(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_logctrl_insert(int unit,
    bcmcth_ts_bs_logctrl_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
        *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bs_uc_msg_log_ctrl_cfg(unit, entry));
    /* Set the operational state. */
    *state = OPERATIONAL_STATE(SUCCESS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_logctrl_update(int unit,
              bcmcth_ts_bs_logctrl_t *cur,
              bcmcth_ts_bs_logctrl_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
        *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bs_uc_msg_log_ctrl_cfg(unit, cur));
    /* Set the operational state. */
    *state = OPERATIONAL_STATE(SUCCESS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_logctrl_delete(int unit)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_logctrl_lookup(int unit,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *state)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
        *state = OPERATIONAL_STATE(APP_NOT_INITIALIZED);
       SHR_EXIT();
    }
    /* Set the operational state. */
    *state = OPERATIONAL_STATE(SUCCESS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_status_lookup(int unit,
                           uint32_t intf_id,
                           bcmcth_ts_bs_interface_status_t *entry)
{
    bcmcth_ts_bs_info_t *bs_info = g_bs_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((bs_info == NULL) ||
        (bs_info->ctrl == NULL) ||
        (bs_info->ctrl->init == false)) {
       SHR_EXIT();
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, " init:%d\n"), bs_info->ctrl->init));

    SHR_IF_ERR_VERBOSE_EXIT
        (bs_uc_msg_interface_status_get(unit, intf_id, entry));

exit:
    SHR_FUNC_EXIT();
}
