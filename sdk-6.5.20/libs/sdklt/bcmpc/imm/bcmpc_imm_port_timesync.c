/*! \file bcmpc_imm_port_timesync.c
 *
 * PC_PORT_TIMESYNCt IMM handler.
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
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORT_TIMESYNCt

/*! The data structure for PC_PORT_TIMESYNCt entry. */
typedef struct pc_port_timesync_entry_s {
    bcmpc_lport_t key;
    bcmpc_port_timesync_t data;
} pc_port_timesync_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_port_timesync_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_port_timesync_entry_t_init(pc_port_timesync_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    bcmpc_port_timesync_t_init(&entry->data);
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
pc_port_timesync_entry_set(int unit, uint32_t fid, uint32_t fidx, uint64_t fval,
                           pc_port_timesync_entry_t *entry)
{
    bcmpc_port_timesync_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PORT_TIMESYNCt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_PORT_TIMESYNCt_IEEE_1588f:
            config->ieee_1588 = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_IEEE_1588_ENABLE);
            break;
        case PC_PORT_TIMESYNCt_ONE_STEP_TIMESTAMPf:
            config->one_step_timestamp = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_ONE_STEP_TIMESTAMP);
            break;
        case PC_PORT_TIMESYNCt_IS_SOPf:
            config->is_sop = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_IS_SOP);
            break;
        case PC_PORT_TIMESYNCt_TS_COMP_MODEf:
            config->ts_comp_mode = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_TS_COMP_MODE);
            break;
        case PC_PORT_TIMESYNCt_STAGE_0_MODEf:
            config->stg0_mode = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_STG0_MODE);
            break;
        case PC_PORT_TIMESYNCt_STAGE_1_MODEf:
            config->stg1_mode = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_STG1_MODE);
            break;
        case PC_PORT_TIMESYNCt_FRACTIONAL_DIVISORf:
            config->fractional_divisor = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_TIMESYNC_LT_FLD_SDM_VAL);
            break;
        case PC_PORT_TIMESYNCt_OPERATIONAL_STATEf:
            config->oper_state = fval;
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
 * Parse the input LT fields and save the data to pc_port_timesync_entry_t.
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
pc_lt_port_timesync_fields_parse(int unit,
                                 const bcmltd_field_t *key_flds,
                                 const bcmltd_field_t *data_flds,
                                 pc_port_timesync_entry_t *entry)
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
            (pc_port_timesync_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_timesync_entry_set(unit, fid, fidx, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PC_PORT_TIMESYNC LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_lt_port_timesync_default_values_init(int unit,
                                        pc_port_timesync_entry_t *lt_entry)
{

    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_port_timesync_t *config = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    config = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_IEEE_1588_ENABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_IEEE_1588f,
                                     1, &def_val, &num));
        config->ieee_1588 = (bool)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_IEEE_1588_ENABLE);
    }

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_ONE_STEP_TIMESTAMP)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_ONE_STEP_TIMESTAMPf,
                                     1, &def_val, &num));
        config->one_step_timestamp = (bool)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_ONE_STEP_TIMESTAMP);
    }

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_IS_SOP)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_IS_SOPf,
                                     1, &def_val, &num));
        config->is_sop = (bool)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_IS_SOP);
    }

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_TS_COMP_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_TS_COMP_MODEf,
                                     1, &def_val, &num));
        config->ts_comp_mode = (uint32_t)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_TS_COMP_MODE);
    }

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_STG0_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_STAGE_0_MODEf,
                                     1, &def_val, &num));
        config->stg0_mode = (uint32_t)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_STG0_MODE);
    }

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_STG1_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_STAGE_1_MODEf,
                                     1, &def_val, &num));
        config->stg1_mode = (uint32_t)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_STG1_MODE);
    }

    if (!BCMPC_LT_FIELD_GET(config->fbmp,
        BCMPC_TIMESYNC_LT_FLD_SDM_VAL)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORT_TIMESYNCt,
                                     PC_PORT_TIMESYNCt_FRACTIONAL_DIVISORf,
                                     1, &def_val, &num));
        config->fractional_divisor = (uint32_t)def_val;
        BCMPC_LT_FIELD_SET(config->fbmp,
                           BCMPC_TIMESYNC_LT_FLD_SDM_VAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for TX_TAPS LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_port_timesync_config_validate(int unit, bcmpc_lport_t key,
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
     * Return success even if the logical to physical port map
     * is not available or pport is invalid, because the logcial to
     * physical port configuratin can be done later, so the
     * tx_taps configuration should not be blocked.
     * set cfg_hw_apply = false and return success
     * Indicate the operational state of the port in pc_port_status.
     */
    rv = bcmpc_port_phys_map_get(unit, key, &pport);

    if ((rv == SHR_E_NOT_FOUND) || (pport == BCMPC_INVALID_PPORT)) {
        *cfg_valid = FALSE;
        SHR_EXIT();
    }

    /* Return error for internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == TRUE) {
        *cfg_valid = FALSE;
        *cfg_hw_apply = FALSE;
        SHR_EXIT();
    }

    /*
     * Check if PC_PORT LT has entry for this logical port.
     * If not present,
     * set cfg_hw_apply = false, cfg_valid = true and return success
     * If present,
     * number of lanes is 0, means logcial port is not configured
     * set cfg_hw_apply = false, cfg_valid = true and return success
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

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmpc_lport_t \c key and a
 * bcmpc_port_timesync_t \c data.
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
pc_imm_port_timesync_entry_parse(int unit, const bcmltd_fields_t *flds,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_port_timesync_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_port_timesync_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_timesync_entry_set(unit, fid, fidx, fval, &entry));
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
 * Convert a bcmpc_lport_t \c key and a bcmpc_port_timesync_t \c data to
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
pc_imm_port_timesync_entry_fill(int unit,
                                size_t key_size, void *key,
                                size_t data_size, void *data,
                                bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_port_timesync_t *config = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_IEEE_1588f, 0, config->ieee_1588, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_ONE_STEP_TIMESTAMPf, 0,
             config->one_step_timestamp, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_IS_SOPf, 0, config->is_sop, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_TS_COMP_MODEf, 0,
             config->ts_comp_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_STAGE_0_MODEf, 0,
             config->stg0_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_STAGE_1_MODEf, 0,
             config->stg1_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_FRACTIONAL_DIVISORf, 0,
             config->fractional_divisor, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_TIMESYNCt_OPERATIONAL_STATEf, 0,
             config->oper_state, fcnt);
    }

    flds->count = fcnt;

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
pc_port_timesync_stage(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event,
                       const bcmltd_field_t *key_flds,
                       const bcmltd_field_t *data_flds,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    pc_port_timesync_entry_t entry;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_port_timesync_entry_t_init(&entry);
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_timesync_fields_parse(unit, key_flds, NULL, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    /*
     * The TIMESYNC staging function is invoked for any operation on
     * PC_TIMESYNC LT. During pre-config stage of System Manager
     * initialization, the staging function gets invoked for any
     * PC_TIMESYNC LT entry in the config.yml. Logical port
     * hardware resources are configured and applied at a lateri
     * stage during the run stage of the System Manager initialization.
     *
     * During the system initialization, the PC omcponent is in
     * bulk configuration mode since the configuration is applied
     * run-stage. After the configuration is applied, the PC
     * configuration is switched to per-port configuration mode.
     *
     * During pre-config stage, since the PC component is in bulk
     * configuration mode, validation on the user input is not done
     * in this stage and only done during run-stage when the
     * configuration is being applied to the hardware.
     *
     * So, if PC compoenent is in bulk configuration mode, and if
     * TIMESYNC LT staging function gets called indicating an
     * LT entry in the config.yml, return success to populate the
     * IMM DB with the user provisioned values.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    /*
     * If the code reaches this point, it means the user is trying to
     * update the TIMESYNC LT entry and the system is run-stage.
     * Do not validate the user configured input, process it for
     * configuring the hardware. Reject the LT operation on invalid
     * input. Do not reject or fail the operation on this LT due to
     * dependency on other LT's.
     * For example:
     * The system could have boot without providing a config.yml
     * or PC component configuration. All the PC LT's should
     * support any provisioning sequence and there should not be
     * any restriction on the user to provision the PC component
     * in a sequence.
     *
     * On TIMESYNC LT operation, validate if the config can be
     * applied to the hardware due to dependencies on other LTs
     * provisioning. If this check fails, return success and
     * update the operational state of the TIMESYNC LT entry
     * as not valid.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_timesync_fields_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_port_timesync_config_validate(unit, entry.key, &config_valid,
                                          &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_timesync_fields_parse(unit,
                                                  NULL,
                                                  data_flds,
                                                  &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_timesync_default_values_init(unit, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_timesync_insert(unit,
                                                     entry.key,
                                                     &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_timesync_fields_parse(unit,
                                                  NULL,
                                                  data_flds,
                                                  &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_timesync_update(unit,
                                                     entry.key,
                                                     &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_timesync_default_values_init(unit, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_timesync_delete(unit,
                                                     entry.key,
                                                     &entry.data));
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
bcmpc_imm_port_timesync_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_port_timesync_entry_parse;
    db_hdl.entry_fill = pc_imm_port_timesync_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_port_timesync_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PORT_TIMESYNC LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 */
int
bcmpc_timesync_config_prepopulate(int unit, bcmpc_lport_t lport,
                                  bcmpc_port_cfg_t *pcfg, bool null_entry)
{
    bcmpc_drv_t *pc_drv = NULL;
    pc_port_timesync_entry_t entry;
    bool is_internal = false;
    int rv = SHR_E_NONE;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    pc_port_timesync_entry_t_init(&entry);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }
    if (!null_entry) {
        /*
         * Check if logical to physical port map is available.
         * Do not populate the special (CPU, internal loopback)
         * ports.
         */
        if (pcfg->pport == BCMPC_INVALID_PPORT) {
            SHR_EXIT();
        }

        /* Do not populate the LT for the internal (CPU, LOOPBACK) ports. */
        SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

        SHR_IF_ERR_EXIT
            (pc_drv->is_internal_port(unit, pcfg->pport, &is_internal));

        if (is_internal == true) {
            SHR_EXIT();
        }
    }

    fid = PC_PORT_TIMESYNCt_PORT_IDf;
    fval = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_port_timesync_entry_set(unit, fid, 0, fval, &entry));

    /*
     * Check if PC_PORT_TIMESYNC LT for this logical port
     * is already present in the DB.
     * If not present, insert the default values based on the
     * current port configuration.
     * If present, then skip this operation.
     */

    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   P(&entry.key), P(&entry.data));

    if (rv == SHR_E_NOT_FOUND) {
        /*
         * If null_entry is set, do not get default values from the hardware.
         * null_entry is TRUE only during pre-config stage get the defaults
         * from the logical table map file.
         */
        if (!null_entry) {
            /* Get default values for the PC_PORT_TIMESYNC LT. */
            SHR_IF_ERR_EXIT
                (pc_lt_port_timesync_default_values_init(unit, &entry));
        }

        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                       sizeof(bcmpc_lport_t), &lport,
                                       sizeof(bcmpc_port_timesync_t),
                                       &entry.data));
    } else {
        /* Check the operational state of the entry and mark it as valid. */
        if (entry.data.oper_state != BCMPC_VALID) {
            entry.data.oper_state = BCMPC_VALID;

            SHR_IF_ERR_EXIT
                (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                           sizeof(bcmpc_lport_t), &lport,
                                           sizeof(bcmpc_port_timesync_t),
                                           &entry.data));
        }
    }


exit:
    SHR_FUNC_EXIT();
}

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated
 * PC_PORT_TIMESYNC LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_timesync_config_delete(int unit, bcmpc_lport_t lport)
{
    pc_port_timesync_entry_t entry;
    int rv = SHR_E_NONE;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    pc_port_timesync_entry_t_init(&entry);

    fid = PC_PORT_TIMESYNCt_PORT_IDf;
    fval = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_port_timesync_entry_set(unit, fid, 0, fval, &entry));

    /*
     * Check if PC_PORT_TIMESYNC LT for this logical port
     * is present in the DB.
     */

    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   P(&entry.key), P(&entry.data));

    entry.data.oper_state = BCMPC_PORT_UNKNOWN;

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                       sizeof(bcmpc_lport_t), &lport,
                                       sizeof(bcmpc_port_timesync_t),
                                       &entry.data));
    }


exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config stage
 * to pre-populate all the PC_PORT_TIMESYNC LT entries
 * with null entries.
 */
int
bcmpc_port_timesync_prepopulate_all(int unit)
{
    uint64_t lport_min = 0, lport_max = 0, lport = 0;
    bcmpc_port_timesync_t ts;
    bcmpc_lport_t key;
    uint32_t num;
    pc_port_timesync_entry_t entry;


    SHR_FUNC_ENTER(unit);

    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, IMM_SID, PC_PORT_TIMESYNCt_PORT_IDf,
                              1, &lport_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, IMM_SID, PC_PORT_TIMESYNCt_PORT_IDf,
                              1, &lport_max, &num));

    pc_port_timesync_entry_t_init(&entry);
    SHR_IF_ERR_EXIT
        (pc_lt_port_timesync_default_values_init(unit, &entry));

    sal_memset(&ts, 0, sizeof(ts));
    sal_memcpy(&ts, &entry.data, sizeof(bcmpc_port_timesync_t));
    ts.oper_state = BCMPC_PORT_UNKNOWN;

    for (lport = lport_min; lport <= lport_max; lport++) {
         key = (bcmpc_lport_t) lport;
         SHR_IF_ERR_EXIT
             (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                        sizeof(bcmpc_lport_t), &key,
                                        sizeof(bcmpc_port_timesync_t),
                                        &ts));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PORT_TIMESYNC LT
 * entry OPERATIONAL_STATE.
 */
int
bcmpc_timesync_oper_state_update(int unit, bcmpc_lport_t lport,
                                 bcmpc_pport_t pport,
                                 bcmpc_entry_state_t oper_state)
{
    bcmpc_drv_t *pc_drv = NULL;
    pc_port_timesync_entry_t entry;
    bool is_internal = false;

    SHR_FUNC_ENTER(unit);

    pc_port_timesync_entry_t_init(&entry);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    /*
     * Check if logical to physical port map is available.
     * Do not populate the special (CPU, internal loopback)
     * ports.
     */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    /* Do not populate the LT for the internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT
        (pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == true) {
        SHR_EXIT();
    }

    /* Update the operational state of the entry.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_port_timesync_t),
                                   &entry.data));

    entry.data.oper_state = oper_state;

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_port_timesync_t),
                                   &entry.data));

exit:
    SHR_FUNC_EXIT();
}
