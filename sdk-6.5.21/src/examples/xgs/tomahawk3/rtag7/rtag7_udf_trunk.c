/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : RTAG7 UDF hashing - LAG
 *
 *  Usage    : BCM.0> cint rtag7_udf_trunk.c
 *
 *  config   : rtag7_config.bcm
 *
 *  Log file : rtag7_udf_trunk_log.txt
 *
 *  Test Topology :
 *
 *                +-----------+
 *                |           |        +--EgressPort1--->
 *                |           |        |
 *  IngressPort-->|   SWITCH  |--trunk-+--EgressPort2--->
 *                |           |        |
 *                |           |        +--EgressPort3--->
 *                +-----------+
 *
 *   Summary:
 *   ========
 *     This cint example configures RTAG7 UDF hashing on the egress trunk using
 *     BCM APIs. The UDF is created for the Src and Dst port of the UDP header
 *
 *   Detailed steps done in the CINT script
 *   ======================================
 *     1) Step1 - Test Setup (Done in test_setup())
 *     ============================================
 *       a) Select one ingress port and three egress ports and configure them in Loopback mode.
 *
 *     2) Step2 - Configuration (Done in rtag7_udf_trunk())
 *     ==================================================== 
 *       a) Create a VLAN(12) and add ingress port and egress port as members.
 *       b) Create a trunk with id 1 and add all the egress ports to this trunk. Set the port
 *          selection criteria to BCM_TRUNK_PSC_PORTFLOW (RTAG7).
 *       c) Configure the destination MAC address of the test packet statically in the L2 table
 *          in order to make the test packet a unicast packet.
 *       d) Set the switch controls required for UDF hashing.
 *       e) Create 2 UDFs for the UDP Src and Dst port and add them to the hashing list
 *
 *     3) Step3 - Verification (Done in verify())
 *     ==========================================
 *       a) Transmit 10 packets with incremental L4 SrcPort and L4 DstPort on the ingress port
 *          and verify that packets are load balanced across the egress ports using "show counters"
 *          Packets
 *          =======
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5000 Dst Port 80
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 88 0A 0A
 *          01 00 0A 0A 02 00 13 88  00 50 00 3D CE 40 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5001 Dst Port 81
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 86 0A 0A
 *          01 01 0A 0A 02 01 13 89  00 51 00 3D CE 3C 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5002 Dst Port 82
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 84 0A 0A
 *          01 02 0A 0A 02 02 13 8A  00 52 00 3D CE 38 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5003 Dst Port 83
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 82 0A 0A
 *          01 03 0A 0A 02 03 13 8B  00 53 00 3D CE 34 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 * 
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5004 Dst Port 84
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 80 0A 0A
 *          01 04 0A 0A 02 04 13 8C  00 54 00 3D CE 30 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5005 Dst Port 85
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 7E 0A 0A
 *          01 05 0A 0A 02 05 13 8D  00 55 00 3D CE 2C 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5006 Dst Port 86
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 7C 0A 0A
 *          01 06 0A 0A 02 06 13 8E  00 56 00 3D CE 28 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5007 Dst Port 87
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 7A 0A 0A
 *          01 07 0A 0A 02 07 13 8F  00 57 00 3D CE 24 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5008 Dst Port 88
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 78 0A 0A
 *          01 08 0A 0A 02 08 13 90  00 58 00 3D CE 20 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 10.10.2.0 SIP 10.10.1.0
 *          Src Port 5009 Dst Port 89
 *          00 00 00 00 AA AA 00 00  00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 51 00 01  00 00 40 11 63 76 0A 0A
 *          01 09 0A 0A 02 09 13 91  00 59 00 3D CE 1C 28 20
 *          DB 20 00 20 00 20 45 20  00 20 00 20 54 20 00 20
 *          00 20 40 20 00 20 40 20  00 20 5E 20 A5 20 CA 20
 *          0B 20 28 20 9E 20 C0 20  A8 20 28 20 B2 20 08 20
 *          00 20 FF
 *       b) Expected Result
 *          ===============
 *          The packets should be load balanced across all the trunk member ports. This can be
 *          checked using "show counters"
 */

/* Reset c interpreter*/
cint_reset();
bcm_port_t ingress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_port_t egress_port_3;

/* This function populates the port_list_arr with the required
   number of ports
*/
bcm_error_t portNumbersGet(int unit, bcm_port_t *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and sets the discard bit
 * in the PORT table
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
    return BCM_E_NONE;
}
/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int port_list[4];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];
    egress_port_3 = port_list[3];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed");
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_1)) {
        printf("egress_port_setup() failed for port:%d", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2)) {
        printf("egress_port_setup() failed for port:%d", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_3)) {
        printf("egress_port_setup() failed for port:%d", egress_port_3);
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * This function creates a trunk and adds the member ports
 */
bcm_error_t create_trunk(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_ports)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_info = NULL;
    bcm_trunk_member_t info;

    bcm_trunk_info_t_init(&trunk_info);

    member_info = sal_alloc((sizeof(info) * count), "trunk members");

    for (i = 0 ; i < count ; i++) {
        bcm_trunk_member_t_init(&member_info[i]);
        BCM_GPORT_MODPORT_SET(member_info[i].gport, 0, member_ports[i]);
    }

    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;

    trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_create %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_set %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/*
 * The UDF chunks UDF_1 chunks 6 and 7 are overlaid on hash bins 2 and 3
 * for RTAG7 calculation.
 */
bcm_error_t udf_hash_init(int unit)
{
    int arg = 0;
    bcm_error_t rv = BCM_E_NONE;

    /* Enable UDF hashing */
    rv = bcm_switch_control_set(unit, bcmSwitchUdfHashEnable, BCM_HASH_FIELD0_ENABLE_UDFHASH |
            BCM_HASH_FIELD1_ENABLE_UDFHASH);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for UdfHashEnable %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Enable Bin 2 and 3 for flex hash */
    arg = BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT;

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4Field0 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4Field1 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpField0 %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpField1 %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpPortsEqualField0  %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashIP4TcpUdpPortsEqualField1  %s.\n",
                 bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                                BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashField0Config %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config,
                                BCM_HASH_FIELD_CONFIG_CRC32LO);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashField1Config %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashSeed0 %s.\n", bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashSeed1 %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_switch_control_get(0, bcmSwitchHashControl, &arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_get for HashControl %s.\n", bcm_errmsg (rv));
        return rv;
    }
    arg |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    rv = bcm_switch_control_set(0, bcmSwitchHashControl, arg);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set for HashControl %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/*
 * Create UDF to get IPv4 UDP packet src & dest ports
 */
bcm_error_t udf_hash_udp_ports_create_udf(int unit)
{
    bcm_udf_alloc_hints_t hints;
    bcm_udf_t udf_info;
    bcm_udf_pkt_format_info_t pkt_format;
    bcm_udf_pkt_format_id_t pkt_format_id = 0;
    bcm_udf_hash_config_t config;
    bcm_udf_id_t udf_ids[2];
    int udf_offset[2] = {0, 16};   /* Source and destination UDP port */
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;

    /* Gnerate packet format to match IP/UDP packet */
    bcm_udf_pkt_format_info_t_init(&pkt_format);
    pkt_format.l2                 = BCM_PKT_FORMAT_L2_ANY;
    pkt_format.vlan_tag           = BCM_PKT_FORMAT_VLAN_TAG_ANY;
    pkt_format.ip_protocol        = 0x11;  /* UDP */
    pkt_format.ip_protocol_mask   = 0xff;
    pkt_format.outer_ip           = BCM_PKT_FORMAT_IP_ANY;
    pkt_format.inner_ip           = BCM_PKT_FORMAT_IP_NONE;
    pkt_format.tunnel             = BCM_PKT_FORMAT_TUNNEL_NONE;
    rv = bcm_udf_pkt_format_create(unit, FALSE /* IPv4 */, &pkt_format, &pkt_format_id);
    if (BCM_FAILURE(rv)) {
        printf ("Failed to create pkt format, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags    = BCM_UDF_CREATE_O_UDFHASH;

    /* Create UDF to match on UDP ports */
    for (i = 0; i < 2; i++) {
        bcm_udf_t_init(&udf_info);
        udf_info.flags = 0;
        udf_info.layer = bcmUdfLayerL4OuterHeader;
        udf_info.start = udf_offset[i];
        udf_info.width = 16;
        rv = bcm_udf_create(unit, &hints, &udf_info, &udf_ids[i]);
        if (BCM_FAILURE(rv)) {
            printf ("failed to create UDF for dest port, error code: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Attach both UDF id to ip UDP packet */

        rv  = bcm_udf_pkt_format_add(unit, udf_ids[i], pkt_format_id);
        if (BCM_FAILURE(rv)) {
            printf ("Failed to attach udf id %d to pkt format %d\n", udf_ids[i], pkt_format_id);
            return rv;
        }

        /* Config hash mask for UDF_1 chunks 6 and 7 overlaid on hash bins 2 and 3 */
        bcm_udf_hash_config_t_init(&config);
        config.udf_id = udf_ids[i];
        config.mask_length = 2;
        config.hash_mask[0] = 0xff;
        config.hash_mask[1] = 0xff;
        rv = bcm_udf_hash_config_add(unit, 0, &config);
        if (BCM_FAILURE(rv)) {
            printf("failed, error code: %s\n", bcm_errmsg(BCM_FAILURE(rv)));
            return rv;
        }
    }

    return rv;
}

bcm_error_t rtag7_udf_trunk(int unit)
{
    bcm_vlan_t vid = 12;
    bcm_pbmp_t pbmp, ubmp;
    bcm_gport_t trunk_gp_id;
    bcm_trunk_t trunk_id = 1;
    bcm_port_t trunk_ports[3];
    bcm_l2_addr_t l2addr;
    bcm_mac_t da = {0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA};
    int i = 0; 
    bcm_error_t rv = BCM_E_NONE;

    /* Create the vlan with specific id */
    rv = bcm_vlan_create (unit, vid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    BCM_PBMP_CLEAR (pbmp);
    BCM_PBMP_CLEAR (ubmp);

    BCM_PBMP_PORT_ADD (pbmp, ingress_port);
    BCM_PBMP_PORT_ADD (pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD (pbmp, egress_port_2);
    BCM_PBMP_PORT_ADD (pbmp, egress_port_3);

    rv = bcm_vlan_port_add (unit, vid, pbmp, ubmp);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    trunk_ports[0] = egress_port_1;
    trunk_ports[1] = egress_port_2;
    trunk_ports[2] = egress_port_3;

    rv = create_trunk(unit, &trunk_id, 3, trunk_ports);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_trunk(): %s.\n", bcm_errmsg (rv));
        return rv;
    }
    BCM_GPORT_TRUNK_SET (trunk_gp_id, trunk_id);

    bcm_l2_addr_t_init(&l2addr, da, vid);
    l2addr.port = trunk_gp_id;
    rv = bcm_l2_addr_add (unit, &l2addr);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_udf_init(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_udf_init(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* UDF hash configurations */
    rv = udf_hash_init(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing udf_hash_init(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = udf_hash_udp_ports_create_udf(unit);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing udf_hash_udp_ports_create(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

void verify(int unit)
{
    char   str[512];

    bshell(unit, "hm ieee");

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163880A0A01000A0A020013880050003DCE402820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163860A0A01010A0A020113890051003DCE3C2820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163840A0A01020A0A0202138A0052003DCE382820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163820A0A01030A0A0203138B0053003DCE342820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163800A0A01040A0A0204138C0054003DCE302820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C080045000051000100004011637E0A0A01050A0A0205138D0055003DCE2C2820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C080045000051000100004011637C0A0A01060A0A0206138E0056003DCE282820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C080045000051000100004011637A0A0A01070A0A0207138F0057003DCE242820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163780A0A01080A0A020813900058003DCE202820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500005100010000401163760A0A01090A0A020913910059003DCE1C2820DB200020002045200020002054200020002040200020402000205E20A520CA200B2028209E20C020A8202820B22008200020FF; sleep quiet 1", ingress_port);
    bshell(unit, str);

    snprintf(str, 512, "show counters");
    bshell(unit, str);
}

bcm_error_t execute()
{
    int unit = 0;
    int rv = 0;

    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = rtag7_udf_trunk(unit)))) {
        printf("random_lb_trunk() failed.\n");
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

