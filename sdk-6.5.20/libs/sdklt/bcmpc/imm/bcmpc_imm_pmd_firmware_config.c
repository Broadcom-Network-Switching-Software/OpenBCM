/*! \file bcmpc_imm_pmd_firmware_config.c
 *
 * PC_PMD_FIRMWAREt IMM handler.
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
#define IMM_SID PC_PMD_FIRMWAREt

/*! The data structure for PC_PMD_FIRWARE_CONFIGt entry. */
typedef struct pc_pmd_firmware_config_entry_s {
    bcmpc_lport_t key;
    bcmpc_pmd_firmware_config_t data;
} pc_pmd_firmware_config_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_pmd_firmware_config_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_pmd_firmware_config_entry_t_init(pc_pmd_firmware_config_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    bcmpc_pmd_firmware_config_t_init(&entry->data);
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
pc_pmd_firmware_config_entry_set(int unit, uint32_t fid, uint64_t fval,
                                 pc_pmd_firmware_config_entry_t *entry)
{
    bcmpc_pmd_firmware_config_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PMD_FIRMWAREt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_PMD_FIRMWAREt_DFE_AUTOf:
            config->dfe_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_DFE_AUTO);
            break;
        case PC_PMD_FIRMWAREt_DFEf:
            config->dfe = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_DFE);
            break;
        case PC_PMD_FIRMWAREt_LP_DFE_AUTOf:
            config->lp_dfe_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_LP_DFE_AUTO);
            break;
        case PC_PMD_FIRMWAREt_LP_DFEf:
            config->lp_dfe = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_LP_DFE);
            break;
        case PC_PMD_FIRMWAREt_MEDIUM_TYPE_AUTOf:
            config->medium_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE_AUTO);
            break;
        case PC_PMD_FIRMWAREt_MEDIUM_TYPEf:
            config->medium = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE);
            break;
        case PC_PMD_FIRMWAREt_CL72_RESTART_TIMEOUT_EN_AUTOf:
            config->cl72_res_to_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN_AUTO);
            break;
        case PC_PMD_FIRMWAREt_CL72_RESTART_TIMEOUT_ENf:
            config->cl72_res_to = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN);
            break;
        case PC_PMD_FIRMWAREt_LP_TX_PRECODER_ON_AUTOf:
            config->lp_tx_precoder_on_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON_AUTO);
            break;
        case PC_PMD_FIRMWAREt_LP_TX_PRECODER_ONf:
            config->lp_tx_precoder_on = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON);
            break;
        case PC_PMD_FIRMWAREt_UNRELIABLE_LOS_AUTOf:
            config->unreliable_los_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS_AUTO);
            break;
        case PC_PMD_FIRMWAREt_UNRELIABLE_LOSf:
            config->unreliable_los = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS);
            break;
        case PC_PMD_FIRMWAREt_SCRAMBLING_ENABLE_AUTOf:
            config->scrambling_en_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE_AUTO);
            break;
        case PC_PMD_FIRMWAREt_SCRAMBLING_ENABLEf:
            config->scrambling_en = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE);
            break;
        case PC_PMD_FIRMWAREt_NORMAL_REACH_PAM4_AUTOf:
            config->norm_reach_pam4_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4_AUTO);
            break;
        case PC_PMD_FIRMWAREt_NORMAL_REACH_PAM4f:
            config->norm_reach_pam4_bmask = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4);
            break;
        case PC_PMD_FIRMWAREt_EXTENDED_REACH_PAM4_AUTOf:
            config->ext_reach_pam4_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4_AUTO);
            break;
        case PC_PMD_FIRMWAREt_EXTENDED_REACH_PAM4f:
            config->ext_reach_pam4_bmask = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4);
            break;
        case PC_PMD_FIRMWAREt_OPERATIONAL_STATEf:
            config->oper_state = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_pmd_firmware_config_entry_t.
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
pc_lt_pmd_firmware_config_flds_parse(int unit,
                                      const bcmltd_field_t *key_flds,
                                      const bcmltd_field_t *data_flds,
                                      pc_pmd_firmware_config_entry_t *entry)
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
            (pc_pmd_firmware_config_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_pmd_firmware_config_entry_set(unit, fid, fval, entry));

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
 * \param [in] entry PC_PMD_FIRMWARE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_pmd_firmware_lt_default_values_init(int unit,
                                       pc_pmd_firmware_config_entry_t *entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_pmd_firmware_config_t *pmd_cfg = &(entry->data);

    SHR_FUNC_ENTER(unit);

    /*
     * Check if DFE_AUTO is configured by the user,
     * if unset (0), then get the default value of dfe
     * if DFEf is not configured by the user.
     * If set (1), then dfe is auto-configured
     * based on the logical port configuration.
     * The default value for DFE is always 0 if user
     * controlled, if it is in auto-configure mode, the dfe
     * setting depends on the port configuration.
     */
    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_DFE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_DFE_AUTOf,
                                     1, &def_val, &num));
        pmd_cfg->dfe_auto = (uint8_t)def_val;
    } else {
        /*
         * dfe_auto field is configured by user
         * if 1 - dfe is set by software, get the
         *        default setting for dfe.
         * if 0 - dfe is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_DFEf,
                                     1, &def_val, &num));
        if (pmd_cfg->dfe_auto) {
            pmd_cfg->dfe = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                    PC_PMD_FIRMWAREt_DFE_AUTOf)) {
                pmd_cfg->dfe = (uint8_t)def_val;
            }
        }
    }

    /*
     * Check if LP_DFE_AUTO is configured by the user,
     * if unset (0), then get the default value of lp_dfe
     * if LP_DFEf is not configured by the user.
     * If set (1), then lp_dfe is auto-configured
     * based on the logical port configuration.
     * The default value for LP_DFE is 0 if user
     * controlled, if it is in auto-configure mode, the lp_dfe
     * setting depends on the port configuration.
     */
    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_LP_DFE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_LP_DFE_AUTOf,
                                     1, &def_val, &num));
        pmd_cfg->lp_dfe_auto = (uint8_t)def_val;
    } else {
        /*
         * lp_dfe_auto field is configured by user
         * if 1 - lp_dfe is set by software, get the
         *        default setting for dfe.
         * if 0 - lp_dfe is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_LP_DFEf,
                                     1, &def_val, &num));
        if (pmd_cfg->lp_dfe_auto) {
            pmd_cfg->lp_dfe = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                    PC_PMD_FIRMWAREt_LP_DFE_AUTOf)) {
                pmd_cfg->lp_dfe = (uint8_t)def_val;
            }
        }
    }

    /*
     * Check if MEDIUM_TYPE_AUTO is configured by the user,
     * if unset (0), then get the default value of medium type
     * if MEDIUM_TYPEf is not configured by the user.
     * If set (1), then medium is auto-configured
     * based on the logical port configuration.
     * The default value for MEDIUM_TYPE is backplane if user
     * controlled, if it is in auto-configure mode, the MEDIUM_TYPE
     * setting depends on the port configuration.
     */
    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_MEDIUM_TYPE_AUTOf,
                                     1, &def_val, &num));
        pmd_cfg->medium_auto = (uint8_t)def_val;
    } else {
        /*
         * medium_auto field is configured by user
         * if 1 - medium is set by software, get the
         *        default setting for dfe.
         * if 0 - medium is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_LP_DFEf,
                                     1, &def_val, &num));
        if (pmd_cfg->medium_auto) {
            pmd_cfg->medium = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                    PC_PMD_FIRMWAREt_MEDIUM_TYPE_AUTOf)) {
                pmd_cfg->medium = (uint8_t)def_val;
            }
        }
    }

    /*
     * Check if SCRAMBLING_ENABLE_AUTO is configured by the user,
     * if unset (0), then get the default value of scrambling_en type
     * if SCRAMBLING_ENABLEf is not configured by the user.
     * If set (1), then scrambling_en is auto-configured
     * based on the logical port configuration.
     * The default value for SCRAMBLING_ENABLE is 0 if user
     * controlled, if it is in auto-configure mode, the SCRAMBLING_ENABLE
     * setting depends on the port configuration.
     */
    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                   PC_PMD_FIRMWAREt_SCRAMBLING_ENABLE_AUTOf,
                                   1, &def_val, &num));
        pmd_cfg->scrambling_en_auto = (uint8_t)def_val;
    } else {
        /*
         * scrambling_en_auto field is configured by user
         * if 1 - scrambling_en is set by software, get the
         *        default setting for scrambling_en.
         * if 0 - scrambling_en is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_SCRAMBLING_ENABLEf,
                                     1, &def_val, &num));
        if (pmd_cfg->scrambling_en_auto) {
            pmd_cfg->scrambling_en = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                             BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE_AUTO)) {
                pmd_cfg->scrambling_en = (uint8_t)def_val;
            }
        }
    }

    /*
     * Check if CL72_RESTART_TIMEOUT_EN_AUTO is configured by the user,
     * if unset (0), then get the default value of scrambling_en type
     * if CL72_RESTART_TIMEOUT_ENf is not configured by the user.
     * If set (1), then scrambling_en is auto-configured
     * based on the logical port configuration.
     * The default value for CL72_RESTART_TIMEOUT_EN is 0 if user
     * controlled, if it is in auto-configure mode, the
     * CL72_RESTART_TIMEOUT_EN setting depends on the port configuration.
     */
    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                              PC_PMD_FIRMWAREt_CL72_RESTART_TIMEOUT_EN_AUTOf,
                              1, &def_val, &num));
        pmd_cfg->cl72_res_to_auto = (uint8_t)def_val;
    } else {
        /*
         * cl72_res_to_auto field is configured by user
         * if 1 - cl72_res_to is set by software, get the
         *        default setting for cl72_res_to.
         * if 0 - cl72_res_to is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_CL72_RESTART_TIMEOUT_ENf,
                                     1, &def_val, &num));
        if (pmd_cfg->cl72_res_to_auto) {
            pmd_cfg->cl72_res_to = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                    BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE_AUTO)) {
                pmd_cfg->cl72_res_to = (uint8_t)def_val;
            }
        }
    }

    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                              PC_PMD_FIRMWAREt_LP_TX_PRECODER_ON_AUTOf,
                              1, &def_val, &num));
        pmd_cfg->lp_tx_precoder_on_auto = (uint8_t)def_val;
    } else {
        /*
         * lp_tx_precoder_on_auto field is configured by user
         * if 1 - lp_tx_precoder_on is set by software, get the
         *        default setting for lp_tx_precoder_on.
         * if 0 - lp_tx_precoder_on is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_LP_TX_PRECODER_ONf,
                                     1, &def_val, &num));
        if (pmd_cfg->lp_tx_precoder_on_auto) {
            pmd_cfg->lp_tx_precoder_on = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                               BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON_AUTO)) {
                pmd_cfg->lp_tx_precoder_on = (uint8_t)def_val;
            }
        }
    }

    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                              PC_PMD_FIRMWAREt_UNRELIABLE_LOS_AUTOf,
                              1, &def_val, &num));
        pmd_cfg->unreliable_los_auto = (uint8_t)def_val;
    } else {
        /*
         * unreliable_los_auto field is configured by user
         * if 1 - unreliable_los is set by software, get the
         *        default setting for unreliable_los.
         * if 0 - unreliable_los is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_UNRELIABLE_LOSf,
                                     1, &def_val, &num));
        if (pmd_cfg->unreliable_los_auto) {
            pmd_cfg->unreliable_los = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                    BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS_AUTO)) {
                pmd_cfg->unreliable_los = (uint8_t)def_val;
            }
        }
    }

    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                              PC_PMD_FIRMWAREt_NORMAL_REACH_PAM4_AUTOf,
                              1, &def_val, &num));
        pmd_cfg->norm_reach_pam4_auto = (uint8_t)def_val;
    } else {
        /*
         * norm_reach_pam4_auto field is configured by user
         * if 1 - unreliable_los is set by software, get the
         *        default setting for norm_reach_pam4.
         * if 0 - norm_reach_pam4 is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_UNRELIABLE_LOS_AUTOf,
                                     1, &def_val, &num));
        if (pmd_cfg->norm_reach_pam4_auto) {
            pmd_cfg->norm_reach_pam4_bmask = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4_AUTO)) {
                pmd_cfg->norm_reach_pam4_bmask = (uint8_t)def_val;
            }
        }
    }

    if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                              PC_PMD_FIRMWAREt_EXTENDED_REACH_PAM4_AUTOf,
                              1, &def_val, &num));
        pmd_cfg->ext_reach_pam4_auto = (uint8_t)def_val;
    } else {
        /*
         * ext_reach_pam4_auto field is configured by user
         * if 1 - unreliable_los is set by software, get the
         *        default setting for ext_reach_pam4.
         * if 0 - ext_reach_pam4 is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PMD_FIRMWAREt,
                                     PC_PMD_FIRMWAREt_EXTENDED_REACH_PAM4_AUTOf,
                                     1, &def_val, &num));
        if (pmd_cfg->ext_reach_pam4_auto) {
            pmd_cfg->ext_reach_pam4_bmask = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(pmd_cfg->fbmp,
                                BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4_AUTO)) {
                pmd_cfg->ext_reach_pam4_bmask = (uint8_t)def_val;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PMD Firmware configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for PMD_FIRMWARE LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_pmd_firmware_config_validate(int unit, bcmpc_lport_t key,
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
pc_pmd_firmware_config_stage(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            bcmimm_entry_event_t event,
                            const bcmltd_field_t *key_flds,
                            const bcmltd_field_t *data_flds,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    pc_pmd_firmware_config_entry_t entry;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_pmd_firmware_config_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    /*
     * The PMD_FIRMWARE staging function is invoked for any operation on
     * PC_PMD_FIRMWARE LT. During pre-config stage of System Manager
     * initialization, the staging function gets invoked for any
     * PC_PMD_FIRMWARE LT entry in the config.yml. Logical port
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
     * PMD_FIRMWARE LT staging function gets called indicating an
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
     * update the PMD_FIRMWARE LT entry and the system is run-stage.
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
     * On PMD_FIRMWARE LT operation, validate if the config can be
     * applied to the hardware due to dependencies on other LTs
     * provisioning. If this check fails, return success and
     * update the operational state of the PMD_FIRMWARE LT entry
     * as not valid.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_pmd_firmware_config_flds_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pmd_firmware_config_validate(unit, entry.key, &config_valid,
                                        &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_pmd_firmware_config_flds_parse(unit, NULL, data_flds,
                                                  &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_pmd_firmware_config_insert(unit, entry.key,
                                                       &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_pmd_firmware_config_flds_parse(unit, NULL, data_flds,
                                                  &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_pmd_firmware_config_update(unit, entry.key,
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
 * Convert bcmltd_fields_t array \c flds to bcmpc_lport_t \c key and a
 * bcmpc_pmd_firmware_config_t \c data.
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
pc_imm_pmd_firmware_config_entry_parse(int unit, const bcmltd_fields_t *flds,
                                       size_t key_size, void *key,
                                       size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    pc_pmd_firmware_config_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_pmd_firmware_config_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_pmd_firmware_config_entry_set(unit, fid, fval, &entry));
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
 * Convert bcmpc_lport_t \c key and a bcmpc_pmd_firmware_config_t \c data to
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
pc_imm_pmd_firmware_config_entry_fill(int unit,
                                      size_t key_size, void *key,
                                      size_t data_size, void *data,
                                      bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_pmd_firmware_config_t *pmd_fw_cfg = data;

    SHR_FUNC_ENTER(unit);

    if (data && data_size != sizeof(*pmd_fw_cfg)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_DFE_AUTOf, 0,
             pmd_fw_cfg->dfe_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_DFEf, 0,
             pmd_fw_cfg->dfe, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_LP_DFE_AUTOf, 0,
             pmd_fw_cfg->lp_dfe_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_LP_DFEf, 0,
             pmd_fw_cfg->lp_dfe, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_MEDIUM_TYPE_AUTOf, 0,
             pmd_fw_cfg->medium_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_MEDIUM_TYPEf, 0,
             pmd_fw_cfg->medium, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_CL72_RESTART_TIMEOUT_EN_AUTOf, 0,
             pmd_fw_cfg->cl72_res_to_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_CL72_RESTART_TIMEOUT_ENf, 0,
             pmd_fw_cfg->cl72_res_to, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_LP_TX_PRECODER_ON_AUTOf, 0,
             pmd_fw_cfg->lp_tx_precoder_on_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_LP_TX_PRECODER_ONf, 0,
             pmd_fw_cfg->lp_tx_precoder_on, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_UNRELIABLE_LOS_AUTOf, 0,
             pmd_fw_cfg->unreliable_los_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_UNRELIABLE_LOSf, 0,
             pmd_fw_cfg->unreliable_los, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_SCRAMBLING_ENABLE_AUTOf, 0,
            pmd_fw_cfg->scrambling_en_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_SCRAMBLING_ENABLEf, 0,
            pmd_fw_cfg->scrambling_en, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_NORMAL_REACH_PAM4_AUTOf, 0,
            pmd_fw_cfg->norm_reach_pam4_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_NORMAL_REACH_PAM4f, 0,
            pmd_fw_cfg->norm_reach_pam4_bmask, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_EXTENDED_REACH_PAM4_AUTOf, 0,
             pmd_fw_cfg->ext_reach_pam4_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_EXTENDED_REACH_PAM4f, 0,
             pmd_fw_cfg->ext_reach_pam4_bmask, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PMD_FIRMWAREt_OPERATIONAL_STATEf, 0,
             pmd_fw_cfg->oper_state, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_pmd_firmware_config_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_pmd_firmware_config_entry_parse;
    db_hdl.entry_fill = pc_imm_pmd_firmware_config_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_pmd_firmware_config_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PMD_FIRMWARE LT
 * with the default values for the port based on the speed,
 * num-lanes and fec configuration in PC_PORT LT.
 * The logical to physical port map must be available when
 * calling this function.
 */
int
bcmpc_pmd_firmware_config_prepopulate(int unit, bcmpc_lport_t lport,
                                      bcmpc_port_cfg_t *pcfg, bool null_entry)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pmd_firmware_config_t fw_cfg;
    bcmpc_pmgr_pmd_firmware_status_t fw_status;
    bool is_internal = false;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    sal_memset(&fw_cfg, 0, sizeof(fw_cfg));
    sal_memset(&fw_status, 0, sizeof(fw_status));

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

        SHR_IF_ERR_EXIT(bcmpc_pmgr_pmd_firmware_status_get(unit, lport,
                                                           &fw_status));

        fw_cfg.dfe = fw_status.dfe;
        fw_cfg.lp_dfe = fw_status.lp_dfe;
        fw_cfg.medium = fw_status.medium;
        fw_cfg.cl72_res_to = fw_status.cl72_res_to;
        fw_cfg.lp_tx_precoder_on = fw_status.lp_tx_precoder_on;
        fw_cfg.unreliable_los = fw_status.unreliable_los;
        fw_cfg.scrambling_en = fw_status.scrambling_en;
        fw_cfg.norm_reach_pam4_bmask = fw_status.norm_reach_pam4_bmask;
        fw_cfg.ext_reach_pam4_bmask = fw_status.ext_reach_pam4_bmask;
    }

    /*
     * On Port add, Insert if enrty present, else update.
     */
    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pmd_firmware_config_t),
                                   &fw_cfg);

    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                      sizeof(bcmpc_lport_t), &lport,
                                      sizeof(bcmpc_pmd_firmware_config_t),
                                      &fw_cfg));
    } else {
        /* Check the operational satet of the entry and mark it as valid. */
        if (fw_cfg.oper_state != BCMPC_VALID) {
            fw_cfg.oper_state = BCMPC_VALID;

            SHR_IF_ERR_EXIT
                (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                           sizeof(bcmpc_lport_t), &lport,
                                           sizeof(bcmpc_pmd_firmware_config_t),
                                           &fw_cfg));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * This function should be invoked when a new logical port is
 * deleted from the system to delete the associated
 * PC_PMD_FIRMWARE LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pmd_firmware_config_delete(int unit, bcmpc_lport_t lport)
{
    bcmpc_pmd_firmware_config_t fw_cfg;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&fw_cfg, 0, sizeof(fw_cfg));

    /*
     * On Port delete, delete if entry present.
     */
    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pmd_firmware_config_t),
                                   &fw_cfg);

    /* Mark the entry as port unknown. */
    fw_cfg.oper_state = BCMPC_PORT_UNKNOWN;

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
           (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pmd_firmware_config_t),
                                   &fw_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PMD_FIRMWARE LT entries
 * with null entries.
 */
int
bcmpc_pmd_firmware_prepopulate_all(int unit)
{
    uint64_t lport_min = 0, lport_max = 0;
    bcmpc_pmd_firmware_config_t fw_cfg;
    pc_pmd_firmware_config_entry_t entry;
    uint32_t num;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;

    SHR_FUNC_ENTER(unit);

    pc_pmd_firmware_config_entry_t_init(&entry);

    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, IMM_SID, PC_PMD_FIRMWAREt_PORT_IDf,
                              1, &lport_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, IMM_SID, PC_PMD_FIRMWAREt_PORT_IDf,
                              1, &lport_max, &num));

    sal_memset(&fw_cfg, 0, sizeof(fw_cfg));

    /* Get the software defaults from the map table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pmd_firmware_lt_default_values_init(unit, &entry));

    /* Copy the default values into the pmd_firmware structure. */
    sal_memcpy(&fw_cfg, &entry.data, sizeof(bcmpc_pmd_firmware_config_t));

    /* Mark the entry as port unknown. */
    fw_cfg.oper_state = BCMPC_PORT_UNKNOWN;

    for (lport = lport_min; lport <= lport_max; lport++) {
        SHR_IF_ERR_EXIT
           (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pmd_firmware_config_t),
                                   &fw_cfg));
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
bcmpc_pmd_firmware_oper_state_update(int unit, bcmpc_lport_t lport,
                                     bcmpc_pport_t pport,
                                     bcmpc_entry_state_t oper_state)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pmd_firmware_config_t fw_cfg;
    bool is_internal = false;

    SHR_FUNC_ENTER(unit);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    sal_memset(&fw_cfg, 0, sizeof(fw_cfg));

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


    /* Update the operational state of the entry. */
    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pmd_firmware_config_t),
                                   &fw_cfg));

    fw_cfg.oper_state = oper_state;
    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_pmd_firmware_config_t),
                                   &fw_cfg));

exit:
    SHR_FUNC_EXIT();
}
