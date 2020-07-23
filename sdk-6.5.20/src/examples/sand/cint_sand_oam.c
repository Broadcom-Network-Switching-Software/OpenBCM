/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
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
 */

bcm_oam_endpoint_t down_mep_id = 0;
bcm_oam_endpoint_t up_mep_id = 0;

/* Globals - MAC addresses , ports & gports*/
bcm_multicast_t mc_group = 1234;
bcm_mac_t mac_mep_1 = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
bcm_mac_t mac_mep_2 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
bcm_mac_t mac_mep_3 = {0x00, 0x00, 0x00, 0x01, 0x02, 0xff};
bcm_mac_t mac_mep_4 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xfd};
bcm_mac_t src_mac_mep_2 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
bcm_mac_t src_mac_mep_3 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x01};
bcm_mac_t src_mac_mep_4 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x07};
bcm_oam_group_info_t group_info_long_ma;
bcm_oam_group_info_t group_info_short_ma;
/*1-const; 32-long format; 13-length; all the rest - MA name*/
uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0xa, 0xb, 0xc, 0xd};
/*1-const; 3-short format; 2-length; all the rest - MA name*/
uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
/* 48 bytes format */
uint8 str_48b_name[BCM_OAM_GROUP_NAME_LENGTH] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3};
int use_long_maid = 0;
int use_11b_maid = 0;
int use_48_maid = 0;
bcm_oam_group_info_t group_info_48b_ma;

bcm_oam_endpoint_info_t mep_acc_info;

bcm_gport_t gport1, gport2, gport3, gport4; /* these are the ports created by the vswitch*/

int cross_connect_configuration = 0;
int is_offloaded = 0;

/** Trap code for protection packets */
int protection_trap;

/** Trap code from configurable MAID miss punt */
int maid_punt_trap;

/** Option to delete trap and create a new one */
int maid_punt_trap_del_create = 0;

/**
* When set to "1" the MEPs will be created with RX statistics.
* RX packets are the packets that are received in the OAMP.
*/
int is_rx_statisitcs_enabled=0;
/**
* When set to "1" the MEPs will be created with TX statistics
* TX packets are the packets transmitted from the OAMP.
*/
int is_tx_statisitcs_enabled=0;
/**
* When set to "1" enables per-opcode-counting
* per-opcode-counting means that different opcode packets will be count on different counters.
*/
int is_per_opcode_statisitcs_enabled=0;
/**
* When set to "1" allocate 8k counters in database 5 for TX packets.
* When set to "2" allocate 8k counters in database 6 for RX packets.
* When set to "3" allocate 8k counters in database 5 for TX packets and 8k counters in database 6 for RX packets.
*/
int enable_oamp_counter_allocation=0;

int lm_counter_base_id_2 = 0;
int counter_if_1 = 1;
bcm_vlan_port_t vp1, vp2;
bcm_vlan_t vsi = 0;

int port_1 = 13; /* physical port (signal generator)*/
int port_2 = 14;
int port_3 = 15;

int sampling_ratio = 0;
int use_port_interface_status = 0;
int port_interface_event_count=0;
int timeout_events_count = 0;
int remote_event_count=0;
int timeout_events_count_multi_oam[5] ={ 0 };

/* Created endpoints information */
oam__ep_info_s ep1;
oam__ep_info_s ep2;
oam__ep_info_s ep3;
oam__ep_info_s ep4;
oam__ep_info_s ep_def;

bcm_oam_endpoint_info_t rmep_info;

int md_level_mip = 7;
bcm_oam_endpoint_info_t mip_info;

/* enable OAM statistics per up mep session, need to load the cint: cint_field_oam_statistics.c*/
int oam_up_statistics_enable=0;

/* Enable LOC events as soon as the remote endpoints are created */
int remote_meps_start_in_loc_enable=0;

/* OAM Action profile */
uint32 mp_profile;

/* Default MEP */
int default_mep_lif_profile = 0;

/*
 * OAM snoop configuration parameters
 */
struct oam_snoop_config_s
{
    /* JER1 & JR2 */
    bcm_gport_t dest_snoop_port;
    bcm_oam_opcode_t opcode;
    /* JER1 */
    bcm_oam_endpoint_t endpoint_id;
    bcm_oam_action_type_t action_type;
    /* JR2 */
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

/*****************************************************************************/
/*                                        OAM BASIC EXAMPLE                                                                               */
/*****************************************************************************/

/**
 * NTP timestamp format will be used by default, unless disabled
 * by soc property
 *
 * @author sinai (09/12/2014)
 *
 * @param unit
 *
 * @return bcm_oam_timestamp_format_t
 */
bcm_oam_timestamp_format_t get_oam_timestamp_format(int unit) {
    return soc_property_get(unit , "oam_dm_ntp_enable",1)  ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;
}

/**
 * Functions used to read various event counters.
 *
 *
 * @param expected_event_count
 *
 * @return int
 */
int read_timeout_event_count(int expected_event_count) {
	printf("timeout_events_count=%d\n",timeout_events_count);

if (timeout_events_count==expected_event_count) {
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
    if (timeout_events_count_multi_oam[ep2.id & 0xff] != expected_event_count_on_ep_1 ||  timeout_events_count_multi_oam[ep3.id & 0xff] != expected_event_count_on_ep_2 ) {
        return BCM_E_FAIL;
    }
  
    return BCM_E_NONE;
}


int read_remote_timeout_event_count(int expected_count) {
    printf("timeout_events_count=%d\n",remote_event_count);

    if (remote_event_count==expected_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

int read_port_interface_event_count(int expected_count) {
    printf("port_interface_event_count=%d\n",port_interface_event_count);


    if (port_interface_event_count==expected_count) {
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
        port_interface_event_count++;
    } else {
        timeout_events_count++;
    }

    if (flags & BCM_OAM_EVENT_FLAGS_MULTIPLE) {
        timeout_events_count_multi_oam[endpoint & 0xff]++;
    }

    if (event_type == bcmOAMEventEndpointRemote || event_type == bcmOAMEventEndpointRemoteUp) {
        remote_event_count++;
    }

    return BCM_E_NONE;
}

int register_events(int unit) {
  bcm_error_t rv;
  bcm_oam_event_types_t timeout_event, timein_event, port_interface_event;

  BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
  rv = bcm_oam_event_register(unit, timeout_event, cb_oam, (void*)1);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
  rv = bcm_oam_event_register(unit, timein_event, cb_oam, (void*)2);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemote);
  rv = bcm_oam_event_register(unit, timein_event, cb_oam, (void*)3);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemoteUp);
  rv = bcm_oam_event_register(unit, timein_event, cb_oam, (void*)4);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortDown);
  rv = bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)5);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceDown);
  rv = bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)6);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortUp);
  rv = bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)7);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceUp);
  rv = bcm_oam_event_register(unit, port_interface_event, cb_oam, (void*)8);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}

/**
 * Cint that only uses bcm APIs to create non accelerated Up,
 * Down MEP
 *
 *
 * @param unit
 * @param port1
 * @param vid1 - vlan ID of port1
 * @param port2
 * @param vid2 - vlan ID of port2
 *
 * @return int
 */
int oam_standalone_example(int unit,int port1, int vid1, int port2, int vid2) {

    bcm_error_t rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t mep_not_acc_info;
    int md_level_1 = 2;
    int md_level_2 = 5;
    int lm_counter_base_id_1 = 0;

    bcm_vswitch_cross_connect_t cross_connect;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    int counter_if_0 = 0;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    /* When oam_up_statistics_enable is enabled, engines are allocated for PMF*/
    if (oam_up_statistics_enable == 0 && (device_type < device_type_jericho2)) {
        /*Endpoints will be created on 2 different LIFs. */
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_1,port1);
        BCM_IF_ERROR_RETURN(rv);
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_2,port2);
        BCM_IF_ERROR_RETURN(rv);
    } 
    else if (device_type >= device_type_jericho2)
    {
        /*
         * Allocate counter engines, and get counters
         */
        rv = set_counter_resource(unit, port_1, counter_if_0, 1, &lm_counter_base_id_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port_2, counter_if_1, 1, &lm_counter_base_id_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /* Set port classification ID */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* Set port classification ID */
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    bcm_vlan_port_t_init(&vp1);
    vp1.flags = 0;
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    if (device_type < device_type_jericho2)
    {
        vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    vp1.port = port1;
    vp1.match_vlan = vid1;
    vp1.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid1;
    rv=bcm_vlan_port_create(unit,&vp1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp1.port, vp1.match_vlan,
           vp1.vlan_port_id, vp1.encap_id);
    rv = bcm_vlan_gport_add(unit, vid1, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_vlan_port_t_init(&vp2);
    vp2.flags = 0;
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    if (device_type < device_type_jericho2)
    {
        vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    vp2.port = port2;
    vp2.match_vlan = vid2;
    vp2.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid2;
    rv=bcm_vlan_port_create(unit,&vp2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid2, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
        return rv;
    }

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp2.port, vp2.match_vlan,
           vp2.vlan_port_id, vp2.encap_id);

    /**
    * Cross-connect the ports
    */
    bcm_vswitch_cross_connect_t_init(&cross_connect);

    cross_connect.port1 = vp1.vlan_port_id;
    cross_connect.port2 = vp2.vlan_port_id;

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

     bcm_oam_group_info_t_init(&group_info_short_ma);
     sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
     rv = bcm_oam_group_create(unit, &group_info_short_ma);
     if( rv != BCM_E_NONE) {
         printf("MEG:\t (%s) \n",bcm_errmsg(rv));
         return rv;
     }


  /*
   * Get default profile encoded values
   * Default profile is 1 and
   * configured on init(application) stage.
   * Relevant for Jr2 ONLY
   */
    if( device_type >= device_type_jericho2)
    {
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(egress)\n");
          return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, vp1.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, 0, vp2.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
    }

    /* 
    * Adding non acc down MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = group_info_short_ma.id;
    mep_not_acc_info.level = md_level_1;
    mep_not_acc_info.gport = vp1.vlan_port_id;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    if(device_type >= device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port1, mac_mep_1);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }
    else
    {
        sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
    }

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
    mep_not_acc_info.lm_counter_if = counter_if_0;
    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created Down MEP with id %d\n", mep_not_acc_info.id);
    ep1.gport = mep_not_acc_info.gport;
    ep1.id = mep_not_acc_info.id;
    ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;


    /**
    * Adding non acc UP MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = group_info_short_ma.id;
    mep_not_acc_info.level = md_level_2;
    mep_not_acc_info.gport = vp2.vlan_port_id;
    mep_not_acc_info.tx_gport = BCM_GPORT_INVALID;
    mep_not_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    if( device_type >= device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port2, mac_mep_2);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }
    else
    {
        sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
    }

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_2;
    mep_not_acc_info.lm_counter_if = counter_if_1;
    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created UP MEP with id %d\n", mep_not_acc_info.id);
    ep2.gport = mep_not_acc_info.gport;
    ep2.id = mep_not_acc_info.id;
    ep2.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;

    return rv;
}
/**
 * Basic OAM example.
 * Creates vswitch on 3 given ports and the folowing endpoint:
 * 1) Default endpoint
 * 2) Non accelerated down MEP on port1
 * 3) Accelerated up MEP on port2 + RMEP
 * 4) Accelerated down MEP on port1 + RMEP
 *
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_run_with_defaults(int unit, int port1, int port2, int port3) {
    bcm_error_t rv;

    single_vlan_tag = 1;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    /*enable oam statistics per mep session*/
    if (oam_up_statistics_enable) {
        rv = oam_tx_up_stat(unit);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    printf("Registering OAM events\n");
    rv = register_events(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = oam_example(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (enable_oamp_counter_allocation)
    {
        /** Allocate counters for 4k MEPs with statistics enable, counter base is the first MEP. */
        /** crps_oam_config_enable_eviction variable is expected to
         *  be set from outside. Should be set to 1 for per-endpoint-statistics */
        rv = cint_oam_oamp_statistics_main(unit, ep2.id, 4192, (enable_oamp_counter_allocation-1), crps_oam_config_enable_eviction);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
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
 *
 * @param unit
 *
 * @return int
 */
int oam_example(int unit) {
    bcm_error_t rv;
    bcm_oam_group_info_t group_info_long_ma_test;
    bcm_oam_group_info_t *group_info;
    bcm_oam_endpoint_info_t mep_not_acc_info;
    bcm_oam_endpoint_info_t mep_not_acc_test_info;
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

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    printf("Creating two groups (short, long and 11b-maid name format)\n");
    bcm_oam_group_info_t_init(&group_info_long_ma);
    sal_memcpy(group_info_long_ma.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
    /* This is meant for QAX and above only */
    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_oam_group_create(unit, &group_info_long_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);

    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    
    /* When oam_up_statistics_enable is enabled, engines are allocated for PMF*/
    if (oam_up_statistics_enable == 0 && (device_type < device_type_jericho2)) {
        /*Endpoints will be created on 2 different LIFs. */
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_2,port_2);
        BCM_IF_ERROR_RETURN(rv);
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_1,port_1);
        BCM_IF_ERROR_RETURN(rv);
    } else if ((device_type >= device_type_jericho2) && !enable_oamp_counter_allocation)
    {
        /*
         * Allocate counter engines and get counters. The counter_if is used for
         * retrieving counters on different engines of the same port. For each
         * counter interface, OAM CRPS database should be configured first on the
         * core that the port belongs to. Counter engines will be added to the database.
         */
        rv = set_counter_resource(unit, port_1, counter_if_0, 1, &lm_counter_base_id_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port_1, counter_if_1, 1, &lm_counter_base_id_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port_2, counter_if_0, 1, &lm_counter_base_id_3);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port_2, counter_if_1, 1, &lm_counter_base_id_4);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /* 11b maid not supported in Jericho2 */
    if (use_11b_maid && (device_type < device_type_jericho2)) {
        /* Add a group with sttring based 11b MAID */
        bcm_oam_group_info_t_init(&group_info_11b_ma);
        sal_memcpy(group_info_11b_ma.name, str_11b_name, BCM_OAM_GROUP_NAME_LENGTH);
        rv = bcm_oam_group_create(unit, &group_info_11b_ma);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        /* Set the used group for the MEPs to this group */
        group_info = &group_info_11b_ma;

    } else if (use_48_maid){
        /* Add a group with flexible 48 Byte MAID */
        bcm_oam_group_info_t_init(&group_info_48b_ma);
        sal_memcpy(group_info_48b_ma.name, str_48b_name, BCM_OAM_GROUP_NAME_LENGTH);
        group_info_48b_ma.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
        if (device_type >= device_type_qax) {
            group_info_48b_ma.group_name_index = 0x2016; /* Bank 8, entry 22 */
            if(device_type >= device_type_jericho2)
            {
                group_info_48b_ma.group_name_index = 0xA008;
            }
        }
        if (device_type == device_type_qux) {
            group_info_48b_ma.group_name_index = 0x669;
        }
        rv = bcm_oam_group_create(unit, &group_info_48b_ma);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        /* Set the used group for the MEPs to this group */
         group_info = &group_info_48b_ma;
    }
    else {
        /* Set the used group for the MEPs to the group with the short name */
        group_info = &group_info_short_ma;
    }

    bcm_oam_group_info_t_init(&group_info_long_ma_test);
    printf("bcm_oam_group_get\n");
    rv = bcm_oam_group_get(unit, group_info_long_ma.id, &group_info_long_ma_test);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_oam_group_destroy\n");
    rv = bcm_oam_group_destroy(unit, group_info_long_ma.id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_group_create(unit, &group_info_long_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

  /*
   * Get default profile encoded values
   * Default profile is 1 and
   * configured on init(application) stage.
   * Relevant for Jr2 ONLY
   */
    if( device_type >= device_type_jericho2)
    {
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(egress)\n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(accelerated ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(accelerated egress) \n");
          return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }

        if(cross_connect_configuration==0)
        {
            rv = bcm_oam_lif_profile_set(unit, 0, gport3, ingress_profile, egress_profile);
            if (rv != BCM_E_NONE) {
                printf("bcm_oam_lif_profile_set\n");
                return rv;
            }
        }


    }

    /*
     * Adding a default MEP
     */
    printf("Add default mep\n");
    bcm_oam_endpoint_info_t_init(&default_info);
    if (device_type < device_type_arad_plus) {
        default_info.id = -1;
    }
    else {
        default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;
    }

    if(default_mep_lif_profile != 0)
    {        
        /* Setting the profile for default MEP (for Jericho2 Only) */
        rv = bcm_oam_lif_profile_set(unit, BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0, BCM_GPORT_INVALID, ingress_profile, BCM_OAM_PROFILE_INVALID);
        if (rv != BCM_E_NONE) {
            printf("bcm_oam_lif_profile_set fail \n");
            return rv;
        }
        
        default_info.level = 3;
    }

    default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    default_info.timestamp_format = get_oam_timestamp_format(unit);
    rv = bcm_oam_endpoint_create(unit, &default_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    ep_def.gport = default_info.gport;
    ep_def.id = default_info.id;

    /* Setting My-CFM-MAC (for Jericho2 Only)*/
    if(device_type >= device_type_jericho2)
    {
        rv = oam__my_cfm_mac_set(unit, port_1, mac_mep_1);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }
    
    /* Non acc mep associated with maid 48 byte group is not supported on JR2*/
    if(!(use_48_maid && (device_type >= device_type_jericho2)))
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

        if( device_type >= device_type_jericho2)
        {
            mep_not_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
        }
        else
        {
            sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
        }

        mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
        if (device_type >= device_type_jericho2)
        {
            mep_not_acc_info.lm_counter_if = counter_if_0;
        }

        printf("bcm_oam_endpoint_create mep_not_acc_info\n");
        mep_not_acc_info.timestamp_format = get_oam_timestamp_format(unit);
        rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("created MEP with id %d\n", mep_not_acc_info.id);
        ep1.gport = mep_not_acc_info.gport;
        ep1.id = mep_not_acc_info.id;
        ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;
    }

    /*
    * Adding acc MEP - upmep
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    if (is_offloaded) {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
    }
    else
    {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    }
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
    mep_acc_info.sampling_ratio = sampling_ratio;
    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from scanner*/
        mep_acc_info.flags2 =  BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;

        if (use_port_interface_status) {
            mep_acc_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
        }
    }

    if (is_rx_statisitcs_enabled)
    {
        mep_acc_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS;
    }

    if (is_tx_statisitcs_enabled)
    {
        mep_acc_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS;
    }

    if (is_per_opcode_statisitcs_enabled)
    {
        mep_acc_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS;
    }

    mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

    if (device_type < device_type_arad_plus) {
        src_mac_mep_2[5] = port_2; /* In devices older than Arad Plus the LSB of the src mac address must equal the local port. No such restriction in Arad+.*/
    }
    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_2, 6);


    if( device_type >= device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port_2, mac_mep_2);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        mep_acc_info.acc_profile_id = acc_egress_profile;
    }
    else
    {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_2, 6);
    }

    /*RX*/
    mep_acc_info.gport = gport2;
    if (device_type >= device_type_jericho2)
    {
        mep_acc_info.lm_counter_base_id = lm_counter_base_id_3;
        mep_acc_info.lm_counter_if = counter_if_0;
    }
    else
    {
        mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;
    }

    /*
     * When OAM/BFD statistics enabled and MEP ACC is required to be counted,
     * mep acc id 0 can't be used
     */
    if (oam_up_statistics_enable) {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = (device_type == device_type_qux) ? 1024 : 4096;
    }

    if (up_mep_id)
    {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = up_mep_id;
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);

    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    if (is_offloaded) {
        printf("created offloaded MEP with id %d\n", mep_acc_info.id);
    } else {
        printf("created MEP with id %d\n", mep_acc_info.id);
    }

    ep2.gport = mep_acc_info.gport;
    ep2.id = mep_acc_info.id;
    ep2.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

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

    if (device_type >= device_type_arad_plus) {
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        if (use_port_interface_status) {
            rmep_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
        }
    }

    if (remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(unit, &rmep_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created RMEP with id %d\n", rmep_info.id);

    ep2.rmep_id = rmep_info.id;

    /*
    * Adding acc MEP - downmep
    */

    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    if (is_offloaded) {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
    }
    else
    {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    }
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_3;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
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

    if (down_mep_id) {
        mep_acc_info.flags = BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = down_mep_id;
    }

    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from RX*/
        mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;

        if (use_port_interface_status) {
            mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
            mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }
    }

    if (is_rx_statisitcs_enabled)
    {
        mep_acc_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS;
    }

    if (is_tx_statisitcs_enabled)
    {
        mep_acc_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS;
    }

    if (is_per_opcode_statisitcs_enabled)
    {
        mep_acc_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS;
    }

    if (device_type == device_type_arad_a0) {
        src_mac_mep_3[5] = port_1;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_3, 6);

    /*RX*/
    mep_acc_info.gport = gport1;

    if (device_type >= device_type_jericho2)
    {
        mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;
        mep_acc_info.lm_counter_if = enable_oamp_counter_allocation ? counter_if_0 : counter_if_1;
    }
    else
    {
        mep_acc_info.lm_counter_base_id = lm_counter_base_id_1;
    }
    mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

    if(device_type >= device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port_1, mac_mep_3);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        mep_acc_info.acc_profile_id = acc_ingress_profile;
    }
    else
    {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created MEP with id %d\n", mep_acc_info.id);
    ep3.gport = mep_acc_info.gport;
    ep3.id = mep_acc_info.id;
    ep3.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

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

    if (device_type >= device_type_arad_plus) {
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        if (use_port_interface_status) {
            rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
            rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }
    }

    if (remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }
    else {
        if((device_type >= device_type_jericho2) &&
           (*(dnxc_data_get(unit, "oam", "oamp",
           "rmep_db_non_zero_lifetime_units_limitation", NULL)))) {
            /** Needed to avoid limitation in JR2 A0 */
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
        }
    }

   printf("bcm_oam_endpoint_create RMEP\n");
   rv = bcm_oam_endpoint_create(unit, &rmep_info);
   if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }
   printf("created RMEP with id %d\n", rmep_info.id);

    ep3.rmep_id = rmep_info.id;

    if (use_port_interface_status) {
        /*
        * Adding acc MEP - downmep
        */

        bcm_oam_endpoint_info_t_init(&mep_acc_info);

        /*TX*/
        mep_acc_info.type = bcmOAMEndpointTypeEthernet;
        mep_acc_info.group = group_info->id;
        mep_acc_info.level = md_level_4;
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
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
        mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

        if (device_type >= device_type_arad_plus) {
            /* Take RDI only from RX*/
            mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;

            if (use_port_interface_status) {
                mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
                mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
                mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
            }
        }

        /* The MAC address that the CCM packets are sent with*/
        sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_4, 6);

        if( device_type >= device_type_jericho2)
        {
            /* Setting My-CFM-MAC (for Jericho2 Only)*/
            rv = oam__my_cfm_mac_set(unit, port_2, mac_mep_4);
            if (rv != BCM_E_NONE) {
                printf("oam__my_cfm_mac_set fail \n");
                return rv;
            }

            mep_acc_info.acc_profile_id = acc_ingress_profile;
            mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
        }
        else
        {
            sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_4, 6);
        }

        /*RX*/
        mep_acc_info.gport = gport2;
        if (device_type >= device_type_jericho2)
        {
            mep_acc_info.lm_counter_base_id = lm_counter_base_id_4;
            mep_acc_info.lm_counter_if = enable_oamp_counter_allocation ? counter_if_0 : counter_if_1;
        }
        else
        {
            mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;
        }

        printf("bcm_oam_endpoint_create mep_acc_info\n");
        rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
        if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
        }

        printf("created MEP with id %d\n", mep_acc_info.id);
        ep4.gport = mep_acc_info.gport;
        ep4.id = mep_acc_info.id;
        ep4.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

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
        if (device_type >= device_type_arad_plus) {
            rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
            if (use_port_interface_status) {
                rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
                rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
                rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
            }
        }

        if (remote_meps_start_in_loc_enable) {
            rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        } else {
            if((device_type >= device_type_jericho2) &&
               (*(dnxc_data_get(unit, "oam", "oamp",
               "rmep_db_non_zero_lifetime_units_limitation", NULL)))) {
                /** Needed to avoid limitation in JR2 A0 */
                rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
            }
        }

        printf("bcm_oam_endpoint_create RMEP\n");
        rv = bcm_oam_endpoint_create(unit, &rmep_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("created RMEP with id %d\n", rmep_info.id);

        ep4.rmep_id = rmep_info.id;
    }

    return rv;
}

int oam_gport_cross_conect_config(bcm_gport_t inLif1,bcm_gport_t inLif2, int port1, int port2)
{
    cross_connect_configuration = 1;
    gport1 = inLif1;
    gport2 = inLif2;
    port_1 = port1;
    port_2 = port2;
    return 0;
}

/**
 * Create a snoop and a trap associated with that snoop.
 * JR1: Snoop OAM packets based on opcode
 * to a given destination.
 * JR2: Snoop OAM packets based on opcode and MAC type
 * to a given destination.
 *
 *
 * @param unit
 * @param snoop_config_info
 *
 * @return int
 */
int oam_change_mep_destination_to_snoop(int unit, oam_snoop_config_s * snoop_config_info)
{
    bcm_error_t rv;

    rv = oam__device_type_get(unit, &device_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error in checking the device type.\n");
        return rv;
    }

    if (device_type >= device_type_jericho2)
    {
        rv = dnx_oam_change_mep_destination_to_snoop(unit, snoop_config_info->dest_snoop_port, snoop_config_info->opcode,
                                                    snoop_config_info->mac_type, snoop_config_info->counter_increment,
                                                    snoop_config_info->profile_id, snoop_config_info->is_ingress);
        if (rv != BCM_E_NONE)
        {
            printf("Error in dnx_oam_change_mep_destination_to_snoop\n");
            return rv;
        }
    }
    else
    {
        rv = dpp_oam_change_mep_destination_to_snoop(unit, snoop_config_info->dest_snoop_port, snoop_config_info->endpoint_id,
                                                    snoop_config_info->action_type, snoop_config_info->opcode);
        if (rv != BCM_E_NONE)
        {
            printf("Error in dpp_oam_change_mep_destination_to_snoop\n");
            return rv;
        }
    }

    return rv;
}

/**
 * Create an accelerated Down-MEP with level 4.
 * The MEP belongs to a long MA group
 *
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_long_ma_example(int unit, int port1, int port2, int port3) {
    bcm_error_t rv;
    int counter_base_id;
    int counter_if;

    single_vlan_tag = 1;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error in oam__device_type_get.\n");
        print rv;
        return rv;
    }

    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("Creating group (long name format)\n");
    bcm_oam_group_info_t_init(&group_info_long_ma);
    sal_memcpy(group_info_long_ma.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_long_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (device_type < device_type_jericho2)
    {
        rv = set_counter_source_and_engines(unit,&counter_base_id,port_1);
        BCM_IF_ERROR_RETURN(rv);
    }
    else if (device_type >= device_type_jericho2)
    {
        /*
         * Allocate counter engines, and get counters
         */
        rv = set_counter_resource(unit, port_1, counter_if, 1, &counter_base_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /*
    * Get default profile encoded values.
    * Default profile is 1 and is configured
    * on init(application) stage.
    * Relevant for JR2 ONLY
    */
    if(device_type >= device_type_jericho2)
    {
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_profile_id_get_by_type(ingress).\n");
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_profile_id_get_by_type(egress).\n");
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_profile_id_get_by_type(accelerated ingress).\n");
            return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_lif_profile_set.\n");
            return rv;
        }
    }

    /* Adding accelerated Down-MEP */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);
    if (device_type < device_type_jericho2)
    {
        mep_acc_info.id = (device_type == device_type_qux) ? 255 : 4095;
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    }

    /* TX */
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info_long_ma.id;
    mep_acc_info.level = 4;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
    mep_acc_info.name = 456;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    mep_acc_info.vlan = 10;
    mep_acc_info.pkt_pri = 0 + (2<<1);  /* dei(1bit) + (pcp(3bit) << 1) */
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 20;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0x9100;
    mep_acc_info.int_pri = 1 + (3<<2);
    mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from RX */
        mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE;
    }

    if (device_type == device_type_arad_a0) {
        src_mac_mep_3[5] = port_1;
    }

    /* The MAC address that the CCM packets are sent with */
    sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_3, 6);

    /* RX */
    mep_acc_info.gport = gport1;
    mep_acc_info.lm_counter_base_id = counter_base_id;
    mep_acc_info.lm_counter_if = counter_if;

    if(device_type >= device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only) */
        rv = oam__my_cfm_mac_set(unit, port_1, mac_mep_3);
        if (rv != BCM_E_NONE) {
            printf("Error in oam__my_cfm_mac_set.\n");
            return rv;
        }

        mep_acc_info.acc_profile_id = acc_ingress_profile;
    }
    else
    {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    printf("Created MEP with ID=%d\n", mep_acc_info.id);

    ep1.id = mep_acc_info.id;

    /* Adding Remote MEP */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 460;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.type = bcmOAMEndpointTypeEthernet;
    rmep_info.ccm_period = 0;
    if((device_type >= device_type_jericho2) &&
       (*(dnxc_data_get(unit, "oam", "oamp",
       "rmep_db_non_zero_lifetime_units_limitation", NULL)))) {
        /** Needed to avoid limitation in JR2 A0 */
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    }
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;
    if (device_type >= device_type_arad_plus) {
      rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(unit, &rmep_info);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    printf("Created RMEP with ID=%d\n", rmep_info.id);

    ep1.rmep_id = rmep_info.id;

    printf("Registering OAM events\n");
    rv = register_events(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Demonstrate RDI generation from multiple remote peers:
 * Create a Down-MEP, create 3 RMEPs for this MEP.
 *
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_multipoint_rdi_assertion(int unit, int port1, int port2, int port3) {
    bcm_error_t rv;
    bcm_oam_group_info_t *group_info;

    int i = 20; /* Initial RMEP ID */

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    int md_level = 3;
    int counter_base_id;
    int counter_if;

    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error in oam__device_type_get.\n");
        print rv;
        return rv;
    }

    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("Creating MA group (short name format)\n");
    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);

    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    group_info = &group_info_short_ma;

    if (device_type < device_type_jericho2)
    {
        rv = set_counter_source_and_engines(unit,&counter_base_id,port_1);
        BCM_IF_ERROR_RETURN(rv);
    }
    else if (device_type >= device_type_jericho2)
    {
        /*
         * Allocate counter engines, and get counters
         */
        rv = set_counter_resource(unit, port_1, counter_if, 1, &counter_base_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /*
    * Get default profile encoded values.
    * Default profile is 1 and is configured
    * on init(application) stage.
    * Relevant for JR2 ONLY
    */
    if(device_type >= device_type_jericho2)
    {
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_profile_id_get_by_type(ingress).\n");
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_profile_id_get_by_type(egress).\n");
            return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_profile_id_get_by_type(accelerated ingress).\n");
            return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_oam_lif_profile_set.\n");
            return rv;
        }
    }

    /*
    * Adding accelerated Down-MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
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

    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from RX */
        mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;
    }

    /* The MAC address that the CCM packets are sent with */
    sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_3, 6);

    /* RX */
    mep_acc_info.gport = gport1;
    mep_acc_info.lm_counter_base_id = counter_base_id;
    mep_acc_info.lm_counter_if = counter_if;
    mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

    if(device_type >= device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only) */
        rv = oam__my_cfm_mac_set(unit, port_1, mac_mep_3);
        if (rv != BCM_E_NONE) {
            printf("Error in oam__my_cfm_mac_set.\n");
            return rv;
        }

        mep_acc_info.acc_profile_id = acc_ingress_profile;
    }
    else
    {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("Created MEP with id %d\n", mep_acc_info.id);

    ep1.gport = mep_acc_info.gport;
    ep1.id = mep_acc_info.id;

    /*
    * Adding Remote MEPs
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.type = bcmOAMEndpointTypeEthernet;
    rmep_info.ccm_period = 0;
    if((device_type >= device_type_jericho2) &&
       (*(dnxc_data_get(unit, "oam", "oamp",
       "rmep_db_non_zero_lifetime_units_limitation", NULL)))) {
        /** Needed to avoid limitation in JR2 A0 */
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    }
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    if (device_type >= device_type_arad_plus) {
         /* Take RDI only from RX */
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI;
    }

    /* Create three RMEPs for the Down-MEP */
    for (i; i < 23; i++)
    {
        rmep_info.name = i;
        printf("Create RMEP with NAME=%d\n",i);
        rv = bcm_oam_endpoint_create(unit, &rmep_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("Created RMEP with id %d\n", rmep_info.id);
    }

    return rv;
}

/**
 * enable protection packets in addition to interrupts. Global
 * setting for all OAM. Function assumes OAM has been
 * inititalized.
 * Configure the protection header as raw data
 *
 * @author sinai (13/02/2014)
 *
 * @param unit
 * @param dest_port
 *
 * @return int
 */
int enable_oam_protection_packets_raw_header(int unit, int dest_port) {

    int rv;
    int system_headers_mode = soc_property_get(unit , "system_headers_mode", 1);
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    bcm_pkt_blk_t packet_header;
    bcm_pkt_blk_t_init(&packet_header);

    rv = bcm_port_get(unit, 232, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_port_get\n", rv);
      return rv;
    }

    /*
     * JR2: Align protection packet headers size to a multiple of 4.
     * Otherwise, additional bytes of all zeroes are added
     */
    packet_header.len = 32;

    /* PTCH (2B) + ITMH (4B) + Internal header */
    if (device_type < device_type_jericho) {
        unsigned char arr_data[] = { 0x70,mapping_info.pp_port,0x40,0x0c,0x00,0xcb,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        packet_header.data = sal_alloc(packet_header.len, "protection packet headers");
        sal_memcpy(packet_header.data, arr_data, packet_header.len);
    } else if ((device_type >= device_type_jericho2) && (system_headers_mode == 1)) {
        unsigned char arr_data[] = { 0x70, mapping_info.pp_port, 0x02, 0x18, 0x01, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        /* Encode Destination system-port into header to its place */
        arr_data[5] = (dest_port & 0x7f) << 0x1;
        arr_data[4] = (dest_port >> 0x7) & 0xff;
        arr_data[3] = (dest_port >> 0xf) | 0x18;

        /** For JR2, valid trap IDs must be used */
        rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapOampProtection, &protection_trap);
        if (rv != BCM_E_NONE) {
            printf("Failed(%d) bcm_rx_trap_type_create\n", rv);
            return rv;
        }
        protection_trap &= 0xFF;
        arr_data[22] = (protection_trap >> 4); /** Upper nibble */
        arr_data[23] = ((protection_trap & 0xF) << 4) | (arr_data[23] & 0xF);

        bcm_rx_trap_config_t trap_config_protection;
        bcm_rx_trap_config_t_init(&trap_config_protection);

        rv = port_to_system_port(unit, dest_port, &trap_config_protection.dest_port);
        if (rv != BCM_E_NONE){
            printf("Error, in port_to_system_port\n");
            return rv;
        }

        trap_config_protection.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID;

        rv = bcm_rx_trap_set(unit, 0x400 | protection_trap, trap_config_protection);
        if (rv != BCM_E_NONE) {
           printf("trap set: (%s) \n",bcm_errmsg(rv));
           return rv;
        }

        packet_header.data = sal_alloc(packet_header.len, "protection packet headers");
        sal_memcpy(packet_header.data, arr_data, packet_header.len);
    } else {
        /* For JR2 device in interop, the system header should be in JR1 format */
        unsigned char arr_data[] = { 0x70, mapping_info.pp_port, 0x41, 0x00, 0xcb, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        /* Encode Destination system-port into header to its place */
        arr_data[4] = dest_port&0xff;
        arr_data[3] = (dest_port&0xff00)>>8;
        packet_header.data = sal_alloc(packet_header.len, "protection packet headers");
        sal_memcpy(packet_header.data, arr_data, packet_header.len);
    }

    rv = bcm_oam_protection_packet_header_set(unit,&packet_header);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_oam_protection_packet_header_set\n");
        return rv;
    }

    sal_free(packet_header.data);
    return rv;
}

/**
 * enable protection packets in addition to interrupts. Global
 * setting for all OAM. Function assumes OAM has been
 * inititalized.
 *
 * @author sinai (13/02/2014)
 *
 * @param unit
 * @param dest_port
 *
 * @return int
 */
int enable_oam_protection_packets(int unit, int dest_port) {
    bcm_rx_trap_config_t trap_config_protection;
    /* For DPP devices, trap code on FHEI will be (trap_code & 0xff), in this case 1.*/
    /* valid trap codes for oamp traps are 0x401 - 0x4ff */
    int trap_code=0x401;
    int rv, is_dnx, flags = BCM_RX_TRAP_WITH_ID;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv)) {
        printf("bcm_device_member_get failed");
        return(rv);
    }
    if (is_dnx)
    {
        /**
         * For DNX devices, real traps must be used.  They
         * can be created without ID.  When created with ID,
         * They should be created as user defined Rx traps first.
         */
	    bcm_rx_trap_config_t_init(&trap_config_protection);
	    trap_config_protection.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID;
	    /** For DNX, traps are created without ID - allocated as user defined */
	    flags = 0;
    }
	
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapOampProtection, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("trap create: (%s) \n",bcm_errmsg(rv));
       return rv;
    }

    protection_trap = (trap_code & 0xFF);

    rv = port_to_system_port(unit, dest_port, &trap_config_protection.dest_port);
    if (rv != BCM_E_NONE){
        printf("Error, in port_to_system_port\n");
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_protection);
    if (rv != BCM_E_NONE) {
       printf("trap set: (%s) \n",bcm_errmsg(rv));
       return rv;
    }

    return rv;
}

/**
 * sets trap for OAM MAID error to specific port
 *
 * @author aviv (14/12/2014)
 *
 * @param unit
 * @param dest_port - should be local port.
 *
 * @return int
 */
int maid_trap_set(int unit, int dest_port_or_queue, int is_queue) {
	bcm_rx_trap_config_t trap_config;
	/*
	 * For DPP, trap code on FHEI  will be (trap_code & 0xff), in this case 0x16.
	 * valid trap codes for oamp traps are 0x401 - 0x4ff
	*/
	int trap_code=0x416;
	/* For DPP, valid trap codes for oamp traps are 0x401 - 0x4ff */
	int rv, is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv)) {
        printf("bcm_device_member_get failed");
        return(rv);
    }
    if (is_dnx)
    {
        /**
         * For DNX family, an actual trap was created on startup for this
         * punt reason.  To modify it, it must be retrieved, then modified
         * or destroyed and recreated
         */
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOampMaidErr, &trap_code);
        if (rv != BCM_E_NONE) {
           printf("trap create: (%s) \n",bcm_errmsg(rv));
           return rv;
        }
        if(maid_punt_trap_del_create)
        {
            /** Destroy and recreate option */
            rv = bcm_rx_trap_type_destroy(unit, trap_code);
            if (rv != BCM_E_NONE) {
               printf("trap create: (%s) \n",bcm_errmsg(rv));
               return rv;
            }
            rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapOampMaidErr, &trap_code);
            if (rv != BCM_E_NONE) {
               printf("trap create: (%s) \n",bcm_errmsg(rv));
               return rv;
            }
        }
        maid_punt_trap = (trap_code & 0xFF);
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_WITH_ID;
    }
    else
    {
        rv =  bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapOampMaidErr, &trap_code);
        if (rv != BCM_E_NONE) {
           printf("trap create: (%s) \n",bcm_errmsg(rv));
           return rv;
        }
    }
    if (is_queue) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(trap_config.dest_port, dest_port_or_queue);
    }
    else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(trap_config.dest_port, dest_port_or_queue);
    }

	rv = bcm_rx_trap_set(unit, trap_code, trap_config);
    if (rv != BCM_E_NONE) {
       printf("trap set: (%s) \n",bcm_errmsg(rv));
       return rv;
   }

	return rv;
}

/**
 * Unregister OAM events registered using funtion register_events

 *
 *
 * @param unit
 * @return int
 */
int unregister_events(int unit) {
  bcm_error_t rv;
  bcm_oam_event_types_t timeout_event, timein_event, port_interface_event;

  BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
  rv = bcm_oam_event_unregister(unit, timeout_event, cb_oam);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
  rv = bcm_oam_event_unregister(unit, timein_event, cb_oam);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemote);
  rv = bcm_oam_event_unregister(unit, timein_event, cb_oam);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointRemoteUp);
  rv = bcm_oam_event_unregister(unit, timein_event, cb_oam);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointPortDown);
  rv = bcm_oam_event_unregister(unit, port_interface_event, cb_oam);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  BCM_OAM_EVENT_TYPE_CLEAR_ALL(port_interface_event);
  BCM_OAM_EVENT_TYPE_SET(port_interface_event, bcmOAMEventEndpointInterfaceDown);
  rv = bcm_oam_event_unregister(unit, port_interface_event, cb_oam);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}

/**
 * Create a MIP
 *
 *
 * @param unit
 *
 * @return int
 */
int create_mip(int unit, int md_level_mip, bcm_oam_group_t group, bcm_gport_t gport, bcm_mac_t dst_mac, int set_my_cfm_mac) {
  bcm_error_t rv;
  uint32 flags = 0;
  bcm_oam_profile_t ingress_profile = 1;
  bcm_oam_profile_t egress_profile  = 1;

  rv = oam__device_type_get(unit, &device_type);
  if (rv != BCM_E_NONE)
  {
    printf("Error in checking the device type\n");
    return rv;
  }

  /*
   * Get default profile encoded values.
   * Default profile is 1 and
   * configured on init(application) stage.
   * Relevant for Jr2 ONLY
   */
    if(device_type >= device_type_jericho2)
    {
      rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
      if (rv != BCM_E_NONE)
      {
        printf("bcm_oam_profile_id_get_by_type(ingress) \n");
        return rv;
      }

      rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
      if (rv != BCM_E_NONE)
      {
        printf("bcm_oam_profile_id_get_by_type(egress)\n");
        return rv;
      }

      rv = bcm_oam_lif_profile_set(unit, 0, gport, ingress_profile, egress_profile);
      if (rv != BCM_E_NONE)
      {
        printf("bcm_oam_lif_profile_set\n");
        return rv;
      }
    }

  /*
  * Adding a MIP
  */
  bcm_oam_endpoint_info_t_init(&mip_info);
  mip_info.type = bcmOAMEndpointTypeEthernet;
  mip_info.group = group;
  mip_info.level = md_level_mip;
  mip_info.gport = gport;
  mip_info.flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;

  if (device_type < device_type_jericho2)
  {
    sal_memcpy(mip_info.dst_mac_address, dst_mac, 6);
  }
  else
  {
    if (set_my_cfm_mac)
    {
      /* Setting My-CFM-MAC (for Jericho2 Only)*/
      rv = oam__my_cfm_mac_set(unit, port_2, dst_mac);
      if (rv != BCM_E_NONE)
      {
        printf("oam__my_cfm_mac_set fail \n");
        return rv;
      }
    }
  }

  printf("bcm_oam_endpoint_create mip_info\n");
  rv = bcm_oam_endpoint_create(unit, &mip_info);
  if (rv != BCM_E_NONE)
  {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }
  printf("Created MIP with id %d\n", mip_info.id);

  ep3.gport = mip_info.gport;
  ep3.id = mip_info.id;

  return rv;
}

/**
 * Create OAM group and MIP
 *
 *
 * @param unit
 *
 * @return int
 */
int oam_create_mip_with_defaults (int unit) {
  bcm_error_t rv;

  printf("Creating group\n");
  bcm_oam_group_info_t_init(&group_info_short_ma);
  sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_short_ma);
  if (rv != BCM_E_NONE)
  {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }

  rv = create_mip(unit, md_level_mip, group_info_short_ma.id, gport2, mac_mep_2, 0);
  if (rv != BCM_E_NONE)
  {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }

  return rv;
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
int default_ep_example(int unit, int port1, int port2, int port3, int advanced_mode) {
  bcm_error_t rv;
  bcm_oam_endpoint_info_t default_info;
  uint32 flags;

  rv = oam__device_type_get(unit, &device_type);
  if (rv < 0) {
      printf("Error in oam__device_type_get.\n");
      print rv;
      return rv;
  }

  if(device_type < device_type_jericho2)
  {
     single_vlan_tag = 1;

     port_1 = port1;
     port_2 = port2;
     port_3 = port3;

     printf("create_vswitch_and_mac_entries\n");
     rv = create_vswitch_and_mac_entries(unit);
     if (rv != BCM_E_NONE) {
             printf("(%s) \n",bcm_errmsg(rv));
             return rv;
     }

     printf("Add default up mep (EGRESS0)\n");
     bcm_oam_endpoint_info_t_init(&default_info);
     default_info.id = BCM_OAM_ENDPOINT_DEFAULT_EGRESS0;
     default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID|BCM_OAM_ENDPOINT_UP_FACING;
     default_info.level = 5;
     default_info.timestamp_format = get_oam_timestamp_format(unit);


     rv = bcm_oam_endpoint_create(unit, &default_info);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n",bcm_errmsg(rv));
         return rv;
     }

     if (advanced_mode) {
         printf("Set LIF profile for port: %d\n", port1);
         rv = bcm_port_class_set(unit, gport1, bcmPortClassFieldIngress, 5 /* InLIF profile.*/);
         if (rv != BCM_E_NONE) {
             printf("(%s) \n",bcm_errmsg(rv));
             return rv;
         }
         printf("Set mapping from inlif profile 5 to OAM trap profile 1\n", port1);
     }
     else {
         printf("Set LIF profile for port: %d\n", port1);
     }
  }

  rv = bcm_port_control_set(unit, gport1, bcmPortControlOamDefaultProfile, 1/*OAM trap profile */ );
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  printf("Add default down mep (INGRESS1)\n");
  bcm_oam_endpoint_info_t_init(&default_info);
  default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS1;
  default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
  default_info.level = 3;
  if(device_type < device_type_jericho2)
  {
     default_info.timestamp_format = get_oam_timestamp_format(unit);
  }

  rv = bcm_oam_endpoint_create(unit, &default_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  if(device_type >= device_type_jericho2)
  {
     flags = BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_1;
     rv = bcm_oam_lif_profile_set(unit, flags, BCM_GPORT_INVALID, mp_profile, BCM_OAM_PROFILE_INVALID);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n",bcm_errmsg(rv));
         return rv;
     }
  }

  return rv;
}

