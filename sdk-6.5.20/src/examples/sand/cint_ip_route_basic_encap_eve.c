/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */


/* **************************************************************************************************************************************************
 * set following soc properties (only relevant for up to Jericho +)                                                                                 *
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.                                                                      *
 * bcm886xx_vlan_translate_mode=1                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  * 
 * This CINT demonstrates Basic Route with Egress Vlan Edit ENCAP Stage.                                                                            * 
 *                                                                                                                                                  *
 * The cint creates a basic route scheme with EVE at ENCAP Stage.                                                                                   *
 * The test uses one input port and one output port.                                                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  * 
 *                                       INGRESS                                                                                                    *
 *                                                                                                                                                  *        
 *                 +-------------+           +-------+           +-----------+                                                                      *
 *                 |             |           |       |           |           |                                                                      *
 *  in_port        |             |           |       |           |           |                                                                      *
 * +-------------->+  LIF        +----------->  VSI  +----------->   VRF     +----------------------+                                               *
 *                 |             |           |       |           |           |                      |                                               *
 *                 |             |           |       |           |           |                      |                                               *
 *                 +-------------+           +-------+           +-----------+                      |                                               *
 *                                                                                                  |                                               *
 *                                                                                                  |                                               *
 *                                                                                                  |                                               *
 *                                  EGRESS                                                          |                                               *
 *                                                                                                  |                                               *
 *                    +---------+              +-----------+                                        |                                               *
 *                    |         |              |           |                                        |                                               *
 *    out_port        | ENCAP   |      UNTAG   |  (ESEM)   |                                        |                                               *
 *  <-----------------+ EVE     +<-----Packet--+  Out-AC   |    <-----------------------------------+                                               *
 *                    |         |              |  Info     |                                                                                        *
 *                    |         |              |           |                                                                                        *
 *                    +---------+              +----+------+                                                                                        *
 *                                                  ^                                                                                               *
 *                                                  |                                                                                               *
 *                                                  |                                                                                               *
 *                                                  |                                                                                               *
 *                                                  |                                                                                               *
 *                                                  |   ESEM Lookup                                                                                 *
 *                                                  +-----------+                                                                                   *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * ./bcm.user                                                                                                                                       *
 * cint /src/examples/sand/utility/cint_sand_utils_vlan_translate.c                                                                                 *
 * cint /src/examples/sand/utility/cint_sand_utils_global.c                                                                                         *
 * cint src/examples/sand/cint_ip_route_basic.c                                                                                                     *
 * cint src/examples/sand/cint_ip_route_basic_encap_eve.c                                                                                           *
 * cint                                                                                                                                             *
 * ip_route_basic_encap_eve_with_ports__main_config__start_run(0,200,201,56,57,17,0);                                                               *
 * ip_route_basic_encap_eve__step_config__start_run(0,1);                                                                                           *
 * ip_route_basic_encap_eve__step_config__start_run(0,2);                                                                                           *
 * ip_route_basic_encap_eve__step_config__start_run(0,3);                                                                                           * 
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_ip_route_basic_encap_eve', before calling                  *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'ip_route_basic_encap_eve_struct_get(ip_route_basic_encap_eve_s)' function and                    *
 * re-write values, prior calling the main function                                                                                                 *
 */
 
/*
 * Utilities APIs
 */

int
out_vlan_port_intf_set(
    int unit,
    int out_port,
    int intf_out,
    bcm_gport_t * vlan_port_id)
{
    int rv = BCM_E_NONE;
    int encoded_lif;
    bcm_vlan_port_t vlan_port;

    /* add port, according to port_vlan_vlan */
    bcm_vlan_port_t_init(&vlan_port);

    /* set port attribures, key <port-vlan-vlan>*/
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = out_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    vlan_port.vsi = intf_out;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    *vlan_port_id = vlan_port.vlan_port_id;
    
    printf("bcm_vlan_port_create: port = %d, out_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
    
    return rv;
}
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int NUM_OF_TEST_STEPS = 4;
int NOF_TAG_FORMAT_CREATED = 5;

/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame (IEEE 802.1Q)*/
int TPID_VALUE_C_8100_TAG = 0x8100;
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1Q)*/
int TPID_VALUE_S_9100_TAG = 0x9100;
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1ad, a.k.a QinQ)*/
int TPID_VALUE_S_88A8_TAG = 0x88A8;
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1ah, a.k.a MAC-IN-MAC)*/
int TPID_VALUE_MAC_IN_MAC_TAG = 0x88E7;

/**
 * Frame has no VLAN TAGs
 */
int ETHERNET_FRAME_VLAN_FORMAT_NONE = 0;
/**
 * Frame has S-VLAN TAG
 */
int ETHERNET_FRAME_VLAN_FORMAT_S_TAG = 4;
/**
 * Frame has C-VLAN TAG
 */
int ETHERNET_FRAME_VLAN_FORMAT_C_TAG = 8;
/**
 * Frame has S-C-VLAN TAGs
 */
int ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG = 16;
/**
 * Frame has S-Priority-C-VLAN TAGs (Dtag S+C and OUTER_VID=0)
 */
int ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG = 24;

/*  Main Struct  */
struct ip_route_basic_encap_eve_s{
    int in_port;
    int out_port;
    int new_outer_vlan;
    int new_inner_vlan;
    int host_format;
    int intf_in;                    /* Incoming packet ETH-RIF */
    int intf_out;                   /* Outgoing packet ETH-RIF */
    int fec;
    int vrf;
    int encap_id;                   /* ARP-Link-layer */
    bcm_mac_t intf_in_mac_address;  /* my-MAC */
    bcm_mac_t intf_out_mac_address; /* my-MAC */
    bcm_mac_t arp_next_hop_mac;     /* next_hop_mac */
    uint32 network_ip;
    uint32 network_mask;
    uint32 host_ip;
    uint32 vlan_edit_profile;
    int action_id;
    bcm_gport_t outlif_gport;
};

/* Initialization of global struct*/
ip_route_basic_encap_eve_s g_ip_route_basic_encap_eve =
{
    /* in_port: */
    0,
    /* out_port: */
    0,
    /* new_outer_vlan: */
    0,
    /* new_inner_vlan: */
    0,
    /* host_format: */
    0,
    /* intf_in: */
    15,
    /* intf_out: */
    100,
    /* fec: */
    40961,
    /* vrf: */
    0,
    /* encap_id: */
    0x1384,
    /* intf_in_mac_address: */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /* intf_out_mac_address: */
    {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a},
    /* arp_next_hop_mac: */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /* network_ip: */
    0x7fffff00,
    /* network_mask: */
    0xfffffff0,
    /* host_ip: */
    0x7fffff02,
    /* vlan_edit_profile: */
    0,
    /* action_id: */
    0,
    /* outlif_gport: */
    0,
};


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values 
 *
 * INPUT: 
 *   param: new values for g_ip_route_basic_encap_eve
 */
int ip_route_basic_encap_eve_init(int unit, ip_route_basic_encap_eve_s *param){
    int rv;

    if (param != NULL) {
       sal_memcpy(&g_ip_route_basic_encap_eve, param, sizeof(g_ip_route_basic_encap_eve));
    }

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);

    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2)
    {
        /* JER1*/
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

        if (!advanced_vlan_translation_mode ) {
            printf("Error, bcm886xx_vlan_translate_mode is not set\n");
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/* 
 * Return ip_route_basic_encap_eve information
 */
void ip_route_basic_encap_eve_struct_get(int unit, ip_route_basic_encap_eve_s *param){

    sal_memcpy(param, &g_ip_route_basic_encap_eve, sizeof(g_ip_route_basic_encap_eve));

    return;
}



/* This function runs the main test configuration function with given ports
 *  
 * INPUT: unit     - unit
 */
int ip_route_basic_encap_eve_with_ports__main_config__start_run(
   int unit,
   int in_port,
   int out_port,
   int new_outer_vlan,
   int new_inner_vlan,
   int vrf,
   int host_format)
{
    int rv;
    ip_route_basic_encap_eve_s param;

    if (is_device_or_above(unit, JERICHO2))
    {
       rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &g_ip_route_basic_encap_eve.fec);
       if (rv != BCM_E_NONE)
       {
           printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
           return rv;
       }
    }

    ip_route_basic_encap_eve_struct_get(unit, &param);

    param.in_port = in_port;
    param.out_port = out_port;
    param.new_outer_vlan = new_outer_vlan;
    param.new_inner_vlan = new_inner_vlan;
    param.vrf = vrf;
    param.host_format = host_format;

    return ip_route_basic_encap_eve__main_config__start_run(unit, &param);
}



/*
 * Main configuration function runs the basic route with egress vlan editing in encap stage. 
 *  
 * Main steps of configuration: 
 *    1. Initialize test parameters
 *    2. creates a basic route.
 *    
 * INPUT: unit  - unit
 *        param - new values for basic route.
 */
int ip_route_basic_encap_eve__main_config__start_run(int unit,  ip_route_basic_encap_eve_s *param){
    int rv;
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);

    rv = ip_route_basic_encap_eve_init(unit, param);

    if (is_device_or_above(unit, JERICHO2))
    {
        g_ip_route_basic_encap_eve.encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    }

    if (rv != BCM_E_NONE) {
        printf("Error, in ip_route_basic_encap_eve_init\n");
        return rv;
    }

    /*
     * Set VLAN DOMAIN
     */
    rv = bcm_port_class_set(unit, g_ip_route_basic_encap_eve.in_port, bcmPortClassId, g_ip_route_basic_encap_eve.in_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_ip_route_basic_encap_eve.in_port);
        return rv;
    }
    rv = bcm_port_class_set(unit, g_ip_route_basic_encap_eve.out_port, bcmPortClassId, g_ip_route_basic_encap_eve.out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_ip_route_basic_encap_eve.out_port);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF 
     */
    rv = in_port_intf_set(unit, g_ip_route_basic_encap_eve.in_port, g_ip_route_basic_encap_eve.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties 
     */

    rv = out_port_set(unit, g_ip_route_basic_encap_eve.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }


    /*
     * Create ETH-RIF and set its properties 
     */
    rv = intf_eth_rif_create(unit, g_ip_route_basic_encap_eve.intf_in, g_ip_route_basic_encap_eve.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, g_ip_route_basic_encap_eve.intf_out, g_ip_route_basic_encap_eve.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties 
     */
    ingress_rif.vrf = g_ip_route_basic_encap_eve.vrf;
    ingress_rif.intf_id = g_ip_route_basic_encap_eve.intf_in;

    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &g_ip_route_basic_encap_eve.encap_id, g_ip_route_basic_encap_eve.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    if (g_ip_route_basic_encap_eve.host_format != 2){
        sand_utils_l3_fec_s fec_structure;
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        BCM_GPORT_LOCAL_SET(fec_structure.destination, g_ip_route_basic_encap_eve.out_port);
        fec_structure.tunnel_gport = g_ip_route_basic_encap_eve.intf_out;
        fec_structure.tunnel_gport2 = g_ip_route_basic_encap_eve.encap_id;
        fec_structure.fec_id = g_ip_route_basic_encap_eve.fec;
        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
    }

    /*
     * Create ESEM AC
     */
    rv = out_vlan_port_intf_set(unit, g_ip_route_basic_encap_eve.out_port, g_ip_route_basic_encap_eve.intf_out, &g_ip_route_basic_encap_eve.outlif_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_vlan_port_intf_set \n");
        return rv;
    }
   

    /*
     * Add Route entry
     */
    rv = add_route_ipv4(unit, g_ip_route_basic_encap_eve.network_ip, g_ip_route_basic_encap_eve.network_mask, g_ip_route_basic_encap_eve.vrf, g_ip_route_basic_encap_eve.fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4, \n");
        return rv;
    }

    /*
     * Add host entry
     */
    BCM_GPORT_LOCAL_SET(gport, g_ip_route_basic_encap_eve.out_port);
    if(g_ip_route_basic_encap_eve.host_format == 1){
        rv = add_host_ipv4(unit, g_ip_route_basic_encap_eve.host_ip, g_ip_route_basic_encap_eve.vrf, g_ip_route_basic_encap_eve.fec, g_ip_route_basic_encap_eve.intf_out,0);
    }
    else if(g_ip_route_basic_encap_eve.host_format == 2){
        rv = add_host_ipv4(unit, g_ip_route_basic_encap_eve.host_ip, g_ip_route_basic_encap_eve.vrf, g_ip_route_basic_encap_eve.intf_out, g_ip_route_basic_encap_eve.encap_id/*arp id*/,gport);
    }
    else{
        rv = add_host_ipv4(unit, g_ip_route_basic_encap_eve.host_ip, g_ip_route_basic_encap_eve.vrf, g_ip_route_basic_encap_eve.fec, 0,0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }

    return rv;
}


/*
 * Step function runs the egress vlan editing in encap stage configuration per each step
 *  
 * Main steps of configuration: 
 *    1. Configures the EVE actions and mapping
 *    
 * INPUT: unit - unit
 *        step - which step in the configuration to set [1:NUM_OF_TEST_STEPS].
 */
int ip_route_basic_encap_eve__step_config__start_run(int unit, int step){
    int rv = BCM_E_PARAM;
 
    if ( (step < 1) || (step > NUM_OF_TEST_STEPS) ) {
        printf("Error, step = %d!!! Should be in range [1:%d]\n", step, NUM_OF_TEST_STEPS); 
        return rv;
    }

    printf("ip_route_basic_encap_eve__step_config__start_run( step = %d )\n", step);

    switch (step)
    {
        case 1:
        {
            /*
             * Set an EVE command {add,add}.
             * Map this command to tag format other than UNTAG. 
             * Expected result:
             *      no EVEC in ENCAP stage (because the command is not mapped to UNTAG format)
             */
            bcm_vlan_port_translation_t port_trans;
            bcm_vlan_action_set_t action;
            bcm_vlan_translate_action_class_t action_class;


            bcm_vlan_port_translation_t_init(&port_trans);
            port_trans.new_outer_vlan = g_ip_route_basic_encap_eve.new_outer_vlan;
            port_trans.new_inner_vlan = g_ip_route_basic_encap_eve.new_inner_vlan;
            port_trans.gport = g_ip_route_basic_encap_eve.outlif_gport;
            port_trans.vlan_edit_class_id = g_ip_route_basic_encap_eve.vlan_edit_profile;
            port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
            rv = bcm_vlan_port_translation_set(unit, &port_trans);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_port_translation_set\n");
                return rv;
            }

            /*
             * Create action ID
             */
            rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_ip_route_basic_encap_eve.action_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_translate_action_id_create\n");
                return rv;
            }

            /*
             * Set translation action 
             */
            bcm_vlan_action_set_t_init(&action);
            action.outer_tpid = 0x9100;
            action.inner_tpid = 0x8100;
            action.dt_outer = bcmVlanActionAdd;
            action.dt_inner = bcmVlanActionAdd;
            rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, g_ip_route_basic_encap_eve.action_id, &action);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_translate_action_id_set\n");
                return rv;
            }

            /*
             * Set translation action class (map edit_profile & tag_format to action_id).
             * tag_format is other then UNTAG (that is other then zero)
             */
            bcm_vlan_translate_action_class_t_init(&action_class);
            action_class.vlan_edit_class_id = g_ip_route_basic_encap_eve.vlan_edit_profile;
            action_class.tag_format_class_id = 4;
            action_class.vlan_translation_action_id = g_ip_route_basic_encap_eve.action_id;
            action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
            rv = bcm_vlan_translate_action_class_set(unit, &action_class);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_translate_action_class_set\n");
                return rv;
            }

            break;        
        }

        case 2:
        {
            /* 
             * Map this EVE command to UNTAG tag format.
             * Send a packet. 
             * Expected result:
             *      EVEC in ENCAP stage is performed (because the command is mapped to UNTAG format)
 */
            bcm_vlan_translate_action_class_t action_class;

            /*
             * Set translation action class (map edit_profile & tag_format to action_id).
             * tag_format is UNTAG (that is zero)
             */
            bcm_vlan_translate_action_class_t_init(&action_class);
            action_class.vlan_edit_class_id = g_ip_route_basic_encap_eve.vlan_edit_profile;
            action_class.tag_format_class_id = 0;
            action_class.vlan_translation_action_id = g_ip_route_basic_encap_eve.action_id;
            action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
            rv = bcm_vlan_translate_action_class_set(unit, &action_class);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_translate_action_class_set\n");
                return rv;
            }

            break;        
        }

        case 3:
        {
            /*
             * Re-set this EVE command {MappedAdd,MappedAdd}.
             * Send a packet. 
             * Expected result:
             *      EVEC in ENCAP stage is performed (because the command is mapped to UNTAG format)
             */
            bcm_vlan_action_set_t action;

            /*
             * Set translation action 
             */
            bcm_vlan_action_set_t_init(&action);
            action.outer_tpid = 0x9100;
            action.inner_tpid = 0x8100;
            action.dt_outer = bcmVlanActionMappedAdd;
            action.dt_inner = bcmVlanActionMappedAdd;
            rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, g_ip_route_basic_encap_eve.action_id, &action);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_translate_action_id_set\n");
                return rv;
            }

            break;        
        }
        case 4:
        {
            uint32 flags;
            bcm_vlan_action_set_t vlan_action;
            int action_id;
            bcm_vlan_translate_action_class_t action_class;
            int vlan_edit_profile, tag_format_index;
            int tag_format_array[NOF_TAG_FORMAT_CREATED] = { ETHERNET_FRAME_VLAN_FORMAT_NONE, ETHERNET_FRAME_VLAN_FORMAT_S_TAG,
                    ETHERNET_FRAME_VLAN_FORMAT_C_TAG, ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG, ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG};
            /*
             * Allocate IVE & EVE Reserved Command IDs
             */
            flags = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
            action_id = 0;
            rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
            flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
            action_id = 0;
            rv =  bcm_vlan_translate_action_id_create(unit, flags, &action_id);
            flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
            action_id = 1;
            rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
            /*
             * Configure the reserved IVE & EVE Command IDs
             * IVEC 0 - NOP
             * EVEC 0 - Add VSI (for untagged)
             * EVEC 1 - NOP (for tagged)
             */
            bcm_vlan_action_set_t_init(&vlan_action);
            vlan_action.dt_outer = bcmVlanActionNone;
            vlan_action.dt_inner = bcmVlanActionNone;
            vlan_action.dt_outer_pkt_prio = bcmVlanActionNone;
            vlan_action.dt_inner_pkt_prio = bcmVlanActionNone;
            vlan_action.outer_tpid = BCM_SWITCH_TPID_VALUE_INVALID;
            vlan_action.inner_tpid = BCM_SWITCH_TPID_VALUE_INVALID;
            /*
             * First set the NOP Command IDs
             */
            rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, 0, &vlan_action);
            rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 1, &vlan_action);
            /*
             * Set the EVEC 0 entry with added VSI
             */
            vlan_action.dt_outer = bcmVlanActionMappedAdd;
            vlan_action.dt_inner = bcmVlanActionNone;
            vlan_action.dt_outer_pkt_prio = bcmVlanActionAdd;
            vlan_action.dt_inner_pkt_prio = bcmVlanActionNone;
            vlan_action.outer_tpid = 0x8100;
            vlan_action.inner_tpid = BCM_SWITCH_TPID_VALUE_INVALID;
            rv =  bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &vlan_action);

            /*
             * Perform IVEC mapping:
             *  - IVEC Untag - 0 - NOP: since all default values are 0, nothing to do here.
             */

            /*
             * Perform EVEC mapping:
             *  - EVEC Untag - 0 - Add VSI: since all default values are 0, nothing to do here.
             *  - EVEC all others - 1 - NOP: need to map all entries but entry 0 to action ID 1.
             */
            bcm_vlan_translate_action_class_t_init(&action_class);

            action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
            action_class.vlan_translation_action_id = 1;

            /*
             * First, sets all the entries to action ID 1 (NOP):
             * vlan_edit_profile is 6 bits
             * tag_format is 5 bits
             */
            for (vlan_edit_profile = 0; vlan_edit_profile < (1 << 6); vlan_edit_profile++)
            {
                action_class.vlan_edit_class_id = vlan_edit_profile;

                for (tag_format_index = 0; tag_format_index < NOF_TAG_FORMAT_CREATED; tag_format_index++)
                {
                    action_class.tag_format_class_id = tag_format_array[tag_format_index];

                    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
                }
            }

            /*
             * Second, set back entry 0 (UNTAG) to action ID 0 (Add VSI):
             */
            action_class.vlan_translation_action_id = 0;
            action_class.vlan_edit_class_id = 0;
            action_class.tag_format_class_id = 0;
            rv = bcm_vlan_translate_action_class_set(unit, &action_class);

            break;        
        }

        default:
        {
            printf("Error, step = %d!!! Should be in range [1:%d]\n", step, NUM_OF_TEST_STEPS); 
            rv = BCM_E_CONFIG;
        }

    }


    return rv;
}
