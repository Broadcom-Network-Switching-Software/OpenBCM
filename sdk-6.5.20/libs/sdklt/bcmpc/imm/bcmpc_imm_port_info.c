/*! \file bcmpc_imm_port_info.c
 *
 * PC_PORT_INFOt IMM handler.
 *
 * There are two handlers in this file.
 * 1. IMM event handler, \ref bcmimm_lt_cb_t.
 *    The event handler is used to notify PC for the various LT operations.
 *
 * 2. IMM-based DB schema handler, \ref bcmpc_db_imm_schema_handler_t.
 *    The schema handler is used to translate IMM (or LT) field array to/from PC
 *    specific data type.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>

int bcmpc_imm_lt_register(int unit);

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORT_INFOt

/*! The data structure for PC_PORT_INFOt entry. */
typedef struct pc_port_info_entry_s {
    bcmpc_lport_t key;
    bcmpc_port_oper_status_t data;
} pc_port_info_entry_t;

/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_port_info_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_port_info_entry_t_init(pc_port_info_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    sal_memset(&(entry->data), 0, sizeof(bcmpc_port_oper_status_t));
}

/*!
 * \brief Fill PC structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_port_info_entry_set(int unit, uint32_t fid, uint32_t fidx, uint64_t fval,
                       pc_port_info_entry_t *entry)
{
    bcmpc_port_oper_status_t *pcfg = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PORT_INFOt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_PORT_INFOt_LAST_OPERATIONAL_STATEf:
            pcfg->status = fval;
            break;
        case PC_PORT_INFOt_NUM_PORTSf:
            pcfg->num_port = fval;
            break;
        case PC_PORT_INFOt_PORTf:
            if (fidx >= 16) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            pcfg->port_list[fidx] = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_port_info_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Port info data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_port_info_fields_parse(int unit,
                             const bcmltd_field_t *key_flds,
                             const bcmltd_field_t *data_flds,
                             pc_port_info_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid, fidx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_info_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_info_entry_set(unit, fid, fidx, fval, entry));

        fld = fld->next;
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
 * Convert bcmltd_fields_t array \c flds to a bcmpc_lport_t \c key and a
 * bcmpc_port_oper_status_t \c data.
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
pc_imm_port_info_entry_parse(int unit, const bcmltd_fields_t *flds,
                             size_t key_size, void *key,
                             size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_port_info_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_port_info_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_info_entry_set(unit, fid, fidx, fval, &entry));
    }

    if (key) {
        sal_memcpy(key, &entry.key, key_size);
    }

    if (data) {
        sal_memcpy(data, &entry.data, data_size);
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
 * Convert a bcmpc_lport_t \c key and a bcmpc_port_cfg_t \c data to
 * bcmltd_fields_t array \c flds.
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
pc_imm_port_info_entry_fill(int unit,
                            size_t key_size, void *key,
                            size_t data_size, void *data,
                            bcmltd_fields_t *flds)
{
    size_t idx, fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_port_oper_status_t *pcfg = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_INFOt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD(flds, PC_PORT_INFOt_LAST_OPERATIONAL_STATEf,
                               0, pcfg->status, fcnt);
        BCMPC_LT_FIELD_VAL_ADD(flds, PC_PORT_INFOt_NUM_PORTSf, 0,
                               pcfg->num_port, fcnt);
        for (idx = 0; idx < 16; idx++) {
            BCMPC_LT_FIELD_VAL_ADD
                (flds, PC_PORT_INFOt_PORTf, idx, pcfg->port_list[idx], fcnt);
        }
    }

    flds->count = fcnt;

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */

static int
pc_port_info_stage(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    pc_port_info_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_port_info_entry_t_init(&entry);
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_info_fields_parse(unit, key_fields, NULL, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_DELETE:
            SHR_ERR_EXIT(SHR_E_FAIL);
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_port_info_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_port_info_entry_parse;
    db_hdl.entry_fill = pc_imm_port_info_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_port_info_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}
