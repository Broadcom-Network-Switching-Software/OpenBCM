/*
 * Configuration example start:
 *
  cint ../../../src/examples/dnx/field/cint_field_qual_bypass_filter.c
  cint ../../../../src/examples/dnx/field/cint_field_qual_bypass_filter.c
  cint;
  int unit = 0;
  bcm_field_context_t epmf_ctx_id = 0;
  cint_field_qual_bypass_filter_main(unit, epmf_ctx_id);
 *
 * Configuration example end
 *   This CINT creates Bypass Filtering by setting a trap with max strength (0xf) in Ingress
 *   and detect the trap by creating field group with RxTrapStrength qualifier for stage EPMF.
 *   That way we can qualify on BypassFilter by qualify that trap strength is max.
 *
 *   CINT flow:
 *    1. Create field group for EPMF with trap strength qual.
 *    2. Create and set trap with max strength of 0xf.
 */

 /**
  * Global variables
  */
bcm_field_group_t cint_field_qual_bypass_filter_fg_id;
bcm_field_entry_t cint_field_qual_bypass_filter_entry_id;
uint32 cint_field_qual_bypass_filter_tc_action_value = 0x4;
int cint_field_qual_bypass_filter_trap_id;

 /**
 * \brief
 *  This function runs the external main function
 * \param [in] unit           - Device ID.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_qual_bypass_filter_main(
        int unit,
        int epmf_ctx_id)
{
    /**
     * Create field group
     */
    BCM_IF_ERROR_RETURN(cint_field_qual_bypass_filter_add_fg(unit, epmf_ctx_id));


    /**
     * Create and set 'SA equals DA' trap in ingress with max strength (0xf) to bypass all the filtering in egress pipe
     */
    BCM_IF_ERROR_RETURN(cint_field_qual_bypass_filter_set_trap(unit));


    return BCM_E_NONE;
}

/**
* \brief
*  This function sets trap in ingress
* \param [in] unit           - Device ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_qual_bypass_filter_set_trap(
       int unit)
{
    bcm_rx_trap_config_t trap_config;

    bcm_rx_trap_config_t_init(&trap_config);

    /* Set trap to max strength */
    trap_config.trap_strength = 0xf;
    /* disable link layer filters in Egress */
    trap_config.flags = BCM_RX_TRAP_BYPASS_FILTERS;

    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaEqualsDa, &cint_field_qual_bypass_filter_trap_id));

    printf("Trap %d was created. \n", cint_field_qual_bypass_filter_trap_id);

    BCM_IF_ERROR_RETURN(bcm_rx_trap_set(unit, cint_field_qual_bypass_filter_trap_id, trap_config));

    printf("Trap %d was set. \n", cint_field_qual_bypass_filter_trap_id);

    return BCM_E_NONE;
}
/**
* \brief
*  This function creates field group in EPMF, attaches it, and adds entry
* \param [in] unit           - Device ID.
* \param [in] epmf_ctx_id    - EPMF Context ID.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_qual_bypass_filter_add_fg(
       int unit,
       int epmf_ctx_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_quals = 1;
    /*
     * Set field group qualifier to RxTrapStrength to track the strength of packets' traps
     */
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapStrength;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_field_qual_bypass_filter_fg_id));

    printf("Field Group %d was created. \n", cint_field_qual_bypass_filter_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_field_qual_bypass_filter_fg_id, epmf_ctx_id, &attach_info));

    printf("Field Group %d was attached to context %d. \n", cint_field_qual_bypass_filter_fg_id, epmf_ctx_id);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    /*
     * Add entry with max trap strength in EPMF, to detect the trap we set and get bypass filtering.
     */
    entry_info.entry_qual[0].value[0] = 0x7;
    entry_info.entry_qual[0].mask[0] =  -1;

    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = cint_field_qual_bypass_filter_tc_action_value;

    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_field_qual_bypass_filter_fg_id, &entry_info, cint_field_qual_bypass_filter_entry_id));

    printf("Entry 0x%X (%d) was added to field group %d. \n", cint_field_qual_bypass_filter_entry_id, cint_field_qual_bypass_filter_entry_id, cint_field_qual_bypass_filter_fg_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function destroys all allocated data by the configuration
 *
 * \param [in] unit            - Device id
 * \param [in] epmf_ctx_id     - EPMF Context ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_qual_bypass_filter_destroy(
        int unit,
        int epmf_ctx_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_field_qual_bypass_filter_fg_id, NULL, cint_field_qual_bypass_filter_entry_id));

    printf("Entry 0x%X (%d) was deleted. \n", cint_field_qual_bypass_filter_entry_id, cint_field_qual_bypass_filter_entry_id);

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_field_qual_bypass_filter_fg_id, epmf_ctx_id));

    printf("Field Group %d was detach from context %d. \n", cint_field_qual_bypass_filter_fg_id, epmf_ctx_id);

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_field_qual_bypass_filter_fg_id));

    printf("Field Group %d was deleted. \n", cint_field_qual_bypass_filter_fg_id);

    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_destroy(unit, cint_field_qual_bypass_filter_trap_id));

    printf("Trap %d was deleted. \n", cint_field_qual_bypass_filter_trap_id);

    return BCM_E_NONE;
}
