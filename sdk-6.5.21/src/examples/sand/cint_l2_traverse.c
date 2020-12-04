/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Mact Management~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_l2_traverse.c
 * Purpose: Example demonstrates how to traverse the MACT.
 *
 * Demonstrate:
 *  - aging enable and aging time setting
 *  - add mact that points to unicast/multicast destinations
 *  - print the content of the MAC Table
 *
 * Default settings:
 * It is assumed diag_init is executed:
 *  o    All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  o    PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  o    By default, learning is managed by device without interference from the CPU.
 *  o    By default, aging is disabled.
 *  o    All VLANs are mapped to Default STG (0).
 *  o    STP state of all ports in default topology is set to forwarding.
 *
 * Note: Without running diag_init, ports are not associated with any TPID,
 *       and thus all packets would be considered as untagged.
 *       Besides, only untagged packets are accepted.
 *
 * Traffic:
 * Flooding (sending packet with unknown DA):
 *  o    Send Ethernet packet with TPID 0x8100 and VID 1 and unknown DA.
 *  o    Packet is flooded to all VLAN members.
 *  o    Packet is accepted in all ports without any editing.
 *  o    The SA of the packet is learned:
 *      o    Call l2_print_mact(unit) to print MAC table content.
 * Sending packet to known DA:
 *  o    Send Ethernet packet with TPID 0x8100 and VID 1 and known DA.
 *  o    Packet will be forwarded to single port according to MAC table.
 * Sending packet to MC group:
 *  o    Send Ethernet packet with TPID 0x8100 and VID 1 and DA points to mulitcast group
 *  o    Packet is duplicated to members of the multicast (by default packet will be forwarded to ports 1 and 4)
 *
 * Remarks:
 *  o    In all cases above, the packet will not be sent back to the incoming port.
 *      -    For flooding, packet is received on all ports except the incoming port.
 *      -    For Unicast (known DA) sending packet to same port, packet is discarded.
 *
 * Traverse:
 *  o    L2 traverse.
 *      -    traverse all entries in the l2 table.
 *           flags = 0;
 *           print l2_traverse(unit, l2_traverse, flags, NULL, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching STATIC entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC;
 *           print l2_traverse(unit, l2_match_traverse, flags, NULL, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching VLAN entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_VLAN;
 *           l2_addr.vid = l2_mact_table_info1.vid1;
 *           print l2_traverse(unit, l2_match_traverse, flags, &l2_addr, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching MAC entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_MAC;
 *           l2_addr.mac[0] = 0;
 *           l2_addr.mac[1] = 0;
 *           l2_addr.mac[2] = 0;
 *           l2_addr.mac[3] = 0;
 *           l2_addr.mac[4] = 0x01;
 *           l2_addr.mac[5] = 0x30;
 *           print l2_traverse(unit, l2_match_traverse, flags, &l2_addr, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching UNICAST DEST entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_DEST;
 *           BCM_GPORT_LOCAL_SET(l2_addr.port, l2_mact_table_info1.dest_port_1);
 *           print l2_traverse(unit, l2_match_traverse, flags, &l2_addr, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching MULTICAST DEST entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_DEST;
 *           BCM_GPORT_MCAST_SET(l2_addr.port, l2_mact_table_info1.mc_id);
 *           print l2_traverse(unit, l2_match_traverse, flags, &l2_addr, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching MAC+VLAN entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_MAC |
 *                   BCM_L2_TRAVERSE_MATCH_VLAN;
 *           l2_addr.vid = l2_mact_table_info1.vid1;
 *           l2_addr.mac[0] = 0;
 *           l2_addr.mac[1] = 0;
 *           l2_addr.mac[2] = 0;
 *           l2_addr.mac[3] = 0;
 *           l2_addr.mac[4] = 0x01;
 *           l2_addr.mac[5] = 0x30;
 *           print l2_traverse(unit, l2_match_traverse, flags, &l2_addr, NULL);
 *
 *  o    L2 match traverse.
 *      -    traverse matching MAC+VLAN+DEST entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_MAC |
 *                   BCM_L2_TRAVERSE_MATCH_VLAN | BCM_L2_TRAVERSE_MATCH_DEST;
 *           l2_addr.vid = l2_mact_table_info1.vid1;
 *           BCM_GPORT_LOCAL_SET(l2_addr.port, l2_mact_table_info1.dest_port_2);
 *           l2_addr.mac[0] = 0;
 *           l2_addr.mac[1] = 0;
 *           l2_addr.mac[2] = 0;
 *           l2_addr.mac[3] = 0;
 *           l2_addr.mac[4] = 0x01;
 *           l2_addr.mac[5] = 0x30;
 *           print l2_traverse(unit, l2_match_traverse, flags, &l2_addr, NULL);
 *  o    L2 match mask traverse.
 *      -    traverse matching all UNICAST entries in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_DEST;
 *           BCM_GPORT_LOCAL_SET(l2_addr.port, 0);
 *           l2_addr_mask.port=0x30000;
 *           print l2_traverse(unit, l2_match_mask_traverse, flags, &l2_addr, &l2_addr_mask);
 *
 *  o    L2 match mask traverse.
 *      -    traverse matching all MULTICAST entries in the l2 table.
 *           flags =  BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_DEST;
 *           BCM_GPORT_MCAST_SET(l2_addr.port, 0);
 *           l2_addr_mask.port=0x30000;
 *           print l2_traverse(unit, l2_match_mask_traverse, flags, &l2_addr, &l2_addr_mask);
 *
 *  o    L2 match mask traverse.
 *      -    traverse matching UNICAST entries with mask in the l2 table.
 *           flags = BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_DEST |
 *                   BCM_L2_TRAVERSE_MATCH_MAC;
 *           BCM_GPORT_LOCAL_SET(l2_addr.port, 0);
 *           l2_addr.mac[0] = 0;
 *           l2_addr.mac[1] = 0;
 *           l2_addr.mac[2] = 0;
 *           l2_addr.mac[3] = 0;
 *           l2_addr.mac[4] = 0x01;
 *           l2_addr.mac[5] = 0x30;
 *           l2_addr_mask.port=0x30000;
 *           l2_addr_mask.mac[0] = 0xff;
 *           l2_addr_mask.mac[1] = 0xff;
 *           l2_addr_mask.mac[2] = 0xff;
 *           l2_addr_mask.mac[3] = 0xff;
 *           l2_addr_mask.mac[4] = 0xff;
 *           l2_addr_mask.mac[5] = 0xfc;
 *           print l2_traverse(unit, l2_match_mask_traverse, flags, &l2_addr, &l2_addr_mask);
 *
 *  o    L2 match mask traverse.
 *      -    traverse matching MULTICAST entries with mask in the l2 table.
 *           flags =  BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_DEST |
 *                    BCM_L2_TRAVERSE_MATCH_MAC;
 *           BCM_GPORT_MCAST_SET(l2_addr.port, 0);
 *           l2_addr.mac[0] = 0;
 *           l2_addr.mac[1] = 0;
 *           l2_addr.mac[2] = 0;
 *           l2_addr.mac[3] = 0;
 *           l2_addr.mac[4] = 0x01;
 *           l2_addr.mac[5] = 0x35;
 *           l2_addr_mask.port=0x30000;
 *           l2_addr_mask.mac[0] = 0xff;
 *           l2_addr_mask.mac[1] = 0xff;
 *           l2_addr_mask.mac[2] = 0xff;
 *           l2_addr_mask.mac[3] = 0xff;
 *           l2_addr_mask.mac[4] = 0xff;
 *           l2_addr_mask.mac[5] = 0xfe;
 *           print l2_traverse(unit, l2_match_mask_traverse, flags, &l2_addr, &l2_addr_mask);
 *
 * Run Script:
 *      BCM> examples/dpp/utility/cint_utils_multicast.c
 *      BCM> examples/dpp/cint_l2_traverse.c
 *      BCM> cint
 *      cint> print l2_mact_run(unit, port1, port2);
 *      cint> print l2_traverse(unit, api_idx, flags, l2_addr, l2_addr_mask);
 */

/* set to one to show informative prints */
int verbose = 1;

/* used for l2_traverse */
int flags;
bcm_l2_addr_t l2_addr;
bcm_l2_addr_t l2_addr_mask;
bcm_l2_addr_t l2_replace_addr;
bcm_l2_addr_t l2_replace_addr_mask;
int count = 0;

/* mac address used as base for MAC addresses generation */
int _mac_address_indx = 0x12f;
uint8 _mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

bcm_mac_t mac1 = {0x1,0x2,0x3,0x1a,0x1b,0x1c};
bcm_mac_t mac2 = {0x2,0x2,0x3,0x1a,0x1b,0x1c};
bcm_mac_t mac3 = {0x3,0x2,0x3,0x1a,0x1b,0x1c};

int vsi1 = 1;
int vsi2 = 2;
int vlan1 = 1;
int vlan2 = 2;
int group1 = 1;
int group2 = 2;

enum bcm_l2_traverse_e {
    l2_traverse,             /* bcm_l2_traverse */
    l2_match_traverse,       /* bcm_l2_matched_traverse */
    l2_match_mask_traverse,  /* bcm_l2_match_masked_traverse */
    l2_traverse_and_abort,
    l2_replace_mask
};

/* structure, fill and call l2_mac_table_init()
 * then 'nof_mac_ddresses' mac adresses will be added to vid1 and vid2,
 * points to address dest_port_1/2
 */
struct l2_mac_table_info_s {
    int aging_time;         /* aging, time, 0 for disable */
    int vid1;               /* vlans mac addresses will be inserted to */
    int vid2;               /* vlans mac addresses will be inserted to */
    int nof_mac_ddresses;   /* number of mac entris to be added */
    int dest_port_1;        /* unicast destination port to forward packets to */
    int dest_port_2;        /* unicast destination port to forward packets to */
    int mc_id;              /* multicast group to send packets to, must include above unicast destination */
    uint8 mac[6];
};

/* structure, fill and call l2_mac_table_init()
 * then 'nof_mac_ddresses' mac adresses will be added to vid1 and vid2,
 * points to address dest_port_1/2
 */
struct l2_mac_table_info2_s {    
    int vsi1;
    int vsi2;
    int vsi3;
    int port1;
    int port2;
    int group0;
    int group1;
    uint8 mac1[6];
    uint8 mac2[6];
};

/* attributes to enable user to control the application*/
l2_mac_table_info_s l2_mact_table_info1;

l2_mac_table_info2_s l2_mact_table_info2;
bcm_l2_addr_t g_l2_addr;

/*
 * l2 mac table init 
 * Add 5 IVL entries (mac1, vsi1, vlan1), (mac1, vsi2, vlan1), (mac1, vsi1, vlan2), (mac2, vsi1, vlan1, group1), (mac2, vsi1, vlan2, group2)
 * Add 3 SVL entries (mac1, vsi1), (mac2, vsi1, group1), (mac2, vsi2, group2)
 */
void l2_ivl_mac_table_init(int unit, int port_1, int port_2, int port_3, int port_4) {

    /** Add IVL entries */
    l2_entry_extended_add(unit, mac1, vsi1, 0, 0, 0, vlan1, 0, 0);
    l2_entry_extended_add(unit, mac1, vsi2, 0, port_1, 0, vlan1, 0, 0);
    l2_entry_extended_add(unit, mac1, vsi1, 0, port_2, 0, vlan2, 0, 0);
    l2_entry_extended_add(unit, mac2, vsi1, 0, port_3, 0, vlan1, group1, 0);
    l2_entry_extended_add(unit, mac2, vsi1, 0, port_4, 0, vlan2, group2, 0);

    /** Add SVL entries */
    l2_entry_extended_add(unit, mac1, vsi1, 0, 0     , 0,      0,     0, 0);
    l2_entry_extended_add(unit, mac2, vsi1, 0, 0     , port_2, 0,     group1, 0);
    l2_entry_extended_add(unit, mac2, vsi2, 0, 0     , port_3, 0,     group2, 0);
}


/*
 * l2 mac table init 
 * Add 3 MAC entries (mac1, vsi1), (mac2, vsi1, group1), (mac2, vsi2, group2)
 */
void l2_mac_table_init_addr(int unit, int port_1, int port_2) {

    /** Add SVL entries */
    l2_entry_extended_add(unit, mac1, vsi1, 0, 0     , 0,      0,     0,      BCM_L2_STATIC);
    l2_entry_extended_add(unit, mac2, vsi1, 0, 0     , port_1, 0,     group1, BCM_L2_STATIC);
    l2_entry_extended_add(unit, mac2, vsi2, 0, 0     , port_2, 0,     group2, 0);
}


/*
 * l2 mac table init for non-blocking flush tests 
 * 1. Add a static entry.
 * 2. Add a static transplantable entry. 
 * 3. Add dynamic entry.
 */
void l2_non_blocking_mac_table_init(int unit, int port_1) {

    /** Add static entry */
    l2_entry_extended_add(unit, mac1, vsi1, 0, 0, 0, 0, 0, 0);
    /** Add static transplantable entry */
    l2_entry_extended_add(unit, mac1, vsi2, 0, port_1, 0, 0, 0, BCM_L2_STATIC | BCM_L2_MOVE_PORT);    
    /** Add a dynamic entry */
    l2_entry_extended_add(unit, mac2, vsi1, 0, 0, 1, 0, 0, BCM_L2_MCAST);    
}

/*
 * l2 mac table init
 */
void l2_mac_table_info_init(int unit, int port1, int port2) {
    bcm_mac_t empty_mac;

    /* two vlans mac addresses will be inserted to */
    l2_mact_table_info1.vid1 = 1;
    l2_mact_table_info1.vid2 = 3;
    /* number of mac entris to be added */
    l2_mact_table_info1.nof_mac_ddresses = 5;
    /* unicast destination port to forward packets to */
    l2_mact_table_info1.dest_port_1 =  port1;
    l2_mact_table_info1.dest_port_2 =  port2;
    /* multicast group to send packets to, the multicast will include the above unicast destination */
    l2_mact_table_info1.mc_id = 4321;
    /* Maximum age time that can be set is 300 seconds in Jericho2 */
    l2_mact_table_info1.aging_time = (is_device_or_above(unit, JERICHO2)) ? 200 : 400; /* 400 seconds */
    bcm_l2_addr_t_init(&l2_addr, empty_mac, 0);
}

int l2_mac_table_info_init2(int unit) {
    bcm_mac_t empty_mac;

    bcm_l2_addr_t_init(&l2_addr, empty_mac, 0);

    return BCM_E_NONE;
}


int l2_entry_extended_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id, int modid, int group, int flags) {
    return l2_entry_add_internal(unit, mac, vlan, dest_type, dest_gport, dest_mc_id, modid, group, flags);
}

int l2_entry_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id){
    return l2_entry_add_internal(unit, mac, vlan, dest_type, dest_gport, dest_mc_id, 0, 0, 0);
}

/*  Setup MAC forwading
 *  dest_type = 0 for GPort
 *              1 for MC Group
 *  flags are used only if !=0. flags are overriding other configurations.
 */
int l2_entry_add_internal(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id, int modid, int group, int flags){
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);
    if(dest_type == 0) {
        /* add static entry */
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = dest_gport;
    } else {
        /* add multicast entry */
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
        l2_addr.l2mc_group = dest_mc_id;
    }

    l2_addr.group = group;
    l2_addr.modid = modid;

    if (flags) 
    {
        l2_addr.flags = flags;
    }
        
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if(verbose){
        printf(" adding %d", rv);
        l2_addr_print_entry(&l2_addr);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, in setup_mac_forwarding, dest_type %d dest_id %d \n", dest_type, dest_gport );
        return rv;
    }
    return rv;
}

/* depeneding on: l2_mact_table_info1
 * add Mac addressed to MAC table:
 * 'nof_mac_ddresses' mac adresses is added to vid1 and vid2,
 * points to address dest_port_1/2 (alternatively)
 */
int
l2_mac_table_init(int unit){
    bcm_mac_t mac;
    int port1,port2;
    int indx;

    for(indx = 0;  indx < l2_mact_table_info1.nof_mac_ddresses; ++indx){
        generat_mac_address(mac);
        if(indx % 2 == 0){
            port1 = l2_mact_table_info1.dest_port_1;
            port2 = l2_mact_table_info1.dest_port_2;
        }
        else
        {
            port2 = l2_mact_table_info1.dest_port_1;
            port1 = l2_mact_table_info1.dest_port_2;
        }
        l2_entry_add(unit, mac, l2_mact_table_info1.vid1, 0, port1, 0);
        l2_entry_add(unit, mac, l2_mact_table_info1.vid2, 0, port2, 0);
    }
    return 0;
}

/* 
 * depeneding on: l2_mact_table_info2  
 * Add two MAC entries with group!=0 field. One dynamic and one static. Use MAC1/VSI1 and MAC2/VSI1. destination=port1.
 * Add two MAC entries with group=0 field. One dynamic and one static. Use MAC1/VSI2 and MAC2/VSI2. destination=port2.
 * Add one static entry with outlif. group=0. MAC2/VSI3. 
 * Add a host entry to the LEM 
 */
int
l2_mac_table_init2(int unit){
    bcm_mac_t mac;
    int port1,port2;
    int indx;

    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    generat_mac_address(l2_mact_table_info2.mac1);
    generat_mac_address(l2_mact_table_info2.mac2);

    /** Add a MAC entry. group1/dynamic/MAC1/VSI1. */
    bcm_l2_addr_t_init(&l2_addr, l2_mact_table_info2.mac1, l2_mact_table_info2.vsi1);

    l2_addr.flags = 0;
    l2_addr.port = l2_mact_table_info2.port1;
    l2_addr.group = l2_mact_table_info2.group1;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if(verbose){
        printf(" adding %d", rv);
        l2_addr_print_entry(&l2_addr);
    }
    if (rv != BCM_E_NONE) {
        printf("Error1, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }

    /** Add a MAC entry. group1/static/MAC2/VSI1. */
    bcm_l2_addr_t_init(&l2_addr, l2_mact_table_info2.mac2, l2_mact_table_info2.vsi1);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = l2_mact_table_info2.port1;
    l2_addr.group = l2_mact_table_info2.group1;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if(verbose){
        printf(" adding %d", rv);
        l2_addr_print_entry(&l2_addr);
    }
    if (rv != BCM_E_NONE) {
        printf("Error2, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }

    /** Add a MAC entry. group0/dynamic/MAC1/VSI2. */
    bcm_l2_addr_t_init(&l2_addr, l2_mact_table_info2.mac1, l2_mact_table_info2.vsi2);

    l2_addr.flags = 0;
    l2_addr.port = l2_mact_table_info2.port2;
    l2_addr.group = l2_mact_table_info2.group0;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if(verbose){
        printf(" adding %d", rv);
        l2_addr_print_entry(&l2_addr);
    }
    if (rv != BCM_E_NONE) {
        printf("Error3, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }

    /** Add a MAC entry. group0/static/MAC2/VSI2. */
    bcm_l2_addr_t_init(&l2_addr, l2_mact_table_info2.mac2, l2_mact_table_info2.vsi2);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = l2_mact_table_info2.port2;
    l2_addr.group = l2_mact_table_info2.group0;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if(verbose){
        printf(" adding %d", rv);
        l2_addr_print_entry(&l2_addr);
    }
    if (rv != BCM_E_NONE) {
        printf("Error4, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }
    
    /** Create an outlif */
    bcm_vlan_port_t vlan_port;
    int vlan_port_id = 17<<26|2<<22|5000;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.vlan_port_id = vlan_port_id;
    vlan_port.match_vlan = l2_mact_table_info2.vsi2;
    vlan_port.match_inner_vlan = 20;
    vlan_port.port = l2_mact_table_info2.port1;
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error5, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }

    /** Add one static entry with outlif. group=0. MAC2/VSI3. */
    bcm_l2_addr_t_init(&l2_addr, l2_mact_table_info2.mac2, l2_mact_table_info2.vsi3);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = vlan_port_id;
    l2_addr.group = l2_mact_table_info2.group0;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if(verbose){
        printf(" adding %d", rv);
        l2_addr_print_entry(&l2_addr);
    }
    if (rv != BCM_E_NONE) {
        printf("Error6, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }

    /** Add a l3 host entry */
    bcm_l3_host_t l3_host;
    bcm_l3_host_t_init(&l3_host);

    l3_host.l3a_ip_addr = 0x0AB80B7B;
    l3_host.l3a_vrf = 10;
    l3_host.l3a_intf = 1;

    rv = bcm_l3_host_add(unit,&l3_host);
    if(verbose){
        printf("Added l3 host entry", rv);
    }
    if (rv != BCM_E_NONE) {
        printf("Error7, in l2_mac_table_init2, rv= %d \n", rv );
        return rv;
    }

    return rv;
}

int
create_multicast(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_multicast_replication_t replications[2];

    rv = bcm_multicast_create(unit, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &l2_mact_table_info1.mc_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    bcm_multicast_replication_t_init(&replications[0]);
    bcm_multicast_replication_t_init(&replications[1]);
    replications[0].port = l2_mact_table_info1.dest_port_1;
    replications[1].port = l2_mact_table_info1.dest_port_2;

    rv = bcm_multicast_add(unit, l2_mact_table_info1.mc_id, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP, 2, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_add\n");
        return rv;
    }

    return rv;
}

/* Application Sequence:
 *  -   Enable aging and set age time to 400 seconds
 *  -   Add number of static entries to MAC table
 *      (some to unicast destination and others to multicast group)
 */
int l2_mact_run(int unit, int port1, int port2){
    int rv = BCM_E_NONE;
    bcm_mac_t mac;

    /* fill mac table with initial info */
    l2_mac_table_info_init(unit, port1, port2);

    /* set aging */
    if (is_device_or_above(unit, JERICHO2)) {
        rv = bcm_l2_age_timer_meta_cycle_set(unit, l2_mact_table_info1.aging_time);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_mact_table_info1 \n");
            return rv;
        }
    } else {
        rv = bcm_l2_age_timer_set(unit,l2_mact_table_info1.aging_time);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_mact_table_info1 \n");
            return rv;
        }
    }

    /* insert some entries */
    rv = l2_mac_table_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_mac_table_init \n");
        return rv;
    }

    /* demonistrate addition of some MC entnries */
    /* open mc */
    if (is_device_or_above(unit, JERICHO2)) {
        rv = create_multicast(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, create_multicast\n");
            return rv;
        }
    } else {
        int gports[2], cuds[2]={0};
        gports[0] = l2_mact_table_info1.dest_port_1;
        gports[1] = l2_mact_table_info1.dest_port_2;
        rv = multicast__open_ingress_mc_group_with_local_ports(unit, l2_mact_table_info1.mc_id, gports, cuds, 2, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
            return rv;
        }
    }

    /* get next mac-address */
    generat_mac_address(mac);

    /* add MAC points to MC address */
    rv = l2_entry_add(unit, mac, l2_mact_table_info1.vid1, 1, 0, l2_mact_table_info1.mc_id);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    generat_mac_address(mac);
    rv = l2_entry_add(unit, mac, l2_mact_table_info1.vid2, 1, 0, l2_mact_table_info1.mc_id);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    return rv;
}

/* delete all MAC entries point to the given destination */
int l2_delete_by_port(int unit,int port ){
    int flags = BCM_L2_DELETE_STATIC;
    int rv;

    rv = bcm_l2_addr_delete_by_port(unit, 0, port,flags);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_delete_by_port \n");
        return rv;
    }
    return 0;
}

/* print MAC table content*/
int l2_print_mact(int unit ){
    int a;
    int rv;
    rv = bcm_l2_traverse(unit,call_back_get_block, &a);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_traverse \n");
        return rv;
    }
    return BCM_E_NONE;
}

/* utilities */
int generat_mac_address(unsigned char* mac){
    int indx;
    int tmp_mac = _mac_address_indx;
    for(indx = 5; indx >= 0; --indx)
    {
        _mac_address[indx] = tmp_mac % 0x100;
        tmp_mac /= 0x100;
    }
    sal_memcpy(mac, _mac_address, 6);
    ++_mac_address_indx;
    return 0;
}

void print_gport_part(int _gport){
    int a = -1, b = 0;
    char* type = "";
    if (a==-1){
        a=BCM_GPORT_LOCAL_GET(_gport);
        type ="local";
    }
    if (a==-1){
        a=BCM_GPORT_MODPORT_MODID_GET(_gport);
        b=BCM_GPORT_MODPORT_PORT_GET(_gport);
        type ="modport";
    }
    if (a==-1){
        a=BCM_GPORT_TRUNK_GET(_gport);
        type ="trunk";
    }
    if (a==-1){
        a=BCM_GPORT_MCAST_GET(_gport);
        type ="mact";
    }
    if (a==-1){
        a=BCM_GPORT_MPLS_PORT_ID_GET(_gport);
        type ="mpls_port";
    }
    if (a==-1){
        a=BCM_GPORT_VLAN_PORT_ID_GET(_gport);
        type ="vlan_port";
    }
    if (a==-1){
        a=BCM_GPORT_SYSTEM_PORT_ID_GET(_gport);
        type ="sys_port";
    }
    if (a==-1){
        a=BCM_GPORT_MIRROR_GET(_gport);
    }
    printf("%s <%d,%d>", type, a, b);
}

/* call back to print mac table entry */
int call_back_get_block(int unit, bcm_l2_addr_t *l2e, int* ud) {
    l2_addr_print_entry(l2e);
    ++*ud;
    return BCM_E_NONE;
}

/* Error returned by the callback should stop the traverse and thaw the aging */
int call_back_get_block_and_abort(int unit, bcm_l2_addr_t *l2e, int* ud) {
    l2_addr_print_entry(l2e);
    ++*ud;
    return BCM_E_FAIL;
}

void _l2_print_mac(bcm_mac_t mac_address){
    unsigned int a,b,c,d,e,f;
    a = 0xff & mac_address[0];
    b = 0xff & mac_address[1];
    c = 0xff & mac_address[2];
    d = 0xff & mac_address[3];
    e = 0xff & mac_address[4];
    f = 0xff & mac_address[5];
    printf("%02x:%02x:%02x:%02x:%02x:%02x |",
           a,b,c,
           d,e,f);
}

/* print l2add entry */
void l2_addr_print_entry(bcm_l2_addr_t *l2e) {
    _l2_print_mac(l2e->mac);

    if(!(l2e->flags & BCM_L2_MCAST)){
        printf("VID=0x%x| PORT=0x%08x|",
        l2e->vid, l2e->port);
        print_gport_part(l2e->port);
    }
    else{
        printf("MC: 0x%08x|",l2e->l2mc_group);
    }
    printf("|sttc:%d|", (l2e->flags & BCM_L2_STATIC)!=0 );
    printf("\n\r");
}

/*
 * traverse all entries/matching entries/matching with flexible mask entries in the l2 table
 * and execute user callback function for each matching entry
 */
int l2_traverse(int unit, int api_idx, int flags, bcm_l2_addr_t *l2_addr, bcm_l2_addr_t *l2_addr_mask)
{
    int rv = BCM_E_NONE;
    count = 0;

    switch(api_idx) {
        case l2_traverse:
            rv = bcm_l2_traverse(unit, call_back_get_block, &count);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_l2_traverse returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            printf("count:%d\n", count);
            break;
        case l2_match_traverse:
            rv = bcm_l2_matched_traverse(unit, flags, l2_addr, call_back_get_block, &count);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_l2_matched_traverse returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            printf("count:%d\n", count);
            break;
        case l2_match_mask_traverse:
            rv = bcm_l2_match_masked_traverse(unit, flags, l2_addr, l2_addr_mask, call_back_get_block, &count);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_l2_match_masked_traverse returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            printf("count:%d\n", count);
            break;
        case l2_traverse_and_abort:
            /** Jericho2 doesn't return an error in bcm_l2_traverse when the user's callback returns an error*/
            /** Jericho does return an error in bcm_l2_traverse when the user's callback returns an error*/
            rv = bcm_l2_traverse(unit, call_back_get_block_and_abort, &count);
            if (rv != BCM_E_NONE && is_device_or_above(unit, JERICHO2))             
            {
                printf("Error, bcm_l2_traverse with call_back_get_block_and_abort returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            else
            {
                rv = BCM_E_NONE;
            }

            break;
    
        default:
            printf("ERR: l2_traverse INVALID PARAMETER api_idx %d\n", api_idx);
            return BCM_E_PARAM;
    }

    return rv;
}

int l2_replace(int unit, int api_idx, int flags, bcm_l2_addr_t *l2_addr, bcm_l2_addr_t *l2_addr_mask,
               bcm_l2_addr_t *l2_replace_addr, bcm_l2_addr_t *l2_replace_addr_mask)
{
    int rv = BCM_E_NONE;
    count = 0;

    switch(api_idx) {
        
    case l2_replace_mask:
        
            rv = bcm_l2_replace_match(unit, flags, l2_addr, l2_addr_mask, l2_replace_addr, l2_replace_addr_mask);
            if (rv != BCM_E_FAIL)
            {
                printf("Error, bcm_l2_replace_match failed. Returned %s\n", bcm_errmsg(rv));
                return rv;
            }
            else
            {
                rv = BCM_E_NONE;
            }
            break;
        default:
            printf("ERR: l2_replace INVALID PARAMETER api_idx %d\n", api_idx);
            return BCM_E_PARAM;
    }

    return rv;
}


int l2_entries_count(int unit)
{
    int rv = BCM_E_NONE;
    count = 0;

    rv = bcm_l2_traverse(unit, call_back_get_block, &count);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_traverse returned %s failed\n", bcm_errmsg(rv));
        return rv;
    }

    return count;
}


