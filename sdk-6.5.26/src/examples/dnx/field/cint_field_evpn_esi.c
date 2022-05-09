/*
 * Configuration example start:
 *

  cint ../../../src/examples/dnx/field/cint_field_evpn_esi.c
  cint ../../../../src/examples/dnx/field/cint_field_evpn_esi.c
  cint;
  int unit = 0;
  int inlif_profile = 2;
  cint_field_evpn_ingress(unit, inlif_profile);
  cint_field_evpn_egress(unit);
 *
 * Configuration example end
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

int evpn_iml_wi_fl_act_val = 1;  
int evpn_iml_wo_fl_act_val = 2;


/*
 * When usind CW/FL, the ESI should be taken from a different
 * offset relative to the FWD layer. Set this value to 1 in
 * order to get it to work.
 * Note: This reference application can't work simultaneously
 * with and without CW/FL.
 */
int field_esi_filter__evpn_cw_enable = 0;
int field_esi_filter__evpn_fl_enable = 0;

int feature_mpls_term_till_bos = 0;
int evpn_esi_prio = 5;


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
    bcm_field_qualify_t esi_valid_qual; /** The bos of the label after IML.*/
    bcm_field_qualify_t iml_range_profile_qual;
    bcm_field_qualify_t inac_leaf_indication_qual;
    bcm_field_qualify_t mact_leaf_indication_qual;
    bcm_field_action_t esi_action; /** Used for copy ESI to UDH3*/
    bcm_field_action_t esi_indication_action; /** Used in cascaded FP for pmf1 action, which is then used for pmf2 cs.*/
    bcm_field_action_t ingress_drop_action;   /**avoid bcmFieldActionRedirect to reduce the action size.*/

    evpn_fg_info_s ing_fg;   /** Ingress FP in ePE*/
    evpn_fg_info_s egr_fg;   /** Egress FP in ePE*/

    evpn_fg_info_s ing_pmf1_fg;   /** Ingress FP in ePE for EVPN with Flow label*/
    evpn_fg_info_s ing_pmf2_fg1;   /** Ingress FP in ePE for EVPN with Flow label*/
    evpn_fg_info_s ing_pmf2_fg2;   /** Ingress FP in ePE for EVPN without Flow label*/

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

    dnx_evpn_field_info.ing_pmf1_fg.presel_id = 59;
    dnx_evpn_field_info.ing_pmf2_fg1.presel_id = 59;
    dnx_evpn_field_info.ing_pmf2_fg2.presel_id = 61;

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
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    char *c_ptr;
    void *void_ptr;

    printf("Running: cint_field_evpn_ingress_fg_add()\n");


    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_type_qual", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_type_qual));


    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 20;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_qual", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_qual));


    bcm_field_group_info_t_init(&dnx_evpn_field_info.ing_fg.fg_info);
    dnx_evpn_field_info.ing_fg.fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dnx_evpn_field_info.ing_fg.fg_info.stage = bcmFieldStageIngressPMF2;
    qual_idx = 0;

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
    dnx_evpn_field_info.ing_fg.fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    dnx_evpn_field_info.ing_fg.fg_info.action_types[act_idx] = dnx_evpn_field_info.esi_action;
    dnx_evpn_field_info.ing_fg.fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    dnx_evpn_field_info.ing_fg.fg_info.nof_actions = act_idx;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &dnx_evpn_field_info.ing_fg.fg_info, fg_id_p));

    return BCM_E_NONE;
}

int cint_field_evpn_ingress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
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

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    return BCM_E_NONE;
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

    bcm_field_context_info_t_init(&context_info);

    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id_p));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. Context created for iPMF1 is also created for iPMF2*/
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);

    BCM_IF_ERROR_RETURN(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id_p, &param_info));

    bcm_field_presel_entry_id_info_init(presel_entry_id);
    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id->presel_id = presel_id;
    presel_entry_id->stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

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

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}

int cint_field_evpn_ingress(int unit, int inlif_profile)
{

    printf("Running: cint_field_evpn_ingress()\n");

    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    feature_mpls_term_till_bos = *dnxc_data_get(unit, "mpls", "general", "mpls_term_till_bos", NULL);

    if (feature_mpls_term_till_bos) {
        /** In the devices with this feature, the FG is different.*/
        BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress(unit, inlif_profile));

        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(cint_field_evpn_ingress_context_create(unit, &dnx_evpn_field_info.ing_fg.context_id, inlif_profile, 3,
                                                dnx_evpn_field_info.ing_fg.presel_id, &dnx_evpn_field_info.ing_fg.presel_entry_id));


    BCM_IF_ERROR_RETURN(cint_field_evpn_ingress_fg_add(unit, &dnx_evpn_field_info.ing_fg.fg_id));

    BCM_IF_ERROR_RETURN(cint_field_evpn_ingress_fg_attach(unit, dnx_evpn_field_info.ing_fg.context_id, dnx_evpn_field_info.ing_fg.fg_id));

    return BCM_E_NONE;
}


int cint_field_evpn_fl_ingress_pmf1_context_create(
    int unit, 
    int presel_id,
    bcm_field_context_t * context_id_p,
    bcm_field_presel_entry_id_t * presel_entry_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    void *dest_char;

    printf("Running: cint_field_evpn_fl_ingress_context_create()\n");

    if (!feature_mpls_term_till_bos) {
        printf("Error! Used for devices with feature of mpls_term_till_bos only\n");
        return BCM_E_UNAVAIL;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &context_info.name[0];
    sal_strncpy_s(dest_char, "EVPN_CAS_IPMF1_CTX", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id_p));


    bcm_field_presel_entry_id_info_init(presel_entry_id);
    presel_entry_id->presel_id = presel_id;
    presel_entry_id->stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_data.nof_qualifiers = 0;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    presel_entry_data.nof_qualifiers ++;

    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = -2;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeMpls;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    presel_entry_data.nof_qualifiers ++;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}

int cint_field_evpn_fl_ingress_pmf1_fg_add(int unit, evpn_fg_info_s *fg)
{
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    evpn_fg_info_s * ing_fg = fg;
    void *void_ptr;

    printf("Running: cint_field_evpn_fl_ingress_fg_add()\n");

    if (!feature_mpls_term_till_bos) {
        printf("Error! Used for devices with feature of mpls_term_till_bos only\n");
        return BCM_E_UNAVAIL;
    }

    bcm_field_group_info_t_init(&ing_fg->fg_info);
    void_ptr = ing_fg->fg_info.name;
    sal_strncpy_s(void_ptr, "EVPN_CAS_PMF1_GRP", sizeof(ing_fg->fg_info.name));

    ing_fg->fg_info.fg_type = bcmFieldGroupTypeTcam;
    ing_fg->fg_info.stage = bcmFieldStageIngressPMF1;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;

    /** layer_qualfier.label_range_profile(4b)*/
    ing_fg->fg_info.qual_types[qual_idx] = bcmFieldQualifyLayerRecordQualifier;
    qual_idx++;

    /** in_lif_profile */
    ing_fg->fg_info.qual_types[qual_idx] = bcmFieldQualifyInVportClass0;
    qual_idx++;

    /** Following two qualifiers are not used but for occupying a full tcam entry*/
    ing_fg->fg_info.qual_types[qual_idx] = bcmFieldQualifySrcIp;
    qual_idx++;
    ing_fg->fg_info.qual_types[qual_idx] = bcmFieldQualifyDstIp;
    qual_idx++;

    ing_fg->fg_info.nof_quals = qual_idx;


    /*
     * Create UDA for ESI Indication. Since the
     * The cascaded preselector qualifier takes into account only the 4msb of the payload.
     * We define a 4bits UDA.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 4;
    action_info.prefix_size= 0;
    action_info.prefix_value= 0;
    void_ptr = action_info.name;
    sal_strncpy_s(void_ptr, "ESI_Indication", sizeof(action_info.name));
    /**
    * Calling the API bcm_field_action_create for positive case
    */
    bcm_field_action_create(unit, 0, &action_info, &dnx_evpn_field_info.esi_indication_action);

    /**Same for the actions to move them*/
    act_idx = 0;
    ing_fg->fg_info.action_types[act_idx] = dnx_evpn_field_info.esi_indication_action;
    ing_fg->fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    ing_fg->fg_info.nof_actions = act_idx;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &ing_fg->fg_info, &ing_fg->fg_id));

    return BCM_E_NONE;
}

int cint_field_evpn_fl_ingress_pmf1_fg_attach(int unit, evpn_fg_info_s * ing_pmf1_fg)
{
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    if (!feature_mpls_term_till_bos) {
        printf("Error! Used for devices with feature of mpls_term_till_bos only\n");
        return BCM_E_UNAVAIL;
    }

    /*
     * Attach the 1st context to the FG
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = ing_pmf1_fg->fg_info.nof_quals;
    for(qual_idx=0; qual_idx < ing_pmf1_fg->fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = ing_pmf1_fg->fg_info.qual_types[qual_idx];
    }

    attach_info.payload_info.nof_actions = ing_pmf1_fg->fg_info.nof_actions;
    for(act_idx=0; act_idx < ing_pmf1_fg->fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = ing_pmf1_fg->fg_info.action_types[act_idx];
    }

    qual_idx = 0;
    /**mpls label_range_profile */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = -2;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /**in_lif_profile */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /*bcmFieldQualifySrcIp*/
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /*bcmFieldQualifyDstIp*/
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, ing_pmf1_fg->fg_id, ing_pmf1_fg->context_id, &attach_info));

    return BCM_E_NONE;
}

int cint_field_evpn_fl_ingress_pmf1_install(int unit, int inlif_profile)
{
    printf("Running: cint_field_evpn_ingress()\n");

    /*
     * FG for EVPN
     */
    printf("Install PMF1 configurations for EVPN.\n");
    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf1_context_create(unit,
                                                        dnx_evpn_field_info.ing_pmf1_fg.presel_id,
                                                        &dnx_evpn_field_info.ing_pmf1_fg.context_id,
                                                        &dnx_evpn_field_info.ing_pmf1_fg.presel_entry_id));


    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf1_fg_add(unit, &dnx_evpn_field_info.ing_pmf1_fg));


    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf1_fg_attach(unit, &dnx_evpn_field_info.ing_pmf1_fg));

    /*
     * Add one entry to the FG for EVPN with Flow Label case
     */
    bcm_field_entry_t entry_handle;
    bcm_field_entry_info_t entry_in_info;
    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = dnx_evpn_field_info.ing_pmf1_fg.fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = (1 << 12);
    entry_in_info.entry_qual[0].mask[0] = (0xF << 12);

    entry_in_info.entry_qual[1].type = dnx_evpn_field_info.ing_pmf1_fg.fg_info.qual_types[1];
    entry_in_info.entry_qual[1].value[0] = inlif_profile;
    entry_in_info.entry_qual[1].mask[0] = 0xF;

    entry_in_info.entry_qual[2].type = dnx_evpn_field_info.ing_pmf1_fg.fg_info.qual_types[2];
    entry_in_info.entry_qual[2].value[0] = 0;
    entry_in_info.entry_qual[2].mask[0] = 0;

    entry_in_info.entry_qual[3].type = dnx_evpn_field_info.ing_pmf1_fg.fg_info.qual_types[3];
    entry_in_info.entry_qual[3].value[0] = 0;
    entry_in_info.entry_qual[3].mask[0] = 0;

    entry_in_info.entry_action[0].type = dnx_evpn_field_info.ing_pmf1_fg.fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = evpn_iml_wi_fl_act_val;

    
    entry_in_info.nof_entry_quals = 4;
    entry_in_info.nof_entry_actions = 1;
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, dnx_evpn_field_info.ing_pmf1_fg.fg_id, &entry_in_info, &entry_handle));

    /** Store the entry handle for destroy convenience.*/
    BCM_IF_ERROR_RETURN(cint_field_evpn_field_entry_handle_store(unit, entry_handle, dnx_evpn_field_info.ing_pmf1_fg.entry_handle));

    /*
     * Add one entry to the FG for EVPN without Flow Label case
     */
    entry_in_info.entry_qual[1].mask[0] = 0;
    
    entry_in_info.entry_action[0].value[0] = evpn_iml_wo_fl_act_val;

    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, dnx_evpn_field_info.ing_pmf1_fg.fg_id, &entry_in_info, &entry_handle));


    /** Store the entry handle for destroy convenience.*/
    BCM_IF_ERROR_RETURN(cint_field_evpn_field_entry_handle_store(unit, entry_handle, dnx_evpn_field_info.ing_pmf1_fg.entry_handle));

    return BCM_E_NONE;
}


int cint_field_evpn_fl_ingress_pmf2_cas_context_create(
    int unit, 
    int fl_enable,
    evpn_fg_info_s pre_fg,
    int presel_id,
    bcm_field_context_t * context_id_p,
    bcm_field_presel_entry_id_t * presel_entry_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    void *dest_char;

    printf("Running: cint_field_evpn_fl_ingress_pmf2_cas_context_create()\n");

    if (!feature_mpls_term_till_bos) {
        printf("Error! Used for devices with feature of mpls_term_till_bos only\n");
        return BCM_E_UNAVAIL;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &context_info.name[0];
    if (fl_enable) {
        sal_strncpy_s(dest_char, "EVPN_CAS_IPMF2_WI_FL_CTX", sizeof(context_info.name));
    } else {
        sal_strncpy_s(dest_char, "EVPN_CAS_IPMF2_WO_FL_CTX", sizeof(context_info.name));
    }
    context_info.cascaded_from = pre_fg.context_id;
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF2, &context_info, context_id_p));


    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
    BCM_IF_ERROR_RETURN(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id_p, &param_info));



    bcm_field_presel_entry_id_info_init(presel_entry_id);
    presel_entry_id->presel_id = presel_id;
    presel_entry_id->stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_data.nof_qualifiers = 0;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCascadedKeyValue;
    presel_entry_data.qual_data[0].qual_arg = pre_fg.fg_id;
    presel_entry_data.qual_data[0].qual_value = fl_enable ? evpn_iml_wi_fl_act_val : evpn_iml_wo_fl_act_val;
    presel_entry_data.qual_data[0].qual_mask = 0xf;
    presel_entry_data.nof_qualifiers ++;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}

int cint_field_evpn_fl_ingress_pmf2_cas_fg_add(int unit, int fl_enable, evpn_fg_info_s *fg)
{
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    evpn_fg_info_s * ing_fg = fg;
    char *c_ptr;
    void *void_ptr;

    printf("Running: cint_field_evpn_fl_ingress_pmf2_cas_fg_add()\n");

    if (!feature_mpls_term_till_bos) {
        printf("Error! Used for devices with feature of mpls_term_till_bos only\n");
        return BCM_E_UNAVAIL;
    }

    if (dnx_evpn_field_info.esi_type_qual == 0) {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 2;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "esi_type_qual", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_type_qual));

    }

    if (dnx_evpn_field_info.esi_qual == 0) {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 20;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "esi_qual", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_qual));

    }

    if (dnx_evpn_field_info.esi_valid_qual == 0) {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "esi_valid_qual", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.esi_valid_qual));

    }

    bcm_field_group_info_t_init(&ing_fg->fg_info);
    void_ptr = ing_fg->fg_info.name;
    if (fl_enable) {   
        sal_strncpy_s(void_ptr, "EVPN_CAS_PMF2_WI_FL_GRP", sizeof(ing_fg->fg_info.name));
    } else {
        sal_strncpy_s(void_ptr, "EVPN_CAS_PMF2_WO_FL_GRP", sizeof(ing_fg->fg_info.name));
    }
    ing_fg->fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    ing_fg->fg_info.stage = bcmFieldStageIngressPMF2;
    qual_idx = 0;

    ing_fg->fg_info.qual_types[qual_idx] = dnx_evpn_field_info.esi_valid_qual;
    qual_idx++;
    ing_fg->fg_info.qual_types[qual_idx] = dnx_evpn_field_info.esi_type_qual;
    qual_idx++;
    ing_fg->fg_info.qual_types[qual_idx] = dnx_evpn_field_info.esi_qual;;
    qual_idx++;
    ing_fg->fg_info.nof_quals = qual_idx;


    /* Create UDA for a 20-bit ESI. UDA is required as the default size of the UDHData3 is 32bit. */
    if (dnx_evpn_field_info.esi_action == 0) {
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
        sal_strncpy_s(c_ptr, "esi_action", sizeof(action_info.name));
        /**
        * Calling the API bcm_field_action_create for positive case
        */
        bcm_field_action_create(unit, 0, &action_info, &dnx_evpn_field_info.esi_action);
    }

    /**Same for the actions to move them*/
    act_idx = 0;
    ing_fg->fg_info.action_types[act_idx] = bcmFieldActionUDHBase3;
    ing_fg->fg_info.action_with_valid_bit[act_idx] = TRUE;
    act_idx++;
    ing_fg->fg_info.action_types[act_idx] = dnx_evpn_field_info.esi_action;
    ing_fg->fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;
    ing_fg->fg_info.nof_actions = act_idx;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &ing_fg->fg_info, &ing_fg->fg_id));

    return BCM_E_NONE;
}

int cint_field_evpn_fl_ingress_pmf2_cas_fg_attach(int unit, int fl_enable, evpn_fg_info_s * ing_pmf2_fg)
{
    int qual_idx = 0;
    int act_idx = 0;
    int cw_enable = field_esi_filter__evpn_cw_enable;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_t context_id = ing_pmf2_fg->context_id;
    bcm_field_group_t fg_id = ing_pmf2_fg->fg_id;

    if (!feature_mpls_term_till_bos) {
        printf("Error! Used for devices with feature of mpls_term_till_bos only\n");
        return BCM_E_UNAVAIL;
    }

    /*
     * Attach the 1st context to the FG
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = ing_pmf2_fg->fg_info.nof_quals;
    for(qual_idx=0; qual_idx < ing_pmf2_fg->fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = ing_pmf2_fg->fg_info.qual_types[qual_idx];
    }

    attach_info.payload_info.nof_actions = ing_pmf2_fg->fg_info.nof_actions;
    for(act_idx=0; act_idx < ing_pmf2_fg->fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = ing_pmf2_fg->fg_info.action_types[act_idx];
    }
    attach_info.payload_info.action_info[0].valid_bit_polarity = fl_enable ? 0 : 1;

    qual_idx = 0;
    /** MPLS Bos 1 bits from the Label after IML */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = -9 - (32 * cw_enable) - (32 * fl_enable);
    qual_idx++;

    /** UDH Data Type */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = evpn_esi_udh_base_3;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /** MPLS Label 20 bits from ESI (MPLS header) */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = -32 - (32 * cw_enable) - (32 * fl_enable);
    qual_idx++;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, ing_pmf2_fg->fg_id, ing_pmf2_fg->context_id, &attach_info));

    return BCM_E_NONE;
}

int cint_field_evpn_fl_ingress_pmf2_install(int unit)
{



    printf("Running: cint_field_evpn_fl_ingress_pmf2_install()\n");


    printf("Install PMF2 configurations for EVPN with Flow Label.\n");

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_cas_context_create(unit, 1,
                                                            dnx_evpn_field_info.ing_pmf1_fg,
                                                            dnx_evpn_field_info.ing_pmf2_fg1.presel_id,
                                                            &dnx_evpn_field_info.ing_pmf2_fg1.context_id,
                                                            &dnx_evpn_field_info.ing_pmf2_fg1.presel_entry_id));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_cas_fg_add(unit, 1, &dnx_evpn_field_info.ing_pmf2_fg1));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_cas_fg_attach(unit, 1, &dnx_evpn_field_info.ing_pmf2_fg1));

    printf("Install PMF2 configurations for EVPN without Flow Label.\n");

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_cas_context_create(unit, 0,
                                                            dnx_evpn_field_info.ing_pmf1_fg,
                                                            dnx_evpn_field_info.ing_pmf2_fg2.presel_id,
                                                            &dnx_evpn_field_info.ing_pmf2_fg2.context_id,
                                                            &dnx_evpn_field_info.ing_pmf2_fg2.presel_entry_id));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_cas_fg_add(unit, 0, &dnx_evpn_field_info.ing_pmf2_fg2));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_cas_fg_attach(unit, 0, &dnx_evpn_field_info.ing_pmf2_fg2));

    return BCM_E_NONE;
}


/*
 * For the device with the feature of mpls_term_till_bos only (e.g., J2C, Q2A, etc):
 * Cascaded FP is used. The first stage is to map the label-range-profile to an action;
 * and the second stage is to direct extract the ESI label to UDH based on the profile.
 *     pmf1 cs:  {forward_layer_type, forward-2_layer_type}
 *     pmf1 qual:{forward-2_label_range_profile, in_lif_profile}
 *     pmf1 action: {void(4b, for pmf2 CS)}
 *     entry1: with in_lif_profile, void(4b = val1)
 *     entry2: without in_lif_profile, void(4b = val2)
 *
 * For cases With Flow_lable:
 *     pmf2 cs:  {void(4b, val1, cascaded from pmf1)}
 *     pmf2 qual:{ESI_LABEL(20), 0b11, BOS_after_IML}
 *     pmf2 action: action: {UDH4[19:0], UDH-BASE4[1:0], VALID(1b, 0)}
 *
 * For cases Without Flow_lable:
 *     pmf2 cs:  {void(4b, val2, cascaded from pmf1)}
 *     pmf2 qual:{ESI_LABEL(20), 0b11, BOS_after_IML}
 *     pmf2 action: action: {UDH4[19:0], UDH-BASE4[1:0], VALID(1b, 1)}
 */
int cint_field_evpn_fl_ingress(int unit, int inlif_profile)
{
    printf("Running: cint_field_evpn_ingress()\n");

    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf1_install(unit, inlif_profile));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fl_ingress_pmf2_install(unit));

    return BCM_E_NONE;
}

int cint_field_evpn_egress_context_create(int unit, bcm_field_context_t *context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    bcm_field_context_info_t_init(&context_info);
    context_info.name[0] = 'E';
    context_info.name[1] = 'S';
    context_info.name[2] = 'I';
    context_info.name[3] = 0;
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, context_id_p));


    /* Set the Presels: Current Layer Protocol and UDH Type*/
    bcm_field_presel_entry_id_info_init(&dnx_evpn_field_info.egr_fg.presel_entry_id);
    dnx_evpn_field_info.egr_fg.presel_entry_id.presel_id = dnx_evpn_field_info.egr_fg.presel_id;
    dnx_evpn_field_info.egr_fg.presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 0;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    presel_entry_data.nof_qualifiers++;

    /** UDH Data Type */
    if (!*dnxc_data_get(unit, "field", "features", "udh_base_cs_is_mapped", NULL)) {
        presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyUDHBase3;
        presel_entry_data.qual_data[1].qual_arg = 0;
        presel_entry_data.qual_data[1].qual_value = evpn_esi_udh_base_3;
        presel_entry_data.qual_data[1].qual_mask = 0x3;
    }
    else
    {
        /* In devices with mapping from ePMF UDH Base to UDH Base CS we use bcmFieldQualifyUDHBaseFieldCs qualifier */
        uint32 epmf_udh_base_cs_var;
        bcm_switch_control_key_t key;
        bcm_switch_control_info_t value;

        epmf_udh_base_cs_var = 0xf;
        /* Create mapping from UDH Base to UDH Base CS var for context selection */
        key.type = bcmSwitchEgressUDHBaseFieldCsMap;
        /* UDHBase3 is on the MSB of UDH Base */
        key.index = evpn_esi_udh_base_3 << 6;
        value.value = epmf_udh_base_cs_var;
        BCM_IF_ERROR_RETURN(bcm_switch_control_indexed_set(unit, key, value));

        presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyUDHBaseFieldCs;
        presel_entry_data.qual_data[1].qual_arg = 0;
        presel_entry_data.qual_data[1].qual_value = epmf_udh_base_cs_var;
        presel_entry_data.qual_data[1].qual_mask = 0xf;
    }
    presel_entry_data.nof_qualifiers++;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &dnx_evpn_field_info.egr_fg.presel_entry_id, &presel_entry_data));


    return BCM_E_NONE;
}


int cint_field_evpn_egress_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    int qual_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t bcm_action_id;

    printf("Running: cint_field_evpn_egress_fg_add()\n");

    bcm_field_group_info_t_init(&dnx_evpn_field_info.egr_fg.fg_info);
    dnx_evpn_field_info.egr_fg.fg_info.fg_type = bcmFieldGroupTypeTcam;
    dnx_evpn_field_info.egr_fg.fg_info.stage = bcmFieldStageEgress;

    dnx_evpn_field_info.egr_fg.fg_info.qual_types[0] = bcmFieldQualifyUDHData3;

    dnx_evpn_field_info.egr_fg.fg_info.qual_types[1] = bcmFieldQualifyOutPort;

    dnx_evpn_field_info.egr_fg.fg_info.nof_quals = 2;

    /* Action - Drop */
    /** bcmFieldActionDrop is not supported in some devices. So, use bcmFieldActionTrap instead.
    dnx_evpn_field_info.egr_fg.fg_info.action_types[0] = bcmFieldActionDrop;
    */
    dnx_evpn_field_info.egr_fg.fg_info.action_types[0] = bcmFieldActionTrap;
    dnx_evpn_field_info.egr_fg.fg_info.action_with_valid_bit[0] = TRUE;

    dnx_evpn_field_info.egr_fg.fg_info.nof_actions = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &dnx_evpn_field_info.egr_fg.fg_info, fg_id_p));

    return BCM_E_NONE;
}


int cint_field_evpn_egress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{


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

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));


    return BCM_E_NONE;
}


int cint_field_evpn_egress_default_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_p)
{

    bcm_gport_t trap_gport_drop;
    bcm_field_entry_info_t entry_in_info;

    BCM_GPORT_TRAP_SET(trap_gport_drop, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 7, 0);

    printf("Running: cint_field_evpn_egress_default_entry_add()\n");

    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = dnx_evpn_field_info.egr_fg.fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = 0;
    entry_in_info.entry_qual[0].mask[0] = 0;

    entry_in_info.entry_action[0].type = dnx_evpn_field_info.egr_fg.fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = trap_gport_drop;

    entry_in_info.priority = dnx_evpn_field_info.default_entry_priority;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));


    /** Store the entry handle for destroy convenience.*/
    BCM_IF_ERROR_RETURN(cint_field_evpn_field_entry_handle_store(unit, *entry_handle_p, dnx_evpn_field_info.egr_fg.entry_handle));

    return BCM_E_NONE;
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
    bcm_field_entry_info_t entry_in_info;
    bcm_gport_t gport;
    int pp_port_size;
    int nof_pp_ports;
    int nof_cores;
    uint8 is_trunk = 0;
    bcm_gport_t trap_gport_drop;


    printf("Running: cint_field_evpn_egress_entry_add()\n");

    BCM_GPORT_TRAP_SET(trap_gport_drop, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 7, 0);

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
    /**entry_in_info.entry_action[0].value[0] = is_drop ? 1 : 0;*/
    entry_in_info.entry_action[0].value[0] = trap_gport_drop;

    entry_in_info.priority = evpn_esi_prio++;
    entry_in_info.nof_entry_quals = 2 - (port == BCM_GPORT_INVALID);
    entry_in_info.nof_entry_actions = is_drop ? 1 : 0;
    if (is_trunk)
    {
        entry_in_info.core = 0;
    }
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p));

    /** Store the entry handle for destroy convenience.*/
    BCM_IF_ERROR_RETURN(cint_field_evpn_field_entry_handle_store(unit, *entry_handle_esi_port_p, dnx_evpn_field_info.egr_fg.entry_handle));


    nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    if (is_trunk && (nof_cores > 1))
    {
        entry_in_info.core = 1;
        BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p));

        /** Store the entry handle for destroy convenience.*/
        BCM_IF_ERROR_RETURN(cint_field_evpn_field_entry_handle_store(unit, *entry_handle_esi_port_p, dnx_evpn_field_info.egr_fg.entry_handle));

    }

    return BCM_E_NONE;
}


int cint_field_evpn_egress_esi_port_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_port_p,
        int esi_label,
        int port)
{
    printf("Running: cint_field_evpn_egress_default_entry_add()\n");

    /* Add the Entry (ESI + Port) --> Drop */
    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_entry_add(unit, context_id, fg_id, entry_handle_esi_port_p, esi_label,port, TRUE));

    return BCM_E_NONE;
}

int cint_field_evpn_egress_esi_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_p,
        int esi_label)
{
    bcm_field_entry_info_t entry_in_info;
    bcm_field_group_info_t fg_info;


    printf("Running: cint_field_evpn_egress_default_entry_add()\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_info_get(unit, fg_id, &fg_info));

    /*
     * Add the Entry (ESI) --> Do Nothing.
     */
    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_entry_add(unit, context_id, fg_id, entry_handle_esi_p,esi_label, BCM_GPORT_INVALID, FALSE));

    return BCM_E_NONE;
}

int cint_field_evpn_egress(int unit)
{

    printf("Running: cint_field_evpn_egress()\n");
    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_context_create(unit, &dnx_evpn_field_info.egr_fg.context_id));


    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_fg_add(unit, &dnx_evpn_field_info.egr_fg.fg_id));


    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_fg_attach(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id));


    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_default_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &dnx_evpn_field_info.default_entry_handle));


    return BCM_E_NONE;
}

/*
 * Tcam type of field group was used for comparing inAC.Leaf_indication and MACT.leaf_indication.
 *     qual:{InAC.generic_data[0], MACT.entry_grouping[3]}
 *     action: {DROP}
 */
int cint_field_evpn_etree_ingress_fg_add(int unit)
{
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    char *c_ptr;
    void *void_ptr;

    printf("Running: cint_field_evpn_etree_ingress_fg_add()\n");

    if (dnx_evpn_field_info.inac_leaf_indication_qual == 0)
    {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "inac_leaf_indication_qual", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.inac_leaf_indication_qual));

    }

    if (dnx_evpn_field_info.mact_leaf_indication_qual == 0)
    {
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "mact_leaf_indication_qual", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &dnx_evpn_field_info.mact_leaf_indication_qual));

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
        BCM_IF_ERROR_RETURN(bcm_field_action_create(unit, 0, &action_info, &dnx_evpn_field_info.ingress_drop_action));

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

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &dnx_evpn_field_info.ing_etree_fg.fg_info, &dnx_evpn_field_info.ing_etree_fg.fg_id));


    return BCM_E_NONE;
}

int cint_field_evpn_etree_ingress_fg_attach(int unit)
{


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
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyAcInLifWideData, bcmFieldStageIngressPMF2, &inac_wide_data_qual_info));

    /** Get the MACT.entry_group qual info, we need to qualify its MSB*/
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyDstClassL2, bcmFieldStageIngressPMF2, &mact_entry_group_qual_info));


    qual_idx = 0;
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = inac_wide_data_qual_info.offset + 0; /** inAC.Wide_Data[0]*/
    qual_idx++;

    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = mact_entry_group_qual_info.offset + 3; /** MACT.ENTRY_GROUP[3]*/
    qual_idx++;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, dnx_evpn_field_info.ing_etree_fg.fg_id, dnx_evpn_field_info.ing_etree_fg.context_id, &attach_info));

    return BCM_E_NONE;
}

int cint_field_evpn_etree_ingress_fg_entry_add(int unit)
{
    /********* Add Entries *********/
    int entry_priority = 100;
    bcm_field_entry_info_t entry_in_info;


    printf("Running: cint_field_evpn_etree_ingress_fg_add()\n");

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
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, dnx_evpn_field_info.ing_etree_fg.fg_id, &entry_in_info, &dnx_evpn_field_info.ing_etree_fg.entry_handle[0]));


    return BCM_E_NONE;
}


int cint_field_evpn_etree_ingress(int unit, int inlif_profile)
{
    printf("Running: cint_field_evpn_etree_ingress()\n");
    /** Initialize field information for EVPN*/
    if (dnx_evpn_field_info.initialized != 1)
    {
        cint_field_evpn_field_info_init(unit);
    }

    BCM_IF_ERROR_RETURN(cint_field_evpn_ingress_context_create(unit, &dnx_evpn_field_info.ing_etree_fg.context_id, inlif_profile, 0,
                                                dnx_evpn_field_info.ing_etree_fg.presel_id, &dnx_evpn_field_info.ing_etree_fg.presel_entry_id));


    BCM_IF_ERROR_RETURN(cint_field_evpn_etree_ingress_fg_add(unit));


    BCM_IF_ERROR_RETURN(cint_field_evpn_etree_ingress_fg_attach(unit));


    BCM_IF_ERROR_RETURN(cint_field_evpn_etree_ingress_fg_entry_add(unit));


    return BCM_E_NONE;
}


int cint_field_evpn_etree_egress_leaf_port_entry_add(
        int unit,
        int leaf_label,
        int port)
{


    bcm_field_entry_t entry_handle;

    printf("Running: cint_field_evpn_etree_egress_leaf_port_entry_add()\n");

    BCM_IF_ERROR_RETURN(cint_field_evpn_egress_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id,
                                          dnx_evpn_field_info.egr_fg.fg_id, &entry_handle,leaf_label, port, FALSE));
    

    return BCM_E_NONE;
}

int cint_field_evpn_destroy_all_entries(int unit)
{


    int entry_idx;

    printf("Running: cint_field_evpn_destroy_all_entries()\n");

    /* Destroy all the entries */
    for (entry_idx = 0; entry_idx < max_nof_entry_handles; entry_idx++)
    {
        printf("Index: %d",entry_idx);

        if (dnx_evpn_field_info.ing_fg.entry_handle[entry_idx] != 0)
        {
            printf(" FG(%d)",dnx_evpn_field_info.ing_fg.fg_id);
            BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,dnx_evpn_field_info.ing_fg.fg_id, NULL, dnx_evpn_field_info.ing_fg.entry_handle[entry_idx]));
        }

        if (dnx_evpn_field_info.ing_fg2.entry_handle[entry_idx] != 0)
        {
            printf(" FG(%d)",dnx_evpn_field_info.ing_fg2.fg_id);
            BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,dnx_evpn_field_info.ing_fg2.fg_id, NULL, dnx_evpn_field_info.ing_fg2.entry_handle[entry_idx]));
        }

        if (dnx_evpn_field_info.egr_fg.entry_handle[entry_idx] != 0)
        {
            printf(" FG(%d)",dnx_evpn_field_info.egr_fg.fg_id);
            BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,dnx_evpn_field_info.egr_fg.fg_id, NULL, dnx_evpn_field_info.egr_fg.entry_handle[entry_idx]));

        }

        if (dnx_evpn_field_info.ing_etree_fg.entry_handle[entry_idx] != 0)
        {
            printf(" FG(%d)",dnx_evpn_field_info.ing_etree_fg.fg_id);
            BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,dnx_evpn_field_info.ing_etree_fg.fg_id, NULL, dnx_evpn_field_info.ing_etree_fg.entry_handle[entry_idx]));
        }

        if (dnx_evpn_field_info.egr_etree_fg.entry_handle[entry_idx] != 0)
        {
            printf(" FG(%d)",dnx_evpn_field_info.egr_etree_fg.fg_id);
            BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,dnx_evpn_field_info.egr_etree_fg.fg_id, NULL, dnx_evpn_field_info.egr_etree_fg.entry_handle[entry_idx]));
        }
        printf("\n");
    }

    return BCM_E_NONE;
}

int cint_field_evpn_destroy_udq_and_uda(int unit)
{
    int index;
    bcm_field_qualify_t udq[max_nof_entry_handles] = {0};
    bcm_field_action_t uda[max_nof_entry_handles] = {0};

    printf("Running: cint_field_evpn_destroy_udq_and_uda()\n");

    udq[0] = dnx_evpn_field_info.esi_type_qual;
    udq[1] = dnx_evpn_field_info.esi_qual;
    udq[2] = dnx_evpn_field_info.iml_range_profile_qual;
    udq[3] = dnx_evpn_field_info.inac_leaf_indication_qual;
    udq[4] = dnx_evpn_field_info.mact_leaf_indication_qual;
    udq[5] = dnx_evpn_field_info.esi_valid_qual;

    uda[0] = dnx_evpn_field_info.esi_action;
    uda[1] = dnx_evpn_field_info.ingress_drop_action;
    uda[2] = dnx_evpn_field_info.esi_indication_action;

    for (index = 0; index < max_nof_entry_handles; index++)
    {
        if (udq[index] != 0)
        {
            BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, udq[index]));

        }

        if (uda[index] != 0)
        {
            BCM_IF_ERROR_RETURN(bcm_field_action_destroy(unit, uda[index]));

        }
    }

    return BCM_E_NONE;
}

int cint_field_evpn_fg_destroy(int unit, evpn_fg_info_s fg_info)
{

    int index;
    bcm_field_presel_entry_data_t presel_entry_data;

    printf("Running: cint_field_evpn_fg_destroy()\n");

    if (fg_info.fg_id == 0)
    {
        /** Suppose nothing need to do for it*/
        return BCM_E_NONE;
    }

    printf("destroy entries");
    for (index = 0; index < max_nof_entry_handles; index++)
    {
        if (fg_info.entry_handle[index] != 0)
        {
            printf(" Index(%d)",index);
            BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,fg_info.fg_id, NULL, fg_info.entry_handle[index]));
        }
    }
    printf("\n");

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, fg_info.fg_id, fg_info.context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, fg_info.fg_id));

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &fg_info.presel_entry_id, &presel_entry_data));

    BCM_IF_ERROR_RETURN(bcm_field_context_destroy(unit, fg_info.presel_entry_id.stage, fg_info.context_id));

    sal_memset(&fg_info, 0, sizeof(fg_info));

    return BCM_E_NONE;
}


int cint_field_evpn_destroy_all(int unit)
{
    printf("Running: cint_field_evpn_destroy_all()\n");

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_fg));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.egr_fg));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_pmf1_fg));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_pmf2_fg1));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_pmf2_fg2));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.ing_etree_fg));

    BCM_IF_ERROR_RETURN(cint_field_evpn_fg_destroy(unit, dnx_evpn_field_info.egr_etree_fg));

    BCM_IF_ERROR_RETURN(cint_field_evpn_destroy_udq_and_uda(unit));

    return BCM_E_NONE;
}

