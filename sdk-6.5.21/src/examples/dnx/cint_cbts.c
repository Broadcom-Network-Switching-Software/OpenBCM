/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* 
cint that demonstrate CBTS- class based tunnel selection 
 
configuration: 
cint ../../../src/examples/sand/utility/cint_sand_utils_global.c 
cint ../../../src/examples/dnx/field/cint_field_utils.c 
cint ../../../src/examples/sand/cint_ip_route_basic.c   
cint ../../../src/examples/sand/cint_mpls_3_level_protection.c 
cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
cint ../../../src/examples/sand/cint_mpls_encapsulation_basic.c 
cint ../../../src/examples/sand/utility/cint_sand_utils_l3.c
cint ../../../src/examples/sand/cint_ecmp_basic.c
cint ../../../src/examples/dnx/cint_cbts.c 
c
cbts_main(0,200,201,202,203);
exit;

MPLS packet: 
tx 1 PSRC=200 DATA=000c0002000000110000011281000000884701e61140450000350000000080005874a0000011a0a1a1a2000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20000000000000000000000000000000000000 
 
Packet will be sent to ECMP with 8 members. 
Because ECMP_LB_KEY(2) in 3 MSB is according to MPLS exp 
than member in ECMP is selected according to exp 
This specifec example works with ECMP with 8 members 
*/ 

bcm_field_group_t   cint_field_hash_ipt_fg_id_ipmf2_hash = 0;
bcm_field_context_t cint_field_hash_ipt_context_id_ipmf1 = 0;
bcm_field_presel_t              cint_field_hash_ipt_presel_id_ipmf1 = 88;

bcm_field_group_t   cint_field_hash_ipt_fg_id_tcam = 0;
bcm_field_context_t cint_field_hash_ipt_context_id_ipmf3 = 0;
bcm_field_presel_t              cint_field_hash_ipt_presel_id_ipmf3 = 88;

int MEMBERS_IN_ECMP_GROUP = 8;

struct cint_mpls_ecmp_basic_info_s
{
    int in_port;                                                    /* incoming port */
    int out_port;                                                   /* outgoing port */
    int intf_in;                                                    /* in RIF */
    int intf_out;                                                   /* out RIF */
    bcm_mac_t intf_in_mac_address;                                  /* mac for in RIF */
    bcm_mac_t intf_out_mac_address;                                 /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                                     /* mac for out RIF */
    int arp_id;                                                     /* Id for ARP entry for push entry */
    bcm_ip_t dip;                                                   /* dip 1 */
    uint32 mask;                                                    /* mask for dip */
    int vrf;                                                        /* VRF */
    bcm_if_t fec_ids[MEMBERS_IN_ECMP_GROUP];                        /* FEC ids for the ecmp group */
    bcm_mpls_label_t ecmp_tunnel_labels[MEMBERS_IN_ECMP_GROUP];     /* Tunnels for each ecmp group member */
    bcm_if_t ecmp_intf;                                             /* ECMP interface id */
    int ingress_qos_profile;                                        /* Ingress qos profile */
};

cint_mpls_ecmp_basic_info_s cint_mpls_ecmp_basic_info =
{
    /*
     * ports : in_port | out_port 
     */
   200, 201,
    /*
     * intf_in | intf_out 
     */
    20, 42,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac | 
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x0f, 0x55}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * arp_id 
     */
    4097,
    /*
     * dip
     */
    0xA0A1A1A3 /* 160.161.161.163 */ ,
    /*
     * mask
     */
    0xfffffff0,
   /*
     * vrf
     */
    1,
    /*
     * fec_id_1 | fec_id_2 | fec_id_3
     */
    {50001, 50002, 50003},
    /*
     * ecmp_tunnel_labels
     */
    {0x101, 0x102, 0x103},
    /*
     * ecmp_intf
     */
    0x104,
    /*
     * ingress_qos_profile | egress_qos_profile
     */
    3
};


/**
* \brief
*  Creates and configure Context+FG for iPMF1
*  hash function takes 13 bit from orignal lb_key(lsbs) and 3
*  bit from MPLS exp(msbs) and copies to new lb_key
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_hash_ipt_main(int unit)
{


    bcm_field_group_info_t fg_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_info_t context_info;
    bcm_field_qualify_t qual_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_group_attach_info_t attach_info;

    /********************
     * Create Context for iPMF1/2
 */
    printf("Creating new context for the hashing\n");
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Hash_ipt_ipmf1_2", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_hash_ipt_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

 
        /**bcmFieldQualifyVrf = bcmFieldActionVSwitchNew = FWD_Domain*/
    bcm_field_qualifier_info_get(unit, bcmFieldQualifyEcmpLoadBalanceKey1, bcmFieldStageIngressPMF1, &qual_info_get);
    qual_info.size = 13; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "LB key", sizeof(qual_info.name));
    bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id);



    printf("Created Context %d for iPMF1 (can also be used for iPMF2)\n", cint_field_hash_ipt_context_id_ipmf1);


    /********************
     * Start iPMF1 configuration - Configure Hash stamp creation
 */

    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionXor16;
    hash_info.order = TRUE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyEcmpLbKey1;
    hash_info.key_info.nof_quals = 2;
    hash_info.key_info.qual_types[0] = bcmFieldQualifyEcmpLoadBalanceKey1;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyMplsLabelExp;
    hash_info.key_info.qual_types[0] = qual_id;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    hash_info.key_info.qual_info[0].input_arg = 0;
    hash_info.key_info.qual_info[0].offset = qual_info_get.offset; 
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    hash_info.key_info.qual_info[1].input_arg = 0;
    hash_info.key_info.qual_info[1].offset = 0;
    bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1,cint_field_hash_ipt_context_id_ipmf1,&hash_info );



    /********************
     * Configure Context Selection for iPMF1/2
 */

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_hash_ipt_presel_id_ipmf1;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_hash_ipt_context_id_ipmf1;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeForwardingMPLS;
    p_data.qual_data[0].qual_mask = 0x1f;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(IPv4) \n",
            cint_field_hash_ipt_presel_id_ipmf1,cint_field_hash_ipt_context_id_ipmf1);

    return 0;
}

/**
* \brief
* configures MPLS fwd to ECMP in hierarchy 2 
* configures ECMP_LB_KEY(2) in 3 MSB to be according to MPLS exp 
* this way the member is selected according to EXP 
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/

int
cbts_main(
    int unit,
    int port1,
    int port2,
    int port3,
    int port4)
{


    int rv = BCM_E_NONE;
    int ttl_value=0x33;
    int i=0;
    int j=0;
    bcm_l3_egress_ecmp_t ecmp;
    int dest_port;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_mpls_encapsulation_basic_info.fec_id_enc);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_mpls_encapsulation_basic_info.fec_id_fwd = cint_mpls_encapsulation_basic_info.fec_id_enc + 20;

    rv = cint_field_hash_ipt_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_hash_ipt_main \n", rv);
       return rv;
    }

      /*
     * Configure port properties for this application 
     */
    rv = mpls_encapsulation_basic_configure_port_properties(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_configure_port_properties\n");
        return rv;
    }

    /*
     * Configure L3 interfaces 
     */
    rv = mpls_encapsulation_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_l3_interfaces\n");
        return rv;
    }

    /*
     * Configure an ARP entry 
     */
    rv = mpls_encapsulation_basic_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_arp_entry\n");
        return rv;
    }

    /*
     * Configure a push or swap entry. The label in the entry will swap the swapped label in case 
     * context is forwarding. In case context is ip routing, label will be pushed. 
     */
    rv = mpls_encapsulation_basic_create_tunnels(unit, ttl_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_tunnels\n");
        return rv;
    }

    /*
     * Configure fec entry for encapsulation flow
     */
    rv = l3__egress_only_fec__create(unit, cint_mpls_encapsulation_basic_info.fec_id_enc,
                                     cint_mpls_encapsulation_basic_info.push_tunnel_id, 0,
                                     cint_mpls_encapsulation_basic_info.out_port, BCM_L3_2ND_HIERARCHY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }


    for (i = 0; i < MEMBERS_IN_ECMP_GROUP; i++)
    {
        
        if (i>5)
        {
           dest_port = port4;
        }
        else if (i>3)
        {
            dest_port = port3;
        }
        else if (i>1)
        {
            dest_port = port2;
        }
        else
        {
            dest_port = port1;
        }
        cint_mpls_ecmp_basic_info.fec_ids[i] = cint_mpls_encapsulation_basic_info.fec_id_fwd + i;
        rv = l3__egress_only_fec__create(unit, cint_mpls_ecmp_basic_info.fec_ids[i], cint_mpls_encapsulation_basic_info.push_tunnel_id, 0, dest_port, BCM_L3_2ND_HIERARCHY);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_encapsulation_basic_create_fec_entry_swap\n");
            return rv;
        }
    }


    rv = l3__ecmp_create(unit, cint_mpls_ecmp_basic_info.ecmp_intf, 8, cint_mpls_ecmp_basic_info.fec_ids, BCM_L3_WITH_ID |BCM_L3_2ND_HIERARCHY, BCM_L3_ECMP_DYNAMIC_MODE_DISABLED,&ecmp);
    if (rv != BCM_E_NONE)
    {
        printf(" Error, in mpls_ecmp_basic_ecmp_create\n");
        return rv;
    }

    /*
     * Configure an ILM entries
     */
    rv = mpls_encapsulation_basic_create_ilm_switch_tunnel_create(unit,
                                                                  cint_mpls_encapsulation_basic_info.swapped_label,
                                                                  cint_mpls_ecmp_basic_info.ecmp_intf);

    /*
     * Connect MPLS tunnel entry to ARP.
     * ARP entry configuration can be also done inside bcm_mpls_tunnel_inititator_create.
     */
    rv = mpls_encapsulation_basic_update_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_arp_entry\n");
        return rv;
    }

    /*
     * Create l3 forwarding entry for the pushed label
     */
    rv = mpls_encapsulation_basic_create_l3_forwarding(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_l3_forwarding\n");
        return rv;
    }
    return rv;


}
