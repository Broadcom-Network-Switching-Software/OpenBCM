/*! \file bcmtm_imm_device_pkt_rxq.c
 *
 * TM Device RX Queue configuration IMM Handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_device_rxq_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmlrd/bcmlrd_map.h>
/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
static int
bcmtm_device_rxq_bmp_pt_get (int unit,
                             bcmltd_sid_t ltid,
                             uint32_t channel,
                             uint64_t *cos_bmp)
{
    bcmtm_drv_t *drv;
    bcmtm_device_rxq_drv_t rxq_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&rxq_drv, 0, sizeof(bcmtm_device_rxq_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->rxq_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->rxq_drv_get(unit, &rxq_drv));

    if (rxq_drv.rxq_bmp_pt_get) {
        SHR_IF_ERR_EXIT
            (rxq_drv.rxq_bmp_pt_get(unit, ltid, channel, cos_bmp));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_device_rxq_bmp_pt_set (int unit,
                             bcmltd_sid_t ltid,
                             uint32_t channel,
                             uint64_t cos_bmp)
{
    bcmtm_drv_t *drv;
    bcmtm_device_rxq_drv_t rxq_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&rxq_drv, 0, sizeof(bcmtm_device_rxq_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->rxq_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->rxq_drv_get(unit, &rxq_drv));

    if (rxq_drv.rxq_bmp_pt_set) {
        SHR_IF_ERR_EXIT
            (rxq_drv.rxq_bmp_pt_set(unit, ltid, channel, cos_bmp));
    }
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief DEVICE RX queue IMM table change callback function for staging.
 *
 * Handle DEVICE RX queue IMM table change.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_device_pkt_rxq_stage(int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event_reason,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context,
                           bcmltd_fields_t *output_fields)
{
    uint32_t channel;
    BCMLTD_BITDCL cos_bmap = 0;
    bcmltd_fid_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    fid = DEVICE_PKT_RX_Qt_RX_Qf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    channel = (uint32_t)fval;

    SHR_IF_ERR_EXIT
        (bcmtm_device_rxq_bmp_pt_get(unit, sid, channel, &cos_bmap));

    fid = DEVICE_PKT_RX_Qt_COSf;
    while (data) {
        if (data->data) {
            BCMLTD_BIT_SET(&cos_bmap, data->idx);
        } else {
            BCMLTD_BIT_CLR(&cos_bmap, data->idx);
        }
        data = data->next;
    }
    if (event_reason == BCMIMM_ENTRY_DELETE) {
        cos_bmap = 0;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_device_rxq_bmp_pt_set(unit, sid, channel, cos_bmap));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_device_pkt_rxq_imm_register(int unit)
{
    /*! Inmemory callback function for DEVICE_PKT_RX_Qt logical table. */
    bcmimm_lt_cb_t device_pkt_rxq_cb = {
        /*! Staging function. */
        .stage = bcmtm_device_pkt_rxq_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, DEVICE_PKT_RX_Qt, &map);
    if (rv == SHR_E_UNAVAIL || map == NULL) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /* Register callback functions for DEVICE_PKT_RX_Qt */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, DEVICE_PKT_RX_Qt, &device_pkt_rxq_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
