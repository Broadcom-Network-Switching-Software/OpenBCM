/**
 *  EFES Polarity basic example.
 *
 *  Configuration example start:
 *
   cint ../../../src/examples/dnx/field/cint_field_efes_polarity_basic_example.c
   cint ../../../../src/examples/dnx/field/cint_field_efes_polarity_basic_example.c
   cint;
   int unit = 0;
   bcm_field_context_t context_id1 = 0;
   cint_field_efes_polarity_zero_basic_example_main(unit, context_id1);
 *
 *  Configuration example end
 *
 *  1. cint_field_efes_polarity_zero_basic_example_main(unit, context_id);
 *      This function sets polarity of an action to '0' for the given context.
 *
 *  2. cint_field_efes_different_polarities_main(unit, context_id1, context_id2);
 *      This function shows how to configure different polarities per the given
 *      contexts and attach them to a same field group.
 *      Polarity of the action will be set to '0' for the context_id1 and '1'
 *      for the context_id2.
 *
 *      Functions 1 and 2 are configuring 1 DE field group for the IPMF2 stage.
 *      Attach the FG to the given context/contexts. Creating 2 user qualifiers,
 *      using one of them as valid bit. The qualifiers are pointing to the
 *      EtherType field in the EHT header (bcmFieldInputTypeLayerAbsolute, arg=0, offset=96).
 */

/** Global variables to be passed through function. */
bcm_field_group_t Cint_field_efes_polarity_basic_example_fg_id;
bcm_field_qualify_t Cint_field_efes_polarity_basic_example_val_bit_qual_id;
bcm_field_qualify_t Cint_field_efes_polarity_basic_example_qual_id;

/**
 * \brief
 *  This function creates 2 user qualifiers, one for valid bit and
 *  one for action value.
 *
 * \param [in] unit   - Device id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_polarity_basic_example_qual_create(
    int unit)
{
    void *dest_char;
    bcm_field_qualifier_info_create_t qual_info;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "PolarityValBit", sizeof(qual_info.name));
    qual_info.name[sizeof(qual_info.name) - 1] = 0;
    qual_info.size = 1;
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_polarity_basic_example_val_bit_qual_id));
    printf("PolarityValBit created \n");

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "PolarityQual", sizeof(qual_info.name));
    qual_info.name[sizeof(qual_info.name) - 1] = 0;
    qual_info.size = 3;
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_efes_polarity_basic_example_qual_id));
    printf("PolarityQual created \n");
    return BCM_E_NONE;
}

/**
 * \brief
 *  This function creates a field group.
 *
 * \param [in] unit   - Device id
 * \param [out] fg_info  - FG info to be used in attach function.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_polarity_basic_example_fg_create(
    int unit,
    bcm_field_group_info_t *fg_info)
{
    void *dest_char;

    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info->stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "Polarity_basic", sizeof(fg_info->name));
    fg_info->nof_quals = 2;
    fg_info->nof_actions = 1;

    fg_info->qual_types[0] = Cint_field_efes_polarity_basic_example_val_bit_qual_id;
    fg_info->qual_types[1] = Cint_field_efes_polarity_basic_example_qual_id;

    fg_info->action_types[0] = bcmFieldActionPrioIntNew;
    fg_info->action_with_valid_bit[0] = TRUE;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, fg_info, &Cint_field_efes_polarity_basic_example_fg_id));

    printf("Field Group DE ID %d was created. iPMF2 stage\n", Cint_field_efes_polarity_basic_example_fg_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function attach a field group to the given context.
 *
 * \param [in] unit   - Device id
 * \param [in] context_id  - Context id.
 * \param [in] fg_info  - FG info.
 * \param [in] polarity  - Polarity of the action, can be '0' or '1'.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_polarity_basic_example_attach(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_group_info_t fg_info,
    int polarity)
{
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 96;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 96;

    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].valid_bit_polarity = polarity;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, Cint_field_efes_polarity_basic_example_fg_id, context_id, &attach_info));

    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_efes_polarity_basic_example_fg_id, context_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function configures 1 DE field group for the IPMF2 stage.
 *  Attach the FG to the given context. Creating 2 user qualifiers,
 *  using one of them as valid bit in order to test the EFES polarity.
 *  Polarity for the action will be set to zero by this function.
 *
 * \param [in] unit   - Device id.
 * \param [in] context_id  - Context id.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_polarity_zero_basic_example_main(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_qual_create(unit));

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_fg_create(unit, &fg_info));

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_attach(unit, context_id, fg_info, 0));

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function configures 1 DE field group for the IPMF2 stage.
 *  Attach the FG to the given contexts. Creating 2 user qualifiers,
 *  using one of them as valid bit in order to test the EFES polarity.
 *  Polarity for the action will be set to zero by this function.
 *
 * \param [in] unit   - Device id.
 * \param [in] context_id1  - Context id 1.
 * \param [in] context_id2  - Context id 2.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_different_polarities_main(
    int unit,
    bcm_field_context_t context_id1,
    bcm_field_context_t context_id2)
{
    bcm_field_group_info_t fg_info;

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_qual_create(unit));

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_fg_create(unit, &fg_info));

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_attach(unit, context_id1, fg_info, 0));

    BCM_IF_ERROR_RETURN(cint_field_efes_polarity_basic_example_attach(unit, context_id2, fg_info, 1));

    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit         - Device id
 * \param [in] context_id  - Context id.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_polarity_basic_example_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_efes_polarity_basic_example_fg_id, context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, Cint_field_efes_polarity_basic_example_fg_id));

    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, Cint_field_efes_polarity_basic_example_val_bit_qual_id));

    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, Cint_field_efes_polarity_basic_example_qual_id));

    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit         - Device id
 * \param [in] context_id1  - Context id 1.
 * \param [in] context_id2  - Context id 2.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_efes_different_polarity_destroy(
    int unit,
    bcm_field_context_t context_id1,
    bcm_field_context_t context_id2)
{
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_efes_polarity_basic_example_fg_id, context_id1));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_efes_polarity_basic_example_fg_id, context_id2));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, Cint_field_efes_polarity_basic_example_fg_id));

    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, Cint_field_efes_polarity_basic_example_val_bit_qual_id));

    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, Cint_field_efes_polarity_basic_example_qual_id));

    return BCM_E_NONE;
}
