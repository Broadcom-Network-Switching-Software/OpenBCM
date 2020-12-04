/*! \file bcmpc_imm_port_diag_control.c
 *
 * PC_PORT_DIAG_CONTROLt IMM handler.
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

#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORT_DIAG_CONTROLt

/* Data Structure for PC_PORT_DIAG_CONTROLt entry */
typedef struct pc_port_diag_control_entry_s {
    bcmpc_lport_t key;
    bcmpc_port_diag_control_t data;
} pc_port_diag_control_entry_t;


/*******************************************************************************
 * Private Functions
 */
 /*!
  * \brief Initialize pc_port_diag_control_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_port_diag_control_entry_t_init (pc_port_diag_control_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    sal_memset(&(entry->data), 0, sizeof(entry->data));
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
pc_port_diag_control_entry_set (int unit, uint32_t fid, uint32_t fidx,
                               uint64_t fval,
                               pc_port_diag_control_entry_t *entry)
{
    bcmpc_lport_t  *lport = &entry->key;
    bcmpc_port_diag_control_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PORT_DIAG_CONTROLt_PORT_IDf:
            *lport = fval;
            break;
        case PC_PORT_DIAG_CONTROLt_ERROR_MASK_63_0f:
            config->error_mask_63_0 = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_ERROR_MASK_63_0);
            break;
        case PC_PORT_DIAG_CONTROLt_ERROR_MASK_127_64f:
            config->error_mask_127_64 = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_ERROR_MASK_127_64);
            break;
        case PC_PORT_DIAG_CONTROLt_ERROR_CONTROL_MAPf:
            config->error_control_map = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_ERROR_CONTROL_MAP);
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
 * Parse the input LT fields and save the data to pc_port_diag_control_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_port_diag_control_flds_parse (int unit,
                                   const bcmltd_field_t *key_flds,
                                   const bcmltd_field_t *data_flds,
                                   pc_port_diag_control_entry_t *entry)
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
            (pc_port_diag_control_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_diag_control_entry_set(unit, fid, fidx, fval, entry));

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
 * Convert bcmltd_fields_t array \c flds to bcmpc_lport_t \c key and a
 * bcmpc_port_diag_control_prof_t \c data.
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
pc_port_diag_control_entry_parse (int unit, const bcmltd_fields_t *flds,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_port_diag_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_port_diag_control_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_diag_control_entry_set(unit, fid, fidx, fval, &entry));
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
 * Convert bcmpc_port_lane_t \c key and a bcmpc_port_diag_control_t \c data to
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
pc_port_diag_control_entry_fill (int unit,
                                size_t key_size, void *key,
                                size_t data_size, void *data,
                                bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t  *lport = key;
    bcmpc_port_diag_control_t *config = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*lport)) ||
            (data && data_size != sizeof(*config))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_DIAG_CONTROLt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_DIAG_CONTROLt_ERROR_MASK_63_0f, 0,
             config->error_mask_63_0, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_DIAG_CONTROLt_ERROR_MASK_127_64f, 0,
             config->error_mask_127_64, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_DIAG_CONTROLt_ERROR_CONTROL_MAPf, 0,
             config->error_control_map, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Port Diag control configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for PC_PORT_DIAG_CONTROL LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_port_diag_control_config_validate (int unit, bcmpc_lport_t key,
                                     bool *cfg_valid, bool *cfg_hw_apply)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bool is_internal = FALSE;
    int rv = SHR_E_NONE;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    /*
     * Check if logical to physical port map is available.
     * Do not populate the special (CPU, internal loopback)
     * ports.
     */
     rv = bcmpc_port_phys_map_get(unit, key, &pport);

    if ((rv == SHR_E_NOT_FOUND) || (pport == BCMPC_INVALID_PPORT)) {
        cfg_valid = FALSE;
        SHR_EXIT();
    }

    /* Return for internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == true) {
        cfg_valid = FALSE;
        SHR_EXIT();
    }

    /*
     * Check if PC_PORT LT has entry for this logical port.
     * If not present,
     * set cfg_hw_apply = false, cfg_valid = true and return success
     * If present,
     * number of lanes is 0, means logcial port is not configured
     * set cfg_hw_apply = false, cfg_valid = true and return success
     * if lane_index > number of lanes
     * set cfg_hw_apply = false, cfg_valid = false and return success
     */
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&key),
                                   P(&pcfg));

    if (rv == SHR_E_NOT_FOUND) {
        *cfg_valid = TRUE;
        *cfg_hw_apply = FALSE;
        SHR_EXIT();
    }

    if (pcfg.num_lanes == 0) {
        *cfg_valid = TRUE;
        *cfg_hw_apply = FALSE;
        SHR_EXIT();
    }

     *cfg_valid = TRUE;
     *cfg_hw_apply = TRUE;
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_port_diag_control_stage (int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key_flds,
                           const bcmltd_field_t *data_flds,
                           void *context,
                           bcmltd_fields_t *output_fields)
{
    pc_port_diag_control_entry_t entry;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;


    SHR_FUNC_ENTER(unit);

    pc_port_diag_control_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_diag_control_flds_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_port_diag_control_config_validate(unit, entry.key, &config_valid,
                                             &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_diag_control_flds_parse(unit, NULL, data_flds,
                                                   &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_diag_control_set(unit, &entry.key,
                                                     &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_diag_control_flds_parse(unit, NULL, data_flds,
                                                   &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_diag_control_set(unit, &entry.key,
                                                     &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_diag_control_delete(unit, &entry.key));
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
bcmpc_imm_port_diag_control_register (int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_port_diag_control_entry_parse;
    db_hdl.entry_fill = pc_port_diag_control_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_port_diag_control_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}


