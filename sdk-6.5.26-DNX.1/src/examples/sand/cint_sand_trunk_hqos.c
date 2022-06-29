/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TRUNK HQOS Example~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* File: cint_sand_trunk_hqos.c
* Purpose: Example of using trunk hqos (Virtual LAGs).
* Note - this cint assumes that trunk members are CPU ports (in header type is injected_to_pp)
*
* Brief:
* This cint demonstrates how to use virtual LAGs to implement HQOS scheme with Trunks. For simplicity this cint create 2
* virtual LAGs and 1 master LAG. Each LAG has a single system port as its member, these system-ports point at the end
* to the same physical destination (modport):
*
*                      +----------------------------------------+
*                      |                                        |
*                      |                                        |
*                      |             Physical Port              |
*                      |                                        |
*                      |                                        |
*                      +------+-------------------------+-------+
*                             |                         |
*                             |  Master Trunk           |
*                     +------->                         <------+
*                     |       +-------------------------+      |
*                     |                                        |
*                     |                                        |
*                     |                                        |
*        +------------+--------+                      +--------+-------------+
*        |                     |                      |                      |
*        |Virtual Trunk 1      |                      |Virtual Trunk 2       |
*        |                     |                      |                      |
*        +---------------------+                      +----------------------+
*
* Usage:
*
* To run TRUNK HQOS test on JR2:
* BCM.0> cd $SDK/src/examples
* BCM.0> cint sand/cint_sand_trunk_hqos.c
* cint
* bcm_trunk_t master_tid = 3;
* bcm_trunk_t virtual_tid1 = 2;
* bcm_trunk_t virtual_tid2 = 1;
* bcm_port_t dst_port = 202;
* int unit = 0;
* bcm_port_t src_port = 200;
* print cint_sand_trunk_hqos_init(unit, src_port, dst_port, master_tid, virtual_tid1, virtual_tid2);
* int trunk_idx = 1;
* print cint_sand_trunk_hqos_forward(unit, trunk_idx, 1)
*
* print cint_sand_trunk_hqos_clean(unit, dst_port)
*
*/


int master_trunk_id;
int virtual_trunks_id[3 - 1];
bcm_gport_t trunk_gport[3];
int connectors_list[3] = {5120, 5128, 5136};
int queues_list[3] = {5120, 5128, 5136};

int sys_port_list[3] = {100, 101, 102};

int create_trunks(int unit, bcm_port_t src_port)
{
    int rv, is_dnx;
    bcm_trunk_member_t trunk_port_members[2];
    bcm_trunk_info_t trunk_info;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    /** create master trunk */
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &master_trunk_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_create,unit $unit turnk_id: %x, $rv\n",master_trunk_id);
        return rv;
    }

    /** create virtual trunk 1 */
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID | BCM_TRUNK_FLAG_VP, &virtual_trunks_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_create,unit $unit turnk_id: %x, $rv\n",virtual_trunks_id[0]);
        return rv;
    }

    /** create vitural trunk 2 */
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID | BCM_TRUNK_FLAG_VP, &virtual_trunks_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_create,unit $unit turnk_id: %x, $rv\n",virtual_trunks_id[1]);
        return rv;
    }

    /** set trunk header type */
    if (is_dnx)
    {
        bcm_gport_t trunk_gport;
        bcm_switch_control_info_t value;
        bcm_switch_control_key_t key;


        BCM_GPORT_TRUNK_SET(trunk_gport, master_trunk_id);

        /** in header type - assuming CPU ports */
        value.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
        key.type = bcmSwitchPortHeaderType;
        key.index = 1;
        /** Set trunk header type to match members header types */
        rv = bcm_switch_control_indexed_port_set(unit, trunk_gport, key, value);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
            return rv;
        }

        /** out */
        value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        key.type = bcmSwitchPortHeaderType;
        key.index = 2;
        /** Set trunk header type to match members header types */
        rv = bcm_switch_control_indexed_port_set(unit, trunk_gport, key, value);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
            return rv;
        }
    }

    /** set psc type and members */
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;

    /**set master trunk */
    BCM_GPORT_SYSTEM_PORT_ID_SET(trunk_port_members[0].gport, sys_port_list[0]);
    BCM_GPORT_SYSTEM_PORT_ID_SET(trunk_port_members[1].gport, src_port);
    rv = bcm_trunk_set(unit, master_trunk_id, &trunk_info, 2, trunk_port_members);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_trunk_set,unit $unit trunk_id: %x, $rv\n",master_trunk_id);
        return rv;
    }

    /** set first virtual trunk */
    trunk_info.master_tid = master_trunk_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(trunk_port_members[0].gport, sys_port_list[1]);
    rv = bcm_trunk_set(unit, virtual_trunks_id[0], &trunk_info, 1, trunk_port_members);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_trunk_set,unit $unit trunk_id: %x, $rv\n",virtual_trunks_id[0]);
        return rv;
    }

    /** set second virtual trunk */
    trunk_info.master_tid = master_trunk_id;
    BCM_GPORT_SYSTEM_PORT_ID_SET(trunk_port_members[0].gport, sys_port_list[2]);
    rv = bcm_trunk_set(unit, virtual_trunks_id[1], &trunk_info, 1, trunk_port_members);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_trunk_set,unit $unit trunk_id: %x, $rv\n",virtual_trunks_id[1]);
        return rv;
    }

    return BCM_E_NONE;
}


/**
 * \brief - Create system ports for each trunk and setup a scheduling scheme (voq and connectors).
 * The system ports all point to the given 'port' for simplicity
 */
int create_system_ports(int unit, int port)
{
    int i, rv;
    int core, my_modid, sysport_id, voq_base_id, core_id;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_gport_t sysport_gport, connector_gport, voq_gport;
    bcm_cosq_voq_connector_gport_t config;
    int modid, num_cos, cos;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_connection_t connection;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_config;
    int delay_tolerance_level, rate_class;
    int modid_count, actual_modid_count;
    int is_dnx;

    /* get port info */
    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_get,unit $unit port: %x, $rv\n",gport);
        return rv;
    }

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv)) {
        printf("bcm_device_member_get failed");
        return(rv);
    }

    if (is_dnx) {
        rv = bcm_stk_modid_count(unit, &modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_count\n");
            return rv;
        }
        bcm_stk_modid_config_t modid_array[modid_count];

        rv = bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_config_get_all\n");
            return rv;
        }
        my_modid = modid_array[0].modid;
    }
    else {
        rv = bcm_stk_my_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_stk_my_modid_get,unit $unit $rv\n");
            return rv;
        }
    }

    core = mapping_info.core;
    modid = my_modid+core;

    num_cos = 8;

    /* create system ports */
    for (i=0; i<3; i++)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port_list[i]);
        /** since the mapping is local, we provide local port instead of modport */
        rv = bcm_stk_sysport_gport_set(unit, sysport_gport, port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_sysport_gport_set,unit $unit sysport_gport: %x port: %d, $rv\n",sysport_gport, port);
            return rv;
        }
    }


    /* create voq connectors */
    flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;

    for (i=0; i<3; i++)
    {
        config.flags = flags;
        config.nof_remote_cores = 2;
        config.numq = num_cos;
        config.remote_modid = modid;
        BCM_COSQ_GPORT_E2E_PORT_SET(config.port, port);
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connector_gport, connectors_list[i], core);

        rv = bcm_cosq_voq_connector_gport_add(unit, &config, connector_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_voq_connector_gport_add,unit $unit gport: %x, $rv\n",connector_gport);
            return rv;
        }

        /* attach connnectors to HR */
        for (cos = 0; cos < num_cos; cos++)
        {
            /* Each VOQ connector attach suitable HR */
            rv = bcm_cosq_gport_sched_set(unit, connector_gport, cos, BCM_COSQ_SP0,0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_gport_sched_set,unit $unit connector_gport: %x, $rv\n",connector_gport);
                return rv;
            }

            BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, port);
            gport_info.cosq = (mapping_info.num_priorities == 8 ? cos : 0);
            rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_gport_handle_get,unit $unit gport: %x, $rv\n",gport_info.in_gport);
                return rv;
            }

            /* attach HR SPi to connecter */
            rv = bcm_cosq_gport_attach(unit, gport_info.out_gport, connector_gport, cos);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_gport_attach,unit $unit gport: %x, $rv\n",gport_info.out_gport);
                return rv;
            }
        }
    }

    /* create voqs for system ports */
    for (i=0; i<3; i++)
    {
        sysport_id = sys_port_list[i];
        voq_base_id = queues_list[i];

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport,voq_base_id);
        BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port_list[i]);

        /** get rate class and delay_tolerance levevl */
        rv = appl_dnx_e2e_voq_profiles_get(unit, 10000 /*10G*/, &delay_tolerance_level, &rate_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, Ingress side appl_dnx_e2e_voq_profiles_get,unit $unit, $rv\n");
            return rv;
        }

        flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
        queue_bundle_config.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
        queue_bundle_config.local_core_id = BCM_CORE_ALL; /** Irrelevant in case of WITH_ID allocation */
        queue_bundle_config.numq = num_cos;
        queue_bundle_config.port = sysport_gport;
        for (cos = 0; cos < queue_bundle_config.numq; cos++)
        {
            queue_bundle_config.queue_atrributes[cos].delay_tolerance_level = delay_tolerance_level;
            queue_bundle_config.queue_atrributes[cos].rate_class = rate_class;
        }

        /** allocate voqs bundle */
        rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_config, &voq_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, Ingress side bcm_cosq_ingress_queue_bundle_gport_add,unit $unit, $rv\n");
            return rv;
        }
    }

    /* Ingress: connect voqs to voq connectors */
    for (i=0; i<3; i++)
    {
        core_id = BCM_CORE_ALL;

        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.remote_modid = modid;
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, core_id, queues_list[i]);
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connector_gport, connectors_list[i], core);
        connection.voq_connector = connector_gport;

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE)
        {
            printf("Error, Ingress side bcm_cosq_gport_connection_set,unit $unit, $rv\n");
            return rv;
        }
    }

    /* Egress: connect voq connectors to voqs */
    for (i=0; i<3; i++)
    {
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.remote_modid = my_modid;
        sysport_id = sys_port_list[i];
        core_id = BCM_CORE_ALL;

        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connector_gport, connectors_list[i], core);
        connection.voq_connector = connector_gport;

        voq_base_id = queues_list[i];
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, core_id, voq_base_id);
        connection.voq = voq_gport;

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE)
        {
            printf("Error, Egress side bcm_cosq_gport_connection_set,unit $unit, $rv\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}


/**
 * \brief - create trunk gports to be used for forwarding
 */
int create_trunk_gports(int unit)
{
     BCM_GPORT_TRUNK_SET(trunk_gport[0], master_trunk_id);
     BCM_GPORT_TRUNK_SET(trunk_gport[1], virtual_trunks_id[0]);
     BCM_GPORT_TRUNK_SET(trunk_gport[2], virtual_trunks_id[1]);

     return BCM_E_NONE;
}

/** set global variables with trunk IDs */
int create_trunk_ids(int unit, bcm_trunk_t master_tid, bcm_trunk_t virtual_tid_1, bcm_trunk_t virtual_tid_2)
{
    int rv, is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    /** dnx requires pool id */
    if (is_dnx)
    {
        BCM_TRUNK_ID_SET(master_trunk_id, 0, master_tid);
        BCM_TRUNK_ID_SET(virtual_trunks_id[0], 0, virtual_tid_1);
        BCM_TRUNK_ID_SET(virtual_trunks_id[1], 0, virtual_tid_2);
    }
    else
    {
        master_trunk_id = master_tid;
        virtual_trunks_id[0] = virtual_tid_1;
        virtual_trunks_id[1] = virtual_tid_2;
    }

     return BCM_E_NONE;
}

/**
 * brief - Main function:
 * 1. Create master and virtual trunks.
 * 2. Setup a scheduling scheme for these trunks
 */
int cint_sand_trunk_hqos_init(int unit, bcm_port_t src_port, bcm_port_t dst_port, bcm_trunk_t master_tid, bcm_trunk_t virtual_tid_1, bcm_trunk_t virtual_tid_2)
{
    int rv;

    /** create system ports pointing to dst_port */
    rv = create_system_ports(unit, dst_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_system_ports, $rv \n");
        return rv;
    }

    /** set trunk ids */
    rv = create_trunk_ids(unit, master_tid, virtual_tid_1, virtual_tid_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_trunk_ids, $rv \n");
        return rv;
    }

    /** create gports */
    rv = create_trunk_gports(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_trunk_gports, $rv \n");
        return rv;
    }

    /** create and set master and virtual trunks */
    rv = create_trunks(unit, src_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_trunks, $rv \n");
        return rv;
    }

     return BCM_E_NONE;
}

/** Forward traffic from source port to trunk in trunk_index */
int cint_sand_trunk_hqos_forward(int unit, int trunk_index, int enable)
{
    int rv;

    rv = bcm_port_force_forward_set(unit, trunk_gport[0], trunk_gport[trunk_index], enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_force_forward_set, $rv \n");
        return rv;
    }

    return BCM_E_NONE;
}


bcm_gport_t master_ac_gport;

/**set pp property for master trunk*/
int cint_sand_trunk_hqos_pp_property_set(int unit, int trunk_index, int vsi)
{
    int rv;
    bcm_port_tpid_class_t tpid_class;
    bcm_vlan_t vlan_id = vsi;
    int trunk_id;
    int trunk_group_id;
    bcm_vlan_port_t   vlan_port;

    /**tpid*/
    tpid_class.tag_format_class_id = 8;
    tpid_class.tpid1 = 0x8100;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = trunk_gport[trunk_index];
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, vlan_id, trunk_gport[trunk_index], BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    /**set vlan domain*/
    trunk_id = BCM_GPORT_TRUNK_GET(trunk_gport[trunk_index]);
    BCM_TRUNK_ID_GROUP_GET(trunk_group_id, trunk_id);
    rv = bcm_port_class_set(unit, trunk_gport[trunk_index], bcmPortClassId, 256 + trunk_group_id);

    /**create VSI and AC*/
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_dnx_vswitch_create_with_id\n");
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = trunk_gport[trunk_index];
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.match_vlan = vlan_id;
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create\n");
        return rv;
    }    

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vswitch_port_add\n");
        return rv;
    }

    master_ac_gport = vlan_port.vlan_port_id;
    return BCM_E_NONE;
}

/*
 * update AC lif: with virtual trunk.
 * the ac_lif only based on master trunk as virtual trunk without pp_port(vlan domain)
 */
int cint_sand_trunk_hqos_update_ac_lif(int unit, bcm_gport_t ac_lif, int virtual_trunk)
{
    bcm_trunk_t       trunk_id;
    bcm_gport_t       virt_trunk_gport;
    bcm_vlan_port_t   bcm_vlan_port_info;
    int               rv;

    BCM_TRUNK_ID_SET(trunk_id, 0, virtual_trunk);
    BCM_GPORT_TRUNK_SET(virt_trunk_gport,trunk_id);

    bcm_vlan_port_t_init(&bcm_vlan_port_info);
    bcm_vlan_port_info.vlan_port_id = ac_lif;

    rv = bcm_vlan_port_find(unit, &bcm_vlan_port_info);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_find\n");
        return rv;
    }

    /***/
    bcm_vlan_port_info.port = virt_trunk_gport;
    bcm_vlan_port_info.flags |= (BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID);
    rv = bcm_vlan_port_create(unit, &bcm_vlan_port_info);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create for replace\n");
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Create AC lif: with virtual trunk, only allow BCM_VLAN_PORT_MATCH_NONE
 */
int cint_sand_trunk_hqos_create_lif_on_virtual_trunk(int unit, int master_trunk, int virtual_trunk, bcm_vlan_t match_vlan, uint8 is_match_none)
{
    bcm_trunk_t       trunk_id;
    bcm_gport_t       virtual_trunk_gport, master_trunk_gport;
    bcm_vlan_port_t   vlan_port, vlan_port_get;
    bcm_port_match_info_t match_info;
    int               rv;

    BCM_TRUNK_ID_SET(trunk_id, 0, virtual_trunk);
    BCM_GPORT_TRUNK_SET(virtual_trunk_gport,trunk_id);
    BCM_TRUNK_ID_SET(trunk_id, 0, master_trunk);
    BCM_GPORT_TRUNK_SET(master_trunk_gport,trunk_id);

    bcm_vlan_port_t_init(&vlan_port);
    /**create AC based on virtual trunk without match*/
    vlan_port.port = virtual_trunk_gport;
    vlan_port.match_vlan = is_match_none ? 0 : match_vlan;
    vlan_port.criteria = is_match_none ? BCM_VLAN_PORT_MATCH_NONE : BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.flags = BCM_VLAN_PORT_INGRESS_WIDE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create with %s match_none\n", is_match_none?"just":"not");
        return rv;
    }

    if (is_match_none)
    {
        /**should delete match before destory AC*/
        bcm_port_match_info_t_init(&match_info);
        match_info.port = master_trunk_gport;
        match_info.match = BCM_PORT_MATCH_PORT_CVLAN;
        match_info.match_vlan = match_vlan;
        match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
        rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_match_add\n");
            return rv;
        }
    }
    
    bcm_vlan_port_t_init(&vlan_port_get);
    vlan_port_get.vlan_port_id=vlan_port.vlan_port_id;
    rv = bcm_vlan_port_find(unit, &vlan_port_get);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_find\n");
        return rv;
    }
    
    if (!is_match_none)
    {
        if (vlan_port_get.flags != BCM_VLAN_PORT_INGRESS_WIDE)
        {
            printf("AC find wrong flags\n");
            return BCM_E_INTERNAL;
        }
        if (vlan_port_get.match_vlan != match_vlan)
        {
            printf("AC find wrong match_vlan\n");
            return BCM_E_INTERNAL;
        }    
        if(vlan_port_get.port != virtual_trunk_gport)
        {
            printf("AC find wrong port\n");
            return BCM_E_INTERNAL;
        }
        if(vlan_port_get.criteria != BCM_VLAN_PORT_MATCH_PORT_CVLAN)
        {
            printf("AC find wrong criteria\n");
            return BCM_E_INTERNAL;
        }
    }

    /*
     * add ac to VSI, then learning enable:
     *      Create vsi if not exist, here no need to create.
     *      Add created ac to vsi
     */
    rv = bcm_vswitch_port_add(unit, match_vlan, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vswitch_port_add\n");
        return rv;
    }
    
    master_ac_gport = vlan_port.vlan_port_id;
    
    return BCM_E_NONE;
}


int cint_sand_trunk_e2e_scheme_destory(int unit, bcm_port_t port)
{
    int rv, i, core_id, sysport_id, voq_base_id, cos;
    bcm_cosq_gport_connection_t connection;
    bcm_gport_t sysport_gport, modport_gport, connector_gport, voq_gport;
    bcm_cosq_gport_info_t gport_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    int num_cos = 8;

    /* get port info */
    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_get,unit $unit port: %x, $rv\n",gport);
        return rv;
    }


    /* Ingress: disconnect voqs to voq connectors */
    for (i=0; i<3; i++)
    {
        core_id = BCM_CORE_ALL;

        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = 0;
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, core_id, queues_list[i]);
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connector_gport, connectors_list[i], mapping_info.core);
        connection.voq_connector = connector_gport;

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE)
        {
            printf("Error, Ingress side bcm_cosq_gport_connection_set,unit $unit, $rv\n");
            return rv;
        }
    }

    /* Egress: diconnect voq connectors to voqs */
    for (i=0; i<3; i++)
    {
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = 0;
        sysport_id = sys_port_list[i];
        core_id = BCM_CORE_ALL;

        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connector_gport, connectors_list[i], mapping_info.core);
        connection.voq_connector = connector_gport;

        voq_base_id = queues_list[i];
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, core_id, voq_base_id);
        connection.voq = voq_gport;

        rv = bcm_cosq_gport_connection_set(unit, &connection);
        if (rv != BCM_E_NONE)
        {
            printf("Error, Egress side bcm_cosq_gport_connection_set,unit $unit, $rv\n");
            return rv;
        }
    }

    /* destory voqs for system ports */
    for (i=0; i<3; i++)
    {
        voq_base_id = queues_list[i];

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport,voq_base_id);

        rv = bcm_cosq_gport_delete(unit, voq_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_gport_delete,unit $unit sysport_gport: %x, $rv\n",sysport_gport);
            return rv;
        }
    }

    /* destory voq connectors */
    for (i=0; i<3; i++)
    {
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connector_gport, connectors_list[i], mapping_info.core);

        /* Disconnect connnectors from HR */
        for (cos = 0; cos < num_cos; cos++)
        {
            BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, port);
            gport_info.cosq = (mapping_info.num_priorities == 8 ? cos : 0);
            rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_gport_handle_get,unit $unit gport: %x, $rv\n",gport_info.in_gport);
                return rv;
            }

            /* attach HR SPi to connecter */
            rv = bcm_cosq_gport_detach(unit, gport_info.out_gport, connector_gport, cos);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_gport_attach,unit $unit gport: %x, $rv\n",gport_info.out_gport);
                return rv;
            }
        }

        /** delete connector */
        rv = bcm_cosq_gport_delete(unit, connector_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_gport_delete,unit $unit sysport_gport: %x, $rv\n",sysport_gport);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/**
 * \brief - clean up configurations
 */
int cint_sand_trunk_hqos_clean(int unit, bcm_port_t dst_port)
{
    int rv;

    /** disable forwarding */
    rv = cint_sand_trunk_hqos_forward(unit, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sand_trunk_hqos_forward when disabling forwarding during cleanup, $rv \n");
        return rv;
    }

    /** destroy virtual trunk 1 */
    rv = bcm_trunk_destroy(unit, virtual_trunks_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_trunk_destroy for virtual trunk 1, $rv \n");
        return rv;
    }

    /** destroy virtual trunk 2 */
    rv = bcm_trunk_destroy(unit, virtual_trunks_id[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_trunk_destroy for virtual trunk 2, $rv \n");
        return rv;
    }

    /** destroy master trunk */
    rv = bcm_trunk_destroy(unit, master_trunk_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_trunk_destroy for master trunk, $rv \n");
        return rv;
    }

    /** destroy e2e scheme */
    rv = cint_sand_trunk_e2e_scheme_destory(unit, dst_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sand_trunk_e2e_scheme_destory, $rv \n");
        return rv;
    }

    return BCM_E_NONE;
}

