/*! \file uft.c
 *
 * Unified format table management.
 *
 * This file contains the main function for UFT LT.
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
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include <bcmptm/bcmptm_spm_internal.h>
#include <bcmptm/bcmptm_itx.h>
#include <bcmissu/issu_api.h>
#include "uft_internal.h"
#include "uft_device.h"
#include "sbr_device.h"
#include "sbr_internal.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Private variables
 */
/*! Fields array handler. */
static shr_famm_hdl_t uft_fld_arr_hdl;

/*! Used to store temporary group bitmap info before a commit. */
static bcmptm_uft_bank_map_t  *grp_bmp_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate fileds array for UFT module.
 *
 * \param [in] unit Logical device ID.
 * \param [in/out] flds Bcmltd_fields_t pointer.
 * \param [in] flds_cnt Count of fields.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_lt_famm_alloc(int unit, bcmltd_fields_t *flds, uint32_t flds_cnt) {

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(flds, SHR_E_PARAM);
    flds->field = shr_famm_alloc(uft_fld_arr_hdl, flds_cnt);
    SHR_NULL_CHECK(flds->field, SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free fileds array for UFT module.
 *
 * \param [in] unit Logical device ID.
 * \param [in] flds Bcmltd_fields_t pointer.
 * \param [in] flds_cnt Count of fields.
 *
 * \retval None.
 */
static void
uft_lt_famm_free(int unit, bcmltd_fields_t *flds, uint32_t flds_cnt) {
    if (flds == NULL) {
        return;
    }
    if (flds->field != NULL) {
        shr_famm_free(uft_fld_arr_hdl, flds->field, flds_cnt);
    }
}

/*!
 * \brief Validate a field of a logical tablefor the device.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical table ID.
 * \param [in] fid Field ID.
 * \param [out] valid True if it is valid, false if it is invalid.
 *
 * \retval None.
 */
static void
uft_lt_field_validate(int unit,
                      bcmltd_sid_t ltid,
                      const bcmlrd_fid_t fid,
                      bool *valid)
{
    uint64_t def_val = 0;
    uint32_t num = 0;
    int rv;

    rv = bcmlrd_field_default_get(unit, ltid, fid, 1, &def_val, &num);
    if (rv == SHR_E_NONE) {
        *valid = TRUE;
    } else {
        *valid = FALSE;
    }
}

/*!
 * \brief Initialzie the update-only logical table DEVICE_EM_BANK.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_bank_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    int f_cnt;
    uint32_t hash_bank_id, bank_id;
    uint64_t min, max;
    uint32_t num;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_min_get(unit, DEVICE_EM_BANKt,
                              DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf,
                              1, &min, &num));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_max_get(unit, DEVICE_EM_BANKt,
                              DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf,
                              1, &max, &num));

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    if (warm) {
        SHR_IF_ERR_EXIT
            (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));
    }

    for (hash_bank_id = min; hash_bank_id <= max; hash_bank_id++) {
        f_cnt = 0;
        in_flds.field[f_cnt]->id   = DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = hash_bank_id;
        f_cnt++;
        if (warm) {
            in_flds.count = f_cnt;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_BANKt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            }
        }

        /*
         * Do the transform from DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf to
         * DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf under the (also correct)
         * assumption that the enum symbols (but not necessarily values) are
         * the same across the two types.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_uft_bank_hash_to_global(unit, hash_bank_id, &bank_id));
        in_flds.field[f_cnt]->id   = DEVICE_EM_BANKt_OFFSETf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = dev->bank_offset[bank_id];
        f_cnt++;

        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_BANKt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the update-only logical table DEVICE_EM_BANK_PAIR.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_bank_pair_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t pair_key;
    uint64_t min, max;
    uint32_t num;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Not to do intialization if this LT is not supported. */
    rv = bcmlrd_map_get(unit, DEVICE_EM_BANK_PAIRt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, DEVICE_EM_BANK_PAIRt,
                              DEVICE_EM_BANK_PAIRt_DEVICE_EM_BANK_IDf,
                              1, &min, &num));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, DEVICE_EM_BANK_PAIRt,
                              DEVICE_EM_BANK_PAIRt_DEVICE_EM_BANK_IDf,
                              1, &max, &num));

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    if (warm) {
        SHR_IF_ERR_EXIT
            (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));
    }

    for (pair_key = min; pair_key <= max; pair_key++) {
        in_flds.field[0]->id   = DEVICE_EM_BANK_PAIRt_DEVICE_EM_BANK_IDf;
        in_flds.field[0]->idx  = 0;
        in_flds.field[0]->data = pair_key;
        in_flds.count = 1;
        if (warm) {
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_BANK_PAIRt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            }
        }
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_BANK_PAIRt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the read-only logical table DEVICE_EM_BANK_INFO.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_bank_info_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t f_cnt = 0, idx;
    uint32_t grp_idx;
    uft_bank_info_t *bank_info = NULL;
    int rv;
    uint64_t min_bank_id, max_bank_id;
    uint32_t num;
    bool lookup_cnt_f_valid = FALSE, type_f_valid = FALSE;
    em_bank_type_t type = EM_BANK_TYPE_HASH;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_min_get(unit, DEVICE_EM_BANK_INFOt,
                              DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf,
                              1, &min_bank_id, &num));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_max_get(unit, DEVICE_EM_BANK_INFOt,
                              DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf,
                              1, &max_bank_id, &num));

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));

    uft_lt_field_validate(unit,
                          DEVICE_EM_BANK_INFOt,
                          DEVICE_EM_BANK_INFOt_LOOKUP_CNTf,
                          &lookup_cnt_f_valid);
    uft_lt_field_validate(unit,
                          DEVICE_EM_BANK_INFOt,
                          DEVICE_EM_BANK_INFOt_TYPEf,
                          &type_f_valid);

    for (idx = min_bank_id; idx <= max_bank_id; idx++) {
        f_cnt = 0;
        bank_info = &(dev->bank_info[idx]);
        in_flds.field[f_cnt]->id   = DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = bank_info->bank_id;
        f_cnt++;

        if (warm) {
            in_flds.count = f_cnt;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_BANK_INFOt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            } else if (rv != SHR_E_NOT_FOUND) {
                SHR_ERR_EXIT(rv);
            }
        }

        in_flds.field[f_cnt]->id   = DEVICE_EM_BANK_INFOt_GROUP_CNTf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = bank_info->grp_cnt;
        f_cnt++;

        for (grp_idx = 0; grp_idx < bank_info->grp_cnt; grp_idx++) {
            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_DEVICE_EM_GROUP_IDf;
            in_flds.field[f_cnt]->data = bank_info->grps[grp_idx];
            in_flds.field[f_cnt]->idx = grp_idx;
            f_cnt++;
        }

        if (type_f_valid) {
            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_TYPEf;
            in_flds.field[f_cnt]->idx = 0;
            SHR_IF_ERR_EXIT
                (bcmptm_uft_bank_type_get(unit, bank_info->bank_id, &type));
            in_flds.field[f_cnt]->data = type;
            f_cnt++;
        }

        if (lookup_cnt_f_valid) {
            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_LOOKUP_CNTf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = bank_info->lookup_cnt;
            f_cnt++;
        }

        if (type == EM_BANK_TYPE_HASH) {
            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_BASE_ENTRY_WIDTHf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = bank_info->base_entry_width;
            f_cnt++;

            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_BASE_BUCKET_WIDTHf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = bank_info->base_buket_width;
            f_cnt++;

            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_NUM_BASE_BUCKETSf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = bank_info->num_base_buckets;
            f_cnt++;

            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_NUM_BASE_ENTRIESf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = bank_info->num_base_entries;
            f_cnt++;
        }

        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_BANK_INFOt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the read-only logical table DEVICE_EM_TILE.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_tile_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    int rv;
    const bcmlrd_map_t *map = NULL;
    uint64_t def_robust = 0, def_seed = 0;
    uint32_t num;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t tile_id;

    SHR_FUNC_ENTER(unit);

    /* Not to do intialization if this LT is not supported. */
    rv = bcmlrd_map_get(unit, DEVICE_EM_TILEt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(dev->var, SHR_E_PARAM);
    SHR_NULL_CHECK(dev->var->em_tile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlrd_field_default_get(unit, DEVICE_EM_TILEt,
                                 DEVICE_EM_TILEt_ROBUSTf,
                                 1, &def_robust, &num));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlrd_field_default_get(unit, DEVICE_EM_TILEt,
                                 DEVICE_EM_TILEt_SEEDf,
                                 1, &def_seed, &num));

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    if (warm) {
        SHR_IF_ERR_EXIT
            (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));
    }

    for (tile_id = 0; tile_id < dev->max_tiles; tile_id++) {
        in_flds.field[0]->id   = DEVICE_EM_TILEt_DEVICE_EM_TILE_IDf;
        in_flds.field[0]->idx  = 0;
        in_flds.field[0]->data = tile_id;
        if (warm) {
            in_flds.count = 1;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_TILEt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            } else if (rv != SHR_E_NOT_FOUND) {
                SHR_ERR_EXIT(rv);
            }
        }

        in_flds.field[1]->id   = DEVICE_EM_TILEt_ROBUSTf;
        in_flds.field[1]->idx  = 0;
        in_flds.field[1]->data = def_robust;

        in_flds.field[2]->id   = DEVICE_EM_TILEt_SEEDf;
        in_flds.field[2]->idx  = 0;
        in_flds.field[2]->data = def_seed;

        in_flds.field[3]->id   = DEVICE_EM_TILEt_MODEf;
        in_flds.field[3]->idx  = 0;
        in_flds.field[3]->data = dev->var->em_tile[tile_id].def_mode;
        in_flds.count = 4;

        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_TILEt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the read-only logical table DEVICE_EM_TILE_INFO.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_tile_info_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    int rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t idx, cnt;
    const bcmptm_uft_var_drv_t *var = dev->var;
    uint32_t f_cnt = 0, mode = 0;

    SHR_FUNC_ENTER(unit);

    /* Not to do intialization if this LT is not supported. */
    rv = bcmlrd_map_get(unit, DEVICE_EM_TILE_INFOt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(var, SHR_E_PARAM);
    SHR_NULL_CHECK(var->em_tile, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));

    for (idx = 0; idx < var->em_tile_cnt; idx++) {
        f_cnt = 0;
        in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = idx;
        f_cnt++;

        if (warm) {
            in_flds.count = f_cnt;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_TILE_INFOt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            } else if (rv != SHR_E_NOT_FOUND) {
                SHR_ERR_EXIT(rv);
            }
        }

        in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_INFOt_NUM_MODEf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = var->em_tile[idx].ava_mode_count;
        f_cnt++;

        for (cnt = 0; cnt < var->em_tile[idx].ava_mode_count; cnt++) {
            mode = var->em_tile[idx].ava_mode[cnt];
            in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_INFOt_MODEf;
            in_flds.field[f_cnt]->idx  = cnt;
            in_flds.field[f_cnt]->data = mode;
            f_cnt++;

            in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_INFOt_MODE_ATTRIBUTEf;
            in_flds.field[f_cnt]->idx  = cnt;
            in_flds.field[f_cnt]->data = var->tile_mode[mode]->attri;
            f_cnt++;
        }

        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_TILE_INFOt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the read-only logical table DEVICE_EM_TILE_MODE_INFO.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_tile_mode_info_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    int rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t idx, cnt;
    const bcmptm_uft_var_drv_t *var = dev->var;
    const em_tile_mode_info_t *tm = NULL;
    uint32_t f_cnt = 0;

    SHR_FUNC_ENTER(unit);

    /* Not to do intialization if this LT is not supported. */
    rv = bcmlrd_map_get(unit, DEVICE_EM_TILE_MODE_INFOt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(var, SHR_E_PARAM);
    SHR_NULL_CHECK(var->em_tile, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));

    for (idx = 0; idx < var->tile_mode_cnt; idx++) {
        tm = var->tile_mode[idx];
        f_cnt = 0;
        in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_MODE_INFOt_MODEf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = idx;
        f_cnt++;

        if (warm) {
            in_flds.count = f_cnt;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_TILE_MODE_INFOt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            } else if (rv != SHR_E_NOT_FOUND) {
                SHR_ERR_EXIT(rv);
            }
        }

        in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_MODE_INFOt_NUM_PDD_TYPEf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = tm->alpm_pdd_type_count;
        f_cnt++;

        for (cnt = 0; cnt < tm->alpm_pdd_type_count; cnt++) {
            in_flds.field[f_cnt]->id   = DEVICE_EM_TILE_MODE_INFOt_PDD_TYPEf;
            in_flds.field[f_cnt]->idx  = cnt;
            in_flds.field[f_cnt]->data = tm->alpm_pdd_type[cnt];
            f_cnt++;
        }

        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_TILE_MODE_INFOt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the update-only logical table TABLE_EM_CONTROL.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_tbl_em_ctrl_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmlrd_map_t *map = NULL;
    int rv;
    uint32_t i, j, attri;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(warm);

    rv = bcmlrd_map_get(unit, TABLE_EM_CONTROLt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));

    /* Just hash LTs in UFT_MODE_1 are shown. */
    for (i = 0; i < dev->max_grps; i++) {
        attri = dev->grp_info[i].attri;
        /* Only hash LT supports MOVE_DEPTH control. */
        if ((attri != EM_GROUP_ATTRI_EM) &&
            (attri != EM_GROUP_ATTRI_FP) &&
            (attri != EM_GROUP_ATTRI_FT) &&
            (attri != EM_GROUP_ATTRI_FT_FP)) {
            continue;
        }

        if (dev->grp_info[i].is_alpm) {
            continue;
        }

        for (j = 0; j < dev->grp_info[i].lt_cnt; j++) {
            in_flds.field[0]->id = TABLE_EM_CONTROLt_TABLE_IDf;
            in_flds.field[0]->idx = 0;
            in_flds.field[0]->data = dev->grp_info[i].lt_sids[j];
            in_flds.count = 1;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, TABLE_EM_CONTROLt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            }
            in_flds.field[0]->id   = TABLE_EM_CONTROLt_TABLE_IDf;
            in_flds.field[0]->idx  = 0;
            in_flds.field[0]->data = dev->grp_info[i].lt_sids[j];

            in_flds.field[1]->id   = TABLE_EM_CONTROLt_MOVE_DEPTHf;
            in_flds.field[1]->idx  = 0;
            in_flds.field[1]->data = BCMPTM_RM_HASH_DEF_HASH_REORDERING_DEPTH;

            in_flds.count = 2;
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit, TABLE_EM_CONTROLt, &in_flds));
        }
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the read-only logical table TABLE_EM_INFO.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_tbl_em_info_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmlrd_map_t *map = NULL;
    int rv;
    uint32_t i, j;
    uint32_t idx, fid, fval, grp_cnt = 0;
    bcmltd_sid_t ltid;
    bool lookup_cnt_valid = FALSE;
    uint32_t lookup_cnt = 0;

    SHR_FUNC_ENTER(unit);

    /* Not to do intialization if this LT is not supported. */
    rv = bcmlrd_map_get(unit, TABLE_EM_INFOt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    uft_lt_field_validate(unit, TABLE_EM_INFOt,
                          TABLE_EM_INFOt_LOOKUP_CNTf, &lookup_cnt_valid);

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));

    for (i = 0; i < dev->max_grps; i++) {
        for (j = 0; j < dev->grp_info[i].lt_cnt; j++) {
            ltid = dev->grp_info[i].lt_sids[j];
            in_flds.field[0]->id = TABLE_EM_INFOt_TABLE_IDf;
            in_flds.field[0]->idx = 0;
            in_flds.field[0]->data = ltid;
            in_flds.count = 1;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, TABLE_EM_INFOt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NOT_FOUND) {
                in_flds.field[0]->id   = TABLE_EM_INFOt_TABLE_IDf;
                in_flds.field[0]->idx  = 0;
                in_flds.field[0]->data = ltid;

                in_flds.field[1]->id   = TABLE_EM_INFOt_DEVICE_EM_GROUP_IDf;
                in_flds.field[1]->idx  = 0;
                in_flds.field[1]->data = dev->grp_info[i].grp_id;

                in_flds.field[2]->id   = TABLE_EM_INFOt_GROUP_CNTf;
                in_flds.field[2]->idx  = 0;
                in_flds.field[2]->data = 1;
                in_flds.count = 3;

                if (lookup_cnt_valid) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_uft_lt_lookup_cnt_get(unit, ltid, &lookup_cnt));
                    in_flds.field[3]->id   = TABLE_EM_INFOt_LOOKUP_CNTf;
                    in_flds.field[3]->idx  = 0;
                    in_flds.field[3]->data = lookup_cnt;
                    in_flds.count = 4;
                }
                SHR_IF_ERR_EXIT
                    (bcmimm_entry_insert(unit, TABLE_EM_INFOt, &in_flds));
            } else if (rv == SHR_E_NONE) {
                if (warm) {
                    continue;
                }
                /*
                 * ALPM UC LTs can belong up to 3 groups.
                 * Get current grp_cnt and update the new group.
                 */
                for (idx = 0; idx < out_flds.count; idx++) {
                    fid = out_flds.field[idx]->id;
                    fval = out_flds.field[idx]->data;
                    if (fid == TABLE_EM_INFOt_GROUP_CNTf) {
                        grp_cnt = fval;
                        break;
                    }
                }
                in_flds.field[0]->id   = TABLE_EM_INFOt_TABLE_IDf;
                in_flds.field[0]->idx  = 0;
                in_flds.field[0]->data = ltid;

                in_flds.field[1]->id   = TABLE_EM_INFOt_DEVICE_EM_GROUP_IDf;
                in_flds.field[1]->idx  = grp_cnt;
                in_flds.field[1]->data = dev->grp_info[i].grp_id;

                in_flds.field[2]->id   = TABLE_EM_INFOt_GROUP_CNTf;
                in_flds.field[2]->idx  = 0;
                in_flds.field[2]->data = grp_cnt + 1;
                in_flds.count = 3;
                SHR_IF_ERR_EXIT
                    (bcmimm_entry_update(unit, TRUE, TABLE_EM_INFOt, &in_flds));
            } else {
                SHR_ERR_EXIT(rv);
            }
        }
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the update-only logical table DEVICE_EM_GROUP.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_grp_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t f_cnt = 0;
    uft_grp_info_t *grp = NULL;
    int rv = SHR_E_NONE;
    uint32_t num_actual = 0;
    bool grp_robust_valid = false;
    uint64_t def_robust = 0;
    uint32_t idx, bank_idx;
    uint32_t invalid_grp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    if (warm) {
        SHR_IF_ERR_EXIT
            (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));
    }

    rv = bcmlrd_field_default_get(unit, DEVICE_EM_GROUPt,
                                  DEVICE_EM_GROUPt_ROBUSTf,
                                  1, &def_robust, &num_actual);
    if (rv == SHR_E_NONE) {
        grp_robust_valid = true;
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit, TABLE_EM_INFOt,
                                      TABLE_EM_INFOt_DEVICE_EM_GROUP_IDf,
                                      "INVALID", &invalid_grp));

    for (idx = 0; idx < dev->max_grps; idx++) {
        f_cnt = 0;
        grp = &(dev->grp_info[idx]);
        if (grp->grp_id == invalid_grp) {
            continue;
        }
        in_flds.field[f_cnt]->id   = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = grp->grp_id;
        f_cnt++;
        if (warm) {
            in_flds.count = f_cnt;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            } else if (rv != SHR_E_NOT_FOUND) {
                SHR_ERR_EXIT(rv);
            }
        }

        if (grp_robust_valid) {
            in_flds.field[f_cnt]->id   = DEVICE_EM_GROUPt_ROBUSTf;
            in_flds.field[f_cnt]->idx  = 0;
            in_flds.field[f_cnt]->data = def_robust;
            f_cnt++;
        }

        in_flds.field[f_cnt]->id   = DEVICE_EM_GROUPt_VECTOR_TYPEf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = grp->vector_type;
        f_cnt++;

        in_flds.field[f_cnt]->id   = DEVICE_EM_GROUPt_NUM_BANKSf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = grp->var_cnt;
        f_cnt++;

        for (bank_idx = 0; bank_idx < grp->var_cnt; bank_idx++) {
            in_flds.field[f_cnt]->id   = DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf;
            in_flds.field[f_cnt]->idx  = bank_idx;
            in_flds.field[f_cnt]->data = grp->var_banks[bank_idx];
            f_cnt++;
        }

        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_GROUPt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialzie the read-only logical table DEVICE_EM_GROUP_INFO.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_dev_em_grp_info_lt_init(int unit, uft_dev_info_t *dev, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t f_cnt = 0;
    uft_grp_info_t *grp = NULL;
    int rv = SHR_E_NONE;
    uint32_t idx, bank_idx;
    uint32_t lt_idx;
    bool attri_valid = FALSE;
    uint64_t def_value;
    uint32_t num;
    uint32_t invalid_grp;
    const bcmptm_uft_var_drv_t *var = dev->var;
    const em_grp_info_t *em_grp;
    bool lookup0_lt_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUP_INFOt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    rv = bcmlrd_field_default_get(unit, DEVICE_EM_GROUP_INFOt,
                                  DEVICE_EM_GROUP_INFOt_ATTRIBUTESf,
                                  1, &def_value, &num);
    if (rv == SHR_E_NONE) {
        attri_valid = TRUE;
    }

    rv = bcmlrd_field_default_get(unit, DEVICE_EM_GROUP_INFOt,
                                  DEVICE_EM_GROUP_INFOt_LOOKUP0_LTf,
                                  1, &def_value, &num);
    if (rv == SHR_E_NONE) {
        lookup0_lt_valid = TRUE;
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit, TABLE_EM_INFOt,
                                      TABLE_EM_INFOt_DEVICE_EM_GROUP_IDf,
                                      "INVALID", &invalid_grp));

    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &in_flds, UFT_FIELD_COUNT_MAX));
    SHR_IF_ERR_EXIT
        (uft_lt_famm_alloc(unit, &out_flds, UFT_FIELD_COUNT_MAX));

    for (idx = 0; idx < dev->max_grps; idx++) {
        grp = &(dev->grp_info[idx]);
        if (grp->grp_id == invalid_grp) {
            continue;
        }

        f_cnt = 0;
        in_flds.field[f_cnt]->id   = DEVICE_EM_GROUP_INFOt_DEVICE_EM_GROUP_IDf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = grp->grp_id;
        f_cnt++;

        if (warm) {
            in_flds.count = f_cnt;
            out_flds.count = UFT_FIELD_COUNT_MAX;
            rv = bcmimm_entry_lookup(unit, DEVICE_EM_GROUP_INFOt,
                                     &in_flds, &out_flds);
            if (rv == SHR_E_NONE) {
                continue;
            } else if (rv != SHR_E_NOT_FOUND) {
                SHR_ERR_EXIT(rv);
            }
        }

        in_flds.field[f_cnt]->id   = DEVICE_EM_GROUP_INFOt_NUM_FIXED_BANKSf;
        in_flds.field[f_cnt]->idx  = 0;
        in_flds.field[f_cnt]->data = grp->fixed_cnt;
        f_cnt++;

        for (bank_idx = 0; bank_idx < grp->fixed_cnt; bank_idx++) {
            in_flds.field[f_cnt]->id =
                DEVICE_EM_GROUP_INFOt_FIXED_DEVICE_EM_BANK_IDf;
            in_flds.field[f_cnt]->idx  = bank_idx;
            in_flds.field[f_cnt]->data = grp->fixed_banks[bank_idx];
            f_cnt++;
        }

        if (attri_valid) {
            in_flds.field[f_cnt]->id   = DEVICE_EM_GROUP_INFOt_ATTRIBUTESf;
            in_flds.field[f_cnt]->idx  = 0;
            in_flds.field[f_cnt]->data = grp->attri;
            f_cnt++;
            in_flds.field[f_cnt]->id   = DEVICE_EM_GROUP_INFOt_NUM_ATTRIBUTESf;
            in_flds.field[f_cnt]->idx  = 0;
            in_flds.field[f_cnt]->data = 1;
            f_cnt++;
        }

        if (lookup0_lt_valid) {
            em_grp = var->em_group[grp->grp_id];
            in_flds.field[f_cnt]->id = DEVICE_EM_GROUP_INFOt_TILE_MODEf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = em_grp->tile_mode;
            f_cnt++;

            for (lt_idx = 0; lt_idx < em_grp->lookup0_lt_count; lt_idx++) {
                in_flds.field[f_cnt]->id =
                    DEVICE_EM_GROUP_INFOt_LOOKUP0_LTf;
                in_flds.field[f_cnt]->idx = lt_idx;
                in_flds.field[f_cnt]->data = em_grp->lookup0_lt[lt_idx];
                f_cnt++;
            }
            in_flds.field[f_cnt]->id =
                DEVICE_EM_GROUP_INFOt_NUM_LOOKUP0_LTf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = em_grp->lookup0_lt_count;
            f_cnt++;

            for (lt_idx = 0; lt_idx < em_grp->lookup1_lt_count; lt_idx++) {
                in_flds.field[f_cnt]->id =
                    DEVICE_EM_GROUP_INFOt_LOOKUP1_LTf;
                in_flds.field[f_cnt]->idx = lt_idx;
                in_flds.field[f_cnt]->data = em_grp->lookup1_lt[lt_idx];
                f_cnt++;
            }
            in_flds.field[f_cnt]->id =
                DEVICE_EM_GROUP_INFOt_NUM_LOOKUP1_LTf;
            in_flds.field[f_cnt]->idx = 0;
            in_flds.field[f_cnt]->data = em_grp->lookup1_lt_count;
            f_cnt++;
        }

        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_GROUP_INFOt, &in_flds));
    }

exit:
    uft_lt_famm_free(unit, &in_flds, UFT_FIELD_COUNT_MAX);
    uft_lt_famm_free(unit, &out_flds, UFT_FIELD_COUNT_MAX);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover each EM group's assigned bank map information from IMM table.
 *
 * \param [in] unit Logical device ID.
 * \param [in] dev Uft_dev_info_t pointer.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_grp_bmp_info_recover(int unit, uft_dev_info_t *dev)
{
    uint32_t grp_id;
    bcmltd_fields_t input = {0}, output = {0};
    size_t idx;
    uint32_t fid, fidx;
    uint64_t fval;
    uint32_t cnt = 0;
    uint32_t bank_arr[DEVICE_EM_BANK_CNT_MAX] = {0};
    uint32_t invalid_grp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      DEVICE_EM_BANK_INFOt,
                                      DEVICE_EM_BANK_INFOt_DEVICE_EM_GROUP_IDf,
                                      "INVALID", &invalid_grp));

    input.count  = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    for (grp_id = 0; grp_id < dev->max_grps; grp_id++) {
        if (grp_id == invalid_grp) {
            continue;
        }
        input.field[0]->id = DEVICE_EM_GROUP_INFOt_DEVICE_EM_GROUP_IDf;
        input.field[0]->data = grp_id;
        output.count = UFT_FIELD_COUNT_MAX;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_lookup(unit, DEVICE_EM_GROUP_INFOt, &input, &output));
        for (idx = 0; idx < output.count; idx++) {
            fid = output.field[idx]->id;
            fidx = output.field[idx]->idx;
            fval = output.field[idx]->data;
            switch (fid) {
            case DEVICE_EM_GROUP_INFOt_NUM_FIXED_BANKSf:
                cnt = fval;
                break;
            case DEVICE_EM_GROUP_INFOt_FIXED_DEVICE_EM_BANK_IDf:
                bank_arr[fidx] = fval;
                break;
            default:
                break;
            }
        }
        for (idx = 0; idx < cnt; idx++) {
            SHR_BITSET(grp_bmp_info[unit][grp_id].bm, bank_arr[idx]);
        }
    }

    for (grp_id = 0; grp_id < dev->max_grps; grp_id++) {
        if (grp_id == invalid_grp) {
            continue;
        }
        input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
        input.field[0]->data = grp_id;
        output.count = UFT_FIELD_COUNT_MAX;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt, &input, &output));
        for (idx = 0; idx < output.count; idx++) {
            fid = output.field[idx]->id;
            fidx = output.field[idx]->idx;
            fval = output.field[idx]->data;
            switch (fid) {
            case DEVICE_EM_GROUPt_NUM_BANKSf:
                cnt = fval;
                break;
            case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
                bank_arr[fidx] = fval;
                break;
            default:
                break;
            }
        }
        for (idx = 0; idx < cnt; idx++) {
            SHR_BITSET(grp_bmp_info[unit][grp_id].bm, bank_arr[idx]);
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize UFT module info.
 *
 * \param [in] unit Logical device ID.
 * \param [in] warm Warmboot flag.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
uft_info_init(int unit, bool warm)
{
    uft_dev_info_t *dev_info;
    uint32_t grp_bmp_sz;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmptm_uft_device_init(unit, warm));
    dev_info = bcmptm_uft_device_info_get(unit);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);
    SHR_IF_ERR_EXIT(bcmptm_uft_imm_register_init(unit));

    grp_bmp_sz = dev_info->max_grps * sizeof(bcmptm_uft_bank_map_t);
    grp_bmp_info[unit] = sal_alloc(grp_bmp_sz, "bcmptmUftGrpBankBmpInfo");
    SHR_NULL_CHECK(grp_bmp_info[unit], SHR_E_MEMORY);
    sal_memset(grp_bmp_info[unit], 0, grp_bmp_sz);

    if (!warm || bcmissu_is_active()) {
        SHR_IF_ERR_EXIT(uft_dev_em_bank_info_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_bank_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_bank_pair_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_grp_info_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_grp_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_tile_info_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_tile_mode_info_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_dev_em_tile_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_tbl_em_ctrl_lt_init(unit, dev_info, warm));
        SHR_IF_ERR_EXIT(uft_tbl_em_info_lt_init(unit, dev_info, warm));
    }
    if (!warm) {
        sal_memcpy(grp_bmp_info[unit], dev_info->grp_c_bmp_arr, grp_bmp_sz);
    } else {
        SHR_IF_ERR_EXIT
            (uft_grp_bmp_info_recover(unit, dev_info));
        sal_memcpy(dev_info->grp_c_bmp_arr, grp_bmp_info[unit], grp_bmp_sz);
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(grp_bmp_info[unit]);
        (void)bcmptm_uft_device_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup UFT module info.
 *
 * \param [in] unit Logical device ID.
 *
 * \retval NONE.
 */
static void
uft_info_cleanup(int unit)
{
    SHR_FREE(grp_bmp_info[unit]);
    (void)bcmptm_uft_device_cleanup(unit);
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_uft_sbr_entry_alloc(int unit,
                           bcmdrd_sid_t spt_sid,
                           bcmltd_sid_t lt_id,
                           bcmptm_uft_sbr_lt_lookup_t lookup_type,
                           bool paired_lookups,
                           uint16_t entry_count,
                           uint16_t *entry_index)
{
    uft_dev_info_t *dev_info = NULL;

    SHR_FUNC_ENTER(unit);
    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_sbr_entry_alloc(unit, spt_sid, lt_id,
                                lookup_type, paired_lookups,
                                entry_count, entry_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_sbr_entry_free(int unit,
                          bcmdrd_sid_t spt_sid,
                          uint16_t entry_count,
                          uint16_t *entry_index)
{
    uft_dev_info_t *dev_info = NULL;

    SHR_FUNC_ENTER(unit);
    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_sbr_entry_free(unit, spt_sid, entry_count, entry_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_sbr_multi_tables_entry_alloc(int unit,
                                        uint16_t spt_sid_count,
                                        bcmdrd_sid_t *spt_sid,
                                        bcmltd_sid_t lt_id,
                                        bcmptm_uft_sbr_lt_lookup_t lookup_type,
                                        bool paired_lookups,
                                        uint16_t entry_count,
                                        uint16_t *entry_index)
{
    bool in_use = FALSE;
    uint16_t t, pt, i, ltbl_sz = 0;
    uint16_t *eindex = NULL;
    uint32_t spt_ecnt = 0;
    bcmptm_sbr_entry_state_t *entry_state = NULL;
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    uft_dev_info_t *dev_info = NULL;

    SHR_FUNC_ENTER(unit);

    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(spt_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);

    if (spt_sid_count == 0 || entry_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (spt_sid_count == 1) {
        SHR_ERR_EXIT
            (bcmptm_sbr_entry_alloc(unit, *spt_sid, lt_id,
                                    lookup_type, paired_lookups,
                                    entry_count, entry_index));
    } else {
        /* Get the total entries in the first SBR profile table. */
        spt_ecnt = bcmptm_pt_index_count(unit, spt_sid[0]);
        /* Compute the table size based on logical lookup type. */
        ltbl_sz = paired_lookups ? (spt_ecnt / 2) : spt_ecnt;
        /*
         * Allocate array memory to store the allocated entry indices. The
         * maximum number of entry alloc attempts in the spt_sid[0] profile
         * table will be equal to ltbl_sz count. In each attempt, entry_count
         * number of entries have to be allocated.
         */
        eindex = sal_alloc((sizeof(*eindex) * spt_ecnt),
                            "bcmptmUftSbrEntIdxArr");
        SHR_NULL_CHECK(eindex, SHR_E_MEMORY);
        for (i = 0; i < spt_ecnt; i++) {
            eindex[i] = SBR_UINT16_INVALID;
        }
        for (t = 0; t < ltbl_sz; t++) {
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_entry_alloc(unit, spt_sid[0], lt_id, lookup_type,
                                        paired_lookups, entry_count,
                                        (eindex + (t * entry_count))));
            in_use = FALSE;
            for (pt = 1; pt < spt_sid_count; pt++) {
                for (i = 0; i < entry_count; i++) {
                    entry_state = NULL;
                    spt_state = NULL;
                    SHR_IF_ERR_EXIT
                        (bcmptm_sbr_entry_state_get(unit, spt_sid[pt],
                                                eindex[(t * entry_count) + i],
                                                &entry_state, &spt_state));
                    SHR_NULL_CHECK(entry_state, SHR_E_INTERNAL);
                    SHR_NULL_CHECK(spt_state, SHR_E_INTERNAL);
                    if (entry_state->usage_mode == BCMPTM_SBR_ENTRY_IN_USE) {
                        /* Entry already in-use in this profile table. */
                        in_use = TRUE;
                        break;
                    }
                }
                if (in_use) {
                    break;
                }
            }
            if (in_use) {
                /*
                 * Entry indices allocated in spt_sid[0] profile table are
                 * already in-use in (spt_sid[pt],
                 * eindex[(t * entry_count) + i]). So, allocate the next entry
                 * or entry_pair (for paired_lookup) in spt_sid[0] profile
                 * table and check for their availability in the rest of the
                 * spt_sid[] profile tables in the list.
                 */
                continue;
            }
            /*
             * Now free the entries allocated for spt_sid[0] that were not used
             * as one of the other SBR profile table had these entries in-use.
             */
            for (i = 0; i < t; i++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_sbr_entry_free(unit, spt_sid[0], entry_count,
                                           (eindex + (i * entry_count))));
            }
            /*
             * Retrieve and update the state of entries for profile tables in
             * the range spt_sid[1] to spt_sid[spt_sid_count - 1].
             */
            for (pt = 1; pt < spt_sid_count; pt++) {
                for (i = 0; i < entry_count; i++) {
                    entry_state = NULL;
                    spt_state = NULL;
                    SHR_IF_ERR_EXIT
                        (bcmptm_sbr_entry_state_get(unit, spt_sid[pt],
                                                eindex[(t * entry_count) + i],
                                                &entry_state, &spt_state));
                    SHR_NULL_CHECK(entry_state, SHR_E_INTERNAL);
                    SHR_NULL_CHECK(spt_state, SHR_E_INTERNAL);
                    entry_state->usage_mode = BCMPTM_SBR_ENTRY_IN_USE;
                    entry_state->ltid = lt_id;
                    entry_state->lookup_type = lookup_type;
                    entry_state->ref_count++;
                    spt_state->free_entries_count--;
                    if (pt == (spt_sid_count - 1)) {
                        /*
                         * Populate allocated entry index to return to
                         * callee.
                         */
                        *(entry_index + i) = eindex[(t * entry_count) + i];
                    }
                }
            }
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    if (eindex != NULL) {
        if (SHR_FUNC_ERR() && spt_sid && spt_sid_count > 1 && entry_index
            && entry_count > 0 && ltbl_sz > 0) {
            /*
             * For error return case, free all entries previously allocated
             * from spt_sid[0] profile table before exiting this function.
             */
            for (t = 0; t < ltbl_sz; t++) {
                i = t * entry_count;
                if (eindex && eindex[i] != SBR_UINT16_INVALID) {
                   (void) bcmptm_sbr_entry_free(unit, spt_sid[0], entry_count,
                                                (eindex + i));
                }
            }
        }
        sal_free(eindex);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_sbr_multi_tables_entry_free(int unit,
                                       uint16_t spt_sid_count,
                                       bcmdrd_sid_t *spt_sid,
                                       uint16_t entry_count,
                                       uint16_t *entry_index)
{
    uint16_t pt, i;
    uft_dev_info_t *dev_info = NULL;
    bcmptm_sbr_entry_state_t *entry_state = NULL;
    bcmptm_sbr_pt_state_t *spt_state = NULL;

    SHR_FUNC_ENTER(unit);

    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(spt_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);

    /*
     * Verify the input entry or entry_pair is in-use in all the profile tables
     * provided as input in spt_sid[] array, prior to freeing them.
     */
    for (pt = 0; pt < spt_sid_count; pt++) {
        for (i = 0; i < entry_count; i++) {
            entry_state = NULL;
            spt_state = NULL;
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_entry_state_get(unit, spt_sid[pt], entry_index[i],
                                            &entry_state, &spt_state));
            SHR_NULL_CHECK(entry_state, SHR_E_INTERNAL);
            SHR_NULL_CHECK(spt_state, SHR_E_INTERNAL);
            if (entry_state->usage_mode != BCMPTM_SBR_ENTRY_IN_USE) {
                /* Entry not in-use in this profile table. */
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "SPT=%s entry_index=%u not in-use to"
                                        "free, usage_mode=%u - rv=%s.\n"),
                             bcmdrd_pt_sid_to_name(unit, spt_sid[pt]),
                             entry_index[i], entry_state->usage_mode,
                             shr_errmsg(SHR_E_NOT_FOUND)));
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
        }
    }
    /* Free the entries. */
    for (pt = 0; pt < spt_sid_count; pt++) {
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_entry_free(unit, spt_sid[pt], entry_count,
                                   entry_index));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_sbr_entry_index_get(int unit,
                               bcmdrd_sid_t spt_sid,
                               bcmltd_sid_t lt_id,
                               bcmptm_uft_sbr_lt_lookup_t lookup_type,
                               uint16_t *entry_index)
{
    uft_dev_info_t *dev_info = NULL;

    SHR_FUNC_ENTER(unit);
    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_sbr_entry_index_get(unit, spt_sid, lt_id, lookup_type,
                                    entry_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_sbr_tile_mode_commit(int unit)
{
    uft_dev_info_t *dev_info = NULL;

    SHR_FUNC_ENTER(unit);
    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_sbr_tile_mode_commit(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_sbr_tile_mode_abort(int unit)
{
    uft_dev_info_t *dev_info = NULL;

    SHR_FUNC_ENTER(unit);
    dev_info = bcmptm_uft_device_info_get(unit);
    if (dev_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_sbr_tile_mode_abort(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_grp_commit(int unit)
{
    uft_dev_info_t *dev_info;
    uint32_t grp_bmp_sz;

    SHR_FUNC_ENTER(unit);

    dev_info = bcmptm_uft_device_info_get(unit);
    grp_bmp_sz = dev_info->max_grps * sizeof(bcmptm_uft_bank_map_t);
    sal_memcpy(grp_bmp_info[unit], dev_info->grp_c_bmp_arr, grp_bmp_sz);

    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_grp_abort(int unit)
{
    uft_dev_info_t *dev_info;
    uint32_t grp_bmp_sz;

    SHR_FUNC_ENTER(unit);

    dev_info = bcmptm_uft_device_info_get(unit);
    grp_bmp_sz = dev_info->max_grps * sizeof(bcmptm_uft_bank_map_t);
    sal_memcpy(dev_info->grp_c_bmp_arr, grp_bmp_info[unit], grp_bmp_sz);

    SHR_FUNC_EXIT();
}

int
bcmptm_uft_init(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUPt, &map);
    if (!(SHR_SUCCESS(rv) && map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(uft_info_init(unit, warm));
    if (!warm) {
        SHR_IF_ERR_EXIT(bcmptm_uft_regs_init(unit));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_sbr_init(unit, warm));
    SHR_IF_ERR_EXIT
        (bcmptm_spm_init(unit, warm));
    SHR_IF_ERR_EXIT
        (bcmptm_itx_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_cleanup(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUPt, &map);
    if (!(SHR_SUCCESS(rv) && map)) {
        SHR_EXIT();
    }

    uft_info_cleanup(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_sbr_cleanup(unit, warm));
    SHR_IF_ERR_EXIT
        (bcmptm_spm_cleanup(unit, warm));
    SHR_IF_ERR_EXIT
        (bcmptm_itx_cleanup(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_fld_hdl_init(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!uft_fld_arr_hdl) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(UFT_FIELD_COUNT_MAX, &uft_fld_arr_hdl));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_uft_fld_hdl_cleanup(void)
{
    if (uft_fld_arr_hdl) {
        shr_famm_hdl_delete(uft_fld_arr_hdl);
        uft_fld_arr_hdl = NULL;
    }
}

