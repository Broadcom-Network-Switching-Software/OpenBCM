/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
* File: cint_vlan_basic_bridge.c
* Purpose: Example of Basic Bridging VLAN membership
*
* Calling sequence for case 1 (vsi =vlan):
*  1. Create vlan and Add port for basic bridging, Add l2 address for unicast.
*        - Call bcm_vlan_create()
*        - Call bcm_vlan_port_add()
*        - Call bcm_l2_addr_add()
*  2. Remove port from vlan and Verify the traffic
*        - Call bcm_vlan_port_remove()
*
* Calling sequence for case 2 (vsi !=vlan):
*  1. Create vlan and vsi, Create the vlan_port with vlan and vsi, Add port for bridging, Add l2 address for unicast.
*        - Call bcm_vlan_create()
*        - Call bcm_vlan_port_create()
*        - Call bcm_vlan_port_add()
*        - Call bcm_l2_addr_add()
*  2. Remove port from vlan and Verify the traffic
*        - Call bcm_vlan_port_remove()
*  3. Create and remove a specific number of vlan_port for both cases:
*        - Call vlan_basic_bridge_vlan_port_test()
*
*Calling sequence for case 3 (vsi = vlan + vsi_base):
*      - Call bcm_vswitch_create_with_id()    - create vswitch (vsi= vsi_base+vid)
*      - Call bcm_vlan_create()               - create vlan
*      - Call bcm_multicast_create()          - create multicast group
*      - Call bcm_vlan_port_create()          - allocate lif only (see parameters)
*      - Call bcm_port_match_add()            - associate port with allocated lif
*      - Call bcm_vswitch_port_add()          - add port to created vsi
*      - Call bcm_multicast_egress_add()      - add port to multicast group for flooding
*      - Call bcm_l2_addr_add()               - add l2 addresses fo unicast
*
* Traffic:
*  1. For case vlan=vsi=10.
*  unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA 00:00:00:00:00:02
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is transmitted to port 2.
*
*  Flooding 
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is flooded to port 2(as DA is not known).
*
*  2. For case vlan=20 vsi=30.
*  unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA 00:00:00:00:00:02
*          -   VLAN tag: VLAN tag type 0x8100, VID =20
*      -   Packet is transmitted to port 2.
*
*  Flooding 
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =20
*      -   Packet is flooded to port 2(as DA is not known).
*
*  3. For case vlan=20 vsi_base = 4096 => vsi = vlan+vsi_base.
*  unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA 00:00:00:00:00:02
*          -   VLAN tag: VLAN tag type 0x8100, VID =20
*      -   Packet is transmitted to port 2.
*
*  Flooding
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =20
*      -   Packet is flooded to port 2(as DA is not known).
* To Activate Above Settings Run: 
*      BCM> cint ../../../../src/examples/dpp/cint_port_tpid.c 
*      BCM> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
*      BCM> cint ../../../../src/examples/dpp/cint_vlan_basic_bridge.c
*      BCM> cint
*      cint> vlan_basic_bridge_run(unit, vid);
*      cint> vlan_basic_bridge_run2(unit, vid, vsi);
*      cint> vlan_base_vid_run(unit,vid,vsi_base);     ----> in order to run this scenario avoid running vlan_basic_bridge_run&vlan_basic_bridge_run2 before
*/

struct vlan_bridge_info_s{
    int ports[10];
    bcm_port_t gport[10];
    int nof_ports;
};

vlan_bridge_info_s vlan_bridge_info;
bcm_gport_t gport_lif1;
bcm_gport_t gport_lif2;

int vlan_basic_bridge_init(int unit, int port1, int port2){
    vlan_bridge_info.ports[0] = port1;
    vlan_bridge_info.ports[1] = port2;
    vlan_bridge_info.nof_ports = 2;

    return BCM_E_NONE;
}

int vlan_basic_bridge_port_add(int unit, bcm_vlan_t vid, int port){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

int vlan_basic_bridge_port_remove(int unit, bcm_vlan_t vid, int port){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_mac_t mac;

    /* remove port from vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_port_remove(unit, vid, pbmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

/*vsi == vlan*/
int vlan_basic_bridge_run(int unit, bcm_vlan_t vid){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_mac_t mac;
    int port_id, port, vlan_domain;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        BCM_PBMP_PORT_ADD(pbmp, vlan_bridge_info.ports[port_id]);
    }
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    /* Set a different vlan domain for the ports */
    for (port_id = 0 ; port_id < vlan_bridge_info.nof_ports; port_id++) {
        port = vlan_bridge_info.ports[port_id];
        rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
        if (rv != BCM_E_NONE) {
          printf("Error, bcm_port_class_set, port %d, vlan_domain %d\n", port, port);
          return rv;
        }
    }

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = 2;
        rv = vlan_basic_bridge_add_l2_addr_to_gport(unit, vlan_bridge_info.ports[port_id], mac, vid);
        if (rv != BCM_E_NONE) {
          printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
          return rv;
        }
    }

    /* On JR2, need to create multicast group and add ports via multicast APIs*/
    if (is_device_or_above(unit, JERICHO2)){
        bcm_multicast_t mc_group;
        bcm_multicast_replication_t rep_array;
        int flags;
        mc_group = vid;

        rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID| BCM_MULTICAST_TYPE_L2, &mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP;

        for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
            bcm_multicast_replication_t_init(&rep_array);
            rep_array.port = vlan_bridge_info.ports[port_id];
            rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x \n", mc_group, port_id);
                return rv;
            }
        }
        
        /* Configure the MC group to work with both cores */
        rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_group, mc_group);
        if (rv != BCM_E_NONE) {
           printf("Error, multicast__open_fabric_mc_or_ingress_mc_for_egress_mc \n");
           return rv;
        }
    }

    return rv;
}

int vlan_basic_bridge_revert(int unit, bcm_vlan_t vid){
    int rv;
    int port_id;
    bcm_mac_t mac;

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = vlan_bridge_info.ports[port_id];
        rv = bcm_l2_addr_delete(unit, mac, vid);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
          printf("Error, bcm_l2_addr_delete rv %d\n", rv);
          return rv;
        }
    }

    rv = bcm_vlan_destroy(unit, vid);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)){
        printf("Error, bcm_vlan_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)){
        rv = bcm_multicast_destroy(unit,vid);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_multicast_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
            return rv;
        }
    }

    return rv;
}

/*
 * add L2 entry points to the given gport
 */
int
vlan_basic_bridge_add_l2_addr_to_gport(int unit, int gport_id, bcm_mac_t mac1, bcm_vlan_t v1){
    int rv;
    bcm_gport_t g1;
    /*bcm_mac_t mac1;*/
    bcm_l2_addr_t l2_addr1;
    /*bcm_vlan_t v1;*/

    /* local: MACT, add entry points to local-port */
    bcm_l2_addr_t_init(&l2_addr1,mac1,v1); 
    l2_addr1.flags = 0x00000020; /* static entry */
    g1 = gport_id;
    l2_addr1.port = g1;
    rv = bcm_l2_addr_add(unit,&l2_addr1);

    return rv;
}

int vlan_basic_bridge_vlan_port_create(int unit, bcm_port_t port, bcm_vlan_t vid, 
    bcm_vlan_t vsi, bcm_gport_t *gport){
    int rv = 0; 
    bcm_vlan_port_t vlan_port;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan  = vid;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid;
    vlan_port.port = port;
    /*
    * On JR2, if using VSI field,it's not symentrical, on egress side, have to set flag:BCM_VLAN_PORT_VLAN_TRANSLATION
    * here only inlif used, so for JR2, set flag:BCM_VLAN_PORT_CREATE_INGRESS_ONLY
    */
    if (is_device_or_above(unit, JERICHO2)){
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    }
    vlan_port.vsi  = vsi;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create unit %d, vid %d, vsi %d, rv %d\n", unit, vid, vsi, rv);
        return rv;
    }

    /* egress_vlan will be used at eve */
    vlan_port.egress_vlan = vid;
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_vlan_port_create_to_translation\n");
            return rv;
        }
    }

    *gport = vlan_port.vlan_port_id;

    return rv;
}

int vlan_basic_bridge_port_remove2(int unit, bcm_vlan_t vid, bcm_vlan_t vsi, int port){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_mac_t mac;

    /* remove port from vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_port_remove(unit, vid, pbmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

/*vsi != vlan*/
int vlan_basic_bridge_run2(int unit, bcm_vlan_t vid, bcm_vlan_t vsi){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int port_id;
    bcm_gport_t gport;
    bcm_mac_t mac;
    bcm_port_t port;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vsi, rv);
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
            port = vlan_bridge_info.ports[port_id];
            port_tpid_init(port, 1, 1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set\n");
                print rv;
                return rv;
            }
        }

        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        port = vlan_bridge_info.ports[port_id];
        rv = vlan_basic_bridge_vlan_port_create(unit, port, vid, vsi, &gport);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_basic_bridge_vlan_port_create failed, rv %d\n", rv);
        }

        vlan_bridge_info.gport[port_id] = gport;

        /* add port to vlan */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
        BCM_PBMP_CLEAR(ubmp);
        rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
            return rv;
        }

        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = 2;
        /* JR2 outlif is in EEDB, so add outlif in MACT entry */
        rv = vlan_basic_bridge_add_l2_addr_to_gport(unit, port, mac, vsi);
        if (rv != BCM_E_NONE) {
          printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
          return rv;
        }
    }

    /* On JR2, need to create multicast group and add ports via multicast APIs*/
    if (is_device_or_above(unit, JERICHO2)){
        bcm_multicast_t mc_group;
        bcm_multicast_replication_t rep_array;
        int flags;
        mc_group = vsi;

        rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID| BCM_MULTICAST_TYPE_L2, &mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP;

        for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
            bcm_multicast_replication_t_init(&rep_array);
            rep_array.port = vlan_bridge_info.ports[port_id];
            rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x \n", mc_group, port_id);
                return rv;
            }
        }
        
        /* Configure the MC group to work with both cores */
        rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_group, mc_group);
        if (rv != BCM_E_NONE) {
           printf("Error, multicast__open_fabric_mc_or_ingress_mc_for_egress_mc \n");
           return rv;
        }
    }

    return rv;
}

int vlan_basic_bridge_revert2(int unit, bcm_vlan_t vid, bcm_vlan_t vsi){
    int rv;
    int port_id;
    bcm_mac_t mac;

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        rv = bcm_vlan_port_destroy(unit, vlan_bridge_info.gport[port_id]);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            printf("Error, bcm_vlan_port_destroy unit %d, gport 0x%x, rv %d\n", unit, vlan_bridge_info.gport[port_id], rv);
            return rv;
        }

        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = vlan_bridge_info.ports[port_id];
        rv = bcm_l2_addr_delete(unit, mac, vsi);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
          printf("Error, bcm_l2_addr_delete rv %d\n", rv);
          return rv;
        }
    }

    rv = bcm_vlan_destroy(unit, vid);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, bcm_vlan_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    rv = bcm_vlan_destroy(unit, vsi);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, bcm_vlan_destroy unit %d, vsi %d, rv %d\n", unit, vsi, rv);
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)){
        rv = bcm_multicast_destroy(unit,vsi);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_multicast_destroy unit %d, vsi %d, rv %d\n", unit, vsi, rv);
            return rv;
        }
    }

    return rv;
}


int vlan_basic_bridge_vlan_port_vsi_base_create(int unit, bcm_port_t port, bcm_vlan_t vid,
    bcm_vlan_t vsi_base, bcm_gport_t *gport){
    int rv = 0;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    bcm_vlan_port_t_init(&vlan_port);

    /* Allocating LIF entry */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.vsi      = vsi_base;
    vlan_port.port     = port;
    vlan_port.flags    = BCM_VLAN_PORT_VSI_BASE_VID;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    bcm_port_match_info_t_init(&match_info);

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.port = port;
    match_info.match_vlan = vid;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id ,&match_info); /* associating the port to lif */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_match_add\n");
        print rv;
        return rv;
    }

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_vlan_port_create_to_translation\n");
            return rv;
        }
    }

    *gport = vlan_port.vlan_port_id;

    return rv;
}


/*vsi = vlan + vsi_base*/
int vlan_base_vid_run(int unit, bcm_vlan_t vid, bcm_vlan_t vsi_base){

    int rv;
    int port_id;
    bcm_gport_t gport;
    bcm_mac_t mac;
    bcm_port_t port;
    int flags;
    bcm_vlan_t vsi_base_vid;
    int encap_id;
    bcm_multicast_t mc_group;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    vsi_base_vid = vid + vsi_base;

    rv = bcm_vswitch_create_with_id(unit, vsi_base_vid);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vsi_base + vid, rv);
        return rv;
    }

    mc_group = vsi_base_vid;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID| BCM_MULTICAST_TYPE_L2, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
            port = vlan_bridge_info.ports[port_id];
            port_tpid_init(port, 1, 1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set\n");
                print rv;
                return rv;
            }
        }

        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        port = vlan_bridge_info.ports[port_id];
        rv = vlan_basic_bridge_vlan_port_vsi_base_create(unit, port, vid, vsi_base, &gport);

        if (rv != BCM_E_NONE) {
            printf("Error, vlan_basic_bridge_vlan_port_vsi_base_create failed, rv %d\n", rv);
        }

        vlan_bridge_info.gport[port_id] = gport;

        /* Set Ingress Vlan membership per Port X Vlan */
        flags = BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
        rv = bcm_vlan_gport_add(unit, vid, gport, flags);
        if (rv != BCM_E_NONE){
            printf("Error, vlan_basic_bridge_vlan_port_vsi_base_create failed(ingress membership), rv %d\n", rv);
            return rv;
        }       

        rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, gport, &encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group, port, gport);
            return rv;
        }

        rv = bcm_multicast_egress_add(unit, mc_group, port, encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_egress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group, port, encap_id);
            return rv;
        }

        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = 2 + port_id;
        rv = vlan_basic_bridge_add_l2_addr_to_gport(unit, port, mac, vsi_base_vid);
        if (rv != BCM_E_NONE) {
          printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
          return rv;
        }

    }

    return rv;
}


/*When same_interface_filter is diabled, two packets are returned when flood occurs(unknowd DA) 
 *When the filter is enables, the packet sent to the tx port is dropped(and you recieve 1 packet)
 *We disabled the filter so we now receive two packets during flood and one during unicast to a known DA.
 *This makes the cint's Dvapi test more informative"
 */
int set_same_interface_filter(int unit, int port, int enabled){
    int rv = 0;
    
    bcm_port_control_set(unit, port, bcmPortControlBridge, enabled);
    if ((rv != BCM_E_NONE)) {
        printf("Error, bcm_port_control_set failed: %d\n", rv);
        return rv;
    } else if (enabled){
        printf("Same interface filter is enabled successfully\n");
    } else {
        printf("Same interface filter is disabled successfully\n");   
    }
    return rv;
}

/* 
 * Verify a specific number of AC-LIFs can be successfully created and removed.
 * Example: vlan_basic_bridge_vlan_port_test(unit, 15, 1, 4001, 1, 9);
 *          This case create and remove 32,000(4000*8) AC-LIFs.
 */
int 
vlan_basic_bridge_vlan_port_test(int unit, bcm_port_t port, int outerLoVlan, int outerHighVlan, int innerLoVlan, int innerHighVlan)
{
    int rv1 = 0, rv2 = 0;
    bcm_vlan_t outervlan, innervlan;
    bcm_vlan_port_t vlan_port;
    int vpreq = 0, vpaddcount = 0, vp = 0;
    bcm_gport_t *pvpgport = NULL;

    if (outerLoVlan >= outerHighVlan || innerLoVlan >= innerHighVlan)
        return BCM_E_PARAM;
    vpreq = (outerHighVlan - outerLoVlan) * (innerHighVlan - innerLoVlan);
    if (vpreq > 32768) {
        printf("Please test vlan port less than 32768!\n");
        return BCM_E_PARAM;
    }
    
    pvpgport = sal_alloc(4*vpreq, "Cint: vlan_basic_bridge_vlan_port_test");
    if (NULL == pvpgport) {
        printf("No memory for allocation pvpgport buffer!");
        return BCM_E_MEMORY;
    }    
        
    printf("Start test vlan port\n");
    /*
     * Create vlan port.
     */
    for (outervlan = outerLoVlan; outervlan < outerHighVlan; outervlan++) {
        for (innervlan = innerLoVlan; innervlan < innerHighVlan; innervlan++) {
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.port = port;      
            vlan_port.match_vlan = outervlan;
            vlan_port.match_inner_vlan = innervlan;
            rv1 = bcm_vlan_port_create(unit, &vlan_port);
            if(rv1 != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_create unit %d, port %d, ovid %d, ivid %d, vpaddcount %d, rv %d\n", unit, port, outervlan, innervlan, vpaddcount, rv1);
                break;
            }
            pvpgport[vpaddcount] = vlan_port.vlan_port_id;
            vpaddcount++;
            if ((vpaddcount%1000) == 0)
                printf(".");
        }
        if (rv1 != BCM_E_NONE)
            break;
    }

    /* 
     * Remove vlan port.
     */
    for (vp = 0; vp < vpaddcount; vp++) {
        rv2 = bcm_vlan_port_destroy(unit, pvpgport[vp]);
        if(rv2 != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy unit %d, vpdelcount %d, rv %d\n", unit, vp, rv2);
            break;
        }
        if ((vp%1000) == 0)
            printf(".");
    }

    printf("\nvlan port create %d, destroy %d\n", vpaddcount, vp);

    sal_free(pvpgport);
        
    return rv1|rv2;     
}


/*add it to verify the flooding(mc,uc,bc) for port black hole*/
int  add_port_to_vlan(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    bcm_vlan_t vlan =10;
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp,port_1);
    BCM_PBMP_PORT_ADD(pbmp,port_2);
    
    rv =bcm_vlan_create(unit,vlan);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create  rv %d\n", rv);
        return rv;
    }
    
    rv =bcm_vlan_port_add(unit,vlan,pbmp,ubmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_add  rv %d\n", rv);
        return rv;
    }
    return BCM_E_NONE;
}


int  remove_port_from_vlan(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    bcm_vlan_t vlan =10;
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp,port_1);
    BCM_PBMP_PORT_ADD(pbmp,port_2);
    
    rv =bcm_vlan_port_remove(unit,vlan,pbmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove  rv %d\n", rv);
        return rv;
    }
    
    rv =bcm_vlan_destroy(unit,vlan);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_destroy  rv %d\n", rv);
        return rv;
    }
    return BCM_E_NONE;
}


int verify_lif_flooding(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan =100;
    bcm_vlan_port_t vlan_port1;
    bcm_vlan_port_t vlan_port2;
    bcm_pbmp_t pbmp,ubmp;
    int encap_id;
    int flags=0;
    bcm_multicast_t mc_group = 100;
    bcm_vlan_t  new_vsi =100;
    
    bcm_vlan_create(unit,vlan);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create  rv %d\n", rv);
        return rv;
    }
    
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp,port1);
    BCM_PBMP_PORT_ADD(pbmp,port2);
    rv=bcm_vlan_port_add(unit,vlan,pbmp,ubmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_add  rv %d\n", rv);
        return rv;
    }
    
    rv=bcm_vswitch_create(unit,&new_vsi);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vswitch_create  rv %d\n", rv);
        return rv;
    }
    
    bcm_vlan_port_t_init(vlan_port1);
    vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port1.port =port1;
    vlan_port1.match_vlan =100;
    rv=bcm_vlan_port_create(unit,&vlan_port1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_create  rv %d\n", rv);
        return rv;
    }
    
    bcm_vlan_port_t_init(vlan_port2);
    vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port2.port =port2;
    vlan_port2.match_vlan =100;
    rv=bcm_vlan_port_create(unit,&vlan_port2);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_create  rv %d\n", rv);
        return rv;
    }
    
    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID|BCM_MULTICAST_TYPE_L2;
    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_multicast_create  rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_multicast_vlan_encap_get(unit, 100, port2, vlan_port2.vlan_port_id, &encap_id);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_multicast_vlan_encap_get  rv %d\n", rv);
        return rv;
    }
    
     rv = bcm_multicast_ingress_add(unit, 100, port2, encap_id);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vswitch_port_add  rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vswitch_port_add(unit, 100, vlan_port1.vlan_port_id);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vswitch_port_add  rv %d\n", rv);
        return rv;
    }
    rv =  bcm_vswitch_port_add(unit, 100, vlan_port2.vlan_port_id);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vswitch_port_add  rv %d\n", rv);
        return rv;
    }
    
    gport_lif1 = vlan_port1.vlan_port_id;
    gport_lif2 = vlan_port2.vlan_port_id;

    return rv;
}

int disable_lif_flooding(int unit)
{
    int rv =BCM_E_NONE;
    rv = bcm_port_control_set(unit, gport_lif2, bcmPortControlFloodUnknownUcastGroup, BCM_GPORT_BLACK_HOLE);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    return rv;
}

int lif_flooding_clear(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan1 =100;
    int encap_id;
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp,port1);
    BCM_PBMP_PORT_ADD(pbmp,port2);

    rv = bcm_vswitch_port_delete(unit, vlan1, gport_lif1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    rv = bcm_vswitch_port_delete(unit, vlan1, gport_lif2);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_multicast_vlan_encap_get(unit, vlan1, port2, gport_lif2, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", 100, port2, gport_lif2);
        return rv;
    }
    
    
    rv = bcm_vlan_port_destroy(unit,gport_lif2);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_destroy ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_port_destroy(unit,gport_lif1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_destroy ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_port_remove(unit,vlan1,pbmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_destroy(unit, vlan1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_multicast_ingress_delete(unit, vlan1, port2, encap_id);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    rv = bcm_multicast_destroy(unit, vlan1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    rv = bcm_vswitch_destroy(unit,vlan1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    return rv;
}

