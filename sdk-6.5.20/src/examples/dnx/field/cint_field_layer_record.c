/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  Testing Layer Record qualifiers.
 *
 *
 * Configuration example start:
 *
 * cint
 * cint_reset();
 * exit;
 * cint ../../src/examples/dnx/field/cint_field_layer_record.c
 * cint;
 * int unit = 0;
 * int fg_info_array_index = 0;
 * cint_field_layer_record_main(unit, fg_info_array_index);
 *
 * Configuration example end
 *
 *
 *  Test scenario:
 *      Configuring a TCAM field group in IPMF1 stage, with relevant qualifiers and actions.
 *      Attaching it to a context and adding an entry to it.
 *      The test is done in 7 cases as follow:
 *          1. General LR qualifiers
 *              - bcmFieldQualifyLayerRecordType
 *              - bcmFieldQualifyLayerRecordOffset
 *              - bcmFieldQualifyLayerRecordQualifier
 *          2. Ethernet Multicast LR qualifiers:
 *              - bcmFieldQualifyEthernetMulticast
 *          3. Ethernet Broadcast LR qualifiers:
 *              - bcmFieldQualifyEthernetBroadcast
 *          4. Ethernet LR qualifiers:
 *              - bcmFieldQualifyEthernetFirstTpidExist
 *              - bcmFieldQualifyEthernetFirstTpidIndex
 *              - bcmFieldQualifyEthernetSecondTpidExist
 *              - bcmFieldQualifyEthernetSecondTpidIndex
 *              - bcmFieldQualifyEthernetThirdTpidExist
 *              - bcmFieldQualifyEthernetThirdTpidIndex
 *          5. IPv4 LR qualifiers:
 *              - bcmFieldQualifyIpFrag
 *              - bcmFieldQualifyIp4DstMulticast
 *              - bcmFieldQualifyIpHasOptions
 *              - bcmFieldQualifyIpFirstFrag
 *          6. IPv6 LR Tunnel Type qualifiers:
 *              - bcmFieldQualifyIp4DstMulticast
 *              - bcmFieldQualifyIpTunnelType
 *          7. IPv6 LR Tunnel Type Raw qualifiers:
 *              - bcmFieldQualifyIp4DstMulticast
 *              - bcmFieldQualifyIpTunnelTypeRaw
 *          8. IPv6 LR qualifiers:
 *              - bcmFieldQualifyIp6MulticastCompatible
 *              - bcmFieldQualifyIp6FirstAdditionalHeaderExist
 *          9. ITMH LR qualifiers:
 *              - bcmFieldQualifyItmhPphType
 *
 *       Following actions will be performed in case of match:
 *          1. bcmFieldActionPrioIntNew
 *
 *       Context Selection is done according to Forwarding type for the specific case:
 *          1. bcmFieldLayerTypeEth
 *          2. bcmFieldLayerTypeIp4
 *          3. bcmFieldLayerTypeIp6
 *          4. bcmFieldLayerTypeTm
 */

/** Number of qualifiers for different test cases. (used as array size)*/
int LR_NOF_IPMF1_GENERAL_QUALS = 3;
int LR_NOF_IPMF1_ETH_MC_QUALS = 1;
int LR_NOF_IPMF1_ETH_BC_QUALS = 1;
int LR_NOF_IPMF1_ETH_QUALS = 6;
int LR_NOF_IPMF1_IPV4_QUALS = 4;
int LR_NOF_IPMF1_IPV4_GRE_QUALS = 2;
int LR_NOF_IPMF1_IPV4_GRE_RAW_QUALS = 2;
int LR_NOF_IPMF1_IPV6_QUALS = 3;
int LR_NOF_IPMF1_ITMH_QUALS = 1;

/** Number of actions to be performed. */
int LR_NOF_IPMF1_ACTIONS = 1;
/** Value to which the TC signal will be update. */
int LR_TC_ACTION_VALUE = 6;

/** Number of Context Selection qualifiers. */
int LR_NOF_IPMF1_CS_QUALS = 1;

/** Number of Field Groups (cases). (used as array size) */
int LR_NOF_FG = 9;

/**The presel_entry. Used for creating and destroying the presel. */
bcm_field_presel_entry_id_t presel_entry_id;
/**The Entry handle. Used for storing the entry handle to be destroyed. */
bcm_field_entry_t entry_handle;

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
struct cint_field_layer_record_qual_info_t
{
    bcm_field_qualify_t qual_type;      /* Qualifier type. */
    uint32 qual_value;          /* Qualifier value. */
    uint32 qual_mask;           /* Qualifier mask. */
    bcm_field_qualify_attach_info_t qual_attach_info;   /* Qualifier Attach info. */
};

/**
 * Structure, which contains needed action information.
 */
struct cint_field_layer_record_action_info_t
{
    bcm_field_action_t action_type;     /* Action type. */
    uint32 action_value;        /* Action value. */
};

/**
 * Structure, which contains needed context selection information.
 */
struct cint_field_layer_record_cs_info_t
{
    uint32 qual_value;          /* CS qualifier value. */
    int qual_arg;               /* CS qualifier argument */
};

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_layer_record_fg_info_t
{
    bcm_field_group_t fg_id;    /* Field Group ID. */
    bcm_field_context_t context_id;     /* Context ID. */
    bcm_field_stage_t stage;    /* Field stage. */
    bcm_field_group_type_t fg_type;     /* Field Group type. */
    int nof_quals;              /* Number of qualifiers per group. */
    int nof_actions;            /* Number of actions per group. */
    cint_field_layer_record_cs_info_t *lr_cs_info;      /* Context Selection Info. */
    cint_field_layer_record_qual_info_t *lr_qual_info;  /* Qualifier Info. */
    cint_field_layer_record_action_info_t *lr_action_info;      /* Action Info. */
};

/** IPMF1 General Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_general_info_array[LR_NOF_IPMF1_GENERAL_QUALS] = {
  /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyLayerRecordType, bcmFieldLayerTypeEth, 0x1F, {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}},
    {bcmFieldQualifyLayerRecordOffset, 2, 0xFF, {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}},
    {bcmFieldQualifyLayerRecordQualifier, 0xc, 0xFFFF, {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}}
};

/** IPMF1 Ethernet Multicast Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ethernet_mc_info_array[LR_NOF_IPMF1_ETH_MC_QUALS] = {
  /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyEthernetMulticast, TRUE, 0x1, {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}}
};

/** IPMF1 Ethernet Multicast Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ethernet_bc_info_array[LR_NOF_IPMF1_ETH_BC_QUALS] = {
  /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyEthernetBroadcast, TRUE, 0x1, {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}}
};

/** IPMF1 Ethernet Broadcast Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ethernet_info_array[LR_NOF_IPMF1_ETH_QUALS] = {
  /**  qual_type                       |   qual_value | qual_mask | input_type                      | input_arg | qual_offset  */
    {bcmFieldQualifyEthernetFirstTpidExist, TRUE,        0x1,    {bcmFieldInputTypeLayerRecordsAbsolute, 0,          0}},
    {bcmFieldQualifyEthernetFirstTpidIndex,   1,         0x7,    {bcmFieldInputTypeLayerRecordsAbsolute, 0,          0}},
    {bcmFieldQualifyEthernetSecondTpidExist,TRUE,        0x1,    {bcmFieldInputTypeLayerRecordsAbsolute, 0,          0}},
    {bcmFieldQualifyEthernetSecondTpidIndex,  1,         0x7,    {bcmFieldInputTypeLayerRecordsAbsolute, 0,          0}},
    {bcmFieldQualifyEthernetThirdTpidExist, TRUE,        0x1,    {bcmFieldInputTypeLayerRecordsAbsolute, 0,          0}},
    {bcmFieldQualifyEthernetThirdTpidIndex,   1,         0x7,    {bcmFieldInputTypeLayerRecordsAbsolute, 0,          0}}
};

/** IPMF1 IPv4 Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ipv4_info_array[LR_NOF_IPMF1_IPV4_QUALS] = {
  /**  qual_type                  |  qual_value   |   qual_mask    |  input_type                |    input_arg | qual_offset  */
    {bcmFieldQualifyIpFrag,          TRUE,               0x1,     {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}},
    {bcmFieldQualifyIp4DstMulticast, TRUE,               0x1,     {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}},
    {bcmFieldQualifyIpHasOptions,    TRUE,               0x1,     {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}},
    {bcmFieldQualifyIpFirstFrag,     TRUE,               0x1,     {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}}
};

/** IPMF1 IPv4 Layer Record GRE Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ipv4_gre_info_array[LR_NOF_IPMF1_IPV4_GRE_QUALS] = {
  /**  qual_type                    |qual_value    |  qual_mask |    input_type                   | input_arg | qual_offset  */
    {bcmFieldQualifyIp4DstMulticast, TRUE,                0x1, {bcmFieldInputTypeLayerRecordsAbsolute,   1,         0}},
    {bcmFieldQualifyIpTunnelType, bcmFieldTunnelTypeGre4, 0xF, {bcmFieldInputTypeLayerRecordsAbsolute,   1,         0}}
};

/** IPMF1 IPv4 Layer Record GRE RAW Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ipv4_gre_raw_info_array[LR_NOF_IPMF1_IPV4_GRE_RAW_QUALS] = {
  /**  qual_type                    |qual_value    |  qual_mask |    input_type                   | input_arg | qual_offset  */
    {bcmFieldQualifyIp4DstMulticast, TRUE,                0x1, {bcmFieldInputTypeLayerRecordsAbsolute,   1,         0}},
    {bcmFieldQualifyIpTunnelTypeRaw, 0x2,                 0xF, {bcmFieldInputTypeLayerRecordsAbsolute,   1,         0}}
};
/** IPMF1 IPv6 Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_ipv6_info_array[LR_NOF_IPMF1_IPV6_QUALS] = {
  /**  qual_type                             | qual_value | qual_mask | input_type                    | input_arg | qual_offset  */
    {bcmFieldQualifyIp6MulticastCompatible,        TRUE,      0x1,   {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}},
    {bcmFieldQualifyIp6FirstAdditionalHeaderExist, TRUE,      0x1,   {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}},
    {bcmFieldQualifyIp6SecondAdditionalHeaderExist, TRUE,      0x1,  {bcmFieldInputTypeLayerRecordsAbsolute, 1,         0}}
};

/** IPMF1 ITMH Layer Record Qualifiers info. */
cint_field_layer_record_qual_info_t field_layer_record_ipmf1_qual_itmh_info_array[LR_NOF_IPMF1_ITMH_QUALS] = {
  /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyItmhPphType, 3, 0x3, {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}}
};

/** IPMF1 Action info. */
cint_field_layer_record_action_info_t field_layer_record_ipmf1_action_info_array[LR_NOF_IPMF1_ACTIONS] = {
    /**     action_type       |   action_value   */
    {bcmFieldActionTtlSet, LR_TC_ACTION_VALUE}
};

/** IPMF1 Ethernet Context Selection info. */
cint_field_layer_record_cs_info_t field_layer_record_ipmf1_ethernet_cs_info_array[LR_NOF_IPMF1_CS_QUALS] = {
  /** qual_value |    qual_arg*/
    {bcmFieldLayerTypeEth, 0}
};

/** IPMF1 IPv4 Context Selection info. */
cint_field_layer_record_cs_info_t field_layer_record_ipmf1_ipv4_cs_info_array[LR_NOF_IPMF1_CS_QUALS] = {
  /** qual_value |    qual_arg*/
    {bcmFieldLayerTypeIp4, 1}
};

/** IPMF1 IPv6 Context Selection info. */
cint_field_layer_record_cs_info_t field_layer_record_ipmf1_ipv6_cs_info_array[LR_NOF_IPMF1_CS_QUALS] = {
  /** qual_value |    qual_arg*/
    {bcmFieldLayerTypeIp6, 1}
};

/** IPMF1 ITMH Context Selection info. */
cint_field_layer_record_cs_info_t field_layer_record_ipmf1_itmh_cs_info_array[LR_NOF_IPMF1_CS_QUALS] = {
  /** qual_value |    qual_arg*/
    {bcmFieldLayerTypeTm, 0}
};

/**
 * Array, which contains information per field group. Like:
 * fg_id, fg_type, context selection, quals, actions.
 */
cint_field_layer_record_fg_info_t layer_record_fg_info_array[LR_NOF_FG] = {
    /** IPMF1 General LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_GENERAL_QUALS,                         /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_ethernet_cs_info_array,    /** Context Selection info */
     field_layer_record_ipmf1_qual_general_info_array,   /** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     },
    /** IPMF1 ETH MC LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_ETH_MC_QUALS,                          /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_ethernet_cs_info_array,    /** Context Selection info */
     field_layer_record_ipmf1_qual_ethernet_mc_info_array,/** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     },
    /** IPMF1 ETH BC LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_ETH_BC_QUALS,                          /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_ethernet_cs_info_array,    /** Context Selection info */
     field_layer_record_ipmf1_qual_ethernet_bc_info_array,/** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     },
    /** IPMF1 ETH LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_ETH_QUALS,                             /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_ethernet_cs_info_array,    /** Context Selection info */
     field_layer_record_ipmf1_qual_ethernet_info_array,  /** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     },
    /** IPMF1 IPv4 LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_IPV4_QUALS,                            /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_ipv4_cs_info_array,        /** Context Selection info */
     field_layer_record_ipmf1_qual_ipv4_info_array,      /** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     },
     /** IPMF1 IPv4 LR quals field group info */
     {
      0,                                                 /** Field group ID */
      0,                                                 /** Context ID */
      bcmFieldStageIngressPMF1,                          /** Field stage */
      bcmFieldGroupTypeTcam,                             /** Field group type */
      LR_NOF_IPMF1_IPV4_GRE_QUALS,                       /** Number of tested qualifiers */
      LR_NOF_IPMF1_ACTIONS,                              /** Number of tested actions */
      field_layer_record_ipmf1_ipv4_cs_info_array,       /** Context Selection info */
      field_layer_record_ipmf1_qual_ipv4_gre_info_array, /** Qualifiers info */
      field_layer_record_ipmf1_action_info_array         /** Actions info */
      },
      /** IPMF1 IPv4 LR raw quals field group info */
      {
       0,                                                 /** Field group ID */
       0,                                                 /** Context ID */
       bcmFieldStageIngressPMF1,                          /** Field stage */
       bcmFieldGroupTypeTcam,                             /** Field group type */
       LR_NOF_IPMF1_IPV4_GRE_RAW_QUALS,                       /** Number of tested qualifiers */
       LR_NOF_IPMF1_ACTIONS,                              /** Number of tested actions */
       field_layer_record_ipmf1_ipv4_cs_info_array,       /** Context Selection info */
       field_layer_record_ipmf1_qual_ipv4_gre_raw_info_array, /** Qualifiers info */
       field_layer_record_ipmf1_action_info_array         /** Actions info */
       },
    /** IPMF1 IPv6 LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_IPV6_QUALS,                            /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_ipv6_cs_info_array,        /** Context Selection info */
     field_layer_record_ipmf1_qual_ipv6_info_array,      /** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     },
    /** IPMF1 ITMH LR quals field group info */
    {
     0,                                                  /** Field group ID */
     0,                                                  /** Context ID */
     bcmFieldStageIngressPMF1,                           /** Field stage */
     bcmFieldGroupTypeTcam,                              /** Field group type */
     LR_NOF_IPMF1_ITMH_QUALS,                            /** Number of tested qualifiers */
     LR_NOF_IPMF1_ACTIONS,                               /** Number of tested actions */
     field_layer_record_ipmf1_itmh_cs_info_array,        /** Context Selection info */
     field_layer_record_ipmf1_qual_itmh_info_array,      /** Qualifiers info */
     field_layer_record_ipmf1_action_info_array          /** Actions info */
     }
};

/**
 * This function is used to create a context and configure
 * the relevant program for the testing.
 */
int
cint_field_layer_record_context(
    int unit,
    bcm_field_stage_t stage,
    cint_field_layer_record_cs_info_t * lr_cs_info,
    bcm_field_context_t * context_id)
{
    int rv = BCM_E_NONE;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "LAYER_RECORD_CS", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, stage, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create \n", rv);
        return rv;
    }

    printf("Context ID %d was created.\n", *context_id);

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = 2;
    presel_entry_id.stage = stage;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = *context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Configure preselection due to forwarding type. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_value = lr_cs_info->qual_value;
    presel_entry_data.qual_data[0].qual_arg = lr_cs_info->qual_arg;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    return rv;
}

/**
 * This function creates field group, with all relevant information to it,
 * attaching it to the context and adding an entry.
 */
int
cint_field_layer_record_group_config(
    int unit,
    cint_field_layer_record_fg_info_t * lr_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    bcm_field_entry_info_t entry_info;

    /**
     * We have only one context selection qualifier per case, which is indicated with LR_NOF_IPMF1_CS_QUALS,
     * because of that we will parse only the first index of lr_cs_info[] array.
     */
    rv = cint_field_layer_record_context(unit, lr_fg_info->stage, lr_fg_info->lr_cs_info[0], &(lr_fg_info->context_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_layer_record_context. \n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = lr_fg_info->fg_type;
    fg_info.stage = lr_fg_info->stage;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "LAYER_RECORD_FG", sizeof(fg_info.name));

    fg_info.nof_quals = lr_fg_info->nof_quals;
    fg_info.nof_actions = lr_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    entry_info.priority = 1;

    for (qual_index = 0; qual_index < lr_fg_info->nof_quals; qual_index++)
    {
        fg_info.qual_types[qual_index] = lr_fg_info->lr_qual_info[qual_index].qual_type;
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type =
            lr_fg_info->lr_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            lr_fg_info->lr_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            lr_fg_info->lr_qual_info[qual_index].qual_attach_info.offset;
        entry_info.entry_qual[qual_index].type = fg_info.qual_types[qual_index];
        entry_info.entry_qual[qual_index].value[0] = lr_fg_info->lr_qual_info[qual_index].qual_value;
        entry_info.entry_qual[qual_index].mask[0] = lr_fg_info->lr_qual_info[qual_index].qual_mask;
    }

    for (action_index = 0; action_index < lr_fg_info->nof_actions; action_index++)
    {
        fg_info.action_types[action_index] = lr_fg_info->lr_action_info[action_index].action_type;
        fg_info.action_with_valid_bit[action_index] = TRUE;
        attach_info.payload_info.action_types[action_index] = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].type = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].value[0] = lr_fg_info->lr_action_info[action_index].action_value;
    }

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(lr_fg_info->fg_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, lr_fg_info->fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", lr_fg_info->fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, lr_fg_info->fg_id, lr_fg_info->context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, lr_fg_info->context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", lr_fg_info->fg_id, lr_fg_info->context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, lr_fg_info->fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", entry_handle, entry_handle, lr_fg_info->fg_id);

    return rv;
}

/**
 * \brief
 * Configure TCAM field group to test the layer record qualifiers.
 * Attach it to a context and add an entry to it.
 * Create two groups:
 *
 * \param [in] unit                 - Device id
 * \param [in] fg_info_array_index  - Array index to be parsed
 *  to the global array "layer_record_fg_info_array[]", which contains
 *  all needed information for the configuration.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_layer_record_main(
    int unit,
    int fg_info_array_index)
{
    int rv = BCM_E_NONE;

    /**
     * Parsing the global array "layer_record_fg_info_array[]" with all needed
     * information for the given fg_info_array_index.
     */
    rv = cint_field_layer_record_group_config(unit, layer_record_fg_info_array[fg_info_array_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_layer_record_group_config. \n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit                 - Device id
 * \param [in] fg_info_array_index  - Array index to be parsed
 *  to the global array "layer_record_fg_info_array[]", which contains
 *  all needed information to be destroyed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_layer_record_destroy(
    int unit,
    int fg_info_array_index)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;

    rv = bcm_field_entry_delete(unit, layer_record_fg_info_array[fg_info_array_index].fg_id, NULL, entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, layer_record_fg_info_array[fg_info_array_index].fg_id,
                                        layer_record_fg_info_array[fg_info_array_index].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, layer_record_fg_info_array[fg_info_array_index].fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = layer_record_fg_info_array[fg_info_array_index].stage;
    p_id.presel_id = 2;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_field_context_destroy(unit, layer_record_fg_info_array[fg_info_array_index].stage,
                                   layer_record_fg_info_array[fg_info_array_index].context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }

    return rv;
}
