/* 
 * $Id: cint_dnx_vswitch_cross_connect_p2p_one_way.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* 
 * the cint creates one port of type vlan. 
 * The created vlan port is uni-directionally cross-conected to a system port (<port2>)  
 * Test Scenario - start 
 *
 * ./bcm.user
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_mpls.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_mpls_port.c
 * cint ../../../../src/examples/dnx/cint_dnx_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/vswitch/cint_dnx_vswitch_cross_connect_p2p_one_way.c
 * cint
 * dnx_vswitch_cross_connect_p2p_one_way_run(0,201,203);
 * exit;
 *
 * // AT_Dnx_Cint_vswitch_cross_connect_p2p_one_way  : Step4: Send packet and receive result 
 * tx 1 psrc=201 data=0x0000b0d9fbd200002516c8ba810000640899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 * // Source port: 0, Destination port: 0 
 * // Data: 0x0000b0d9fbd200002516c8ba810000640899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * Test Scenario - end
 */


int verbose = 3;

struct cross_connect_info_s {

    /* physical ports */
    bcm_port_t port_1; 
    bcm_port_t port_2; 

    /* logical ports */
    bcm_gport_t vlan_port_1; /* vlan ports */

    bcm_vswitch_cross_connect_t gports;

    /* data for vlan port */
    bcm_vlan_t outer_vlan_base;
};

cross_connect_info_s cross_connect_info;

void
cross_connect_info_init(int port1, int port2) {

    bcm_vswitch_cross_connect_t_init(&cross_connect_info.gports);

    cross_connect_info.port_1 = port1; 
    cross_connect_info.port_2 = port2;

    cross_connect_info.outer_vlan_base = 100;
}

/* create vlan_port (Logical interface identified by port-vlan-vlan) */
int
vlan_port_create(
    int unit,
    uint32 flags,
    bcm_port_t port_id,  
    bcm_vlan_t match_vlan, /* outer vlan */
    bcm_gport_t *gport
    ){

    int rv;
    bcm_vlan_port_t vp;
    bcm_pbmp_t pbmp, ubmp;

    rv = bcm_vlan_create(unit, match_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", match_vlan);
    }

    bcm_vlan_port_t_init(&vp);
    vp.flags = flags;
  
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = match_vlan; 

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    if(verbose >= 2) {
        printf("created vlan-port:0x0%8x  \n", vp.vlan_port_id);
        printf("  encap-id: 0x%8x  \n", vp.encap_id);
        printf("  in-port: %d\n", vp.port);
        printf("  in-vlan: %d\n", vp.match_vlan);
    }

    /* return port id to user */
    *gport = vp.vlan_port_id;
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    } else {
        BCM_PBMP_CLEAR(pbmp); /* tagged */
        BCM_PBMP_CLEAR(ubmp); /* untagged */
        BCM_PBMP_PORT_ADD(pbmp, vp.port);
        
        /* set vlan port membership */
        rv = bcm_vlan_port_add(unit, match_vlan, pbmp, ubmp);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_add with vlan %d\n", match_vlan);
            print rv;
            return rv;
        }
    }
    return BCM_E_NONE;
}


int 
dnx_vswitch_cross_connect_p2p_one_way_run(int unit, int port1, int port2) {
    int port, i;
    int rv;
    int tag_format_stag = 4;

    cross_connect_info_init(port1, port2);
    
    bcm_port_tpid_class_t port_tpid_class;
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    port_tpid_class.tpid1 = 0x8100; /* port_tpid_info1.outer_tpids[0]; */
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = tag_format_stag;
    port_tpid_class.port = port1;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set with port_2\n");
        print rv;
        return rv;
    }
        
    /* create vlan port */
    rv = vlan_port_create(unit,
                          0,
                          cross_connect_info.port_1, 
                          cross_connect_info.outer_vlan_base,  
                          &(cross_connect_info.vlan_port_1));
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_1);
        return rv;
    }
    if(verbose >= 1) {
        printf("created vlan_port_1 : 0x0%8x\n", cross_connect_info.vlan_port_1);
    }

    cross_connect_info.gports.port1 = cross_connect_info.vlan_port_1;
    cross_connect_info.gports.port2 = cross_connect_info.port_2;
    cross_connect_info.gports.flags |= BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL; 

    /* cross connect the 2 ports */
    if(verbose >= 1) {
        printf("connect port1:0x%08x with port2:0x%08x \n", cross_connect_info.gports.port1, cross_connect_info.gports.port2);
    }

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_info.gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

