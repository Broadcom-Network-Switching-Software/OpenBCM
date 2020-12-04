/*! \file bcmpc_imm_port_monitor.c
 *
 * PC_PORT_MONITORt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORT_MONITORt

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_port_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Port config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
port_monitor_fields_parse(int unit,
                          const bcmltd_field_t *key_flds,
                          const bcmltd_field_t *data_flds,
                          uint32_t *pc_thread_dis)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;
        switch (fid) {
        case PC_PORT_MONITORt_PM_THREAD_DISABLEf:
            *pc_thread_dis = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief LM_CONTROL IMM table change callback function for staging.
 *
 * Handle LM_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
port_monitor_imm_stage(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event_reason,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    uint32_t pc_thread_dis = 0;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
       (port_monitor_fields_parse(unit, key, data, &pc_thread_dis));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_thread_disable_config(unit, pc_thread_dis));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_thread_enable(unit, !pc_thread_dis));
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_port_monitor_entry_parse(int unit, const bcmltd_fields_t *flds,
                            size_t key_size, void *key,
                            size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    uint32_t pc_thread_dis = 0;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        switch (fid) {
        case PC_PORT_MONITORt_PM_THREAD_DISABLEf:
            pc_thread_dis = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    if (data) {
        sal_memcpy(data, &pc_thread_dis, data_size);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_port_monitor_entry_fill(int unit,
                           size_t key_size, void *key,
                           size_t data_size, void *data,
                           bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    uint32_t *pc_thread_dis = data;

    SHR_FUNC_ENTER(unit);

    if (data && data_size != sizeof(uint32_t)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_MONITORt_PM_THREAD_DISABLEf, 0, *pc_thread_dis, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_port_monitor_register(int unit)
{
    bcmimm_lt_cb_t event_hdl;
    bcmpc_db_imm_schema_handler_t db_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_port_monitor_entry_parse;
    db_hdl.entry_fill = pc_port_monitor_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = port_monitor_imm_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config
 * stage to pre-populate the PC_PORT_MONITORt LT entry
 */
int
bcmpc_port_monitor_prepopulate(int unit)
{
    uint32_t pc_thread_dis;
    SHR_FUNC_ENTER(unit);

    pc_thread_dis = 0;
    SHR_IF_ERR_EXIT
         (bcmpc_db_imm_entry_insert(unit, IMM_SID,0, NULL,
                                    sizeof(pc_thread_dis), &pc_thread_dis));
exit:
    SHR_FUNC_EXIT();
}
