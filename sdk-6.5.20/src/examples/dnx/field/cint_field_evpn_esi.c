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
 * cint ../../../../src/examples/dnx/field/cint_field_evpn_esi.c
 * cint;
 * int unit = 0;
 * int inlif_profile = 2;
 * cint_field_evpn_ingress(unit, inlif_profile);
 * cint_field_evpn_egress(unit);
 *
 * Configuration example end
 *
 *
 * cint ../../../../src/examples/dnx/field/cint_field_evpn_esi.c
 * cint;
 * cint_field_evpn_ingress(0, 2);
 * cint_field_evpn_egress(0);
 * cint_field_evpn_egress_esi_port_entry_add(0, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &dnx_evpn_field_info.entry_handle_esi_port, 5, 2);
 * cint_field_evpn_egress_esi_port_entry_add(0, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &dnx_evpn_field_info.entry_handle_esi_port_1, 5, 9);
 * cint_field_evpn_egress_esi_entry_add(0, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &dnx_evpn_field_info.entry_handle_esi, 5, 2);
 *
 * cint_field_evpn_destroy_all(0);
 * quit;
*/

/* Global Vars */
int max_nof_entry_handles = 10;  /** Suppose no more than 10 entries in the example FG*/
int evpn_esi_udh_base_3 = 3;

/*
 * When usind CW/FL, the ESI should be taken from a different
 * offset relative to the FWD layer. Set this value to 1 in
 * order to get it to work.
 * Note: This reference application can't work simultaneously
 * with and without CW/FL.
 */
int field_esi_filter__evpn_cw_enable = 0;
int field_esi_filter__evpn_fl_enable = 0;

int feature_mpls_term_1_or_2_labels = 0;


struct evpn_fg_info_s
{
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    bcm_field_context_t context_id;
    bcm_field_presel_t presel_id;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_entry_t entry_handle[max_nof_entry_handles];
};

struct dnx_evpn_field_info_s
{
    uint8 initialized;

    bcm_field_entry_t default_entry_handle;
    int default_entry_priority;

    bcm_field_qualify_t esi_qual;
    bcm_field_qualify_t esi_type_qual;
    bcm_field_qualify_t iml_range_profile_qual;
    bcm_field_qualify_t inac_leaf_indication_qual;
    bcm_field_qualify_t mact_leaf_indication_qual;
    bcm_field_action_t esi_action;
    bcm_field_action_t ingress_drop_action;   /**avoid bcmFieldActionRedirect to reduce the action size.*/

    evpn_fg_info_s ing_fg;   /** Ingress FP in ePE*/
    evpn_fg_info_s egr_fg;   /** Egress FP in ePE*/

    evpn_fg_info_s ing_etree_fg;  /** Ingress FP in iPE*/
    evpn_fg_info_s egr_etree_fg;  /** Egress FP in ePE*/
};

dnx_evpn_field_info_s dnx_evpn_field_info;


void cint_field_evpn_field_info_init(int unit)
{
    sal_memset(&dnx_evpn_field_info, 0, sizeof(dnx_evpn_field_info));

    dnx_evpn_field_info.initialized = 1;

    dnx_evpn_field_info.default_entry_priority = 50000;

    /** Init FG information for ESI*/
    dnx_evpn_field_info.ing_fg.presel_id = 61;
    dnx_evpn_field_info.egr_fg.presel_id = 65;

     /** Init FG information for E-Tree*/
    dnx_evpn_field_info.ing_etree_fg.presel_id = 60;
    dnx_evpn_field_info.egr_etree_fg.presel_id = 64;
}


int cint_field_evpn_field_entry_handle_store(int unit, bcm_field_entry_t entry_handle, bcm_field_entry_t *entry_handles)
{
    int entry_idx;
    for (entry_idx = 0; entry_idx < max_nof_entry_handles; entry_idx++)
    {
        if (entry_handles[entry_idx] == 0)
        {
            entry_handles[entry_idx] = entry_handle;
            break;
        }
    }

    if (entry_idx == max_nof_entry_handles)
    {
        printf("All entry handle storage are in-use!\n");
        return BCM_E_FULL;
    }

    return BCM_E_NONE;
}


/*
 * DirectExtract type of field group was used for constructing UDH and UDH-BASE.
 * For the device without the feature of mpls_term_1_or_2_labels (e.g., JR2):
 *     qual:{ESI_LABEL(20), 0b11}
 *     action: {UDH4[19:0],UDH-BASE4[1:0]}
 * For the device with the feature of mpls_term_1_or_2_labels (e.g., J2C, Q2A, etc):
 *     qual:{ESI_LABEL(20), 0b11, MPLS-LABEL-RANGE-PROFILE[0]}
 *     action: {UDH4[19:0], UDH-BASE4[1:0], VALID(1)}
 * In this example, if feature mpls_term_1_or_2_labels is enabled, PLS-LABEL-RANGE-PROFILE[0] was reserved for IML range indication.
 */
int cint_field_evpn_ingress_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    char *c_ptr;
    void *void_ptr;

    proc_name = "cint_field_evpn_ingress_fg_add";


    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_type_qual", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_type_qual);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_qualifier_create esi_type_qual\n", proc_name);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 20;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_qual", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_qual);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_qualifier_create esi_qual\n", proc_name);
        return rv;
    }

    if (feature_mpls_term_1_or_2_labels) {
        /** Create 1bit qual from mpls-label-range-profile[0] as valid bit for action.*/
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "iml_range_profile_bit0_qual", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.iml_range_profile_qual);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error in bcm_field_qualifier_create iml_range_profile_qual\n", proc_name);
            return rv;
        }
    }

    bcm_field_group_info_t_init(&dnx_evpn_field_info.ing_fg.fg_info);
    dnx_evpn_field_info.ing_fg.fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dnx_evpn_field_info.ing_fg.fg_info.stage = bcmFieldStageIngressPMF2;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;

    if (feature_mpls_term_1_or_2_labels) {
        /** Works as the valid bit for action esi_type_qual*/
        dnx_evpn_field_info.ing_fg.fg_info.qual_types[qual_idx] = dnx_evpn_field_info.iml_range_profile_qual;
        qual_idx++;
    }

    dnx_evpn_field_info.ing_fg.fg_info.qual_types[qual_idx] = dnx_evpn_field_info.esi_type_qual;
    qual_idx++;

    dnx_evpn_field_info.ing_fg.fg_info.qual_types[qual_idx] = dnx_evpn_field_info.esi_qual;
    qual_idx++;

    dnx_evpn_field_info.ing_fg.fg_info.nof_quals = qual_idx;


    /* Create UDA for a 20-bit ESI. UDA is required as the default size of the UDHData3 is 32bit. */
    bcm_field_action_info_t_init(&action_info);
    /**
    * Fill the structure needed for bcm_field_qualifier_create API
    */
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData3;
    action_info.size = 20;
    action_info.prefix_size=12;
    action_info.prefix_value=0;
    void_ptr = action_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "UDHData3", sizeof(action_info.name));
    /**
    * Calling the API bcm_field_action_create for positive case
    */
    bcm_field_action_create(unit, 0, &action_info, &dnx_evpn_field_info.esi_action);


    /**Same for the actions to move them*/
    act_idx = 0;
    /** Order of actions should be the same as order of qualifiers*/
    dnx_evpn_field_info.ing_fg.fg_info.action_types[act_idx] = bcmFieldActionUDHBase3;
    dnx_evpn_field_info.ing_fg.fg_info.action_with_valid_bit[act_idx] = feature_mpls_term_1_or_2_labels ? TRUE: FALSE;
    act_idx++;

    dnx_evpn_field_info.ing_fg.fg_info.action_types[act_idx] = dnx_evpn_field_info.esi_action;
    dnx_evpn_field_info.ing_fg.fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    dnx_evpn_field_info.ing_fg.fg_info.nof_actions = act_idx;

    rv = bcm_field_group_add(unit, 0, &dnx_evpn_field_info.ing_fg.fg_info, fg_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_add\n", proc_name);
        return rv;
    }

    return rv;
}

int cint_field_evpn_ingress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
    int rv = BCM_E_NONE;

    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = dnx_evpn_field_info.ing_fg.fg_info.nof_quals;
    attach_info.payload_info.nof_actions = dnx_evpn_field_info.ing_fg.fg_info.nof_actions;

    for(qual_idx=0; qual_idx < dnx_evpn_field_info.ing_fg.fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = dnx_evpn_field_info.ing_fg.fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx < dnx_evpn_field_info.ing_fg.fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = dnx_evpn_field_info.ing_fg.fg_info.action_types[act_idx];
    }

    qual_idx = 0;

    if (feature_mpls_term_1_or_2_labels) {
        /**Get the offset (within the layer record) of the layer offset (within the header).*/
        bcm_field_qualifier_info_get_t layer_qual_info;
        rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF2, &layer_qual_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
            return rv;
        }
        printf("\n\n layer_qual_info: offset = %d, size = %d\n\n", layer_qual_info.offset, layer_qual_info.size);

        /**iml range profile 4bits is from layer_qualifiers[fwd - 2][15:12] */
        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerRecordsFwd;
        attach_info.key_info.qual_info[qual_idx].input_arg = -2;
        attach_info.key_info.qual_info[qual_idx].offset = layer_qual_info.offset + 12;  /** take layer_qualifiers[fwd - 2][12]*/
        qual_idx++;
    }

    /** UDH Data Type */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = evpn_esi_udh_base_3;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /**Destination 20 bits from ESI (MPLS header) */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = -32 - (32 * field_esi_filter__evpn_cw_enable) - (32 * field_esi_filter__evpn_fl_enable);
    qual_idx++;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", fg_id, context_id);
        return rv;
    }

    return rv;
}

int cint_field_evpn_ingress_context_create(
    int unit, bcm_field_context_t * context_id_p,
    int inlif_profile,
    int fwd_layer_index,
    int presel_id,
    bcm_field_presel_entry_id_t * presel_entry_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    int rv= 0;


    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create
                    (unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_ingress_context_create: Error in bcm_field_context_create context %d\n", *context_id_p);
        return rv;
    }

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. Context created for iPMF1 is also created for iPMF2*/
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);

    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id_p, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_param_set for context %d\n", *context_id_p);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(presel_entry_id);
    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id->presel_id = presel_id;
    presel_entry_id->stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    if (!feature_mpls_term_1_or_2_labels) {
        
        presel_entry_data.nof_qualifiers = 2;
        presel_entry_data.context_id = *context_id_p;
        presel_entry_data.entry_valid = TRUE;

        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
        presel_entry_data.qual_data[0].qual_arg = 0;
        presel_entry_data.qual_data[0].qual_value = inlif_profile;
        presel_entry_data.qual_data[0].qual_mask = 0xF;

        presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerIndex;
        presel_entry_data.qual_data[1].qual_arg = 0;
        presel_entry_data.qual_data[1].qual_value = fwd_layer_index;
        presel_entry_data.qual_data[1].qual_mask = 0x7;
    } else {
        presel_entry_data.nof_qualifiers = 2;
        presel_entry_data.context_id = *context_id_p;
        presel_entry_data.entry_valid = TRUE;

        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
        presel_entry_data.qual_data[0].qual_arg = 0;
        presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
        presel_entry_data.qual_data[0].qual_mask = 0x1F;

        presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
        presel_entry_data.qual_data[1].qual_arg = -1;
        presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeForwardingMPLS;
        presel_entry_data.qual_data[1].qual_mask = 0x1F;
        
    }
    rv = bcm_field_presel_set(unit, 0, presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
        return rv;
    }
    return rv;
}




int cint_field_evpn_ingress(int unit, int inlif_profile)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_ingress";

    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL);

    rv = cint_field_evpn_ingress_context_create(unit, &dnx_evpn_field_info.ing_fg.context_id, inlif_profile, 3,
                                                dnx_evpn_field_info.ing_fg.presel_id, &dnx_evpn_field_info.ing_fg.presel_entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_context_create\n", proc_name);
        return rv;
    }


    rv = cint_field_evpn_ingress_fg_add(unit, &dnx_evpn_field_info.ing_fg.fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_add\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_ingress_fg_attach(unit, dnx_evpn_field_info.ing_fg.context_id, dnx_evpn_field_info.ing_fg.fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_attach\n", proc_name);
        return rv;
    }

    return rv;
}

int cint_field_evpn_egress_context_create(int unit, bcm_field_context_t *context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    int rv= 0;


    bcm_field_context_info_t_init(&context_info);
    context_info.name[0] = 'E';
    context_info.name[1] = 'S';
    context_info.name[2] = 'I';
    context_info.name[3] = 0;
    rv = bcm_field_context_create
                    (unit, 0, bcmFieldStageEgress, &context_info, context_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_egress_context_create: Error in bcm_field_context_create\n");
        return rv;
    }

    /* Set the Presels: Current Layer Protocol and UDH Type*/
    bcm_field_presel_entry_id_info_init(&dnx_evpn_field_info.egr_fg.presel_entry_id);
    dnx_evpn_field_info.egr_fg.presel_entry_id.presel_id = dnx_evpn_field_info.egr_fg.presel_id;
    dnx_evpn_field_info.egr_fg.presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;


    /** UDH Data Type */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase3;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = evpn_esi_udh_base_3;
    presel_entry_data.qual_data[0].qual_mask = 0x3;


    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &dnx_evpn_field_info.egr_fg.presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_context_create Error in Egress bcm_field_presel_set for context %d\n", *context_id_p);
        return rv;
    }

    return rv;
}


int cint_field_evpn_egress_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t bcm_action_id;

    proc_name = "cint_field_evpn_egress_fg_add";

    bcm_field_group_info_t_init(&dnx_evpn_field_info.egr_fg.fg_info);
    dnx_evpn_field_info.egr_fg.fg_info.fg_type = bcmFieldGroupTypeTcam;
    dnx_evpn_field_info.egr_fg.fg_info.stage = bcmFieldStageEgress;

    dnx_evpn_field_info.egr_fg.fg_info.qual_types[0] = bcmFieldQualifyUDHData3;

    dnx_evpn_field_info.egr_fg.fg_info.qual_types[1] = bcmFieldQualifyOutPort;

    dnx_evpn_field_info.egr_fg.fg_info.nof_quals = 2;

    /* Action - Drop */
    dnx_evpn_field_info.egr_fg.fg_info.action_types[0] = bcmFieldActionDrop;
    dnx_evpn_field_info.egr_fg.fg_info.action_with_valid_bit[0] = TRUE;

    dnx_evpn_field_info.egr_fg.fg_info.nof_actions = 1;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &dnx_evpn_field_info.egr_fg.fg_info, fg_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_add\n", proc_name);
        return rv;
    }

    return rv;
}


int cint_field_evpn_egress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
    int rv = BCM_E_NONE;

    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = dnx_evpn_field_info.egr_fg.fg_info.nof_quals;
    attach_info.payload_info.nof_actions = dnx_evpn_field_info.egr_fg.fg_info.nof_actions;

    for(qual_idx=0; qual_idx < dnx_evpn_field_info.egr_fg.fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = dnx_evpn_field_info.egr_fg.fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx < dnx_evpn_field_info.egr_fg.fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = dnx_evpn_field_info.egr_fg.fg_info.action_types[act_idx];
    }

    qual_idx = 0;
    /** UDH Data Type */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    qual_idx++;

    /** Out Port */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    qual_idx++;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", fg_id, context_id);
        return rv;
    }

    return rv;
}


int cint_field_evpn_egress_default_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_p)
{
    char *proc_name;

    bcm_field_entry_info_t entry_in_info;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_default_entry_add";

    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = dnx_evpn_field_info.egr_fg.fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = 0;
    entry_in_info.entry_qual[0].mask[0] = 0;

    entry_in_info.entry_action[0].type = dnx_evpn_field_info.egr_fg.fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 1;

    entry_in_info.priority = dnx_evpn_field_info.default_entry_priority;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", proc_name, rv, context_id, fg_id);
        return rv;
    }

    /** Store the entry handle for destroy convenience.*/
    rv = cint_field_evpn_field_entry_handle_store(unit, *entry_handle_p, dnx_evpn_field_info.egr_fg.entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_field_entry_handle_store for handle %d\n", proc_name, rv, *entry_handle_p);
        return rv;
    }

    return rv;
}

uint32 log2_round_up(uint32 x)
{
    uint32 msb_bit = 0;

    while (x > (1 << msb_bit))
    {
        msb_bit++;
    }

    return msb_bit;
}

int cint_field_evpn_egress_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_port_p,
        int esi_label,
        int port,
        uint8 is_drop)
{
    char *proc_name;

    bcm_field_entry_info_t entry_in_info;
    bcm_gport_t gport;
    int pp_port_size;
    int nof_pp_ports;
    int nof_cores;
    uint8 is_trunk = 0;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_entry_add";


    nof_pp_ports = *dnxc_data_get(unit, "port", "general", "nof_pp_ports", NULL);
    pp_port_size = log2_round_up(nof_pp_ports);

    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = dnx_evpn_field_info.egr_fg.fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = esi_label;
    entry_in_info.entry_qual[0].mask[0] = 0xfffff;

    if (port != BCM_GPORT_INVALID)
    {
        if (BCM_GPORT_IS_SET(port))
        {
            gport = port;
        }
        else
        {
            BCM_GPORT_LOCAL_SET(gport, port);
        }

        is_trunk = BCM_GPORT_IS_TRUNK(gport);

        entry_in_info.entry_qual[1].type = dnx_evpn_field_info.egr_fg.fg_info.qual_types[1];
        entry_in_info.entry_qual[1].value[0] = gport;
        entry_in_info.entry_qual[1].mask[0] = (1 << pp_port_size) - 1;
    }

    entry_in_info.entry_action[0].type = dnx_evpn_field_info.egr_fg.fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = is_drop ? 1 : 0;

    entry_in_info.priority = 10;
    entry_in_info.nof_entry_quals = 2 - (port == BCM_GPORT_INVALID);
    entry_in_info.nof_entry_actions = 1;
    if (is_trunk)
    {
        entry_in_info.core = 0;
    }
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", proc_name, rv, context_id, fg_id);
        return rv;
    }

    /** Store the entry handle for destroy convenience.*/
    rv = cint_field_evpn_field_entry_handle_store(unit, *entry_handle_esi_port_p, dnx_evpn_field_info.egr_fg.entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_field_entry_handle_store for handle %d\n", proc_name, rv, *entry_handle_esi_port_p);
        return rv;
    }

    nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    if (is_trunk && (nof_cores > 1))
    {
        entry_in_info.core = 1;
        rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d, core %d\n", proc_name, rv, context_id, fg_id, entry_in_info.core);
            return rv;
        }

        /** Store the entry handle for destroy convenience.*/
        rv = cint_field_evpn_field_entry_handle_store(unit, *entry_handle_esi_port_p, dnx_evpn_field_info.egr_fg.entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in cint_field_evpn_field_entry_handle_store for handle %d\n", proc_name, rv, *entry_handle_esi_port_p);
            return rv;
        }
    }

    return rv;
}


int cint_field_evpn_egress_esi_port_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_port_p,
        int esi_label,
        int port)
{
    char *proc_name;

    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_default_entry_add";

    /* Add the Entry (ESI + Port) --> Drop */
    rv = cint_field_evpn_egress_entry_add(unit, context_id, fg_id, entry_handle_esi_port_p, esi_label,port, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_egress_entry_add for port %d\n", proc_name, rv, port);
        return rv;
    }

    return rv;
}

int cint_field_evpn_egress_esi_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_p,
        int esi_label)
{
    char *proc_name;

    bcm_field_entry_info_t entry_in_info;
    bcm_field_group_info_t fg_info;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_default_entry_add";
    bcm_field_group_info_get( unit, fg_id, &fg_info);

    /*
     * Add the Entry (ESI) --> Do Nothing.
     */
    rv = cint_field_evpn_egress_entry_add(unit, context_id, fg_id, entry_handle_esi_p,
                                          esi_label, BCM_GPORT_INVALID, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_egress_entry_add for ESI %d\n", proc_name, rv, esi_label);
        return rv;
    }

    return rv;
}

int cint_field_evpn_egress(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress";
    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    rv = cint_field_evpn_egress_context_create(unit, &dnx_evpn_field_info.egr_fg.context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_context_create\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_egress_fg_add(unit, &dnx_evpn_field_info.egr_fg.fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_fg_add\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_egress_fg_attach(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_fg_attach\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_egress_default_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &dnx_evpn_field_info.default_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_default_entry_add\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Tcam type of field group was used for comparing inAC.Leaf_indication and MACT.leaf_indication.
 *     qual:{InAC.generic_data[0], MACT.entry_grouping[3]}
 *     action: {DROP}
 */
int cint_field_evpn_etree_ingress_fg_add(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    char *c_ptr;
    void *void_ptr;

    proc_name = "cint_field_evpn_etree_ingress_fg_add";

    if (dnx_evpn_field_info.inac_leaf_indication_qual == 0)
    {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "inac_leaf_indication_qual", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.inac_leaf_indication_qual);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error in bcm_field_qualifier_create inac_leaf_indication_qual\n", proc_name);
            return rv;
        }
    }

    if (dnx_evpn_field_info.mact_leaf_indication_qual == 0)
    {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "mact_leaf_indication_qual", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.mact_leaf_indication_qual);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error in bcm_field_qualifier_create mact_leaf_indication_qual\n", proc_name);
            return rv;
        }
    }

    if (dnx_evpn_field_info.ingress_drop_action == 0)
    {
        bcm_field_action_info_t_init(&action_info);
        action_info.action_type = bcmFieldActionForward;
        action_info.prefix_size = 31;
        /**11 bit of qualifier are 0 and to drop all dest bit are 1's. With lsb 1 from entry payload we get 0x001FFFFF*/
        action_info.prefix_value = 0x000BFFFF;
        action_info.size = 1;
        action_info.stage = bcmFieldStageIngressPMF2;
        void_ptr = &(action_info.name[0]);
        sal_strncpy_s(void_ptr, "ingress_drop_action", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &dnx_evpn_field_info.ingress_drop_action);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error in bcm_field_action_create\n", proc_name);
            return rv;
        }
    }

    bcm_field_group_info_t_init(&dnx_evpn_field_info.ing_etree_fg.fg_info);
    dnx_evpn_field_info.ing_etree_fg.fg_info.fg_type = bcmFieldGroupTypeTcam;
    dnx_evpn_field_info.ing_etree_fg.fg_info.stage = bcmFieldStageIngressPMF2;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;
    dnx_evpn_field_info.ing_etree_fg.fg_info.qual_types[qual_idx] = dnx_evpn_field_info.inac_leaf_indication_qual;
    qual_idx++;

    dnx_evpn_field_info.ing_etree_fg.fg_info.qual_types[qual_idx] = dnx_evpn_field_info.mact_leaf_indication_qual;
    qual_idx++;

    dnx_evpn_field_info.ing_etree_fg.fg_info.nof_quals = qual_idx;


    act_idx = 0;
    dnx_evpn_field_info.ing_etree_fg.fg_info.action_types[act_idx] = dnx_evpn_field_info.ingress_drop_action;
    dnx_evpn_field_info.ing_etree_fg.fg_info.action_with_valid_bit[act_idx] = TRUE;
    act_idx++;

    dnx_evpn_field_info.ing_etree_fg.fg_info.nof_actions = act_idx;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &dnx_evpn_field_info.ing_etree_fg.fg_info, &dnx_evpn_field_info.ing_etree_fg.fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_add\n", proc_name);
        return rv;
    }

    return rv;
}

int cint_field_evpn_etree_ingress_fg_attach(int unit)
{
    int rv = BCM_E_NONE;

    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_get_t inac_wide_data_qual_info, mact_entry_group_qual_info;

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = dnx_evpn_field_info.ing_etree_fg.fg_info.nof_quals;
    attach_info.payload_info.nof_actions = dnx_evpn_field_info.ing_etree_fg.fg_info.nof_actions;

    for(qual_idx=0; qual_idx < dnx_evpn_field_info.ing_etree_fg.fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = dnx_evpn_field_info.ing_etree_fg.fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx < dnx_evpn_field_info.ing_etree_fg.fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = dnx_evpn_field_info.ing_etree_fg.fg_info.action_types[act_idx];
    }

    /** Get the inAC.wide_data qual info, we need to qualify its LSB*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyAcInLifWideData, bcmFieldStageIngressPMF2, &inac_wide_data_qual_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    /** Get the MACT.entry_group qual info, we need to qualify its MSB*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyDstClassL2, bcmFieldStageIngressPMF2, &mact_entry_group_qual_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    qual_idx = 0;
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = inac_wide_data_qual_info.offset + 0; /** inAC.Wide_Data[0]*/
    qual_idx++;

    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = mact_entry_group_qual_info.offset + 3; /** MACT.ENTRY_GROUP[3]*/
    qual_idx++;

    rv = bcm_field_group_context_attach(unit, 0, dnx_evpn_field_info.ing_etree_fg.fg_id, dnx_evpn_field_info.ing_etree_fg.context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", dnx_evpn_field_info.ing_etree_fg.fg_id, dnx_evpn_field_info.ing_etree_fg.context_id);
        return rv;
    }

    return rv;
}

int cint_field_evpn_etree_ingress_fg_entry_add(int unit)
{
    int rv = BCM_E_NONE;

    /********* Add Entries *********/
    int entry_priority = 100;
    bcm_field_entry_info_t entry_in_info;
    char *proc_name;

    proc_name = "cint_field_evpn_etree_ingress_fg_add";

    /*
     * Here we add 1 entry for dropping all leaf-to-leaf packets.
     */
    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = dnx_evpn_field_info.ing_etree_fg.fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = 1;
    entry_in_info.entry_qual[0].mask[0] = 0x1;
    

    entry_in_info.entry_qual[1].type = dnx_evpn_field_info.ing_etree_fg.fg_info.qual_types[1];
    entry_in_info.entry_qual[1].value[0] = 1;
    entry_in_info.entry_qual[1].mask[0] = 0x1;

    entry_in_info.entry_action[0].type = dnx_evpn_field_info.ingress_drop_action;
    entry_in_info.entry_action[0].value[0] = 1;

    entry_in_info.priority = entry_priority++;
    entry_in_info.nof_entry_quals = dnx_evpn_field_info.ing_etree_fg.fg_info.nof_quals;
    entry_in_info.nof_entry_actions = dnx_evpn_field_info.ing_etree_fg.fg_info.nof_actions;
    rv = bcm_field_entry_add(unit, 0, dnx_evpn_field_info.ing_etree_fg.fg_id, &entry_in_info, &dnx_evpn_field_info.ing_etree_fg.entry_handle[0]);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add with context %d, fg_id %d for entry 1\n", proc_name,
               rv, dnx_evpn_field_info.ing_etree_fg.context_id, dnx_evpn_field_info.ing_etree_fg.fg_id);
        return rv;
    }

    return rv;
}


int cint_field_evpn_etree_ingress(int unit, int inlif_profile)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_etree_ingress";
    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    rv = cint_field_evpn_ingress_context_create(unit, &dnx_evpn_field_info.ing_etree_fg.context_id, inlif_profile, 0,
                                                dnx_evpn_field_info.ing_etree_fg.presel_id, &dnx_evpn_field_info.ing_etree_fg.presel_entry_id);
    
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_context_create\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_etree_ingress_fg_add(unit);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_add\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_etree_ingress_fg_attach(unit);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_attach\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_etree_ingress_fg_entry_add(unit);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_attach\n", proc_name);
        return rv;
    }

    return rv;
}


int cint_field_evpn_etree_egress_leaf_port_entry_add(
        int unit,
        int leaf_label,
        int port)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_t entry_handle;

    proc_name = "cint_field_evpn_etree_egress_leaf_port_entry_add";
    
    rv = cint_field_evpn_egress_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id,
                                          dnx_evpn_field_info.egr_fg.fg_id, &entry_handle,leaf_label, port, FALSE);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_entry_add for leaf %d\n", proc_name, leaf_label);
        return rv;
    }

    return rv;
}

int cint_field_evpn_destroy_all_entries(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int entry_idx;

    proc_name = "cint_field_evpn_destroy_all_entries";

    /* Destroy all the entries */
    for (entry_idx = 0; entry_idx < max_nof_entry_handles; entry_idx++)
    {
        if (dnx_evpn_field_info.ing_fg.entry_handle[entry_idx] != 0)
        {
            rv = bcm_field_entry_delete(unit,dnx_evpn_field_info.ing_fg.fg_id, NULL, dnx_evpn_field_info.ing_fg.entry_handle[entry_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv,
                       dnx_evpn_field_info.ing_fg.fg_id, dnx_evpn_field_info.ing_fg.entry_handle[entry_idx]);
                return rv;
            }
        }

        if (dnx_evpn_field_info.egr_fg.entry_handle[entry_idx] != 0)
        {
            rv = bcm_field_entry_delete(unit,dnx_evpn_field_info.egr_fg.fg_id, NULL, dnx_evpn_field_info.egr_fg.entry_handle[entry_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv,
                       dnx_evpn_field_info.egr_fg.fg_id, dnx_evpn_field_info.egr_fg.entry_handle[entry_idx]);
                return rv;
            }
        }

        if (dnx_evpn_field_info.ing_etree_fg.entry_handle[entry_idx] != 0)
        {
            rv = bcm_field_entry_delete(unit,dnx_evpn_field_info.ing_etree_fg.fg_id, NULL, dnx_evpn_field_info.ing_etree_fg.entry_handle[entry_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv,
                       dnx_evpn_field_info.ing_etree_fg.fg_id, dnx_evpn_field_info.ing_etree_fg.entry_handle[entry_idx]);
                return rv;
            }
        }

        if (dnx_evpn_field_info.egr_etree_fg.entry_handle[entry_idx] != 0)
        {
            rv = bcm_field_entry_delete(unit,dnx_evpn_field_info.egr_etree_fg.fg_id, NULL, dnx_evpn_field_info.egr_etree_fg.entry_handle[entry_idx]);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv,
                       dnx_evpn_field_info.egr_etree_fg.fg_id, dnx_evpn_field_info.egr_etree_fg.entry_handle[entry_idx]);
                return rv;
            }
        }
    }

    return rv;
}

int cint_field_evpn_destroy_udq_and_uda(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int index;
    bcm_field_qualify_t udq[max_nof_entry_handles] = {0};
    bcm_field_action_t uda[max_nof_entry_handles] = {0};

    proc_name = "cint_field_evpn_destroy_udq_and_uda";

    udq[0] = dnx_evpn_field_info.esi_type_qual;
    udq[1] = dnx_evpn_field_info.esi_qual;
    udq[2] = dnx_evpn_field_info.iml_range_profile_qual;
    udq[3] = dnx_evpn_field_info.inac_leaf_indication_qual;
    udq[4] = dnx_evpn_field_info.mact_leaf_indication_qual;

    uda[0] = dnx_evpn_field_info.esi_action;
    uda[1] = dnx_evpn_field_info.ingress_drop_action;

    for (index = 0; index < max_nof_entry_handles; index++)
    {
        if (udq[index] != 0)
        {
            rv = bcm_field_qualifier_destroy(unit, udq[index]);
            if(rv != BCM_E_NONE)
            {
                printf("%s Error in bcm_field_qualifier_destroy qual %d\n", proc_name, udq[index]);
                return rv;
            }
        }

        if (uda[index] != 0)
        {
            rv = bcm_field_action_destroy(unit, uda[index]);
            if(rv != BCM_E_NONE)
            {
                printf("%s Error in bcm_field_action_destroy action %d\n", proc_name, uda[index]);
                return rv;
            }
        }
    }

    return rv;
}

int cint_field_evpn_fg_destroy(int unit, evpn_fg_info_s fg_info)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int index;
    bcm_field_presel_entry_data_t presel_entry_data;

    proc_name = "cint_field_evpn_fg_destroy";

    if (fg_info.fg_id == 0)
    {
        /** Suppose nothing need to do for it*/
        return rv;
    }

    /** Destroy the FG entries if exist*/
    for (index = 0; index < max_nof_entry_handles; index++)
    {
        if (fg_info.entry_handle[index] != 0)
        {
            rv = bcm_field_entry_delete(unit,fg_info.fg_id, NULL, fg_info.entry_handle[index]);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv,
                       fg_info.fg_id, fg_info.entry_handle[index]);
                return rv;
            }
        }
    }

    rv = bcm_field_group_context_detach(unit, fg_info.fg_id, fg_info.context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_context_detach for fg_id %d context_id %d\n", proc_name, fg_info.fg_id, fg_info.context_id);
        return rv;
    }

    rv = bcm_field_group_delete(unit, fg_info.fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_delete for egress fg_id \n", proc_name, dnx_evpn_field_info.egr_fg.fg_id);
        return rv;
    }

    /* Destroy the preselector */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &fg_info.presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_presel_destroy for egress context %d presel_id %d \n", proc_name, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.presel_id);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, fg_info.presel_entry_id.stage, fg_info.context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_context_destroy for egress context %d\n",proc_name, dnx_evpn_field_info.egr_fg.context_id);
        return rv;
    }

    sal_memset(&fg_info, 0, sizeof(fg_info));

    return rv;
}


int cint_field_evpn_destroy_all(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_destroy_all";

    rv = cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_fg_destroy for fg_id %d\n", proc_name, rv, dnx_evpn_field_info.ing_fg.fg_id);
        return rv;
    }

    rv = cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.egr_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_fg_destroy for fg_id %d\n", proc_name, rv, dnx_evpn_field_info.egr_fg.fg_id);
        return rv;
    }

    rv = cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_etree_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_fg_destroy for fg_id %d\n", proc_name, rv, dnx_evpn_field_info.ing_etree_fg.fg_id);
        return rv;
    }

    rv = cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.egr_etree_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_fg_destroy for fg_id %d\n", proc_name, rv, dnx_evpn_field_info.egr_etree_fg.fg_id);
        return rv;
    }


    /* Destroy UDQ & UDA */
    rv = cint_field_evpn_destroy_udq_and_uda(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_destroy_ugq_and_uda for ingress for fg_id %d for udq and uda\n", proc_name, rv, dnx_evpn_field_info.ing_fg.fg_id);
        return rv;
    }

    return rv;
}

