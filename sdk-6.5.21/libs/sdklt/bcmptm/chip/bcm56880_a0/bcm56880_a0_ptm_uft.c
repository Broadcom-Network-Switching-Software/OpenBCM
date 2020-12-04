/*! \file bcm56880_a0_ptm_uft.c
 *
 * Chip specific functions for PTM UFT
 *
 * This file contains the chip specific functions for PTM UFT
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
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmdi/bcmdi.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_uft_internal.h>
#include "bcm56880_a0_ptm_uft.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

/*!
 * \brief UFT global info for supporting tile mode.
 */
typedef struct uft_gl_tile_mode_s {
    /* Global inited. */
    bool init;

    /* Global group info. */
    uft_grp_info_t grps[DEVICE_EM_GROUP_CNT_MAX];

    /* Group's available banks. */
    bcmptm_uft_bank_map_t grps_ava_arr[DEVICE_EM_GROUP_CNT_MAX];

    /* Global bank info. */
    uft_bank_info_t banks[UFT_PHYS_BANKS_MAX];

} uft_gl_tile_mode_t;

static uft_gl_tile_mode_t *uft_gl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

static int
uft_global_init_tile_mode(int unit, const bcmptm_uft_var_drv_t *var_info)
{
    uint32_t i, j, k;
    uint32_t grp_cnt, grp_id;
    const em_grp_info_t **em_grp;
    uint32_t first_uft_bank, uft_bank_cnt, max_bank_cnt;
    uint32_t em_grp_cnt;
    uint32_t quad_ent_sz;
    uft_grp_info_t *uft_grp;
    uft_bank_info_t *uft_bank;
    const em_bank_info_t *em_bank;
    uint32_t lkp0_cnt = 0, lkp1_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (uft_gl[unit]->init) {
        SHR_EXIT();
    }
    uft_gl[unit]->init = TRUE;

    em_grp = var_info->em_group;
    first_uft_bank = var_info->first_uft_bank;
    em_grp_cnt = var_info->em_group_cnt;
    max_bank_cnt = var_info->em_bank_cnt;
    uft_bank_cnt = var_info->uft_bank_cnt;
    em_bank = var_info->em_bank;

    for (i = 0; i < em_grp_cnt; i++) {
        uft_grp = &(uft_gl[unit]->grps[i]);
        grp_id = em_grp[i]->grp_id;
        uft_grp->grp_id = grp_id;
        uft_grp->ovly_ptsid = INVALIDm;
        uft_grp->vector_type = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B;
        uft_grp->is_alpm = em_grp[i]->is_alpm;
        uft_grp->attri = em_grp[i]->attri;
        uft_grp->fixed_cnt = em_grp[i]->fixed_cnt;
        for (j = 0; j < em_grp[i]->fixed_cnt; j++) {
            uft_grp->fixed_banks[j] = em_grp[i]->fixed_banks[j];
        }
        for (j = 0; j < em_grp[i]->cap_cnt; j++) {
            SHR_BITSET(uft_gl[unit]->grps_ava_arr[i].bm,
                       em_grp[i]->cap_banks[j]);
        }
        lkp0_cnt = em_grp[i]->lookup0_lt_count;
        lkp1_cnt = em_grp[i]->lookup1_lt_count;
        uft_grp->lt_cnt = lkp0_cnt + lkp1_cnt;
        for (j = 0; j < lkp0_cnt; j++) {
            uft_grp->lt_sids[j] = em_grp[i]->lookup0_lt[j];
        }
        for (j = 0; j < lkp1_cnt; j++) {
            uft_grp->lt_sids[lkp0_cnt + j] = em_grp[i]->lookup1_lt[j];
        }
    }

    for (i = 0; i < max_bank_cnt; i++) {
        uft_bank = &(uft_gl[unit]->banks[i]);
        uft_bank->bank_id = i;
        uft_bank->lookup_cnt = em_bank[i].lookup_cnt;
        grp_cnt = 0;
        for (j = 0; j < em_grp_cnt; j++) {
            if (SHR_BITGET(uft_gl[unit]->grps_ava_arr[j].bm, i)) {
                uft_bank->grps[grp_cnt] = j;
                grp_cnt++;
            } else {
                for (k = 0; k < em_grp[j]->fixed_cnt; k++) {
                    if (em_grp[j]->fixed_banks[k] == i) {
                        uft_bank->grps[grp_cnt] = j;
                        grp_cnt++;
                        break;
                    }
                }
            }
        }

        uft_bank->grp_cnt = grp_cnt;
        if (i < (first_uft_bank + uft_bank_cnt)) {
            uft_bank->base_entry_width = 120;
            uft_bank->base_buket_width = 4;
            quad_ent_sz =
                bcmdrd_pt_index_max(unit, var_info->bank_ptsid[i]) -
                bcmdrd_pt_index_min(unit, var_info->bank_ptsid[i]) +
                1;
            uft_bank->num_base_buckets = quad_ent_sz;
            uft_bank->num_base_entries = quad_ent_sz * 4;
        } else {
            /* Non-Hash banks(Mini banks, TCAM banks). */
            uft_bank->base_entry_width = 0;
            uft_bank->base_buket_width = 0;
            uft_bank->num_base_buckets = 0;
            uft_bank->num_base_entries = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Switch uft mode.
 *
 * \param [in] unit Device unit.
 * \param [in] new_mode New uft mode.
 * \param [in|out] regs Register info to be updated.
 *
 * \return SHR_E_XXX.
 */
static int
uft_bank_assigned_get(int unit, uft_dev_info_t *dev, int bank_id,
                      bool *assigned, uint32_t *attri, bool *alpm)
{
    uint32_t i, grp_id;
    uft_grp_info_t *grp;
    const bcmptm_uft_var_drv_t *var = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dev, SHR_E_PARAM);
    var = dev->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);
    SHR_NULL_CHECK(assigned, SHR_E_PARAM);

    *assigned = 0;
    for (i = 0; i < dev->max_grps; i++) {
        grp = &(dev->grp_info[i]);
        grp_id = grp->grp_id;
        if (SHR_BITGET(dev->grp_c_bmp_arr[grp_id].bm, bank_id)) {
            *assigned = TRUE;
            if (attri) {
                *attri = grp->attri;
            }
            if (alpm) {
                *alpm = grp->is_alpm;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Level-1 registers
 *
 * \param [in] unit Device unit.
 * \param [in] dev UFT device information pointer.
 * \param [in|out] regs Register info
 *
 * \return SHR_E_XXX.
 */
static int
bcm56880_a0_uft_regs_init(int unit,
                          uft_dev_info_t *dev,
                          uft_regs_info_t *regs)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize UFT device information for tile mode management.
 *
 * \param [in] unit Device unit.
 * \param [in] var_info UFT variant information.
 * \param [out] dev UFT device information.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56880_a0_uft_device_info_init(int unit,
                                 const bcmptm_uft_var_drv_t *var_info,
                                 uft_dev_info_t *dev)
{
    size_t size;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dev, SHR_E_INIT);
    uft_gl[unit] = sal_alloc(sizeof(uft_gl_tile_mode_t), "bcmptmUftGlobal");
    SHR_NULL_CHECK(uft_gl[unit], SHR_E_MEMORY);
    sal_memset(uft_gl[unit], 0, sizeof(uft_gl_tile_mode_t));
    SHR_IF_ERR_EXIT(uft_global_init_tile_mode(unit, var_info));

    dev->var = var_info;
    dev->max_grps = var_info->em_group_cnt;
    dev->max_tiles = var_info->em_tile_cnt;
    dev->max_banks = var_info->em_bank_cnt;
    dev->first_uft_bank = var_info->first_uft_bank;
    dev->grp_info = uft_gl[unit]->grps;
    dev->bank_info = uft_gl[unit]->banks;
    dev->grp_a_bmp_arr = uft_gl[unit]->grps_ava_arr;
    size = dev->max_grps * sizeof(bcmptm_uft_bank_map_t);
    SHR_ALLOC(dev->grp_c_bmp_arr, size, "bcmptmUftBankBmpCur");
    SHR_NULL_CHECK(dev->grp_c_bmp_arr, SHR_E_MEMORY);
    sal_memset(dev->grp_c_bmp_arr, 0, size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-Initialize device specific info.
 *
 * \param [in] unit Device unit.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56880_a0_uft_device_deinit(int unit)
{
    SHR_FREE(uft_gl[unit]);
    uft_gl[unit] = NULL;

    return SHR_E_NONE;
}


static int
bcm56880_a0_uft_tbl_em_ctrl_validate(int unit,
                                     uft_tbl_em_ctrl_t *ctrl)
{
    uint64_t max_depth = 0;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    /* Check validity of move_depth. */
    if (SHR_BITGET(ctrl->fbmp, TABLE_EM_CONTROLt_MOVE_DEPTHf)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_max_get(unit, TABLE_EM_CONTROLt,
                                  TABLE_EM_CONTROLt_MOVE_DEPTHf,
                                  1, &max_depth, &num));
        if (ctrl->move_depth > max_depth) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_uft_dev_em_grp_validate(int unit,
                                    uft_dev_em_grp_t *grp,
                                    uft_dev_info_t *dev)
{
    const bcmptm_uft_var_drv_t *var = dev->var;
    uint32_t idx;
    const em_grp_info_t **em_grp;
    uint32_t bank_id;
    int bank_cnt;
    bcmptm_uft_bank_map_t new_bmp;
    bool assigned = 0, found = 0;
    uint32_t attri = 0, grp_attri = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    em_grp = var->em_group;
    for (idx = 0; idx < var->em_group_cnt; idx++) {
        if (em_grp[idx]->grp_id == grp->grp_id) {
            grp_attri = em_grp[idx]->attri;
            found = 1;
            break;
        }
    }
    if (!found) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(new_bmp.bm, 0, sizeof(new_bmp));
    for (idx = 0; idx < grp->bank_cnt; idx++) {
        bank_id = grp->bank[idx];
        SHR_BITSET(new_bmp.bm, bank_id);
        if (SHR_BITGET(dev->grp_a_bmp_arr[grp->grp_id].bm, bank_id) == 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (uft_bank_assigned_get(unit, dev, bank_id,
                                   &assigned, &attri, NULL));
        if (assigned && (grp_attri != attri)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_BITCOUNT_RANGE(new_bmp.bm, bank_cnt, 0, UFT_PHYS_BANKS_MAX);
    if (bank_cnt != grp->bank_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_uft_dev_em_grp_set(int unit,
                               uft_dev_em_grp_t *grp,
                               uft_dev_info_t *dev,
                               uft_regs_info_t *regs)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t i, tile_id, b0_id = 0;
    bcmlrd_sid_t sid = INVALIDm;
    bool b0_found = FALSE;
    bool b0_assigned = FALSE, b1_assigned = FALSE;
    bool b0_alpm = FALSE, b1_alpm = FALSE;
    bool b0_hash = FALSE, b1_hash = FALSE;
    int value_0 = 0, value_1 = 0;
    int cnt = 0;

    SHR_FUNC_ENTER(unit);

    var = dev->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    if (grp->grp_id > var->em_group_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    tile_id = var->em_group[grp->grp_id]->tile;
    sid = var->em_tile[tile_id].ctrl_ptid;
    /* Only IFTA90 tils need to be configured. */
    if (sid != IFTA90_E2T_00_HASH_CONTROLm &&
        sid != IFTA90_E2T_01_HASH_CONTROLm &&
        sid != IFTA90_E2T_02_HASH_CONTROLm &&
        sid != IFTA90_E2T_03_HASH_CONTROLm) {
        SHR_EXIT();
    }

    for (i = 0; i < var->em_bank_cnt; i++) {
        /* Find the tile's first related banks. */
        if (var->tile_id[i] == tile_id) {
            b0_id = i;
            b0_found = TRUE;
            break;
        }
    }
    if (!b0_found) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_bank_assigned_get(unit, dev, b0_id,
                               &b0_assigned, NULL, &b0_alpm));
    SHR_IF_ERR_VERBOSE_EXIT
        (uft_bank_assigned_get(unit, dev, b0_id + 1,
                               &b1_assigned, NULL, &b1_alpm));
    b0_hash = !(b0_assigned && b0_alpm);
    b1_hash = !(b1_assigned && b1_alpm);

    /*
     * For 56880, UFT E-Tile is always configured to perform
     * one lookup per packet.
     */
    if (!b0_hash && !b1_hash) {
        value_0 = 0;
    } else if (b0_hash && !b1_hash) {
        value_0 = 1; /* 0b01 */
    } else if (!b0_hash && b1_hash) {
        value_0 = 2; /* 0b10 */
    } else if (b0_hash && b1_hash) {
        value_0 = 3; /* 0b11 */
    }
    value_1 = 0;
    regs->reg_map[cnt].sid = sid;
    regs->reg_map[cnt].f_ids[0] = HASH_TABLE_BANK_CONFIG_LANE0f;
    regs->reg_map[cnt].f_data[0] = value_0;
    regs->reg_map[cnt].f_ids[1] = HASH_TABLE_BANK_CONFIG_LANE1f;
    regs->reg_map[cnt].f_data[1] = value_1;
    regs->reg_map[cnt].f_cnt = 2;
    cnt++;

    regs->cnt = cnt;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank)
{
    uint64_t max_offset = 0;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    if (SHR_BITGET(bank->fbmp, DEVICE_EM_BANKt_OFFSETf)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_max_get(unit, DEVICE_EM_BANKt,
                                  DEVICE_EM_BANKt_OFFSETf,
                                  1, &max_offset, &num));
        if (bank->offset > max_offset) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_uft_dev_em_bank_set(int unit,
                                uft_dev_em_bank_t *bank,
                                uft_dev_info_t *dev,
                                uft_regs_info_t  *regs)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    const em_bank_info_t *em_bank;
    const bcmptm_uft_var_drv_t *var = dev->var;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dev, SHR_E_PARAM);
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    em_bank = var->em_bank;
    if (SHR_BITGET(bank->fbmp, DEVICE_EM_BANKt_OFFSETf)) {
        sid = em_bank[bank->bank_id].ctrl_ptid;
        fid = em_bank[bank->bank_id].offset_fid;
        dev->bank_offset[bank->bank_id] = bank->offset;

        regs->reg_map[0].sid = sid;
        regs->reg_map[0].f_ids[0] = fid;
        regs->reg_map[0].f_data[0] = bank->offset;
        regs->reg_map[0].f_cnt = 1;
        regs->cnt = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_uft_dev_em_tile_set(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                uft_dev_em_tile_t *tile,
                                uft_dev_info_t *dev,
                                uft_regs_info_t  *regs)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t tile_id;
    const bcmptm_uft_var_drv_t *var = dev->var;
    const em_tile_info_t *em_tile;
    const char *tile_mode_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dev, SHR_E_PARAM);
    SHR_NULL_CHECK(var, SHR_E_PARAM);
    em_tile = var->em_tile;

    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_ROBUSTf)) {
        tile_id = tile->tile_id;
        if (tile_id >= dev->max_tiles) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (tile_id != em_tile[tile_id].tile_id) {
            /* Tile enum misaligned. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        sid = em_tile[tile_id].ctrl_ptid;
        fid = ROBUST_HASH_ENf;

        regs->reg_map[0].sid = sid;
        regs->reg_map[0].f_ids[0] = fid;
        regs->reg_map[0].f_data[0] = tile->robust;
        regs->reg_map[0].f_cnt = 1;
        regs->cnt = 1;
    }

    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_MODEf)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          DEVICE_EM_TILE_INFOt,
                                          DEVICE_EM_TILE_INFOt_MODEf,
                                          tile->mode,
                                          &tile_mode_name));
        /* Load tile mode flexcode only when applied. */
        if (sal_strcmp(tile_mode_name, "TILE_MODE_DISABLED")) {
            if (var->tile_mode[tile->mode]->flexcode != NULL) {
                SHR_IF_ERR_EXIT
                    (bcmdi_flexcode_load(unit, (char *)tile_mode_name,
                                         op_arg->trans_id, FALSE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56880_a0_uft_driver_register(int unit, uft_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->regs_init            = bcm56880_a0_uft_regs_init;
        drv->device_info_init     = bcm56880_a0_uft_device_info_init;
        drv->device_deinit        = bcm56880_a0_uft_device_deinit;
        drv->tbl_em_ctrl_validate = bcm56880_a0_uft_tbl_em_ctrl_validate;
        drv->dev_em_grp_validate  = bcm56880_a0_uft_dev_em_grp_validate;
        drv->dev_em_grp_set       = bcm56880_a0_uft_dev_em_grp_set;
        drv->dev_em_bank_validate = bcm56880_a0_uft_dev_em_bank_validate;
        drv->dev_em_bank_set      = bcm56880_a0_uft_dev_em_bank_set;
        drv->dev_em_tile_set      = bcm56880_a0_uft_dev_em_tile_set;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

