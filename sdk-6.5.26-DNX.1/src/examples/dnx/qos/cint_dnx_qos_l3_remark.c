/*
 * cint_dnx_qos_l3_remark.c
 *
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/qos/cint_dnx_qos_l3_remark.c
 * cint
 * dnx_basic_example_ipv4_qos(0,200,201,52430);
 * exit;
 *
 * Sending IPv4 packet
 *   tx 1 psrc=200 data=0x000c000200000000070001008100000f080045fa0035000000001e005b4cc08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 *   Data: 0x00000000cd1d00123456789a8100b064080045f90035000000001d005c4dc08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */

int l3_in_map_id = -1;
int l3_phb_opcode = -1;
int l3_remark_opcode = -1;
int l3_eg_map_id = -1;
int l3_eg_remark_opcode = -1;
int use_existed = 0; /* if set, use existed profile */

int in_lif_profile_dscp_preserve_value = 2;
bcm_field_presel_t fai_update_presel_id = 58;

/** Used for both IPv4 and IPv6. To avoid create the same qual one more time, we define it here.*/
bcm_field_qualify_t qual_id_fai_2_0 = bcmFieldQualifyCount;
bcm_field_qualify_t qual_id_const_1_bit = bcmFieldQualifyCount;

/*
 * Map: Incoming dscp -> TC (int_pri), DP (color), nwk_qos (remark_int_pri)
 *   (dscp + 1) % 8 -> TC
 *   (dscp + 1) % 2 -> DP
 *   (dscp + 1) & 255 -> nwk_qos;
*/
int
qos_map_l3_ingress_profile(int unit, int is_ipv6)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_in_map;
    int dscp;
    int flags;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    /* Create qos-profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    if (l3_in_map_id != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for ingress cos-profile\n");
        return rv;
    }

    printf("bcm_qos_map_create ingress profile: %d\n", l3_in_map_id);

    /* Create opcode for PHB */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    if (l3_phb_opcode != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_phb_opcode);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for ingress phb opcode\n");
        return rv;
    }

    printf("bcm_qos_map_create ingress PHB opcode: %d\n", l3_phb_opcode);

    /* Add the maps for PHB */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = l3_phb_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_add() for ingress opcode for PHB\n");
        return rv;
    }

    /* Add qos mapping entry for PHB */
    bcm_qos_map_t_init(&l3_in_map);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_PHB | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    for (dscp = 0; dscp < 256; dscp++)
    {
        l3_in_map.dscp = dscp;
        l3_in_map.int_pri = (dscp + 1) % 8;
        l3_in_map.color = (dscp + 1) % 2;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_phb_opcode);
        if (rv != BCM_E_NONE) {
            printf("Error: in bcm_qos_map_add() for ingress qos map for PHB\n");
            return rv;
        }
    }

    /* Create opcode for remark */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l3_remark_opcode != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for ingress remark opcode\n");
        return rv;
    }

    printf("bcm_qos_map_create ingress PHB opcode: %d\n", l3_remark_opcode);

    /* Add the maps for remark */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = l3_remark_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_add() for ingress opcode for remark\n");
        return rv;
    }

    /* Add qos mapping entry for remark */
    bcm_qos_map_t_init(&l3_in_map);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    for (dscp = 0; dscp < 256; dscp++)
    {
        l3_in_map.dscp = dscp;
        l3_in_map.remark_int_pri = (dscp + 1) & 255;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_remark_opcode);
        if (rv != BCM_E_NONE) {
            printf("Error: in bcm_qos_map_add() for ingress qos map for remark\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Map: nwk_qos (int_pri) + DP (color) -> outgoing dscp (dscp)
 *   Current layer: (nwk_qos - 2) & 255 -> dscp
 *   Next layer: nwk_qos -> new nwk_qos, dp -> new dp
*/
int
qos_map_l3_egress_profile(int unit, int is_ipv6)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_eg_map;
    int int_pri,dp;
    int flags;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    /* Create qos-profile */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    if (l3_eg_map_id != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for egress profile\n");
        return rv;
    }

    printf("bcm_qos_map_create egress profile: %d\n", l3_eg_map_id);

    /* Create opcode */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l3_eg_remark_opcode != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_eg_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for egress remark opcode\n");
        return rv;
    }

    printf("bcm_qos_map_create egress remark opcode: %d\n", l3_eg_remark_opcode);

    /* Add the maps */
    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = l3_eg_remark_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_add() for egress opcode\n");
        return rv;
    }

    /* Add qos mapping entry */
    bcm_qos_map_t_init(&l3_eg_map);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    for (int_pri = 0; int_pri < 256; int_pri++)
    {
        for (dp = 0; dp < 4; dp++)
        {
            l3_eg_map.int_pri = int_pri;
            l3_eg_map.color = dp;
            l3_eg_map.dscp = (int_pri - 2) & 255;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_remark_opcode);
            if (rv != BCM_E_NONE) {
                printf("Error: in bcm_qos_map_add() for egress qos map\n");
                return rv;
            }
        }
    }

    /* Next layer mapping */
    bcm_qos_map_t_init(&l3_eg_map);
    for (int_pri = 0; int_pri < 256; int_pri++)
    {
        for (dp = 0; dp < 4; dp++)
        {
            l3_eg_map.int_pri = int_pri;
            l3_eg_map.color = dp;
            l3_eg_map.remark_int_pri = int_pri;
            l3_eg_map.remark_color = dp;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error: in bcm_qos_map_add() for egress qos map\n");
                return rv;
            }
        }
    }

    return rv;
}

/*
 * L3 IPv4 qos example, specifically
 *   ingress L3 tos to tc/dp/nwk_qos mapping
 *   egress nwk_qos+dp to tos mapping
 */
int
dnx_basic_example_ipv4_qos(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    int rv;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_intf_t l3if;

    /* Basic IPv4 routing */
    rv = dnx_basic_example_ipv4_inner(unit, in_port, out_port, kaps_result);
    if (rv != BCM_E_NONE) {
        printf("Error: in dnx_basic_example_ipv4_inner\n");
        return rv;
    }

    if (!use_existed) {
        /* Ingress profile and mapping */
        rv = qos_map_l3_ingress_profile(unit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error: in qos_map_l3_ingress_profile\n");
            return rv;
        }
    }

    /* Attch the ingress QoS profile to inRIF */
    bcm_l3_ingress_t_init(&l3_ing_if);
    rv = bcm_l3_ingress_get(unit, cint_ip_route_info.intf_in, &l3_ing_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_get\n");
        return rv;
    }

    l3_ing_if.qos_map_id = l3_in_map_id;
    l3_ing_if.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_ip_route_info.intf_in);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_ingress_create\n");
        return rv;
    }

    if (!use_existed) {
        /* Egress profile and mapping */
        rv = qos_map_l3_egress_profile(unit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error: in qos_map_l3_egress_profile\n");
            return rv;
        }
    }

    /* Attch the egress QoS profile to outRIF */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_intf_id = cint_ip_route_info.intf_out;
    rv = bcm_l3_intf_get(unit, &l3if);

    l3if.l3a_flags |= BCM_L3_WITH_ID | BCM_L3_REPLACE;
    l3if.dscp_qos.qos_map_id = l3_eg_map_id;

    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    return rv;
}

/*
 * L3 IPv6 qos example, specifically
 *   ingress L3 traffic class to tc/dp/nwk_qos mapping
 *   egress nwk_qos+dp to traffic class mapping
 */
int
dnx_basic_example_ipv6_qos(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_intf_t l3if;

    /* Basic IPv6 routing */
    rv = basic_example_ipv6(unit, in_port, out_port,0,0,0);
    if (rv != BCM_E_NONE) {
        printf("Error: in basic_example_ipv6\n");
        return rv;
    }

    /* Ingress profile and mapping */
    rv = qos_map_l3_ingress_profile(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error: in qos_map_l3_ingress_profile\n");
        return rv;
    }

    /* Attch the ingress QoS profile to inRIF */
    bcm_l3_ingress_t_init(&l3_ing_if);
    rv = bcm_l3_ingress_get(unit, cint_ip_route_info.intf_in, &l3_ing_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_get\n");
        return rv;
    }

    l3_ing_if.qos_map_id = l3_in_map_id;
    l3_ing_if.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_ip_route_info.intf_in);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_ingress_create\n");
        return rv;
    }

    /* Egress profile and mapping */
    rv = qos_map_l3_egress_profile(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error: in qos_map_l3_egress_profile\n");
        return rv;
    }

    /* Attch the egress QoS profile to outRIF */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_intf_id = cint_ip_route_info.intf_out;
    rv = bcm_l3_intf_get(unit, &l3if);

    l3if.l3a_flags |= BCM_L3_WITH_ID | BCM_L3_REPLACE;
    l3if.dscp_qos.qos_map_id = l3_eg_map_id;

    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    return rv;
}


/*
 * outlif.remark_profile is used to set qos preserve by BCM_QOS_OPCODE_PRESERVE.
 * Note: only routing case is supported.
 *
 * Note, used in JR2 mode only
 * Note, exposed to TCL
 */
int
qos_l3_dscp_preserve_per_outlif_config(int unit, int egr_qos_profile,int is_ipv6)
{
    int rv =BCM_E_NONE;
    int qos_profile;
    int qos_map_id;
    uint32 flags;
    uint32 app_flag = 0;
    int dp = 0;

    if (egr_qos_profile == -1) {
        qos_profile = l3_eg_map_id;
    } else {
        qos_profile = egr_qos_profile;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_id_get_by_profile(unit, flags, qos_profile, &qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error %d, bcm_qos_map_id_get_by_profile\n", rv);
    }

    if (is_ipv6)
    {
        app_flag = BCM_QOS_MAP_IPV6;
    }
    else
    {
         app_flag = BCM_QOS_MAP_IPV4;
    }

    /** Set preserve for IPV4.DSCP*/
    bcm_qos_map_t map_entry;
    bcm_qos_map_t_init(&map_entry);
    map_entry.opcode = BCM_QOS_OPCODE_PRESERVE;
    flags = BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | app_flag;
    rv = bcm_qos_map_add(unit, flags, &map_entry, qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error %d, bcm_qos_map_add for dscp preserve.\n", rv);
    }

    printf("DSCP preserve for remark_profile 0x%X successfully!\n", qos_map_id);
    return rv;
}

/*
 * iPMF for updating FAI[3] with the given value.
 * in-lif-profile is used for preselect
 * forwarding-type is IPv4 or IPv6
 */
int qos_l3_dscp_preserve_per_inlif_set(int unit, int in_lif_profile, bcm_field_layer_type_t forwarding_type, int enable)
{
    int rv= 0;
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    if (!is_jericho2_mode) {
        printf("Error, Procedure is used for Jericho2 mode only, for Jericho1 mode, use bcmPortControlPreserveDscpIngress instead!\n");
        return BCM_E_UNAVAIL;
    }

    /********* Create PRESELECTOR *************/
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "DSCP preserve", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if(rv != BCM_E_NONE) {
        printf("dnx_qos_l3_dscp_preserve_per_inlif_set: Error in bcm_field_context_create context %d\n", context_id);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id.presel_id = fai_update_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    presel_entry_data.qual_data[0].qual_arg = 1;  /** in-lif-profile1 (in-lif-profile0 is from RIF)*/
    presel_entry_data.qual_data[0].qual_value = in_lif_profile;
    presel_entry_data.qual_data[0].qual_mask = 0xFF;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = forwarding_type;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE) {
        printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
        return rv;
    }

    /********* Add QUALIFIERS *********/
    bcm_field_group_info_t fg_info;
    int qual_and_action_index;
    char *proc_name;
    void *dst_char;
    bcm_field_group_t ingress_fg_id;
    proc_name = "field_dir_extract_fai_to_fai";

    /** Init the fg_info structure. */
    bcm_field_group_info_t_init(&fg_info);

    /*
     * Create user define qualifier with FAI[2:0] for FAI[2:0]
     */
    if (qual_id_fai_2_0 == bcmFieldQualifyCount) {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 3;
        dst_char = &(qual_info.name[0]);
        sal_strncpy_s(dst_char, "FAI_BIT_2_0", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_fai_2_0);
        if(rv != BCM_E_NONE) {
            printf("%s Error in bcm_field_qualifier_create qual_id_fai_2_0\n", proc_name);
            return rv;
        }
    }

    /*
     * Create user define qualifier with CONST for FAI[3:3]
     */
    if (qual_id_const_1_bit == bcmFieldQualifyCount) {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        dst_char = &(qual_info.name[0]);
        sal_strncpy_s(dst_char, "CONST_1_BIT", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_const_1_bit);
        if(rv != BCM_E_NONE) {
            printf("%s Error in bcm_field_qualifier_create qual_id_const_1_bit\n", proc_name);
            return rv;
        }
    }

    /*
     *  Fill the fg_info structure, Add the FG.
     */
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_id_fai_2_0;
    fg_info.qual_types[1] = qual_id_const_1_bit;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_with_valid_bit[0] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info, &ingress_fg_id);
    if (rv != BCM_E_NONE) {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    /*
     * Attach the field group ID to the context.
     */
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyForwardingAdditionalInfo, bcmFieldStageIngressPMF2, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_info_get\n", proc_name, rv);
        return rv;
    }
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_info_get(unit, ingress_fg_id, &fg_info);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(qual_idx=0; qual_idx< fg_info.nof_quals; qual_idx++) {
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx<fg_info.nof_actions; act_idx++) {
        attach_info.payload_info.action_types[act_idx] = fg_info.action_types[act_idx];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = enable ? 1 : 0; /** Set FAI[3]*/
    attach_info.key_info.qual_info[1].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, ingress_fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", ingress_fg_id, context_id);
        return rv;
    }
    return rv;
}


/*
 * FAI[3] is used for DSCP preserve enabler from ingress. It is set per in-lif-profile.
 * Note: only routing case is supported.
 */
int
qos_l3_dscp_preserve_per_inlif_config(int unit, int port, int enable)
{
    int rv;
    int is_jericho2_mode;

    is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));

    if (!is_jericho2_mode) {
        BCM_GPORT_TUNNEL_ID_SET(port, BCM_L3_ITF_VAL_GET(port));
        rv = bcm_port_control_set(unit, port, bcmPortControlPreserveDscpIngress, enable);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", port);
            return rv;
        }
    }
    else
    {
        int in_lif_profile = in_lif_profile_dscp_preserve_value;

        /** Set in-lif-profile for PMF to the port!*/
        if (*(dnxc_data_get(unit, "field", "features", "cs_lif_profile", NULL)))
        {
            /** In some devices inlif profile for CS is done separately from entry inlif profile */
            rv = bcm_port_class_set(unit, port, bcmPortClassFieldIngressVportCs, in_lif_profile);
            if (rv != BCM_E_NONE) {
                printf("ERROR: in bcm_port_class_set with port 0x%08X, value %d\n", port, in_lif_profile);
                return rv;
            }
        } else {
            rv = bcm_port_class_set(unit, port, bcmPortClassFieldIngressVport, in_lif_profile);
            if (rv != BCM_E_NONE) {
                printf("ERROR: in bcm_port_class_set with port 0x%08X, value %d\n", port, in_lif_profile);
                return rv;
            }
        }

        /** Call PMF to enable ingress DSCP preserve.*/
        rv = qos_l3_dscp_preserve_per_inlif_set(unit, in_lif_profile, bcmFieldLayerTypeIp4, enable);
        if(rv != BCM_E_NONE) {
            printf("Error in dnx_qos_l3_dscp_preserve_per_inlif_set for ipv4!\n");
            return rv;
        }

        fai_update_presel_id++;

        rv = qos_l3_dscp_preserve_per_inlif_set(unit, in_lif_profile, bcmFieldLayerTypeIp6, enable);
        if(rv != BCM_E_NONE) {
            printf("Error in dnx_qos_l3_dscp_preserve_per_inlif_set for ipv6!\n");
            return rv;
        }
    }

    return rv;
}

/*
 * iPMF for updating FAI[3] with the the value of FAI field in RCH.
 * in-lif-profile is used for preselect
 * forwarding-type is IPv4 or IPv6
 */
int qos_l3_dscp_preserve_fai_convert(int unit, bcm_field_layer_type_t forwarding_type, int recycle_port)
{
    int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    int orientation_sel = *(dnxc_data_get(unit,  "lif", "feature", "iop_mode_orientation_selection", NULL));
    bcm_field_qualify_t qual_id_fai = bcmFieldQualifyCount;
    uint32 dscp_preserve_profile = 3;

    /**set recycle port profile*/
    bcm_port_class_set(unit, recycle_port, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, dscp_preserve_profile);

    int rv= 0;
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    /********* Create PRESELECTOR *************/
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "DSCP preserve_convert", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if(rv != BCM_E_NONE) {
        printf("dnx_qos_l3_dscp_preserve_per_inlif_set: Error in bcm_field_context_create context %d\n", context_id);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    fai_update_presel_id++;
    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id.presel_id = fai_update_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = dscp_preserve_profile;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = forwarding_type;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE) {
        printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
        return rv;
    }

    fai_update_presel_id++;
    /********* Add QUALIFIERS *********/
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;
    int qual_and_action_index;
    char *proc_name;
    void *dst_char;
    bcm_field_group_t ingress_fg_id;
    proc_name = "field_dir_extract_fai_to_fai";

    /** Init the fg_info structure. */
    bcm_field_group_info_t_init(&fg_info);

    /*
     * Create user define qualifier with FAI[2:0] for FAI[2:0]
     */
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 4;
    dst_char = &(qual_info.name[0]);
    sal_strncpy_s(dst_char, "FAI_BIT", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_fai);
    if(rv != BCM_E_NONE) {
        printf("%s Error in bcm_field_qualifier_create qual_id_fai\n", proc_name);
        return rv;
    }
    if (!is_jericho2_mode) {
         /*
         *  Fill the fg_info structure, Add the FG.
         */
        fg_info.stage = bcmFieldStageIngressPMF2;
        fg_info.fg_type = bcmFieldGroupTypeExactMatch;
        fg_info.nof_quals = 2;
        fg_info.qual_types[0] = qual_id_fai;
        fg_info.qual_types[1] = bcmFieldQualifyInVportClass0;

        fg_info.nof_actions = 1;
        fg_info.action_types[0] = bcmFieldActionInVportClass0;
        fg_info.action_with_valid_bit[0] = TRUE;
        rv = bcm_field_group_add(unit, 0, &fg_info, &ingress_fg_id);
        if (rv != BCM_E_NONE) {
            printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
            return rv;
        }

        /*
         * Attach the field group ID to the context.
         */
        int qual_idx = 0;
        int act_idx = 0;
        bcm_field_group_attach_info_t attach_info;

        bcm_field_group_info_t_init(&fg_info);
        bcm_field_group_info_get(unit, ingress_fg_id, &fg_info);
        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        for(qual_idx=0; qual_idx< fg_info.nof_quals; qual_idx++) {
            attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
        }
        for(act_idx=0; act_idx<fg_info.nof_actions; act_idx++) {
            attach_info.payload_info.action_types[act_idx] = fg_info.action_types[act_idx];
        }
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = 132;
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[1].input_arg = 0;
        attach_info.key_info.qual_info[1].offset = 0;
        rv = bcm_field_group_context_attach(unit, 0, ingress_fg_id, context_id, &attach_info);
        if(rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", ingress_fg_id, context_id);
            return rv;
        }
        /**dscp preserve bit is always bit[0] of RCH.FAI*/
        /** the max value of in-lif-profilie qualifier is 3, only two bits*/
        int in_lif_profile = 0;
        for (in_lif_profile=0; in_lif_profile< 4;in_lif_profile++)
        {
            bcm_field_entry_info_t_init(&entry_info);
            entry_info.nof_entry_quals = fg_info.nof_quals;
            entry_info.nof_entry_actions = fg_info.nof_actions;
            entry_info.entry_qual[0].type = fg_info.qual_types[0];
            entry_info.entry_qual[0].value[0] = 1;
            entry_info.entry_qual[1].type = fg_info.qual_types[1];
            entry_info.entry_qual[1].value[0] = in_lif_profile & 3;
            entry_info.entry_action[0].type = fg_info.action_types[0];
            if (orientation_sel)
            {
                entry_info.entry_action[0].value[0] = in_lif_profile | (1<<3);
            }
            else
            {
                entry_info.entry_action[0].value[0] = in_lif_profile | (1<<4);
            }
            rv = bcm_field_entry_add(unit, 0, ingress_fg_id, &entry_info, &entry_handle);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_entry_add\n", rv);
                return rv;
            }
        }
    } else {
        /*
         *  Fill the fg_info structure, Add the FG.
         */
        fg_info.stage = bcmFieldStageIngressPMF2;
        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.nof_quals = 1;
        fg_info.qual_types[0] = qual_id_fai;
        fg_info.nof_actions = 1;
        fg_info.action_types[0] = bcmFieldActionForwardingAdditionalInfo;
        fg_info.action_with_valid_bit[0] = FALSE;
        rv = bcm_field_group_add(unit, 0, &fg_info, &ingress_fg_id);
        if (rv != BCM_E_NONE) {
            printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
            return rv;
        }
        /*
         * Attach the field group ID to the context.
         */
        int qual_idx = 0;
        int act_idx = 0;
        bcm_field_group_attach_info_t attach_info;

        bcm_field_group_info_t_init(&fg_info);
        bcm_field_group_info_get(unit, ingress_fg_id, &fg_info);
        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        for(qual_idx=0; qual_idx< fg_info.nof_quals; qual_idx++) {
            attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
        }
        for(act_idx=0; act_idx<fg_info.nof_actions; act_idx++) {
            attach_info.payload_info.action_types[act_idx] = fg_info.action_types[act_idx];
        }
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = 132;
        rv = bcm_field_group_context_attach(unit, 0, ingress_fg_id, context_id, &attach_info);
        if(rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", ingress_fg_id, context_id);
            return rv;
        }
    }
    return rv;
}

/*
 * Config DSCP preserve enabler from ingress and egress.
 * Note: only routing case is supported.
 * Note: exposed to TCL.
 */
int
qos_l3_dscp_preserve_per_inlif_outlif_config(int unit, int ing_port, int egr_qos_profile)
{
    int rv;
    int enable = 1;
    int egr_port;

    rv = qos_l3_dscp_preserve_per_inlif_config(unit, ing_port, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_l3_dscp_preserve_per_inlif_set, port(0x08x)\n", ing_port);
        return rv;
    }

    rv = qos_l3_dscp_preserve_per_outlif_config(unit, egr_qos_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_l3_dscp_preserve_per_outlif_config, port(0x08x)\n", egr_qos_profile);
        return rv;
    }

    return rv;
}
