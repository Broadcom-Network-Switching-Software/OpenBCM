/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 *
 * This cint provides a basic OAM example by creating vswitch on 3 given ports
 * and the following endpoints:
 * 1) Non accelerated down MEP on port1
 * 2) Accelerated up MEP on port2
 * 3) Accelerated down MEP on port1
 *
 * How to run:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_oam.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_tpid.c
  cint ../../../../src/examples/dnx/vswitch/cint_dnx_vswitch_metro_mp.c
  cint ../../../../src/examples/dnx/cint_dnx_advanced_vlan_translation_mode.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
  cint ../../../../src/examples/sand/cint_l2_traverse.c
  cint ../../../../src/examples/dnx/oam/cint_dnx_oam.c
  cint
  int port1 = 200;
  int port2 = 201;
  int port3 = 202;
  int rv =0;
  int unit =0;
  rv = oam_run_with_defaults(unit, port1, port2, port3);
 ***
 OAMP-v2 requires calling:
 
 int recycle_port=221;
 dnx_oam__switch_contol_indexed_port_set(unit,recycle_port,BCM_SWITCH_PORT_HEADER_TYPE_IBCH1_MODE,1);
 
 This is required on a recycle port on each core where a Up MEP packet will be trapped
 ***
 
 * Test Scenario - end
 *
 * This cint uses cint_vswitch_metro_mp_single_vlan.c to build the following vswitch:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                       |
 *  |                   o  \                         o                      |
 *  |                       \  \<4>       <10,20>/  /                       |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |                <40>\  \  |    \  /     |\/ /  /                       |
 *  |                     \  \ |     \/      |  /  /<30>                    |
 *  |                       p3\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1                           |
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               |
 *  |                   /\  /p2  -----------                                |
 *  |                <5>/  /  /                                             |
 *  |                  /  /  /<3>                                           |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */

/* Md level of mip */
int MP_LEVEL_MIP = 7;

bcm_gport_t gport1, gport2, gport3, gport4; /* these are the ports created by the vswitch*/

/*
 * OAM snoop configuration parameters
 */
struct oam_snoop_config_s
{
    bcm_gport_t dest_snoop_port;
    bcm_oam_opcode_t opcode;
    bcm_oam_dest_mac_type_t mac_type;
    uint8 counter_increment;
    bcm_oam_profile_t profile_id;
    int is_ingress;
};

/*
 * Used by oam_change_mep_destination_to_snoop
 * to create and configure an additional snoop
 */
oam_snoop_config_s snoop_config;

/*
 * OAM endpoint configuration parameters
 */
struct dnx_oam_ep_gobal_config_s
{
  bcm_oam_endpoint_t down_mep_id;
  bcm_oam_endpoint_t up_mep_id;

  bcm_vlan_t  vsi;

  /* MAC addresses , ports & gports*/
  bcm_mac_t mac_mep_1;
  bcm_mac_t mac_mep_2;
  bcm_mac_t mac_mep_3;
  bcm_mac_t mac_mep_4;
  bcm_mac_t src_mac_mep_2;
  bcm_mac_t src_mac_mep_3;
  bcm_mac_t src_mac_mep_4;

  /*1-const; 32-long format; 13-length; all the rest - MA name*/
  uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH];
  /* 48 bytes format */
  uint8 str_48b_name[BCM_OAM_GROUP_NAME_LENGTH];
  int use_48_maid;

 /* physical port (signal generator)*/
  int port_1;
  int port_2;
  int port_3;

  /* multicast gports */
  int mc_gport_1;
  int mc_gport_2;
  int mc_gport_3;

  int sampling_ratio;
  int use_port_interface_status;
  /* Enable LOC events as soon as the remote endpoints are created */
  int remote_meps_start_in_loc_enable;

  /* OAM Action profile */
  uint32 mp_profile;

  /* VLAN ports for standalone */
  bcm_vlan_port_t vp1, vp2;
  /* VLAN ports for FLOW initiator ports */
  bcm_vlan_port_t initiator_vp1, initiator_vp2;

  /**
  * This is for oamp-v1 only
  * When set to "1" allocate 8k counters in database 5 for TX packets.
  * When set to "2" allocate 8k counters in database 6 for RX packets.
  * When set to "3" allocate 8k counters in database 5 for TX packets and 8k counters in database 6 for RX packets.
  */
  int enable_oamp_counter_allocation;
};

/* Endpoint info */
struct oam__ep_info_s {
    int id;
    int rmep_id;
    bcm_gport_t gport;
    bcm_gport_t mpls_out_gport;
    int out_lif;
    int label;
    int lm_counter_base_id;
    int lm_counter_if;
};
int NUMBER_OF_TPID = 2;
uint16 jr2_tpid[NUMBER_OF_TPID] = {0x8100, 0x9100};

/*
 * Structure for returned information
 */
struct dnx_oam_results_count_s
{
   /** Event counters */
  int port_interface_event_count;
  int timeout_events_count;
  int remote_event_count;
  int timeout_events_count_multi_oam[5];

  /* Group structures */
  bcm_oam_group_info_t group_info_gl;
  bcm_oam_group_info_t group_info_48b_ma;

  /* Created endpoints information */
  oam__ep_info_s ep1;
  oam__ep_info_s ep2;
  oam__ep_info_s ep3;
  oam__ep_info_s ep4;
  oam__ep_info_s ep_def;
  /** Trap code for protection packets */
  int protection_trap;
  /** Trap code from configurable MAID miss punt */
  int maid_punt_trap;
  /** My CFM MAC */
  int station_id[4];
};


/*
 * OAM endpoint init configuration
 */
dnx_oam_ep_gobal_config_s dnx_oam_endpoint_info_glb = {
 /* down_mep_id */
 0,
 /* up_mep_id */
 0,
 /* vsi */
 0,
 /* mac addresses */
 {0x00, 0x00, 0x00, 0x01, 0x02, 0x03},
 {0x00, 0x00, 0x00, 0xff, 0xff, 0xff},
 {0x00, 0x00, 0x00, 0x01, 0x02, 0xff},
 {0x00, 0x00, 0x00, 0xff, 0xff, 0xfd},
 {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},
 {0x00, 0x01, 0x02, 0x03, 0x04, 0x01},
 {0x00, 0x01, 0x02, 0x03, 0x04, 0x07},
 /* Long Name: 1-const; 32-long format; 13-length; all the rest - MA name*/
 {1, 32, 13, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0xa, 0xb, 0xc, 0xd},
 /* 48 bytes format */
 {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3},
 /* use_48_maid */
 0,
 /* physical ports port_1, port_2, port_3 */
 13,14,15,
 /* sampling_ratio */
 0,
 /* use_port_interface_status */
 0,
 /* remote_meps_start_in_loc_enable */
 0,
 /* OAM Action profile */
 0,
 /* OAMP counter allocated for oamp-v1 only */
 0
};

/*
 * Returned information
 */
dnx_oam_results_count_s dnx_oam_results_glb = {
 /* port_interface_event_count , timeout_events_count , remote_event_count*/
 0,0,0,
 /* timeout_events_count_multi_oam */
 { 0 },
 /* Group structures */
 { 0 },
 { 0 },
 /* Created endpoints information */
 { 0 },
 { 0 },
 { 0 },
 { 0 },
 { 0 },
   /* protection_trap */
   0,
   /* maid_punt_trap */
   0,
   /* station_id */
 { 0 }
};

/*****************************************************************************/
/*                                        OAM BASIC EXAMPLE                                                                               */
/*****************************************************************************/

/**
 * Functions used to read various event counters.
 *
 * @param expected_event_count
 *
 * @return int
 */
int read_timeout_event_count(int expected_event_count) {
    printf("timeout_events_count=%d\n",dnx_oam_results_glb.timeout_events_count);

    if (dnx_oam_results_glb.timeout_events_count==expected_event_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

/**
 * Function verifies that for each of the two accelerated 
 * endpoints, the cb was called as many times as expected. 
 * 
 * @author sinai (24/11/2013)
 * 
 * @param expected_event_count 
 * 
 * @return int 
 */
int read_timeout_event_count_multi_oam(int expected_event_count_on_ep_1,int expected_event_count_on_ep_2) {
    int i;
    if (dnx_oam_results_glb.timeout_events_count_multi_oam[dnx_oam_results_glb.ep2.id & 0xff] != expected_event_count_on_ep_1 ||  dnx_oam_results_glb.timeout_events_count_multi_oam[dnx_oam_results_glb.ep3.id & 0xff] != expected_event_count_on_ep_2 ) {
        return BCM_E_FAIL;
    }
  
    return BCM_E_NONE;
}


int read_remote_timeout_event_count(int expected_count) {
    printf("timeout_events_count=%d\n",dnx_oam_results_glb.remote_event_count);

    if (dnx_oam_results_glb.remote_event_count==expected_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

int read_port_interface_event_count(int expected_count) {
    printf("port_interface_event_count=%d\n",dnx_oam_results_glb.port_interface_event_count);


    if (dnx_oam_results_glb.port_interface_event_count==expected_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

/* This is an example of using bcm_oam_event_register api.
 * A simple callback is created for CCM timeout event.
 * After a mep and rmep are created, the callback is called
 * whenever CCMTimeout event is generated.
 */
int cb_oam(int unit,
           uint32 flags,
           bcm_oam_event_type_t event_type,
           bcm_oam_group_t group,
           bcm_oam_endpoint_t endpoint,
           void *user_data) {
    int et = event_type, gp = group, ep = endpoint;
    printf("UNIT: %d, FLAGS: %d, EVENT: %d, GROUP: %d, MEP: 0x%0x\n",unit,flags,et,gp,ep);

    if ((event_type == bcmOAMEventEndpointInterfaceDown) || (event_type == bcmOAMEventEndpointPortDown) ||
          (event_type == bcmOAMEventEndpointInterfaceUp) || (event_type == bcmOAMEventEndpointPortUp)) {
    	dnx_oam_results_glb.port_interface_event_count++;
    } else {
        dnx_oam_results_glb.timeout_events_count++;
    }

    if (flags & BCM_OAM_EVENT_FLAGS_MULTIPLE) {
    	dnx_oam_results_glb.timeout_events_count_multi_oam[endpoint & 0xff]++;
    }

    if (event_type == bcmOAMEventEndpointRemote || event_type == bcmOAMEventEndpointRemoteUp) {
    	dnx_oam_results_glb.remote_event_count++;
    }

    return BCM_E_NONE;
}

int register_events(int unit) {
  bcm_oam_event_types_t timeout_event, timein_event, port_interface_event;

  BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, timeout_event, cb_oam, (void*)1), "");

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, timein_event, cb_oam, (void*)2), "");

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemote);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, timein_event, cb_oam, (void*)3), "");

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemoteUp);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, timein_event, cb_oam, (void*)4), "");

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortDown);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)5), "");

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceDown);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)6), "");

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortUp);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)7), "");

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceUp);
  BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)8), "");

  return BCM_E_NONE;
}

/**
 * Cint that only uses bcm APIs to create non accelerated Up,
 * Down MEP
 * @param unit
 * @param port1
 * @param vid1 - vlan ID of port1
 * @param port2
 * @param vid2 - vlan ID of port2
 * Allocate counter engines, and get counters.
 * 1) non acc down MEP on port1
 * 2) non acc up MEP on port2
 * @return int
 */
int oam_standalone_example(int unit,int port1, int vid1, int port2, int vid2) {

    bcm_oam_endpoint_info_t mep_not_acc_info;
    int md_level_1 = 2;
    int md_level_2 = 5;
    int lm_counter_base_id_1 = 0, lm_counter_base_id_2 = 0;

    bcm_vswitch_cross_connect_t cross_connect;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    int counter_if_0 = 0, counter_if_1 = 1;

    int device_type_v2;

    device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));

    /*
     * Allocate counter engines, and get counters
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, port1, counter_if_0, 1, &lm_counter_base_id_1), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, port2, counter_if_1, 1, &lm_counter_base_id_2), "");

    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port1, bcmPortClassId, port1), "");
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port2, bcmPortClassId, port2), "");


    bcm_vlan_port_t_init(&dnx_oam_endpoint_info_glb.vp1);
    dnx_oam_endpoint_info_glb.vp1.flags = 0;
    dnx_oam_endpoint_info_glb.vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    dnx_oam_endpoint_info_glb.vp1.port = port1;
    dnx_oam_endpoint_info_glb.vp1.match_vlan = vid1;
    dnx_oam_endpoint_info_glb.vp1.egress_vlan = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit,&dnx_oam_endpoint_info_glb.vp1), "");

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", dnx_oam_endpoint_info_glb.vp1.port, dnx_oam_endpoint_info_glb.vp1.match_vlan,
           dnx_oam_endpoint_info_glb.vp1.vlan_port_id, dnx_oam_endpoint_info_glb.vp1.encap_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid1, port1, 0), "");

    bcm_vlan_port_t_init(&dnx_oam_endpoint_info_glb.vp2);
    dnx_oam_endpoint_info_glb.vp2.flags = 0;
    dnx_oam_endpoint_info_glb.vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    dnx_oam_endpoint_info_glb.vp2.port = port2;
    dnx_oam_endpoint_info_glb.vp2.match_vlan = vid2;
    dnx_oam_endpoint_info_glb.vp2.egress_vlan = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit,&dnx_oam_endpoint_info_glb.vp2), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid2, port2, 0), "");

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", dnx_oam_endpoint_info_glb.vp2.port, dnx_oam_endpoint_info_glb.vp2.match_vlan,
           dnx_oam_endpoint_info_glb.vp2.vlan_port_id, dnx_oam_endpoint_info_glb.vp2.encap_id);

    /**
    * Cross-connect the ports
    */
    bcm_vswitch_cross_connect_t_init(&cross_connect);

    cross_connect.port1 = dnx_oam_endpoint_info_glb.vp1.vlan_port_id;
    cross_connect.port2 = dnx_oam_endpoint_info_glb.vp2.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cross_connect), "");

     bcm_oam_group_info_t_init(&dnx_oam_results_glb.group_info_gl);
     sal_memcpy(dnx_oam_results_glb.group_info_gl.name, dnx_oam_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
     BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_results_glb.group_info_gl), "MEG:");


    /*
     * Get default profile encoded values
     * Default profile is 1 and
     * configured on init(application) stage.
   * Relevant for Jr2 ONLY
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, dnx_oam_endpoint_info_glb.vp1.vlan_port_id, ingress_profile, egress_profile), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, dnx_oam_endpoint_info_glb.vp2.vlan_port_id, ingress_profile, egress_profile), "");

    /*
    * Adding non acc down MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = dnx_oam_results_glb.group_info_gl.id;
    mep_not_acc_info.level = md_level_1;
    mep_not_acc_info.gport = dnx_oam_endpoint_info_glb.vp1.vlan_port_id;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, port1, dnx_oam_endpoint_info_glb.mac_mep_1), "");
    dnx_oam_results_glb.station_id[0] = gl_station_id;

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
    mep_not_acc_info.lm_counter_if = counter_if_0;
    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_not_acc_info), "");
    printf("created Down MEP with id %d\n", mep_not_acc_info.id);
    dnx_oam_results_glb.ep1.gport = mep_not_acc_info.gport;
    dnx_oam_results_glb.ep1.id = mep_not_acc_info.id;
    dnx_oam_results_glb.ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;


    /**
    * Adding non acc UP MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = dnx_oam_results_glb.group_info_gl.id;
    mep_not_acc_info.level = md_level_2;
    mep_not_acc_info.gport = dnx_oam_endpoint_info_glb.vp2.vlan_port_id;
    mep_not_acc_info.tx_gport = BCM_GPORT_INVALID;
    mep_not_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    /* Setting My-CFM-MAC */
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, port2, dnx_oam_endpoint_info_glb.mac_mep_2), "");
    dnx_oam_results_glb.station_id[1] = gl_station_id;

    mep_not_acc_info.lm_counter_base_id = lm_counter_base_id_2;
    mep_not_acc_info.lm_counter_if = counter_if_1;

    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_not_acc_info), "");
    printf("created UP MEP with id %d\n", mep_not_acc_info.id);
    dnx_oam_results_glb.ep2.gport = mep_not_acc_info.gport;
    dnx_oam_results_glb.ep2.id = mep_not_acc_info.id;
    dnx_oam_results_glb.ep2.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;

    return BCM_E_NONE;
}

/* Example of destroying flow lifs for OAM */
int oam_flow_gport_destroy(int unit, int port, int vid, int gport, int initiator_gport)
{
    char *flow_name;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_field_id_t field_id = 0;
    bcm_flow_special_fields_t special_fields;
    uint32 vlan_domain;
    char error_msg[200]={0,};

    /* ------------------------------- */
    /* Delete flow match configuration */
    /* ------------------------------- */
    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);

    flow_name = "VLAN_PORT_LL_IN_AC_C_VLAN_TERM_MATCH";
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, flow_name, flow_handle), "");

    /* Special fields */
    special_fields.actual_nof_special_fields = 2;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "C_VID", &field_id), "");
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = vid;

    snprintf(error_msg, sizeof(error_msg), "port = %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, port, bcmPortClassId, &vlan_domain), error_msg);
    printf("port = %d, vlan_domain = %d \n", port, vlan_domain);

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id), "");
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = vlan_domain;

    flow_handle_info.flow_id = gport;
    flow_handle_info.flow_handle = flow_handle;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_match_info_delete(unit, &flow_handle_info, &special_fields), "");

    /* ------------------------------- */
    /* Delete flow initiator  lif      */
    /* ------------------------------- */
    flow_name = "VLAN_PORT_LL_INITIATOR";

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, flow_name, &flow_handle), "");
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = initiator_gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_destroy(unit, &flow_handle_info), "");

    /* ------------------------------- */
    /* Delete flow terminator lif      */
    /* ------------------------------- */
    flow_name = "VLAN_PORT_LL_TERMINATOR";

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, flow_name, &flow_handle), "");
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_terminator_info_destroy(unit, &flow_handle_info), "");

    return BCM_E_NONE;
}

/* Example of destroying flow lifs for OAM */
int oam_flow_stand_alone_destroy(int unit,int port1, int vid1, int port2, int vid2) {

    BCM_IF_ERROR_RETURN_MSG(oam_flow_gport_destroy(unit,port1,vid1,dnx_oam_endpoint_info_glb.vp1.vlan_port_id,dnx_oam_endpoint_info_glb.initiator_vp1.vlan_port_id), "");
    BCM_IF_ERROR_RETURN_MSG(oam_flow_gport_destroy(unit,port2,vid2,dnx_oam_endpoint_info_glb.vp2.vlan_port_id,dnx_oam_endpoint_info_glb.initiator_vp2.vlan_port_id), "");
    printf("flow ports destroyed\n");
    return BCM_E_NONE;
}

/*
 *  Example of creating flow lifs for OAM
 * Symetric initiator and terminator lifs are created
 */
int oam_flow_gport_create(int unit, int port, int vid, int *gport, int *initiator_gport)
{
    bcm_flow_initiator_info_t initiator_info;
    bcm_flow_terminator_info_t terminator_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_field_id_t field_id = 0;
    char *flow_name;
    uint32 vlan_domain;
    char error_msg[200]={0,};

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_initiator_info_t_init(&initiator_info);
    bcm_flow_terminator_info_t_init(&terminator_info);

    int cint_flow_ac_lif__encap_id;
    int cint_flow_ac_lif__flow_id = -1;

    /* ------------------------------- */
    /* Create flow initiator lif       */
    /* ------------------------------- */
    flow_name = "VLAN_PORT_LL_INITIATOR";
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, flow_name, &flow_handle), "");

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC;

    initiator_info.valid_elements_set = BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID;
    initiator_info.l2_egress_info.egress_network_group_id = cint_flow_ac_lif_initiator_basic_info.egress_network_group_id;
    if (*dnxc_data_get(unit, "lif", "global_lif", "global_lif_group_max_val", NULL))
    {
        initiator_info.valid_elements_set = BCM_FLOW_INITIATOR_ELEMENT_OUTLIF_PROFILE_GROUP_VALID;
        initiator_info.outlif_profile_group = 1;
    }
    initiator_info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_OAM_SET_VALID;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_initiator_info_create(unit, &flow_handle_info, &initiator_info, &special_fields), "");

    cint_flow_ac_lif__encap_id = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id);
    cint_flow_ac_lif__flow_id = flow_handle_info.flow_id;

    /* Return initiator lif */
    *initiator_gport = cint_flow_ac_lif__flow_id;

    printf("Initiator lif created encap_id = %d, flow_id = 0x%08X \n", cint_flow_ac_lif__encap_id, cint_flow_ac_lif__flow_id);

    /* ------------------------------- */
    /* Create flow initiator lif       */
    /* ------------------------------- */
    flow_name = "VLAN_PORT_LL_TERMINATOR";
    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, flow_name, &flow_handle), "");

    terminator_info.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_OAM_SET_VALID;

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
    /* Use initiator flow id and create "with_id" */
    BCM_GPORT_SUB_TYPE_IN_LIF_SET(flow_handle_info.flow_id, cint_flow_ac_lif__flow_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_terminator_info_create(unit, &flow_handle_info, &terminator_info, &special_fields), "");

    cint_flow_ac_lif__encap_id = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info.flow_id);
    cint_flow_ac_lif__flow_id = flow_handle_info.flow_id;

     printf("Terminator lif created encap_id = %d, flow_id = 0x%08X \n", cint_flow_ac_lif__encap_id, cint_flow_ac_lif__flow_id);

     /* Return terminator lif */
     *gport = cint_flow_ac_lif__flow_id;

     /* ------------------------------- */
     /* Create flow match               */
     /* ------------------------------- */
    flow_name = "VLAN_PORT_LL_IN_AC_C_VLAN_TERM_MATCH";

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_handle_get(unit, flow_name, &flow_handle), "");

    /* Special fields */
    special_fields.actual_nof_special_fields = 2;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "C_VID", &field_id), "");
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = vid;

    snprintf(error_msg, sizeof(error_msg), "port = %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, port, bcmPortClassId, &vlan_domain), error_msg);

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_logical_field_id_get(unit, flow_handle, "VLAN_DOMAIN", &field_id), "");
    special_fields.special_fields[1].field_id = field_id;
    special_fields.special_fields[1].shr_var_uint32 = vlan_domain;

    flow_handle_info.flow_id = cint_flow_ac_lif__flow_id;
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;

    BCM_IF_ERROR_RETURN_MSG(bcm_flow_match_info_add(unit, &flow_handle_info, &special_fields), "");
    printf("FLOW match configured - port = %d, vid %d, vlan_domain = %d \n", port, vid, vlan_domain);

    return BCM_E_NONE;

}

/**
 * Cint that only uses bcm APIs to create non accelerated up and down MEP
 * @param unit
 * @param port1
 * @param vid1 - vlan ID of port1
 * @param port2
 * @param vid2 - vlan ID of port2
 * Allocate counter engines, and get counters.
 * 1) non acc down MEP on port1
 * @return int
 */
int oam_flow_standalone_example(int unit,int port1, int vid1, int port2, int vid2) {

    bcm_oam_endpoint_info_t mep_not_acc_info;
    int md_level_1 = 2;
    int md_level_2 = 5;
    int lm_counter_base_id_1 = 0, lm_counter_base_id_2 = 0;

    bcm_vswitch_cross_connect_t cross_connect;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    int counter_if_0 = 0, counter_if_1 = 1;

    int device_type_v2;

    device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));

    /*
     * Allocate counter engines, and get counters
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, port1, counter_if_0, 1, &lm_counter_base_id_1), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, port2, counter_if_1, 1, &lm_counter_base_id_2), "");

    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port1, bcmPortClassId, port1), "");
    /* Set port classification ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port2, bcmPortClassId, port2), "");

    BCM_IF_ERROR_RETURN_MSG(oam_flow_gport_create(unit,port1,vid1,&dnx_oam_endpoint_info_glb.vp1.vlan_port_id,&dnx_oam_endpoint_info_glb.initiator_vp1.vlan_port_id),"");
    printf("gport created: 0x%x\n",dnx_oam_endpoint_info_glb.vp1.vlan_port_id);
    BCM_IF_ERROR_RETURN_MSG(oam_flow_gport_create(unit,port2,vid2,&dnx_oam_endpoint_info_glb.vp2.vlan_port_id,&dnx_oam_endpoint_info_glb.initiator_vp2.vlan_port_id),"");
    printf("gport created: 0x%x\n",dnx_oam_endpoint_info_glb.vp2.vlan_port_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid1, port1, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid2, port2, 0), "");

     bcm_oam_group_info_t_init(&dnx_oam_results_glb.group_info_gl);
     sal_memcpy(dnx_oam_results_glb.group_info_gl.name, dnx_oam_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);
     BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_results_glb.group_info_gl), "MEG:");

    /*
     * Get default profile encoded values
     * Default profile is 1 and
     * configured on init(application) stage.
     * Relevant for Jr2 ONLY
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, dnx_oam_endpoint_info_glb.vp1.vlan_port_id, ingress_profile, egress_profile), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, dnx_oam_endpoint_info_glb.vp2.vlan_port_id, ingress_profile, egress_profile), "");

    /*
    * Adding non acc down MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = dnx_oam_results_glb.group_info_gl.id;
    mep_not_acc_info.level = md_level_1;
    mep_not_acc_info.gport = dnx_oam_endpoint_info_glb.vp1.vlan_port_id;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, port1, dnx_oam_endpoint_info_glb.mac_mep_1), "");
    dnx_oam_results_glb.station_id[0] = gl_station_id;

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
    mep_not_acc_info.lm_counter_if = counter_if_0;
    printf("bcm_oam_endpoint_create mep_not_acc_info\n");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_not_acc_info), "");
    printf("created Down MEP with id %d\n", mep_not_acc_info.id);
    dnx_oam_results_glb.ep1.gport = mep_not_acc_info.gport;
    dnx_oam_results_glb.ep1.id = mep_not_acc_info.id;
    dnx_oam_results_glb.ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;

    /**
    * Adding non acc UP MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = dnx_oam_results_glb.group_info_gl.id;
    mep_not_acc_info.level = md_level_2;
    mep_not_acc_info.gport = dnx_oam_endpoint_info_glb.vp2.vlan_port_id;
    mep_not_acc_info.tx_gport = BCM_GPORT_INVALID;
    mep_not_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    /* Setting My-CFM-MAC */
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, port2, dnx_oam_endpoint_info_glb.mac_mep_2), "");
    dnx_oam_results_glb.station_id[1] = gl_station_id;

    mep_not_acc_info.lm_counter_base_id = lm_counter_base_id_2;
    mep_not_acc_info.lm_counter_if = counter_if_1;

    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_not_acc_info), "");
    printf("created UP MEP with id %d\n", mep_not_acc_info.id);
    dnx_oam_results_glb.ep2.gport = mep_not_acc_info.gport;
    dnx_oam_results_glb.ep2.id = mep_not_acc_info.id;
    dnx_oam_results_glb.ep2.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;

    return BCM_E_NONE;
}

/**
 * Basic OAM example.
 * Creates vswitch on 3 given ports and the folowing endpoint:
 * 1) Non accelerated down MEP on port1
 * 2) Accelerated up MEP on port2 + RMEP
 * 3) Accelerated down MEP on port1 + RMEP
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_run_with_defaults(int unit, int port1, int port2, int port3) {

    single_vlan_tag = 1;

    dnx_oam_endpoint_info_glb.port_1 = port1;
    dnx_oam_endpoint_info_glb.port_2 = port2;
    dnx_oam_endpoint_info_glb.port_3 = port3;
    dnx_oam_endpoint_info_glb.vsi = 2;

    printf("Registering OAM events\n");
    BCM_IF_ERROR_RETURN_MSG(register_events(unit), "");

    BCM_IF_ERROR_RETURN_MSG(create_vswitch_and_mac_entries(unit), "");

    BCM_IF_ERROR_RETURN_MSG(oam_example(unit), "");

    if (dnx_oam_endpoint_info_glb.enable_oamp_counter_allocation)
    {
        /** This is for oamp-v1 only. */
        /** Allocate counters for 4k MEPs with statistics enable, counter base is the first MEP. */
        /** crps_oam_config_enable_eviction variable is expected to
         *  be set from outside. Should be set to 1 for per-endpoint-statistics */
        BCM_IF_ERROR_RETURN_MSG(cint_oam_oamp_statistics_main(unit, dnx_oam_results_glb.ep2.id, 4192, (dnx_oam_endpoint_info_glb.enable_oamp_counter_allocation-1), 0), "");
    }

    return BCM_E_NONE;
}


/**
 * Basic OAM example. creates the following entities:
 * 1) vswitch on which OAM endpoints are defined.
 * 2) OAM group with short MA name (set on outgoing CCMs for
 * accelerated endpoints).
 * 3) Default endpoint.
 * 4) Non accelerated endpoint
 * 5) Accelerated down MEP
 * 6) Accelerated up MEP
 *
 * @param unit
 *
 * @return int
 */
int oam_example(int unit) {
    bcm_oam_group_info_t *group_info;
    bcm_oam_endpoint_info_t mep_not_acc_info;
    bcm_oam_endpoint_info_t mep_acc_info;
    bcm_oam_endpoint_info_t rmep_info;
    bcm_oam_endpoint_info_t default_info;

    int md_level_1 = 4;
    int md_level_2 = 5;
    int md_level_3 = 2;
    int md_level_4 = 1;
    int lm_counter_base_id_1;
    int lm_counter_base_id_2;
    int lm_counter_base_id_3;
    int lm_counter_base_id_4;
    int counter_if_0 = 0;
    int counter_if_1 = 1;

    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;

    int dev_type_val;
    int device_type_v2;

    device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));

    printf("Creating OAM group \n");
    bcm_oam_group_info_t_init(&dnx_oam_results_glb.group_info_gl);
    sal_memcpy(dnx_oam_results_glb.group_info_gl.name, dnx_oam_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_results_glb.group_info_gl), "");

    /*
     * Allocate counter engines and get counters. The counter_if is used for
     * retrieving counters on different engines of the same port. For each
     * counter interface, OAM CRPS database should be configured first on the
     * core that the port belongs to. Counter engines will be added to the database.
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_endpoint_info_glb.port_1, counter_if_0, 1, &lm_counter_base_id_1), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_endpoint_info_glb.port_1, counter_if_1, 1, &lm_counter_base_id_2), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_endpoint_info_glb.port_2, counter_if_0, 1, &lm_counter_base_id_3), "");
    if(!device_type_v2)
    {
        BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_endpoint_info_glb.port_2, counter_if_1, 1, &lm_counter_base_id_4), "");
    }

    if (dnx_oam_endpoint_info_glb.use_48_maid) {
        /* Add a group with flexible 48 Byte MAID */
        bcm_oam_group_info_t_init(&dnx_oam_results_glb.group_info_48b_ma);
        sal_memcpy(dnx_oam_results_glb.group_info_48b_ma.name, dnx_oam_endpoint_info_glb.str_48b_name, BCM_OAM_GROUP_NAME_LENGTH);
        dnx_oam_results_glb.group_info_48b_ma.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
        if (!device_type_v2) {
            dnx_oam_results_glb.group_info_48b_ma.group_name_index = 0xA008;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_results_glb.group_info_48b_ma), "");
        /* Set the used group for the MEPs to this group */
        group_info = &dnx_oam_results_glb.group_info_48b_ma;
    }
    else {
        /* Set the used group for the MEP */
        group_info = &dnx_oam_results_glb.group_info_gl;
    }

    /*
     * Get default profile encoded values
     * Default profile is 1 and
     * configured on init(application) stage.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile), "(accelerated ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile), "(accelerated egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile), "");


    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport3, ingress_profile, egress_profile), "");


    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_endpoint_info_glb.port_1,  dnx_oam_endpoint_info_glb.mac_mep_1), "");

    dnx_oam_results_glb.station_id[0] = gl_station_id;

    /* Non acc mep associated with maid 48 byte group is not supported on JR2*/
    if(!(dnx_oam_endpoint_info_glb.use_48_maid))
    {

        /*
        * Adding non acc MEP
        */
        bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
        mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
        mep_not_acc_info.group = group_info->id;
        mep_not_acc_info.level = md_level_1;
        mep_not_acc_info.gport = gport1;
        mep_not_acc_info.name = 0;
        mep_not_acc_info.ccm_period = 0;

        mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
        mep_not_acc_info.lm_counter_if = counter_if_0;

        printf("bcm_oam_endpoint_create mep_not_acc_info\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_not_acc_info), "");
        printf("created MEP with id %d\n", mep_not_acc_info.id);
        dnx_oam_results_glb.ep1.gport = mep_not_acc_info.gport;
        dnx_oam_results_glb.ep1.id = mep_not_acc_info.id;
        dnx_oam_results_glb.ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;
    }

    /*
    * Adding acc MEP - upmep
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_2;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
    mep_acc_info.name = 123;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 5;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.sampling_ratio = dnx_oam_endpoint_info_glb.sampling_ratio;

    if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_endpoint_info_glb.src_mac_mep_2, 6);

    /* Setting My-CFM-MAC */
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_endpoint_info_glb.port_2, dnx_oam_endpoint_info_glb.mac_mep_2), "");

    dnx_oam_results_glb.station_id[1] = gl_station_id;

    mep_acc_info.acc_profile_id = acc_egress_profile;

    /*RX*/
    mep_acc_info.gport = gport2;
    mep_acc_info.lm_counter_base_id = lm_counter_base_id_3;
    mep_acc_info.lm_counter_if = counter_if_0;

    if (dnx_oam_endpoint_info_glb.up_mep_id)
    {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = dnx_oam_endpoint_info_glb.up_mep_id;
    }

    /* local_id is used to carry responder MEP_ID for SLM */
    if(device_type_v2) {
        mep_acc_info.local_id = 0xff;
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");

    dnx_oam_results_glb.ep2.gport = mep_acc_info.gport;
    dnx_oam_results_glb.ep2.id = mep_acc_info.id;
    dnx_oam_results_glb.ep2.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

    /*
    * Adding Remote MEP
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0xff;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;

    if(device_type_v2) {
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10S;
    }
    if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
        rmep_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
        rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
    }

    if (dnx_oam_endpoint_info_glb.remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;

        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
    printf("created RMEP with id %d\n", rmep_info.id);

    dnx_oam_results_glb.ep2.rmep_id = rmep_info.id;

   /*
    * Adding acc MEP - downmep
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_3;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, dnx_oam_endpoint_info_glb.port_1);
    mep_acc_info.name = 456;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 10;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.int_pri = 1 + (3<<2);

    if (dnx_oam_endpoint_info_glb.down_mep_id) {
        mep_acc_info.flags = BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = dnx_oam_endpoint_info_glb.down_mep_id;
    }

    if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_endpoint_info_glb.src_mac_mep_3, 6);

    /*RX*/
    mep_acc_info.gport = gport1;

    mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;
    mep_acc_info.lm_counter_if = (dnx_oam_endpoint_info_glb.enable_oamp_counter_allocation || device_type_v2) ? counter_if_0:counter_if_1;

    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_endpoint_info_glb.port_1, dnx_oam_endpoint_info_glb.mac_mep_3), "");

    dnx_oam_results_glb.station_id[2] = gl_station_id;

    mep_acc_info.acc_profile_id = acc_ingress_profile;

    /* local_id is used to carry responder MEP_ID for SLM */
    if(device_type_v2) {
        mep_acc_info.local_id = 0x11;
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");
    printf("created MEP with id %d\n", mep_acc_info.id);
    dnx_oam_results_glb.ep3.gport = mep_acc_info.gport;
    dnx_oam_results_glb.ep3.id = mep_acc_info.id;
    dnx_oam_results_glb.ep3.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

    /*
    * Adding Remote MEP
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0x11;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
    if(device_type_v2) {
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10S;
    }
    if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
        rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
        rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
    }

    if (dnx_oam_endpoint_info_glb.remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;

        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
    printf("created RMEP with id %d\n", rmep_info.id);

    dnx_oam_results_glb.ep3.rmep_id = rmep_info.id;
    
    if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
        /*
        * Adding acc MEP - downmep
        */

        bcm_oam_endpoint_info_t_init(&mep_acc_info);

        /*TX*/
        mep_acc_info.type = bcmOAMEndpointTypeEthernet;
        mep_acc_info.group = group_info->id;
        mep_acc_info.level = md_level_4;
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, dnx_oam_endpoint_info_glb.port_1);
        mep_acc_info.name = 789;
        mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
        mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

        mep_acc_info.vlan = 10;
        mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
        mep_acc_info.outer_tpid = 0x8100;
        mep_acc_info.inner_vlan = 0;
        mep_acc_info.inner_pkt_pri = 0;
        mep_acc_info.inner_tpid = 0;
        mep_acc_info.int_pri = 1 + (3<<2);

        if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
            mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
            mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }

        /* The MAC address that the CCM packets are sent with*/
        sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_endpoint_info_glb.src_mac_mep_4, 6);

        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_endpoint_info_glb.port_2, dnx_oam_endpoint_info_glb.mac_mep_4), "");

        dnx_oam_results_glb.station_id[3] = gl_station_id;

        mep_acc_info.acc_profile_id = acc_ingress_profile;

        /*RX*/
        mep_acc_info.gport = gport2;
        mep_acc_info.lm_counter_base_id = device_type_v2 ? lm_counter_base_id_3 : lm_counter_base_id_4;
        mep_acc_info.lm_counter_if = (dnx_oam_endpoint_info_glb.enable_oamp_counter_allocation || device_type_v2) ? counter_if_0: counter_if_1;

        printf("bcm_oam_endpoint_create mep_acc_info\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");

        printf("created MEP with id %d\n", mep_acc_info.id);
        dnx_oam_results_glb.ep4.gport = mep_acc_info.gport;
        dnx_oam_results_glb.ep4.id = mep_acc_info.id;
        dnx_oam_results_glb.ep4.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

        /*
        * Adding Remote MEP
        */
        bcm_oam_endpoint_info_t_init(&rmep_info);
        rmep_info.name = 0x12;
        rmep_info.local_id = mep_acc_info.id;
        rmep_info.ccm_period = 0;
        rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
        rmep_info.loc_clear_threshold = 1;
        rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        rmep_info.id = mep_acc_info.id;
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        if(device_type_v2) {
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10S;
        }
        if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
            rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
            rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
        }

        if (dnx_oam_endpoint_info_glb.remote_meps_start_in_loc_enable) {
            rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        }

        printf("bcm_oam_endpoint_create RMEP\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
        printf("created RMEP with id %d\n", rmep_info.id);

        dnx_oam_results_glb.ep4.rmep_id = rmep_info.id;
    }
    return BCM_E_NONE;
}

/**
 * Create a snoop and a trap associated with that snoop.
 * Snoop OAM packets based on opcode and MAC type
 * to a given destination.
 *
 * @param unit
 * @param snoop_config_info
 *
 * @return int
 */
int oam_change_mep_destination_to_snoop(int unit, oam_snoop_config_s * snoop_config_info)
{

    BCM_IF_ERROR_RETURN_MSG(dnx_oam_change_mep_destination_to_snoop(unit, snoop_config_info->dest_snoop_port, snoop_config_info->opcode,
                                                snoop_config_info->mac_type, snoop_config_info->counter_increment,
                                                snoop_config_info->profile_id, snoop_config_info->is_ingress), "");
    return BCM_E_NONE;
}

/**
 * Demonstrate RDI generation from multiple remote peers:
 * Create a Down-MEP, create 3 RMEPs for this MEP.
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_multipoint_rdi_assertion(int unit, int port1, int port2, int port3) {
    bcm_oam_group_info_t *group_info;
    bcm_oam_endpoint_info_t mep_acc_info;
    bcm_oam_endpoint_info_t rmep_info;

    int i = 20; /* Initial RMEP ID */

    dnx_oam_endpoint_info_glb.port_1 = port1;
    dnx_oam_endpoint_info_glb.port_2 = port2;
    dnx_oam_endpoint_info_glb.port_3 = port3;

    dnx_oam_endpoint_info_glb.vsi = 2;

    int md_level = 3;
    int counter_base_id;
    int counter_if;

    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;

    int device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));

    BCM_IF_ERROR_RETURN_MSG(create_vswitch_and_mac_entries(unit), "");

    printf("Creating MA group (13 byte name format)\n");
    bcm_oam_group_info_t_init(&dnx_oam_results_glb.group_info_gl);
    sal_memcpy(dnx_oam_results_glb.group_info_gl.name, dnx_oam_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_results_glb.group_info_gl), "");
    group_info = &dnx_oam_results_glb.group_info_gl;

    /*
     * Allocate counter engines, and get counters
     */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, dnx_oam_endpoint_info_glb.port_1, counter_if, 1, &counter_base_id), "");

    /*
     * Get default profile encoded values.
     * Default profile is 1 and is configured
     * on init(application) stage.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile), "(accelerated ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile), "");

    /*
     * Adding accelerated Down-MEP
     */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, dnx_oam_endpoint_info_glb.port_1);
    mep_acc_info.name = 456;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    mep_acc_info.vlan = 10;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1) */
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.int_pri = 1 + (3<<2);

    /* In OAMP v2, RDI from Rx doesn't work */
    if(!device_type_v2)
    {
        /* Take RDI only from RX */
        mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;
    }

    /* The MAC address that the CCM packets are sent with */
    sal_memcpy(mep_acc_info.src_mac_address, dnx_oam_endpoint_info_glb.src_mac_mep_3, 6);

    /* RX */
    mep_acc_info.gport = gport1;
    mep_acc_info.lm_counter_base_id = counter_base_id;
    mep_acc_info.lm_counter_if = counter_if;

    /* Setting My-CFM-MAC (for Jericho2 Only) */
    BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_endpoint_info_glb.port_1, dnx_oam_endpoint_info_glb.mac_mep_3), "");
    dnx_oam_results_glb.station_id[0] = gl_station_id;

    mep_acc_info.acc_profile_id = acc_ingress_profile;

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");
    printf("Created MEP with id %d\n", mep_acc_info.id);

    dnx_oam_results_glb.ep1.gport = mep_acc_info.gport;
    dnx_oam_results_glb.ep1.id = mep_acc_info.id;

    /*
    * Adding Remote MEPs
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.type = bcmOAMEndpointTypeEthernet;
    /* Take RDI only from RX */
    rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI;
    if(!device_type_v2)
    {
        if(*(dnxc_data_get(unit, "oam", "oamp", "rmep_db_non_zero_lifetime_units_limitation", NULL))) {
            /** Needed to avoid limitation in JR2 A0 */
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
        }
        rmep_info.ccm_period = 0;
    }
    else
    {
        /* For OAMP v2, RDI comes only from LOC */
        rmep_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    }
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    
    /* Create three RMEPs for the Down-MEP */
    for (i; i < 23; i++)
    {
        rmep_info.name = i;
        printf("Create RMEP with NAME=%d\n",i);
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &rmep_info), "");
        printf("Created RMEP with id %d\n", rmep_info.id);
    }

    return BCM_E_NONE;
}

/**
 * Sets trap for OAM MAID error to specific port
 *
 * @param unit
 * @param dest_port - should be local port.
 * @param maid_punt_trap_del_create - Option to delete trap and create a new one
 *
 * @return int
 */
int maid_trap_set(int unit, int dest_port_or_queue, int is_queue, int maid_punt_trap_del_create) {
    bcm_rx_trap_config_t trap_config;
    int trap_code=0;

    /**
     * For DNX family, an actual trap was created on startup for this
     * punt reason.  To modify it, it must be retrieved, then modified
     * or destroyed and recreated
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapOampMaidErr, &trap_code), "");
    if(maid_punt_trap_del_create)
    {
        /** Destroy and recreate option */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, trap_code), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapOampMaidErr, &trap_code), "");
    }
    /** Trap code on FHEI of trapped packet of this kind will be
     *  trap_code & 0xff */
    dnx_oam_results_glb.maid_punt_trap =  (trap_code & 0xFF);
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID|BCM_RX_TRAP_UPDATE_DEST;

    if (is_queue) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config.dest_port, dest_port_or_queue);
    }
    else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(trap_config.dest_port, dest_port_or_queue);
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_code, trap_config), "");

    return BCM_E_NONE;
}

/**
 * Unregister OAM events registered using funtion register_events

 *
 * @param unit
 * @return int
 */
int unregister_events(int unit) {
    bcm_oam_event_types_t timeout_event, timein_event, port_interface_event;

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_unregister(unit, timeout_event, cb_oam), "");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_unregister(unit, timein_event, cb_oam), "");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemote);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_unregister(unit, timein_event, cb_oam), "");

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemoteUp);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_unregister(unit, timein_event, cb_oam), "");

    BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
    BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortDown);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_unregister(unit, port_interface_event, cb_oam), "");

    BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
    BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceDown);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_event_unregister(unit, port_interface_event, cb_oam), "");

    return BCM_E_NONE;
}

/**
 * Create a MIP
 *
 * @param unit
 *
 * @return int
 */
int create_mip(int unit, int md_level_mip, bcm_oam_group_t group, bcm_gport_t gport, bcm_mac_t dst_mac, int set_my_cfm_mac) {
    bcm_oam_endpoint_info_t mip_info;
    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;

    /*
     * Get default profile encoded values.
     * Default profile is 1 and
     * configured on init(application) stage.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile), "(ingress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile), "(egress)");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, 0, gport, ingress_profile, egress_profile), "");

    /*
    * Adding a MIP
    */
    bcm_oam_endpoint_info_t_init(&mip_info);
    mip_info.type = bcmOAMEndpointTypeEthernet;
    mip_info.group = group;
    mip_info.level = md_level_mip;
    mip_info.gport = gport;
    mip_info.flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;

    if (set_my_cfm_mac)
    {
        /* Setting My-CFM-MAC */
        BCM_IF_ERROR_RETURN_MSG(oam__my_cfm_mac_set(unit, dnx_oam_endpoint_info_glb.port_2, dst_mac), "");
    }

    printf("bcm_oam_endpoint_create mip_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mip_info), "");
    printf("Created MIP with id %d\n", mip_info.id);

    dnx_oam_results_glb.ep3.gport = mip_info.gport;
    dnx_oam_results_glb.ep3.id = mip_info.id;

    return BCM_E_NONE;
}

/**
 * Create OAM group and MIP
 *
 * @param unit
 *
 * @return int
 */
int oam_create_mip_with_defaults (int unit) {
    bcm_error_t rv;
    bcm_oam_group_info_t group_info_get;

    bcm_oam_group_info_t_init(&group_info_get);
    rv = bcm_oam_group_get(unit, dnx_oam_results_glb.group_info_gl.id, &group_info_get);

    if (rv == BCM_E_NOT_FOUND) {
        printf("Creating OAM group \n");
        bcm_oam_group_info_t_init(&dnx_oam_results_glb.group_info_gl);
        sal_memcpy(dnx_oam_results_glb.group_info_gl.name, dnx_oam_endpoint_info_glb.long_name, BCM_OAM_GROUP_NAME_LENGTH);

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &dnx_oam_results_glb.group_info_gl), "");
    }

    BCM_IF_ERROR_RETURN_MSG(create_mip(unit, MP_LEVEL_MIP, dnx_oam_results_glb.group_info_gl.id, gport2, dnx_oam_endpoint_info_glb.mac_mep_2, 0), "");

    return BCM_E_NONE;
}

/**
 * Create OAM group and MIP
 *
 * @param unit
 * @param default_mep_lif_profile
 *
 * @return int
 */
int oam_create_default_mep (int unit, int default_mep_lif_profile) {
    bcm_oam_endpoint_info_t default_info;

    bcm_oam_endpoint_info_t_init(&default_info);
    default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;

    if(default_mep_lif_profile != 0)
    {
        /* Setting the profile for default MEP */
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0, BCM_GPORT_INVALID, default_mep_lif_profile, BCM_OAM_PROFILE_INVALID), "");

        default_info.level = 3;
    }

    default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &default_info), "");

    dnx_oam_results_glb.ep_def.gport = default_info.gport;
    dnx_oam_results_glb.ep_def.id = default_info.id;

    return BCM_E_NONE;
}


/**
 * enable protection packets in addition to interrupts.
 *
 * @param unit
 * @param dest_port
 *
 * @return int
 */
int enable_oam_protection_packets(int unit, int dest_port) {
    bcm_rx_trap_config_t trap_config_protection;
    int trap_code; /* trap code on FHEI  will be (trap_code & 0xff), in this case 1.*/
    /* valid trap codes for oamp traps are 0x401 - 0x4ff */
    int flags;

    bcm_rx_trap_config_t_init(&trap_config_protection);
    trap_config_protection.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID;
    /** For DNX, traps are created without ID - allocated as user defined */

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, flags, bcmRxTrapOampProtection, &trap_code), "");

    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(unit, dest_port, &trap_config_protection.dest_port), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_code, trap_config_protection), "");

    dnx_oam_results_glb.protection_trap = trap_code & 0xFF;

    return BCM_E_NONE;
}


/**
 * enable protection packets in addition to interrupts.
 * Global setting for all OAM. Function assumes OAM has been inititalized.
 * Configure the protection header as raw data
 *
 * @param unit
 * @param dest_port
 *
 * @return int
 */
int enable_oam_protection_packets_raw_header(int unit, int dest_port) {

    int system_headers_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    int trap_code;

    bcm_pkt_blk_t packet_header;
    bcm_pkt_blk_t_init(&packet_header);

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, 232, &flags, &interface_info, &mapping_info), "");

    /*
     * Align protection packet headers size to a multiple of 4.
     */
    packet_header.len = 32;

    /* PTCH (2B) + ITMH (4B) + Internal header */
    if (system_headers_mode == 1) {
        unsigned char arr_data[] = { 0x70, mapping_info.pp_port, 0x02, 0x18, 0x01, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        /* Encode Destination system-port into header to its place */
        arr_data[5] = (dest_port & 0x7f) << 0x1;
        arr_data[4] = (dest_port >> 0x7) & 0xff;
        arr_data[3] = (dest_port >> 0xf) | 0x18;

        /** Valid trap IDs must be used */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapOampProtection, &trap_code), "");
        dnx_oam_results_glb.protection_trap = trap_code & 0xFF;

        arr_data[22] = (dnx_oam_results_glb.protection_trap >> 4); /** Upper nibble */
        arr_data[23] = ((dnx_oam_results_glb.protection_trap & 0xF) << 4) | (arr_data[23] & 0xF);

        bcm_rx_trap_config_t trap_config_protection;
        bcm_rx_trap_config_t_init(&trap_config_protection);

        BCM_IF_ERROR_RETURN_MSG(port_to_system_port(unit, dest_port, &trap_config_protection.dest_port), "");

        trap_config_protection.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID;

        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, 0x400 | dnx_oam_results_glb.protection_trap, trap_config_protection), "");

        packet_header.data = sal_alloc(packet_header.len, "protection packet headers");
        sal_memcpy(packet_header.data, arr_data, packet_header.len);
    } else {
        /* For interop, the system header should be in JR1 format */
        unsigned char arr_data[] = { 0x70, mapping_info.pp_port, 0x41, 0x00, 0xcb, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        /* Encode Destination system-port into header to its place */
        arr_data[4] = dest_port&0xff;
        arr_data[3] = (dest_port&0xff00)>>8;
        packet_header.data = sal_alloc(packet_header.len, "protection packet headers");
        sal_memcpy(packet_header.data, arr_data, packet_header.len);
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_protection_packet_header_set(unit,&packet_header), "");

    sal_free(packet_header.data);
    return BCM_E_NONE;
}



/*
 * default_ep_example
 *
 * This example uses the OAM default profile to create an upmep with MDL=5
 * and a downmep with MDL=3 for ports with OAM trap profile = 1.
 * If the inlif profiles are in simple mode (indicated by advanced_mode=0),
 * then port1's inlif profile bits allocated to the OAM trap profile are modified to 0x1
 * In advanced mode, a mapping is done between inlif-profile=5 -> oam-trap-profile=1 and
 * port1's inlif profile is set to 5.
 */
int default_ep_example(int unit, int mp_profile) {
    bcm_oam_endpoint_info_t default_info;
    uint32 flags;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, gport1, bcmPortControlOamDefaultProfile, 1/*OAM trap profile */ ), "");

    printf("Add default down mep (INGRESS1)\n");
    bcm_oam_endpoint_info_t_init(&default_info);
    default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS1;
    default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    default_info.level = 3;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &default_info), "");

    flags = BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_1;
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, flags, BCM_GPORT_INVALID, mp_profile, BCM_OAM_PROFILE_INVALID), "");

    dnx_oam_results_glb.ep_def.gport = default_info.gport;
    dnx_oam_results_glb.ep_def.id = default_info.id;

    return BCM_E_NONE;
}

/* Delete anything that was created by oam_run_with_defaults */
int oam_default_cleanup(int unit)
{
    bcm_mac_t kmac;
    int known_mac_lsb = 0xce;
    int known_vlan = 3;
    int index, counter_if, core_id, tm_port, database_id;
    int vlan_translate_action_id[9];
    char error_msg[200]={0,};
    kmac[5] = known_mac_lsb;
    vlan_translate_action_id[0] = EGRESS_REMOVE_TAGS_PROFILE + EGRESS_SINGLE_TAGGED_OFFSET;
    vlan_translate_action_id[1] = EGRESS_REMOVE_TAGS_PROFILE + EGRESS_DOUBLE_TAGGED_OFFSET;
    vlan_translate_action_id[2] = EGRESS_REMOVE_TAGS_PROFILE + EGRESS_UNTAGGED_OFFSET;
    vlan_translate_action_id[3] = EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + EGRESS_SINGLE_TAGGED_OFFSET;
    vlan_translate_action_id[4] = EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + EGRESS_DOUBLE_TAGGED_OFFSET;
    vlan_translate_action_id[5] = EGRESS_REMOVE_TAGS_PUSH_1_PROFILE + EGRESS_UNTAGGED_OFFSET;
    vlan_translate_action_id[6] = EGRESS_REMOVE_TAGS_PUSH_2_PROFILE + EGRESS_SINGLE_TAGGED_OFFSET;
    vlan_translate_action_id[7] = EGRESS_REMOVE_TAGS_PUSH_2_PROFILE + EGRESS_DOUBLE_TAGGED_OFFSET;
    vlan_translate_action_id[8] = EGRESS_REMOVE_TAGS_PUSH_2_PROFILE + EGRESS_UNTAGGED_OFFSET;
    if (dnx_oam_endpoint_info_glb.use_port_interface_status) {
        snprintf(error_msg, sizeof(error_msg), "Could not delete MEP %d\n", dnx_oam_results_glb.ep4.id);
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_destroy(unit, dnx_oam_results_glb.ep4.id), error_msg);
    }
    snprintf(error_msg, sizeof(error_msg), "Could not delete MEP %d\n", dnx_oam_results_glb.ep3.id);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_destroy(unit, dnx_oam_results_glb.ep3.id), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete station %d\n", dnx_oam_results_glb.station_id[0]);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_station_delete(unit, dnx_oam_results_glb.station_id[0]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete MEP %d\n", dnx_oam_results_glb.ep2.id);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_destroy(unit, dnx_oam_results_glb.ep2.id), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete station %d\n", dnx_oam_results_glb.station_id[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_station_delete(unit, dnx_oam_results_glb.station_id[1]), error_msg);
    if(!(dnx_oam_endpoint_info_glb.use_48_maid)) {
        snprintf(error_msg, sizeof(error_msg), "Could not delete MEP %d\n", dnx_oam_results_glb.ep1.id);
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_destroy(unit, dnx_oam_results_glb.ep1.id), error_msg);
    }
    snprintf(error_msg, sizeof(error_msg), "Could not delete station %d\n", dnx_oam_results_glb.station_id[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_station_delete(unit, dnx_oam_results_glb.station_id[2]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete group %d\n", dnx_oam_results_glb.group_info_gl.id);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_destroy(unit, dnx_oam_results_glb.group_info_gl.id), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete L2 address MAC %0x:%0x:%0x:%0x:%0x:%0x vlan: %d\n", dnx_oam_endpoint_info_glb.mac_mep_2[0],
            dnx_oam_endpoint_info_glb.mac_mep_2[1], dnx_oam_endpoint_info_glb.mac_mep_2[2], dnx_oam_endpoint_info_glb.mac_mep_2[3],dnx_oam_endpoint_info_glb.mac_mep_2[4],
            dnx_oam_endpoint_info_glb.mac_mep_2[5], dnx_oam_endpoint_info_glb.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_delete(unit, dnx_oam_endpoint_info_glb.mac_mep_2, dnx_oam_endpoint_info_glb.vsi), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete L2 address MAC %0x:%0x:%0x:%0x:%0x:%0x vlan: %d\n", dnx_oam_endpoint_info_glb.mac_mep_1[0],
            dnx_oam_endpoint_info_glb.mac_mep_1[1], dnx_oam_endpoint_info_glb.mac_mep_1[2], dnx_oam_endpoint_info_glb.mac_mep_1[3],dnx_oam_endpoint_info_glb.mac_mep_1[4],
            dnx_oam_endpoint_info_glb.mac_mep_1[5], dnx_oam_endpoint_info_glb.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_delete(unit, dnx_oam_endpoint_info_glb.mac_mep_1, dnx_oam_endpoint_info_glb.vsi), error_msg);
    for (counter_if = 0; counter_if < 2; counter_if ++) {
        snprintf(error_msg, sizeof(error_msg), "Could not read core ID for port %d on unit %d\n", dnx_oam_endpoint_info_glb.port_1, unit);
        BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(unit, dnx_oam_endpoint_info_glb.port_1, &core_id, &tm_port), error_msg);
        database_id = core_id*6 + counter_if;
        snprintf(error_msg, sizeof(error_msg), "Could not delete CRPS database %d\n", database_id);
        BCM_IF_ERROR_RETURN_MSG(crps_oam_database_destroy(unit, core_id, database_id), error_msg);
        snprintf(error_msg, sizeof(error_msg), "Could not read core ID for port %d on unit %d\n", dnx_oam_endpoint_info_glb.port_2, unit);
        BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(unit, dnx_oam_endpoint_info_glb.port_2, &core_id, &tm_port), error_msg);
        database_id = core_id*6 + counter_if + 3;
        snprintf(error_msg, sizeof(error_msg), "Could not delete CRPS database %d\n", database_id);
        BCM_IF_ERROR_RETURN_MSG(crps_oam_database_destroy(unit, core_id, database_id), error_msg);
    }
    for (index = 8; index >= 0; index--) {
        snprintf(error_msg, sizeof(error_msg), "Could not delete egress vlan translation %d\n", vlan_translate_action_id[index]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, vlan_translate_action_id[index]), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS), "Could not delete ingress vlan translation\n");
    snprintf(error_msg, sizeof(error_msg), "Could not delete L2 address MAC LSB %d, vlan %d\n", known_mac_lsb, known_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_delete(unit, kmac, known_vlan), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete multicast encap from vlan %d system port %d gport %d\n",
            dnx_oam_endpoint_info_glb.vsi, vswitch_metro_mp_info.sysports[2], dnx_oam_endpoint_info_glb.mc_gport_3);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_delete(unit, dnx_oam_endpoint_info_glb.vsi, vswitch_metro_mp_info.sysports[2], dnx_oam_endpoint_info_glb.mc_gport_3, 0),
            error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not vswitch port from vlan %d gport %d\n", dnx_oam_endpoint_info_glb.vsi, dnx_oam_endpoint_info_glb.mc_gport_3);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_delete(unit, dnx_oam_endpoint_info_glb.vsi, dnx_oam_endpoint_info_glb.mc_gport_3), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vlan gport from vlan %d\n", vswitch_metro_mp_info.p_vlans[8]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_delete_all(unit, vswitch_metro_mp_info.p_vlans[8]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vlan port %d\n", dnx_oam_endpoint_info_glb.mc_gport_3);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, dnx_oam_endpoint_info_glb.mc_gport_3), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete multicast encap from vlan %d system port %d gport %d\n",
            dnx_oam_endpoint_info_glb.vsi, vswitch_metro_mp_info.sysports[1], dnx_oam_endpoint_info_glb.mc_gport_2);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_delete(unit, dnx_oam_endpoint_info_glb.vsi, vswitch_metro_mp_info.sysports[1], dnx_oam_endpoint_info_glb.mc_gport_2, 0),
            error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not vswitch port from vlan %d gport %d\n", dnx_oam_endpoint_info_glb.vsi, dnx_oam_endpoint_info_glb.mc_gport_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_delete(unit, dnx_oam_endpoint_info_glb.vsi, dnx_oam_endpoint_info_glb.mc_gport_2), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vlan gport from vlan %d\n", vswitch_metro_mp_info.p_vlans[4]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_delete_all(unit, vswitch_metro_mp_info.p_vlans[4]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vlan port %d\n", dnx_oam_endpoint_info_glb.mc_gport_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, dnx_oam_endpoint_info_glb.mc_gport_2), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete multicast encap from vlan %d system port %d gport %d\n",
            dnx_oam_endpoint_info_glb.vsi, vswitch_metro_mp_info.sysports[0], dnx_oam_endpoint_info_glb.mc_gport_1);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_delete(unit, dnx_oam_endpoint_info_glb.vsi, vswitch_metro_mp_info.sysports[0], dnx_oam_endpoint_info_glb.mc_gport_1, 0),
            error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not vswitch port from vlan %d gport %d\n", dnx_oam_endpoint_info_glb.vsi, dnx_oam_endpoint_info_glb.mc_gport_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_delete(unit, dnx_oam_endpoint_info_glb.vsi, dnx_oam_endpoint_info_glb.mc_gport_1), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vlan gport from vlan %d\n", vswitch_metro_mp_info.p_vlans[0]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_delete_all(unit, vswitch_metro_mp_info.p_vlans[0]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vlan port %d\n", dnx_oam_endpoint_info_glb.mc_gport_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, dnx_oam_endpoint_info_glb.mc_gport_1), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Could not delete vswitch from vlan %d\n", dnx_oam_endpoint_info_glb.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_destroy(unit, dnx_oam_endpoint_info_glb.vsi), error_msg);
    for (index = 0; index < 12; index++) {
        if (single_vlan_tag && (((index % 2) != 0))) {
            continue;
        }
        snprintf(error_msg, sizeof(error_msg), "Could not delete vlan %d\n", vswitch_metro_mp_info.p_vlans[index]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy(unit, vswitch_metro_mp_info.p_vlans[index]), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(unregister_events(unit), "Could not unregister events\n");

    return BCM_E_NONE;
}

