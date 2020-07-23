/*! \file bcmpc_imm_fdr_port_control.c
 *
 * PC_FDR_CONTROLt IMM handler.
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

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_fdr.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>

/*
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
*/
/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_FDR_CONTROLt

/*! The data structure for PC_FDR_CONTROLt entry. */
typedef struct pc_fdr_port_control_entry_s {
    bcmpc_lport_t key;
    bcmpc_fdr_port_control_t data;
} pc_fdr_port_control_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_fdr_port_control_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_fdr_port_control_entry_t_init(pc_fdr_port_control_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    sal_memset(&entry->data, 0, sizeof(bcmpc_fdr_port_control_t));
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
pc_fdr_port_control_entry_set(int unit, uint32_t fid, uint64_t fval,
                                pc_fdr_port_control_entry_t *entry)
{
    bcmpc_fdr_port_control_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_FDR_CONTROLt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_FDR_CONTROLt_ENABLEf:
            config->enable = (bool)fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FDR_CTRL_LT_FLD_ENABLE);
            break;
        case PC_FDR_CONTROLt_SYMBOL_ERROR_WINDOWf:
            config->window_size = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FDR_CTRL_LT_FLD_SYMERR_WINDOW_SIZE);
            break;
        case PC_FDR_CONTROLt_SYMBOL_ERROR_THRESHOLDf:
            config->symbol_error_threshold = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FDR_CTRL_LT_FLD_SYMERR_THRESHOLD);
            break;
        case PC_FDR_CONTROLt_SYMBOL_ERROR_START_VALUEf:
            config->symbol_error_start_value = (bool)fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FDR_CTRL_LT_FLD_SYMERR_START_VALUE);
            break;
        case PC_FDR_CONTROLt_INTERRUPT_ENABLEf:
            config->interrupt_enable = (bool)fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FDR_CTRL_LT_FLD_FDR_INTR_ENABLE);
            break;
        case PC_FDR_CONTROLt_OPERATIONAL_STATEf:
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
 * Parse the input LT fields and save the data to pc_fdr_port_control_entry_t.
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
pc_lt_fdr_port_control_flds_parse(int unit,
                                    const bcmltd_field_t *key_flds,
                                    const bcmltd_field_t *data_flds,
                                    pc_fdr_port_control_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_fdr_port_control_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_fdr_port_control_entry_set(unit, fid, fval, entry));

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
 * \param [in] lt_entry FEC_FDR_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_fdr_ctrl_lt_default_values_init(int unit,
                                   pc_fdr_port_control_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_fdr_port_control_t *fdr_ctrl = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    fdr_ctrl = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(fdr_ctrl->fbmp,
                            BCMPC_FDR_CTRL_LT_FLD_ENABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get
                (unit, PC_FDR_CONTROLt,
                 PC_FDR_CONTROLt_ENABLEf,
                 1, &def_val, &num));
        fdr_ctrl->enable = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(fdr_ctrl->fbmp,
                            BCMPC_FDR_CTRL_LT_FLD_SYMERR_WINDOW_SIZE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get
                (unit, PC_FDR_CONTROLt,
                 PC_FDR_CONTROLt_SYMBOL_ERROR_WINDOWf,
                 1, &def_val, &num));
        fdr_ctrl->window_size = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(fdr_ctrl->fbmp,
                            BCMPC_FDR_CTRL_LT_FLD_SYMERR_THRESHOLD)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get
                (unit, PC_FDR_CONTROLt,
                 PC_FDR_CONTROLt_SYMBOL_ERROR_THRESHOLDf,
                 1, &def_val, &num));
        fdr_ctrl->symbol_error_threshold = def_val;
    }

    if (!BCMPC_LT_FIELD_GET(fdr_ctrl->fbmp,
                            BCMPC_FDR_CTRL_LT_FLD_SYMERR_START_VALUE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get
                (unit, PC_FDR_CONTROLt,
                 PC_FDR_CONTROLt_SYMBOL_ERROR_START_VALUEf,
                 1, &def_val, &num));
        fdr_ctrl->symbol_error_start_value = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(fdr_ctrl->fbmp,
                            BCMPC_FDR_CTRL_LT_FLD_FDR_INTR_ENABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get
                (unit, PC_FDR_CONTROLt,
                 PC_FDR_CONTROLt_INTERRUPT_ENABLEf,
                 1, &def_val, &num));
        fdr_ctrl->interrupt_enable = (bool)def_val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Logical port FDR configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for PC_FDR_CONTROL LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_fdr_port_control_config_validate(int unit, bcmpc_lport_t key,
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

    /* Return for internal (CPU, LOOPBACK) ports. */
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
pc_fdr_port_control_stage(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t event,
                     const bcmltd_field_t *key_flds,
                     const bcmltd_field_t *data_flds,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    pc_fdr_port_control_entry_t entry;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_fdr_port_control_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    /*
     * During pre-config stage the FDR_CONTROL staging function is invoked
     * for entries in the config. The hardware is not configured during this
     * stage and the configuration happens later duting the run stage
     * of the System Manager initialization.
     *
     * During the system initialization, the PC component is in
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
     * FDR_CONTROL LT staging function gets called indicating an
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
     * update the FDR_CONTROL LT entry and the system is run-stage.
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
     * On FDR_CONTROL LT operation, validate if the config can be
     * applied to the hardware due to dependencies on other LTs
     * provisioning. If this check fails, return success and
     * update the operational state of the LT entry as not valid.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_fdr_port_control_flds_parse(unit, key_flds, data_flds, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_fdr_port_control_config_validate(unit, entry.key, &config_valid,
                                             &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_fdr_ctrl_lt_default_values_init(unit, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_fdr_cfg_insert(unit, entry.key,
                                                        &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_fdr_cfg_update(unit, entry.key,
                                                    &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
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
 * Convert an integer port id \c key and a bcmpc_fdr_port_control_t \c data to
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
pc_fdr_port_control_oper_flds_fill(int unit,
                                   bcmpc_fdr_port_control_t *data,
                                   bcmltd_fields_t *flds)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    if (!data) {
        SHR_EXIT();
    }

    /* update the operational fields in the LT. */
    for (i = 0; i < flds->count; i++) {
        switch (flds->field[i]->id) {
            case PC_FDR_CONTROLt_OPERATIONAL_STATEf:
                flds->field[i]->data = data->oper_state;
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function for FDR CONTROL LT lookup.
 *
 *  The callback function will be called during normal lookup,
 * traverse or when required to read the content from hardware.
 * The purpose of this call is to allow the component to overwrite
 * read-only operational fields which are dynamic in nature.
 *
 * The lookup operations defined by \ref bcmimm_lookup_type_t can be classified
 * into two operation categories, lookup and traverse.
 *
 * For the lookup category the key fields will be found in the \c in parameter
 * while for the traverse category the key fields will be found in the \c out
 * parameter.
 *
 * In all the operations the \c out field will contain all the data fields and
 * their associated values as maintained by the IMM or default values
 * (for fields that were not yet set).
 * The component may only change the data portions of the data fields.
 * For improved performance the fields will be sorted based on
 * their field ID. Array fields will be sorted based on their index.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Is a structure which contains the key fields but may include
 * also other fields. The component should only focus on the key fields.
 * \param [in,out] out this structure contains all the fields of the
 * table, so that the component should not delete or add any field to this
 * structure. The data values of the field were set by the IMM so the component
 * may only overwrite the data section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_fdr_port_control_lookup(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context,
                      bcmimm_lookup_type_t lkup_type,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{
    int rv = SHR_E_NONE;
    pc_fdr_port_control_entry_t entry;
    const bcmltd_field_t *key_flds = NULL;
    bool port_based_cfg_enable = FALSE;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_fdr_port_control_t fdr_cfg;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    pc_fdr_port_control_entry_t_init(&entry);

    /* key fields are in the "out" parameter for traverse operations */
    key_flds = (lkup_type == BCMIMM_TRAVERSE)? *(out->field): *(in->field);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    sal_memset(&fdr_cfg, 0, sizeof(fdr_cfg));
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_fdr_port_control_flds_parse(unit, NULL, key_flds, &entry));

    pport = bcmpc_lport_to_pport(unit, entry.key);

    /*
     * Check the logical port configuration and update the operational
     * state.
     */
    if (pport == BCMPC_INVALID_PPORT) {
        fdr_cfg.oper_state = BCMPC_PORT_MAP_UNKNOWN;
    } else {
        sal_memset(&pcfg, 0, sizeof(bcmpc_port_cfg_t));
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&entry.key),
                                       P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            fdr_cfg.oper_state = BCMPC_PORT_UNKNOWN;
        }
    }

    /*
     * Retrieve hardware settings to update the operational values.
     */
    /*SHR_IF_ERR_EXIT
     *   (bcmpc_pmgr_fdr_port_control_get(unit, entry.key, pport, &fdr_cfg));
     */

    /* Update the read-only operational values in the FDR_CONTROL LT. */
    SHR_IF_ERR_EXIT
        (pc_fdr_port_control_oper_flds_fill(unit, &fdr_cfg, out));

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
 * Convert bcmltd_fields_t array \c flds to an integer port id \c key and a
 * bcmpc_fdr_port_control_t \c data.
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
pc_fdr_port_control_entry_parse(int unit, const bcmltd_fields_t *flds,
                                  size_t key_size, void *key,
                                  size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    pc_fdr_port_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_fdr_port_control_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_fdr_port_control_entry_set(unit, fid, fval, &entry));
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
 * Convert an integer port id \c key and a bcmpc_fdr_port_control_t \c data to
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
pc_fdr_port_control_entry_fill(int unit,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data,
                                 bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *port_id = key;
    bcmpc_fdr_port_control_t *config = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_CONTROLt_PORT_IDf, 0,
             *port_id, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_CONTROLt_SYMBOL_ERROR_WINDOWf, 0,
             config->window_size, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_CONTROLt_SYMBOL_ERROR_THRESHOLDf,
             0, config->symbol_error_threshold, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_CONTROLt_SYMBOL_ERROR_START_VALUEf, 0,
             config->symbol_error_start_value, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_CONTROLt_INTERRUPT_ENABLEf, 0,
             config->interrupt_enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_CONTROLt_ENABLEf, 0,
             config->enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_CONTROLt_OPERATIONAL_STATEf, 0,
             config->oper_state, fcnt);
    }

    flds->count = fcnt;

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_fdr_port_control_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_fdr_port_control_entry_parse;
    db_hdl.entry_fill = pc_fdr_port_control_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_fdr_port_control_stage;
    event_hdl.lookup = pc_fdr_port_control_lookup;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}
