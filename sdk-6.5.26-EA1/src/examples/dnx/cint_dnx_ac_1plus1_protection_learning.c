/* $Id: cint_dnx_ac_1plus1_protection_learning.c
 * **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for 1+1 Ingress Protection learning                                                                                 *
 *                                                                                                                                                  *
 * Packets can be sent from the In-ACs towards the Out-AC to create learning entries for them.                                                      *
 * Packets from the Protection In-ACs (Primary & Secondary) are expected to invoke MC learning of both ACs.                                         *
 * Packets send from the Out-AC with the Protection In-AC MAC DA are expected to be forwarded to the ACs that participate in the learnt MC group,   *
 *   while if no learning was perfomed, flooding can address any of the defined ACs.                                                                *
 * A learning MC Group may be modified (on both Protection In-ACs) and take effect once a packet is sent from either of them.                       *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                  ____________________________________                                                            *
 *                                                 |              ____________          |                                                           *
 *                           Primary In-AC         |             |            |         |                                                           *
 *                           --------------------  | <--|        |  Learning  |         |                                                           *
 *                                                 |    |------- |  MC Group  |         |                                                           *
 *                           Secondary In-AC       |    | <-|    |____________|         |                                                           *
 *                           --------------------- | <--|   |                           |                                                           *
 *                                                 |        |     ____________      |-->|       Out-AC                                              *
 *                                                 |        |    |            |     |   | ------------                                              *
 *                                                 |        |    |  Flooding  |     |   |                                                           *
 *                           Non-Protected In-AC   | <---------- |  MC Group  |-----|   |                                                           *
 *                           --------------------- |             |____________|         |                                                           *
 *                                                 |                                    |                                                           *
 *                                                 |____________________________________|                                                           *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * ./bcm.user                                                                                                                                       *
 * cd ../../../../regress/bcm                                                                                                                       *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c                                                                                    *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c                                                                                 *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c                                                                                        *
 * cint ../../src/./examples/dnx/cint_dnx_ac_1plus1_protection_learning.c                                                                           *
 * cint                                                                                                                                             *
 * ac_1plus1_protection__learning_with_ports__start_run(0,0,1811939528,1811939529,1811939530,1811939531);                                           *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * Stage 1: Send a packet from Out-Port. Expect to receive flooding to all 3 In-Ports.                                                              *
 * tx 1 psrc=202 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 2: Send a packet from the Non-Protected In-Port to the Out-Port.                                                                           *
 * tx 1 psrc=203 data=0x00110000001300550000006691000008ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00110000001300550000006691000008ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 3: Send a packet from Out-Port. Expect to receive flooding to all 3 In-Ports.                                                              *
 * tx 1 psrc=202 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 4: Send a packet from the In-Primary-Port. Expect to receive on the Out-Port.                                                              *
 * tx 1 psrc=200 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 5: Send a packet from Out-Port. Expect to receive on the two learnt protected In-Ports.                                                    *
 * tx 1 psrc=202 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 6: Change the failover_mc_group to the flooding group and send the same packet                                                             *
 * tx 1 psrc=202 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 7: Send a packet from the In-Primary-Port. Expect to receive on the Out-Port.                                                              *
 * tx 1 psrc=200 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * Stage 8: Send a packet from Out-Port. Expect to receive on all three flooding ports                                                              *
 * tx 1 psrc=202 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * cint                                                                                                                                                *
 * ac_1plus1_protection_learning__cleanup(0,0);                                                                                                        *
 * exit;                                                                                                                                            *
 *                                                                                                                                                    *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |  6/7/8/9   |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_ac_1plus1_protection_learning', before calling the main    *
 * function. In order to re-write only part of the values, call 'ac_1plus1_protection_learning_struct_get(1plus1_protection_learning_s)'            *
 * function and re-write the values prior to calling the main function.                                                                             *
 *                                                                                                                                                  *
 * Test name: AT_Dnx_Cint_failover_ac_1plus1_protection_learning                                                                                    *
 *                                                                                                                                                  *
 ************************************************************************************************************************************************** */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
int FAILOVER_PRIMARY=0;
int FAILOVER_SECONDARY=1;
int NOF_FAILOVER_IDS=2;
int outer_tpid =0x9100;

struct protection_ac_s {
    bcm_gport_t port;
    bcm_vlan_t  vlan;
    bcm_gport_t vlan_port_id;
    bcm_if_t encap_id;
};

/*  Main Struct  */
struct ac_1plus1_protection_learning_s {
    protection_ac_s in_ac[NOF_FAILOVER_IDS];
    protection_ac_s out_ac;
    protection_ac_s non_protected_in_ac;
    bcm_mac_t mac_address_out;
    bcm_mac_t mac_address_in;
    bcm_mac_t mac_address_in_non_protected;
    bcm_multicast_t failover_mc_group;
    bcm_vlan_t vsi;
    bcm_failover_t ingress_failover_id;
    bcm_if_t fec_id;
};

/* Initialization of a global struct*/
ac_1plus1_protection_learning_s g_ac_1plus1_protection_learning = {
        /*  AC configurations
            Phy Port    VLAN    vlan_port_id    */
           {{ 200,      9,         0x44800010 }, /* In-ACs */
            { 201,      6,         0x44001001 }},
            { 202,      7,         0x44001002 }, /* Out-AC */
            { 203,      8,         0x44001020 }, /* Non-Protected-AC */
            /* Additional parameters */
            {0x00, 0x11, 0x00, 0x00, 0x00, 0x13},   /* MAC Address Out */
            {0x00, 0x33, 0x00, 0x00, 0x00, 0x33},   /* MAC Address In */
            {0x00, 0x55, 0x00, 0x00, 0x00, 0x66},   /* MAC Address Non-Protected */
            50,                                     /* faliover mc_group */
            20,                                     /* VSI */
            2,                                      /* Ingress failover_id */
            0xCCCE};                                /* FEC ID*/

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_ac_1plus1_protection_learning
 */
int ac_1plus1_protection_learning_init(int unit,ac_1plus1_protection_learning_s *params) {
    int rv, flags;
    bcm_multicast_t mc_group;

    /* Create an Ingress failover ID */
    flags = BCM_FAILOVER_WITH_ID | BCM_FAILOVER_INGRESS;
    rv = bcm_failover_create(unit, flags, &g_ac_1plus1_protection_learning.ingress_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1plus1_protection_learning_init failed during Ingress bcm_failover_create, rv - %d\n", rv);
        return rv;
    }

    /* Create a vswitch */
    rv = bcm_vswitch_create_with_id(unit, g_ac_1plus1_protection_learning.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1plus1_protection_learning_init failed during bcm_vswitch_create_with_id for vsi %d, rv - %d\n", g_ac_1plus1_protection_learning.vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = g_ac_1plus1_protection_learning.vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }

    /* Create a failover MC Group for the In-Ports */
    mc_group = g_ac_1plus1_protection_learning.failover_mc_group;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }
    BCM_L3_ITF_SET(g_ac_1plus1_protection_learning.fec_id,  BCM_L3_ITF_TYPE_FEC, 0x1200);

    return BCM_E_NONE;
}

/* Configuration of a non-protected AC.
 * Define the AC and attach it to a vswitch.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single non-protected AC.
 */
int ac_1plus1_protection_learning__set_non_protected_ac(int unit, protection_ac_s *ac_info) {
    bcm_vlan_port_t vlan_port;
    int rv, mc_group;
    int is_local=1, local_port;
    int encap_id;

    rv = system_port_is_local(unit, ac_info->port, &local_port, &is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv = %d\n", rv);
        return rv;
    }
    printf("system port[%d]-local_port[%d]-is_local[%d]\n", ac_info->port, local_port, is_local);

    /* Set the VLAN-Domain for the port */
    rv = bcm_port_class_set(unit, ac_info->port, bcmPortClassId, local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n", ac_info->port, rv);
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_info->port;
    vlan_port.match_vlan = ac_info->vlan;
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port.vsi = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, 0, ac_info->vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error failed during bcm_vlan_port_create, rv - %d\n", rv);
        return rv;
    }

    ac_info->vlan_port_id = vlan_port.vlan_port_id;
    ac_info->encap_id = vlan_port.encap_id;
    rv = bcm_vlan_gport_add(unit, ac_info->vlan, ac_info->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n", ac_info->vlan, ac_info->port, rv);
        return rv;
    }

    /* Attach the Out-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, g_ac_1plus1_protection_learning.vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n", vlan_port.vlan_port_id, vlan_port.vsi, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    mc_group = g_ac_1plus1_protection_learning.vsi;
    encap_id = vlan_port.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit, mc_group, &vlan_port.port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", vlan_port.vlan_port_id, mc_group, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, ac_info->port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n", ac_info->port, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/* Configuration of the Secondary In-AC.
 * Define the Secondary In-AC and attach them to a vswitch and a MC group.
 * The AC is defined with the appropriate failover_ids Ingress Protection.
 * INPUT:
 *   ac_1plus1_protection_learning_s: Configuration info for a Secondary In-AC.
 */
int ac_1plus1_protection_learning__set_secondary_in_ac(int unit1, int unit2,ac_1plus1_protection_learning_s *ac_1plus1_protection_learning_info) {
    bcm_vlan_port_t in_ac;
    bcm_vlan_port_t in_ac_info;
    int rv, mc_group;
    int work_is_local=1, protect_is_local=1, local_port;
    bcm_gport_t fec_gport_id = BCM_GPORT_INVALID;
    int encap_id;

    rv = system_port_is_local(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, &local_port, &protect_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv - %d\n", rv);
        return rv;
    }
    printf("Protect:in_ac.port[%d]-local_port[%d]-is_local[%d]\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, local_port, protect_is_local);

    /* Set the VLAN-Domain for the ports */
    rv = bcm_port_class_set(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, bcmPortClassId, local_port);
    if (rv != BCM_E_NONE) {
        printf("%Error during bcm_port_class_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    rv = bcm_vlan_create(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_create for vlan - %d , rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan,  rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    /* Open up the ports for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    /* Create the Protecting In-AC */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.port = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port;
    in_ac.match_vlan = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan;
    in_ac.vsi = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);
    in_ac.ingress_failover_id = ac_1plus1_protection_learning_info->ingress_failover_id;
    int failover_mc_group;
    BCM_MULTICAST_L2_SET(failover_mc_group,ac_1plus1_protection_learning_info->failover_mc_group);
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id, ac_1plus1_protection_learning_info->fec_id);
    in_ac.failover_port_id = 0;
    in_ac.failover_mc_group = failover_mc_group;
    rv = bcm_vlan_port_create(unit2, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create during Secondary In-AC creation, rv - %d\n", rv);
        return rv;
    }

    ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id = in_ac.vlan_port_id;
    ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].encap_id = in_ac.encap_id;
    printf("in_ac[FAILOVER_SECONDARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);

    /* Attach the Secondary In-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit2, ac_1plus1_protection_learning_info->vsi, in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add during Secondary bcm_vswitch_port_add to vsi %d, rv - %d\n", ac_1plus1_protection_learning_info->vsi, rv);
        return rv;
    }

    /* Attach to a flooding group */
    mc_group = ac_1plus1_protection_learning_info->vsi;
    encap_id = in_ac.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit2, mc_group, &in_ac.port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", in_ac.vlan_port_id, mc_group, rv);
        return rv;
    }

    /* Attach to a failover MC group */
    mc_group = ac_1plus1_protection_learning_info->failover_mc_group;
    encap_id = in_ac.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit2, mc_group, &in_ac.port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", in_ac.vlan_port_id, mc_group, rv);
        return rv;
    }

    if (unit1 != unit2) {
        rv = bcm_vlan_create(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_create for vlan - %d , rv - %d\n",
                   ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan,  rv);
            return rv;
        }

        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit1, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, flooding multicast__add_multicast_entry, rv - %d\n", rv);
            return rv;
        }

        /* Attach to a failover MC group */
        mc_group = g_ac_1plus1_protection_learning.failover_mc_group;
        rv = multicast__add_multicast_entry(unit1, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, failover multicast__add_multicast_entry, rv - %d\n", rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Configuration of the Primary In-AC.
 * Define the Primary In-AC and attach them to a vswitch and a MC group.
 * The AC is defined with the appropriate failover_ids Ingress Protection.
 * INPUT:
 *   ac_1plus1_protection_learning_s: Configuration info for Primary In-AC.
 */
int ac_1plus1_protection_learning__set_primary_in_ac(int unit1, int unit2,ac_1plus1_protection_learning_s *ac_1plus1_protection_learning_info) {
    bcm_vlan_port_t in_ac;
    bcm_vlan_port_t in_ac_info;
    int rv, mc_group;
    int work_is_local=1, protect_is_local=1, local_port;
    bcm_gport_t fec_gport_id = BCM_GPORT_INVALID;
    int encap_id;

    rv = system_port_is_local(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, &local_port, &work_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in system_port_is_local for JR2, rv - %d\n", rv);
        return rv;
    }
    printf("Working: in_ac.port[%d]-local_port[%d]-is_local[%d]\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, local_port, work_is_local);

    rv = bcm_port_class_set(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, bcmPortClassId, local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
            ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }

    rv = bcm_port_control_set(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }

    /* Create the Primary In-AC */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.port = ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port;
    in_ac.match_vlan = ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan;
    in_ac.vsi = 0;

    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);

    int failover_mc_group;
    BCM_MULTICAST_L2_SET(failover_mc_group,ac_1plus1_protection_learning_info->failover_mc_group);
    in_ac.ingress_failover_id = ac_1plus1_protection_learning_info->ingress_failover_id;
    in_ac.ingress_failover_port_id = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id ;

    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id, ac_1plus1_protection_learning_info->fec_id);
    in_ac.failover_port_id = 0;
    in_ac.failover_mc_group = failover_mc_group;
    rv = bcm_vlan_port_create(unit1, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create during Primary In-AC creation, rv - %d\n", rv);
        return rv;
    }

    ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan_port_id = in_ac.vlan_port_id;
    ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].encap_id = in_ac.encap_id;
    printf("in_ac[FAILOVER_PRIMARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);

    /* Attach the Primary In-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit1, ac_1plus1_protection_learning_info->vsi, in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error during Primary bcm_vswitch_port_add for vsi %d, rv - %d\n", ac_1plus1_protection_learning_info->vsi, rv);
        return rv;
    }

    /* Attach to a flooding group */
    mc_group = ac_1plus1_protection_learning_info->vsi;
    encap_id = in_ac.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit1, mc_group, &in_ac.port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", in_ac.vlan_port_id, mc_group, rv);
        return rv;
    }

    /* Attach to a failover MC group */
    mc_group = ac_1plus1_protection_learning_info->failover_mc_group;
    encap_id = in_ac.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit1, mc_group, &in_ac.port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", in_ac.vlan_port_id, mc_group, rv);
        return rv;
    }

    if (unit2 != unit1) {
        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit2, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, flooding multicast__add_multicast_entry, rv - %d\n", rv);
            return rv;
        }

        /* Attach to a failover MC group */
        mc_group = g_ac_1plus1_protection_learning.failover_mc_group;
        rv = multicast__add_multicast_entry(unit2, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, failover multicast__add_multicast_entry, rv - %d\n", rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Retrieve the initial confgiuration for the example.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single In-AC.
 */
void ac_1plus1_protection_learning__struct_get(ac_1plus1_protection_learning_s *param) {
    sal_memcpy(param, &g_ac_1plus1_protection_learning, sizeof(*param));
    return;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT:
 *   unit1: out_ac, primary_ac locating
 *   unit2: non_protected_in_ac, secondary_ac locating
 *   params: Configuration info for running the example.
 */
int ac_1plus1_protection_learning__start_run(int unit1, int unit2, ac_1plus1_protection_learning_s *params) {
    int rv, unit, mc_group;
    int is_local, local_port;
    uint32 *nof_rifs;
    char *proc_name;

    if (params != NULL) {
       sal_memcpy(&g_ac_1plus1_protection_learning, params, sizeof(g_ac_1plus1_protection_learning));
    }

    /* LIF idx must be greater than nof RIFs */
     nof_rifs=dnxc_data_get(unit1, "l3", "rif", "nof_rifs", NULL);
     g_ac_1plus1_protection_learning.out_ac.vlan_port_id += *nof_rifs;
     g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id += *nof_rifs;

     nof_rifs=dnxc_data_get(unit2, "l3", "rif", "nof_rifs", NULL);
     g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id += *nof_rifs;
     g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id += *nof_rifs;

    /* Initialize the test parameters */
    rv = ac_1plus1_protection_learning_init(unit1,params);
    if (rv != BCM_E_NONE) {
        printf("Error in ac_1plus1_protection_learning_init, rv - %d\n", rv);
        return rv;
    }
    if (unit2 != unit1) {
        rv = ac_1plus1_protection_learning_init(unit2,params);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1plus1_protection_learning_init, rv - %d\n", rv);
            return rv;
        }
    }

    /* Configure the Out-AC */
    proc_name = "ac_1plus1_protection_learning__set_non_protected_ac";
    rv = ac_1plus1_protection_learning__set_non_protected_ac(unit1, &g_ac_1plus1_protection_learning.out_ac);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create an Out-AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    printf("out_ac.vlan_port_id: %#x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);

    if (unit2 != unit1) {
        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit2, mc_group, g_ac_1plus1_protection_learning.out_ac.port, &g_ac_1plus1_protection_learning.out_ac.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id, mc_group, rv);
            return rv;
        }
    }

    /* Configure the Non-Protected In-AC */
    rv = ac_1plus1_protection_learning__set_non_protected_ac(unit2, &g_ac_1plus1_protection_learning.non_protected_in_ac);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create a non-protected In-AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    printf("non_protected_in_ac.vlan_port_id: %#x\n", g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id);

    if (unit2 !=unit1) {
        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit1, mc_group, g_ac_1plus1_protection_learning.non_protected_in_ac.port, &g_ac_1plus1_protection_learning.non_protected_in_ac.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error during multicast__add_multicast_entry of gport 0x%08x to mc_group %d, rv - %d\n", g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id, mc_group, rv);
            return rv;
        }
    }

    /* Configure the secondary In-ACs */
    proc_name = "ac_1plus1_protection_learning__set_secondary_in_ac";
    rv = ac_1plus1_protection_learning__set_secondary_in_ac(unit1,unit2, &g_ac_1plus1_protection_learning);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create the protected In-ACs, rv - %d\n", proc_name, rv);
        return rv;
    }

    /* Configure the primary In-ACs */
    proc_name = "ac_1plus1_protection_learning__set_primary_in_ac";
    rv = ac_1plus1_protection_learning__set_primary_in_ac(unit1,unit2, &g_ac_1plus1_protection_learning);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create the protecting In-ACs, rv - %d\n", proc_name, rv);
        return rv;
    }
    return BCM_E_NONE;
}

/* This function runs the main test function with specified ports
 *
 * INPUT: in_port_primary - ingress primary port
 *        in_port_secondary - ingress secondary port
 *        out_port - egress port
 *        non_protected_in_port - Non-Protected In-Port
 */
int ac_1plus1_protection__learning_with_ports__start_run(int unit1, int unit2, int in_port_primary, int in_port_secondary, int out_port, int non_protected_in_port) {
    int rv;
    ac_1plus1_protection_learning_s param;
    ac_1plus1_protection_learning__struct_get(&param);

    param.in_ac[FAILOVER_PRIMARY].port = in_port_primary;
    param.in_ac[FAILOVER_SECONDARY].port = in_port_secondary;
    param.out_ac.port = out_port;
    param.non_protected_in_ac.port = non_protected_in_port;

    return ac_1plus1_protection_learning__start_run(unit1, unit2, &param);
}

/* Performs failover mc setting for the two protection In-ACs to modify the learning object
 *
 * INPUT: failover_mc_group  - The MC group to set as the learning object
 */
int ac_1plus1_protection_learning__failover_mc_set(int unit1, int unit2, int failover_mc_group) {
    bcm_vlan_port_t in_ac;
    int rv;

    /* Retrieve the Secondary In-AC and Replace the failover_mc_group value */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.vlan_port_id = g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id;
    rv = bcm_vlan_port_find(unit2, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_find for Secondary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    in_ac.failover_mc_group = failover_mc_group;
    in_ac.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    rv = bcm_vlan_port_create(unit2, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, replacing failover_mc_group Secondary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    /* Retrieve the Primary In-AC and Replace the failover_mc_group value */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.vlan_port_id = g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id;
    rv = bcm_vlan_port_find(unit1, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_find for the Primary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    in_ac.failover_mc_group = 0;
    in_ac.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    rv = bcm_vlan_port_create(unit1, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, replacing failover_mc_group for Primary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}

int ac_1plus1_protection_learning__cleanup(int unit1, int unit2) {
    int rv;

    rv = bcm_vlan_port_destroy(unit2, g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit1, g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit2, g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit1, g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_failover_destroy(unit1, g_ac_1plus1_protection_learning.ingress_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_multicast_destroy(unit1, g_ac_1plus1_protection_learning.failover_mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_multicast_destroy(unit1, g_ac_1plus1_protection_learning.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    if (unit2 != unit1) {
        rv = bcm_failover_destroy(unit2, g_ac_1plus1_protection_learning.ingress_failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_failover_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_multicast_destroy(unit2, g_ac_1plus1_protection_learning.failover_mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_multicast_destroy(unit2, g_ac_1plus1_protection_learning.vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }
    }

    return BCM_E_NONE;
}
