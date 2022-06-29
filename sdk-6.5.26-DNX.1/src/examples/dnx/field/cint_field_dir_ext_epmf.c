 /*
 * Configuration example start:
 *
  cint ../../../src/examples/dnx/field/cint_field_dir_ext_epmf.c
  cint ../../../../src/examples/dnx/field/cint_field_dir_ext_epmf.c
  cint;
  int unit = 0;
  bcm_field_context_t egress_context_id = 0;
  cint_field_dir_ext_epmf_main(unit, egress_context_id);
 *
 * Configuration example end
 *
 *  This cint shows examples of using Direct Extraction field group in Egress PMF stage.
 *  Extracting the PCP values of the ETH1 header and updating the egress fwd action index.
 *
 */

bcm_field_group_t Cint_field_dir_ext_epmf_fg_id;

/**
* \brief
*   Checking if the dnx_data feature dir_ext_epmf is set.
*
* \param [in] unit    - Device ID.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_dir_ext_epmf_init(
    int unit)
{
    int dnx_data_dir_ext_epmf_feature_value;

    dnx_data_dir_ext_epmf_feature_value = *(dnxc_data_get(unit, "field", "features", "dir_ext_epmf", NULL));

    if (dnx_data_dir_ext_epmf_feature_value == 0)
    {
        printf("Direct Extraction on ePMF not supported on this device (supported for J2C and above devices)! \n");
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/**
* \brief
*   Configure field group, attaching it to a context.
*
* \param [in] unit    - Device ID.
* \param [in] context_id  -  PMF context Id.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_dir_ext_epmf_main(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;

    /** Check if DNXDATA feature 'dir_ext_epmf' is enabled. */
    cint_field_dir_ext_epmf_init(unit);

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageEgress;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "dir_ext_epmf", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = bcmFieldQualifyVlanPri;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;

    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[0] = FALSE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /** Create the field group. */
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &Cint_field_dir_ext_epmf_fg_id));

    printf("Field Group ID %d was created. \n", Cint_field_dir_ext_epmf_fg_id);

    
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, Cint_field_dir_ext_epmf_fg_id, context_id, &attach_info));

    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_dir_ext_epmf_fg_id, context_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit   - Device id
 * \param [in] context_id  -  PMF context Id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_dir_ext_epmf_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_dir_ext_epmf_fg_id, context_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, Cint_field_dir_ext_epmf_fg_id));

    return BCM_E_NONE;
}
