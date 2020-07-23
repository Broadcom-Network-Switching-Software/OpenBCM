/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_pon_mac_limit.c
 * Purpose: An example of the mac limiting base the tunnel id function. 
 *          
 * Calling sequence:
 *
 * Initialization:
 *  1. Add the following port configureations to config-sand.bcm
 *     
 *     # Map bcm local port to Network-Interface[.channel] interfaces - TBD
 *     ucode_port_128.BCM88650=10GBase-R15
 *     ucode_port_1.BCM88650=10GBase-R14
 *     ## pon application
 *     pon_application_support_enabled_1.BCM88650=TRUE
 *     mcs_load_uc0=pon to enable loading ukernel core 0
 *     l2_learn_limit_mode=2 to configure the PON tunnel Id MAC limit.
 *     port_priorities.BCM88650=8 to configure the number of priorities at egq.
 *     num_queues_pci=40 to configure the ARAD cosq numbers, from 0-39.
 *     num_queues_uc0=8 to configure the ARM core 0 cosq numbers, from 40-47. 
 *     ucode_port_204.BCM88650=CPU.5 to configure local port 204 to send the DSP packet.  
 *
 *  2. Add the following PON application enabling configureations to config-sand.bcm
 *        pon_application_support_enabled_1.BCM88650=TRUE
 *        vlan_match_criteria_mode=PON_PCP_ETHERTYPE
 *  3  Add the following configuration to enable mac limiting base tunnel id function
 *       l2_learn_limit_mode=2
 *  3. Set PON port and NNI port to recognize single stag, single ctag and s_c_tag frames.
 *        - call port_tpid_init()
 *        - call bcm_port_tpid_add()
 *  4. Remove Ports from VLAN 1.
 *        - call bcm_vlan_gport_delete_all()
 *  5. Disable membership in PON ports (SDK initialization already disabled membership in NNI ports).
 *        - call bcm_port_vlan_member_set()
 *  6. Enable additional port tunnel lookup in PON ports.
 *        - call bcm_vlan_control_port_set()
 *
 * N:1 Service:
 *
 * Set up sequence:
 *  1. Create PON LIF
 *        - Call bcm_vlan_port_create() with criteria BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN.
 *  2. Add PON LIF to vswitch
 *        - Call bcm_vswitch_port_add()
 *  3. Set multicast settings,Add PON-LIFs to MC-ID = VSI + 16K
 *        - Call bcm_multicast_vlan_encap_get()
 *        - Call bcm_multicast_ingress_add()
 *  4. Set PON LIF ingress VLAN editor.
 *        - Call bcm_vlan_translate_action_create()
 *  5. Set PON LIF egress VLAN editor.
 *        - Call bcm_vlan_translate_egress_action_add()
 *  6. Create NNI LIF
 *        - Call bcm_vlan_port_create()with criterias BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED.
 *  7. Add NNI LIF to vswitch
 *        - Call bcm_vswitch_port_add()
 *  8. Set multicast settings, Add NNI-LIF to MC-ID = VSI
 *        - Call bcm_multicast_vlan_encap_get()
 *        - Call bcm_multicast_ingress_add()
 *  9. Set the multicast group offset to flood downstream packets in multicast group
 *        - Call bcm_port_control_set (NNI-LIF, offset= X constant)
 * Clean up sequence:
 *  1. Remove PON and NNI LIFs from VSI.
 *        - Call bcm_vswitch_port_delete()
 *  2. Delete PON and NNI LIFs.
 *        - Call bcm_vlan_port_destroy()
 *  3. Destroy the multicast groups.
 *        - Call bcm_multicast_destroy()
 *  4. Destroy the VSI.
 *        - Call bcm_vswitch_destroy()

 * Service Model:
 * 1:1 Service:
 *     PON Port 1 or 7  <------------------------------->  VSI  <---> NNI Port 128
 *     Pon 1 Tunnel-ID 1 clvan 10 ------------------------------|---4096  |----------- SVLAN 100 cvlan 10
 *     Pon 1 Tunnel-ID xxx cvlan 10 ----------------------------|
 *     Pon 1 Tunnel-ID max_tunnel_num cvlan 10 -----------------|
 *
 * Traffic: set MAC limiting number in tuunel 10 is 5, and send traffic with 10 different SAs. 
 *          System can only learn 5 SAs and only the packets with these SAs will be passed.
 *
 *  1. PON Port 1 Tunnel-ID 10 CVLAN 10 <-CrossConnect-> NNI Port 128 SVLAN 100 CVLAN 10
 *        - From PON port:
 *              -   ethernet header with any DA, and SA will increase by 1 per packet
 *              -   Tunnel ID:10
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 10
 *        - From NNI port:
 *              -   ethernet header with any DA, SA 
 *              -   VLAN tag: VLAN tag type 0x8100, VID = 100 and 0x9100, VID = 10
 *
 *
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_mact_limit.c
 *      BCM> cint
 *      cint> pon_mact_limit_init(unit, pon_port, nni_port, max_tunnel_num);
 *      cint> pon_mact_limit_app(unit, cpu_learn, dsp_parsing, dsp_port);
 *      cint> pon_mac_limit_set(unit, pon_port,tunnel_id,limit_num);
 *      cint> 
 *      cint> pon_mact_limit_app_clean(unit);
 *
 *      For verify L2 management APIs, please run this function
 *      cint> pon_l2_runs(unit, api_idx, flags, port, vlan, mac_addr_1)
 */

struct pon_mact_limit_info_s{
    bcm_vlan_t  vsi;
    bcm_gport_t pon_gport[16384]; /*2048*8*/
    bcm_gport_t nni_gport[1];
    bcm_port_t pon_port[8];
    bcm_port_t nni_port[8];
    int nni_svlan;/* outer vlan number at NNI port*/
    int pon_cvlan;
    int age_seconds;
    int max_tunnel_num; /*max tunnel id number per pon port*/
    int lif_offset; /* 16*1024, Offset for downstream flooding multicast group(MC = VSI + lif_offset)*/
};

pon_mact_limit_info_s pon_mact_limit_info;

/* Initialize PON application configurations.
 * Example: pon_mact_limit_init(unit, 1, 128, 16);
 */
int pon_mact_limit_init(int unit, bcm_port_t pon_port, bcm_port_t nni_port, int max_tunnel_num)
{
    int rv = BCM_E_NONE;
    int i = 0;
    int j = 0;

    /*Enable the l2 learn limit feature for PON MACT application*/
    rv = bcm_l2_learn_limit_enable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_enable failed $rv\n");
        return rv;
    }
 
    pon_mact_limit_info.vsi = 0;
    pon_mact_limit_info.max_tunnel_num = max_tunnel_num;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < pon_mact_limit_info.max_tunnel_num; j++)
        {
            pon_mact_limit_info.pon_gport[i*2048 + j] = 0;
        }
    }
    pon_mact_limit_info.nni_gport[0] = 0;

    for (i = 0; i < 8; i++)
    {
        pon_mact_limit_info.pon_port[i] = -1;
        pon_mact_limit_info.nni_port[i] = -1;
    }

    pon_mact_limit_info.age_seconds = 60;
    pon_mact_limit_info.lif_offset = lif_offset;

    /* use 2 pon port: pon 1 and pon 7, 1 nni port: 128 */
    pon_mact_limit_info.nni_svlan = 100;
    pon_mact_limit_info.pon_cvlan = 10;
    pon_mact_limit_info.nni_port[0] = nni_port;
    pon_mact_limit_info.pon_port[0] = pon_port;

    pon_app_init(unit, pon_port, nni_port, 0, 0);
}

/*
 * set up the pon mact learning message.
 */
int pon_mact_learning_msg_set(int unit, int dsp_port)
{
    int rv = BCM_E_NONE;
    bcm_l2_learn_msgs_config_t learn_msgs;
    
    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;
    /* follow attriburtes set the destination port/group shadow messages are distributed to */
    learn_msgs.dest_port = dsp_port;

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

/*********************************************************************************
* Setup pon mac limiting test model if cpu_learn isn't 0; otherwise, it's a normal PP model.
*     PON Port 1 or 7  <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 1 clvan 10 ------------------------------|---4096  |----------- SVLAN 100 cvlan 10
*     Pon 1 Tunnel-ID xxx cvlan 10 ----------------------------|
*     Pon 1 Tunnel-ID max_tunnel_num cvlan 10 -----------------|
*                                                 
* steps
 */
int pon_mact_limit_app(int unit, int cpu_learn, int dsp_parsing, int dsp_port)
{
    int rv = 0;
    int i = 0;
    bcm_if_t encap_id = 0;
    int tunnel_id = 0;
    int pon_port_idx = 0;
    int action_id = 0;
    /* In pon_app_init(), lif_offset will be updated with differenced value based on device type. */
    pon_mact_limit_info.lif_offset = lif_offset;

    if (cpu_learn != 0) {
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
    }

    /* Enabling aging to 60 sec */
    rv = bcm_l2_age_timer_set(unit, pon_mact_limit_info.age_seconds);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_age_timer_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_create(unit, &(pon_mact_limit_info.vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (pon_port_idx = 0; pon_port_idx < 8; pon_port_idx++)
    {
        if (pon_mact_limit_info.pon_port[pon_port_idx] != (-1))
        {
            for (tunnel_id = 0; tunnel_id < pon_mact_limit_info.max_tunnel_num; tunnel_id++)
            {
                /* Create PON LIF */
                rv = pon_lif_create(unit,
                                    pon_mact_limit_info.pon_port[pon_port_idx], 
                                    match_otag,
                                    0, 0,
                                    tunnel_id,
                                    tunnel_id,
                                    pon_mact_limit_info.pon_cvlan,
                                    0, 
                                    0, 0, 
                                    &(pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id]));
                if (rv != BCM_E_NONE)
                {
                    printf("Error, pon_lif_create returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }

                /* Add PON LIF to vswitch */
                rv = bcm_vswitch_port_add(unit,
                                          pon_mact_limit_info.vsi, 
                                          pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_vswitch_port_add returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }
                /*For downstream*/
                rv = multicast_vlan_port_add(unit, 
                                             pon_mact_limit_info.vsi+pon_mact_limit_info.lif_offset, 
                                             pon_mact_limit_info.pon_port[pon_port_idx], 
                                             pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id]);
											 
				if (rv != BCM_E_NONE)
                {
                    printf("Error, in multicast_vlan_port_add");
                    return rv;
                }							 

                /* Set PON LIF ingress VLAN editor */
                rv = pon_port_ingress_vt_set(unit, 
                                             otag_to_o_i_tag,
                                             pon_mact_limit_info.nni_svlan, 
                                             pon_mact_limit_info.pon_cvlan,
                                             pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id], 
                                             0,
                                             &action_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, pon_port_ingress_vt_set returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }                                             
                /* Set PON LIF egress VLAN editor */
                rv = pon_port_egress_vt_set(unit,
                                            otag_to_otag2,
                                            tunnel_id,
                                            pon_mact_limit_info.pon_cvlan, 0,
                                            pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, pon_port_egress_vt_set returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }
            }
        }
    }

     /* Create NNI LIF */
     rv = nni_lif_create(unit, 
                         pon_mact_limit_info.nni_port[0], 
                         match_o_i_tag, 0, 
                         pon_mact_limit_info.nni_svlan,
                         pon_mact_limit_info.pon_cvlan,
                         0,
                         &(pon_mact_limit_info.nni_gport[0]), &encap_id);
     if (rv != BCM_E_NONE) {
         printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
         return rv;
     }

     /*For upstream*/
     rv = multicast_vlan_port_add(unit, pon_mact_limit_info.vsi, pon_mact_limit_info.nni_port[0],
                                  pon_mact_limit_info.nni_gport[0]);
     if (rv != BCM_E_NONE) {
         printf("Error, multicast_vlan_port_add returned %s nni_gport\n", bcm_errmsg(rv));
         return rv;
     }

     /* add to vswitch */
     rv = bcm_vswitch_port_add(unit, pon_mact_limit_info.vsi, pon_mact_limit_info.nni_gport[0]);
     if (rv != BCM_E_NONE) {
         printf("Error, bcm_vswitch_port_add returned %s nni_gport\n", bcm_errmsg(rv));
         return rv;
     }
     
     /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
      bcm_port_control_set(unit, pon_mact_limit_info.nni_gport[0], bcmPortControlFloodUnknownUcastGroup, lif_offset);
      bcm_port_control_set(unit, pon_mact_limit_info.nni_gport[0], bcmPortControlFloodUnknownMcastGroup, lif_offset);
      bcm_port_control_set(unit, pon_mact_limit_info.nni_gport[0], bcmPortControlFloodBroadcastGroup, lif_offset);

    return rv;
}

/*********************************************************************************
* Setup the MAC limit number for per tunnel id: 
*/
int pon_mac_limit_set(int unit, int port, int tunnel_id, int limit_num) 
{
    int rv = 0;
    bcm_l2_learn_limit_t limit;
    
    bcm_l2_learn_limit_t_init(&limit);
    limit.flags |= BCM_L2_LEARN_LIMIT_TUNNEL;
    limit.port = port;
    limit.tunnel_id = tunnel_id;
    limit.limit = limit_num;

    rv = bcm_l2_learn_limit_set(unit, &limit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int pon_range_mac_limit_set(int unit, int port, int tunnel_id_start, int tunnel_id_end, int limit_num) 
{
    int rv = 0;
    bcm_l2_learn_limit_t limit;
    int i = 0;
    
    bcm_l2_learn_limit_t_init(&limit);
    limit.flags |= BCM_L2_LEARN_LIMIT_TUNNEL;
    limit.port = port;
    limit.limit = limit_num;

    for (i = tunnel_id_start; i <= tunnel_id_end; i++)
    {
        limit.tunnel_id = i;
        rv = bcm_l2_learn_limit_set(unit, &limit);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_learn_limit_set returned %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

int pon_mac_limit_disable(int unit, int port, int tunnel_id, int limit_num) 
{
    int rv = 0;
    bcm_l2_learn_limit_t limit;
    
    bcm_l2_learn_limit_t_init(&limit);
    limit.flags |= (BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TUNNEL);
    limit.port = port;
    limit.tunnel_id = tunnel_id;
    if (limit_num == -1)
    {
        limit.limit = 0x7FFF;
    }

    rv = bcm_l2_learn_limit_set(unit, &limit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int pon_mac_aging_disable(int unit) 
{
    int rv = 0;

    rv = bcm_l2_age_timer_set(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_age_timer_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int pon_mac_aging_enable(int unit, int age_seconds) 
{
    int rv = 0;

    rv = bcm_l2_age_timer_set(unit, age_seconds);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_age_timer_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Clean  pon mac limiting configuration: 
*/
int pon_mact_limit_app_clean(int unit)
{
    int rv;
    int tunnel_id = 0;
    int pon_port_idx = 0;

    /* Delete pon gport */
    for (pon_port_idx = 0; pon_port_idx < 8; pon_port_idx++)
    {
        if (pon_mact_limit_info.pon_port[pon_port_idx] != (-1))
        {
            for (tunnel_id = 0; tunnel_id < pon_mact_limit_info.max_tunnel_num; tunnel_id++)
            {
                if (pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id] != 0)
                {
                    rv = vswitch_delete_port(unit, pon_mact_limit_info.vsi, pon_mact_limit_info.pon_gport[pon_port_idx*2048+tunnel_id]);
                    if (rv != BCM_E_NONE) 
                    {
                        printf("Error, vswitch_delete_port() returned %s remove pon gport[%d] failed\n",
                               bcm_errmsg(rv), pon_port_idx*2048+tunnel_id);
                        return rv;
                    }
                }
            }
        }
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_mact_limit_info.vsi, pon_mact_limit_info.nni_gport[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s, remove nni gport[0] failed\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_delete(unit, pon_mact_limit_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete returned %s remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }

    /*Disable the l2 learn limit feature for PON MACT application*/
    rv = bcm_l2_learn_limit_disable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_disable failed $rv\n");
        return rv;
    } 

    return rv;
}

/* call back for get block */ 
int call_back_get_block(int unit, bcm_l2_addr_t *l2e, void* ud)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x |",
		 l2e->mac[0],l2e->mac[1],l2e->mac[2],
		 l2e->mac[3],l2e->mac[4],l2e->mac[5]);
    printf("VLAN_TAG=0x%x| PORT=%d, |\n",
		 l2e->vid, l2e->port);
		 
		 return BCM_E_NONE;
}

/* call back to count */
int call_back_count(int unit, bcm_l2_addr_t *l2e, int* ud) 
{
    ++*ud;
    return BCM_E_NONE;
}

/* Get MAC Address list per VLAN port */
int get_mac_per_ac(int unit, int pon_port, int tunnel_id)
{
    bcm_l2_addr_t match_addr;
    bcm_mac_t mac1;
    int v =0;
    bcm_l2_addr_t_init(&match_addr,mac1,v);
    int flags = BCM_L2_TRAVERSE_MATCH_DEST | BCM_L2_TRAVERSE_IGNORE_DES_HIT;
    int vlan_port_id;
    int a=0;
    int rv;
    int i = 0;

    for (i = 0; i < 8; i++)
    {
        if(pon_mact_limit_info.pon_port[i] == pon_port)
        {
            break;
        }
    }

	vlan_port_id = pon_mact_limit_info.pon_gport[i*2048+tunnel_id];
	printf("pon port is %d, index:%d, channel is %d - gport is %d\n", pon_port, i, tunnel_id, vlan_port_id);
	match_addr.port = vlan_port_id;
	rv = bcm_l2_matched_traverse(unit,flags, &match_addr, call_back_get_block, &a);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_matched_traverse returned %s, output l2 entries\n", bcm_errmsg(rv));
        return rv;
    }

	a = 0;
	rv = bcm_l2_matched_traverse(unit,flags, &match_addr, call_back_count, &a);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_matched_traverse returned %s, get counter\n", bcm_errmsg(rv));
        return rv;
    }
    printf("\ncount is: %d\n\r",a);

	return rv;
}

/* Get MAC address number per VLAN port */
int get_mac_num_per_ac(int unit, int pon_port, int tunnel_id)
{
    bcm_l2_addr_t match_addr;
    bcm_mac_t mac1;
    int v =0;
    bcm_l2_addr_t_init(&match_addr,mac1,v);
    int flags = BCM_L2_TRAVERSE_MATCH_DEST | BCM_L2_TRAVERSE_IGNORE_DES_HIT;
    int vlan_port_id;
    int a=0;
    int rv;
    int i = 0;

    for (i = 0; i < 8; i++)
    {
        if(pon_mact_limit_info.pon_port[i] == pon_port)
        {
            break;
        }
    }

	vlan_port_id = pon_mact_limit_info.pon_gport[i*2048+tunnel_id];
	match_addr.port = vlan_port_id;
	rv = bcm_l2_matched_traverse(unit,flags, &match_addr, call_back_count, &a);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_matched_traverse returned %s\n", bcm_errmsg(rv));
        return rv;
    }

	return a;
}

enum bcm_pon_l2_apis_e {
    pon_l2_add = 0,              /* bcm_l2_addr_add */
    pon_l2_delete,               /* bcm_l2_addr_delete */
    pon_l2_get,                  /* bcm_l2_addr_get */
    pon_l2_delete_by_mac,        /* bcm_l2_addr_delete_by_mac */
    pon_l2_delete_by_mac_port,   /* bcm_l2_addr_delete_by_mac_port */
    pon_l2_delete_by_vlan,       /* bcm_l2_addr_delete_by_vlan */
    pon_l2_delete_by_vlan_port,  /* bcm_l2_addr_delete_by_vlan_port */
    pon_l2_delete_by_port,       /* bcm_l2_addr_delete_by_port */
    pon_l2_replace,              /* bcm_l2_replace */
    pon_l2_match_traverse,       /* bcm_l2_matched_traverse */
    pon_l2_traverse              /* bcm_l2_traverse */
};

/*
 * Verify BCM L2 APIs function.
 *   bcm_l2_addr_add - api_idx = 0
 *   bcm_l2_addr_delete  - api_idx = 1
 *   bcm_l2_addr_get - api_idx = 2
 *   bcm_l2_addr_delete_by_mac - api_idx = 3
 *   bcm_l2_addr_delete_by_mac_port - api_idx = 4
 *   bcm_l2_addr_delete_by_vlan - api_idx = 5
 *   bcm_l2_addr_delete_by_vlan_port - api_idx = 6
 *   bcm_l2_addr_delete_by_port - api_idx = 7
 *   bcm_l2_replace - api_idx = 8
 *   bcm_l2_matched_traverse - api_idx = 9
 *   bcm_l2_traverse - api_idx = 10
 *
 * Steps
 *   bcm_mac_t mac_addr = {0,0,0,3,2,1};
 *   bcm_mac_t static_mac_addr = {0,0,0,0,2,1};
 *   print pon_l2_runs(0, pon_l2_add, 0, pon_mact_limit_info.pon_gport[1], pon_mact_limit_info.vsi, mac_addr);
 *   print pon_l2_runs(0, pon_l2_add, BCM_L2_STATIC, pon_mact_limit_info.pon_gport[1], pon_mact_limit_info.vsi, static_mac_addr);
 *   print pon_l2_runs(0, pon_l2_get, 0, 0,  pon_mact_limit_info.vsi, mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete, 0, 0,  pon_mact_limit_info.vsi, mac_addr);
 *   bcm_mac_t match_mac_addr = {0x0,0x0,0x30,0x0, 0x0,0x01};
 *   print pon_l2_runs(0, pon_l2_delete_by_mac, 0, 0,  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_mac_port, 0, pon_mact_limit_info.pon_port[0],  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_vlan, 0, 0,  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_vlan, BCM_L2_DELETE_NO_CALLBACKS, 0,  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_vlan_port, 0, pon_mact_limit_info.pon_gport[1],  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_vlan_port, BCM_L2_DELETE_NO_CALLBACKS, pon_mact_limit_info.pon_port[0],  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_port, 0, pon_mact_limit_info.pon_gport[1],  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_delete_by_port, BCM_L2_DELETE_NO_CALLBACKS, pon_mact_limit_info.pon_gport[1],  pon_mact_limit_info.vsi, match_mac_addr);
 *
 *   int flags = BCM_L2_REPLACE_MATCH_STATIC|BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_NO_CALLBACKS|
 *        BCM_L2_REPLACE_IGNORE_DISCARD_SRC|BCM_L2_REPLACE_IGNORE_DES_HIT;
 *
 *   print pon_l2_runs(0, pon_l2_replace, flags, pon_mact_limit_info.pon_gport[1],  pon_mact_limit_info.vsi, match_mac_addr);
 *
 *   int flags = BCM_L2_TRAVERSE_MATCH_DEST | BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC | BCM_L2_TRAVERSE_IGNORE_DES_HIT;
 *   print pon_l2_runs(0, pon_l2_match_traverse, flags, pon_mact_limit_info.pon_gport[1],  pon_mact_limit_info.vsi, match_mac_addr);
 *   print pon_l2_runs(0, pon_l2_traverse, 0, pon_mact_limit_info.pon_gport[1],  pon_mact_limit_info.vsi, match_mac_addr);
 */
int pon_l2_runs(int unit, int api_idx, int flags, int port, int vlan, bcm_mac_t mac_addr_1)
{
    int rv = BCM_E_NONE;
    bcm_mac_t mac_addr_zero = {0,0,0,0,0,0};
    bcm_l2_addr_t l2_addr;
    int count = 0;

    switch(api_idx) {
        case pon_l2_add:
            bcm_l2_addr_t_init(&l2_addr, mac_addr_1, vlan); 
            l2_addr.port = port;
            l2_addr.flags = flags;
            rv = bcm_l2_addr_add(unit, &l2_addr);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_add returned %s \n", bcm_errmsg(rv));
                return rv;
            }
            break;
        case pon_l2_delete:
            rv = bcm_l2_addr_delete(unit, mac_addr_1, vlan);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_delete returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            break;
        case pon_l2_get:
            bcm_l2_addr_t_init(&l2_addr, mac_addr_zero, 0);
            rv = bcm_l2_addr_get(unit, mac_addr_1, vlan, &l2_addr);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_get returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            printf("L2 entry:\n");
            print l2_addr;
            printf("\n");
            break;
        case pon_l2_delete_by_mac:
            rv = bcm_l2_addr_delete_by_mac(unit, mac_addr_1, flags);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_delete_by_mac returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }

            break;
        case pon_l2_delete_by_mac_port:
            rv = bcm_l2_addr_delete_by_mac_port(unit, mac_addr_1, 0, port, flags);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_delete_by_mac returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }

            break;
        case pon_l2_delete_by_vlan:
            rv = bcm_l2_addr_delete_by_vlan(unit, vlan, flags);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_delete_by_vlan returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            break;
        case pon_l2_delete_by_vlan_port:
            rv = bcm_l2_addr_delete_by_vlan_port(unit, vlan, 0, port, flags);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_delete_by_vlan_port returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            break;
        case pon_l2_delete_by_port:
            rv = bcm_l2_addr_delete_by_port(unit, 0, port, flags);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_l2_addr_delete_by_port returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            break;
        case pon_l2_replace:
            bcm_l2_addr_t_init(&l2_addr, mac_addr_zero, 0);
            l2_addr.port = port;
            rv = bcm_l2_replace(unit, flags, &l2_addr, 0, 0, 0);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_replace returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            break;
        case pon_l2_match_traverse:
            bcm_l2_addr_t_init(&l2_addr, mac_addr_zero, 0);
            l2_addr.port = port;
            rv = bcm_l2_matched_traverse(unit, flags, &l2_addr, call_back_count, &count);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_matched_traverse returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            printf("count:%d for gport %08x\n", count, port);
            break;
        case pon_l2_traverse:
            rv = bcm_l2_traverse(unit, call_back_count, &count);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_traverse returned %s failed\n", bcm_errmsg(rv));
                return rv;
            }
            printf("count:%d\n", count);
            break;
        default:
            printf("ERR: pon_l2_runs INVALID PARAMETER api_idx %d\n", api_idx);
            return BCM_E_PARAM;
    }

    return rv;

}

