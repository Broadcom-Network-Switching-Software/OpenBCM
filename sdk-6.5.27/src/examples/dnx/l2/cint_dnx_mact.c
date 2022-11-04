/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/*
 * L2 cint, static MACT entry forwarding.
 *
 * test 1: example to add mac entries to default vlan.
 * run:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/l2/cint_dnx_mact.c
  cint
  example_dflt_vlan_entries(<unit>, <dest_port> , <nof_entries>);
 * Test Scenario - end
 *
 * run packet:
 *      ethernet header with DA {0x00, 0x00, 0x00, 0x00, 0x00, 0xab : 0xab+ <nof_entries>} and any SA
 *      vlan tag with vlan tag id 1
 * verify packet is recieved at <dest_port>
 *
 * test 2: example to open vlan and add entries to MACT.
 *
 * run:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/l2/cint_dnx_mact.c
  cint
  example_new_vlan_entries(0,0x70f,201,201,3);
 * Test Scenario - end
 *
 * run packet 1:
 *      ethernet header with DA {0x00, 0x00, 0x00, 0x00, 0x00, 0xab : 0xab+ <nof_entries>} and any SA
 *      vlan tag with vlan tag id <vlan>
 *      from <in_port>
 * verify packet is recieved at <dest_port>
 *
 * Note: You can also use the individual functions that are included in the example file.
 *
 */

int print_level = 3;
/*
 * global variables
 */

/* reference mac to update */
bcm_mac_t g_mact_mac_address  = {0x00, 0x00, 0x00, 0x00, 0x00, 0xab};

/*
* Init g_mact_mac_address
*/
void l2_mac_set_mac_address(bcm_mac_t mac_addres){
    sal_memcpy(g_mact_mac_address, mac_addres, 6);
}

/*
* Print MAC address
*/
void l2_mac_addr_print(bcm_mac_t mac_addr)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
    mac_addr[5],mac_addr[4],mac_addr[3],
    mac_addr[2],mac_addr[1],mac_addr[0]);
}

/*
* Increment MAC address
*/
void mac_inc(bcm_mac_t *mac_address_p){
    int i;
    for(i = 6; i > 0; --i) {
        if(mac_address_p[i-1] < 255) {
            ++mac_address_p[i-1];
            break;
        }
        mac_address_p[i-1] = 0;
    }
}

/*
* Limit entries for given VLAN or globally.
* - vlan : vlan to limit, set to -1 for global limit.
* - limit: number of entries. -1 for unlimit.
*
*/
int l2_mact_limit_set(int unit, int vlan, int learn_limit){

    bcm_l2_learn_limit_t limit;

    bcm_l2_learn_limit_t_init(&limit);

    if(vlan == -1) { /* global */
        limit.flags = BCM_L2_LEARN_LIMIT_SYSTEM;
    }
    else{ /* according to vlan */
        limit.flags = BCM_L2_LEARN_LIMIT_VLAN;
        limit.vlan = vlan;
    }

    /* the limit */
    limit.limit = learn_limit;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_limit_set(unit, &limit), "");

    return BCM_E_NONE;
}

/*
* Get limit entries for given VLAN or globally.
* - vlan : vlan to limit, set to -1 for global limit.
* - limit: number of entries. -1 for unlimit.
*
*/
int l2_mact_limit_get(int unit, int vlan, int *limit){
    bcm_l2_learn_limit_t limit;

    bcm_l2_learn_limit_t_init(&limit);

    if(vlan == -1) {
        limit.flags = BCM_L2_LEARN_LIMIT_SYSTEM;
    }
    else{ /* according to vlan */
        limit.flags = BCM_L2_LEARN_LIMIT_VLAN;
        limit.vlan = vlan;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_limit_get(unit, &limit), "");

    /* limit */
    *limit = limit.limit;

    return BCM_E_NONE;
}

/*
 *  Limit entries for given LIF
 *  - gport : gport of the LIF to limit
 *  - learn_limit: number of entries. -1 for unlimited.
 *
 */
int l2_mact_limit_set_per_gport(int unit, int gport, int learn_limit){
    bcm_l2_learn_limit_t limit;

    bcm_l2_learn_limit_t_init(&limit);

    /* Format the limit struct */
    limit.flags = BCM_L2_LEARN_LIMIT_PORT;
    limit.port = gport;
    limit.limit = learn_limit;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_limit_set(unit, &limit), "");

    return BCM_E_NONE;
}

/*
 *  Get limit entries for given LIF
 *  - gport : gport of the LIF to limit
 *  - learn_limit: number of entries. -1 for unlimited.
 *
 */
int l2_mact_limit_get_per_gport(int unit, int gport, int *learn_limit) {
    bcm_l2_learn_limit_t limit;

    bcm_l2_learn_limit_t_init(&limit);

    /* Format the limit struct */
    limit.flags = BCM_L2_LEARN_LIMIT_PORT;
    limit.port = gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_limit_get(unit, &limit), "");

    /* Retrieve the limit value */
    *learn_limit = limit.limit;

    return BCM_E_NONE;
}

/*
 *  Create a LIF for MACT limit per LIF testing. The supplied port
 *  is associated with a VLAN domain, a LIF is created, associated to
 *  a vswitch and finaly a MACT Limit is set for the LIF. On first LIF
 *  creation, the vswitch should be created.
 *
 *  - port_id: The physical port to use
 *  - vlan : The same VLAN, both for incoming and outgoing
 *  - learn_limit: MAC limit for the created LIF
 *  - lif_id: ID of the created LIF, if not 0
 *  - is_vswitch_create: create the VSwitch or not
 *
 */
int l2_mact_vlan_port_create_with_limit(
    int unit,
    bcm_port_t port_id,
    bcm_vlan_t vlan, /* incoming outer vlan and also outgoing vlan */
    int learn_limit,
    int lif_id,
    int is_vswitch_create)
{
    char error_msg[200]={0,};
    bcm_vlan_port_t vp;
    bcm_vlan_t vsi = 2;
    int with_id;
    bcm_multicast_t multi_group;
    char *proc_name;

    proc_name = "l2_mact_vlan_port_create_with_limit";

    /* Attach the port to a VLAN domain with the same number as the port */
    snprintf(error_msg, sizeof(error_msg), "(%d):", port_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port_id, bcmPortClassId, port_id), error_msg);

    /* Perform VLAN-Port create */
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = vlan;
    vp.egress_vlan = 0; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = 0;
    vp.flags = 0;
    if (lif_id != 0) {
        vp.flags = BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vp.vlan_port_id, lif_id);
        vp.vlan_port_id |= 0x800000;
    }

    snprintf(error_msg, sizeof(error_msg), "(%ld-%ld):", vlan, port_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vp), error_msg);
    printf("%s(): Allocated gport id - %ld\n", proc_name, vp.vlan_port_id);

    /* Perform vswitch create for the first gport on this VSI */
    if (is_vswitch_create) {
        with_id = 1;
        snprintf(error_msg, sizeof(error_msg), "(%ld):", vsi);
        BCM_IF_ERROR_RETURN_MSG(vswitch_create(unit, &vsi, with_id), error_msg);
        printf("%s(): Performed bcm_vswitch_create_with_id for vsi %ld\n", proc_name, vsi);
    }

    /* Add the LIF to the vswitch */
    snprintf(error_msg, sizeof(error_msg), "(%ld-%ld):", vsi, vp.vlan_port_id);
    BCM_IF_ERROR_RETURN_MSG(vswitch_add_port(unit, vsi, port_id, vp.vlan_port_id), error_msg);

    /* Set the limit for the LIF */
    snprintf(error_msg, sizeof(error_msg), "(%ld-%ld):", vp.vlan_port_id, learn_limit);
    BCM_IF_ERROR_RETURN_MSG(l2_mact_limit_set_per_gport(unit, vp.vlan_port_id, learn_limit), error_msg);

    return BCM_E_NONE;
}

/*
 * Add entry to mac table <vlan,mac> --> port
 */
int
l2_addr_add(int unit, bcm_mac_t mac, uint16 vid, bcm_gport_t port) {
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);

    l2addr.port = port;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2addr), "");

    if(print_level > 2) {
        printf("l2 entry: fid = %d mac =", vid);
        l2_mac_addr_print(mac);
        printf("  dest-port 0x%08x \n", port);
    }

    return BCM_E_NONE;
}

/*
 * Get entry from mac table and print it
 */
int
l2_addr_get(int unit, bcm_mac_t mac, uint16 vid) {

    bcm_l2_addr_t l2addr;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_get(unit, mac, vid , &l2addr), "");

    print l2addr;

    return BCM_E_NONE;
}

/*
 * example to add mac entries for default vlan
 * - dest_port destination port.
 * - nof_entries: number of entries to add to MACT
 * - add entries to VLAN 1 mac-address is inc over mac_address
 */
int example_dflt_vlan_entries(int unit, int dest_port, int nof_entries) {
    int idx;
    char error_msg[200]={0,};

    for(idx = 0; idx < nof_entries; ++idx) {
        snprintf(error_msg, sizeof(error_msg), "(%d)", idx);
        BCM_IF_ERROR_RETURN_MSG(l2_addr_add(unit, g_mact_mac_address, 1, dest_port), error_msg);

        mac_inc(g_mact_mac_address);
    }

    return BCM_E_NONE;
}

/*
 * example to open vlan and add entries to MACT:
 * - vlan: vlan to open/use
 * - in-port: to set TPID for
 * - dest_port destination port.
 * - nof_entries: number of entries to add to MACT
 */
int
 example_new_vlan_entries(int unit, int vlan, int in_port, int dest_port, int nof_entries) {
    int idx;
    uint16 tpid = 0x8100;
    int nof_ports=1;
    int port_id = 0;
    int cur_dest_port;
    bcm_vlan_port_t pvid_map;
    bcm_gport_t ports[10]= {0x0};
    uint8 tagged[10]= {0x0};

    /* set in-port as member in vlan */
    ports[port_id] = in_port;
    tagged[port_id] = in_port % 2;

    cur_dest_port = dest_port;
    /* open vlan with ports */
    for(port_id = 1; port_id < nof_ports; ++port_id) {
        ports[port_id] = dest_port;
        tagged[port_id] = dest_port % 2;
        ++dest_port;
    }

    BCM_IF_ERROR_RETURN_MSG(vlan__open_vlan_per_ports(unit,vlan,ports,tagged,nof_ports), "");

    /* set in-port PVID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, in_port, vlan), "");

    /* Add entries to MACT */
    for(idx = 0; idx < nof_entries; ++idx) {
        BCM_IF_ERROR_RETURN_MSG(l2_addr_add(unit, g_mact_mac_address, vlan, dest_port), "");
        mac_inc(g_mact_mac_address);
    }

    printf("add %d entries to MACT \n", nof_entries);

    return BCM_E_NONE;
}


