/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_protection.c
 * Purpose: Example of mpls protection. 
 */

/* 
 *  
 * Configuration:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit;  
 * cint ../../../../src/examples/dpp/utility/cint_utils_port.c   
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c                      
 * cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c                     
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../../../src/examples/dpp/cint_port_tpid.c                     
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dpp/cint_mpls_protection.c                                        
 * cint                                                                   
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = mpls_protection_main(unit,in_port,out_port);
 * print rv; 
 * 
 * To change the failover instance:
 * rv = mpls_protection_failover_set(unit,0);
 * print tv;
 * 
 * 
 *  Mpls protection scenarios configured in this cint:
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  Mpls ingress protection
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  This scenario is configured with mpls_protection_one_plus_one_example():
 *  1) Create a swap entry by which the incoming packet will be forwarded.
 *  2) Create a failover instance.
 *  3) Create a backup termination entry. A packet holding an inner label matching this entry will be dropped.
 *  4) Create a working termination entry. A packet holding an inner label matching this entry will be mpls terminated and forwarded
 *     according to the swap label.
 *  5) Change the failover instance with mpls_protection_failover_set(). The backup entry will become working and vice versa.
 * 
 *  Traffic:
 * 
 *  Send traffic according to the above scenario. 
 * 
 *  
 *  ##############################################################################################
 *  Sending packet from port == 200 on working path:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:6100||Label:5000||01:14|01:13|0x8100|30 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:60    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:8000||01:14|01:13|0x8100|30 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:59    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *  
 * ############################################################################################### 
 *  Sending packet from port == 200 on backup path:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:6000||Label:5000||01:14|01:13|0x8100|30 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:60    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving : None. Filtered due to protection configuratoin   <------------
 *  ##############################################################################################
 *  
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  Change failover instance with mpls_protection_failover_set().
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  
 *  
 *  ##############################################################################################
 *  Sending packet from port == 200 on new working path:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:6000||Label:5000||01:14|01:13|0x8100|30 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:60    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:8000||01:14|01:13|0x8100|30 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:59    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  ##############################################################################################
 *  
 *  ##############################################################################################
 *  Sending packet from port == 200 on new backup path:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:6100||Label:5000||01:14|01:13|0x8100|30 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:60    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  
 *  Receiving : None. Filtered due to protection configuratoin   <------------
 *  ##############################################################################################
 */

/* Will hold all ports of the application */
int mpls_protection_port_array[2];

struct cint_mpls_protection_info_s {
    int port1; /* port1*/
    int port2; /* port2 */
    int vsi;   /* vsi for this application */  
    int tpid;     /* tpid value for all vlan tags in the application*/ 
    int vport_id1; /* vlan port id of first ac for this application */ 
    int vlan1; /* First vlan for this application */
    int vlan2; /* Second vlan for this application */
    bcm_mpls_label_t one_plus_one_swapped_label; /* Label to be swapped in one plus one scenario */
    bcm_mpls_label_t one_plus_one_egress_label; /* Label that swaps one_plus_one_swapped_label */
    bcm_mpls_label_t one_plus_one_termination_label1; /* Label to be terminated in one plus one scenario, if belongs to working path*/
    bcm_mpls_label_t one_plus_one_termination_label2; /* Label to be terminated in one plus one scenario, if belongs to working path*/
    int one_plus_one_failover_id; /* Failover id of one plus one scenario */
    bcm_mac_t global_mac; /* global mac for this application */
    bcm_mac_t my_mac_lsb; /* my mac lsb for this application */
    bcm_mac_t payload_mac; /* my mac for payload */
    int fec; /* FEC for next hop information */
};

cint_mpls_protection_info_s cint_mpls_protection_info = 
/* ports :
   port1   |   port2  */
    {200,      201,
/*   vsi  */
     20 , 
/*   tpid  */
     0x8100, 
/*   vport_id1  */ 
       0,  
/*    vlan1  */
     20,
/*    vlan 2 */
     30,
/* Application's labels:
   one_plus_one_swapped_label  |  one_plus_one_egress_label  |  one_plus_one_termination_label1  |  one_plus_one_termination_label2*/
              5000,                          8000,                           6000,                                6100, 
/* One plus one failover id */
            7,
/*               global_mac              |            my_mac_lsb              |                payload_mac */              
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x11},   {0x00, 0x00, 0x00, 0x00, 0x01, 0x12},    
};


void mpls_protection_info_init(int unit,
                    int port1,
                    int port2
                    ) {
    mpls_protection_port_array[0] = cint_mpls_protection_info.port1 = port1; 
    mpls_protection_port_array[1] = cint_mpls_protection_info.port2 = port2; 

}

/* Main function for mpls protection scnenarios */
int mpls_protection_main(int unit, int port1, int port2){

    int rv = BCM_E_NONE;
    int mpls_termination_label_index_enable;
    int mpls_termination_label_index_database_mode;

    mpls_protection_info_init(unit, port1, port2);

    /* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    mpls_termination_label_index_database_mode = soc_property_get(unit , spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);

    if ((mpls_termination_label_index_enable == 1) &&
        ((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {

        rv = bcm_vlan_control_port_set(unit, port1, bcmVlanPortDoubleLookupEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_vlan_control_port_set(unit, port2, bcmVlanPortDoubleLookupEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* Configure vsi for this application */
    rv = bcm_vswitch_create_with_id(unit, cint_mpls_protection_info.vsi); 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vswitch_create_with_id with vid = %d\n", cint_mpls_protection_info.vsi);
        return rv;
    }

    /* Configure port properties for this application */
    rv = mpls_protection_configure_port_properties(unit, mpls_protection_port_array, 2); 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls_protection_configure_port_properties\n");
        return rv;
    }

    /* Configure L2 information */
    rv = mpls_protection_create_l2_interfaces(unit); 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls_protection_create_l2_interfaces\n");
        return rv;
    }


           
    /* Configure L3information (router interfaces) */
    rv = mpls_protection_create_l3_interfaces(unit);  
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls_protection_create_l3_interfaces\n");
        return rv;
    }

    /* Configure a one plus one protection scenario */
    rv = mpls_protection_one_plus_one_example(unit);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls_protection_one_plus_one_example\n");
        return rv;
    }

    return rv;
}


/* Configure properties for ports of the application */
int mpls_protection_configure_port_properties(int unit, int *port_array, int nof_ports){

    int rv = BCM_E_NONE;
    int i;
    bcm_pbmp_t pbmp;

    for (i=0; i< nof_ports; i++) {
        /* set VLAN domain to each port */
        rv = port__vlan_domain__set(unit, port_array[i], port_array[i]); 
        if (rv != BCM_E_NONE) {
            printf("Error, in port__vlan_domain__set, port=%d, \n", port_array[i]);
            return rv;
        }

        /* Set outer and inner tpid */
        if (!is_device_or_above(unit, JERICHO2)) {
            rv = port__tpid__set(unit, port_array[i], cint_mpls_protection_info.tpid, cint_mpls_protection_info.tpid); 
            if (rv != BCM_E_NONE) {
                printf("Error, in port__tpid__set, port=%d, \n",  port_array[i]);
                return rv;
            }
        }

        rv = bcm_vlan_gport_add(unit, cint_mpls_protection_info.vsi, port_array[i], BCM_VLAN_GPORT_ADD_INGRESS_ONLY); 
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan);
          return rv;
        }
    }

    return rv;
}

/* Create attachment circuits for this application and (possibly) other l2 information */
int mpls_protection_create_l2_interfaces(int unit){

    int rv = BCM_E_NONE;
    int flags = 0;
    int vsi = is_device_or_above(unit, JERICHO2)? 0 : cint_mpls_protection_info.vsi;

    /* Create a vlan port*/
    rv = l2__port_vlan__create(unit, flags,  cint_mpls_protection_info.port1, cint_mpls_protection_info.vlan1, vsi, &cint_mpls_protection_info.vport_id1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__port_vlan__create\n");
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        rv = bcm_vswitch_port_add(unit, cint_mpls_protection_info.vsi, cint_mpls_protection_info.vport_id1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_vswitch_port_add\n");
            return rv;
        }
    }

    return rv;
}

/* Configure L3information (router interfaces) */
int mpls_protection_create_l3_interfaces(int unit){

    int rv = BCM_E_NONE;
    create_l3_intf_s l3_intf;
    create_l3_egress_s l3_egress;

    l3_intf.my_lsb_mac = cint_mpls_protection_info.my_mac_lsb;
    l3_intf.my_global_mac = cint_mpls_protection_info.global_mac;
    l3_intf.rif = cint_mpls_protection_info.vsi;
    l3_intf.vsi = cint_mpls_protection_info.vsi;
    l3_intf.rpf_valid = 0;

    /* create rif */
    rv = l3__intf_rif__create(unit, &l3_intf); 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__intf_rif__create\n");
        return rv;
    }
    
   /* configure fec for next hop information */
    l3_egress.next_hop_mac_addr = cint_mpls_protection_info.payload_mac;
    l3_egress.vlan = cint_mpls_protection_info.vsi; /* VSI is located in ARP.VSI, in LSR , FEC points directly to ARP */
    l3_egress.out_gport = cint_mpls_protection_info.port2;
    rv = l3__egress__create(unit, &l3_egress);/* 3.4, 7.6 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress__create\n");
        return rv;
    }

    cint_mpls_protection_info.fec = l3_egress.fec_id;;

    return rv;
}

/* Configure a one plus one scenario */
int mpls_protection_one_plus_one_example(int unit){

    int rv = BCM_E_NONE, flags = 0;
    mpls__ingress_tunnel_utils_s mpls_swapped_tunnel_properties;
    mpls__ingress_tunnel_utils_s mpls_terminated_tunnel1_properties;
    mpls__ingress_tunnel_utils_s mpls_terminated_tunnel2_properties;

    int mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    int mpls_termination_label_index_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);
    int val;

    /* Add a swap action */
    mpls_swapped_tunnel_properties.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    mpls_swapped_tunnel_properties.swap_label = cint_mpls_protection_info.one_plus_one_egress_label;
    mpls_swapped_tunnel_properties.label = cint_mpls_protection_info.one_plus_one_swapped_label;
    if (mpls_termination_label_index_enable) {
        val = cint_mpls_protection_info.one_plus_one_swapped_label;
        BCM_MPLS_INDEXED_LABEL_SET(mpls_swapped_tunnel_properties.label, val, 1);   
    }
    mpls_swapped_tunnel_properties.flags = flags;
    mpls_swapped_tunnel_properties.fec = cint_mpls_protection_info.fec;
    rv = mpls__add_switch_entry(unit, &mpls_swapped_tunnel_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    /* Create a failover instance */
    rv = bcm_failover_create(unit, BCM_FAILOVER_INGRESS, &cint_mpls_protection_info.one_plus_one_failover_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_failover_create\n");
        return rv;
    }

    /* Create a termination entry for the backup path */
    mpls_terminated_tunnel1_properties.action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_terminated_tunnel1_properties.label = cint_mpls_protection_info.one_plus_one_termination_label1;
    if (mpls_termination_label_index_enable) {
        val = cint_mpls_protection_info.one_plus_one_termination_label1;
        if ((mpls_termination_label_index_database_mode >= 20) && (mpls_termination_label_index_database_mode <= 23))
        {
            BCM_MPLS_INDEXED_LABEL_SET(mpls_terminated_tunnel1_properties.label, val, 1);
        }
        else {
            BCM_MPLS_INDEXED_LABEL_SET(mpls_terminated_tunnel1_properties.label, val, 2);
        }
    }
    mpls_terminated_tunnel1_properties.flags = flags;
    mpls_terminated_tunnel1_properties.failover_id = cint_mpls_protection_info.one_plus_one_failover_id;
    mpls_terminated_tunnel1_properties.failover_tunnel_id = 0; /* For backup, this value should be 0 */
    rv = mpls__add_switch_entry(unit, &mpls_terminated_tunnel1_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    if (!(mpls_termination_label_index_database_mode >= 20 && mpls_termination_label_index_database_mode <= 23) && !is_device_or_above(unit, JERICHO2)) {
      if (mpls_termination_label_index_enable) {
        val = cint_mpls_protection_info.one_plus_one_termination_label1;  
        BCM_MPLS_INDEXED_LABEL_SET(mpls_terminated_tunnel1_properties.label, val, 1);
      }
      rv = mpls__add_switch_entry(unit, &mpls_terminated_tunnel1_properties);
      if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
      }
    }

    /* Create a termination entry for the primary path */
    mpls_terminated_tunnel2_properties.action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_terminated_tunnel2_properties.label = cint_mpls_protection_info.one_plus_one_termination_label2;
    if (mpls_termination_label_index_enable) {
        val = cint_mpls_protection_info.one_plus_one_termination_label2;
        BCM_MPLS_INDEXED_LABEL_SET(mpls_terminated_tunnel2_properties.label, val, 1); 
    }
    mpls_terminated_tunnel2_properties.flags = flags;
    mpls_terminated_tunnel2_properties.failover_id = cint_mpls_protection_info.one_plus_one_failover_id;
    mpls_terminated_tunnel2_properties.failover_tunnel_id = 1; /* For primary, this value should be 1 */
    rv = mpls__add_switch_entry(unit, &mpls_terminated_tunnel2_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    if (!(mpls_termination_label_index_database_mode >= 20 && mpls_termination_label_index_database_mode <= 23) && !is_device_or_above(unit, JERICHO2))
    {
      if (mpls_termination_label_index_enable) {
        val = cint_mpls_protection_info.one_plus_one_termination_label2;   
        BCM_MPLS_INDEXED_LABEL_SET(mpls_terminated_tunnel2_properties.label, val, 2); 
      }
      rv = mpls__add_switch_entry(unit, &mpls_terminated_tunnel2_properties);
      if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
      }
    }

    return rv;
}

/* Set the value for the working path */
int mpls_protection_failover_set(int unit, int enable){

    int rv = BCM_E_NONE;

    rv = bcm_failover_set(unit, cint_mpls_protection_info.one_plus_one_failover_id, enable);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", cint_mpls_protection_info.one_plus_one_failover_id);
        return rv;
    }
    
    return rv;
}

