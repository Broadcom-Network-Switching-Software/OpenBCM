/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
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
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_ace.c
 * cint
 * int erpp_trap_id;
 * cint_rx_trap_erpp_ace_main(0, &erpp_trap_id);
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
    int rv = BCM_E_NONE;

    /** Configure the ACE format */
    bcm_field_ace_format_info_t_init(&ace_format_info);
 
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionTtlSet;

    /* We do not give the TTL action a valid bit.*/
    ace_format_info.action_with_valid_bit[0]=FALSE;

    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &cint_rx_trap_erpp_ace_format_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

    ace_entry_info.entry_action[0].type = bcmFieldActionTtlSet;
    ace_entry_info.entry_action[0].value[0] = 0x23;  

    rv = bcm_field_ace_entry_add(unit, 0, cint_rx_trap_erpp_ace_format_id, &ace_entry_info, &cint_rx_trap_erpp_ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    return rv;
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
    int rv = BCM_E_NONE, ingress_trap_id;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, erpp_trap_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create\n");
        return rv;
    }      

    rv = cint_rx_trap_erpp_ace_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_erpp_ace_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.encap_id = cint_rx_trap_erpp_ace_entry_handle;

    rv = bcm_rx_trap_set(unit, *erpp_trap_id_p, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_set\n");
        return rv;
    }     

    return rv;    
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
    int rv = BCM_E_NONE;
    
    rv = bcm_field_ace_entry_delete(unit, cint_rx_trap_erpp_ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_delete\n", rv);
        return rv;
    }   
  
    rv = bcm_field_ace_format_delete(unit, cint_rx_trap_erpp_ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_delete\n", rv);
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, erpp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error destroying erpp trap, in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    return rv;
}
