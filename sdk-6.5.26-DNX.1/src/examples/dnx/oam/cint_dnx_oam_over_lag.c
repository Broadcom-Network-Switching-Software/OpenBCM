/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_oam_over_lag.c
 * Purpose: Example of building an OAM endpoint over lag.
 *
 * Note that lag and trunk will be used interchangeably through this cint.
 *
 * Configuration Steps
 * --------------------
 *
 * This cint demonstrates the construction of an OAM endpoint over lag. The steps are:
 * 1. Create a lag (trunk).
 * 2. Add (three) ports to this lag.
 * 3. Create an ethernet port (vlan-port) over the lag.
 *  3.5 Optional: Create another vlan_port and cross connect both vlan ports, for testing purposes (explained below).
 *      In trunk only test, the test will end here.
 * 4. Create an OAM group.
 * 5. Create the Oam endpoint (MEP) with the vlan gport.
 *
 * Test Scenario - start 
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_oam.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/sand/cint_vswitch_metro_mp.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/sand/cint_l2_traverse.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 * cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../../../src/examples/sand/cint_l2_bridge_trunk.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam_over_lag.c
 * cint
 * dnx_oam_over_lag__init(1,14,17,203); 
 * dnx_oam_over_lag_main(0); 
 * exit;
 *
 * // 1. Verify CCM packet injected from downmep
 * // Received CCM should be: 
 * // Data: 0x0180c20000330001020304018100200a8902600103460000000001c8010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 
 *
 * // 2. Send CCM packet to MEP, expect to be trapped to the OAMP 
 * // Data: 0x0180c20000330000172b3c4b81000064890260010346000000000011010302abcd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200010100
 *
 * Test Scenario - end
 *
 */

/* Global constants. */
const int MAX_NOF_PORTS_OVER_LAG    = 4;
const int DEFAULT_OAM_LEVEL         = 3;
const int DEFAULT_LM_COUNTER_BASE   = 5;
const int DEFAULT_IN_VLAN           = 100;
const int DEFAULT_EG_VLAN           = 1000;
const uint8 DEFAULT_SHORT_NAME[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
int use_lag_vid                     = 0;

bcm_mac_t src_mac_mep = {0x00, 0x01, 0x02, 0x03, 0x04, 0x01};


/*
 * Test varaibles. Varibables marked as IN can be changed before running the test to change configuration. Variables marked as OUT
 * will be populated throughout the tests's run.
 */
struct dnx_oam_over_lag_s {
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
};

dnx_oam_over_lag_s dnx_oam_over_lag_1;    /* The tests's variables. */

int verbose = 1;    /* Set to 0 to mute test prints. */

/*
 * dnx_oam_over_lag__init
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
 *
 */
int dnx_oam_over_lag__init (
    bcm_port_t trunk_port1,
    bcm_port_t trunk_port2,
    bcm_port_t trunk_port3,
    bcm_port_t eg_port)
{
    bcm_mac_t default_mymac = {0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
    int i;

    /* First clear the struct. */
    sal_memset(&dnx_oam_over_lag_1, 0, sizeof(dnx_oam_over_lag_1));

    dnx_oam_over_lag_1.trunk_ports[0]  = trunk_port1;
    dnx_oam_over_lag_1.trunk_ports[1]  = trunk_port2;
    dnx_oam_over_lag_1.trunk_ports[2]  = trunk_port3;
    dnx_oam_over_lag_1.eg_port         = eg_port;
    dnx_oam_over_lag_1.nof_ports   = MAX_NOF_PORTS_OVER_LAG - 1;
    dnx_oam_over_lag_1.in_vlan     = DEFAULT_IN_VLAN;
    dnx_oam_over_lag_1.eg_vlan     = DEFAULT_EG_VLAN;
    dnx_oam_over_lag_1.oam_level   = DEFAULT_OAM_LEVEL;
    dnx_oam_over_lag_1.lm_counter_base_id = DEFAULT_LM_COUNTER_BASE;
    sal_memcpy(dnx_oam_over_lag_1.mymac, default_mymac, 6);

    return BCM_E_NONE;
}


/** TEST FUNCTIONS **/

/*
 * dnx_oam_over_lag_main
 *
 * Sets the configurations in the cint header.
 *
 * Parameters:
 *  int     unit        - Device to be configured.
 *
 *  All fields of dnx_oam_over_lag_1 marked as "IN".
 *
 *  Returns:
 *  BCM_E_NONE: If all the apis pass without a problem.
 *  BCM_E_*:    If something went wrong.
 *
 */
int dnx_oam_over_lag_main (
    int unit)
{
    int         rv;
    char*       test_name = "Oam endpoint over lag test: ";

    /* OAM group variables */
    bcm_oam_group_info_t oam_group_info;

    /* 1. Create a lag (trunk). */
    rv = dnx_oam_over_lag_trunk_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_oam_over_lag_trunk_create\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone creating trunk. trunk_id = 0x%x\n", test_name, dnx_oam_over_lag_1.trunk_id);
    }

    /* 2. Add ports to this lag. */
    rv = dnx_oam_over_lag_members_add(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_oam_over_lag_members_add\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone adding members to lag\n", test_name);
    }

    /* 3. Add trunk common setting */
    bcm_port_tpid_class_t port_tpid_class;
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = dnx_oam_over_lag_1.trunk_gport;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }

    rv = l2_pp_lag_property_set(unit, dnx_oam_over_lag_1.trunk_gport);
    if (rv != BCM_E_NONE){
        printf("Error, in l2_pp_lag_property_set\n");
        return rv;
    }

    /* 4. Create an ethernet port (vlan-port) over the lag */
    dnx_oam_over_lag_vlan_port_create(unit, 1);
    if (rv != BCM_E_NONE){
        printf("Error, in vlan_port_create\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone creating vlan port. trunk_vlan_gport: 0x%x\n", test_name, dnx_oam_over_lag_1.trunk_vlan_gport);
    }

    /* 4.1: Create another vlan_port and cross connect both vlan ports. */
    rv = dnx_oam_over_lag_cross_connect_add(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_oam_over_lag_cross_connect_add\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone creating second vlan port and corss connecting. second_vlan_gport: 0x%x\n", test_name,
               dnx_oam_over_lag_1.second_vlan_gport);
    }

    /* 5. Create an OAM group. */

    bcm_oam_group_info_t_init(&oam_group_info);
    sal_memcpy(oam_group_info.name, DEFAULT_SHORT_NAME, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &oam_group_info);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_oam_group_create\n");
        return rv;
    }

    dnx_oam_over_lag_1.oam_group_id = oam_group_info.id;

    if (verbose) {
        printf("%sDone creating OAM group. Group id: 0x%x\n", test_name, dnx_oam_over_lag_1.oam_group_id);
    }


    /* 6. Create the Oam endpoint (MEP) with the vlan gport. */
    rv = dnx_oam_over_lag_endpoint_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: in dnx_oam_over_lag_endpoint_create\n");
        return rv;
    }

    if (verbose) {
        printf("%sDone creating endpoint.\n", test_name);
    }

    if (verbose) {
        printf("%sDone\n",test_name);
    }

    return rv;
}

/*
 * cint_dnx_oam_over_lag_cleanup
 *
 * Destorys all configuration created in the main function.
 *
 * Parameters:
 *  int     unit        - Device to be configured.
 *  uint8   test_level  - 0: Destroy full configurations.          1: Destroy configuration except for lag ports' mymac.
 *                        2: Destroy lag and cross connect only.
 *
 *  All fields of dnx_oam_over_lag_1 marked as "OUT" must be configured according to the created components.
 *
 *  Returns:
 *  BCM_E_NONE: If all the apis pass without a problem.
 *  BCM_E_*:    If something went wrong.
 *
 */
int dnx_oam_over_lag_cleanup (
    int unit)
{
    int i, rv;

    /*
     * 5. Destroy OAM endpoint.
     * 4. Destroy OAM group
     *  (Both are destoyed in the same API.)
     */
    rv = bcm_oam_group_destroy(unit, dnx_oam_over_lag_1.oam_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_oam_group_destroy\n");
        return rv;
    }

    /* 3.5 If the second vlan port exists, clean it and the cross connect up. */
    if (dnx_oam_over_lag_1.second_vlan_gport) {
        rv = dnx_oam_over_lag_cross_connect_destroy(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in dnx_oam_over_lag_cross_connect_destroy\n");
            return rv;
        }
    }

    /* 3. Destroy the ethernet port (vlan-port) over the lag.*/
    rv = bcm_vlan_port_destroy(unit, dnx_oam_over_lag_1.trunk_vlan_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy\n");
        return rv;
    }

    /* 2. Remove ports from lag. */
    rv = bcm_trunk_member_delete_all(unit, dnx_oam_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
        printf ("Error, in bcm_trunk_member_delete_all \n");
        return rv;
    }

    /* 1. Destroy lag. */
    rv = bcm_trunk_destroy(unit, dnx_oam_over_lag_1.trunk_id);
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
 * dnx_oam_over_lag_members_add
 *
 * Add ports to the preconfigured lag.
 *
 * Parameters:
 *
 * int          unit                                    - (IN) Device to be configured.
 * int          dnx_oam_over_lag_1.nof_ports       - (IN) Used to iterate over the ports.
 * bcm_port_t   dnx_oam_over_lag_1.trunk_ports[0-x]- (IN) Ports to be configured.
 * bcm_trunk_t  dnx_oam_over_lag_1.trunk_id        - (IN) The precofigured trunk (lag) id.
 *
 * Returns:
 * BCM_E_NONE:  If all ports were added successfully.
 * BCM_E_*:     If something went wrong.
 */
int
dnx_oam_over_lag_members_add (
    int unit)
{
    int i, rv = BCM_E_NONE;
    bcm_trunk_member_t  member;
    bcm_port_t  port;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t  member_array[20];
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    /* Translate the trunk id to a gport id to be used for the ehternet port. */
    BCM_GPORT_TRUNK_SET(dnx_oam_over_lag_1.trunk_gport, dnx_oam_over_lag_1.trunk_id);

    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    rv = bcm_switch_control_indexed_port_set(unit, dnx_oam_over_lag_1.trunk_gport, key, value);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_switch_control_indexed_port_set\n");
        return rv;
    }

    for (i = 0; i < dnx_oam_over_lag_1.nof_ports; i++) {
        port = dnx_oam_over_lag_1.trunk_ports[i];
        rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_switch_control_indexed_port_set\n");
            return rv;
        }
    }

    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    sal_memset(member_array, 0, sizeof(member_array));
    for (i = 0; i < dnx_oam_over_lag_1.nof_ports; i++) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, dnx_oam_over_lag_1.trunk_ports[i]);
        dnx_oam_over_lag_1.system_ports[i] = member_array[i].gport;
    }

    rv = bcm_trunk_set(unit, dnx_oam_over_lag_1.trunk_id, &trunk_info, 3, member_array);
    if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_set(), rv=%d\n", rv);
        return rv;
    }

    return rv;
}

int dnx_oam_over_lag_endpoint_create (
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t mep;
    bcm_oam_endpoint_info_t rmep;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    int counter_if_0 = 0;

    bcm_oam_endpoint_info_t_init(&mep);

    rv = set_counter_resource(unit, dnx_oam_over_lag_1.trunk_ports[0], counter_if_0, 1, &dnx_oam_over_lag_1.lm_counter_base_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_profile_id_get_by_type(ingress) \n");
      return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_profile_id_get_by_type(egress)\n");
      return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_profile_id_get_by_type(accelerated ingress) \n");
      return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_profile_id_get_by_type(accelerated egress) \n");
      return rv;
    }

    rv = bcm_oam_lif_profile_set(unit, 0, dnx_oam_over_lag_1.trunk_vlan_gport, ingress_profile, egress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_lif_profile_set\n");
      return rv;
    }

    rv = bcm_oam_lif_profile_set(unit, 0,  dnx_oam_over_lag_1.second_vlan_gport, ingress_profile, egress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_lif_profile_set\n");
      return rv;
    }

    /*
    * Adding acc MEP
    */
     /*
    * When OAM/BFD statistics enabled and MEP ACC is required to be counted,
    * mep acc id 0 can't be used
    */
    /*TX*/
    mep.type = bcmOAMEndpointTypeEthernet;
    mep.group = dnx_oam_over_lag_1.oam_group_id;
    mep.level = dnx_oam_over_lag_1.oam_level;
    mep.tx_gport = dnx_oam_over_lag_1.trunk_gport;
    mep.name = 456;
    mep.ccm_period = 1000;
    mep.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep.vlan = (use_lag_vid) ? dnx_oam_over_lag_1.in_vlan : 10;
    mep.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep.outer_tpid = 0x8100;
    mep.inner_vlan = 0;
    mep.inner_pkt_pri = 0;
    mep.inner_tpid = 0;
    mep.int_pri = 1 + (3<<2);

    /* Take RDI only from RX*/
    mep.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep.src_mac_address, src_mac_mep, 6);

    /*RX*/
    mep.acc_profile_id = acc_ingress_profile;
    mep.lm_counter_base_id  = dnx_oam_over_lag_1.lm_counter_base_id;
    mep.gport = dnx_oam_over_lag_1.trunk_vlan_gport;

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
    rmep.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;

    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created RMEP with id %d\n", rmep.id);

    dnx_oam_over_lag_1.endpoint_id = mep.id;

    return rv;
}

/*
 * dnx_oam_over_lag_vlan_port_create
 *
 * Create a vlan port, either over lag, or for the test.
 *
 * Parameters:
 *
 * int              unit                                    - (IN) Device to be configured.
 * uint8            is_trunk_port                           - (IN) 1 if this vlan port is configured over lag, 0 is not.
 *
 * if is_trunk_port
 *  bcm_gport_t     dnx_oam_over_lag_1.trunk_gport     - (IN) The lag gport.
 *  bcm_vlan_t      dnx_oam_over_lag_1.in_vlan         - (IN) Vlan to be used as part of the key for this vlan port.
 *  bcm_gport_t     dnx_oam_over_lag_1.trunk_vlan_gport- (OUT)Will be filled with the created gport's id.
 * else
 *  bcm_gport_t     dnx_oam_over_lag_1.eg_port         - (IN) The test port to be cross connected.
 *  bcm_vlan_t      dnx_oam_over_lag_1.eg_vlan         - (IN) Vlan to be used as part of the key for this vlan port.
 *  bcm_gport_t     dnx_oam_over_lag_1.second_vlan_gport- (OUT)Will be filled with the created gport's id.
 *
 * Returns:
 * BCM_E_NONE:  If the vlan port was created successfully.
 * BCM_E_*:     If something went wrong.
 */
int dnx_oam_over_lag_vlan_port_create (
    int unit,
    uint8 is_trunk_port)
{
    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria     = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port         = (is_trunk_port) ? dnx_oam_over_lag_1.trunk_gport : dnx_oam_over_lag_1.eg_port;
    vp.match_vlan   = (is_trunk_port) ? dnx_oam_over_lag_1.in_vlan : dnx_oam_over_lag_1.eg_vlan;
    vp.flags        = 0;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    if (is_trunk_port) {
        dnx_oam_over_lag_1.trunk_vlan_gport = vp.vlan_port_id;
    } else {
        dnx_oam_over_lag_1.second_vlan_gport = vp.vlan_port_id;
    }

    return rv;
}


/*
 * dnx_oam_over_lag_cross_connect_add
 *
 * Create a vlan port, and cross connect it with the lag.
 *
 * Parameters:
 *
 * int              unit                                    - (IN) Device to be configured.
 * bcm_gport_t      dnx_oam_over_lag_1.eg_port         - (IN) The test port to be cross connected.
 * bcm_vlan_t       dnx_oam_over_lag_1.eg_vlan         - (IN) Vlan to be used as part of the key for this vlan port.
 * bcm_gport_t      dnx_oam_over_lag_1.trunk_vlan_gport- (IN) Preconfigured vlan port over lag to be cross connected.
 * bcm_gport_t      dnx_oam_over_lag_1.second_vlan_gport- (OUT)Will be filled with the created gport's id and cross connected to the
 *                                                                  trunk vlan port.
 *
 * Returns:
 * BCM_E_NONE:  If the second vlan port was created and cross connected successfully.
 * BCM_E_*:     If something went wrong.
 */
int dnx_oam_over_lag_cross_connect_add (
    int unit)
{
    int rv;
    bcm_vswitch_cross_connect_t cross_connect;

    rv = dnx_oam_over_lag_vlan_port_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in dnx_oam_over_lag_vlan_port_create\n");
        return rv;
    }

    bcm_vswitch_cross_connect_t_init(&cross_connect);

    cross_connect.port1 = dnx_oam_over_lag_1.trunk_vlan_gport;
    cross_connect.port2 = dnx_oam_over_lag_1.second_vlan_gport;

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

/*
 * dnx_oam_over_lag_cross_connect_destroy
 *
 * Destroy the cross connection and the second vlan port.
 *
 * Parameters:
 *
 * int              unit                                    - (IN) Device to be configured.
 * bcm_gport_t      dnx_oam_over_lag_1.trunk_vlan_gport- (IN) First gport of cross connection.
 * bcm_gport_t      dnx_oam_over_lag_1.second_vlan_gport- (IN)Second gport of cross connection.
 *
 * Returns:
 * BCM_E_NONE:  If the second vlan port was created and cross connected successfully.
 * BCM_E_*:     If something went wrong.
 */
int dnx_oam_over_lag_cross_connect_destroy (
    int unit)
{
    int rv;
    bcm_vswitch_cross_connect_t cross_connect;

    cross_connect.port1 = dnx_oam_over_lag_1.trunk_vlan_gport;
    cross_connect.port2 = dnx_oam_over_lag_1.second_vlan_gport;

    /* Destroy the vswitch cross connect. */
    rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_delete\n");
        return rv;
    }

    /* Destroy second vlan gport. */
    rv = bcm_vlan_port_destroy(unit, dnx_oam_over_lag_1.second_vlan_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy\n");
        return rv;
    }

    return rv;
}

/*
 * dnx_oam_over_lag_trunk_create
 *
 * Create a trunk, then set it to be round robin.
 *
 * Parameters:
 *
 * int              unit                                - (IN) Device to be configured.
 * bcm_trunk_t      dnx_oam_over_lag_1.trunk_id    - (OUT)Created lag.
 *
 * Returns:
 * BCM_E_NONE:  If the trunk was created successfully.
 * BCM_E_*:     If something went wrong.
 */
int dnx_oam_over_lag_trunk_create (
    int unit)
{
    int     rv;
    uint32  trunk_flags = 0;

    /* On JR2, need to set trunk id before creating */
    BCM_TRUNK_ID_SET(dnx_oam_over_lag_1.trunk_id, 0, dnx_oam_over_lag_1.trunk_id);

    rv = bcm_trunk_create(unit, trunk_flags, &dnx_oam_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rv);
      return rv;
    }

    return rv;
}

  int timeout_events_count;

/* This is an example of using bcm_oam_event_register api.
 * A simple callback is created for CCM timeout event.
 * After a mep and rmep are created, the callback is called
 * whenever CCMTimeout event is generated.
 */
int cb_oam (
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
{
    int et = event_type, gp = group, ep = endpoint;
    printf("UNIT: %d, FLAGS: %d, EVENT: %d, GROUP: %d, MEP: 0x%0x\n",unit,flags,et,gp,ep);

    if ((event_type == bcmOAMEventEndpointInterfaceDown) || (event_type == bcmOAMEventEndpointPortDown) ||
          (event_type == bcmOAMEventEndpointInterfaceUp) || (event_type == bcmOAMEventEndpointPortUp)) {
    } else {
        timeout_events_count++;
    }

    return BCM_E_NONE;
}

int register_events (
    int unit)
{
    bcm_error_t rv;
    bcm_oam_event_types_t timeout_event;

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    rv = bcm_oam_event_register(unit, timeout_event, cb_oam, (void*)1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }


  return rv;
}

int unregister_events (
    int unit)
{
    bcm_error_t rv;
    bcm_oam_event_types_t timeout_event;

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    rv = bcm_oam_event_unregister(unit, timeout_event, cb_oam);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

