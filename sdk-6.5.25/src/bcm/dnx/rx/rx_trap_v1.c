/** \file rx_trap_v1.c
 * 
 *
 * RX procedures for DNX.
 *
 * Here add DESCRIPTION.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/*
 * Include files.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <shared/shrextend/shrextend_debug.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>
#include <src/bcm/dnx/rx/rx_trap_map_dispatch.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_rx_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/drv.h>
#include <soc/cmicx.h>
#include <soc/mem.h>

#ifdef BCM_DNX2_SUPPORT
#include <src/bcm/dnx/rx/rx_trap_v1.h>

/**
 * \brief -
 *   Initialize the trap model for for the ERPP Filters.
 *
 * \param [in] unit - Unit ID
 * \param [in] type - Trap type for the filter
 * \param [out] trap_gport - Trap Gport
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *  None.
 */

static shr_error_e
dnx_rx_trap_v1_erpp_drop_profile_create(
    int unit,
    bcm_rx_trap_t type,
    bcm_gport_t * trap_gport)
{
    int trap_id;
    bcm_rx_trap_config_t trap_config;

    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
    trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, type, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    BCM_GPORT_TRAP_SET(*trap_gport, trap_id, dnx_data_trap.strength.default_trap_strength_get(unit),
                       trap_config.snoop_strength);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the parameters of dnx_rx_trap_etpp_config_t to default
*  values
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] etpp_config_p  -  Pointer type of dnx_rx_trap_etpp_config_t\n
*           \b As \b output - \n
*           This pocedure loads pointed memory by defualt value
*           of dnx_rx_trap_etpp_config_t structure
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_config_t_init(
    int unit,
    dnx_rx_trap_etpp_config_t * etpp_config_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(etpp_config_p, _SHR_E_PARAM, "etpp_config_p");

    sal_memset(&etpp_config_p->action, 0, sizeof(dnx_rx_trap_etpp_action_t));
    etpp_config_p->action.is_int_stats_en = TRUE;
    sal_memset(&etpp_config_p->recycle_cmd_config, 0, sizeof(dnx_rx_trap_recycle_cmd_config_t));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function is to set the global enabler of ETPP visibility trap
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] enable    -  enable/disable visibility trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_visibility_enable_hw_set(
    int unit,
    uint8 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_GLOBAL_MODE_VISIBILITY_TRAP_EN, INST_SINGLE,
                                 enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get etpp drop configurable statistics actions from HW.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [out] etpp_action_p  -  Hold the etpp trap action to set
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_drop_hw_get(
    int unit,
    dnx_rx_trap_etpp_action_t * etpp_action_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CFG_PP_DROP_REASON_FOR_CRPS, INST_SINGLE,
                               (uint32 *) &(etpp_action_p->pp_drop_reason));
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_CFG_INTERNAL_STATISTICS_ENABLE, INST_SINGLE,
                              &etpp_action_p->is_int_stats_en);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set etpp trap action in HW.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] profile  -  profile num to configure in HW
*   \param [in] etpp_action_p  -  Hold the etpp trap action to set
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_user_defined_action_hw_get(
    int unit,
    int profile,
    dnx_rx_trap_etpp_action_t * etpp_action_p)
{
    uint32 entry_handle_id;
    uint8 gen_recycle_copy, is_user_defined = FALSE;
    dnx_rx_trap_map_etpp_profile_dbal_fields_t etpp_profile_dbal_fields;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));

    sal_memset(&etpp_profile_dbal_fields, 0, sizeof(dnx_rx_trap_map_etpp_profile_dbal_fields_t));

    SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_ud_profile_map_info_get(unit, profile, &etpp_profile_dbal_fields));

    /** Retrieve the values */
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_gen_recycle_copy, INST_SINGLE,
                              &gen_recycle_copy);
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_prio, INST_SINGLE,
                              &(etpp_action_p->is_recycle_high_priority));
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_crop, INST_SINGLE,
                              &(etpp_action_p->is_recycle_crop_pkt));
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_append_orig, INST_SINGLE,
                              &(etpp_action_p->is_recycle_append_ftmh));
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_cmd, INST_SINGLE,
                              &(etpp_action_p->recycle_cmd));
    dbal_value_field32_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_pp_drop_reason, INST_SINGLE,
                               (uint32 *) &(etpp_action_p->pp_drop_reason));
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_kill_mirror_copy, INST_SINGLE,
                              &(etpp_action_p->kill_mirror_copy));

    SHR_IF_ERR_EXIT(dnx_rx_trap_is_user_defined_dispatch(unit, DNX_RX_TRAP_BLOCK_ETPP, profile, &is_user_defined));
    if (is_user_defined)
    {
        dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_keep_sys_hdr, INST_SINGLE,
                                  &(etpp_action_p->is_pkt_keep_fabric_headers));
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    etpp_action_p->is_rcy_copy_drop = !gen_recycle_copy;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set etpp drop configurable statistics actions in HW.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] etpp_action_p  -  Hold the etpp trap action to set
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_drop_hw_set(
    int unit,
    dnx_rx_trap_etpp_action_t * etpp_action_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_PP_DROP_REASON_FOR_CRPS, INST_SINGLE,
                                 etpp_action_p->pp_drop_reason);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CFG_INTERNAL_STATISTICS_ENABLE, INST_SINGLE,
                                etpp_action_p->is_int_stats_en);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set etpp trap action in HW.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] profile  -  profile num to configure in HW
*   \param [in] is_clear -  is clear profile, indication if to set profile to default
*   \param [in] etpp_action_p  -  Hold the etpp trap action to set
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_user_defined_action_hw_set(
    int unit,
    int profile,
    uint8 is_clear,
    dnx_rx_trap_etpp_action_t * etpp_action_p)
{
    uint32 entry_handle_id;
    uint8 is_user_defined;
    dnx_rx_trap_map_etpp_profile_dbal_fields_t etpp_profile_dbal_fields;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));

    sal_memset(&etpp_profile_dbal_fields, 0, sizeof(dnx_rx_trap_map_etpp_profile_dbal_fields_t));

    SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_ud_profile_map_info_get(unit, profile, &etpp_profile_dbal_fields));

    if (is_clear)
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_drop_fwd_copy, INST_SINGLE,
                                    FALSE);
        dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_gen_recycle_copy, INST_SINGLE,
                                    FALSE);
    }
    else
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_drop_fwd_copy, INST_SINGLE,
                                    TRUE);
        dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_gen_recycle_copy, INST_SINGLE,
                                    !(etpp_action_p->is_rcy_copy_drop));
    }

    dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_prio, INST_SINGLE,
                                etpp_action_p->is_recycle_high_priority);
    dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_crop, INST_SINGLE,
                                etpp_action_p->is_recycle_crop_pkt);
    dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_append_orig, INST_SINGLE,
                                etpp_action_p->is_recycle_append_ftmh);
    dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_cmd, INST_SINGLE,
                                etpp_action_p->recycle_cmd);
    dbal_entry_value_field32_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_pp_drop_reason, INST_SINGLE,
                                 etpp_action_p->pp_drop_reason);
    dbal_entry_value_field32_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_kill_mirror_copy, INST_SINGLE,
                                 etpp_action_p->kill_mirror_copy);

    SHR_IF_ERR_EXIT(dnx_rx_trap_is_user_defined_dispatch(unit, DNX_RX_TRAP_BLOCK_ETPP, profile, &is_user_defined));
    if (is_user_defined)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_keep_sys_hdr, INST_SINGLE,
                                     etpp_action_p->is_pkt_keep_fabric_headers);
        dbal_entry_value_field32_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_keep_term_hdr, INST_SINGLE,
                                     etpp_action_p->is_pkt_keep_fabric_headers);
        dbal_entry_value_field32_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_candidate_hdr_en, INST_SINGLE,
                                     (!(etpp_action_p->is_pkt_keep_fabric_headers) & !is_clear));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the etpp user defined trap action.
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit Id
*   \param [in] profile         -  etpp profile to configure
*   \param [in] is_clear        -  is clear profile, indication if to set profile to default
*   \param [in] etpp_config_p   -  Hold the etpp trap action profile configuration to set for specific trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_user_defined_set(
    int unit,
    int profile,
    uint8 is_clear,
    dnx_rx_trap_etpp_config_t * etpp_config_p)
{
    dnx_rx_trap_etpp_action_t etpp_action_get = { 0 };
    int new_recycle_cmd;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_user_defined_action_hw_get(unit, profile, &etpp_action_get));

    /** Exchange recycle command in cases:
      * 1. recycle strength is non-zero, meaning user is asking for valid recycle command
      * 2. clearing command (zero strength) and old recycle command is non-zero */
    if (etpp_config_p->recycle_cmd_config.recycle_strength != 0 ||
        (etpp_config_p->recycle_cmd_config.recycle_strength == 0 && etpp_action_get.recycle_cmd != 0))
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_profile_set
                        (unit, etpp_action_get.recycle_cmd, &(etpp_config_p->recycle_cmd_config), &new_recycle_cmd));

        etpp_config_p->action.recycle_cmd = new_recycle_cmd;
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_user_defined_action_hw_set(unit, profile, is_clear, &(etpp_config_p->action)));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set etpp oam trap action in HW.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_type   -  etpp oam trap type (SW id) to configure
*   \param [in] etpp_action_p  -  Hold the etpp trap action to set
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_oam_action_hw_set(
    int unit,
    int trap_type,
    dnx_rx_trap_etpp_action_t * etpp_action_p)
{
    uint32 entry_handle_id;
    dnx_rx_trap_map_etpp_profile_dbal_fields_t etpp_profile_dbal_fields;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));

    sal_memset(&etpp_profile_dbal_fields, 0, sizeof(dnx_rx_trap_map_etpp_profile_dbal_fields_t));

    SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_oam_profile_map_info_get(unit, trap_type, &etpp_profile_dbal_fields));

    /** Set the values */
    dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_prio, INST_SINGLE,
                                etpp_action_p->is_recycle_high_priority);
    dbal_entry_value_field8_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_cmd, INST_SINGLE,
                                etpp_action_p->recycle_cmd);

    if (DNX_RX_TRAP_IS_ETPP_OAM_UP_MEP(trap_type))
    {
        /** for UP MEP traps need to set LM, DM and CCM trap codes */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CPU_TRAP_CODE_CCM, INST_SINGLE,
                                     etpp_action_p->ingress_trap_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CPU_TRAP_CODE_LM, INST_SINGLE,
                                     etpp_action_p->ingress_trap_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CPU_TRAP_CODE_DM, INST_SINGLE,
                                     etpp_action_p->ingress_trap_id);
    }
    else if (DNX_RX_TRAP_IS_ETPP_OAM_ERR(trap_type))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_cpu_trap_code, INST_SINGLE,
                                     etpp_action_p->ingress_trap_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get etpp oam trap action in HW.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_type   -  etpp oam trap type (SW id) to configure
*   \param [out] etpp_action_p  -  Hold the etpp trap action retrieved
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_oam_action_hw_get(
    int unit,
    int trap_type,
    dnx_rx_trap_etpp_action_t * etpp_action_p)
{
    uint32 entry_handle_id;
    dnx_rx_trap_map_etpp_profile_dbal_fields_t etpp_profile_dbal_fields;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TRAP, &entry_handle_id));

    sal_memset(&etpp_profile_dbal_fields, 0, sizeof(dnx_rx_trap_map_etpp_profile_dbal_fields_t));

    SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_oam_profile_map_info_get(unit, trap_type, &etpp_profile_dbal_fields));

    /** Retrieve the values */
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_prio, INST_SINGLE,
                              &etpp_action_p->is_recycle_high_priority);
    dbal_value_field8_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_rcy_cmd, INST_SINGLE,
                              &etpp_action_p->recycle_cmd);

    if (trap_type == DNX_RX_TRAP_ETPP_OAM_REFLECTOR)
    {
        etpp_action_p->ingress_trap_id = 0;
    }
    else
    {
        dbal_value_field32_request(unit, entry_handle_id, etpp_profile_dbal_fields.cfg_cpu_trap_code, INST_SINGLE,
                                   (uint32 *) &etpp_action_p->ingress_trap_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the etpp oam trap action.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_type   -  etpp oam trap type (SW id) to configure
*   \param [in] etpp_config_p  -  Hold the etpp trap configuration to set for specific trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_oam_set(
    int unit,
    int trap_type,
    dnx_rx_trap_etpp_config_t * etpp_config_p)
{
    dnx_rx_trap_etpp_action_t etpp_action_get = { 0 };
    int new_recycle_cmd;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_oam_action_hw_get(unit, trap_type, &etpp_action_get));

    /** Exchange recycle command in cases:
     * 1. recycle strength is non-zero, meaning user is asking for valid recycle command
     * 2. clearing command (zero strength) and old recycle command is non-zero */
    if (etpp_config_p->recycle_cmd_config.recycle_strength != 0 ||
        (etpp_config_p->recycle_cmd_config.recycle_strength == 0 && etpp_action_get.recycle_cmd != 0))
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_profile_set
                        (unit, etpp_action_get.recycle_cmd, &(etpp_config_p->recycle_cmd_config), &new_recycle_cmd));

        etpp_config_p->action.recycle_cmd = new_recycle_cmd;
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_oam_action_hw_set(unit, trap_type, &(etpp_config_p->action)));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify the parameters of etpp trap configuration
* \par DIRECT INPUT:
*   \param [in] unit           -  Unit Id
*   \param [in] trap_class     -  trap class to configure
*   \param [in] trap_type      -  trap class to configure
*   \param [in] etpp_config_p  -  etpp trap configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_set_verify(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type,
    dnx_rx_trap_etpp_config_t * etpp_config_p)
{
    dnx_rx_trap_block_e recycle_trap_id_block = DNX_RX_TRAP_BLOCK_INVALID;
    dnx_rx_trap_block_e stamped_trap_id_block = DNX_RX_TRAP_BLOCK_INVALID;
    uint8 is_allocated, is_user_defined = FALSE;
    int recycle_trap_id, stamped_trap_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(etpp_config_p, _SHR_E_PARAM, "etpp_config_p");

    recycle_trap_id = etpp_config_p->recycle_cmd_config.ingress_trap_id;
    recycle_trap_id_block = DNX_RX_TRAP_ID_BLOCK_GET(recycle_trap_id);
    if (recycle_trap_id_block != DNX_RX_TRAP_BLOCK_INGRESS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cpu_trap_gport can only encode an INGRESS trap code!");
    }

    /** Check if ingress trap id was allocated */
    SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, recycle_trap_id, &is_allocated));
    if (etpp_config_p->recycle_cmd_config.recycle_strength != 0 && is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "trap_id=%d was not created, call bcm_rx_trap_type_create() first \n",
                     recycle_trap_id);
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_is_user_defined_dispatch(unit, DNX_RX_TRAP_BLOCK_ETPP, trap_type, &is_user_defined));
    if ((etpp_config_p->action.is_pkt_keep_fabric_headers == TRUE) && !is_user_defined)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Action Profile %d, cannot set action keep fabric header, please use ETPP user trap id \n",
                     trap_type);
    }

    stamped_trap_id = etpp_config_p->action.ingress_trap_id;
    if (stamped_trap_id != 0 && trap_class != DNX_RX_TRAP_CLASS_OAM)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "stamped_trap_code can only be configured for OAM ETPP traps\n");
    }

    stamped_trap_id_block = DNX_RX_TRAP_ID_BLOCK_GET(stamped_trap_id);
    if (stamped_trap_id_block != DNX_RX_TRAP_BLOCK_INGRESS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "stamped_trap_code=%d is not an INGRESS trap code!", stamped_trap_id);
    }

    if (DNX_RX_TRAP_IS_BOOL_INDICATION(etpp_config_p->action.is_recycle_high_priority) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "is_recycle_high_priority should be of value FALSE(0) or TRUE(1) only!");
    }

    if (DNX_RX_TRAP_IS_BOOL_INDICATION(etpp_config_p->action.is_recycle_crop_pkt) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "is_recycle_crop_pkt should be of value FALSE(0) or TRUE(1) only!");
    }

    if (DNX_RX_TRAP_IS_BOOL_INDICATION(etpp_config_p->action.is_recycle_append_ftmh) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "is_recycle_append_ftmh should be of value FALSE(0) or TRUE(1) only!");
    }

    if ((etpp_config_p->action.is_int_stats_en == FALSE) && (trap_type != DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_DROP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_RX_TRAP_FLAGS2_INT_STAT_DISABLE flag can only be set for BCM_RX_TRAP_EG_TX_TRAP_ID_DROP!");
    }

    if ((etpp_config_p->action.kill_mirror_copy == TRUE) && trap_class == DNX_RX_TRAP_CLASS_OAM)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "BCM_RX_TRAP_FLAGS2_MIRROR_CODE_CLEAR can not be configured for OAM ETPP traps\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Extract ETPP trap configuration from general trap configuration.
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit id
*   \param [in] profile  -  ETPP trap profile to configure
*   \param [in] config_p   -  general trap action profile configuration
*   \param [out] etpp_config_p   -  etpp trap action profile configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_config_from_config_fill(
    int unit,
    int profile,
    bcm_rx_trap_config_t * config_p,
    dnx_rx_trap_etpp_config_t * etpp_config_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config_p, _SHR_E_PARAM, "config_p");

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_config_t_init(unit, etpp_config_p));
    etpp_config_p->action.is_rcy_copy_drop = FALSE;

    if (config_p->flags & BCM_RX_TRAP_COPY_DROP)
    {
        if (profile < DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1
            || profile > DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_VISIBILITY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid action, BCM_RX_TRAP_COPY_DROP can only be set for UserDefined and Visibility trap profiles!");
        }
        else
        {
            etpp_config_p->action.is_rcy_copy_drop = TRUE;
        }
    }

    if (config_p->flags2 & BCM_RX_TRAP_FLAGS2_KEEP_FABRIC_HEADER)
    {
        etpp_config_p->action.is_pkt_keep_fabric_headers = TRUE;
    }

    if (!BCM_GPORT_IS_TRAP(config_p->cpu_trap_gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cpu_trap_gport needs to be encoded as trap gport."
                     "Use BCM_GPORT_TRAP_SET to encode an ingress trap id and forward strength");
    }
    else
    {
        etpp_config_p->recycle_cmd_config.recycle_strength = BCM_GPORT_TRAP_GET_STRENGTH(config_p->cpu_trap_gport);
        /** configure recycle trap_id if strength is non-zero */
        if (etpp_config_p->recycle_cmd_config.recycle_strength != 0)
        {
            etpp_config_p->recycle_cmd_config.ingress_trap_id = BCM_GPORT_TRAP_GET_ID(config_p->cpu_trap_gport);
        }
    }

    etpp_config_p->action.is_recycle_high_priority = config_p->is_recycle_high_priority;
    etpp_config_p->action.is_recycle_crop_pkt = config_p->is_recycle_crop_pkt;
    etpp_config_p->action.is_recycle_append_ftmh = config_p->is_recycle_append_ftmh;
    etpp_config_p->action.ingress_trap_id = config_p->stamped_trap_code;
    etpp_config_p->action.pp_drop_reason = config_p->pp_drop_reason;
    etpp_config_p->action.is_int_stats_en = ((config_p->flags2 & BCM_RX_TRAP_FLAGS2_INT_STAT_DISABLE) == 0);
    etpp_config_p->action.kill_mirror_copy = ((config_p->flags2 & BCM_RX_TRAP_FLAGS2_MIRROR_CODE_CLEAR) != 0);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Fill bcm_rx_trap_config_t from ETPP trap configuration struct.
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit id
*   \param [in] etpp_config_p   -  etpp trap action profile configuration
*   \param [out] config_p   -  general trap action profile configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_config_from_etpp_config_fill(
    int unit,
    dnx_rx_trap_etpp_config_t * etpp_config_p,
    bcm_rx_trap_config_t * config_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(etpp_config_p, _SHR_E_PARAM, "etpp_config_p");

    bcm_rx_trap_config_t_init(config_p);

    if (etpp_config_p->action.is_pkt_drop == TRUE)
    {
        config_p->flags |= BCM_RX_TRAP_UPDATE_DEST;
        config_p->dest_port = BCM_GPORT_BLACK_HOLE;
    }
    else if (etpp_config_p->action.is_rcy_copy_drop == TRUE)
    {
        config_p->flags |= BCM_RX_TRAP_COPY_DROP;
    }
    else
    {
        BCM_GPORT_TRAP_SET(config_p->cpu_trap_gport, etpp_config_p->recycle_cmd_config.ingress_trap_id,
                           etpp_config_p->recycle_cmd_config.recycle_strength, 0);
        config_p->stamped_trap_code = etpp_config_p->action.ingress_trap_id;
        /** In HW 0 = high priority, 1 = low priority, so we invert the given value */
        config_p->is_recycle_high_priority = (etpp_config_p->action.is_recycle_high_priority == FALSE);
        config_p->is_recycle_crop_pkt = etpp_config_p->action.is_recycle_crop_pkt;
        config_p->is_recycle_append_ftmh = etpp_config_p->action.is_recycle_append_ftmh;
    }

    config_p->pp_drop_reason = etpp_config_p->action.pp_drop_reason;

    if (etpp_config_p->action.is_int_stats_en == FALSE)
    {
        config_p->flags2 |= BCM_RX_TRAP_FLAGS2_INT_STAT_DISABLE;
    }

    if (etpp_config_p->action.is_pkt_keep_fabric_headers == TRUE)
    {
        config_p->flags2 |= BCM_RX_TRAP_FLAGS2_KEEP_FABRIC_HEADER;
    }

    if (etpp_config_p->action.kill_mirror_copy == TRUE)
    {
        config_p->flags2 |= BCM_RX_TRAP_FLAGS2_MIRROR_CODE_CLEAR;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  The function checks the reference count of trap types using the same profile(trap_id).
*  Counts how many Trap types are connected to the same profile, we don't want to delete profile,
*  which has multiple trap_types connected to it.
* \par DIRECT INPUT:
*   \param [in] unit        -  Unit id
*   \param [in] profile     -  Trap ID(profile).
*   \param [out] ref_count  -  Reference count of profiles.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_etpp_user_defined_profile_ref_count_get(
    int unit,
    int profile,
    int *ref_count)
{
    dnx_rx_trap_action_profile_t action_profile;
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);

    *ref_count = 0;

    for (trap_id = DBAL_ENUM_FVAL_ETPP_TRAP_ID_SPLIT_HORIZON; trap_id < DBAL_NOF_ENUM_ETPP_TRAP_ID_VALUES; trap_id++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_action_profile_hw_get(unit, trap_id, &action_profile));
        if (action_profile.profile == profile)
        {
            (*ref_count)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the ingress LIF trap Profile in the correct IN_LIF table, for the supplied IN_LIF.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_hw_resources_p  - LIF's HW resources
*   \param [in] lif_trap_profile - LIF trap profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_lif_ingress_profile_hw_set(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    uint32 lif_trap_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_hw_resources_p->inlif_dbal_table_id, &entry_handle_id));

    /** key construction -In LIF*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, lif_hw_resources_p->local_in_lif);

    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 lif_hw_resources_p->inlif_dbal_result_type);

    /** Set the LIF trap profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, lif_trap_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the ingress LIF trap Profile in the correct IN_LIF table, for the supplied IN_LIF.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_hw_resources_p  - LIF's HW resources
*   \param [out] lif_trap_profile_p - LIF trap profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_lif_ingress_profile_hw_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    uint32 *lif_trap_profile_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_hw_resources_p->inlif_dbal_table_id, &entry_handle_id));

    /** key construction -In LIF*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, lif_hw_resources_p->local_in_lif);

    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 lif_hw_resources_p->inlif_dbal_result_type);

    /** Set the LIF trap profile */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, lif_trap_profile_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the ETPP LIF trap Profile in the correct OUT_LIF table, for the supplied OUT_LIF.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_gport  - LIF gport
*   \param [in] lif_hw_resources_p  - LIF's HW resources
*   \param [in] lif_trap_profile - LIF trap Profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_lif_etpp_profile_lif_hw_set(
    int unit,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    uint32 lif_trap_profile)
{
    uint32 entry_handle_id;
    int is_table_direct;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_hw_resources_p->outlif_dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set(unit, entry_handle_id, lif_gport, lif_hw_resources_p));

    /** Set the LIF trap profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, lif_trap_profile);

    SHR_IF_ERR_EXIT(dbal_tables_table_is_direct(unit, lif_hw_resources_p->outlif_dbal_table_id, &is_table_direct));

    if (is_table_direct == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the ETPP LIF trap Profile in the correct OUT_LIF table, for the supplied OUT_LIF.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_gport  - LIF gport
*   \param [in] lif_hw_resources_p  - LIF's HW resources
*   \param [out] lif_trap_profile_p - LIF trap Profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_rx_trap_v1_lif_etpp_profile_lif_hw_get(
    int unit,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    uint32 *lif_trap_profile_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_hw_resources_p->outlif_dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set(unit, entry_handle_id, lif_gport, lif_hw_resources_p));

    /** Get the LIF trap profile */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, lif_trap_profile_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_create(
    int unit,
    int flags,
    dnx_rx_trap_class_e trap_class,
    bcm_rx_trap_t type,
    int *trap_id_p)
{
    int oam_trap_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");

    switch (trap_class)
    {
        case DNX_RX_TRAP_CLASS_USER_DEFINED:
            SHR_IF_ERR_EXIT(dnx_algo_rx_trap_id_etpp_allocate(unit, flags, type, trap_id_p));
            if ((type == bcmRxTrapEgTxVisibility) &&
                dnx_data_trap.etpp.feature_get(unit, dnx_data_trap_etpp_visibility_trap_support))
            {
                SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_visibility_enable_hw_set(unit, TRUE));
            }
            *trap_id_p = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED, *trap_id_p);
            break;
        case DNX_RX_TRAP_CLASS_OAM:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_GET(*trap_id_p);
            SHR_IF_ERR_EXIT(dnx_algo_rx_trap_id_etpp_oam_allocate(unit, &oam_trap_type));
            *trap_id_p = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, *trap_id_p);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal trap_class=%d\n", trap_class);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_erpp_filters_init(
    int unit,
    bcm_gport_t init_trap_gport)
{
    uint8 unknown_da_enabled = FALSE;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);

    unknown_da_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_unknown_da_trap_en);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_erpp_ser_handling_init(unit, BCM_GPORT_TRAP_GET_ID(init_trap_gport)));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgHairPinFilter, init_trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpmcTtlErr, init_trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4Ttl0, init_trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4Ttl1, init_trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgExcludeSrc, init_trap_gport));

    if (unknown_da_enabled && system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        /** Trap is not working in Jr2-A0(Jr2 System Headers mode) */
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgUnknownDa, init_trap_gport));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_filters_init(
    int unit)
{
    bcm_gport_t trap_gport = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Setting the Trap GPORT for DROP profile with default trap strength and 0 for snoop strength.
     */
    BCM_GPORT_TRAP_SET(trap_gport,
                       BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, dnx_data_trap.strength.default_trap_strength_get(unit), 0);

    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxPortNotVlanMember, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxSplitHorizonFilter, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxStpStateFail, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxProtectionPathUnexpected, trap_gport));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_egress_filters_init(
    int unit)
{
    bcm_gport_t trap_gport_drop;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initializations for handling ERPP Filters
     */
    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_erpp_drop_profile_create(unit, bcmRxTrapEgUserDefine, &trap_gport_drop));
    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_erpp_filters_init(unit, trap_gport_drop));
    /*
     * Initializations for handling ETPP Filters
     */
    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_filters_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_set(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type,
    bcm_rx_trap_config_t * config_p)
{
    int oam_trap_type;
    dnx_rx_trap_etpp_config_t etpp_config;
    SHR_FUNC_INIT_VARS(unit);

    dnx_rx_trap_v1_etpp_config_t_init(unit, &etpp_config);
    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_config_from_config_fill(unit, trap_type, config_p, &etpp_config));
    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_set_verify(unit, trap_class, trap_type, &etpp_config));

    /** etpp trap action. In HW 0 = high priority, 1 = low priority, so we invert the given value */
    etpp_config.action.is_recycle_high_priority = (etpp_config.action.is_recycle_high_priority == FALSE);

    switch (trap_class)
    {
        case DNX_RX_TRAP_CLASS_USER_DEFINED:
            if (trap_type == DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_DROP)
            {
                SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_drop_hw_set(unit, &etpp_config.action));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_user_defined_set(unit, trap_type, FALSE, &etpp_config));
            }
            break;
        case DNX_RX_TRAP_CLASS_OAM:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_GET(trap_type);
            SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_oam_set(unit, oam_trap_type, &etpp_config));
            /** In order to improve the rx performance, store the trap ids of oam up mep destination 1 and 2 to swstate for rx system parser. */
            switch (oam_trap_type)
            {
                case DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1:
                    dnx_rx_parser_info.etpp_oam_up_mep_dest_trap_id1.set(unit,
                                                                         etpp_config.
                                                                         recycle_cmd_config.ingress_trap_id);
                    break;
                case DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2:
                    dnx_rx_parser_info.etpp_oam_up_mep_dest_trap_id2.set(unit,
                                                                         etpp_config.
                                                                         recycle_cmd_config.ingress_trap_id);
                    break;
                default:
                    break;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal trap_class=%d\n", trap_class);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_get(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type,
    bcm_rx_trap_config_t * config_p)
{
    int oam_trap_type;
    dnx_rx_trap_etpp_config_t etpp_config;
    SHR_FUNC_INIT_VARS(unit);

    dnx_rx_trap_v1_etpp_config_t_init(unit, &etpp_config);

    switch (trap_class)
    {
        case DNX_RX_TRAP_CLASS_USER_DEFINED:
            switch (trap_type)
            {
                case DNX_RX_TRAP_ID_TYPE_GET(BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT):
                    break;
                case DNX_RX_TRAP_ID_TYPE_GET(BCM_RX_TRAP_EG_TX_TRAP_ID_DROP):
                    etpp_config.action.is_pkt_drop = TRUE;
                    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_drop_hw_get(unit, &(etpp_config.action)));
                    break;
                default:
                    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_user_defined_action_hw_get(unit, trap_type,
                                                                                   &(etpp_config.action)));
                    SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_hw_get(unit, DBAL_CORE_DEFAULT,
                                                                        etpp_config.action.recycle_cmd,
                                                                        &(etpp_config.recycle_cmd_config)));
            }
            break;
        case DNX_RX_TRAP_CLASS_OAM:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_GET(trap_type);
            SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_oam_action_hw_get(unit, oam_trap_type, &(etpp_config.action)));
            SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_hw_get
                            (unit, DBAL_CORE_DEFAULT, etpp_config.action.recycle_cmd,
                             &(etpp_config.recycle_cmd_config)));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal trap_class=%d\n", trap_class);
            break;
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_config_from_etpp_config_fill(unit, &etpp_config, config_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_destroy(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type)
{
    dnx_rx_trap_etpp_config_t etpp_config;
    int ref_count;
    int oam_trap_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_config_t_init(unit, &etpp_config));

    switch (trap_class)
    {
        case DNX_RX_TRAP_CLASS_USER_DEFINED:
            /** Check if no traps are associated with the provided ERPP profile */
            SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_user_defined_profile_ref_count_get(unit, trap_type, &ref_count));
            if (ref_count != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "There are multiple trap types set to the same Trap ID."
                             "Clear the trap types before deleting the Trap ID \n");
            }
            SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_user_defined_set(unit, trap_type, TRUE, &etpp_config));
            SHR_IF_ERR_EXIT(dnx_algo_rx_trap_id_etpp_deallocate(unit, trap_type));
            if (trap_type == DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_VISIBILITY)
            {
                SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_visibility_enable_hw_set(unit, FALSE));
            }
            break;
        case DNX_RX_TRAP_CLASS_OAM:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_GET(trap_type);
            SHR_IF_ERR_EXIT(dnx_rx_trap_v1_etpp_oam_set(unit, oam_trap_type, &etpp_config));
            SHR_IF_ERR_EXIT(dnx_algo_rx_trap_id_etpp_oam_deallocate(unit, oam_trap_type));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal trap_class=%d\n", trap_class);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_action_profile_hw_set(
    int unit,
    int trap_id,
    dnx_rx_trap_action_profile_t * action_profile_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_ACTION_PROFILE, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_TRAP_ID, trap_id);

    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ACTION_PROFILE, INST_SINGLE,
                                 action_profile_p->profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE,
                                 action_profile_p->fwd_strength);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE,
                                 action_profile_p->snp_strength);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_action_profile_hw_get(
    int unit,
    int trap_id,
    dnx_rx_trap_action_profile_t * action_profile_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_ACTION_PROFILE, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_TRAP_ID, trap_id);

    /*
     * Set the values
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_ACTION_PROFILE, INST_SINGLE,
                               (uint32 *) &action_profile_p->profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE,
                               (uint32 *) &action_profile_p->fwd_strength);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE,
                               (uint32 *) &action_profile_p->snp_strength);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_etpp_action_profile_hw_clear(
    int unit,
    int dbal_trap_id)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_ACTION_PROFILE, &entry_handle_id));

    /*
     * Keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_TRAP_ID, dbal_trap_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_mtu_profile_hw_set(
    int unit,
    bcm_rx_mtu_profile_key_t * mtu_key_p,
    bcm_rx_mtu_profile_value_t * mtu_data_p)
{
    uint32 entry_handle_id;
    int trap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_MTU_MAP, &entry_handle_id));

    /** Keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_COMPRESSED, mtu_key_p->cmp_layer_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, mtu_key_p->mtu_profile);

    /** Values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_VALUE, INST_SINGLE, mtu_data_p->mtu_val);
    trap_id = BCM_GPORT_TRAP_GET_ID(mtu_data_p->trap_gport);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ACTION_PROFILE, INST_SINGLE,
                                DNX_RX_TRAP_EGRESS_ACTION_PROFILE_GET(trap_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE,
                                 BCM_GPORT_TRAP_GET_STRENGTH(mtu_data_p->trap_gport));
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE,
                                BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(mtu_data_p->trap_gport));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_mtu_profile_hw_get(
    int unit,
    bcm_rx_mtu_profile_key_t * mtu_key_p,
    bcm_rx_mtu_profile_value_t * mtu_data_p)
{
    uint32 entry_handle_id;
    int trap_id, fwd_act_profile, trap_strength, snoop_strength;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_MTU_MAP, &entry_handle_id));

    /** Keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_COMPRESSED, mtu_key_p->cmp_layer_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, mtu_key_p->mtu_profile);

    /** Values */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MTU_VALUE, INST_SINGLE, &(mtu_data_p->mtu_val));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_ACTION_PROFILE, INST_SINGLE,
                               (uint32 *) &fwd_act_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE,
                               (uint32 *) &trap_strength);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE,
                               (uint32 *) &snoop_strength);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED, fwd_act_profile);
    BCM_GPORT_TRAP_SET(mtu_data_p->trap_gport, trap_id, trap_strength, snoop_strength);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_mtu_profile_hw_clear(
    int unit,
    bcm_rx_mtu_profile_key_t * mtu_key_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_TRAP_MTU_MAP, &entry_handle_id));

    /** Keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, mtu_key_p->mtu_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_COMPRESSED, mtu_key_p->cmp_layer_type);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_mtu_lif_set(
    int unit,
    bcm_rx_mtu_config_t * mtu_config_p)
{

    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, mtu_config_p->gport,
                                                                   DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                                   &gport_hw_resources), _SHR_E_NOT_FOUND,
                                _SHR_E_PARAM);

    /**
     * Set the MTU profile to the OUT_LIF - MTU_PROFILE field in the MDB
     */
    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_mtu_profile_hw_set(unit,
                                                       gport_hw_resources.outlif_dbal_table_id,
                                                       DBAL_FIELD_OUT_LIF,
                                                       gport_hw_resources.outlif_dbal_result_type,
                                                       gport_hw_resources.local_out_lif, mtu_config_p->mtu_profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_mtu_lif_get(
    int unit,
    bcm_rx_mtu_config_t * mtu_config_p)
{
    int mtu_profile;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);

    /** Retrieve the DBAL table and OUT_LIF for the given GPORT */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, mtu_config_p->gport,
                                                                   DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                                   &gport_hw_resources), _SHR_E_NOT_FOUND,
                                _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_mtu_profile_hw_get(unit,
                                                       gport_hw_resources.outlif_dbal_table_id,
                                                       DBAL_FIELD_OUT_LIF,
                                                       gport_hw_resources.outlif_dbal_result_type,
                                                       gport_hw_resources.local_out_lif, &mtu_profile));
    mtu_config_p->mtu_profile = mtu_profile;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_lif_ingress_profile_set(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t action_gport)
{
    uint8 is_first = 0, is_last = 0;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    dnx_rx_trap_action_profile_t trap_action_profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_lif_ingress_profile_hw_get(unit, lif_hw_resources_p, &old_lif_trap_profile));

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.exchange
                    (unit, 0, &action_gport,
                     old_lif_trap_profile, NULL, (int *) &new_lif_trap_profile, &is_first, &is_last));

    /** If this is the last trap pointing to this lif trap profile, clear profile from HW */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_clear(unit, old_lif_trap_profile));
    }

    /** If this is the first time the lif trap profile is allocated, set it in HW */
    if (is_first)
    {
        trap_action_profile.profile = BCM_GPORT_TRAP_GET_ID(action_gport);
        trap_action_profile.fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(action_gport);
        trap_action_profile.snp_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport);

        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_set(unit, new_lif_trap_profile,
                                                                      &trap_action_profile));
    }

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_lif_ingress_profile_hw_set(unit, lif_hw_resources_p, new_lif_trap_profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_lif_ingress_profile_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t * action_gport_p)
{
    uint32 lif_trap_profile = 0;
    int ref_count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_lif_ingress_profile_hw_get(unit, lif_hw_resources_p, &lif_trap_profile));

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get(unit, lif_trap_profile, &ref_count, action_gport_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_lif_etpp_profile_lif_set(
    int unit,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t action_gport)
{
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_lif_etpp_profile_lif_hw_get
                    (unit, lif_gport, lif_hw_resources_p, &old_lif_trap_profile));

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                    (unit, old_lif_trap_profile, action_gport, &new_lif_trap_profile));

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_lif_etpp_profile_lif_hw_set
                    (unit, lif_gport, lif_hw_resources_p, new_lif_trap_profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See rx_trap_v1.h for more info
 */
shr_error_e
dnx_rx_trap_v1_lif_etpp_profile_lif_get(
    int unit,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t * action_gport_p)
{
    uint32 lif_trap_profile = 0;
    int ref_count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_v1_lif_etpp_profile_lif_hw_get(unit, lif_gport, lif_hw_resources_p, &lif_trap_profile));

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get(unit, lif_trap_profile, &ref_count, action_gport_p));

exit:
    SHR_FUNC_EXIT;
}

#endif /** #ifdef BCM_DNX2_SUPPORT */
