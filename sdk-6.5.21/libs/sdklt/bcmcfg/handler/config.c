/*! \file config.c
 *
 * Config table custom table handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_config.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_TABLE

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

int
bcmcfg_config_validate(int unit,
                       bcmlt_opcode_t opcode,
                       const bcmltd_fields_t *in,
                       const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);
    SHR_FUNC_EXIT();
}

int
bcmcfg_config_insert(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);
    SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
    SHR_FUNC_EXIT();
}

int
bcmcfg_config_lookup(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    const bcmlrd_map_t *map;
    size_t fid;
    size_t actual;
    size_t available;
    size_t cnt;
    size_t idx;

    int rv;

    SHR_FUNC_ENTER(unit);
    do {
        /* Look up table. */
        rv = bcmlrd_map_get(unit, arg->sid, &map);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        /* Copy fields out. */
        available = out->count;
        out->count = 0;
        for (fid = 0; fid < map->field_data->fields; fid++) {
            uint64_t *fvalue = NULL;

            if (map->field_data->field[fid].flags & BCMLRD_FIELD_F_UNMAPPED) {
                /* This field is unmapped, so skip it. */
                continue;
            }

            if (out->count >= available) {
                /* Ran out of room, so stop. Normally shouldn't happen. */
                SHR_IF_ERR_CONT(SHR_E_INTERNAL);
                break;
            }

            cnt = bcmlrd_field_idx_count_get(unit,
                                             arg->sid,
                                             fid);

            fvalue = sal_alloc(cnt * sizeof(uint64_t), "bcmcfgConfigTableFieldValue");
            if (fvalue == NULL) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META("Error allocating memory for config data.\n")));
                SHR_IF_ERR_CONT(SHR_E_MEMORY);
                break;
            }
            sal_memset(fvalue , 0, cnt * sizeof(uint64_t));

            rv = bcmcfg_table_get_internal(unit,
                                           arg->sid,
                                           fid,
                                           0,
                                           cnt,
                                           fvalue,
                                           &actual);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_CONT(rv);
                sal_free(fvalue);
                break;
            }
            for (idx = 0; idx < actual; idx++) {
                out->field[out->count]->id = fid;
                out->field[out->count]->idx = idx;
                out->field[out->count]->data = fvalue[idx];
                out->count++;
            }

            sal_free(fvalue);
        }
    } while (0);
    SHR_FUNC_EXIT();
}

int
bcmcfg_config_delete(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);
    SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
    SHR_FUNC_EXIT();
}

int
bcmcfg_config_update(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);
    SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
    SHR_FUNC_EXIT();
}

int
bcmcfg_config_first(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out,
                    const bcmltd_generic_arg_t *arg)
{
    COMPILER_REFERENCE(in);
    return bcmcfg_config_lookup(unit, op_arg, NULL, out, arg);
}

int
bcmcfg_config_next(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);
    SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
    SHR_FUNC_EXIT();
}

