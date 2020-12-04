/*! \file inmem_direct_hlr.c
 *
 * In memory direct table handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmimm/bcmimm_direct.h>
#include <bcmimm/bcmimm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

/*******************************************************************************
 * Public functions
 */
int bcmimm_direct_validate(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_basic_validate(unit, opcode, in, arg));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_insert(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    NULL,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_insert_allocate(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_insert(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    out,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}


int bcmimm_direct_lookup(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_lookup(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    out,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_delete(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    BCMIMM_TABLE_NB));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_update(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_commit(int unit,
                         uint32_t trans_id,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_basic_commit(unit, trans_id, arg));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_abort(int unit,
                        uint32_t trans_id,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_basic_abort(unit, trans_id, arg));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_get_first(unit,
                                       op_arg,
                                       arg->sid,
                                       out,
                                       BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_get_next(unit,
                                      op_arg,
                                      arg->sid,
                                      in,
                                      out,
                                      BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_table_entry_inuse_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_inuse_arg_t *table_arg,
        bcmltd_table_entry_inuse_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_basic_table_entry_inuse_get(unit,
                                            trans_id,
                                            sid,
                                            table_arg,
                                            table_data,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_table_max_entries_set(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_max_entries_arg_t *table_arg,
        const bcmltd_table_max_entries_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_basic_table_max_entries_set(unit,
                                            trans_id,
                                            sid,
                                            table_arg,
                                            table_data,
                                            arg));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_table_entry_limit_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_limit_arg_t *table_arg,
        bcmltd_table_entry_limit_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    table_info_t *tbl;
    uint32_t idx;
    bcmimm_lt_ent_lmt_get_cb *cb_func;
    uint32_t ent_limit;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, &idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    /* Call the component if it provided a callback for this event */
    cb_func = bcmimm_tables_cb[unit][idx].cb.ent_limit_get;
    if (cb_func) {
        SHR_IF_ERR_EXIT(cb_func(unit, trans_id, sid, &ent_limit));
        table_data->entry_limit = ent_limit;
    } else {
        table_data->entry_limit = table_arg->entry_maximum;
    }

exit:
    SHR_FUNC_EXIT();
}


