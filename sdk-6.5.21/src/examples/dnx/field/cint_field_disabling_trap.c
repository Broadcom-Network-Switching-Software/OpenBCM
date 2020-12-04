/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_disabling_trap.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t ipmf1_ctx_id = BCM_FIELD_CONTEXT_DEFAULT;
 * int trap_id = 0;
 * cint_field_disabling_trap_set_trap(unit, &trap_id);
 * int default_trap_id = 0;
 * cint_field_disabling_trap_main(unit, ipmf1_ctx_id, &default_trap_id);
 *
 * Configuration example end
 *   This CINT disables a created trap on a specific context, by setting a trap in Ingress
 *   and change it by creating field group with Trap action for stage IPMF1.
 *   That way we can overwrite any trap that was set with a default trap.
 *
 * Configuration steps:
 * 1. Create and set trap of source MAC address equals to destination MAC address.
 * 2. Create field group for IPMF1 with trap qual and trap action.
 */

 /**
  * Global variables
  */
bcm_field_context_t cint_field_disabling_trap_ctx_id;
bcm_field_group_t cint_field_disabling_trap_fg_id;
bcm_field_entry_t cint_field_disabling_trap_entry_id;

int cint_field_disabling_trap_trap_id;

/**
* \brief
*  This function sets trap in ingress
* \param [in] unit           - Device ID.
* \param [out] trap_id       - trap ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_disabling_trap_set_trap(
       int unit,
       int *trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    bcm_rx_trap_config_t_init(&trap_config);

    /* Set trap strength */
    trap_config.trap_strength = 0x3;
    /* Change dest port for trap - to DROP the packet */
    trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;

    /* Create the trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaEqualsDa, &cint_field_disabling_trap_trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_create\n");
        return rv;
    }
    printf("Trap %d was created. \n", cint_field_disabling_trap_trap_id);

    /* Set the trap */
    rv = bcm_rx_trap_set(unit, cint_field_disabling_trap_trap_id, trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    }
    printf("Trap %d was set. \n", cint_field_disabling_trap_trap_id);

    *trap_id = cint_field_disabling_trap_trap_id;
    return rv;
}

/**
* \brief
*  This function creates field group in IPMF1, attaches it, and adds entry
* \param [in] unit              - Device ID.
* \param [out] default_trap_id  - Default trap ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_disabling_trap_add_fg(
       int unit,
       int *default_trap_id)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    bcm_field_group_info_t_init(&fg_info);
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "DISABLING_TRAP", sizeof(fg_info.name));
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_quals = 1;
    /* Qualify on RX Trap Code */
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
    fg_info.nof_actions = 1;
    /* Set Trap code action */
    fg_info.action_types[0] = bcmFieldActionTrap;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_disabling_trap_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Field Group %d was created. \n", cint_field_disabling_trap_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    rv = bcm_field_group_context_attach(unit, 0, cint_field_disabling_trap_fg_id, cint_field_disabling_trap_ctx_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group %d was attached to context %d. \n", cint_field_disabling_trap_fg_id, cint_field_disabling_trap_ctx_id);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    /* Created trap id */
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = cint_field_disabling_trap_trap_id;
    /* If mask is set to 0, this example would apply to all traps with lower strength */
    entry_info.entry_qual[0].mask[0] = 0x1ff;

    /* Configure default trap - a trap that does nothing */
    bcm_gport_t trap_gport;
    /* Set default trap with strength bigger than the created trap (0x3)
     * Trap strength behaves differently per device,
     * In some devices, the strength of the action trap doesn't has to be bigger than the configured trap, in order to overwrite it */
    int trap_strength = 0x8;
    bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, default_trap_id);
    BCM_GPORT_TRAP_SET(trap_gport, *default_trap_id, trap_strength, 0);

    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport;

    rv = bcm_field_entry_add(unit, 0, cint_field_disabling_trap_fg_id, &entry_info, cint_field_disabling_trap_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry 0x%X (%d) was added to field group %d. \n", cint_field_disabling_trap_entry_id, cint_field_disabling_trap_entry_id, cint_field_disabling_trap_fg_id);

    return rv;
}

/**
* \brief
*  This function runs the external main function
* \param [in] unit              - Device ID.
* \param [in] ipmf1_ctx_id      - IPMF1 Context ID.
* \param [out] default_trap_id  - Default trap ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_disabling_trap_main(
       int unit,
       int ipmf1_ctx_id,
       int *default_trap_id)
{
   int rv = BCM_E_NONE;
   cint_field_disabling_trap_ctx_id = ipmf1_ctx_id;

   /**
    * Create field group
    */
   rv = cint_field_disabling_trap_add_fg(unit, default_trap_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error (%d), in cint_field_disabling_trap_add_fg\n", rv);
       return rv;
   }

   return rv;
}

/**
 * \brief
 *  This function destroys all allocated data by the configuration
 *
 * \param [in] unit            - Device id
 * \param [in] ipmf1_ctx_id    - IPMF1 Context ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_disabling_trap_destroy(
        int unit,
        int ipmf1_ctx_id)
{
    int rv = BCM_E_NONE;

    /*
    * Delete the entry from the field group
    */
    rv = bcm_field_entry_delete(unit, cint_field_disabling_trap_fg_id, NULL, cint_field_disabling_trap_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    printf("Entry 0x%X (%d) was deleted. \n", cint_field_disabling_trap_entry_id, cint_field_disabling_trap_entry_id);

    /*
     * Detach the FG from its context
     */
    rv = bcm_field_group_context_detach(unit, cint_field_disabling_trap_fg_id, ipmf1_ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    printf("Field Group %d was detach from context %d. \n", cint_field_disabling_trap_fg_id, ipmf1_ctx_id);

    /*
     * Delete the FG
     */
    rv = bcm_field_group_delete(unit, cint_field_disabling_trap_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    printf("Field Group %d was deleted. \n", cint_field_disabling_trap_fg_id);

    /*
     * Delete the trap
     */
    rv = bcm_rx_trap_type_destroy(unit, cint_field_disabling_trap_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy\n");
        return rv;
    }
    printf("Trap %d was deleted. \n", cint_field_disabling_trap_trap_id);

    return rv;
}
