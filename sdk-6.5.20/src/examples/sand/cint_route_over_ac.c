/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * 
 * **************************************************************************************************************************************************
 * 
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
 *                              |     |                  |
 *                              |     |                  |
 *                          host1    host2             host3
 *                         <10,100>  <20>             <30,300>
 * 
 *                         <x,y> - x is outer vlan
 *                                 y is inner vlan
 * 
 * Default values:
 * VLAN values are in diagramm
 * Host1   -   MAC: 00:00:11:11:11:11
 *             IP:  11.11.11.11
 * Host2   -   MAC: 00:00:22:22:22:22
 *             IP:  22.22.22.22
 * Host3   -   MAC: 00:00:33:33:33:33
 *             IP:  33.33.33.33
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
 * cint_route_over_ac.c
 * cint
 * route_over_ac_with_ports__start_run(unit,port1,port2);
 * 
 * All default values could be re-written with initialization of the global structure 'g_route_over_ac', before calliing the main  function 
 * In order to re-write only part of values, call 'route_over_ac_struct_get(route_over_ac_s)' function and re-write values, 
 * prior calling the main function
 * 
 * ##############    Scenario #1    #######################
 *
 *  Traffic from Host3 to Host1
 *  Testing routing over ac between two AC's that belong to different VSI's
 *  
 *    Send:                                           
 *              ---------------------------------------------   
 *        eth: |    DA     |     SA      |   VLAN-O | VLAN-I |
 *              ---------------------------------------------
 *             |  RIF2_mac | host3_mac   |    30    |  300   |
 *              ---------------------------------------------
 * 
 *                  ----------------------- 
 *             ip: |   SIP     | DIP       | 
 *                  ----------------------- 
 *                 | host3_ip  |  host1_ip | 
 *                  ----------------------- 
 *    Receive:
 *              ---------------------------------------------   
 *        eth: |    DA     |     SA      |   VLAN-O | VLAN-I |
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
int ROUTE_OVER_AC_NUM_OF_HOSTS             = 3;
int ROUTE_OVER_AC_NUM_OF_VLAN_ACTIONS      = 2;
int ROUTE_OVER_AC_NUM_OF_VLAN_EDIT_ENTRIES = 2;

/* Struct that define main host attributes*/
struct host_route_over_ac_s{
    bcm_mac_t mac;          /* host mac */
    uint32 ipv4_address;    /* IPv4 address of host*/
    bcm_gport_t gport;      /* gport - for internal use no need to initialize*/
    int vlan_o;             /* outer vlan  */
    int vlan_i;             /* inner vlan  */
    int vlan_edit_profile;  /* vlan editing profile */    
    int fec;                /* allocated FEC         - for internal use no need to initialize*/
    int outLif;             /* allocated outLif      - for internal use no need to initialize*/
    int encap_id;           /* encapsulation ID(ARP) - for internal use no need to initialize*/
    int ecmp_intf;			/* ecmp_intf 			 - for internal use no need to initialize*/
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
route_over_ac_s g_route_over_ac = {   /*               host MAC                 | host ipv4 address |  gport  |   vlan_o | vlan_i  |  edit profile  |  fec |outLiF | encap_id (must be EVEN) | ecmp_intf */
                                        { { {0x00, 0x00, 0x11, 0x11, 0x11, 0x11},    0x11111111     ,    -1   ,     10   ,  100    ,        10      ,   -1  , -1    ,    0x3006              ,   0},
                                          { {0x00, 0x00, 0x22, 0x22, 0x22, 0x22},    0x22222222     ,    -1   ,     20   ,    0    ,        11      ,   -1  , -1    ,    0x3008              ,   0},
                                          { {0x00, 0x00, 0x33, 0x33, 0x33, 0x33},    0x33333333     ,    -1   ,     30   ,  300    ,        10      ,   -1  , -1    ,    0x300a              ,   0}},
                                      /*  sys_port  |   vsi   |  my_lsb_mac */
                                        { {13       ,    11   , { 0x00, 0x00, 0x00, 0x00, 0x01, 0x11 }},
                                          {14       ,    22   , { 0x00, 0x00, 0x00, 0x00, 0x02, 0x22 }}},
                                          g_l2_global_mac_utils, /* global mac taken from cint_utils_l2.c only 36 MSB bits are relevant*/
                                          10  /*vrf*/,
                                      /*    action id | outer tpid |     outer action     | inner tpid |  inner action   */       /* used when packet destination is: */
                                        { {   7      ,   0x8100   , bcmVlanActionAdd     ,   0x9100   , bcmVlanActionAdd},        /*    host with 2 vlan tags    */
                                          {   8      ,   0x8100   , bcmVlanActionAdd     ,      0     , bcmVlanActionNone}},      /*    thost with 1 vlan tag    */
                                      /*      edit profile |  tag format | action id  */
                                          { {    10       ,     0       ,     7      },
                                            {    11       ,     0       ,     8      }}};
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
    if (!is_device_or_above(unit, JERICHO2)) {
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

        if (!advanced_vlan_translation_mode ) {
            return BCM_E_PARAM;
        }
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

    if (!is_device_or_above(unit,JERICHO)) {
        printf("Error, supported on Jericho device only.");
        return BCM_E_PARAM;
    }

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
        rv = bcm_vlan_gport_add(unit, 30, g_route_over_ac.rif_def[i].sys_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add \n");
            return rv;
        }
        rv = bcm_vlan_gport_add(unit, 10, g_route_over_ac.rif_def[i].sys_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add \n");
            return rv;
        }
    }

    /* set TPIDs for both ports and create tag formats(bcm_port_tpid_class_set in */
    /* Tag formats that created are: */
    /*                               tag_fomat     tpid1      tpid2   */
    /*                                   2         0x8100      any    */
    /*                                   3         0x9100      any    */
    /*                                   6         0x8100      0x9100 */
    
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
/*  Create l2 logical interface, egress object and add route          */
/* ****************************************************************** */
     rv = route_over_ac_l2_l3_create(unit,RIF_1,HOST_1);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_ac_l2_l3_create for rif index %d, host index %d\n", RIF_1, HOST_1);
          return rv;
     }

     rv = route_over_ac_l2_l3_create(unit,RIF_2,HOST_2);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_ac_l2_l3_create for rif index %d, host index %d\n", RIF_1, HOST_2);
          return rv;
     }

     rv = route_over_ac_l2_l3_create(unit,RIF_2,HOST_3);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_ac_l2_l3_create for rif index %d, host index %d\n", RIF_2, HOST_3);
          return rv;
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
/*                           VLAN editing                             */
/* ****************************************************************** */
    /* set TPIDs for both ports and create tag formats(bcm_port_tpid_class_set in */
    /* Tag formats that created are: */
    /*                               tag_fomat     tpid1      tpid2   */
    /*                                   2         0x8100      any    */
    /*                                   3         0x9100      any    */
    /*                                   6         0x8100      0x9100 */


    /* Fill OutLif (EEDB entry)*/
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_HOSTS; i++){

        rv = vlan__port_translation__set(unit, g_route_over_ac.host_def[i].vlan_o,
                                            g_route_over_ac.host_def[i].vlan_i,
                                            g_route_over_ac.host_def[i].gport,
                                            g_route_over_ac.host_def[i].vlan_edit_profile,
                                            0);
        if (rv != BCM_E_NONE) {
           printf("Fail  vlan__port_translation__set for host %d ", (i+1));
           return rv;
        }
     }

    /* Define actions*/
    for (i=0; i< ROUTE_OVER_AC_NUM_OF_VLAN_ACTIONS; i++){

        bcm_vlan_action_set_t action;

        /* Create action ID*/
        rv = bcm_vlan_translate_action_id_create( unit, (BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID), &g_route_over_ac.vlan_action[i].action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = g_route_over_ac.vlan_action[i].o_action;
        action.dt_inner = g_route_over_ac.vlan_action[i].i_action;
        action.outer_tpid = g_route_over_ac.vlan_action[i].o_tpid;
        action.inner_tpid = g_route_over_ac.vlan_action[i].i_tpid;
        /* Since L2 header is terminated, pcp-dei action should be same as vlan action (bcmVlanActionAdd) */
        action.dt_outer_pkt_prio = g_route_over_ac.vlan_action[i].o_action;
        action.dt_inner_pkt_prio = g_route_over_ac.vlan_action[i].i_action;

        rv = bcm_vlan_translate_action_id_set( unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               g_route_over_ac.vlan_action[i].action_id,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set, action = %d\n", i);
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
    intf.vrf = g_route_over_ac.vrf;
    intf.vrf_valid = TRUE;
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
    if (is_device_or_above(unit,JERICHO2)) {
        bcm_vlan_port_t vp;
        bcm_l3_egress_t l3eg;
        int system_headers_mode = soc_property_get(unit , "system_headers_mode", 1);

        bcm_vlan_port_t_init(&vp);
        bcm_l3_egress_t_init(&l3eg);
        if (g_route_over_ac.host_def[host_index].vlan_i != -1){
            vlan__vlan_port_vsi_create(unit, g_route_over_ac.rif_def[rif_index].vsi, g_route_over_ac.rif_def[rif_index].sys_port,
                g_route_over_ac.host_def[host_index].vlan_o, g_route_over_ac.host_def[host_index].vlan_i, 0, &g_route_over_ac.host_def[host_index].gport);
        } else{
            vlan__vlan_port_vsi_create(unit, g_route_over_ac.rif_def[rif_index].vsi, g_route_over_ac.rif_def[rif_index].sys_port,
                g_route_over_ac.host_def[host_index].vlan_o, BCM_VLAN_INVALID, 0, &g_route_over_ac.host_def[host_index].gport);
        }
        vp.vlan_port_id = g_route_over_ac.host_def[host_index].gport;
        rv = bcm_vlan_port_find(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vlan_port_find of gport(%ld) before associating - %s\n", vp.vlan_port_id, bcm_errmsg(rv));
            return rv;
        }
        g_route_over_ac.host_def[host_index].outLif = vp.encap_id;
        l3eg.port = g_route_over_ac.rif_def[rif_index].sys_port;
        /* In JR mode, outlif pointer as EEI */
        if (system_headers_mode == 0) {
            l3eg.flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
        }
        BCM_L3_ITF_SET(l3eg.intf, BCM_L3_ITF_TYPE_LIF, g_route_over_ac.host_def[host_index].outLif);
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY , &l3eg, &g_route_over_ac.host_def[host_index].fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rc;
        }
    } else {
        if (g_route_over_ac.host_def[host_index].vlan_i != -1) {
            /* Create non protected l2 logical interface according <port,vlan,vlan>*/

            rv = l2__fec_forward_group_port_vlan_vlan__create(unit,  g_route_over_ac.rif_def[rif_index].sys_port,
                                                                     g_route_over_ac.host_def[host_index].vlan_o,
                                                                     g_route_over_ac.host_def[host_index].vlan_i,
                                                                     g_route_over_ac.rif_def[rif_index].vsi,
                                                                    &g_route_over_ac.host_def[host_index].fec,
                                                                    &g_route_over_ac.host_def[host_index].outLif);
        }
        else{
            /* Create non protected l2 logical interface according <port,vlan>*/
            rv = l2__fec_forward_group_port_vlan__create(unit,  g_route_over_ac.rif_def[rif_index].sys_port,
                                                                g_route_over_ac.host_def[host_index].vlan_o,
                                                                g_route_over_ac.rif_def[rif_index].vsi,
                                                               &g_route_over_ac.host_def[host_index].fec,
                                                               &g_route_over_ac.host_def[host_index].outLif);
        }
        g_route_over_ac.host_def[host_index].gport = g_route_over_ac.host_def[host_index].fec;
    }

    if (rv != BCM_E_NONE) {
          printf("Fail  l2__non_protection_port_vlan_vlan__create ");
          return rv;
    }

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

    /* vlan_port_id to L3_ITF_FEC format */
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
    g_route_over_ac.host_def[host_index].ecmp_intf = ecmp.ecmp_intf;

    /* Add route to host table with ecmp id */
    rv = l3__ipv4_route_to_overlay_host__add(unit, g_route_over_ac.host_def[host_index].ipv4_address,
                                                   g_route_over_ac.vrf,
                                                   g_route_over_ac.host_def[host_index].encap_id,
                                                   g_route_over_ac.rif_def[rif_index].vsi,
                                                   tmp_fwd_grp);
    if (rv != BCM_E_NONE) {
         printf("Fail  l3__ipv4_route_host__add ");
         return rv;
    }

    return rv;
}

int route_over_ac_cleanup(int unit)
{
    int rv = BCM_E_NONE, i;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_host_t host_info;
    bcm_l3_host_t_init(&host_info);

    for (i = (ROUTE_OVER_AC_NUM_OF_HOSTS - 1); i  >= 0; i--) {

        rv = bcm_vlan_port_destroy(unit, g_route_over_ac.host_def[i].gport);
        if (rv != BCM_E_NONE) {
            printf("\n\n >>>>>  bcm_vlan_port_destroy rv = 0x%x (0x%x)\n\n",rv, g_route_over_ac.host_def[i].gport);
            return rv;
        }

        rv = bcm_l3_egress_destroy(unit, g_route_over_ac.host_def[i].encap_id);
        if (rv != BCM_E_NONE) {
            printf("\n\n >>>>> bcm_l3_egress_destroy rv =0x%x (0x%x)\n\n",rv, g_route_over_ac.host_def[i].encap_id);
            return rv;
        }

        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.max_paths = 1;
        ecmp.ecmp_intf = g_route_over_ac.host_def[i].ecmp_intf;
        rv = bcm_l3_egress_ecmp_destroy(unit, &ecmp);
        if (rv != BCM_E_NONE) {
             printf("\n\n >>>>> bcm_l3_egress_ecmp_destroy rv =0x%x (0x%x)\n\n",rv, g_route_over_ac.host_def[i].ecmp_intf);
             return rv;
        }
    }

    rv = bcm_l3_host_delete_all(unit, &host_info);
    if (rv != BCM_E_NONE) {
         printf("Fail  bcm_l3_host_delete_all ");
         return rv;
    }
    return rv;

}
