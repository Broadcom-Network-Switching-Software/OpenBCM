/*! \file bcmpc_imm_port.c
 *
 * PC_PORTt IMM handler.
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
#include <bcmpc/bcmpc_lt_cfg.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORTt

/*! The data structure for PC_PORTt entry. */
typedef struct pc_port_entry_s {
    bcmpc_lport_t key;
    bcmpc_port_cfg_t data;
} pc_port_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_port_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_port_entry_t_init(pc_port_entry_t *entry)
{
    sal_memset(entry, 0, sizeof(*entry));
    entry->key = BCMPC_INVALID_LPORT;
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
pc_port_entry_set(int unit, uint32_t fid, uint32_t fidx, uint64_t fval,
                  pc_port_entry_t *entry)
{
    bcmpc_port_cfg_t *pcfg = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PORTt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_PORTt_ENABLEf:
            pcfg->enable = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_ENABLE);
            break;
        case PC_PORTt_SPEEDf:
            pcfg->speed = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SPEED);
            break;
        case PC_PORTt_NUM_LANESf:
            pcfg->num_lanes = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_NUM_LANES);
            break;
        case PC_PORTt_FEC_MODEf:
            pcfg->fec_mode = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_FEC_MODE);
            break;
        case PC_PORTt_ENCAPf:
            pcfg->encap_mode = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_ENCAP);
            break;
        case PC_PORTt_LINK_TRAININGf:
            pcfg->link_training = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_LINK_TRAINING);
            break;
        case PC_PORTt_LOOPBACK_MODEf:
            pcfg->lpbk_mode = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE);
            break;
        case PC_PORTt_MAX_FRAME_SIZEf:
            pcfg->max_frame_size = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_MAX_FRAME_SIZE);
            break;
        case PC_PORTt_LAG_FAILOVERf:
            pcfg->lag_failover = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_LAG_FAILOVER);
            break;
        case PC_PORTt_AUTONEGf:
            pcfg->autoneg = fval;
            break;
        case PC_PORTt_NUM_VALID_AN_PROFILESf:
            if (fval > NUM_AUTONEG_PROFILES_MAX) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            pcfg->num_an_profs = fval;
             BCMPC_LT_FIELD_SET(pcfg->fbmp,
                                BCMPC_PORT_LT_FLD_NUM_VALID_AN_PROFILES);
            break;
        case PC_PORTt_PC_AUTONEG_PROFILE_IDf:
            if (fidx >= NUM_AUTONEG_PROFILES_MAX) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            pcfg->autoneg_profile_ids[fidx] = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp,
                               BCMPC_PORT_LT_FLD_AUTONEG_PROFILE_ID);
            break;
        case PC_PORTt_RLMf:
            pcfg->rlm_mode = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_RLM);
            break;
        case PC_PORTt_INITIATORf:
            pcfg->initiator = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_INITIATOR);
            break;
        case PC_PORTt_ACTIVE_LANE_MASKf:
            pcfg->active_lane_mask = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_ACTIVE_LANE_MASK);
            break;
        case PC_PORTt_SPEED_VCO_FREQf:
            pcfg->speed_vco_freq = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SPEED_VCO_FREQ);
            break;
        case PC_PORTt_SUSPENDf:
            pcfg->suspend = fval;
            BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SUSPEND);
            break;
        case PC_PORTt_OPERATIONAL_STATEf:
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
pc_lt_port_fields_parse(int unit,
                        const bcmltd_field_t *key_flds,
                        const bcmltd_field_t *data_flds,
                        pc_port_entry_t *entry)
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
            (pc_port_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_entry_set(unit, fid, fidx, fval, entry));

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
 * \param [in] lt_entry PC_PORT LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_lt_default_values_init(int unit, pc_port_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    uint32_t i;
    bcmpc_port_cfg_t *pcfg = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    pcfg = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_ENABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_ENABLEf,
                                     1, &def_val, &num));
        pcfg->enable = (int)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SPEED)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_SPEEDf,
                                     1, &def_val, &num));
        pcfg->speed = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_NUM_LANES)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_NUM_LANESf,
                                     1, &def_val, &num));
        pcfg->num_lanes = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_FEC_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_FEC_MODEf,
                                     1, &def_val, &num));
        pcfg->fec_mode = (uint32_t)def_val;
        BCMPC_LT_FIELD_SET(pcfg->fbmp, BCMPC_PORT_LT_FLD_FEC_MODE);
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_ENCAP)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_ENCAPf,
                                     1, &def_val, &num));
        pcfg->encap_mode = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_LINK_TRAINING)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_LINK_TRAININGf,
                                     1, &def_val, &num));
        pcfg->link_training = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_LOOPBACK_MODEf,
                                     1, &def_val, &num));
        pcfg->lpbk_mode = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_MAX_FRAME_SIZE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_MAX_FRAME_SIZEf,
                                     1, &def_val, &num));
        pcfg->max_frame_size = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_LAG_FAILOVER)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_LAG_FAILOVERf,
                                     1, &def_val, &num));
        pcfg->lag_failover = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp,
                            BCMPC_PORT_LT_FLD_NUM_VALID_AN_PROFILES)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_PORT_IDf,
                                     1, &def_val, &num));
        pcfg->num_an_profs = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_AUTONEG_PROFILE_ID)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt,
                                     PC_PORTt_PC_AUTONEG_PROFILE_IDf,
                                     1, &def_val, &num));
        for (i = 0; i < NUM_AUTONEG_PROFILES_MAX; i++) {
            pcfg->autoneg_profile_ids[i] = def_val;
        }
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_RLM)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_RLMf,
                                     1, &def_val, &num));
        pcfg->rlm_mode = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_INITIATOR)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_INITIATORf,
                                     1, &def_val, &num));
        pcfg->initiator = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_ACTIVE_LANE_MASK)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_ACTIVE_LANE_MASKf,
                                     1, &def_val, &num));
        pcfg->active_lane_mask = (uint16_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SPEED_VCO_FREQ)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_SPEED_VCO_FREQf,
                                     1, &def_val, &num));
        pcfg->speed_vco_freq = def_val;
    }

    if (!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SUSPEND)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PORTt, PC_PORTt_SUSPENDf,
                                     1, &def_val, &num));
        pcfg->suspend = def_val;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pc_update_auto_populate_lts(int unit, int lport, bcmpc_port_cfg_t *pcfg)
{
    SHR_FUNC_ENTER(unit);

    /* Update the PC_PMD_FIRMWARE LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmd_firmware_config_prepopulate(unit, lport, pcfg, FALSE));

    /* Update the PC_MAC_CONTROL LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_mac_control_config_prepopulate(unit, lport, pcfg, FALSE));

    /* Update the PC_PHY_CONTROL LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_phy_control_config_prepopulate(unit, lport, pcfg, FALSE));

    /* Update the PC_PFC LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pfc_config_prepopulate(unit, lport, pcfg, FALSE));

    /* Update the PC_PORT_TIMESYNC LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_timesync_config_prepopulate(unit, lport, pcfg, FALSE));

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
 * bcmpc_port_cfg_t \c data.
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
pc_imm_port_entry_parse(int unit, const bcmltd_fields_t *flds,
                        size_t key_size, void *key,
                        size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_port_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_port_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_entry_set(unit, fid, fidx, fval, &entry));
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
pc_imm_port_entry_fill(int unit,
                       size_t key_size, void *key,
                       size_t data_size, void *data,
                       bcmltd_fields_t *flds)
{
    size_t idx, fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_port_cfg_t *pcfg = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_ENABLEf, 0, pcfg->enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_SPEEDf, 0, pcfg->speed, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_NUM_LANESf, 0, pcfg->num_lanes, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_LINK_TRAININGf, 0, pcfg->link_training, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_ENCAPf, 0, pcfg->encap_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_LOOPBACK_MODEf, 0, pcfg->lpbk_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_MAX_FRAME_SIZEf, 0, pcfg->max_frame_size, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_FEC_MODEf, 0, pcfg->fec_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_AUTONEGf, 0, pcfg->autoneg, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_NUM_VALID_AN_PROFILESf, 0, pcfg->num_an_profs,
            fcnt);
        for (idx = 0; idx < NUM_AUTONEG_PROFILES_MAX; idx++) {
            BCMPC_LT_FIELD_VAL_ADD
                (flds, PC_PORTt_PC_AUTONEG_PROFILE_IDf, idx,
                 pcfg->autoneg_profile_ids[idx], fcnt);
        }
        BCMPC_LT_FIELD_VAL_ADD (flds, PC_PORTt_RLMf, 0, pcfg->rlm_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD (flds, PC_PORTt_INITIATORf, 0,
                                pcfg->initiator, fcnt);
        BCMPC_LT_FIELD_VAL_ADD (flds, PC_PORTt_ACTIVE_LANE_MASKf, 0,
                                pcfg->active_lane_mask, fcnt);
        BCMPC_LT_FIELD_VAL_ADD (flds, PC_PORTt_RLMf, 0, pcfg->rlm_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_LAG_FAILOVERf, 0, pcfg->lag_failover, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_SPEED_VCO_FREQf, 0, pcfg->speed_vco_freq, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_SUSPENDf, 0, pcfg->suspend, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORTt_OPERATIONAL_STATEf, 0, pcfg->oper_state, fcnt);
    }

    flds->count = fcnt;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Logical port configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for PC_PORT LT.
 * \param [in] event IMM operation type.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 * \param [out] reason Indicates the reason for failure to config port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_port_lt_config_validate(int unit, bcmpc_lport_t key,
                           bcmimm_entry_event_t event,
                           bcmpc_port_cfg_t *pcfg,
                           bool *cfg_valid, bool *cfg_hw_apply,
                           bcmpc_entry_state_t *reason)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bool is_internal = FALSE;
    int rv = SHR_E_NONE;
    bcmpc_port_cfg_t old_cfg;

    SHR_FUNC_ENTER(unit);

    *cfg_valid = FALSE;
    *cfg_hw_apply = FALSE;

    /*
     * Check if logical to physical port map is available.
     * Do not populate the special (CPU, internal loopback)
     * ports.
     * Return success even if the logical to physical port map
     * is not available or pport is invalid, because the logcial to
     * physical port configuratin can be done later, so the
     * configuration should not be blocked.
     * set cfg_hw_apply = false and return success
     * Indicate the operational state of the port in pc_port_status.
     */
    rv = bcmpc_port_phys_map_get(unit, key, &pport);

    if ((rv == SHR_E_NOT_FOUND) || (pport == BCMPC_INVALID_PPORT)) {
        *cfg_valid = FALSE;
        *cfg_hw_apply = FALSE;
        *reason = BCMPC_PORT_MAP_UNKNOWN;
        SHR_EXIT();
    }

    /*
     * General note:
     * For a logical port configuration, the PC_PM_CORE configuration
     * is mandatory, but there is no validation logic here, since
     * PC_PM_CORE LT is update-only LT with default values if not
     * provisioned by the user, meaning the PC_PM_CORE entry is
     * always present.
     */

    /* Return error for internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    /*
     * For internal ports return success, the Port Macro is
     * never programmed since there is no Port Macro associated
     * with the internal ports. This ports are only setup by
     * the TM component, PC only notifies the internal port
     * LT operations.
     */
    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == TRUE) {
        *cfg_valid = TRUE;
        *cfg_hw_apply = TRUE;
        *reason = BCMPC_PORT_UNKNOWN;
        SHR_EXIT();
    }

    /*
     * The logic beow handles the logical port parital configuration
     * and updates the reason code accordingly, which is used to set
     * the PC_PORT OPERATION_STATE field.
     */
    if (event == BCMIMM_ENTRY_INSERT) {
        if ((pcfg->speed == 0) || (pcfg->num_lanes == 0)) {
            *cfg_valid = FALSE;
            *cfg_hw_apply = FALSE;
            *reason = (pcfg->speed == 0)? BCMPC_PORT_SPEED_UNKNOWN:
                                         BCMPC_PORT_NUM_LANES_UNKNOWN;
            SHR_EXIT();
        }
    } else if (event == BCMIMM_ENTRY_UPDATE) {
        /*
         * Check if PC_PORT LT has entry for this logical port.
         * If not present,
         * set cfg_hw_apply = false, cfg_valid = true and return success
         * If present,
         * number of lanes is 0, means logcial port is not configured
         * set cfg_hw_apply = false, cfg_valid = true and return success
         */
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&key),
                                       P(&old_cfg));

        if ((!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_SPEED)) &&
            (old_cfg.speed == 0)) {
            *cfg_valid = FALSE;
            *cfg_hw_apply = FALSE;
            *reason = BCMPC_PORT_SPEED_UNKNOWN;
            SHR_EXIT();
        }

        if ((!BCMPC_LT_FIELD_GET(pcfg->fbmp, BCMPC_PORT_LT_FLD_NUM_LANES)) &&
            (old_cfg.num_lanes == 0)) {
            *cfg_valid = FALSE;
            *cfg_hw_apply = FALSE;
            *reason = BCMPC_PORT_NUM_LANES_UNKNOWN;
            SHR_EXIT();
        }
    }

    /*
     * indicates that the PC_PORT LTs required to configure a logical port
     * are provisioned. The actual validation of the configuration is done
     * later during hardware configuration.
     */
    *cfg_valid = TRUE;
    *cfg_hw_apply = TRUE;
    *reason = BCMPC_VALID;

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
pc_port_stage(int unit,
              bcmltd_sid_t sid,
              uint32_t trans_id,
              bcmimm_entry_event_t event,
              const bcmltd_field_t *key_flds,
              const bcmltd_field_t *data_flds,
              void *context,
              bcmltd_fields_t *output_fields)
{
    pc_port_entry_t entry, committed;
    bcmpc_drv_t *pc_drv = NULL;
    bool is_internal = FALSE;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;
    bcmpc_entry_state_t reason = BCMPC_VALID;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    pc_port_entry_t_init(&entry);
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_fields_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_fields_parse(unit, NULL, data_flds, &entry));

    /*
     * The following block of code validates the PC_PORT LT configuration.
     * This code is executed for INSERT and UPDATE LT operations.
     * Need to determine if the PC_PORT configuration needs to be applied
     * to the hardware or not. If the primary configuration properties of
     * a logical port or not provisioned, then need to skip calling the below
     * driver functions to allocate resources and update the PC_PORT IMM DB
     * entry OPERATIONAL_STATE with the right reason to indicate what is
     * missing.
     */
    if (event != BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_lt_config_validate(unit, entry.key, event, &entry.data,
                                        &config_valid, &config_hw_apply,
                                        &reason));

        if (config_valid != TRUE || config_hw_apply != TRUE) {
            output_fields->field[0]->id = PC_PORTt_OPERATIONAL_STATEf;
            output_fields->field[0]->data = reason;
            output_fields->field[0]->idx = 0;
            output_fields->field[0]->flags = 0;
            output_fields->count = 1;
            SHR_EXIT();
        }
    }

    /* Get device driver. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_fields_parse(unit, NULL, data_flds, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_default_values_init(unit, &entry));
            entry.data.is_new = TRUE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_cfg_insert(unit, entry.key, &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            /* If updating internal port, just return. */
            entry.data.pport = bcmpc_lport_to_pport(unit, entry.key);
            if (entry.data.pport != BCMPC_INVALID_PPORT) {
                SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, entry.data.pport,
                                &is_internal));
                if (is_internal == true) {
                    SHR_ERR_EXIT(SHR_E_DISABLED);
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_db_imm_entry_lookup(unit,
                                           IMM_SID,
                                           P(&entry.key), P(&entry.data)));
            sal_memcpy(&committed, &entry, sizeof(committed));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_fields_parse(unit, NULL, data_flds, &entry));
            if ((sal_memcmp(&entry, &committed, sizeof(committed)) == 0) &&
                !(BCMPC_LT_FIELD_GET(entry.data.fbmp, BCMPC_PORT_LT_FLD_RLM) &&
                  entry.data.rlm_mode)) {
                /*
                 * Do nothing if the config is not changed and
                 * user does not intend to re-start RLM.
                 */
                SHR_EXIT();
            }

            BCMPC_LT_FILED_BMP_CLR(entry.data.fbmp, PC_PORT_FIELD_ID_MAX);
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_fields_parse(unit, NULL, data_flds, &entry));

            /* Check if port is in suspended state. */
            if (committed.data.suspend) {
                entry.data.is_suspended = TRUE;
            }

            /* Check if its a port suspend or resume operation. */
            if (committed.data.suspend != entry.data.suspend) {
                /* If suspend is false, means a resume operation. */
                if (!entry.data.suspend) {
                    entry.data.resume = TRUE;
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_cfg_update(unit, entry.key, &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* If delete internal port, just return. */
            entry.data.pport = bcmpc_lport_to_pport(unit, entry.key);
            if (entry.data.pport != BCMPC_INVALID_PPORT) {
                SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, entry.data.pport,
                                &is_internal));
                if (is_internal == true) {
                    SHR_ERR_EXIT(SHR_E_DISABLED);
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_cfg_delete(unit, entry.key));

            /* Delete the PC_PMD_FIRMWARE LT for this logical port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pmd_firmware_config_delete(unit, entry.key));

            /* Delete the PC_MAC_CONTROL LT for this logical port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_mac_control_config_delete(unit, entry.key));

            /* Delete the PC_PHY_CONTROL LT for this logical port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_phy_control_config_delete(unit, entry.key));

            /* Delete the PC_PFC LT for this logical port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pfc_config_delete(unit, entry.key));

            /* Delete the PC_PORT_TIMESYNC LT for this logical port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_timesync_config_delete(unit, entry.key));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * If the code execution reaches this point, it means the PC_PORT LT
     * operation was successful.
     * Need to update the OPERATIONAL_STATE of the PC_PORT LT entry if
     * it is currently set to any value other than VALID. This operation
     * is only valid for INSERT and UPDATE ILT operations.
     * IF updating the PC_PORT OPERATIONAL_STATE to VALID, it should be
     * noted that all the update-only PC LTs for this logical port are also
     * currently marked as NOT VALID, and those LTs also need to be updated
     * to indicate the OPERATIONAL_STATE as VALID.
     */
    if (event == BCMIMM_ENTRY_UPDATE || event == BCMIMM_ENTRY_INSERT) {
        bcmpc_port_cfg_t tmp_pcfg;

        if (event == BCMIMM_ENTRY_UPDATE) {
            sal_memset(&tmp_pcfg, 0, sizeof(bcmpc_port_cfg_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_db_imm_entry_lookup(unit,
                                           IMM_SID,
                                           P(&entry.key), P(&tmp_pcfg)));
        }

        if ((event == BCMIMM_ENTRY_INSERT) ||
             (tmp_pcfg.oper_state != BCMPC_VALID)) {

            /* Defensive check, the output fields is not NULL. */
            if (output_fields) {
                /* Update the PC_PORT entry operational state to valid. */
                output_fields->field[0]->id = PC_PORTt_OPERATIONAL_STATEf;
                output_fields->field[0]->data = BCMPC_VALID;
                output_fields->field[0]->idx = 0;
                output_fields->field[0]->flags = 0;
                output_fields->count = 1;
            }

            /*
             * Now mark all the other pre-populated lt entries for this
             * logical port as VALID.
             */
            entry.data.pport = bcmpc_lport_to_pport(unit, entry.key);
            SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, entry.data.pport,
                            &is_internal));
            if (is_internal == true) {
                SHR_EXIT();
            }

            SHR_IF_ERR_EXIT
                (pc_update_auto_populate_lts(unit, entry.key, &entry.data));

        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_port_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_port_entry_parse;
    db_hdl.entry_fill = pc_imm_port_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_port_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_port_lt_entry_update(int unit, bcmpc_lport_t lport)
{
    pc_port_entry_t entry;
    SHR_FUNC_ENTER(unit);

    pc_port_entry_t_init(&entry);

    entry.key = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, IMM_SID, P(&entry.key),
                                   P(&entry.data)));

    /* get the logical port to physical port map. */
    entry.data.pport = bcmpc_lport_to_pport(unit, entry.key);

    if (entry.data.oper_state != BCMPC_VALID) {
        entry.data.oper_state = BCMPC_VALID;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_db_imm_entry_update(unit, IMM_SID, P(&entry.key),
                                       P(&entry.data)));

        SHR_IF_ERR_EXIT
            (pc_update_auto_populate_lts(unit, entry.key, &entry.data));

    }
exit:
    SHR_FUNC_EXIT();
}
