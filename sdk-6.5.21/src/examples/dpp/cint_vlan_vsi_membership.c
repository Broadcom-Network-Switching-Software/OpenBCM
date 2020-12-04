/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * CINT files:
 * utility/cint_utils_vlan.c
 *
 *
 * Purpose: Example of employing vlan vsi membership
 *
 *
 * Tests scenario:
 *
 *
 * case 1: Ingress vlan membership filter with drop packet trap
 *
 * 1. Configure bcmRxTrapPortNotVlanMember drop packet trap
 * 2. Use bcm_vlan_gport_add in order to add vlan to port filtering (the vlan will not be filtered)
 * 3. Enable port filtering with bcm_port_vlan_member_set (if disable, filter won't work)
 *
 * Send packet : vlan id 20
 *               in_port 200
 *               - check with diag pp fdt that it's accepted
 * Send packet : vlan id 16
 *               in_port 200
 *               - check with diag pp fdt that it's dropped
 *
 *
 *
 * case 2: Ingress vlan membership filter with send to cpu packet trap
 *
 * 1. Configure bcmRxTrapPortNotVlanMember send to cpu packet trap
 * 2. Use bcm_vlan_gport_add in order to add vlan to port filtering (the vlan will not be filtered)
 * 3. Enable port filtering with bcm_port_vlan_member_set (if disable, filter won't work)
 *
 * Send packet : vlan id 20
 *               in_port 200
 *               - check with diag pp fdt that it's accepted
 * Send packet : vlan id 16
 *               in_port 200
 *               - check with diag pp fdt that it's dropped
 *
 *
 *
 * case 3: Egress vsi membership filter - enable
 *
 * -----------
 * The test won't work after cases 1&2 because of TrapPortNotVlanMember configuration
 * In order to work, you can use:
 * bcm_vlan_gport_add(0,16,200,BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * bcm_vlan_gport_add(0,20,200,BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * -----------
 *
 * 1. Configure mac entries
 * 2. Use bcm_vlan_gport_add in order to add vsi to port filtering (the vlan will not be filtered)
 * 3. Enable port filtering with bcm_port_vlan_member_set (if disable, filter won't work)
 *
 * Send packet : vlan id 20
 *               in_port 200
 *               dst mac A600000000e1
 *               - check with Debug rx verb that it's accepted
 * Send packet : vlan id 16
 *               in_port 200
 *               dst mac A600000000e1
 *               - check with Debug rx verb that it's dropped
 *
 *
 *
 * case 4: Egress vsi membership filter - disable
 *
 * 1. Disable port filtering with bcm_port_vlan_member_set (filter won't work)
 *
 * This case should run after case 2 in order to see the affects
 *
 * Send packet : vlan id 20
 *               in_port 200
 *               dst mac A600000000e1
 * Send packet : vlan id 16
 *               in_port 200
 *               dst mac A600000000e1
 * -- Check with Debug rx verb both packets accepted
 *
 *
 *
 * Ingress:
 * port 200 vlan membership filter
 *
 *                INPUT        ________________________        OUTPUT
 *                            |                        |
 *                       200  |                        |
 *                    vlan=20 |                        |mc
 *      --------------------->|                        |----------------------->
 *                            |________________________|
 *
 *                             ________________________
 *                       200  |                        |
 *                    vlan=16 |       /                |
 *      --------------------->|------/--------> drop   |
 *                            |     /                  |
 *                            |________________________|
 *
 *
 *
 * Egress:
 * port 201 vsi membership filter
 *               INPUT         ________________________        OUTPUT
 *                            |                        |
 *                       200  |                        | 201
 *                            |                        | vsi=20
 *      --------------------->|                        |----------------------->
 *                            |________________________|
 *
 *                             ________________________
 *                       200  |                        | 201      /
 *                            |                        | vsi=16  /
 *      --------------------->|                        |--------/---------> drop
 *                            |                        |       /
 *                            |________________________|      /
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

/*  Main Struct  */
struct vlan_vsi_membership_s{
    bcm_vlan_t  vlan[2];
    bcm_gport_t in_port;
    bcm_gport_t out_port;
    bcm_mac_t mac;
};


/* Initialization of global struct*/
vlan_vsi_membership_s g_vlan_vsi_membership = {
    {20, 16},                               /* vlan, filtered_vlan */
    200,                                    /* in-port */
    201,                                    /* out-port */
    {0xa6, 0x00, 0x00, 0x00, 0x00, 0xe1}    /* mac */
};


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */




int vlan_vsi_memberhip_init(int unit, vlan_vsi_membership_s *param)
{
    int rv = BCM_E_NONE;
    int i;

    if (param != NULL) {
        printf("param!=null\n");
       sal_memcpy(&g_vlan_vsi_membership , param, sizeof(g_vlan_vsi_membership ));
    }

    /* vlan create */
    for (i=0; i<2; i++) {
        rv = bcm_vlan_create(unit, g_vlan_vsi_membership.vlan[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add\n");
            return rv;
        }
    }

    return rv;
}


int vlan_vsi_membership__mac_entry_create(int unit, bcm_mac_t mac, bcm_vlan_t vid, int port)
{
    int rv = BCM_E_NONE;

    bcm_l2_addr_t l2_addr;
    l2_addr.mac = mac;
    l2_addr.vid = vid;
    l2_addr.port = port;

    rv = bcm_l2_addr_add(unit, l2_addr);
    if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_addr_add\n");
            return rv;
    }

    return rv;
}


/* case 1: Ingress - vlan membership - drop packet trap */
int vlan_vsi_memberhip_ingress_black_hole_trap__start_run(int unit, vlan_vsi_membership_s *param)
{
    int rv = BCM_E_NONE;

    rv = vlan_vsi_memberhip_init(unit, param);
    if (rv != BCM_E_NONE) {
            printf("Error, in vlan_vsi_memberhip_init\n");
            return rv;
    }

    rv = vlan__ingress_membership_create(unit, g_vlan_vsi_membership.vlan[0], g_vlan_vsi_membership.in_port, BCM_GPORT_BLACK_HOLE);
    if (rv != BCM_E_NONE) {
            printf("Error, in vlan__ingress_membership_create\n");
            return rv;
    }

    return rv;
}

/* case 2: Ingress - vlan membership - send to cpu trap*/
int vlan_vsi_memberhip_ingress_cpu_trap_example__start_run(int unit, vlan_vsi_membership_s *param)
{
    int rv = BCM_E_NONE;

    vlan_vsi_memberhip_init(unit, param);
    if (rv != BCM_E_NONE) {
            printf("Error, in vlan_vsi_memberhip_init\n");
            return rv;
    }

    vlan__ingress_membership_create(unit, g_vlan_vsi_membership.vlan[0], g_vlan_vsi_membership.in_port, BCM_GPORT_LOCAL_CPU);
    if (rv != BCM_E_NONE) {
            printf("Error, in vlan__ingress_membership_create\n");
            return rv;
    }

    return rv;
}

/* case 3: Egress - vsi membership enable */
int vlan_vsi_memberhip_egress_enable__start_run(int unit, vlan_vsi_membership_s *param)
{
    int rv = BCM_E_NONE;
    int i;

    vlan_vsi_memberhip_init(unit, param);
    if (rv != BCM_E_NONE) {
            printf("Error, in vlan_vsi_memberhip_init\n");
            return rv;
    }

    /* mac entries create */
    for (i=0; i<2; i++) {
        vlan_vsi_membership__mac_entry_create(unit, g_vlan_vsi_membership.mac, g_vlan_vsi_membership.vlan[i], g_vlan_vsi_membership.out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_vsi_membership__mac_entry_create\n");
            return rv;
        }
    }

    vlan__egress_membership_create(unit,g_vlan_vsi_membership.vlan[0],g_vlan_vsi_membership.out_port);
    if (rv != BCM_E_NONE) {
            printf("Error, in vlan__egress_membership_create\n");
            return rv;
    }

    return rv;
}

 /* case 4: Egress - vsi membership disable */
int vlan_vsi_memberhip_egress_disable__start_run(int unit, vlan_vsi_membership_s *param)
{
    int rv = BCM_E_NONE;

    vlan_vsi_memberhip_init(unit, param);

    bcm_port_vlan_member_set(unit,g_vlan_vsi_membership.out_port,0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }

    return rv;
}
