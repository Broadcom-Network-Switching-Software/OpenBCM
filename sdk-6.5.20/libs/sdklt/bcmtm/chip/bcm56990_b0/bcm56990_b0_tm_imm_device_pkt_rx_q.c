/*! \file bcm56990_b0_tm_imm_device_pkt_rx_q.c
 *
 * TM DEVICE RX queue imm function handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include "bcm56990_b0_tm_imm_internal.h"
#include <bcmtm/bcmtm_pt_internal.h>

/* Number of CPU COS */
#define MAX_CPU_COS     48
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

/* RX queue COS map low 32 bits configuration registers */
static bcmdrd_sid_t rx_q_low[] = {
    CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_0r,
    CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_0r,
    CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_0r,
};

/* RX queue COS map high 16 bits configuration registers */
static bcmdrd_sid_t rx_q_high[] = {
    CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_1r,
    CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_1r,
    CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_1r,
};


/*!
 * \brief
 *
 * Function to read DEVICE RX queue.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] channel RX channel
 * \param [out] cos_bmp COS bitmap
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56990_b0_tm_device_rx_q_pt_get(int unit, bcmltd_sid_t ltid, uint32_t channel,
                                  uint64_t *cos_bmp)
{
    uint32_t ltm_buf[3] = {0};
    uint32_t fval_lo, fval_hi;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);

    fid = COS_BMPf;

    /* Fetch low 32 bits */
    sid = rx_q_low[channel];
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltm_buf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltm_buf, &fval_lo));

    /* Fetch high 16 bits */
    sid = rx_q_high[channel];
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltm_buf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltm_buf, &fval_hi));

    *cos_bmp = fval_hi;
    *cos_bmp = (*cos_bmp << 32) | fval_lo;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief
 *
 * Function implementing PT write for DEVICE RX queue.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] channel RX channel
 * \param [in] cos_bmp COS bitmap
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56990_b0_tm_device_rx_q_pt_set(int unit, bcmltd_sid_t ltid,
                                  uint32_t channel, uint64_t cos_bmp)
{
    uint32_t ltm_buf[3] = {0};
    uint32_t fval;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);

    fid = COS_BMPf;

    /* Configure low 32 bits */
    sid = rx_q_low[channel];
    fval = (uint32_t)cos_bmp;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltm_buf));

    /* Configure high 16 bits */
    sid = rx_q_high[channel];
    fval = (uint32_t)(cos_bmp >> 32);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltm_buf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE RX queue IMM table change callback function.
 *
 * Handle DEVICE RX queue IMM table change.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] action Action on the entry.
 * \param [in] key Key fields identifying the entry.
 * \param [in] data Config data.
 * \param [in] context Callback user data.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56990_b0_bcmtm_device_rx_q_imm_stage_callback(int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t action,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    uint32_t channel;
    BCMLTD_BITDCL cos_bmap;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }

    if (key->id != DEVICE_PKT_RX_Qt_RX_Qf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    channel = (uint32_t)key->data;

    cos_bmap = 0;
    if (action == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (bcm56990_b0_tm_device_rx_q_pt_get(unit, sid, channel, &cos_bmap));
    }

    if (action == BCMIMM_ENTRY_INSERT || action == BCMIMM_ENTRY_UPDATE) {
        while (data) {
            if (data->id != DEVICE_PKT_RX_Qt_COSf) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
            if (data->data) {
                BCMLTD_BIT_SET(&cos_bmap, data->idx);
            } else {
                BCMLTD_BIT_CLR(&cos_bmap, data->idx);
            }
            data = data->next;
        }
    } else if (action != BCMIMM_ENTRY_DELETE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcm56990_b0_tm_device_rx_q_pt_set(unit, sid, channel, cos_bmap));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56990_b0_tm_device_imm_rx_q_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    bcmimm_lt_cb_t cpu_cos_imm_callback;
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cpu_cos_imm_callback, 0, sizeof(bcmimm_lt_cb_t));
    cpu_cos_imm_callback.stage = bcm56990_b0_bcmtm_device_rx_q_imm_stage_callback;

    rv = bcmlrd_map_get(unit, DEVICE_PKT_RX_Qt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, DEVICE_PKT_RX_Qt, &cpu_cos_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
