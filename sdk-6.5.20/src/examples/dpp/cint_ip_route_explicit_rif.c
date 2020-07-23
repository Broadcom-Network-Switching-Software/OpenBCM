/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * set following soc properties                                                                                                                     *
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.                                                                      *
 * bcm886xx_vlan_translate_mode=1                                                                                                                   *
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 * The cint creates a routing scheme with two different Routing Interfaces(RIF) that are based on <Port, VLAN>.                                     *
 * The packet will be received on ingress port with vid 10 and will be routed via routing interface 11 to egress port                               * 
 *                                                                                                                                                  * 
 * All values are referring to default values!!! Could be changed(see below)                                                                        * 
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 *               INPUT         ____________________________________        OUTPUT                                                                   *
 *                            |      ________          _________   |                         ________                                               *
 *                         14 |     |        |        |         |  | 13                     |        |                                              *
 *      --------------------->|     |  RIF=5 |------->|  RIF=11 |  |----------------------->   HOST  |                                              *
 *                            |     |________|        |_________|  |                        |        |                                              *
 *                            |                                    |                        |________|                                              *
 *                            |                                    |                                                                                *
 *                            |____________________________________|                                                                                *
 *                                                                                                                                                  *
 * PORT X configuration:    MAC Address 00:11:00:00:01:11                                                                                           * 
 *                          VLAN 10                                                                                                                 * 
 * PORT Y configuration:    MAC Address 00:11:00:00:02:22                                                                                           * 
 *                          VLAN 200                                                                                                                * 
 * HOST configuration:      MAC Address 00:00:00:00:cd:1d                                                                                           * 
 *                          IP address 127.255.255.0x (127.255.255.0/28                                                                             * 
 * Routing Table of RIF 10: IP Address 127.255.255.0 do to port Y                                                                                   * 
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 * run:                                                                                                                                             * 
 * cd ../../../../src/examples/dpp                                                                                                                  * 
 * cint cint_ip_route_explicit_rif.c                                                                                                                * 
 * cint ../sand/utility/cint_sand_utils_global.c                                                                                                                 * 
 * cint utility/cint_utils_l2.c                                                                                                                     * 
 * cint utility/cint_utils_l3.c                                                                                                                     * 
 * cint utility/cint_utils_vlan.c                                                                                                                   * 
 * cint utility/cint_utils_port.c                                                                                                                   * 
 * cint cint_advanced_vlan_translation_mode.c   -  TODO: Remove this file once cint_utils_vlan.c will be implemented                                * 
 * cint cint_port_tpid.c                        -  TODO: Remove this file once cint_utils_port.c will be implemented                                * 
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    * 
 * ip_route_explicit_rif__start_run(int unit,  ip_route_explicit_rif_s *param) - param could be NULL if you want to use default parameters          * 
 * OR                                                                                                                                               * 
 * ip_route_explicit_rif_with_ports__start_run(int unit,  int in_port, int out_port) - if you want to change only ports(ingress/egress)             * 
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 * INPUT:                                                                                                                                           * 
 * Send packet to 14:                                                                                                                               * 
 *      ethernet header with DA 00:11:00:00:01:11                                                                                                   * 
 *      Vlan tag: vlan-id 10, vlan tag type 0x8100                                                                                                  * 
 *      ip header with any DIP of 127.255.255.0/28                                                                                                  * 
 * OUTPUT:                                                                                                                                          * 
 * Get packet on 13:                                                                                                                                * 
 *      ethernet header with DA 00:00:00:00:cd:1d, SA 00:11:00:00:02:22                                                                             * 
 *      Vlan tag: vlan-id 200, vlan tag type 0x8100                                                                                                 * 
 *      ip header with DIP 127.255.255.1                                                                                                            *
 *                                                                                                                                                  * 
 * All default values could be re-written with initialization of the global structure 'g_ip_route_explicit_rif', before calliing the main  function * 
 * In order to re-write only part of values, call 'ip_route_explicit_rif_struct_get(ip_route_explicit_rif_s)' function and re-write values,         *
 * prior calling the main function                                                                                                                  * 
 */
 
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

struct l3_rif_s{
    int sys_port;
    int vlan;
    int vrf;
    int rif;
    bcm_mac_t my_lsb_mac;
    bcm_gport_t gport;
};


/*  Main Struct  */
struct ip_route_explicit_rif_s{
    l3_rif_s  rif[NUMBER_OF_PORTS];   /* rif[0] - used for l3 'in' interface, rif[1] - used for l3 'out' interface */
    l3_ipv4_route_entry_utils_s route_entry;
    bcm_mac_t global_mac;
    bcm_mac_t next_hop_mac;
};


/* Initialization of global struct*/
/*                                                   | sys_port | vlan | vrf | rif | my_lsb_mac(only 12 my-mac LSBs  are relevant   | gport |   */
ip_route_explicit_rif_s g_ip_route_explicit_rif = { {{     14,       10,    0,    5, { 0x00, 0x00, 0x00, 0x00, 0x01, 0x11 }      , -1    },   /* configuration of ingress port*/
                                                     {     13,      200,    0,   11, { 0x00, 0x00, 0x00, 0x00, 0x02, 0x22 }      , -1    }},  /* configuration of egress port*/
                                                        /* address   |   mask   */
                                                          {0x7fffff00, 0xfffffff0}, /* routing entry*/
                                                          g_l2_global_mac_utils, /* global mac taken from cint_utils_l2.c only 36 MSB bits are relevant*/
                                                          {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d} /* next hop mac*/};



/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */
/* Initialization of main struct
 * Function allow to re-write default values 
 *
 * INPUT: 
 *   params: new values for g_ip_route_explicit_rif
 */
int ip_route_explicit_rif_init(int unit, ip_route_explicit_rif_s *param){

    if (param != NULL) {
       sal_memcpy(&g_ip_route_explicit_rif, param, sizeof(g_ip_route_explicit_rif));
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

        if (!advanced_vlan_translation_mode ) {
            return BCM_E_PARAM;
        }
    } 

    return BCM_E_NONE;
}

/* 
 * Return explicit_rif information
 */
void ip_route_explicit_rif_struct_get(int unit, ip_route_explicit_rif_s *param){

    sal_memcpy( param, &g_ip_route_explicit_rif, sizeof(g_ip_route_explicit_rif));

    return;
}

/* This function runs the main test function with given ports
 *  
 * INPUT: unit     - unit
 *        in_port  - ingress port 
 *        out_port - egress port
 */
int ip_route_explicit_rif_with_ports__start_run(int unit,  int in_port, int out_port){
    int rv;

    ip_route_explicit_rif_s param;

    ip_route_explicit_rif_struct_get(unit, &param);

    param.rif[IN_PORT].sys_port = in_port;
    param.rif[OUT_PORT].sys_port = out_port;

    return ip_route_explicit_rif__start_run(unit, &param);

}

/*
 * Main function runs the excplicit rif example 
 *  
 * Main steps of configuration: 
 *    1. Initialize test parameters
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Setting relevant TPID's for ingress/egress ports.
 *    4. Setting global MAC.
 *    5. Creating l3 routing interfaces for ingress/egress ports.
 *    6. Setting FEC and ARP entry for egress interface.
 *    7. Add routing  information.
 *    8. Setting vlan translation configuration for egress port.
 *    
 * INPUT: unit  - unit
 *        param - new values for explicit_rif(in case it's NULL default values will be used).
 */
int ip_route_explicit_rif__start_run(int unit,  ip_route_explicit_rif_s *param){
    int rv;
    int i;
    int fec = 0;
    int host;
    int encap_id = 0; 
    sand_utils_l3_arp_s arp_structure;
    sand_utils_l3_fec_s fec_structure;

    rv = ip_route_explicit_rif_init(unit, param);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_default_mode_init\n");
        return rv;
    }


    /* set VLAN domain to each port */
    for (i=0; i< NUMBER_OF_PORTS; i++) {
        rv = port__vlan_domain__set(unit, g_ip_route_explicit_rif.rif[i].sys_port, g_ip_route_explicit_rif.rif[i].sys_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in port__vlan_domain__set, port=%d, \n", g_ip_route_explicit_rif.rif[i].sys_port);
            return rv;
        }
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* set TPIDs for both ports */
        for (i=0; i< NUMBER_OF_PORTS; i++) {
            port__default_tpid__set(unit, g_ip_route_explicit_rif.rif[i].sys_port);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set with port_1\n");
                print rv;
                return rv;
            }
        }
    } else {
        rv = tpid__tpids_clear_all(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpids_clear_all\n");
            return rv;
        }

        rv = tpid__tpid_add(unit, 0x8100);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpid_add for outer_tpid\n");
            return rv;
        }

        rv = tpid__tpid_add(unit, 0x9100);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpid_add for inner_tpid\n");
            return rv;
        }
    }

    rv = bcm_vlan_gport_add(unit, 10, g_ip_route_explicit_rif.rif[IN_PORT].sys_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* set global MAC(36 MSB bits)*/
        l2__mymac_properties_s mymac_properties;
        sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
        mymac_properties.mymac_type = l2__mymac_type_global_msb;
        sal_memcpy(mymac_properties.mymac, g_ip_route_explicit_rif.global_mac, 6);
        
        rv = l2__mymac__set(unit, &mymac_properties);
        if (rv != BCM_E_NONE) {
             printf("Error, setting global my-MAC");
        }
    }

    /*  create router interface for both ports */
    for (i=0; i< NUMBER_OF_PORTS; i++) {
        rv = ip_route_explicit_rif__l3_intf_create(unit, i); 
        if (rv != BCM_E_NONE) {
            printf("Error, creating interface: port=%d,  unit %d \n", g_ip_route_explicit_rif.rif[i].sys_port, unit);
        }
    }

    /* create egress object */
    /* Sets FEC and ARP entry for the egress L3 interface*/
    l3_egress_s l3_egress;
    sand_utils_l3_egress_s_init(unit, 0, &l3_egress);
    l3_egress.out_gport = g_ip_route_explicit_rif.rif[OUT_PORT].sys_port;
    l3_egress.out_tunnel_or_rif = g_ip_route_explicit_rif.rif[OUT_PORT].rif;
    l3_egress.next_hop_mac_addr = g_ip_route_explicit_rif.next_hop_mac;
    l3_egress.arp_encap_id = encap_id;
    l3_egress.fec_id = fec;

    rv = sand_utils_l3_egress_create(unit ,&l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
    }
    encap_id = l3_egress.arp_encap_id;
    fec = l3_egress.fec_id;

    /* add route point to FEC */
    rv = l3__ipv4_route__add(unit, g_ip_route_explicit_rif.route_entry, g_ip_route_explicit_rif.rif[IN_PORT].vrf, fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, adding route, in_sysport=%d in unit %d, \n", l3_intrf[IN_PORT].sys_port, unit);
    }

    /* set port translation info*/
    rv = vlan__eve_default__set(unit, g_ip_route_explicit_rif.rif[OUT_PORT].gport, g_ip_route_explicit_rif.rif[OUT_PORT].vlan, 0, bcmVlanActionAdd, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__eve_default__set in unit %d sys_port %d \n", unit, g_ip_route_explicit_rif.rif[OUT_PORT].sys_port);
    }
    return rv;
}


/*
 * create l3 interface - ingress/egress 
 *  
 * Main steps of configuration: 
 *  1. Set my mac (12 LSB bits).
 *  2. Create L2 logical port according <port,vlan>
 *  3. Create L3 Routing Interface
 *  
 *  INPUT: unit  -  unit
 *         rif index - index of RIF in explicit_rif(according to this index relevant values(port, vlan, rif, my_mac) will be taken.
 *  OUTPUT: gport - set L2 logical interface(gport field in explicit_rif)
 */
int ip_route_explicit_rif__l3_intf_create(int unit, int rif_index) {

    int rc;
    create_l3_intf_s intf;
    sand_utils_l3_port_s l3_port_structure;

    if (!is_device_or_above(unit, JERICHO2)) {
        /* Set my_mac (12 LSB bits)*/
        l2__mymac_properties_s mymac_properties;
        sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
        mymac_properties.mymac_type = l2__mymac_type_ipv4_vsi;
        sal_memcpy(mymac_properties.mymac, g_ip_route_explicit_rif.rif[rif_index].my_lsb_mac, 6);
        mymac_properties.vsi = g_ip_route_explicit_rif.rif[rif_index].rif;

        rc = l2__mymac__set(unit, &mymac_properties);
        if (rc != BCM_E_NONE) {
              printf("Fail  l2__mymac__set: port(%d) vlan(%d)", g_ip_route_explicit_rif.rif[rif_index].sys_port, g_ip_route_explicit_rif.rif[rif_index].vlan);
              return rc;
        }

        /* Create l2 logical interface*/
        rc = l2__port_vlan__create(unit,0,
                                         g_ip_route_explicit_rif.rif[rif_index].sys_port, 
                                         g_ip_route_explicit_rif.rif[rif_index].vlan, 
                                         g_ip_route_explicit_rif.rif[rif_index].rif, 
                                         &g_ip_route_explicit_rif.rif[rif_index].gport);
        if (rc != BCM_E_NONE) {
              printf("Fail  vlan__port_vlan__create: port(%d) vlan(%d)", g_ip_route_explicit_rif.rif[rif_index].sys_port, g_ip_route_explicit_rif.rif[rif_index].vlan);
              return rc;
        }
    } else {
        sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, g_ip_route_explicit_rif.rif[rif_index].sys_port, g_ip_route_explicit_rif.rif[rif_index].rif);
        rc = sand_utils_l3_port_set(unit, &l3_port_structure);
        if (rc != BCM_E_NONE)
        {
            printf("Error, access_port sand_utils_l3_port_set\n");
            return rc;
        }
        g_ip_route_explicit_rif.rif[rif_index].gport = l3_port_structure.eg_vlan_port_id;
    }
    
    /* Create L3 Routing Interface*/
    intf.my_global_mac = g_ip_route_explicit_rif.global_mac;
    intf.my_lsb_mac =  g_ip_route_explicit_rif.rif[rif_index].my_lsb_mac;
    intf.vsi = g_ip_route_explicit_rif.rif[rif_index].rif;
    rc = l3__intf_rif__create(unit,&intf);
    if (rc != BCM_E_NONE) {
          printf("Fail  l3__intf_rif_with_id__create: port(%d) ", g_ip_route_explicit_rif.rif[rif_index].sys_port);
          return rc;
    }

    return rc;
}


