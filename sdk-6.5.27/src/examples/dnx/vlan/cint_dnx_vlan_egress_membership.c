/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example of employing egress vlan membership
 *
 * Tests scenario:
 *
 * step 1:  Egress VLAN membership filter disabled
 *          No packets are dropped
 *
 * step 2:  Egress VLAN membership filter enabled on port_i and membership list is empty
 *          bcm_port_vlan_member_set(unit, port_i, 2)
 *          all packets exiting port_i will be dropped
 *
 * step 3:  Egress VLAN membership filter enabled and port_i is added to memberset of vlan_i
 *          bcm_vlan_gport_add(unit, vlan_i, port_i, (BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE))
 *          only packets with vlan_i will not be dropped
 *
 * step 4:  Egress VLAN membership filter enabled and port_i is removed from memberset of vlan_i
 *          bcm_vlan_gport_delete(unit, vlan_i, port_i)
 *          all packets will be dropped
 *
 * Example: Egress membership filtering is enabled for port 201 and vlan 100 is the only member
 *
 *                INPUT        ________________________        OUTPUT
 *                            |                        |
 *                   port 200 |                        | port 201
 *                   vlan 100 |                        | vlan 100
 *      --------------------->|                        |----------------------->
 *                            |________________________|
 *
 *                             ________________________
 *                            |                        |
 *                   port 200 |                        |
 *                   vlan 150 |         /              |
 *      --------------------->| -------/---> drop      |
 *                            |_______/________________|
 *
 * SOC properties:
 * egress_membership_mode = 1
 *
 * CINT files:
 * Test Scenario - start
  cd ../../../../src/examples/dnx/
  cint utility/cint_dnx_utils_l2.c
  cint utility/cint_dnx_utils_vlan_translate.c
  cint cint_dnx_vlan_egress_membership.c
 *
  cint
  dnx_vlan_egress_membership__start_run(int unit, dnx_vlan_egress_membership_s *test_data) - test_data = NULL for default params
 * Test Scenario - end
 */

/*
 * Global Variables Definition and Initialization  START
 */

/* Main Struct VLAN membership */
struct dnx_vlan_egress_membership_s {
    bcm_vlan_t  vsi;
    bcm_vlan_t  vlan[2];
    int         ports[2];
    bcm_gport_t gport[2][2];
    bcm_mac_t   mac1;
    bcm_mac_t   mac2;
    int         dt_action_id;
};

/* Initialization of global struct */
dnx_vlan_egress_membership_s g_dnx_vlan_egress_membership = {

    16,                                     /* vsi */
    {100, 200},                             /* vlan */
    {200, 201},                             /* ports */
    {{0, 0}, {0, 0}},                       /* gport */
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},   /* mac1 */
    {0x00, 0x55, 0x44, 0x33, 0x22, 0x11},   /* mac2 */
    0                                       /* dt_action_id */
};

/*
 * Global Variables Definition and Initialization  END
 */

void
dnx_vlan_egress_membership_init(dnx_vlan_egress_membership_s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g_dnx_vlan_egress_membership, test_data, sizeof(g_dnx_vlan_egress_membership));
    }

}

void
dnx_vlan_egress_membership__get_struct(dnx_vlan_egress_membership_s *test_data)
{
    sal_memcpy(test_data, &g_dnx_vlan_egress_membership, sizeof(*test_data));
    return;
}

/* Main function of VLAN membership test */
int
dnx_vlan_egress_membership__start_run(int unit, dnx_vlan_egress_membership_s *test_data)
{
    int i;
    bcm_vlan_port_t vlan_port;
    char error_msg[200]={0,};

    dnx_vlan_egress_membership_init(test_data);

    /* Create VSI */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, g_dnx_vlan_egress_membership.vsi), "");

    for (i = 0; i < 2; i++) {
        /* Configure the TPIDs for the port and their classification */
        snprintf(error_msg, sizeof(error_msg), "for port - %d", g_dnx_vlan_egress_membership.ports[i]);
        BCM_IF_ERROR_RETURN_MSG(vlan_translate_tag_classification_set(unit, g_dnx_vlan_egress_membership.ports[i], /*tag_format*/ 4, 0x8100, 0xFFFFFFFF), error_msg);

        /* Set translation action 0 */
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &action), "");

        /* Create an AC LIF for vlan1 */
        bcm_vlan_port_t_init(&vlan_port);

        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.port = g_dnx_vlan_egress_membership.ports[i];
        vlan_port.match_vlan = g_dnx_vlan_egress_membership.vlan[0];

        snprintf(error_msg, sizeof(error_msg), "for port - %d, vlan - %d",
            g_dnx_vlan_egress_membership.ports[i], g_dnx_vlan_egress_membership.vlan[0]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_dnx_vlan_egress_membership.vlan[0], g_dnx_vlan_egress_membership.ports[i], 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "for port - %d, vlan - %d",
            g_dnx_vlan_egress_membership.ports[i], g_dnx_vlan_egress_membership.vlan[1]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_dnx_vlan_egress_membership.vlan[1], g_dnx_vlan_egress_membership.ports[i], 0), error_msg);

        g_dnx_vlan_egress_membership.gport[i][0] = vlan_port.vlan_port_id;
        printf("\n>>> Port %d VLAN %d created: gport = 0x%x)\n",
               g_dnx_vlan_egress_membership.ports[i], vlan_port.match_vlan, vlan_port.vlan_port_id);

        /* Attach the AC LIF1 to the Bridge VSI */
        snprintf(error_msg, sizeof(error_msg), "vsi - %d, vlan_port_id - %d",
            g_dnx_vlan_egress_membership.vsi, g_dnx_vlan_egress_membership.gport[i][0]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_dnx_vlan_egress_membership.vsi, vlan_port.vlan_port_id), error_msg);

        /* Create an AC LIF for vlan 2 */
        vlan_port.match_vlan = g_dnx_vlan_egress_membership.vlan[1];
        snprintf(error_msg, sizeof(error_msg), "for port - %d, vlan - %d",
            g_dnx_vlan_egress_membership.ports[i], g_dnx_vlan_egress_membership.vlan[1]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

        g_dnx_vlan_egress_membership.gport[i][1] = vlan_port.vlan_port_id;
        printf("\n>>> Port %d VLAN %d created: gport = 0x%x)\n",
               g_dnx_vlan_egress_membership.ports[i], vlan_port.match_vlan, vlan_port.vlan_port_id);

        /* Attach the AC LIF2 to the Bridge VSI */
        snprintf(error_msg, sizeof(error_msg), "vsi - %d, vlan_port_id - %d",
            g_dnx_vlan_egress_membership.vsi, g_dnx_vlan_egress_membership.gport[i][1]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_dnx_vlan_egress_membership.vsi, vlan_port.vlan_port_id), error_msg);
    }

    /* Configure a mac address for dst mac. */
    l2__mact_properties_s mact_entry = {
        g_dnx_vlan_egress_membership.gport[1][0],
        g_dnx_vlan_egress_membership.mac1,
        g_dnx_vlan_egress_membership.vsi
    };

    l2__mact_properties_s mact_entry2 = {
        g_dnx_vlan_egress_membership.gport[1][1],
        g_dnx_vlan_egress_membership.mac2,
        g_dnx_vlan_egress_membership.vsi
    };

    BCM_IF_ERROR_RETURN_MSG(l2__mact_entry_create(unit, &mact_entry), "");

    BCM_IF_ERROR_RETURN_MSG(l2__mact_entry_create(unit, &mact_entry2), "");

    return BCM_E_NONE;
}

/* Main function tcl interface */
int
dnx_vlan_egress_membership__start_run_with_ports(int unit, int port1, int port2, bcm_vlan_t vsi, bcm_vlan_t vlan1, bcm_vlan_t vlan2)
{
    dnx_vlan_egress_membership_s temp_s;

    sal_memcpy(&temp_s, &g_dnx_vlan_egress_membership, sizeof(temp_s));

    temp_s.ports[0] = port1;
    temp_s.ports[1] = port2;
    temp_s.vsi      = vsi;
    temp_s.vlan[0]  = vlan1;
    temp_s.vlan[1]  = vlan2;

    BCM_IF_ERROR_RETURN_MSG(dnx_vlan_egress_membership__start_run(unit, &temp_s), "");

    return BCM_E_NONE;
}

/* Add port to VLAN's memberset */
int
dnx_vlan_egress_membership__port_add(int unit, bcm_vlan_t vlan)
{
    bcm_gport_t gport = ((vlan == g_dnx_vlan_egress_membership.vlan[0]) ? g_dnx_vlan_egress_membership.gport[1][0] : g_dnx_vlan_egress_membership.gport[1][1]);

    /* VLAN membership */
    printf("Port %d (gport - %x) added to VLAN %d memberset\n",
           g_dnx_vlan_egress_membership.ports[1], gport, vlan);

    return bcm_vlan_gport_add(unit, vlan, g_dnx_vlan_egress_membership.ports[1], 0);
}

/* Remove port from VLAN's memberset */
int
dnx_vlan_egress_membership__port_delete(int unit, bcm_vlan_t vlan)
{
    bcm_gport_t gport = (vlan == g_dnx_vlan_egress_membership.vlan[0] ? g_dnx_vlan_egress_membership.gport[1][0] : g_dnx_vlan_egress_membership.gport[1][1]);

    /* VLAN membership */
    printf("Port %d (gport - %x) removed from VLAN %d\n",
           g_dnx_vlan_egress_membership.ports[1], gport, vlan);

    return bcm_vlan_gport_delete(unit, vlan, g_dnx_vlan_egress_membership.ports[1]);
}
