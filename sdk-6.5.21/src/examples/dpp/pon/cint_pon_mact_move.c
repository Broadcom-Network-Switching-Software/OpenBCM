/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_mac_move.c
* Purpose: An example of the PON application for MAC move basedon DSP parsing mode
*     or MACT Event FIFO mode. The MAC move feature can be supported both on PON and NNI port.
*
* Soc Properties configuration:
*     mcs_load_uc0=pon to enable loading ukernel core 0
*     l2_learn_limit_mode=2 to configure the PON tunnel Id MAC limit.
*     port_priorities.BCM88650=8 to configure the number of priorities at egq.
*     num_queues_pci=40 to configure the ARAD cosq numbers, from 0-39.
*     num_queues_uc0=8 to configure the ARM core 0 cosq numbers, from 40-47. 
*     ucode_port_204.BCM88650=CPU.5 to configure local port 204 to send the DSP packet.  
*
* Calling sequence:
*
* Initialization:
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_129.BCM88650=10GBase-R15
*        ucode_port_5.BCM88650=10GBase-R14
*        ucode_port_4.BCM88650=10GBase-R13
*  2. Set PON port and NNI port to recognize single stag, single ctag and s_c_tag frames.
*        - call port_tpid_init()
*        - call bcm_port_tpid_add()
*  3. Remove Ports from VLAN 1.
*        - call bcm_vlan_gport_delete_all()
*  4. Disable membership in PON ports (SDK initialization already disabled membership in NNI ports).
*        - call bcm_port_vlan_member_set()
*  5. Enable additional port tunnel lookup in PON ports.
*        - call bcm_vlan_control_port_set()
*
* N:1 Service:
* Set up sequence:
*  1. Create vswitch for basic bridging
*        - Call bcm_vswitch_create()
*        - Call bcm_multicast_create() 
*  2. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED, 
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE,
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE
*  3. Add PON LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  4. Set multicast settings,Add PON-LIFs to MC-ID = VSI + 16K
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  5. Set PON LIF ingress VLAN editor.
*        - Call bcm_vlan_translate_action_create()
*  6. Set PON LIF egress VLAN editor.
*        - Call bcm_vlan_translate_egress_action_add()
*  7. Create NNI LIF only once
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED           
*  8. Add NNI LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  9. Set multicast settings, Add NNI-LIF to MC-ID = VSI
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  10.Set the multicast group offset to flood downstream packets in multicast group
*        - Call bcm_port_control_set (NNI-LIF, offset= X constant)
* Clean up sequence:
*  1. Remove PON and NNI LIFs from VSI.
*        - Call bcm_vswitch_port_delete()
*  2. Delete PON and NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*
* Clean up sequence:
*  1. Remove PON and NNI LIFs from VSI.
*        - Call bcm_vswitch_port_delete()
*  2. Delete PON and NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*  3. Destroy the multicast groups.
*        - Call bcm_multicast_destroy()
*  4. Destroy the VSI.
*        - Call bcm_vswitch_destroy()
*
* Service Model:
* N:1 Service:
*           PON Port 4,5  <--------------------->  VSI  <---> 1 NNI Port 129
*     PON 1 Tunnel-ID 1000 CVLAN 60--------------|
*     PON 2 Tunnel-ID 1001 CVLAN 61--------------|--- 4097  ---  SVLAN 600 CVLAN 601
*
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/pon/cint_pon_utils.c
*      BCM> cint examples/dpp/pon/cint_pon_mact_move.c
*      BCM> cint
*      cint> pon_mact_learning_setup(unit, dsp_parsing, port, aging_time);
*      cint> pon_app_init(unit, pon_port_1, pon_port_2, nni_port);
*      cint> pon_n_1_service(unit);
*      cint> pon_mact_limit_set(unit, port, tunnel_id, limit_num);
*      cint> pon_mact_mac_move_event_set(unit, enable);
*      cint> pon_mact_event_handle_register(unit);
*      cint> pon_n_1_service_cleanup(unit);
*/

struct pon_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_port_t pon_port;
    bcm_port_t nni_port;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
};


pon_service_info_s pon_n_1_service_info[2][7];

bcm_vlan_t n_1_service_vsi[2];   /*VSI for N:1 services*/
int num_of_n_1_services[2];


/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_mact_move_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0; 
    int service;
    int index;

    rv = bcm_l2_learn_limit_enable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_enable\n");
        return rv;
    } 

    /* N:1 Service without the local route */
    service = 0;
    /* PON Port 4 Tunnel-ID 1000 CVLAN 60 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 600 CVLAN 601 */
    index = 0;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1000;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 60;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 600;
    pon_n_1_service_info[service][index].down_ivlan = 601;
    pon_n_1_service_info[service][index].pon_port = port_pon;
    pon_n_1_service_info[service][index].nni_port = port_nni;

    /* PON Port 5 Tunnel-ID 1001 CVLAN 61 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 600 CVLAN 601 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1001;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 61;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 600;
    pon_n_1_service_info[service][index].down_ivlan = 601;
    pon_n_1_service_info[service][index].pon_port = port_pon;
    pon_n_1_service_info[service][index].nni_port = port_nni;

    /*count number of n_1 services*/
    num_of_n_1_services[service] = index + 1;

    pon_app_init(unit, port_pon, port_nni, 0, 0);

    /* Enable MAC move in PON port */
    rv = bcm_port_control_set(unit, port_pon, bcmPortControlL2Move, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set bcmPortControlL2Move\n");
        return rv;
    }

    return 0;
}

/*
 * Set up N:1 sercies, create VSI and add PON and NNI LIFs to it.
 */
int pon_n_1_service(int unit)
{
    int rv;
    int service;
    int index, is_with_id = 0;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;
    bcm_port_t port_pon, port_nni;

    vswitch_create(unit, &n_1_service_vsi[service]);
    for (index = 0; index < num_of_n_1_services[service]; index++)
    {
        pon_gport = 0;
        nni_gport = 0;
        service_mode           = pon_n_1_service_info[service][index].service_mode;
        tunnel_id              = pon_n_1_service_info[service][index].tunnel_id;
        pon_lif_type           = pon_n_1_service_info[service][index].up_lif_type;
        up_ovlan               = pon_n_1_service_info[service][index].up_ovlan;
        up_ivlan               = pon_n_1_service_info[service][index].up_ivlan;
        up_pcp                 = pon_n_1_service_info[service][index].up_pcp;
        up_ethertype           = pon_n_1_service_info[service][index].up_ethertype;
        nni_lif_type           = pon_n_1_service_info[service][index].down_lif_type;
        down_ovlan             = pon_n_1_service_info[service][index].down_ovlan;
        down_ivlan             = pon_n_1_service_info[service][index].down_ivlan;
        port_pon               = pon_n_1_service_info[service][index].pon_port;
        port_nni               = pon_n_1_service_info[service][index].nni_port;
        /* Create PON LIF */
        pon_lif_create(unit, port_pon, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);

        /* Add PON LIF to vswitch */
        rv = bcm_vswitch_port_add(unit, n_1_service_vsi[service], pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }

        /*For downstream*/
        multicast_vlan_port_add(unit, n_1_service_vsi[service]+lif_offset, port_pon, pon_gport);

        /* Set PON LIF ingress VLAN editor */
        pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);

        /* Set PON LIF egress VLAN editor */
        pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);

        /*Only create one NNI LIF*/
        if (index == 0)
        {
            /* Create NNI LIF */
            nni_lif_create(unit, port_nni, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);

            /*For upstream*/
            multicast_vlan_port_add(unit, n_1_service_vsi[service], port_nni, nni_gport);
            /* Add NNI LIF to vswitch */
            rv = bcm_vswitch_port_add(unit, n_1_service_vsi[service], nni_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vswitch_port_add\n");
                return rv;
            }
            /* Set the multicast group offset to flood downstream packets in multicast group(n_1_service_vsi+lif_offset) */
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset); 
        }
        else
        {
            nni_gport = pon_n_1_service_info[service][0].nni_gport;
        }

        pon_n_1_service_info[service][index].pon_gport = pon_gport;
        pon_n_1_service_info[service][index].nni_gport = nni_gport;
    }

    return rv;
}

/*
 * clean up the configurations of N:1 sercies.
 */
int pon_n_1_service_cleanup(int unit)
{
    int rv;
    int service;
    int index;
    bcm_gport_t pon_gport, nni_gport;

    rv = bcm_l2_learn_limit_disable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_disable\n");
        return rv;
    }

    for (index = 0; index < num_of_n_1_services[service]; index++)
    {
        pon_gport = pon_n_1_service_info[service][index].pon_gport;
        nni_gport = pon_n_1_service_info[service][index].nni_gport;      
           
        rv = vswitch_delete_port(unit, n_1_service_vsi[service], pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_metro_delete_port\n");
            return rv;
        }

        /*Only one NNI LIF*/
        if (index == 0)
        {
            rv = vswitch_delete_port(unit, n_1_service_vsi[service], nni_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_delete_port\n");
                return rv;
            }
        }
    }

    rv = vswitch_delete(unit, n_1_service_vsi[service]);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_delete \n");
        return rv;
    }
	  
    return rv;        
} 


/*
 * set up the pon mact learning mode.
 */
int pon_mact_learning_mode_set(int unit, int dsp_parsing)
{
    int rv = BCM_E_NONE;
    int mode;
    
    if (dsp_parsing) {
        mode = BCM_L2_INGRESS_CENT;
    } else {        
        mode = BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, mode);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set failed $rv\n");
        return rv;
    }

    return rv;
}


/*
 * set up the pon mact aging time.
 */
int pon_mact_aging_time_set(int unit, int aging_time) 
{
    int rv = BCM_E_NONE;

    rv = bcm_l2_age_timer_set(unit, aging_time);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_age_timer_set failed $rv\n");
        return rv;
    }

    return rv;
}


/*
 * set up the pon mact learning message.
 */
int pon_mact_learning_msg_set(int unit, int port)
{
    int rv = BCM_E_NONE;
    bcm_l2_learn_msgs_config_t learn_msgs;
    uint8 _src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
    uint8 _dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};
    
    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;
    /* follow attriburtes set the destination port/group shadow messages are distributed to */
    learn_msgs.dest_port = port;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set failed $rv\n");
        return rv;
    }
    
    return rv;
}


/*
 * set up the pon mact limit.
 */
int pon_mact_limit_set(int unit, int port, int tunnel_id, int limit_num) 
{
    int rv = BCM_E_NONE;
    bcm_l2_learn_limit_t limit;
    
    bcm_l2_learn_limit_t_init(&limit);
    limit.flags |= BCM_L2_LEARN_LIMIT_TUNNEL;
    limit.port = port;
    limit.tunnel_id = tunnel_id;
    limit.limit = limit_num;

    rv = bcm_l2_learn_limit_set(unit, &limit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_set failed $rv\n");
        return rv;
    }

    return rv;
}


/*
 * set up the pon mact learning.
 */
int pon_mact_learning_setup(int unit, int dsp_parsing, int dsp_port, int aging_time)
{
    int rv = BCM_E_NONE;

	if (dsp_parsing) {
	    rv = pon_mact_learning_msg_set(unit, dsp_port);
	    if (rv != BCM_E_NONE) {
            printf("Error, pon_mact_learning_msg_set returned %s\n", bcm_errmsg(rv));
            return rv;
        }            
        rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_CENT);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set returned %s\n", bcm_errmsg(rv));
            return rv;
        } 
    } else {
        rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_CENT|BCM_L2_LEARN_CPU);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set returned %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Enabling aging to 60 sec */
    rv = bcm_l2_age_timer_set(unit, aging_time);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_age_timer_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_l2_addr_t l2_addr;

void pon_mact_mac_move_event_handle(int unit,
	     bcm_l2_addr_t *l2addr,
	     int operation,
	     void *userdata)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr;
    bcm_vlan_port_t vlan_port;

    if (l2addr == NULL || userdata == NULL || operation != BCM_L2_CALLBACK_MOVE_EVENT)
    {
        return rv;
    }

    if (operation == BCM_L2_CALLBACK_MOVE_EVENT) {
        bcm_vlan_port_t_init(&vlan_port);
        /*memcpy(&new_l2_addr, userdata, sizeof(bcm_l2_addr_t));*/
        new_l2_addr = userdata;

        printf("BCM_L2_CALLBACK_MOVE_EVENT handle\n");
        printf("old l2 entry:\n");
        print *l2addr;
        printf("new l2 entry:\n");
        print *new_l2_addr;

        /* get physical port and vlan information by l2 entry */
        vlan_port.vlan_port_id = l2addr->port;
        rv = bcm_vlan_port_find(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_find returned %s failed\n", bcm_errmsg(rv));
        }
        printf("old vlan port:\n");
        print vlan_port;

        vlan_port.vlan_port_id = l2addr->port;
        rv = bcm_vlan_port_find(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, old entry bcm_vlan_port_find returned %s failed\n", bcm_errmsg(rv));
        }
        printf("old vlan port:\n");
        print vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = new_l2_addr->port;
        rv = bcm_vlan_port_find(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, new entry bcm_vlan_port_find returned %s failed\n", bcm_errmsg(rv));
        }
        printf("old vlan port:\n");
        print vlan_port;  
    } 


    return rv;
}


int pon_mact_event_handle_register(int unit) {
    int rv = BCM_E_NONE;
    rv = bcm_l2_addr_register(unit,pon_mact_mac_move_event_handle, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_register \n");
        return rv;
    }

    return rv;
}

/* 
 * Disable/enable report mac move event.
 * 0 -- disable, 1 -- enable
 */
int pon_mact_mac_move_event_set(int unit, int enable) {
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t event_control;

    /* event_id: 1 is presenting MAC move event.
     */
    event_control.action = bcmSwitchEventRead;
    event_control.event_id = 1;
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_event_control_set\n");
        return rv;
    }

    return rv;
}

    
