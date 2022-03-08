/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_srv6_scale.c
 * cint;
 * int unit = 0;
 * cint_field_external_pmf_main(unit);
 *
 * Configuration example end
 *
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 * */

bcm_field_group_t cint_field_external_pmf_fg_id = BCM_FIELD_INVALID;
bcm_field_group_t cint_field_external_pmf_fg_id_term = BCM_FIELD_INVALID;
uint32 cint_field_external_pmf_payload_id = 2;
bcm_field_AppType_t cint_field_external_pmf_external_app_type = bcmFieldAppTypeIp4UcastRpf;
/** This APPType is for FWD NOP */
bcm_field_AppType_t cint_field_external_pmf_external_app_type_term = bcmFieldAppTypeTrafficManagement;
/* put UE result in 256b KBP result bus, after 8b hits bits and 64b, and back the 72b of the UE size
 * that usually used by regular payload ids 0,1 in FWD stages
 * |   256b | --> | 8b hit | 96b FWD | 72b UE | xxxxx | LSB (PMF offset from LSB), KBP from MSB)
 * since configure of KBP is done with PMF interface, it's in LSB, as well as the PMF2 that uses the result
 */
uint32 cint_field_external_pmf_payload_offset = 256 -8 -96 -72;
/** UE of 72b is constructed with 3 x 32b actions, were in last action we need only 8b */
uint32 cint_field_external_pmf_payload_value[3]= {0x44333355, 0x22111144, 0x22};
uint32 cint_field_ue_size = 72;
uint32 cint_field_external_pmf_priority = 10;
/** following is uint32 lsb to msb representation of below 72b LSB fit for PMF qual structure */
uint32 ipv6_lsb_ue_term[4] =      { 0x22221111, 0x44443333, 0x00000055, 0x00000000 };
uint32 ip6_ue_mask[4] =           { 0xFFFFFFFF, 0xFFFFFFFF, 0x000000FF, 0x00000000 };


bcm_field_action_t cint_field_external_pmf_external_uda_0 = BCM_FIELD_INVALID;
bcm_field_action_t cint_field_external_pmf_external_uda_1 = BCM_FIELD_INVALID;
bcm_field_action_t cint_field_external_pmf_external_uda_2 = BCM_FIELD_INVALID;
bcm_field_action_t cint_field_external_pmf_external_uda_term_hit = BCM_FIELD_INVALID;
int cint_field_external_pmf_presel_entry_id = 15;
int cint_field_external_pmf_presel_entry_id_term = 16;
bcm_field_context_t cint_field_external_pmf_ipmf1_context = BCM_FIELD_INVALID;
bcm_field_entry_t cint_field_external_pmf_entry_id;
bcm_field_entry_t cint_field_external_pmf_entry_id_term;

bcm_field_group_t cint_field_external_pmf_fg_id_ipmf2 = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_field_external_pmf_ipmf2_ue_qual = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_field_external_pmf_ipmf2_udhbase_qual = BCM_FIELD_INVALID;
bcm_field_action_t cint_field_external_pmf_ipmf2_udh2_action = BCM_FIELD_INVALID;

bcm_field_qualify_t ue_term_ext_qual = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_ipmf2_black_hole_qual = BCM_FIELD_INVALID;
bcm_field_group_t cint_field_term_fg_id_ipmf2 = BCM_FIELD_INVALID;
/** context of Egress USD in the device, to combine with */
bcm_field_context_t cint_field_term_ipmf1_context = 5;

/*
 * number of words in IPv6 DIP
 */
int  SRV6_HIGH_SCALE_WORDS_IN_IP6_DIP     = 4;

/** Indicates IPv6 or IPv4 over SRv6.*/
int is_ipv6_flow = 0;

int
cint_field_external_pmf_fg_add(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{

    unsigned int flags = 0;

    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    proc_name = "cint_field_external_pmf_fg_add";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    /*
     * Optimize TCAM to get to max entries
     */
    fg_info_p->external_info.capacity_valid = TRUE;
    fg_info_p->external_info.capacity = 100000;
    fg_info_p->external_info.algorithmic_mode_valid = TRUE;
    fg_info_p->external_info.algorithmic_mode = 3;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG");
    /*
     *  Add qualifiers to the Field Group, to create KBP lookup with IPv4 VRF + DIP
     */
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyVrfValue;
    fg_info_p->qual_types[qual_idx++] = is_ipv6_flow ? bcmFieldQualifyDstIp6Low : bcmFieldQualifyDstIp;
    fg_info_p->nof_quals = qual_idx;
    /*
     * Mark bcmFieldQualifyL4SrcPort as a ranged qualifier.
     */
    fg_info_p->qual_is_ranged[3] = TRUE;

    /*
     *  Create x3 void user defined actions, each 32b to support 72b result for UE
     */
    bcm_field_action_info_t_init(&action_info);

    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);

    /** action 0 */
    action_info.size = 32;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_ua_0", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_external_uda_0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_pmf_external_uda_0;
    fg_info_p->action_with_valid_bit[0] = FALSE;


    /** action 1 */
    action_info.size = 32;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_ua_1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_external_uda_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[1] = cint_field_external_pmf_external_uda_1;
    fg_info_p->action_with_valid_bit[1] = FALSE;

    /** action 2 */
    action_info.size = 8;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_ua_2", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_external_uda_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[2] = cint_field_external_pmf_external_uda_2;
    fg_info_p->action_with_valid_bit[2] = FALSE;


    fg_info_p->nof_actions = 3;

    rv = bcm_field_group_add(unit, flags, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_pmf_fg_add_term(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{

    unsigned int flags = 0;

    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    char *tmp_ptr;
    proc_name = "cint_field_external_pmf_fg_add_term";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    /*
     * Optimize TCAM to get to max entries
     */
    /*
      fg_info_p->external_info.capacity_valid = TRUE;
      fg_info_p->external_info.capacity = 4000000;
      fg_info_p->external_info.algorithmic_mode_valid = TRUE;
      fg_info_p->external_info.algorithmic_mode = 3;
     */

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG_term");

    /*
     *  Add qualifiers to the Field Group, to create KBP lookup with IPv6 UE 72b LSB
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = cint_field_ue_size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "q_ue_ext_term", sizeof(qual_info.name));
    rv=bcm_field_qualifier_create(unit, 0, &qual_info, &ue_term_ext_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->qual_types[qual_idx++] = ue_term_ext_qual;
    fg_info_p->nof_quals = qual_idx;


    /*
     *  Create void user defined action of 1b to reflect the KBP HitBit
     */
    bcm_field_action_info_t_init(&action_info);

    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);

    /** action of 8b just to work */
    action_info.size = 8;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda_term_hit", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_external_uda_term_hit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_pmf_external_uda_term_hit;
    fg_info_p->action_with_valid_bit[0] = FALSE;


    fg_info_p->nof_actions = 1;

    rv = bcm_field_group_add(unit, flags, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    return rv;
}


int
cint_field_external_pmf_fg_attach(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_AppType_t external_app_type /*opcode*/,
    uint32 payload_id,
    uint32 payload_offset)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    proc_name = "cint_field_external_pmf_fg_attach";

    bcm_field_group_attach_info_t_init(&group_attach_info);

    /*
     * Build the attach information
     */
    group_attach_info.key_info.nof_quals = fg_info_p->nof_quals;
    group_attach_info.payload_info.nof_actions = fg_info_p->nof_actions;
    /* Payload id is the lookup id in the external */
    group_attach_info.payload_info.payload_id = payload_id;
    /*
     * Payload offset is the location of the result inside the external payload buffer.
     * In order to extract the result this value should be used
     * as the qualifier offset in the PMF12.
     * */
    group_attach_info.payload_info.payload_offset = payload_offset;

    for (qual_ndx = 0; qual_ndx < fg_info_p->nof_quals; qual_ndx++)
    {
        group_attach_info.key_info.qual_types[qual_ndx] = fg_info_p->qual_types[qual_ndx];
    }

    for (action_ndx = 0; action_ndx < fg_info_p->nof_actions; action_ndx++)
    {
        group_attach_info.payload_info.action_types[action_ndx] = fg_info_p->action_types[action_ndx];
    }
    /*
     * Build the qualifier attach information
     * -the qualifiers are VRF and DIP, take them from the FWD layer
     */

    group_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    group_attach_info.key_info.qual_info[1].input_arg = 0;
    group_attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[1].offset = 0;

    /* Cast */
    context_id  = (external_app_type & 0x00ff) ;

    /*
     * Call the attach API
     */
    rv = bcm_field_group_context_attach(unit, 0, external_fg_id, context_id, &group_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach fg_id %d external_app_type %d context %d\n", proc_name, rv, external_fg_id,   (external_app_type & 0x00ff), context_id);
        return rv;
    }

    return rv;
}


int
cint_field_external_pmf_fg_attach_term(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_AppType_t external_app_type /*opcode*/,
    uint32 payload_id,
    uint32 payload_offset)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    proc_name = "cint_field_external_pmf_fg_attach_term";

    bcm_field_group_attach_info_t_init(&group_attach_info);

    /*
     * Build the attach information
     */
    group_attach_info.key_info.nof_quals = fg_info_p->nof_quals;
    group_attach_info.payload_info.nof_actions = fg_info_p->nof_actions;
    /* Payload id is the lookup id in the external */
    group_attach_info.payload_info.payload_id = payload_id;
    /*
     * Payload offset is the location of the result inside the external payload buffer.
     * In order to extract the result this value should be used
     * as the qualifier offset in the PMF12.
     * */
    group_attach_info.payload_info.payload_offset = payload_offset;

    for (qual_ndx = 0; qual_ndx < fg_info_p->nof_quals; qual_ndx++)
    {
        group_attach_info.key_info.qual_types[qual_ndx] = fg_info_p->qual_types[qual_ndx];
    }

    for (action_ndx = 0; action_ndx < fg_info_p->nof_actions; action_ndx++)
    {
        group_attach_info.payload_info.action_types[action_ndx] = fg_info_p->action_types[action_ndx];
    }
    /*
     * Build the 72b KBP lookup key qualifier attach information
     * -the qualifiers is 72b LSB of the IPv6 DIP, take it from the FWD layer-1 (FWD layer is SRv6 at IPv6 Termination)
     */
    group_attach_info.key_info.qual_info[0].input_arg = -1;
    group_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    /** take the 72b by jumping from MSB to the DIP and another 56b Prefix, and take to LSB next 72b */
    group_attach_info.key_info.qual_info[0].offset = 192 + 56;

    /* Cast */
    context_id  = (external_app_type & 0x00ff) ;

    /*
     * Call the attach API
     */
    rv = bcm_field_group_context_attach(unit, 0, external_fg_id, context_id, &group_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach fg_id %d external_app_type %d context %d\n", proc_name, rv, external_fg_id,   (external_app_type & 0x00ff), context_id);
        return rv;
    }

    return rv;
}

int
cint_field_external_pmf_entry_add(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_pmf_entry_add";

    bcm_field_entry_info_t_init(&entry_info);

    uint32 max_entry_index = 1; /** 30000000; */
    uint32 entry_index = 0;
    uint32 ip_addr = is_ipv6_flow ? 0x0000FF13 : 0x7fffff02;

    while (entry_index < max_entry_index)
    {

        entry_info.entry_qual[0].type = bcmFieldQualifyVrfValue;
        entry_info.entry_qual[0].value[0] = 0x1;
        entry_info.entry_qual[0].mask[0] = 0xf;
        entry_info.entry_qual[1].type = is_ipv6_flow ? bcmFieldQualifyDstIp6Low : bcmFieldQualifyDstIp;
        /** making sure the actual DIP is updated in entry 0, all rest are entries for max test */
        entry_info.entry_qual[1].value[0] = (entry_index == 0) ? ip_addr : entry_index;
        entry_info.entry_qual[1].mask[0] = 0xffffffff;

        entry_info.nof_entry_quals = fg_info_p->nof_quals;

        entry_info.entry_action[0].type = fg_info_p->action_types[0];
        entry_info.entry_action[0].value[0] = (entry_index == 0) ? cint_field_external_pmf_payload_value[0]:entry_index;

        entry_info.entry_action[1].type = fg_info_p->action_types[1];
        entry_info.entry_action[1].value[0] = (entry_index == 0) ? cint_field_external_pmf_payload_value[1]:entry_index*5;

        entry_info.entry_action[2].type = fg_info_p->action_types[2];
        entry_info.entry_action[2].value[0] = (entry_index == 0) ? cint_field_external_pmf_payload_value[2]: (entry_index+500) & 0xff;

        entry_info.nof_entry_actions = fg_info_p->nof_actions;

        entry_info.priority = cint_field_external_pmf_priority;

        rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &cint_field_external_pmf_entry_id);
        if (rv != BCM_E_NONE)
        {
            void *dest_char;
            char *fg_name;
            dest_char = &(fg_info_p->name[0]);
            fg_name = dest_char;

            printf("%s Error (%d), in bcm_field_entry_add fg_id %d fg_name %s, max entry added %d \n", proc_name, rv, external_fg_id, fg_name, entry_index);
            return rv;
        }

        entry_index++;

    }

    printf("%s SUCCESSFULLY ADDED (%d) ENTRIES In ENCAP \n", proc_name, entry_index);


    return rv;
}

int
cint_field_external_pmf_entry_add_term(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_pmf_entry_add_term";

    bcm_field_entry_info_t_init(&entry_info);

    uint32 max_entry_index = 1; /** 30000000; */
    uint32 entry_index = 0;
    int ii;

    while (entry_index < max_entry_index)
    {

        entry_info.entry_qual[0].type = ue_term_ext_qual;

        /** making sure the actual DIP is updated in entry 0, all rest are entries for max test */
        if (entry_index == 0)
        {
            for (ii=0; ii<SRV6_HIGH_SCALE_WORDS_IN_IP6_DIP; ii++)
                    {
                        entry_info.entry_qual[0].value[ii] = ipv6_lsb_ue_term[ii];
                        entry_info.entry_qual[0].mask[ii] = ip6_ue_mask[ii];
                    }
        }
        else
        {
            entry_info.entry_qual[0].value[0] = entry_index;
            entry_info.entry_qual[0].mask[0] = 0xffffffff;
        }

        entry_info.nof_entry_quals = fg_info_p->nof_quals;


        entry_info.nof_entry_actions = 0;

        entry_info.priority = cint_field_external_pmf_priority;

        rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &cint_field_external_pmf_entry_id_term);
        if (rv != BCM_E_NONE)
        {
            void *dest_char;
            char *fg_name;
            dest_char = &(fg_info_p->name[0]);
            fg_name = dest_char;

            printf("%s Error (%d), in bcm_field_entry_add fg_id %d fg_name %s, max entry added %d \n", proc_name, rv, external_fg_id, fg_name, entry_index);
            return rv;
        }

        entry_index++;

    }

    printf("%s SUCCESSFULLY ADDED (%d) ENTRIES In TERM \n", proc_name, entry_index);


    return rv;
}

int
cint_field_external_pmf_context_ipmf1_create(
    int unit)
{
    char *proc_name;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t ipmf1_p_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_pmf_context_ipmf1_create";

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_test", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_external_pmf_ipmf1_context);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);

    presel_entry_id.presel_id = cint_field_external_pmf_presel_entry_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_external_pmf_ipmf1_context;
    ipmf1_p_data.nof_qualifiers = 1;
    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyAppType;
    ipmf1_p_data.qual_data[0].qual_value = cint_field_external_pmf_external_app_type;
    ipmf1_p_data.qual_data[0].qual_mask = 0x3F;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    printf("Configured context id %d on iPMF1\n", cint_field_external_pmf_ipmf1_context);

    return rv;
}

int
cint_field_external_pmf_context_ipmf1_create_term(
    int unit)
{
    char *proc_name;

    bcm_field_presel_entry_data_t ipmf1_p_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_pmf_context_ipmf1_create_term";

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);

    presel_entry_id.presel_id = cint_field_external_pmf_presel_entry_id_term;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_term_ipmf1_context;
    ipmf1_p_data.nof_qualifiers = 1;
    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyAppType;
    ipmf1_p_data.qual_data[0].qual_value = cint_field_external_pmf_external_app_type_term;
    ipmf1_p_data.qual_data[0].qual_mask = 0x3F;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    printf("Configured context id %d on iPMF1\n", cint_field_term_ipmf1_context);

    return rv;
}

int
cint_field_external_pmf_fg_ipmf2_add(
    int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    int ii;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_group_attach_info_t attach_info;
    int action_type_size;
    int action_offset_in_fg;

    proc_name = "cint_field_external_pmf_fg_ipmf2_add";

    bcm_field_group_info_t_init( &fg_info);


    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_FG", sizeof(fg_info.name));

    /*
     *  Create a user defined qualifier - the 72b of UE, result from KBP lookup
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = cint_field_ue_size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "q_ue_iPMF2", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_external_pmf_ipmf2_ue_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }


    /*
     *  Create a user defined qualifier - for UDH Base to activate the UDH containers
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 6;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "q_udhbase_iPMF2", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_external_pmf_ipmf2_udhbase_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }


    /*
     *  Create a void user defined action for UDH2 only
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData1;
    action_info.size = 72 - (2*32);
    action_info.prefix_size = 32 - action_info.size;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "a_udh2_8b_PMF2", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_ipmf2_udh2_action);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = cint_field_external_pmf_ipmf2_ue_qual;
    fg_info.qual_types[1] = cint_field_external_pmf_ipmf2_udhbase_qual;

    fg_info.nof_actions = 6;
    fg_info.action_types[0] = cint_field_external_pmf_ipmf2_udh2_action;
    fg_info.action_types[1] = bcmFieldActionUDHData2;
    fg_info.action_types[2] = bcmFieldActionUDHData3;
    fg_info.action_types[3] = bcmFieldActionUDHBase1;
    fg_info.action_types[4] = bcmFieldActionUDHBase2;
    fg_info.action_types[5] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_with_valid_bit[5] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_external_pmf_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeExternal; /** KBP Result */

    /** get the action_0 offset (the action in external PMF, KBP, first 32b action of the result, which is 0 by default) */
    rv = bcm_field_group_action_offset_get(unit, 0, cint_field_external_pmf_fg_id, cint_field_external_pmf_external_uda_0, &action_offset_in_fg);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_action_offset_get\n", proc_name, rv);
        return rv;
    }
    /** use the same offset in the KBP result type, that used to configure the KBP results + the relative action offset inside */
    attach_info.key_info.qual_info[0].offset = cint_field_external_pmf_payload_offset + action_offset_in_fg;

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg =  0x2A;


    rv = bcm_field_group_context_attach(unit, 0, cint_field_external_pmf_fg_id_ipmf2, cint_field_external_pmf_ipmf1_context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    printf("Config DE FG on iPMF2, fg_id=%d\n", cint_field_external_pmf_fg_id_ipmf2);

    return rv;
}



int
cint_field_external_pmf_fg_ipmf2_add_term(
    int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    int ii;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t action_drop;
    bcm_field_group_attach_info_t attach_info;
    int action_type_size;
    int action_offset_in_fg;

    proc_name = "cint_field_external_pmf_fg_ipmf2_add_term";

    bcm_field_group_info_t_init( &fg_info);


    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_term_FG", sizeof(fg_info.name));

    /*
     *  Create a user defined qualifier - the 32b for black hole action forwarding value to drop packet
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 32;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "q_black_hole_iPMF2", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_ipmf2_black_hole_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    /*
     * -Use HIT 1b of the KBP - in reverse polarity in action - for action Valid BIT
     * -and another 1b qualifier constant for the action of Forwarding to a BCM_GPORT_BLACK_HOLE (to drop)
     *  in case of not HIT
     */

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyExternalHit2;
    fg_info.qual_types[1] = cint_ipmf2_black_hole_qual;


    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForward;
    fg_info.action_with_valid_bit[0] = TRUE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_term_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /** KBP HitBit */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    /*
     * Constant value of Black Hole value for action of forward destination
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;

    /* TBD - instead of using constant HW value of Black Hole 0x001FFFFF, get it by API
    SHR_IF_ERR_EXIT(bcm_field_action_value_map
                    (unit, bcmFieldStageIngressPMF1, bcmFieldActionForward, BCM_GPORT_BLACK_HOLE,
                     hw_action_value));
    */

    /** HW value of a black hole value for a forward destination action */
    attach_info.key_info.qual_info[1].input_arg =  0x001FFFFF;
    /** Reverese the Valid bit, so that when KBP is HIT, packet is passed, dropped at HITBit 0*/
    attach_info.payload_info.action_info[0].valid_bit_polarity = 0;


    rv = bcm_field_group_context_attach(unit, 0, cint_field_term_fg_id_ipmf2, cint_field_term_ipmf1_context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    printf("Config DE FG on iPMF2, fg_id=%d\n", cint_field_term_fg_id_ipmf2);

    return rv;
}

int
cint_field_srv6_kbp_config(
                    int unit,
                    bcm_field_group_info_t fg_info_encap,
                    bcm_field_group_info_t fg_info_term)
{

    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_srv6_kbp_config";

    /*
     * Configure KBP for Encap
     */

    rv = cint_field_external_pmf_fg_add(unit, &fg_info_encap, &cint_field_external_pmf_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_add fg_id %d\n", proc_name, rv, cint_field_external_pmf_fg_id);
        return rv;
    }

    cint_field_external_pmf_external_app_type = is_ipv6_flow ? bcmFieldAppTypeIp6UcastRpf : bcmFieldAppTypeIp4UcastRpf;
    rv = cint_field_external_pmf_fg_attach(unit, cint_field_external_pmf_fg_id, &fg_info_encap, cint_field_external_pmf_external_app_type, cint_field_external_pmf_payload_id, cint_field_external_pmf_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    /*
     * Configure KBP for Term
     */

    rv = cint_field_external_pmf_fg_add_term(unit, &fg_info_term, &cint_field_external_pmf_fg_id_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_add_term fg_id %d\n", proc_name, rv, cint_field_external_pmf_fg_id_term);
        return rv;
    }

    rv = cint_field_external_pmf_fg_attach_term(unit, cint_field_external_pmf_fg_id_term, &fg_info_term, cint_field_external_pmf_external_app_type_term, cint_field_external_pmf_payload_id, cint_field_external_pmf_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_attach_term fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id_term,  (cint_field_external_pmf_external_app_type_term & 0x00ff));
        return rv;
    }

    /*
     * Sync TCAM
     */

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    /*
     * Adding KBP entries for Encap
     */

    rv = cint_field_external_pmf_entry_add(unit, cint_field_external_pmf_fg_id, &fg_info_encap);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    /*
     * Adding KBP entries for Term
     */

    rv = cint_field_external_pmf_entry_add_term(unit, cint_field_external_pmf_fg_id_term, &fg_info_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_entry_add_term fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id_term,  (cint_field_external_pmf_external_app_type_term & 0x00ff));
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_pmf_fg_id_term,  (cint_field_external_pmf_external_app_type_term & 0x00ff));


    return rv;
}

int
cint_field_srv6_pmf_config_encap(
                            int unit,
                            bcm_field_group_info_t fg_info)
{

    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_srv6_pmf_config_encap";


    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));

    rv = cint_field_external_pmf_context_ipmf1_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_context_ipmf1_create\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_pmf_fg_ipmf2_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_ipmf2_add\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_srv6_pmf_config_term(
                            int unit,
                            bcm_field_group_info_t fg_info)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_srv6_pmf_config_term";

    rv = cint_field_external_pmf_context_ipmf1_create_term(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_context_ipmf1_create_term\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_pmf_fg_ipmf2_add_term(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_ipmf2_add_term\n", proc_name, rv);
        return rv;
    }

    return rv;
}


/**
 * \brief - run external main function
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_pmf_main(0)"
 */
int
cint_field_external_pmf_main(int unit)
{
    bcm_field_group_info_t fg_info_encap;
    bcm_field_group_info_t fg_info_term;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_pmf_main";

    bcm_field_group_info_t_init( &fg_info_encap);
    bcm_field_group_info_t_init( &fg_info_term);

    rv = cint_field_srv6_kbp_config(unit, fg_info_encap, fg_info_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_srv6_kbp_config \n", proc_name, rv);
        return rv;
    }

    rv = cint_field_srv6_pmf_config_encap(unit, fg_info_encap);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_srv6_pmf_config_encap \n", proc_name, rv);
        return rv;
    }

    rv = cint_field_srv6_pmf_config_term(unit, fg_info_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_srv6_pmf_config_term \n", proc_name, rv);
        return rv;
    }

    return rv;
}


