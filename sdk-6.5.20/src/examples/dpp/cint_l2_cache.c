/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: L2 Cache~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_l2_cache.c 
 * Purpose: Example of the usage of l2_cahce APIs to trap BPDUs. 
 *  
 * Via this capability, you can control packets based on <incoming-port, MAC-DA, subtype>. 
 * The action can be to drop/trap/redirect/snoop the packet. 
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
 * In Arad, the l2-cache entry has to fulfill the following rules:
 *  o    Classification has to include the explicit port, that is, src_port_mask = 0 
 *       and src_port has to be a valid local port.
 *  o    vlan_mask/vlan are ignored and has to be set to  - 1. 
 *  o    DA can be one of the following:
 *       -    Either MAC DA is reserved MC MAC, and mac_mask has to be all zero
 *       OR 
 *       -    MAC DA can be any MAC address and mac_mask can mask only consecutive lsb-bits of the MAC
 *       -    In the second case, you can supply also the subtype.
 *  
 * Calling l2_cache_init(unit, -1, 0x0180, 0xc2000002,  $0x0188, $0xc2005505, 1, 5, 10);
 *  o    Send packets ingress from port 1 with DA: 01-80-c2-00-00-02 to port 5 with priority (Traffic-Class) 6
 *  o    Drop packets ingress from port 1 with DA: 01-88-c2-00-55-xx and subtype 0 
 *
 * Multi Device
 *  o    For a two-unit example, call l2_cache_init(unit, 1, 0x0180, 0xc2000002,  $0x0188, $0xc2005505, 1, 5, 10); 
 * Note:     
 *  o    This is the same as the one-device example, except that the In port is on device 0 
 *       and the Out port is on device 1.
 *  o    The "unit" device should be the master unit and the "second_unit" device should be the slave unit.
 */
 
uint8 all_ones_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8 all_zeros_mac[6] = {0, 0, 0, 0, 0, 0};
uint8 reserved_mac_base[6] = {0x01, 0x80, 0xc2, 0x0, 0x0, 0x00};
uint8 reserved_mac_max[6] = {0x01, 0x80, 0xc2, 0x0, 0x0, 0x3f};
uint8 sample_mac[6] = {0x01, 0x88, 0xc2, 0x0, 0x55, 0x00};
uint8 is_two_device_example = 0;
int   second_device_module_id = 1;

/* set key of the l2-cache */
void l2_cache_key_fill(int port, bcm_mac_t mac_add, bcm_mac_t mac_add_mask, uint8 subtype, bcm_l2_cache_addr_t* cache_addr){
    bcm_l2_cache_addr_t_init(cache_addr);
    /* destination mac address */
    sal_memcpy(cache_addr->mac, mac_add, 6);
    /* mac mask, can be all-ones, or consecutive from msb to lsb */
    sal_memcpy(cache_addr->mac_mask, mac_add_mask, 6);
    /* Incoming port */
    cache_addr->src_port = port;
    cache_addr->src_port_mask = 0; /* has to be 0, means to consider in-port as part of the key */
    cache_addr->subtype = subtype;
}

/* set action of l2-cache */
void
l2_cache_action_fill(int flags, int dest_port, int mc_id, bcm_l2_cache_addr_t* cache_addr){
    /* action includes:          
        - destination port / mc-group to forwarded packet port          
        - or flags, for drop, trap, disable learn...     */
    cache_addr->flags = flags;
    /* packet will be forwarded to MC or dest port according to choice of flag */
    cache_addr->group = mc_id;
    cache_addr->dest_port = dest_port;
    if (is_two_device_example)
    {
        BCM_GPORT_MODPORT_SET(cache_addr->dest_port, second_device_module_id, dest_port);
    }
    else{
        cache_addr->dest_port = dest_port;
    }    
}

/* add given l2_cache_addr */
int
l2_cache_add(int unit, bcm_l2_cache_addr_t *cache_addr, int *index_used)
{
    int rv;
    int index = -1; /* has to be -1 */

    rv = bcm_l2_cache_set(unit, index, cache_addr, index_used);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_cache_set, \n");
        return rv;
    }

    return rv;
}

/* delete given l2_cache_addr */
int
l2_cache_delete(int unit, int index_used)
{
    int rv;
    
    rv = bcm_l2_cache_delete(unit, index_used);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_cache_delete, \n");
        return rv;
    }
    
    return rv;
}



/* add given l2_cache_addr */
int
l2_cache_get(int unit, int index_used)
{
    int rv;
    bcm_l2_cache_addr_t cache_addr;

    rv = bcm_l2_cache_get(unit, index_used, &cache_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_cache_get, \n");
        return rv;
    }
    print cache_addr;

    return rv;
}

void mac_int_to_char_array (int high_int, int low_int, uint8 *res) {
    uint8 tmp[8];
    
    sal_memcpy(tmp, &high_int, 4);
    sal_memcpy(&tmp[4], &low_int, 4);
    sal_memcpy(res, &tmp[2], 6);
    printf ("high_int=%d p=%x %x %x %x %x %x\n", high_int, res[0], res[1], res[2], res[3], res[4], res[5] );
}

/*
 * Example:
 * l2_cache_init(unit, -1, 0x0180, 0xc2000002,  0x0188, 0xc2005505, 1, 5, 10, 0, 0, 0);   
 */
int l2_cache_init(int unit, int second_unit, int dest_mac1_high, int dest_mac1_low, 
                       int dest_mac2_high, int dest_mac2_low, int src_port, int dest_port,
                       uint8 subtype, int trap_id, int vlan, int is_reserved_vlan)
{
    int rv;
    bcm_l2_cache_addr_t cache_addr;
    bcm_mac_t mac_add;
    bcm_mac_t mac_add_mask;
    int dest_gport;
    
    int flags;
    /*int dest_port;*/
    int mc_id;
    int act_index;
    uint8 tmp_mac[6];

    if (second_unit>=0) /*i.e two devices*/
    {
        is_two_device_example = 1;
    }
    mac_int_to_char_array(dest_mac1_high, dest_mac1_low, tmp_mac);
    /*printf("------dests: %a , %a\n",dest_mac1, dest_mac2);*/
    /* port:1, MAC: 01-80-c2-00-00-02 --> don't switch and forward to port 2 and update prio (TC)*/
    
    sal_memcpy(mac_add, tmp_mac, 6);
    
    sal_memcpy(mac_add_mask, all_ones_mac, 6);
    
    l2_cache_key_fill(src_port , mac_add, mac_add_mask, 0, &cache_addr);
    /* set action for above key */
    flags = 0;
    /* send packet to port dest_port */
    mc_id = 0;
    /* changed packet priority (TC) */
    cache_addr.prio = 2;
    flags |= BCM_L2_CACHE_SETPRI;


    if (is_reserved_vlan)
    {
    /* If MEF L2CP use the given vlan and trap code */
        BCM_GPORT_TRAP_SET(dest_port, trap_id, 7, 0);
        l2_cache_action_fill(flags, dest_port, mc_id, &cache_addr);        
        cache_addr.vlan = vlan;    
        cache_addr.src_port = 0;
        cache_addr.vlan_mask = -1;
   }
    else {          
        BCM_GPORT_LOCAL_SET(dest_gport, dest_port);
        l2_cache_action_fill(flags, dest_gport, mc_id, &cache_addr);    
        cache_addr.vlan_mask = 0;         
    }
    
    rv = l2_cache_add(unit, &cache_addr, &act_index);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2_cache_add, \n");
        return rv;
    }
    printf ("act_index 1: 0x%x \n", act_index);
    
    rv = l2_cache_get(unit, act_index);
    
    printf ("act_index 1: 0x%x \n", act_index);
    
     if (!is_reserved_vlan) {   
        /* port:1, MAC: 01-88-c2-00-55-xx & subtype:1 --> drop */
        mac_int_to_char_array(dest_mac2_high, dest_mac2_low, tmp_mac);
        sal_memcpy(mac_add, tmp_mac, 6);
        sal_memcpy(mac_add_mask, all_ones_mac, 6);
        mac_add_mask[5] = 0x00;
        l2_cache_key_fill(src_port , mac_add, mac_add_mask, subtype, &cache_addr);
        /* set action for above key */
        /* flags, drop/trap, consider subtype from key */
        flags = BCM_L2_CACHE_DISCARD|BCM_L2_CACHE_SUBTYPE;
        dest_port = 0;
        mc_id = 0;
        l2_cache_action_fill(flags, dest_port, mc_id, &cache_addr);
        rv = l2_cache_add(unit, &cache_addr, &act_index);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in l2_cache_add, \n");
            return rv;
        }
    
        printf ("act_index 2: 0x%x \n", act_index);
        rv = l2_cache_get(unit, act_index);
        if (rv != BCM_E_NONE) {
            printf("Error, in l2_cache_get, \n");
            return rv;
        }
        printf ("act_index 1: 0x%x \n", act_index);
    }
    return rv;
}
  
/*
 * for one device Call (second_unit<0) :
 *      l2_cache_run_with_defaults(unit, -1, 1, 5);
 * 
 * for two devices Call : 
 *      l2_cache_run_with_defaults(unit, 1, 1, 5);

 *                                             
 * mac 01:88:c2:00:55:xx: Packet is dropped and sub type is considered. 
 */
int l2_cache_run_with_defaults(int unit, int second_unit, int src_port, int dest_port)
{ 
    return l2_cache_init(/*unit*/             unit,
                         /*second_unit*/      second_unit,
                         /*dest_mac1_high*/   0x0180,
                         /*dest_mac1_low*/    0xc2000002,
                         /*dest_mac2_high*/       0x0188,
                         /*dest_mac2_low*/       0xc2005505,
                         /*src_port*/       src_port,
                         /*dest_port*/       dest_port,
                         /*subtype*/       10,
                         /*trap_id*/       0,
                         /*vlan*/        0,
                         /*is_reserved_vlan*/       0);
}
    
/*
 * Example: l2_cache_vlan_run_with_defaults(unit, -1);  
 * mac 01:80:c2:00:00:02 (lsb 2) vlan 10 default action is trap to CPU.
 */
int l2_cache_mef_l2cp_run_with_defaults(int unit, int second_unit)
{ 

    bcm_rx_trap_config_t trap_config;
    int trap_id;
    int rv;
       
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.dest_port = 0;
    trap_config.trap_strength = 7;
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapL2cpPeer, &trap_id);
    printf("trap id = $trap_id \n");
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    } 
    return l2_cache_init(/*unit*/             unit,
                         /*second_unit*/      -1,
                         /*dest_mac1_high*/      0x0180,
                         /*dest_mac1_low*/      0xc2000002,
                         /*dest_mac2_high*/      0x0188,
                         /*dest_mac2_low*/      0xc2005505,
                         /*src_port*/      0,
                         /*dest_port*/      0,
                         /*subtype*/      0, 
                         /*trap_id*/      trap_id,
                         /*vlan*/      10,
                         /*is_reserved_vlan*/      1);
}

/*
 * Configure transparency for the input l2cp packet.
 * is_reserved_vlan :
 *     1, configure with vlan;
 *     2, configure with vlan and encap_id;
 *     0, invalid value here.
 */
int l2_cache_mef_l2cp_transparency(int unit, int second_unit, bcm_port_t port,
                                   int mac_high, int mac_low,
                                   int vlan, int is_reserved_vlan)
{
    int rv;
    bcm_l2_cache_addr_t cache_addr;
    bcm_mac_t mac_add;
    bcm_mac_t mac_add_mask;
    int dest_gport;
    int dest_port;
    int encap_id;

    int flags;
    /*int dest_port;*/
    int mc_id;
    int act_index;
    uint8 tmp_mac[6] = { 0 };

    rv = l2_cache_mef_l2cp_eve(unit, port, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in create eve \n");
        return rv;
    }

    mac_int_to_char_array(mac_high, mac_low, tmp_mac);
    printf("------Dest Mac Address: %2x-%2x-%2x-%2x-%2x-%2x\n",
           tmp_mac[0], tmp_mac[1],tmp_mac[2],tmp_mac[3],tmp_mac[4],tmp_mac[5]);

    sal_memcpy(mac_add, tmp_mac, 6);
    sal_memcpy(mac_add_mask, all_ones_mac, 6);

    bcm_l2_cache_addr_t_init(&cache_addr);
    l2_cache_key_fill(0 , mac_add, mac_add_mask, 0, &cache_addr);
    /* set action for above key */
    flags = 0;
    /* send packet to port dest_port */
    mc_id = 0;
    /* changed packet priority (TC) */
    cache_addr.prio = 2;
    flags |= BCM_L2_CACHE_SETPRI;
    flags |= BCM_L2_CACHE_TUNNEL;

    /*MEF L2CP use the given vlan and trap code */
    BCM_GPORT_TRAP_SET(dest_port, 0, 7, 0);
    l2_cache_action_fill(flags, dest_port, mc_id, &cache_addr);
    cache_addr.src_port = 0;

    if (is_reserved_vlan == 1) {
        cache_addr.vlan = 1;
        cache_addr.vlan_mask = -1;
    } else if (is_reserved_vlan == 2){
        cache_addr.encap_id = encap_id;
        cache_addr.vlan = 1;
        cache_addr.vlan_mask = -1;
    } else {
        cache_addr.encap_id = encap_id;
        printf("Error, Valid vlan is needed!\n");
    }

    rv = l2_cache_add(unit, &cache_addr, &act_index);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2_cache_add, \n");
        return rv;
    }
    printf ("act_index 1: 0x%x \n", act_index);

    rv = l2_cache_get(unit, act_index);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2_cache_get, \n");
        return rv;
    }
    printf ("act_index 1: 0x%x \n", act_index);
    return rv;
}

/*
 * Example: l2_cache_vlan_run_with_defaults(unit, -1);
 * mac 01:80:c2:00:00:02 (lsb 2) vlan 10 default action is trap to CPU.
 */
int l2_cache_mef_l2cp_eve(int unit, bcm_port_t port, int *encap_id)
{
    int rv;
    bcm_vlan_port_t vp;
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vp.port = port;
    vp.vsi = BCM_VLAN_ALL;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create \n");
        return rv;
    }

    int edit_class_id=5;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = vp.vlan_port_id;
    port_trans.vlan_edit_class_id = edit_class_id;
    port_trans.new_outer_vlan = 20;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_translation_set \n");
        return rv;
    }

    int action_id_1=2;
    bcm_vlan_action_set_t action;
    int flags=BCM_VLAN_ACTION_SET_EGRESS;

    bcm_vlan_translate_action_id_create(unit, flags|BCM_VLAN_ACTION_SET_WITH_ID, &action_id_1);
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionAdd;
    action.priority = 0;
    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id_1, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_action_id_set \n");
        return rv;
    }

    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = edit_class_id;
    action_class.tag_format_class_id =0;
    action_class.vlan_translation_action_id= action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_action_class_set \n");
        return rv;
    }

    *encap_id = vp.encap_id;
    printf("encap_is is %d from bcm_vlan_port_create.\n", *encap_id);

    return rv;
}

/*
 * Example: l2_cache_mef_l2cp_transparency
 * mac 01:80:c2:00:00:02 (lsb 2) vlan 1 with action of transparency.
 */
int l2_cache_mef_l2cp_run_with_transparency(int unit, bcm_port_t port, int vlan)
{
    int mac_high = 0x0180;
    int mac_low  = 0xc2000002;
    return l2_cache_mef_l2cp_transparency(unit, -1, port,
                                          mac_high, mac_low, vlan, 2);
}
