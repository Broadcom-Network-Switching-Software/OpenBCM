/*~~~~~~~~~~~~~~~~~~~~~~~~~~Multipoint Metro Service~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *File: cint_vswitch_metro_mp.c
 * Purpose: Example of Multipoint Metro Service, useable on one ARADs.
 *
 * In this service, you can attach more than two logical ports to the VSI.
 * Packets are forwarded/flooded, based on the lookup in MAC table.
 *
 * Open multipoint service and attach logical ports to the service.
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \                         o                      |
 *  |                    \  \<4,8>        <10,20>/  /                       |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |             <40,80>\  \  |    \  /     |\/ /  /                       |
 *  |                     \  \ |     \/      |  /  /<30,60>                 |
 *  |                       p3\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1                           |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2  -----------                                |
 *  |              <5,2>/  /  /                                             |
 *  |                  /  /  /<3,6>                                         |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  |            +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+             |
 *  |            | Figure 12: Attach Logical Ports to Service |             |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  
 * Calling sequence:
 *  1.  For port 1, set outer TPID to 0x8100 and inner TPID to 0x9100 (using cint_port_tpid.c).
 *  2.  Create the Multipoint VSI (refer to vswitch_create).
 *      a.  Create new VSI.
 *          1.   Calls bcm_vswitch_create()
 *          2.   Returns handle to opened VSI
 *      b.  Open multicast group to be used for flooding (refer to open_ingress_mc_group)
 *          1.   Calls bcm_multicast_create()
 *          2.   The multicast is defined at ingress for L2 usage
 *          3.   For this purpose call bcm_multicast_create with following flags: 
                BCM_MULTICAST_INGRESS_GROUP | and BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_L2
 *      c.  Attach the multicast group to the created VSI
 *          1.   Note that uc/mc/bc group have the same value as VSI or uc 
 *          2.   Another option is to set uc_group = VSI, and mc = uc + 4k, and bc = mc + 4k
 *          3.   Calls bcm_vlan_control_vlan_set()
 *  3.  Add VLAN ports to the VSI.
 *      a.  vswitch_metro_add_port_1/2/3 creates VLAN port gport and returns gport ID.
 *          1.  Fills gport attributes (match and egress VLANs, etc.).
 *          2.   Calls bcm_vlan_port_create().
 *      b.  Then the returned gport ID is added to the VSI (vswitch_add_port).
 *          1.   Calls bcm_vswitch_port_add() to add the gport to VSI.
 *          2.   Calls multicast__gport_encap_add() to update the multicast group with the new gport. 
 * 4.  Delete VSI
 *      a.  Find gport in VSI by returned gport ID.
 *          ?    bcm_vlan_port_find()
 *      b.  Remove gport from VSI per gport ID.
 *          ?    bcm_vswitch_port_delete()
 *      c.  Destroy VSI
 *          ?    bcm_vswitch_destroy()
 *
 * Traffic:
 *  Flooding (sending packet with unknown DA):
 *      1.   Send Ethernet packet with:
 *          a.   Any SA
 *          b.   Unknown DA
 *          c.   VLAN tag1: VLAN tag type 0x8100, VID =10
 *          d.   VLAN tag2: VLAN tag type 0x9100, VID =20 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *   |                                                                       |
 *   |                      tag1                 tag2                        |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||   Data   |  |
 *   |  |    |    || 0x8100|      | 10  || 0x9100|      | 20  ||          |  |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                                                                       |
 *   |                   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                       |
 *   |                   | Figure 13: Flooded Packet |                       | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *      2.   Packet is flooded (as DA is not known).
 *          a.   The packet is received: 
 *                 In P2: port 1 with VLAN tag1: VID = 3, VLAN tag2: VLAN ID = 6.
 *                 In P3: port 1 with VLAN tag1 VID = 4, VLAN tag2: VLAN ID = 8.
 *          Note the packet is not received on P1 
 *          (as packet was injected into this port, that is, hair-pin filtering).
 *      3.   SA of the packet is learned.
 *          a.   Call l2_print_mact(unit) to print MAC table content.
 *  Sending to known DA:
 *      1.   Send Ethernet packet (from P2).
 *          a.   Any SA. 
 *          b.   DA equal to SA from previous packet.
 *          c.   VLAN tag1: VLAN tag type 0x8100, VID =1.
 *          d.   VLAN tag2: VLAN tag type 0x9100, VID =2. 
 *      2.   Packet is forwarded (as DA is known) and received in P1: 
 *          port 1 with VLAN tag1: VID =10, VLAN tag2: VLAN ID = 20. 
 *      3.   The SA of the packet is learned.
 *          a.   Call l2_print_mact(unit) to print MAC table content.
 *
 * Remarks:
 *      1.   You can statically add an entry to the MACT pointing to a 
 *          gport using vswitch_add_l2_addr_to_gport(unit, gport_id). 
 *      2.   Set verbose = 1/0 to show/hide informative prints.
 *      3.   Set single_vlan_tag = 1/0 to add one/two tags.
 * 
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/utility/cint_utils_vlan.c
 *      BCM> cint examples/dpp/utility/cint_utils_multicast.c
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_l2_mact.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint
 *      cint> vswitch_metro_mp_run_with_defaults(unit, port_1, port_2, port_3);
 * 
 * Note: This example is also available for a two-unit system. 
 *       More details can be found in cint_vswitch_metro_mp_multi_device.c
 * 
 * The description for this cint's functionality can be found in cint_vswitch_metro_mp.c. Below are only the diffrences relevant for this multi device version.
 * 
 *      * Instead of accepting only unit, the _run functions accept an array of units as first argument, and nof_units as second argument.
 *      * Instead of accepting ports, the cint accepts system ports. To create system ports, call port_to_system_port with the unit number, and its local port.
 *      * Please note that right now, all the delete and remove functions are not yet implemented to support multi device functionality.
 * 
 * To Activate the cint on three devices, for example, Run:
 *      BCM> cint examples/sand/utility/cint_sand_utils_vlan.c
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_l2_mact.c
 *      BCM> cint examples/dpp/cint_multi_device_utils.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/sand/cint_vswitch_metro_mp.c
 *      BCM> cint
 *      cint> int units[3] = {unit1, unit2, unit3};
 *      cint> int sysport1, sysport2, sysport3;
 *      cint> print port_to_system_port(unit1, port_1, &sysport1);
 *      cint> print port_to_system_port(unit2, port_2, &sysport2);
 *      cint> print port_to_system_port(unit3, port_3, &sysport3);
 *      cint> vswitch_metro_mp_run_with_defaults_multi_device(units, 3, 0, sysport_1, sysport_2, sysport_3);
 */
 

/* set to one for informative prints */
int verbose = 1;
/* set to configure single vlan tag */
int single_vlan_tag = 0;

struct vswitch_metro_mp_s {
    bcm_port_t sysports[3];
    int p_vlans[12];  /* vlans for ports ndx = (port - 1) * 4 + i; */
    bcm_vlan_t vsi;
    uint32 flush_group_id[3];
    bcm_failover_t ingress_failover_id[3];
    bcm_failover_t egress_failover_id[3];
    int multi_bidir; /* Configure the multi-dev bidir with same configuration */
};


vswitch_metro_mp_s vswitch_metro_mp_info;

/**
 * Convert system port to local port
*/
int
multi_dev_system_port_to_local(int unit, int sysport, int *local_port){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid;
    int gport;
    int mod_port_id;


    rv = bcm_stk_sysport_gport_get(unit, sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's gport is: 0x%x\n", gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    mod_port_id = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid(core) is: 0x%x\n", gport_modid);
    printf("Sysport's local port is: 0x%x\n", mod_port_id);

    BCM_GPORT_LOCAL_SET(*local_port,mod_port_id);


    return rv;
}


void
vswitch_metro_mp_ring_protection_info_init(   int port_num,
                                       uint32 flush_group_id,
                                       bcm_failover_t ingress_failover_id,
                                       bcm_failover_t egress_failover_id){
    vswitch_metro_mp_info.flush_group_id[port_num] = flush_group_id;
    vswitch_metro_mp_info.ingress_failover_id[port_num] = ingress_failover_id;
    vswitch_metro_mp_info.egress_failover_id[port_num] = egress_failover_id;
}

void 
vswitch_metro_mp_info_init(int sysport_1, int sysport_2, int sysport_3){

    vswitch_metro_mp_info.flush_group_id[0] = 0;
    vswitch_metro_mp_info.ingress_failover_id[0] = 0;
    vswitch_metro_mp_info.egress_failover_id[0] = 0;
    vswitch_metro_mp_info.flush_group_id[1] = 0;
    vswitch_metro_mp_info.ingress_failover_id[1] = 0;
    vswitch_metro_mp_info.egress_failover_id[1] = 0;
    vswitch_metro_mp_info.flush_group_id[2] = 0;
    vswitch_metro_mp_info.ingress_failover_id[2] = 0;
    vswitch_metro_mp_info.egress_failover_id[2] = 0;

    vswitch_metro_mp_info.sysports[0] = sysport_1;
    vswitch_metro_mp_info.sysports[1] = sysport_2;
    vswitch_metro_mp_info.sysports[2] = sysport_3;

    vswitch_metro_mp_info.p_vlans[0] = 10;
    vswitch_metro_mp_info.p_vlans[1] = 20;
    vswitch_metro_mp_info.p_vlans[2] = 30;
    vswitch_metro_mp_info.p_vlans[3] = 60;
    vswitch_metro_mp_info.p_vlans[4] = 5;
    vswitch_metro_mp_info.p_vlans[5] = 2;
    vswitch_metro_mp_info.p_vlans[6] = 3;
    vswitch_metro_mp_info.p_vlans[7] = 6;
    vswitch_metro_mp_info.p_vlans[8] = 4;
    vswitch_metro_mp_info.p_vlans[9] = 8;
    vswitch_metro_mp_info.p_vlans[10] = 40;
    vswitch_metro_mp_info.p_vlans[11] = 80;

    vswitch_metro_mp_info.vsi = 0;
}

/*
 * create vlan-ports
 *   vlan-port: is Logical interface identified by (port-vlan-vlan). 
 */
int
vswitch_metro_add_port(int unit, int port_num,  bcm_gport_t *port_id, int flags){
    int rv;
    int base = port_num * 4;
    bcm_vlan_port_t vp1;
    bcm_info_t info;
    bcm_vlan_port_t_init(&vp1);

    /*
     * ring protection parameters
     */
    vp1.group = vswitch_metro_mp_info.flush_group_id[port_num];
    vp1.ingress_failover_id = vswitch_metro_mp_info.ingress_failover_id[port_num];
    vp1.failover_port_id = 0;
    vp1.egress_failover_id = vswitch_metro_mp_info.egress_failover_id[port_num];
    vp1.egress_failover_port_id = 0;

    /* the match criteria is port:1, out-vlan:10,  in-vlan:20 */
    vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_VLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp1.port = vswitch_metro_mp_info.sysports[port_num];
    vp1.match_vlan = vswitch_metro_mp_info.p_vlans[base + 0];
    vp1.match_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 1];

    /*
     * packet forwarded to this port will be set with out-vlan:100,  in-vlan:200
     * In JR2, the following two inputs are not for bcm_vlan_port_create, but for vlan_translation_vlan_port_create_to_translation.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        vp1.egress_vlan = vswitch_metro_mp_info.p_vlans[base + 2];
        vp1.egress_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 3];
    }

    vp1.flags = flags;
    vp1.vlan_port_id = *port_id;
    /* this is relevant only when get the gport, not relevant for creation */
    /* will be pupulated when the gport is added to service, using vswitch_port_add */
    vp1.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vp1);
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    /* The following two inputs are used for eve */
    vp1.egress_vlan = vswitch_metro_mp_info.p_vlans[base + 2];
    vp1.egress_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 3];

    /* save the created gport */
    *port_id = vp1.vlan_port_id;

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (soc_property_get(unit , "bcm886xx_vlan_translate_mode",0) || is_device_or_above(unit, JERICHO2)) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }
    if(verbose >= 1){
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x in unit %d\n",vp1.vlan_port_id, vp1.port, vp1.match_vlan, vp1.match_inner_vlan, unit);
    }

  
  return rv;
}

/*
 * add L2 entry points to the given gport
 */
int
vswitch_add_l2_addr_to_gport(int unit, int gport_id, bcm_mac_t mac1, bcm_vlan_t v1){
    int rv;
    bcm_gport_t g1;
    bcm_l2_addr_t l2_addr1;
    
    /* local: MACT, add entry points to local-port */
    bcm_l2_addr_t_init(&l2_addr1,mac1,v1); 
    l2_addr1.flags = BCM_L2_STATIC; /* static entry */
    g1 = gport_id;
    l2_addr1.port = g1;
    rv = bcm_l2_addr_add(unit,&l2_addr1);
    
    /*rv = bcm_vlan_create(unit,v1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with %d\n", v1);
        return rv;
    }
    printf("  0x%08x\n\r",rv);*/
    return rv;
}

int
vswitch_metro_mp_init_vlan(int port, int ndx, int vlan){
     vswitch_metro_mp_info.p_vlans[(port -1) *4 + ndx] = vlan;
}

/** This function accept a unit, system port, and two pointers to int, fills with local port and is local or not.*/
int
system_port_is_local(int unit, int sysport, int *local_port, int *is_local){
    int rv = BCM_E_NONE;
    int my_modid, max_modid, gport_modid, num_modid, modid_idx;
    int gport;

    *is_local = 0;

    if (!BCM_GPORT_IS_SET(sysport))
    {
        *local_port = sysport;
        *is_local = 1;
        return BCM_E_NONE;
    }

    rv = bcm_stk_sysport_gport_get(unit, sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's unit%d  sysport is :%d,gport is: 0x%x\n",unit, sysport, gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    *local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid is: 0x%x\n", gport_modid);
    printf("Sysport's local port is: 0x%x\n", *local_port);
    rv = bcm_stk_modid_count(unit, &num_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_count\n");
    }
    rv = bcm_stk_modid_count(unit, &max_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_count\n");
    }
    if (is_device_or_above(unit, JERICHO2)) {
        bcm_stk_modid_config_t modid_config[max_modid];
        rv = bcm_stk_modid_config_get_all(unit, max_modid, modid_config, &num_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_config_get_all\n");
        }

        for (modid_idx = 0; modid_idx < num_modid; ++modid_idx){
            if (gport_modid == modid_config[modid_idx].modid){
                *is_local = 1;
                printf("sysport 0x%x is a local port for unit %d\n", sysport, unit);
                break;
            } 
        }
        if (*is_local == 0) {
            printf("sysport 0x%x is not a local port for unit %d\n", sysport, unit);
        }
    } else {
        rv = bcm_stk_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
        }

        /* By default, modids configured on a device are always consecutive, and the modid returned from bcm_stk_modid_get is always the lowest. */
        if ((gport_modid >= my_modid) && (gport_modid < (my_modid + max_modid))){
            *is_local = 1;
            printf("sysport 0x%x is a local port for unit %d\n", sysport, unit);
        } else {
            *is_local = 0;
            printf("sysport 0x%x is not a local port for unit %d\n", sysport, unit);
        }
    }

    return rv;
}

int
vswitch_metro_mp_multi_device_arad_run(int *units_ids, int nof_units, int known_mac_lsb, int known_vlan){
    int rv;
    bcm_gport_t gport1,gport2, gport3;
    bcm_vlan_t  vsi1;
    bcm_mac_t kmac;
    int index;
    bcm_vlan_t kvlan;
    int i, unit, local_unit, remote_unit, flags;
    bcm_port_tpid_class_t port_tpid_class;
    int local_port;

    kmac[5] = known_mac_lsb;
    kvlan = known_vlan;

    /* init values */  
    /* following setting will be set according to this call */
    /*vswitch_metro_mp_info_init(port);*/
  
    /* 
     * For JR, set ports to identify double tags packets.
     * For JR2 and above devices, use the default configuration.
     */

    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[0]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if (!is_device_or_above(unit, JERICHO2)) {
            port_tpid_init(vswitch_metro_mp_info.sysports[0],1,1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set, in unit %d\n", unit);
                print rv;
                return rv;
            }
         } else {
            bcm_port_tpid_class_t_init(&port_tpid_class);
            multi_dev_system_port_to_local(unit, vswitch_metro_mp_info.sysports[0], &local_port);

            port_tpid_class.tpid1 = 0x8100;
            port_tpid_class.tpid2 = 0x9100;
            port_tpid_class.port = local_port;
            port_tpid_class.tag_format_class_id = 16;
            port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    
            rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_class_set( in_port )\n");
                return rv;
            }
        }
    }

    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[1]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if (!is_device_or_above(unit, JERICHO2)) {
            port_tpid_init(vswitch_metro_mp_info.sysports[1],1,1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set, in unit %d\n", unit);
                print rv;
                return rv;
            }
         } else {
            bcm_port_tpid_class_t_init(&port_tpid_class);
            multi_dev_system_port_to_local(unit, vswitch_metro_mp_info.sysports[1], &local_port);

            port_tpid_class.tpid1 = 0x8100;
            port_tpid_class.tpid2 = 0x9100;
            port_tpid_class.port = local_port;
            port_tpid_class.tag_format_class_id = 16;
            port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    
            rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_class_set( in_port )\n");
                return rv;
            }
        }
    }

    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[2]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if (!is_device_or_above(unit, JERICHO2)) {
            port_tpid_init(vswitch_metro_mp_info.sysports[2],1,1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set, in unit %d\n", unit);
                print rv;
                return rv;
            }
        } else {
            bcm_port_tpid_class_t_init(&port_tpid_class);
            multi_dev_system_port_to_local(unit, vswitch_metro_mp_info.sysports[2], &local_port);

            port_tpid_class.tpid1 = 0x8100;
            port_tpid_class.tpid2 = 0x9100;
            port_tpid_class.port = local_port;
            port_tpid_class.tag_format_class_id = 16;
            port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    
            rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_class_set( in_port )\n");
                return rv;
            }
        }
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */    
    for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
        if (soc_property_get(unit , "bcm886xx_vlan_translate_mode",0) || is_device_or_above(unit, JERICHO2)) {
            rv = vlan_translation_default_mode_init(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_translation_default_mode_init\n");
                return rv;
            }
        }
    }

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        for (index = 0; index < 12; index++)
        {
            if (single_vlan_tag && (((index % 2) != 0))) {
                continue;
            }
            rv = vlan__init_vlan(unit,vswitch_metro_mp_info.p_vlans[index]);
            if (rv != BCM_E_NONE) {
                printf("Error, vlan__init_vlan\n");
                print rv;
                return rv;
            }
        }
    }

    /* 1. create vswitch + create Multicast for flooding */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_create(unit, &vsi1, i /* On first iteration, with_id = i = 0*/);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_create\n");
            print rv;
            return rv;
        }
    }

    /* 2. create first vlan-port */

    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[0]);
    flags = (is_device_or_above(unit, JERICHO2) && vswitch_metro_mp_info.egress_failover_id[0]) ? BCM_VLAN_PORT_EGRESS_PROTECTION : 0;
    gport1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if(is_system_port_is_local(unit, vswitch_metro_mp_info.sysports[0]) || vswitch_metro_mp_info.multi_bidir) {
            rv = vswitch_metro_add_port(unit, 0, &gport1, flags);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_add_port\n");
                print rv;
                return rv;
            }
            if(verbose){
                printf("created vlan-port   0x%08x in unit %d\n",gport1, unit);
            }        
            flags |= BCM_VLAN_PORT_WITH_ID;
        }
    }
    /* 3. add vlan-port to the vswitch and multicast */
    local_unit = units_ids[0];
    remote_unit = (nof_units > 1) ? units_ids[1] : -1;
    rv = vswitch_add_port_multi_dev(local_unit, remote_unit, vsi1, vswitch_metro_mp_info.sysports[0], gport1);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port_multi_dev\n");
        return rv;
    }
    
    /* add another port to the vswitch */

    /* 4. create second vlan-port */
    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[1]);
    flags = (is_device_or_above(unit, JERICHO2) && vswitch_metro_mp_info.egress_failover_id[1]) ? BCM_VLAN_PORT_EGRESS_PROTECTION : 0;
    gport2 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if(is_system_port_is_local(unit, vswitch_metro_mp_info.sysports[1]) || vswitch_metro_mp_info.multi_bidir) {
            rv = vswitch_metro_add_port(unit, 1, &gport2, flags);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_add_port_2\n");
                return rv;
            }
            if(verbose){
                printf("created vlan-port   0x%08x\n\r",gport2);
            }    
            flags |= BCM_VLAN_PORT_WITH_ID;
        }
    }
    /* 5. add vlan-port to the vswitch and multicast */
    local_unit = units_ids[0];
    remote_unit = (nof_units > 1) ? units_ids[1] : -1;
    rv = vswitch_add_port_multi_dev(local_unit, remote_unit, vsi1, vswitch_metro_mp_info.sysports[1], gport2);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port_multi_dev\n");
        return rv;
    }

    /* add a third port to the vswitch */
  
    /* 6. create third vlan-port */
    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[2]);
    flags = (is_device_or_above(unit, JERICHO2) && vswitch_metro_mp_info.egress_failover_id[2]) ? BCM_VLAN_PORT_EGRESS_PROTECTION : 0;
    gport3 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if(is_system_port_is_local(unit, vswitch_metro_mp_info.sysports[2]) || vswitch_metro_mp_info.multi_bidir) {
            rv = vswitch_metro_add_port(unit, 2, &gport3, flags);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_add_port_2\n");
                return rv;
            }
            if(verbose){
                printf("created vlan=port   0x%08x in unit %d\n",gport3, unit);
            }     
            flags |= BCM_VLAN_PORT_WITH_ID;
        }
    }
    /* 7. add vlan-port to the vswitch and multicast */
    local_unit = units_ids[0];
    remote_unit = (nof_units > 1) ? units_ids[1] : -1;
    rv = vswitch_add_port_multi_dev(local_unit, remote_unit, vsi1, vswitch_metro_mp_info.sysports[2], gport3);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port_multi_dev\n");
        return rv;
    }

    /* 8. add vlan-port to the l2 addr */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if(is_system_port_is_local(unit, vswitch_metro_mp_info.sysports[2]) || vswitch_metro_mp_info.multi_bidir) {
            rv = vswitch_add_l2_addr_to_gport(unit, gport3, kmac, kvlan);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_add_l2_addr_to_gport\n");
                return rv;
            }
        }
    }

    vswitch_metro_mp_info.vsi = vsi1;
    return rv;
}


int vswitch_create(int unit, bcm_vlan_t  *new_vsi, int with_id){
    int rv;
    bcm_vlan_t vsi1;
    int multicast_id1;
    bcm_vlan_control_vlan_t vsi_control;
   
    /* 1. create vswitch */
    if (!with_id){
        rv = bcm_vswitch_create(unit,new_vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_create\n");
            return rv;
        }
        
        if(verbose){
            printf("created vswitch   0x%08x\n\r", *new_vsi);
        }
    } else {
        rv = bcm_vswitch_create_with_id(unit, *new_vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_create\n");
            return rv;
        }
        
        if(verbose){
            printf("created vswitch with id  0x%08x\n\r", *new_vsi);
        }
    }
    vsi1 = *new_vsi;     
    /* 2. create multicast: 
     *     - to be used for vswitch flooding 
     *     - has to be same id as vswitch 
     */
	egress_mc = 0;
	int mc_group = vsi1;
    rv = multicast__open_mc_group(unit, &mc_group, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
    if (rv != BCM_E_NONE) {
    	printf("Error, multicast__open_mc_group\n");
    	return rv;
    }

    if (vsi1 < 4096) {
        /* Also egress create in case of smaller than 4K */        
        multicast_id1 = vsi1;
        rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID|(is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2), &multicast_id1);
        if (rv != BCM_E_NONE) {
            printf("Error, in mc create, mc_group $vsi1 \n");
            return rv;
        }
        return rv;
    }
    if(verbose){
        printf("created multicast   0x%08x\n\r",vsi1);
    }
    
    /*  set vlan control, to flood */
    bcm_vlan_control_vlan_t_init(&vsi_control);
    /* setting FID, filtering database, for accessing the MACT, for shared learning */
    /* for VSI (not vlans) has to be equal to VSI */
    vsi_control.forwarding_vlan = vsi1;
    /* set multicast groups for flooding */
    /* 2 options:
    1. all values equal, and equal to VSI
    2. uc_group = VSI, and mc = uc + 4k, and bc = mc + 4k */
    vsi_control.unknown_unicast_group   = vsi1;
    vsi_control.unknown_multicast_group = vsi1;
    vsi_control.broadcast_group         = vsi1;
    
    
    rv = bcm_vlan_control_vlan_set(unit,vsi1,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }
    
    return BCM_E_NONE;
}


int system_multicast_vlan_port_add(int local_unit, int remote_unit, bcm_port_t port, bcm_gport_t gport, int mc_group_id) {
    int rv;
    int encap_id;
    int modid, modport;

    rv = bcm_multicast_vlan_encap_get(local_unit, mc_group_id, port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
		printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, port, gport);
        return rv;
    }

    if (!BCM_GPORT_IS_MODPORT(port) && !BCM_GPORT_IS_SYSTEM_PORT(port)) {

        int is_dnx;
        int modid_count, actual_modid_count;

        rv = bcm_device_member_get(local_unit, 0, bcmDeviceMemberDNX, &is_dnx);
        if (BCM_FAILURE(rv)) {
            printf("bcm_device_member_get failed");
            return(rv);
        }

        if (is_dnx) {
            rv = bcm_stk_modid_count(local_unit, &modid_count);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_stk_modid_count\n");
                return rv;
            }
            bcm_stk_modid_config_t modid_array[modid_count];
            rv = bcm_stk_modid_config_get_all(local_unit, modid_count, modid_array, &actual_modid_count);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_stk_modid_config_get_all\n");
                return rv;
            }
            modid = modid_array[0].modid;
        }
        else {
            rv = bcm_stk_my_modid_get(local_unit, &modid);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_stk_modid_get\n");
                return rv;
            }
        }

        BCM_GPORT_MODPORT_SET(modport, modid, port);
    } else {
        modport = port;
    }
    
    rv = multicast__add_multicast_entry(local_unit, mc_group_id, &modport , &encap_id, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry local_unit: %d mc_group_id:  0x%08x  modport:  0x%08x  encap_id:  0x%08x \n", local_unit, mc_group_id, modport, encap_id);
        return rv;
    }

    if (remote_unit != -1) {
        rv = multicast__add_multicast_entry(remote_unit, mc_group_id, &modport , &encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in multicast__add_multicast_entry remote_unit: %d mc_group_id:  0x%08x  modport:  0x%08x  encap_id:  0x%08x \n", remote_unit, mc_group_id, modport, encap_id);
            return rv;
        }
    }
    return rv;
}

int vswitch_add_port_multi_dev(int local_unit, int remote_unit, bcm_vlan_t  vsi, bcm_port_t phy_port, bcm_gport_t gport){
    int rv;
    int sysport_is_local = 0;
    int local_port = 0;
    
    rv = bcm_vswitch_port_add(local_unit, vsi, gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    if(verbose){
        printf("add port   0x%08x to vswitch   0x%08x \n\r",gport, vsi);
    }
    /* update Multicast to have the added port  */
    rv = system_multicast_vlan_port_add(local_unit, remote_unit, phy_port , gport, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, system_multicast_vlan_port_add\n");
        return rv;
    }
    if(verbose){
        printf("add port   0x%08x to multicast 0x%08x in local_unit %d, remote_unit %d\n",gport, vsi, local_unit, remote_unit);
    }

    return BCM_E_NONE;
}


int vswitch_add_port(int unit, bcm_vlan_t  vsi, bcm_port_t phy_port, bcm_gport_t gport){
    int rv;

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit, vsi, gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    if(verbose){
        printf("add port   0x%08x to vswitch   0x%08x \n\r",gport, vsi);
    }
    /* update Multicast to have the added port  */
    rv = multicast__gport_encap_add(unit, vsi, phy_port , gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__gport_encap_add\n");
        return rv;
    }
    if(verbose){
        printf("add port   0x%08x to multicast 0x%08x in unit %d\n",gport, vsi, unit);
    }

    return BCM_E_NONE;
}


/* use vswitch_metro_mp_run_with_defaults to run with default settings:
 * otherwise : do 
 * vswitch_metro_mp_info_init 
 * vswitch_metro_mp_init_vlan
 * vswitch_metro_mp_run
 */

int
vswitch_metro_mp_run_with_defaults_multi_device (int *units_ids, int nof_units , int multi_bidir, int sysport_1, int sysport_2, int sysport_3){
    int rv;

    vswitch_metro_mp_info.multi_bidir = multi_bidir;
    vswitch_metro_mp_info_init(sysport_1, sysport_2, sysport_3);
    rv = vswitch_metro_mp_multi_device_arad_run(units_ids, nof_units, 0xce, 3);
    if (rv != BCM_E_NONE){
        printf("Error, in vswitch_metro_mp_multi_device_arad_run\n");
        return rv;
    }
    return rv;
}

/* Kept for backward compatibilty*/
int
vswitch_metro_mp_run_with_defaults(int unit, int port1, int port2, int port3) {
    int rv;
    int sysport1, sysport2, sysport3;
    port_to_system_port(unit, port1, &sysport1);
    port_to_system_port(unit, port2, &sysport2);
    port_to_system_port(unit, port3, &sysport3);

    rv = vswitch_metro_mp_run_with_defaults_multi_device(&unit, 1, 0, sysport1, sysport2, sysport3);
    return rv;
}

int
vswitch_metro_mp_delete_vlan_mac(int unit, int known_mac_lsb, int known_vlan){
  int rv;
  int index;
  bcm_mac_t kmac;
  bcm_vlan_t kvlan;
  bcm_pbmp_t p,u;
  bcm_port_config_t c;

  rv = bcm_port_config_get(unit, &c);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_port_config_get\n");
    return rv;
  }
  BCM_PBMP_ASSIGN(p, c.e);
  
  for (index = 0; index < 12; index++) {
    if (single_vlan_tag && (((index % 2) != 0))) {
       continue;
    } 
    rv = bcm_vlan_port_remove(unit, vswitch_metro_mp_info.p_vlans[index], p);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vlan_port_remove, vlan=%d, \n", vswitch_metro_mp_info.p_vlans[index]);
      return rv;
    }
    rv = bcm_vlan_destroy(unit,vswitch_metro_mp_info.p_vlans[index]);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vlan_destroy, vlan=%d, \n", vswitch_metro_mp_info.p_vlans[index]);
      return rv;
    }
  }

  kmac[5] = known_mac_lsb;
  kvlan = known_vlan;
  
  rv = bcm_l2_addr_delete(unit,kmac,kvlan);
  
  if (rv != BCM_E_NONE) {
      printf("Error, in bcm_l2_addr_delete with %d\n", kvlan);
      return rv;
  }

  return rv;
}

/* 
* Search vlan-port with index in array vswitch_metro_mp_info.
* vlan-port: is Logical interface identified by (port-vlan-vlan). 
* index 0 - valn_port 1, index 1 - vlan_port 2, index 2 - vlan_port 3
*/
int
vswitch_metro_find_port(int unit, int index, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    
    /* the match criteria is port, out-vlan, in-vlan*/
    vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_VLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
      
    vp1.port = vswitch_metro_mp_info.sysports[index];
    
    vp1.match_vlan = vswitch_metro_mp_info.p_vlans[index*4];
    vp1.match_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[index*4+1];

    rv = bcm_vlan_port_find(unit,&vp1);
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_find\n");
        print rv;
        return rv;
    }

    *port_id = vp1.vlan_port_id;

    return rv;
}


/*
 * Remove vlan-port from vsi and delete it.
 * index 0 - vlan_port 1, index 1 - vlan_port 2, index 2 - vlan_port 3
 */
int
vswitch_metro_delete_port(int unit, int index, bcm_gport_t *port_id){
    int rv;
    bcm_mac_t kmac;
    uint16 vsi;
  
    kmac[5] = 0xce;	
    
    if (*port_id == -1){
        rv = vswitch_metro_find_port(unit, index, port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_metro_find_port\n");
            return rv;
        }
    }
    
    rv = bcm_vswitch_port_get(unit, *port_id, &vsi);
    if (rv != BCM_E_NONE) {
  	    printf("Error, bcm_vswitch_port_get\n");
  	    return rv;
    }
    
    rv = bcm_vswitch_port_delete(unit, vsi, *port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_delete\n");
        return rv;
    }
  
    if(verbose){
        printf("remove port   0x%08x from vsi   0x%08x \n",*port_id, vsi);
    }
  
    rv = bcm_vlan_port_destroy(unit,*port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }
  
    if(verbose){
        printf("delete port   0x%08x in unit %d \n",*port_id, unit);
    }
    
    return rv;
}


int vswitch_delete(int unit){
   int rv;

   /*1. delete multicast group*/
   rv = bcm_multicast_destroy(unit, vswitch_metro_mp_info.vsi);
   if (rv != BCM_E_NONE) {
       printf("Error, bcm_multicast_destroy \n");
       return rv;
   }

   if(verbose){
       printf("delete multicast   0x%08x in unit %d\n",vswitch_metro_mp_info.vsi, unit);
   }
   
   /*2. destroy vswitch */
   rv = bcm_vswitch_destroy(unit, vswitch_metro_mp_info.vsi);
   if (rv != BCM_E_NONE) {
     printf("Error, bcm_vswitch_destroy \n");
     return rv;
   }

   if(verbose){
       printf("destroy vswitch   0x%08x in unit %d\n", vswitch_metro_mp_info.vsi, unit);
    }
     
   return BCM_E_NONE;
}


/* 
*use vswitch_metro_mp_remove_all to remove all gports, vswitch and multicast group.
* Or use vswitch_metro_delete_port to delete one gport.
*/

int
vswitch_metro_mp_remove_all (int *units_ids, int nof_units){
    int rv;
	int index;
    int unit, i;
    bcm_gport_t port_id;


    for (index = 0; index < 3; index++) {
        units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[index]);
        port_id = -1;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = vswitch_metro_delete_port(unit,index, &port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_delete_port in unit %d\n", unit);
                return rv;
            }
        }
    }

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_delete(unit);
        if (rv != BCM_E_NONE) {
          printf("Error, vswitch_delete in unit %d\n", unit);
          return rv;
        }
    }
    /*delete other vlans and static mac*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_metro_mp_delete_vlan_mac(unit, 0xce, 3);
        if (rv != BCM_E_NONE) {
          printf("Error, vswitch_metro_mp_delete_vlan_mac in unit %d\n", unit);
          return rv;
        }
    }
	
    return rv;
}

int
vswitch_metro_mp_remove_all_dvapi (int unit){
    return vswitch_metro_mp_remove_all (&unit, 1);
}

