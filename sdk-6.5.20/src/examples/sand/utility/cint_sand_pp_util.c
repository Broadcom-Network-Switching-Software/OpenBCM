/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PP VLAN Implementation
 */

/* PP Util
 how to use:
  - l2_mac_addr_print(bcm_mac_t mac_addr)                                                               - prints mac addr
  - l2_gen_mac_addr(int id, int mc)                                                                     - generates mac addr (if mc is asserted, asserts bit 40)
  - l2_addr_add (unit, mac_address, vid, port, mc_id)                                                   - add entry to mac table <vlan,mac> --> destination 
  - l2_addr_get(int unit, bcm_mac_t mac, uint16 vid)                                                    - get entry from mac table <vlan,mac> --> destination
  - open_vlan(int unit, uint16 vid, bcm_gport_t *ports, uint8 *untagged, int nof_ports)                 - open new vlan and adds all ports in ports arr as members
  - egr_vlan_edit(int unit, int curr_port, int next_port, int curr_vid, int next_vid)                   - Egress Vlan editing, replace old vid with new vid on port  
  - set_snoop_destination(int unit, int port, int snoop_cmd, int vid, int fwd_sys_port, int fwd_mc_id)  - Set FWD + Snoop destination based on incomig DA + Vlan (DA + Vlan --> (snoop,fwd) destination)
                                                                                                          If fwd_sys_port = 0 & fwd_mc_id = 0 will generate fwd destination = NV (0x3ffff)
*/

void l2_mac_addr_print(
    bcm_mac_t mac_addr)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
    mac_addr[0],mac_addr[1],mac_addr[2],
    mac_addr[3],mac_addr[4],mac_addr[5]);
}

bcm_mac_t l2_gen_mac_addr(
    int id,
    int mc)
{
    bcm_mac_t mac_addr;

    mac_addr[0] = (mc == 1) ? 0x1 : 0x0;
    mac_addr[1] = 0x0;
    mac_addr[2] = get_field(id, 24, 31);
    mac_addr[3] = get_field(id, 16, 23);
    mac_addr[4] = get_field(id, 8, 15);
    mac_addr[5] = get_field(id, 0, 7);

    return mac_addr;
}

/*
 * add entry to mac table <vlan,mac> --> port
 * <VID, MAC> --> port
 */

int l2_addr_add(
    int unit,
    bcm_mac_t mac,
    uint16 vid,
    bcm_gport_t port,
    int mc_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.flags = BCM_L2_STATIC;

    if (mc_id != 0)
    {
        l2addr.l2mc_group = mc_id;
        l2addr.flags |= BCM_L2_MCAST;
    }
    else
    {
        l2addr.port = port;
    }
    l2addr.vid = vid;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    printf("l2 entry: fid = %d mac =", vid);
    l2_mac_addr_print(mac);
    if (mc_id == 0)
    {
        printf("  dest-port 0x%08x \n", port);
    }
    else
    {
        printf("  mc_id 0x%08x \n", mc_id);
    }

    return rv;
}

/*
 * get entry from mac table <vlan,mac> --> port
 */
int l2_addr_get(
    int unit,
    bcm_mac_t mac,
    uint16 vid)
{

    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    rv = bcm_l2_addr_get(unit, mac, vid , &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

int open_vlan(
    int unit,
    uint16 vid,
    bcm_gport_t *ports,
    uint8 *untagged,
    int nof_ports)
{

    bcm_error_t rv;
    int port_id;

    /* open vlan */
    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE)
    {
        if(rv == BCM_E_EXISTS)
        {
          rv = BCM_E_NONE;
        }
        else
        {
            printf("Error, in bcm_vlan_create2, vlan=%d, \n", vid);
            return rv;
        }
    }

    printf("Open vlan %d \n", vid);

    /* add ports to vlan */
    for (port_id = 0; port_id < nof_ports; ++port_id)
    {
        rv = bcm_vlan_gport_add(unit, vid, ports[port_id], untagged[port_id]);
        if (rv != BCM_E_NONE)
        {
            printf("fail add port(0x%08x) to vlan(%d)\n", ports[port_id], vid);
            return rv;
        }
        printf(" - port %d: (0x%08x) untagged:%d\n", port_id,ports[port_id], untagged[port_id]);
    }
    return rv;
}

/* Egress Vlan editing, replace old vid with new vid on port */
int egr_vlan_edit(
    int unit,
    int curr_port,
    int next_port,
    int curr_vid,
    int next_vid)
{
    int                       rv = BCM_E_NONE;
    int                       is_jr2;
    bcm_vlan_action_set_t     action;
    uint32                    vlan_domain;
    int                       C_TPID      = 0x8100;
    bcm_gport_t               ports[1]    = {curr_port};  /* list of ports to use in vlan creation    */
    uint8                     untagged[1] = {0};          /* port untagged status */

    /* open vlan with ports */
    rv = open_vlan(unit, curr_vid, ports, untagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, open_vlan\n");
        return rv;
    }
    ports[0] = next_port;
    rv = open_vlan(unit, next_vid, ports   , untagged , 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, open_vlan\n");
        return rv;
    }
    rv = is_device_jericho2(unit, &is_jr2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2 (unit %d)\n", unit);
    }
    /* EVE - Add CTAG */
    if (!is_jr2)
    {
        bcm_vlan_action_set_t_init(&action);
        action.ot_outer       = bcmVlanActionReplace;
        action.new_outer_vlan = next_vid;
        action.outer_tpid     = C_TPID;
        rv                    = bcm_port_class_get(unit, next_port, bcmPortClassId, &vlan_domain);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_class_get %d $rv %s \n", next_port,  bcm_errmsg(rv));
        }

        rv                    = bcm_vlan_translate_egress_action_add(unit, vlan_domain, curr_vid, BCM_VLAN_NONE, &action);  
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_egress_action_add %d $rv %s \n", next_port,  bcm_errmsg(rv));
        }
    }
    else
    {
        int vlan_edit_profile=2;
        int incoming_tag_structure=4;
        int eve_action_id=3;
        int vlan_port_id;

        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags=BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.port = next_port;
        vlan_port.vsi = curr_vid;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create (unit %d)\n", unit);
        }
        vlan_port_id=vlan_port.vlan_port_id;

        bcm_vlan_port_translation_t vlan_port_translation;
        bcm_vlan_port_translation_t_init(&vlan_port_translation);
        vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS;
        vlan_port_translation.gport = vlan_port_id;
        vlan_port_translation.new_outer_vlan = next_vid;
        vlan_port_translation.vlan_edit_class_id=vlan_edit_profile;
        rv = bcm_vlan_port_translation_set(unit,&vlan_port_translation);
        
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_translation_set (unit %d)\n", unit);
        }

        bcm_port_tpid_class_t tpid_class;
        bcm_port_tpid_class_t_init(&tpid_class);
        tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
        tpid_class.port = next_port;
        tpid_class.tpid1 = C_TPID;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
        tpid_class.tag_format_class_id=incoming_tag_structure;
        rv = bcm_port_tpid_class_set(unit, &tpid_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_tpid_class_set (unit %d)\n", unit);
        }

        bcm_vlan_translate_action_class_t vlan_translate_action_class;
        bcm_vlan_translate_action_class_t_init(&vlan_translate_action_class);
        vlan_translate_action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        vlan_translate_action_class.vlan_edit_class_id = vlan_edit_profile;
        vlan_translate_action_class.tag_format_class_id = incoming_tag_structure;
        vlan_translate_action_class.vlan_translation_action_id = eve_action_id;
        rv = bcm_vlan_translate_action_class_set(unit,&vlan_translate_action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set (unit %d)\n", unit);
        }

        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &eve_action_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_create (unit %d)\n", unit);
        }
         
        bcm_vlan_action_set_t eve_action;
        bcm_vlan_action_set_t_init(&eve_action);
        eve_action.dt_outer = bcmVlanActionReplace;
        eve_action.dt_inner = bcmVlanActionNone;
        eve_action.outer_tpid = C_TPID;
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, eve_action_id, &eve_action);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_set (unit %d)\n", unit);
        }
    }
    if (rv == BCM_E_NONE)
    {
        printf("\n *** egr_vlan_edit - DONE *** \n");
    }
    return rv;
}

int set_snoop_destination(
    int unit,
    int port,
    int snoop_cmd,
    int vid,
    int fwd_sys_port,
    int fwd_mc_id)
{
    int                   rv = BCM_E_NONE;
    int                   flags = 0;
    int                   trap_id;
    bcm_rx_trap_config_t  trap_config;
    bcm_gport_t           trap_gport;
    int                   ports[1]    = {port};
    uint8                 untagged[1] = {0};
    bcm_mac_t             mac_addr;

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags         = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.trap_strength = 0; /* for user define set as part of destination */
    trap_config.snoop_cmnd    = snoop_cmd;
    /* Generaing FWD MAC address*/
    if (fwd_mc_id == 0)
    {
        if (fwd_sys_port > 0)
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(trap_config.dest_port, fwd_sys_port);
        }
        else
        {
            trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
        }
    }
    else
    {
        BCM_GPORT_MCAST_SET(trap_config.dest_port, fwd_mc_id);
    }
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in trap set \n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, 7, 3);

    rv = open_vlan(unit, vid, ports, untagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, open_vlan\n");
        return rv;
    }
    /* Adding new gport to MACT*/
    mac_addr = (fwd_mc_id > 0) ? mac_addr = l2_gen_mac_addr(fwd_mc_id, 1) : l2_gen_mac_addr(fwd_sys_port, 0);
    rv = l2_addr_add (unit, mac_addr, vid, trap_gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l2_addr_add (%0d, %0d)\n",port, system_port);
        return rv;
    }
    return rv;
}

/* Set VLAN domain */
int port__vlan_domain__set(int unit, bcm_port_t port, int vlan_domain){

    return bcm_port_class_set(unit, port, bcmPortClassId, vlan_domain);
}

/* Classify a Ports combination of TPIDs to a specific tag format.
 * Applicable only in AVT mode. 
 *  
 * INPUT: 
 *   port: Physical port or a PWE gport
 *   tag_format: Tag format.
 *   outer_tpid: Outer TPID value
 *   inner_tpid: Inner TPID value
 */
int port__tag_classification__set(int unit,
                                  bcm_port_t port,
                                  bcm_port_tag_format_class_t tag_format,
                                  uint32 outer_tpid,
                                  uint32 inner_tpid)
{
    int rv;
    bcm_port_tpid_class_t tpid_class;

    bcm_port_tpid_class_t_init(&tpid_class);

    tpid_class.flags = 0;   /* Both for Ingress and Egress */
    tpid_class.port = port;
    tpid_class.tag_format_class_id = tag_format;
    tpid_class.tpid1 = outer_tpid;
    tpid_class.tpid2 = inner_tpid;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port - %d, rv - %d\n", port, rv);
        return rv;
    }

    return BCM_E_NONE;
}
