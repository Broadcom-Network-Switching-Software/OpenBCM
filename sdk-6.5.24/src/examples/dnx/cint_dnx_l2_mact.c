/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Mact Management~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * File: cint_dnx_l2_mact.c
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
 * Make sure interrupt is raised on time Learn mac by sending traffic to exceed the limit
 * Run Script:
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/cint_dnx_l2_mact.c
 * cint
 * limit_entries_num_per_fid(0,0xf,3);
 * mac_lel_fid_limit_interrupt_clear(0);
 * validate_mac_lel_fid_limit_interrupt(0,0,0);
 * exit;
 *
 * tx 1 psrc=202 data=0x0000000000110000000000338100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 0
 * Data: 0x01a400650000000003e05ff2f87eb7f53e004000001e00000007fa00030000040be5000000000000110000000000338100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * tx 1 psrc=202 data=0x0000000000110000000000348100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 0
 * Data: 0x01a400650000000003e0e34f397f9ab048004000001e00000007fa00030000040be5000000000000110000000000348100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * tx 1 psrc=202 data=0x0000000000110000000000358100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 0
 * Data: 0x01a400650000000003e070eb8d80741968004000001e00000007fa00030000040be5000000000000110000000000358100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * cint
 * validate_mac_lel_fid_limit_interrupt(0,0,0);
 * exit;
 *
 * tx 1 psrc=202 data=0x0000000000110000000000368100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 202, Destination port: 0
 * Data: 0x01a400650000000003e0f9b8ea817223b6004000001e00000007fa00030000040be5000000000000110000000000368100000f0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * cint
 * validate_mac_lel_fid_limit_interrupt(0,1,0);
 * mac_lel_fid_limit_interrupt_clear(0);
 * validate_mac_lel_fid_limit_interrupt(0,0,0);
 * validate_mac_lel_fid_limit_interrupt(0,0,0);
 * limit_entries_num_per_fid(0,0xf,-1);
 * exit;
 *
 * Make sure interrupt is raised on time Call bcm_l2_addr_add() to exceed the limit
 * cint ../../src/./examples/dnx/cint_dnx_l2_mact.c
 * cint
 * limit_entries_num_per_fid(0,0xf,3);
 * mac_fid_limit_interrupt_clear(0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * validate_mac_fid_limit_interrupt(0,1,0);
 * mac_fid_limit_interrupt_clear(0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * limit_entries_num_per_fid(0,0xf,3);
 * mac_fid_limit_interrupt_clear(0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * validate_mac_fid_limit_interrupt(0,1,0);
 * mac_fid_limit_interrupt_clear(0);
 * validate_mac_fid_limit_interrupt(0,0,0);
 * validate_mac_fid_limit_interrupt(0,0,0); 
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
void l2_mac_table_info_init(int unit) {
    bcm_port_config_t port_config;
    bcm_pbmp_t ports;
    int port = 4;

    bcm_port_config_get(unit, &port_config);
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

/* depeneding on: l2_mact_table_info1
 * add Mac addressed to MAC table:
 * 'nof_mac_ddresses' mac adresses is added to vid1 and vid2,
 * points to address dest_port_1/2 (alternatively)
 */
int l2_mac_table_init(int unit) {
    bcm_mac_t mac;
    int port1,port2;
    int indx;
    bcm_l2_addr_t l2_addr;

    for(indx = 0;  indx < l2_mact_table_info1.nof_mac_ddresses; ++indx) {
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

		bcm_l2_addr_t_init(&l2_addr, mac, l2_mact_table_info1.vid1);
        /* add static entry */
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = port1;
		rv = bcm_l2_addr_add(unit,&l2_addr);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_l2_addr_add \n");
			return rv;
		}

		bcm_l2_addr_t_init(&l2_addr, mac, l2_mact_table_info1.vid2);
        /* add static entry */
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = port2;
		rv = bcm_l2_addr_add(unit,&l2_addr);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_l2_addr_add \n");
			return rv;
		}
    }
    return BCM_E_NONE;
}

/* Application Sequence:
 *  -   Enable aging and set age time to 400 seconds
 *  -   Add number of static entries to MAC table
 *      (some to unicast destination and others to multicast group)
 */
int l2_mact_run(int unit) {
    int rv = BCM_E_NONE;
    bcm_mac_t mac;
    bcm_l2_addr_t l2_addr;

    /* fill mac table with initial info */
    l2_mac_table_info_init(unit);

    /* set aging */
    rv = bcm_l2_age_timer_set(unit,l2_mact_table_info1.aging_time);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_age_timer_set \n");
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
    bcm_l2_addr_t_init(&l2_addr, mac, l2_mact_table_info1.vid1);
    /* add multicast entry */
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = l2_mact_table_info1.mc_id;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    generat_mac_address(mac);
    bcm_l2_addr_t_init(&l2_addr, mac, l2_mact_table_info1.vid2);
    /* add multicast entry */
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = l2_mact_table_info1.mc_id;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    return rv;
}

/* utilities */
int generat_mac_address(unsigned char* mac) {
    int indx;
    int tmp_mac = _mac_address_indx;
    for(indx = 5; indx >= 0; --indx)
    {
        _mac_address[indx] = tmp_mac % 0x100;
        tmp_mac /= 0x100;
    }
    sal_memcpy(mac, _mac_address, 6);
    ++_mac_address_indx;
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

int limit_entries_num_per_fid(int unit,int fid,int limit_) {
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

int validate_mac_fid_limit_interrupt(int unit,int is_up,int is_sim) {
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.action = bcmSwitchEventRead;
    type.index = 0x0; /* block index PPDB_B */
    type.event_id = 0;
    /* MACTMNGMNTREQFIDEXCEEDLIMIT */
    type.event_id = *(dnxc_data_get(unit, "l2", "general", "mact_mngmnt_fid_exceed_limit_int", NULL));
    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant change in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
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

int mac_fid_limit_interrupt_clear(int unit) {
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.action = bcmSwitchEventClear;
    type.index =  0x0; /* block index PPDB_B */
    type.event_id = 0;
    /* MACTMNGMNTREQFIDEXCEEDLIMIT */
    type.event_id = *(dnxc_data_get(unit, "l2", "general", "mact_mngmnt_fid_exceed_limit_int", NULL));
    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant change in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
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

/* Check if MACT_LARGE_EM_LELA_EXCEED_LIMIT_FID is set */
int validate_mac_lel_fid_limit_interrupt(int unit,int is_up,int is_sim) {
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.action = bcmSwitchEventRead;
    type.index = 0x0; /* block index PPDB_B */
    type.event_id = *(dnxc_data_get(unit, "l2", "general", "mact_lela_fid_exceed_limit_int", NULL));
    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant change in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
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

/* Clear MACT_LARGE_EM_LELA_EXCEED_LIMIT_FID */
int mac_lel_fid_limit_interrupt_clear(int unit) {
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t type;
    unsigned int interrupt_val = 0;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    type.action = bcmSwitchEventClear;
    type.index =  0x0; /* block index PPDB_B */
    type.event_id = *(dnxc_data_get(unit, "l2", "general", "mact_lela_fid_exceed_limit_int", NULL));
    if(type.event_id == 0) { /* a new device should be added */
        printf("please make the relevant change in SDK for the device to support MACTMNGMNTREQFIDEXCEEDLIMIT!\n");
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