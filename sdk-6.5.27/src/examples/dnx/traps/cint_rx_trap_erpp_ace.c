/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_rx_trap_erpp_ace.c
 * Purpose: Example for creating an ACE format and setting it as ERPP trap action.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_ace.c
 * Main Function:
 *      cint_rx_trap_erpp_ace_main(unit, erpp_trap_id_p);
 * Destroy Function:
 *      cint_rx_trap_erpp_ace_destroy(unit,erpp_trap_id);
 *
 * Example Config:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_ace.c
  cint
  int erpp_trap_id;
  cint_rx_trap_erpp_ace_main(0, &erpp_trap_id);
 * Test Scenario - end
 */

bcm_field_ace_format_t cint_rx_trap_erpp_ace_format_id;
uint32 cint_rx_trap_erpp_ace_entry_handle;

/**
* \brief
*  Creates an ACE format and ACE entry.
* \param [in] unit             - Device ID
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_erpp_ace_create(int unit)
{
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;

    /** Configure the ACE format */
    bcm_field_ace_format_info_t_init(&ace_format_info);
 
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionTtlSet;

    /* We do not give the TTL action a valid bit.*/
    ace_format_info.action_with_valid_bit[0]=FALSE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_format_add(unit, 0, &ace_format_info, &cint_rx_trap_erpp_ace_format_id), "");

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

    ace_entry_info.entry_action[0].type = bcmFieldActionTtlSet;
    ace_entry_info.entry_action[0].value[0] = 0x23;  

    BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_entry_add(unit, 0, cint_rx_trap_erpp_ace_format_id, &ace_entry_info, &cint_rx_trap_erpp_ace_entry_handle), "");

    return BCM_E_NONE;
}

 /**
 * \brief  
 *  Main function for configuring ERPP trap id to overwrite ACE pointer
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 *   \param [in] erpp_trap_id_p - erpp trap id
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int cint_rx_trap_erpp_ace_main(
int unit,
int* erpp_trap_id_p)
{
    int ingress_trap_id;
    bcm_rx_trap_config_t trap_config;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, erpp_trap_id_p), "");

    BCM_IF_ERROR_RETURN_MSG(cint_rx_trap_erpp_ace_create(unit), "");

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.encap_id = cint_rx_trap_erpp_ace_entry_handle;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, *erpp_trap_id_p, &trap_config), "");

    return BCM_E_NONE;
}

/**
* \brief  
*  Destroy erpp trap ans ACE configuration
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] erpp_trap_id - ERPP trap id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int -  Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int cint_rx_trap_erpp_ace_destroy(
int unit,
int erpp_trap_id)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_entry_delete(unit, cint_rx_trap_erpp_ace_entry_handle), "");
  
    BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_format_delete(unit, cint_rx_trap_erpp_ace_format_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, erpp_trap_id), "");

    return BCM_E_NONE;
}
