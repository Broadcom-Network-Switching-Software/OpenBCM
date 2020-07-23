/*! \file bcmgene_editor.c
 *
 * Broadcom Generic Extensible NPL Encapsulation.
 * This file contains GENE database of devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/chip/bcmgene_editor.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

/******************************************************************************
 * Private functions
 */

static int
bcmgene_sub_feature_get(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t func,
    uint32_t *sub_f)
{
    bcmgene_func_desc_t *function = NULL;
    bcmgene_table_desc_t *tbl = NULL;
    uint32_t f = 0;
    int i, j;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gene, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(sub_f, SHR_E_PARAM);

    for (i = 0; i < gene->funcs; i++) {
        function = &gene->func[i];
        if (func != function->func) {
            continue;
        }
        for (j = 0; j < function->tbls; j++) {
            tbl = &function->tbl[j];
            f |= tbl->sub_f;
        }
    }

    *sub_f = f;

exit:
    SHR_FUNC_EXIT();
}

static void
bcmgene_handle_t_init(bcmgene_handle_t *hndl)
{
    if (hndl) {
        sal_memset(hndl, 0, sizeof(*hndl));
    }
}

static int
bcmgene_targeting(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t func,
    uint32_t sub_f,
    bcmgene_handle_t *hndl)
{
    bcmgene_func_desc_t *function = NULL;
    int i, j, found = 0;
    bcmgene_table_desc_t *tbl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gene, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    bcmgene_handle_t_init(hndl);

    for (i = 0; i < gene->funcs; i++) {
        function = &gene->func[i];
        if (function->func == func) {
            for (j = 0; j < function->tbls; j++) {
                tbl = &function->tbl[j];
                if (tbl->sub_f & sub_f) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
    if (!found) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hndl->unit = unit;
    hndl->tbl = tbl;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_batch_handle_create(bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->batch_handle == BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                        &hndl->batch_handle));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_batch_handle_destroy(bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->batch_handle != BCMLT_INVALID_HDL) {
        (void)bcmlt_transaction_free(hndl->batch_handle);
        hndl->batch_handle = BCMLT_INVALID_HDL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_handle_create(bcmgene_handle_t *hndl)
{
    int dunit;
    bcmgene_table_desc_t *tbl = NULL;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    tbl = hndl->tbl;
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    dunit = bcmi_ltsw_dev_dunit(hndl->unit);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, tbl->table, &hndl->handle));
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_batch_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_handle_destroy(bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(hndl->handle);
        hndl->handle = BCMLT_INVALID_HDL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_validate_tag(
    bcmgene_handle_t *hndl,
    int blk,
    int offset)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    int i, supported;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk &&
            fld->offset == offset) {
            supported = 1;
            break;
        }
    }
    if (!supported) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_validate_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t value)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, found = 0, supported;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk && fld->offset == offset) {
            supported = 1;
            if (fld->converter) {
                cvt = fld->converter;
                found = 0;
                for (j = 0; j < cvt->maps; j++) {
                    map = &cvt->map[j];
                    if (value == map->in) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            break;
        }
    }
    if (!supported) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_validate_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    int index,
    uint64_t value)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, found = 0, supported;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk &&
            fld->offset == offset) {
            supported = 1;
            if (fld->depth > 1) {
                if (index >= fld->depth) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                if (fld->converter) {
                    cvt = fld->converter;
                    found = 0;
                    for (j = 0; j < cvt->maps; j++) {
                        map = &cvt->map[j];
                        if (value == map->in) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                }
            }
            break;
        }
    }
    if (!supported) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_flags(
    bcmgene_handle_t *hndl,
    uint32_t flags)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    hndl->flags = flags;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_key(
    bcmgene_handle_t *hndl,
    int index,
    uint64_t value)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *key = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int j, found;
    const char *valsym;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);
    SHR_NULL_CHECK(hndl->tbl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    tbl = hndl->tbl;
    if (index >= 0 && index < tbl->keys) {
        key = &tbl->key[index];
        if (key->converter) {
            cvt = key->converter;
            found = 0;
            for (j = 0; j < cvt->maps; j++) {
                map = &cvt->map[j];
                if (value == map->in) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            valsym = map->out.s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(hndl->handle, key->field, valsym));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(hndl->handle, key->field, value));
        }
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
        (void)bcmgene_batch_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t value)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, found = 0, supported;
    const char *valsym;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk &&
            fld->offset == offset) {
            supported = 1;
            if (hndl->flags & BCMGENE_F_HANDLE_CALLBACK) {
                bcmgene_field_data_t fd;
                fd.fld = fld;
                fd.index = 0;
                fd.value = &value;
                fd.count = 1;
                fd.depth = fld->depth;
                if (hndl->cb) {
                    SHR_IF_ERR_VERBOSE_EXIT(hndl->cb(hndl, &fd));
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else {
                if (fld->converter) {
                    cvt = fld->converter;
                    found = 0;
                    for (j = 0; j < cvt->maps; j++) {
                        map = &cvt->map[j];
                        if (value == map->in) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    valsym = map->out.s;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_symbol_add(hndl->handle, fld->field,
                                                      valsym));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(hndl->handle, fld->field,
                                               value));
                }
            }
            break;
        }
    }

    if (!supported) {
        if (hndl->flags & BCMGENE_F_HANDLE_GRACEFUL) {
            hndl->flags &= ~BCMGENE_F_HANDLE_GRACEFUL;
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
        (void)bcmgene_batch_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint32_t index,
    uint64_t *value,
    uint32_t count)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, k, found = 0, supported;
    const char *valsym;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk &&
            fld->offset == offset) {
            supported = 1;
            if (hndl->flags & BCMGENE_F_HANDLE_CALLBACK) {
                bcmgene_field_data_t fd;
                fd.fld = fld;
                fd.index = index;
                fd.value = value;
                fd.count = count;
                fd.depth = fld->depth;
                if (hndl->cb) {
                    SHR_IF_ERR_VERBOSE_EXIT(hndl->cb(hndl, &fd));
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else {
                if (fld->depth > 1) {
                    if ((index + count) > fld->depth) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    for (k = 0; k < count; k++) {
                        if (fld->converter) {
                            cvt = fld->converter;
                            found = 0;
                            for (j = 0; j < cvt->maps; j++) {
                                map = &cvt->map[j];
                                if (value[k] == map->in) {
                                    found = 1;
                                    break;
                                }
                            }
                            if (!found) {
                                SHR_ERR_EXIT(SHR_E_INTERNAL);
                            }
                            valsym = map->out.s;
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_array_symbol_add(hndl->handle,
                                                                    fld->field,
                                                                    index + k,
                                                                    &valsym,
                                                                    1));
                        } else {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_array_add(hndl->handle,
                                                             fld->field,
                                                             index + k,
                                                             &value[k], 1));
                        }
                    }
                } else {
                    if (index != 0 && count != 1) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    if (fld->converter) {
                        cvt = fld->converter;
                        found = 0;
                        for (j = 0; j < cvt->maps; j++) {
                            map = &cvt->map[j];
                            if (value[index] == map->in) {
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }
                        valsym = map->out.s;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_symbol_add(hndl->handle,
                                                          fld->field,
                                                          valsym));
                    } else {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_add(hndl->handle, fld->field,
                                                   value[index]));
                    }
                }
            }
            break;
        }
    }

    if (!supported) {
        if (hndl->flags & BCMGENE_F_HANDLE_GRACEFUL) {
            hndl->flags &= ~BCMGENE_F_HANDLE_GRACEFUL;
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
        (void)bcmgene_batch_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_callback(
    bcmgene_handle_t *hndl,
    int (*cb)(bcmgene_handle_t *hndl, void *user_data))
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    hndl->cb = cb;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_priority(
    bcmgene_handle_t *hndl,
    int priority)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    hndl->priority = priority;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_opcode(
    bcmgene_handle_t *hndl,
    int opcode)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    hndl->opcode = opcode;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_fill_batch_handle(
    bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_entry_add(hndl->batch_handle,
                                     hndl->opcode, hndl->handle));
    hndl->handle = BCMLT_INVALID_HDL;
exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
        (void)bcmgene_batch_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_commit(bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    switch(hndl->opcode) {
        case BCMLT_OPCODE_INSERT:
        case BCMLT_OPCODE_DELETE:
        case BCMLT_OPCODE_UPDATE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(hndl->handle, hndl->opcode, hndl->priority));

exit:
    if (hndl && !(hndl->flags & BCMGENE_F_HANDLE_RESERVED)) {
        (void)bcmgene_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_commit_done(bcmgene_handle_t *hndl)
{
    return bcmgene_handle_destroy(hndl);
}

static int
bcmgene_commit_batch(bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->batch_handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(hndl->batch_handle, hndl->priority));

exit:
    if (hndl) {
        (void)bcmgene_batch_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_edit(bcmgene_handle_t *hndl)
{
    int rv;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmgene_fill_flags(hndl, BCMGENE_F_HANDLE_RESERVED));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmgene_fill_opcode(hndl, BCMLT_OPCODE_UPDATE));

    rv = bcmgene_commit(hndl);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmgene_fill_opcode(hndl, BCMLT_OPCODE_INSERT));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmgene_commit(hndl));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    (void)bcmgene_commit_done(hndl);
    SHR_FUNC_EXIT();
}

static int
bcmgene_unset(bcmgene_handle_t *hndl)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, idx, found;
    const char *valsym;
    uint32_t count;
    uint64_t value;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);
    SHR_NULL_CHECK(hndl->tbl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    tbl = hndl->tbl;

    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->depth > 1) {
            for (idx = 0; idx < fld->depth; idx++) {
                if (fld->converter) {
                    cvt = fld->converter;
                    found = 0;
                    for (j = 0; j < cvt->maps; j++) {
                        map = &cvt->map[j];
                        if (0 == map->in) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_symbol_get(hndl->handle,
                                                            fld->field,
                                                            idx, &valsym, 1,
                                                            &count));
                    if (sal_strcmp(map->out.s, valsym) == 0) {
                        continue;
                    }
                    valsym = map->out.s;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_symbol_add(hndl->handle,
                                                            fld->field,
                                                            idx, &valsym, 1));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(hndl->handle, fld->field,
                                                     idx, &value, 1, &count));
                    if (value == 0) {
                        continue;
                    }
                    value = 0;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(hndl->handle, fld->field,
                                                     idx, &value, 1));
                }
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(hndl->handle, fld->field, &value));
            if (value == 0) {
                continue;
            }
            value = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(hndl->handle, fld->field, value));
        }
    }
exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_lookup(bcmgene_handle_t *hndl)
{
    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmgene_fill_opcode(hndl, BCMLT_OPCODE_LOOKUP));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(hndl->handle, hndl->opcode, hndl->priority));

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_lookup_key(
    bcmgene_handle_t *hndl,
    int index,
    uint64_t *value)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *key = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int j, found;
    const char *valsym;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);
    SHR_NULL_CHECK(hndl->tbl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    tbl = hndl->tbl;
    if (index >= 0 && index < tbl->keys) {
        key = &tbl->key[index];
        if (key->converter) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(hndl->handle, key->field,
                                              &valsym));
            cvt = key->converter;
            found = 0;
            for (j = 0; j < cvt->maps; j++) {
                map = &cvt->map[j];
                if (sal_strcmp(valsym, map->out.s) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            *value = map->in;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(hndl->handle, key->field, value));
        }
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_lookup_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t *value)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, found = 0, supported;
    const char *valsym;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk && fld->offset == offset) {
            supported = 1;
            if (fld->converter) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(hndl->handle, fld->field,
                                                  &valsym));
                cvt = fld->converter;
                found = 0;
                for (j = 0; j < cvt->maps; j++) {
                    map = &cvt->map[j];
                    if (sal_strcmp(valsym, map->out.s) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                *value = map->in;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(hndl->handle, fld->field, value));
            }
            break;
        }
    }

    if (!supported) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_lookup_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint32_t index,
    uint64_t *value,
    uint32_t count,
    uint32_t *depth)
{
    bcmgene_table_desc_t *tbl = NULL;
    bcmgene_field_desc_t *fld = NULL;
    bcmgene_field_value_converter_t *cvt = NULL;
    bcmgene_field_value_map_t *map = NULL;
    int i, j, k, found = 0, supported;
    const char *valsym;
    uint32_t cnt = 0;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    if (hndl->handle == BCMLT_INVALID_HDL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    tbl = hndl->tbl;
    supported = 0;
    for (i = 0; i < tbl->flds; i++) {
        fld = &tbl->fld[i];
        if (fld->blk == blk && fld->offset == offset) {
            supported = 1;
            if (fld->depth > 1) {
                if ((index + count) > fld->depth) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                for (k = 0; k < count; k++) {
                    if (fld->converter) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_array_symbol_get(hndl->handle,
                                                                fld->field,
                                                                index + k,
                                                                &valsym, 1,
                                                                &cnt));
                        cvt = fld->converter;
                        found = 0;
                        for (j = 0; j < cvt->maps; j++) {
                            map = &cvt->map[j];
                            if (sal_strcmp(valsym, map->out.s) == 0) {
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }

                        value[k] = map->in;
                    } else {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_array_get(hndl->handle,
                                                         fld->field,
                                                         index + k,
                                                         &value[k], 1,
                                                         &cnt));
                    }
                }
                *depth = fld->depth;
            } else {
                if (index != 0 && count > 1) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                if (count == 1) {
                    if (fld->converter) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_symbol_get(hndl->handle,
                                                          fld->field,
                                                          &valsym));
                        cvt = fld->converter;
                        found = 0;
                        for (j = 0; j < cvt->maps; j++) {
                            map = &cvt->map[j];
                            if (sal_strcmp(valsym, map->out.s) == 0) {
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }
                        value[index] = map->in;
                    } else {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_get(hndl->handle, fld->field,
                                                   &value[index]));
                    }
                }
            }
            break;
        }
    }

    if (!supported) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (hndl && SHR_FUNC_ERR()) {
        (void)bcmgene_handle_destroy(hndl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmgene_lookup_done(bcmgene_handle_t *hndl)
{
    return bcmgene_handle_destroy(hndl);
}

static int
bcmgene_traverse(
    bcmgene_handle_t *hndl,
    void *user_data)
{
    int rv;

    SHR_FUNC_ENTER(hndl ? hndl->unit : BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(hndl, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmgene_handle_create(hndl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmgene_fill_opcode(hndl, BCMLT_OPCODE_TRAVERSE));

    while ((rv = bcmlt_entry_commit(hndl->handle,
                                    hndl->opcode,
                                    hndl->priority)) == SHR_E_NONE) {
        if (hndl->cb) {
            SHR_IF_ERR_VERBOSE_EXIT
                (hndl->cb(hndl, user_data));
        }
    }

    if ((rv == SHR_E_NOT_FOUND) || (rv == SHR_E_NONE)) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    (void)bcmgene_handle_destroy(hndl);
    SHR_FUNC_EXIT();
}

static int
bcmgene_clear(
    int unit,
    bcmgene_desc_t *gene)
{
    bcmgene_func_desc_t *func = NULL;
    bcmgene_table_desc_t *tbl = NULL;
    int i, j;

    SHR_FUNC_ENTER(unit);

    if (!gene) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    for (i = 0; i < gene->funcs; i++) {
        func = &gene->func[i];
        for (j = 0; j < func->tbls; j++) {
            tbl = &func->tbl[j];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, tbl->table));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmgene_error_return(
    bcmgene_handle_t *hndl,
    int rv)
{
    if (hndl && SHR_FAILURE(rv)) {
        (void)bcmgene_handle_destroy(hndl);
        (void)bcmgene_batch_handle_destroy(hndl);
    }

    return rv;
}

static int
bcmgene_func_traverse(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t function,
    int (*cb)(bcmgene_handle_t *hndl, void *user_data),
    void *user_data)
{
    int rv;
    int i, j;
    bcmgene_handle_t hndl;
    bcmgene_func_desc_t *func = NULL;

    SHR_FUNC_ENTER(unit);

    if (!gene) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    bcmgene_handle_t_init(&hndl);

    hndl.unit = unit;
    hndl.cb = cb;

    for (i = 0; i < gene->funcs; i++) {
        func = &gene->func[i];
        if (func->func == function) {
            for (j = 0; j < func->tbls; j++) {
                hndl.tbl = &func->tbl[j];
                if (hndl.cb) {
                    rv = hndl.cb(&hndl, user_data);
                    if (rv != SHR_E_NOT_FOUND) {
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }
                }
            }
            break;
        }
    }

    if (i == gene->funcs) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_gene_sub_feature_get(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t func,
    uint32_t *sub_f)
{
    return bcmgene_sub_feature_get(unit, gene, func, sub_f);
}

int
bcm56780_a0_gene_targeting(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t func,
    uint32_t sub_f,
    bcmgene_handle_t *hndl)
{
    return bcmgene_targeting(unit, gene, func, sub_f, hndl);
}

int
bcm56780_a0_gene_batch_handle_create(bcmgene_handle_t *hndl)
{
    return bcmgene_batch_handle_create(hndl);
}

int
bcm56780_a0_gene_batch_handle_destroy(bcmgene_handle_t *hndl)
{
    return bcmgene_batch_handle_destroy(hndl);
}

int
bcm56780_a0_gene_handle_create(bcmgene_handle_t *hndl)
{
    return bcmgene_handle_create(hndl);
}

int
bcm56780_a0_gene_handle_destroy(bcmgene_handle_t *hndl)
{
    return bcmgene_handle_destroy(hndl);
}

int
bcm56780_a0_gene_validate_tag(
    bcmgene_handle_t *hndl,
    int blk,
    int offset)
{
    return bcmgene_validate_tag(hndl, blk, offset);
}

int
bcm56780_a0_gene_validate_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t value)
{
    return bcmgene_validate_field(hndl, blk, offset, value);
}

int
bcm56780_a0_gene_validate_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    int index,
    uint64_t value)
{
    return bcmgene_validate_field_array(hndl, blk, offset, index, value);
}

int
bcm56780_a0_gene_fill_flags(
    bcmgene_handle_t *hndl,
    uint32_t flags)
{
    return bcmgene_fill_flags(hndl, flags);
}

int
bcm56780_a0_gene_fill_key(
    bcmgene_handle_t *hndl,
    int index,
    uint64_t value)
{
    return bcmgene_fill_key(hndl, index, value);
}

int
bcm56780_a0_gene_fill_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t value)
{
    return bcmgene_fill_field(hndl, blk, offset, value);
}

int
bcm56780_a0_gene_fill_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint32_t index,
    uint64_t *value,
    uint32_t count)
{
    return bcmgene_fill_field_array(hndl, blk, offset, index, value, count);
}

int
bcm56780_a0_gene_fill_callback(
    bcmgene_handle_t *hndl,
    int (*cb)(bcmgene_handle_t *hndl, void *user_data))
{
    return bcmgene_fill_callback(hndl, cb);
}

int
bcm56780_a0_gene_fill_priority(
    bcmgene_handle_t *hndl,
    int priority)
{
    return bcmgene_fill_priority(hndl, priority);
}

int
bcm56780_a0_gene_fill_opcode(
    bcmgene_handle_t *hndl,
    int opcode)
{
    return bcmgene_fill_opcode(hndl, opcode);
}

int
bcm56780_a0_gene_fill_batch_handle(bcmgene_handle_t *hndl)
{
    return bcmgene_fill_batch_handle(hndl);
}

int
bcm56780_a0_gene_commit(bcmgene_handle_t *hndl)
{
    return bcmgene_commit(hndl);
}

int
bcm56780_a0_gene_commit_done(bcmgene_handle_t *hndl)
{
    return bcmgene_commit_done(hndl);
}

int
bcm56780_a0_gene_commit_batch(bcmgene_handle_t *hndl)
{
    return bcmgene_commit_batch(hndl);
}

int
bcm56780_a0_gene_edit(bcmgene_handle_t *hndl)
{
    return bcmgene_edit(hndl);
}

int
bcm56780_a0_gene_unset(bcmgene_handle_t *hndl)
{
    return bcmgene_unset(hndl);
}

int
bcm56780_a0_gene_lookup(bcmgene_handle_t *hndl)
{
    return bcmgene_lookup(hndl);
}

int
bcm56780_a0_gene_lookup_key(
    bcmgene_handle_t *hndl,
    int index,
    uint64_t *value)
{
    return bcmgene_lookup_key(hndl, index, value);
}

int
bcm56780_a0_gene_lookup_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t *value)
{
    return bcmgene_lookup_field(hndl, blk, offset, value);
}

int
bcm56780_a0_gene_lookup_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint32_t index,
    uint64_t *value,
    uint32_t count,
    uint32_t *depth)
{
    return bcmgene_lookup_field_array(hndl, blk, offset, index, value, count, depth);
}

int
bcm56780_a0_gene_lookup_done(bcmgene_handle_t *hndl)
{
    return bcmgene_lookup_done(hndl);
}

int
bcm56780_a0_gene_traverse(
    bcmgene_handle_t *hndl,
    void *user_data)
{
    return bcmgene_traverse(hndl, user_data);
}

int
bcm56780_a0_gene_clear(
    int unit,
    bcmgene_desc_t *gene)
{
    return bcmgene_clear(unit, gene);
}

int
bcm56780_a0_gene_error_return(
    bcmgene_handle_t *hndl,
    int rv)
{
    return bcmgene_error_return(hndl, rv);
}

int
bcm56780_a0_gene_func_traverse(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t function,
    int (*cb)(bcmgene_handle_t *hndl, void *user_data),
    void *user_data)
{
    return bcmgene_func_traverse(unit, gene, function, cb, user_data);
}

