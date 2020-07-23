/*! \file bcm56880_a0_tm_device_rxq.c
 *
 * TM Device RX queue chip specific functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmtm/bcmtm_device_rxq_internal.h>
#include "bcm56880_a0_tm_device_rxq.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Read or write device RX queue physical table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] channel RX Channel ID.
 * \param [in] action Action to perform (1 == read and 0 == write)
 * \param [in/out] cos_bmp COS bitmap.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56880_a0_tm_rxq_bmp_pt_update(int unit,
                                 bcmltd_sid_t ltid,
                                 uint32_t channel,
                                 uint8_t action,
                                 uint64_t *cos_bmp)
{
    bcmdrd_sid_t rx_q_lo[] = {
        CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_0r, CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_0r,
        CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_0r, CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_0r,
        CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_0r, CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_0r,
        CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_0r, CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_0r,
        CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_0r, CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_0r,
        CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_0r, CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_0r,
        CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_0r, CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_0r,
        CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_0r, CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_0r,
    };
    bcmdrd_sid_t rx_q_hi[] = {
        CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_1r, CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_1r,
        CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_1r, CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_1r,
        CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_1r, CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_1r,
        CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_1r, CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_1r,
        CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_1r, CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_1r,
        CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_1r, CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_1r,
        CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_1r, CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_1r,
        CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_1r, CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_1r
    };
    uint32_t ltm_buf[3] = {0};
    uint32_t fval_lo, fval_hi;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    if (action) {
        sid = rx_q_lo[channel];
        fid = COS_BMPf;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltm_buf));
        SHR_IF_ERR_EXIT(bcmtm_field_get(unit, sid, fid, ltm_buf, &fval_lo));

        sid = rx_q_hi[channel];
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltm_buf));
        SHR_IF_ERR_EXIT(bcmtm_field_get(unit, sid, fid, ltm_buf, &fval_hi));

        *cos_bmp = ((uint64_t)fval_hi << 32);
        *cos_bmp |= fval_lo;
    } else {
        sid = rx_q_lo[channel];
        fid = COS_BMPf;
        fval_lo = (uint32_t)(*cos_bmp);
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval_lo));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltm_buf));

        sid = rx_q_hi[channel];
        fval_hi = (uint32_t)((*cos_bmp) >> 32);
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval_hi));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltm_buf));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device RX queue physical table lookup.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] ltid Logical table ID.
 * \param [in] channel RX channel for CMIC.
 * \param [out] cos_bmp COS bitmap for RX channel.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56880_a0_tm_rxq_bmp_pt_get(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t channel,
                              uint64_t *cos_bmp)
{

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_rxq_bmp_pt_update(unit, ltid, channel, 1, cos_bmp));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device RX queue physical table write.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] ltid Logical table ID.
 * \param [in] channel RX channel for CMIC.
 * \param [in] cos_bmp COS bitmap for RX channel.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56880_a0_tm_rxq_bmp_pt_set(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t channel,
                              uint64_t cos_bmp)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_rxq_bmp_pt_update(unit, ltid, channel, 0, &cos_bmp));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56880_a0_tm_rxq_drv_get(int unit, void *rxq_drv)
{
    bcmtm_device_rxq_drv_t bcm56880_a0_rxq_drv = {
        .rxq_bmp_pt_get = bcm56880_a0_tm_rxq_bmp_pt_get,
        .rxq_bmp_pt_set = bcm56880_a0_tm_rxq_bmp_pt_set
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rxq_drv, SHR_E_INTERNAL);

    *((bcmtm_device_rxq_drv_t *)rxq_drv) = bcm56880_a0_rxq_drv;
exit:
    SHR_FUNC_EXIT();
}
