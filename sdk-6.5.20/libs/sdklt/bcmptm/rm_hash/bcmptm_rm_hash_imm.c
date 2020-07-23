/*! \file bcmptm_rm_hash_imm.c
 *
 * IMM table for Hash Table
 *
 * This file contains the manager for TABLE_HASH_STORE_INFO table.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_imm.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Private functions
 */

static int
rm_hash_lt_fields_buff_free(int unit, bcmltd_fields_t *in)
{
    SHR_FUNC_ENTER(unit);

    if (!in) {
        SHR_EXIT();
    }
    if (in->field) {
        SHR_FREE(in->field[0]);
        SHR_FREE(in->field);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
rm_hash_lt_fields_buff_alloc(int unit, bcmltd_fields_t *in)
{
    uint32_t idx = 0;
    bcmltd_field_t *f_ptr = NULL;
    size_t cnt;

    SHR_FUNC_ENTER(unit);

    if (!in || (in->count < 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    cnt = in->count;
    in->field = sal_alloc(cnt * sizeof(bcmltd_field_t *),
                          "bcmptmRmhashLtFieldPtArr");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, cnt * sizeof(bcmltd_field_t *));

    /* Allocate all fields struct at one time. */
    f_ptr = sal_alloc(cnt * sizeof(bcmltd_field_t), "bcmptmRmhashLtFieldArr");
    SHR_NULL_CHECK(f_ptr, SHR_E_MEMORY);
    sal_memset(f_ptr, 0, cnt * sizeof(bcmltd_field_t));

    for (idx = 0; idx < cnt; idx++) {
        in->field[idx] = f_ptr + idx;
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void) rm_hash_lt_fields_buff_free(unit, in);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_hash_store_info_update(int unit,
                                 bcmltd_sid_t ltid,
                                 uint32_t hash_store)
{
    int rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t f_cnt = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TABLE_HASH_STORE_INFOt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    in_flds.count = TABLE_HASH_STORE_INFOt_FIELD_COUNT;
    out_flds.count = TABLE_HASH_STORE_INFOt_FIELD_COUNT;
    SHR_IF_ERR_VERBOSE_EXIT
        (rm_hash_lt_fields_buff_alloc(unit, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (rm_hash_lt_fields_buff_alloc(unit, &out_flds));
    in_flds.field[f_cnt]->id = TABLE_HASH_STORE_INFOt_TABLE_IDf;
    in_flds.field[f_cnt]->idx = 0;
    in_flds.field[f_cnt]->data = ltid;
    f_cnt++;
    in_flds.count = f_cnt;

    rv = bcmimm_entry_lookup(unit, TABLE_HASH_STORE_INFOt, &in_flds, &out_flds);
    in_flds.field[f_cnt]->id = TABLE_HASH_STORE_INFOt_HASH_STOREf;
    in_flds.field[f_cnt]->idx = 0;
    in_flds.field[f_cnt]->data = hash_store;
    f_cnt++;
    in_flds.count = f_cnt;
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_insert(unit, TABLE_HASH_STORE_INFOt, &in_flds));
    } else if (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_update(unit, 0, TABLE_HASH_STORE_INFOt, &in_flds));
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    (void) rm_hash_lt_fields_buff_free(unit, &in_flds);
    (void) rm_hash_lt_fields_buff_free(unit, &out_flds);
    SHR_FUNC_EXIT();
}

