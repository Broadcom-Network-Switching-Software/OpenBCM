/*! \file bcmcth_l3_bank.c
 *
 * L3_LIMIT_CONTROL Custom Handler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_l3_bank.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_L3
#define L3_BANK_CTRL_LT_SID L3_LIMIT_CONTROLt

/******************************************************************************
 * Local definitions
 */

/* Start- For imm-based L3_LIMIT_CONTROL LT. */
typedef bcmcth_l3_bank_drv_t *(*bcmcth_l3_bank_drv_get_f)(int unit);

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {_bc##_cth_l3_bank_drv_get},
static struct {
    bcmcth_l3_bank_drv_get_f drv_get;
} l3_bank_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 }  /* end-of-list */
};
/* End- For imm-based L3_LIMIT_CONTROL LT. */

static bcmcth_l3_bank_drv_t *l3_bank_drv[BCMDRD_CONFIG_MAX_UNITS];

static bcmcth_l3_bank_control_t l3_bank_control[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Public functions
 */
int
bcmcth_l3_bank_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    l3_bank_drv[unit] = l3_bank_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_l3_bank_drv_get(int unit, bcmcth_l3_bank_drv_t **drv)
{

    SHR_FUNC_ENTER(unit);

    if (l3_bank_drv[unit] != NULL) {
        *drv = l3_bank_drv[unit];
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l3_bank_control_init(int unit, bool warm)
{
    bcmcth_l3_bank_t l3_bank;
    const bcmltd_op_arg_t op_arg = {0, 0};

    SHR_FUNC_ENTER(unit);
    sal_memset(&l3_bank_control[unit], 0, sizeof(bcmcth_l3_bank_control_t));
    if (warm && l3_bank_drv[unit] &&
        l3_bank_drv[unit]->l3_bank_get) {
        sal_memset(&l3_bank, 0, sizeof(bcmcth_l3_bank_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_bank_drv[unit]->l3_bank_get(unit,
                                            &op_arg,
                                            L3_BANK_CTRL_LT_SID,
                                            &l3_bank,
                                            true));
        SHR_BITCOPY_RANGE(l3_bank_control[unit].nhop_bank_bitmap,
                          0,
                          l3_bank.nhop_bank_bitmap,
                          0,
                          L3_BANK_MAX_BIT);
        SHR_BITCOPY_RANGE(l3_bank_control[unit].eif_bank_bitmap,
                          0,
                          l3_bank.eif_bank_bitmap,
                          0,
                          L3_BANK_MAX_BIT);
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmcth_l3_bank_usage_check(int unit,
                           uint32_t trans_id,
                           bcmcth_l3_bank_t l3_bank)
{
    bcmltd_sid_t *list = NULL;
    uint32_t list_length = 0;
    uint32_t inuse_cnt = 0;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    if (l3_bank.num_o_nhop_valid) {
        if (l3_bank_drv[unit] && l3_bank_drv[unit]->nhop_lt_id_list) {
            list = l3_bank_drv[unit]->nhop_lt_id_list;
            list_length = l3_bank_drv[unit]->nhop_lt_id_count;
            for (i = 0; i < list_length; i++){
                SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlrd_table_inuse_count_get(unit, trans_id,
                                                      list[i], &inuse_cnt));
                if (inuse_cnt > 0) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                 "An entry exists in a NHOP LT\n")));
                    SHR_ERR_EXIT(SHR_E_BUSY);
                }
            }
        }
    }

    if (l3_bank.num_o_eif_valid) {
        if (l3_bank_drv[unit] && l3_bank_drv[unit]->eif_lt_id_list) {
            list = l3_bank_drv[unit]->eif_lt_id_list;
            list_length = l3_bank_drv[unit]->eif_lt_id_count;
            for (i = 0; i < list_length; i++){
                SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlrd_table_inuse_count_get(unit, trans_id,
                                                      list[i], &inuse_cnt));
                if (inuse_cnt > 0) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                 "An entry exists in an EIF LT\n")));
                    SHR_ERR_EXIT(SHR_E_BUSY);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l3_bank_nhop_id_overlay(int unit,
                               uint16_t nhop_id,
                               bool *is_overlay)
{
    uint8_t bank_index = 0;
    bool tmp_overlay = 0;

    bank_index = nhop_id >> l3_bank_drv[unit]->nhop_bank_index_offset;
    tmp_overlay = SHR_BITGET(l3_bank_control[unit].nhop_bank_bitmap,
                             bank_index);
    *is_overlay = tmp_overlay;
    return SHR_E_NONE;
}

int
bcmcth_l3_bank_eif_id_overlay(int unit,
                              uint16_t eif_id,
                              bool *is_overlay)
{
    uint8_t bank_index = 0;
    bool tmp_overlay = 0;

    bank_index = eif_id >> l3_bank_drv[unit]->eif_bank_index_offset;
    tmp_overlay = SHR_BITGET(l3_bank_control[unit].eif_bank_bitmap,
                             bank_index);
    *is_overlay = tmp_overlay;
    return SHR_E_NONE;
}

int
bcmcth_l3_bank_set(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   bcmltd_sid_t lt_id,
                   bcmcth_l3_bank_t l3_bank)
{
    SHR_FUNC_ENTER(unit);

    if (l3_bank_drv[unit] && l3_bank_drv[unit]->l3_bank_encode &&
        l3_bank_drv[unit]->l3_bank_set) {
        l3_bank_drv[unit]->l3_bank_encode(unit, &l3_bank);

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_bank_drv[unit]->l3_bank_set(unit,
                                            op_arg,
                                            lt_id,
                                            l3_bank));

        SHR_BITCOPY_RANGE(l3_bank_control[unit].nhop_bank_bitmap,
                          0,
                          l3_bank.nhop_bank_bitmap,
                          0,
                          L3_BANK_MAX_BIT);
        SHR_BITCOPY_RANGE(l3_bank_control[unit].eif_bank_bitmap,
                          0,
                          l3_bank.eif_bank_bitmap,
                          0,
                          L3_BANK_MAX_BIT);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l3_bank_get(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   bcmltd_sid_t lt_id,
                   bcmcth_l3_bank_t *l3_bank)
{
    SHR_FUNC_ENTER(unit);

    if (l3_bank_drv[unit] && l3_bank_drv[unit]->l3_bank_decode &&
        l3_bank_drv[unit]->l3_bank_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_bank_drv[unit]->l3_bank_get(unit,
                                            op_arg,
                                            lt_id,
                                            l3_bank,
                                            false));
        l3_bank_drv[unit]->l3_bank_decode(unit, l3_bank);
    }

exit:
    SHR_FUNC_EXIT();
}
