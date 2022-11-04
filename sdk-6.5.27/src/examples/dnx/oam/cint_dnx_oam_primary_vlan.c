/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * File: cint_dnx_oam_primary_vlan.c
 * Purpose: Example of OAM primary VLAN basic configuration.
 */

/*
 *
 * This cint provides a basic OAM primary VLAN example by creating p2p vswitch on 2 given ports
 * and the following endpoints:
 * 1) Non accelerated down MEP on port1
 * 2) Accelerated up MEP on port2
 * 3)Accelerated up MEP for primary VLAN
 * 4) Accelerated down MEP on port1
 * 5) Accelerated down MEP for primary VLAN
 *
 * How to run:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_oam.c
  cint ../../../../src/examples/dnx/vswitch/cint_dnx_vswitch_cross_connect_p2p.c
  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
  cint ../../../../src/examples/dnx/oam/cint_dnx_oam_primary_vlan.c
  cint ../../../../src/examples/dnx/field/cint_field_oam_primary_vlan.c
  cint
  int rv =0;
  int unit =0;
  int port1 = 200;
  int port2 = 201;
  int primary_vlan = 20;
  int gport200 = 0;
  int gport201 = 0;
  is_oam_primary_vlan = 1;
  rv = cross_connect(unit, port1, &gport200, port2, &gport201, vlan);
  gport1 = gport200;
  gport2 = gport201;
  dnx_oam_primary_vlan_info_glb.port_1 = port1;
  dnx_oam_primary_vlan_info_glb.port_2 = port2;
  dnx_oam_primary_vlan_info_glb.match_vlan = vlan;
  dnx_oam_primary_vlan_info_glb.primary_vlan = primary_vlan;
  rv = oam_primary_vlan_example(unit);
 * Test Scenario - end
 *
 * This cint uses cint_dnx_vswitch_cross_connect_p2p.c to build the p2p vswitch.
 */

bcm_gport_t gport1, gport2; /* these are the ports created by the vswitch*/

/*
 * OAM endpoint configuration parameters
 */
struct dnx_oam_ep_gobal_config_s
{
  bcm_oam_endpoint_t down_mep_id;
  bcm_oam_endpoint_t up_mep_id;

  /* MAC addresses , ports & gports*/
  bcm_mac_t mac_mep_1;
  bcm_mac_t mac_mep_2;
  bcm_mac_t mac_mep_3;
  bcm_mac_t mac_mep_4;
  bcm_mac_t src_mac_mep_2;
  bcm_mac_t src_mac_mep_3;
  bcm_mac_t src_mac_mep_4;

  /*1-const; 32-long format; 13-length; all the rest - MA name*/
  uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH];

 /* physical port (signal generator)*/
  int port_1;
  int port_2;

  int use_port_interface_status;
  /* Enable LOC events as soon as the remote endpoints are created */
  int remote_meps_start_in_loc_enable;

  /* match VLAN and primary VLAN */
  bcm_vlan_t match_vlan;
  bcm_vlan_t primary_vlan;
};

/* Endpoint info */
struct oam__ep_info_s {
    int id;
    int rmep_id;
    bcm_gport_t gport;
    int lm_counter_base_id;
};

/*
 * Structure for returned information
 */
struct dnx_oam_results_count_s
{
  /* Group structures */
  bcm_oam_group_info_t group_info_gl;

  /* Created endpoints information */
  oam__ep_info_s ep1;
  oam__ep_info_s ep2;
  oam__ep_info_s ep3;
  oam__ep_info_s ep4;

  /* Created primary object information */
  int up_mep_gport;
  int down_mep_gport;
  oam__ep_info_s ep_oam_primary_vlan_up;
  oam__ep_info_s ep_oam_primary_vlan_down;
};


/*
 * OAM endpoint init configuration
 */
dnx_oam_ep_gobal_config_s dnx_oam_primary_vlan_info_glb = {
 /* down_mep_id */
 0,
 /* up_mep_id */
 0,
 /* mac addresses */
 {0x00, 0x00, 0x00, 0x01, 0x02, 0x03},
 {0x00, 0x00, 0x00, 0xff, 0xff, 0xff},
 {0x00, 0x00, 0x00, 0x01, 0x02, 0xff},
 {0x00, 0x00, 0x00, 0xff, 0xff, 0xfd},
 {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},
 {0x00, 0x01, 0x02, 0x03, 0x04, 0x01},
 {0x00, 0x01, 0x02, 0x03, 0x04, 0x07},
 /* Long Name: 1-const; 32-long format; 13-length; all the rest - MA name*/
 {1, 32, 13, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0xa, 0xb, 0xc, 0xd},
 /* physical ports port_1, port_2*/
 13,14,
 /* use_port_interface_status */
 0,
 /* remote_meps_start_in_loc_enable */
 0,
 /* match VLAN and primary VLAN */
 0,
 0
};

/*
 * Returned information
 */
dnx_oam_results_count_s dnx_oam_primary_vlan_results_glb = {
 /* Group structures */
 { 0 },
 /* Created endpoints information */
 { 0 },
 { 0 },
 { 0 },
 { 0 },
 /* Created primary object information */
 0,
 0,
 { 0 },
 { 0 }
};

/*****************************************************************************/
/*                                        OAM BASIC EXAMPLE                                                                               */
/*****************************************************************************/


/**
 * Basic OAM example. creates the following entities:
 * 1) vswitch on which OAM endpoints are defined.
 * 2) OAM group with short MA name (set on outgoing CCMs for
 * accelerated endpoints).
 * 3) Default endpoint.
 * 4) Non accelerated endpoint
 * 5) Accelerated down MEP
 * 6) Accelerated up MEP
 *
 * @param unit
 *
 * @return int
 */
int oam_primary_vlan_example(int unit) {
    bcm_oam_group_info_t *group_info;
    bcm_oam_endpoint_info_t mep_not_acc_info;
    bcm_oam_endpoint_info_t mep_acc_info;
    bcm_oam_endpoint_info_t rmep_info;
    bcm_oam_endpoint_info_t default_info;
    bcm_oam_primary_vlan_info_t primary_vlan_info;

    int md_level_1 = 4;
    int md_level_2 = 5;
    int md_level_3 = 2;
    int md_level_4 = 1;
    int lm_counter_base_id_1;
    int lm_counter_base_id_2;
    int lm_counter_base_id_3;
    int lm_counter_base_id_4;
    int counter_if_0 = 0;
    int counter_if_1 = 1;

    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;

    printf("Creating OAM group \n");
    bcm_oam_group_info_t_init(&dnx_oam_primary_vlan_results_glb.group_info_gl);
    sal_memcpy(dnx_oam_primary_vlan_results_glb.group_info_gl.name, dnx_oam_primary_vlan_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_primary_vlan_results_glb.group_info_gl), "");

    /*
     * Allocate counter engines and get counters. The counter_if is used for
     * retrieving counters on different engines of the same port. For each
     * counter interface, OAM CRPS database should be configured first on the
     * core that the port belongs to. Counter engines will be added to the database.
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_primary_vlan_info_glb.port_1, counter_if_0, 1, &lm_counter_base_id_1), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_primary_vlan_info_glb.port_1, counter_if_1, 1, &lm_counter_base_id_2), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_primary_vlan_info_glb.port_2, counter_if_0, 1, &lm_counter_base_id_3), "");

    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_primary_vlan_info_glb.port_2, counter_if_1, 1, &lm_counter_base_id_4), "");

    /* Set the used group for the MEP */
    group_info = &dnx_oam_primary_vlan_results_glb.group_info_gl;

  /*
   * Get default profile encoded values
   * Default profile is 1 and
   * configured on init(application) stage.
   */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile),
        "(accelerated ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile),
        "(accelerated egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile), "");

    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_primary_vlan_info_glb.port_1,  dnx_oam_primary_vlan_info_glb.mac_mep_1), "");

    /*
     * Adding non acc MEP
     */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = group_info->id;
    mep_not_acc_info.level = md_level_1;
    mep_not_acc_info.gport = gport1;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
    mep_not_acc_info.lm_counter_if = counter_if_0;

    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_not_acc_info), "");
    printf("created MEP with id %d\n", mep_not_acc_info.id);
    dnx_oam_primary_vlan_results_glb.ep1.gport = mep_not_acc_info.gport;
    dnx_oam_primary_vlan_results_glb.ep1.id = mep_not_acc_info.id;
    dnx_oam_primary_vlan_results_glb.ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;

    /*
    * Adding acc MEP - upmep
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_2;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
    mep_acc_info.name = 123;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 5;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.sampling_ratio = 0;

    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_primary_vlan_info_glb.src_mac_mep_2, 6);

    /* Setting My-CFM-MAC */
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_primary_vlan_info_glb.port_2, dnx_oam_primary_vlan_info_glb.mac_mep_2), "");

    mep_acc_info.acc_profile_id = acc_egress_profile;

    /*RX*/
    mep_acc_info.gport = gport2;
    mep_acc_info.lm_counter_base_id = lm_counter_base_id_3;
    mep_acc_info.lm_counter_if = counter_if_0;

    if (dnx_oam_primary_vlan_info_glb.up_mep_id)
    {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = dnx_oam_primary_vlan_info_glb.up_mep_id;
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");

    dnx_oam_primary_vlan_results_glb.ep2.gport = mep_acc_info.gport;
    dnx_oam_primary_vlan_results_glb.ep2.id = mep_acc_info.id;
    dnx_oam_primary_vlan_results_glb.ep2.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

    /*
    * Adding Remote MEP
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0xff;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        rmep_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
    }

    if (dnx_oam_primary_vlan_info_glb.remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;

        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
    printf("created RMEP with id %d\n", rmep_info.id);

    dnx_oam_primary_vlan_results_glb.ep2.rmep_id = rmep_info.id;

     /*
    * Adding acc MEP - upmep for primary VLAN
    */

    bcm_oam_primary_vlan_info_t_init(&primary_vlan_info);
    primary_vlan_info.flags |= BCM_OAM_PRIMARY_VLAN_UP_FACING;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_primary_vlan_create(unit, &primary_vlan_info), "");
    dnx_oam_primary_vlan_results_glb.up_mep_gport = primary_vlan_info.primary_vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, dnx_oam_primary_vlan_results_glb.up_mep_gport, BCM_OAM_PROFILE_INVALID, egress_profile), "");

    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_2;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
    mep_acc_info.name = 321;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 5;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.sampling_ratio = 0;

    /* The MAC address that the CCM packets are sent with */
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_primary_vlan_info_glb.src_mac_mep_2, 6);

    mep_acc_info.acc_profile_id = acc_egress_profile;

    /*RX*/
    mep_acc_info.gport = dnx_oam_primary_vlan_results_glb.up_mep_gport;
    mep_acc_info.lm_counter_base_id = 0;
    mep_acc_info.lm_counter_if = 0;

    if (dnx_oam_primary_vlan_info_glb.up_mep_id)
    {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = dnx_oam_primary_vlan_info_glb.up_mep_id;
    }

    printf("bcm_oam_endpoint_create() acc upmep for primary VLAN\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");


    BCM_IF_ERROR_RETURN_MSG(cint_field_oam_primary_vlan_main(unit, dnx_oam_primary_vlan_results_glb.up_mep_gport, gport2, dnx_oam_primary_vlan_info_glb.primary_vlan, 1), "");

        /*
        * Adding Remote MEP for primary VLAN upmep
        */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0xff;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;

    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        rmep_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
    }

    if (dnx_oam_primary_vlan_info_glb.remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
    printf("created RMEP with id %d\n", rmep_info.id);

    dnx_oam_primary_vlan_results_glb.ep_oam_primary_vlan_up.rmep_id = rmep_info.id;


   /*
    * Adding acc MEP - downmep
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_3;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, dnx_oam_primary_vlan_info_glb.port_1);
    mep_acc_info.name = 456;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 10;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.int_pri = 1 + (3<<2);

    if (dnx_oam_primary_vlan_info_glb.down_mep_id) {
        mep_acc_info.flags = BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = dnx_oam_primary_vlan_info_glb.down_mep_id;
    }

    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_primary_vlan_info_glb.src_mac_mep_3, 6);

    /*RX*/
    mep_acc_info.gport = gport1;

    mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;
    mep_acc_info.lm_counter_if = counter_if_1;

    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_primary_vlan_info_glb.port_1, dnx_oam_primary_vlan_info_glb.mac_mep_3), "");

    mep_acc_info.acc_profile_id = acc_ingress_profile;

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");
    printf("created MEP with id %d\n", mep_acc_info.id);
    dnx_oam_primary_vlan_results_glb.ep3.gport = mep_acc_info.gport;
    dnx_oam_primary_vlan_results_glb.ep3.id = mep_acc_info.id;
    dnx_oam_primary_vlan_results_glb.ep3.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

    /*
    * Adding Remote MEP
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0x11;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    if (dnx_oam_primary_vlan_info_glb.remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;

        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
    printf("created RMEP with id %d\n", rmep_info.id);

    dnx_oam_primary_vlan_results_glb.ep3.rmep_id = rmep_info.id;

    /*
       * Adding acc MEP - downmep for primary VLAN
       */
    bcm_oam_primary_vlan_info_t_init(&primary_vlan_info);
    primary_vlan_info.criteria = BCM_OAM_PRIMARY_VLAN_MATCH_PORT_VLAN_VLAN;
    primary_vlan_info.match_port = dnx_oam_primary_vlan_info_glb.port_1;
    primary_vlan_info.match_vlan = dnx_oam_primary_vlan_info_glb.match_vlan;
    primary_vlan_info.match_inner_vlan = dnx_oam_primary_vlan_info_glb.primary_vlan;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_primary_vlan_create(unit, &primary_vlan_info), "");
    dnx_oam_primary_vlan_results_glb.down_mep_gport= primary_vlan_info.primary_vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, dnx_oam_primary_vlan_results_glb.down_mep_gport, ingress_profile, BCM_OAM_PROFILE_INVALID), "");

    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_3;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, dnx_oam_primary_vlan_info_glb.port_1);
    mep_acc_info.name = 654;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 10;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = dnx_oam_primary_vlan_info_glb.primary_vlan;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0x9100;
    mep_acc_info.int_pri = 1 + (3<<2);

    if (dnx_oam_primary_vlan_info_glb.down_mep_id) {
        mep_acc_info.flags = BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = dnx_oam_primary_vlan_info_glb.down_mep_id;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_primary_vlan_info_glb.src_mac_mep_3, 6);

    /* RX */
    mep_acc_info.gport = dnx_oam_primary_vlan_results_glb.down_mep_gport;

    mep_acc_info.lm_counter_base_id = 0;
    mep_acc_info.lm_counter_if = 0;

    mep_acc_info.acc_profile_id = acc_ingress_profile;

    printf("bcm_oam_endpoint_create() acc downmep for primary VLAN\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");
    printf("created MEP with id %d\n", mep_acc_info.id);

        /*
        * Adding Remote MEP for primary VLAN downmep
        */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0x11;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    if (dnx_oam_primary_vlan_info_glb.remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;

        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
    printf("created RMEP with id %d\n", rmep_info.id);

    dnx_oam_primary_vlan_results_glb.ep_oam_primary_vlan_down.rmep_id = rmep_info.id;

    if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
        /*
        * Adding acc MEP - downmep
        */

        bcm_oam_endpoint_info_t_init(&mep_acc_info);

        /*TX*/
        mep_acc_info.type = bcmOAMEndpointTypeEthernet;
        mep_acc_info.group = group_info->id;
        mep_acc_info.level = md_level_4;
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, dnx_oam_primary_vlan_info_glb.port_1);
        mep_acc_info.name = 789;
        mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
        mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

        mep_acc_info.vlan = 10;
        mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
        mep_acc_info.outer_tpid = 0x8100;
        mep_acc_info.inner_vlan = 0;
        mep_acc_info.inner_pkt_pri = 0;
        mep_acc_info.inner_tpid = 0;
        mep_acc_info.int_pri = 1 + (3<<2);

        if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
            mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
            mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }

        /* The MAC address that the CCM packets are sent with*/
        sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_primary_vlan_info_glb.src_mac_mep_4, 6);

        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_primary_vlan_info_glb.port_2, dnx_oam_primary_vlan_info_glb.mac_mep_4), "");

        mep_acc_info.acc_profile_id = acc_ingress_profile;

        /*RX*/
        mep_acc_info.gport = gport2;
        mep_acc_info.lm_counter_base_id = lm_counter_base_id_4;
        mep_acc_info.lm_counter_if = counter_if_1;

        printf("bcm_oam_endpoint_create mep_acc_info\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");

        printf("created MEP with id %d\n", mep_acc_info.id);
        dnx_oam_primary_vlan_results_glb.ep4.gport = mep_acc_info.gport;
        dnx_oam_primary_vlan_results_glb.ep4.id = mep_acc_info.id;
        dnx_oam_primary_vlan_results_glb.ep4.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

        /*
        * Adding Remote MEP
        */
        bcm_oam_endpoint_info_t_init(&rmep_info);
        rmep_info.name = 0x12;
        rmep_info.local_id = mep_acc_info.id;
        rmep_info.ccm_period = 0;
        rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
        rmep_info.loc_clear_threshold = 1;
        rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        rmep_info.id = mep_acc_info.id;
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        if (dnx_oam_primary_vlan_info_glb.use_port_interface_status) {
            rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
            rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
        }

        if (dnx_oam_primary_vlan_info_glb.remote_meps_start_in_loc_enable) {
            rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        }

        printf("bcm_oam_endpoint_create RMEP\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
        printf("created RMEP with id %d\n", rmep_info.id);

        dnx_oam_primary_vlan_results_glb.ep4.rmep_id = rmep_info.id;
    }
    return BCM_E_NONE;
}

