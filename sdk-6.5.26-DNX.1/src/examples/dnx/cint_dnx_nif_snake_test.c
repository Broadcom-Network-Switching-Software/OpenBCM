/*
 * $Id: cint_dnx_nif_snake_test.c
 *
 * 1. Port snake test example
 * Test Scenario - start
 *
 * ./bcm.user
 * config delete ucode_port_*.BCM8880X
 * config add ucode_port_1.0=CDGE0:core_0.1
 * config add ucode_port_2.0=CDGE1:core_0.2
 * config add ucode_port_3.0=CDGE2:core_0.3
 * config add ucode_port_4.0=CDGE3:core_0.4
 * config add lane_to_serdes_map_nif_lane0.0=rx0:tx0
 * config add lane_to_serdes_map_nif_lane1.0=rx1:tx1
 * config add lane_to_serdes_map_nif_lane2.0=rx2:tx2
 * config add lane_to_serdes_map_nif_lane3.0=rx3:tx3
 * config add appl_enable_l2.0=0
 * 0: tr 141 NoInit=1
 * config add custom_feature_init_verify=1
 * config delete custom_feature_multithread_en*
 * config delete custom_feature_kbp_multithread_en*
 * 0: tr 141 NoDeinit=1
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
 * cint ../../../../src/examples/dnx/cint_dnx_nif_phy_loopback.c
 * cint ../../../../src/examples/dnx/cint_dnx_nif_snake_test.c
 * cint
 * cint_bcm_nif_snake_test(0,3,1);
 * exit;
 * Test Scenario - end
 *
 * 2. Run snake test on all default NIF ports
 * 2.1 Choose all NIF ports.
 * 2.2 Create several streams with different packet size, send to the 1st NIF port.
 * 2.3 Set PHY loopback for each NIF port.
 * 2.4 Add MACT entry to next port, replace VID using EVE.
 * 2.5 Last port, don't need to set EVE. only count tx counts
 * Test Scenario - start
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/cint_dnx_mact.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan.c
 * cint ../../../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
 * cint ../../../../src/examples/dnx/cint_dnx_nif_phy_loopback.c
 * cint ../../../../src/examples/dnx/cint_dnx_nif_snake_test.c
 * cint
 * exit;
 *
 * cint
 * int ports[6] = {1, 13, 14, 17, 271, 528};
 * bcm_mac_t mac_ddress = {00, 00, 00, 00, 00, 22};
 * cint_vlan_nif_snake_test_on_port_arr(0,mac_ddress,100,&ports,6,1);
 * exit;
 *
 * Test Scenario - end
 */

/* Basic parameters */
int MAX_NUMBER_OF_PORTS = 256;

/*
 * Function:
 *      cint_bcm_nif_snake_config
 * Purpose:
 *      Configure snake test between the nif ports of the device.
 * Parameters:
 *      unit         (IN) - unit
 *      anchor_port  (IN) - first port of the snake
 *      last_port    (OUT)- last port of the snake
 */
int
cint_bcm_nif_snake_config(int unit, bcm_port_t anchor_port, bcm_port_t *last_port, int add_loopback)
{
    int rv;
    bcm_port_config_t config;
    bcm_pbmp_t pbmp_nif,lane_pbmp;
    bcm_port_t port = -1, next_port;
    int is_first = 1, count = 0;
    int is_jericho2;
    int is_ext_lb,status,is_port_with_lane;
    /*
     * On JR2, lanes 88 and 89 have optic module, link training is not supported, if external
     * loopback is used, the link quality of the port with the lanes may be unstable, so should
     * set internal loopback on the ports
     */
    int lanes[] = {88, 89};
    int idx;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get \n");
        return rv;
    }

    *last_port = anchor_port;
    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed: unit %d.\n", unit);
        return rv;
    }

    /* configure snake between nif ports, take only nif ports */
    pbmp_nif = config.nif;
    count = sizeof(lanes) / sizeof(lanes[0]);
    for(idx = 0; idx < count; idx++) {
        BCM_PBMP_PORT_ADD(lane_pbmp,lanes[idx]);
    }

    BCM_PBMP_ITER(pbmp_nif, port)
    {
        if (port == anchor_port)
        { /* anchor port is configured later */
            continue;
        }

        if (is_first)
        {
            is_first = 0;
            next_port = port;
            *last_port = port;
        } else
        {
            printf("Setting force_forward %d-->%d\n", port, next_port);
            rv = bcm_port_force_forward_set(unit, port, next_port, 1);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_force_forward_set failed: unit %d, port %d to port %d.\n", unit, port, next_port);
                return rv;
            }
        }

        if (add_loopback == 1)
        {
            rv = dnx_port_phy_loopback_set(unit, port);
            if (BCM_FAILURE(rv))
            {
                printf("dnx_port_phy_loopback_set failed: unit %d, port %d.\n", unit, port);
                return rv;
            }
        } else if (add_loopback == 2)
        {
            /* phy loopback is not allowd on the PMs with lane_swap*/
            rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_loopback_set failed: unit %d, port %d.\n", unit, port);
                return rv;
            }
       } else if (add_loopback == 3)
       {
            /*Using exteranl loopback if port has exteranl loopback module*/
            is_ext_lb = is_ext_lb_port(unit, port);
            if (is_ext_lb == 1)
            {
                printf("Port %d is external loopbacked, skip loopback config\n",port);
            } else
            {
                /*
                 * If some ports in a PM have external loopback and the others are configured
                 * to phy loopback, ports may be down after lanes are aligned
                 * so the other ports have no exteranl looback module should be configured to
                 * mac loopback
                 */
                rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC);
                if (BCM_FAILURE(rv))
                {
                    printf("bcm_port_loopback_set failed: unit %d, port %d.\n", unit, port);
                    return rv;
                }
            }
        }
        next_port = port;
    }

    /*
     * port may change to down after the neighbor port is up.
     * here check if external loopback port goes down after the other ports are up
     * then configure the down port to mac loopback also
     */
    BCM_PBMP_ITER(pbmp_nif, port)
    {
        break;
        if (add_loopback == 3)
        {
            BCM_IF_ERROR_RETURN(bcm_port_link_status_get(unit, port, &status));
            if(status)
            {
                continue;
            } else
            {
                rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC);
                if (BCM_FAILURE(rv))
                {
                    printf("bcm_port_loopback_set failed: unit %d, port %d.\n", unit, port);
                    return rv;
                }
            }
        }
    }

    BCM_PBMP_COUNT(pbmp_nif, count);
    if (count > 1)
    {
        /* send traffic from anchor port to the snake between nif ports */
        printf("Setting force_forward %d-->%d\n", anchor_port, next_port);
        rv = bcm_port_force_forward_set(unit, anchor_port, next_port, 1);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_force_forward_set failed: unit %d, port %d to port %d.\n", unit, anchor_port, next_port);
            return rv;
        }
    }

    printf("cint_bcm_nif_snake_config: PASS\n\n");
    return rv;
}

/*
 * Function:
 *      cint_bcm_nif_snake_test
 * Purpose:
 *      Configure nif snake test on a fap.
 * Parameters:
 *      unit         (IN) - unit
 *      in_port      (IN) - traffic port
 */
int
cint_bcm_nif_snake_test(int unit, bcm_port_t in_port, int add_loopback)
{
    int rv;
    bcm_port_t last_port;

    rv = cint_bcm_nif_snake_config(unit, in_port, &last_port, add_loopback);
    if (BCM_FAILURE(rv))
    {
        printf("cint_bcm_nif_snake_config failed.\n");
        return rv;
    }

    printf("Setting force_forward %d-->%d\n", last_port, in_port);
    rv = bcm_port_force_forward_set(unit, last_port, in_port, 1);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_force_forward_set failed: from port %d to port %d.\n", last_port, in_port);
        return rv;
    }

    printf("cint_bcm_nif_snake_test: PASS\n\n");
    return rv;
}


/*
 * Function:
 *      cint_vlan_nif_snake_test
 * Purpose:
 *      run snake between ports.
 *
 * Params:
 *      unit    - unit
 *      mac_address - mac address of the incoming traffic.
 *      base_vid    - vlan id of the incoming traffic.
 *      in_port     - Ixia port.
 */
int
cint_vlan_nif_snake_test(int unit, bcm_mac_t mac_address, bcm_vlan_t base_vid, bcm_port_t in_port)
{
    int rv;
    bcm_gport_t sys_port;
    bcm_vlan_t  curr_vid, next_vid;
    int i, port_idx;
    bcm_gport_t voq_gport;
    bcm_cos_t   cosq;
    bcm_port_config_t config;
    next_vid = base_vid;

    /* get all nif ports */
    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed: unit %d.\n", unit);
        return rv;
    }
    BCM_PBMP_XOR(config.port, config.sfi); /* remove fabric ports */

    /* snake between ports */
    BCM_PBMP_ITER(config.port, port_idx)
    {
        if (port_idx == in_port)
        {
            continue;
        }
        printf("port = %d\n", port_idx);
        curr_vid = next_vid;
        next_vid = curr_vid + 1;
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, port_idx);
        rv = egr_vlan_edit(unit, port_idx, port_idx, curr_vid, next_vid);
        if (BCM_FAILURE(rv))
        {
            printf("egr_vlan_edit failed: port %d, curr_vid %d, next_vid %d.\n", port_idx, curr_vid, next_vid);
            return rv;
        }

        rv = l2_addr_add(unit, mac_address, curr_vid, sys_port);
        if (BCM_FAILURE(rv))
        {
            printf("l2_addr_add failed: vid %d.\n", curr_vid);
            return rv;
        }
    }

    /* send traffic back to in_port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, in_port);
    rv = l2_addr_add(unit, mac_address, next_vid, sys_port);
    if (BCM_FAILURE(rv))
    {
        printf("l2_addr_add failed: vid %d.\n", next_vid);
        return rv;
    }

    printf("cint_vlan_nif_snake_test: PASS\n\n");
    return rv;
}

/*
 * Function:
 *      cint_vlan_nif_snake_test_on_port_arr
 * Purpose:
 *      run snake between ports
 *
 * Params:
 *      unit           - unit
 *      mac_address    - mac address of the incoming traffic.
 *      base_vid       - vlan id of the incoming traffic.
 *      port_arr       - array of ports to be tested, traffic from port_arr[0] and back to port_arr[0]
 *      loopback       - if loopback all ports.
 *
 */
int
cint_vlan_nif_snake_test_on_port_arr(int unit, bcm_mac_t mac_address, bcm_vlan_t base_vid, int *port_arr, int port_cnt, int loopback)
{
    int rv, i;
    bcm_gport_t sys_port;
    bcm_vlan_t  curr_vid, next_vid;
    next_vid = base_vid;
    int next_port;

    /* configure loopback for each port except the 1st port which is in_port */
    if (loopback == 1)
    {
        for (i=1; i < port_cnt; i++)
        {
            rv = dnx_port_phy_loopback_set(unit, port_arr[i]);
            if (BCM_FAILURE(rv))
            {
                printf("dnx_port_phy_loopback_set failed for port %d.\n", port_arr[i]);
                return rv;
            }
        }
    }

    /* snake VLAN translation and MACT config */
    for (i=0; i < port_cnt - 1; i++)
    {
        printf("port = %d\n", port_arr[i]);

        next_port = port_arr[i+1];
        curr_vid = next_vid;
        next_vid = curr_vid + 1;

        rv = egr_vlan_edit(unit, port_arr[i], next_port, curr_vid, next_vid);
        if (BCM_FAILURE(rv))
        {
            printf("egr_vlan_edit failed: current port %d, next port %d, curr_vid %d, next_vid %d.\n",
                    port_arr[i], next_port, curr_vid, next_vid);
            return rv;
        }

        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, next_port);
        rv = l2_addr_add(unit, mac_address, curr_vid, sys_port);
        if (BCM_FAILURE(rv))
        {
            printf("l2_addr_add failed: vid %d, port %d\n", curr_vid, next_port);
            return rv;
        }
    }

    /* send traffic back to in_port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, port_arr[0]);
    rv = l2_addr_add(unit, mac_address, next_vid, sys_port);
    if (BCM_FAILURE(rv))
    {
        printf("l2_addr_add failed: vid %d, port %d\n", next_vid, port_arr[0]);
        return rv;
    }

    printf("cint_sat_vlan_nif_snake_test: PASS\n\n");
    return rv;
}

/*
 * Function:
 *      is_ext_lb_port
 * Purpose:
 *      Verify if port is external loopback port
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  port.
*/

int
is_ext_lb_port(int unit, int port)
{
    uint32_t tx_cnt, rx_cnt;
    uint64 tx_cnt_dnx, rx_cnt_dnx;
    int tx_dnx, rx_dnx;
    char cmd[20];
    int rv;
    int status;

    /* for each port in pbmp_ut, send pkts and check which port receives them */
    BCM_IF_ERROR_RETURN(bcm_port_link_status_get(unit, port, &status));
    if(!status) return 0;

    bshell(unit, "clear cou");
    snprintf(cmd,20, "tx 1 pbm=%d vl=2",port);
    bshell(unit, cmd);
    bcm_stat_sync(unit);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, port, snmpIfHCOutUcastPkts, &tx_cnt_dnx));
    COMPILER_64_TO_32_LO(tx_dnx, tx_cnt_dnx);
    tx_cnt = tx_dnx;

    if (tx_cnt)
    {
        /* check if port rx */
        BCM_IF_ERROR_RETURN(bcm_stat_get(unit, port, snmpIfHCInUcastPkts, &rx_cnt_dnx));
        COMPILER_64_TO_32_LO(rx_dnx, rx_cnt_dnx); rx_cnt=rx_dnx;
        if (tx_cnt == 1 && rx_cnt == 1)
        {
            return 1;
        }
    }

    return 0;
}

/* Egress Vlan editing, replace old vid with new vid on port */
int egr_vlan_edit(
    int unit,
    int curr_port,
    int next_port,
    int curr_vid,
    int next_vid)
{
    int                       rv = BCM_E_NONE;
    bcm_vlan_action_set_t     action;
    uint32                    vlan_domain;
    int                       C_TPID      = 0x8100;
    bcm_gport_t               ports[1]    = {curr_port};  /* list of ports to use in vlan creation    */
    uint8                     tagged[1] = {1};          /* port untagged status */
    int vlan_edit_profile=2;
    int incoming_tag_structure=4;
    int eve_action_id=3;
    int vlan_port_id;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_translation_t vlan_port_translation;
    bcm_port_tpid_class_t tpid_class;
    bcm_vlan_translate_action_class_t vlan_translate_action_class;
    bcm_vlan_action_set_t eve_action;

    /* open vlan with ports */
    rv = vlan__open_vlan_per_ports(unit, curr_vid, ports, tagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan__open_vlan_per_ports\n");
        return rv;
    }

    ports[0] = next_port;
    rv = vlan__open_vlan_per_ports(unit, next_vid, ports, tagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan__open_vlan_per_ports\n");
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.flags=BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.port = next_port;
    vlan_port.vsi = curr_vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create (unit %d)\n", unit);
    }
    vlan_port_id=vlan_port.vlan_port_id;

    bcm_vlan_port_translation_t_init(&vlan_port_translation);
    vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS;
    vlan_port_translation.gport = vlan_port_id;
    vlan_port_translation.new_outer_vlan = next_vid;
    vlan_port_translation.vlan_edit_class_id=vlan_edit_profile;
    rv = bcm_vlan_port_translation_set(unit,&vlan_port_translation);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set (unit %d)\n", unit);
    }

    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.port = next_port;
    tpid_class.tpid1 = C_TPID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tag_format_class_id=incoming_tag_structure;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_tpid_class_set (unit %d)\n", unit);
    }

    bcm_vlan_translate_action_class_t_init(&vlan_translate_action_class);
    vlan_translate_action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    vlan_translate_action_class.vlan_edit_class_id = vlan_edit_profile;
    vlan_translate_action_class.tag_format_class_id = incoming_tag_structure;
    vlan_translate_action_class.vlan_translation_action_id = eve_action_id;
    rv = bcm_vlan_translate_action_class_set(unit,&vlan_translate_action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set (unit %d)\n", unit);
    }

    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &eve_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create (unit %d)\n", unit);
    }

    bcm_vlan_action_set_t_init(&eve_action);
    eve_action.dt_outer = bcmVlanActionReplace;
    eve_action.dt_inner = bcmVlanActionNone;
    eve_action.outer_tpid = C_TPID;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, eve_action_id, &eve_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set (unit %d)\n", unit);
    }

    if (rv == BCM_E_NONE)
    {
        printf("\n *** egr_vlan_edit - DONE *** \n");
    }
    return rv;
}