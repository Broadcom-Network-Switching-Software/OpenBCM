/*! \file bcmltm_device_op_pt_info.c
 *
 * Interface to update in-memory table DEVICE_OP_PT_INFO.
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
#include <bcmltm/bcmltm_state_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_device_op_pt_info.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

static shr_famm_hdl_t device_op_pt_info_arr_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Populate the field set for DEVICE_OP_PT_INFO IMM table operations.
 *
 * This routine populates the field set for DEVICE_OP_PT_INFO IMM table
 * operations.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ptrm_status PT passthrough operation status.
 * \param [in] attr Field Id to be populated.
 * \param [out] flds Pointer to field set.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
static int
device_op_pt_stats_read(int unit, bcmltm_device_op_pt_info_t *ptrm_status,
                        uint32_t attr, bcmltd_field_t **flds)
{
    if (ptrm_status == NULL) {
        /* Entry not found. */
        return SHR_E_PARAM;
    }

    switch (attr) {
    case DEVICE_OP_PT_INFOt_PT_IDf:
        flds[attr]->data = ptrm_status->pt_id;
        break;
    case DEVICE_OP_PT_INFOt_PT_INDEXf:
        flds[attr]->data = ptrm_status->pt_index;
        break;
    case DEVICE_OP_PT_INFOt_PT_INSTANCEf:
        flds[attr]->data = ptrm_status->pt_instance;
        break;
    case DEVICE_OP_PT_INFOt_PT_OPf:
        flds[attr]->data = ptrm_status->pt_op;
        break;
    case DEVICE_OP_PT_INFOt_PT_OP_STATUSf:
        flds[attr]->data = ptrm_status->pt_op_status;
        break;
    default:
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Update or insert an entry into DEVICE_OP_PT_INFO IMM table.
 *
 * This routine updates or inserts an entry into DEVICE_OP_PT_INFO IMM table.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ptrm_status PT passthrough operation status.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
static int
device_op_pt_info_update_data(int unit, bcmltm_device_op_pt_info_t *ptrm_status)
{
    bcmltd_fields_t in_flds, out_flds;
    uint32_t fld;
    int entry_exists = 0;
    bcmlrd_field_def_t  field_def;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Init for error return. */
    out_flds.field = NULL;

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(device_op_pt_info_arr_hdl[unit], 1);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = DEVICE_OP_PT_INFOt_FIELD_COUNT;
    out_flds.field = shr_famm_alloc(device_op_pt_info_arr_hdl[unit],
                                    DEVICE_OP_PT_INFOt_FIELD_COUNT);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Set logical table ID */
    fld = DEVICE_OP_PT_INFOt_PT_IDf;
    in_flds.field[0]->id = fld;
    if (device_op_pt_stats_read(unit, ptrm_status, fld,
                                in_flds.field) == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    /* Lookup the entry. Update the entry if exists. */
    if (bcmimm_entry_lookup(unit, DEVICE_OP_PT_INFOt, &in_flds,
                            &out_flds) != SHR_E_NOT_FOUND) {
        entry_exists = 1;
    }

    shr_famm_free(device_op_pt_info_arr_hdl[unit], in_flds.field, in_flds.count);

    in_flds.count = DEVICE_OP_PT_INFOt_FIELD_COUNT;
    in_flds.field = shr_famm_alloc(device_op_pt_info_arr_hdl[unit],
                                   DEVICE_OP_PT_INFOt_FIELD_COUNT);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Fill complete field set. */
    for (fld = 0; fld < DEVICE_OP_PT_INFOt_FIELD_COUNT; fld++) {
        sal_memset(&field_def, 0, sizeof(field_def));

        rv = bcmlrd_table_field_def_get(unit, DEVICE_OP_PT_INFOt, fld,
                                        &field_def);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_UNAVAIL) {
                continue;  /* Skip unmapped field */
            } else {
                SHR_ERR_EXIT(rv);
            }
        }

        in_flds.field[fld]->id = fld;
        if (device_op_pt_stats_read(unit, ptrm_status, fld,
                                    in_flds.field) == SHR_E_UNAVAIL) {
            SHR_EXIT();
        }
    }
    in_flds.count = DEVICE_OP_PT_INFOt_FIELD_COUNT;

    if (entry_exists) {
        SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, 1, DEVICE_OP_PT_INFOt,
                                            &in_flds));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, DEVICE_OP_PT_INFOt, &in_flds));
    }

exit:
    if (in_flds.field) {
        shr_famm_free(device_op_pt_info_arr_hdl[unit], in_flds.field,
                      DEVICE_OP_PT_INFOt_FIELD_COUNT);
    }
    if (out_flds.field) {
        shr_famm_free(device_op_pt_info_arr_hdl[unit], out_flds.field,
                      DEVICE_OP_PT_INFOt_FIELD_COUNT);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmltm_device_op_pt_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!device_op_pt_info_arr_hdl[unit]) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(DEVICE_OP_PT_INFOt_FIELD_COUNT,
                               &device_op_pt_info_arr_hdl[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_device_op_pt_info_cleanup(int unit)
{
    if (device_op_pt_info_arr_hdl[unit]) {
        shr_famm_hdl_delete(device_op_pt_info_arr_hdl[unit]);
        device_op_pt_info_arr_hdl[unit] = 0;
    }

    return SHR_E_NONE;
}

int
bcmltm_device_op(int unit, uint32_t ptid, bcmlt_pt_opcode_t pt_op,
                 int tbl_inst, int entry_index, int op_status)
{
    bcmltm_device_op_pt_info_t ptrm_status;
    SHR_FUNC_ENTER(unit);

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.pt_id = ptid;
    ptrm_status.pt_index = entry_index;
    ptrm_status.pt_instance = tbl_inst;
    ptrm_status.pt_op = pt_op;
    /* If return value < 0 then failure(1) else success(0) */
    ptrm_status.pt_op_status =  (op_status < 0) ? 1 : 0;

    SHR_IF_ERR_EXIT
        (device_op_pt_info_update_data(unit, &ptrm_status));
    SHR_IF_ERR_EXIT
        (bcmltm_dev_ptstat_update(unit, ptid, pt_op,
                                  ptrm_status.pt_op_status));

exit:
    SHR_FUNC_EXIT();
}

