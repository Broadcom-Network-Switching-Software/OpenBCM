/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * 
 * **************************************************************************************************************************************************
 * This CINT script is an example how to setup route over ac function in PON application downstream.
 *  
 * Network diagram
 *  
 *  
 *                          --------------------------------------
 *                         |                                      |
 *                         |                                      | 
 *                         |               ROUTER                 |
 *                         |                                      | 
 *                         |   ---------               ------     |
 *                         |  |  VSIx   |             | VSIy |    | 
 *                         |   ---------               ------     |
 *                         |    |     |                  |        |
 *                         |    | RIF1|              RIF2|        |
 *                         |____|_____|__________________|________|
 *                              |                              |
 *                              |                              |
 *                          host1 (PON  port)         host3 (NNI port)
 *                         <10,100>                     <30>
 * 
 *                         <x,y> - x is tunnel ID     <y> - y is outer VID   
 *                                    y is outer VID
 * 
 * Default values:
 * VLAN values are in diagramm
 * Host1   -   MAC: 00:00:11:11:11:11
 *                IP:  17.17.17.17
 * Host3   -   MAC: 00:00:33:33:33:33
 *                IP:  51.51.51.51
 * PORT1(RIF1) - MAC: 00:11:00:00:01:11
 * PORT2(RIF2) - MAC: 00:11:00:00:02:22
 * 
 * run:
 * cd   ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_utils_port.c
 * cint utility/cint_utils_l2.c
 * cint utility/cint_utils_l3.c
 * cint utility/cint_utils_vlan.c
 * cint cint_port_tpid.c
 * cint pon/cint_route_over_ac.c
 * cint pon/cint_pon_field_router_over_ac.c
 * cint
 * print route_over_ac_with_ports__start_run(unit,pon_port,nni_port);
 * print field_group_router_over_ac_pselect_setup(unit, nni_port);
 * print field_group_router_over_ac_setup(unit, group_pri);
 * 
 * All default values could be re-written with initialization of the global structure 'g_route_over_ac', before calliing the main  function 
 * In order to re-write only part of values, call 'route_over_ac_struct_get(route_over_ac_s)' function and re-write values, 
 * prior calling the main function
 * 
 * ##############    Scenario #1    #######################
 *
 *  Traffic from Host3 (NNI port) to Host1 (PON port)
 *  Testing routing over ac between two AC's that belong to different VSI's
 *  
 *    Send:                                           
 *              ---------------------------------------------   
 *        eth: |    DA     |     SA      |   VLAN-O |
 *              ---------------------------------------------
 *             |  RIF2_mac | host3_mac   |    30    |  
 *              ---------------------------------------------
 * 
 *                  ----------------------- 
 *             ip: |   SIP     | DIP       | 
 *                  ----------------------- 
 *                 | host3_ip  |  host1_ip | 
 *                  ----------------------- 
 *    Receive:
 *              ---------------------------------------------   
 *        eth: |    DA     |     SA      |   tunnel ID | VLAN-O |
 *              ---------------------------------------------
 *             | host1_mac |  RIF1_mac   |    10    |  100   |
 *              ---------------------------------------------
 * 
 *                  ----------------------- 
 *             ip: |   SIP     | DIP       | 
 *                  ----------------------- 
 *                 | host3_ip  |  host1_ip | 
 *                  ----------------------- 
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int ROUTE_OVER_AC_NUM_OF_RIFS              = 2;
int ROUTE_OVER_AC_NUM_OF_HOSTS             = 2;
int ROUTE_OVER_AC_NUM_OF_VLAN_ACTIONS      = 1;
int ROUTE_OVER_AC_NUM_OF_VLAN_EDIT_ENTRIES = 1;

/* Struct that define main host attributes*/
struct host_route_over_ac_s{
    bcm_mac_t mac;          /* host mac */
    uint32 ipv4_address;    /* IPv4 address of host*/
    bcm_gport_t gport;      /* gport - for internal use no need to initialize*/
    int tunnel_ID;          /* tunnel ID  */
    int vlan_o;             /* outer VID  */
    int vlan_edit_profile;  /* vlan editing profile */    
    int fec;                /* allocated FEC         - for internal use no need to initialize*/
    int outLif;             /* allocated outLif      - for internal use no need to initialize*/
    int encap_id;           /* encapsulation ID(ARP) - for internal use no need to initialize*/
};

/* Struct that define main RIF attributes */
struct rif_route_over_ac_s{
    int sys_port;    
    int vsi;                /* vsi */
    bcm_mac_t my_lsb_mac;   /* 12 lsb bits of MAC on rif*/

};

/*  Main Struct  */
struct route_over_ac_s{
    host_route_over_ac_s  host_def[ROUTE_OVER_AC_NUM_OF_HOSTS];   /* host_def[0] - used for host1
                                                                     host_def[1] - used for host2
                                                                     host_def[2] - used for host3 */ 
    rif_route_over_ac_s  rif_def[ROUTE_OVER_AC_NUM_OF_RIFS ];     /* rif_def[0]  - used for rif1
                                                                     rif_def[1]  - used for rif2 */   
    bcm_mac_t global_mac;        /* 36 msb bits of global MAC*/
    int vrf;                     /* vrf */

    vlan_action_utils_s  vlan_action[ROUTE_OVER_AC_NUM_OF_VLAN_ACTIONS]; /* vlan actions for vlan editing*/

    vlan_edit_utils_s    vlan_edit_entry[ROUTE_OVER_AC_NUM_OF_VLAN_EDIT_ENTRIES]; /* vlan editing entries*/
};


/* Global struct initialization*/
route_over_ac_s g_route_over_ac = {   /*               host MAC                 | host ipv4 address |  gport  |   tunnel ID | vlan_o  |  edit profile  |  fec |outLiF | encap_id */
                                        { { {0x00, 0x00, 0x11, 0x11, 0x11, 0x11},    0x11111111     ,    -1   ,     10   ,  100    ,        10      ,   0,   -1    ,    0x3006},
                                          { {0x00, 0x00, 0x33, 0x33, 0x33, 0x33},    0x33333333     ,    -1   ,     -1   ,  30     ,        -1      ,   0,   -1    ,    0x300a}},
                                      /*  sys_port  |   vsi   |  my_lsb_mac */
                                        { {13       ,    11   , { 0x00, 0x00, 0x00, 0x00, 0x01, 0x11 }},
                                          {14       ,    22   , { 0x00, 0x00, 0x00, 0x00, 0x02, 0x22 }}},   
                                          g_l2_global_mac_utils, /* global mac taken from cint_utils_l2.c only 36 MSB bits are relevant*/
                                          15  /*vrf*/,
                                      /*    action id | outer tpid |     outer action     | inner tpid |  inner action   */       /* used when packet come: */
                                         { {   5      ,   0x8100   , bcmVlanActionAdd ,   0x9100   , bcmVlanActionAdd    }   /*    from host with 1 vlan tag  and forwarded to host with 2 vlan tags */},
                                     /*      edit profile |  tag format | action id  */ 
                                          { {    10       ,     0       ,     5      }}};
/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */
/* Initialization of main struct
 * This function allow to re-write default values or to leave default values without changes
 *
 * INPUT: 
 *   params: new values for g_route_over_ac
 */
int route_over_ac_init(int unit, route_over_ac_s *param){

    if (param != NULL) {
       sal_memcpy(&g_route_over_ac, param, sizeof(g_route_over_ac));
    }

    is_advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    if (!is_advanced_vlan_translation_mode ) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* 
 * Return route_over_ac default value   
 */
void route_over_ac_struct_get(int unit, route_over_ac_s *param){

    sal_memcpy( param, &g_route_over_ac, sizeof(g_route_over_ac));

    return;
}

/* This function runs the main test function with given ports
 *  
 * INPUT: unit     - unit
 *        port1  - port for host1 and host2 
 *        port2  - port for host3
 */
int route_over_ac_with_ports__start_run(int unit,  int port1, int port2){
 
    route_over_ac_s param;

    route_over_ac_struct_get(unit, &param);

    param.rif_def[RIF_1].sys_port = port1;
    param.rif_def[RIF_2].sys_port = port2;
   
    return route_over_ac__start_run(unit, &param);
}


int route_over_ac__start_run(int unit, route_over_ac_s *param)
{
    int i;
    int rv;
    int yesno;

    /* Initializing global parameters*/
    rv = route_over_ac_init(unit, param);
        
    if (rv != BCM_E_NONE) {
        printf("Error, in route_over_ac_init\n");
        return rv;
    }

    /* set VLAN domain to each port */
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_RIFS; i++) {
        rv = port__vlan_domain__set(unit, g_route_over_ac.rif_def[i].sys_port,  g_route_over_ac.rif_def[i].sys_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in port__vlan_domain__set, port=%d, \n", g_route_over_ac.rif_def[i].sys_port);
            return rv;
        }
    }

    /* set TPIDs for both ports and create tag formats(bcm_port_tpid_class_set in */
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_RIFS; i++) {
        port__default_tpid__set(unit, g_route_over_ac.rif_def[i].sys_port);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
    }

    /* set global MAC(36 MSB bits)*/
    l2__mymac_properties_s mymac_properties;
    sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
    mymac_properties.mymac_type = l2__mymac_type_global_msb;
    sal_memcpy(mymac_properties.mymac, g_route_over_ac.global_mac, 6);

    rv = l2__mymac__set(unit, &mymac_properties);
    if (rv != BCM_E_NONE) {
         printf("Error, setting global my-MAC");
    }

/* ****************************************************************** */
/*                  Create rif and setting mac for PORT's             */
/* ****************************************************************** */

    for (i=0; i< ROUTE_OVER_AC_NUM_OF_RIFS; i++) {
        rv = route_over_ac_rif_mac_create(unit, i);
        if (rv != BCM_E_NONE) {
          printf("Fail  route_over_ac_rif_mac_create for rif index %d ", i);
          return rv;
        }
    }

/* ****************************************************************** */
/*  Create l2 logical interface, egress object and add route          */
/* ****************************************************************** */
     rv = route_over_ac_l2_l3_create(unit,RIF_1,HOST_1);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_ac_l2_l3_create for rif index %d, host index", RIF_1, HOST_1);
          return rv;
     }

     rv = route_over_ac_l2_l3_create(unit,RIF_2,HOST_2);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_ac_l2_l3_create for rif index %d, host index", RIF_2, HOST_3);
          return rv;
     }


/* ****************************************************************** */
/*                           VLAN editing                             */
/* ****************************************************************** */
    /* set TPIDs for both ports and create tag formats(bcm_port_tpid_class_set in */

    /* Fill OutLif (EEDB entry)*/
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_HOSTS; i++){
         if (g_route_over_ac.host_def[i].vlan_edit_profile != (-1)) {
             rv = vlan__port_translation__set(unit, g_route_over_ac.host_def[i].tunnel_ID, 
                                                    g_route_over_ac.host_def[i].vlan_o, 
                                                    g_route_over_ac.host_def[i].gport, 
                                                    g_route_over_ac.host_def[i].vlan_edit_profile,
                                                    0); 
             if (rv != BCM_E_NONE) {
                   printf("Fail  route_over_ac_host_translation_set for host %d ", (i+1));
                   return rv;
             }
         }
     }

    
    /* Define actions*/
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_VLAN_ACTIONS; i++){
        rv = vlan__translate_action_with_id__set(unit, g_route_over_ac.vlan_action[i].action_id,                                                  
                                                       g_route_over_ac.vlan_action[i].o_tpid,   
                                                       g_route_over_ac.vlan_action[i].o_action, 
                                                       g_route_over_ac.vlan_action[i].i_tpid,   
                                                       g_route_over_ac.vlan_action[i].i_action, 
                                                       0);                                                 

        if (rv != BCM_E_NONE) {
              printf("Fail  vlan__translate_action_with_id__set for action %d ", (i+1));
              return rv;
        }
    }



    /* Set translation action - connect tag format and edit profile with action*/
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_VLAN_EDIT_ENTRIES; i++){        
            
        rv = vlan__translate_action_class__set(unit, g_route_over_ac.vlan_edit_entry[i].action_id,
                                                     g_route_over_ac.vlan_edit_entry[i].edit_profile,
                                                     g_route_over_ac.vlan_edit_entry[i].tag_format,
                                                     0);

        if (rv != BCM_E_NONE) {
            printf("Fail  vlan__egress_translate_action_class__set vlan edit profile %d  ", (i+1));
            return rv;
        }
    }

    return rv;
}
           
/* Fuction create rif and setting mac*/
int route_over_ac_rif_mac_create(int unit, int rif_index)
{
    int rv;
    create_l3_intf_s intf;
    intf.my_global_mac = g_route_over_ac.global_mac;
    intf.my_lsb_mac =  g_route_over_ac.rif_def[rif_index].my_lsb_mac;
    intf.vsi = g_route_over_ac.rif_def[rif_index].vsi;
    intf.vrf_valid = 1;
    intf.vrf = g_route_over_ac.vrf;
    /* Create L3 Interface */
    rv = l3__intf_rif__create(unit,&intf);
    if (rv != BCM_E_NONE) {
          printf("Fail  l3__intf_rif__create");
          return rv;
    }

    /* Set my_mac (12 LSB bits)*/
    l2__mymac_properties_s mymac_properties;
    sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
    mymac_properties.mymac_type = l2__mymac_type_ipv4_vsi;
    sal_memcpy(mymac_properties.mymac, g_route_over_ac.rif_def[rif_index].my_lsb_mac, 6);
    mymac_properties.vsi = g_route_over_ac.rif_def[rif_index].vsi;

    rv = l2__mymac__set(unit, &mymac_properties);
    if (rv != BCM_E_NONE) {
          printf("Fail  l2__mymac__set: rif(%d) ", g_route_over_ac.rif_def[rif_index].sys_port);
          return rv;
    }

    return rv;
}

      
int route_over_ac_l2_l3_create(int unit, int rif_index, int host_index)
{
    int rv;
    int fec_id = 0;

    vlan_port_with_vsi = g_route_over_ac.rif_def[rif_index].vsi;
    if (g_route_over_ac.host_def[host_index].tunnel_ID != (-1)) {
        /* Create forward group l2 logical interface according  <port,tunnel_ID,vlan>  in PON port */
        fwd_group = 1;
        rv = pon_lif_create(unit,
                            g_route_over_ac.rif_def[rif_index].sys_port,
                            match_otag,
                            0,
                            0,
                            g_route_over_ac.host_def[host_index].tunnel_ID,
                            g_route_over_ac.host_def[host_index].tunnel_ID,
                            g_route_over_ac.host_def[host_index].vlan_o,
                            g_route_over_ac.host_def[host_index].vlan_o,
                            0,
                            0,
                           &g_route_over_ac.host_def[host_index].fec);
        if (rv != BCM_E_NONE) {
              printf("Fail to pon_lif_create with port=%d, tunnel_ID=%d, outer_vid=%d\n", 
                g_route_over_ac.rif_def[rif_index].sys_port,
                g_route_over_ac.host_def[host_index].tunnel_ID,
                g_route_over_ac.host_def[host_index].vlan_o);
              return rv;
        }
    }
    else{
        /* Create general l2 logical interface according  <port,vlan>  in NNI port */
        fwd_group= 0;
        rv = nni_lif_create(unit,
                            g_route_over_ac.rif_def[rif_index].sys_port,
                            match_otag,
                            0,
                            g_route_over_ac.host_def[host_index].vlan_o,
                            g_route_over_ac.host_def[host_index].vlan_o,
                            0,
                            &g_route_over_ac.host_def[host_index].fec,   
                            &g_route_over_ac.host_def[host_index].outLif);
        if (rv != BCM_E_NONE) {
              printf("Fail to nni_lif_create with port=%d, outer_vid=%d\n", 
                g_route_over_ac.rif_def[rif_index].sys_port,
                g_route_over_ac.host_def[host_index].vlan_o);
              return rv;
        }
    }

    g_route_over_ac.host_def[host_index].gport = g_route_over_ac.host_def[host_index].fec;

    if (fwd_group) {
        /* Create egress object without allocation FEC(FEC was allocated in l2__fec_port_vlan_vlan__create function)*/
        create_l3_egress_s l3_egress_encap;
        l3_egress_encap.out_gport = g_route_over_ac.rif_def[rif_index].sys_port;
        l3_egress_encap.vlan = g_route_over_ac.rif_def[rif_index].vsi;
        l3_egress_encap.next_hop_mac_addr = g_route_over_ac.host_def[host_index].mac;
        l3_egress_encap.arp_encap_id = g_route_over_ac.host_def[host_index].encap_id;
        rv = l3__egress_only_encap__create(unit, &l3_egress_encap);
        if (rv != BCM_E_NONE) {
              printf("Fail  l3__egress_only__create ");
              return rv;
        }
        g_route_over_ac.host_def[host_index].encap_id = l3_egress_encap.arp_encap_id;

        /* Add route to host table as format III in arad+      : FEC + ARP + out-RIF
         * For jericho, the lowerst fec ID is 0x1000 and LEM format-III fec bits is 12.
         * So use ecmp at LEM format-III.
         */
        if (is_device_or_above(unit, JERICHO)){

             int tmp_fec = 0;
              BCM_L3_ITF_SET(&tmp_fec, BCM_L3_ITF_TYPE_FEC, (g_route_over_ac.host_def[host_index].fec & 0xFFFF));

              /* Create ecmp entry pointing to FEC */
              bcm_l3_egress_ecmp_t ecmp;
              bcm_l3_egress_ecmp_t_init(&ecmp);

              ecmp.max_paths = 1;
              rv = bcm_l3_egress_ecmp_create(unit, &ecmp,
                                             1, /* nof paths */
                                             &tmp_fec /* interface object of type FEC */
                                             );
               if (rv != BCM_E_NONE) {
                    printf("Fail  bcm_l3_egress_ecmp_create ");
                    return rv;
              }

               /* ecmp id to FORWARD_PORT format */
              int tmp_fwd_grp = 0;
              BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(&tmp_fwd_grp, ecmp.ecmp_intf);

              /* Add route to host table with ecmp id */
              rv = l3__ipv4_route_to_overlay_host__add(unit, g_route_over_ac.host_def[host_index].ipv4_address,
                                                             g_route_over_ac.vrf,
                                                             g_route_over_ac.host_def[host_index].encap_id,
                                                             g_route_over_ac.rif_def[rif_index].vsi,
                                                             tmp_fwd_grp);

        }
        else {
            fec_id = g_route_over_ac.host_def[host_index].fec & 0x7FFF;
            BCM_GPORT_FORWARD_PORT_SET(g_route_over_ac.host_def[host_index].fec, fec_id);         /* overlay tunnel: vxlan gport */
            rv = l3__ipv4_route_to_overlay_host__add(unit, g_route_over_ac.host_def[host_index].ipv4_address,
                                                           g_route_over_ac.vrf,
                                                           g_route_over_ac.host_def[host_index].encap_id,
                                                           g_route_over_ac.rif_def[rif_index].vsi,
                                                           g_route_over_ac.host_def[host_index].fec);
        }

        if (rv != BCM_E_NONE) {
              printf("Fail  l3__ipv4_route_host__add ");
              return rv;
        }
    }

    return rv;
}

int route_over_ac_cleanup(int unit)
{
    int rv = BCM_E_NONE, i;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_host_t host_info;
    bcm_l3_host_t_init(&host_info);

    for (i = 0; i < ROUTE_OVER_AC_NUM_OF_HOSTS; i++) {

        rv = bcm_vlan_port_destroy(unit, g_route_over_ac.host_def[i].gport);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_destroy rv = 0x%x (0x%x)\n",rv, g_route_over_ac.host_def[i].gport);
            return rv;
        }
    }
    rv = bcm_l3_egress_destroy(unit, g_route_over_ac.host_def[RIF_1].encap_id);
    if (rv != BCM_E_NONE) {
        printf(" bcm_l3_egress_destroy rv =0x%x (0x%x)\n",rv, g_route_over_ac.host_def[RIF_1].encap_id);
        return rv;
    }

    host_info.l3a_flags = 0;
    host_info.l3a_intf  = g_route_over_ac.rif_def[RIF_1].vsi;
    rv = bcm_l3_host_delete_all(unit, &host_info);
    if (rv != BCM_E_NONE) {
         printf("Fail  bcm_l3_host_delete_all ");
         return rv;
    }
    return rv;

}

