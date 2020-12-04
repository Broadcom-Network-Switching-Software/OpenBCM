/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides two protection examples:                                                                                                       *
 *                                                                                                                                                  *
 * Example 1:                                                                                                                                       *
 * ==========                                                                                                                                       *
 * Facility FEC (The Super-FEC Protection Pointer is -2)                                                                                            *
 *                                                                                                                                                  *
 * 1. Set the Destination-Status of the working port to 0 (Disable)                                                                                 *
 * 2. Expect to receive the packet in protecting port                                                                                               *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                  ________________________________                                                                *
 *                                                 |                   _____        |                                                               *
 *                           In-AC                 |                  |     | ----> | -----                                                         *
 *                               ----------------> | -   -   -   -  > | FEC |       |      |                                                        *
 *                                                 |                  |_____| ----> | ---  |                                                        *
 *                                                 |                                |    | |                                                        *
 *                                                 |                                |    | |                                                        *
 *                           Working Out-AC  /     |                                |    | |                                                        *
 *                               <----------/----- | < -- - -  -   -  -   -   -  -  | <--  |                                                        *
 *                                         /       |                                |      |                                                        *
 *                           Protecting Out-AC     |                                |      |                                                        *
 *                               <---------------- | < -- - -  -   -  -   -   -  -  | <----                                                         *
 *                                                 |                                |                                                               *
 *                                                 |________________________________|                                                               *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * Example 2:                                                                                                                                       *
 * ==========                                                                                                                                       *
 * Facility FEC with simultaneous mode (Facility & The Super-FEC Protection Pointer has a vaild Path pointer)                                       *
 *                                                                                                                                                  *
 * Set following SOC properties:                                                                                                                    *
 * Configure the device to be in "bcm886xx_fec_accelerated_reroute_mode=1"                                                                          *
 *                                                                                                                                                  *
 * 1. Set protection_path to protecting port                                                                                                        *
 * 2. Set the Destination-Status of the protecting port to 0 (Disable)                                                                              *
 * 3. Expect to receive the packet in working port                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                  ________________________________                                                                *
 *                                                 |                   _____        |                                                               *
 *                           In-AC                 |                  |     | ----> | -----                                                         *
 *                               ----------------> | -   -   -   -  > | FEC |       |      |                                                        *
 *                                                 |                  |_____| ----> | ---  |                                                        *
 *                                                 |                                |    | |                                                        *
 *                                                 |                                |    | |                                                        *
 *                           Working Out-AC        |                                |    | |                                                        *
 *                               <---------------- | < -- - -  -   -  -   -   -  -  | <--  |                                                        *
 *                                                 |                                |      |                                                        *
 *                           Protecting Out-AC  /  |                                |      |                                                        *
 *                               <-------------/-- | < -- - -  -   -  -   -   -  -  | <----                                                         *
 *                                            /    |                                |                                                               *
 *                                                 |________________________________|                                                               *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *                                                                                                                                                *
 * Configuration:                                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dpp                                                                                                                  *
 * cint cint_ac_fec_facility_protection.c                                                                                                           *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * ac_fec_facility_protection__start_run(int unit, int in_port, int out_port_working, int out_port_protecting, int failover_id)                     *
 */

struct ac_fec_facility_protection_s {
    bcm_vlan_t vlan;
    bcm_mac_t mac;

    int in_port;
    int out_port_working;
    int out_port_protecting;
    bcm_failover_t failover_id;
};

/* Initialization of global struct*/
ac_fec_facility_protection_s g_ac_fec_facility_protection = {

            20,                                     /* VSI */
            {0xa6, 0x00, 0x00, 0x00, 0x00, 0xe1},   /* MAC Address Out */

            201,                                    /* IN-PORT */
            13,                                     /* OUT-PORT WORKING */
            14,                                     /* OUT-PORT PROTECTING */
            0};                                     /* FAILOVER-ID*/

int fec_standby=0x2000A563;
int fec_primary=0x2000A562;

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */



/* Initialization of the main struct
 * Function allows the re-write of default values
 *
 * INPUT:
 *   params: new values for g_ac_fec_facility_protection
 */
int ac_fec_facility_protection_init(int unit, ac_fec_facility_protection_s *params) {

    int rv;
    int flags;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }

    if (params != NULL) {
       sal_memcpy(&g_ac_fec_facility_protection, params, sizeof(g_ac_fec_facility_protection));
    }

    if (g_ac_fec_facility_protection.failover_id) {
        /* This is the simultaneous mode case, where the received failover_id has a vaild Path pointer */

        if(!device_is_jericho2)
        {
            flags = (BCM_FAILOVER_WITH_ID | BCM_FAILOVER_FEC);
        }
        else
        {
            flags = (BCM_FAILOVER_WITH_ID | BCM_FAILOVER_FEC | BCM_FAILOVER_FEC_3RD_HIERARCHY);
        }

        /* Create failover object. This call can fail in case that the user allready created the failover object */
        rv = bcm_failover_create(unit, flags, &g_ac_fec_facility_protection.failover_id);
        if (rv != BCM_E_NONE  && rv != BCM_E_RESOURCE ) {
            printf("Error in bcm_failover_create, rv - %d\n", rv);
            return rv;
        }
    }
    else {
        /* This is the simple facility mode case. Here we assign the failover_id to -2 */

        /* This is to get the encoding of the FEC type */
        g_ac_fec_facility_protection.failover_id = 1;
        rv = bcm_failover_create(unit, (BCM_FAILOVER_WITH_ID | BCM_FAILOVER_FEC), &g_ac_fec_facility_protection.failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_failover_create, rv - %d\n", rv);
            return rv;
        }

        if(!device_is_jericho2)
        {
            bcm_failover_t failover_type_encoding = g_ac_fec_facility_protection.failover_id - 1;
            g_ac_fec_facility_protection.failover_id = failover_type_encoding + SOC_DPP_DEFS_GET_NOF_FAILOVER_FEC_IDS(&unit);  /* -2 */
        }
        else
        {
            g_ac_fec_facility_protection.failover_id = BCM_FAILOVER_ID_LOCAL;
        }
    }

    printf("Failover_id: %d\n", g_ac_fec_facility_protection.failover_id);

    return BCM_E_NONE;
}


/* Retrieve the initial confgiuration for the example.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single In-AC.
 */
void ac_fec_facility_protection__struct_get(ac_fec_facility_protection_s *param){

    sal_memcpy(param, &g_ac_fec_facility_protection, sizeof(*param));
    return;
}


/* Main function that performs all the configuarations for the example.
 * Performs init configuration before configuring the ACs and adding a static MAC
 * entry for the UC packets.
 * INPUT:
 *   protection_ac_s: Configuration info for running the example.
 */
int ac_fec_facility_protection__start_run(int unit, ac_fec_facility_protection_s *params) {
    int rv;
    bcm_l3_egress_t fec;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }

    rv = ac_fec_facility_protection_init(unit, *params);
    if (rv != BCM_E_NONE) {
        printf("Error in ac_fec_facility_protection_init, rv - %d\n", rv);
        return rv;
    }

    /* Create the protecting FEC entry */
    bcm_vlan_port_t vport_t_protection;
    bcm_vlan_port_t_init(&vport_t_protection);
    vport_t_protection.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vport_t_protection.match_vlan = g_ac_fec_facility_protection.vlan;
    vport_t_protection.port = g_ac_fec_facility_protection.out_port_protecting;
    if(device_is_jericho2)
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, &fec_primary);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        fec_primary |= 0x20000000;
        fec_standby = fec_primary + 1;
        vport_t_protection.failover_id = 0;
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(vport_t_protection.failover_port_id, fec_standby);
    }
    else
    {
        vport_t_protection.failover_id = g_ac_fec_facility_protection.failover_id;
    }

    rv = bcm_vlan_port_create(unit, &vport_t_protection);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create, rv - %d\n", rv);
        return rv;
    }
    printf("Created vlan_port: port-%d vlan-%d\n", g_ac_fec_facility_protection.out_port_protecting, g_ac_fec_facility_protection.vlan);

    /* Create the working FEC entry */
    bcm_vlan_port_t vport_working;
    bcm_vlan_port_t_init(&vport_working);
    vport_working.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vport_working.flags = BCM_VLAN_PORT_WITH_ID;
    vport_working.vlan_port_id = vport_t_protection.vlan_port_id + 1;
    vport_working.match_vlan = g_ac_fec_facility_protection.vlan;
    vport_working.port = g_ac_fec_facility_protection.out_port_working;
    if(device_is_jericho2)
    {
        vport_working.failover_id = 0;
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(vport_working.failover_port_id, fec_primary);
    }
    else
    {
        vport_working.failover_id = g_ac_fec_facility_protection.failover_id;
        vport_working.failover_port_id = vport_t_protection.vlan_port_id;
    }

    rv = bcm_vlan_port_create(unit, &vport_working);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create, rv - %d\n", rv);
        return rv;
    }
    printf("Created vlan_port: port-%d vlan-%d\n", g_ac_fec_facility_protection.out_port_working, g_ac_fec_facility_protection.vlan);

    if(device_is_jericho2)
    {
        /*
         * Creates backup FEC:
         */
        bcm_l3_egress_t_init(&fec);
        fec.flags=BCM_L3_3RD_HIERARCHY;
        fec.intf=vport_t_protection.vlan_port_id;
        fec.vlan=g_ac_fec_facility_protection.vlan;
        fec.port=g_ac_fec_facility_protection.out_port_protecting;
        fec.failover_id=g_ac_fec_facility_protection.failover_id;
        fec.failover_if_id =0;
        rv=bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, fec, &fec_standby);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_l3_egress_create\n");
            return rv;
        }

        /*
         * Creates primary FEC:
         */
        bcm_l3_egress_t_init(&fec);
        fec.flags=BCM_L3_3RD_HIERARCHY;
        fec.intf=vport_working.vlan_port_id;
        fec.vlan=g_ac_fec_facility_protection.vlan;
        fec.port=g_ac_fec_facility_protection.out_port_working;
        fec.failover_id=g_ac_fec_facility_protection.failover_id;
        fec.failover_if_id =fec_standby;
        rv=bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, fec, &fec_primary);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_l3_egress_create\n");
            return rv;
        }
    }

    /* Create the in-ac */
    bcm_vlan_port_t in_ac;
    bcm_vlan_port_t_init(&in_ac);
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.port = g_ac_fec_facility_protection.in_port;
    in_ac.match_vlan = g_ac_fec_facility_protection.vlan;
    in_ac.vsi = 0;
    rv = bcm_vlan_port_create(unit, &in_ac);

    rv = bcm_vswitch_create_with_id(unit,g_ac_fec_facility_protection.vlan);
    rv = bcm_vswitch_port_add(unit, g_ac_fec_facility_protection.vlan, in_ac.vlan_port_id);
    rv = bcm_vlan_gport_add(unit, g_ac_fec_facility_protection.vlan, g_ac_fec_facility_protection.in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    /* Add a MACT entry */
    bcm_l2_addr_t l2_addr;
    l2_addr.mac = g_ac_fec_facility_protection.mac;
    l2_addr.vid = g_ac_fec_facility_protection.vlan;
    l2_addr.port = vport_t_protection.vlan_port_id;
    bcm_l2_addr_add(unit, &l2_addr);

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *
 * INPUT: in_port  - ingress port .
 *        out_port_working - egress port.
 *        out_port_protecting - egress port.
 *        failover_id - If simultaneous_mode- failover_id. Otherwise, 0.
 */
int ac_fec_facility_protection_with_ports__start_run(int unit, int in_port, int out_port_working, int out_port_protecting, bcm_failover_t failover_id) {

    ac_fec_facility_protection_s param;

    ac_fec_facility_protection__struct_get(&param);

    param.in_port = in_port;
    param.out_port_working = out_port_working;
    param.out_port_protecting = out_port_protecting;
    param.failover_id = failover_id;

    return ac_fec_facility_protection__start_run(unit, &param);
}







