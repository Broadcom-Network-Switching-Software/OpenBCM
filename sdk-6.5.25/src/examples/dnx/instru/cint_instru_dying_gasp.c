/*
 * $Id: cint_instru_dying_gasp.c
 * $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * File: cint_instru_dying_gasp.c
 * Purpose: Provide an example of necessary configuration which in order to generate dying gasp packets
 *
 * Configure sequence:
 * ------
 *              1) Dying gasp packet egress sending configuration:
 *                  - cint_instru_eventor_vlan_translation - Dying gasp packet egress L2 untag encapulation configuration
 *                  - cint_multicast_to_all_ports_cfg - Dying gasp packet multicast to all NIF interfaces
 *                  - cint_instru_eventor_packet_forward Dying gasp packet L2 forwarding to multicast group
 *
 *              2) Eventor block configuration to generate dying gasp packet
 *                  - cint_instru_eventor_tx_context_cfg - Tx context id configuration
 *                  - cint_instru_eventor_event_cfg - Tx eventor id configuration
 *                  - cint_instru_eventor_tx_builder_dying_gasp_cfg Tx builder id configuration
 *                  - cint_instru_eventor_activate - Activate eventor block
 *
 * Test Scenario:
 * ------------------------manually trigger case------------------------
 * ./bcm.user
 * cd ../../../../src/examples/dnx/instru
 * c cint_instru_eventor.c
 * c cint_instru_dying_gasp.c
 * cint
 * cint_dying_gasp_pkt_gen_config_example(0);
 * cint_packet_manual_generate(0);
 * exit;
 * ------------------------periodically trigger case--------------------
 * ./bcm.user
 * cd ../../../../src/examples/dnx/instru
 * c cint_instru_eventor.c
 * c cint_instru_dying_gasp.c
 * cint
 * cint_dying_gasp_pkt_gen_config_example(0);
 * cint_packet_period_generate(0);
 * exit;
 */
struct event_trigger_s{
     int  int_host_event_id;  /*[7:0]*/
     int  host_event_req;   /*[8]*/
     int  host_event_done;   /*[9]*/
     };

/*
 *  set Eventor builder for tx eventor testing:
 *
 */
int
cint_instru_eventor_tx_builder_dying_gasp_set(
        int unit,
        int builder_id,
        int thresh_size,
        int thresh_time)
{
    bcm_port_interface_info_t int_inf;
    bcm_port_mapping_info_t mapping_info;
    bcm_instru_eventor_builder_conf_t builder_conf={0};
    int rv;
    unsigned int flags=0;

    uint8 header_data[64] = {
            0xd0, 0x18, dying_gasp_dst_mac[0], dying_gasp_dst_mac[1], dying_gasp_dst_mac[2], dying_gasp_dst_mac[3], dying_gasp_dst_mac[4], dying_gasp_dst_mac[5], 0x00, 0x00,
            0x00, 0x00, 0x00, 0x11, 0x88, 0x09, 0x03, 0x00, 0x02, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00};

    printf("cint_instru_eventor_tx_builder_set: builder_id = %d\n", builder_id);

    /* Get Eventor PP port */
    uint8 cpu_port=201; /* to do use parameter */
    rv = bcm_port_get(unit, cpu_port, &flags, &int_inf, &mapping_info);
    if (rv != BCM_E_NONE)
    {
       printf("Error, in bcm_port_get\n");
       return rv;
    }
    flags = 0;

    /* in PTCH2, In_PP_Port is 10 bits [9:0]*/
    uint8 eventor_pp_port_high = ((mapping_info.pp_port) >> 8) & 0x3;
    uint8 eventor_pp_port_low = (mapping_info.pp_port) & 0xFF;
    /* PTCH2 : In_PP_Port [9:0], Parser_Program_Control [15] - '1' - Indicates that no next header */
    header_data[0] = (eventor_pp_port_high | 0xd0);
    header_data[1] = eventor_pp_port_low;

    builder_conf.flags = 0;
    builder_conf.thresh_size = thresh_size;
    builder_conf.thresh_time = thresh_time;
    builder_conf.header_length = 20;
    builder_conf.header_data = header_data;
    /*
     * Set eventor Tx builder
     */
    rv = bcm_instru_eventor_builder_set(unit, flags, builder_id, &builder_conf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_builder_set\n");
        return rv;
    }

    BCM_IF_ERROR_RETURN(cint_instru_eventor_vlan_translation(unit, out_vlan_port_edit_prfoile, &vlan_port));
    BCM_IF_ERROR_RETURN(cint_multicast_to_all_ports_cfg(unit, ing_mc_id, vlan_port.encap_id));
    BCM_IF_ERROR_RETURN(cint_instru_eventor_packet_forward(unit, ing_mc_id));

    printf("cint_instru_eventor_tx_builder_set: PASS \n");
    return rv;
}

int cint_instru_eventor_vlan_translation(int unit, int vlan_edit_profile, bcm_vlan_port_t *vlan_port)
{
    int rv = 0;
    int action_id;
    int flags;
    bcm_vlan_action_set_t action;
    int tag_format_untag = 0;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;

    bcm_vlan_port_t_init(vlan_port);
    vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf ("bcm_vlan_port_create failed: %x \n", rv);
        return rv;
    }

    flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionNone;
    action.dt_outer_pkt_prio = bcmVlanActionNone;
    action.dt_inner = bcmVlanActionNone;
    action.dt_inner_pkt_prio = bcmVlanActionNone;
    flags = BCM_VLAN_ACTION_SET_EGRESS;

    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = out_vlan_port_edit_prfoile;
    action_class.tag_format_class_id = tag_format_untag;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }


    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = vlan_port->vlan_port_id;
    port_trans.vlan_edit_class_id = out_vlan_port_edit_prfoile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    return BCM_E_NONE;
}
int cint_multicast_to_all_ports_cfg(int unit, bcm_multicast_t mc_group_id, bcm_if_t encap)
{
    int rv = 0;
    int flags_mc = BCM_MULTICAST_INGRESS_GROUP;
    int port_index = 0;
    int nof_ports_config;
    bcm_port_config_t ports_config;
    bcm_port_t port_iter;
    bcm_port_t port_cpu = 200;
    uint32 phy_num;
    phy_num = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    bcm_multicast_replication_t replications[phy_num];

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &mc_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_create (ingress)\n");
        return rv;
    }

    /* get ports bitmap */
    rv = bcm_port_config_get(unit, &ports_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_config_get\n");
        return rv;
    }

    /* iterate on nif ports */
    BCM_PBMP_ITER(ports_config.nif, port_iter)
    {
        BCM_GPORT_LOCAL_SET(replications[port_index].port, port_iter);
        replications[port_index].encap1 = encap;
        printf("port %d id %d\n", port_index, port_iter);
        port_index++;
    }

        BCM_GPORT_LOCAL_SET(replications[port_index].port, port_cpu);
        replications[port_index].encap1 = encap;
        printf("port %d id %d\n", port_index, port_cpu);
        port_index++;
    printf("Total port counter: %d\n", port_index);

    nof_ports_config = port_index;
    rv = bcm_multicast_set(unit, mc_group_id, flags_mc, nof_ports_config, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_set\n");
        return rv;
    }

    return BCM_E_NONE;
}
int cint_instru_eventor_packet_forward(int unit, bcm_multicast_t mc_group_id)
{
    int rv;
    bcm_l2_addr_t l2_addr;
    uint8 dst_mac_addr[6];

    dst_mac_addr[0] = dying_gasp_dst_mac[0];
    dst_mac_addr[1] = dying_gasp_dst_mac[1];
    dst_mac_addr[2] = dying_gasp_dst_mac[2];
    dst_mac_addr[3] = dying_gasp_dst_mac[3];
    dst_mac_addr[4] = dying_gasp_dst_mac[4];
    dst_mac_addr[5] = dying_gasp_dst_mac[5];
    bcm_l2_addr_t_init(&l2_addr, dst_mac_addr, 1);
    l2_addr.flags |= BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_group_id;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_l2_addr_add\n");
        return rv;
    }
    return BCM_E_NONE;
}
int cint_instru_eventor_event_cfg(int unit, bcm_eventor_context_id_t context_id, bcm_eventor_event_id_t event_id)
{
    int rv = 0;
    bcm_instru_eventor_event_id_config_t event_config;
    event_config.flags = 0;
    event_config.context = context_id;
    rv = bcm_instru_eventor_event_id_config_set(unit, 0, event_id,  &event_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_event_id_config_set\n");
        return rv;
    }
    printf("cint_instru_eventor_event_cfg: PASS \n");

    return BCM_E_NONE;
}

int
cint_instru_eventor_tx_init_sram_descriptor_data(int unit,uint32 nof_operations)
{
    int i = 0;
    for(i = 0; i < nof_operations; i++)
    {
        bshell(unit,"w MTM_MCDB\[0\] $i 1 $i $i $i $i $i $i $i $i $i");
    }
    return BCM_E_NONE;
}


int
cint_instru_eventor_activate(int unit)
{
    int rv;
    uint32 flags = 0;

    /*
    * Activate eventor
    */
    rv = bcm_instru_eventor_active_set(unit, flags, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_active_set\n");
        return rv;
    }

    printf("cint_instru_eventor_activate: PASS \n");

    return BCM_E_NONE;
}

int ing_mc_id = 5000;
int out_vlan_port_edit_prfoile = 3;
bcm_vlan_port_t vlan_port;
uint8 dying_gasp_dst_mac[6] = {0x1,0x80,0xc2,0x00,0x00,0x2};


int cint_dying_gasp_pkt_gen_config_example(int unit)
{
    uint32 nof_operations = 5;
    int tx_context_id = 10;
    int builder_id = 0;
    int bank = 2;
    int buffer_size = 80;
    int buffer_offset = 0;
    int program_offset = 0x50;
    int trigger_event_id = 10;
    int offset = 1;
    int builder_thresh = 80;
    int builders = 1 << builder_id;

    cint_instru_eventor_tx_init_sram_descriptor_data(unit,nof_operations);
    /*Configure eventor context id*/
    BCM_IF_ERROR_RETURN(cint_instru_eventor_tx_context_set(unit,tx_context_id,bank,bank+1,buffer_size,buffer_offset,buffer_offset,program_offset,program_offset,builders,offset,0,nof_operations));
    /*Configure eventor build id to generate dying gasp packet*/
    BCM_IF_ERROR_RETURN(cint_instru_eventor_tx_builder_dying_gasp_set(unit, builder_id, builder_thresh, -1));
    /*Configure eventor event*/
    BCM_IF_ERROR_RETURN(cint_instru_eventor_event_cfg(unit, tx_context_id, trigger_event_id));
    /*Activate eventor block*/
    BCM_IF_ERROR_RETURN(cint_instru_eventor_activate(unit));
    return BCM_E_NONE;
}

int cint_instru_eventor_tx_manual_dying_gasp_test(int unit, int tx_context_id,int event_id,int nof_events_to_send,int nof_events_in_packet,int nof_operations)
{
    int i,flags,done_count,rv = 0;
    char new_cmdbuf[300];
    uint32 done_indication = 0;
    reg_val payload_val; /*array of 20 integers*/
    event_trigger_s payload;

    bshell(unit, "g IRE_EVENTOR_PACKET_COUNTER.IRE0");

    sprintf(new_cmdbuf, "g chg EVNT_TX_ENG_EVENTS_CNT(%d)", event_id);
    bshell(unit, new_cmdbuf);
    
    printf("send single events: event_id=%d, nof_events_to_send=%d, nof_events_in_packet=%d\n",event_id,nof_events_to_send,nof_events_in_packet);

    for(i = 0; i < nof_events_to_send; i++)
    {
        printf("send single events: event_id=%d, event number=%d\n",event_id,i);
        rv = bcm_instru_eventor_event_generate(unit,0,event_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_instru_eventor_context_set\n");
            return rv;
        }

    }
    return BCM_E_NONE;
}

int cint_packet_manual_generate(int unit)
{
    uint32 nof_operations = 5;
    int tx_context_id = 10;
    int trigger_event_id = 10;
    int builder_thresh = 80;
    int nof_events_to_send = 1;
    int builder_id = 0;
    int builders = 1 << builder_id;
    int max_aggregated_events = builder_thresh/context_conf_set.out_event_size;
    int nof_events_in_packet = max_aggregated_events;
    int i=0;
    BCM_IF_ERROR_RETURN(cint_instru_eventor_tx_manual_dying_gasp_test(unit,tx_context_id,trigger_event_id,nof_events_to_send,nof_events_in_packet,nof_operations));
    for (i = 0; i < 5; i++)
    {
        nof_events_to_send = max_aggregated_events-1;
        BCM_IF_ERROR_RETURN(cint_instru_eventor_tx_manual_dying_gasp_test(unit,tx_context_id,trigger_event_id,nof_events_to_send,nof_events_in_packet,nof_operations));
        nof_events_to_send = 1;
        BCM_IF_ERROR_RETURN(cint_instru_eventor_tx_manual_dying_gasp_test(unit,tx_context_id,trigger_event_id,nof_events_to_send,nof_events_in_packet,nof_operations));
    }

    return BCM_E_NONE;
}

int cint_instru_eventor_tx_periodic_dying_gasp_events_test(int unit, int tx_context_id,int event_id,int trigger_source,int source_index,uint64 trigger_period,int nof_events_to_send)
{
    int i,flags,done_count = 0;
    char new_cmdbuf[300];
    uint32 done_indication = 0;
    reg_val payload_val; /*array of 20 integers*/
    event_trigger_s payload;
    int nof_sources = 1;
    int enable = 0;
    bcm_instru_synced_trigger_config_t config;
    bcm_instru_eventor_periodic_source_t source;
    bcm_instru_eventor_periodic_config_t eventor_config;

    BCM_IF_ERROR_RETURN(cint_instru_eventor_periodic_trigger_enable_set(unit,flags,nof_sources,trigger_source,enable));
    config.nof_triggers = nof_events_to_send;

    COMPILER_64_SET(config.start_time,0,0);
    config.trigger_period = trigger_period;
    BCM_IF_ERROR_RETURN(bcm_instru_synced_trigger_config_set(unit,flags,trigger_source,&config));

    source.source = trigger_source;
    source.source_index = source_index;

    eventor_config.enable = 1;
    eventor_config.event_id = event_id;
    BCM_IF_ERROR_RETURN(bcm_instru_eventor_generate_periodic_set(unit,flags,&source,&eventor_config));

    enable = 1;
    BCM_IF_ERROR_RETURN(cint_instru_eventor_periodic_trigger_enable_set(unit,flags,nof_sources,trigger_source,enable));

    return BCM_E_NONE;
}

int cint_packet_period_generate(int unit)
{
    uint32 nof_operations = 5;
    int tx_context_id = 10;
    int trigger_event_id = 10;
    int builder_thresh = 80;
    int nof_events_to_send = 1;
    int builder_id = 0;
    int builders = 1 << builder_id;
    int max_aggregated_events = builder_thresh/context_conf_set.out_event_size;
    int nof_events_in_packet = max_aggregated_events;

    int trigger_source = bcmInstruSyncedTriggerTypeNif;
    uint64 trigger_period;
    int source_index = 0;
    int nof_rcv_packets = 0;
    int i=0;
    /*set 1000000 ns as event period; means 1 ms*/
    COMPILER_64_SET(trigger_period,0,1000000);
    for (i = 0; i < 2; i++)
    {
        nof_events_to_send = max_aggregated_events*20;
        BCM_IF_ERROR_RETURN(cint_instru_eventor_tx_periodic_dying_gasp_events_test(unit,tx_context_id,trigger_event_id,trigger_source,source_index,trigger_period,nof_events_to_send));
        sal_sleep(5);
    }

    return BCM_E_NONE;
}


