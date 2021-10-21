/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
 *                         |    |                        |        |
 *                         |    | RIF1               RIF2|        |
 *                         |____|________________________|________|
 *                              |                        |
 *                              |                        |
 *                          host1                     host2 - extender port
 *                         <10,100>                  <ecid = 203, vlan = 30>
 *
 *                         <x,y> - x is outer vlan
 *                                 y is inner vlan
 *
 * Default values:
 * VLAN values are in diagramm
 * Host1   -   MAC: 00:00:11:11:11:11
 *             IP:  11.11.11.11
 * Host2   -   MAC: 00:00:33:33:33:33
 *             IP:  33.33.33.33
 * PORT1(RIF1) - MAC: 00:11:00:00:01:11
 * PORT2(RIF2) - MAC: 00:11:00:00:02:22
 *
 * run:
 * cd   ../../../../src/examples/dnx
 * cint ../dnx/utility/cint_dnx_utils_global.c
 * cint utility/cint_dnx_utils_extender.c
 * cint cint_dnx_advanced_vlan_translation_mode.c
 * cint utility/cint_dnx_utils_vlan.c
 * cint utility/cint_dnx_utils_l3.c
 * cint utility/cint_dnx_utils_l2.c
 * cint utility/cint_dnx_utils_port.c
 * cint utility/cint_dnx_utils_tpid.c
 * cint cint_dnx_route_over_extender.c
 * cint
 * route_over_extender_with_ports__start_run(unit,port1,port2);
 *
 * All default values could be re-written with initialization of the global structure 'g_route_over_extender', before calling the main  function
 * In order to re-write only part of values, call 'route_over_extender_struct_get(route_over_extender_s)' function and re-write values,
 * prior calling the main function
 *
 * Testing routing between AC and extender ports that belong to different VSI's:
 *
 * ##############    Scenario #1    #######################
 *
 *  Traffic from Host2 (extender) to Host1
 *
 *    Send:
 *              --------------------------------------------
 *        eth: |    DA     |     SA      |   ECID |   VLAN  |
 *              --------------------------------------------
 *             |  RIF2_mac | host2_mac   |    203 |    30   |
 *              --------------------------------------------
 *
 *                  -----------------------
 *             ip: |   SIP     | DIP       |
 *                  -----------------------
 *                 | host2_ip  |  host1_ip |
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
 *                 | host2_ip  |  host1_ip |
 *                  -----------------------
 *
 * ##############    Scenario #2    #######################
 *
 *  Traffic from Host1 to Host2 (extender)
 *
 *    Send:
 *              ---------------------------------------------
 *        eth: |    DA     |     SA      |   VLAN-O | VLAN-I |
 *              ---------------------------------------------
 *             | RIF1_mac |  host1_mac   |    10    |  100   |
 *              ---------------------------------------------
 *
 *                  -----------------------
 *             ip: |   SIP     | DIP       |
 *                  -----------------------
 *                 | host1_ip  |  host2_ip |
 *                  -----------------------
 *    Receive:
 *              -------------------------------------------
 *        eth: |    DA     |     SA      |  ECID  |  VLAN  |
 *              -------------------------------------------
 *             | host2_mac |  RIF2_mac   |   203  |  30    |
 *              -------------------------------------------
 *
 *                  -----------------------
 *             ip: |   SIP     | DIP       |
 *                  -----------------------
 *                 | host1_ip  |  host2_ip |
 *                  -----------------------
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int ROUTE_OVER_EXTENDER_NUM_OF_RIFS              = 2;
int ROUTE_OVER_EXTENDER_NUM_OF_HOSTS             = 2;
int ROUTE_OVER_EXTENDER_NUM_OF_VLAN_ACTIONS      = 2;
int ROUTE_OVER_EXTENDER_NUM_OF_VLAN_EDIT_ENTRIES = 2;

/* Struct that define main host attributes*/
struct host_route_over_extender_s {
    bcm_mac_t mac;          /* host mac */
    uint32 ipv4_address;    /* IPv4 address of host*/
    bcm_gport_t gport;      /* gport - for internal use no need to initialize*/
    int vlan_o;             /* outer vlan  */
    int vlan_i;             /* inner vlan  */
    int vlan_edit_profile;  /* vlan editing profile */
    int fec;                /* allocated FEC         - for internal use no need to initialize*/
    int encap_id;           /* encapsulation ID(ARP) - for internal use no need to initialize*/
    int ecmp_intf;          /* ecmp_intf             - for internal use no need to initialize*/
    int is_extender_port;
    int is_cascaded_port;
};

/* Struct that define main RIF attributes */
struct rif_route_over_extender_s {
    int sys_port;
    int vsi;                /* vsi */
    bcm_mac_t my_lsb_mac;   /* 12 lsb bits of MAC on rif*/

};

/*  Main Struct  */
struct route_over_extender_s {
    host_route_over_extender_s  host_def[ROUTE_OVER_EXTENDER_NUM_OF_HOSTS];   /* host_def[0] - used for host1
                                                                                 host_def[1] - used for host2 */
    rif_route_over_extender_s  rif_def[ROUTE_OVER_EXTENDER_NUM_OF_RIFS ];     /* rif_def[0]  - used for rif1
                                                                     rif_def[1]  - used for rif2 */
    bcm_mac_t global_mac;        /* 36 msb bits of global MAC*/
    int vrf;                     /* vrf */

    vlan_action_utils_s  vlan_action[ROUTE_OVER_EXTENDER_NUM_OF_VLAN_ACTIONS]; /* vlan actions for vlan editing*/

    vlan_edit_utils_s    vlan_edit_entry[ROUTE_OVER_EXTENDER_NUM_OF_VLAN_EDIT_ENTRIES]; /* vlan editing entries*/
};

/* Global struct initialization*/
route_over_extender_s g_route_over_extender = {   /*               host MAC | host ipv4 address |  gport  |   vlan_o | vlan_i  |  edit profile  |  fec  | encap_id (must be EVEN) | ecmp_intf */
                                        { { {0x00, 0x00, 0x11, 0x11, 0x11, 0x11},    0x11111111     ,    -1   ,     10   ,  100    ,        10      ,   -1      ,    0x3006              ,   0, 0, 0},
                                          { {0x00, 0x00, 0x33, 0x33, 0x33, 0x33},    0x33333333     ,    -1   ,     203   ,  30    ,        11      ,   -1      ,    0x3008              ,   0, 1, 1}},
                                      /*  sys_port  |   vsi   |  my_lsb_mac */
                                        { {13       ,    11   , { 0x00, 0x00, 0x00, 0x00, 0x01, 0x11 }},
                                          {14       ,    22   , { 0x00, 0x00, 0x00, 0x00, 0x02, 0x22 }}},
                                          {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, /* global mac taken from cint_utils_l2.c only 36 MSB bits are relevant*/
                                          0  /*vrf*/,
                                      /*    action id | outer tpid |     outer action     | inner tpid |  inner action   */       /* used when packet destination is: */
                                        { {   7      ,   0x8100   , bcmVlanActionAdd     ,   0x9100   , bcmVlanActionAdd},        /*    dest host with 2 vlan tags    */
                                          {   8      ,   0x893F   , bcmVlanActionAdd     ,   0x8100   , bcmVlanActionAdd}},       /*    dest host is extender port with 1 vlan tag    */
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
 *   params: new values for g_route_over_extender
 */
int route_over_extender_init(int unit, route_over_extender_s *param) {
    if (param != NULL) {
       sal_memcpy(&g_route_over_extender, param, sizeof(g_route_over_extender));
    }

    advanced_vlan_translation_mode = 1;
    g_route_over_extender.vrf = 3;
    if (!advanced_vlan_translation_mode ) {
        return BCM_E_PARAM;
    }

    return extender__init(unit, 0x893F);
}

/*
 * Return route_over_extender default value
 */
void route_over_extender_struct_get(int unit, route_over_extender_s *param) {
    sal_memcpy( param, &g_route_over_extender, sizeof(g_route_over_extender));

    return;
}

/*
 * Just to replace the VID, support on JR2
 */
int route_over_extender_vlan_edit_extender_ac_set(int unit, bcm_gport_t gport, uint8 is_ingress) {
    int rv = BCM_E_NONE;
    int ve_profile = 5; /*should be different from non-extender port*/

    uint32 outer_vlan = 30;
    uint32 inner_vlan = 0;
    /* configure AC-lif.vlans and AC-lif.VE-profile */
    rv = vlan__port_translation__set(unit,
                                     outer_vlan /* outer vlan */,
                                     inner_vlan /* inner vlan */,
                                     gport /* ac-lif */,
                                     ve_profile,
                                     is_ingress /* is ingress */);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    /* create vlan edit command */
    int vlan_action_id;
    rv = vlan__avt_vid_action__set(unit,
                                   is_ingress /* is ingress */,
                                   0x8100 /* outer TPID */,
                                   0x9100 /* inner TPID */,
                                   bcmVlanActionAdd, /* outer action */
                                   bcmVlanActionNone, /* inner action */
                                   &vlan_action_id  /* Vlan edit command id */);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ve_profile; /* vlan edit profile */
    action_class.vlan_translation_action_id = vlan_action_id; /* vlan edit command */
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }
    return rv;
}

/* This function runs the main test function with given ports
 *
 * INPUT: unit     - unit
 *        port1  - port for host1
 *        port2  - port for host2
 */
int route_over_extender_with_ports__start_run(int unit,  int port1, int port2) {
    route_over_extender_s param;

    route_over_extender_struct_get(unit, &param);
    param.rif_def[RIF_1].sys_port = port1;
    param.rif_def[RIF_2].sys_port = port2;

    return route_over_extender__start_run(unit, &param);
}


int route_over_extender__start_run(int unit, route_over_extender_s *param) {
    int i;
    int rv;

    /* Initializing global parameters*/
    rv = route_over_extender_init(unit, param);
    if (rv != BCM_E_NONE) {
        printf("Error, in route_over_extender_init\n");
        return rv;
    }

    /* set VLAN domain to each port */
    for (i=0; i< ROUTE_OVER_EXTENDER_NUM_OF_RIFS; i++) {
        rv = bcm_vswitch_create_with_id(unit, g_route_over_extender.rif_def[i].vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_create failed for vsi - %d, rv - %d\n", g_route_over_extender.rif_def[i].vsi, rv);
            return rv;
        }
        if (g_route_over_extender.host_def[i].is_cascaded_port) {
            rv = bcm_vlan_gport_add(unit, g_route_over_extender.host_def[i].vlan_i, g_route_over_extender.rif_def[i].sys_port, 0);
        } else {
            rv = bcm_vlan_gport_add(unit, g_route_over_extender.host_def[i].vlan_o, g_route_over_extender.rif_def[i].sys_port, 0);
        }
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vlan_gport_add for port %d\n", g_route_over_extender.rif_def[i].sys_port);
            return rv;
        }
    }

    int station_id;
    bcm_l2_station_t station;
    bcm_l2_station_t_init(&station);
    station.dst_mac_mask[0] = 0xff;
    station.dst_mac_mask[1] = 0xff;
    station.dst_mac_mask[2] = 0xff;
    station.dst_mac_mask[3] = 0xff;
    station.dst_mac_mask[4] = 0xff;
    station.dst_mac_mask[5] = 0xff;
    station.src_port_mask = 0;
    /* Copy the mac address */
    sal_memcpy(station.dst_mac, g_route_over_extender.global_mac, 6);
    rv = bcm_l2_station_add(unit, &station_id, &station);

    /* ****************************************************************** */
    /*                  Create rif and setting mac for PORT's             */
    /* ****************************************************************** */

    for (i=0; i< ROUTE_OVER_EXTENDER_NUM_OF_RIFS; i++) {
        rv = route_over_extender_rif_mac_create(unit, i);
        if (rv != BCM_E_NONE) {
          printf("Fail  route_over_extender_rif_mac_create for rif index %d ", i);
          return rv;
        }
    }

    /* ****************************************************************** */
    /*  Create l2 logical interface, egress object and add route          */
    /* ****************************************************************** */
     rv = route_over_extender_l2_l3_create(unit,RIF_1,HOST_1);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_extender_l2_l3_create for rif index %d, host index %d\n", RIF_1, HOST_1);
          return rv;
     }

     rv = route_over_extender_l2_l3_create(unit,RIF_2,HOST_2);
     if (rv != BCM_E_NONE) {
          printf("Fail  route_over_extender_l2_l3_create for rif index %d, host index %d\n", RIF_2, HOST_2);
          return rv;
     }


    /* ****************************************************************** */
    /*                           VLAN editing                             */
    /* ****************************************************************** */
    /**set eve */
    for (i=0; i< ROUTE_OVER_EXTENDER_NUM_OF_HOSTS; i++) {
        if (!g_route_over_extender.host_def[i].is_extender_port) {
            rv = vlan__default_out_ac_allocate_and_set(unit, g_route_over_extender.rif_def[i].sys_port);
            if (rv != BCM_E_NONE) {
                printf("Error, vlan__default_out_ac_allocate_and_set\n");
                return rv;
            }
        } else {
            rv = route_over_extender_vlan_edit_extender_ac_set(unit, g_route_over_extender.host_def[i].gport, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, route_over_extender_vlan_edit_extender_ac_set\n");
                return rv;
            }
        }
    }
    return rv;
}

/* Fuction create rif and setting mac*/
int route_over_extender_rif_mac_create(int unit, int rif_index) {
    int rv;
    create_l3_intf_s intf;
    intf.my_global_mac = g_route_over_extender.global_mac;
    intf.my_lsb_mac =  g_route_over_extender.rif_def[rif_index].my_lsb_mac;
    intf.vsi = g_route_over_extender.rif_def[rif_index].vsi;

    bcm_l3_intf_t l3if;
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, g_route_over_extender.rif_def[rif_index].my_lsb_mac, 6);
    sal_memcpy(l3if.l3a_mac_addr, g_route_over_extender.global_mac, 4);
    l3if.l3a_intf_id = l3if.l3a_vid = g_route_over_extender.rif_def[rif_index].vsi;
    l3if.l3a_vrf = g_route_over_extender.rif_def[rif_index].vsi;
    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l3_intf_create for egress\n", rv);
        return rv;
    }

    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ing_if.vrf = g_route_over_extender.vrf;
    rv = bcm_l3_ingress_create(unit, l3_ing_if, g_route_over_extender.rif_def[rif_index].vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l3_ingress_create\n", rv);
        return rv;
    }

    int station_id;
    bcm_l2_station_t station;
    bcm_l2_station_t_init(&station);
    station.dst_mac_mask[0] = 0x00; /* Only use 12 LSBs */
    station.dst_mac_mask[1] = 0x00;
    station.dst_mac_mask[2] = 0x00;
    station.dst_mac_mask[3] = 0x00;
    station.dst_mac_mask[4] = 0x0f;
    station.dst_mac_mask[5] = 0xff;
    station.vlan            = g_route_over_extender.rif_def[rif_index].vsi;
    station.vlan_mask       = 0xffff; /* vsi is  valid */
    station.flags           = BCM_L2_STATION_IPV4;
    station.src_port_mask = 0;
    /* Copy the mac address */
    sal_memcpy(station.dst_mac, g_route_over_extender.rif_def[rif_index].my_lsb_mac, 6);
    rv = bcm_l2_station_add(unit, &station_id, &station);
    return rv;
}

/* Create port extender and FEC */
int route_over_extender__fec_extender_port_create(int unit, int rif_index, int host_index) {
    int rv;

    if (g_route_over_extender.host_def[host_index].is_cascaded_port) {
        rv = extender__cascaded_port_configure(unit, g_route_over_extender.rif_def[rif_index].sys_port);
        if (rv != BCM_E_NONE) {
            printf("Error, extender__cascaded_port_configure for port - %d, rv - %d\n", g_route_over_extender.rif_def[rif_index].sys_port, rv);
            return rv;
        }
    }

    extender__port_extender_port_s extender_port;
    extender_port.port = g_route_over_extender.rif_def[rif_index].sys_port;
    extender_port.tag_vid = g_route_over_extender.host_def[host_index].vlan_i;
    extender_port.extender_vid = 203;

    /* Configure Port-Extender LIFs and Register E-Channel */
    rv = extender__extender_port_configure(unit, &extender_port);
    if (rv != BCM_E_NONE) {
        printf("Error in extender__port_extender_set for  rv - %d\n", rv);
        return rv;
    }

    /* Attach the Extender-Port to a specific VSI */
    rv = bcm_vswitch_port_add(unit, g_route_over_extender.rif_def[rif_index].vsi, extender_port.extender_gport_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_route_over_extender.rif_def[rif_index].vsi, extender_port.extender_gport_id, rv);
    }
    g_route_over_extender.host_def[host_index].gport = extender_port.extender_gport_id;

    /* FEC Entry: Allocate a FEC entry pointing to an extender outlif*/
    dnx_utils_l3_fec_s fec_structure;
    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = g_route_over_extender.rif_def[rif_index].sys_port;
    BCM_L3_ITF_SET(&fec_structure.tunnel_gport, BCM_L3_ITF_TYPE_LIF, (g_route_over_extender.host_def[host_index].gport & 0xFFFF));
    uint32 system_headers_mode = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_mode", NULL);
    fec_structure.flags = (system_headers_mode == 0 ? BCM_L3_ENCAP_SPACE_OPTIMIZED : 0);
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);
    g_route_over_extender.host_def[host_index].fec = fec_structure.l3eg_fec_id;
    return rv;
}

int route_over_extender_l2_l3_create(int unit, int rif_index, int host_index) {
    int rv, outLif_dummy;

    if (g_route_over_extender.host_def[host_index].is_extender_port)
    {
        /* Configure extender port + FEC */
        rv = route_over_extender__fec_extender_port_create(unit, rif_index, host_index);
        printf("Configure extender port + FEC...\n");
    } else if (g_route_over_extender.host_def[host_index].vlan_i != -1) {
        /**use following fec:*/
        dnx_utils_l3_fec_s l3_fec;
        dnx_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        l3_fec.destination  = g_route_over_extender.rif_def[rif_index].sys_port;
        BCM_L3_ITF_SET(&l3_fec.tunnel_gport, BCM_L3_ITF_TYPE_RIF, g_route_over_extender.rif_def[rif_index].vsi);
        rv = dnx_utils_l3_fec_create(unit, &l3_fec);
        g_route_over_extender.host_def[host_index].fec = l3_fec.l3eg_fec_id;

        /**create vlan_port*/
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vp.port = g_route_over_extender.rif_def[rif_index].sys_port;
        vp.match_vlan = g_route_over_extender.host_def[host_index].vlan_o;
        vp.match_inner_vlan = g_route_over_extender.host_def[host_index].vlan_i;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            print rv;
            return rv;
        }
        rv = bcm_vswitch_port_add(unit, g_route_over_extender.rif_def[rif_index].vsi, vp.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port - %#x\n", g_route_over_extender.rif_def[rif_index].vsi, vp.vlan_port_id);
            return rv;
        }
    }
    else
    {
        /* Create non protected l2 logical interface according <port,vlan>*/
        rv = l2__fec_forward_group_port_vlan__create(unit,  g_route_over_extender.rif_def[rif_index].sys_port,
                                                            g_route_over_extender.host_def[host_index].vlan_o,
                                                            g_route_over_extender.rif_def[rif_index].vsi,
                                                           &g_route_over_extender.host_def[host_index].fec,
                                                           &outLif_dummy);
        g_route_over_extender.host_def[host_index].gport = g_route_over_extender.host_def[host_index].fec;
        printf("Create non protected l2 logical interface: single tag\n");
    }

    if (rv != BCM_E_NONE) {
          printf("Fail  L2 and FEC entry create\n");
          return rv;
    }

    /* ARP Entry: Create egress object without FEC allocation */
    dnx_utils_l3_arp_s arp_structure;
    dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, g_route_over_extender.host_def[host_index].encap_id, 0, 0, g_route_over_extender.host_def[host_index].mac, g_route_over_extender.rif_def[rif_index].vsi);
    rv = dnx_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    g_route_over_extender.host_def[host_index].encap_id = arp_structure.l3eg_arp_id;

    /* vlan_port_id to L3_ITF_FEC format */
    int tmp_fec = 0;
    BCM_L3_ITF_SET(&tmp_fec, BCM_L3_ITF_TYPE_FEC, (g_route_over_extender.host_def[host_index].fec & 0xFFFF));

    /* Create ecmp entry pointing to FEC */
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_egress_ecmp_t_init(&ecmp);

    ecmp.max_paths = 1;
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp,
                                   1, /* nof paths */
                                   &tmp_fec /* interface object of type FEC */
                                   );
     if (rv != BCM_E_NONE) {
          printf("Fail  bcm_l3_egress_ecmp_create\n");
          return rv;
    }

     /* ecmp id to FORWARD_PORT format */
    int tmp_fwd_grp = 0;
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(&tmp_fwd_grp, ecmp.ecmp_intf);
    g_route_over_extender.host_def[host_index].ecmp_intf = ecmp.ecmp_intf;

    /* Add route to host table with ecmp id */
    bcm_l3_host_t l3_host;
    bcm_l3_host_t_init(l3_host);
    /* key of host entry */
    l3_host.l3a_vrf = g_route_over_extender.vrf;
    l3_host.l3a_ip_addr = g_route_over_extender.host_def[host_index].ipv4_address;
    /* data of host entry */
    l3_host.l3a_port_tgid = tmp_fwd_grp;
    l3_host.l3a_intf = g_route_over_extender.rif_def[rif_index].vsi;
    l3_host.encap_id = g_route_over_extender.host_def[host_index].encap_id;
    rv = bcm_l3_host_add(unit, &l3_host);

    printf(">>>host_index[%d]-rif_index[%d]: vrf[%d],encap_id[%#x],vsi[%d],fec[%#x]\n",
        host_index, rif_index,
        g_route_over_extender.vrf,
        g_route_over_extender.host_def[host_index].encap_id,
        g_route_over_extender.rif_def[rif_index].vsi,
        tmp_fwd_grp);

    if (rv != BCM_E_NONE) {
         printf("Fail bcm_l3_host_add\n");
         return rv;
    }

    return rv;
}

int route_over_extender_cleanup(int unit) {
    int rv = BCM_E_NONE, i;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_host_t host_info;
    bcm_l3_host_t_init(&host_info);

    for (i = (ROUTE_OVER_EXTENDER_NUM_OF_HOSTS - 1); i  >= 0; i--) {

        /* Clean extender port configuration */
        if (g_route_over_extender.host_def[i].is_extender_port) {
            rv = extender__extender_port_cleanup(unit, g_route_over_extender.host_def[i].gport);
            if (rv != BCM_E_NONE) {
                printf("\n\n Fail  extender__extender_port_cleanup rv = 0x%x\n\n",rv);
                return rv;
            }
        }

        rv = bcm_l3_egress_destroy(unit, g_route_over_extender.host_def[i].encap_id);
        if (rv != BCM_E_NONE) {
            printf("\n\n Fail bcm_l3_egress_destroy rv =0x%x (0x%x)\n\n",rv, g_route_over_extender.host_def[i].encap_id);
            return rv;
        }

        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.max_paths = 1;
        ecmp.ecmp_intf = g_route_over_extender.host_def[i].ecmp_intf;
        rv = bcm_l3_egress_ecmp_destroy(unit, &ecmp);
        if (rv != BCM_E_NONE) {
             printf("\n\n Fail bcm_l3_egress_ecmp_destroy rv =0x%x (0x%x)\n\n",rv, g_route_over_extender.host_def[i].ecmp_intf);
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
