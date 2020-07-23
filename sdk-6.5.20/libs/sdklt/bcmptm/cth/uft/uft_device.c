/*! \file uft_device.c
 *
 * UFT device specific info
 *
 * This file contains device specific info which are internal to
 * UFT Resource Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "uft_device.h"
#include "sbr_device.h"
#include "sbr_internal.h"
#include "uft_internal.h"
#include "uft_util.h"

/*******************************************************************************
 * Private variables
 */
static uft_dev_info_t *uft_dev_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uft_driver_t *uft_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};

static bool uft_enable_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT
#define UFT_DEV(unit)             (uft_dev_info[unit])
#define UFT_DRV(unit)             (uft_driver[unit])
#define UFT_DRV_EXEC(unit, fn)    (UFT_DRV(unit)->fn)

/*******************************************************************************
 * Private functions
 */
static int
uft_reg_info_update(int unit, bcmltd_sid_t lt_id,
                    const bcmltd_op_arg_t *op_arg,
                    uft_regs_info_t *regs)
{
    uft_hw_map_t *reg_map = NULL;
    uint32_t entry[UFT_MAX_PT_ENTRY_WORDS];
    int idx, f_idx;

    SHR_FUNC_ENTER(unit);
    sal_memset(entry, 0, sizeof(entry));

    for (idx = 0; idx < regs->cnt; idx++) {
        reg_map = &regs->reg_map[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_uft_lt_hw_read(unit, lt_id, op_arg,
                                   reg_map->sid, reg_map->index,
                                   UFT_MAX_PT_ENTRY_WORDS, entry));
        for (f_idx = 0; f_idx < reg_map->f_cnt; f_idx++) {
            bcmdrd_pt_field_set(unit, reg_map->sid, entry,
                                reg_map->f_ids[f_idx],
                                &reg_map->f_data[f_idx]);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_uft_lt_hw_write(unit, lt_id, op_arg,
                                    reg_map->sid, reg_map->index, entry));
    }
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

uft_dev_info_t *
bcmptm_uft_device_info_get(int unit)
{
    return UFT_DEV(unit);
}

uft_driver_t *
bcmptm_uft_driver_get(int unit)
{
    return UFT_DRV(unit);
}

int
bcmptm_uft_regs_init(int unit)
{
    uft_hw_map_t *reg_map = NULL;
    uint32_t entry[UFT_MAX_PT_ENTRY_WORDS];
    int idx, f_idx;
    uft_regs_info_t *regs = NULL;

    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->regs_init == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    UFT_ALLOC(regs, sizeof(uft_regs_info_t), "bcmptmUftRegsInfo");
    sal_memset(regs, 0, sizeof(uft_regs_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, regs_init)(unit, UFT_DEV(unit), regs));

    for (idx = 0; idx < regs->cnt; idx++) {
        reg_map = &(regs->reg_map[idx]);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_uft_lt_ireq_read(unit, DEVICE_EM_BANK_INFOt,
                                     reg_map->sid, reg_map->index,
                                     UFT_MAX_PT_ENTRY_WORDS, entry));
        for (f_idx = 0; f_idx < reg_map->f_cnt; f_idx++) {
            bcmdrd_pt_field_set(unit, reg_map->sid, entry,
                                reg_map->f_ids[f_idx],
                                &reg_map->f_data[f_idx]);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_uft_lt_ireq_write(unit, DEVICE_EM_BANK_INFOt,
                                      reg_map->sid, reg_map->index, entry));
    }

exit:
    SHR_FREE(regs);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_device_info_init(int unit, bool warm,
                            const bcmptm_uft_var_drv_t *var_info,
                            uft_dev_info_t *dev)
{
    const em_bank_info_t *bank;
    uint32_t idx;
    bcmdrd_sid_t ptsid;
    bcmdrd_sid_t fid;
    uint32_t entry[UFT_MAX_PT_ENTRY_WORDS] = {0};
    uint32_t def_offset;

    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->device_info_init == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, device_info_init)(unit, var_info, dev));

    /* Only for device that supports variants. */
    if (!warm && var_info != NULL) {
        for (idx = 0; idx < var_info->em_bank_cnt; idx++) {
            bank = &(var_info->em_bank[idx]);
            ptsid = bank->ctrl_ptid;
            def_offset = 0;

            /* Check if the PT is valid for the SKU. */
            if (!bcmdrd_pt_index_valid(unit, ptsid, -1, 0)) {
                continue;
            }
            if (ptsid != INVALIDm) {
                fid = bank->offset_fid;
                SHR_IF_ERR_EXIT
                    (bcmptm_uft_lt_ireq_read(unit, DEVICE_EM_BANKt,
                                             ptsid, 0,
                                             UFT_MAX_PT_ENTRY_WORDS, entry));
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit, ptsid,
                                         entry, fid, &def_offset));
                dev->bank_offset[idx] = def_offset;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_tbl_em_ctrl_validate(int unit, uft_tbl_em_ctrl_t *ctrl)
{
    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->tbl_em_ctrl_validate == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, tbl_em_ctrl_validate)(unit, ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_grp_validate(int unit, uft_dev_em_grp_t *grp)
{
    uft_dev_info_t *dev = UFT_DEV(unit);

    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->dev_em_grp_validate == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, dev_em_grp_validate)(unit, grp, dev));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_grp_set(int unit, bcmltd_sid_t lt_id,
                          const bcmltd_op_arg_t *op_arg,
                          uft_dev_em_grp_t *grp)
{
    uft_dev_info_t *dev = UFT_DEV(unit);
    uft_regs_info_t *regs = NULL;
    int idx;
    bcmptm_uft_bank_map_t req, temp;

    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->dev_em_grp_set == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    UFT_ALLOC(regs, sizeof(uft_regs_info_t), "bcmptmUftRegsInfo");
    sal_memset(regs, 0, sizeof(uft_regs_info_t));
    sal_memset(req.bm, 0, sizeof(req));
    sal_memset(temp.bm, 0, sizeof(temp));

    for (idx = 0; idx < grp->bank_cnt; idx++) {
        SHR_BITSET(req.bm, grp->bank[idx]);
    }

    SHR_BITAND_RANGE(dev->grp_a_bmp_arr[grp->grp_id].bm, req.bm,
                     0, UFT_PHYS_BANKS_MAX, temp.bm);
    if (!SHR_BITEQ_RANGE(req.bm, temp.bm, 0, UFT_PHYS_BANKS_MAX)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    SHR_BITCOPY_RANGE(dev->grp_c_bmp_arr[grp->grp_id].bm, 0,
                      req.bm, 0, UFT_PHYS_BANKS_MAX);

    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, dev_em_grp_set)(unit, grp, UFT_DEV(unit), regs));

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_reg_info_update(unit, lt_id, op_arg, regs));

exit:
    SHR_FREE(regs);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank)
{
    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->dev_em_bank_validate == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    UFT_DRV_EXEC(unit, dev_em_bank_validate)(unit, bank);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_bank_set(int unit, bcmltd_sid_t lt_id,
                           const bcmltd_op_arg_t *op_arg,
                           uft_dev_em_bank_t *bank)
{
    uft_regs_info_t *regs = NULL;
    uft_dev_info_t *dev;

    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->dev_em_bank_set == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    UFT_ALLOC(regs, sizeof(uft_regs_info_t), "bcmptmUftRegsInfo");
    sal_memset(regs, 0, sizeof(uft_regs_info_t));
    dev = UFT_DEV(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, dev_em_bank_set)(unit, bank, dev, regs));

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_reg_info_update(unit, lt_id, op_arg, regs));

exit:
    SHR_FREE(regs);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_tile_set(int unit, bcmltd_sid_t lt_id,
                           const bcmltd_op_arg_t *op_arg,
                           uft_dev_em_tile_t *tile)
{
    uft_regs_info_t *regs = NULL;
    uft_dev_info_t *dev;
    const em_tile_mode_info_t *tile_mode_info = NULL;
    bcmdrd_sid_t lts_tcam_sid = 0;
    uint32_t tbl_size = 0, ent_size = 0;
    uint32_t *dma_buf = NULL;
    uint32_t i;
    uint32_t cur_mode, disabled_mode;

    SHR_FUNC_ENTER(unit);

    if (UFT_DRV(unit)->dev_em_tile_set == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    UFT_ALLOC(regs, sizeof(uft_regs_info_t), "bcmptmUftRegsInfo");
    sal_memset(regs, 0, sizeof(uft_regs_info_t));
    dev = UFT_DEV(unit);
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_MODEf)) {
        if (!uft_enable_atomic_trans[unit]) {
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_tile_mode_set(unit, tile->tile_id, tile->mode,
                                          TRUE, lt_id, op_arg));
        }

        SHR_IF_ERR_EXIT
            (bcmlrd_field_symbol_to_value(unit,
                                          DEVICE_EM_TILEt,
                                          DEVICE_EM_TILEt_MODEf,
                                          "TILE_MODE_DISABLED",
                                          &disabled_mode));
        /* Get lts_tcam_to_clear info from currrent tile mode. */
        SHR_IF_ERR_EXIT
            (bcmptm_uft_tile_cfg_get(unit, tile->tile_id,
                                     &cur_mode, NULL, NULL));
        if (cur_mode != disabled_mode) {
            /* Clear LTS_TCAM entry info before calling flexcode. */
            if (cur_mode >= dev->var->tile_mode_cnt) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            tile_mode_info = dev->var->tile_mode[cur_mode];
            for (i = 0; i < tile_mode_info->sid_lts_tcam_to_clear_count; i++) {
                lts_tcam_sid = tile_mode_info->sid_lts_tcam_to_clear[i];
                tbl_size = bcmdrd_pt_index_max(unit, lts_tcam_sid)
                           - bcmdrd_pt_index_min(unit, lts_tcam_sid) + 1;
                ent_size = bcmdrd_pt_entry_wsize(unit, lts_tcam_sid);
                dma_buf = sal_alloc(BCMDRD_WORDS2BYTES(ent_size * tbl_size),
                                    "bcmptmUftLtsDmaBuf");
                SHR_NULL_CHECK(dma_buf, SHR_E_MEMORY);
                sal_memset(dma_buf, 0,
                           BCMDRD_WORDS2BYTES(ent_size * tbl_size));
                SHR_IF_ERR_EXIT
                    (bcmptm_uft_hw_write_dma(unit, op_arg, lt_id,
                                             lts_tcam_sid, 0,
                                             tbl_size, dma_buf));
                SHR_FREE(dma_buf);
                dma_buf = NULL;
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, dev_em_tile_set)(unit, op_arg, tile, dev, regs));

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_reg_info_update(unit, lt_id, op_arg, regs));

    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_MODEf)) {
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_tile_mode_set(unit, tile->tile_id, tile->mode,
                                      FALSE, lt_id, op_arg));
    }

exit:
    SHR_FREE(regs);
    SHR_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_device_init(int unit, bool warm)
{
    int rv;
    const bcmptm_uft_var_drv_t *var = NULL;
    bcmcfg_feature_ctl_config_t feature_conf;

    SHR_FUNC_ENTER(unit);

    UFT_ALLOC(UFT_DEV(unit), sizeof(uft_dev_info_t), "bcmptmUftDevInfo");
    sal_memset(UFT_DEV(unit), 0, sizeof(uft_dev_info_t));

    UFT_ALLOC(UFT_DRV(unit), sizeof(uft_driver_t), "bcmptmUftDrv");
    sal_memset(UFT_DRV(unit), 0, sizeof(uft_driver_t));

    rv = bcmptm_uft_driver_register(unit, UFT_DRV(unit));
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_var_info_get(unit, &var));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_device_info_init(unit, warm, var, UFT_DEV(unit)));

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        uft_enable_atomic_trans[unit] = FALSE;
    } else {
        uft_enable_atomic_trans[unit] = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_device_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    uft_enable_atomic_trans[unit] = FALSE;

    if (UFT_DRV(unit)->device_deinit != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
        (UFT_DRV_EXEC(unit, device_deinit)(unit));
    }

    if (UFT_DEV(unit)) {
        SHR_FREE(UFT_DEV(unit)->grp_c_bmp_arr);
        SHR_FREE(UFT_DEV(unit));
    }
    SHR_FREE(UFT_DRV(unit));

exit:
    SHR_FUNC_EXIT();
}
