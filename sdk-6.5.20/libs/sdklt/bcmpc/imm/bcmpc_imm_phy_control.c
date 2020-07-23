/*! \file bcmpc_imm_phy_control.c
 *
 * PC_PHY_CONTROLt IMM handler.
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
#define IMM_SID PC_PHY_CONTROLt

/*! The data structure for PC_PHY_CONTROLt entry. */
typedef struct pc_phy_control_entry_s {
    bcmpc_port_lane_t  key;
    bcmpc_phy_control_t data;
} pc_phy_control_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_phy_control_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_phy_control_entry_t_init(pc_phy_control_entry_t *entry)
{
    entry->key.lport = BCMPC_INVALID_PROF_ID;
    entry->key.lane_index = -1;
    bcmpc_phy_control_t_init(&entry->data);
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
pc_phy_control_entry_set(int unit, uint32_t fid, uint32_t fidx,
                         uint64_t fval,
                         pc_phy_control_entry_t *entry)
{
    bcmpc_port_lane_t  *port_lane = &entry->key;
    bcmpc_phy_control_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PHY_CONTROLt_PORT_IDf:
            port_lane->lport = fval;
            break;
        case PC_PHY_CONTROLt_LANE_INDEXf:
            port_lane->lane_index = fval;
            break;
        case PC_PHY_CONTROLt_TX_SQUELCH_AUTOf:
            config->tx_squelch_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_TX_SQUELCH_AUTO);
            break;
        case PC_PHY_CONTROLt_TX_SQUELCHf:
            config->tx_squelch = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_TX_SQUELCH);
            break;
        case PC_PHY_CONTROLt_RX_SQUELCH_AUTOf:
            config->rx_squelch_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_SQUELCH_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_SQUELCHf:
            config->rx_squelch = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_SQUELCH);
            break;
        case PC_PHY_CONTROLt_RX_ADAPTATION_RESUME_AUTOf:
            config->rx_adpt_resume_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_ADAPTION_RESUME_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_ADAPTION_RESUMEf:
            config->rx_adpt_resume = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_ADAPTION_RESUME);
            break;
        case PC_PHY_CONTROLt_RX_AFE_VGA_AUTOf:
            config->rx_afe_vga_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_AFE_VGA_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_AFE_VGAf:
            config->rx_afe_vga = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_AFE_VGA);
            break;
        case PC_PHY_CONTROLt_RX_AFE_PEAKING_FILTER_AUTOf:
            config->rx_afe_pk_flt_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_RX_AFE_PEAKING_FILTER_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_AFE_PEAKING_FILTERf:
            config->rx_afe_pk_flt = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_AFE_PEAKING_FILTER);
            break;
        case PC_PHY_CONTROLt_RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTOf:
            config->rx_afe_low_fre_pk_flt_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_AFE_LOW_FREQ_PEAKING_FILTERf:
            config->rx_afe_low_fre_pk_flt = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER);
            break;
        case PC_PHY_CONTROLt_RX_AFE_HIGH_FREQ_PEAKING_FILTER_AUTOf:
            config->rx_afe_hi_fre_pk_flt_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_AFE_HIGH_FREQ_PEAKING_FILTERf:
            config->rx_afe_hi_fre_pk_flt = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER);
            break;
        case PC_PHY_CONTROLt_RX_AFE_DFE_TAP_AUTOf:
            config->rx_afe_dfe_tap_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_AFE_DFE_TAP_AUTO);
            break;
        case PC_PHY_CONTROLt_RX_AFE_DFE_TAP_SIGNf:
            if (fidx >= BCMPC_NUM_RX_DFE_TAPS_MAX) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            config->rx_afe_dfe_tap_sign[fidx] = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_AFE_DFE_TAP_SIGN);
            break;
        case PC_PHY_CONTROLt_RX_AFE_DFE_TAPf:
            if (fidx >= BCMPC_NUM_RX_DFE_TAPS_MAX) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            config->rx_afe_dfe_tap[fidx] = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_RX_AFE_DFE_TAP);
            break;
        case PC_PHY_CONTROLt_PAM4_TX_PATTERN_AUTOf:
            config->pam4_tx_pat_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_PAM4_TX_PATTERN_AUTO);
            break;
        case PC_PHY_CONTROLt_PAM4_TX_PATTERNf:
            config->pam4_tx_pat = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_PAM4_TX_PATTERN);
            break;
        case PC_PHY_CONTROLt_PAM4_TX_PRECODER_AUTOf:
            config->pam4_tx_precoder_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_PAM4_TX_PRECODER_AUTO);
            break;
        case PC_PHY_CONTROLt_PAM4_TX_PRECODERf:
            config->pam4_tx_precoder = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_PAM4_TX_PRECODER);
            break;
        case PC_PHY_CONTROLt_PHY_ECC_INTR_ENABLE_AUTOf:
            config->phy_ecc_intr_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_PHY_ECC_INTR_EN_AUTO);
            break;
        case PC_PHY_CONTROLt_PHY_ECC_INTR_ENABLEf:
            config->phy_ecc_intr = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_PHY_ECC_INTR_EN);
            break;
        case PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDE_AUTOf:
            config->tx_pi_ovride_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_TX_PI_FREQ_OVERRIDE_AUTO);
            break;
        case PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDEf:
            config->tx_pi_ovride = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_TX_PI_FREQ_OVERRIDE);
            break;
        case PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDE_SIGNf:
            config->tx_pi_ovride_sign = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_TX_PI_FREQ_OVERRIDE_SIGN);
            break;
        case PC_PHY_CONTROLt_PMD_DEBUG_LANE_EVENT_LOG_LEVELf:
            config->pmd_debug_lane_event_log_level = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_PMD_DEBUG_LANE_EVENT_LOG_LEVEL);
            break;
        case PC_PHY_CONTROLt_FEC_BYPASS_INDICATION_AUTOf:
            config->phy_fec_bypass_ind_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_FLD_FEC_BYPASS_INDICATION_AUTO);
            break;
        case PC_PHY_CONTROLt_FEC_BYPASS_INDICATIONf:
            config->phy_fec_bypass_ind = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_FLD_FEC_BYPASS_INDICATION);
            break;
        case PC_PHY_CONTROLt_OPERATIONAL_STATEf:
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
 * Parse the input LT fields and save the data to pc_phy_control_entry_t.
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
pc_lt_phy_control_flds_parse(int unit,
                                 const bcmltd_field_t *key_flds,
                                 const bcmltd_field_t *data_flds,
                                 pc_phy_control_entry_t *entry)
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
            (pc_phy_control_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_phy_control_entry_set(unit, fid, fidx, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PHY control configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for TX_TAPS LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_phy_control_config_validate(int unit, bcmpc_port_lane_t key,
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
     * phy control configuration should not be blocked.
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
        SHR_ERR_EXIT(SHR_E_PARAM);
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

/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PC_PHY_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_phy_ctrl_lt_default_values_init(int unit, pc_phy_control_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_phy_control_t *phy_ctrl = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    phy_ctrl = &(lt_entry->data);

    /* Populate the PHY_CONTROL entry with known defaults. */

    /*
     * If AUTO LT fields are not set by the user, populate with defaults
     * which is true, meaning the driver configures the PHY control
     * properties internally based on port configuration.
     */
    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp, BCMPC_FLD_TX_SQUELCH_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_TX_SQUELCH_AUTOf,
                                     1, &def_val, &num));
        phy_ctrl->tx_squelch_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp, BCMPC_FLD_RX_SQUELCH_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_RX_SQUELCH_AUTOf,
                                     1, &def_val, &num));
        phy_ctrl->rx_squelch_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_ADAPTION_RESUME_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_RX_ADAPTATION_RESUME_AUTOf,
                                     1, &def_val, &num));
        phy_ctrl->rx_adpt_resume_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_AFE_VGA_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_RX_AFE_VGA_AUTOf,
                                     1, &def_val, &num));
        phy_ctrl->rx_afe_vga_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_AFE_VGA_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_RX_AFE_VGA_AUTOf,
                                     1, &def_val, &num));
        phy_ctrl->rx_afe_vga_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_AFE_PEAKING_FILTER_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_RX_AFE_PEAKING_FILTER_AUTOf,
                                     1, &def_val, &num));
        phy_ctrl->rx_afe_pk_flt_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->rx_afe_low_fre_pk_flt_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_RX_AFE_HIGH_FREQ_PEAKING_FILTER_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->rx_afe_hi_fre_pk_flt_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_RX_AFE_DFE_TAP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_RX_AFE_DFE_TAP_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->rx_afe_dfe_tap_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_PAM4_TX_PATTERN_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_PAM4_TX_PATTERN_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->pam4_tx_pat_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_PAM4_TX_PRECODER_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_PAM4_TX_PRECODER_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->pam4_tx_precoder_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_PHY_ECC_INTR_EN_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_PHY_ECC_INTR_ENABLE_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->phy_ecc_intr_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_TX_PI_FREQ_OVERRIDE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDE_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->tx_pi_ovride_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_FEC_BYPASS_INDICATION_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                         PC_PHY_CONTROLt_FEC_BYPASS_INDICATION_AUTOf,
                         1, &def_val, &num));
        phy_ctrl->phy_fec_bypass_ind_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(phy_ctrl->fbmp,
                            BCMPC_FLD_PMD_DEBUG_LANE_EVENT_LOG_LEVEL)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PHY_CONTROLt,
                             PC_PHY_CONTROLt_PMD_DEBUG_LANE_EVENT_LOG_LEVELf,
                                     1, &def_val, &num));
        phy_ctrl->pmd_debug_lane_event_log_level = (int)def_val;
        BCMPC_LT_FIELD_SET(phy_ctrl->fbmp,
                           BCMPC_FLD_PMD_DEBUG_LANE_EVENT_LOG_LEVEL);
    }

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
pc_phy_control_stage(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t event,
                     const bcmltd_field_t *key_flds,
                     const bcmltd_field_t *data_flds,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    pc_phy_control_entry_t entry, committed;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;


    SHR_FUNC_ENTER(unit);

    pc_phy_control_entry_t_init(&entry);
    pc_phy_control_entry_t_init(&committed);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_phy_control_flds_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_phy_control_config_validate(unit, entry.key, &config_valid,
                                         &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_phy_control_flds_parse(unit, NULL, data_flds,
                                              &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_phy_control_insert(unit, &entry.key,
                                                   &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_phy_control_flds_parse(unit, NULL, data_flds,
                                              &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_phy_control_update(unit, &entry.key,
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
 * Convert bcmltd_fields_t array \c flds to bcmpc_port_lane_t \c key and a
 * bcmpc_phy_control_prof_t \c data.
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
pc_phy_control_entry_parse(int unit, const bcmltd_fields_t *flds,
                           size_t key_size, void *key,
                           size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_phy_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_phy_control_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_phy_control_entry_set(unit, fid, fidx, fval, &entry));
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
 * Convert bcmpc_port_lane_t \c key and a bcmpc_phy_control_t \c data to
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
pc_phy_control_entry_fill(int unit,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data,
                                 bcmltd_fields_t *flds)
{
    size_t idx, fcnt = 0;
    bcmpc_port_lane_t  *port_lane = key;
    bcmpc_phy_control_t *config = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*port_lane)) ||
        (data && data_size != sizeof(*config))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PORT_IDf, 0, port_lane->lport, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_LANE_INDEXf, 0, port_lane->lane_index, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_TX_SQUELCH_AUTOf, 0,
            config->tx_squelch_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_TX_SQUELCHf, 0,
            config->tx_squelch, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_SQUELCH_AUTOf, 0,
             config->rx_squelch_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_SQUELCHf, 0,
             config->rx_squelch, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_ADAPTATION_RESUME_AUTOf, 0,
            config->rx_adpt_resume_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_ADAPTION_RESUMEf, 0,
            config->rx_adpt_resume, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_VGA_AUTOf, 0,
            config->rx_afe_vga_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_VGAf, 0,
            config->rx_afe_vga, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_PEAKING_FILTERf, 0,
             config->rx_afe_pk_flt, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_PEAKING_FILTER_AUTOf, 0,
             config->rx_afe_pk_flt_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTOf, 0,
            config->rx_afe_low_fre_pk_flt_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_LOW_FREQ_PEAKING_FILTERf, 0,
            config->rx_afe_low_fre_pk_flt, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_HIGH_FREQ_PEAKING_FILTER_AUTOf, 0,
            config->rx_afe_hi_fre_pk_flt_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_HIGH_FREQ_PEAKING_FILTERf, 0,
            config->rx_afe_hi_fre_pk_flt, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_RX_AFE_DFE_TAP_AUTOf, 0,
            config->rx_afe_dfe_tap_auto, fcnt);
        for (idx = 0; idx < BCMPC_NUM_RX_DFE_TAPS_MAX; idx++) {
            BCMPC_LT_FIELD_VAL_ADD
                (flds, PC_PHY_CONTROLt_RX_AFE_DFE_TAP_SIGNf, idx,
                config->rx_afe_dfe_tap_sign[idx], fcnt);
            BCMPC_LT_FIELD_VAL_ADD
                (flds, PC_PHY_CONTROLt_RX_AFE_DFE_TAPf, idx,
                config->rx_afe_dfe_tap[idx], fcnt);
        }
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PAM4_TX_PATTERN_AUTOf, 0,
            config->pam4_tx_pat_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PAM4_TX_PATTERNf, 0,
            config->pam4_tx_pat, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PAM4_TX_PRECODER_AUTOf, 0,
            config->pam4_tx_precoder_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PAM4_TX_PRECODERf, 0,
            config->pam4_tx_precoder, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PHY_ECC_INTR_ENABLE_AUTOf, 0,
            config->phy_ecc_intr_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PHY_ECC_INTR_ENABLEf, 0,
            config->phy_ecc_intr, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDE_AUTOf, 0,
            config->tx_pi_ovride_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDEf, 0,
            config->tx_pi_ovride, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_TX_PI_FREQ_OVERRIDE_SIGNf, 0,
            config->tx_pi_ovride_sign, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_PMD_DEBUG_LANE_EVENT_LOG_LEVELf, 0,
            config->pmd_debug_lane_event_log_level, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_FEC_BYPASS_INDICATION_AUTOf, 0,
            config->phy_fec_bypass_ind_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_FEC_BYPASS_INDICATIONf, 0,
            config->phy_fec_bypass_ind, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PHY_CONTROLt_OPERATIONAL_STATEf, 0,
             config->oper_state, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_phy_control_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_phy_control_entry_parse;
    db_hdl.entry_fill = pc_phy_control_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_phy_control_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PHY_CONTROL LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 */
int
bcmpc_phy_control_config_prepopulate(int unit, bcmpc_lport_t lport,
                                     bcmpc_port_cfg_t *pcfg, bool null_entry)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_phy_control_t phy_control;
    pc_phy_control_entry_t entry;
    bcmpc_port_lane_t port_lane;
    bool is_internal = false;
    uint8_t idx = 0, idx_max = 0;
    int rv = SHR_E_NONE;
    uint64_t max_val = 0;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    /*
     * Set the lane index to 0, to get the firmware configuration
     * for the first lane and use it to populate all the lanes.
     */
    port_lane.lport = lport;
    port_lane.lane_index = 0;

    if (!null_entry) {
        /*
         * Check if logical to physical port map is available.
         * Do not populate the special (CPU, internal loopback)
         * ports.
         */
        if (pcfg->pport == BCMPC_INVALID_PPORT) {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

        SHR_IF_ERR_EXIT
            (pc_drv->is_internal_port(unit, pcfg->pport, &is_internal));
        if (is_internal == true) {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_max_get(unit, IMM_SID, PC_PHY_CONTROLt_LANE_INDEXf,
                                 1, &max_val, &num));

    max_val += 1;
    if (!null_entry) {
        idx_max = (pcfg->num_lanes < (uint8_t)max_val)? pcfg->num_lanes:
                                                       (uint8_t) max_val;
    } else {
        idx_max = max_val;
    }


    for (idx = 0; idx < idx_max; idx++) {
        port_lane.lane_index = idx;

        sal_memset(&phy_control, 0, sizeof(phy_control));
        sal_memset(&entry, 0, sizeof(entry));

        /* get the default value for the PMD lane event log level */
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_phy_ctrl_lt_default_values_init(unit, &entry));
        /*
         * On Port add, Insert if entry not present.
         */
        rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                       sizeof(bcmpc_port_lane_t), &port_lane,
                                       sizeof(bcmpc_phy_control_t),
                                       &phy_control);
        if (rv == SHR_E_NOT_FOUND) {

            /* prepopulate the pmd lane event log level with default value */
            phy_control.pmd_debug_lane_event_log_level =
                             entry.data.pmd_debug_lane_event_log_level;

            SHR_IF_ERR_EXIT
                (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                          sizeof(bcmpc_port_lane_t), &port_lane,
                                          sizeof(bcmpc_phy_control_t),
                                          &phy_control));
        } else {
            /* Check the operational state of the entry and mark it as valid. */
            if (phy_control.oper_state != BCMPC_VALID) {
                phy_control.oper_state = BCMPC_VALID;

                SHR_IF_ERR_EXIT
                    (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                        sizeof(bcmpc_port_lane_t), &port_lane,
                                        sizeof(bcmpc_phy_control_t),
                                        &phy_control));
            }
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * This function should be invoked when a new logical port is
 * deleted from the system to delete the associated PC_PHY_CONTROL LT
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
bcmpc_phy_control_config_delete(int unit, bcmpc_lport_t lport)
{
    bcmpc_phy_control_t phy_control;
    bcmpc_port_lane_t port_lane;
    uint8_t idx = 0;
    int rv = SHR_E_NONE;
    uint64_t max_val = 0;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    /*
     * Set the lane index to 0, to get the firmware configuration
     * for the first lane and use it to populate all the lanes.
     */
    port_lane.lport = lport;
    port_lane.lane_index = 0;

    sal_memset(&phy_control, 0, sizeof(phy_control));

    SHR_IF_ERR_EXIT(
        bcmlrd_field_max_get(unit, IMM_SID, PC_PHY_CONTROLt_LANE_INDEXf,
                                 1, &max_val, &num));

    for (idx = 0; idx <= max_val; idx++) {
        port_lane.lane_index = idx;

        /*
         * On Port delete, mark the entry as Port unknwon.
         */
        rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                       sizeof(bcmpc_port_lane_t), &port_lane,
                                       sizeof(bcmpc_phy_control_t),
                                       &phy_control);
        /* Mark the entry as port unknown. */
        phy_control.oper_state = BCMPC_PORT_UNKNOWN;

        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT
                (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                    sizeof(bcmpc_port_lane_t), &port_lane,
                                    sizeof(bcmpc_phy_control_t),
                                        &phy_control));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PHY_CONTROL LT entries
 * with null entries.
 */
int
bcmpc_phy_control_prepopulate_all(int unit)
{
    uint64_t lport_min = 0, lport_max = 0, lport = 0;
    uint64_t ln_idx_min = 0, ln_idx_max = 0, ln_idx = 0;
    bcmpc_phy_control_t phy_control;
    bcmpc_port_lane_t port_lane;
    pc_phy_control_entry_t entry;
    uint32_t num;


    SHR_FUNC_ENTER(unit);

    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, IMM_SID, PC_PHY_CONTROLt_PORT_IDf,
                              1, &lport_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, IMM_SID, PC_PHY_CONTROLt_PORT_IDf,
                              1, &lport_max, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, IMM_SID, PC_PHY_CONTROLt_LANE_INDEXf,
                              1, &ln_idx_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, IMM_SID, PC_PHY_CONTROLt_LANE_INDEXf,
                              1, &ln_idx_max, &num));

    sal_memset(&phy_control, 0, sizeof(phy_control));
    sal_memset(&entry, 0, sizeof(pc_phy_control_entry_t));

    /* get the default value for the PHY_CONTROL LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_phy_ctrl_lt_default_values_init(unit, &entry));

    sal_memcpy(&phy_control, &entry.data, sizeof(bcmpc_phy_control_t));

    /* Mark the entry as port unknown. */
    phy_control.oper_state = BCMPC_PORT_UNKNOWN;

    for (lport = lport_min; lport <= lport_max; lport++) {
         port_lane.lport = (uint32_t) lport;

         for (ln_idx = ln_idx_min; ln_idx <= ln_idx_max; ln_idx++) {
              port_lane.lane_index = (uint32_t) ln_idx;
              SHR_IF_ERR_EXIT
                 (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                         sizeof(bcmpc_port_lane_t), &port_lane,
                                         sizeof(bcmpc_phy_control_t),
                                         &phy_control));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PMD_FIRMWARE LT
 * entry OPERATIONAL_STATE.
 */
int
bcmpc_phy_control_oper_state_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_pport_t pport, int num_lanes,
                                    bcmpc_entry_state_t oper_state)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_phy_control_t phy_control;
    bcmpc_port_lane_t port_lane;
    bool is_internal = false;
    uint8_t idx = 0, idx_max = 0;
    uint64_t max_val = 0;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    /*
     * Set the lane index to 0, to get the firmware configuration
     * for the first lane and use it to populate all the lanes.
     */
    port_lane.lport = lport;
    port_lane.lane_index = 0;

    /*
     * Check if logical to physical port map is available.
     * Do not populate the special (CPU, internal loopback)
     * ports.
     */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT
        (pc_drv->is_internal_port(unit, pport, &is_internal));
    if (is_internal == true) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, IMM_SID, PC_PHY_CONTROLt_LANE_INDEXf,
                             1, &max_val, &num));

    max_val += 1;
    idx_max = max_val;

    for (idx = 0; idx < idx_max; idx++) {
        port_lane.lane_index = idx;

        sal_memset(&phy_control, 0, sizeof(phy_control));
        /*
         * Update the operational state of the entry.
         */
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                       sizeof(bcmpc_port_lane_t), &port_lane,
                                       sizeof(bcmpc_phy_control_t),
                                       &phy_control));

        if ((idx < num_lanes) && (oper_state == BCMPC_VALID)) {
            phy_control.oper_state = oper_state;
        } else {
            phy_control.oper_state = BCMPC_CONFIG_INVALID;
        }

        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                sizeof(bcmpc_port_lane_t), &port_lane,
                                sizeof(bcmpc_phy_control_t),
                                &phy_control));
    }

exit:

    SHR_FUNC_EXIT();
}
