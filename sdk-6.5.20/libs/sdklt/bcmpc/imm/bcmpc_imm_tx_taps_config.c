/*! \file bcmpc_imm_tx_taps_config.c
 *
 * PC_TX_TAPSt IMM handler.
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

#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>
#include <bcmpc/bcmpc_util_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_TX_TAPSt

/*!
 * \brief Update the \c _fld value in \c _data with the given \c _sign.
 *
 * \param [in] _sign Pointer to a pc_tx_taps_sign_flds_t.
 * \param [in,out] _data Pointer to a bcmpc_pm_tx_taps_config_t.
 * \param [fld] _fld Field name.
 */
#define PC_SIGNED_VALUE_UPDATE(_sign, _data, _fld)      \
    do {                                                \
        if (_sign->_fld) {                              \
            _data->_fld = _data->_fld * -1;             \
        }                                               \
    } while (0);

/*! The data structure for PC_TX_TAPSt entry. */
typedef struct pc_tx_taps_config_entry_s {
    bcmpc_port_lane_t key;
    bcmpc_tx_taps_config_t data;
} pc_tx_taps_config_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_tx_taps_config_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_tx_taps_config_entry_t_init(pc_tx_taps_config_entry_t *entry)
{
    entry->key.lport = BCMPC_INVALID_LPORT;
    entry->key.lane_index = -1;
    bcmpc_tx_taps_config_t_init(&entry->data);
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
pc_tx_taps_config_entry_set(int unit, uint32_t fid, uint64_t fval,
                             pc_tx_taps_config_entry_t *entry)
{
    bcmpc_port_lane_t *port_lane = &entry->key;
    bcmpc_tx_taps_config_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_TX_TAPSt_PORT_IDf:
            port_lane->lport = fval;
            break;
        case PC_TX_TAPSt_LANE_INDEXf:
            port_lane->lane_index = fval;
            break;
        case PC_TX_TAPSt_TX_PRE_AUTOf:
            config->pre_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_PRE_AUTO);
            break;
        case PC_TX_TAPSt_TX_PREf:
            config->pre = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_PRE);
            break;
        case PC_TX_TAPSt_TX_PRE_SIGNf:
            config->pre_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_PRE_SIGN);
            break;
        case PC_TX_TAPSt_TX_PRE2_AUTOf:
            config->pre2_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_PRE2_AUTO);
            break;
        case PC_TX_TAPSt_TX_PRE2f:
            config->pre2 = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_PRE2);
            break;
        case PC_TX_TAPSt_TX_PRE2_SIGNf:
            config->pre2_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_PRE2_SIGN);
            break;
        case PC_TX_TAPSt_TX_MAIN_AUTOf:
            config->main_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_MAIN_AUTO);
            break;
        case PC_TX_TAPSt_TX_MAINf:
            config->main = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_MAIN);
            break;
        case PC_TX_TAPSt_TX_MAIN_SIGNf:
            config->main_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_MAIN_SIGN);
            break;
        case PC_TX_TAPSt_TX_POST_AUTOf:
            config->post_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST_AUTO);
            break;
        case PC_TX_TAPSt_TX_POSTf:
            config->post = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST);
            break;
        case PC_TX_TAPSt_TX_POST_SIGNf:
            config->post_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST_SIGN);
            break;
        case PC_TX_TAPSt_TX_POST2_AUTOf:
            config->post2_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST2_AUTO);
            break;
        case PC_TX_TAPSt_TX_POST2f:
            config->post2 = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST2);
            break;
        case PC_TX_TAPSt_TX_POST2_SIGNf:
            config->post2_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST2_SIGN);
            break;
        case PC_TX_TAPSt_TX_POST3_AUTOf:
            config->post3_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST3_AUTO);
            break;
        case PC_TX_TAPSt_TX_POST3f:
            config->post3 = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST3);
            break;
        case PC_TX_TAPSt_TX_POST3_SIGNf:
            config->post3_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_POST3_SIGN);
            break;
        case PC_TX_TAPSt_TX_RPARA_AUTOf:
            config->rpara_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_RPARA_AUTO);
            break;
        case PC_TX_TAPSt_TX_RPARAf:
            config->rpara = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_RPARA);
            break;
        case PC_TX_TAPSt_TX_RPARA_SIGNf:
            config->rpara_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_RPARA_SIGN);
            break;
        case PC_TX_TAPSt_TX_AMP_AUTOf:
            config->amp_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_AMP_AUTO);
            break;
        case PC_TX_TAPSt_TX_AMPf:
            config->amp = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_AMP);
            break;
        case PC_TX_TAPSt_TX_AMP_SIGNf:
            config->amp_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_AMP_SIGN);
            break;
        case PC_TX_TAPSt_TX_DRV_MODE_AUTOf:
            config->drv_mode_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE_AUTO);
            break;
        case PC_TX_TAPSt_TX_DRV_MODEf:
            config->drv_mode = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE);
            break;
        case PC_TX_TAPSt_TX_DRV_MODE_SIGNf:
            config->drv_mode_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE_SIGN);
            break;
        case PC_TX_TAPSt_TXFIR_TAP_MODE_AUTOf:
            config->txfir_tap_mode_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TXFIR_TAP_MODE_AUTO);
            break;
        case PC_TX_TAPSt_TXFIR_TAP_MODEf:
            config->txfir_tap_mode = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TXFIR_TAP_MODE);
            break;
        case PC_TX_TAPSt_TX_SIG_MODE_AUTOf:
            config->sig_mode_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_SIG_MODE_AUTO);
            break;
        case PC_TX_TAPSt_TX_SIG_MODEf:
            config->sig_mode = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_TX_TAPS_LT_FLD_TX_SIG_MODE);
            break;
        case PC_TX_TAPSt_OPERATIONAL_STATEf:
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
 * Parse the input LT fields and save the data to pc_tx_taps_config_entry_t.
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
pc_lt_tx_taps_config_flds_parse(int unit,
                                 const bcmltd_field_t *key_flds,
                                 const bcmltd_field_t *data_flds,
                                 pc_tx_taps_config_entry_t *entry)
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
            (pc_tx_taps_config_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_tx_taps_config_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TX TAPS configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for TX_TAPS LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_tx_taps_config_validate(int unit, bcmpc_port_lane_t key,
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
     rv = bcmpc_port_phys_map_get(unit, key.lport, &pport);

    if ((rv == SHR_E_NOT_FOUND) || (pport == BCMPC_INVALID_PPORT)) {
        cfg_valid = FALSE;
        SHR_EXIT();
    }

    /* Return error for internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == true) {
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
     * if lane_index > number of lanes
     * set cfg_hw_apply = false, cfg_valid = false and return success
     */
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&key.lport),
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

    if (key.lane_index >= pcfg.num_lanes) {
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
pc_tx_taps_config_stage(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            bcmimm_entry_event_t event,
                            const bcmltd_field_t *key_flds,
                            const bcmltd_field_t *data_flds,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    pc_tx_taps_config_entry_t entry, committed;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;


    SHR_FUNC_ENTER(unit);

    pc_tx_taps_config_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_tx_taps_config_flds_parse(unit, key_flds, NULL,
                                         &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_tx_taps_config_validate(unit, entry.key, &config_valid,
                                    &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_tx_taps_config_flds_parse(unit, NULL, data_flds,
                                                  &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmpc_internal_tx_taps_config_insert(unit, &entry.key,
                                                       &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_lt_entry_get(unit, IMM_SID,
                                    P(&entry.key), P(&entry.data)));
            sal_memcpy(&committed, &entry, sizeof(committed));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_tx_taps_config_flds_parse(unit, NULL, data_flds,
                                                 &entry));
            if (sal_memcmp(&entry, &committed, sizeof(committed)) == 0) {
                /* Do nothing if the config is not changed. */
                SHR_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_tx_taps_config_update(unit, &entry.key,
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


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmpc_lane_port_t \c key and a
 * bcmpc_tx_taps_config_t \c data.
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
pc_imm_tx_taps_config_entry_parse(int unit, const bcmltd_fields_t *flds,
                                  size_t key_size, void *key,
                                  size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_port_lane_t *port_lane = key;
    bcmpc_tx_taps_config_t *taps_cfg = data;
    pc_tx_taps_config_entry_t entry;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*port_lane)) ||
        (data && data_size != sizeof(*taps_cfg))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pc_tx_taps_config_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_tx_taps_config_entry_set(unit, fid, fval, &entry));
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
 * Convert an integer profile id \c key and a bcmpc_tx_taps_config_t \c data to
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
pc_imm_tx_taps_config_entry_fill(int unit,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data,
                                 bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_port_lane_t *port_lane = key;
    bcmpc_tx_taps_config_t *tx_taps_cfg = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*port_lane)) ||
        (data && data_size != sizeof(*tx_taps_cfg))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_PORT_IDf, 0, port_lane->lport, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_LANE_INDEXf, 0, port_lane->lane_index,
             fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_PREf, 0, tx_taps_cfg->pre, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_PRE2f, 0, tx_taps_cfg->pre2, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_MAINf, 0, tx_taps_cfg->main, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POSTf, 0, tx_taps_cfg->post, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST2f, 0, tx_taps_cfg->post2, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST3f, 0, tx_taps_cfg->post3, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_RPARAf, 0, tx_taps_cfg->rpara, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_AMPf, 0, tx_taps_cfg->amp, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_DRV_MODEf, 0, tx_taps_cfg->drv_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_PRE_SIGNf, 0,
             tx_taps_cfg->pre_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_PRE2_SIGNf, 0,
             tx_taps_cfg->pre2_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_MAIN_SIGNf, 0,
             tx_taps_cfg->main_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST_SIGNf, 0,
             tx_taps_cfg->post_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST2_SIGNf, 0,
             tx_taps_cfg->post2_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST3_SIGNf, 0,
              tx_taps_cfg->post3_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_RPARA_SIGNf, 0,
              tx_taps_cfg->rpara_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_AMP_SIGNf, 0,
              tx_taps_cfg->amp_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_DRV_MODE_SIGNf, 0,
             tx_taps_cfg->drv_mode_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TXFIR_TAP_MODEf, 0,
             tx_taps_cfg->txfir_tap_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_SIG_MODEf, 0,
             tx_taps_cfg->sig_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_PRE_AUTOf, 0,
             tx_taps_cfg->pre_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_PRE2_AUTOf, 0,
             tx_taps_cfg->pre2_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_MAIN_AUTOf, 0,
             tx_taps_cfg->main_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST_AUTOf, 0,
             tx_taps_cfg->post_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST2_AUTOf, 0,
             tx_taps_cfg->post2_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_POST3_AUTOf, 0,
             tx_taps_cfg->post3_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_RPARA_AUTOf, 0,
             tx_taps_cfg->rpara_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_AMP_AUTOf, 0,
             tx_taps_cfg->amp_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_DRV_MODE_AUTOf, 0,
             tx_taps_cfg->drv_mode_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TXFIR_TAP_MODE_AUTOf, 0,
             tx_taps_cfg->txfir_tap_mode_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_TX_SIG_MODE_AUTOf, 0,
             tx_taps_cfg->sig_mode_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_TX_TAPSt_OPERATIONAL_STATEf, 0,
             tx_taps_cfg->oper_state, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function for PC_TX_TAPS LT lookup.
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
pc_tx_taps_lookup_handler(int unit, bcmltd_sid_t sid,
                          uint32_t trans_id, void *context,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    int rv = SHR_E_NONE;
    uint32_t i = 0;
    bcmpc_drv_t *pc_drv = NULL;
    bool is_internal = FALSE;
    pc_tx_taps_config_entry_t entry;
    bcmpc_port_cfg_t pcfg;
    const bcmltd_field_t *key_flds = NULL;
    bool port_based_cfg_enable = FALSE;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    SHR_FUNC_ENTER(unit);

    /* key fields are in the "out" parameter for traverse operations */
    key_flds = (lkup_type == BCMIMM_TRAVERSE)? *(out->field): *(in->field);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_tx_taps_config_flds_parse(unit, NULL, key_flds, &entry));

    pport = bcmpc_lport_to_pport(unit, entry.key.lport);

    /* If logical to physical port map unknown, set operational state. */
    if (pport == BCMPC_INVALID_PPORT) {
        entry.data.oper_state = BCMPC_PORT_MAP_UNKNOWN;
    } else {
        /* Return error for internal (CPU, LOOPBACK) ports. */
        SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

        SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

        if (is_internal == true) {
            entry.data.oper_state = BCMPC_PORT_UNKNOWN;
        }

        sal_memset(&pcfg, 0, sizeof(bcmpc_port_cfg_t));
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&entry.key.lport),
                                       P(&pcfg));

        if (SHR_FAILURE(rv) || (pcfg.num_lanes == 0) ||
            (entry.key.lane_index >= pcfg.num_lanes)) {
            entry.data.oper_state = BCMPC_PORT_UNKNOWN;
        }
    }

    /* Update the read-only operational values in the TX_TAPS LT. */
    for (i = 0; i < out->count; i++) {
        switch (out->field[i]->id) {
            case PC_TX_TAPSt_OPERATIONAL_STATEf:
                out->field[i]->data =  entry.data.oper_state;
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_tx_taps_config_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_tx_taps_config_entry_parse;
    db_hdl.entry_fill = pc_imm_tx_taps_config_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_tx_taps_config_stage;
    event_hdl.lookup = pc_tx_taps_lookup_handler;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config
 * stage to pre-populate all the TX_TAPS LT entries
 * with null entries.
 */
int
bcmpc_tx_taps_prepopulate_all(int unit)
{
    uint64_t lport_min = 0, lport_max = 0, lport = 0;
    uint64_t ln_idx_min = 0, ln_idx_max = 0, ln_idx = 0;
    bcmpc_port_lane_t port_lane;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, PC_TX_TAPSt, PC_TX_TAPSt_PORT_IDf,
                              1, &lport_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, PC_TX_TAPSt, PC_TX_TAPSt_PORT_IDf,
                              1, &lport_max, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, PC_TX_TAPSt, PC_TX_TAPSt_LANE_INDEXf,
                              1, &ln_idx_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, PC_TX_TAPSt, PC_TX_TAPSt_LANE_INDEXf,
                              1, &ln_idx_max, &num));

    for (lport = lport_min; lport <= lport_max; lport++) {
         port_lane.lport = (uint32_t) lport;

         for (ln_idx = ln_idx_min; ln_idx <= ln_idx_max; ln_idx++) {
              port_lane.lane_index = (uint32_t) ln_idx;
              SHR_IF_ERR_EXIT
                 (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                            sizeof(bcmpc_port_lane_t),
                                            &port_lane, 0, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}
