/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 *
 * This cint provides and example of using OAMP server
 *
 * How to run:
 *
 * cd $SDK
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_oam.c
  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
  cint ../../../../src/examples/dnx/oam/cint_dnx_oam.c
  cint ../../../../src/examples/dnx/oam/cint_dnx_oam_server.c
  cint
  int client_unit=1, server_unit=0;
  dnx_oam_server_client_endpoint_info_glb.maid_type=1;
  print oamp_server_example(server_unit, client_unit, 200, 201, 200, 201);
 * Test Scenario - end
 */

/*
 * OAM server/client endpoint configuration parameters
 */
struct dnx_oam_server_client_ep_global_config_s {
    /*
     * Type of MAID:
     * 0 - short MAID (not supported for OAMP v2), 1 - "long" MAID, 2 - 48B MAID
     */
    int maid_type;

    bcm_vlan_port_t server_vp1, server_vp2, client_vp1, client_vp2, client_vp3;

    bcm_vswitch_cross_connect_t server_cross_connect, client_cross_connect;

    int counter_base_id_upmep, counter_base_id_downmep;

    bcm_oam_group_info_t group_info_short_ma_client, group_info_short_ma_server;

    bcm_mac_t mac_mep_1;

    bcm_mac_t mac_mep_2;

    bcm_mac_t src_mac;

    bcm_mac_t src_mac_1;

    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH], long_name[BCM_OAM_GROUP_NAME_LENGTH];

    bcm_rx_trap_config_t trap_to_server_oamp;

    int trap_code, trap_code_up, trap_code_up_ing;

    bcm_oam_endpoint_info_t ep_server_down, ep_server_up, ep_client_down, ep_client_up, rmep;

    bcm_oam_endpoint_info_t ep_server_down2, ep_server_up2, ep_client_down2, ep_client_up2;

    bcm_oam_endpoint_info_t ep_server_mpls, ep_client_mpls;

    int down_mdl;

    int up_mdl;

    int server_oamp_port;

    int recycle_port_core0;

    int recycle_port_core1;

    int recycle_port_core0_jr2;

    int recycle_port_core1_jr2;

    /*
     *  Used for OAMP v2 server
     *  with OAMP v1 client
     */
    int dedicated_recycle_port;

    /* Created endpoints information */
    oam__ep_info_s server_upmep;

    oam__ep_info_s server_downmep;

    oam__ep_info_s client_downmep;

    oam__ep_info_s client_upmep;

    oam__ep_info_s server_mpls_mep;

    oam__ep_info_s client_mpls_mep;
};


/* Globals */

/*
 * OAM server/client endpoint init configuration
 */
dnx_oam_server_client_ep_global_config_s dnx_oam_server_client_endpoint_info_glb = {
    /* maid_type */
    0,

    /* server_vp1, server_vp2, client_vp1, client_vp2, client_vp3 */
    {0}, {0}, {0}, {0}, {0},

    /* server_cross_connect, client_cross_connect */
    {0}, {0},

    /* counter_base_id_upmep, counter_base_id_downmep */
    0, 0,

    /* group_info_short_ma_client, group_info_short_ma_server */
    {0}, {0},

    /* mac_mep_1 */
    {0x00, 0x00, 0x00, 0x01, 0x02, 0x03},

    /* mac_mep_2 */
    {0x00, 0x00, 0x00, 0xff, 0xff, 0xff},

    /* src_mac */
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},

    /* src_mac_1 */
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xfe},

    /* short_name */
    {1, 3, 2, 0xab, 0xcd},

    /* long_name */
    {1, 32, 13, 'A', 'T', 'T', 00, 00, 02, 04, 'E', 'V', 'C', '1', 0},

    /* trap_to_server_oamp */
    {0},

    /* trap_code, trap_code_up, trap_code_up_ing */
    0, 0, 0,

    /* ep_server_down, ep_server_up, ep_client_down, ep_client_up, rmep */
    {0}, {0}, {0}, {0}, {0},

    /* ep_server_down_2, ep_server_up_2, ep_client_down_2, ep_client_up_2 */
    {0}, {0}, {0}, {0},

    /* ep_server_mpls, ep_client_mpls  */
    {0}, {0},

    /* down_mdl */
    4,

    /* up_mdl */
    5,

    /* server_oamp_port */
    232,

    /* recycle_port_core0 */
    42,

    /* recycle_port_core1 */
    43,

    /* recycle_port_core0_jr2 */
    221,

    /* recycle_port_core1_jr2 */
    222,

    /* dedicated_recycle_port */
    0,

    /* server_upmep */
    {0},

    /* server_downmep */
    {0},

    /* client_downmep */
    {0},

    /* client_upmep */
    {0},

    /* server_mpls_mep */
    {0},

    /* client_mpls_mep */
    {0}
};

/**
 *
 * Vlan-ports vp1 and vp2 will be created on the server unit and
 * will be cross-connected
 * Vlan-ports vp3 and vp4 will be created on the client unit and
 * will be cross-connected
 *
 * @author avive (25/11/2015)
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 *
 * @return int
 */
int oamp_server_create_vlan_ports(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {

    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(server_unit, server_port1, bcmPortClassId, server_port1), "");
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(server_unit, server_port2, bcmPortClassId, server_port2), "");
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(client_unit, client_port1, bcmPortClassId, client_port1), "");
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(client_unit, client_port2, bcmPortClassId, client_port2), "");

    /* Server VLAN port for the down MEP */
    dnx_oam_server_client_endpoint_info_glb.server_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    bcm_vlan_port_t_init(&dnx_oam_server_client_endpoint_info_glb.server_vp1);
    dnx_oam_server_client_endpoint_info_glb.server_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    dnx_oam_server_client_endpoint_info_glb.server_vp1.flags = 0;
    dnx_oam_server_client_endpoint_info_glb.server_vp1.port = server_port1;
    dnx_oam_server_client_endpoint_info_glb.server_vp1.match_vlan = 11;
    dnx_oam_server_client_endpoint_info_glb.server_vp1.egress_vlan = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(server_unit,&dnx_oam_server_client_endpoint_info_glb.server_vp1), "");
    printf("Vlan-port created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.server_vp1.vlan_port_id);

    /** Adding vlan gports is necessary to avoid "not in VLAN" traps */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(server_unit, 11, server_port1, 0), "");

    /* Server VLAN port for the up MEP */
    dnx_oam_server_client_endpoint_info_glb.server_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    bcm_vlan_port_t_init(&dnx_oam_server_client_endpoint_info_glb.server_vp2);
    dnx_oam_server_client_endpoint_info_glb.server_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    dnx_oam_server_client_endpoint_info_glb.server_vp2.flags = 0;
    dnx_oam_server_client_endpoint_info_glb.server_vp2.port = server_port2;
    dnx_oam_server_client_endpoint_info_glb.server_vp2.match_vlan = 12;
    dnx_oam_server_client_endpoint_info_glb.server_vp2.egress_vlan = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(server_unit,&dnx_oam_server_client_endpoint_info_glb.server_vp2), "");
    printf("Vlan-port created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.server_vp2.vlan_port_id);


    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(server_unit, 12, server_port2, 0), "");
    /* Client VLAN port for the down MEP */
    dnx_oam_server_client_endpoint_info_glb.client_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(client_unit, client_port1, bcmPortClassId, client_port1), "");
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(client_unit, client_port2, bcmPortClassId, client_port2), "");

    bcm_vlan_port_t_init(&dnx_oam_server_client_endpoint_info_glb.client_vp1);
    dnx_oam_server_client_endpoint_info_glb.client_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    dnx_oam_server_client_endpoint_info_glb.client_vp1.flags = 0;
    dnx_oam_server_client_endpoint_info_glb.client_vp1.port = client_port1;
    dnx_oam_server_client_endpoint_info_glb.client_vp1.match_vlan = 21;
    dnx_oam_server_client_endpoint_info_glb.client_vp1.egress_vlan = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(client_unit,&dnx_oam_server_client_endpoint_info_glb.client_vp1), "");
    printf("Vlan-port created on client (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(client_unit, 21, client_port1, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(client_unit, 22, client_port1, 0), "");


    /* Client VLAN port for the up MEP */
    dnx_oam_server_client_endpoint_info_glb.client_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    bcm_vlan_port_t_init(&dnx_oam_server_client_endpoint_info_glb.client_vp2);
    dnx_oam_server_client_endpoint_info_glb.client_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    dnx_oam_server_client_endpoint_info_glb.client_vp2.flags = 0;
    dnx_oam_server_client_endpoint_info_glb.client_vp2.port = client_port2;
    dnx_oam_server_client_endpoint_info_glb.client_vp2.match_vlan = 22;
    dnx_oam_server_client_endpoint_info_glb.client_vp2.egress_vlan = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(client_unit,&dnx_oam_server_client_endpoint_info_glb.client_vp2), "");
    printf("Vlan-port created on client (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(client_unit, 21, client_port2, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(client_unit, 22, client_port2, 0), "");

    /* Up MEPs are being trapped in the egress
       In oreder to create supportive environment for this, the two created VLAN ports will be cross-connected
       Packets destined for the up MEP on vp2 will be transmitted to vp1 */
    bcm_vswitch_cross_connect_t_init(&dnx_oam_server_client_endpoint_info_glb.server_cross_connect);
    dnx_oam_server_client_endpoint_info_glb.server_cross_connect.port1 = dnx_oam_server_client_endpoint_info_glb.server_vp1.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.server_cross_connect.port2 = dnx_oam_server_client_endpoint_info_glb.server_vp2.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(server_unit, &dnx_oam_server_client_endpoint_info_glb.server_cross_connect), "");

    bcm_vswitch_cross_connect_t_init(&dnx_oam_server_client_endpoint_info_glb.client_cross_connect);
    dnx_oam_server_client_endpoint_info_glb.client_cross_connect.port1 = dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.client_cross_connect.port2 = dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(client_unit, &dnx_oam_server_client_endpoint_info_glb.client_cross_connect), "");

    return BCM_E_NONE;
}

/**
 *
 * ACC down MEP will reside on vp1 (server_vp1) + RMEP
 * ACC up MEP will reside on vp2 (server_vp2) + RMEP
 *
 * @author avive (25/11/2015)
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 * @param is_server_client - if 1, add "client" endpoint on server device
 *
 * @return int
 */
int oamp_server_set_server_endpoints(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2, int is_server_client) {
    int recycle_port, is_server_oamp_v2, is_client_oamp_v2;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    bcm_vlan_port_t vlan_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_oam_endpoint_info_t ep_server_client_down, ep_server_client_up;

    /* Down-MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_down);
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.ep_server_down.remote_gport, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0); /* Required for handling the traps coming from the client */
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, client_port1, &dnx_oam_server_client_endpoint_info_glb.ep_server_down.tx_gport), ""); /* Set the transmission port to be the down MEP's port on the client */
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.ep_server_down.src_mac_address, dnx_oam_server_client_endpoint_info_glb.src_mac, 6);
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.gport = BCM_GPORT_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.acc_profile_id = BCM_OAM_PROFILE_INVALID;

    if(is_server_client)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(server_unit, server_port1, dnx_oam_server_client_endpoint_info_glb.mac_mep_1), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(server_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(server_unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(server_unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile), "(accelerated ingress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(server_unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile), "(accelerated egress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(server_unit, 0, dnx_oam_server_client_endpoint_info_glb.server_vp1.vlan_port_id, ingress_profile, egress_profile), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(server_unit, 0, dnx_oam_server_client_endpoint_info_glb.server_vp2.vlan_port_id, ingress_profile, egress_profile), "");
        bcm_oam_endpoint_info_t_init(ep_server_client_down);
        ep_server_client_down.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
        ep_server_client_down.acc_profile_id = acc_ingress_profile;
        ep_server_client_down.gport = dnx_oam_server_client_endpoint_info_glb.server_vp1.vlan_port_id;
        ep_server_client_down.flags = BCM_OAM_ENDPOINT_WITH_ID;
        ep_server_client_down.type = bcmOAMEndpointTypeEthernet;
        ep_server_client_down.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
        ep_server_client_down.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
        ep_server_client_down.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;
    }
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.name = 101;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.vlan = 11;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.outer_tpid = 0x8100;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;
    /* Adding TLV */
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.flags = BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.interface_state = BCM_OAM_INTERFACE_TLV_UP;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_down), "");
    printf("Down MEP created on server (unit %d) with id %d, tx_gport is: %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_down.id, dnx_oam_server_client_endpoint_info_glb.ep_server_down.tx_gport);

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 201;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    if(is_server_client)
    {
        ep_server_client_down.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &ep_server_client_down), "");
        printf("Server-client down-MEP created on server (unit %d) with id %d\n", server_unit, ep_server_client_down.id);
    }

    /* Store information on global struct */
    dnx_oam_server_client_endpoint_info_glb.server_downmep.gport = dnx_oam_server_client_endpoint_info_glb.ep_server_down.gport;
    dnx_oam_server_client_endpoint_info_glb.server_downmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;
    dnx_oam_server_client_endpoint_info_glb.server_downmep.rmep_id = dnx_oam_server_client_endpoint_info_glb.rmep.id;

    /* Setting the correct recycle port based on client's up MEP port core */

    dnx_oam_server_client_endpoint_info_glb.recycle_port_core0 = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0_jr2;
    dnx_oam_server_client_endpoint_info_glb.recycle_port_core1 = dnx_oam_server_client_endpoint_info_glb.recycle_port_core1_jr2;


    recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0;
    int core, tm_port;

    BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(client_unit,client_port2,&core,&tm_port), "");

    if (core) {
        recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core1;
    }

    is_server_oamp_v2 = *(dnxc_data_get(server_unit, "oam", "oamp", "oamp_v2", NULL));
    is_client_oamp_v2 = *(dnxc_data_get(client_unit, "oam", "oamp", "oamp_v2", NULL));
    if(is_server_oamp_v2 && !is_client_oamp_v2) {
        /*
         * OAMP v2 server injects a packet with 3 additional bytes.
         * The OAMP v1 client recycle port must remove them.
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(client_unit, recycle_port, bcmPortControlFirstHeaderSize, 3), "Could not configure recycle port\n");
    }

    /* Up-MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_up);

    /**
     * remote_gport is used for reception: this trap
     * needs to be added to the server's OAMP trap
     * TCAM table, otherwise it will be punted
     */
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.ep_server_up.remote_gport, dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing, 7, 0);

    /**
     * The tx_gport will be placed in the OAMP generated ITMH.
     * In this case, the packet will be directed to the client's recycle port
     */
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, recycle_port, &dnx_oam_server_client_endpoint_info_glb.ep_server_up.tx_gport), "");

    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.ep_server_up.src_mac_address, dnx_oam_server_client_endpoint_info_glb.src_mac_1, 6);
    /**
     * For up-MEP, PP port is usually written
     * to the OAMP_MEP_DB table.  Since server
     * cannot convert gport to PP port, valid
     * PP port must be provided from the client
     */

    /** First, get port */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_find(client_unit, &vlan_port), "");

    /** Then, get PP port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(client_unit, vlan_port.port, &flags, &interface_info, &mapping_info), "");
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.gport = mapping_info.pp_port;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    if(is_server_client)
    {
        /** For server client, add client endpoint on server device */

        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(server_unit, 12, server_port1, 0), "");

        bcm_oam_endpoint_info_t_init(ep_server_client_up);
        ep_server_client_up.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
        ep_server_client_up.acc_profile_id = acc_egress_profile;
        ep_server_client_up.gport = dnx_oam_server_client_endpoint_info_glb.server_vp2.vlan_port_id;
        ep_server_client_up.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
        ep_server_client_up.type = bcmOAMEndpointTypeEthernet;
        ep_server_client_up.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
        ep_server_client_up.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
        ep_server_client_up.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep;
    }
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.name = 102;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.flags = BCM_OAM_ENDPOINT_UP_FACING;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.vlan = 22; /* Should be according to tx_gport */
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.outer_tpid = 0x8100;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep; /* For up-MEPs the counter base ID must be set for both the server and client MEPs */

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_up), "");
    printf("Up MEP created on server (unit %d) with id %d, tx_gport is: %d, remote_gport is: %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_up.id, dnx_oam_server_client_endpoint_info_glb.ep_server_up.tx_gport, dnx_oam_server_client_endpoint_info_glb.ep_server_up.remote_gport);

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 202;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    if(is_server_client)
    {
        ep_server_client_up.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &ep_server_client_up), "");
        printf("Server-client up-MEP created on server (unit %d) with id %d\n", server_unit, ep_server_client_up.id);
    }


    /* Store information on global struct */
    dnx_oam_server_client_endpoint_info_glb.server_upmep.gport = dnx_oam_server_client_endpoint_info_glb.ep_server_up.gport;
    dnx_oam_server_client_endpoint_info_glb.server_upmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    dnx_oam_server_client_endpoint_info_glb.server_upmep.rmep_id = dnx_oam_server_client_endpoint_info_glb.rmep.id;

    return BCM_E_NONE;
}

/**
 *
 * nonACC down MEP will reside on vp3 (client_vp1)
 * nonACC up MEP will reside on vp4 (client_vp2)
 *
 * @author avive (25/11/2015)
 *
 * @param client_unit
 * @param client_port1
 * @param client_port2
 *
 * @return int
 */
int oamp_server_set_client_endpoints(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;

    /* Down-MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_down);
    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(client_unit, client_port1, dnx_oam_server_client_endpoint_info_glb.mac_mep_1), "");

    /** Profile for trapping packets for down-MEP in client */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(server_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_create(client_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile), "");

    action_key.dest_mac_type = bcmOAMDestMacTypeMcast;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = bcmOamOpcodeCCM;

    action_result.counter_increment = 0;
    BCM_GPORT_TRAP_SET(action_result.destination, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0);
    action_result.flags = 0;
    action_result.meter_enable = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_action_set(client_unit, flags, acc_ingress_profile, &action_key, &action_result), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id, ingress_profile, egress_profile), "");

    /** Profile for trapping packets for up-MEP in client */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_create(client_unit, flags, bcmOAMProfileEgressAcceleratedEndpoint, &acc_egress_profile), "");

    BCM_GPORT_TRAP_SET(action_result.destination, dnx_oam_server_client_endpoint_info_glb.trap_code_up, 7, 0);

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_action_set(client_unit, flags, acc_egress_profile, &action_key, &action_result), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id, ingress_profile, egress_profile), "");
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.acc_profile_id = acc_ingress_profile;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.gport = dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id; /* The client endpoint must be set up with the ID of the server endpoint. */
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.flags = BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_down), "");
    printf("Down MEP created on client (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_down.id);

    /* Store information on global struct */
    dnx_oam_server_client_endpoint_info_glb.client_downmep.gport = dnx_oam_server_client_endpoint_info_glb.ep_client_down.gport;
    dnx_oam_server_client_endpoint_info_glb.client_downmep.id = dnx_oam_server_client_endpoint_info_glb.ep_client_down.id;

    /* Up-MEP*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_up);

    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(client_unit, client_port2, dnx_oam_server_client_endpoint_info_glb.mac_mep_2), "");
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.acc_profile_id = acc_egress_profile;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.gport = dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id; /* The client endpoint must be set up with the ID of the server endpoint. */
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_up), "");
    printf("Up MEP created on client (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_up.id);

    /* Store information on global struct */
    dnx_oam_server_client_endpoint_info_glb.client_upmep.gport = dnx_oam_server_client_endpoint_info_glb.ep_client_up.gport;
    dnx_oam_server_client_endpoint_info_glb.client_upmep.id = dnx_oam_server_client_endpoint_info_glb.ep_client_up.id;

    return BCM_E_NONE;
}

/**
 *
 * Down MEP server1 will reside on server_unit + RMEP
 * Down MEP server2 will reside on client_unit + RMEP
 * Down MEP client1 will reside on client_vp1 on client_unit
 * Down MEP client2 will reside on client_vp2 on client_unit
 *
 * @author xunsun (22/09/2021)
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 *
 * @return int
 */
int oamp_server_set_multi_server_endpoints_down(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    bcm_port_mapping_info_t mapping_info;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;

    /** Config oam lif profile in client device */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile), "(accelerated ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile), "(accelerated egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id, ingress_profile, egress_profile), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id, ingress_profile, egress_profile), "");

    /** Create accellerated profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_create(client_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile), "");

    /** Set action for trapping packets in client device */
    action_key.dest_mac_type = bcmOAMDestMacTypeMcast;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = bcmOamOpcodeCCM;
    action_result.counter_increment = 0;
    BCM_GPORT_TRAP_SET(action_result.destination, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0);
    action_result.flags = 0;
    action_result.meter_enable = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_action_set(client_unit, flags, acc_ingress_profile, &action_key, &action_result), "");

    /** Config OAM Server down MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_down);
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.ep_server_down.src_mac_address, dnx_oam_server_client_endpoint_info_glb.src_mac, 6);
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.ep_server_down.remote_gport, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0);
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, client_port1, &dnx_oam_server_client_endpoint_info_glb.ep_server_down.tx_gport), "");
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.gport = BCM_GPORT_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.name = 101;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.vlan = 21;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down.outer_tpid = 0x8100;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_down), "");
    printf("Down MEP created on server (unit %d) with id %d, tx_gport is: %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_down.id, dnx_oam_server_client_endpoint_info_glb.ep_server_down.tx_gport);

    /** Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 201;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_down2);
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.ep_server_down2.src_mac_address, dnx_oam_server_client_endpoint_info_glb.src_mac_1, 6);
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.ep_server_down2.remote_gport, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0);
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, client_port2, &dnx_oam_server_client_endpoint_info_glb.ep_server_down2.tx_gport), "");
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.gport = BCM_GPORT_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.name = 102;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.vlan = 22;
    dnx_oam_server_client_endpoint_info_glb.ep_server_down2.outer_tpid = 0x8100;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_down2), "");
    printf("Down MEP created on server (unit %d) with id %d, tx_gport is: %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_down2.id, dnx_oam_server_client_endpoint_info_glb.ep_server_down2.tx_gport);

    /** Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 202;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_down2.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down2.id;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    /** Config OAM Client down MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_down);
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.acc_profile_id = acc_ingress_profile;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.gport = dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.flags = BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down.local_id = server_unit;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_down), "");
    printf("Server-client down-MEP created on server (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_down.id);

    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_down2);
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.acc_profile_id = acc_ingress_profile;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.gport = dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.flags = BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.level = dnx_oam_server_client_endpoint_info_glb.down_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.lm_counter_base_id = 0;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.id = dnx_oam_server_client_endpoint_info_glb.ep_server_down2.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_down2.local_id = client_unit;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_down2), "");
    printf("Server-client down-MEP created on server (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_down2.id);

    return BCM_E_NONE;
}

/**
 *
 * UP MEP server1 will reside on server_unit + RMEP
 * UP MEP server2 will reside on client_unit + RMEP
 * UP MEP client1 will reside on client_vp1 on client_unit
 * UP MEP client2 will reside on client_vp2 on client_unit
 *
 * @author xunsun (22/09/2021)

 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 *
 * @return int
 */
int oamp_server_set_multi_server_endpoints_up(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {
    int recycle_port;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    bcm_vlan_port_t vlan_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_oam_action_key_t action_key;   
    bcm_oam_action_result_t action_result;

    /** Config oam lif profile in client device */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile), "(accelerated ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile), "(accelerated egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id, ingress_profile, egress_profile), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id, ingress_profile, egress_profile), "");

    /** Create accellerated profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_create(client_unit, flags, bcmOAMProfileEgressAcceleratedEndpoint, &acc_egress_profile), "");

    /** Set action for trapping packets in client device */
    action_key.dest_mac_type = bcmOAMDestMacTypeMcast;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = bcmOamOpcodeCCM;
    action_result.counter_increment = 0;
    action_result.flags = 0;
    action_result.meter_enable = 0;
    BCM_GPORT_TRAP_SET(action_result.destination, dnx_oam_server_client_endpoint_info_glb.trap_code_up, 7, 0);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_action_set(client_unit, flags, acc_egress_profile, &action_key, &action_result), "");

    /** Setting the correct recycle port based on client's up MEP port core */
    dnx_oam_server_client_endpoint_info_glb.recycle_port_core0 = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0_jr2;
    dnx_oam_server_client_endpoint_info_glb.recycle_port_core1 = dnx_oam_server_client_endpoint_info_glb.recycle_port_core1_jr2;

    /** Config OAM Server Up-MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_up);
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_up2);
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.ep_server_up.src_mac_address, dnx_oam_server_client_endpoint_info_glb.src_mac, 6);
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.ep_server_up2.src_mac_address, dnx_oam_server_client_endpoint_info_glb.src_mac_1, 6);

    /**
     * remote_gport is used for reception: this trap
     * needs to be added to the server's OAMP trap
     * TCAM table, otherwise it will be punted
     */
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.ep_server_up.remote_gport, dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing, 7, 0);
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.remote_gport = dnx_oam_server_client_endpoint_info_glb.ep_server_up.remote_gport;

    /**
     * The tx_gport will be placed in the OAMP generated ITMH.
     * In this case, the packet will be directed to the client's recycle port
     */

    /** Config tx_gport for OAM server1 */
    int core, tm_port;
    BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(client_unit,client_port1,&core,&tm_port), "");
    recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0;
    if (core) {
        recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core1;
    }
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, recycle_port, &dnx_oam_server_client_endpoint_info_glb.ep_server_up.tx_gport), "");
    /** Config tx_gport for OAM server2 */
    BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(client_unit,client_port2,&core,&tm_port), "");
    recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0;
    if (core) {
        recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core1;
    }
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, recycle_port, &dnx_oam_server_client_endpoint_info_glb.ep_server_up2.tx_gport), "");

    /**
     * For up-MEP, PP port is usually written
     * to the OAMP_MEP_DB table.  Since server
     * cannot convert gport to PP port, valid
     * PP port must be provided from the client
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(client_unit, client_port1, &flags, &interface_info, &mapping_info), "");
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.gport = mapping_info.pp_port;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(client_unit, client_port2, &flags, &interface_info, &mapping_info), "");
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.gport = mapping_info.pp_port;

    dnx_oam_server_client_endpoint_info_glb.ep_server_up.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.name = 101;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.flags = BCM_OAM_ENDPOINT_UP_FACING;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.vlan = 21;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.outer_tpid = 0x8100;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_up), "");
    printf("Up MEP created on server (unit %d) with id %d, tx_gport is: %d, remote_gport is: %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_up.id, dnx_oam_server_client_endpoint_info_glb.ep_server_up.tx_gport, dnx_oam_server_client_endpoint_info_glb.ep_server_up.remote_gport);

    /** Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 201;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.name = 102;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.flags = BCM_OAM_ENDPOINT_UP_FACING;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.vlan = 22;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up2.outer_tpid = 0x8100;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_up2), "");
    printf("Up MEP created on server (unit %d) with id %d, tx_gport is: %d, remote_gport is: %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_up2.id, dnx_oam_server_client_endpoint_info_glb.ep_server_up2.tx_gport, dnx_oam_server_client_endpoint_info_glb.ep_server_up2.remote_gport);

    /** Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 202;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_up2.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up2.id;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_up);
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.acc_profile_id = acc_egress_profile;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.gport = dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up.local_id = server_unit;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_up), "");
    printf("Server-client up-MEP created on server (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_up.id);

    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_up2);
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.acc_profile_id = acc_egress_profile;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.gport = dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.level = dnx_oam_server_client_endpoint_info_glb.up_mdl;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up2.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_up2.local_id = client_unit;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_up2), "");
    printf("Server-client up-MEP created on server (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_up2.id);

    return BCM_E_NONE;
}

/**
 * In JR2, we need to create a special ingress trap.
 * Similar to the default accelerated egress profile,
 * the egress trap will edit the FHEI to point to the
 * ingress trap.  Under the ingress trap, system headers
 * will not be modified.  The difference here is the
 * destination: server's OAMP instead of local
 */
int create_special_ingress_trap_for_upmep(int client_unit, int server_unit)
{
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_rx_trap_config_t server_solo_trap_config;
    int server_solo_trap_id;

    dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags = BCM_RX_TRAP_UPDATE_DEST;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing), "");
    if(*(dnxc_data_get(server_unit, "oam", "oamp", "oamp_v2", NULL)))
    {
        /* Server unit is OAMP v2 - configure outLIF for trap */
        bcm_rx_trap_type_get(server_unit, /* flags */ 0, bcmRxTrapEgTxOamUpMEPOamp, &server_solo_trap_id);
        bcm_rx_trap_get(server_unit, server_solo_trap_id, &server_solo_trap_config);
        bcm_rx_trap_get(server_unit, BCM_GPORT_TRAP_GET_ID(server_solo_trap_config.cpu_trap_gport), &server_solo_trap_config);
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags |= BCM_RX_TRAP_UPDATE_ENCAP_ID;
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.encap_id = server_solo_trap_config.encap_id;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(client_unit, dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing, dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp), "");
    printf("Trap created trap_code = %d \n", dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing);

    if(*(dnxc_data_get(client_unit, "oam", "oamp", "oamp_v2", NULL)) == 0)
    {
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "OAM_No_Header", sizeof(context_info.name));
        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(client_unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id), "");

        /** Changing new context SYS HDR profile to None (No SYS HDRs are added) */
        context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        context_param.param_arg = 0;
        context_param.param_val = bcmFieldSystemHeaderProfileNone;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(client_unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param), "");

        context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
        context_param.param_arg = 0;
        context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset0, 0);
        /**Context that was created for iPMF1 is also created for iPMF2*/
        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(client_unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param), "");

        /** Adding trap_code preselector for the context */
        presel_entry_id.presel_id = 3;
        presel_entry_id.stage = bcmFieldStageIngressPMF1;

        presel_data.entry_valid = TRUE;
        presel_data.context_id = context_id;
        presel_data.nof_qualifiers = 1;
        presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
        presel_data.qual_data[0].qual_value = dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing;
        presel_data.qual_data[0].qual_mask = 0x1FF;

        BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(client_unit, 0, &presel_entry_id, &presel_data), "");
    }
    return BCM_E_NONE;
}

/**
 * Traps for server example
 * First trap: for down-MEPs, traps directly to server OAMP
 * Second trap: for up-MEPs, special trap with editing
 * Third trap: trap for stamping, used by OAMP for verification
 *
 * @param server_unit
 * @param client_unit
 *
 * @return int
 */
int oamp_server_create_traps(int server_unit, int client_unit) {

    int server_solo_trap_id, trap_creator_unit = client_unit, trap_copier_unit = server_unit, is_server_oamp_v2, is_client_oamp_v2;
    bcm_rx_trap_config_t server_solo_trap_config;

    is_server_oamp_v2 = *(dnxc_data_get(server_unit, "oam", "oamp", "oamp_v2", NULL));
    is_client_oamp_v2 = *(dnxc_data_get(client_unit, "oam", "oamp", "oamp_v2", NULL));

    if(is_server_oamp_v2 && !is_client_oamp_v2)
    {
        trap_creator_unit = server_unit;
        trap_copier_unit = client_unit;
    }

    /* Configure a trap which updates the destination to be the OAMP of the server unit.
       This trap will be used by the client's down MEP */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(trap_creator_unit, 0, bcmRxTrapUserDefine, &dnx_oam_server_client_endpoint_info_glb.trap_code), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(trap_copier_unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &dnx_oam_server_client_endpoint_info_glb.trap_code), "");

    bcm_rx_trap_config_t_init(&dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp);
    dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(server_unit, dnx_oam_server_client_endpoint_info_glb.server_oamp_port, &dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.dest_port), "");

    if(is_server_oamp_v2)
    {
        /* Server unit is OAMP v2 - configure outLIF for trap */
        bcm_rx_trap_type_get(server_unit, /* flags */ 0, bcmRxTrapOamEthAccelerated, &server_solo_trap_id);
        bcm_rx_trap_get(server_unit, server_solo_trap_id, &server_solo_trap_config);
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags |= BCM_RX_TRAP_UPDATE_ENCAP_ID;
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.encap_id = server_solo_trap_config.encap_id;
    }

    if (*(dnxc_data_get(client_unit, "oam", "feature", "oam_offset_supported", NULL)))
    {
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags |= BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.egress_forwarding_index = 7;
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(client_unit, dnx_oam_server_client_endpoint_info_glb.trap_code, dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp), "");
    printf("Trap created trap_code = %d \n", dnx_oam_server_client_endpoint_info_glb.trap_code);

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(server_unit, dnx_oam_server_client_endpoint_info_glb.trap_code, dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp), "");

    /* Configure another trap which updates the destination to be the OAMP of the server unit.
       This trap will be used by the client's up MEP */

    /** Special trap for up-MEP - see function header */
    BCM_IF_ERROR_RETURN_MSG(create_special_ingress_trap_for_upmep(client_unit, server_unit), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &dnx_oam_server_client_endpoint_info_glb.trap_code_up), "");

    /** In JR2, this trap should map to the ingress trap
        Set the stamped trap code to regular OAMP trap code.*/
    int stamped_trap_code;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(client_unit,0,bcmRxTrapOamEthAccelerated,&stamped_trap_code), "");
    bcm_rx_trap_config_t_init(&dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp);
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.cpu_trap_gport, dnx_oam_server_client_endpoint_info_glb.trap_code_up_ing, 15, 0);
    dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.stamped_trap_code = stamped_trap_code;

    if(is_client_oamp_v2)
    {
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags2 = BCM_RX_TRAP_FLAGS2_MIRROR_CODE_CLEAR | BCM_RX_TRAP_FLAGS2_SNOOP_CODE_CLEAR;
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.dest_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0_jr2;
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.is_recycle_append_ftmh = 1;
        dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp.flags = BCM_RX_TRAP_UPDATE_DEST;
    }

    /* Using same trap configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(client_unit, dnx_oam_server_client_endpoint_info_glb.trap_code_up, dnx_oam_server_client_endpoint_info_glb.trap_to_server_oamp), "");
    printf("Trap created trap_code = %d \n", dnx_oam_server_client_endpoint_info_glb.trap_code_up);

    if(is_server_oamp_v2 && !is_client_oamp_v2)
    {
        /* Reroute the egress trap in the server to the second recycle port */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(server_unit, 0, bcmRxTrapEgTxOamUpMEPOamp, &server_solo_trap_id), "Could not get egress trap ID");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_get(server_unit, server_solo_trap_id, &server_solo_trap_config), "Could not get egress trap params");
        server_solo_trap_config.dest_port = dnx_oam_server_client_endpoint_info_glb.dedicated_recycle_port;
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(server_unit, server_solo_trap_id, &server_solo_trap_config), "Could not modify egress trap");
        BCM_IF_ERROR_RETURN_MSG(dnx_oam__switch_contol_indexed_port_set(server_unit,dnx_oam_server_client_endpoint_info_glb.dedicated_recycle_port,BCM_SWITCH_PORT_HEADER_TYPE_IBCH1_MODE,1), "Could not configure dedicated recycle port");
    }

    return BCM_E_NONE;
}

/**
 * An example of setting up the OAMP as a server.
 * Create an up MEP and a down MEP, each first on the server
 * side (transmitting and recieving CCMs), then on the client
 * side (trapping OAM PDUs)
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1 - ACC Down MEP will reside on a LIF
 *              created at this port
 * @param server_port2 - ACC Up MEP will reside on a LIF created
 *              at this port
 * @param client_port1 - Down MEP will reside on a LIF created
 *              at this port
 * @param client_port2 - Up MEP will reside on a LIF created at
 *              this port
 *
 * @return int
 *                 ___
 *             ___(   )_
 *           _(         )_
 *          (  Network ___)
 *         (_      ___)
 *          (_____)<-+
 *                   |
 *                   |
 *                   |
 *  _____________    |    _____________
 * |             |<--+-->|             |
 * |             |       |             |
 * |   Server    |       |   Client    |
 * |             |\     /|             | (nonACC down & up MEPS)
 * | (vp1)       |  \ /  | (vp3)       |
 * |   (vp2)     |  / \  |   (vp4)     |
 * |_____________|/     \|_____________|
 *     |
 *   __|___
 *  |      |
 *  | OAMP | (ACC down & up MEPS)
 *  |______|
 *
 */
int oamp_server_example(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {
    int counter_if_0 = 0, counter_if_1 = 1;
    int index;

    /* Create two VLAN ports on each unit (client and server) for the MEPs */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_vlan_ports(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");

    /*
     * Allocate counter engines, and get counters
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(client_unit, client_port1, counter_if_0, 1, &dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(client_unit, client_port2, counter_if_1, 1, &dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep), "");

    /* OAM Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client);
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server);
    if (dnx_oam_server_client_endpoint_info_glb.maid_type) {
        if(dnx_oam_server_client_endpoint_info_glb.maid_type == 2)
        {
            /** necessary only for server unit - client group is only for passing validation */
            for(index = 0; index < 48; index++)
            {
                dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name[index] = 150 - index;
            }
            dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
            if(!(*(dnxc_data_get(server_unit, "oam", "oamp", "oamp_v2", NULL))))
            {
                dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.group_name_index = 0x10098;
            }
        }
        else
        {
            sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
     } else {
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
    }

    /* The server side group is used to determine the MAID on CCMs, both those transmitted and the expected MAID on recieved CCMs.
       This is conveyed through the name field.*/
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server), "");
    /* The client side is only used for logically clustering endpoint together. Recieved/Transmitted packets are not affected by this*/
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client), "");

    /** Create traps that connect the two devices */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_traps(server_unit, client_unit), "");

    /* Set the server endpoints */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_set_server_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2, FALSE), "");

    /* Set the client endpoints */
    /* It is the user responsibility to verify that the fields of the server and client device match. */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_set_client_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");

    return BCM_E_NONE;
}

/**
 * Replaces tx_gport / remote_gport and vlan of an up MEP
 *
 * @author avive (15/12/2015)
 *
 * @param server_unit
 * @param server_upmep_id
 * @param new_tx_unit
 * @param new_tx_port
 * @param new_entering_vlan
 *
 * @return int
 */
int oamp_server_switch_upmep_tx_unit(int server_unit, int server_upmep_id, int new_tx_unit, int new_tx_port, int new_entering_vlan, int configure_rcy_port) {
    int recycle_port;
    bcm_vlan_port_t vlan_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags=0;

    /* Get the server up MEP */
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_up);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_get(server_unit, server_upmep_id, &dnx_oam_server_client_endpoint_info_glb.ep_server_up), "");

    /* Setting the correct recycle port based on tx_unit's up MEP port core */
    recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core0;
    int core, tm_port;

    BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(new_tx_unit,new_tx_port,&core,&tm_port), "");

    if (core) {
        recycle_port = dnx_oam_server_client_endpoint_info_glb.recycle_port_core1;
    }

    if(configure_rcy_port)
    {
        /* The uPEM adds IBCH1 and the recycle port is configured for INJECTED_2_PP - configure recycle port to remove IBCH1 */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(new_tx_unit, recycle_port, bcmPortControlFirstHeaderSize, 3), "Could not configure recycle port\n");
    }

    /** Tx gport is the recycle port of the client unit */
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(new_tx_unit, recycle_port, &dnx_oam_server_client_endpoint_info_glb.ep_server_up.tx_gport), ""); /* After recycling, source port (on PTCH) will be new_tx_gport*/

    /** Then, get PP port */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = dnx_oam_server_client_endpoint_info_glb.server_vp2.vlan_port_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_find(new_tx_unit, &vlan_port), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(new_tx_unit, vlan_port.port, &flags, &interface_info, &mapping_info), "");
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.gport = mapping_info.pp_port;
    dnx_oam_server_client_endpoint_info_glb.ep_server_up.vlan = new_entering_vlan; /* Should be according to tx_gport */

    /* Update the server up MEP endpoint */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_destroy(server_unit, server_upmep_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_up), "");

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 202;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = bcmOAMEndpointTypeEthernet;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_up.id;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");

    printf("Up MEP 0x%x on server unit %d will now transmit injected packets through unit %d, port %d, vlan %d\n", server_upmep_id, server_unit, new_tx_unit, new_tx_port, new_entering_vlan);

    return BCM_E_NONE;
}

/*
 * Sets the key to be [OAM-Trap-code, OAMP-dest-port] and sets the value for that key
 * That value is used to identify packets trapped to the OAMP and apply different configurations if necessary.
 * Relevant for Jericho1 only
 */
int field_oam_advanced_features(int unit) {
    bcm_field_group_t grp_tcam = 1;
    int group_priority = 10;
    int qual_id = 1;
    int trap_id;
    int flags = 0;

    /*Get trap_id value for OAM trap*/
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit,flags,bcmRxTrapOamEthAccelerated,&trap_id), "");

    /* Create field group for OAM EndTLV */
    BCM_IF_ERROR_RETURN_MSG(field__prge_action_create_field_group(unit, group_priority, grp_tcam, qual_id), "");

    /* Set action code for the created group */
    printf("Adding entry to mark OAM packets for advances processing\n");
    /* Default trap, for up MEPs */
    BCM_IF_ERROR_RETURN_MSG(field__prge_action_code_add_entry(unit, grp_tcam, trap_id, 232/*dest_port (OAMP PP port)*/,
                                               qual_id, 3 /*prge_action_code_oam_end_tlv_wa*/), "");
    /* Configured trap (the one created in oamp_server_example), for down MEPs */
    BCM_IF_ERROR_RETURN_MSG(field__prge_action_code_add_entry(unit, grp_tcam, dnx_oam_server_client_endpoint_info_glb.trap_code, 232/*dest_port (OAMP PP port)*/,
                                               qual_id, 3 /*prge_action_code_oam_end_tlv_wa*/), "");

    return BCM_E_NONE;
}

/**
 * Server-client example (JR2 and above)
 * Similar setup to oamp_server_example,
 * except here  client endpoints are
 * created on the server unit as well.
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 *
 * @return int
 */
int oamp_server_client_example(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {

    int counter_if_0 = 0, counter_if_1 = 1;
    int index;

    /* Create two VLAN ports on each unit (client and server) for the MEPs */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_vlan_ports(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");

    /*
     * Allocate counter engines, and get counters
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(client_unit, client_port1, counter_if_0, 1, &dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(client_unit, client_port2, counter_if_1, 1, &dnx_oam_server_client_endpoint_info_glb.counter_base_id_upmep), "");

    /* OAM Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client);
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server);
    if (dnx_oam_server_client_endpoint_info_glb.maid_type) {
        if(dnx_oam_server_client_endpoint_info_glb.maid_type == 2)
        {
            /** necessary only for server unit - client group is only for passing validation */
            for(index = 0; index < 48; index++)
            {
                dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name[index] = 150 - index;
            }
            dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
            if(!(*(dnxc_data_get(server_unit, "oam", "oamp", "oamp_v2", NULL))))
            {
                dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.group_name_index = 0x10098;
            }
        }
        else
        {
            sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
     } else {
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
     }

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client), "");

    /** Create traps that connect the two devices */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_traps(server_unit, client_unit), "");

    /* Set the server endpoints */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_set_server_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2, TRUE), "");

    /* Set the client endpoints */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_set_client_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");

    return BCM_E_NONE;
}

/**
 * Server-client example (JR2 and above)
 * Similar setup to oamp_server_example,
 * except here two client endpoints are
 * created and mapped respectively to
 * two server endpoints.
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 * @param is_up
 *
 * @return int
 */
int oamp_multi_servers_clients_example(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2, int is_up) {

    int counter_if_0 = 0, counter_if_1 = 1;
    int index;

    /** Create two VLAN ports on each unit (client and server) for the MEPs */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_vlan_ports(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");

    /** OAM Groups must be created on server device and client device.*/
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client);
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server);
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client), "");

    /** Create traps that connect the two devices */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_traps(server_unit, client_unit), "");

    if (is_up) {
        /**
         * CCM packet forworded to client_vp1 and client_vp2 will hit up MEP
         * trap incorrectly since up MEP is created on them. So one addtional
         * VLAN port is created here for forwording the injected CCM packet
         * by up MEP.
         */
        bcm_vlan_port_t_init(&dnx_oam_server_client_endpoint_info_glb.client_vp3);
        dnx_oam_server_client_endpoint_info_glb.client_vp3.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
        dnx_oam_server_client_endpoint_info_glb.client_vp3.flags = 0;
        dnx_oam_server_client_endpoint_info_glb.client_vp3.port = client_port2;
        dnx_oam_server_client_endpoint_info_glb.client_vp3.match_vlan = 100;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(client_unit,&dnx_oam_server_client_endpoint_info_glb.client_vp3), "");
        printf("Vlan-port created on client (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.client_vp3.vlan_port_id);

        /** Config coross connection between client_vp1 and client_vp3 */
        bcm_vswitch_cross_connect_t_init(&dnx_oam_server_client_endpoint_info_glb.client_cross_connect);
        dnx_oam_server_client_endpoint_info_glb.client_cross_connect.port1 = dnx_oam_server_client_endpoint_info_glb.client_vp1.vlan_port_id;
        dnx_oam_server_client_endpoint_info_glb.client_cross_connect.port2 = dnx_oam_server_client_endpoint_info_glb.client_vp3.vlan_port_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(client_unit, &dnx_oam_server_client_endpoint_info_glb.client_cross_connect), "");

        /** Config coross connection between client_vp1 and client_vp3 */
        bcm_vswitch_cross_connect_t_init(&dnx_oam_server_client_endpoint_info_glb.client_cross_connect);
        dnx_oam_server_client_endpoint_info_glb.client_cross_connect.port1 = dnx_oam_server_client_endpoint_info_glb.client_vp2.vlan_port_id;
        dnx_oam_server_client_endpoint_info_glb.client_cross_connect.port2 = dnx_oam_server_client_endpoint_info_glb.client_vp3.vlan_port_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(client_unit, &dnx_oam_server_client_endpoint_info_glb.client_cross_connect), "");

        /** Add client_port2 to VLAN 100 */
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(client_unit, 100, client_port2, 0), "");

        /** Config the server and client up endpoints */
        BCM_IF_ERROR_RETURN_MSG(oamp_server_set_multi_server_endpoints_up(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");
    }
    else 
    {
        /** Config the server and client down endpoints */
        BCM_IF_ERROR_RETURN_MSG(oamp_server_set_multi_server_endpoints_down(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2), "");
    }
    return BCM_E_NONE;
}

/**
 *
 * Create OAMoMPLS server endpoint
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 * @param type - type of MEP to create
 * @param is_server_client - if 1, add "client" endpoint on server device
 * @param outlif - value to put in intf_id
 * @param label - value to use for MPLS/PWE label
 * @param gport - value to use for gport (server-client)
 *
 * @return int
 */
int oamp_server_set_mpls_server_endpoint(
    int server_unit,
    int client_unit,
    int server_port1,
    int server_port2,
    int client_port1,
    int client_port2,
    bcm_oam_endpoint_type_t type,
    int is_server_client,
    int outlif,
    int label,
    int gport)
{
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = BCM_OAM_PROFILE_INVALID;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_vlan_port_t vlan_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_oam_endpoint_info_t ep_server_client_mpls;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_mpls_tp_channel_type_tx_set(server_unit, bcmOamMplsTpChannelPweonoam, 0x8902), "");

    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_server_mpls);
    BCM_GPORT_TRAP_SET(dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.remote_gport, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0); /* Required for handling the traps coming from the client */
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(client_unit, client_port1, &dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.tx_gport), ""); /* Set the transmission port to be the MEP's port on the client */
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.gport = BCM_GPORT_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.intf_id = outlif;

    if(is_server_client)
    {
        /* Setting My-CFM-MAC */
        BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(server_unit, server_port1, dnx_oam_server_client_endpoint_info_glb.mac_mep_1), "");

        action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
        action_key.endpoint_type = bcmOAMMatchTypeMEP;
        action_key.flags = 0;
        action_key.inject = 0;
        action_key.opcode = bcmOamOpcodeCCM;

        action_result.counter_increment = 0;
        BCM_GPORT_TRAP_SET(action_result.destination, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0);
        action_result.flags = 0;
        action_result.meter_enable = 0;

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(server_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_create(server_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_action_set(server_unit, flags, acc_ingress_profile, &action_key, &action_result), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(server_unit, 0, gport, ingress_profile, egress_profile), "");

        bcm_oam_endpoint_info_t_init(ep_server_client_mpls);
        ep_server_client_mpls.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
        ep_server_client_mpls.acc_profile_id = acc_ingress_profile;
        ep_server_client_mpls.gport = gport;
        ep_server_client_mpls.flags = BCM_OAM_ENDPOINT_WITH_ID;
        ep_server_client_mpls.type = type;
        ep_server_client_mpls.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
        ep_server_client_mpls.level = 7;
        ep_server_client_mpls.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;
    }
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.type = type;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.id;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.level = 7;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.name = 101;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;

    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.egress_label.label = label;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.egress_label.ttl = 0xa;
    dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.egress_label.exp = 1;


    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.ep_server_mpls), "");
    printf("MPLS MEP created on server (unit %d) with id %d, tx_gport is: %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.id, dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.tx_gport);

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.rmep);
    dnx_oam_server_client_endpoint_info_glb.rmep.name = 201;
    dnx_oam_server_client_endpoint_info_glb.rmep.local_id = dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.id;
    dnx_oam_server_client_endpoint_info_glb.rmep.type = type;
    dnx_oam_server_client_endpoint_info_glb.rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    dnx_oam_server_client_endpoint_info_glb.rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.rmep.loc_clear_threshold = 1;
    dnx_oam_server_client_endpoint_info_glb.rmep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.id;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.rmep), "");
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, dnx_oam_server_client_endpoint_info_glb.rmep.id);

    if(is_server_client)
    {
        ep_server_client_mpls.id = dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.id;
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(server_unit, &ep_server_client_mpls), "");
        printf("Server-client MPLS MEP created on server (unit %d) with id %d\n", server_unit, ep_server_client_mpls.id);
    }

    /* Store information on global struct */
    dnx_oam_server_client_endpoint_info_glb.server_mpls_mep.gport = dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.gport;
    dnx_oam_server_client_endpoint_info_glb.server_mpls_mep.id = dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.id;
    dnx_oam_server_client_endpoint_info_glb.server_mpls_mep.rmep_id = dnx_oam_server_client_endpoint_info_glb.rmep.id;

    return BCM_E_NONE;
}

/**
 *
 * Create OAMoMPLS client endpoint
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1
 * @param server_port2
 * @param client_port1
 * @param client_port2
 * @param type
 * @param gport
 *
 * @return int
 */
int oamp_server_set_mpls_client_endpoint(
    int server_unit,
    int client_unit,
    int server_port1,
    int server_port2,
    int client_port1,
    int client_port2,
    bcm_oam_endpoint_type_t type,
    int gport)
{
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = BCM_OAM_PROFILE_INVALID;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;

    bcm_oam_endpoint_info_t_init(dnx_oam_server_client_endpoint_info_glb.ep_client_mpls);

    /* Setting My-CFM-MAC */
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(client_unit, client_port1, dnx_oam_server_client_endpoint_info_glb.mac_mep_1), "");

    action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = bcmOamOpcodeCCM;

    action_result.counter_increment = 0;
    BCM_GPORT_TRAP_SET(action_result.destination, dnx_oam_server_client_endpoint_info_glb.trap_code, 7, 0);
    action_result.flags = 0;
    action_result.meter_enable = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(client_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    if(type == bcmOAMEndpointTypeBhhSection) {
        flags |= BCM_OAM_PROFILE_TYPE_SECTION;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_create(client_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_action_set(client_unit, 0, acc_ingress_profile, &action_key, &action_result), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(client_unit, 0, gport, ingress_profile, egress_profile), "");

    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.acc_profile_id = acc_ingress_profile;

    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.gport = gport;
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.id = dnx_oam_server_client_endpoint_info_glb.ep_server_mpls.id; /* The client endpoint must be set up with the ID of the server endpoint. */
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.flags = BCM_OAM_ENDPOINT_WITH_ID;
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.type = type;
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.group = dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.id;
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.level = 7;
    dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.lm_counter_base_id = dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.ep_client_mpls), "");
    printf("MPLS MEP created on client (unit %d) with id %d\n", client_unit, dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.id);

    /* Store information on global struct */
    dnx_oam_server_client_endpoint_info_glb.client_mpls_mep.gport = dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.gport;
    dnx_oam_server_client_endpoint_info_glb.client_mpls_mep.id = dnx_oam_server_client_endpoint_info_glb.ep_client_mpls.id;

    return BCM_E_NONE;
}

/**
 * Create accelerated y.1731 OAM sever endpoint and client
 * of type MPLS-TP or PWE on multi-device.
 *
 * @param server_unit
 * @param client_unit
 * @param server_port1 - in port for server unit
 * @param server_port2 - out port for server unit
 * @param client_port1 - in port for client unit
 * @param client_port2 - out port for client unit
 * @param type - MEP type to set
 * @param is_server_client - client packets can be sent to server device
 *
 * @return int
 */
int oam_server_over_mpls_tp_pwe_example(
    int server_unit,
    int client_unit,
    int server_port1,
    int server_port2,
    int client_port1,
    int client_port2,
    bcm_oam_endpoint_type_t type,
    int is_server_client)
{
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int counter_if_0 = 0, outlif, label, out_gport;
    int server_gport, client_gport;
    int arp_out_lif_server, arp_out_lif_client;

    dnx_oam_ep_info_glb.port_1 = server_port1;
    dnx_oam_ep_info_glb.port_2 = server_port2;

    BCM_IF_ERROR_RETURN_MSG(mpls_oam_init(server_unit, type, &server_gport, &outlif, &arp_out_lif_server, &label, &out_gport, &tunnel_switch), "");

    dnx_oam_ep_info_glb.port_1 = client_port1;
    dnx_oam_ep_info_glb.port_2 = client_port2;

    BCM_IF_ERROR_RETURN_MSG(mpls_oam_init(client_unit, type, &client_gport, &outlif, &arp_out_lif_client, &label, &out_gport, &tunnel_switch), "");

    /*
     * Allocate counter engine and get counter
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(client_unit, client_port1, counter_if_0, 1, &dnx_oam_server_client_endpoint_info_glb.counter_base_id_downmep), "");

    /* OAM Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client);
    bcm_oam_group_info_t_init(&dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server);
    if (dnx_oam_server_client_endpoint_info_glb.maid_type) {
        if(dnx_oam_server_client_endpoint_info_glb.maid_type == 2)
        {
            /** necessary only for server unit - client group is only for passing validation */
            for(index = 0; index < 48; index++)
            {
                dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name[index] = 150 - index;
            }
            dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
            dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.group_name_index = 0x10098;
        }
        else
        {
            sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
     } else {
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
        sal_memcpy(dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server.name, dnx_oam_server_client_endpoint_info_glb.short_name, BCM_OAM_GROUP_NAME_LENGTH);
    }

    /* The server side group is used to determine the MAID on CCMs, both those transmitted and the expected MAID on recieved CCMs.
       This is conveyed through the name field.*/
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(server_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_server), "");
    /* The client side is only used for logically clustering endpoint together. Recieved/Transmitted packets are not affected by this*/
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(client_unit, &dnx_oam_server_client_endpoint_info_glb.group_info_short_ma_client), "");

    /** Create traps that connect the two devices */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_create_traps(server_unit, client_unit), "");

    /* Set the server endpoint */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_set_mpls_server_endpoint(server_unit, client_unit, server_port1, server_port2,
            client_port1, client_port2, type, is_server_client, outlif, label, server_gport), "");

    /* Set the client endpoint */
    /* It is the user's responsibility to verify that the fields of the server and client device match. */
    BCM_IF_ERROR_RETURN_MSG(oamp_server_set_mpls_client_endpoint(server_unit, client_unit, server_port1,
            server_port2, client_port1, client_port2, type, server_gport), "");

    return BCM_E_NONE;
}
