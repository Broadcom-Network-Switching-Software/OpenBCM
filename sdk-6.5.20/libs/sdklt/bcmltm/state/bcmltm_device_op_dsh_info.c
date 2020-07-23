/*! \file bcmltm_device_op_dsh_info.c
 *
 * Interface to update in-memory table DEVICE_OP_DSH_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>

#include <shr/shr_debug.h>

#include <bcmltd/chip/bcmltd_id.h>

#include <bcmimm/bcmimm_int_comp.h>

#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_state_internal.h>
#include <bcmltm/bcmltm_device_op_dsh_info.h>


/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

/*!
 * \brief Add a field to the field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fval Field value, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define DSH_LT_FIELD_VAL_ADD(_flds, _fid, _fval, _fcnt) \
            do { \
                assert(_flds.count > fcnt); \
                _flds.field[_fcnt]->id = _fid; \
                _flds.field[_fcnt]->idx = 0; \
                _flds.field[_fcnt]->data = (uint64_t)_fval; \
                _fcnt++; \
            } while (0);

/*!
 * \brief Symbol operation status.
 *
 * This structure contains the status for the dirict symbol operation.
 */
typedef struct bcmltm_device_op_dsh_info_s {

    /*! Symbol id. */
    uint32_t sid;

    /*! Symbol index. */
    uint32_t index;

    /*! Symbol instance. */
    uint32_t instance;

    /*! Symbol operation. */
    bcmlt_pt_opcode_t op;

    /*! Operation status. */
    bool failure;

} bcmltm_device_op_dsh_info_t;

/* DEVICE_OP_DSH_INFO table handler for each unit. */
static shr_famm_hdl_t famm_hdl[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Update or insert an entry into DEVICE_OP_DSH_INFO IMM table.
 *
 * \param [in] unit Logical device ID.
 * \param [in] info Symbol operation status.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
static int
device_op_dsh_info_update(int unit, bcmltm_device_op_dsh_info_t *info)
{
    int rv, is_new = 0;
    uint32_t fcnt;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t in_flds, out_flds;

    SHR_FUNC_ENTER(unit);

    in_flds.field = NULL;
    out_flds.field = NULL;

    /* Return E_NONE when this module is not initialized. */
    if (!famm_hdl[unit]) {
        SHR_EXIT();
    }

    /* Return E_NONE when there is no map to this table. */
    if (SHR_FAILURE(bcmlrd_map_get(unit, DEVICE_OP_DSH_INFOt, &map))) {
        SHR_EXIT();
    }

    /*
     * Check if the entry exists first.
     */
    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(famm_hdl[unit], 1);
    SHR_NULL_CHECK(in_flds.field, SHR_E_MEMORY);

    out_flds.count = DEVICE_OP_DSH_INFOt_FIELD_COUNT;
    out_flds.field = shr_famm_alloc(famm_hdl[unit],
                                    DEVICE_OP_DSH_INFOt_FIELD_COUNT);
    SHR_NULL_CHECK(out_flds.field, SHR_E_MEMORY);

    /* Fill the key field for LOOKUP */
    fcnt = 0;
    DSH_LT_FIELD_VAL_ADD(in_flds, DEVICE_OP_DSH_INFOt_PT_IDf, info->sid, fcnt);
    in_flds.count = fcnt;

    rv = bcmimm_entry_lookup(unit, DEVICE_OP_DSH_INFOt, &in_flds, &out_flds);
    if (rv == SHR_E_NOT_FOUND) {
        is_new = 1;
    }
    shr_famm_free(famm_hdl[unit], in_flds.field, in_flds.count);

    /*
     * Do entry insert or update.
     */
    in_flds.count = DEVICE_OP_DSH_INFOt_FIELD_COUNT;
    in_flds.field = shr_famm_alloc(famm_hdl[unit],
                                   DEVICE_OP_DSH_INFOt_FIELD_COUNT);
    SHR_NULL_CHECK(in_flds.field, SHR_E_MEMORY);

    /* Fill complete fields. */
    fcnt = 0;
    DSH_LT_FIELD_VAL_ADD(in_flds, DEVICE_OP_DSH_INFOt_PT_IDf,
                         info->sid, fcnt);
    DSH_LT_FIELD_VAL_ADD(in_flds, DEVICE_OP_DSH_INFOt_PT_INDEXf,
                         info->index, fcnt);
    DSH_LT_FIELD_VAL_ADD(in_flds, DEVICE_OP_DSH_INFOt_PT_INSTANCEf,
                         info->instance, fcnt);
    DSH_LT_FIELD_VAL_ADD(in_flds, DEVICE_OP_DSH_INFOt_PT_OPf,
                         info->op, fcnt);
    DSH_LT_FIELD_VAL_ADD(in_flds, DEVICE_OP_DSH_INFOt_PT_OP_STATUSf,
                         info->failure, fcnt);
    in_flds.count = fcnt;

    if (is_new) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_OP_DSH_INFOt, &in_flds));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, 1, DEVICE_OP_DSH_INFOt, &in_flds));
    }

exit:
    if (in_flds.field) {
        shr_famm_free(famm_hdl[unit], in_flds.field,
                      DEVICE_OP_DSH_INFOt_FIELD_COUNT);
    }
    if (out_flds.field) {
        shr_famm_free(famm_hdl[unit], out_flds.field,
                      DEVICE_OP_DSH_INFOt_FIELD_COUNT);
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmltm_device_op_dsh_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (famm_hdl[unit]) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(DEVICE_OP_DSH_INFOt_FIELD_COUNT, &famm_hdl[unit]));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_device_op_dsh_info_cleanup(int unit)
{
    if (famm_hdl[unit]) {
        shr_famm_hdl_delete(famm_hdl[unit]);
        famm_hdl[unit] = 0;
    }

    return SHR_E_NONE;
}

int
bcmltm_device_op_dsh_info_set(int unit, uint32_t sid, bcmlt_pt_opcode_t op,
                              int inst, int index, int status)
{
    bcmltm_device_op_dsh_info_t info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&info, 0, sizeof(info));
    info.sid = sid;
    info.index = index;
    info.instance = inst;
    info.op = op;
    info.failure = (status < 0) ? 1 : 0;

    SHR_IF_ERR_EXIT
        (device_op_dsh_info_update(unit, &info));
    SHR_IF_ERR_EXIT
        (bcmltm_dev_ptstat_update(unit, sid, op, info.failure));

exit:
    SHR_FUNC_EXIT();
}

