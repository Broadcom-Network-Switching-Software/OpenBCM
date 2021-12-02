/* $Id: cint_dnx_ac_1to1_coupled_protection.c
 *
 * **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides two protection examples:                                                                                                       *
 *                                                                                                                                                  *
 * Example 1:                                                                                                                                       *
 * ==========                                                                                                                                       *
 * AC 1:1 Protection for UC & MC traffic.                                                                                                           *
 *                                                                                                                                                  *
 * Packets can be sent from the In-AC towards the Out-ACs:                                                                                          *
 *   UC - Unicast traffic is achieved by using the FEC Protection and sending a packet with a known DA.                                             *
 *   MC - Multicast Protection is achieved by defining a MC group an using Egress Protection for unknown DA packets.                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                  ________________________________                                                                *
 *                                                 |                   _____        |                                                               *
 *                           In-AC                 |                  |     | ----> | -----                                                         *
 *                               ----------------> | -   -   -   -  > | FEC |       |      |                                                        *
 *                                                 |                  |_____| ----> | ---  |                                                        *
 *                                                 |                                |    | |                                                        *
 *                                                 |                                |    | |                                                        *
 *                           Working Out-AC        |      ______                    |    | |                                                        *
 *                               <---------------- | < --|  Eg  | -   -   -   -  -  | <--  |                                                        *
 *                           Protecting Out-AC     |     | Prot |                   |      |                                                        *
 *                               <---------------- | < --|______| -   -   -   -  -  | <----                                                         *
 *                                                 |                                |                                                               *
 *                                                 |________________________________|                                                               *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * Example 2:                                                                                                                                       *
 * ==========                                                                                                                                       *
 * AC 1:1 & 1+1 Protection for UC traffic                                                                                                           *
 *                                                                                                                                                  *
 * Packets can be sent from the In-AC towards the Out-ACs (FEC failover connection)                                                                 *
 * Packets can be sent from the Out-ACs (Ingress failover connection) towards the In-AC                                                             *
 *                                                                                                                                                  *
 *                                                  ________________________________                                                                *
 *                             Working In/Out-AC   |     _____                      |                                                               *
 *                                ---------------> |--->| In  |\                    |                                                               *
 *                                                 |    | LIF | \                   |                                                               *
 *                                                 |    |_____|   --------------->  | - - - -> In-AC                                                *
 *                          Protecting In/Out-AC   |    | In  | /                   |                                                               *
 *                                ---------------> |--->| LIF |/                    |                                                               *
 *                                                 |    |_____|                     |                                                               *
 *                                                 |                   _____        |                                                               *
 *                                      In-AC      |                  |     | ----> |- - > Working In/Out-AC                                        *
 *                               ----------------> | ---------------> | FEC |       |                                                               *
 *                                                 |                  |_____| ----> |- - > Protecting In/Out-AC                                     *
 *                                                 |                                |                                                               *
 *                                                 |________________________________|                                                               *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * Configuration:                                                                                                                                   *
 * For Example 1 set ac_1to1_coupled_protection->is_egress = 1 (default)                                                                            *
 * For Example 2 set ac_1to1_coupled_protection->is_egress = 0                                                                                      *
 *                                                                                                                                                  *
 *************************************************AC Multicast and Unicast Protection with static MAC************************************************
 * run:                                                                                                                                             *
 * ./bcm.user                                                                                                                                       *
 * cd ../../../../regress/bcm                                                                                                                       *
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c                                                                                       *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c                                                                                    *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c                                                                                 *
 * cint ../../src/./examples/dnx/cint_dnx_ac_1to1_coupled_protection.c                                                                              *
 * cint                                                                                                                                             *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * g_ac_1to1_coupled_protection.dynamic_learning=0;                                                                                                 *
 * g_ac_1to1_coupled_protection.is_ext_lif=0;                                                                                                       *
 * ac_1to1_coupled_protection_with_ports__start_run(units_ids,1,1811939529,1811939530,1811939531,1);                                                *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * stage 1: Send a packet with unknown DA. Expect to receive on the Working port.                                                                   *
 * tx 1 psrc=201 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * stage 2: Send a packet with known DA. Expect to receive on the none protected port.                                                              *
 * tx 1 psrc=202 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,0,0);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * stage 3: Set failover state only for Egress Protection and Send a packet with known DA. Expect no received packets.                              *
 * tx 1 psrc=201 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,1,0);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * stage4: Set failover state for FEC Protection as well and Send a packet with known DA. Expect to receive on the Protecting port.                 *
 * tx 1 psrc=201 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * stage5: Send a packet with unknown DA. Expect to receive on the Protecting port.                                                                 *
 * tx 1 psrc=201 data=0x000073c2329100330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x000073c2329100330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,0,1);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,1,1);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * stage6: Clear failovers and Send a packet with DA. Expect to receive on the Working port.                                                        *
 * tx 1 psrc=201 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                      *
 *                                                                                                                                                  *
 * Received packets on unit 0 should be:                                                                                                            *
 * Data: 0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000             *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection__cleanup(units_ids,1);                                                                                                *
 * exit;                                                                                                                                            *
 *************************************************AC 1:1 and 1+1 Protection based on trunk***********************************************************
 * run:                                                                                                                                             *
 * ./bcm.user                                                                                                                                       *
 * cd ../../../../regress/bcm                                                                                                                       *
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c                                                                                       *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c                                                                                    *
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c                                                                                 *
 * cint ../../src/./examples/dnx/cint_dnx_ac_1to1_coupled_protection.c                                                                              *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__start_run(units_ids,1,234881027,234881028,234881029,0);                                                   *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * Stage 1: Send a UC packet from P1. Expect to receive on P2 (Out-Working-Port).                                                                   *
 * tx 1 psrc=13 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 1: Send a UC packet from P1. Expect to receive on P2 (Out-Working-Port).                                                                   *
 * tx 1 psrc=17 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 2: Send a UC packet from P2 (In-Working-Port). Expect to receive on P1.                                                                    *
 * tx 1 psrc=16 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 2: Send a UC packet from P2 (In-Working-Port). Expect to receive on P1.                                                                    *
 * tx 1 psrc=15 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 3: Send a UC packet from P3 (In-Protecting-Port). Expect no received packets.                                                              *
 * tx 1 psrc=14 data=0x00330000003300110000001391000006ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,1,0);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * Stage 4: Set failover state for FEC Protection. Send a UC packet from P1. Expect to receive on P3 (Out-Protecting-Port).                         *
 * tx 1 psrc=13 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 4: Set failover state for FEC Protection. Send a UC packet from P1. Expect to receive on P3 (Out-Protecting-Port).                         *
 * tx 1 psrc=17 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 5: Send a UC packet from P2 (In-Working-Port). Expect to receive on P1.                                                                    *
 * tx 1 psrc=16 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 5: Send a UC packet from P2 (In-Working-Port). Expect to receive on P1.                                                                    *
 * tx 1 psrc=15 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 6: Send a UC packet from P3 (In-Protecting-Port). Expect no received packets.                                                              *
 * tx 1 psrc=14 data=0x00330000003300110000001391000006ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,0,0);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * Stage 7: et failover state for Ingress Protection. Send a UC packet from P2 (In-Working-Port). Expect no received packets.                       *
 * tx 1 psrc=16 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 7: et failover state for Ingress Protection. Send a UC packet from P2 (In-Working-Port). Expect no received packets.                       *
 * tx 1 psrc=15 data=0x00330000003300110000001391000007ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 8: Send a UC packet from P3 (In-Protecting-Port). Expect to receive on P1.                                                                 *
 * tx 1 psrc=14 data=0x00330000003300110000001391000006ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 9: Send a UC packet from P1. Expect to receive on P3 (Out-Protecting-Port).                                                                *
 * tx 1 psrc=13 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 9: Send a UC packet from P1. Expect to receive on P3 (Out-Protecting-Port).                                                                *
 * tx 1 psrc=17 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,0,1);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * cint                                                                                                                                             *
 * ac_1to1_coupled_protection_with_ports__failover_set(units_ids,1,1,1);                                                                            *
 * exit;                                                                                                                                            *
 *                                                                                                                                                  *
 * Stage 10 :Clear failover state for FEC and Ingress Protection. Send a UC packet from P1. Expect to receive on P2 (Out-Working-Port).             *
 * tx 1 psrc=13 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 * Stage 10 :Clear failover state for FEC and Ingress Protection. Send a UC packet from P1. Expect to receive on P2 (Out-Working-Port).             *
 * tx 1 psrc=17 data=0x00110000001300330000003391000009ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20                       *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 ************************************************************************************************************************************************** */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
int FAILOVER_PROTECTING=0;
int FAILOVER_WORKING=1;
int NOF_FAILOVER_IDS=2;

struct protection_ac_s {
    bcm_gport_t port;
    bcm_vlan_t  vlan;
    bcm_gport_t vlan_port_id;
    bcm_gport_t encap_id;
};

/*  Main Struct  */
struct ac_1to1_coupled_protection_s {
    protection_ac_s in_ac;
    protection_ac_s out_ac[NOF_FAILOVER_IDS];
    bcm_mac_t mac_address_out;
    bcm_mac_t mac_address_in;
    int is_egress;
    bcm_vlan_t vsi;
    bcm_failover_t fec_failover_id;
    bcm_failover_t eg_in_failover_id;
    int dynamic_learning;
    int is_ext_lif;
};

int fec_standby=0x2000A563;
int fec_primary=0x2000A562;

/* Initialization of global struct */
ac_1to1_coupled_protection_s g_ac_1to1_coupled_protection = {
        /*  AC configurations
            Phy Port    VLAN    vlan_port_id    encap_id   */
            { 200,      9,         0x44800010, 0   }, /* In-AC  */
           {{ 201,      6,         0x44001011, 0   }, /* Out-ACs */
            { 202,      7,         0x44001012, 0   }},

            /* Additional parameters */
            {0x00, 0x11, 0x00, 0x00, 0x00, 0x13},   /* MAC Address Out */
            {0x00, 0x33, 0x00, 0x00, 0x00, 0x33},   /* MAC Address In */
            1,                                      /* Is Egress */
            20,                                     /* VSI */
            1,                                      /* FEC failover_id */
            2,                                      /* Egress/Ingress failover_id */
            0,                                      /* dynamic_learning */
            0};                                     /* Extend lif */


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_ac_1to1_coupled_protection
 */
int ac_1to1_coupled_protection_init(int unit, ac_1to1_coupled_protection_s *params) {
    int rv, flags;
    bcm_multicast_t mc_group;
    uint32 *nof_rifs;

    if (params != NULL) {
       sal_memcpy(&g_ac_1to1_coupled_protection, params, sizeof(g_ac_1to1_coupled_protection));
    }

    /* LIF idx must be greater than nof RIFs */
    nof_rifs=dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL);
    g_ac_1to1_coupled_protection.in_ac.vlan_port_id += *nof_rifs;

    /* Create a FEC failover ID for UC */
    rv = bcm_failover_create(unit, (BCM_FAILOVER_WITH_ID | BCM_FAILOVER_FEC), &g_ac_1to1_coupled_protection.fec_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1to1_coupled_protection_init failed during FEC bcm_failover_create, rv - %d\n", rv);
        return rv;
    }

    /* Create Egress/Ingress failover ID */
    rv = bcm_failover_create(unit, (BCM_FAILOVER_WITH_ID | (params->is_egress ? BCM_FAILOVER_ENCAP : BCM_FAILOVER_INGRESS)), &g_ac_1to1_coupled_protection.eg_in_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1to1_coupled_protection_init failed during Ingress/Egress bcm_failover_create, rv - %d\n", rv);
        return rv;

    }

    /* Create a vswitch */
    rv = bcm_vswitch_create_with_id(unit, g_ac_1to1_coupled_protection.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1to1_coupled_protection_init failed during bcm_vswitch_create_with_id for vsi %d, rv - %d\n", g_ac_1to1_coupled_protection.vsi, rv);
        return rv;
    }

    /* Create MC Group */
    mc_group = g_ac_1to1_coupled_protection.vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }

    return BCM_E_NONE;
}


/* Configuration of a protection In-AC.
 * Define the In-AC and attach it to a vswitch.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single In-AC.
 */
int ac_1to1_coupled_protection__set_in_ac(int unit, ac_1to1_coupled_protection_s *ac_1to1_coupled_protection_info) {
    bcm_vlan_port_t in_ac;
    bcm_if_t encap_id;
    int rv;
    int is_local=1, local_port;

    bcm_vlan_port_t_init(&in_ac);
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.port = ac_1to1_coupled_protection_info->in_ac.port;
    in_ac.match_vlan = ac_1to1_coupled_protection_info->in_ac.vlan;
    in_ac.vsi = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1to1_coupled_protection_info->in_ac.vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);

    rv = system_port_is_local(unit, in_ac.port, &local_port, &is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2\n");
        return rv;
    }
    printf(">>>In_ac.port[%d]-local_port[%d]-is_local[%d]\n", in_ac.port, local_port, is_local);
    if (!is_local) {
        printf(">>>Skip for In_AC create...\n");
        return BCM_E_NONE;
    }

    rv = bcm_vlan_port_create(unit, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1to1_coupled_protection__set_in_ac failed during bcm_vlan_port_create, rv - %d\n", rv);
        return rv;
    }
    ac_1to1_coupled_protection_info->in_ac.vlan_port_id = in_ac.vlan_port_id;
    rv = bcm_vlan_gport_add(unit, ac_1to1_coupled_protection_info->in_ac.vlan, ac_1to1_coupled_protection_info->in_ac.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    /* Attach the In-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, ac_1to1_coupled_protection_info->vsi, in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1to1_coupled_protection__set_in_ac failed during bcm_vswitch_port_add for vsi %d, rv - %d\n", in_ac.vsi, rv);
        return rv;
    }
    printf("In-AC = %d, encap_id - %d\n", in_ac.vlan_port_id, in_ac.encap_id);
    return BCM_E_NONE;
}

int create_protection_fec(int unit, ac_1to1_coupled_protection_s *ac_1to1_coupled_protection_info)
{
    int rv=BCM_E_NONE;
    bcm_l3_egress_t fec;
    int is_local;

    /*
     * Creates backup FEC:
     */
    printf(">>>Create secondary FEC for unit[%d]...\n",unit);
    bcm_l3_egress_t_init(&fec);
    fec.intf=ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan_port_id;
    fec.vlan=ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan;
    fec.port=ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].port;
    fec.failover_id=ac_1to1_coupled_protection_info->fec_failover_id;
    fec.failover_if_id =0;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, fec, &fec_standby);
    if (rv != BCM_E_NONE)
    {
        printf("Error, protect, in bcm_l3_egress_create\n");
        return rv;
    }

    /*
     * Creates primary FEC:
     */
    printf(">>>Create primary FEC for unit[%d]...\n",unit);
    bcm_l3_egress_t_init(&fec);
    fec.intf=ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan_port_id;
    fec.vlan=ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan;
    fec.port=ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].port;
    fec.failover_id=ac_1to1_coupled_protection_info->fec_failover_id;
    fec.failover_if_id =fec_standby;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, fec, &fec_primary);
    if (rv != BCM_E_NONE)
    {
        printf("Error, working, in bcm_l3_egress_create\n");
        return rv;
    }

    return rv;
}


/* Configuration of the protection Out-ACs.
 * Define the Working & Protecting Out-ACs and attach them to a vswitch and MC group.
 * The ACs are defined with the appropriate failover_ids for FEC & Egress Protection.
 * INPUT:
 *   protection_ac_s: Configuration info for a couple of protection Out-ACs.
 */
int ac_1to1_coupled_protection__set_protection_acs(int unit, ac_1to1_coupled_protection_s *ac_1to1_coupled_protection_info) {
    bcm_vlan_port_t out_ac;
    bcm_vlan_port_t out_ac_info;
    bcm_if_t encap_id;
    int rv, mc_group;
    int check_pass = 1;
    bcm_l3_egress_t fec;
    int work_is_local=1, protect_is_local=1, local_port;

    /* Create the Protecting Out-AC */
    bcm_vlan_port_t_init(&out_ac);
    out_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_ac.flags = BCM_VLAN_PORT_WITH_ID;
    out_ac.port = ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].port;
    out_ac.match_vlan = ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan;
    out_ac.vsi = 0;
    out_ac.failover_id = 0;
    if (g_ac_1to1_coupled_protection.is_ext_lif == 1)
    {
        out_ac.group = 2;
    }
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(out_ac.failover_port_id, fec_standby);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_ac.vlan_port_id, 0, ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_ac.vlan_port_id, out_ac.vlan_port_id);

    if (ac_1to1_coupled_protection_info->is_egress) {
        out_ac.flags |= BCM_VLAN_PORT_EGRESS_PROTECTION;
        out_ac.egress_failover_id = ac_1to1_coupled_protection_info->eg_in_failover_id;
    } else {
        out_ac.ingress_failover_id = ac_1to1_coupled_protection_info->eg_in_failover_id;
    }

    rv = system_port_is_local(unit, ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].port, &local_port, &protect_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2\n");
        return rv;
    }
    printf(">>>Protect:out_ac.port[%d]-local_port[%d]-is_local[%d]\n", out_ac.port, local_port, protect_is_local);

    rv = system_port_is_local(unit, ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].port, &local_port, &work_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2\n");
        return rv;
    }
    printf(">>>Working: out_ac.port[%d]-local_port[%d]-is_local[%d]\n", out_ac.port, local_port, work_is_local);

    if(work_is_local) {
        rv = bcm_vlan_gport_add(unit, ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan, ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add\n");
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan, ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add\n");
            return rv;
        }
    }

    if(protect_is_local) {
        rv = bcm_vlan_gport_add(unit, ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan, ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add\n");
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan, ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add\n");
            return rv;
        }
    }

    if(!protect_is_local) {
        printf(">>>Port is remote system port, skip create protect AC\n");
    } else {
        rv = bcm_vlan_port_create(unit, &out_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection__set_protection_acs failed during Protecting Out-AC creation, rv - %d\n", rv);
            return rv;
        }

        ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan_port_id = out_ac.vlan_port_id;
        ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].encap_id = out_ac.encap_id;
        printf(">>>Out_ac[FAILOVER_PROTECTING].vlan_port_id: %#x\n", ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].vlan_port_id);

        /* Attach the Protecting Out-AC to the vswitch */
        rv = bcm_vswitch_port_add(unit, ac_1to1_coupled_protection_info->vsi, out_ac.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection__set_protection_acs failed during Protecting bcm_vswitch_port_add for vsi %d, rv - %d\n", out_ac.vsi, rv);
            return rv;
        }

        printf("Out-AC Protecting = %d, encap_id - %d\n", out_ac.vlan_port_id, out_ac.encap_id);
    }

    /* Attach to a MC group */
    mc_group = ac_1to1_coupled_protection_info->vsi;
    encap_id = out_ac.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit, mc_group, &g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry\n");
        return rv;
    }

    /* Create the Working Out-AC */
    bcm_vlan_port_t_init(&out_ac);
    out_ac.flags = BCM_VLAN_PORT_WITH_ID;
    out_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_ac.port = ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].port;
    out_ac.match_vlan = ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan;
    out_ac.vsi = 0;
    out_ac.failover_id = 0;
    if (g_ac_1to1_coupled_protection.is_ext_lif == 1)
    {
        out_ac.group = 2;
    }
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(out_ac.failover_port_id, fec_primary);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_ac.vlan_port_id, 0, ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_ac.vlan_port_id, out_ac.vlan_port_id);

    if (ac_1to1_coupled_protection_info->is_egress) {
        out_ac.flags |= BCM_VLAN_PORT_EGRESS_PROTECTION;
        out_ac.egress_failover_id = ac_1to1_coupled_protection_info->eg_in_failover_id;
        out_ac.egress_failover_port_id = ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].encap_id;
    } else {
        out_ac.ingress_failover_id = ac_1to1_coupled_protection_info->eg_in_failover_id;
        out_ac.ingress_failover_port_id = ac_1to1_coupled_protection_info->out_ac[FAILOVER_PROTECTING].encap_id;
    }

    if(!work_is_local) {
        printf(">>>Port is remote system port, skip create working AC\n");
    } else {
        rv = bcm_vlan_port_create(unit, &out_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection__set_protection_acs failed during Working Out-AC creation, rv - %d\n", rv);
            return rv;
        }

        /*Find the working Out-AC out*/
        bcm_vlan_port_t_init(&out_ac_info);
        out_ac_info.vlan_port_id = out_ac.vlan_port_id;
        rv = bcm_vlan_port_find(unit, &out_ac_info);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection__set_protection_acs failed during getting Working Out-AC, rv - %d\n", rv);
            return rv;
        }

        if (out_ac.flags & BCM_VLAN_PORT_EGRESS_PROTECTION) {
            if (!(out_ac_info.flags & BCM_VLAN_PORT_EGRESS_PROTECTION)) {
                printf("Error, ac_1to1_coupled_protection__set_protection_acs failed,get Working Out-AC flags failed\n");
                check_pass = 0;
            }
            if (!out_ac.egress_failover_port_id != !out_ac_info.egress_failover_port_id) {
                printf("Error, ac_1to1_coupled_protection__set_protection_acs failed,get Working Out-AC faolover port failed\n");
                check_pass = 0;
            }
        }

        if (check_pass == 0) {
            printf("Error! Out-Ac info is not the same between creation and getting.\n");
            printf("Out-Ac information used in creation: \n");
            print out_ac;
            printf("Out-Ac information got after creating: \n");
            print out_ac_info;
            assert(FALSE);
        }
    }

    if(!work_is_local) {
        printf(">>>Skip working adding outAC to vswitch and MC group\n");
    } else {
        ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan_port_id = out_ac.vlan_port_id;
        ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].encap_id = out_ac.encap_id;

        printf(">>>Out_ac[FAILOVER_WORKING].vlan_port_id: %#x\n", ac_1to1_coupled_protection_info->out_ac[FAILOVER_WORKING].vlan_port_id);

        /* Attach the Working Out-AC to the vswitch */
        rv = bcm_vswitch_port_add(unit, ac_1to1_coupled_protection_info->vsi, out_ac.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection__set_protection_acs failed during Working bcm_vswitch_port_add for vsi %d, rv - %d\n", out_ac.vsi, rv);
            return rv;
        }
    }

    /* Attach to a MC group */
    mc_group = ac_1to1_coupled_protection_info->vsi;
    encap_id = out_ac.vlan_port_id & 0xfffff;
    rv = multicast__add_multicast_entry(unit, mc_group, &g_ac_1to1_coupled_protection.out_ac[FAILOVER_WORKING].port, &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry\n");
        return rv;
    }

    return BCM_E_NONE;
}

/* Retrieve the initial confgiuration for the example.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single In-AC.
 */
void ac_1to1_coupled_protection__struct_get(ac_1to1_coupled_protection_s *param){
    sal_memcpy(param, &g_ac_1to1_coupled_protection, sizeof(*param));
    return;
}

/* Main function that performs all the configuarations for the example.
 * Performs init configuration before configuring the ACs and adding a static MAC
 * entry for the UC packets.
 * INPUT:
 *   protection_ac_s: Configuration info for running the example.
 */
int ac_1to1_coupled_protection__start_run(int *unit_ids, int nof_units, ac_1to1_coupled_protection_s *params) {
    int rv,  unit, i;
    int is_local, local_port;
    int first_fec;
    bcm_l2_addr_t l2_address;

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];
        printf(">>> unit[%d]......\n", unit);
        /* Initialize the test parameters */
        rv = ac_1to1_coupled_protection_init(unit, params);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1to1_coupled_protection_init, rv - %d\n", rv);
            return rv;
        }
    }

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];

        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        fec_primary = 0x20000000 | first_fec;
        fec_standby = 0x20000000 | (first_fec+1);

        /* Configure the In-AC */
        rv = ac_1to1_coupled_protection__set_in_ac(unit, &g_ac_1to1_coupled_protection);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1to1_coupled_protection__start_run, failed to create an In-AC, rv - %d\n", rv);
            return rv;
        }

        /* Configure the protection Out-ACs */
        rv = ac_1to1_coupled_protection__set_protection_acs(unit, &g_ac_1to1_coupled_protection);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1to1_coupled_protection__start_run, failed to create an Out-AC, rv - %d\n", rv);
            return rv;
        }
    }

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];

        rv = create_protection_fec(unit,&g_ac_1to1_coupled_protection);
        if (rv != BCM_E_NONE) {
            printf("Error in create FEC for unit[%d]\n", unit);
            return rv;
        }
    }

    if (g_ac_1to1_coupled_protection.dynamic_learning != 1) {
        for (i = 0 ; i < nof_units ; i++) {
            /* Add a MACT entry */
            unit = unit_ids[i];
            rv = system_port_is_local(unit, g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].port, &local_port, &is_local);
            if (rv != BCM_E_NONE) {
                printf("Error in gport_is_local for JR2\n");
                return rv;
            }

            if (!is_local) {
                bcm_l2_addr_t_init(&l2_address,g_ac_1to1_coupled_protection.mac_address_out,g_ac_1to1_coupled_protection.vsi);
                l2_address.flags = BCM_L2_STATIC;
                l2_address.port = g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].port;
                l2_address.encap_id = g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].encap_id;
                rv = bcm_l2_addr_add(unit,&l2_address);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l2_addr_add\n");
                    return rv;
                }
            } else {
                bcm_l2_addr_t_init(&l2_address, g_ac_1to1_coupled_protection.mac_address_out, g_ac_1to1_coupled_protection.vsi);
                l2_address.flags = BCM_L2_STATIC; /* static entry */
                l2_address.port = g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].vlan_port_id;
                rv = bcm_l2_addr_add(unit,&l2_address);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l2_addr_add\n");
                    return rv;
                }
            }

            /* Add a MACT entry */
            rv = system_port_is_local(unit, g_ac_1to1_coupled_protection.in_ac.port, &local_port, &is_local);
            if (rv != BCM_E_NONE) {
                printf("Error in gport_is_local for JR2\n");
                return rv;
            }

            if (!is_local) {
                bcm_l2_addr_t_init(&l2_address,g_ac_1to1_coupled_protection.mac_address_in,g_ac_1to1_coupled_protection.vsi);
                l2_address.flags = BCM_L2_STATIC;
                l2_address.port = g_ac_1to1_coupled_protection.in_ac.port;
                l2_address.encap_id = g_ac_1to1_coupled_protection.in_ac.encap_id;
                rv = bcm_l2_addr_add(unit,&l2_address);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l2_addr_add\n");
                    return rv;
                }
            } else {
                bcm_l2_addr_t_init(&l2_address, g_ac_1to1_coupled_protection.mac_address_in, g_ac_1to1_coupled_protection.vsi);
                l2_address.flags = BCM_L2_STATIC; /* static entry */
                l2_address.port = g_ac_1to1_coupled_protection.in_ac.vlan_port_id;
                rv = bcm_l2_addr_add(unit,&l2_address);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l2_addr_add\n");
                    return rv;
                }
            }
        }
    }
    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *
 * INPUT: in_port  - ingress port
 *        out_port - egress port
 */
int ac_1to1_coupled_protection_with_ports__start_run(int *unit_ids, int nof_units, int in_port, int out_port_working, int out_port_protecting, int is_egress) {
    int rv;
    ac_1to1_coupled_protection_s param;
    ac_1to1_coupled_protection__struct_get(&param);

    param.in_ac.port = in_port;
    param.out_ac[FAILOVER_WORKING].port = out_port_working;
    param.out_ac[FAILOVER_PROTECTING].port = out_port_protecting;
    param.is_egress = is_egress;

    return ac_1to1_coupled_protection__start_run(unit_ids, nof_units, &param);
}


/* Performs failover setting for the two protection Out-ACs
 *
 * INPUT: is_fec_protection  - Determine if the setting is for FEC-Protection or Egress/Ingress Protection
 *        is_enable - The failover state to set. 1: Working Path, 0: Protecting Path.
 */
int ac_1to1_coupled_protection_with_ports__failover_set(int *unit_ids, int nof_units, int is_fec_protection, int is_enable) {
    int rv, i, unit;
    bcm_failover_t  failover_id = (is_fec_protection) ? g_ac_1to1_coupled_protection.fec_failover_id : g_ac_1to1_coupled_protection.eg_in_failover_id;

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];
        rv = bcm_failover_set(unit, failover_id, is_enable);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_failover_set failover_id:  0x%08x, is_fec: %d, is_enable: %d \n", failover_id, is_fec_protection, is_enable);
            return rv;
        }
    }
    return rv;
}

int ac_1to1_coupled_protection__cleanup(int *unit_ids, int nof_units) {
    int rv, i, unit;
    int work_is_local=1, protect_is_local=1, in_is_local=1, local_port;

    for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
        rv = system_port_is_local(unit, g_ac_1to1_coupled_protection.in_ac.port, &local_port, &in_is_local);
        if (rv != BCM_E_NONE) {
            printf("Error in gport_is_local for JR2\n");
            return rv;
        }

        if(in_is_local) {
            rv = bcm_vlan_port_destroy(unit, g_ac_1to1_coupled_protection.in_ac.vlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_port_destroy gport:  0x%08x\n", g_ac_1to1_coupled_protection.in_ac.vlan_port_id);
                return rv;
            }
        }

        rv = system_port_is_local(unit, g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].port, &local_port, &protect_is_local);
        if (rv != BCM_E_NONE) {
            printf("Error in gport_is_local for JR2\n");
            return rv;
        }

        if(protect_is_local) {
            rv = bcm_vlan_port_destroy(unit, g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].vlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_port_destroy gport:  0x%08x\n", g_ac_1to1_coupled_protection.out_ac[FAILOVER_PROTECTING].vlan_port_id);
                return rv;
            }
        }

        rv = system_port_is_local(unit, g_ac_1to1_coupled_protection.out_ac[FAILOVER_WORKING].port, &local_port, &work_is_local);
        if (rv != BCM_E_NONE) {
            printf("Error in gport_is_local for JR2\n");
            return rv;
        }

        if(work_is_local) {
            rv = bcm_vlan_port_destroy(unit, g_ac_1to1_coupled_protection.out_ac[FAILOVER_WORKING].vlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_port_destroy gport:  0x%x\n", g_ac_1to1_coupled_protection.out_ac[FAILOVER_WORKING].vlan_port_id);
                return rv;
            }
        }

        rv = bcm_failover_destroy(unit, g_ac_1to1_coupled_protection.fec_failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection_init failed during FEC bcm_failover_destroy, rv - %d\n", rv);
            return rv;
        }

        rv = bcm_failover_destroy(unit, g_ac_1to1_coupled_protection.eg_in_failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_1to1_coupled_protection_init failed during Egress bcm_failover_destroy, rv - %d\n", rv);
            return rv;

        }
    }

    return rv;
}

