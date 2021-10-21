/*
 * cint_dnx_qos_l3_remark.c
 *
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/qos/cint_dnx_qos_l3_remark.c
 * cint
 * dnx_basic_example_ipv4_qos(0,200,201,52430);
 * exit;
 *
 * Sending IPv4 packet
 *   tx 1 psrc=200 data=0x000c000200000000070001008100000f080045fa0035000000001e005b4cc08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 *   Data: 0x00000000cd1d00123456789a8100b064080045f90035000000001d005c4dc08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

int l3_in_map_id = -1;
int l3_phb_opcode = -1;
int l3_remark_opcode = -1;
int l3_eg_map_id = -1;
int l3_eg_remark_opcode = -1;

/*
 * Map: Incoming dscp -> TC (int_pri), DP (color), nwk_qos (remark_int_pri)
 *   (dscp + 1) % 8 -> TC
 *   (dscp + 1) % 2 -> DP
 *   (dscp + 1) & 255 -> nwk_qos;
*/
int
qos_map_l3_ingress_profile(int unit, int is_ipv6)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_in_map;
    int dscp;
    int flags;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    /* Create qos-profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    if (l3_in_map_id != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for ingress cos-profile\n");
        return rv;
    }

    printf("bcm_qos_map_create ingress profile: %d\n", l3_in_map_id);

    /* Create opcode for PHB */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    if (l3_phb_opcode != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_phb_opcode);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for ingress phb opcode\n");
        return rv;
    }

    printf("bcm_qos_map_create ingress PHB opcode: %d\n", l3_phb_opcode);

    /* Add the maps for PHB */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = l3_phb_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_add() for ingress opcode for PHB\n");
        return rv;
    }

    /* Add qos mapping entry for PHB */
    bcm_qos_map_t_init(&l3_in_map);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_PHB | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    for (dscp = 0; dscp < 256; dscp++)
    {
        l3_in_map.dscp = dscp;
        l3_in_map.int_pri = (dscp + 1) % 8;
        l3_in_map.color = (dscp + 1) % 2;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_phb_opcode);
        if (rv != BCM_E_NONE) {
            printf("Error: in bcm_qos_map_add() for ingress qos map for PHB\n");
            return rv;
        }
    }

    /* Create opcode for remark */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l3_remark_opcode != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for ingress remark opcode\n");
        return rv;
    }

    printf("bcm_qos_map_create ingress PHB opcode: %d\n", l3_remark_opcode);

    /* Add the maps for remark */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = l3_remark_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_add() for ingress opcode for remark\n");
        return rv;
    }

    /* Add qos mapping entry for remark */
    bcm_qos_map_t_init(&l3_in_map);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    for (dscp = 0; dscp < 256; dscp++)
    {
        l3_in_map.dscp = dscp;
        l3_in_map.remark_int_pri = (dscp + 1) & 255;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_remark_opcode);
        if (rv != BCM_E_NONE) {
            printf("Error: in bcm_qos_map_add() for ingress qos map for remark\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Map: nwk_qos (int_pri) + DP (color) -> outgoing dscp (dscp)
 *   Current layer: (nwk_qos - 2) & 255 -> dscp
 *   Next layer: nwk_qos -> new nwk_qos, dp -> new dp
*/
int
qos_map_l3_egress_profile(int unit, int is_ipv6)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_eg_map;
    int int_pri,dp;
    int flags;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    /* Create qos-profile */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    if (l3_eg_map_id != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for egress profile\n");
        return rv;
    }

    printf("bcm_qos_map_create egress profile: %d\n", l3_eg_map_id);

    /* Create opcode */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l3_eg_remark_opcode != -1) {
        flags |= BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_eg_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_create() for egress remark opcode\n");
        return rv;
    }

    printf("bcm_qos_map_create egress remark opcode: %d\n", l3_eg_remark_opcode);

    /* Add the maps */
    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = l3_eg_remark_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error: in bcm_qos_map_add() for egress opcode\n");
        return rv;
    }

    /* Add qos mapping entry */
    bcm_qos_map_t_init(&l3_eg_map);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_REMARK | (is_ipv6?BCM_QOS_MAP_IPV6:BCM_QOS_MAP_IPV4);
    for (int_pri = 0; int_pri < 256; int_pri++)
    {
        for (dp = 0; dp < 4; dp++)
        {
            l3_eg_map.int_pri = int_pri;
            l3_eg_map.color = dp;
            l3_eg_map.dscp = (int_pri - 2) & 255;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_remark_opcode);
            if (rv != BCM_E_NONE) {
                printf("Error: in bcm_qos_map_add() for egress qos map\n");
                return rv;
            }
        }
    }

    /* Next layer mapping */
    bcm_qos_map_t_init(&l3_eg_map);
    for (int_pri = 0; int_pri < 256; int_pri++)
    {
        for (dp = 0; dp < 4; dp++)
        {
            l3_eg_map.int_pri = int_pri;
            l3_eg_map.color = dp;
            l3_eg_map.remark_int_pri = int_pri;
            l3_eg_map.remark_color = dp;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error: in bcm_qos_map_add() for egress qos map\n");
                return rv;
            }
        }
    }

    return rv;
}

/*
 * L3 IPv4 qos example, specifically
 *   ingress L3 tos to tc/dp/nwk_qos mapping
 *   egress nwk_qos+dp to tos mapping
 */
int
dnx_basic_example_ipv4_qos(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    int rv;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_intf_t l3if;

    /* Basic IPv4 routing */
    rv = dnx_basic_example_ipv4_inner(unit, in_port, out_port, kaps_result);
    if (rv != BCM_E_NONE) {
        printf("Error: in dnx_basic_example_ipv4_inner\n");
        return rv;
    }

    /* Ingress profile and mapping */
    rv = qos_map_l3_ingress_profile(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: in qos_map_l3_ingress_profile\n");
        return rv;
    }

    /* Attch the ingress QoS profile to inRIF */
    bcm_l3_ingress_t_init(&l3_ing_if);
    rv = bcm_l3_ingress_get(unit, cint_ip_route_info.intf_in, &l3_ing_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_get\n");
        return rv;
    }

    l3_ing_if.qos_map_id = l3_in_map_id;
    l3_ing_if.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_ip_route_info.intf_in);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_ingress_create\n");
        return rv;
    }

    /* Egress profile and mapping */
    rv = qos_map_l3_egress_profile(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: in qos_map_l3_egress_profile\n");
        return rv;
    }

    /* Attch the egress QoS profile to outRIF */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_intf_id = cint_ip_route_info.intf_out;
    rv = bcm_l3_intf_get(unit, &l3if);

    l3if.l3a_flags |= BCM_L3_WITH_ID | BCM_L3_REPLACE;
    l3if.dscp_qos.qos_map_id = l3_eg_map_id;

    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    return rv;
}

/*
 * L3 IPv6 qos example, specifically
 *   ingress L3 traffic class to tc/dp/nwk_qos mapping
 *   egress nwk_qos+dp to traffic class mapping
 */
int
dnx_basic_example_ipv6_qos(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_intf_t l3if;

    /* Basic IPv6 routing */
    rv = basic_example_ipv6(unit, in_port, out_port,0,0,0);
    if (rv != BCM_E_NONE) {
        printf("Error: in basic_example_ipv6\n");
        return rv;
    }

    /* Ingress profile and mapping */
    rv = qos_map_l3_ingress_profile(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error: in qos_map_l3_ingress_profile\n");
        return rv;
    }

    /* Attch the ingress QoS profile to inRIF */
    bcm_l3_ingress_t_init(&l3_ing_if);
    rv = bcm_l3_ingress_get(unit, cint_ip_route_info.intf_in, &l3_ing_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_get\n");
        return rv;
    }

    l3_ing_if.qos_map_id = l3_in_map_id;
    l3_ing_if.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_ip_route_info.intf_in);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_ingress_create\n");
        return rv;
    }

    /* Egress profile and mapping */
    rv = qos_map_l3_egress_profile(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error: in qos_map_l3_egress_profile\n");
        return rv;
    }

    /* Attch the egress QoS profile to outRIF */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_intf_id = cint_ip_route_info.intf_out;
    rv = bcm_l3_intf_get(unit, &l3if);

    l3if.l3a_flags |= BCM_L3_WITH_ID | BCM_L3_REPLACE;
    l3if.dscp_qos.qos_map_id = l3_eg_map_id;

    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    return rv;
}
