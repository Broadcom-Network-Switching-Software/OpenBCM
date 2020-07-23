/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_bfd.c
 * Purpose: Example of setting client/server BFD endpoint over LAG. 
 *
 * Usage:
 * 
 
To run server example on JR1/Qumran and below: 
 
   cd
   cd ../../../../src/examples/dpp
   cint ../sand/utility/cint_sand_utils_l3.c
   cint utility/cint_utils_l3.c
   cint cint_ip_route.c
   cint ../sand/cint_sand_bfd.c 
   cint cint_bfd.c 
   cint ../sand/cint_bfd_over_lag.c 
   cint cint_multi_device_utils.c
   cint 
   int client_unit=0, server_unit=2;
   int port1=13,port2=14,port3=15; 
   print bfd_endpoint_over_lag__init(port1,port2,client_unit,server_unit,15,15);
   print bfd_server_example(client_unit, server_unit);
 
To run server example on JR2/Q2A and above:

   cd
   cd ../../../../src/examples/sand
   cint utility/cint_sand_utils_vlan.c
   cint utility/cint_sand_utils_l3.c
   cint utility/cint_sand_utils_mpls.c
   cint utility/cint_sand_utils_oam.c
   cint cint_ip_route.c
   cint cint_sand_bfd.c
   cint cint_bfd_over_lag.c
   int client_unit=0, server_unit=2;
   int port1=13,port2=14,port3=15; 
   print bfd_endpoint_over_lag__init(port1,port2,client_unit,server_unit,15,15);
   print bfd_server_example(client_unit, server_unit);
 */


const int MAX_NOF_PORTS_OVER_LAG    = 4;

int srv_ep_rem_gport= 0;

bcm_field_presel_t  cint_bfd_ipv6_echo_presel_id = 11;

bcm_field_context_t cint_bfd_ipv6_echo_context_id_ipmf1 = BCM_FIELD_CONTEXT_ID_INVALID;

/*
 * When set to 1, classification in bfd server will be configured in addition to classification
 * in bfd client.
 */
int server_classification = 0;

/*
 * When set to 1, bfd server IPv6 will be configured
 */
int is_ipv6 = 0;

/*
 * When set to 0, bfd server over MPLS will be configured
 */
int is_mpls = 0;

int is_jr1_interop = 0;

/* Seamless BFD feature.
 * This tells whether the test is
 * currently being run on a seamless
 * bfd initiator server/client endpoint */
uint8 is_s_ser_cli_bfd_init_ep = 0;


bcm_bfd_endpoint_t server_endpoint_id;
bcm_bfd_endpoint_t client_endpoint_id;
bcm_bfd_endpoint_t server_client_endpoint_id;

/* 
 * example varaibles. Varibables marked as IN can be changed before running the test to change configuration. Variables marked as OUT
 * will be populated throughout the tests's run.
 */
struct bfd_endpoint_over_lag_s {
    bcm_port_t          trunk_ports[MAX_NOF_PORTS_OVER_LAG];        /* (IN) Ports to be configured over the lag. */
    int          trunk_ports_units[MAX_NOF_PORTS_OVER_LAG];        /* (IN) unit associated with ports in bcm_port_t. */
    bcm_port_t          eg_port;                                    /* (IN) Port to which the packets will be sent before step 6. */
    bcm_port_t          eg_port2;                                   /* (IN) Port to which the packets will be sent in the second unit. */
    int                 nof_ports;                                  /* (IN) Number of ports  on the lag. */
    bcm_port_t          system_ports[MAX_NOF_PORTS_OVER_LAG];       /* (OUT)System ports matching port-unit */
    bcm_gport_t         trunk_vlan_gport;                           /* (OUT)Ethernet port id over which the endpoint will be created. */
    bcm_gport_t         second_vlan_gport;                          /* (OUT)Ethernet that will be cross connected for testing. */
    bcm_trunk_t         trunk_id;                                   /* (OUT)Created trunk id. */
    bcm_gport_t         trunk_gport;                                /* (OUT)Trunk gport over which the vlan port will be created. */
    int                 station_ids[MAX_NOF_PORTS_OVER_LAG - 1];    /* (OUT)Station ids containing the ports-mymac assignment. */
};

bfd_endpoint_over_lag_s            bfd_endpoint_over_lag_1;


/* 
 * Arrays for: 
 *      fe_to_fap0_links_array                  (IN) - fe_to_fap0_links_array # The links from the FE to FAP0
 *                                                      which after the cint execution will be in retimer mode.
 *      fe_to_fap1_links_array                  (IN) - fe_to_fap1_links_array # The links from the FE to FAP1
 *                                                      which after the cint execution will be in retimer mode.
 */
int fe_to_fap0_links_array[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
int fe_to_fap0_min_links_array[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71};
int fe_to_fap1_links_array[] = {96,97,98,99,100,101,102,103,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,144,145,146,147,148,149,150,151,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143};
int fe_to_fap1_min_links_array[] = {96,97,98,99,100,101,102,103,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,144,145,146,147,148,149,150,151,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};
bcm_port_t fe_fap_links_1[] = {256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367};
bcm_port_t fe_fap_min_links_1[] = {1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071};
bcm_port_t fap_id_array_1[] = {2,3};
bcm_port_t fap_id_min_array_1[] = {3,4,5};
bcm_port_t fe_fap_links_2[] = {264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367};
bcm_port_t fe_fap_min_links_2[] = {1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071};
bcm_port_t fap_id_array_2[] = {0,1};
bcm_port_t fap_id_min_array_2[] = {0,1,2};

int *fe_to_fap0_links_ptr;
int *fe_to_fap1_links_ptr;
bcm_port_t *fe_fap_links_1_ptr;
bcm_port_t *fe_fap_links_2_ptr;
bcm_port_t *fap_id_ptr_1;
bcm_port_t *fap_id_ptr_2;
int nof_links;
int fap_id_count;


/* 
 * example varaibles. Varibables marked as IN can be changed before running the test to change configuration. Variables marked as OUT
 * will be populated throughout the tests's run.
 */
struct dnx_bfd_endpoint_over_lag_s {
    bcm_port_t          trunk_ports[MAX_NOF_PORTS_OVER_LAG];        /* (IN) Ports to be configured over the lag. */
    int          trunk_ports_units[MAX_NOF_PORTS_OVER_LAG];        /* (IN) unit associated with ports in bcm_port_t. */
    bcm_port_t          eg_port;                                    /* (IN) Port to which the packets will be sent before step 6. */
    int                 nof_ports;                                  /* (IN) Number of ports  on the lag. */
    bcm_port_t          system_ports[MAX_NOF_PORTS_OVER_LAG];       /* (OUT)System ports matching port-unit */
    bcm_gport_t         trunk_vlan_gport;                           /* (OUT)Ethernet port id over which the endpoint will be created. */
    bcm_gport_t         second_vlan_gport;                          /* (OUT)Ethernet that will be cross connected for testing. */
    bcm_trunk_t         trunk_id;                                   /* (OUT)Created trunk id. */
    bcm_gport_t         trunk_gport;                                /* (OUT)Trunk gport over which the vlan port will be created. */
    int                 station_ids[MAX_NOF_PORTS_OVER_LAG - 1];    /* (OUT)Station ids containing the ports-mymac assignment. */
};

/* 
 * cint_bfd_mesh_config 
 *  
 * Configures mesh for bfd server and client units
 *  
 * Parameteres: 
 *  int  fe_unit       - (IN) fe unit
 *  int  server_unit   - (IN) unit of the bfd server
 *  int  client_unit   - (IN) unit of the bfd client
 *  
 * Retruns: 
 *  BCM_E_NONE : In any case. 
 */
int
cint_bfd_mesh_config(int fe_unit,int server_unit,int client_unit){
   int rv = BCM_E_NONE;
   int server_nof_links, client_nof_links;

   cint_vlan_not_member_trap_destroy(server_unit);
   cint_vlan_not_member_trap_destroy(client_unit);

   bcm_mgmt_isolation_test__isolate(server_unit,1);
   bcm_mgmt_isolation_test__isolate(client_unit,1);

   server_nof_links = *(dnxc_data_get(server_unit, "fabric", "links", "nof_links", NULL));
   client_nof_links = *(dnxc_data_get(client_unit, "fabric", "links", "nof_links", NULL));

   if((server_nof_links < 96) || (client_nof_links < 96))
   {
       nof_links = 48;
       fap_id_count = 3;
       fe_to_fap0_links_ptr = fe_to_fap0_min_links_array;
       fe_to_fap1_links_ptr = fe_to_fap1_min_links_array;
       fe_fap_links_1_ptr = fe_fap_min_links_1;
       fe_fap_links_2_ptr = fe_fap_min_links_2;
       fap_id_ptr_1 = fap_id_min_array_1;
       fap_id_ptr_2 = fap_id_min_array_2;
   }
   else
   {
       nof_links = 96;
       fap_id_count = 2;
       fe_to_fap0_links_ptr = fe_to_fap0_links_array;
       fe_to_fap1_links_ptr = fe_to_fap1_links_array;
       fe_fap_links_1_ptr = fe_fap_links_1;
       fe_fap_links_2_ptr = fe_fap_links_2;
       fap_id_ptr_1 = fap_id_array_1;
       fap_id_ptr_2 = fap_id_array_2;
   }

   cint_dnxf_fe2_to_loopback_retimer_set($fe_unit,server_unit,client_unit,nof_links,fe_to_fap0_links_ptr,fe_to_fap1_links_ptr);

   fabric_mesh_config_example (server_unit,0,fap_id_count,fap_id_ptr_1,nof_links,fe_fap_links_1_ptr);
   fabric_mesh_config_example (client_unit,0,fap_id_count,fap_id_ptr_2,nof_links,fe_fap_links_2_ptr);

   bcm_mgmt_isolation_test__isolate(server_unit,0);
   bcm_mgmt_isolation_test__isolate(client_unit,0);
   return rv;
}

/* 
 * bfd_endpoint_over_lag__init 
 *  
 * Inits the test variables to default values. 
 *  
 * Parameteres: 
 *  bcm_port_t  trunk_port_1-3  - (IN) 3 ports to be configured as one LAG (trunk).
 *  bcm_port_t  eg_port         - (IN) A port to be cross connected to the lag for testing. If you wish to skip the test and just set
 *                                      the configuration, set to -1.
 *  
 * Retruns: 
 *  BCM_E_NONE : In any case. 
 */
int bfd_endpoint_over_lag__init(bcm_port_t trunk_port1, bcm_port_t trunk_port2, int trunk_port1_unit,int trunk_port_2_unit , bcm_port_t eg_port, bcm_port_t eg_port2) {
    int i;

    /* First clear the struct. */
    sal_memset(&bfd_endpoint_over_lag_1, 0, sizeof(bfd_endpoint_over_lag_1));
    
    bfd_endpoint_over_lag_1.trunk_ports[trunk_port1_unit]  = trunk_port1;
    bfd_endpoint_over_lag_1.trunk_ports[trunk_port_2_unit]  = trunk_port2;
    bfd_endpoint_over_lag_1.trunk_ports_units[trunk_port1_unit]  = trunk_port1_unit;
    bfd_endpoint_over_lag_1.trunk_ports_units[trunk_port_2_unit]  = trunk_port_2_unit;
    bfd_endpoint_over_lag_1.eg_port         = eg_port;    
    bfd_endpoint_over_lag_1.eg_port2        = eg_port2;
    bfd_endpoint_over_lag_1.nof_ports   = 2;

    return BCM_E_NONE;
}



/*
 * bfd_over_lag_trunk_create
 *  
 * Create a trunk, then set it to be round robin. 
 *  
 * Parameters: 
 *  
 * int              unit                                - (IN) Device to be configured. 
 * bcm_trunk_t      bfd_endpoint_over_lag_1.trunk_id    - (OUT)Created lag. 
 *  
 * Returns: 
 * BCM_E_NONE:  If the trunk was created successfully.
 * BCM_E_*:     If something went wrong. 
 */
int bfd_over_lag_trunk_create(int unit){

    int i = unit; 
    int rv;
    bcm_trunk_member_t  member;
    bcm_trunk_info_t trunk_info;                
    bcm_port_t  port;                           
    int flags = 0;

    bcm_trunk_member_t_init(member);
    bcm_trunk_info_t_init(trunk_info);

    rv = bcm_trunk_create(unit, flags, &bfd_endpoint_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rv);
      return rv;
    }

    /* set Port Selection Criteria for the trunk*/
    rv = bcm_trunk_psc_set(unit, bfd_endpoint_over_lag_1.trunk_id, BCM_TRUNK_PSC_ROUND_ROBIN);
    if (rv != BCM_E_NONE) {
      printf ("Error, in bcm_trunk_psc_set\n");
      return rv;
    }

    port = bfd_endpoint_over_lag_1.trunk_ports[i];
    trunk_info.psc= BCM_TRUNK_PSC_ROUND_ROBIN; 


    /* bcm_trunk_member_add only accepts modports or system ports. Translate it. */
    rv = port_to_system_port(bfd_endpoint_over_lag_1.trunk_ports_units[i], port, &member.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_to_system_port\n");
        return rv;
    }

    /* Finish trunk setting*/
    /* If the trunk already has member ports, they will be replaced by the new list of ports contained in the member array*/
    bcm_trunk_set(unit, bfd_endpoint_over_lag_1.trunk_id, &trunk_info, 1, member);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_trunk_set failed $rv\n");
        return rv;
    }

    /* Save the gport to global structure*/
    bfd_endpoint_over_lag_1.system_ports[i] = member.gport;

    return rv;
}

/**
 * 
 * 
 * @author sinai (26/10/2014) 
 *  
 * @param server_unit 
 * @param client_unit 
 * @param port1 
 * 
 * @return int 
 */
int bfd_server_example(int server_unit, int client_unit) {
    int rv;
    bcm_bfd_endpoint_info_t client_ep, server_ep;
    int server_oamp_port= 232; /* proper apllication must be used so that this will actually be configured as the server OAMP port*/
    bcm_gport_t remote_gport;
    int is_dnx = 0;
    int oam_offset_supported;
    bcm_field_presel_t  presel_id;
    bcm_field_layer_type_t fwd_layer;
    bcm_ip6_t *src_ipv6;
    void *context_name;
    char dest_char[8];
    bcm_field_context_t  cint_bfd_context_id_ipmf1;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int server_tunnel_id, client_tunnel_id;
    bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};

    oam_offset_supported = *(dnxc_data_get(client_unit, "oam", "feature", "oam_offset_supported", NULL));
    /** Step 0: Create LAG.  */
    if((is_device_or_above(server_unit, JERICHO2))&&((is_device_or_above(client_unit, JERICHO2))))
    {
       is_dnx=1;
       rv = dnx_bfd_over_lag_trunk_create(server_unit,client_unit);
       if (rv != BCM_E_NONE) {
           printf("Error, in bfd_over_lag_trunk_create\n");
           return rv;
       }
    } 
    else if ((is_device_or_above(server_unit, JERICHO2))||((is_device_or_above(client_unit, JERICHO2))))
    {
       printf("Not supported.\n");
       return BCM_E_UNIT;
    }
    else
    {
       /* 
        *  Both units below jericho2
        */
       /*  0.1 Create the LAG over the server unit.*/
       rv = bfd_over_lag_trunk_create(server_unit);    
       if (rv != BCM_E_NONE) {
           printf("Error, in bfd_over_lag_trunk_create\n");
           return rv;
       }

       /*  0.1 Create the LAG over the client unit.*/
       rv = bfd_over_lag_trunk_create(client_unit);    
       if (rv != BCM_E_NONE) {
           printf("Error, in bfd_over_lag_trunk_create\n");
           return rv;
       }
    }
    if (is_dnx) 
    {
        if(is_mpls)
        {
            rv = mpls_init(server_unit, &tunnel_switch, &server_tunnel_id,0, 0, port_1, port_2);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_init\n");
                return rv;
            }
            rv = mpls_init(client_unit, &tunnel_switch, &client_tunnel_id,0, 0, port_1, port_2);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_init\n");
                return rv;
            }
        }
        else
        {
            if(is_ipv6)
            {
                rv = basic_example_ipv6(client_unit,bfd_endpoint_over_lag_1.eg_port, bfd_endpoint_over_lag_1.trunk_gport,0,0,0);

                if (rv != BCM_E_NONE) {
                    printf("Error, in basic_example_ipv6\n");
                    return rv;
                }
                rv = basic_example_ipv6(server_unit,bfd_endpoint_over_lag_1.eg_port2, bfd_endpoint_over_lag_1.trunk_gport,0,0,0);

                if (rv != BCM_E_NONE) {
                    printf("Error, in basic_example_ipv6\n");
                    return rv;
                }
                BCM_L3_ITF_SET(next_hop_mac,BCM_L3_ITF_TYPE_LIF,0x1384);
            }
            else
            {
                /* Step 1: init BFD and set up IP routing on LAG
                 *  port configured above
                 */
                rv = l3_interface_init(server_unit, bfd_endpoint_over_lag_1.trunk_gport, bfd_endpoint_over_lag_1.eg_port2, &next_hop_mac, &tunnel_id, 0, 0, 0);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
                rv = l3_interface_init(client_unit, bfd_endpoint_over_lag_1.trunk_gport, bfd_endpoint_over_lag_1.eg_port, &next_hop_mac, &tunnel_id, 0, 0, 0);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }
    else
    {
       /** Step 1: init BFD and set up IP routing on LAG port
        *  configured above */
       rv = l3_interface_init(client_unit, bfd_endpoint_over_lag_1.system_ports[0], bfd_endpoint_over_lag_1.eg_port, &next_hop_mac, &tunnel_id, 0, 0, 0);
       if (rv != BCM_E_NONE) {
           printf("(%s) \n", bcm_errmsg(rv));
           return rv;
       }

       /* EEDB must be configured on both devices (a LAG member exists on both devices).*/
       rv = l3_interface_init(server_unit, bfd_endpoint_over_lag_1.system_ports[1], bfd_endpoint_over_lag_1.eg_port, &next_hop_mac, &tunnel_id, 0, 0, 0);
       if (rv != BCM_E_NONE) {
           printf("(%s) \n", bcm_errmsg(rv));
           return rv;
       }
    }

    /* Step 2: set a trap with the destination set to the server
     *  side OAMP.
     */
    int trap_code;
    bcm_rx_trap_config_t trap_remote_oamp;
    rv =  bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    bcm_rx_trap_config_t_init(&trap_remote_oamp);
    trap_remote_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    if ((!is_dnx) || (oam_offset_supported))
    {
        trap_remote_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    }
    /* Update the forwarding offset to be ETH1+Ipv4+UDP */
    if (bfd_echo == 1) {
        trap_remote_oamp.forwarding_header = bcmRxTrapForwardingHeaderThirdHeader;
    } else {
       if ((!is_dnx) || (oam_offset_supported))
       {
         trap_remote_oamp.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
       }
    }
    /* Set the destination*/
    if (is_dnx)
    {
      int my_modid;
      int remote_oamp_sysport;
      bcm_gport_t oamp_gport[2];
      int count;

       if(is_dnx)
       {
           int modid_count, actual_modid_count;
           rv = bcm_stk_modid_count(server_unit, &modid_count);
           if (rv != BCM_E_NONE) {
              printf("(%s) \n", bcm_errmsg(rv));
              return rv;
           }
           bcm_stk_modid_config_t modid_array[modid_count]; 

           rv = bcm_stk_modid_config_get_all(server_unit, modid_count, modid_array, &actual_modid_count);
           if (rv != BCM_E_NONE) {
              printf("(%s) \n", bcm_errmsg(rv));
              return rv;
           }

           my_modid = modid_array[0].modid;
       }
       else
       {
           rv = bcm_stk_my_modid_get(server_unit, &my_modid);
           if (rv != BCM_E_NONE) {
              printf("(%s) \n", bcm_errmsg(rv));
              return rv;
           }
       }
       appl_dnx_logical_port_to_sysport_get(server_unit,my_modid,server_oamp_port,&remote_oamp_sysport);
       BCM_GPORT_SYSTEM_PORT_ID_SET(&trap_remote_oamp.dest_port, remote_oamp_sysport);

       trap_remote_oamp.flags|= (BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER);
       if(is_mpls)
       {
           trap_remote_oamp.egress_forwarding_index = 5; /* bfd index */
           trap_remote_oamp.forwarding_header = 5; /* bfd index */
       }
       else
       {
           trap_remote_oamp.egress_forwarding_index = 3; /* bfd index */
           trap_remote_oamp.forwarding_header = 3; /* bfd index */
       }
    }
    else
    {
       rv =  port_to_system_port(server_unit,server_oamp_port, &trap_remote_oamp.dest_port);
       if (rv != BCM_E_NONE) {
           printf("(%s) \n", bcm_errmsg(rv));
           return rv;
       }
    }

    rv = bcm_rx_trap_set(client_unit, trap_code, trap_remote_oamp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code=%d \n", trap_code);

    /* For BFD Echo - Need to configure PMF to trap packet in client and forward to server-oamp */
    if(is_device_or_above(client_unit, JERICHO2) && (bfd_echo == 1))
    {
       bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};
       context_name = &dest_char[0];
       if (is_ipv6) {
          presel_id = cint_bfd_ipv6_echo_presel_id;
          fwd_layer = bcmFieldLayerTypeIp6;
          sal_strncpy(context_name, "ipv6 hit", sizeof(dest_char));
          src_ipv6 = &src_ipv6_tmp;
       }
       else
       {
          presel_id = cint_bfd_echo_presel_id;
          fwd_layer = bcmFieldLayerTypeIp4;
          sal_strncpy(context_name, "ipv4 hit", sizeof(dest_char));
          src_ipv6 = NULL;
       }
       rv = field_presel_fwd_layer_main(client_unit,presel_id, bcmFieldStageIngressPMF1, fwd_layer, &cint_bfd_context_id_ipmf1, context_name);

       if (rv != BCM_E_NONE)
       {
           printf("Error (%d), in cint_field_always_hit_context_main Ingress\n", rv);
           return rv;
       }
       rv = cint_field_bfd_echo_main(client_unit, cint_bfd_context_id_ipmf1, is_ipv6);
       if (rv != BCM_E_NONE) {
         printf("Error cint_field_bfd_echo_main.\n");
         return rv;
       }

       /* Use trap code with server-oamp as dest_port */
       cint_field_echo_trap_id = trap_code;
       rv = cint_field_bfd_echo_entry_add(client_unit, 0x7fffff03, src_ipv6, bfd_local_discr & 0xFFFF);
       if (rv != BCM_E_NONE) {
         printf("Error cint_field_bfd_echo_entry_add.\n");
         return rv;
       }
    }

    /** Step 3: set up the server side endpoint */

    bcm_bfd_endpoint_info_t_init(&server_ep);
    if(is_mpls) {
        server_ep.type = bcmBFDTunnelTypeMpls;
        server_ep.egress_if = server_tunnel_id;
        if(!is_ipv6) {
            server_ep.dst_ip_addr = 0;
            server_ep.src_ip_addr = 0x30F0701;
        }
        server_ep.label = mpls_label;
        server_ep.egress_label.ttl = 0xa;
        server_ep.egress_label.exp = 1;
        server_ep.ip_ttl = 1;
    }
    else
    {
        server_ep.type = bcmBFDTunnelTypeUdp;
        server_ep.flags = BCM_BFD_ENDPOINT_MULTIHOP;
        server_ep.egress_if = next_hop_mac;
        server_ep.ip_ttl = 255;
        if(!is_ipv6) {
            if (bfd_echo == 1) {
                server_ep.flags |= BCM_BFD_ECHO;
                server_ep.dst_ip_addr = 0x030F0701;
                server_ep.src_ip_addr = 0x7fffff03;
            } else {
                server_ep.src_ip_addr = 0x030F0701;
                server_ep.dst_ip_addr = 0x7fffff03;
            }
        }
    }
    if(is_ipv6) {
        server_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
        server_ep.ipv6_extra_data_index = 8192;

        bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};
        bcm_ip6_t dst_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01};
        if (bfd_echo == 1) {
            server_ep.flags |= BCM_BFD_ECHO;
            sal_memcpy(server_ep.src_ip6_addr, src_ipv6_tmp, sizeof(dst_ipv6_tmp));
            sal_memcpy(server_ep.dst_ip6_addr, dst_ipv6_tmp, sizeof(src_ipv6_tmp));
        } else {
            sal_memcpy(server_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
            sal_memcpy(server_ep.dst_ip6_addr, dst_ipv6_tmp, sizeof(dst_ipv6_tmp));
        }
    }
    server_ep.flags |= BCM_BFD_ENDPOINT_IN_HW;
    server_ep.ip_tos = 255;
    server_ep.udp_src_port = 0xC001;
    server_ep.int_pri = 1;
    server_ep.bfd_period = 100;
    server_ep.local_min_tx = 2;
    server_ep.local_min_rx = 3;
    if (!is_dnx)
    {
       server_ep.local_state = 3;
       server_ep.remote_discr = 0x10002;
    }
    else
    {
       if (bfd_echo == 0)
       {
           server_ep.local_state = 3;
           server_ep.remote_discr = 0x10002;
       }
    }
    server_ep.local_flags = 2;
    server_ep.local_detect_mult = 208;
    server_ep.remote_detect_mult = 30;
    server_ep.flags |= BCM_BFD_ENDPOINT_HW_ACCELERATION_SET;
    server_ep.local_discr =  0x30004;

    BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
    server_ep.remote_gport = remote_gport;

    srv_ep_rem_gport = remote_gport;
    printf("srv_ep_rem_gport = 0x%x\r\n",srv_ep_rem_gport);

    if (is_dnx) 
    {
       BCM_GPORT_TRUNK_SET(server_ep.tx_gport, bfd_endpoint_over_lag_1.trunk_gport); /* port that the traffic will be transmitted on: LAG port */
    }
    else
    {
       BCM_GPORT_TRUNK_SET(server_ep.tx_gport, bfd_endpoint_over_lag_1.trunk_id); /* port that the traffic will be transmitted on: LAG port */
    }

    if(is_s_ser_cli_bfd_init_ep) {
        server_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
    /* Some number outside 49152 - 65535 range since
     * SBFD does not have limitations on UDP src port range */
        server_ep.udp_src_port = 20020;
        /* local_min_rx and local_min_echo needs to be zero */
        server_ep.local_min_rx = 0;
        server_ep.local_min_echo = 0;
        /* TTL should be set to 255 - if no MPLS tunnel */
        if (!is_mpls) {
            server_ep.ip_ttl = 255;
        }
        /* local_state set to UP */
        server_ep.local_state = 3;
        /* Local flag should have demand bit (bit 1) set
         * P | F | C | A | D | M
         * 0 | 0 | 0 | 0 | 1 | 0
         */
        server_ep.local_flags = 0x2;
    }

    rv = bcm_bfd_endpoint_create(server_unit, &server_ep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    server_endpoint_id = server_ep.id;

    rv = register_events(server_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (bfd_echo != 1) {
        /* Client side config is not required for BFD echo, since PMF does the rx classification */
        /** Step 4: Set up the client side  */
        bcm_bfd_endpoint_info_t_init(&client_ep);
        client_ep.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP;
        if (!is_dnx) {
           client_ep.flags |= BCM_BFD_ENDPOINT_WITH_ID;
        }
        else {
           if(is_ipv6) {
              client_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
              client_ep.ipv6_extra_data_index = 0x10000;
           }
        }

        client_ep.id = server_ep.id;
        client_ep.remote_gport = remote_gport; 
        client_ep.type = bcmBFDTunnelTypeUdp;
        client_ep.local_discr =  0x30004;
        if(!is_ipv6) {
           client_ep.src_ip_addr = 0x30F0701;
        }
        else{
           sal_memcpy(client_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
        }
        if(is_s_ser_cli_bfd_init_ep) {
            client_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
            client_ep.udp_src_port = 20020;
        }
        rv = bcm_bfd_endpoint_create(client_unit, &client_ep);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        client_endpoint_id = client_ep.id;

        /*
         * If classifier in server side was required, 
         * add an endpoint in server unit. 
         * Packets received by the server unit will also be trapped to OAMP.
         */
        if(server_classification==1) {
           if(!is_ipv6) {
               rv = bcm_rx_trap_type_get(server_unit,0/* flags */ , bcmRxTrapOamBfdIpv4, &trap_code);
               if (rv != BCM_E_NONE) {
                   printf("(%s) \n",bcm_errmsg(rv));
               }

              bcm_rx_trap_config_t trap_config;
              rv = bcm_rx_trap_get(server_unit,trap_code, &trap_config);
              if (rv != BCM_E_NONE) {
                   printf("(%s) \n",bcm_errmsg(rv));
              }
              if (is_jr1_interop) {
                   trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                   trap_config.forwarding_header = 3;
              }
              else
              {
                    trap_config.flags|= (BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER);
                    trap_config.egress_forwarding_index = 3; /* bfd index */
                    trap_config.forwarding_header = 3; /* bfd index */
              }
              rv = bcm_rx_trap_set(server_unit, trap_code, trap_config);
              if (rv != BCM_E_NONE) {
                  printf("(%s) \n", bcm_errmsg(rv));
                  return rv;
              }
           } else {
               rv =  bcm_rx_trap_type_create(server_unit, 0, bcmRxTrapUserDefine, &trap_code);
               if (rv != BCM_E_NONE) {
                   printf("(%s) \n", bcm_errmsg(rv));
                   return rv;
               }
               rv = bcm_rx_trap_set(server_unit, trap_code, trap_remote_oamp);
               if (rv != BCM_E_NONE) {
                   printf("(%s) \n", bcm_errmsg(rv));
                   return rv;
               }
           }
           
           BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
           client_ep.remote_gport = remote_gport;

           bcm_bfd_endpoint_info_t_init(&client_ep);
           client_ep.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_MULTIHOP;
           client_ep.id = server_ep.id;
           client_ep.remote_gport = remote_gport; 
           client_ep.type = bcmBFDTunnelTypeUdp;

           /** We create a MEP with the same local_disc, so it's client entry of the same MEP */
           client_ep.local_discr =  0x30004; 
           if(!is_ipv6) {
              client_ep.src_ip_addr = 0x30F0701;
           }
           else{
              client_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
              sal_memcpy(client_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
           }

           if(is_s_ser_cli_bfd_init_ep) {
               client_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
               client_ep.udp_src_port = 20020;
           }

           rv = bcm_bfd_endpoint_create(server_unit, &client_ep);
           if (rv != BCM_E_NONE) {
               printf("(%s) \n",bcm_errmsg(rv));
               return rv;
           }
           server_client_endpoint_id = client_ep.id;
        }
    }

    return rv;
}

/*
 * bfd_over_lag_trunk_create
 *  
 * Create a trunk, then set it to be round robin. 
 *  
 * Parameters: 
 *  
 * int              unit                                - (IN) Device to be configured. 
 * bcm_trunk_t      bfd_endpoint_over_lag_1.trunk_id    - (OUT)Created lag. 
 *  
 * Returns: 
 * BCM_E_NONE:  If the trunk was created successfully.
 * BCM_E_*:     If something went wrong. 
 */
int dnx_bfd_over_lag_trunk_create(int unit1,int unit2){

    int rv;
    bcm_trunk_member_t  member;
    bcm_trunk_info_t trunk_info;                
    bcm_port_t  port;                           
    int flags = 0;
    bcm_trunk_t trunk_id;
    bcm_gport_t trunk_gport;
    bcm_gport_t gport;
    bcm_gport_t temp_gport;
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;

    bcm_trunk_member_t_init(member);
    /*
     * Configure Trunk
     */
    bcm_trunk_info_t_init(trunk_info);

    BCM_TRUNK_ID_SET(trunk_id, 0, bfd_endpoint_over_lag_1.trunk_id);

    rv = bcm_trunk_create(unit1, flags, &trunk_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_trunk_create\n");
        return rv;
    }
    printf("Trunk id created is = 0x%x\n", trunk_id);

    rv = bcm_trunk_create(unit2, flags, &trunk_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_trunk_create\n");
        return rv;
    }
    printf("Trunk id created is = 0x%x\n", trunk_id);

    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    printf("Trunk gport is: 0x%x\n",trunk_gport);

    bfd_endpoint_over_lag_1.trunk_gport = trunk_gport;

    bcm_trunk_member_t_init(member);
    port = bfd_endpoint_over_lag_1.trunk_ports[unit1];
    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW; 
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, bfd_endpoint_over_lag_1.trunk_ports[unit1]);
    member.gport = gport;

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;
    /** Set trunk header type to match members header types */
    rv = bcm_switch_control_indexed_port_set(unit1, trunk_gport, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
        return rv;
    }

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    /** Set trunk header type to match members header types */
    rv = bcm_switch_control_indexed_port_set(unit1, trunk_gport, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
        return rv;
    }

    /* Finish trunk setting*/
    /* If the trunk already has member ports, they will be replaced by the new list of ports contained in the member array*/
    bcm_trunk_set(unit1, trunk_id, &trunk_info, 1, member);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_trunk_set failed $rv\n");
        return rv;
    }

    /* Save the gport to global structure*/
    bfd_endpoint_over_lag_1.system_ports[unit1] = member.gport;

    bcm_trunk_member_t_init(member);
    port = bfd_endpoint_over_lag_1.trunk_ports[unit2];
    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW; 
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, bfd_endpoint_over_lag_1.trunk_ports[unit2]);
    member.gport = gport;

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    /** Set trunk header type to match members header types */
    rv = bcm_switch_control_indexed_port_set(unit2, trunk_gport, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
        return rv;
    }

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    /** Set trunk header type to match members header types */
    rv = bcm_switch_control_indexed_port_set(unit2, trunk_gport, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
        return rv;
    }

    /* Finish trunk setting*/
    /* If the trunk already has member ports, they will be replaced by the new list of ports contained in the member array*/
    bcm_trunk_set(unit2, trunk_id, &trunk_info, 1, member);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_trunk_set failed $rv\n");
        return rv;
    }

    /* Save the gport to global structure*/
    bfd_endpoint_over_lag_1.system_ports[unit2] = member.gport;

    return rv;
}


