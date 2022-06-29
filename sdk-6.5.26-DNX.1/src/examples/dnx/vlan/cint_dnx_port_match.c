/*
 * File: cint_dnx_port_match.c
 * Purpose: Example of the port match application
 *
 *  The purpose of the port match application is to configure several mappings over the same gport.
 *  It has two modes: ingress match and egress match.
 *  Egress match allows you to set several <vlan_domain, vid> combinations on the same egress vlan port.
 *  Ingress match allows you to set several <vlan_domain, vid> combinations on the same ingress vlan port.
 *      * Ingress match is also divided into two modes: with initial match, and without initial match.
 *          * With initial match means that bcm_vlan_port_create will also add the first <vlan_domain, vid> mapping
 *          * Without initial match means that bcm_vlan_port_create will not add the first <vlan_domain, vid> mapping.
 *
 * Test Scenario:
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/cint_dnx_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/vlan/cint_dnx_port_match.c
 * cint
 * exit;
 *
 * Testing traffic to dst. mac 00:00:00:00:00:01 with vid 10
 *   tx 1 psrc=200 data=0x000000000001000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000001000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:02 with vid 10
 *   tx 1 psrc=200 data=0x000000000002000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000002000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:01 with vid 30
 *   tx 1 psrc=200 data=0x000000000001000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000001000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:02 with vid 30
 *   tx 1 psrc=200 data=0x000000000002000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000002000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:01 with vid 10
 *   tx 1 psrc=200 data=0x000000000001000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000001000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:02 with vid 10
 *   tx 1 psrc=200 data=0x000000000002000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000002000000aabbcc8100000affff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:01 with vid 30
 *   tx 1 psrc=200 data=0x000000000001000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000001000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:02 with vid 30
 *   tx 1 psrc=200 data=0x000000000002000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000002000000aabbcc8100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:01
 *   tx 1 psrc=200 data=0x000000000001000000aabbcc81000032ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000001000000aabbcc81000028ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:02
 *   tx 1 psrc=200 data=0x000000000002000000aabbcc81000032ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x000000000002000000aabbcc81000028ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Testing traffic to dst. mac 00:00:00:00:00:03
 *   tx 1 psrc=200 data=0x000000000003000000aabbcc81000032ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */

struct port_match_info_s {
    int in_vsi;
    int eg_vsi;
    int in_vids[3];
    int eg_vids[2];
    int ports[4];
    bcm_mac_t dmac_1;
    bcm_mac_t dmac_2;
    bcm_mac_t dmac_3;
    int tpid;
    bcm_gport_t in_gport;
    bcm_gport_t eg_gport_ingress;
    bcm_gport_t eg_gport_egress;
    bcm_gport_t vlan_port_egress_default;
} ;

port_match_info_s port_match_info_1;


int
port_match_info_init_default(bcm_gport_t port_1, bcm_gport_t port_2, bcm_gport_t port_3, bcm_gport_t port_4){
    bcm_mac_t dmac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    bcm_mac_t dmac_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t dmac_3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};

    port_match_info_1.tpid = 0x8100;
    port_match_info_1.in_vsi = 500;
    port_match_info_1.eg_vsi = 600;

    port_match_info_1.in_vids[0] = 10;
    port_match_info_1.eg_vids[0] = 20;
    port_match_info_1.in_vids[1] = 30;
    port_match_info_1.eg_vids[1] = 40;
    port_match_info_1.in_vids[2] = 50;

    sal_memcpy(port_match_info_1.dmac_1, dmac_1, 6);

    sal_memcpy(port_match_info_1.dmac_2, dmac_2, 6);

    sal_memcpy(port_match_info_1.dmac_3, dmac_3, 6);

    port_match_info_1.ports[0] = port_1;
    port_match_info_1.ports[1] = port_2;
    port_match_info_1.ports[2] = port_3;
    port_match_info_1.ports[3] = port_4;
    port_match_info_1.in_gport = -1;
    port_match_info_1.eg_gport_ingress = -1;
    port_match_info_1.eg_gport_egress = -1;
    port_match_info_1.vlan_port_egress_default = -1;
    return BCM_E_NONE;
}

/* Add a new match criteria to an existing gport. Packets with this <port, vid> combinations will be routed same as the other packets in the gport. */
int
port_match_add_remove(
    int unit,
    bcm_port_match_t match,
    uint32 flags,
    int match_vlan,
    bcm_gport_t match_port,
    bcm_gport_t gport,
    int add)
{
    bcm_port_match_info_t matchInfo;
    int rv;

    bcm_port_match_info_t_init(&matchInfo);
    matchInfo.match = match;
    matchInfo.flags = flags;
    matchInfo.match_vlan = match_vlan;
    matchInfo.port = match_port;

    if (add){
        rv = bcm_port_match_add(unit, gport, &matchInfo);
        if (rv != 0){
            printf("Error, in bcm_port_match_add. Unit %d, match_port: %d, match_vid is: %d\n", unit, match_port, match_vlan);
            return rv;
        }
        printf("Unit %d: Added port match <port, vid> = <%d,%d> to gport 0x%x\n", unit, match_port, match_vlan, gport);
    } else {
        rv = bcm_port_match_delete(unit, gport, &matchInfo);
        if (rv != 0){
            printf("Error, in bcm_port_match_delete.\n");
            return rv;
        }
    }

    return rv;
}

int port_match_run_ingress_match(
    int *units_ids,
    int nof_units,
    int port1,
    int port2,
    int port3,
    int no_initial_match)
{
    int rv;
    int i,unit;
    bcm_l2_addr_t l2_addr;
    uint32 flags;
    bcm_vlan_port_t vlan_port;

    int unit1 = units_ids[0];
    int unit2 = units_ids[1];

    /*
     * Init config
     * For multiple device, port1 on uni1, port2 & port3 on unit2
    */
    port_match_info_init_default(port1,port2,port3,0);

    /* Add first l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_1, port_match_info_1.in_vsi);
    l2_addr.port =port_match_info_1.ports[1];
    l2_addr.flags = BCM_L2_STATIC;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Add second l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_2, port_match_info_1.in_vsi);
    l2_addr.port =port_match_info_1.ports[2];
    l2_addr.flags = BCM_L2_STATIC;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Create VLANs on unit1 */
    for (i = 0 ; i < 2 ; i++)
    {
        rv = bcm_vlan_create(unit1, port_match_info_1.in_vids[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            return rv;
        }

        rv = bcm_vlan_gport_add(unit1, port_match_info_1.in_vids[i], port_match_info_1.ports[0], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add port1\n");
            return rv;
        }
    }

    /*
     * Create VLAN ports
    */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = no_initial_match ? BCM_VLAN_PORT_MATCH_NONE : BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.port = no_initial_match ? 0:port_match_info_1.ports[0];
    vlan_port.match_vlan = no_initial_match ? 0:port_match_info_1.in_vids[0];
    vlan_port.vsi = port_match_info_1.in_vsi;
    rv = bcm_vlan_port_create(unit1, &vlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /* The following two inputs are used for eve */
    vlan_port.egress_vlan = port_match_info_1.eg_vids[0];

    /* save the created gport */
    port_match_info_1.in_gport = vlan_port.vlan_port_id;

    if (no_initial_match) {
        /* Add the match criteria we've missed by setting the vlan port on _PORT_MATCH_NONE */
        rv = port_match_add_remove(unit1,
                       BCM_PORT_MATCH_PORT_CVLAN,
                       BCM_PORT_MATCH_INGRESS_ONLY,
                       port_match_info_1.in_vids[0],
                       port_match_info_1.ports[0],
                       port_match_info_1.in_gport,
                       1);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_match_add_remove\n");
            return rv;
        }
    }

    /* Add multiple match critreia for the existing vlan port */
    rv = port_match_add_remove(unit1,
                   BCM_PORT_MATCH_PORT_CVLAN,
                   BCM_PORT_MATCH_INGRESS_ONLY,
                   port_match_info_1.in_vids[1],
                   port_match_info_1.ports[0],
                   port_match_info_1.in_gport,
                   1);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_match_add_remove\n");
        return rv;
    }

    printf("Done configuring ingress port match\n");
    return rv;
}

int port_match_run_egress_match(
    int *units_ids,
    int nof_units,
    int port1,
    int port2,
    int port3,
    int port4)
{
    int rv;
    int i,unit;
    int port;
    bcm_l2_addr_t l2_addr;
    uint32 flags;
    bcm_vlan_port_t vlan_port;
    int nof_eg_ports = 3;
    int nof_dropped_ports = 1;

    int unit1 = units_ids[0];
    int unit2 = units_ids[1];

    /*
     * Init config
     * For multiple device, port1 on uni1; port2,port3,port4 on unit2
    */
    port_match_info_init_default(port1,port2,port3,port4);

    /* Add first l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_1, port_match_info_1.eg_vsi);
    l2_addr.port =port_match_info_1.ports[1];
    l2_addr.flags = BCM_L2_STATIC;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Add second l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_2, port_match_info_1.eg_vsi);
    l2_addr.port =port_match_info_1.ports[2];
    l2_addr.flags = BCM_L2_STATIC;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Add third l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_3, port_match_info_1.eg_vsi);
    l2_addr.port =port_match_info_1.ports[3];
    l2_addr.flags = BCM_L2_STATIC;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Create in VLANs on unit1 */
    rv = bcm_vlan_create(unit1, port_match_info_1.in_vids[2]);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit1, port_match_info_1.in_vids[2], port_match_info_1.ports[0], 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add port1\n");
        return rv;
    }

    /*
     * Create VLAN ports
    */
    flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.flags = flags;
    vlan_port.port = port_match_info_1.ports[0];
    vlan_port.match_vlan = port_match_info_1.in_vids[2];
    vlan_port.vsi = port_match_info_1.eg_vsi;
    rv = bcm_vlan_port_create(unit1, &vlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    port_match_info_1.eg_gport_ingress = vlan_port.vlan_port_id;

    /* Create an Egress only VLAN port, so that LIF matching for the various Egress ports can be added.
       The egress vlan port should be created first on the same unit as the egress ports. */
    flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = flags;
    rv = bcm_vlan_port_create(unit2, &vlan_port);

    if (rv != BCM_E_NONE) {
         printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    port_match_info_1.eg_gport_egress = vlan_port.vlan_port_id;

    /* Create EVE on egress AC */
    rv = vlan_translate_ive_eve_translation_set(unit2, port_match_info_1.eg_gport_egress, /* lif  */
                                                      0x8100,                         /* outer_tpid */
                                                      0,                              /* inner_tpid */
                                                      bcmVlanActionReplace,           /* outer_action */
                                                      bcmVlanActionNone,              /* inner_action */
                                                      port_match_info_1.eg_vids[1],   /* new_outer_vid */
                                                      0,                              /* new_inner_vid */
                                                      3,                              /* vlan_edit_profile */
                                                      8,                              /* tag_format - here is ctag */
                                                      FALSE                           /* is_ive */
                                                      );
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress vlan editing\n");
        return rv;
    }

    /* Create an additional Egress LIF in order to define port default behaviour */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = flags;
    rv = bcm_vlan_port_create(unit2, &vlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    port_match_info_1.vlan_port_egress_default = vlan_port.vlan_port_id;

    /* Add multiple egress match for the existing vlan port */
    for (i = 0 ; i < nof_eg_ports ; i++) {
        port = port_match_info_1.ports[i + 1];
        if (i < (nof_eg_ports - nof_dropped_ports)) {
            rv = port_match_add_remove(unit2,
                                       BCM_PORT_MATCH_PORT,
                                       BCM_PORT_MATCH_EGRESS_ONLY,
                                       -1,
                                       port,
                                       port_match_info_1.eg_gport_egress,
                                       1);
            if (rv != BCM_E_NONE){
                printf("Error, in port_match_add_remove\n");
                return rv;
            }
        } else {
            /* Add a port match to the Egress default LIF */
            rv = port_match_add_remove(unit2,
                                       BCM_PORT_MATCH_PORT,
                                       BCM_PORT_MATCH_EGRESS_ONLY,
                                       -1,
                                       port,
                                       port_match_info_1.vlan_port_egress_default,
                                       1);
            if (rv != BCM_E_NONE){
                printf("Error, in Default Egress port_match_add_remove\n");
                return rv;
            }
        }
    }

    /* Set the behaviour for Default Egress to Drop */
    bcm_rx_trap_lif_config_t lif_config;
    bcm_gport_t action_gport;
    BCM_GPORT_TRAP_SET(action_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 13, 0);
    lif_config.lif_type = bcmRxTrapLifTypeOutLif;
    lif_config.lif_gport = port_match_info_1.vlan_port_egress_default;
    lif_config.action_gport = action_gport;
    rv = bcm_rx_trap_lif_set(unit2, 0, &lif_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in Default Egress bcm_rx_trap_lif_set\n");
        return rv;
    }

    printf("Done configuring egress port match\n");
    return rv;
}


/* Delete the port matches and gports*/
int 
port_match_ingress_cleaup(
    int *units_ids,
    int nof_units,
    int no_initial_match)
{
    int rv;
    int unit,i;
    int unit1 = units_ids[0];
    int unit2 = units_ids[1];

    /* Delete multiple port match critreia from the existing vlan port */
    if (no_initial_match) {
        /* Add the match criteria we've missed by setting the vlan port on _PORT_MATCH_NONE */
        rv = port_match_add_remove(unit1,
                       BCM_PORT_MATCH_PORT_CVLAN,
                       BCM_PORT_MATCH_INGRESS_ONLY,
                       port_match_info_1.in_vids[0],
                       port_match_info_1.ports[0],
                       port_match_info_1.in_gport,
                       0);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_match_add_remove\n");
            return rv;
        }
    }

    /* Add multiple match critreia for the existing vlan port */
    rv = port_match_add_remove(unit1,
                   BCM_PORT_MATCH_PORT_CVLAN,
                   BCM_PORT_MATCH_INGRESS_ONLY,
                   port_match_info_1.in_vids[1],
                   port_match_info_1.ports[0],
                   port_match_info_1.in_gport,
                   0);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_match_add_remove\n");
        return rv;
    }

    /* Delete VLAN port for Simple bridge */
    rv = bcm_vlan_port_destroy(unit1, port_match_info_1.in_gport);
    if (rv != 0){
        printf("Error, in bcm_vlan_port_destroy\n");
        return rv;
    }

    for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
        rv = vlan_translation_default_mode_destroy(unit);
        if (rv != 0) {
            printf("Error, in vlan_translation_default_mode_destroy\n");
            return rv;
        }
    }

    return rv;
}
