/* $Id: cint_failover_asym_ac_dynamic_learning.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * This example update the fec protection information for ingress ac and encap protection for egress ac. See figure below*
*-------+--------------+------------+--------------+------------+--------------+------------+--------------+-------------*
*       | ingress                                               | egress                                                 *
*       +---------------------------+---------------------------+---------------------------+----------------------------*
*       | secondary                 | primary                   | secondary                 | primary                    *
*       +--------------+------------+--------------+------------+--------------+------------+--------------+-------------*
*       | prot_pointer | learn_info | prot_pointer | learn_info | prot_pointer | encap_id   | prot_pointer | encap_id    *
*-------+--------------+------------+--------------+------------+--------------+------------+--------------+-------------*
* step1 | N/A          | 200        | N/A          | 201        | 0x80000002   | 0x44a01000 | 0x80000002   | 0x44a01001  *
*-------+--------------+------------+--------------+------------+--------------+            +--------------+             *
* step2 | 0x40000001   | 0x2000cccf | 0x40000001   | 0x2000ccce | 0x80000003   |            | 0x80000003   |             *
*-------+--------------+------------+--------------+------------+              |            |              |             *
* step3 | 0x40000001   | 0x2000ccd1 | 0x40000001   | 0x2000ccd0 |              |            |              |             *
*-------+--------------+------------+--------------+------------+              |            |              |             *
* step4 | N/A          | 200        | N/A          | 201        |              |            |              |             *
 */

/*
 * Test Scenario
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/dnx/cint_failover_asym_ac_dynamic_learning.c
 * cint ../../src/examples/sand/cint_sand_field_fec_id_align.c
 * cint
 * print dnx_asym_ac_failover_create(0);
 * print dnx_asym_ac_failover_mc_service_setup(0);
 * exit;
 *
 * 1: 1.1 Ingress lif:
 *        all 3 ingress ACs are none_protected, learn_info is port
 *    1.2 Egress lif:
 *        access egress ac is none_protected
 *        two protected egress ac, protection pointer is eg_failover_id

 *  tx 1 psrc=201 data=0x001100000013001100000012910001900899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 201, Destination port: 200/201/202, Learn_info: 0xc9
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 201, Learn_info: 0xca, out_lif is 0x1002
 *  tx 1 psrc=200 data=0x0011000000130011000000119100012c0899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 200, Destination port: 202, Learn_info: 0xc8
 *  l2 clear all
 *
 * 2: 2.1 Ingress lif:
 *        update the protection information. It's changed from none_proteced ACs to protected ACs
 *        The learn_info is changed from port to fec_if_primary/secondary
 *        do switchover between secondary and primary
 *    2.2 Egress lif
 *        update the protection information. The protection pointer is changed from eg_failover_id to eg_failover_id2
 *        do switchover between secondary and primary
 *  print dnx_asym_ac_fec_encap_failover_update(0);

 *  tx 1 psrc=201 data=0x001100000013001100000012910001900899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 201, Destination port: 200/201/202, Learn_info: 0xccce
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 201, Learn_info: 0xca, out_lif is 0x1002
 *  tx 1 psrc=200 data=0x0011000000130011000000119100012c0899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 200, Destination port: 202, Learn_info: 0xcccf
 *  print dnx_asym_ac_failover_switch(0,0);
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 200, out_lif is 0x1000
 *  print dnx_asym_ac_failover_switch(0,1);
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 201, out_lif is 0x1001
 *  l2 clear all

 * 3: 3.1 Ingress lif
 *        update the protection information. It keeps the protected ACs
 *        The learn_info is changed from fec_if_primary/secondary to fec_if_primary_2/secondary2
 *        do switchover between secondary and primary
 *        The difference of ingress lif from step 1 is the fec_id, but still with the same destination and same encap
 *    3.2 Egress lif
 *        do switchover between secondary and primary
 *  print dnx_asym_ac_fec_encap_failover_update(0);

 *  tx 1 psrc=201 data=0x001100000013001100000012910001900899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 201, Destination port: 200/201/202, Learn_info: 0xccd0
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 201, Learn_info: 0xca, out_lif is 0x1002
 *  tx 1 psrc=200 data=0x0011000000130011000000119100012c0899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 200, Destination port: 202, Learn_info: 0xccd1
 *  print dnx_asym_ac_failover_switch(0,0);
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 200, out_lif is 0x1000
 *  print dnx_asym_ac_failover_switch(0,1);
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 201, out_lif is 0x1001
 *  l2 clear all
 *
 * 4: 4.1 Ingress lif
 *        update the protection information. It's changed from protected ACs to none_protected ACs
 *        The learn_info is changed from fec_if_primary_2/secondary2 to port
 *  print dnx_asym_ac_fec_failover_cleanup(0);

 *  tx 1 psrc=201 data=0x001100000013001100000012910001900899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 201, Destination port: 200/201/202, Learn_info: 0xc9
 *  tx 1 psrc=202 data=0x001100000012001100000013910001f40899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 202, Destination port: 201, Learn_info: 0xca, out_lif is 0x1002
 *  tx 1 psrc=200 data=0x0011000000130011000000119100012c0899ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 200, Destination port: 202, Learn_info: 0xc8
 *  l2 clear all
 */

struct ac_info_s {
    bcm_port_t  port;
    bcm_vlan_t  vlan;
    bcm_gport_t gport;
    bcm_mac_t   mac_address;
};

/*  Main Struct  */
struct dnx_asym_ac_failover_s {
    bcm_gport_t fec_failover_id;
    bcm_gport_t eg_failover_id;
    bcm_gport_t eg_failover_id2;
    bcm_vlan_t  vsi;
    ac_info_s   in_ac_secondary;
    ac_info_s   in_ac_primary;
    ac_info_s   in_ac_access;
    bcm_gport_t vlan_port_egress_secondary_id;
    bcm_gport_t vlan_port_egress_primary_id;
};

/* Initialization of a global struct */
dnx_asym_ac_failover_s g_dnx_asym_ac_failover = {
     1,  /* FEC failover ID */
     2,  /* 1st egress failover ID */
     3,  /* 2nd egress failover ID */
     10, /* VSI */
     /*  AC configurations
     Phy Port    VLAN    gport   MAC*/
     {200,       300,     0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x11}}, /* In-AC-secondary */
     {201,       400,     0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x12}}, /* In-AC-primary */
     {202,       500,     0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x13}}, /* In-AC-access */
};
int fec_if_primary;

/*
 * create fec/egress failover_id
 */
int dnx_asym_ac_failover_create(int unit) {
    int rv = BCM_E_NONE;

    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC | BCM_FAILOVER_WITH_ID, g_dnx_asym_ac_failover.fec_failover_id);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_create for fec failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.fec_failover_id, rv);
        return rv;
    }

    /* create the 1st egress failover id */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP | BCM_FAILOVER_WITH_ID, g_dnx_asym_ac_failover.eg_failover_id);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_create for egress failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.eg_failover_id, rv);
        return rv;
    }

    /* create the 2nd egress failover id */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP | BCM_FAILOVER_WITH_ID, g_dnx_asym_ac_failover.eg_failover_id2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_create for egress failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.eg_failover_id2, rv);
        return rv;
    }

    return rv;
}

/*
 * The basic mc service setup
 */
int dnx_asym_ac_failover_mc_service_setup(int unit) {
    int rv = BCM_E_NONE;
    int i, j, vlan;
    bcm_vlan_port_t port_vlan;
    bcm_gport_t vlan_port_access_id;
    int match_vlan_list[3] = {g_dnx_asym_ac_failover.in_ac_secondary.vlan,
                              g_dnx_asym_ac_failover.in_ac_primary.vlan,
                              g_dnx_asym_ac_failover.in_ac_access.vlan};
    int port_list[3] = {g_dnx_asym_ac_failover.in_ac_secondary.port,
                        g_dnx_asym_ac_failover.in_ac_primary.port,
                        g_dnx_asym_ac_failover.in_ac_access.port};
    int encap_id_list[3];

    /* Create match_vlan/vsi and add port to vlan */
    for (i = 0; i < 3; i++) {
        vlan = match_vlan_list[i];
        rv = bcm_vlan_create(unit, vlan);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during bcm_vlan_create for vlan %d, rv - %d\n", proc_name, vlan, rv);
            return rv;
        }

        for (j = 0; j < 3; j++) {
            rv = bcm_vlan_gport_add(unit, vlan, port_list[j], 0);
            if (rv != BCM_E_NONE) {
                printf("%s(): Error during bcm_vlan_gport_add for vlan %d port %d, rv - %d\n", proc_name, vlan, port_list[j], rv);
                return rv;
            }
        }
    }

    rv = bcm_vlan_create(unit, g_dnx_asym_ac_failover.vsi);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_create for vsi %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.vsi, rv);
        return rv;
    }

    for (j = 0; j < 3; j++) {
        rv = bcm_vlan_gport_add(unit, g_dnx_asym_ac_failover.vsi, port_list[j], 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during during bcm_vlan_gport_add for vsi %d port %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.vsi, port_list[j], rv);
            return rv;
        }
    }

    /* Create secondary ingress AC */
    bcm_vlan_port_t_init(&port_vlan);
    port_vlan.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    port_vlan.port = g_dnx_asym_ac_failover.in_ac_secondary.port;
    port_vlan.match_vlan = g_dnx_asym_ac_failover.in_ac_secondary.vlan;
    port_vlan.vsi = g_dnx_asym_ac_failover.vsi;
    rv = bcm_vlan_port_create(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during during bcm_vlan_port_create \n
                for secondary ingress ac match_vlan %d port %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_secondary.vlan, g_dnx_asym_ac_failover.in_ac_secondary.port, rv);
        return rv;
    }
    g_dnx_asym_ac_failover.in_ac_secondary.gport = port_vlan.vlan_port_id;

    /* Create primary ingress AC */
    port_vlan.port = g_dnx_asym_ac_failover.in_ac_primary.port;
    port_vlan.match_vlan = g_dnx_asym_ac_failover.in_ac_primary.vlan;
    rv = bcm_vlan_port_create(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create \n
                for primary ingress ac match_vlan %d port %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_primary.vlan, g_dnx_asym_ac_failover.in_ac_primary.port, rv);
        return rv;
    }
    g_dnx_asym_ac_failover.in_ac_primary.gport = port_vlan.vlan_port_id;

    /* Create secondary egress AC */
    bcm_vlan_port_t_init(&port_vlan);
    port_vlan.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_EGRESS_PROTECTION;
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_NONE;
    port_vlan.egress_failover_id = g_dnx_asym_ac_failover.eg_failover_id;
    rv = bcm_vlan_port_create(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create \n
                for secondary egress ac egress_failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.eg_failover_id, rv);
        return rv;
    }
    g_dnx_asym_ac_failover.vlan_port_egress_secondary_id = port_vlan.vlan_port_id;
    encap_id_list[0] = g_dnx_asym_ac_failover.vlan_port_egress_secondary_id & 0x3fffff;

    /* Create primary egress AC */
    port_vlan.egress_failover_port_id = g_dnx_asym_ac_failover.vlan_port_egress_secondary_id;
    rv = bcm_vlan_port_create(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create \n
                for primary egress ac egress_failover_id %d egress_failover_port_id %d,rv - %d\n",proc_name, g_dnx_asym_ac_failover.egress_failover_id,g_dnx_asym_ac_failover.vlan_port_egress_secondary_id,rv);
        return rv;
    }
    g_dnx_asym_ac_failover.vlan_port_egress_primary_id = port_vlan.vlan_port_id;
    encap_id_list[1] = g_dnx_asym_ac_failover.vlan_port_egress_primary_id & 0x3fffff;

    /* Create in AC */
    bcm_vlan_port_t_init(&port_vlan);
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    port_vlan.port = g_dnx_asym_ac_failover.in_ac_access.port;
    port_vlan.match_vlan = g_dnx_asym_ac_failover.in_ac_access.vlan;
    rv = bcm_vlan_port_create(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create \n
                for access ac match_vlan %d port %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_access.vlan, g_dnx_asym_ac_failover.in_ac_access.port, rv);
        return rv;
    }
    g_dnx_asym_ac_failover.in_ac_access.gport = port_vlan.vlan_port_id;
    encap_id_list[2] = g_dnx_asym_ac_failover.in_ac_access.gport & 0x3fffff;

    /* Add access ac to vswitch */
    rv = bcm_vswitch_port_add(unit, g_dnx_asym_ac_failover.vsi, g_dnx_asym_ac_failover.in_ac_access.gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_add \n
                for vlan gport %d added to vsi %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_access.gport, g_dnx_asym_ac_failover.vsi, rv);
        return rv;
    }

    /* Create multicast group and add ports to MC group */
    rv = multicast__open_egress_mc_group_with_gports(unit, g_dnx_asym_ac_failover.vsi, port_list, encap_id_list, 3, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__open_egress_mc_group_with_gports rv - %d\n", proc_name, rv);
        return rv;
    }

    rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, g_dnx_asym_ac_failover.vsi, g_dnx_asym_ac_failover.vsi);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__open_fabric_mc_or_ingress_mc_for_egress_mc rv - %d\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/*
 *update the fec protection for ingress lif and encap protection for egress lif
*/
int dnx_asym_ac_fec_encap_failover_update(int unit) {
    int rv = BCM_E_NONE;
    sand_utils_l3_fec_s fec_structure;
    bcm_vlan_port_t vlan_port;
    int fec_gport;
    int fec_if_secondary;
    bcm_gport_t fec_failover_id = g_dnx_asym_ac_failover.fec_failover_id;

    /* Create secondary FEC */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.flags        = BCM_L3_INGRESS_ONLY;
    fec_structure.failover_id  = fec_failover_id;
    fec_structure.destination  = g_dnx_asym_ac_failover.in_ac_secondary.port;
    fec_structure.tunnel_gport = g_dnx_asym_ac_failover.vlan_port_egress_secondary_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during sand_utils_l3_fec_create secondary fec creation \n
                for port %d encap_id %d failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_secondary.port, g_dnx_asym_ac_failover.vlan_port_egress_secondary_id, fec_failover_id, rv);
        return rv;
    }
    fec_if_secondary = fec_structure.l3eg_fec_id;

    /* Create primary FEC */
    fec_structure.destination  = g_dnx_asym_ac_failover.in_ac_primary.port;
    fec_structure.tunnel_gport = g_dnx_asym_ac_failover.vlan_port_egress_primary_id;
    fec_structure.failover_if_id = fec_if_secondary;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during  sand_utils_l3_fec_create primary fec creation \n
                for port %d encap_id %d failover_id %d failover_if_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_primary.port, g_dnx_asym_ac_failover.vlan_port_egress_primary_id, fec_failover_id, fec_if_secondary, rv);
        return rv;
    }
    fec_if_primary = fec_structure.l3eg_fec_id;

    /* Update secondary ingress AC with FEC protection */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = g_dnx_asym_ac_failover.in_ac_secondary.gport;
    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_find secondary ingress ac \n
                for vlan gport %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_secondary.gport, rv);
        return rv;
    }

    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport, fec_if_secondary);
    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    vlan_port.failover_port_id = fec_gport;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create updating secondary ingress AC with FEC protection \n
                for vlan gport %d using failover_port_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_secondary.gport, fec_if_secondary, rv);
        return rv;
    }

    /* Update primary ingress AC with FEC protection */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = g_dnx_asym_ac_failover.in_ac_primary.gport;
    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_find primary ingress ac \n
                for vlan gport %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_primary.gport, rv);
        return rv;
    }

    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport, fec_if_primary);
    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    vlan_port.failover_port_id = fec_gport;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create updating primary ingress AC with FEC protection \n
                for vlan gport %d using failover_port_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_primary.gport, fec_if_primary, rv);
        return rv;
    }

    /* Update secondary egress AC with new failover id */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = g_dnx_asym_ac_failover.vlan_port_egress_secondary_id;
    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_find secondary egress ac \n
                for vlan gport %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.vlan_port_egress_secondary_id, rv);
        return rv;
    }

    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    vlan_port.egress_failover_id = g_dnx_asym_ac_failover.eg_failover_id2;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create updating secondary egress AC with egress encap protection \n
                for vlan gport %d using egress_failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.vlan_port_egress_secondary_id, g_dnx_asym_ac_failover.eg_failover_id2, rv);
        return rv;
    }

    /* Update primary egress AC with new failover id */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = g_dnx_asym_ac_failover.vlan_port_egress_primary_id;
    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_find primary egress ac \n
                for vlan gport %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.vlan_port_egress_primary_id, rv);
        return rv;
    }

    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    vlan_port.egress_failover_id = g_dnx_asym_ac_failover.eg_failover_id2;
    vlan_port.egress_failover_port_id = g_dnx_asym_ac_failover.vlan_port_egress_secondary_id;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create updating primary egress AC with egress encap protection \n
                for vlan gport %d using egress_failover_id %d egress_failover_port_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.vlan_port_egress_primary_id, g_dnx_asym_ac_failover.eg_failover_id2, g_dnx_asym_ac_failover.vlan_port_egress_secondary_id, rv);
        return rv;
    }

    return rv;
}

/*
 *update ingress lif to none_protection
*/
int dnx_asym_ac_fec_failover_cleanup(int unit) {
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /* Update secondary ingress AC with none-protection */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = g_dnx_asym_ac_failover.in_ac_secondary.gport;
    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_find secondary ingress ac \n
                for vlan gport %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_secondary.gport, rv);
        return rv;
    }

    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    vlan_port.failover_port_id = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create updating secondary ingress AC with none protection,rv - %d\n", proc_name, rv);
        return rv;
    }

    /* Update primary ingress AC with none-protection */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = g_dnx_asym_ac_failover.in_ac_primary.gport;
    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_find primary ingress ac \n
                for vlan gport %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.in_ac_primary.gport, rv);
        return rv;
    }

    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    vlan_port.failover_port_id = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vlan_port_create updating primary ingress AC with none protection,rv - %d\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/*
 * Failover switchover
 */
int dnx_asym_ac_failover_switch(int unit, int is_primary) {
    int rv;

    rv = bcm_failover_set(unit, g_dnx_asym_ac_failover.fec_failover_id, is_primary);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_set for fec failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.fec_failover_id, rv);
        return rv;
    }

    rv = bcm_failover_set(unit, g_dnx_asym_ac_failover.eg_failover_id2, is_primary);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_set for egress failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.eg_failover_id2, rv);
        return rv;
    }

    return rv;
}

/*
 * fec/egress failover_id destroy
 */
int dnx_asym_ac_failover_destroy(int unit) {
    int rv = BCM_E_NONE;

    rv = bcm_failover_destroy(unit, g_dnx_asym_ac_failover.fec_failover_id);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_destroy for fec failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.fec_failover_id, rv);
        return rv;
    }

    rv = bcm_failover_destroy(unit, g_dnx_asym_ac_failover.eg_failover_id);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_destroy for egress failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.eg_failover_id, rv);
        return rv;
    }

    rv = bcm_failover_destroy(unit, g_dnx_asym_ac_failover.eg_failover_id2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_failover_destroy for egress failover_id %d, rv - %d\n", proc_name, g_dnx_asym_ac_failover.eg_failover_id2, rv);
        return rv;
    }

    return rv;
}