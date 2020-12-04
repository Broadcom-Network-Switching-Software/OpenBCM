/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_oam_endpoint_over_lag.c
 * Purpose: Example of building an OAM endpoint over lag.
 * 
 * Note that lag and trunk will be used interchangeably through this cint.
 * 
 * Configuration Steps
 * --------------------
 *  
 * This cint demonstrates the construction of an OAM endpoint over lag. The steps are:
 * 1. Create a lag (trunk). 
 * 2. Add (four) ports to this lag.
 * 3. Create an ethernet port (vlan-port) over the lag.
 *  3.5 Optional: Create another vlan_port and cross connect both vlan ports, for testing purposes (explained below).
 *      In trunk only test, the test will end here.
 * 4. Create an OAM group.
 * 5. Create the Oam endpoint (MEP) with the vlan gport.
 * 6. Enable mymac for the other lag ports. (Step 5 only enables the endpoint mymac for the first physical port)
 * 
 * Calling order:
 * --------------
 * 
 * cint <sdk>/src/examples/dpp/cint_multi_device_utils.c
 * cint <sdk>/src/examples/dpp/cint_oam_endpoint_over_lag.c
 * c
 * oam_endpoint_over_lag__init(<lag_port1>,<lag_port2>,<lag_port3>,<not_lag_port>);
 * * Optional: Configure the values define as "IN" in the oam_endpoint_over_lag_1 struct.
 * print oam_endpoint_over_lag_main(<unit>, <test_level>);
 * 
 * 
 * * Note: If not running the test, the not_lag_port can be -1.
 * 
 * Running the test (test level 1):
 * -----------------
 * 
 * The purpose of the test is to show that it is neccessary to configure the mymac manually for each lag port. For that, we'll
 * configure the trunk, two cross connected vlan ports, and an endpoint over one of the vlan ports. We'll then send traffic,
 * and see how the endpoint is only configured on the first physical port in the trunk, and the rest of the ports switch to the
 * cross connected port. Next, we'll configure the mymac, and see how traffic through all ports is trapped and sent to cpu.
 * 
 * Test steps:
 * 
 * 1. Run the calling order above, with test_level=1.
 * 2. Send traffic through the lag ports with the headers below. You'll see that the packet through the first lag port will be trapped
 * and sent to cpu, and that the packets thorugh the other ports will be switched to the cross connected port and vlan translated:
 * Eth header (link layer):
 * DA: 00:00:11:22:33:44
 * SA: Any
 * tpid: 0x8100
 * vlan: 100
 * next_protocol_id: 0x8902
 * 
 * OAM header:
 * mdl: 3
 * opcode: 3
 * lti: 1
 * version: 0
 * oamtype: 3
 * offset: 4
 * 
 * 3. Call oam_endpoint_over_lag_assign_mymac_to_ports.
 * 4. Send the traffic from 2. All of it will be trapped to cpu.
 * 
 * 
 * Running Trunk Only test (test level 2)
 * --------------------------------------
 * 
 * This test is not related to OAM, and it's used to test that switching over trunk is working as expected.
 * In this test, we configure a trunk, cross connect it, and then send packets through all trunk ports.
 * 
 * Test steps:
 * 
 * 1. Run the calling order above, with test_level=2.
 * 2. Send traffic through the lag ports with the headers below. You'll see that the packets will be switched to the cross connected
 * port and vlan translated:
 * Eth header (link layer):
 * DA: 00:00:11:22:33:44
 * SA: Any
 * tpid: 0x8100
 * vlan: 100
 * 
 * The rx packet will be:
 * 
 * DA: 00:00:11:22:33:44
 * SA: Any
 * tpid: 0x8100
 * vlan: 1000
 */

/* Global constants. */
const int MAX_NOF_PORTS_OVER_LAG    = 4;
const int DEFAULT_OAM_LEVEL         = 3;
const int DEFAULT_LM_COUNTER_BASE   = 5;
const int DEFAULT_IN_VLAN           = 100;
const int DEFAULT_EG_VLAN           = 1000;
const uint8 DEFAULT_SHORT_NAME[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
int is_up                           = 0;
int use_lag_vid                     = 0;
int oam_up_statistics_enable        = 0;


/* 
 * Test varaibles. Varibables marked as IN can be changed before running the test to change configuration. Variables marked as OUT
 * will be populated throughout the tests's run.
 */
struct oam_endpoint_over_lag_s {
    bcm_port_t          trunk_ports[MAX_NOF_PORTS_OVER_LAG];        /* (IN) Ports to be configured over the lag. */
    bcm_port_t          eg_port;                                    /* (IN) Port to which the packets will be sent before step 6. */
    int                 nof_ports;                                  /* (IN) Number of ports  on the lag. */
    bcm_mac_t           mymac;                                      /* (IN) Mymac to be used for OAM endpoint. */
    bcm_vlan_t          in_vlan;                                    /* (IN) Ingress vlan match for the vlan port. */
    bcm_vlan_t          eg_vlan;                                    /* (IN) Egress vlan match for the vlan port. */
    int                 oam_level;                                  /* (IN) OAM level for the trunk. */
    int                 lm_counter_base_id;                         /* (IN) LM counter base for the endpoint. */
    bcm_port_t          system_ports[MAX_NOF_PORTS_OVER_LAG];       /* (OUT)System ports matching port-unit */
    bcm_gport_t         trunk_vlan_gport;                           /* (OUT)Ethernet port id over which the endpoint will be created. */
    bcm_gport_t         second_vlan_gport;                          /* (OUT)Ethernet that will be cross connected for testing. */
    bcm_trunk_t         trunk_id;                                   /* (OUT)Created trunk id. */
    bcm_gport_t         trunk_gport;                                /* (OUT)Trunk gport over which the vlan port will be created. */
    bcm_oam_group_t     oam_group_id;                               /* (OUT)Created OAM group id. */
    bcm_oam_endpoint_t  endpoint_id;                                /* (OUT)Created OAM endpoint id. */
    int                 station_ids[MAX_NOF_PORTS_OVER_LAG - 1];    /* (OUT)Station ids containing the ports-mymac assignment. */
};

oam_endpoint_over_lag_s oam_endpoint_over_lag_1;    /* The tests's variables. */

int verbose = 1;    /* Set to 0 to mute test prints. */

/* 
 * oam_endpoint_over_lag__init 
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
int oam_endpoint_over_lag__init(bcm_port_t trunk_port1, bcm_port_t trunk_port2, bcm_port_t trunk_port3, bcm_port_t eg_port){
    bcm_mac_t default_mymac = {0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
    int i;

    /* First clear the struct. */
    sal_memset(&oam_endpoint_over_lag_1, 0, sizeof(oam_endpoint_over_lag_1));
    
    oam_endpoint_over_lag_1.trunk_ports[0]  = trunk_port1;
    oam_endpoint_over_lag_1.trunk_ports[1]  = trunk_port2;
    oam_endpoint_over_lag_1.trunk_ports[2]  = trunk_port3;
    oam_endpoint_over_lag_1.eg_port         = eg_port;    
    oam_endpoint_over_lag_1.nof_ports   = MAX_NOF_PORTS_OVER_LAG - 1;
    oam_endpoint_over_lag_1.in_vlan     = DEFAULT_IN_VLAN;
    oam_endpoint_over_lag_1.eg_vlan     = DEFAULT_EG_VLAN;
    oam_endpoint_over_lag_1.oam_level   = DEFAULT_OAM_LEVEL;
    oam_endpoint_over_lag_1.lm_counter_base_id = DEFAULT_LM_COUNTER_BASE;
    sal_memcpy(oam_endpoint_over_lag_1.mymac, default_mymac, 6);
    
    return BCM_E_NONE;
}


/** TEST FUNCTIONS **/

/*
 * oam_endpoint_over_lag_main
 * 
 * Sets the configurations in the cint header.
 * 
 * Parameters:
 *  int     unit        - Device to be configured.
 *  uint8   test_level  - 0: No test, set full configurations.          1: Set configurations without configuring lag ports' mymac.
 *                        2: Configure lag and cross connect only.
 * 
 *  All fields of oam_endpoint_over_lag_1 marked as "IN".
 * 
 * 
 *  Returns:
 *  BCM_E_NONE: If all the apis pass without a problem.
 *  BCM_E_*:    If something went wrong.
 */
int oam_endpoint_over_lag_main(int unit, uint8 test_level){
    int         rv;
    char*       test_name = (test_level == 2) ? "Trunk test: "                  :
                            (test_level == 1) ? "Oam endpoint over lag test: "  :
                                                "Oam endpoint over lag full configuration: ";

    /* OAM group variables */
    bcm_oam_group_info_t oam_group_info;

    /* 1. Create a lag (trunk). */
    rv = oam_endpoint_over_lag_trunk_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in oam_endpoint_over_lag_trunk_create\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone creating trunk. trunk_id = 0x%x\n", test_name, oam_endpoint_over_lag_1.trunk_id);
    }

    /* 2. Add ports to this lag. */
    rv = oam_endpoint_over_lag_add_members(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in oam_endpoint_over_lag_add_members\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone adding members to lag\n", test_name);
    }

    /* Add trunk common setting */
    if (test_level == 2 && is_device_or_above(unit, JERICHO2)) {
        rv = l2_pp_lag_property_set(unit, oam_endpoint_over_lag_1.trunk_gport);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_pp_lag_property_set\n");
            return rv;
        }
    }

    /* 3. Create an ethernet port (vlan-port) over the lag */
    oam_endpoint_over_lag_vlan_port_create(unit, 1);
    if (rv != BCM_E_NONE){
        printf("Error, in vlan_port_create\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone creating vlan port. trunk_vlan_gport: 0x%x\n", test_name, oam_endpoint_over_lag_1.trunk_vlan_gport);
    }

    /*enable oam statistics per mep session*/
    if (oam_up_statistics_enable) {
        rv = oam_tx_up_stat(unit);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if (test_level) {
        /* 3.5: Create another vlan_port and cross connect both vlan ports. */
        rv = oam_endpoint_over_lag_add_cross_connect(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in oam_endpoint_over_lag_add_cross_connect\n");
            return rv;
        }

        if (verbose) {
            printf("%sDone creating second vlan port and corss connecting. second_vlan_gport: 0x%x\n", test_name, 
                   oam_endpoint_over_lag_1.second_vlan_gport);
        }
    } 

    /* If test level is 2, the test ends here. */
    if (test_level != 2) {

        /* 4. Create an OAM group. */

        bcm_oam_group_info_t_init(&oam_group_info);
        sal_memcpy(oam_group_info.name, DEFAULT_SHORT_NAME, BCM_OAM_GROUP_NAME_LENGTH);
        rv = bcm_oam_group_create(unit, &oam_group_info);
        if (rv != BCM_E_NONE) {
            printf("Error: in bcm_oam_group_create\n");
            return rv;
        }

        oam_endpoint_over_lag_1.oam_group_id = oam_group_info.id;

        if (verbose) {
            printf("%sDone creating OAM group. Group id: 0x%x\n", test_name, oam_endpoint_over_lag_1.oam_group_id);
        }


        /* 5. Create the Oam endpoint (MEP) with the vlan gport. */

        rv = oam_endpoint_over_lag_create_endpoint(unit, 1/*acc-MEP*/);
        if (rv != BCM_E_NONE) {
            printf("Error: in oam_endpoint_over_lag_create_endpoint\n");
            return rv;
        }

        if (verbose) {
            printf("%sDone creating endpoint.\n", test_name);
        }

        /* 6. Enable mymac for the other lag ports. Skipped is test_level is set */

        if (!test_level) {
            rv = oam_endpoint_over_lag_assign_mymac_to_ports(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, in oam_endpoint_over_lag_assign_mymac_to_ports\n");
                return rv;
            }
            if (verbose) {
                printf("%sDone assigining mymac to ports\n", test_name);
            }
        }
    }

    if (verbose) {
        printf("%sDone\n",test_name);
    }

    return rv;
}

/*
 * cint_oam_endpoint_over_lag_cleanup
 * 
 * Destorys all configuration created in the main function. 
 * 
 * Parameters:
 *  int     unit        - Device to be configured.
 *  uint8   test_level  - 0: Destroy full configurations.          1: Destroy configuration except for lag ports' mymac.
 *                        2: Destroy lag and cross connect only.
 * 
 *  All fields of oam_endpoint_over_lag_1 marked as "OUT" must be configured according to the created components.
 * 
 * 
 *  Returns:
 *  BCM_E_NONE: If all the apis pass without a problem.
 *  BCM_E_*:    If something went wrong.
 */
int oam_endpoint_over_lag_cleanup(int unit, int test_level){
    int i,
        rv;

    if (test_level != 2) {
        /* 6. Detach lag ports' mymac. */
        if (!test_level) {  /* Only call this if the test ran to the end. */
            for (i = 1 ; i < oam_endpoint_over_lag_1.nof_ports; i++) {
                rv = bcm_l2_station_delete(unit, oam_endpoint_over_lag_1.station_ids[i - 1]);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_l2_station_delete. Port number: %d\n", l2_station.src_port);
                    return rv;
                }
            }
        }

        /* 
         * 5. Destroy OAM endpoint. 
         * 4. Destroy OAM group
         *  (Both are destoyed in the same API.)
         */
        rv = bcm_oam_group_destroy(unit, oam_endpoint_over_lag_1.oam_group_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_oam_group_destroy\n");
            return rv;
        }
    }

    /* 3.5 If the second vlan port exists, clean it and the cross connect up. */
    if (oam_endpoint_over_lag_1.second_vlan_gport) {
        rv = oam_endpoint_over_lag_destroy_cross_connect(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in oam_endpoint_over_lag_destroy_cross_connect\n");
            return rv;
        }
    }

    /* 3. Destroy the ethernet port (vlan-port) over the lag.*/
    rv = bcm_vlan_port_destroy(unit, oam_endpoint_over_lag_1.trunk_vlan_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy\n");
        return rv;
    }

    /* 2. Remove ports from lag. */
    rv = bcm_trunk_member_delete_all(unit, oam_endpoint_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
        printf ("Error, in bcm_trunk_member_delete_all \n");
        return rv;
    }

    /* 1. Destroy lag. */
    rv = bcm_trunk_destroy(unit, oam_endpoint_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
        printf ("Error, in bcm_trunk_destroy\n");
        return rv;
    }
    
    if (verbose) {
        printf("Done cleaning up\n");
    }

    return rv;
}

/*
 * oam_endpoint_over_lag_add_members
 *
 * Add ports to the preconfigured lag.
 *
 * Parameters:
 *
 * int          unit                                    - (IN) Device to be configured.
 * int          oam_endpoint_over_lag_1.nof_ports       - (IN) Used to iterate over the ports.
 * bcm_port_t   oam_endpoint_over_lag_1.trunk_ports[0-x]- (IN) Ports to be configured.
 * bcm_trunk_t  oam_endpoint_over_lag_1.trunk_id        - (IN) The precofigured trunk (lag) id.
 *
 * Returns:
 * BCM_E_NONE:  If all ports were added successfully.
 * BCM_E_*:     If something went wrong.
 */
int
oam_endpoint_over_lag_add_members(int unit){
    int i, rv = BCM_E_NONE;
    bcm_trunk_member_t  member;
    bcm_port_t  port;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t  member_array[20];
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    /* Translate the trunk id to a gport id to be used for the ehternet port. */
    BCM_GPORT_TRUNK_SET(oam_endpoint_over_lag_1.trunk_gport, oam_endpoint_over_lag_1.trunk_id); 

    if (is_device_or_above(unit, JERICHO2)) {
        key.index = 0;
        key.type = bcmSwitchPortHeaderType;
        value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        rv = bcm_switch_control_indexed_port_set(unit, oam_endpoint_over_lag_1.trunk_gport, key, value);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_switch_control_indexed_port_set\n");
            return rv;
        }

        for (i = 0; i < oam_endpoint_over_lag_1.nof_ports; i++) {
            port = oam_endpoint_over_lag_1.trunk_ports[i];
            rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_switch_control_indexed_port_set\n");
                return rv;
            }
        }
    }

    if (is_device_or_above(unit, JERICHO2)){
        trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
        sal_memset(member_array, 0, sizeof(member_array));
        for (i = 0; i < oam_endpoint_over_lag_1.nof_ports; i++) {
            BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, oam_endpoint_over_lag_1.trunk_ports[i]);
            oam_endpoint_over_lag_1.system_ports[i] = member_array[i].gport;
        }

        rv = bcm_trunk_set(unit, oam_endpoint_over_lag_1.trunk_id, &trunk_info, 3, member_array);
        if(rv != BCM_E_NONE) {
            printf("Error: bcm_trunk_set(), rv=%d\n", rv);
            return rv;
        }
    } else {
        bcm_trunk_member_t_init(&member);
        for (i = 0; i < oam_endpoint_over_lag_1.nof_ports; i++) {
            port = oam_endpoint_over_lag_1.trunk_ports[i];

            /* bcm_trunk_member_add only accepts modports or system ports. Translate it. */
            rv = port_to_system_port(unit, port, &member.gport);
            if (rv != BCM_E_NONE) {
                printf("Error, in port_to_system_port\n");
                return rv;
            }

            rv = bcm_trunk_member_add(unit, oam_endpoint_over_lag_1.trunk_id, &member);
            if (rv != BCM_E_NONE) {
              printf ("bcm_trunk_member_add with port %d failed: %d \n", port, rv);
              return rv;
            }
            oam_endpoint_over_lag_1.system_ports[i] = member.gport;
        }
    }

    return rv;
}

/*
 * oam_endpoint_over_lag_create_endpoint
 *
 * Create the OAM endpoint.
 *
 * Parameters:
 *
 * int              unit                                    - (IN) Device to be configured. 
 * bcm_oam_group_t  oam_endpoint_over_lag_1.oam_group_id    - (IN) The preconfigured OAM group over which the endpoint will be created.
 * int              oam_endpoint_over_lag_1.oam_level       - (IN) The OAM level for the endpoint.
 * bcm_gport_t      oam_endpoint_over_lag_1.trunk_vlan_gport- (IN) The precofigured vlan port (over lag) id. 
 * bcm_mac_t        oam_endpoint_over_lag_1.mymac           - (IN) The mymac for the endpoint. 
 * int              oam_endpoint_over_lag_1.lm_counter_base_id - (IN) The lm counter base id. 
 * bcm_oam_endpoint_t oam_endpoint_over_lag_1.endpoint_id   - (OUT)The created OAM endpoint id.
 *  
 * Returns: 
 * BCM_E_NONE:  If the endpoint was created successfully.
 * BCM_E_*:     If something went wrong. 
 */
int oam_endpoint_over_lag_create_endpoint(int unit, int create_acc_mep){
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t mep;
    bcm_oam_endpoint_info_t rmep;
    bcm_oam_endpoint_info_t_init(&mep);
     
    /* When oam_up_statistics_enable is enabled, engines are allocated for PMF*/ 
    if (oam_up_statistics_enable == 0) {
        /* Set the counter*/
        rv = set_counter_source_and_engines(unit, &oam_endpoint_over_lag_1.lm_counter_base_id,oam_endpoint_over_lag_1.trunk_ports[0]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_oam_endpoint_create\n");
            return rv;
        }
    }


    if (!create_acc_mep) {
        /*
         * Non-accelerated endpoint
         */
        mep.type = bcmOAMEndpointTypeEthernet;
        mep.group = oam_endpoint_over_lag_1.oam_group_id;
        mep.level = oam_endpoint_over_lag_1.oam_level;
        mep.gport = oam_endpoint_over_lag_1.trunk_vlan_gport;
        mep.name = 0;
        mep.ccm_period = 0;
        sal_memcpy(mep.dst_mac_address, oam_endpoint_over_lag_1.mymac, 6);
        mep.lm_counter_base_id  = oam_endpoint_over_lag_1.lm_counter_base_id;
        mep.timestamp_format = soc_property_get(unit,"oam_dm_ntp_enable",1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;

        printf("bcm_oam_endpoint_create non_acc mep\n");
        rv = bcm_oam_endpoint_create(unit, &mep);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_oam_endpoint_create\n");
            return rv;
        }
    }
    else {
        rv = oam__device_type_get(unit, &device_type);
        if (rv < 0) {
          printf("Error checking whether the device is arad+.\n");
          print rv;
          return rv;
        }

        /*
        * Adding acc MEP
        */

         /*
        * When OAM/BFD statistics enabled and MEP ACC is required to be counted,
        * mep acc id 0 can't be used
        */
        if (oam_up_statistics_enable) {
            mep.flags |= BCM_OAM_ENDPOINT_WITH_ID;
            mep.id = (device_type == device_type_qux) ? 1024 : 4096;
        }

        /*TX*/
        mep.type = bcmOAMEndpointTypeEthernet;
        mep.group = oam_endpoint_over_lag_1.oam_group_id;
        mep.level = oam_endpoint_over_lag_1.oam_level;
        mep.tx_gport = (is_up) ? BCM_GPORT_INVALID : oam_endpoint_over_lag_1.trunk_gport; /*Up MEP requires gport invalid.*/
        mep.flags |= (is_up) ? BCM_OAM_ENDPOINT_UP_FACING : 0; /*Up MEP requires flag.*/
        mep.name = 456;
        mep.ccm_period = 1000;
        mep.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

        mep.vlan = (use_lag_vid) ? oam_endpoint_over_lag_1.in_vlan : 10;
        mep.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
        mep.outer_tpid = 0x8100;
        mep.inner_vlan = 0;
        mep.inner_pkt_pri = 0;
        mep.inner_tpid = 0;
        mep.int_pri = 1 + (3<<2);


        if (device_type >= device_type_arad_plus) {
            /* Take RDI only from RX*/
            mep.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;
        }

        if (device_type == device_type_arad_a0) {
            src_mac_mep_3[5] = oam_endpoint_over_lag_1.trunk_vlan_gport;
        }

        /* The MAC address that the CCM packets are sent with*/
        sal_memcpy(mep.src_mac_address, src_mac_mep_3, 6);

        /*RX*/
        sal_memcpy(mep.dst_mac_address, oam_endpoint_over_lag_1.mymac, 6);
        mep.lm_counter_base_id  = oam_endpoint_over_lag_1.lm_counter_base_id;
        mep.gport = oam_endpoint_over_lag_1.trunk_vlan_gport;
        mep.timestamp_format = get_oam_timestamp_format(unit);

        printf("bcm_oam_endpoint_create mep\n");
        rv = bcm_oam_endpoint_create(unit, &mep);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        printf("created MEP with id %d\n", mep.id);

        /*
        * Adding Remote MEP
        */
        bcm_oam_endpoint_info_t_init(&rmep);
        rmep.name = 0x11;
        rmep.local_id = mep.id;
        rmep.type = bcmOAMEndpointTypeEthernet;
        rmep.ccm_period = 0;
        rmep.flags |= BCM_OAM_ENDPOINT_REMOTE;
        rmep.loc_clear_threshold = 1;
        rmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        rmep.id = mep.id;
        if (device_type >= device_type_arad_plus) {
            rmep.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        }

        printf("bcm_oam_endpoint_create RMEP\n");
        rv = bcm_oam_endpoint_create(unit, &rmep);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("created RMEP with id %d\n", rmep.id);
    }

    oam_endpoint_over_lag_1.endpoint_id = mep.id;

    return rv;
}

/*
 * oam_endpoint_over_lag_assign_mymac_to_ports
 *  
 * Configure the endpoint's mymac over all the lag ports.
 *  
 * Parameters: 
 *  
 * int              unit                                    - (IN) Device to be configured. 
 * bcm_mac_t        oam_endpoint_over_lag_1.mymac           - (IN) The mymac for the endpoint. 
 * bcm_port_t       oam_endpoint_over_lag_1.trunk_ports     - (IN) The rest of the ports on the trunk. 
 * int              oam_endpoint_over_lag_1.station_ids     - (OUT)The created l2 stations.
 *  
 *  
 * Returns: 
 * BCM_E_NONE:  If the mymac was configured successfully.
 * BCM_E_*:     If something went wrong. 
 */
int oam_endpoint_over_lag_assign_mymac_to_ports(int unit){
    int                 i, rv = BCM_E_NONE;
    bcm_l2_station_t    l2_station;
    bcm_mac_t           mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_port_t          port_mask = -1;
    uint32              flags = BCM_L2_STATION_OAM;

    l2_station.flags                    = flags;
    l2_station.src_port_mask            = port_mask;
    sal_memcpy(l2_station.dst_mac,      oam_endpoint_over_lag_1.mymac, 6);
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, 6);

    /* Assign all ports except for the first. */
    for (i = 1 ; i < oam_endpoint_over_lag_1.nof_ports; i++) {
        l2_station.src_port = oam_endpoint_over_lag_1.trunk_ports[i];
        rv = bcm_l2_station_add(unit, &oam_endpoint_over_lag_1.station_ids[i - 1], &l2_station);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_station_add. Port number: %d\n", l2_station.src_port);
            return rv;
        }
    }

    return rv;
}

/*
 * oam_endpoint_over_lag_vlan_port_create
 *
 * Create a vlan port, either over lag, or for the test.
 *
 * Parameters:
 *
 * int              unit                                    - (IN) Device to be configured.
 * uint8            is_trunk_port                           - (IN) 1 if this vlan port is configured over lag, 0 is not.
 *
 * if is_trunk_port
 *  bcm_gport_t     oam_endpoint_over_lag_1.trunk_gport     - (IN) The lag gport.
 *  bcm_vlan_t      oam_endpoint_over_lag_1.in_vlan         - (IN) Vlan to be used as part of the key for this vlan port.
 *  bcm_gport_t     oam_endpoint_over_lag_1.trunk_vlan_gport- (OUT)Will be filled with the created gport's id.
 * else
 *  bcm_gport_t     oam_endpoint_over_lag_1.eg_port         - (IN) The test port to be cross connected.
 *  bcm_vlan_t      oam_endpoint_over_lag_1.eg_vlan         - (IN) Vlan to be used as part of the key for this vlan port.
 *  bcm_gport_t     oam_endpoint_over_lag_1.second_vlan_gport- (OUT)Will be filled with the created gport's id.
 *
 * Returns:
 * BCM_E_NONE:  If the vlan port was created successfully.
 * BCM_E_*:     If something went wrong.
 */
int oam_endpoint_over_lag_vlan_port_create(int unit, uint8 is_trunk_port){
    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria     = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port         = (is_trunk_port) ? oam_endpoint_over_lag_1.trunk_gport : oam_endpoint_over_lag_1.eg_port;
    vp.match_vlan   = (is_trunk_port) ? oam_endpoint_over_lag_1.in_vlan : oam_endpoint_over_lag_1.eg_vlan;
    if (!is_device_or_above(unit, JERICHO2)){
        vp.flags        = BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE;
    } else {
        vp.flags        = 0;
    }

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    if (is_trunk_port) {
        oam_endpoint_over_lag_1.trunk_vlan_gport = vp.vlan_port_id;
    } else {
        oam_endpoint_over_lag_1.second_vlan_gport = vp.vlan_port_id;
    }
    
    return rv;
}


/*
 * oam_endpoint_over_lag_add_cross_connect
 *  
 * Create a vlan port, and cross connect it with the lag.
 *  
 * Parameters: 
 *  
 * int              unit                                    - (IN) Device to be configured. 
 * bcm_gport_t      oam_endpoint_over_lag_1.eg_port         - (IN) The test port to be cross connected.
 * bcm_vlan_t       oam_endpoint_over_lag_1.eg_vlan         - (IN) Vlan to be used as part of the key for this vlan port. 
 * bcm_gport_t      oam_endpoint_over_lag_1.trunk_vlan_gport- (IN) Preconfigured vlan port over lag to be cross connected.
 * bcm_gport_t      oam_endpoint_over_lag_1.second_vlan_gport- (OUT)Will be filled with the created gport's id and cross connected to the 
 *                                                                  trunk vlan port.
 *  
 * Returns: 
 * BCM_E_NONE:  If the second vlan port was created and cross connected successfully.
 * BCM_E_*:     If something went wrong. 
 */
int oam_endpoint_over_lag_add_cross_connect(int unit){
    int rv;
    bcm_vswitch_cross_connect_t cross_connect;

    rv = oam_endpoint_over_lag_vlan_port_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in oam_endpoint_over_lag_vlan_port_create\n");
        return rv;
    }

    bcm_vswitch_cross_connect_t_init(&cross_connect);

    cross_connect.port1 = oam_endpoint_over_lag_1.trunk_vlan_gport;
    cross_connect.port2 = oam_endpoint_over_lag_1.second_vlan_gport;

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

/*
 * oam_endpoint_over_lag_destroy_cross_connect
 *  
 * Destroy the cross connection and the second vlan port.
 *  
 * Parameters: 
 *  
 * int              unit                                    - (IN) Device to be configured. 
 * bcm_gport_t      oam_endpoint_over_lag_1.trunk_vlan_gport- (IN) First gport of cross connection.
 * bcm_gport_t      oam_endpoint_over_lag_1.second_vlan_gport- (IN)Second gport of cross connection.
 *  
 * Returns: 
 * BCM_E_NONE:  If the second vlan port was created and cross connected successfully.
 * BCM_E_*:     If something went wrong. 
 */
int oam_endpoint_over_lag_destroy_cross_connect(int unit){
    int rv;
    bcm_vswitch_cross_connect_t cross_connect;

    cross_connect.port1 = oam_endpoint_over_lag_1.trunk_vlan_gport;
    cross_connect.port2 = oam_endpoint_over_lag_1.second_vlan_gport;

    /* Destroy the vswitch cross connect. */
    rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_delete\n");
        return rv;
    }

    /* Destroy second vlan gport. */
    rv = bcm_vlan_port_destroy(unit, oam_endpoint_over_lag_1.second_vlan_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy\n");
        return rv;
    }

    return rv;
}

/*
 * oam_endpoint_over_lag_trunk_create
 *
 * Create a trunk, then set it to be round robin.
 *
 * Parameters:
 *
 * int              unit                                - (IN) Device to be configured.
 * bcm_trunk_t      oam_endpoint_over_lag_1.trunk_id    - (OUT)Created lag.
 *
 * Returns:
 * BCM_E_NONE:  If the trunk was created successfully.
 * BCM_E_*:     If something went wrong.
 */
int oam_endpoint_over_lag_trunk_create(int unit){
    int     rv;
    uint32  trunk_flags = 0;

    /* On JR2, need to set trunk id before creating */
    if (is_device_or_above(unit, JERICHO2)){
        BCM_TRUNK_ID_SET(oam_endpoint_over_lag_1.trunk_id, 0, oam_endpoint_over_lag_1.trunk_id);
    }

    rv = bcm_trunk_create(unit, trunk_flags, &oam_endpoint_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rv);
      return rv;
    }

    if (!is_device_or_above(unit, JERICHO2)){
        rv = bcm_trunk_psc_set(unit, oam_endpoint_over_lag_1.trunk_id, BCM_TRUNK_PSC_ROUND_ROBIN);
        if (rv != BCM_E_NONE) {
          printf ("Error, in bcm_trunk_psc_set\n");
          return rv;
        }
    }

    return rv;
}
