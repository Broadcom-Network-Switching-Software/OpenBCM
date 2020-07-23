/*! \file bcmltm_table_op_pt_info.c
 *
 * Interface to update in-memory table with PT status.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_state_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffffL
#endif

/* Arbitrary high table field length, to accommodate for array depth */
#define TABLE_OP_PT_INFO_MAX_FIELD_LEN 256

static uint32_t last_trans_id[BCMDRD_CONFIG_MAX_UNITS][BCMLTD_TABLE_COUNT];

static shr_famm_hdl_t table_op_pt_info_arr_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/* Convert IMM entry to bcmltm_table_op_pt_info_t */
static int
bcmltm_table_op_pt_info_get(bcmltm_table_op_pt_info_t *ptrm_status,
                            bcmltd_field_t *flds)
{
    uint32_t attr = flds->id;

    if (ptrm_status == NULL) {
        /* Entry not found. */
        return SHR_E_PARAM;
    }

    switch (attr) {
    case TABLE_OP_PT_INFOt_TABLE_IDf:
        ptrm_status->lt_id = flds->data;
        break;
    case TABLE_OP_PT_INFOt_PT_INSTANCEf:
        if (flds->idx < ptrm_status->pt_sid_cnt) {
            ptrm_status->pt_instance[flds->idx] = flds->data;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_ID_CNTf:
        ptrm_status->pt_sid_cnt = flds->data;
        break;
    case TABLE_OP_PT_INFOt_PT_IDf:
        if (flds->idx < ptrm_status->pt_sid_cnt) {
            ptrm_status->pt_sid[flds->idx] = flds->data;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_CNTf:
        ptrm_status->pt_index_cnt = flds->data;
        break;
    case TABLE_OP_PT_INFOt_PT_INDEXf:
        if (flds->idx < ptrm_status->pt_index_cnt) {
            ptrm_status->pt_index[flds->idx] = flds->data;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_ID_DATA_CNTf:
        ptrm_status->pt_sid_data_cnt =flds->data;
        break;
    case TABLE_OP_PT_INFOt_PT_ID_DATAf:
        if (flds->idx < ptrm_status->pt_sid_data_cnt) {
            ptrm_status->pt_sid_data[flds->idx] = flds->data;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_DATA_CNTf:
        ptrm_status->pt_index_data_cnt = flds->data;
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_DATAf:
        if (flds->idx < ptrm_status->pt_index_data_cnt) {
            ptrm_status->pt_index_data[flds->idx] = flds->data;
        }
        break;

    default:
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/* Append new record after prev. In case of overflow, keep the latest */
static int
bcmltm_table_op_pt_info_update_ptrm_info(bcmltm_table_op_pt_info_t *prev_ptrm,
                                         bcmltm_table_op_pt_info_t *ptrm)
{
    uint32_t prev_records = 0; /* Num of prev records to keep in case of overflow */
    uint32_t prev_start_index = 0; /* Start idx of prev data preserved on overflow */
    uint32_t i = 0;

    if (prev_ptrm->pt_sid_cnt + ptrm->pt_sid_cnt <=
                                         TABLE_OP_INFO_MAX_INDEX) {
        /* Append new records to prev_ptrm */
        sal_memcpy(&prev_ptrm->pt_sid[prev_ptrm->pt_sid_cnt],
                   ptrm->pt_sid, sizeof(uint32_t)*ptrm->pt_sid_cnt);
        sal_memcpy(&prev_ptrm->pt_instance[prev_ptrm->pt_sid_cnt],
                   ptrm->pt_instance, sizeof(uint32_t)*ptrm->pt_sid_cnt);
        sal_memcpy(&prev_ptrm->pt_index[prev_ptrm->pt_sid_cnt],
                   ptrm->pt_index, sizeof(uint32_t)*ptrm->pt_sid_cnt);
        prev_ptrm->pt_sid_cnt += ptrm->pt_sid_cnt;
        prev_ptrm->pt_index_cnt += ptrm->pt_index_cnt;

    } else {
        /* Overflow case, we would always want to record the last data */
        if (ptrm->pt_sid_cnt < TABLE_OP_INFO_MAX_INDEX) {
            prev_records = TABLE_OP_INFO_MAX_INDEX - ptrm->pt_sid_cnt;
            prev_start_index = prev_ptrm->pt_sid_cnt - prev_records;
            /* keep only last prev_records */
            for (i=0; i<prev_records; i++) {
                prev_ptrm->pt_sid[i] = prev_ptrm->pt_sid[prev_start_index + i];
                prev_ptrm->pt_instance[i] = prev_ptrm->pt_instance[prev_start_index + i];
                prev_ptrm->pt_index[i] = prev_ptrm->pt_index[prev_start_index + i];
            }
            /* Append new records */
            for (i=0; i<ptrm->pt_sid_cnt; i++) {
                prev_ptrm->pt_sid[prev_records + i] = ptrm->pt_sid[i];
                prev_ptrm->pt_instance[prev_records + i] = ptrm->pt_instance[i];
                prev_ptrm->pt_index[prev_records + i] = ptrm->pt_index[i];
            }

        } else {
            /* Num new records equal to TABLE_OP_INFO_MAX_INDEX,
               keep only new records */
            sal_memcpy(prev_ptrm->pt_sid,
                       ptrm->pt_sid, sizeof(uint32_t)*ptrm->pt_sid_cnt);
            sal_memcpy(prev_ptrm->pt_instance,
                       ptrm->pt_instance, sizeof(uint32_t)*ptrm->pt_sid_cnt);
            sal_memcpy(prev_ptrm->pt_index,
                       ptrm->pt_index, sizeof(uint32_t)*ptrm->pt_sid_cnt);
        }
        prev_ptrm->pt_sid_cnt = TABLE_OP_INFO_MAX_INDEX;
        prev_ptrm->pt_index_cnt = TABLE_OP_INFO_MAX_INDEX;
    }


    /* Handle data sid and index (non aggregate view) */
    if (ptrm->pt_sid_data_cnt) {
        if (prev_ptrm->pt_sid_data_cnt + ptrm->pt_sid_data_cnt <=
                                            TABLE_OP_INFO_MAX_INDEX) {
            /* Append new records to prev_ptrm */
            sal_memcpy(&prev_ptrm->pt_sid_data[prev_ptrm->pt_sid_data_cnt],
                       ptrm->pt_sid_data,
                       sizeof(uint32_t)*ptrm->pt_sid_data_cnt);
            sal_memcpy(&prev_ptrm->pt_index_data[prev_ptrm->pt_sid_data_cnt],
                       ptrm->pt_index_data,
                       sizeof(uint32_t)*ptrm->pt_sid_data_cnt);
            prev_ptrm->pt_sid_data_cnt += ptrm->pt_sid_data_cnt;
            prev_ptrm->pt_index_data_cnt += ptrm->pt_index_data_cnt;

        } else {
            /* Overflow case, we would always want to record the last data */
            if (ptrm->pt_sid_data_cnt < TABLE_OP_INFO_MAX_INDEX) {
                prev_records = TABLE_OP_INFO_MAX_INDEX - ptrm->pt_sid_data_cnt;
                prev_start_index = prev_ptrm->pt_sid_data_cnt - prev_records;
                /* keep only last prev_records */
                for (i=0; i<prev_records; i++) {
                    prev_ptrm->pt_sid_data[i] =
                         prev_ptrm->pt_sid_data[prev_start_index + i];
                    prev_ptrm->pt_index_data[i] =
                         prev_ptrm->pt_index_data[prev_start_index + i];
                }

                /* Append new records */
                for (i=0; i<ptrm->pt_sid_data_cnt; i++) {
                    prev_ptrm->pt_sid_data[prev_records + i] =
                           ptrm->pt_sid_data[i];
                    prev_ptrm->pt_index_data[prev_records + i] =
                           ptrm->pt_index_data[i];
                }
            } else {
                /* Num new records equal to TABLE_OP_INFO_MAX_INDEX,
                   keep only new records */
                sal_memcpy(prev_ptrm->pt_sid_data,
                           ptrm->pt_sid_data,
                           sizeof(uint32_t)*ptrm->pt_sid_data_cnt);
                sal_memcpy(prev_ptrm->pt_index_data,
                           ptrm->pt_index_data,
                           sizeof(uint32_t)*ptrm->pt_sid_data_cnt);
            }
            prev_ptrm->pt_sid_data_cnt = TABLE_OP_INFO_MAX_INDEX;
            prev_ptrm->pt_index_data_cnt = TABLE_OP_INFO_MAX_INDEX;
        }
    }

    return SHR_E_NONE;
}

static int
bcmltm_table_op_pt_stats_read(int unit, bcmltm_table_op_pt_info_t *ptrm_status,
                              uint32_t attr, bcmltd_field_t **flds,
                              uint32_t *fld_count, uint32_t index)
{
    if (ptrm_status == NULL) {
        /* Entry not found. */
        return SHR_E_PARAM;
    }

    switch (attr) {
    case TABLE_OP_PT_INFOt_TABLE_IDf:
        flds[*fld_count]->data = ptrm_status->lt_id;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_INSTANCEf:
        if (index < ptrm_status->pt_sid_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_instance[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_ID_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_sid_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_IDf:
        if (index < ptrm_status->pt_sid_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_sid[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_index_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_INDEXf:
        if (index < ptrm_status->pt_index_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_index[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_ID_DATA_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_sid_data_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_ID_DATAf:
        if (index < ptrm_status->pt_sid_data_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_sid_data[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_DATA_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_index_data_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_DATAf:
        if (index < ptrm_status->pt_index_data_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_index_data[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;

    default:
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

static int
bcmltm_table_op_pt_info_update_data(int unit,
                                 bcmltm_table_op_pt_info_t *ptrm_status)
{
    bcmltd_fields_t in_flds, out_flds;
    uint32_t fld, fld_count=0, index;
    int entry_exists = 0, hi_pri = 1;
    bcmlrd_field_def_t  field_def;
    bcmltm_table_op_pt_info_t prev_ptrm_status, *updated_ptrm_status;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Init for error return. */
    out_flds.field = NULL;

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(table_op_pt_info_arr_hdl[unit], 1);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = TABLE_OP_PT_INFO_MAX_FIELD_LEN;
    out_flds.field = shr_famm_alloc(table_op_pt_info_arr_hdl[unit],
                                    TABLE_OP_PT_INFO_MAX_FIELD_LEN);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Set logical table ID */
    fld = TABLE_OP_PT_INFOt_TABLE_IDf;
    in_flds.field[0]->id = fld;
    if (bcmltm_table_op_pt_stats_read(unit, ptrm_status, fld,
                                      in_flds.field, &fld_count, 0) ==
            SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    updated_ptrm_status = ptrm_status;

    /* Lookup the entry. Update the entry if exists. */
    if (bcmimm_entry_lookup(unit, TABLE_OP_PT_INFOt,
                                &in_flds, &out_flds) != SHR_E_NOT_FOUND) {
        entry_exists = 1;
        /* If another PT operation within same LT transaction, fetch existing data */
        if ((ptrm_status->lt_trans_id != 0xFFFFFFFF) &&
            (ptrm_status->lt_trans_id == last_trans_id[unit][ptrm_status->lt_id]))
        {
            sal_memset(&prev_ptrm_status, 0, sizeof(prev_ptrm_status));
            prev_ptrm_status.lt_id = ptrm_status->lt_id;
            for (fld = 0; fld < out_flds.count; fld++) {
                rv = bcmltm_table_op_pt_info_get(&prev_ptrm_status,
                                                 out_flds.field[fld]);
                if (rv != SHR_E_NONE) {
                    SHR_ERR_EXIT(rv);
                }
            }

            /* Consolidate current and prev info in prev_ptrm_status */
            rv = bcmltm_table_op_pt_info_update_ptrm_info(&prev_ptrm_status,
                                                          ptrm_status);
            if (rv != SHR_E_NONE) {
                SHR_ERR_EXIT(rv);
            }
            updated_ptrm_status = &prev_ptrm_status;
        }
    }

    shr_famm_free(table_op_pt_info_arr_hdl[unit], in_flds.field, in_flds.count);

    in_flds.count = TABLE_OP_PT_INFO_MAX_FIELD_LEN;
    in_flds.field = shr_famm_alloc(table_op_pt_info_arr_hdl[unit],
                                   TABLE_OP_PT_INFO_MAX_FIELD_LEN);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Fill complete field set. */
    fld_count = 0;
    for (fld = 0; fld < TABLE_OP_PT_INFOt_FIELD_COUNT; fld++) {
        sal_memset(&field_def, 0, sizeof(field_def));

        rv = bcmlrd_table_field_def_get(unit, TABLE_OP_PT_INFOt, fld,
                                        &field_def);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_UNAVAIL) {
                continue;  /* Skip unmapped field */
            } else {
                SHR_ERR_EXIT(rv);
            }
        }

        for (index = 0; index < (field_def.depth ? field_def.depth : 1);
             index++) {
            in_flds.field[fld_count]->id = fld;
            if (bcmltm_table_op_pt_stats_read(unit, updated_ptrm_status, fld,
                                              in_flds.field,
                                              &fld_count, index) ==
                    SHR_E_UNAVAIL) {
                SHR_EXIT();
            }
        }
    }
    in_flds.count = fld_count;

    if (entry_exists) {
        SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, hi_pri,
                                            TABLE_OP_PT_INFOt, &in_flds));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, TABLE_OP_PT_INFOt, &in_flds));
    }

   last_trans_id[unit][ptrm_status->lt_id] = ptrm_status->lt_trans_id;
exit:
    if (in_flds.field) {
        shr_famm_free(table_op_pt_info_arr_hdl[unit], in_flds.field,
                      TABLE_OP_PT_INFO_MAX_FIELD_LEN);
    }
    if (out_flds.field) {
        shr_famm_free(table_op_pt_info_arr_hdl[unit], out_flds.field,
                      TABLE_OP_PT_INFO_MAX_FIELD_LEN);
    }
    SHR_FUNC_EXIT();
}

static int
bcmltm_pt_status_info_check(int unit, uint32_t lt_id)
{
    int rv;
    bcmltm_lt_state_t *lt_state = NULL;

    if (lt_id == BCMLTM_LTID_INVALID) {
        return FALSE;
    }
    rv = bcmltm_state_lt_get(unit, lt_id, &lt_state);
    if (SHR_FAILURE(rv) || (lt_state == NULL)) {
        return FALSE;
    }

    return lt_state->table_op_pt_info_enable;
}

/*******************************************************************************
 * Public Functions
 */

int
bcmltm_table_op_pt_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!table_op_pt_info_arr_hdl[unit]) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(TABLE_OP_PT_INFO_MAX_FIELD_LEN,
                               &table_op_pt_info_arr_hdl[unit]));
    /* Set all elemets to 0xFFFFFFFF */
    sal_memset(last_trans_id, 0xFF, sizeof(last_trans_id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_table_op_pt_info_cleanup(int unit)
{
    if (table_op_pt_info_arr_hdl[unit]) {
        shr_famm_hdl_delete(table_op_pt_info_arr_hdl[unit]);
        table_op_pt_info_arr_hdl[unit] = 0;
    }

    return SHR_E_NONE;
}

int
bcmltm_pt_status_mreq_keyed_lt(int unit, bcmltd_sid_t ltid,
                               uint32_t lt_trans_id,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_keyed_lt_mrsp_info_t *mrsp_info)
{
    bcmltm_table_op_pt_info_t ptrm_status;
    unsigned int i = 0;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_pt_status_info_check(unit, ltid)) {
        SHR_EXIT();
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.lt_trans_id = lt_trans_id;
    if (mrsp_info->pt_op_info) {
        ptrm_status.pt_sid_cnt = mrsp_info->pt_op_info->rsp_entry_sid_cnt;
        sal_memcpy(ptrm_status.pt_sid, mrsp_info->pt_op_info->rsp_entry_sid,
                   sizeof(ptrm_status.pt_sid[0])*ptrm_status.pt_sid_cnt);
        /* For a single PTM request, instance would still be same for all PTs */
        for (i = 0; i < ptrm_status.pt_sid_cnt; i++) {
            ptrm_status.pt_instance[i] = pt_dyn_info->tbl_inst;
        }
        ptrm_status.pt_index_cnt = mrsp_info->pt_op_info->rsp_entry_index_cnt;
        sal_memcpy(ptrm_status.pt_index,
                   mrsp_info->pt_op_info->rsp_entry_index,
                   sizeof(ptrm_status.pt_index[0])*ptrm_status.pt_index_cnt);
        if (mrsp_info->pt_op_info->rsp_entry_sid_data_cnt) {
            ptrm_status.pt_sid_data_cnt =
                mrsp_info->pt_op_info->rsp_entry_sid_data_cnt;
            sal_memcpy(ptrm_status.pt_sid_data,
                       mrsp_info->pt_op_info->rsp_entry_sid_data,
                       sizeof(ptrm_status.pt_sid_data[0])*
                       ptrm_status.pt_sid_data_cnt);
            ptrm_status.pt_index_data_cnt =
                mrsp_info->pt_op_info->rsp_entry_index_data_cnt;
            sal_memcpy(ptrm_status.pt_index_data,
                       mrsp_info->pt_op_info->rsp_entry_index_data,
                       sizeof(ptrm_status.pt_index_data[0])*
                       ptrm_status.pt_index_data_cnt);
        }
    }

    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_pt_status_mreq_indexed_lt(int unit, bcmltd_sid_t ltid,
                                 uint32_t lt_trans_id, uint32_t ptid,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    bcmltm_table_op_pt_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_pt_status_info_check(unit, ltid)) {
        SHR_EXIT();
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.lt_trans_id = lt_trans_id;
    ptrm_status.pt_sid_cnt = 1;
    ptrm_status.pt_sid[0] = ptid;
    ptrm_status.pt_index_cnt = 1;
    ptrm_status.pt_index[0] = pt_dyn_info->index;
    ptrm_status.pt_instance[0] = pt_dyn_info->tbl_inst;

    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_pt_status_ireq_op(int unit, bcmltd_sid_t ltid, uint32_t lt_trans_id,
                         uint32_t ptid,
                         bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmltm_pt_status_mreq_indexed_lt(unit, ltid, lt_trans_id,
                                                     ptid, pt_dyn_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_pt_status_ireq_hash_lt(int unit, bcmltd_sid_t ltid, uint32_t lt_trans_id,
                              uint32_t ptid, uint32_t entry_index,
                              bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info)
{
    bcmltm_table_op_pt_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_pt_status_info_check(unit, ltid)) {
        SHR_EXIT();
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.lt_trans_id = lt_trans_id;
    ptrm_status.pt_sid_cnt = 1;
    ptrm_status.pt_sid[0] = ptid;
    ptrm_status.pt_index_cnt = 1;
    ptrm_status.pt_index[0] = entry_index;
    ptrm_status.pt_instance[0] = pt_dyn_hash_info->tbl_inst;

    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

