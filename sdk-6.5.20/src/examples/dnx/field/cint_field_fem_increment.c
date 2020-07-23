/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 *  Example of usage of the FEM increment feature.
 *
 *  Configuration example start:
 *
 *  cint;
 *  cint_reset();
 *  exit;
 *  cint ../../src/examples/dnx/internal/field/cint_field_fem_increment.c
 *  cint;
 *  int unit = 0;
 *  bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 *  cint_field_fem_increment_main(unit, context_id);
 *
 *  Configuration example end
 *
 *  This cint shows how to increment some metadata signal using FEM based on 4 bit condition
 *  in this case the incremented metadata will be EgressForwardingIndex and the condition is
 *  done upon ForwardingLayerIndex = 1.
 */

bcm_field_group_t Cint_field_fem_increment_fg_id;
bcm_field_action_t Cint_field_fem_increment_void_action_id;
uint32 Cint_field_fem_increment_fem_increment_value = 2;

/**
 * Structure, which contains need information
 * for configuring of FEM condition actions.
 */
struct cint_field_fem_increment_fem_action_info_t
{
    int nof_conditions;
    int condition_ids[16];
    int nof_extractions;
    int nof_mapping_bits;
    bcm_field_fem_action_info_t fem_action_info;
};

/** Array with all needed information about actions, which are using FEMs. */
cint_field_fem_increment_fem_action_info_t Cint_field_fem_increment_fem_action_info_array[1] = {
    {
        1,  /** nof_conditions */
        {1},  /** condition_ids */
        1,  /** nof_extractions */
        0,  /** nof_mapping_bits */
        {
            {
                0,  /** input_offset */
                0   /** overriding_fg_id */
            },  /** fem_input */
            3,  /** condition_msb */
            {
                {
                    0,  /** extraction_id */
                    1   /** is_action_valid */
                }
            },  /** fem_condition */
            {
                {
                    bcmFieldActionEgressForwardingIndex,   /** action_type */
                    {
                        {
                            bcmFieldFemExtractionOutputSourceTypeCount,  /** source_type */
                            0,  /** offset */
                            0  /** forced_value */
                        }
                    },  /** output_bit */
                    Cint_field_fem_increment_fem_increment_value  /** increment */
                }
            }   /** fem_extraction */
        }   /** fem_action_info */
    }
};

/**
 * \brief
 *  Creates DE FG and attaching it to the given context.
 *
 * \param [in] unit   - Device id
 * \param [in] context_id   - Context id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_fem_increment_config_ipmf2_de(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int rv = BCM_E_NONE;
    bcm_field_fem_action_info_t  fem_action_info;
    void *dest_char;
    bcm_field_action_info_t action_info;
    bcm_field_action_priority_t action_priority;
    int *condition_id = Cint_field_fem_increment_fem_action_info_array[0].condition_ids;
    int mapping_bits_index, conditions_index, extractions_index;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "fem_increment", sizeof(fg_info.name));

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyForwardingLayerIndex;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "fem_increment", sizeof(action_info.name));
    action_info.size = 3;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.prefix_size = 29;
    action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_fem_increment_void_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = Cint_field_fem_increment_void_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &Cint_field_fem_increment_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("Field Group ID %d was created. \n", Cint_field_fem_increment_fg_id);

    /** FEM actions. */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_input.input_offset;
    fem_action_info.condition_msb = Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.condition_msb;
    for (conditions_index = 0; conditions_index < Cint_field_fem_increment_fem_action_info_array[0].nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id[conditions_index]].extraction_id =
            Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id[conditions_index]].is_action_valid =
            Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_condition[conditions_index].is_action_valid;
    }
    for (extractions_index = 0; extractions_index < Cint_field_fem_increment_fem_action_info_array[0].nof_extractions; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
            Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_extraction[extractions_index].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_extraction[extractions_index].increment;
        for (mapping_bits_index = 0; mapping_bits_index < Cint_field_fem_increment_fem_action_info_array[0].nof_mapping_bits;
             mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].forced_value;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                Cint_field_fem_increment_fem_action_info_array[0].fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].source_type;
        }
    }
    /** We choose FEM 10, because this FEM is not used by any applications and it is 24 bits, where increment can be performed. */
    action_priority = BCM_FIELD_ACTION_POSITION(3, 2);
    rv = bcm_field_fem_action_add(unit,0 , Cint_field_fem_increment_fg_id, action_priority, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_fem_increment_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_fem_increment_fg_id, context_id);

    return rv;
}

/**
 * \brief
 *  Creates DE FG and attaching it to the given context.
 *  And adding FEM action to the created FG in order to
 *  show usage of the FEM increment feature.
 *
 * \param [in] unit   - Device id
 * \param [in] context_id   - Context id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_fem_increment_main(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = 0;

    rv = cint_field_fem_increment_config_ipmf2_de(unit, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_fem_increment_config_ipmf2_de\n", rv);
        return rv;
    }

    return rv;
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
cint_field_fem_increment_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    bcm_field_action_priority_t action_priority;

    rv = bcm_field_group_context_detach(unit, Cint_field_fem_increment_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    action_priority = BCM_FIELD_ACTION_POSITION(3, 2);
    rv = bcm_field_fem_action_delete(unit, Cint_field_fem_increment_fg_id, action_priority);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_delete. \n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_fem_increment_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    rv = bcm_field_action_destroy(unit, Cint_field_fem_increment_void_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy. \n");
        return rv;
    }

    return rv;
}
