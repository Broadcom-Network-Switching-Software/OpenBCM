/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides L2 basic functionality and defines L2 global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */
bcm_mac_t g_l2_global_mac_utils    = {0x00, 0x11, 0x00, 0x00, 0x00, 0x00};

uint8 src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
uint8 dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

static int OLP_DEFAULT_CORE = 0;
int is_section_oam = 0;

/* *************************************************************************************************** */


struct l2__mact_properties_s
{
    int gport_id;
    bcm_mac_t mac_address;
    bcm_vlan_t vlan;
};
    
struct l2__arp_entry_properties_s
{
    uint32 flags;               /* BCM_L2_EGRESS_XXX flags. */
    bcm_mac_t dest_mac;         /* Destination MAC address to replace with. */
    bcm_mac_t src_mac;          /* Source MAC address to replace with. */
    bcm_vlan_t outer_vlan;      /* outer vlan tag to replace with. */
    uint16 outer_tpid;          /* outer tpid to replace with. */
    bcm_vlan_t inner_vlan;      /* inner vlan tag to replace with. */
    uint16 inner_tpid;          /* inner tpid to replace with. */
    bcm_gport_t dest_port;      /* Destination gport to associate this l2 egress
                                   entry with */
    bcm_ethertype_t ethertype;  /* Ethertype to replace with. */
    bcm_if_t encap_id;          /* Encapsulation index */
    bcm_if_t l3_intf;           /* L3 interface (tunnel). Used only in Native
                                   Routing overlay protocols. */
    int mpls_extended_label_value; /* extended label to be placed on top of mpls stack (e.g. for frr label) */
};

enum l2__mymac_type_s {
    l2__mymac_type_global_msb,          /* Global my-MAC */
    l2__mymac_type_ipv4_vsi,            /* VSI 12 LSBs of my-MAC */
    l2__mymac_type_multiple_mymac,
    l2__mymac_type_count
};

struct l2__mymac_properties_s
{
    l2__mymac_type_s    mymac_type;
    uint32              flags;          /* BCM_L2_STATION_* flags */
    bcm_mac_t           mymac;
    int                 vsi;
    int                 station_id;
};




/*
 *  Set a mymac according to the mymac_properties->mymac_type.
 */
int l2__mymac__set(int unit, l2__mymac_properties_s *mymac_properties)
{
    int rc, station_id;
    bcm_l2_station_t station;
    
    /* Intitialize an L2 station structure */
    bcm_l2_station_t_init(&station);

    /* 
     *  Set the fields that are common to most station types. 
     */ 
    station.dst_mac_mask[0] = 0xff; /* Common mac mask is all 1s */
    station.dst_mac_mask[1] = 0xff;
    station.dst_mac_mask[2] = 0xff;
    station.dst_mac_mask[3] = 0xff;
    station.dst_mac_mask[4] = 0xff;
    station.dst_mac_mask[5] = 0xff;

    station.src_port_mask = 0; /* Most mymacs don't use port */

    /* Copy the mac address */
    sal_memcpy(station.dst_mac, mymac_properties->mymac, 6);


    switch (mymac_properties->mymac_type) {
    case l2__mymac_type_global_msb:
        /* Nothing else to do here */
        break;
    case l2__mymac_type_ipv4_vsi:
        station.dst_mac_mask[0] = 0x00; /* Only use 12 LSBs */
        station.dst_mac_mask[1] = 0x00;
        station.dst_mac_mask[2] = 0x00;
        station.dst_mac_mask[3] = 0x00;
        station.dst_mac_mask[4] = 0x0f;
        station.dst_mac_mask[5] = 0xff;
        station.vlan            = mymac_properties->vsi;
        station.vlan_mask       = 0xffff; /* vsi is  valid */
        station.flags           = BCM_L2_STATION_IPV4;
        break;
    case l2__mymac_type_multiple_mymac:
        station.vlan_mask   = 0xffff;
        station.vlan        = mymac_properties->vsi;
        station.flags       = mymac_properties->flags;
        break;
    default:
        printf("Error, unknown mymac type: %d", mymac_properties->mymac_type);
        return BCM_E_PARAM;
    }
    


    rc = bcm_l2_station_add(unit, &station_id, &station);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l2_station_add\n");
        print rc;
    }

    mymac_properties->station_id = station_id;
    return BCM_E_NONE;
}


/* Delete L2 Logical port */
int l2__port__delete(int unit, bcm_gport_t gport )
{
    return bcm_vlan_port_destroy(unit, gport);
}
int l2__lif_match_none__create(int unit, int flags,  bcm_gport_t port, int vsi, bcm_gport_t *gport ){
    int rv;
    bcm_vlan_port_t vp;
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vp.port = port;
    vp.vsi = vsi;
    vp.flags = flags;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }
    *gport = vp.vlan_port_id;
    return BCM_E_NONE;
}

/* Create L2 Logical port according <port,vlan>*/
int l2__port_vlan__create(int unit, int flags, bcm_gport_t port, int vlan, int vsi, bcm_gport_t *gport){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port;
    vp.match_vlan = vlan;
    if(is_section_oam){
      vp.match_ethertype= 0x8847;
    }
    vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vlan;
    vp.vsi = vsi;
    vp.flags = flags;
    vp.vlan_port_id = *gport; 

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id; 

    return BCM_E_NONE;

}

/* Create L2 Logical port according <port,vlan,vlan>*/
int l2__port_vlan_vlan__create(int unit, int flags, bcm_gport_t port, int vlan, int inner_vlan, int vsi, bcm_gport_t *gport){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.match_inner_vlan = inner_vlan;
    vp.vsi = vsi; 
    vp.flags = flags;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  

    return BCM_E_NONE;
}


/* Create non protection L2 forward group logical port according <port,vlan,vlan> */
int l2__fec_forward_group_port_vlan_vlan__create(int unit, bcm_gport_t port, int vlan, int inner_vlan, int vsi, int *fec, int *out_lif){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.match_inner_vlan = inner_vlan;
    vp.vsi = vsi; 
    vp.flags = BCM_VLAN_PORT_FORWARD_GROUP;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }
    print vp;
    *fec = vp.vlan_port_id;
    *out_lif = vp.encap_id;

    return BCM_E_NONE;
}

/* Create non protection L2 forward group logical port according <port,vlan> */
int l2__fec_forward_group_port_vlan__create(int unit, bcm_gport_t port, int vlan, int vsi, int *fec, int *out_lif){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.vsi = vsi; 
    vp.flags = BCM_VLAN_PORT_FORWARD_GROUP;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }
    print vp;
    *fec = vp.vlan_port_id;
    *out_lif = vp.encap_id;

    return BCM_E_NONE;
}

/* Return the default OLP port */
int l2__olp_port__get(int unit, bcm_gport_t *port){

    bcm_gport_t olp[BCM_PIPES_MAX];
    int count, rv = BCM_E_NONE;

    rv = bcm_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&count);
    if (rv != BCM_E_NONE) {
        printf("Error, l2__olp_port__get\n");
        return rv;
    }
    if (count < 1) {
        printf("Error, No olp port configured\n");
        return BCM_E_NOF_FOUND;
    }

    *port = olp[OLP_DEFAULT_CORE];

    return BCM_E_NONE;
}


int l2__learning__config(int unit){

    int rv;
    bcm_l2_learn_msgs_config_t learn_msgs;
    int age_seconds;
    bcm_gport_t olp_port;

    rv = l2__olp_port__get(unit, &olp_port);
    if (rv != BCM_E_NONE) {
        printf("Error, l2__olp_port__get failed\n");
        return rv;
    }

    printf("olp_port: %d\n", olp_port);

    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;

    learn_msgs.dest_port = olp_port;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, dest_mac_address, 6);

    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

/* Create a mac table entry with da, vsi and port (gport) as inputs */
int l2__mact_entry_create(int unit, l2__mact_properties_s *mact_properties){

    int rv;
    bcm_l2_addr_t l2_address;

    /* local: MACT, add entry points to local-port */
    bcm_l2_addr_t_init(&l2_address,mact_properties->mac_address,mact_properties->vlan); 
    l2_address.flags = BCM_L2_STATIC; /* static entry */
    l2_address.port = mact_properties->gport_id;
    rv = bcm_l2_addr_add(unit,&l2_address);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }
    return rv;
    

    return BCM_E_NONE;
}



/* Create an ARP entry in EEDB
   In ROO, the overlay LL encapsulation is built with a different API call 
   (bcm_l2_egress_create instead of bcm_l3_egress create)
   For untagged vlan, pass outer_tpid == 0
   For one tagged vlan, pass outer_tpid != 0, inner_tpid == 0
   For double tagged vlan pass outer_tpid != 0, inner_tpid != 0 */
int l2__egress_create(int unit, l2__arp_entry_properties_s *arp_entry_properties){

    int rv = BCM_E_NONE;
    bcm_l2_egress_t arp_entry;

    bcm_l2_egress_t_init(&arp_entry);

    arp_entry.dest_mac   = arp_entry_properties->dest_mac; /* next hop */
    arp_entry.src_mac    = arp_entry_properties->src_mac; /* my-mac */
    arp_entry.outer_vlan = arp_entry_properties->outer_vlan;      
    arp_entry.ethertype  = arp_entry_properties->ethertype;       /* ethertype for IP */
    arp_entry.outer_tpid = arp_entry_properties->outer_tpid;      /* outer tpid */
    arp_entry.inner_vlan = arp_entry_properties->inner_vlan; /* inner vlan */
    arp_entry.inner_tpid = arp_entry_properties->inner_tpid; /* inner tpid */
    arp_entry.mpls_extended_label_value = arp_entry_properties->mpls_extended_label_value;

    rv = bcm_l2_egress_create(unit,&arp_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_egress_create\n");
        return rv;
    }

    arp_entry_properties->encap_id = arp_entry.encap_id;

    return rv;
}

/* Get Payload Data for entry in LEM Table*/
int get_payload_from_fid_and_mac(int unit, int fid, bcm_mac_t mac, reg_val *res){
    char cmd[100];
    int rv;
    reg_val key = {0};
    reg_val lookup = {0};
    reg_val temp_diag = {0};
    reg_val temp_key = {0};

    /*Concat key for lookup - fid and mac address*/
    key[1] += (fid << 16) + ((mac[0] & 0xff) << 8) + ((mac[1] & 0xff));
    key[0] += ((mac[2] & 0xff) << 24) + ((mac[3] & 0xff) << 16) + ((mac[4] & 0xff) << 8) + ((mac[5] & 0xff));

    /* Set LOOKUP Mode */
    lookup[0] = 1;

    /* Save KEY register */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS_KEY");
    rv = diag_reg_get(unit, cmd, temp_key);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    /* Set KEY register */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS_KEY");
    rv = diag_reg_set(unit, cmd, key);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    /* Save DIAGNOSTICS register */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS");
    rv = diag_reg_get(unit, cmd, temp_diag);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    /* Set LOOKUP register */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS");
    rv = diag_reg_set(unit, cmd, lookup);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    /* Read LOOKUP RESULT */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS_LOOKUP_RESULT");
    rv = diag_reg_get(unit, cmd, *res);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    /* Revert LOOKUP register */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS");
    rv = diag_reg_set(unit, cmd, temp_diag);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    /* Revert KEY register */
    sprintf(cmd, "PPDB_B_LARGE_EM_DIAGNOSTICS_KEY");
    rv = diag_reg_set(unit, cmd, temp_key);
    if (rv != BCM_E_NONE){
        printf("Error running command %s\n", cmd);
        return rv;
    }

    return rv;
}


/*****************************************************************************
* Function:  l2__vpn_to_vsi_profile_set
* Purpose:   Set spicific vsi profile to certain vlan
* Params:
* unit        - 
* vsi         - 
* profile_idx - 
 */
void l2__vpn_to_vsi_profile_set(int unit, uint32 vsi, uint32 profile_idx)
{ 
    int rv;
 
    rv =bcm_l2_cache_vpn_to_profile_map_set (unit, vsi, profile_idx);
    if (rv != BCM_E_NONE){
        printf("Error bcm_l2_cache_vpn_to_profile_map_set %d\n",rv);
        return rv;
    }
    return rv;
}



