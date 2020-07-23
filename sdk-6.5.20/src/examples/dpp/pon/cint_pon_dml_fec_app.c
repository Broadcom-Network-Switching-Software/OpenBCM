/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PON FEC example for DPoE IP-HSD:
 * In the following CINT we will try to provide an example of use forward group AC for DPoE application.
 *    The focus of the CINT is on AC-LIFs (bcm_vlan_port_t object in BCM API)
 *    and HOW TO use PMF to update L2 lookup destination.
 *
 * DPoE IP-HSD backgroud
 * Upstream:
 *     ONU implements upstream packet classification matching on L2/3/4 header fields and mapping frames to LLIDs. 
 * Downstream:
 *     ARAD performs an L2 lookup to map frames to a destination.(i.e., group of LLIDs on an ONU).
 *     ARAD implements downstream packet classification matching on L2/3/4 header field and mapping frames to PON LIFs.
 *
 * DPoE IP-HSD example - VOIP
 *     ONU configuration uses multiple LLIDs to provide service to a single CPE
 *     Use case is a DPoE VOIP application.
 *     IP/HSD service (untagged) configuration using three LLIDS:
 *        LLID0: VOIP signaling (UDP port 5060; SIP signaling)
 *        LLID1: VOIP bearer (TOS/DSCP value 0xB8; UDP RTP)
 *        LLID2: default flow, Internet traffic
 *    The VOIP signaling and bearer traffic originate from the same CPE device MAC address
 *
 * How to run:
 * cint cint_port_tpid.c
 * cint pon/cint_pon_utils.c
 * cint pon/cint_pon_dml_fec_app.c
 * c
 * verbose=1;
 * print pon_dml_fec_app(unit, pon_port, nni_port);
 * exit;
 */ 

/* set to one for informative prints */
int verbose = 1;

 /*
 * Multipoint Example:
 *  devices : unit
 * 
 *  physical port:
 *      - nni_port, pon_port
 *  
 *  two gports
 *  - gport1
 *      - resources:
 *          - unit: LIF 
 *  - gport2 - forward group
 *      - resources:
 *          - unit: fec_a, lif_a
 *          - unit: fec_b, lif_b, which learned FEC is fec_a
 */ 
int pon_dml_fec_app(int unit, int pon_port, int nni_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t nni_vp_lif;
    bcm_vlan_port_t vp2_lif_a;
    bcm_vlan_port_t vp2_lif_b;
    bcm_vlan_port_t vp2_fec_a;
    bcm_vlan_port_t vp2_fec_b;
    int base = 0;
    bcm_gport_t pon_gport_lif_a =5000+base;
    bcm_gport_t pon_gport_lif_b =5001+base;
    bcm_gport_t pon_gport_fec_a =5000+base;
    bcm_gport_t pon_gport_fec_b =5001+base;
    uint32 tunnel_a = 20;
    uint32 tunnel_b = 30;
    bcm_gport_t nni_gport_lif =6000+base;
    bcm_vlan_t vsi;
    bcm_failover_t failover_id;
    bcm_if_t encap_id;

    fwd_group = 1;
    vsi = 0;

    pon_app_init(unit, pon_port, nni_port, 0, 0);
    /* create vswitch on both devices */
    vswitch_create(unit,&vsi);
    if (verbose)
    {
        printf("unit vswitch_create:%x\n", vsi);
    }

    /* create gport1 in unit1 without protection with untag */
    rv = nni_lif_create(unit, nni_port, match_all_tags, 0, 0, 0, 0, &nni_gport_lif, &encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, nni_lif_create, nni_port\n"); 
        return rv;
    }
    if (verbose)
    {
        printf("unit nni_gport_lif:%x\n", nni_gport_lif);
    }

    rv = bcm_vswitch_port_add(unit, vsi, nni_gport_lif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add, nni_port\n"); 
        return rv;
    }

    /* create gport2.LIFa with learning info as FEC but without creating FEC entry */
    rv = pon_lif_create(unit, 
                        pon_port, 
                        match_untag,
                        0, 0,
                        tunnel_a,
                        tunnel_a,
                        0,
                        0, 0, 0,
                        &(pon_gport_lif_a));
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create pon_gport_lif_a\n"); 
        return rv;
    }
    if (verbose)
    {
        printf("unit pon_gport_lif_a:%x\n", pon_gport_lif_a);
    }

    rv = bcm_vswitch_port_add(unit, vsi, pon_gport_lif_a);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_vswitch_port_add, pon_gport_lif_a\n"); 
        return rv;
    }

    /* create gport2.LIFb with learning info as FEC but without creating FEC entry */
    rv = pon_lif_create_fwd_grp_with_learn_fec(unit, 
                                                pon_port, 
                                                match_untag,
                                                0, 0,
                                                tunnel_b,
                                                tunnel_b,
                                                0,
                                                0, 0, 0,
                                                pon_gport_lif_a,
                                                &(pon_gport_lif_b));
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create_fwd_grp_with_learn_fec pon_gport_lif_b\n"); 
        return rv;
    }
    if (verbose)
    {
        printf("unit pon_gport_lif_b:%x\n", pon_gport_lif_b);
    }

    rv = bcm_vswitch_port_add(unit, vsi, pon_gport_lif_b);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add, pon_gport_lif_b\n"); 
        return rv;
    }

    /* Adding the gports to the multicast for upstream */
    rv = multicast_vlan_port_add(unit, vsi, nni_port, nni_gport_lif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in multicast_vlan_port_add, nni_gport_lif\n");
        return rv;
    }

    /* Adding the gports to the multicast for downstream */
    rv = multicast_vlan_port_add(unit, vsi+lif_offset, pon_port, pon_gport_lif_a);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in multicast_vlan_port_add, pon_gport_lif_a\n");
        return rv;
    }

    /* Adding the gports to the multicast for downstream  */
    rv = multicast_vlan_port_add(unit, vsi+lif_offset, pon_port, pon_gport_lif_b);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in multicast_vlan_port_add, pon_gport_lif_b\n");
        return rv;
    }

    bcm_port_control_set(unit, nni_gport_lif, bcmPortControlFloodUnknownUcastGroup, lif_offset);
    bcm_port_control_set(unit, nni_gport_lif, bcmPortControlFloodUnknownMcastGroup, lif_offset);
    bcm_port_control_set(unit, nni_gport_lif, bcmPortControlFloodBroadcastGroup,    lif_offset);

    return rv;
}

bcm_field_presel_set_t psset_voip;
int presel_id_voip = 0;
int pon_voip_pselect_setup(int unit, bcm_pbmp_t in_pbmp)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int presel_flags = 0;

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id_voip);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_voip | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id_voip);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_voip | presel_flags);
            return result;
        }
    }

    /* 
     * Set the preselector to be on the ports
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm, in_pbmp);
    for(i = 0; i < 256; i++)
    { 
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }
  
    result = bcm_field_qualify_Stage(unit, presel_id_voip | presel_flags | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_voip | presel_flags);
        return result;
    }
    
    result = bcm_field_qualify_InPorts(unit, presel_id_voip | presel_flags | BCM_FIELD_QUALIFY_PRESEL, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_voip | presel_flags);
        return result;
    }


    result = bcm_field_qualify_L2DestHit(unit, presel_id_voip | presel_flags | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x01);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[0] in bcm_field_qualify_L2DestHit\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_voip | presel_flags);
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset_voip);
    BCM_FIELD_PRESEL_ADD(psset_voip, presel_id_voip);

    return result;
}

/* This is a example to update destination for VOIP signal and bearer service which have same SA.
 * In case Forward groups (FEC & LIF) will be with consecutive FECs.
 * That meaning have 2 tunnel IDs. Then will allocate in PON port such as
 *     FEC 5000 , LIF 5000 for tunnel ID 20(voip signal).
 *     FEC 5001,  LIF 5001 for tunnel ID 30(voip bearer).
 * So in downstream, 
 *     If the service flow is voip signal(bcmFieldQualifyL4SrcPort), 
 *     PMF will add offset 0 with MAC table lookup result.
 *     if the service flow is voip bearer(Data qualify, checking IP-TOS==0xB8),
 *     PMF will add offset 1 with MAC table lookup result.
 */
int pon_voip_update_dest_in_downstream(/* in */ int unit,
                                        /* in */ int port,
                                        /* in */ int group_priority_1,
                                        /* out */ bcm_field_group_t *group_1,
                                        /* in */ int group_priority_2,
                                        /* out */ bcm_field_group_t *group_2) 
{
    int result;
    int auxRes;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[2]; 
    uint32 dq_ndx, dq_ndx2, dq_length[2] = {16, 3};
    uint32 dq_lsb[2] = {0, 16};
    bcm_field_qualify_t dq_qualifier[2] = {bcmFieldQualifyDstL3Egress, bcmFieldQualifyDstL3Egress};
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext[3]; 
    bcm_field_extraction_action_t extact;
    uint8 i = 0, j = 0;
    bcm_field_action_t grp_action = bcmFieldActionRedirect;

    bcm_field_group_config_t grp_cascaded;
    bcm_field_entry_t cascaded_ent[2];
    bcm_field_data_qualifier_t dq_ipv4_tos;
    bcm_pbmp_t in_pbmp;

    if (NULL == group_1 || NULL == group_2) 
    {
        printf("Pointer to group ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_PORT_ADD(in_pbmp, port);

    /* setup preselect */
    result = pon_voip_pselect_setup(unit, in_pbmp);
    if (BCM_E_NONE != result)
    {
        printf("Error in pon_voip_pselect_setup result:%d\n", result);
        return result;
    }

    bcm_field_group_config_t_init(&grp);
    bcm_field_group_config_t_init(&grp_cascaded);
    grp.group = -1;
    grp_cascaded.group = -1;

    /* 
    * Add offset 0 or 1 with MAC table lookup result
    * Suppose max LLID per ONU is 128, so take 8 bits. if bit:7 is 1, indicates
    * the packet match in 1st FG.
    */
    result = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth,8);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_control_set result:%d\n", result);
        return result;
    }

    /* Define the group2 QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyCascadedKeyValue);

    dq_ndx = 0;
    bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
    data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
    data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
    data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
    data_qual[dq_ndx].length = dq_length[dq_ndx];
    result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_data_qualifier_create for FG2, result:%d, grp\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_qset_data_qualifier_add for FG2, result:%d, grp\n", result);
        return result;
    }

    dq_ndx++;
    bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
    data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
    data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
    data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
    data_qual[dq_ndx].length = dq_length[dq_ndx];
    result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_data_qualifier_create for FG2, result:%d, grp\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_qset_data_qualifier_add for FG2, result:%d, grp\n", result);
        return result;
    }

    /*
     *  This Field Group can update MAC table lookup result.
     */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, grp_action);

    /*  Create the Field group 2 */
    grp.priority = group_priority_2;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset_voip;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create in grp\n");
        return result;
    }

    /*
     *  Add a single entry to the Field group.
     */
    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create in grp\n");
        return result;
    }

    /* First field group lookup result is found */
    result = bcm_field_qualify_CascadedKeyValue(unit, ent, 0x80, 0xFF);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_CascadedKeyValue\n");
        return result;
    }

    bcm_field_extraction_action_t_init(&extact);
    extact.action = grp_action;
    extact.bias = 0;
    /* cascaded key:1 bit */
    dq_ndx = 0;
    bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
    ext[dq_ndx].qualifier = bcmFieldQualifyCascadedKeyValue;
    ext[dq_ndx].lsb = 0;
    ext[dq_ndx].bits = 1;

    /* L2 lookup result bit[15:1] */
    dq_ndx++;
    bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
    ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[dq_ndx].qualifier = data_qual[dq_ndx-1].qual_id;
    ext[dq_ndx].lsb = 1;
    ext[dq_ndx].bits = 15;

    /* L2 lookup result bit[18:16] */
    dq_ndx++;
    bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
    ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[dq_ndx].qualifier = data_qual[dq_ndx-1].qual_id;
    ext[dq_ndx].lsb = 0;
    ext[dq_ndx].bits = 3;

    /* MAC table lookup result bits[15:1] + cascaded result 1LSB */
    result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extact,
                                                  3/* count */,
                                                  ext);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_direct_extraction_action_add - grp\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install - grp\n");
        return result;
    }

    /* qualifier Set */
    BCM_FIELD_QSET_INIT(grp_cascaded.qset);
    BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyL4SrcPort);

    /* 
     * Define the Data qualifier for IP-TOS: 
     * Take the 2nd byte of the IPv4 header 
     */
    bcm_field_data_qualifier_t_init(&dq_ipv4_tos); 
    dq_ipv4_tos.offset_base = bcmFieldDataOffsetBaseFirstHeader; 
    dq_ipv4_tos.offset = 1;
    dq_ipv4_tos.length = 1;
    result = bcm_field_data_qualifier_create(unit, &dq_ipv4_tos);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }
    /* Add the IP-TOS Data qualifier to the QSET */
    result = bcm_field_qset_data_qualifier_add(unit, &(grp_cascaded.qset), dq_ipv4_tos.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add, dq_ipv4_tos\n");
        return result;
    }

    /* Actions Set */
    BCM_FIELD_ASET_INIT(grp_cascaded.aset);
    BCM_FIELD_ASET_ADD(grp_cascaded.aset, bcmFieldActionCascadedKeyValueSet);

    /* Create FG 1  - cascadedkey group */
    grp_cascaded.priority = group_priority_1;
    grp_cascaded.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;;
    grp_cascaded.mode = bcmFieldGroupModeDirect;
    grp.preselset = psset_voip;
    result = bcm_field_group_config_create(unit, &grp_cascaded);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create grp_cascaded\n");
        return result;
    }
    /*
     * Have 2 entries with 1x1 mapping between the bit 1 with voip service flow.
 */
    for (i = 0; i < 2; i++)
    {
        uint8 mask = 0xFF;
        uint8 data = 0xB8;

        result = bcm_field_entry_create(unit, grp_cascaded.group, &cascaded_ent[i]);
        if (BCM_E_NONE != result) 
        {
            printf("Error in bcm_field_entry_create - grp_cascaded entry:%d\n", i);
            break;
        }

        /* 
        * bcmFieldQualifyIpProtocol Qualifier -- UDP: 
        */
        result = bcm_field_qualify_IpProtocol(unit, cascaded_ent[i], 0x11, 0xff);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_qualify_IpProtocol, grp_cascaded entry:%d\n", i);
            return result;
        }

        if (i == 0)
        {
            /* VOIP signal */
            bcm_l4_port_t l4_src_port = 0x13C4;
            result = bcm_field_qualify_L4SrcPort(unit, cascaded_ent[i], l4_src_port, 0xffff);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data - grp_cascaded voip signal, entry:%d\n", i);
                return result;
            }
        } 
        else 
        {
            /* IP-TOS data qualify -- VOIP bearer */
            result = bcm_field_qualify_data(unit,
                                   cascaded_ent[i],
                                   dq_ipv4_tos.qual_id,
                                   &data,
                                   &mask,
                                   1);
            if (BCM_E_NONE != result)
            {
                printf("Error in bcm_field_qualify_data - grp_cascaded voip bearer, entry:%d\n", i);
                return result;
            }
        }

        result = bcm_field_action_add(unit,
                            cascaded_ent[i],
                            bcmFieldActionCascadedKeyValueSet,
                            (i | 0x80),
                            0xff);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_action_add - grp_cascaded entry:%d\n", i);
            return result;
        }
    }

    result = bcm_field_group_install(unit, grp_cascaded.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install - FG1 cascaded group\n");
        return result;
    }

    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
    *group_2 = grp.group; 
    *group_1 = grp_cascaded.group;

    return result;
}

