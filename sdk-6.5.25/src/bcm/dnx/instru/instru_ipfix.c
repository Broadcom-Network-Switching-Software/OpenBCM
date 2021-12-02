/** \file instru_ipfix.c
 * 
 *
 * Procedures for DNX in IPFIX module.
 *
 * This file contains functions for Instrumantation - IPFIX configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>

#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm/trunk.h>
#include <include/bcm_int/dnx/auto_generated/dnx_trunk_dispatch.h>
#include <bcm/types.h>
#include <src/bcm/dnx/trunk/trunk_sw_db.h>
#include <src/bcm/dnx/trunk/trunk_utils.h>

#include <bcm_int/dnx/lif/lif_lib.h>

/*
 * }
 */

/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */

/**
 *  flags that may be used when creating IPFIX
 */
#define IPFIX_CREATE_SUPPORTED_FLAGS (BCM_INSTRU_IPFIX_ENCAP_WITH_ID | BCM_INSTRU_IPFIX_ENCAP_REPLACE)

/**
 *  Macro - find if a has any bits with a value of "1" that b
 *  has with a value of "0"
 */
#define CHECK_FLAGS_INVERTED_MASK(a, b) ((a & ~b) != 0)

/*
  * }
  */

 /*
  * Internal functions.
  * {
  */

/**
 * \brief - Set Virtual Register for IPFIX
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] field - dbal field of the virtual register
 * \param [in] value - Value for the virtual register to be set
 *
 * \return
 *   shr_error_e
 *
 * \see
 */
static shr_error_e
dnx_instru_ipfix_virtual_register_set(
    int unit,
    dbal_fields_e field,
    int value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IPFIX, &entry_handle_id));

    /** Set the sampling rate field. */
    dbal_entry_value_field32_set(unit, entry_handle_id, field, INST_SINGLE, value);

    /** Commit dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Virtual Register for IPFIX
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] field - dbal field of the virtual register
 * \param [out] value - Virtual register value
 *
 * \return
 *   shr_error_e
 *
 * \see
 */
static shr_error_e
dnx_instru_ipfix_virtual_register_get(
    int unit,
    dbal_fields_e field,
    int *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_IPFIX, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field, INST_SINGLE, (uint32 *) value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX system Up time virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] uptime -
 *   IPFIX System Up time
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_system_up_time_set(
    int unit,
    int uptime)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_set(unit, DBAL_FIELD_IPFIX_SETUP_TIME, uptime));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX eventor id virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] eventor_id -
 *   IPFIX eventor rx context id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_eventor_id_set(
    int unit,
    int eventor_id)
{
    int nof_eventor_rx_contexts;

    SHR_FUNC_INIT_VARS(unit);

    nof_eventor_rx_contexts = dnx_data_instru.eventor.nof_contexts_get(unit);

    if ((eventor_id < 0) || (eventor_id >= nof_eventor_rx_contexts))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Eventor ID = %d is out of range [0:%d]!\n", eventor_id,
                     nof_eventor_rx_contexts - 1);
    }

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_set(unit, DBAL_FIELD_IPFIX_EVENTOR_HEADER, eventor_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX template id virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] template_id -
 *   IPFIX template id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_template_id_set(
    int unit,
    int template_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (template_id > 0xffff)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPFIX Template ID = %d is out of range [0:%d]!\n", template_id, 0xffff);
    }

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_set(unit, DBAL_FIELD_IPFIX_TEMPLATE_ID, template_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX tx_time virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] tx_time -
 *   IPFIX packet export time
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_tx_time_set(
    int unit,
    int tx_time)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_set(unit, DBAL_FIELD_IPFIX_TX_TIME, tx_time));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX Observation_Domain virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] observation_domain -
 *   IPFIX Observation Domain
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_observation_domain_set(
    int unit,
    int observation_domain)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_set
                    (unit, DBAL_FIELD_IPFIX_OBSERVATION_DOMAIN, observation_domain));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix uptime virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] uptime -
 *  Pointer that will be filled with the uptime of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_system_up_time_get(
    int unit,
    int *uptime)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_get(unit, DBAL_FIELD_IPFIX_SETUP_TIME, uptime));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix eventor id virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] eventor_id -
 *  Pointer that will be filled with the eventor_id of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_eventor_id_get(
    int unit,
    int *eventor_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_get(unit, DBAL_FIELD_IPFIX_EVENTOR_HEADER, eventor_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix template id virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] template_id -
 *  Pointer that will be filled with the template id of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_template_id_get(
    int unit,
    int *template_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_get(unit, DBAL_FIELD_IPFIX_TEMPLATE_ID, template_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix tx time virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] tx_time -
 *  Pointer that will be filled with the tx time of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_tx_time_get(
    int unit,
    int *tx_time)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_get(unit, DBAL_FIELD_IPFIX_TX_TIME, tx_time));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix observation domain virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [out] observation_domain -
 *   IPFIX Observation Domain
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e
dnx_instru_ipfix_observation_domain_get(
    int unit,
    int *observation_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_virtual_register_get
                    (unit, DBAL_FIELD_IPFIX_OBSERVATION_DOMAIN, observation_domain));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify ipfix encap is indeed exists and it is with the correct table/result_type/phase
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] ipfix_encap_id - ipfix global lif
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
static shr_error_e
dnx_instru_ipfix_encap_id_verify(
    int unit,
    int ipfix_encap_id)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;
    dbal_result_type_t nof_result_types_eedb_ipfix;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Turn the global lif into a tunnel, then call gport to hw resources.
     */
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, ipfix_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get
                    (unit, DBAL_TABLE_OLP_HEADER_CONFIGURATION, (int *) &nof_result_types_eedb_ipfix));

    if ((hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_IPFIX_CODE)
        || (hw_res.outlif_dbal_result_type >= nof_result_types_eedb_ipfix)
        || (hw_res.logical_phase != DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! IPFIX encap 0x%x is not of type IPFIX, table_id = %s, result_type = %d, logical_phase = %d\n",
                     ipfix_encap_id, dbal_logical_table_to_string(unit, hw_res.outlif_dbal_table_id),
                     hw_res.outlif_dbal_result_type, hw_res.logical_phase);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify IPFIX entity create
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_encap_info - Information of ipfix entity
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_encap_create
 */
shr_error_e
dnx_instru_ipfix_encap_create_verify(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipfix_encap_info, _SHR_E_PARAM, "ipfix_encap_info");
    if (CHECK_FLAGS_INVERTED_MASK(ipfix_encap_info->flags, IPFIX_CREATE_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: some flags are not supported for IPFIX Create;\n"
                     "the value 0x%08X has bits that fall outside "
                     "the supported flags mask, 0x%08X\n", ipfix_encap_info->flags, IPFIX_CREATE_SUPPORTED_FLAGS);
    }

    /** Verify replace case */
    if ((_SHR_IS_FLAG_SET(ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_REPLACE))
        || (_SHR_IS_FLAG_SET(ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_WITH_ID)))
    {
        /** Verify that the flag WITH_ID is set */
        if (!(_SHR_IS_FLAG_SET(ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! flags = 0x%08X. flag BCM_INSTRU_IPFIX_ENCAP_REPLACE (0x%08X) is set - flag BCM_INSTRU_IPFIX_ENCAP_WITH_ID (0x%08X) must be set as well!\n",
                         ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_REPLACE, BCM_INSTRU_IPFIX_ENCAP_WITH_ID);
        }

        SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_id_verify(unit, ipfix_encap_info->ipfix_encap_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Given an instru global lif, returns the local lif and it's result type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_encap_id - global lif
 * \param [out] local_lif - returned local lif
 * \param [out] result_type - returned result type
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_encap_create
 */
static shr_error_e
dnx_instru_ipfix_encap_id_to_local_lif(
    int unit,
    int ipfix_encap_id,
    int *local_lif,
    uint32 *result_type)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Turn the global lif into a tunnel, then call gport to hw resources.
     */
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, ipfix_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    *local_lif = hw_res.local_out_lif;
    *result_type = hw_res.outlif_dbal_result_type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate an ipfix encap lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] ipfix_encap_info - - Information of ipfix entity
 * \param [out] ipfix_local_outlif - Allocated local encap lif
 * \param [in] dbal_result_type - EEDB result_type.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_encap_allocate(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info,
    int *ipfix_local_outlif,
    uint32 dbal_result_type)
{
    int lif_alloc_flags = 0;
    lif_mngr_local_outlif_info_t outlif_info;
    lif_mngr_global_lif_info_t global_lif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If we were given the replace flag, just translate the ipfix encap to local lif and we're done.
     * The lif's existence has already been verified.
     */
    if (_SHR_IS_FLAG_SET(ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_REPLACE))
    {
        /** Get local lif from algo gpm. */
        uint32 result_type;

        SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_id_to_local_lif
                        (unit, ipfix_encap_info->ipfix_encap_id, ipfix_local_outlif, &result_type));
        SHR_EXIT();
    }

    /*
     * If it's not replace, then allocate the ipfix encap entry.
     */
    if (_SHR_IS_FLAG_SET(ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_WITH_ID))
    {
        /*
         * If WITH_ID flag is used - get the global out-lif from the user input and
         * add alloc_with_id flag
         */
        global_lif_info.global_lif = ipfix_encap_info->ipfix_encap_id;
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_IPFIX_CODE;
    outlif_info.dbal_result_type = dbal_result_type;
    outlif_info.logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_info, NULL, &outlif_info));

    /*
     * Return the allocated global and local lifs.
     */
    ipfix_encap_info->ipfix_encap_id = global_lif_info.global_lif;
    *ipfix_local_outlif = outlif_info.local_outlif;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ipfix encap lif
 *          Set eedb raw data to map lif to fwd_code
 *
 * \param [in] unit - Relevant unit
 * \param [in] ipfix_local_outlif - Local ipfix encap lif
 * \param [in] dbal_result_type - the EEDB IPFIX result type.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_encap_write(
    int unit,
    int ipfix_local_outlif,
    uint32 dbal_result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPFIX_CODE, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, ipfix_local_outlif);

    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPFIX_RX_FWD_CODE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the fwd_code associated to eedb raw data lif
 *
 * \param [in] unit - Relevant unit
 * \param [in] ipfix_local_outlif - Local ipfix encap lif
 * \param [in] dbal_result_type - the EEDB IPFIX result type.
 * \param [out] fwd_code - forward code associated to the ipfix lif
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_encap_get(
    int unit,
    int ipfix_local_outlif,
    uint32 dbal_result_type,
    uint32 *fwd_code)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPFIX_CODE, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, ipfix_local_outlif);

    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD_CODE, INST_SINGLE, fwd_code);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get flags of ipfix_encap_info according to
 *          the fwd_code
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] ipfix_encap_info - - Information of ipfix entity
 * \param [in] fwd_code - forward code associated to the ipfix lif
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_encap_get_flags(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info,
    uint32 fwd_code)
{

    SHR_FUNC_INIT_VARS(unit);

    ipfix_encap_info->flags = 0;
    if (fwd_code == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPFIX_RX_FWD_CODE)
    {
        ipfix_encap_info->flags |= 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPFIX entry of not supported type is allocated.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create ipfix encap lif
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] ipfix_encap_info - - Information of ipfix entity
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_encap_create(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info)
{
    int ipfix_local_outlif = 0;
    uint32 result_type = DBAL_RESULT_TYPE_EEDB_IPFIX_CODE_ETPS_DATA_128B;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate IPFIX EEDB resource
     */
    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_allocate(unit, ipfix_encap_info, &ipfix_local_outlif, result_type));

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_write(unit, ipfix_local_outlif, result_type));

    if (!_SHR_IS_FLAG_SET(ipfix_encap_info->flags, BCM_INSTRU_IPFIX_ENCAP_REPLACE))
    {
        /**
         * No need to write to GLEM if we perform replace since the global and local lif don't change.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, ipfix_encap_info->ipfix_encap_id, ipfix_local_outlif, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
  * }
  */

/*
 * API functions.
 * {
 */

/**
 * \brief - IPFIX entity create
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_encap_info - Information of ipfix entity
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_encap_get
 *   bcm_dnx_instru_ipfix_encap_delete
 *   bcm_dnx_instru_ipfix_encap_traverse
 */
shr_error_e
bcm_dnx_instru_ipfix_encap_create(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verify ipfix encap create parameters 
     */
    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_create_verify(unit, ipfix_encap_info));

    /*
     * Allocate IPFIX EEDB resource
     */
    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_create(unit, ipfix_encap_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - IPFIX entity get
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] ipfix_encap_info - Information of ipfix entity
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_encap_create
 *   bcm_dnx_instru_ipfix_encap_delete
 *   bcm_dnx_instru_ipfix_encap_traverse
 */
shr_error_e
bcm_dnx_instru_ipfix_encap_get(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info)
{
    uint32 result_type;
    uint32 fwd_code;
    int ipfix_local_outlif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_id_verify(unit, ipfix_encap_info->ipfix_encap_id));

    /** Get local lif from algo gpm. */

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_id_to_local_lif
                    (unit, ipfix_encap_info->ipfix_encap_id, &ipfix_local_outlif, &result_type));

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_get(unit, ipfix_local_outlif, result_type, &fwd_code));

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_get_flags(unit, ipfix_encap_info, fwd_code));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify ipfix delete API params.
 */
static shr_error_e
dnx_instru_ipfix_encap_delete_verify(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipfix_encap_info, _SHR_E_PARAM, "ipfix_encap_info");

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_id_verify(unit, ipfix_encap_info->ipfix_encap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear ipfix EEDB entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] ipfix_local_outlif - the ipfix lif id, key to the dbal table
 * \param [in] result_type - the ipfix EEDB entry result type
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_encap_clear(
    int unit,
    int ipfix_local_outlif,
    uint32 result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPFIX_CODE, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, ipfix_local_outlif);
    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    /** Clear dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete IPFIX entity
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] ipfix_encap_info - Information of ipfix entity
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_encap_create
 *   bcm_dnx_instru_ipfix_encap_delete
 *   bcm_dnx_instru_ipfix_encap_traverse
 */
shr_error_e
bcm_dnx_instru_ipfix_encap_delete(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info)
{
    int local_lif;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_ipfix_encap_delete_verify(unit, ipfix_encap_info));

    /**
     * Clear HW.
     */
    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_id_to_local_lif
                    (unit, ipfix_encap_info->ipfix_encap_id, &local_lif, &result_type));

    /** Remove global lif from GLEM */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, ipfix_encap_info->ipfix_encap_id));

    SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_clear(unit, local_lif, result_type));

    /*
     * Free IPFIX encap outlif - Delete global and local lifs.
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, ipfix_encap_info->ipfix_encap_id, NULL, local_lif));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse all IPFIX entities
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] cb - Pointer to callback function
 * \param [in] user_data - Pointer to callback function
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_encap_create
 *   bcm_dnx_instru_ipfix_encap_delete
 *   bcm_dnx_instru_ipfix_encap_get
 */
shr_error_e
bcm_dnx_instru_ipfix_encap_traverse(
    int unit,
    bcm_instru_ipfix_encap_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end = 0;
    uint32 fwd_code = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPFIX_CODE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_instru_ipfix_encap_info_t ipfix_encap_info;
        uint32 result_type, local_out_lif;
        bcm_gport_t gport;

        sal_memset(&ipfix_encap_info, 0, sizeof(bcm_instru_ipfix_encap_info_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                            INST_SINGLE, &result_type));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, local_out_lif, &gport));

        ipfix_encap_info.ipfix_encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);

        /** Get IPFIX info */
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_get(unit, local_out_lif, result_type, &fwd_code));

        SHR_IF_ERR_EXIT(dnx_instru_ipfix_encap_get_flags(unit, &ipfix_encap_info, fwd_code));

        /*
         * If user provided a name for the callback function,
         * call it with passing the data from the found entry.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &ipfix_encap_info, user_data));
        }

        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Encode Source-System-Port from System-Port.
 *
 * \see
 *   dnx_instru_ipfix_interface_add_input
 */
static shr_error_e
dnx_instru_ipfix_interface_input_system_port_to_source_system_port(
    int unit,
    bcm_gport_t system_port,
    uint32 *source_system_port)
{
    bcm_trunk_t trunk_id;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * The system-port can be part of LAG
     */
    rv = bcm_dnx_trunk_find(unit, 0, system_port, &trunk_id);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        /** system port */
        *source_system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(system_port);
    }
    else
    {
        /*
         * The system port is part of LAG.
         * Need to decode it as SSPA.
         */
        SHR_IF_ERR_EXIT(dnx_trunk_system_port_to_spa_map_get(unit, system_port, trunk_id, source_system_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add IPFIX mapping  - ingress port to input interface and ingress-vrf.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_add_input(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 entry_handle_id;
    uint32 port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * The system-port can be part of LAG
     */
    SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_input_system_port_to_source_system_port
                    (unit, ipfix_interface_info->port, &port));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, port);

    /** Set VALUE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_PEM_ACCESS_1_DB_IPFIX_ESEM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPFIX_INTERFACE, INST_SINGLE,
                                 ipfix_interface_info->interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPFIX_VRF, INST_SINGLE,
                                 ipfix_interface_info->vrf_id);

    if (_SHR_IS_FLAG_SET(ipfix_interface_info->flags, BCM_INSTRU_IPFIX_INTERFACE_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get IPFIX mapping  - ingress port to input interface and ingress-vrf.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_get_input(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, ipfix_interface_info->port);

    /** Set VALUE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_PEM_ACCESS_1_DB_IPFIX_ESEM);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IPFIX_INTERFACE, INST_SINGLE,
                               &ipfix_interface_info->interface);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IPFIX_VRF, INST_SINGLE,
                               (uint32 *) (&ipfix_interface_info->vrf_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Remove IPFIX mapping  - ingress port to input interface and ingress-vrf.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_remove_input(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, ipfix_interface_info->port);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add IPFIX mapping  - egress port to output interface and egress-vrf.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_add_output(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 entry_handle_id;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    destination = ipfix_interface_info->port;

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, destination);

    /** Set VALUE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_PEM_ACCESS_2_DB_IPFIX_ESEM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPFIX_INTERFACE, INST_SINGLE,
                                 ipfix_interface_info->interface);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPFIX_VRF, INST_SINGLE,
                                 ipfix_interface_info->vrf_id);

    if (_SHR_IS_FLAG_SET(ipfix_interface_info->flags, BCM_INSTRU_IPFIX_INTERFACE_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Remove IPFIX mapping  - egress port to output interface and egress-vrf.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_remove_output(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, ipfix_interface_info->port);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get IPFIX mapping  - egress port to input interface and egress-vrf.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_get_output(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, ipfix_interface_info->port);

    /** Set VALUE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_PEM_ACCESS_2_DB_IPFIX_ESEM);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IPFIX_INTERFACE, INST_SINGLE,
                               &ipfix_interface_info->interface);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IPFIX_VRF, INST_SINGLE,
                               (uint32 *) (&ipfix_interface_info->vrf_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add IPFIX interface
 *          Adds port to interface mapping
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_interface_info - Information of port to interface mapping
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_interface_remove
 *   bcm_dnx_instru_ipfix_interface_get
 *   bcm_dnx_instru_ipfix_interface_traverse
 */
shr_error_e
bcm_dnx_instru_ipfix_interface_add(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_NULL_CHECK(ipfix_interface_info, _SHR_E_PARAM, "ipfix_interface_info");

    if ((ipfix_interface_info->flags & BCM_INSTRU_IPFIX_INTERFACE_INPUT) == BCM_INSTRU_IPFIX_INTERFACE_INPUT)
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_add_input(unit, ipfix_interface_info));
    }
    else
    {
        if ((ipfix_interface_info->flags & BCM_INSTRU_IPFIX_INTERFACE_OUTPUT) == BCM_INSTRU_IPFIX_INTERFACE_OUTPUT)
        {
            SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_add_output(unit, ipfix_interface_info));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! One of the flags BCM_INSTRU_IPFIX_INTERFACE_INPUT or BCM_INSTRU_IPFIX_INTERFACE_OUTPUT must be set\n");
        }
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify ipfix interface
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_interface_info - Information of port to interface mapping
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_interface_remove
 *   bcm_dnx_instru_ipfix_interface_add
 *   bcm_dnx_instru_ipfix_interface_traverse
 *   bcm_dnx_instru_ipfix_interface_get
 */
shr_error_e
dnx_instru_ipfix_interface_verify(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipfix_interface_info, _SHR_E_PARAM, "ipfix_interface_info");

    /*
     * Verify only supported flags are set
     */
    supported_flags = BCM_INSTRU_IPFIX_INTERFACE_INPUT | BCM_INSTRU_IPFIX_INTERFACE_OUTPUT;
    if (ipfix_interface_info->flags & (~supported_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flags = 0x%08x! Only 0x%08x are supported!\n", ipfix_interface_info->flags,
                     supported_flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get IPFIX interface
 *          Gets port to interface mapping
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] ipfix_interface_info - Information of port to interface mapping
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_interface_remove
 *   bcm_dnx_instru_ipfix_interface_add
 *   bcm_dnx_instru_ipfix_interface_traverse
 */
shr_error_e
bcm_dnx_instru_ipfix_interface_get(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_ipfix_interface_verify(unit, ipfix_interface_info));

    if (_SHR_IS_FLAG_SET(ipfix_interface_info->flags, BCM_INSTRU_IPFIX_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_get_input(unit, ipfix_interface_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_get_output(unit, ipfix_interface_info));
    }

    SHR_EXIT();
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove IPFIX interface
 *          Removes port to interface mapping
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_interface_info - Information of port to interface mapping
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_interface_get
 *   bcm_dnx_instru_ipfix_interface_add
 *   bcm_dnx_instru_ipfix_interface_traverse
 */
shr_error_e
bcm_dnx_instru_ipfix_interface_remove(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_ipfix_interface_verify(unit, ipfix_interface_info));

    if (_SHR_IS_FLAG_SET(ipfix_interface_info->flags, BCM_INSTRU_IPFIX_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_remove_input(unit, ipfix_interface_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_remove_output(unit, ipfix_interface_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse IPFIX mapping  - ingress port to ipfix interface.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_input_traverse(
    int unit,
    bcm_instru_ipfix_interface_traverse_info_t * ipfix_interface_traverse_info,
    bcm_instru_ipfix_interface_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_1_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        bcm_instru_ipfix_interface_info_t ipfix_interface_info;
        uint32 port, interface, vrf_id;

        ipfix_interface_info.flags = BCM_INSTRU_IPFIX_INTERFACE_INPUT;

        /*
         * Get port -table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, &port));

        /*
         * The key is SSPA (can be physical-port or LAG-member)
         */
        ipfix_interface_info.port = port;

        /*
         * Get interface value
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPFIX_INTERFACE, INST_SINGLE, &interface));
        ipfix_interface_info.interface = interface;

        /*
         * Get vrf_id value
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPFIX_VRF, INST_SINGLE, &vrf_id));
        ipfix_interface_info.vrf_id = vrf_id;

        /*
         * Run delete or callback function
         */
        SHR_IF_ERR_EXIT(cb(unit, &ipfix_interface_info, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse IPFIX mapping  - egress port to ipfix output interface.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ipfix_interface_output_traverse(
    int unit,
    bcm_instru_ipfix_interface_traverse_info_t * ipfix_interface_traverse_info,
    bcm_instru_ipfix_interface_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PEM_ACCESS_2_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        bcm_instru_ipfix_interface_info_t ipfix_interface_info;
        uint32 port, interface, vrf_id;

        ipfix_interface_info.flags = BCM_INSTRU_IPFIX_INTERFACE_OUTPUT;

        /*
         * Get port -table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, &port));

        /*
         * The key is SSPA (can be physical-port or LAG-member)
         */
        ipfix_interface_info.port = port;

        /*
         * Get interface value
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPFIX_INTERFACE, INST_SINGLE, &interface));
        ipfix_interface_info.interface = interface;

        /*
         * Get vrf_id value
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPFIX_VRF, INST_SINGLE, &vrf_id));
        ipfix_interface_info.vrf_id = vrf_id;

        /*
         * Run delete or callback function
         */
        SHR_IF_ERR_EXIT(cb(unit, &ipfix_interface_info, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse all IPFIX interfaces
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ipfix_interface_traverse_info - Pointer to ipfix traverse structure
 * \param [in] cb - Pointer to callback function
 * \param [in] user_data - Pointer to callback function
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   bcm_dnx_instru_ipfix_interface_create
 *   bcm_dnx_instru_ipfix_interface_delete
 *   bcm_dnx_instru_ipfix_interface_get
 */
shr_error_e
bcm_dnx_instru_ipfix_interface_traverse(
    int unit,
    bcm_instru_ipfix_interface_traverse_info_t * ipfix_interface_traverse_info,
    bcm_instru_ipfix_interface_traverse_cb cb,
    void *user_data)
{
    bcm_instru_ipfix_interface_info_t ipfix_interface_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    ipfix_interface_info.flags = ipfix_interface_traverse_info->flags;
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_ipfix_interface_verify(unit, &ipfix_interface_info));

    if (_SHR_IS_FLAG_SET(ipfix_interface_traverse_info->flags, BCM_INSTRU_IPFIX_INTERFACE_INPUT))
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_input_traverse(unit, ipfix_interface_traverse_info, cb, user_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipfix_interface_output_traverse(unit, ipfix_interface_traverse_info, cb, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
  * }
  */
