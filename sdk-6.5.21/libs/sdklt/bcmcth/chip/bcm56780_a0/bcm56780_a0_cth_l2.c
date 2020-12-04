/*! \file bcm56780_a0_cth_l2.c
 *
 * L2 drivers for bcm56780 A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/chip/bcm56780_a0_lp_intr.h>
#include <bcmbd/chip/bcm56780_a0_ipep_intr.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_util.h>

#include <bcmcth/bcmcth_l2_util.h>
#include <bcmcth/bcmcth_l2.h>

#define BSL_LOG_MODULE          BSL_LS_BCMCTH_L2

typedef struct bcm56780_cache_fld_s {
    /* Cache field ID */
    uint32_t id;
    /* Start bit position */
    uint8_t sbit;
    /* End bit position */
    uint8_t ebit;
} bcm56780_cache_fld_t;

static bcm56780_cache_fld_t cache_flds[] = {
    { LEARN_CACHE_DATAt_CACHE_FIELD0f, 0,   15 },
    { LEARN_CACHE_DATAt_CACHE_FIELD1f, 16,  31 },
    { LEARN_CACHE_DATAt_CACHE_FIELD2f, 32,  47 },
    { LEARN_CACHE_DATAt_CACHE_FIELD3f, 48,  63 },
    { LEARN_CACHE_DATAt_CACHE_FIELD4f, 64,  79 },
    { LEARN_CACHE_DATAt_CACHE_FIELD5f, 80,  95 },
    { LEARN_CACHE_DATAt_CACHE_FIELD6f, 96,  99 },
    { LEARN_CACHE_DATAt_CACHE_FIELD7f, 100, 103 }
};

/* L2 fields array IMM handler. */
static shr_famm_hdl_t l2_fld_arr_hdl[BCML2_DEV_NUM_MAX];

static int l2_learn_intr[] = {
    PIPE0_IP_TO_CMIC_INTR,
    PIPE1_IP_TO_CMIC_INTR,
};

static int
bcm56780_a0_cth_l2_intr_enable_set(int unit, bool enable, bool in_intr)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    if (enable) {
        SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
            bcmbd_cmicx_lp_intr_disable(unit, l2_learn_intr[idx]);
        }
        /*
         * LP interrupt should be disabled, if we want to clear this interrupt.
         * Otherwise, LEARN_CACHE_INTR never come again.
         */
        SHR_IF_ERR_EXIT
            (bcmbd_ipep_intr_clear(unit, LEARN_CACHE_INTR));

        SHR_IF_ERR_EXIT
            (bcmbd_ipep_intr_enable(unit, LEARN_CACHE_INTR));

        SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
            bcmbd_cmicx_lp_intr_enable(unit, l2_learn_intr[idx]);
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmbd_ipep_intr_disable(unit, LEARN_CACHE_INTR));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_intr_handler_set(int unit, bcmcth_l2_drv_intr_handler_f cb)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ipep_intr_func_set(unit, LEARN_CACHE_INTR, cb, 0));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_hw_enable(int unit, uint32_t trans_id, bool enable)
{
    LEARN_CACHE_CTRLr_t cache_ctrl;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_read(unit,
                                      trans_id,
                                      LEARN_CACHE_CONTROLt,
                                      LEARN_CACHE_CTRLr,
                                      0,
                                      0,
                                      &cache_ctrl));

    LEARN_CACHE_CTRLr_CLEAR_ON_READ_ENf_SET(cache_ctrl, 1);
    LEARN_CACHE_CTRLr_CLEAR_ENTRY_ON_PURGEf_SET(cache_ctrl, enable);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_write(unit,
                                       trans_id,
                                       LEARN_CACHE_CONTROLt,
                                       LEARN_CACHE_CTRLr,
                                       -1,
                                       0,
                                       &cache_ctrl));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_entry_delete(int unit, void *data)
{
    uint32_t pipe = 0, data_id = 0;
    const bcmltd_field_t *lt_field;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    lt_field = (bcmltd_field_t *)data;
    while (lt_field) {
        fid = lt_field->id;
        switch (fid) {
        case LEARN_CACHE_DATAt_PIPEf:
            pipe = (uint8_t)lt_field->data;
            break;
        case LEARN_CACHE_DATAt_LEARN_CACHE_DATA_IDf:
            data_id = (uint16_t)lt_field->data;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "LT delete entry: [%d][%d].\n"),
              pipe, data_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_l2_learn_queue_entry_delete(unit, pipe, data_id));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_entry_parse(int unit, const bcmltd_field_t *key,
                               const bcmltd_field_t *data, void *lt)
{
    l2_learn_control_info_t *lt_info = (l2_learn_control_info_t *)lt;
    const bcmltd_field_t *lt_field = data;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        switch (fid) {
        case LEARN_CACHE_CONTROLt_REPORTf:
            lt_info->enable = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_REPORT);
            break;

        case LEARN_CACHE_CONTROLt_SLOW_POLLf:
            lt_info->slow_poll = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_SLOW_POLL);
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_entry_insert(int unit, uint32_t pipe, uint32_t index,
                                void *data)
{
    LEARN_CACHE_CACHEm_t *hw_entry;
    uint32_t cache_key[4] = {0};
    bcmltd_fields_t in_flds;
    uint32_t i = 0, fval32 = 0, fld_idx;

    SHR_FUNC_ENTER(unit);

    in_flds.count = LEARN_CACHE_DATAt_FIELD_COUNT;
    in_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit], in_flds.count);
    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    hw_entry = (LEARN_CACHE_CACHEm_t *)data;
    LEARN_CACHE_CACHEm_CACHE_KEYf_GET(*hw_entry, cache_key);

    /* key fields */
    i = 0;
    in_flds.field[i]->id = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_IDf;
    in_flds.field[i]->data = index;

    i++;
    in_flds.field[i]->id = LEARN_CACHE_DATAt_PIPEf;
    in_flds.field[i]->data = pipe;

    /* data fields */
    i++;
    for (fld_idx = 0; fld_idx < COUNTOF(cache_flds); i++, fld_idx++) {
        in_flds.field[i]->id = cache_flds[fld_idx].id;
        (void)bcmdrd_field_get(cache_key, cache_flds[fld_idx].sbit,
                               cache_flds[fld_idx].ebit, &fval32);
        in_flds.field[i]->data = fval32;
    }

    in_flds.field[i]->id = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_INFO_IDf;
    in_flds.field[i]->data = LEARN_CACHE_CACHEm_CACHE_KEY_TYPEf_GET(*hw_entry);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, LEARN_CACHE_DATAt, &in_flds));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "LT insert entry: [%d][%d].\n"),
              pipe, index));

exit:
    if (in_flds.field != NULL) {
        shr_famm_free(l2_fld_arr_hdl[unit], in_flds.field, in_flds.count);
    }

    SHR_FUNC_EXIT();
}

static bool
bcm56780_a0_cth_l2_entry_valid(int unit, void *data)
{
    LEARN_CACHE_CACHEm_t *hw_entry;

    hw_entry = (LEARN_CACHE_CACHEm_t *)data;

    if (LEARN_CACHE_CACHEm_VALID_EOPf_GET(*hw_entry) == 1) {
        return true;
    } else {
        return false;
    }
}

static int
bcm56780_a0_cth_l2_init(int unit, bool warm)
{
    uint32_t index, profile_depth;
    LEARN_CACHE_BITP_PROFILEm_t bitp_profile;

    SHR_FUNC_ENTER(unit);

    /* IMM */
    SHR_IF_ERR_VERBOSE_EXIT
        (shr_famm_hdl_init(LEARN_CACHE_DATAt_FIELD_COUNT,
                           &l2_fld_arr_hdl[unit]));

    /* Cache data profile */
    if (!warm) {
        profile_depth =
            bcmdrd_pt_index_max(unit, LEARN_CACHE_BITP_PROFILEm) + 1;
        for (index = 0; index < profile_depth; index++) {
            SHR_IF_ERR_EXIT
                (bcmcth_l2_per_pipe_ireq_read(unit,
                                              LEARN_CACHE_DATAt,
                                              LEARN_CACHE_BITP_PROFILEm,
                                              0,
                                              index,
                                              0,
                                              &bitp_profile));
            LEARN_CACHE_BITP_PROFILEm_CACHE_KEY_TYPEf_SET(bitp_profile, index);

            SHR_IF_ERR_EXIT
                (bcmcth_l2_per_pipe_ireq_write(unit,
                                               LEARN_CACHE_DATAt,
                                               LEARN_CACHE_BITP_PROFILEm,
                                               0,
                                               index,
                                               &bitp_profile));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (l2_fld_arr_hdl[unit] != NULL) {
        shr_famm_hdl_delete(l2_fld_arr_hdl[unit]);
        l2_fld_arr_hdl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcm56780_a0_cth_l2_attach(int unit)
{
    bcmcth_l2_drv_t *drv = bcmcth_l2_drv_get(unit);

    drv->cfg_sid = LEARN_CACHE_CONTROLt;
    drv->state_sid = LEARN_CACHE_DATAt;
    drv->state_ptsid = LEARN_CACHE_CACHEm;

    drv->init = bcm56780_a0_cth_l2_init;
    drv->cleanup = bcm56780_a0_cth_l2_cleanup;
    drv->hw_enable = bcm56780_a0_cth_l2_hw_enable;
    drv->l2_intr_enable_set = bcm56780_a0_cth_l2_intr_enable_set;
    drv->intr_handler_set = bcm56780_a0_cth_l2_intr_handler_set;
    drv->entry_delete = bcm56780_a0_cth_l2_entry_delete;
    drv->entry_parse = bcm56780_a0_cth_l2_entry_parse;
    drv->entry_insert = bcm56780_a0_cth_l2_entry_insert;
    drv->entry_valid = bcm56780_a0_cth_l2_entry_valid;

    return SHR_E_NONE;
}

