/*! \file bcm56990_a0_tm_ebst.c
 *
 * TM EBST chip specific functionality implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56990_a0_tm.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmbd/chip/bcm56990_a0_mmui_intr.h>
#include <bcmbd/bcmbd_ts.h>
#include "bcm56990_a0_tm_ebst.h"
#include <bcmbd/bcmbd_mmu_intr.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
#define TH4_EBST_FIFODMA_BUFFER_ENTRIES 16384
#define TH4_EBST_FIFO_CHAN0 1
#define TH4_EBST_FIFO_CHAN1 2
#define EBST_MSGQ_TIME_OUT 10000000

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief EBST timestamp configuration.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameter.
 */
static int
bcm56990_a0_tm_ebst_timestamp_init(int unit)
{
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf, fval;
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bool ts_en = false;

    SHR_FUNC_ENTER(unit);

    if (psim || asim) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, 1, &ts_en));
    if (!ts_en) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    sid = MMU_GLBCFG_MISCCONFIGr;
    fid = ENABLE_TS_TO_THDOf;
    fval = 1;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, -1, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMTM EBST chip specific port configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] port_cfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_NOT_FOUND Port not found.
 */
static int
bcm56990_a0_tm_ebst_port_config_set(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmtm_lport_t lport,
                                    bcmtm_ebst_port_cfg_t *port_cfg)
{
    bcmdrd_sid_t sid = MMU_THDO_EBST_PORT_CONFIGm;
    bcmdrd_fid_t fid = EBST_PORT_ENABLEf;
    uint32_t ltmbuf = 0;
    bcmtm_pt_info_t pt_info = {0};
    int chip_port = 0;
    uint32_t val;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mchip_port_get(unit, lport, &chip_port));

    BCMTM_PT_DYN_INFO(pt_info, chip_port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    val = port_cfg->enable;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update EBST queue configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] q_id Queue ID.
 * \param [in] q_cfg Queue configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_NOT_FOUND Logical port not found.
 */
static int
bcm56990_a0_tm_ebst_q_config_set(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmtm_lport_t lport,
                                 uint16_t q_id,
                                 bcmtm_ebst_q_cfg_t *q_cfg)
{
    int cpu_chip_pmap[] = {19, 58, 59, 99};
    uint32_t ltmbuf, fval;
    int chip_port = 0;
    bcmdrd_sid_t sid = MMU_THDO_EBST_PORT_CONFIGm;
    bcmltd_fid_t q_fid[] = {
        EBST_QUEUE_ENABLE_0f, EBST_QUEUE_ENABLE_1f,  EBST_QUEUE_ENABLE_2f,
        EBST_QUEUE_ENABLE_3f, EBST_QUEUE_ENABLE_4f,  EBST_QUEUE_ENABLE_5f,
        EBST_QUEUE_ENABLE_6f, EBST_QUEUE_ENABLE_7f,  EBST_QUEUE_ENABLE_8f,
        EBST_QUEUE_ENABLE_9f, EBST_QUEUE_ENABLE_10f, EBST_QUEUE_ENABLE_11f,
    };
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    if (bcmtm_lport_is_cpu(unit, lport)) {
        chip_port = cpu_chip_pmap[q_id / COUNTOF(q_fid)];
        q_id %= COUNTOF(q_fid);
    } else {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_mchip_port_get(unit, lport, &chip_port));
        if (q_cfg->q_type == MC_Q) {
            q_id += bcm56990_a0_tm_num_uc_q_non_cpu_port_get(unit);
        }
    }

    BCMTM_PT_DYN_INFO(pt_info, chip_port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    fval = q_cfg->enable;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, q_fid[q_id], &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST device specific scan cofigurations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] ebst_cfg EBST configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 */
static int
bcm56990_a0_tm_ebst_scan_config_set(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmtm_ebst_control_cfg_t *ebst_cfg)
{
#define TM_EBST_MAX_BUF_WIDTH   2
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[TM_EBST_MAX_BUF_WIDTH] = {0};
    bcmtm_pt_info_t pt_info = {0};
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ebst_cfg, SHR_E_PARAM);
    sid = MMU_THDO_EBST_SCAN_CONFIGr;

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);

    fid = EBST_PTR_RESETf;
    val = (ebst_cfg->enable_mode == EBST_ENABLE_MODE_OFF)? 1: 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    fid = EBST_USE_Qf;
    val = ebst_cfg->scan_mode;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    fid = EBST_SCAN_ROUNDf;
    val = ebst_cfg->scan_round;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    fid = EBST_FIFO_SCAN_AVAIL_THDf;
    val = ebst_cfg->scan_thd;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    fid = EBST_ENABLEf;
    val = (ebst_cfg->enable_mode == EBST_ENABLE_MODE_START) ? 1: 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse the EBST data based on the scan mode.
 *
 * \param [in] unit Logical unit number.
 * \param [in] entry Data entry to be parsed.
 * \param [in] scan_mode Scan mode for EBST scan.
 * \param [out] entity EBST data for the entity based on scan mode.
 * \param [out] data EBST data.
 *
 * \retval SHR_E_NONE No error.
 */
static int
bcm56990_a0_tm_ebst_data_parse(int unit,
                               uint32_t *entry,
                               bcmtm_ebst_scan_mode_t scan_mode,
                               uint32_t *entity,
                               bcmtm_ebst_data_t *data)
{
    uint32_t ts[2];
    bcmdrd_sid_t sid = MMU_THDO_EBST_POPm;
    uint32_t drop_state, type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry, BST_TIMESTAMPf, ts));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry, BST_COUNTf, &data->count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry, BST_ENTITYf, entity));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, sid,  entry, BST_TYPEf, &type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry, BST_FLAGf, &drop_state));

    data->timestamp = ((uint64_t)ts[0] << 32); /* src_hi */
    data->timestamp |= ts[1]; /* src_lo */
    /* ignoring the max bit for mc service pool. */
    if (scan_mode == EBST_SCAN_MODE_PORTSP_MC) {
        *entity &= 0x3ff;
    }
    data->type = (type == 1) ? EBST_SCAN_MODE_SP: scan_mode;
    data->red_drop_state = (drop_state & 0x1)? 1: 0;
    data->yellow_drop_state = (drop_state & 0x2)? 1: 0;
    data->green_drop_state = (drop_state & 0x4)? 1: 0;
exit:
    SHR_FUNC_EXIT();
}

static void
bcm56990_a0_tm_ebst_intr_notify(int unit, uint32_t intr_param)
{
    uint32_t intr_inst, intr_num;
    bcmtm_ebst_msgq_notif_t notif;
    int rv;
    bcmtm_ebst_dev_info_t *ebst_info;

    intr_num = MMU_INTR_NUM_GET(intr_param);
    intr_inst = MMU_INTR_INST_GET(intr_param);
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    (void) bcmbd_mmui_intr_disable(unit, intr_num);
    notif.msg = EBST_FIFO_FULL;
    notif.buffer_pool = intr_inst;

    rv = sal_msgq_post(ebst_info->msgq_intr, &notif, 0, EBST_MSGQ_TIME_OUT);
    if (rv == SAL_MSGQ_E_TIMEOUT) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "[EBST_MSG_FIFO_FULL] message timeout")));
    }
}


/*!
 * \brief EBST fifo full interrupt enable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] enable Interrupt state.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56990_a0_tm_ebst_enable(int unit,
                           bcmltd_sid_t ltid,
                           bool enable)
{
    uint32_t intr_num;
    bcmdrd_sid_t sid = MMU_GLBCFG_BST_TRACKING_ENABLEr;
    bcmdrd_fid_t fid[] = { BST_TRACK_EN_THDO0f, BST_TRACK_EN_THDO1f};
    uint32_t ltmbuf = 0, fval = 0, idx;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);
    intr_num = MMUI_INTR_THDO_EBST_FIFO_FULL;

    if (enable) {
        SHR_IF_ERR_EXIT
            (bcmtm_mmui_interrupt_enable(unit, intr_num,
                                 bcm56990_a0_tm_ebst_intr_notify, INTR_ENABLE));
    } else {
        SHR_IF_ERR_EXIT
            (bcmtm_mmui_interrupt_enable(unit, intr_num, NULL, INTR_DISABLE));
    }

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));

    fval = enable;
    for (idx = 0; idx < (sizeof(fid)/sizeof(bcmdrd_fid_t)); idx++) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid[idx], &ltmbuf, &fval));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST data map allocation.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY No memory.
 */
static int
bcm56990_a0_tm_ebst_map_alloc(int unit)
{
    int size;
    bcmtm_ebst_map_info_t *map_info;
    bcmtm_ebst_entity_map_t *map = NULL;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_portsp_map_t *portsp_map;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    map_info = &(ebst_info->map_info);

    sal_mutex_take(map_info->lock, SAL_MUTEX_FOREVER);

    /* EBST queue map. */
    size = TH4_TM_CHIP_QUEUES * sizeof(bcmtm_ebst_entity_map_t);
    map = sal_alloc(size, "bcmtmEbstQueueMap");
    SHR_NULL_CHECK(map, SHR_E_MEMORY);
    sal_memset(map, 0, size);
    map_info->q_map = map;


    /* EBST port service pool. */
    size = TH4_DEV_PORTS_PER_DEV * TH4_MAX_SERVICE_POOLS *
           (sizeof(bcmtm_ebst_portsp_map_t));
    portsp_map = sal_alloc(size, "bcmtmEbstPortServicePoolMap");
    SHR_NULL_CHECK(portsp_map, SHR_E_MEMORY);
    sal_memset(portsp_map, 0, size);
    map_info->portsp_map = portsp_map;

exit:
    sal_mutex_give(map_info->lock);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the index for mapping table for port service pools.
 * This returns the index for port service pool used for populating data table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] sp_id Service pool ID.
 * \param [out] map_idx Mapping index for port service pool map.
 *
 * \retval SHR_E_NOT_FOUND Logical port not valid.
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56990_a0_tm_ebst_portsp_index_get(int unit,
                                     bcmtm_lport_t lport,
                                     uint8_t spid,
                                     uint16_t *map_idx)
{
    int chip_port;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmtm_lport_mchip_port_get(unit, lport, &chip_port));

    *map_idx = (chip_port << 2) + spid;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the index for mapping table for queues.
 * This returns the index for queue types used for populating data table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] q_id Queue ID. (unicast queue ID or multicast queue ID).
 * \param [in] q_type Queue type.
 * \param [out] map_idx Mapping index for queue map.
 *
 * \retval SHR_E_NOT_FOUND Logical port not valid.
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56990_a0_tm_ebst_q_map_index_get(int unit,
                                    bcmtm_lport_t lport,
                                    uint16_t q_id,
                                    bcmtm_q_type_t q_type,
                                    uint16_t *map_idx)
{
    uint32_t chip_q;
    int num_ucq;

    SHR_FUNC_ENTER(unit);
    if (q_type == MC_Q) {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_ucq_get(unit, lport, &num_ucq));
        q_id += num_ucq;
    }
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_chip_q_get(unit, lport, q_id, &chip_q));
    *map_idx = chip_q;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56990_a0_tm_ebst_drv_get(int unit, void *ebst_drv)
{
    bcmtm_ebst_drv_t bcm56990_a0_tm_ebst_drv = {
        .ebst_port_config_set = bcm56990_a0_tm_ebst_port_config_set,
        .ebst_scan_config_set = bcm56990_a0_tm_ebst_scan_config_set,
        .ebst_portsp_index_get = bcm56990_a0_tm_ebst_portsp_index_get,
        .ebst_map_alloc = bcm56990_a0_tm_ebst_map_alloc,
        .ebst_q_config_set = bcm56990_a0_tm_ebst_q_config_set,
        .ebst_q_map_index_get = bcm56990_a0_tm_ebst_q_map_index_get,
        .ebst_data_parse = bcm56990_a0_tm_ebst_data_parse,
        .ebst_enable = bcm56990_a0_tm_ebst_enable,
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ebst_drv, SHR_E_INTERNAL);
    *((bcmtm_ebst_drv_t *)ebst_drv) = bcm56990_a0_tm_ebst_drv;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_ebst_init(int unit, bool warm)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    uint16_t chan_id[TH4_ITMS_PER_DEV] = {TH4_EBST_FIFO_CHAN0,
                                          TH4_EBST_FIFO_CHAN1};
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmtm_ebst_dev_info_alloc(unit));
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    /* populate ebst info */
    ebst_info->max_buffer_pool = TH4_ITMS_PER_DEV;
    ebst_info->num_entries = TH4_EBST_FIFODMA_BUFFER_ENTRIES;
    ebst_info->thd_entries = TH4_EBST_FIFODMA_BUFFER_ENTRIES / 10;
    for (idx = 0; idx < TH4_ITMS_PER_DEV; idx++) {
        ebst_info->chan[idx] = chan_id[idx];
    }
    ebst_info->fifo_sid = MMU_THDO_EBST_POPm;
    SHR_IF_ERR_EXIT(bcmtm_ebst_init(unit, warm));
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_ebst_timestamp_init(unit));
    }
exit:
    SHR_FUNC_EXIT();
}
