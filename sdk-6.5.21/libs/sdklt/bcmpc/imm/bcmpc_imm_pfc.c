/*! \file bcmpc_imm_pfc.c
 *
 * PC_PFCt IMM handler.
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
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PFCt

/*! The data structure for PC_PFCt entry. */
typedef struct pc_pfc_entry_s {
    bcmpc_lport_t key;
    bcmpc_pfc_control_t data;
} pc_pfc_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_pfc_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_pfc_entry_t_init(pc_pfc_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    bcmpc_pfc_ctrl_t_init(&entry->data);
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
pc_pfc_entry_set(int unit, uint32_t fid, uint32_t fidx, uint64_t fval,
                  pc_pfc_entry_t *entry)
{
    bcmpc_pfc_control_t *pcfg = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PFCt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_PFCt_ENABLE_TXf:
            pcfg->tx_enable = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_ENABLE_TX);
            break;
        case PC_PFCt_ENABLE_RXf:
            pcfg->rx_enable = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_ENABLE_RX);
            break;
        case PC_PFCt_ENABLE_STATSf:
            pcfg->stats_enable = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_ENABLE_STATS);
            break;
        case PC_PFCt_REFRESH_TIMERf:
            pcfg->refresh_timer = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_REFRESH_TIMER);
            break;
        case PC_PFCt_ETH_TYPEf:
            pcfg->eth_type = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_ETH_TYPE);
            break;
        case PC_PFCt_OPCODEf:
            pcfg->opcode = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_OPCODE);
            break;
        case PC_PFCt_DEST_ADDRf:
            sal_memcpy(pcfg->da, &fval, sizeof(pcfg->da));
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_DEST_ADDR);
            break;
        case PC_PFCt_XOFF_TIMERf:
            pcfg->xoff_timer = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_XOFF_TIMER);
            break;
        case PC_PFCt_PFC_PASSf:
            pcfg->pfc_pass = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PFC_LT_FLD_PFC_PASS);
            break;
        case PC_PFCt_OPERATIONAL_STATEf:
            pcfg->oper_state = fval;
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
 * Parse the input LT fields and save the data to pc_pfc_entry_t.
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
pc_lt_port_fields_parse(int unit,
                        const bcmltd_field_t *key_flds,
                        const bcmltd_field_t *data_flds,
                        pc_pfc_entry_t *entry)
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
            (pc_pfc_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_pfc_entry_set(unit, fid, fidx, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is used for prepopulating the table.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PC_PFC LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_pfc_lt_default_values_init(int unit, pc_pfc_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_pfc_control_t *pfc = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    pfc = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_ENABLE_TX)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_ENABLE_TXf,
                                     1, &def_val, &num));
        pfc->tx_enable = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_ENABLE_RX)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_ENABLE_RXf,
                                     1, &def_val, &num));
        pfc->rx_enable = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_ENABLE_STATS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_ENABLE_STATSf,
                                     1, &def_val, &num));
        pfc->stats_enable = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_REFRESH_TIMER)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_REFRESH_TIMERf,
                                     1, &def_val, &num));
        pfc->refresh_timer = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_ETH_TYPE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_ETH_TYPEf,
                                     1, &def_val, &num));
        pfc->eth_type = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_OPCODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_OPCODEf,
                                     1, &def_val, &num));
        pfc->opcode = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_PFC_PASS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_PFC_PASSf,
                                     1, &def_val, &num));
        pfc->pfc_pass = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_XOFF_TIMER)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_XOFF_TIMERf,
                                     1, &def_val, &num));
        pfc->xoff_timer = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pfc->fbmp, BCMPC_PFC_LT_FLD_DEST_ADDR)) {

        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PFCt, PC_PFCt_DEST_ADDRf,
                                     1, &def_val, &num));
        sal_memcpy(pfc->da, &def_val, sizeof(pfc->da));
    }

exit:
    SHR_FUNC_EXIT();
}

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
pc_pfc_config_validate(int unit, bcmpc_lport_t key,
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
 * bcmpc_pfc_control_t \c data.
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
pc_imm_pfc_entry_parse(int unit, const bcmltd_fields_t *flds,
                        size_t key_size, void *key,
                        size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_pfc_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_pfc_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_pfc_entry_set(unit, fid, fidx, fval, &entry));
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
 * Convert a bcmpc_lport_t \c key and a bcmpc_pfc_control_t \c data to
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
pc_imm_pfc_entry_fill(int unit,
                       size_t key_size, void *key,
                       size_t data_size, void *data,
                       bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_pfc_control_t *pcfg = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_ENABLE_TXf, 0, pcfg->tx_enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_ENABLE_RXf, 0, pcfg->rx_enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_ENABLE_STATSf, 0, pcfg->stats_enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_REFRESH_TIMERf, 0, pcfg->refresh_timer, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_ETH_TYPEf, 0, pcfg->eth_type, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_OPCODEf, 0, pcfg->opcode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_XOFF_TIMERf, 0, pcfg->xoff_timer, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_PFC_PASSf, 0, pcfg->pfc_pass, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PFCt_OPERATIONAL_STATEf, 0, pcfg->oper_state, fcnt);
        flds->field[fcnt]->id = PC_PFCt_DEST_ADDRf;
        flds->field[fcnt]->idx = 0;
        sal_memcpy(&(flds->field[fcnt]->data), &pcfg->da, sizeof(pcfg->da));
        fcnt++;
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
pc_pfc_stage(int unit,
              bcmltd_sid_t sid,
              uint32_t trans_id,
              bcmimm_entry_event_t event,
              const bcmltd_field_t *key_flds,
              const bcmltd_field_t *data_flds,
              void *context,
              bcmltd_fields_t *output_fields)
{
    pc_pfc_entry_t entry;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_pfc_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    /*
     * The PFC staging function is invoked for any operation on
     * PC_PFC LT. During pre-config stage of System Manager
     * initialization, the staging function gets invoked for any
     * PC_PFC LT entry in the config.yml. Logical port
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
     * PFC LT staging function gets called indicating an
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
     * update the PFC LT entry and the system is run-stage.
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
     * On PFC LT operation, validate if the config can be
     * applied to the hardware due to dependencies on other LTs
     * provisioning. If this check fails, return success and
     * update the operational state of the PFC LT entry
     * as not valid.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_fields_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pfc_config_validate(unit, entry.key, &config_valid,
                                &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_fields_parse(unit, NULL, data_flds, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_pfc_ctrl_insert(unit, entry.key, &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_fields_parse(unit, NULL, data_flds, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_pfc_ctrl_update(unit, entry.key, &entry.data));
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
 * Public Functions
 */

int
bcmpc_imm_pfc_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_pfc_entry_parse;
    db_hdl.entry_fill = pc_imm_pfc_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_pfc_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PFC LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 */
int
bcmpc_pfc_config_prepopulate(int unit, bcmpc_lport_t lport,
                             bcmpc_port_cfg_t *pcfg, bool null_entry)
{
    bcmpc_drv_t *pc_drv = NULL;
    pc_pfc_entry_t entry;
    bcmpc_pmgr_pfc_status_t pfc_st;
    bool is_internal = false;
    int rv = SHR_E_NONE;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    pc_pfc_entry_t_init(&entry);
    sal_memset(&pfc_st, 0, sizeof(pfc_st));

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

        SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pcfg->pport,
                                                 &is_internal));

        if (is_internal == true) {
            SHR_EXIT();
        }
    }

    /* Get the default PFC settings for the logical port. */
    fid = PC_PFCt_PORT_IDf;
    fval = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pfc_entry_set(unit, fid, 0, fval, &entry));

    /*
     * Check if PC_PFC LT for this logical port
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
            /* Get PFC hardware configuration and update the DB. */
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_pfc_status_get(unit, lport, pcfg->pport,
                                           &entry.data));
        }

        sal_memcpy(&pfc_st, &entry.data, sizeof(bcmpc_pmgr_pfc_status_t));

        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                       sizeof(bcmpc_lport_t), &lport,
                                       sizeof(bcmpc_pmgr_pfc_status_t),
                                       &pfc_st));
    } else {
        /* Check the operational state of the entry and mark it as valid. */
        if (entry.data.oper_state != BCMPC_VALID) {
            entry.data.oper_state = BCMPC_VALID;

            SHR_IF_ERR_EXIT
                (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                           sizeof(bcmpc_lport_t), &lport,
                                           sizeof(bcmpc_pfc_control_t),
                                           &entry.data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * This function should be invoked when a new logical port is
 * deleted from the system to delete the associated PC_PFC LT
 * entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pfc_config_delete(int unit, bcmpc_lport_t lport)
{
    pc_pfc_entry_t entry;
    int rv = SHR_E_NONE;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    pc_pfc_entry_t_init(&entry);

    /* Get the default PFC settings for the logical port. */
    fid = PC_PFCt_PORT_IDf;
    fval = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pfc_entry_set(unit, fid, 0, fval, &entry));

    /*
     * Check if PC_PFC LT for this logical port
     * is already present in the DB.
     */

    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   P(&entry.key), P(&entry.data));

    if (rv != SHR_E_NOT_FOUND) {
        entry.data.oper_state = BCMPC_PORT_UNKNOWN;

        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                       sizeof(bcmpc_lport_t), &lport,
                                       sizeof(bcmpc_pfc_control_t),
                                       &entry.data));
    }


exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config stage
 * to pre-populate all the PC_PFC LT entries
 * with null entries.
 */
int
bcmpc_pfc_prepopulate_all(int unit)
{
    uint64_t lport_min = 0, lport_max = 0, lport = 0;
    pc_pfc_entry_t pfc_cfg;
    uint32_t num;


    SHR_FUNC_ENTER(unit);

    pc_pfc_entry_t_init(&pfc_cfg);

    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, IMM_SID, PC_PFCt_PORT_IDf,
                              1, &lport_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, IMM_SID, PC_PFCt_PORT_IDf,
                              1, &lport_max, &num));

    /* Get the software defaults from the map table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pfc_lt_default_values_init(unit, &pfc_cfg));

    pfc_cfg.data.oper_state = BCMPC_PORT_UNKNOWN;

    for (lport = lport_min; lport <= lport_max; lport++) {
         pfc_cfg.key = lport;
         SHR_IF_ERR_EXIT
             (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                        sizeof(bcmpc_lport_t), &(pfc_cfg.key),
                                        sizeof(bcmpc_pfc_control_t),
                                        &(pfc_cfg.data)));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PFC LT
 * entry OPERATIONAL_STATE.
 */
int
bcmpc_pfc_oper_state_update(int unit, bcmpc_lport_t lport,
                            bcmpc_pport_t pport,
                            bcmpc_entry_state_t oper_state)
{
    bcmpc_drv_t *pc_drv = NULL;
    pc_pfc_entry_t entry;
    bool is_internal = false;

    SHR_FUNC_ENTER(unit);

    pc_pfc_entry_t_init(&entry);

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

    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == true) {
        SHR_EXIT();
    }

    /* Update the operational state of the entry. */

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pfc_control_t),
                                   &entry.data));

    entry.data.oper_state = oper_state;

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pfc_control_t),
                                   &entry.data));

exit:
    SHR_FUNC_EXIT();
}
