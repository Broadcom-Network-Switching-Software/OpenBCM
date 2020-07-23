/*! \file bcmltx_hash_entry_index.c
 *
 * Hash Logic table Handler Header file.
 * This file contains entry index field transform information for
 * hash Logic table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmltx/general/bcmltx_hash_entry_index.h>

/******************************************************************************
* Local definitions
 */
#define PHYSICAL_ENTRY_MAX_COUNT  2
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/******************************************************************************
* Private functions
 */

/******************************************************************************
* Public functions
 */
/*!
 * \brief opaque tag id transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            L2_OPAQUE_TAG_ID field.
 * \param [out] out         __TABLE_SEL fields.
 * \param [in]  arg           Transform arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_hash_entry_index_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    uint32_t entry_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_hit_index_info_get(unit, arg->value[0], &entry_index));

    out->field[0]->data = entry_index;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

    out->count = 1;

exit:
    SHR_FUNC_EXIT();
}
