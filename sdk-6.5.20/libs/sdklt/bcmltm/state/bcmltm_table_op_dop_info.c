/*! \file bcmltm_table_op_dop_info.c
 *
 * Interface to update in-memory table with PT status.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_pb_local.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmltd/chip/bcmltd_id.h>

#include <bcmimm/bcmimm_int_comp.h>

#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm_state_internal.h>
#include <bcmltm/bcmltm_table_op_dop_info.h>

#include <bcmpkt/bcmpkt_trace.h>
#include <bcmpkt/bcmpkt_trace_internal.h>

/*******************************************************************************
 * Local definitions
 */
/*! Invalid Index or ID. */
#define BCMPORT_INVALID (-1)

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffffL
#endif

#define MAX_DOP_PHASE       20

/* Arbitrary high table field length, to accommodate for array depth */
#define TABLE_OP_DOP_INFO_MAX_FIELD_LEN 256

static shr_famm_hdl_t table_op_dop_info_arr_hdl[BCMDRD_CONFIG_MAX_UNITS];

static const shr_enum_map_t dop_field_names[] =
{
    BCMPKT_TRACE_DOP_FIELD_NAME_MAP_INIT
};

/*******************************************************************************
 * Private functions
 */
static int
bcmltm_table_op_dop_read_data(int unit, uint32_t pt_id,
                              bcmltm_table_op_dop_info_t *ptrm_status,
                              uint32_t index, int port,
                              shr_pb_t *pb)
{
    bcmpkt_trace_data_t     trace_data;
    uint32_t                trace_data_len;
    uint8_t                 *data = NULL;
    uint32_t                group;
    uint32_t                fid;
    uint32_t                idx;
    uint32_t                *field_data = NULL;
    uint8_t                 field_data_len;
    uint32_t                *fid_list = NULL;
    uint8_t                 fid_count;
    int                     i, j;
    const char              *pt_name = NULL;
    int                     rv;
    uint32_t                max_dop_phase;
    uint32_t                fid_max_count;
    uint32_t                dop_num;
    SHR_PB_LOCAL_DECL(pb);

    SHR_FUNC_ENTER(unit);
    pt_name = bcmdrd_pt_sid_to_name(unit, pt_id);
    rv = bcmpkt_trace_pt_to_dop_info_get(unit, pt_name, data, &dop_num,
                                         &group, &fid);
    if (rv == SHR_E_UNAVAIL) {
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(bcmpkt_trace_max_dop_phase_get(unit, &max_dop_phase));
    trace_data_len = max_dop_phase * sizeof(uint32_t);

    data = sal_alloc(sizeof(uint8_t) * trace_data_len, "bcmaBcmltmDopData");
    trace_data.buf_size = trace_data_len;
    trace_data.buf = data;

    SHR_IF_ERR_EXIT(bcmpkt_trace_dop_data_collect(unit, port, dop_num,
                                                  &trace_data));

    ptrm_status->dop_data[index] = data;

    shr_pb_printf(pb, "\nPT_NAME:%s DOP_NUM:%d GROUP:%d\n",
                  pt_name, dop_num, group);
    for (idx = 0;  idx < trace_data.len; idx++) {
        if ((idx & 0xf) == 0) {
            shr_pb_printf(pb, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            shr_pb_printf(pb, "- ");
        }
            shr_pb_printf(pb, "%02x ", trace_data.buf[idx]);

        if ((idx & 0xf) == 0xf) {
            shr_pb_printf(pb, "\n");
        }
    }
    shr_pb_printf(pb, "\n");

    SHR_IF_ERR_EXIT
        (bcmpkt_trace_max_dop_field_count_get(unit, &fid_max_count));

    fid_list = (uint32_t *)sal_alloc(sizeof(uint32_t)*fid_max_count,
                                     "bcmaBcmltDopFidList");
    fid_count = (uint8_t) fid_max_count;
    SHR_IF_ERR_EXIT(bcmpkt_trace_dop_fids_get(unit, dop_num,
                                              fid_list, &fid_count));

    field_data_len = max_dop_phase * sizeof(uint32_t);
    field_data = (uint32_t *)sal_alloc(field_data_len,
                                       "bcmaBcmltDopFieldData");
    for (i = 0; i < fid_count; i++) {
        field_data_len = max_dop_phase * sizeof(uint32_t);
        sal_memset(field_data, 0, field_data_len);
        SHR_IF_ERR_EXIT(bcmpkt_trace_dop_field_get(unit, trace_data.buf,
                                                   fid_list[i], field_data,
                                                   &field_data_len));
        shr_pb_printf(pb, "%s: ", dop_field_names[fid_list[i]].name);
        for (j = 0; j < field_data_len; j++) {
            shr_pb_printf(pb, "0x%x ", *(uint32_t *)&field_data[j]);
        }
        shr_pb_printf(pb, "\n");
    }

exit:
    SHR_PB_LOCAL_DONE();
    if (field_data) {
        sal_free(field_data);
    }

    if (fid_list) {
        sal_free(fid_list);
    }

    SHR_FUNC_EXIT();
}

static int
bcmltm_table_op_dop_stats_read(int unit,
                               bcmltm_table_op_dop_info_t *ptrm_status,
                               uint32_t attr, bcmltd_field_t **flds,
                               uint32_t *fld_count, uint32_t index, int port)
{
    uint32_t    val;
    int         rv = 0;
    const char *pt_name = NULL;
    uint8_t    *data = NULL;
    int         pipe;

    if (ptrm_status == NULL) {
        /* Entry not found. */
        return SHR_E_PARAM;
    }

    pipe = bcmdrd_dev_logic_port_pipe(unit, port);

    switch (attr) {
    case TABLE_OP_DOP_INFOt_TABLE_IDf:
        flds[*fld_count]->data = ptrm_status->lt_id;
        (*fld_count)++;
        break;
    case TABLE_OP_DOP_INFOt_PT_INSTANCEf:
        if (index < ptrm_status->pt_sid_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_instance[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_DOP_INFOt_PT_ID_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_sid_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_DOP_INFOt_PT_IDf:
        if (index < ptrm_status->pt_sid_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_sid[index];
            flds[*fld_count]->idx = index;
            if ((port) && (port > BCMPORT_INVALID) && (pipe >= 0)) {
                bcmltm_table_op_dop_read_data(unit, ptrm_status->pt_sid[index],
                                              ptrm_status, index, port, NULL);
            }
            (*fld_count)++;
        }
        break;
    case TABLE_OP_DOP_INFOt_PT_INDEX_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_index_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_DOP_INFOt_PT_INDEXf:
        if (index < ptrm_status->pt_index_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_index[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_DOP_INFOt_PT_HITf:
        if (index < ptrm_status->pt_index_cnt) {
            pt_name = bcmdrd_pt_sid_to_name(unit, ptrm_status->pt_sid[index]);
            if ((port) && (port > BCMPORT_INVALID) && (pipe >= 0)) {
                data = ptrm_status->dop_data[index];
                rv = bcmpkt_trace_pt_hit_info_get(unit, pt_name, data, port,
                                                  &val);
                if (SHR_SUCCESS(rv)) {
                    flds[*fld_count]->data = val;
                    flds[*fld_count]->idx = index;
                }
            }
            (*fld_count)++;
        }
        break;
    case TABLE_OP_DOP_INFOt_PT_LOOKUPf:
        if (index < ptrm_status->pt_index_cnt) {
            pt_name = bcmdrd_pt_sid_to_name(unit, ptrm_status->pt_sid[index]);
            if ((port) && (port > BCMPORT_INVALID) && (pipe >= 0)) {
                data = ptrm_status->dop_data[index];
                rv = bcmpkt_trace_pt_lookup_info_get(unit, pt_name, data, port,
                                                     &val);
                if (SHR_SUCCESS(rv)) {
                    flds[*fld_count]->data = val;
                    flds[*fld_count]->idx = index;
                }
            }
            (*fld_count)++;
        }
        break;
    case TABLE_OP_DOP_INFOt_PORT_IDf:
        flds[*fld_count]->data = ptrm_status->port_id;
        (*fld_count)++;
        break;
    default:
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

static int
bcmltm_table_op_dop_info_update_data(int unit,
                                     bcmltm_table_op_dop_info_t *ptrm_status)
{
    bcmltd_fields_t in_flds, out_flds;
    uint32_t fld, fld_count=0, index;
    int entry_exists = 0, hi_pri = 1;
    bcmlrd_field_def_t  field_def;
    int rv;
    int port = BCMPORT_INVALID;

    SHR_FUNC_ENTER(unit);

    /* Init for error return. */
    out_flds.field = NULL;

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(table_op_dop_info_arr_hdl[unit], 1);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = TABLE_OP_DOP_INFO_MAX_FIELD_LEN;
    out_flds.field = shr_famm_alloc(table_op_dop_info_arr_hdl[unit],
                                    TABLE_OP_DOP_INFO_MAX_FIELD_LEN);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Set logical table ID */
    fld = TABLE_OP_DOP_INFOt_TABLE_IDf;
    in_flds.field[0]->id = fld;
    if (bcmltm_table_op_dop_stats_read(unit, ptrm_status, fld,
                                       in_flds.field, &fld_count, 0,
                                       BCMPORT_INVALID) ==
            SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    /* Lookup the entry. Update the entry if exists. */
    if (bcmimm_entry_lookup(unit, TABLE_OP_DOP_INFOt,
                            &in_flds, &out_flds) != SHR_E_NOT_FOUND) {
        entry_exists = 1;
        /* Extract the PORT ID */
        for (fld = 0; fld < out_flds.count; fld++) {
            if (out_flds.field[fld]->id == TABLE_OP_DOP_INFOt_PORT_IDf) {
                port = out_flds.field[fld]->data;
                ptrm_status->port_id = port;
                break;
            }
        }
    }

    shr_famm_free(table_op_dop_info_arr_hdl[unit], in_flds.field, in_flds.count);

    in_flds.count = TABLE_OP_DOP_INFO_MAX_FIELD_LEN;
    in_flds.field = shr_famm_alloc(table_op_dop_info_arr_hdl[unit],
                                   TABLE_OP_DOP_INFO_MAX_FIELD_LEN);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Fill complete field set. */
    fld_count = 0;
    for (fld = 0; fld < TABLE_OP_DOP_INFOt_FIELD_COUNT; fld++) {
        sal_memset(&field_def, 0, sizeof(field_def));

        rv = bcmlrd_table_field_def_get(unit, TABLE_OP_DOP_INFOt, fld,
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
            if (bcmltm_table_op_dop_stats_read(unit, ptrm_status, fld,
                                              in_flds.field,
                                              &fld_count, index, port) ==
                    SHR_E_UNAVAIL) {
                SHR_EXIT();
            }
        }
    }
    in_flds.count = fld_count;

    if (entry_exists) {
        SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, hi_pri,
                                            TABLE_OP_DOP_INFOt, &in_flds));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, TABLE_OP_DOP_INFOt, &in_flds));
    }

exit:
    if (in_flds.field) {
        shr_famm_free(table_op_dop_info_arr_hdl[unit], in_flds.field,
                      TABLE_OP_DOP_INFO_MAX_FIELD_LEN);
    }
    if (out_flds.field) {
        shr_famm_free(table_op_dop_info_arr_hdl[unit], out_flds.field,
                      TABLE_OP_DOP_INFO_MAX_FIELD_LEN);
    }

    for (index=0; index<ptrm_status->pt_sid_cnt; index++) {
        if (ptrm_status->dop_data[index]) {
            sal_free(ptrm_status->dop_data[index]);
            ptrm_status->dop_data[index] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcmltm_dop_status_info_check(int unit, uint32_t lt_id)
{
    int rv;
    bcmltm_lt_state_t *lt_state = NULL;

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
bcmltm_table_op_dop_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!table_op_dop_info_arr_hdl[unit]) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(TABLE_OP_DOP_INFO_MAX_FIELD_LEN,
                               &table_op_dop_info_arr_hdl[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_table_op_dop_info_cleanup(int unit)
{
    if (table_op_dop_info_arr_hdl[unit]) {
        shr_famm_hdl_delete(table_op_dop_info_arr_hdl[unit]);
        table_op_dop_info_arr_hdl[unit] = 0;
    }

    return SHR_E_NONE;
}

int
bcmltm_dop_status_mreq_keyed_lt(int unit, bcmltd_sid_t ltid,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_keyed_lt_mrsp_info_t *mrsp_info)
{
    unsigned int i = 0;
    bcmltm_table_op_dop_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_dop_status_info_check(unit, ltid)) {
        SHR_EXIT();
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    /* For a single PTM request, instance would still be same for all PTs */
    for (i = 0; i < ptrm_status.pt_sid_cnt; i++) {
        ptrm_status.pt_instance[i] = pt_dyn_info->tbl_inst;
    }
    if (mrsp_info->pt_op_info) {
        ptrm_status.pt_sid_cnt = mrsp_info->pt_op_info->rsp_entry_sid_cnt;
        sal_memcpy(ptrm_status.pt_sid, mrsp_info->pt_op_info->rsp_entry_sid,
                   sizeof(ptrm_status.pt_sid[0])*ptrm_status.pt_sid_cnt);
        ptrm_status.pt_index_cnt = mrsp_info->pt_op_info->rsp_entry_index_cnt;
        sal_memcpy(ptrm_status.pt_index,
                   mrsp_info->pt_op_info->rsp_entry_index,
                   sizeof(ptrm_status.pt_index[0])*ptrm_status.pt_index_cnt);
    }

    SHR_IF_ERR_EXIT(bcmltm_table_op_dop_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_dop_status_mreq_indexed_lt(int unit, bcmltd_sid_t ltid,
                                  uint32_t ptid,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    bcmltm_table_op_dop_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_dop_status_info_check(unit, ltid)) {
        SHR_EXIT();
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.pt_sid_cnt = 1;
    ptrm_status.pt_sid[0] = ptid;
    ptrm_status.pt_index_cnt = 1;
    ptrm_status.pt_index[0] = pt_dyn_info->index;
    ptrm_status.pt_instance[0] = pt_dyn_info->tbl_inst;

    SHR_IF_ERR_EXIT(bcmltm_table_op_dop_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_dop_status_ireq_op(int unit, bcmltd_sid_t ltid,
                          uint32_t ptid,
                          bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmltm_dop_status_mreq_indexed_lt(unit, ltid,
                                                     ptid, pt_dyn_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_dop_status_ireq_hash_lt(int unit, bcmltd_sid_t ltid,
                               uint32_t ptid, uint32_t entry_index,
                               bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info)
{
    bcmltm_table_op_dop_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_dop_status_info_check(unit, ltid)) {
        SHR_EXIT();
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.pt_sid_cnt = 1;
    ptrm_status.pt_sid[0] = ptid;
    ptrm_status.pt_index_cnt = 1;
    ptrm_status.pt_index[0] = entry_index;
    ptrm_status.pt_instance[0] = pt_dyn_hash_info->tbl_inst;

    SHR_IF_ERR_EXIT(bcmltm_table_op_dop_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}
