/*
 * Configuration example start:
 *
 cint ../../../src/examples/dnx/field/cint_field_srv6_evpn_esi.c
 cint ../../../../src/examples/dnx/field/cint_field_srv6_evpn_esi.c
 cint
 cint_field_srv6_evpn_ingress(0,0);

*/

/* Global Vars */

/** EPMF EVPN Filtering by ESI Activation */
int EVPN_ESI_UDH_BASE_3 = 0x3;    /** UDH3 Type to active EVPN filtering by ESI (2/2 pass or 1/1 pass) */

/*
 * EPMF BTR Activation
 */
int EVPN_IDLE_UDH_BASE_3 = 0x1;   /** UDH3 Type to skip EVPN filtering (1/2 pass) */
int EVPN_LE_BTR_UDH_BASE_2 = 0x3; /** UDH2 Type to activate BTR compensation (1/2 pass currently for J2P) */

/** Size of egress additional termination in bytes */
int SRV6_TERMINATION_ARRAY_SIZE = 24;

int termination_size[SRV6_TERMINATION_ARRAY_SIZE] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 150, 150, 150, 150 };


/** List of 1st Pass Ingress FG types */
enum field_srv6_evpn_ipmf_1st_pass_e
{
    FIRST_PASS_IPV6,
    FIRST_PASS_SRV6
};

int NOF_FIRST_PASS_INSTANCES = FIRST_PASS_SRV6+1;

bcm_field_entry_t default_entry_handle;
int default_entry_priority = 50000;

bcm_field_entry_t entry_handle_esi_port;
bcm_field_entry_t entry_handle_esi;

bcm_field_entry_t entry_handle_esi_port_1;
bcm_field_group_t egress_fg_id;
bcm_field_group_t egress_btr_fg_id;
bcm_field_group_t ingress_2nd_pass_fg_id;
bcm_field_group_t ingress_1st_pass_ipv6_fg_id;
bcm_field_group_t ingress_1st_pass_srv6_fg_id;

bcm_field_group_info_t egress_fg_info;
bcm_field_group_info_t ingress_2nd_pass_fg_info;

bcm_field_context_t egress_context_id;
bcm_field_context_t egress_btr_context_id;
bcm_field_context_t ingress_context_2nd_pass_id;
bcm_field_context_t ingress_context_1st_pass_ipv6_id;
bcm_field_context_t ingress_context_1st_pass_srv6_id;

bcm_field_presel_t ingress_2nd_pass_presel_id = 61;
bcm_field_presel_t ingress_1st_pass_presel_ipv6_id = 64;
bcm_field_presel_t ingress_1st_pass_presel_srv6_id = 13;  /** Higher priority than USD*/
bcm_field_presel_t egress_presel_id = 65;
bcm_field_presel_t egress_btr_presel_id = 67;

bcm_field_presel_entry_id_t ingress_2nd_pass_presel_entry_id;
bcm_field_presel_entry_id_t ingress_1st_pass_presel_ipv6_entry_id;
bcm_field_presel_entry_id_t ingress_1st_pass_presel_srv6_entry_id;
bcm_field_presel_entry_id_t egress_presel_entry_id;

bcm_field_qualify_t esi_qual = 0;
bcm_field_qualify_t esi_type_qual = 0;
bcm_field_action_t esi_action;

char *qual_udh_type_msb_name[NOF_FIRST_PASS_INSTANCES] = {"esi_typ_q_msb_ip", "esi_typ_q_msb_sr"};
char *qual_udh_type_lsb_name[NOF_FIRST_PASS_INSTANCES] = {"esi_typ_q_lsb_ip", "esi_typ_q_lsb_sr"};
char *qual_udh_data_name[NOF_FIRST_PASS_INSTANCES] = {"esi_dat_q_ip", "esi_dat_q_sr"};
char *user_action_name[NOF_FIRST_PASS_INSTANCES] = {"usr_act_ip_udh_esi", "usr_act_sr_udh_esi"};


/** */
bcm_field_presel_t srv6_evpn_esi_encap_egress_presel_id = 68;

struct cint_field_srv6_evpn_esi_encap_info_s {
    bcm_field_context_t egress_context_id;
    bcm_field_presel_t egress_presel_id;
    bcm_field_group_info_t egress_fg_info;
    bcm_field_group_t egress_fg_id;

    int nof_esi_encap_egress_entries;
    bcm_field_entry_t egress_entry_handles[10];

    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_id[10];

    uint8 fg_initialized;
};

cint_field_srv6_evpn_esi_encap_info_s field_srv6_evpn_esi_encap_info = { 0 };


/*
 * DirectExtract type of field group was used for constructing UDH and UDH-BASE.
 *     qual:{ESI_LABEL(16b)}
 *     action: {UDH4[19:0],UDH-BASE4[1:0]}
 */
int cint_field_srv6_evpn_ingress_2nd_pass_rch_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    char *c_ptr;
    void *void_ptr;

    printf("Running: cint_field_srv6_evpn_ingress_2nd_pass_rch_fg_add()\n");


    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_type_qual", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &esi_type_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_qual", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &esi_qual));


    bcm_field_group_info_t_init(&ingress_2nd_pass_fg_info);
    ingress_2nd_pass_fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    ingress_2nd_pass_fg_info.stage = bcmFieldStageIngressPMF2;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;

    ingress_2nd_pass_fg_info.qual_types[qual_idx] = esi_type_qual;
    qual_idx++;

    ingress_2nd_pass_fg_info.qual_types[qual_idx] = esi_qual;
    qual_idx++;

    ingress_2nd_pass_fg_info.nof_quals = qual_idx;


    /* Create UDA for a 16-bit ESI. UDA is required as the default size of the UDHData3 is 32bit. */
    bcm_field_action_info_t_init(&action_info);
    /**
    * Fill the structure needed for bcm_field_qualifier_create API
    */
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData3;
    action_info.size = 16;
    action_info.prefix_size=16;
    action_info.prefix_value=0;
    void_ptr = action_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "UDHData3", sizeof(action_info.name));
    /**
    * Calling the API bcm_field_action_create for positive case
    */
    bcm_field_action_create(unit, 0, &action_info, &esi_action);


    /**Same for the actions to move them*/
    act_idx = 0;
    /** Order of actions should be the same as order of qualifiers*/
    ingress_2nd_pass_fg_info.action_types[act_idx] = bcmFieldActionUDHBase3;
    ingress_2nd_pass_fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    ingress_2nd_pass_fg_info.action_types[act_idx] = esi_action;
    ingress_2nd_pass_fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    ingress_2nd_pass_fg_info.nof_actions = act_idx;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &ingress_2nd_pass_fg_info, fg_id_p));
    return BCM_E_NONE;
}

/*
 * DirectExtract type of field group was used for constructing UDH and UDH-BASE.
 *     qual:{ESI_LABEL(16b)}
 *     action: {UDH4[19:0],UDH-BASE4[1:0]}
 */
int cint_field_srv6_evpn_ingress_1st_pass_fg_add(int unit, bcm_field_context_t context_id,
                                                     bcm_field_group_t *fg_id_p, int first_pass_case,
                                                     int btr_compensation)
{
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_group_info_t fg_info;
    bcm_field_action_t udh3_esi_data_user_action;
    bcm_field_action_t udh2_btr_le_data_user_action;
    bcm_field_qualify_t esi_udh3_data_qual = 0;
    bcm_field_qualify_t esi_type_msb_qual = 0;
    bcm_field_qualify_t esi_type_lsb_qual = 0;
    bcm_field_qualify_t btr_le_udh2_data_qual = 0;
    bcm_field_qualify_t btr_le_udh2_type_qual=0;
    char *c_ptr;
    void *void_ptr;

    int srv6_btr_compensation = btr_compensation && (first_pass_case == FIRST_PASS_SRV6);
    printf("Running: cint_field_srv6_evpn_ingress_1st_pass_fg_add()\n");


    /*
     * The UDH type MSB bit which is always constant '1'
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, qual_udh_type_msb_name[first_pass_case], sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &esi_type_msb_qual));


    /*
     * The UDH type LSB bit which:
     * - in IPv6 or in SRv6 with SRH 1-Pass process is '1'
     * - in SRv6 with SRH 2-pass process is '0'
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, qual_udh_type_lsb_name[first_pass_case], sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &esi_type_lsb_qual));

    /*
     * ESI qualifier
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, qual_udh_data_name[first_pass_case], sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &esi_udh3_data_qual));

    if (srv6_btr_compensation)
    {
        /*
         * SRH LE field, for BTR estimation in J2P
         */
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 8;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "srv6_evpn_ext_term_btr_data", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &btr_le_udh2_data_qual));

        /*
         * UDH2 BaseType for SRH LE and BTR calculation
         */
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 2;
        void_ptr = qual_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "srv6_evpn_ext_term_btr_type", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &btr_le_udh2_type_qual));


    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;

    fg_info.qual_types[qual_idx] = esi_type_msb_qual;
    qual_idx++;

    fg_info.qual_types[qual_idx] = esi_type_lsb_qual;
    qual_idx++;

    fg_info.qual_types[qual_idx] = esi_udh3_data_qual;
    qual_idx++;

    if (srv6_btr_compensation)
    {
        fg_info.qual_types[qual_idx] = btr_le_udh2_data_qual;
        qual_idx++;

        fg_info.qual_types[qual_idx] = btr_le_udh2_type_qual;
        qual_idx++;
    }

    fg_info.nof_quals = qual_idx;


    /* Create UDA for a 16-bit ESI. UDA is required as the default size of the UDHData3 is 32bit. */
    bcm_field_action_info_t_init(&action_info);
    /**
    * Fill the structure needed for bcm_field_qualifier_create API
    */
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData3;
    action_info.size = 16;
    action_info.prefix_size=16;
    action_info.prefix_value=0;
    void_ptr = action_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, user_action_name[first_pass_case], sizeof(action_info.name));
    /**
    * Calling the API bcm_field_action_create for positive case
    */
    BCM_IF_ERROR_RETURN(bcm_field_action_create(unit, 0, &action_info, &udh3_esi_data_user_action));

    if (srv6_btr_compensation)
    {
        /* Create UDA for a 8-bit LE. UDA is required as the default size of the UDHData3 is 32bit. */
        bcm_field_action_info_t_init(&action_info);
        /**
        * Fill the structure needed for bcm_field_qualifier_create API
        */
        action_info.stage = bcmFieldStageIngressPMF2;
        action_info.action_type = bcmFieldActionUDHData2;
        action_info.size = 8;
        action_info.prefix_size=24;
        action_info.prefix_value=0;
        void_ptr = action_info.name;
        c_ptr = void_ptr;
        sal_strncpy_s(c_ptr, "usr_act_sr_udh_le", sizeof(action_info.name));
        /**
        * Calling the API bcm_field_action_create for positive case
        */
        BCM_IF_ERROR_RETURN(bcm_field_action_create(unit, 0, &action_info, &udh2_btr_le_data_user_action));
    }


    /**Same for the actions to move them*/
    act_idx = 0;
    /** Order of actions should be the same as order of qualifiers*/
    fg_info.action_types[act_idx] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    fg_info.action_types[act_idx] = udh3_esi_data_user_action;
    fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    if (srv6_btr_compensation)
    {
        fg_info.action_types[act_idx] = udh2_btr_le_data_user_action;
        fg_info.action_with_valid_bit[act_idx] = FALSE;
        act_idx++;

        fg_info.action_types[act_idx] = bcmFieldActionUDHBase2;
        fg_info.action_with_valid_bit[act_idx] = FALSE;
        act_idx++;
    }

    fg_info.nof_actions = act_idx;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, fg_id_p));

    qual_idx = 0;
    act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(qual_idx=0; qual_idx < fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx < fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = fg_info.action_types[act_idx];
    }

    qual_idx = 0;


    /** UDH Data Type MSB bit - '1' constant */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = 1;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /*
     * UDH Data Type LSB bit - '1' constant for most cases
     * - only in case of SRv6 layer presense - look at SRv6 Lyr Qualifier[3] to set it
     */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = 1;
    attach_info.key_info.qual_info[qual_idx].offset = 0;

    /** This is relevant only in devices that do ITPP cut and terminate SRv6 lyr as well (FWD Lyr = SRv6 + 1)*/
    if (first_pass_case == FIRST_PASS_SRV6)
    {
        /**Get the offset (within the layer record) of the layer offset (within the header).*/
        bcm_field_qualifier_info_get_t layer_qual_info;
        BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF2, &layer_qual_info));

        printf("\n\n layer_qual_info: offset = %d, size = %d\n\n", layer_qual_info.offset, layer_qual_info.size);

        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerRecordsFwd;
        attach_info.key_info.qual_info[qual_idx].input_arg = -1;
        attach_info.key_info.qual_info[qual_idx].offset = layer_qual_info.offset + 7;  /** take layer_qualifiers[fwd - 1][7] */
    }

    qual_idx++;

    /** Take the ESI from IPv6 DIP 16b LSB - header idx 1, and offset from MSB 320-16b */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[qual_idx].input_arg = 1;
    attach_info.key_info.qual_info[qual_idx].offset = 304;
    qual_idx++;

    if (srv6_btr_compensation)
    {
        /** Take the LE from SRH header, offset 32 from MSB */
        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
        attach_info.key_info.qual_info[qual_idx].input_arg = -1;
        attach_info.key_info.qual_info[qual_idx].offset = 320 + 32;
        qual_idx++;

        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[qual_idx].input_arg = EVPN_LE_BTR_UDH_BASE_2;
        qual_idx++;
    }


    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, *fg_id_p, context_id, &attach_info));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_ingress_2nd_pass_rch_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = ingress_2nd_pass_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = ingress_2nd_pass_fg_info.nof_actions;

    for(qual_idx=0; qual_idx < ingress_2nd_pass_fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = ingress_2nd_pass_fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx < ingress_2nd_pass_fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = ingress_2nd_pass_fg_info.action_types[act_idx];
    }

    qual_idx = 0;


    /** UDH Data Type */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = EVPN_ESI_UDH_BASE_3;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /**Destination 16 bits from ESI (in RCH header, from field type_specific_container) */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = 176; /** offset of type_specific_container in RCH*/
    qual_idx++;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    return BCM_E_NONE;
}


/*
 * This context is created for the 2nd Pass flow in SRv6 USP termination, to copy ESI from RCH header.
 * Done in any device when does 2-Pass SRv6 USP termination.
 * Presel Qualifiers:
 *  - layer 0 is RCH
 *  - LIF (reclassified from 1st Pass) is EVPN
 */
int cint_field_srv6_evpn_ingress_2nd_pass_rch_context_create(int unit, bcm_field_context_t * context_id_p, int inlif_profile)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    printf("Running: cint_field_srv6_evpn_ingress_2nd_pass_rch_context_create()\n");
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_evpn_2nd_pass_pmf1", sizeof(context_info.name));

    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id_p));


    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. Context created for iPMF1 is also created for iPMF2*/
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);

    BCM_IF_ERROR_RETURN(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id_p, &param_info));

    bcm_field_presel_entry_id_info_init(&ingress_2nd_pass_presel_entry_id);
    /**For iPMF2, iPMF1 presel must be configured*/
    ingress_2nd_pass_presel_entry_id.presel_id = ingress_2nd_pass_presel_id;
    ingress_2nd_pass_presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    /*
     * LIF of EVPN type is recognized by inlif_profile 2bits for PMF
     */

    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = inlif_profile;
    presel_entry_data.qual_data[0].qual_mask = 0xF;

    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = -1;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeRch;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &ingress_2nd_pass_presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}

/*
 * This context is created for the 1st Pass Ingress PMF for several cases:
 * 1. No SRH, IPv6 only (1-Pass IPv6 termination)
 *    - copy ESI from IPv6 DIP to UDH3
 *    - set UDH3 type to ESI (activate EPMF for ESI filtering)
 * 2. With SRH (1st pass, for devices that don't use IPMF for 2-pass USP in 2-pass termination)
 *    - copy ESI from IPv6 DIP to UDH3
 *    - set UDH3 type to regular Data Type (not activate EPMF in this pass for ESI filtering)
 *
 * Presel Qualifiers - 2 Presels would lead to same Context
 * 1.
 *  - (FWD-layer-1) is IPv6 (case of IPv6 termination 1-pass of 1-pass, no SRH)
 *  - LIF (reclassified from 1st Pass) is EVPN
 * 2.
 *  - (FWD-layer-1) is SRv6 (case of SRv6 termination 1-pass of 2-pass in devices that terminate SRv6 in VTT5)
 *  - LIF (reclassified from 1st Pass) is EVPN
 *
 * Actions
 * 1. In first case - UDH Type set to 3 (ESI type) cause it's 1-pass of 1-pass
 * 2. In 2nd case - UDH type set to 2 cause it's 1-pass of 2-pass
 * --- The difference would be chosen by EFES machine ----
 *  - If (FWD-Layer -1) == SRv6 && 2-Pass Qualifier set --> UDH Type = 2 (no ESI filtering)
 *    else: UDH Type = 3 (ESI filtering)
 */
int cint_field_srv6_evpn_ingress_1st_pass_context_create(int unit, bcm_field_context_t * context_id_p,
                                                         bcm_field_presel_t presel_id,
                                                         bcm_field_presel_entry_id_t *presel_entry_id_p,
                                                         int inlif_profile, int with_srh)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    printf("Running: cint_field_srv6_evpn_ingress_1st_pass_context_create()\n");

    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);

    if (with_srh == 0)
    {
        sal_strncpy_s(dest_char, "srv6_evpn_no_srh_one_pass_pmf1", sizeof(context_info.name));
    }
    else if (with_srh == 1)
    {
        sal_strncpy_s(dest_char, "srv6_evpn_with_srh_first_pass_pmf1", sizeof(context_info.name));
    }


    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id_p));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. Context created for iPMF1 is also created for iPMF2*/
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);

    BCM_IF_ERROR_RETURN(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id_p, &param_info));

    bcm_field_presel_entry_id_info_init(presel_entry_id_p);
    /**For iPMF2, iPMF1 presel must be configured - Presel1 - FWD-1 is IPv6, LIF is EVPN */
    presel_entry_id_p->presel_id = presel_id;
    presel_entry_id_p->stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    /*
     * LIF of EVPN type is recognized by inlif_profile 2bits for PMF
     */

    presel_entry_data.nof_qualifiers = 2;
    if (with_srh == 1)
    {
        presel_entry_data.nof_qualifiers = 3;
    }

    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = inlif_profile;
    presel_entry_data.qual_data[0].qual_mask = 0xF;

    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = -1;
    presel_entry_data.qual_data[1].qual_value =  bcmFieldLayerTypeIp6;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    if (with_srh == 1)
    {
        presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
        presel_entry_data.qual_data[2].qual_arg = -1;
        presel_entry_data.qual_data[2].qual_value =  0x2 | (0xb << 2);
        presel_entry_data.qual_data[2].qual_mask = 0x2 | (0x1f << 2);
    }

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, presel_entry_id_p, &presel_entry_data));


    return BCM_E_NONE;
}


int cint_field_srv6_evpn_ingress(int unit, int inlif_profile)
{
    int btr_compensation = 0;

    printf("Running: cint_field_srv6_evpn_ingress()\n");

    /*
     * Define Ingress PMF 2nd Pass Context
     * -copy ESI from RCH to UDH3
     * -set UDH3 type as ESI type (activate EPMF for ESI filtering)
     */
    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_2nd_pass_rch_context_create(unit, &ingress_context_2nd_pass_id, inlif_profile));

    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_2nd_pass_rch_fg_add(unit, &ingress_2nd_pass_fg_id));

    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_2nd_pass_rch_fg_attach(unit, ingress_context_2nd_pass_id, ingress_2nd_pass_fg_id));

    /*
     * Define Ingress PMF 1st Pass Context, for case:
     * 1. No SRH, IPv6 only (1-Pass)
     *    - copy ESI from IPv6 DIP to UDH3
     *    - set UDH3 type to ESI (activate EPMF for ESI filtering)
     */
    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_1st_pass_context_create(unit, &ingress_context_1st_pass_ipv6_id,
                                                             ingress_1st_pass_presel_ipv6_id,
                                                             &ingress_1st_pass_presel_ipv6_entry_id,
                                                             inlif_profile, 0));

   /*
    * Define Ingress PMF 1st Pass Context, for case:
    * 2. With SRH (1st pass in 1-pass process)
    *    - copy ESI from IPv6 DIP to UDH3
    *    - set UDH3 type to ESI (activate EPMF for ESI filtering)
    *    With SRH (1st pass in 2-pass process)
    *    - copy ESI from IPv6 DIP to UDH3
    *    - set UDH3 type to Normal mode (not set EPMF for ESI filtering)
    */
    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_1st_pass_context_create(unit, &ingress_context_1st_pass_srv6_id,
                                                             ingress_1st_pass_presel_srv6_id,
                                                             &ingress_1st_pass_presel_srv6_entry_id,
                                                             inlif_profile, 1));


    /** check if device requires estimated BTR */
    btr_compensation = *dnxc_data_get(unit, "dev_init", "general", "egress_estimated_bta_btr_hw_config", NULL);


    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_1st_pass_fg_add(unit, ingress_context_1st_pass_ipv6_id,
                                                          &ingress_1st_pass_ipv6_fg_id, FIRST_PASS_IPV6, btr_compensation));


    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_ingress_1st_pass_fg_add(unit, ingress_context_1st_pass_srv6_id,
                                                          &ingress_1st_pass_srv6_fg_id, FIRST_PASS_SRV6, btr_compensation));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_egress_context_create(int unit, bcm_field_context_t *context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    printf("Running: cint_field_srv6_evpn_egress_context_create()\n");

    bcm_field_context_info_t_init(&context_info);
    context_info.name[0] = 'E';
    context_info.name[1] = 'S';
    context_info.name[2] = 'I';
    context_info.name[3] = 0;
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, context_id_p));


    /* Set the Presels: Current Layer Protocol and UDH Type*/
    bcm_field_presel_entry_id_info_init(&egress_presel_entry_id);
    egress_presel_entry_id.presel_id = egress_presel_id;
    egress_presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;


    /** UDH Data Type */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase3;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = EVPN_ESI_UDH_BASE_3;
    presel_entry_data.qual_data[0].qual_mask = 0x3;


    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &egress_presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_egress_btr_comp_context_create(int unit, bcm_field_context_t *context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    printf("Running: cint_field_srv6_evpn_egress_btr_comp_context_create()\n");

    bcm_field_context_info_t_init(&context_info);
    context_info.name[0] = 'B';
    context_info.name[1] = 'T';
    context_info.name[2] = 'R';
    context_info.name[3] = '\0';
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, context_id_p));

    /*
     * Set the Presels: UDH2 is BTR compensation type; UDH3 is non ESI filtering type (not final pass); UDH1,2 off
     */
    bcm_field_presel_entry_id_info_init(&egress_presel_entry_id);
    egress_presel_entry_id.presel_id = egress_btr_presel_id;
    egress_presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 4;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;


    /** UDH3 Type - Non ESI filtering (not final round of termination) */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase3;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = EVPN_IDLE_UDH_BASE_3;
    presel_entry_data.qual_data[0].qual_mask = 0x3;

    /** UDH2 Type - BTR compensation */
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyUDHBase2;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = EVPN_LE_BTR_UDH_BASE_2;
    presel_entry_data.qual_data[1].qual_mask = 0x3;

    /** UDH1 Type - NULL */
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyUDHBase1;
    presel_entry_data.qual_data[2].qual_arg = 0;
    presel_entry_data.qual_data[2].qual_value = 0;
    presel_entry_data.qual_data[2].qual_mask = 0x3;

    /** UDH0 Type - NULL */
    presel_entry_data.qual_data[3].qual_type = bcmFieldQualifyUDHBase0;
    presel_entry_data.qual_data[3].qual_arg = 0;
    presel_entry_data.qual_data[3].qual_value = 0;
    presel_entry_data.qual_data[3].qual_mask = 0x3;


    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &egress_presel_entry_id, &presel_entry_data));

    return BCM_E_NONE;
}


int cint_field_srv6_evpn_egress_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    int qual_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t bcm_action_id;

    printf("Running: cint_field_srv6_evpn_egress_fg_add()\n");

    bcm_field_group_info_t_init(&egress_fg_info);
    egress_fg_info.fg_type = bcmFieldGroupTypeTcam;
    egress_fg_info.stage = bcmFieldStageEgress;

    egress_fg_info.qual_types[0] = bcmFieldQualifyUDHData3;

    egress_fg_info.qual_types[1] = bcmFieldQualifyOutPort;

    egress_fg_info.nof_quals = 2;

    /* Action - Drop */
    egress_fg_info.action_types[0] = bcmFieldActionDrop;
    egress_fg_info.action_with_valid_bit[0] = TRUE;

    egress_fg_info.nof_actions = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &egress_fg_info, fg_id_p));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_egress_btr_comp_fg_add_attach(int unit, bcm_field_context_t context_id)
{
    int qual_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t bcm_action_id;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t egress_btr_fg_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_t ent_id;

    int last_entry;

    printf("Running: cint_field_srv6_evpn_egress_btr_comp_fg_add_attach()\n");

    bcm_field_group_info_t_init(&egress_btr_fg_info);
    egress_btr_fg_info.fg_type = bcmFieldGroupTypeTcam;
    egress_btr_fg_info.stage = bcmFieldStageEgress;

    /** Qual - the SRH LE is in the UDH2 */
    egress_btr_fg_info.qual_types[0] = bcmFieldQualifyUDHData2;
    egress_btr_fg_info.nof_quals = 1;

    /* Action - Drop */
    egress_btr_fg_info.action_types[0] = bcmFieldActionEstimatedStartPacketStrip;
    egress_btr_fg_info.nof_actions = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &egress_btr_fg_info, &fg_id));

    /*
     * FG attach
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = egress_btr_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = egress_btr_fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = egress_btr_fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = egress_btr_fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    for (last_entry = 0; last_entry < SRV6_TERMINATION_ARRAY_SIZE; last_entry++)
    {
        /*
         * Add entry
         */
        bcm_field_entry_info_t_init(&ent_info);
        ent_info.priority = 1;

        ent_info.nof_entry_quals = 1;
        ent_info.entry_qual[0].type = egress_btr_fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = last_entry;
        ent_info.entry_qual[0].mask[0] = 0x1F;

        ent_info.nof_entry_actions = 1;
        ent_info.entry_action[0].type = egress_btr_fg_info.action_types[0];
        ent_info.entry_action[0].value[0] = termination_size[last_entry];

        BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &ent_info, &ent_id));
    }

    return BCM_E_NONE;
}


int cint_field_srv6_evpn_egress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = egress_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = egress_fg_info.nof_actions;

    for(qual_idx=0; qual_idx < egress_fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = egress_fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx < egress_fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = egress_fg_info.action_types[act_idx];
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


int cint_field_srv6_evpn_egress_default_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_p)
{
    bcm_field_entry_info_t entry_in_info;

    printf("Running: cint_field_srv6_evpn_egress_default_entry_add()\n");

    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = egress_fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = 0;
    entry_in_info.entry_qual[0].mask[0] = 0;

    entry_in_info.entry_action[0].type = egress_fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 1;

    entry_in_info.priority = default_entry_priority;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));

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

int cint_field_srv6_evpn_egress_esi_port_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_port_p,
        int esi_label,
        int port)
{


    bcm_field_entry_info_t entry_in_info;
    bcm_gport_t gport;
    int pp_port_size;
    int nof_pp_ports;


    printf("Running: cint_field_srv6_evpn_egress_esi_port_entry_add()\n");

    printf("Adding to EPMF Port:%d, ESI:%d\n",port, esi_label);

    if (BCM_GPORT_IS_SET(port))
    {
        gport = port;
    }
    else
    {
        BCM_GPORT_LOCAL_SET(gport, port);
    }

    printf("Adding to EPMF GPort:%d, ESI:%d\n",gport, esi_label);

    nof_pp_ports = *dnxc_data_get(unit, "port", "general", "nof_pp_ports", NULL);
    pp_port_size = log2_round_up(nof_pp_ports);

    /* Add the Entry (ESI + Port) --> Drop */
    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = egress_fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = esi_label;
    entry_in_info.entry_qual[0].mask[0] = 0xfffff;

    entry_in_info.entry_qual[1].type = egress_fg_info.qual_types[1];
    entry_in_info.entry_qual[1].value[0] = gport;
    entry_in_info.entry_qual[1].mask[0] = (1 << pp_port_size) - 1;

    entry_in_info.entry_action[0].type = egress_fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 1;

    entry_in_info.priority = 10;
    entry_in_info.nof_entry_quals = 2;
    entry_in_info.nof_entry_actions = 1;
    if (BCM_GPORT_IS_TRUNK(gport))
    {
        entry_in_info.core = 0;
    }

    uint32 flags = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    /** find the correct core for the Port, to not get duplication of PP_Port which can be same on different cores */
    BCM_IF_ERROR_RETURN(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
    entry_in_info.core = mapping_info.core;

    /** since the qualifier to action bcmFieldQualifyOutPort that's used as qualifier, Gport is later converted to PP_Port */
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_egress_esi_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_p,
        int esi_label)
{
    bcm_field_entry_info_t entry_in_info;
    bcm_field_group_info_t fg_info;


    printf("Running: cint_field_srv6_evpn_egress_esi_entry_add()\n");
    bcm_field_group_info_get( unit, fg_id, &fg_info);

    /*
     * Add the Entry (ESI) --> Do Nothing.
     */
    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = esi_label;
    entry_in_info.entry_qual[0].mask[0] = 0xfffff;

    entry_in_info.entry_action[0].type = fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 0;

    entry_in_info.priority = 15;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_p));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_egress(int unit)
{
    int btr_compensation = 0;

    printf("Running: cint_field_srv6_evpn_egress()\n");

    /*
     * configure EVPN filtering, that would occur on 1/1 or 2/2 pass, final round of termination
     */

    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_egress_context_create(unit, &egress_context_id));

    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_egress_fg_add(unit, &egress_fg_id));

    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_egress_fg_attach(unit, egress_context_id, egress_fg_id));


    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_egress_default_entry_add(unit, egress_context_id, egress_fg_id, &default_entry_handle));

    /*
     * configure BTR compensation for devices that need it, on 1/2 pass of extended termination
     */

    /** check if device requires estimated BTR */
    btr_compensation = *dnxc_data_get(unit, "dev_init", "general", "egress_estimated_bta_btr_hw_config", NULL);
    printf("btr_compensation: %d \n",btr_compensation);
    if (btr_compensation)
    {
        BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_egress_btr_comp_context_create(unit, &egress_btr_context_id));

        BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_egress_btr_comp_fg_add_attach(unit, egress_btr_context_id));
    }

    return BCM_E_NONE;
}

/*
 *
 * Field for ESI encapsulation
 * Preselectors: out-port-profile, forward-code(bridge)
 * Qualifiers: inlif/sspa, outlif0, [fai.bum]
 * Actions: mc_rep_index
 *
 * In case UC and BUM traffic share the VPN SID, a qualifier is needed to distiguish
 * the UC and BUM traffic. FAI/ftmh_taim can be used for this purpose.
 * In single pass application, ftmh_taim is recommened. While in multi-pass application,
 * the FAI which is carried to the later passes by RCH is recommened.
 * Generally UC and BUM taffic should not share the VPN SID, since the VPN SID for BUM traffic
 * includes IML info which doesn't make sense for UC traffic.
 *
 * is_virtual_esi - virtual ESI or not; 1: inAC is used as the qualifier, 0: sspa is used as the qualifier.
 * dst_port - out-port of the ESI packets.
 */
int cint_field_srv6_evpn_esi_encap_egress_fg_add(
    int unit, 
    int is_virtual_esi, 
    int dst_port)
{
    char * fg_name = "SRv6_EVPN_ESI_encap";
    void * void_ptr = NULL;

    printf("Running: cint_field_srv6_evpn_esi_encap_egress()\n");

    if (field_srv6_evpn_esi_encap_info.fg_initialized == 1) {
        return BCM_E_NONE;
    }

    /************ Init the port **************/
    uint32 port_class_evpn_epmf = 2;
    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, dst_port, bcmPortClassFieldEgressPacketProcessingPortCs, port_class_evpn_epmf));

    /************ Create the context **************/
    field_srv6_evpn_esi_encap_info.egress_presel_id = srv6_evpn_esi_encap_egress_presel_id;

    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    bcm_field_context_info_t_init(&context_info);
    void_ptr = context_info.name;
    sal_strncpy_s(void_ptr, fg_name, sizeof(context_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &field_srv6_evpn_esi_encap_info.egress_context_id));


    /* Set the Presels: Current Layer Protocol and port cs profile*/
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = field_srv6_evpn_esi_encap_info.egress_presel_id;
    presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = field_srv6_evpn_esi_encap_info.egress_context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = port_class_evpn_epmf;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /************ Add the field group **************/
    bcm_field_group_info_t_init(&field_srv6_evpn_esi_encap_info.egress_fg_info);
    void_ptr = field_srv6_evpn_esi_encap_info.egress_fg_info.name;
    sal_strncpy_s(void_ptr, fg_name, sizeof(field_srv6_evpn_esi_encap_info.egress_fg_info.name));

    field_srv6_evpn_esi_encap_info.egress_fg_info.fg_type = bcmFieldGroupTypeTcam;
    field_srv6_evpn_esi_encap_info.egress_fg_info.stage = bcmFieldStageEgress;

    field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[0] = is_virtual_esi ? bcmFieldQualifyInVPort0 : bcmFieldQualifySrcPort;
    field_srv6_evpn_esi_encap_info.egress_fg_info.nof_quals ++;
    field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[1] = bcmFieldQualifyOutVPort0;
    field_srv6_evpn_esi_encap_info.egress_fg_info.nof_quals ++;
    /**
    field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[2] = bcmFieldQualifyForwardingAdditionalInfo;
    field_srv6_evpn_esi_encap_info.egress_fg_info.nof_quals ++;
    */

    field_srv6_evpn_esi_encap_info.egress_fg_info.nof_actions = 1;
    field_srv6_evpn_esi_encap_info.egress_fg_info.action_types[0] = bcmFieldActionAceEntryId;
    field_srv6_evpn_esi_encap_info.egress_fg_info.action_with_valid_bit[0] = TRUE;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &field_srv6_evpn_esi_encap_info.egress_fg_info, &field_srv6_evpn_esi_encap_info.egress_fg_id));

    /************ Configure the ACE format **************/
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 0;
    ace_format_info.action_types[ace_format_info.nof_actions++] = bcmFieldActionOutVport0;

    void_ptr = &(ace_format_info.name[0]);
    sal_strncpy_s(void_ptr, fg_name, sizeof(ace_format_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_ace_format_add(unit, 0, &ace_format_info, &field_srv6_evpn_esi_encap_info.ace_format_id));


    /************ Attach the field group with Context **************/
    int qual_idx, act_idx;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = field_srv6_evpn_esi_encap_info.egress_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = field_srv6_evpn_esi_encap_info.egress_fg_info.nof_actions;

    for(qual_idx=0; qual_idx < field_srv6_evpn_esi_encap_info.egress_fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[qual_idx];
        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    }
    for(act_idx=0; act_idx < field_srv6_evpn_esi_encap_info.egress_fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = field_srv6_evpn_esi_encap_info.egress_fg_info.action_types[act_idx];
    }

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, field_srv6_evpn_esi_encap_info.egress_fg_id, field_srv6_evpn_esi_encap_info.egress_context_id, &attach_info));

    field_srv6_evpn_esi_encap_info.fg_initialized = 1;

    return BCM_E_NONE;
}

/*
 * Add entries for field group: field_srv6_evpn_esi_encap_info.egress_fg_id
 *
 * src_vp - source virtual port, it's a src-sys-port(is_virtual_esi == 0) or a vlan-port-id(is_virtual_esi == 1).
 * dst_vp - destination virtual port, it's the original IPv6 tunnel gport without ESI.
 * is_virtual_esi - virtual ESI or not. In virtual ESI case, inAC(vlan-port-id) is used for indentifying the ES.
 *                  Otherwise, source system-port is used for indentifying the ES.
 * esi_tunnel_id - new tunnel_id with ESI. It's used to updated outlif0 by ACR.
 *
 */
int cint_field_srv6_evpn_esi_encap_egress_entry_add(
    int unit, 
    int is_virtual_esi, 
    int src_vp, 
    int dst_vp,
    int esi_tunnel_id)
{
    bcm_field_entry_info_t entry_in_info;

    printf("esi_encap_egress_entry_add(): adding to EPMF source vp:0x%08X, dest vp:%08X --> esi_tunnel_id:%08X\n",src_vp, dst_vp, esi_tunnel_id);

    /*********************** Add the ACE entry ************************/
    bcm_field_ace_entry_info_t ace_entry_info;
    uint32 ace_entry_id;

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionOutVport0;
    ace_entry_info.entry_action[0].value[0] = esi_tunnel_id;
    BCM_IF_ERROR_RETURN(bcm_field_ace_entry_add(unit, 0, field_srv6_evpn_esi_encap_info.ace_format_id, &ace_entry_info, &ace_entry_id));

    field_srv6_evpn_esi_encap_info.ace_entry_id[field_srv6_evpn_esi_encap_info.nof_esi_encap_egress_entries] = ace_entry_id;

    /*********************** Add the FG entry ************************/

    /* Add the Entry (src_vp + dst_vp [+ fai.bum]) --> esi_tunnel_encap_id */
    bcm_field_entry_info_t_init(&entry_in_info);
    entry_in_info.nof_entry_quals = field_srv6_evpn_esi_encap_info.egress_fg_info.nof_quals;
    entry_in_info.entry_qual[0].type = field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = src_vp;
    entry_in_info.entry_qual[0].mask[0] = is_virtual_esi ? 0xFFFFFFFF : 0xFFFF;

    entry_in_info.entry_qual[1].type = field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[1];
    entry_in_info.entry_qual[1].value[0] = dst_vp;
    entry_in_info.entry_qual[1].mask[0] = 0xFFFFFFFF;

    /** Required only when UC and BUM share the same VPN SID.
    entry_in_info.entry_qual[2].type = field_srv6_evpn_esi_encap_info.egress_fg_info.qual_types[2];
    entry_in_info.entry_qual[2].value[0] = 1;
    entry_in_info.entry_qual[2].mask[0] = 1;
    */

    entry_in_info.nof_entry_actions = field_srv6_evpn_esi_encap_info.egress_fg_info.nof_actions;
    entry_in_info.entry_action[0].type = field_srv6_evpn_esi_encap_info.egress_fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = ace_entry_id;

    entry_in_info.priority = 10;

    bcm_field_entry_t *entry_handle = &field_srv6_evpn_esi_encap_info.egress_entry_handles[field_srv6_evpn_esi_encap_info.nof_esi_encap_egress_entries];
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, field_srv6_evpn_esi_encap_info.egress_fg_id, &entry_in_info, entry_handle));

    field_srv6_evpn_esi_encap_info.nof_esi_encap_egress_entries ++;

    return BCM_E_NONE;
}

/*
 * Destroy the field configuration for SRv6 EVPN ESI encapsulation
 */
int cint_field_srv6_evpn_esi_encap_egress_destroy(
    int unit)
{
    int index;
    printf("Running: cint_field_srv6_evpn_esi_encap_egress_destroy()\n");

    if (field_srv6_evpn_esi_encap_info.fg_initialized == 0)
    {
        return BCM_E_NONE;
    }

    /** Delete the FG and ACE entries*/
    printf("Deleteing ACE entries:");
    for (index = 0; index < field_srv6_evpn_esi_encap_info.nof_esi_encap_egress_entries; index ++) {
        printf(" [%d](%x)",index,field_srv6_evpn_esi_encap_info.egress_entry_handles[index]);
        BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, field_srv6_evpn_esi_encap_info.egress_fg_id, NULL, field_srv6_evpn_esi_encap_info.egress_entry_handles[index]));

        BCM_IF_ERROR_RETURN(bcm_field_ace_entry_delete(unit, field_srv6_evpn_esi_encap_info.ace_entry_id[index]));
    }
    printf("\n");

    /** Delete the ace format*/
    BCM_IF_ERROR_RETURN(bcm_field_ace_format_delete(unit, field_srv6_evpn_esi_encap_info.ace_format_id));

    /** Deattach the context and FG */
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, field_srv6_evpn_esi_encap_info.egress_fg_id, field_srv6_evpn_esi_encap_info.egress_context_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, field_srv6_evpn_esi_encap_info.egress_fg_id));

    /* Destroy the preselector */
    bcm_field_presel_entry_id_t presel_entry_id;
    presel_entry_id.presel_id = field_srv6_evpn_esi_encap_info.egress_presel_id;
    presel_entry_id.stage = bcmFieldStageEgress;
    
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    BCM_IF_ERROR_RETURN(bcm_field_context_destroy(unit, bcmFieldStageEgress, field_srv6_evpn_esi_encap_info.egress_context_id));

    sal_memset(&field_srv6_evpn_esi_encap_info, 0, sizeof(field_srv6_evpn_esi_encap_info));

    return BCM_E_NONE;
}


int cint_field_srv6_evpn_destroy_all_entries(int unit)
{
    printf("Running: cint_field_srv6_evpn_destroy_all_entries()\n");

    /* Destroy the default EVPN ESI entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,egress_fg_id, NULL, default_entry_handle));

    if(entry_handle_esi_port != 0)
    {
        /* Destroy the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,egress_fg_id, NULL, entry_handle_esi_port));
    }
    if(entry_handle_esi != 0)
    {
        /* Destroy the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,egress_fg_id, NULL, entry_handle_esi));

    }
    if(entry_handle_esi_port_1 != 0)
    {
        /* Destroy the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit,egress_fg_id, NULL, entry_handle_esi_port_1));
    }
    return BCM_E_NONE;
}

int cint_field_evpn_destroy_ugq_and_uda(int unit)
{
    printf("Running: cint_field_evpn_destroy_ugq_and_uda()\n");

    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, esi_type_qual));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, esi_qual));
    BCM_IF_ERROR_RETURN(bcm_field_action_destroy(unit, esi_action));

    return BCM_E_NONE;
}

int cint_field_srv6_evpn_destroy_all(int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;

    printf("Running: cint_field_srv6_evpn_destroy_all()\n");

    /* Destroy Egress configuration */
    BCM_IF_ERROR_RETURN(cint_field_srv6_evpn_destroy_all_entries(unit));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, egress_fg_id, egress_context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, egress_fg_id));

    /* Destroy the preselector */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &egress_presel_entry_id, &presel_entry_data));

    BCM_IF_ERROR_RETURN(bcm_field_context_destroy(unit, bcmFieldStageEgress, egress_context_id));

    /* Destroy Ingress configuration */

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, ingress_2nd_pass_fg_id, ingress_context_2nd_pass_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, ingress_2nd_pass_fg_id));

    /* Destroy UDQ & UDA */
    BCM_IF_ERROR_RETURN(cint_field_evpn_destroy_ugq_and_uda(unit));

    BCM_IF_ERROR_RETURN(bcm_field_presel_set(unit, 0, &ingress_2nd_pass_presel_entry_id, &presel_entry_data));

    BCM_IF_ERROR_RETURN(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, ingress_context_2nd_pass_id));

    return BCM_E_NONE;
}

