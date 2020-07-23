/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_oam_server.c
 * Purpose: Example of using OAMP server
 *
 * Usage:
 *
 * cd
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_utils_oam.c
 * cint cint_multi_device_utils.c
 * cint cint_queue_tests.c
 * cd ../sand
 * cint cint_sand_oam_server.c
 *
 * cint
 * int client_unit=0, server_unit=1;
 * print oamp_server_example(server_unit, client_unit, 200, 201, 200, 201);
 *
 *
 * Server endpoints over MPLS-TP for DNX family:
 *
 * cd ../../../../src/examples
 * cint sand/utility/cint_sand_utils_global.c
 * cint sand/utility/cint_sand_utils_l3.c 
 * cint sand/utility/cint_sand_utils_oam.c 
 * cint sand/utility/cint_sand_utils_mpls.c 
 * cint sand/utility/cint_sand_utils_mpls_port.c
 * cint dnx/crps/cint_crps_oam_config.c 
 * cint dnx/oam/cint_oam_basic.c
 * cint dnx/utility/cint_dnx_utility_mpls.c
 * cint sand/cint_sand_oam_y1731_over_tunnel.c
 * cint sand/cint_sand_oam_server.c
 * cint
 * int client_unit=2, server_unit=1;
 *
 *
 * ********** Set to 1 to test server/client - client EP on server device ***************
 * int is_server_client=0;
 * 
 * 
 *
 * print oam_server_over_mpls_tp_pwe_example(server_unit, client_unit, 200, 201, 200, 201, bcmOAMEndpointTypeBHHMPLS, is_server_client);
 *
 * Server emdpoints over MPLS section for DNX family:
 *
 * cd ../../../../src/examples
 * cint sand/utility/cint_sand_utils_global.c
 * cint sand/utility/cint_sand_utils_l3.c 
 * cint sand/utility/cint_sand_utils_oam.c 
 * cint sand/utility/cint_sand_utils_mpls.c 
 * cint sand/utility/cint_sand_utils_mpls_port.c
 * cint sand/cint_ip_route_basic.c
 * cint sand/cint_vpls_mp_basic.c
 * cint dnx/crps/cint_crps_oam_config.c 
 * cint dnx/oam/cint_oam_basic.c
 * cint dnx/utility/cint_dnx_utility_mpls.c
 * cint sand/cint_sand_oam_y1731_over_tunnel.c
 * cint sand/cint_sand_oam_server.c
 * cint
 * int client_unit=2, server_unit=1;
 *
 *
 * ********** Set to 1 to test server/client - client EP on server device ***************
 * int is_server_client=0;
 * 
 * 
 * print oam_server_over_mpls_tp_pwe_example(server_unit, client_unit, 200, 201, 200, 201, bcmOAMEndpointTypeBhhSection, is_server_client);
 *
 * Server endpoints over PWE for DNX family:
 *
 * cd ../../../../src/examples
 * cint sand/utility/cint_sand_utils_global.c
 * cint sand/utility/cint_sand_utils_l3.c 
 * cint sand/utility/cint_sand_utils_oam.c 
 * cint sand/utility/cint_sand_utils_mpls.c 
 * cint sand/utility/cint_sand_utils_mpls_port.c
 * cint dnx/utility/cint_dnx_utility_vpls_advanced.c
 * cint dnx/crps/cint_crps_oam_config.c 
 * cint dnx/oam/cint_oam_basic.c
 * cint dnx/utility/cint_dnx_utility_mpls.c
 * cint sand/cint_sand_oam_y1731_over_tunnel.c
 * cint sand/cint_sand_oam_server.c
 * cint
 * int client_unit=2, server_unit=1;
 *
 *
 * ********** Set to 1 to test server/client - client EP on server device ***************
 * int is_server_client=0;
 * 
 * 
 * print oam_server_over_mpls_tp_pwe_example(server_unit, client_unit, 200, 201, 200, 201, bcmOAMEndpointTypeBHHPwe, is_server_client);
 *
 * Same sequence for PWE-GAL, except the last line:
 * print oam_server_over_mpls_tp_pwe_example(server_unit, client_unit, 200, 201, 200, 201, bcmOAMEndpointTypeBHHPweGAL, is_server_client); 
 */

/* Globals */
  bcm_mac_t mac_mep_1 = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
  bcm_mac_t mac_mep_2 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
  bcm_mac_t src_mac = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  bcm_mac_t src_mac_1 = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xfe};
  /*1-const; 3-short format; 2-length; all the rest - MA name*/
  uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
  /*4-const; 3-MD-Length; MD-Name+Zero-padding; 2-const; MA-Length; MA-name*/
  uint8 str_11b_name[BCM_OAM_GROUP_NAME_LENGTH] = {4, 3, 'A', 'T', 'T', 00, 00, 02, 04, 'E', 'V', 'C', '1', 0, 0};
  uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 'A', 'T', 'T', 00, 00, 02, 04, 'E', 'V', 'C', '1', 0};

  bcm_vlan_port_t server_vp1, server_vp2, client_vp1, client_vp2;
  bcm_vswitch_cross_connect_t server_cross_connect, client_cross_connect;

  bcm_oam_endpoint_info_t ep_server_down, ep_server_up, ep_client_down, ep_client_up, rmep;
  bcm_oam_endpoint_info_t ep_server_mpls, ep_client_mpls;
  int server_oamp_port = 232; /* proper apllication must be used so that this will actually be configured as the server OAMP port */
  int recycle_port_core0 = 42;
  int recycle_port_core1 = 43; /* Jericho: up MEP port and recycle port should match (in unit scope) */
  int recycle_port_core0_jr2 = 221;
  int recycle_port_core1_jr2 = 222;
  /*
   * For the JR2-J2C device, recycle port is port 41.
   * This value was set in the SOC files.  See
   * $SDK/rc/systems/multi-dev-jr2-comp-j2c-mode.soc
   * and
   * $SDK/rc/systems/multi-dev-j2c-comp-jr2-mode.soc
   */
  int recycle_port_core0_j2c_comp_jr2 = 41;

  bcm_oam_group_info_t group_info_short_ma_client, group_info_short_ma_server;
  int counter_base_id_upmep, counter_base_id_downmep;
  bcm_rx_trap_config_t trap_to_server_oamp;

  int trap_code, trap_code_up, trap_code_up_ing;
  int down_mdl = 4;
  int up_mdl = 5;
  /*
   * Type of MAID: different for JR2 and above
   * For JR2 and above: 0 - short MAID, 1 - "long" MAID, 2 - 48B MAID.
   * Otherwise: 0 - short MAID, 1 - 11b MAID
   */
  int maid_type = 0;

  /* Created endpoints information */
  oam__ep_info_s server_upmep;
  oam__ep_info_s server_downmep;
  oam__ep_info_s client_upmep;
  oam__ep_info_s client_downmep;
  oam__ep_info_s server_mpls_mep;
  oam__ep_info_s client_mpls_mep;
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
    int rv;

    /* Set port classification ID */
    rv = bcm_port_class_set(server_unit, server_port1, bcmPortClassId, server_port1); 
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* Set port classification ID */
    rv = bcm_port_class_set(server_unit, server_port2, bcmPortClassId, server_port2); 
        if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* Set port classification ID */
    rv = bcm_port_class_set(client_unit, client_port1, bcmPortClassId, client_port1);
        if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* Set port classification ID */
    rv = bcm_port_class_set(client_unit, client_port2, bcmPortClassId, client_port2);
        if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Server VLAN port for the down MEP */
    server_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    if(is_device_or_above(server_unit, JERICHO2))
    {
        bcm_vlan_port_t_init(&server_vp1);
        server_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
        server_vp1.flags = 0;
    }
    server_vp1.port = server_port1;
    server_vp1.match_vlan = 11;
    server_vp1.egress_vlan = is_device_or_above(server_unit, JERICHO2) ? 0 : 11;
    rv = bcm_vlan_port_create(server_unit,&server_vp1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("Vlan-port created on server (unit %d) with id %d\n", server_unit, server_vp1.vlan_port_id);

    if(is_device_or_above(server_unit, JERICHO2))
    {
        /** Adding vlan gports is necessary to avoid "not in VLAN" traps */
        rv = bcm_vlan_gport_add(server_unit, 11, server_port1, 0);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Server VLAN port for the up MEP */
    server_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    if(is_device_or_above(server_unit, JERICHO2))
    {
        bcm_vlan_port_t_init(&server_vp2);
        server_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
        server_vp2.flags = 0;
    }
    server_vp2.port = server_port2;
    server_vp2.match_vlan = 12;
    server_vp2.egress_vlan = is_device_or_above(server_unit, JERICHO2) ? 0 : 12;
    rv = bcm_vlan_port_create(server_unit,&server_vp2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("Vlan-port created on server (unit %d) with id %d\n", server_unit, server_vp2.vlan_port_id);


    if(is_device_or_above(server_unit, JERICHO2))
    {
        rv = bcm_vlan_gport_add(server_unit, 12, server_port2, 0);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
            return rv;
        }
    }
    /* Client VLAN port for the down MEP */
    client_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    if(is_device_or_above(client_unit, JERICHO2))
    {
        /* Set port classification ID */
        rv = bcm_port_class_set(client_unit, client_port1, bcmPortClassId, client_port1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        /* Set port classification ID */
        rv = bcm_port_class_set(client_unit, client_port2, bcmPortClassId, client_port2);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        bcm_vlan_port_t_init(&client_vp1);
        client_vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
        client_vp1.flags = 0;
    }
    client_vp1.port = client_port1;
    client_vp1.match_vlan = 21;
    client_vp1.egress_vlan = is_device_or_above(client_unit, JERICHO2) ? 0 : 21;
    rv = bcm_vlan_port_create(client_unit,&client_vp1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("Vlan-port created on client (unit %d) with id %d\n", client_unit, client_vp1.vlan_port_id);

    if(is_device_or_above(client_unit, JERICHO2))
    {
        rv = bcm_vlan_gport_add(client_unit, 21, client_port1, 0);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_vlan_gport_add(client_unit, 22, client_port1, 0);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
            return rv;
        }
    }


    /* Client VLAN port for the up MEP */
    client_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    if(is_device_or_above(client_unit, JERICHO2))
    {
        bcm_vlan_port_t_init(&client_vp2);
        client_vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
        client_vp2.flags = 0;
    }
    client_vp2.port = client_port2;
    client_vp2.match_vlan = 22;
    client_vp2.egress_vlan = is_device_or_above(client_unit, JERICHO2) ? 0 : 22;
    rv = bcm_vlan_port_create(client_unit,&client_vp2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("Vlan-port created on client (unit %d) with id %d\n", client_unit, client_vp2.vlan_port_id);

    if(is_device_or_above(client_unit, JERICHO2))
    {
        rv = bcm_vlan_gport_add(client_unit, 21, client_port2, 0);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_vlan_gport_add(client_unit, 22, client_port2, 0);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Up MEPs are being trapped in the egress
       In oreder to create supportive environment for this, the two created VLAN ports will be cross-connected
       Packets destined for the up MEP on vp2 will be transmitted to vp1 */
    bcm_vswitch_cross_connect_t_init(&server_cross_connect);
    server_cross_connect.port1 = server_vp1.vlan_port_id;
    server_cross_connect.port2 = server_vp2.vlan_port_id;

    rv = bcm_vswitch_cross_connect_add(server_unit, &server_cross_connect);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_vswitch_cross_connect_t_init(&client_cross_connect);
    client_cross_connect.port1 = client_vp1.vlan_port_id;
    client_cross_connect.port2 = client_vp2.vlan_port_id;

    rv = bcm_vswitch_cross_connect_add(client_unit, &client_cross_connect);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return 0;
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
    int rv;
    int recycle_port;
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
    bcm_oam_endpoint_info_t_init(ep_server_down);
    BCM_GPORT_TRAP_SET(ep_server_down.remote_gport, trap_code, 7, 0); /* Required for handling the traps coming from the client */
    rv = port_to_system_port(client_unit, client_port1, &ep_server_down.tx_gport); /* Set the transmission port to be the down MEP's port on the client */
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    sal_memcpy(ep_server_down.src_mac_address, src_mac, 6);
    if(is_device_or_above(server_unit, JERICHO2))
    {
        ep_server_down.gport = BCM_GPORT_INVALID;
        ep_server_down.acc_profile_id = BCM_OAM_PROFILE_INVALID;

        if(is_server_client)
        {
            /* Setting My-CFM-MAC (for Jericho2 Only)*/
            rv = oam__my_cfm_mac_set(server_unit, server_port1, mac_mep_1);
            if (rv != BCM_E_NONE) {
                printf("oam__my_cfm_mac_set fail \n");
                return rv;
            }
            rv = bcm_oam_profile_id_get_by_type(server_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
            if (rv != BCM_E_NONE) {
              printf("bcm_oam_profile_id_get_by_type(ingress) \n");
              return rv;
            }

            rv = bcm_oam_profile_id_get_by_type(server_unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
            if (rv != BCM_E_NONE) {
              printf("bcm_oam_profile_id_get_by_type(egress)\n");
              return rv;
            }

            rv = bcm_oam_profile_id_get_by_type(server_unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
            if (rv != BCM_E_NONE) {
              printf("bcm_oam_profile_id_get_by_type(accelerated ingress) \n");
              return rv;
            }

            rv = bcm_oam_profile_id_get_by_type(server_unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
            if (rv != BCM_E_NONE) {
              printf("bcm_oam_profile_id_get_by_type(accelerated egress) \n");
              return rv;
            }

            rv = bcm_oam_lif_profile_set(server_unit, 0, server_vp1.vlan_port_id, ingress_profile, egress_profile);
            if (rv != BCM_E_NONE) {
              printf("bcm_oam_lif_profile_set\n");
              return rv;
            }
            rv = bcm_oam_lif_profile_set(server_unit, 0, server_vp2.vlan_port_id, ingress_profile, egress_profile);
            if (rv != BCM_E_NONE) {
              printf("bcm_oam_lif_profile_set\n");
              return rv;
            }
            bcm_oam_endpoint_info_t_init(ep_server_client_down);
            ep_server_client_down.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
            ep_server_client_down.acc_profile_id = acc_ingress_profile;
            ep_server_client_down.gport = server_vp1.vlan_port_id;
            ep_server_client_down.flags = BCM_OAM_ENDPOINT_WITH_ID;
            ep_server_client_down.type = bcmOAMEndpointTypeEthernet;
            ep_server_client_down.group = group_info_short_ma_client.id;
            ep_server_client_down.level = down_mdl;
            ep_server_client_down.lm_counter_base_id = counter_base_id_downmep;
        }
    }
    else
    {
        sal_memcpy(ep_server_down.dst_mac_address, mac_mep_1, 6);
        ep_server_down.gport = server_vp1.vlan_port_id; /*BCM_GPORT_INVALID*/
    }
    ep_server_down.type = bcmOAMEndpointTypeEthernet;
    ep_server_down.group = group_info_short_ma_server.id;
    ep_server_down.level = down_mdl;
    ep_server_down.name = 101;
    ep_server_down.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    if(!is_device_or_above(server_unit, JERICHO2))
    {
        ep_server_down.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    }
    ep_server_down.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    ep_server_down.vlan = 11;
    ep_server_down.outer_tpid = 0x8100;
    ep_server_down.lm_counter_base_id = counter_base_id_downmep;
    /* Adding TLV */
    ep_server_down.flags = BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
    ep_server_down.interface_state = BCM_OAM_INTERFACE_TLV_UP;

    rv = bcm_oam_endpoint_create(server_unit, &ep_server_down);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Down MEP created on server (unit %d) with id %d, tx_gport is: %d\n", server_unit, ep_server_down.id, ep_server_down.tx_gport);

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(rmep);
    rmep.name = 201;
    rmep.local_id = ep_server_down.id;
    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep.loc_clear_threshold = 1;
    rmep.id = ep_server_down.id;

    rv = bcm_oam_endpoint_create(server_unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, rmep.id);

    if(is_server_client)
    {
        ep_server_client_down.id = ep_server_down.id;
        rv = bcm_oam_endpoint_create(server_unit, &ep_server_client_down);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        printf("Server-client down-MEP created on server (unit %d) with id %d\n", server_unit, ep_server_client_down.id);
    }

    /* Store information on global struct */
    server_downmep.gport = ep_server_down.gport;
    server_downmep.id = ep_server_down.id;
    server_downmep.rmep_id = rmep.id;

    /* Setting the correct recycle port based on client's up MEP port core */
    if(is_device_or_above(client_unit, JERICHO2))
    {
        /** JR2 has different recycle ports */
        if(soc_property_get(client_unit, "system_headers_mode", 1) != 0)
        {
            recycle_port_core0 = recycle_port_core0_jr2;
            recycle_port_core1 = recycle_port_core1_jr2;
        }
        else
        {
            int client_multi_core = *(dnxc_data_get(client_unit, "device", "general", "multi_core", NULL));
            int server_multi_core = *(dnxc_data_get(server_unit, "device", "general", "multi_core", NULL));

            printf("client_multi_core = %d, server_multi_core = %d\n", client_multi_core, server_multi_core);

            if((client_multi_core == 0) != (server_multi_core == 0))
            {
                /** Special case: one device is single core, the other is dual core */
                recycle_port_core0 = recycle_port_core0_j2c_comp_jr2;
            }
        }
    }


    recycle_port = recycle_port_core0;
    if (is_device_or_above(client_unit,JERICHO)) {
        int core, tm_port;

        rv = get_core_and_tm_port_from_port(client_unit,client_port2,&core,&tm_port);
        if (rv != BCM_E_NONE){
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }

        if (core) {
            recycle_port = recycle_port_core1;
        }
    }

    /* Up-MEP */
    bcm_oam_endpoint_info_t_init(ep_server_up);

    if(is_device_or_above(client_unit, JERICHO2))
    {
        /**
         * remote_gport is used for reception: this trap
         * needs to be added to the server's OAMP trap
         * TCAM table, otherwise it will be punted
         */
        BCM_GPORT_TRAP_SET(ep_server_up.remote_gport, trap_code_up_ing, 7, 0);

        /**
         * The tx_gport will be placed in the OAMP generated ITMH.
         * In this case, the packet will be directed to the client's recycle port
         */
        rv = port_to_system_port(client_unit, recycle_port, &ep_server_up.tx_gport);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }
    else
    {
        rv = port_to_system_port(client_unit, recycle_port, &ep_server_up.remote_gport); /* Injected packets directed to client's recycle port */
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = port_to_system_port(client_unit, client_port2, &ep_server_up.tx_gport); /* After recycling, source port (on PTCH) will be the local client's port */
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

    }

    sal_memcpy(ep_server_up.src_mac_address, src_mac_1, 6);
    if(is_device_or_above(server_unit, JERICHO2))
    {
        /**
         * For up-MEP, PP port is usually written
         * to the OAMP_MEP_DB table.  Since server
         * cannot convert gport to PP port, valid
         * PP port must be provided from the client
         */

        /** First, get port */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = client_vp2.vlan_port_id;
        rv = bcm_vlan_port_find(client_unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_find fail \n");
            return rv;
        }

        /** Then, get PP port */
        rv = bcm_port_get(client_unit, vlan_port.port, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_get fail \n");
            return rv;
        }
        ep_server_up.gport = mapping_info.pp_port;
        ep_server_up.acc_profile_id = BCM_OAM_PROFILE_INVALID;
        if(is_server_client)
        {
            /** For server client, add client endpoint on server device */

            rv = bcm_vlan_gport_add(server_unit, 12, server_port1, 0);
            if (rv != BCM_E_NONE)
            {
                printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
                return rv;
            }

            bcm_oam_endpoint_info_t_init(ep_server_client_up);
            ep_server_client_up.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
            ep_server_client_up.acc_profile_id = acc_egress_profile;
            ep_server_client_up.gport = server_vp2.vlan_port_id;
            ep_server_client_up.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
            ep_server_client_up.type = bcmOAMEndpointTypeEthernet;
            ep_server_client_up.group = group_info_short_ma_client.id;
            ep_server_client_up.level = up_mdl;
            ep_server_client_up.lm_counter_base_id = counter_base_id_upmep;
        }
    }
    else
    {
        sal_memcpy(ep_server_up.dst_mac_address, mac_mep_2, 6);
        ep_server_up.gport = server_vp2.vlan_port_id;
    }
    ep_server_up.type = bcmOAMEndpointTypeEthernet;
    ep_server_up.group = group_info_short_ma_server.id;
    ep_server_up.level = up_mdl;
    ep_server_up.name = 102;
    ep_server_up.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    if(!is_device_or_above(server_unit, JERICHO2))
    {
        ep_server_up.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    }
    ep_server_up.flags = BCM_OAM_ENDPOINT_UP_FACING;
    ep_server_up.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    ep_server_up.vlan = 22; /* Should be according to tx_gport */
    ep_server_up.outer_tpid = 0x8100;
    ep_server_up.lm_counter_base_id = counter_base_id_upmep; /* For up-MEPs the counter base ID must be set for both the server and client MEPs */

    rv = bcm_oam_endpoint_create(server_unit, &ep_server_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Up MEP created on server (unit %d) with id %d, tx_gport is: %d, remote_gport is: %d\n", server_unit, ep_server_up.id, ep_server_up.tx_gport, ep_server_up.remote_gport);

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(rmep);
    rmep.name = 202;
    rmep.local_id = ep_server_up.id;
    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep.loc_clear_threshold = 1;
    rmep.id = ep_server_up.id;

    rv = bcm_oam_endpoint_create(server_unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, rmep.id);

    if(is_server_client)
    {
        ep_server_client_up.id = ep_server_up.id;
        rv = bcm_oam_endpoint_create(server_unit, &ep_server_client_up);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        printf("Server-client up-MEP created on server (unit %d) with id %d\n", server_unit, ep_server_client_up.id);
    }


    /* Store information on global struct */
    server_upmep.gport = ep_server_up.gport;
    server_upmep.id = ep_server_up.id;
    server_upmep.rmep_id = rmep.id;

    return 0;
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
    int rv;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;

    /* Down-MEP */
    bcm_oam_endpoint_info_t_init(ep_client_down);
    if(is_device_or_above(client_unit, JERICHO2))
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(client_unit, client_port1, mac_mep_1);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        /** Profile for trapping packets for down-MEP in client */
        rv = bcm_oam_profile_id_get_by_type(server_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(client_unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(egress)\n");
          return rv;
        }

        rv = bcm_oam_profile_create(client_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_profile_create.\n", rv);
            return rv;
        }

        action_key.dest_mac_type = bcmOAMDestMacTypeMcast;
        action_key.endpoint_type = bcmOAMMatchTypeMEP;
        action_key.flags = 0;
        action_key.inject = 0;
        action_key.opcode = bcmOamOpcodeCCM;

        action_result.counter_increment = 0;
        BCM_GPORT_TRAP_SET(action_result.destination, trap_code, 7, 0);
        action_result.flags = 0;
        action_result.meter_enable = 0;

        rv = bcm_oam_profile_action_set(client_unit, flags, acc_ingress_profile, &action_key, &action_result);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
            return rv;
        }

        rv = bcm_oam_lif_profile_set(client_unit, 0, client_vp1.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }

        /** Profile for trapping packets for up-MEP in client */
        rv = bcm_oam_profile_create(client_unit, flags, bcmOAMProfileEgressAcceleratedEndpoint, &acc_egress_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_profile_create.\n", rv);
            return rv;
        }

        BCM_GPORT_TRAP_SET(action_result.destination, trap_code_up, 7, 0);

        rv = bcm_oam_profile_action_set(client_unit, flags, acc_egress_profile, &action_key, &action_result);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
            return rv;
        }

        rv = bcm_oam_lif_profile_set(client_unit, 0, client_vp2.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
        ep_client_down.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
        ep_client_down.acc_profile_id = acc_ingress_profile;
    }
    else
    {
        BCM_GPORT_TRAP_SET(ep_client_down.remote_gport, trap_code, 7, 0);
        sal_memcpy(ep_client_down.dst_mac_address, mac_mep_1, 6);
    }
    ep_client_down.gport = client_vp1.vlan_port_id;
    ep_client_down.id = ep_server_down.id; /* The client endpoint must be set up with the ID of the server endpoint. */
    ep_client_down.flags = BCM_OAM_ENDPOINT_WITH_ID;
    ep_client_down.type = bcmOAMEndpointTypeEthernet;
    ep_client_down.group = group_info_short_ma_client.id;
    ep_client_down.level = down_mdl;
    if(!is_device_or_above(client_unit, JERICHO2))
    {
        ep_client_down.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    }
    ep_client_down.lm_counter_base_id = counter_base_id_downmep;

    rv = bcm_oam_endpoint_create(client_unit, &ep_client_down);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Down MEP created on client (unit %d) with id %d\n", client_unit, ep_client_down.id);

    /* Store information on global struct */
    client_downmep.gport = ep_client_down.gport;
    client_downmep.id = ep_client_down.id;

    /* Up-MEP*/
    bcm_oam_endpoint_info_t_init(ep_client_up);

    if(is_device_or_above(client_unit, JERICHO2))
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(client_unit, client_port2, mac_mep_2);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
        ep_client_up.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
        ep_client_up.acc_profile_id = acc_egress_profile;
    }
    else
    {
        BCM_GPORT_TRAP_SET(ep_client_up.remote_gport, trap_code_up, 7, 0); /* Client's EPs remote_gport field must be a trap */
        sal_memcpy(ep_client_up.dst_mac_address, mac_mep_2, 6);
    }
    ep_client_up.gport = client_vp2.vlan_port_id;
    ep_client_up.id = ep_server_up.id; /* The client endpoint must be set up with the ID of the server endpoint. */
    ep_client_up.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
    ep_client_up.type = bcmOAMEndpointTypeEthernet;
    ep_client_up.group = group_info_short_ma_client.id;
    ep_client_up.level = up_mdl;
    if(!is_device_or_above(client_unit, JERICHO2))
    {
        ep_client_up.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    }
    ep_client_up.lm_counter_base_id = counter_base_id_upmep;

    rv = bcm_oam_endpoint_create(client_unit, &ep_client_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Up MEP created on client (unit %d) with id %d\n", client_unit, ep_client_up.id);

    /* Store information on global struct */
    client_upmep.gport = ep_client_up.gport;
    client_upmep.id = ep_client_up.id;

    return 0;
}

/**
 * In JR2, we need to create a special ingress trap.
 * Similar to the default accelerated egress profile,
 * the egress trap will edit the FHEI to point to the
 * ingress trap.  Under the ingress trap, system headers
 * will not be modified.  The difference here is the
 * destination: server's OAMP instead of local
 */
int create_special_ingress_trap_for_upmep(int client_unit)
{
    int rv;
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_entry_id;

    trap_to_server_oamp.flags = BCM_RX_TRAP_UPDATE_DEST;
    rv = bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &trap_code_up_ing);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_set(client_unit, trap_code_up_ing, trap_to_server_oamp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code = %d \n", trap_code_up_ing);

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_No_Header", sizeof(context_info.name));
    rv = bcm_field_context_create(client_unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Changing new context SYS HDR profile to None (No SYS HDRs are added) */
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_arg = 0;
    context_param.param_val = bcmFieldSystemHeaderProfileNone;
    rv = bcm_field_context_param_set(client_unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_arg = 0;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset0, 0);
    /**Context that was created for iPMF1 is also created for iPMF2*/
    rv = bcm_field_context_param_set(client_unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Adding trap_code preselector for the context */
    presel_entry_id.presel_id = 3;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    presel_data.qual_data[0].qual_value = trap_code_up_ing;
    presel_data.qual_data[0].qual_mask = 0x1FF;

    rv = bcm_field_presel_set(client_unit, 0, &presel_entry_id, &presel_data);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    return 0;
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

    int rv;

    /* Configure a trap which updates the destination to be the OAMP of the server unit.
       This trap will be used by the client's down MEP */
    rv = bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_to_server_oamp);
    trap_to_server_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    rv = port_to_system_port(server_unit, server_oamp_port, &trap_to_server_oamp.dest_port);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if (is_device_or_above(server_unit, JERICHO2) && (*(dnxc_data_get(client_unit, "oam", "feature", "oam_offset_supported", NULL))))
    {
        /*
         * Have the trap set the offset in the system headers. Three options:
         * 1. In Jr2_A0 device this is done through the PMF (not trap)
         * 2. Jer1 mode system headers through forwarding_header
         * 3. Jr2 mode system headers through egress_forwarding_index
 */
        if (soc_property_get(client_unit, "system_headers_mode", 1) == 0)
        {
            trap_to_server_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_to_server_oamp.forwarding_header = 1;
        }
        else
        {
            trap_to_server_oamp.flags |= BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX;
            trap_to_server_oamp.egress_forwarding_index = 7;
        }
    }


    rv = bcm_rx_trap_set(client_unit, trap_code, trap_to_server_oamp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code = %d \n", trap_code);

    rv = bcm_rx_trap_type_create(server_unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_rx_trap_set(server_unit, trap_code, trap_to_server_oamp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    /* Configure another trap which updates the destination to be the OAMP of the server unit.
       This trap will be used by the client's up MEP */

    if(is_device_or_above(client_unit, JERICHO2))
    {
        /** Special trap for up-MEP - see function header */
        rv = create_special_ingress_trap_for_upmep(client_unit);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &trap_code_up);
    }
    else
    {
        rv = bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapOamUpMEP4, &trap_code_up);
    }
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
    }

    if(is_device_or_above(client_unit, JERICHO2))
    {
        /** In JR2, this trap should map to the ingress trap */
        bcm_rx_trap_config_t_init(&trap_to_server_oamp);
        BCM_GPORT_TRAP_SET(trap_to_server_oamp.cpu_trap_gport, trap_code_up_ing, 15, 0);
        trap_to_server_oamp.stamped_trap_code = trap_code_up_ing;
    }

    /* Using same trap configuration */
    rv = bcm_rx_trap_set(client_unit, trap_code_up, trap_to_server_oamp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code = %d \n", trap_code_up);

    return 0;
}

/**
 * An example of setting up the OAMP as a server.
 * Create an up MEP and a down MEP, each first on the server
 * side (transmitting and recieving CCMs), then on the client
 * side (trapping OAM PDUs)
 *
 * @author sinai (01/09/2014)
 * updated by avive (25/11/2015)
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
 */
int oamp_server_example(int server_unit, int client_unit, int server_port1, int server_port2, int client_port1, int client_port2) {
    int rv;
    int counter_if_0 = 0, counter_if_1 = 1;
    int index;

    if (!is_device_or_above(server_unit, ARAD_PLUS)) {
        printf("Server unit must be Arad+ or above\n");
        return 21;
    }

    /* Create two VLAN ports on each unit (client and server) for the MEPs */
    rv = oamp_server_create_vlan_ports(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if(is_device_or_above(server_unit, JERICHO2))
    {
        /*
         * Allocate counter engines, and get counters
         */
        rv = set_counter_resource(client_unit, client_port1, counter_if_0, 1, &counter_base_id_downmep);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(client_unit, client_port2, counter_if_1, 1, &counter_base_id_upmep);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }
    else
    {
        /* Prepare the counters */
        rv = set_counter_source_and_engines(client_unit,&counter_base_id_downmep,client_port1);
        BCM_IF_ERROR_RETURN(rv);
        rv = set_counter_source_and_engines(client_unit,&counter_base_id_upmep,client_port2);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* OAM Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&group_info_short_ma_client);
    bcm_oam_group_info_t_init(&group_info_short_ma_server);
    if (maid_type) {
        if(is_device_or_above(server_unit, JERICHO2))
        {
            if(maid_type == 2)
            {
                /** necessary only for server unit - client group is only for passing validation */
                for(index = 0; index < 48; index++)
                {
                    group_info_short_ma_server.name[index] = 150 - index;
                }
                group_info_short_ma_server.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
                group_info_short_ma_server.group_name_index = 0x10098;
            }
            else
            {
                sal_memcpy(group_info_short_ma_server.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
            }
            sal_memcpy(group_info_short_ma_client.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        else
        {
             /* Using long MAID - 11B */
            sal_memcpy(group_info_short_ma_client.name, str_11b_name, BCM_OAM_GROUP_NAME_LENGTH);
            sal_memcpy(group_info_short_ma_server.name, str_11b_name, BCM_OAM_GROUP_NAME_LENGTH);
        }
    } else {
        sal_memcpy(group_info_short_ma_client.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
        sal_memcpy(group_info_short_ma_server.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    }

    /* The server side group is used to determine the MAID on CCMs, both those transmitted and the expected MAID on recieved CCMs.
       This is conveyed through the name field.*/
    rv = bcm_oam_group_create(server_unit, &group_info_short_ma_server);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* The client side is only used for logically clustering endpoint together. Recieved/Transmitted packets are not affected by this*/
    rv = bcm_oam_group_create(client_unit, &group_info_short_ma_client);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /** Create traps that connect the two devices */
    rv = oamp_server_create_traps(server_unit, client_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Configure PMF to fix TLV offset on Arad+ */
    if (!is_device_or_above(server_unit, JERICHO) || (maid_type && !is_device_or_above(server_unit, JERICHO2))) {
        rv = field__prge_action_code_add_entry(server_unit, 1, trap_code, 232/*dest_port (OAMP PP port)*/,
                                               1, 3 /*prge_action_code_oam_end_tlv_wa*/);
    }

    /* Set the server endpoints */
    rv = oamp_server_set_server_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2, FALSE);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set the client endpoints */
    /* It is the user responsibility to verify that the fields of the server and client device match. */
    rv = oamp_server_set_client_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return 0;
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
int oamp_server_switch_upmep_tx_unit(int server_unit, int server_upmep_id, int new_tx_unit, int new_tx_port, int new_entering_vlan) {
    int rv;
    int recycle_port;
    bcm_vlan_port_t vlan_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags=0;

    /* Get the server up MEP */
    bcm_oam_endpoint_info_t_init(ep_server_up);
    rv = bcm_oam_endpoint_get(server_unit, server_upmep_id, &ep_server_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Setting the correct recycle port based on tx_unit's up MEP port core */
    recycle_port = recycle_port_core0;
    if (is_device_or_above(new_tx_unit,JERICHO)) {
        int core, tm_port;

        rv = get_core_and_tm_port_from_port(new_tx_unit,new_tx_port,&core,&tm_port);
        if (rv != BCM_E_NONE){
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }

        if (core) {
            recycle_port = recycle_port_core1;
        }
    }

    if(is_device_or_above(new_tx_unit, JERICHO2))
    {
        /** Tx gport is the recycle port of the client unit */
        rv = port_to_system_port(new_tx_unit, recycle_port, &ep_server_up.tx_gport); /* After recycling, source port (on PTCH) will be new_tx_gport*/
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /** Then, get PP port */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = server_vp2.vlan_port_id;
        rv = bcm_vlan_port_find(new_tx_unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_find fail \n");
            return rv;
        }

        rv = bcm_port_get(new_tx_unit, vlan_port.port, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_get fail \n");
            return rv;
        }
        ep_server_up.gport = mapping_info.pp_port;
    }
    else
    {
        /* Set the new remote_gport, tx_gport and vlan*/
        rv = port_to_system_port(new_tx_unit, recycle_port, &ep_server_up.remote_gport); /* Injected packets directed to new_remote_gport */
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = port_to_system_port(new_tx_unit, new_tx_port, &ep_server_up.tx_gport); /* After recycling, source port (on PTCH) will be new_tx_gport */
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }
    ep_server_up.vlan = new_entering_vlan; /* Should be according to tx_gport */

    /* Update the server up MEP endpoint */
    rv = bcm_oam_endpoint_destroy(server_unit, server_upmep_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_oam_endpoint_create(server_unit, &ep_server_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(rmep);
    rmep.name = 202;
    rmep.local_id = ep_server_up.id;
    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep.loc_clear_threshold = 1;
    rmep.id = ep_server_up.id;
    rv = bcm_oam_endpoint_create(server_unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    printf("Up MEP 0x%x on server unit %d will now transmit injected packets through unit %d, port %d, vlan %d\n", server_upmep_id, server_unit, new_tx_unit, new_tx_port, new_entering_vlan);

    return 0;
}

/*
 * Sets the key to be [OAM-Trap-code, OAMP-dest-port] and sets the value for that key
 * That value is used to identify packets trapped to the OAMP and apply different workarounds if necessary.
 * Relevant for Jericho1 only
 */
int field_oam_advanced_features(int unit) {
    int result;
    bcm_field_group_t grp_tcam = 1;
    int group_priority = 10;
    int qual_id = 1;
    int trap_id;
    int flags = 0;

    /*Get trap_id value for OAM trap*/
    result = bcm_rx_trap_type_get(unit,flags,bcmRxTrapOamEthAccelerated,&trap_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_rx_trap_type_get\n");
        return result;
    }

    /* Create field group for OAM EndTLV */
    result = field__prge_action_create_field_group(unit, group_priority, grp_tcam, qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_create_field_group\n");
        return result;
    }

    /* Set action code for the created group */
    printf("Adding entry to mark OAM packets for advances processing\n");
    /* Default trap, for up MEPs */
    result = field__prge_action_code_add_entry(unit, grp_tcam, trap_id, 232/*dest_port (OAMP PP port)*/,
                                               qual_id, 3 /*prge_action_code_oam_end_tlv_wa*/);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_add_entry\n");
        return result;
    }
    /* Configured trap (the one created in oamp_server_example), for down MEPs */
    result = field__prge_action_code_add_entry(unit, grp_tcam, trap_code, 232/*dest_port (OAMP PP port)*/,
                                               qual_id, 3 /*prge_action_code_oam_end_tlv_wa*/);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_add_entry\n");
        return result;
    }

    return result;
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

    int rv;
    int counter_if_0 = 0, counter_if_1 = 1;
    int index;

    /* Create two VLAN ports on each unit (client and server) for the MEPs */
    rv = oamp_server_create_vlan_ports(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /*
     * Allocate counter engines, and get counters
     */
    rv = set_counter_resource(client_unit, client_port1, counter_if_0, 1, &counter_base_id_downmep);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
    rv = set_counter_resource(client_unit, client_port2, counter_if_1, 1, &counter_base_id_upmep);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }

    /* OAM Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&group_info_short_ma_client);
    bcm_oam_group_info_t_init(&group_info_short_ma_server);
    if (maid_type) {
        if(maid_type == 2)
        {
            /** necessary only for server unit - client group is only for passing validation */
            for(index = 0; index < 48; index++)
            {
                group_info_short_ma_server.name[index] = 150 - index;
            }
            group_info_short_ma_server.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
            group_info_short_ma_server.group_name_index = 0x10098;
        }
        else
        {
            sal_memcpy(group_info_short_ma_server.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        sal_memcpy(group_info_short_ma_client.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
     } else {
        sal_memcpy(group_info_short_ma_client.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
        sal_memcpy(group_info_short_ma_server.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
     }

    rv = bcm_oam_group_create(server_unit, &group_info_short_ma_server);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_group_create(client_unit, &group_info_short_ma_client);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /** Create traps that connect the two devices */
    rv = oamp_server_create_traps(server_unit, client_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set the server endpoints */
    rv = oamp_server_set_server_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2, TRUE);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set the client endpoints */
    rv = oamp_server_set_client_endpoints(server_unit, client_unit, server_port1, server_port2, client_port1, client_port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return 0;
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
    int rv;
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

    rv = bcm_oam_mpls_tp_channel_type_tx_set(server_unit, bcmOamMplsTpChannelPweonoam, 0x8902);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_mpls_tp_channel_type_tx_set.\n", rv);
        return rv;
    }


    bcm_oam_endpoint_info_t_init(ep_server_mpls);
    BCM_GPORT_TRAP_SET(ep_server_mpls.remote_gport, trap_code, 7, 0); /* Required for handling the traps coming from the client */
    rv = port_to_system_port(client_unit, client_port1, &ep_server_mpls.tx_gport); /* Set the transmission port to be the MEP's port on the client */
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    ep_server_mpls.gport = BCM_GPORT_INVALID;
    ep_server_mpls.acc_profile_id = BCM_OAM_PROFILE_INVALID;
    ep_server_mpls.intf_id = outlif;

    if(is_server_client)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(server_unit, server_port1, mac_mep_1);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
        action_key.endpoint_type = bcmOAMMatchTypeMEP;
        action_key.flags = 0;
        action_key.inject = 0;
        action_key.opcode = bcmOamOpcodeCCM;

        action_result.counter_increment = 0;
        BCM_GPORT_TRAP_SET(action_result.destination, trap_code, 7, 0);
        action_result.flags = 0;
        action_result.meter_enable = 0;

        rv = bcm_oam_profile_id_get_by_type(server_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_create(server_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_profile_create.\n", rv);
            return rv;
        }

        rv = bcm_oam_profile_action_set(server_unit, flags, acc_ingress_profile, &action_key, &action_result);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
            return rv;
        }

        rv = bcm_oam_lif_profile_set(server_unit, 0, gport, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }

        bcm_oam_endpoint_info_t_init(ep_server_client_mpls);
        ep_server_client_mpls.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
        ep_server_client_mpls.acc_profile_id = acc_ingress_profile;
        ep_server_client_mpls.gport = gport;
        ep_server_client_mpls.flags = BCM_OAM_ENDPOINT_WITH_ID;
        ep_server_client_mpls.type = type;
        ep_server_client_mpls.group = group_info_short_ma_client.id;
        ep_server_client_mpls.level = 7;
        ep_server_client_mpls.lm_counter_base_id = counter_base_id_downmep;
    }
    ep_server_mpls.type = type;
    ep_server_mpls.group = group_info_short_ma_server.id;
    ep_server_mpls.level = 7;
    ep_server_mpls.name = 101;
    ep_server_mpls.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    ep_server_mpls.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    ep_server_mpls.lm_counter_base_id = counter_base_id_downmep;

    ep_server_mpls.egress_label.label = label;
    ep_server_mpls.egress_label.ttl = 0xa;
    ep_server_mpls.egress_label.exp = 1;


    rv = bcm_oam_endpoint_create(server_unit, &ep_server_mpls);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("MPLS MEP created on server (unit %d) with id %d, tx_gport is: %d\n", server_unit, ep_server_mpls.id, ep_server_mpls.tx_gport);

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(rmep);
    rmep.name = 201;
    rmep.local_id = ep_server_mpls.id;
    rmep.type = type;
    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep.loc_clear_threshold = 1;
    rmep.id = ep_server_mpls.id;

    rv = bcm_oam_endpoint_create(server_unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("RMEP created on server (unit %d) with id %d\n", server_unit, rmep.id);

    if(is_server_client)
    {
        ep_server_client_mpls.id = ep_server_mpls.id;
        rv = bcm_oam_endpoint_create(server_unit, &ep_server_client_mpls);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        printf("Server-client MPLS MEP created on server (unit %d) with id %d\n", server_unit, ep_server_client_mpls.id);
    }

    /* Store information on global struct */
    server_mpls_mep.gport = ep_server_mpls.gport;
    server_mpls_mep.id = ep_server_mpls.id;
    server_mpls_mep.rmep_id = rmep.id;
    
    return 0;
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
        int gport) {
    int rv;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = BCM_OAM_PROFILE_INVALID;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;

    bcm_oam_endpoint_info_t_init(ep_client_mpls);

    /* Setting My-CFM-MAC */
    rv = oam__my_cfm_mac_set(client_unit, client_port1, mac_mep_1);
    if (rv != BCM_E_NONE) {
        printf("oam__my_cfm_mac_set fail \n");
        return rv;
    }

    action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = bcmOamOpcodeCCM;

    action_result.counter_increment = 0;
    BCM_GPORT_TRAP_SET(action_result.destination, trap_code, 7, 0);
    action_result.flags = 0;
    action_result.meter_enable = 0;


    rv = bcm_oam_profile_id_get_by_type(client_unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_profile_id_get_by_type(ingress) \n");
      return rv;
    }

    if(type == bcmOAMEndpointTypeBhhSection) {
        flags |= BCM_OAM_PROFILE_TYPE_SECTION;
    }
    rv = bcm_oam_profile_create(client_unit, flags, bcmOAMProfileIngressAcceleratedEndpoint, &acc_ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_create.\n", rv);
        return rv;
    }

    rv = bcm_oam_profile_action_set(client_unit, 0, acc_ingress_profile, &action_key, &action_result);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
        return rv;
    }

    rv = bcm_oam_lif_profile_set(client_unit, 0, gport, ingress_profile, egress_profile);
    if (rv != BCM_E_NONE) {
      printf("bcm_oam_lif_profile_set\n");
      return rv;
    }

    ep_client_mpls.flags2 |= BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET;
    ep_client_mpls.acc_profile_id = acc_ingress_profile;

    ep_client_mpls.gport = gport;
    ep_client_mpls.id = ep_server_mpls.id; /* The client endpoint must be set up with the ID of the server endpoint. */
    ep_client_mpls.flags = BCM_OAM_ENDPOINT_WITH_ID;
    ep_client_mpls.type = type;
    ep_client_mpls.group = group_info_short_ma_client.id;
    ep_client_mpls.level = 7;
    ep_client_mpls.lm_counter_base_id = counter_base_id_downmep;

    rv = bcm_oam_endpoint_create(client_unit, &ep_client_mpls);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("MPLS MEP created on client (unit %d) with id %d\n", client_unit, ep_client_mpls.id);

    /* Store information on global struct */
    client_mpls_mep.gport = ep_client_mpls.gport;
    client_mpls_mep.id = ep_client_mpls.id;

    return 0;
}

/**
 * Create accelerated y.1731 OAM sever endpoint and client
 * of type MPLS-TP or PWE on multi-device.
 *
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
    int rv, counter_if_0 = 0, use_mpls_out_gport = 1, outlif, label, out_gport, mpls_termination_label_index_enable;
    int server_gport, client_gport;

    if(type == bcmOAMEndpointTypeBhhSection) {
        cint_vpls_basic_info.cw_present = 1;
        is_section_oam = 1;
    }

    port_1 = server_port1;
    port_2 = server_port2;
    rv = mpls_oam_init(server_unit, type, use_mpls_out_gport, &server_gport, &outlif, &label, &out_gport, &tunnel_switch, &mpls_termination_label_index_enable);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    port_1 = client_port1;
    port_2 = client_port2;
    rv = mpls_oam_init(client_unit, type, use_mpls_out_gport, &client_gport, &outlif, &label, &out_gport, &tunnel_switch, &mpls_termination_label_index_enable);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /*
     * Allocate counter engine and get counter
     */
    rv = set_counter_resource(client_unit, client_port1, counter_if_0, 1, &counter_base_id_downmep);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }

    /* OAM Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&group_info_short_ma_client);
    bcm_oam_group_info_t_init(&group_info_short_ma_server);
    sal_memcpy(group_info_short_ma_client.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(group_info_short_ma_server.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);

    rv = bcm_oam_group_create(server_unit, &group_info_short_ma_server);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* The client side is only used for logically clustering endpoint together. Received/Transmitted packets are not affected by this*/
    rv = bcm_oam_group_create(client_unit, &group_info_short_ma_client);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /** Create traps that connect the two devices */
    rv = oamp_server_create_traps(server_unit, client_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set the server endpoint */
    rv = oamp_server_set_mpls_server_endpoint(server_unit, client_unit, server_port1, server_port2,
            client_port1, client_port2, type, is_server_client, outlif, label, server_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set the client endpoint */
    /* It is the user's responsibility to verify that the fields of the server and client device match. */
    rv = oamp_server_set_mpls_client_endpoint(server_unit, client_unit, server_port1,
            server_port2, client_port1, client_port2, type, server_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
 
    return 0;
}
