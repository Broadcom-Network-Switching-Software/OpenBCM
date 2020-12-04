/*! \file bcmcth_dlb_ecmp_stats.c
 *
 * Handler implementation for DLB_ECMP_STATS LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_dlb_drv.h>
#include <bcmcth/bcmcth_dlb_util.h>
#include <bcmcth/bcmcth_dlb_ecmp_stats.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Private variables
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

static bcmcth_dlb_ecmp_stats_info_t *dlb_ecmp_stats_info[BCMPORT_NUM_UNITS_MAX]
                                    = {0};

#define BCMCTH_DLB_ECMP_STATS_INFO(_u) (dlb_ecmp_stats_info[(_u)])

#define BCMCTH_DLB_ECMP_STATS_TBL_ENT(_u) \
        ((BCMCTH_DLB_ECMP_STATS_INFO(_u))->ha_ent_arr)

#define BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(_u, _i) \
        (((BCMCTH_DLB_ECMP_STATS_INFO(_u))->ha_ent_arr[_i]).valid)

#define BCMCTH_DLB_ECMP_STATS_TBL_ENT_FAIL_CNT(_u, _i) \
        (((BCMCTH_DLB_ECMP_STATS_INFO(_u))->ha_ent_arr[_i]).fail_cnt)

#define BCMCTH_DLB_ECMP_STATS_TBL_ENT_PORT_REASSIGNMENT_CNT(_u, _i) \
        (((BCMCTH_DLB_ECMP_STATS_INFO(_u))->ha_ent_arr[_i]).port_reassignment_cnt)

#define BCMCTH_DLB_ECMP_STATS_TBL_ENT_MEMBER_REASSIGNMENT_CNT(_u, _i) \
        (((BCMCTH_DLB_ECMP_STATS_INFO(_u))->ha_ent_arr[_i]).member_reassignment_cnt)

/*******************************************************************************
 * Private functions
 */

static void
lth_field_data_to_u64(uint32_t width, const bcmlrd_field_data_t *field_data,
                      uint64_t *min, uint64_t *max)
{
    if (width == 1) {
        *min = (uint64_t)field_data->min->is_true;
        *max = (uint64_t)field_data->max->is_true;
    } else if (width <= 8) {
        *min = (uint64_t)field_data->min->u8;
        *max = (uint64_t)field_data->max->u8;
    } else if (width <= 16) {
        *min = (uint64_t)field_data->min->u16;
        *max = (uint64_t)field_data->max->u16;
    } else if (width <= 32) {
        *min = (uint64_t)field_data->min->u32;
        *max = (uint64_t)field_data->max->u32;
    } else {
        *min = (uint64_t)field_data->min->u64;
        *max = (uint64_t)field_data->max->u64;
    }
}

static int
lth_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                          uint64_t *min, uint64_t *max)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    lth_field_data_to_u64(field_md->width, field_data, min, max);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_dlb_ecmp_stats_lt_fields_fill(int unit,
                                     const bcmltd_field_t *in,
                                     bcmcth_dlb_ecmp_stats_t *lt_entry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t num;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    fid = in->id;
    fval = in->data;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, DLB_ECMP_STATSt, fid,
                                      1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
    case DLB_ECMP_STATSt_DLB_IDf:
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_STATS_LT_FIELD_DLB_ID);
        lt_entry->dlb_id = fval;
        break;

    case DLB_ECMP_STATSt_FAIL_CNTf:
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_STATS_LT_FIELD_FAIL_CNT);
        lt_entry->fail_cnt = fval;
        break;

    case DLB_ECMP_STATSt_PORT_REASSIGNMENT_CNTf:
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
            BCMCTH_DLB_ECMP_STATS_LT_FIELD_PORT_REASSIGNMENT_CNT);
        lt_entry->port_reassignment_cnt = fval;
        break;

    case DLB_ECMP_STATSt_MEMBER_REASSIGNMENT_CNTf:
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
            BCMCTH_DLB_ECMP_STATS_LT_FIELD_MEMBER_REASSIGNMENT_CNT);
        lt_entry->member_reassignment_cnt = fval;
        break;

    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_dlb_ecmp_stats_lt_fields_parse(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmcth_dlb_ecmp_stats_t *lt_entry,
                                      bool key_only)
{
    bcmltd_field_t *field;
    int idx;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    for (idx = 0; idx < (int)in->count; idx++) {
        field = in->field[idx];
        if (key_only && field->id != DLB_ECMP_STATSt_DLB_IDf) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_dlb_ecmp_stats_lt_fields_fill(unit, field, lt_entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmcth_dlb_ecmp_stats_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    bcmcth_dlb_ecmp_stats_t lt_entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_lt_fields_parse(unit, in, &lt_entry, TRUE));

    switch (opcode) {
    case BCMLT_OPCODE_INSERT:
        if (BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, lt_entry.dlb_id)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        break;

    case BCMLT_OPCODE_LOOKUP:
    case BCMLT_OPCODE_UPDATE:
    case BCMLT_OPCODE_DELETE:
    case BCMLT_OPCODE_TRAVERSE:
        if (!BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, lt_entry.dlb_id)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        break;

    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_dlb_ecmp_stats_t lt_entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_lt_fields_parse(unit, in, &lt_entry, TRUE));

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_set(unit, &lt_entry));

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, lt_entry.dlb_id) = 1;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_FAIL_CNT(unit, lt_entry.dlb_id)
        = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_PORT_REASSIGNMENT_CNT(unit, lt_entry.dlb_id)
        = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_MEMBER_REASSIGNMENT_CNT(unit, lt_entry.dlb_id)
        = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_dlb_ecmp_stats_t lt_entry;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_lt_fields_parse(unit, in, &lt_entry, TRUE));

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_get(unit, &lt_entry));

    out->field[idx]->id = DLB_ECMP_STATSt_DLB_IDf;
    out->field[idx]->data = lt_entry.dlb_id;
    out->field[idx]->idx = 0;
    idx++;

    out->field[idx]->id = DLB_ECMP_STATSt_FAIL_CNTf;
    out->field[idx]->data = lt_entry.fail_cnt;
    out->field[idx]->idx = 0;
    idx++;

    out->field[idx]->id = DLB_ECMP_STATSt_PORT_REASSIGNMENT_CNTf;
    out->field[idx]->data = lt_entry.port_reassignment_cnt;
    out->field[idx]->idx = 0;
    idx++;

    out->field[idx]->id = DLB_ECMP_STATSt_MEMBER_REASSIGNMENT_CNTf;
    out->field[idx]->data = lt_entry.member_reassignment_cnt;
    out->field[idx]->idx = 0;
    idx++;

    out->count = idx;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_dlb_ecmp_stats_t lt_entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_lt_fields_parse(unit, in, &lt_entry, TRUE));

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_set(unit, &lt_entry));

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, lt_entry.dlb_id) = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_FAIL_CNT(unit, lt_entry.dlb_id)
        = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_PORT_REASSIGNMENT_CNT(unit, lt_entry.dlb_id)
        = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_MEMBER_REASSIGNMENT_CNT(unit, lt_entry.dlb_id)
        = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bcmcth_dlb_ecmp_stats_t lt_entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_lt_fields_parse(unit, in, &lt_entry, TRUE));

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_set(unit, &lt_entry));

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, lt_entry.dlb_id) = 1;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_FAIL_CNT(unit, lt_entry.dlb_id)
        = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_PORT_REASSIGNMENT_CNT(unit, lt_entry.dlb_id)
        = 0;

    BCMCTH_DLB_ECMP_STATS_TBL_ENT_MEMBER_REASSIGNMENT_CNT(unit, lt_entry.dlb_id)
        = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_first(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t dlb_id, min, max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (lth_field_value_range_get(unit, DLB_ECMP_STATSt,
                                   DLB_ECMP_STATSt_DLB_IDf,
                                   &min, &max));

    for (dlb_id = min; dlb_id <= max; dlb_id++) {
        if (BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, dlb_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = DLB_ECMP_STATSt_DLB_IDf;
            flds.field[0]->data = dlb_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_dlb_ecmp_stats_lookup(unit, op_arg, &flds, out, arg));
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_next(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmcth_dlb_ecmp_stats_t lt_entry;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld = {0};
    uint64_t dlb_id, min, max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (lth_field_value_range_get(unit, DLB_ECMP_STATSt,
                                   DLB_ECMP_STATSt_DLB_IDf,
                                   &min, &max));

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_stats_lt_fields_parse(unit, in, &lt_entry, TRUE));

    for (dlb_id = lt_entry.dlb_id + 1; dlb_id <= max; dlb_id++) {
        if (BCMCTH_DLB_ECMP_STATS_TBL_ENT_VALID(unit, dlb_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = DLB_ECMP_STATSt_DLB_IDf;
            flds.field[0]->data = dlb_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_dlb_ecmp_stats_lookup(unit, op_arg, &flds, out, arg));
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_resources_alloc(int unit, bool warm)
{
    int dlb_grp_cnt = 0;
    void *ptr = NULL;
    uint32_t alloc_sz = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_grp_cnt_get(unit, &dlb_grp_cnt));

    alloc_sz = sizeof(bcmcth_dlb_ecmp_stats_info_t);
    SHR_ALLOC(ptr, alloc_sz, "bcmcthDlbEcmpStats");
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    sal_memset(ptr, 0, alloc_sz);
    BCMCTH_DLB_ECMP_STATS_INFO(unit) = ptr;

    alloc_sz = dlb_grp_cnt * sizeof(bcmcth_dlb_ecmp_stats_entry_info_t);
    ptr = shr_ha_mem_alloc(unit,
                           BCMMGMT_DLB_COMP_ID,
                           BCMPTM_HA_SUBID_DLB_ECMP_STATS,
                           "bcmcthDlbEcmpStatsHaInfo",
                           &alloc_sz);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(ptr, 0, alloc_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit,
                BCMMGMT_DLB_COMP_ID,
                BCMPTM_HA_SUBID_DLB_ECMP_STATS,
                0,
                sizeof(bcmcth_dlb_ecmp_stats_entry_info_t),
                dlb_grp_cnt,
                BCMCTH_DLB_ECMP_STATS_ENTRY_INFO_T_ID));
    }
    BCMCTH_DLB_ECMP_STATS_TBL_ENT(unit) = ptr;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_stats_resources_free(int unit)
{
    SHR_FREE(BCMCTH_DLB_ECMP_STATS_INFO(unit));

    return SHR_E_NONE;
}
