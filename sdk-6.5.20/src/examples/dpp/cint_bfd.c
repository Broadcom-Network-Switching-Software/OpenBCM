/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BFD test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_bfd.c
* Purpose: Example of using BFD APIs.
*
* Usage:
* 
* 
* 
* To run IPV4 test:
* BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
* BCM> cint examples/dpp/utility/cint_utils_l3.c
* BCM> cint examples/dpp/utility/cint_utils_vlan.c
* BCM> cint examples/sand/utility/cint_sand_utils_global.c
* BCM> cint examples/dpp/utility/cint_utils_oam.c
* BCM> cint examples/dpp/cint_ip_route.c
* BCM> cint examples/dpp/cint_bfd.c
* If you run on ARAD+ you also require BCM> cint examples/dpp/cint_field_bfd_ipv4_single_hop.c
* BCM> cint
* bfd_ipv4_run_with_defaults(0,13,14,-1,0,BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP,0,0);
*
* To run MPLS test:
* BCM> cint examples/sand/utility/cint_sand_utils_global.c
* BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
* BCM> cint examples/dpp/utility/cint_utils_l3.c
* BCM> cint examples/dpp/utility/cint_utils_vlan.c
* BCM> cint examples/dpp/utility/cint_utils_oam.c
* BCM> cint examples/dpp/cint_ip_route.c
* BCM> cint examples/dpp/cint_bfd.c
* BCM> cint
* bfd_mpls_run_with_defaults(0,13,14);
* exit;
*
* To run PWE tests:
* BCM> cint examples/sand/utility/cint_sand_utils_global.c
* BCM> cint examples/dpp/utility/cint_utils_l3.c
* BCM> cint examples/dpp/utility/cint_utils_l2.c
* BCM> cint examples/dpp/utility/cint_utils_multicast.c
* BCM> cint examples/dpp/utility/cint_utils_vlan.c
* BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
* BCM> cint examples/dpp/utility/cint_utils_mpls_port.c
* BCM> cint examples/dpp/utility/cint_utils_oam.c
* BCM> cint examples/dpp/cint_port_tpid.c
* BCM> cint examples/dpp/cint_qos.c
* BCM> cint examples/dpp/cint_vswitch_vpls.c
* BCM> cint examples/dpp/cint_bfd.c
* BCM> cint examples/dpp/cint_mpls_lsr.c
* BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
* cint
* int type = 1; or int type = 2; or int type = 4;
* #Type 1: BFD over CW (ACH)  over PWE
* #Type 2: PWE  over router alert
* #Type 4: BFD over G-ACH over GAL over PWE.
* bfd_pwe_run_with_defaults(0,13,14,type);
* 
* comments:
* 1) In order to prevent from OAMP send packets do: BCM.0> m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0
* 2) gport, egress_if, tx_dest_system_port in different types:
* IP:
* bfd_endpoint_info.gport = 0; in lif - no need for this field in IPv4
* bfd_endpoint_info.egress_if = 0x40001002; next hop mac at encap-id... Taken from create_l3_egress(unit,flags,out_port,out_vlan,ing_intf_out,next_hop_mac, &fec, &encap_id)
* bfd_endpoint_info.tx_dest_system_port = 0x400000d; out port
* MPLS:
* bfd_endpoint_info.gport = 0; in lif: tunnel_switch.tunnel_id
* bfd_endpoint_info.egress_if =  0x40001000; out lif: tunnel_id in example_ip_to_tunnel
* bfd_endpoint_info.tx_dest_system_port = 0x400000d; out port
* PWE:
* bfd_endpoint_info.gport = 0x4c004000; in lif: tunnel_switch.tunnel_id
* bfd_endpoint_info.egress_if =  0x40001000; out lif: mpls_port->encap_id
* bfd_endpoint_info.tx_dest_system_port = 0x400000d; out port
* 
* 
* This cint also test bfd echo. In order to enable BFD echo set "bfd_echo"
* BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
* BCM> cint examples/dpp/utility/cint_utils_l3.c
* BCM> cint examples/dpp/utility/cint_utils_vlan.c
* BCM> cint examples/sand/utility/cint_sand_utils_global.c
* BCM> cint examples/dpp/utility/cint_utils_oam.c
* BCM> cint examples/dpp/cint_ip_route.c
* BCM> cint examples/dpp/cint_bfd.c
* BCM> cint examples/dpp/cint_field_bfd_echo.c
* If you run on ARAD+ you also require BCM> cint examples/dpp/cint_field_bfd_ipv4_single_hop.c
* BCM> cint
* bfd_echo_example(0,13,14,0,-1,0);
* 
* To run BFD IPv4 single hop test:
* cd ../../../../regress/bcm
* BCM> cint examples/sand/utility/cint_sand_utils_global.c
* BCM> examples/sand/utility/cint_sand_utils_mpls.c
* BCM> cint examples/dpp/utility/cint_utils_l3.c
* BCM> cint examples/dpp/utility/cint_utils_vlan.c
* BCM> cint examples/dpp/utility/cint_utils_oam.c
* BCM> cint examples/dpp/cint_ip_route.c
* BCM> cint examples/dpp/cint_bfd.c
* BCM> cint examples/dpp/cint_field_bfd_ipv4_single_hop.c
* cint
* bfd_ipv4_run_with_defaults(0,13,14,-1,0,BCM_BFD_ENDPOINT_IN_HW,0,0);

*/

/* set in order to do rmep_id encoding using utility functions and not using APIs. */
int encoding = 0;
bcm_bfd_endpoint_t remote_id_system = 0;
bcm_bfd_endpoint_t local_id_system = 0;
int is_server = 0;
int bfd_echo_grp_pri = 9;

/* Globals - MAC addresses , ports & gports*/
int next_hop_mac;
int tunnel_id;


int remote_gport_trap_code = 0;

/* context id initialization, will be retrieved from field_presel_fwd_layer_main */
bcm_field_context_t cint_bfd_echo_context_id_ipmf1 = BCM_FIELD_CONTEXT_ID_INVALID;
/* context priority */
bcm_field_presel_t  cint_bfd_echo_presel_id = 10;

/* GAL is a reserved label with value 13 */
int gal_label = 13;

/*enable bfd echo, 1- implementation without lem, 2 - implementation with lem*/
int bfd_echo=0;
/*bfd echo field group*/
bcm_field_group_t echo_group;
/* bfd echo qualifier*/
int echo_qual_id;

/*enable bfd ipv4 single hop*/
int single_hop_extended =0;

/* When enabled the IPv4 SIP, generated by the OAMP, is
* configured by the user and replaced by the PRGE. 
* Soc property bfd_extended_ipv4_src_ip must be enabled as well. */
int bfd_extended_sip = 0;

/*Trap code for snooping accelerated packets*/
int trap_code_4_snooping = 0;
int use_trap_with_snoop = 0;

/* Id of the created the created BFD EP*/
int bfd_id = 0;

/* int_pri of bfd_ipv4 endpoint */
int int_pri = 0;

/* Created endpoints information */
oam__ep_info_s bfd_ep3;

/*State change event count for BFD */
int state_change_events_count_bfd = 0;
int is_field_created = 0;

/* BFD local_discr */
uint32 bfd_local_discr = 0x30004;

/* BFD IPv4 FLEX IPTOS */
uint8 bfd_ipv4_flex_iptos = 0;

int ipv6_echo_trap_code = 0;
int ipv6_single_tag = 0;
int ipv6_in_port = 0;

int system_endpoint_to_endpoint_id_bfd(bcm_bfd_endpoint_info_t *mep_info) {
    if (!(mep_info->opcode_flags & BCM_BFD_ENDPOINT_IN_HW)) {
        printf("Can not create non-accelerated endpoint with id\n");
        return BCM_E_FAIL;
    }

    if (mep_info->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) {
        mep_info->remote_id |= (1 << 25 /*_BCM_OAM_REMOTE_MEP_INDEX_BIT*/);
    }
    return BCM_E_NONE;
}

/* In BFD ACHO the PMF replace the classifier.
If the packet is identified as BFD echo (TCAM match- udp_src_port, dest_ip, local descrimiantor), 
The action is to set the trap code to bcmRxTrapBfdEchoOverIpv4/bcmRxTrapOamBfdIpv6 and Trap-Qualifier=BFD.Your-Discriminator[15:0].
The Trap-Qualifier is the MEP-ID used by the OAMP to access the MEP-DB */

int bfd_echo_field_classifier(int unit, int is_ipv6) {
    bcm_field_presel_set_t presel_set;  
    int presel_id;

    bcm_field_group_config_t grp;
    int group_id = 0; 

    bcm_field_aset_t aset;

    bcm_field_entry_t action_entry;

    bcm_field_entry_t ent;
    uint32 out_port=40;
    int sys_gport;
    bcm_field_stage_t stage;
    bcm_field_data_qualifier_t your_descriminator;
    bcm_field_data_qualifier_t trap_code;
    bcm_field_data_qualifier_t strength;
    int result=0;



    bcm_gport_t trap_gport;
    int rc;
      
    int trap_id;
    int rv;

    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv); 

    if(!is_ipv6) { 
       if (device_type < device_type_jericho2) {
        rc = bcm_rx_trap_type_get(unit,0, bcmRxTrapBfdEchoOverIpv4 ,&trap_id);
       } else {
          rv =  bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n", bcm_errmsg(rv));
              return rv;
          }

       }
    } else {
        trap_id = ipv6_echo_trap_code;
    }

    if (rc != BCM_E_NONE) 
    {
    printf ("bcm_rx_trap_type_get failed: %x \n", rc);
    }
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, 7,0); 

    if (verbose >= 1) {
        printf("Step: bfd echo classifier - trap id %d \n", trap_id);
    }

    /*create user define quelifier*/
    bcm_field_data_qualifier_t_init(&your_descriminator); /* BFD.Your-Discriminator[15:0] */
    your_descriminator.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    your_descriminator.offset_base = bcmFieldDataOffsetBaseThirdHeader;
    your_descriminator.offset = 10; /* points to the lsb of you.descriminator */
    your_descriminator.length = 2 * 8; /* bits */
    result = bcm_field_data_qualifier_create(unit, &your_descriminator);
    if (BCM_E_NONE != result) {
    print bcm_field_show(unit,"");
    return result;
    }


    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);

    /* 
     * Define Programabble Field Group
     */ 

    grp.priority = bfd_echo_grp_pri;


    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);

    result = bcm_field_qset_data_qualifier_add(unit,
                                             grp.qset,
                                             your_descriminator.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n",result);
        return result;
    }

    if(!is_ipv6) { 
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp);
    } else {
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp6);
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);
    } 
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL4DstPort);



    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n",result);
        return result;
    }


    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    if(is_ipv6) { 
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionOam);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    }
    /* Attached the action to the field group */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n",result);
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

    echo_group=grp.group;
    echo_qual_id=your_descriminator.qual_id;

    return result;

}

int bfd_echo_field_classifier_entry_add(int unit, bcm_field_group_t group, int qual_id,uint32 local_discr, bcm_gport_t remote_gport, int is_ipv6){

    bcm_field_group_config_t grp;
    int group_id = 0; 
    bcm_field_entry_t action_entry;
    bcm_field_entry_t ent;
    int sys_gport;
    bcm_error_t result=0;
    bcm_gport_t trap_gport;
    int trap_id;

    if (remote_gport == BCM_GPORT_INVALID) {
        printf("remote gport invalid\r\n");
        if(!is_ipv6) { 
            result = bcm_rx_trap_type_get(unit,0, bcmRxTrapBfdEchoOverIpv4 ,&trap_id);
        } else {
           trap_id = ipv6_echo_trap_code;
        }
        if (BCM_E_NONE != result) {
            printf ("bcm_rx_trap_type_get failed:,(%s) \n", bcm_errmsg(result));
            return result;
        }

        BCM_GPORT_TRAP_SET(trap_gport, trap_id, 7,0);
    } else {
        trap_gport = remote_gport;
        printf("trap gport set to 0x%x\r\n", remote_gport);
    }

    /*create an entry*/
    result = bcm_field_entry_create(unit, group, &action_entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n",bcm_errmsg(result));
        return result;
    }

    if(is_ipv6) { 
        int data_port= ipv6_in_port;

        result = bcm_field_qualify_InPort(unit, action_entry, data_port, BCM_FIELD_EXACT_MATCH_MASK);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_InPort Err, (%s) \n",bcm_errmsg(result));
            return result;
        }
    }
    bcm_l4_port_t data_port=0x0ec9 ;
    bcm_l4_port_t mask_port = 0xffff;

    result = bcm_field_qualify_L4DstPort(unit, action_entry, data_port, mask_port);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_L4SrcPort Err, (%s) \n",bcm_errmsg(result));
        return result;
    }

    if(!is_ipv6) { 
        bcm_ip_t data_ip=0x7fffff03;
        bcm_ip_t mask_ip=0xffffffff;
        result = bcm_field_qualify_DstIp(unit, action_entry, data_ip, mask_ip);
    } else {
        bcm_ip6_t dip = {0xFE,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
        bcm_ip6_t dip_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        result = bcm_field_qualify_DstIp6(unit, action_entry, dip, dip_mask);
    }
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err, (%s) \n",bcm_errmsg(result));
        return result;
    }

    uint16 tmp_local_discr=local_discr;
    uint8 data[2];

    data[1]=local_discr&0xff;
    local_discr=local_discr>>8;
    data[0]=local_discr&0xff;

    uint8 mask[2]= {0xff,0xff};
    result = bcm_field_qualify_data(unit, action_entry, qual_id, data, mask, 2);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data (%s) \n",bcm_errmsg(result));
        return result;
    } 

    if(!is_ipv6) { 
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionTrap, trap_gport, tmp_local_discr);
    } else {
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionTrap, trap_gport, 0x0005);
    }

    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit,ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    if(is_ipv6) {

        if(ipv6_single_tag == 1) { 
            /*OAM Offset is 0x42 */
            result = bcm_field_action_add(unit, action_entry, bcmFieldActionOam, 0x2200, 0);
        }
        else {
            /*OAM Offset is 0x46 */
            result = bcm_field_action_add(unit, action_entry, bcmFieldActionOam, 0x2600, 0);
        }
        if (result != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
        }
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionDrop, 0, 0);
        if (result != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
        }
    }

    result = bcm_field_entry_install(unit, action_entry);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install (%s) \n",bcm_errmsg(result));
        return result;
    }

    return result;
}

int bfd_echo_pmf_config(int unit) 
{
    bcm_error_t rv;

    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv); 

    if (device_type < device_type_jericho2) {
      rv = bfd_echo_field_classifier(unit, 0);
      if (rv != BCM_E_NONE) {
          printf("Error bfd_echo_field_trap_qualifier.\n");
          return rv;
      }

      rv = bfd_echo_field_classifier_entry_add(unit, echo_group, echo_qual_id,0x4,srv_ep_rem_gport, 0);
      if (BCM_E_NONE != rv) {
          printf("Error in bfd_echo_field_classifier_entry_add Err %x\n",rv);
          return rv;
      }
    }
    else {
       bfd_echo = 1;
       rv = field_presel_fwd_layer_main(unit,cint_bfd_echo_presel_id, bcmFieldStageIngressPMF1, bcmFieldLayerTypeIp4, &cint_bfd_echo_context_id_ipmf1, "ipv4 hit");
       if (rv != BCM_E_NONE)
       {
           printf("Error (%d), in cint_field_always_hit_context_main Ingress\n", rv);
           return rv;
        }
        rv = cint_field_bfd_echo_main(unit, cint_bfd_echo_context_id_ipmf1, 0);
        if (rv != BCM_E_NONE) {
          printf("Error cint_field_bfd_echo_main.\n");
          return rv;
        }
        rv = cint_field_bfd_echo_entry_add(unit, 0x7fffff03, NULL, bfd_local_discr & 0xFFFF);
        if (rv != BCM_E_NONE) {
          printf("Error cint_field_bfd_echo_entry_add.\n");
          return rv;
        }
    }

    return rv;
}

int bfd_ipv6_echo_config(int unit, int serv_type, int bfd_in_port, int local_discr, bcm_gport_t remote_gport)
{
    bcm_error_t rv;
    bcm_rx_trap_config_t ipv6_trap_config;
    bcm_rx_trap_config_t trap_config;
    int bfd_ipv6_trap_code = 0;

    sal_memset(&ipv6_trap_config, 0 , sizeof(ipv6_trap_config));

    rv = bcm_rx_trap_type_get(unit,0, bcmRxTrapBfdEchoOverIpv6,&bfd_ipv6_trap_code);
    BCM_IF_ERROR_RETURN(rv);
    
    printf("IPv6 echo Trap code is %d\n", bfd_ipv6_trap_code);
    ipv6_echo_trap_code = bfd_ipv6_trap_code;
    ipv6_single_tag = serv_type;
    ipv6_in_port = bfd_in_port;
    rv = bfd_echo_field_classifier(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error bfd_echo_field_trap_qualifier.\n");
        return rv;
    }

    /*
    rv = bfd_echo_field_classifier_entry_add(unit, echo_group, echo_qual_id,5010,BCM_GPORT_INVALID, 1);
    */
    rv = bfd_echo_field_classifier_entry_add(unit, echo_group, echo_qual_id,local_discr,remote_gport, 1);
    if (BCM_E_NONE != rv) {
        printf("Error in bfd_echo_field_classifier_entry_add Err %x\n",rv);
        return rv;
    }

    return rv;
}

 
int bfd_echo_example(int unit, int port1, int port2, int is_acc, int remote_port, int is_with_lem) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;
    bcm_gport_t remote_gport;
    int trap_code;

    /*enable BFD echo*/
    if (is_with_lem) {
        bfd_echo = 2; 
    } else {
        bfd_echo = 1;
    }

    if (remote_port == -1) {
      remote_gport = BCM_GPORT_INVALID;
    } else {
        if (is_server) {
            BCM_GPORT_TRAP_SET(remote_gport, remote_port, 0, 0);
        } else {
            BCM_GPORT_LOCAL_SET(remote_gport, remote_port);

            /* remote_gport field must be a trap */
            rv = bfd_create_trap_from_gport(unit, remote_gport, &trap_code);
            if (rv != BCM_E_NONE) {
                printf("Error bfd_create_trap_from_gport.\n");
                return rv;
            }
            BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
        }
    }

    if (bfd_echo == 1) {
       rv = bfd_echo_field_classifier(unit, 0);
       if (rv != BCM_E_NONE) {
         printf("Error bfd_echo_field_trap_qualifier.\n");
         return rv;
       }

       rv = bfd_echo_field_classifier_entry_add(unit, echo_group, echo_qual_id,0x4,BCM_GPORT_INVALID, 0);
       if (BCM_E_NONE != rv) {
           printf("Error in bfd_echo_field_classifier_entry_add Err %x\n",rv);
           return rv;
       } 
    } else {
        rv = bfd_echo_with_lem_preselector(unit);
        if (rv != BCM_E_NONE) {
          printf("Error bfd_echo_with_lem_preselector.\n");
          return rv;
        } 
    }

    rv = bfd_ipv4_example_init(unit, port_1, port_2, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("-------------init \n");


    rv = bfd_ipv4_example(unit, remote_gport, BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ECHO | BCM_BFD_ENDPOINT_MULTIHOP, 0);
    if (rv != BCM_E_NONE) {
      printf("Error bfd_ipv4_example.\n");
      return rv;
    }

    rv = register_events(unit);
    return rv;
}


/**
* Use to calcualate "expeted" ip header checksum
* "expeted" packet format as below:
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Vers | IHL|   DSCP   |  ECN   |         Totol Length          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|    Identification (0)         |    Flags |  Fragment Offset   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|    Time To Live Protocol      |   Header Checksum (assuming DIP==0)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Source IP Address                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  Dest IP Address (0.0.0.0)                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* @author xiao
*
* @param unit
*
* @return int
*/
void sbfd_reflector_expected_checksum_get( int unit,uint16 *expected_checksum, int is_mpls) 
{
    int ip_field[10] = {0x45ff, 0x0034, 0x0000, 0x0000, 0xff11, 0x0, 0x030f, 0x0701 ,0x0, 0x0}; 
    int mpls_ip_field[10] = {0x45ff, 0x0034, 0x0000, 0x0000, 0x0111, 0x0, 0x7f00, 0x01, 0x0, 0x0}; 
    int checksum = 0;
    int i = 0;

    for(i=0; i<10; i++)
    {
        checksum+= (is_mpls ? mpls_ip_field[i] : ip_field[i]);   
    }
    checksum = (checksum>>16) + (checksum & 0xffff);   
    checksum+= (checksum>>16);
    checksum = 0xffff - checksum;   
    printf("checksum=%x \n",checksum);
    
    *expected_checksum = checksum;
    return ;
}

/**
* Set Cascade PMF for SBFD IP checksum calculation and trap packet to recycle port
* Group a:
   Qualify: DIP==My-SBFD-Reflector-DIP(or  ttl==1 for mpls)  && Dest-UDP-Port==7784
*  Action: Trap ( trap config: LIF = Dummy ARP LIF  Destination = SBFDReflector recycle port)
*          Set Cascade Key
* Group b:
   Qualify: Cascade Key
*  Action: UDH (3B) = ~(expected checksum) + SIP[15:0] (17b result)
*
* @author xiao
*
* @param unit
*
* @return int
*/
int sbfd_reflector_src_ip_pmf_config( int unit,bcm_port_t recycle_port, int dummy_lif, int is_mpls) 
{
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    uint32 flags;
    bcm_mac_t mymac  = {0x00, 0x0c, 0x00, 0x02,0x00,0x22};
    bcm_rx_trap_config_t trap_config_sbfd;
    int trap_code_sbfd;
    int result;
    int auxRes;
    bcm_field_group_config_t grp_config;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent_a;
    bcm_field_entry_t ent_b;
    bcm_gport_t gport;
    bcm_field_group_t group_a = 12;
    bcm_field_group_t group_b = 13;
    bcm_field_ForwardingType_t forwarding_type;
    bcm_field_extraction_field_t extraction;
    bcm_field_extraction_action_t extract;
    int group_pa = 13;
    int group_pb = 14; 
    uint16 expected_checksum = 0;
    uint16 complement_checksum = 0;
    bcm_ip_t dst_ip = 0x30F0701;
    bcm_ip_t dst_ip_mask = 0xffffffff;
    int presel_id;
    bcm_field_entry_t presel_flags = 0;
    bcm_field_presel_set_t presel_set;  
    bcm_field_presel_set_t psset;

    /* 
    * Set the preselector 
    */
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
        result = bcm_field_presel_create(unit, &(presel_id));
    }
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create\n");
        return result;
    }
    
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }
    
    /* Define the preselector-set */
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);
    
    /*
    * Set the cascaded key length to 20 bits
    */
    result = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth, 20 /* bits in cascaded key */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_control_set\n");
        return result;
    }
    bcm_field_group_config_t_init(&grp_config);
    grp_config.group = group_b;
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp_config.qset);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyCascadedKeyValue);
    BCM_FIELD_QSET_ADD(grp_config.qset, (is_mpls? bcmFieldQualifyInnerSrcIp : bcmFieldQualifySrcIp));
    
    /*
    *  This Field Group change the UDH
    */
    BCM_FIELD_ASET_INIT(grp_config.aset);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionClassDestSet);
    
    /*  Create the Field group */
    grp_config.priority = group_pb;
    grp_config.group = group_b;
    
    grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET| BCM_FIELD_GROUP_CREATE_WITH_PRESELSET; 
    grp_config.mode = bcmFieldGroupModeDirectExtraction;
    grp_config.preselset = presel_set;
    result = bcm_field_group_config_create(unit, &grp_config);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        auxRes = bcm_field_group_destroy(unit, grp_config.group);
        return result;
    }
    
    /*
    *  Add entry to the Field group.
 */
    result = bcm_field_entry_create(unit, grp_config.group, &ent_b);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_group_destroy(unit, grp_config.group);
        return result;
    }

    result = bcm_field_qualify_CascadedKeyValue(unit, ent_b, 0x80000, 0x80000);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_CascadedKeyValue\n");
        return result;
    }
    
    /* calculate expected_checksum*/
    sbfd_reflector_expected_checksum_get(unit,&expected_checksum,is_mpls);

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&extraction);
    complement_checksum = ~ expected_checksum;
    /* Build the action to change the  UDH (3B) = ~(expected checksum) + SIP[15:0] (17b result) */
    extract.action = bcmFieldActionClassDestSet;
    extract.bias = complement_checksum; /* ~Check sum */
    extraction.flags = 0;
    extraction.bits = 16;   /* SIP[15:0] bits */
    extraction.lsb = 0;
    extraction.qualifier = (is_mpls? bcmFieldQualifyInnerSrcIp : bcmFieldQualifySrcIp); /* sip */
    result = bcm_field_direct_extraction_action_add(unit,
      ent_b,
      extract,
      1 /* count */,
      &extraction);
    
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        auxRes = bcm_field_group_destroy(unit, grp_config.group);
        return result;
    }
    
    result = bcm_field_group_install(unit, grp_config.group);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp_config.group);
        return result;
    } 

    
    /********** group a for lookup and trap packet **********/
    /*1, create Dummy ARP LIF*/
    /*1-1, on 1st pass, create ARP LIF use to add ETH header for SBFD packet */
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, mymac, 6); /*Destination MAC address*/
    l3eg.vlan = 101; /* DA + VLAN will be MyMAC*/
    l3egid = 0;
    flags = (BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID);
    
    /*2 , Create TRAP to trap SBFD packet to recycle port */
    result = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_sbfd);
    if (result != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(result));
        return result;
    }
    bcm_rx_trap_config_t_init(&trap_config_sbfd);
    trap_config_sbfd.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID |BCM_RX_TRAP_UPDATE_DEST;
    trap_config_sbfd.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config_sbfd.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
    
    l3eg.port = recycle_port;
    l3eg.encap_id = dummy_lif; /*0x40002002*/
    result = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
    if (result != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(result));
        return result;
    }
    
    trap_config_sbfd.encap_id = l3eg.encap_id & 0x3fffff;
    trap_config_sbfd.dest_port = recycle_port;
    result = bcm_rx_trap_set(unit, trap_code_sbfd, &trap_config_sbfd);
    if (result != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(result));
        return result;
    }
    
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp_config.qset);
    if(is_mpls){
        BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInnerL4DstPort);
        BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInnerTtl);
    }
    else{
        BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyL4DstPort);
        BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyDstIp);
    }
    
    BCM_FIELD_ASET_INIT(grp_config.aset);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionCascadedKeyValueSet);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionTrap);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionOam);

    /*  Create the Field group */
    grp_config.group = group_a;
    grp_config.priority= group_pa;
    
    grp_config.flags =  BCM_FIELD_GROUP_CREATE_WITH_ID |BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET/* | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET*/;
    /* use Direct table for the first lookup field group */
    grp_config.mode = bcmFieldGroupModeAuto;
    grp_config.preselset = psset;
    result = bcm_field_group_config_create(unit, &grp_config);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        return result;
    }
    
    /*
    *  Add an entry to the group.
    */
    result = bcm_field_entry_create(unit, group_a, &ent_a);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    
    if(is_mpls){
        result = bcm_field_qualify_InnerTtl(unit,ent_a,1, 0xff);
        if (result != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
            return result;
        }
        result = bcm_field_qualify_InnerL4DstPort(unit,ent_a,7784, 0xffff);
        if (result!= BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
            return result;
        }
        /*{OAM-Up-Mep, OAM-Sub-Type, OAM-offset, OAM-Stamp-Offset}*/
        /* OAM Offset is 0x34,which should be adjusted by vlan tag format */
        result = bcm_field_action_add(unit, ent_a, bcmFieldActionOam, 0x1a00, 0);
        if (result != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
        }
    }
    else{
        result = bcm_field_qualify_DstIp(unit,ent_a,dst_ip, dst_ip_mask);
        if (result != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
            return result;
        }
        result = bcm_field_qualify_L4DstPort(unit,ent_a,7784, 0xffff);
        if (result != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(result));
           return result;
        }
        /*OAM Offset is 0x30 */
        result = bcm_field_action_add(unit, ent_a, bcmFieldActionOam, 0x1800, 0);
        if (result != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(result));
        }
    }
    
    /* Change the cascaded value to be the nickname */
    result = bcm_field_action_add(unit, ent_a, bcmFieldActionCascadedKeyValueSet, 0x80000, 0x80000);
    if (BCM_E_NONE != result) {
    printf("Error in bcm_field_action_add\n");
    return result;
    }
    
    BCM_GPORT_TRAP_SET(gport, trap_code_sbfd, 7, 0);
    
    result = bcm_field_action_add(unit, ent_a, bcmFieldActionTrap, gport, 0);
    if (result != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(result));
        return result;
    }
    
    /* Install all to the HW */
    result = bcm_field_group_install(unit, group_a);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }
    
    return result;
}



/**
* Set PMF for SBFD reflector
*  Qualify: DIP==My-SBFD-Reflector-DIP(or  ttl==1 for mpls)  && Dest-UDP-Port==7784
*  Action: Trap ( trap config: LIF = Dummy ARP LIF  Destination = SBFDReflector recycle port)
*
* @author xiao
*
* @param unit
* @param recycle_port
* @param dummy_lif
* @param is_mpls
*
* @return int
*/
int sbfd_reflector_pmf_config(int unit, bcm_port_t recycle_port, int dummy_lif,int is_mpls)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    uint32 flags;
    bcm_mac_t mymac  = {0x00, 0x0c, 0x00, 0x02,0x00,0x22};
    bcm_rx_trap_config_t trap_config_sbfd;
    int trap_code_sbfd;
    bcm_field_aset_t  aset;
    bcm_field_qset_t qset;
    bcm_port_t in_port;
    /* pmf group*/
    bcm_field_group_t sbfd_group;
    /*pmf action entry*/
    bcm_field_entry_t sbfd_entry;
    bcm_ip_t dst_ip = 0x30F0701;
    bcm_ip_t dst_ip_mask = 0xffffffff;
    bcm_gport_t gport;

    /*1, create Dummy ARP LIF*/
    /*1-1, on 1st pass, create ARP LIF use to add ETH header for SBFD packet */
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, mymac, 6); /*Destination MAC address*/
    l3eg.vlan = 101; /* DA + VLAN will be MyMAC*/
    l3egid = 0;
    flags = (BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID);

    /*2 , Create TRAP to trap SBFD packet to recycle port */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_sbfd);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    bcm_rx_trap_config_t_init(&trap_config_sbfd);
    trap_config_sbfd.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID |BCM_RX_TRAP_UPDATE_DEST;
    trap_config_sbfd.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config_sbfd.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;

    l3eg.port = recycle_port;
    l3eg.encap_id = dummy_lif; /*0x40002002*/
    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    trap_config_sbfd.encap_id = l3eg.encap_id & 0x3fffff;
    trap_config_sbfd.dest_port = recycle_port;
    rv = bcm_rx_trap_set(unit, trap_code_sbfd, &trap_config_sbfd);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_QSET_INIT(qset);
    if(is_mpls){
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerL4DstPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerTtl);
    }
    else{
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
    }

    rv = bcm_field_group_create(unit, qset, 1, &sbfd_group);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    rv = bcm_field_group_action_set(unit, sbfd_group, aset);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_field_entry_create(unit, sbfd_group, &sbfd_entry);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }
    
    if(is_mpls){
        rv = bcm_field_qualify_InnerTtl(unit,sbfd_entry,1, 0xff);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_field_qualify_InnerL4DstPort(unit,sbfd_entry,7784, 0xffff);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    else{
        rv = bcm_field_qualify_DstIp(unit,sbfd_entry,dst_ip, dst_ip_mask);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
       rv = bcm_field_qualify_L4DstPort(unit,sbfd_entry,7784, 0xffff);
       if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
       }
    }
    
    BCM_GPORT_TRAP_SET(gport, trap_code_sbfd, 7, 0);

    rv = bcm_field_action_add(unit, sbfd_entry, bcmFieldActionTrap, gport, 0);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_field_group_install(unit, sbfd_group);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;

}


/**
* Set in-ttl to 255 for SBFD reflector
* TTL should be 255 for reflector packet, but we can't modify it in PRGE program since the TTL chang will cause checksum changes
* We don't have enough instrutions recaculate checksum, so using PMF change it.
* After that, TTL could be decreased from 255 to 254 , this is not a detail covered by the RFC. 
*
* @author xiao
*
* @param unit
* @param recycle_port0
* @param recycle_port1
*
* @return int
*/
int sbfd_reflector_ttl_set(int unit, bcm_port_t recycle_port0, bcm_port_t recycle_port1)
{
    bcm_field_aset_t  aset;
    bcm_field_qset_t qset;
    int rv;
    bcm_port_t in_port;
    /* pmf group*/
    bcm_field_group_t ttl_group;
    /*pmf action entry*/
    bcm_field_entry_t ttl_entry;
    int no_of_entry = 1;
    int index = 0;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    rv = bcm_field_group_create(unit, qset, 12, &ttl_group);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTtlSet);
    rv = bcm_field_group_action_set(unit, ttl_group, aset);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if(recycle_port1!= 0){
        no_of_entry = 2;
    }
    for(index = 0; index < no_of_entry; index++){
        rv = bcm_field_entry_create(unit, ttl_group, &ttl_entry);
        if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
        }

        in_port = ((index == 0) ? recycle_port0 : recycle_port1);

        rv = bcm_field_qualify_InPort(unit,ttl_entry,in_port, BCM_FIELD_EXACT_MATCH_MASK);
        if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
        }

        rv = bcm_field_action_add(unit, ttl_entry, bcmFieldActionTtlSet, 255, 0);
        if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
        }
    }

    rv = bcm_field_group_install(unit, ttl_group);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;

}

/**
* Example for non_bfd_forwarding on two cores
*
* @author xiao
*
* @param unit
* @param inport1
* @param inport2
* @param core_0_rcy_port
* @param core_1_rcy_port
* @param outport1
* @param outport2
* @return int
*/
void non_bfd_forwarding_two_cores_example(int unit,int inport1, int inport2, int core_0_rcy_port, int core_1_rcy_port, int outport1, int outport2)
{
    int in_port[2] = {0};
    int in_vid[2]  = {13, 15};
    int in_vsi[2]  = {1000, 3000};
    int in_rif[2]  = {1000, 3000};
    int out_port[2] = {0};
    bcm_mac_t in_rif_mac[2] = { {0x00, 0x0c, 0x00, 0x02, 0x00, 0x13}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x15} };
    bcm_mac_t l2_station_mask  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t station;
    int station_id;
    bcm_vlan_port_t in_vp[2];
    bcm_l3_intf_t l3_in_rif;
    uint32 dip=0x7fffff00;       
    bcm_l3_host_t l3host;
    int rcy_port[2];
    int core_i;
    int rv;

    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_gport_t src_gport[2];
    bcm_gport_t dst_gport[2];
    int src_i, dst_i, entry_i;
    bcm_field_action_core_config_t core_config_arr[2];
    int statId;
    
    if (core_0_rcy_port <= 0 && core_1_rcy_port <= 0) {
        printf("Must set at least one rcy port > 0\n");
        return;
    }
    in_port[0] = inport1;
    in_port[1] = inport2;
    out_port[0] = outport1;
    out_port[0] = outport2;
    rcy_port[0] = (core_0_rcy_port > 0 ? core_0_rcy_port : core_1_rcy_port);
    rcy_port[1] = (core_1_rcy_port > 0 ? core_1_rcy_port : core_0_rcy_port);
    printf("Packets received on core 0 are recycle d on port %d\n", rcy_port[0]);
    printf("Packets received on core 1 are recycle d on port %d\n", rcy_port[1]);
    
    print bcm_port_control_set(unit, rcy_port[0], bcmPortControlOverlayRecycle, 1);
    print bcm_port_control_set(unit, rcy_port[1], bcmPortControlOverlayRecycle, 1);
    
    for (core_i = 0; core_i < 2; core_i++) {
        
        print bcm_port_tpid_delete_all(unit, in_port[core_i]);
        print bcm_port_tpid_add(unit, in_port[core_i], 0x8100, 0);
        print bcm_port_tpid_add(unit, in_port[core_i], 0x88a8, 0);
        print bcm_port_class_set(unit, in_port[core_i], bcmPortClassId, in_port[core_i]);
        
        bcm_l2_station_t_init(&station);
        station.flags = 0;
        station.src_port_mask = 0;
        station.vlan_mask = 0;
        sal_memcpy(station.dst_mac_mask, l2_station_mask, 6);
        sal_memcpy(station.dst_mac, in_rif_mac[core_i], 6); 
        print bcm_l2_station_add(unit, &station_id, &station);
        
        bcm_vlan_port_t_init(&in_vp[core_i]);
        in_vp[core_i].criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        in_vp[core_i].match_vlan  = in_vid[core_i];
        in_vp[core_i].egress_vlan = in_vid[core_i];
        in_vp[core_i].vsi  = in_vsi[core_i];
        in_vp[core_i].port = in_port[core_i];
        print bcm_vlan_port_create(unit, &in_vp[core_i]);
        
        bcm_l3_intf_t_init(&l3_in_rif);
        sal_memcpy(l3_in_rif.l3a_mac_addr, in_rif_mac[core_i], 6);
        l3_in_rif.l3a_vid = in_rif[core_i];          
        l3_in_rif.l3a_vrf = 5+core_i;
        l3_in_rif.l3a_mtu = 1000;
        print bcm_l3_intf_create(unit, &l3_in_rif);
        
        bcm_l3_ingress_t l3_ing;
        bcm_l3_ingress_t_init(&l3_ing);
        l3_ing.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_WITH_ID;
        rv = bcm_l3_ingress_create(unit, &l3_ing, l3_in_rif.l3a_intf_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
        }
    }
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = 0x12345601;
    BCM_GPORT_SYSTEM_PORT_ID_SET(l3host.l3a_port_tgid, rcy_port[0]);
    print bcm_l3_host_add(0, &l3host);
    
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = 0x12345602;
    BCM_GPORT_SYSTEM_PORT_ID_SET(l3host.l3a_port_tgid, rcy_port[1]);
    print bcm_l3_host_add(0, &l3host);
    
    BCM_GPORT_SYSTEM_PORT_ID_SET(src_gport[0], in_port[0]);
    BCM_GPORT_SYSTEM_PORT_ID_SET(src_gport[1], in_port[1]);
    BCM_GPORT_SYSTEM_PORT_ID_SET(dst_gport[0], rcy_port[0]);
    BCM_GPORT_SYSTEM_PORT_ID_SET(dst_gport[1], rcy_port[1]);
    
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    print bcm_field_group_create(unit, qset, 4, &group);
    print bcm_field_group_action_set(unit, group, aset);
    entry_i = 0;
    
    for (src_i = 0; src_i < 2; src_i++) {
        for (dst_i = 0; dst_i < 2; dst_i++) {
            print bcm_field_entry_create(0, group, &entry);
            print bcm_field_qualify_SrcPort(unit, entry, 0, 0, src_gport[src_i], 0xffffffff);
            print bcm_field_qualify_DstPort(unit, entry, 0, 0, dst_gport[dst_i], 0xffffffff);
            statId = (4*1024 - 256 - 1) + entry_i;
            statId |= (in_port[src_i] << 19);
            for (core_i = 0; core_i < 2; core_i++) {
                core_config_arr[core_i].param0 = statId;
                BCM_GPORT_LOCAL_SET(core_config_arr[core_i].param1, rcy_port[dst_i]);
                core_config_arr[core_i].param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            }
            print bcm_field_action_config_add(unit, entry, bcmFieldActionStat, 2, &core_config_arr[0]);
            entry_i++;
        }
    }
    print bcm_field_group_install(unit, group);
    
    /*  Configure BFD endpoint */
    int flags = 0;
    flags |= BCM_BFD_ENDPOINT_IN_HW;
    flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    print bfd_ipv4_run_with_defaults(0, in_port[0], out_port[0], -1, 0, flags, 0, 0);

}




/* Example of using the default MEPs by lif profile */
int bfd_default_mep_example(int unit, int port1, int port2, char advanced_mode) {
    int rv;
    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int tunnel_id, ret;
    int lif;
    int trap_code;

    port_1 = port1;
    port_2 = port2;

    tunnel_id = 99;

    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv); 

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = mpls_init(unit, &tunnel_switch, &tunnel_id, 1, 0, port1, port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    lif = tunnel_switch.tunnel_id;

    /* Set port profile */
    if (advanced_mode) {
        printf("Set LIF profile for port: %d\n", lif);
        rv = bcm_port_class_set(unit, lif, bcmPortClassFieldIngress, 5);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("Set mapping from inlif profile 5 to OAM trap profile 1\n");
    }
    else {
        printf("Set LIF profile for port: %d (LIF: 0x%x)\n", port1, lif);
    }
    rv = bcm_port_control_set(unit, lif, bcmPortControlOamDefaultProfile, 1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /*
    * Adding BFD default endpoint
    */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);

    if (advanced_mode) {
        /* This is not related to the Advanced mode, it only
           Divercifies the test */
        BCM_GPORT_LOCAL_SET(bfd_endpoint_info.remote_gport, port2);

        /* remote_gport field must be a trap */
        rv = bfd_create_trap_from_gport(unit, bfd_endpoint_info.remote_gport, &trap_code);
        if (rv != BCM_E_NONE) {
            printf("Error bfd_create_trap_from_gport.\n");
            return rv;
        }
        BCM_GPORT_TRAP_SET(bfd_endpoint_info.remote_gport, trap_code, 7, 0);
    } else {
        bfd_endpoint_info.remote_gport = BCM_GPORT_INVALID;
    }
    bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_WITH_ID;
    bfd_endpoint_info.id = BCM_BFD_ENDPOINT_DEFAULT1;

    printf("bcm_bfd_endpoint_create default\n");
    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created endpoint with id %d\n", bfd_endpoint_info.id);

    return rv;
}




/* 
* Add GAL label in order to avoid trap unknown_label
* Point to egress-object: egress_intf, returned by create_l3_egress
*/
int
mpls_add_gal_entry(int unit)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;

    /* GAL label */
    entry.label = gal_label;

    /* egress attribures*/   
    entry.egress_if = 0;    

    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}


/* Creates a BFD endpoint of type MPLS-TP (BFD-CC)
 */
int bfd_pwe_gal_example(int unit) {
    bcm_error_t rv;
    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int tunnel_id, ret;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if (device_type<device_type_arad_plus) {
        printf("bcmBFDTunnelTypePweGal is supported only on Arad+\n");
        return rv;
    }

    is_gal = 1; /* Enabling GAL termination */
    rv = pwe_init(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /*
    * Adding BFDoGACHoGALoPWE endpoint
    */

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);

    bfd_endpoint_info.type = bcmBFDTunnelTypePweGal;
    bfd_endpoint_info.gport = network_port_id; /*in lif: mpls_port->mpls_port_id */ 
    bfd_endpoint_info.dst_ip_addr = 0;
    bfd_endpoint_info.src_ip_addr = 0;
    bfd_endpoint_info.ip_tos = 0;
    bfd_endpoint_info.ip_ttl = 0;
    bfd_endpoint_info.udp_src_port = 0;
    bfd_endpoint_info.egress_if = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(network_port_id);
    BCM_L3_ITF_SET(bfd_endpoint_info.egress_if, BCM_L3_ITF_TYPE_LIF, bfd_endpoint_info.egress_if);
    bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_IN_HW;
    bfd_endpoint_info.local_discr =  0x30004;
    bfd_endpoint_info.egress_label.label = gal_label;
    bfd_endpoint_info.egress_label.ttl = 0x40;
    bfd_endpoint_info.egress_label.exp = 0;

    /* set TX fields*/
    bfd_endpoint_info.int_pri = 5;
    bfd_endpoint_info.remote_discr = 0x10002;
    bfd_endpoint_info.local_flags = 0x12;
    bfd_endpoint_info.local_state = 3;
    bfd_endpoint_info.local_min_tx = 0xa;
    bfd_endpoint_info.loc_clear_threshold =  1;
    bfd_endpoint_info.local_min_echo = 0xbadf00d;
    bfd_endpoint_info.local_min_rx = 0xa;
    bfd_endpoint_info.bfd_period = 100;
    bfd_endpoint_info.local_detect_mult = 208;
    bfd_endpoint_info.remote_detect_mult = 30;
    bcm_stk_sysport_gport_get(unit,port_1, &bfd_endpoint_info.tx_gport);
    printf("bcm_bfd_endpoint_create bfd_endpoint_info\n");
    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    bfd_ep1.id = bfd_endpoint_info.id;
    printf("created endpoint with id %d\n", bfd_endpoint_info.id);

    return rv;
}


int bfd_pwe_gal_run_with_defaults(int unit, int port1, int port2) {
    bcm_error_t rv;

    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
      return rv;
    }

    rv =  bfd_pwe_gal_example(unit);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }

    rv = register_events(unit);
    return rv;
}

    /**
    * Creates a trap with snoop command to specified trap and snoop
    * GPorts. Returns the created trap code in *trap_code.
    */
int trap_snoop_create(int unit, bcm_gport_t trap_gport,
                      bcm_gport_t snoop_gport, int* trap_code) {

    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;
    bcm_gport_t gport;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd;

    /* First allocate a new snoop command */
    rv = bcm_rx_snoop_create(unit, 0/*flags*/, &snoop_cmnd);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print snoop_cmnd;

    /* Setup the new snoop */
    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = snoop_gport;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    print snoop_config_cpu;

    rv =  bcm_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Now create a new trap code */
    rv =  bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print *trap_code;

    /* Setup the new trap */
    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = BCM_RX_TRAP_UPDATE_DEST
        | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER | BCM_RX_TRAP_TRAP;
    trap_config_snoop.snoop_strength = 3;
    trap_config_snoop.snoop_cmnd = snoop_cmnd; /*Connect the snoop command to the trap*/
    trap_config_snoop.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
    trap_config_snoop.dest_port = trap_gport;

    rv = bcm_rx_trap_set(unit, *trap_code, &trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    return rv;

}

    /**
    *  Configure snooping to CPU packets that are trapped to the
    *  OAMP.
    *  Use this function to create a new trap code with OAMP
    *  destination that's connected to a snoop to snoop_port
    *  command.
 */
int acc_snoop_init(int unit, int snoop_port) {

    bcm_error_t rv;
    bcm_gport_t oamp_gport;

    rv = oamp_gport_get(unit, &oamp_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print oamp_gport;

    rv = trap_snoop_create(unit, oamp_gport, snoop_port, &trap_code_4_snooping);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    return rv;
}

/*bfd ipv4 single tunnel add and delete operation*/
int bfd_ipv4_tunnel_operation(int unit, int out_port) {
    bcm_error_t rv;
    int out_vlan = 100;
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();/* my-MAC {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  */
    bcm_mac_t next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_tunnel_initiator_t tunnel_1;

    int tunnel_itf1=0;

    /*** create egress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
        return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;

    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    /*** create IP tunnel ***/
    bcm_tunnel_initiator_t_init(&tunnel_1);
    tunnel_1.dip = 0x0;
    tunnel_1.sip = 0xA0000011; /* 160.0.0.17*/
    tunnel_1.dscp = 10;
    tunnel_1.flags = 0x0;
    tunnel_1.ttl = 60;
    tunnel_1.type = bcmTunnelTypeIpAnyIn4;
    tunnel_1.vlan = out_vlan;
    tunnel_1.dscp_sel = bcmTunnelDscpAssign;

    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 1\n");
    }

    rv = remove_ip_tunnel(unit,tunnel_itf1);
    if (rv != BCM_E_NONE) {
        printf("Error, remove_ip_tunnel 1\n");
    }

    return rv;
}

