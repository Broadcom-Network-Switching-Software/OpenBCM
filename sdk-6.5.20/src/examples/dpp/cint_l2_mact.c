/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Mact Management~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_l2_mact.c
 * Purpose: Example demonstrates how to manipulate and manage the MACT.
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
 * Run Script:
 *      BCM> examples/dpp/utility/cint_utils_multicast.c
 *      BCM> examples/dpp/cint_l2_mact.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = l2_mact_run(unit);
 */

/* set to one to show informative prints */
int verbose = 1;

/* mac address used as base for MAC addresses generation */
int _mac_address_indx = 0x12f;
uint8 _mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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

/* attributes to enable user to control the application*/
l2_mac_table_info_s l2_mact_table_info1;

void
l2_mac_table_info_init(int unit) {
    bcm_port_config_t port_config;
    bcm_pbmp_t ports;
    int port = 4;

    bcm_port_config_get(unit, &port_config);

    BCM_PBMP_ASSIGN(ports, port_config.e);

    BCM_PBMP_ASSIGN(ports, port_config.e);
    if (BCM_PBMP_MEMBER(ports, 14))
    {
      port = 14;
    }
    /* two vlans mac addresses will be inserted to */
    l2_mact_table_info1.vid1 = 1;
    l2_mact_table_info1.vid2 = 3;
    /* number of mac entris to be added */
    l2_mact_table_info1.nof_mac_ddresses = 5;
    /* unicast destination port to forward packets to */
    l2_mact_table_info1.dest_port_1 =  1;
    l2_mact_table_info1.dest_port_2 =  port;
    /* multicast group to send packets to, the multicast will include the above unicast destination */
    l2_mact_table_info1.mc_id = 4321;
    l2_mact_table_info1.aging_time = 400; /* 400 seconds */
}


/*  Setup MAC forwading
 *  dest_type = 0 for GPort
 *              1 for MC Group
 */
int l2_entry_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id){
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

/*
 * utility to open vlan with on port
 */
int l2_open_vlan_with_port(int unit, int vlan, int port){
   int rc;
      rc = bcm_vlan_create(unit, vlan);
      if (rc != BCM_E_NONE) {
        printf("fail open vlan(%d)\n", vlan);
        printf("continue..\n");
      }
  rc = bcm_vlan_gport_add(unit, vlan, port, 0);
  if (rc != BCM_E_NONE) {
      printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
    return rc;
  }
  return rc;
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
    int  indx;

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

/* Application Sequence:
 *  -   Enable aging and set age time to 400 seconds
 *  -   Add number of static entries to MAC table
 *      (some to unicast destination and others to multicast group)
 */
int l2_mact_run(int unit){
    int rv = BCM_E_NONE;
    bcm_mac_t mac;

    /* fill mac table with initial info */
    l2_mac_table_info_init(unit);

    /* set aging */
    rv = bcm_l2_age_timer_set(unit,l2_mact_table_info1.aging_time);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_mact_table_info1 \n");
        return rv;
    }

    /* insert some entries */
    rv = l2_mac_table_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_mac_table_init \n");
        return rv;
    }

    /* demonistrate addition of some MC entnries */
    /* open mc */
    int gports[2], cuds[2]={0};
    gports[0] = l2_mact_table_info1.dest_port_1;
    gports[1] = l2_mact_table_info1.dest_port_2;
    rv = multicast__open_ingress_mc_group_with_local_ports(unit, l2_mact_table_info1.mc_id, gports, cuds, 2, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
        return rv;
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

/* get and print the content of entry according to given key */
int entry_get(int unit, bcm_mac_t mac, bcm_vlan_t vid){
    bcm_l2_addr_t my_addr2;
    int rv;
    rv = bcm_l2_addr_get(unit, mac, vid, &my_addr2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_get \n");
        return rv;
    }
    print my_addr2;
    print rv;
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
};

/* call back to print mac table entry */
int call_back_get_block(int unit, bcm_l2_addr_t *l2e, void* ud) {
    l2_addr_print_entry(l2e);
    return BCM_E_NONE;
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




int limit_entries_num_per_fid(int unit,int fid,int limit_){
    int rv = BCM_E_NONE;
    bcm_l2_learn_limit_t limit;
    bcm_l2_learn_limit_t_init(&limit);

    limit.flags = BCM_L2_LEARN_LIMIT_VLAN;
    limit.vlan = fid;
    limit.limit = limit_;

    rv = bcm_l2_learn_limit_set(unit, &limit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_set\n");
        return rv;
    }

    return rv;
}



int validate_mac_fid_limit_interrupt(int unit,int is_up,int is_sim){
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    type.action = bcmSwitchEventRead;
    type.index = 0x0; /* block index PPDB_B */

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.event_id = 0;

    /* MACTMNGMNTREQFIDEXCEEDLIMIT */
    if(info.device == 0x8675 || info.device == 0x8375) { /* Jericho or QMX */
        type.event_id = 1424;
    } else if(info.device == 0x8680 ){ /* Jericho Plus */
        type.event_id = 1510;
    } else if(info.device == 0x8470 ){ /* QAX */
        type.event_id = 1538;
    } else if(info.device == 0x8270 ){ /* QUX */
        type.event_id = 1336;
    } else if(info.device == 0x8690 ){ /* Jericho2 */
        type.event_id = 1653;
    } else if(info.device == 0x8800 ){ /* J2C*/
        type.event_id = 1197;
    } else if ((info.device == 0x8480) || (info.device == 0x8485) || ((info.device & 0xfff0) == 0x8280)) { /* Q2A */
        type.event_id = 1087;
    }

    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant chnage in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
printf("\n its : %x\n", info.device);
        return -1;
    }

    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,type,&interrupt_val);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_event_control_get\n");
        return rv;
    }

    /*
     *Skip validating interrupt on simulator
     */
    if(!is_sim){
        if(is_up != interrupt_val){
            printf("Error, interrupt value is not as expected: %d, value read is: %d\n", is_up,interrupt_val);
            return -1;
        }
    }

    return rv;
}


int mac_fid_limit_interrupt_clear(int unit){

    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    type.action = bcmSwitchEventClear;
    type.index =  0x0; /* block index PPDB_B */

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.event_id = 0;

    /* MACTMNGMNTREQFIDEXCEEDLIMIT */
    if(info.device == 0x8675 || info.device == 0x8375) { /* Jericho or QMX */
        type.event_id = 1424;
    } else if(info.device == 0x8680 ){ /* Jericho Plus */
        type.event_id = 1510;
    } else if(info.device == 0x8470 ){ /* QAX */
        type.event_id = 1538;
    } else if(info.device == 0x8270 ){ /* QUX */
        type.event_id = 1336;
    } else if(info.device == 0x8690 ){ /* Jericho2 */
        type.event_id = 1653;
    } else if(info.device == 0x8800 ){ /* J2C*/
        type.event_id = 1197;
    } else if ((info.device == 0x8480) || (info.device == 0x8485) || ((info.device & 0xfff0) == 0x8280)) { /* Q2A */
        type.event_id = 1087;
    }

    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant chnage in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
printf("\n its : %x\n", info.device);
        return -1;
    }

    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,type,0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_event_control_set in mac_fid_limit_interrupt_clear\n");
        return rv;
    }

    return rv;

}

/** Check if MACT_LARGE_EM_LELA_EXCEED_LIMIT_FID is set */
int validate_mac_lel_fid_limit_interrupt(int unit,int is_up,int is_sim){
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    type.action = bcmSwitchEventRead;
    type.index = 0x0; /* block index PPDB_B */

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.event_id = 0;
    
    if((info.device & 0xfff0) == 0x8690 ){ /* Jericho2 */
        type.event_id = 1653;
    } else if((info.device & 0xffd0) == 0x8800 ){ /* J2C*/
        type.event_id = 1207;
    } else if (((info.device & 0xfff0) == 0x8480) || ((info.device & 0xfff0) == 0x8280)) { /* Q2A */
        type.event_id = 1097;
    }


    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant chnage in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
        printf("\n its : %x\n", info.device);
        return -1;
    }

    rv = bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,type,&interrupt_val);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_event_control_get\n");
        return rv;
    }

    /*
     *Skip validating interrupt on simulator
     */
    if(!is_sim){
        if(is_up != interrupt_val){
            printf("Error, interrupt value is not as expected: %d, value read is: %d\n", is_up,interrupt_val);
            return -1;
        }
    }

    return rv;
}

/** Clear MACT_LARGE_EM_LELA_EXCEED_LIMIT_FID */
int mac_lel_fid_limit_interrupt_clear(int unit){

    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    type.action = bcmSwitchEventClear;
    type.index =  0x0; /* block index PPDB_B */

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.event_id = 0;

    
    if((info.device & 0xfff0) == 0x8690 ){ /* Jericho2 */
        type.event_id = 1653;
    } else if((info.device & 0xffd0) == 0x8800 ){ /* J2C*/
        type.event_id = 1207;
    } else if (((info.device & 0xfff0) == 0x8480) || ((info.device & 0xfff0) == 0x8280)) { /* Q2A */
        type.event_id = 1097;
    }


    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant chnage in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
        printf("\n its : %x\n", info.device);
        return -1;
    }

    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,type,0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_event_control_set in mac_fid_limit_interrupt_clear\n");
        return rv;
    }

    return rv;

}


