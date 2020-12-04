/*! \file bcm56990_b0_cth_l2.c
 *
 * L2 learn drivers for bcm56990 B0.
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
#include <bcmbd/chip/bcm56990_b0_lp_intr.h>
#include <bcmbd/chip/bcm56990_b0_acc.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmlrd/chip/bcm56990_b0/bcm56990_b0_lrd_enum_ctype.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
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

/* FLTG did not generate MACRO for minimal valid value for each field */
#define VALID_MIN_L3_MC_GRP_ID  1
#define VALID_MIN_NHOP_ID       1

/* L2 fields array IMM handler. */
static shr_famm_hdl_t l2_fld_arr_hdl[BCML2_DEV_NUM_MAX];

static int l2_learn_intr[] = {
    QUAD0_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD1_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD2_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD3_IP_TO_CMIC_LEARN_CACHE_INTR
};

static int
bcm56990_b0_cth_l2_intr_enable_set(int unit, bool enable, bool in_intr)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
        if (enable) {
            bcmbd_cmicx_lp_intr_enable(unit, l2_learn_intr[idx]);
        } else {
            if (in_intr) {
                bcmbd_cmicx_lp_intr_disable_nosync(unit, l2_learn_intr[idx]);
            } else {
                bcmbd_cmicx_lp_intr_disable(unit, l2_learn_intr[idx]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_intr_handler_set(int unit, bcmcth_l2_drv_intr_handler_f cb)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicx_lp_intr_func_set(unit, l2_learn_intr[idx], cb, idx));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_hw_enable(int unit, uint32_t trans_id, bool enable)
{
    L2_LEARN_COPY_CACHE_CTRLr_t cache_ctrl;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_read(unit,
                                      trans_id,
                                      L2_LEARN_CONTROLt,
                                      L2_LEARN_COPY_CACHE_CTRLr,
                                      0,
                                      0,
                                      &cache_ctrl));

    L2_LEARN_COPY_CACHE_CTRLr_CLEAR_ON_READ_ENf_SET(cache_ctrl, 1);
    L2_LEARN_COPY_CACHE_CTRLr_CACHE_INTERRUPT_CTRLf_SET(cache_ctrl, 1);
    L2_LEARN_COPY_CACHE_CTRLr_L2_LEARN_CACHE_ENf_SET(cache_ctrl, enable);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_write(unit,
                                       trans_id,
                                       L2_LEARN_CONTROLt,
                                       L2_LEARN_COPY_CACHE_CTRLr,
                                       -1,
                                       0,
                                       &cache_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_entry_delete(int unit, void *data)
{
    uint32_t pipe = 0, data_id = 0;
    const bcmltd_field_t *lt_field;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    lt_field = (bcmltd_field_t *)data;
    while (lt_field) {
        fid = lt_field->id;
        switch (fid) {
        case L2_VFI_LEARN_DATAt_PIPEf:
            pipe = (uint8_t)lt_field->data;
            break;
        case L2_VFI_LEARN_DATAt_L2_VFI_LEARN_DATA_IDf:
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
bcm56990_b0_cth_l2_entry_parse(int unit, const bcmltd_field_t *key,
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
        case L2_LEARN_CONTROLt_REPORTf:
            lt_info->enable = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_REPORT);
            break;

        case L2_LEARN_CONTROLt_SLOW_POLLf:
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
bcm56990_b0_cth_l2_entry_insert(int unit, uint32_t pipe, uint32_t index,
                                void *data)
{
    L2_LEARN_CACHEm_t *hw_entry;
    bcmltd_fields_t in_flds;
    int i = 0;
    uint32_t fval32[2] = {0};
    uint32_t type;

    SHR_FUNC_ENTER(unit);

    /* L2_LEARN_DATAt_MODIDf not mapped */
    in_flds.count = L2_VFI_LEARN_DATAt_FIELD_COUNT - 1;
    in_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit], in_flds.count);
    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    hw_entry = (L2_LEARN_CACHEm_t *)data;

    /* key fields */
    i = 0;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_PIPEf;
    in_flds.field[i]->data = pipe;

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_L2_VFI_LEARN_DATA_IDf;
    in_flds.field[i]->data = index;

    /* data fields */
    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_VFI_IDf;
    in_flds.field[i]->data = L2_LEARN_CACHEm_L2_VFIf_GET(*hw_entry);

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_MAC_ADDRf;
    L2_LEARN_CACHEm_L2_MAC_ADDRf_GET(*hw_entry, fval32);
    in_flds.field[i]->data = fval32[1];
    in_flds.field[i]->data = in_flds.field[i]->data << 32 | fval32[0];

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_SVPf;
    in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DVPf_GET(*hw_entry);

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_SRC_TYPEf;
    in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DEST_TYPEf_GET(*hw_entry);
    type = L2_LEARN_CACHEm_L2_DEST_TYPEf_GET(*hw_entry);

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_MODPORTf;
    if (type == BCM56990_B0_LRD_L2_DEST_T_T_PORT) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(*hw_entry);
    }

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_TRUNK_IDf;
    if (type == BCM56990_B0_LRD_L2_DEST_T_T_TRUNK) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(*hw_entry);
    }

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_L2_MC_GROUP_IDf;
    if (type == BCM56990_B0_LRD_L2_DEST_T_T_L2_MC_GRP) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(*hw_entry);
    }

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_TM_MC_GROUP_IDf;
    if (type == BCM56990_B0_LRD_L2_DEST_T_T_L3_MC_GRP) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(*hw_entry);
    } else {
        in_flds.field[i]->data = VALID_MIN_L3_MC_GRP_ID;
    }

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_NHOP_IDf;
    if (type == BCM56990_B0_LRD_L2_DEST_T_T_NHOP) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(*hw_entry);
    } else {
        in_flds.field[i]->data = VALID_MIN_NHOP_ID;
    }

    i++;
    in_flds.field[i]->id = L2_VFI_LEARN_DATAt_ECMP_IDf;
    if (type == BCM56990_B0_LRD_L2_DEST_T_T_ECMP_GRP) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(*hw_entry);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, L2_VFI_LEARN_DATAt, &in_flds));

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
bcm56990_b0_cth_l2_entry_valid(int unit, void *data)
{
    L2_LEARN_CACHEm_t *hw_entry;

    hw_entry = (L2_LEARN_CACHEm_t *)data;

    if (!L2_LEARN_CACHEm_VALID_EOPf_GET(*hw_entry) ||
        !L2_LEARN_CACHEm_VALID_SOPf_GET(*hw_entry)) {
        return false;
    } else {
        return true;
    }
}

static int
bcm56990_b0_cth_l2_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* IMM */
    SHR_IF_ERR_VERBOSE_EXIT
        (shr_famm_hdl_init(L2_VFI_LEARN_DATAt_FIELD_COUNT - 1,
                           &l2_fld_arr_hdl[unit]));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (l2_fld_arr_hdl[unit] != NULL) {
        shr_famm_hdl_delete(l2_fld_arr_hdl[unit]);
        l2_fld_arr_hdl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcm56990_b0_cth_l2_attach(int unit)
{
    bcmcth_l2_drv_t *drv = bcmcth_l2_drv_get(unit);

    drv->cfg_sid = L2_LEARN_CONTROLt;
    drv->state_sid = L2_VFI_LEARN_DATAt;
    drv->state_ptsid = L2_LEARN_CACHEm;

    drv->init = bcm56990_b0_cth_l2_init;
    drv->cleanup = bcm56990_b0_cth_l2_cleanup;
    drv->hw_enable = bcm56990_b0_cth_l2_hw_enable;
    drv->l2_intr_enable_set = bcm56990_b0_cth_l2_intr_enable_set;
    drv->intr_handler_set = bcm56990_b0_cth_l2_intr_handler_set;
    drv->entry_delete = bcm56990_b0_cth_l2_entry_delete;
    drv->entry_parse = bcm56990_b0_cth_l2_entry_parse;
    drv->entry_insert = bcm56990_b0_cth_l2_entry_insert;
    drv->entry_valid = bcm56990_b0_cth_l2_entry_valid;

    return SHR_E_NONE;
}
