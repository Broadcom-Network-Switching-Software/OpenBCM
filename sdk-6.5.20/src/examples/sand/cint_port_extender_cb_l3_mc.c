
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * Purpose: The CINT demonstrates a Controlling bridge configuration as part of a Port Extender (802.BR) application.
 *
 * The example below handles L3 MC traffic.
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
 *                         |  |  VSI1   |   . . .     | VSI4 |    |
 *                         |   ---------               ------     |
 *                         |    |                        |        |
 *                         |    | RIF1      . . .    RIF4|        |
 *                         |____|________________________|________|
 *                              |                        |
 *                              |                        |
 *                             host1                    host4
 *                          <ecid 100,vid 10>         <ecid 103,vid 13>
 *
 *
 * SOC properties:
 * 1. Configure the device to be a Control Bridge: extender_control_bridge_enable. 1: Enable, 0: Disable.
 * 2. Set the number of bytes to prepend to the start of the packet: prepend_tag_bytes = 8B
 * 3. Set the offset in bytes from the DA/SA in the packet from which to prepend the tag: prepend_tag_offset = 0
 *
 * CINT files:
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint cint_utils_port.c
 * cint cint_advanced_vlan_translation_mode.c
 * cint cint_utils_vlan.c
 * cint cint_utils_extender.c
 * cint cint_port_extender_cb_l3_mc.c
 * cint
 *
 * port_extender_cb_l3_mc__start_run(int unit, port_extender_cb_l3_mc_s *param) - param = NULL for default params
 *
 * All default values could be re-written with initialization of the global structure 'g_port_extender_cb_l3_mc', before calling the main function.
 * In order to re-write only part of the values, call 'port_extender_cb_uc_struct_get(port_extender_cb_l3_mc_s)' function and re-write the values
 * prior to calling the main function.
 */

/*****************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int NOF_EXTENDER_PORTS = 4;

/* Per Port Struct */
struct port_extender_cb_l3_mc__port_s {
    bcm_gport_t     port;
    bcm_gport_t     gport;
    bcm_vlan_t      vid;
    bcm_vlan_t      ecid;
    bcm_mac_t       mac;
    uint8           is_cascaded;          /* Mark only once per phy port */
};


/*  Main Struct  */
struct port_extender_cb_l3_mc_s {
    port_extender_cb_l3_mc__port_s  ports[NOF_EXTENDER_PORTS];
    uint32                          extender_tpid;
    int                             ipmc_index;
};

/* Initialization of global struct*/
port_extender_cb_l3_mc_s g_port_extender_cb_l3_mc = { /* Ports configuration
                                                 port      gport   vid/vsi   ecid       MAC      is_cascaded       */
                                                {{ 0,       -1,       10,   100,  {0,0,0,0,0,0x10},   1},     /* VM1 */
                                                 { 0,       -1,       11,   101,  {0,0,0,0,0,0x11},   0},     /* VM2 */
                                                 { 200,     -1,       12,   102,  {0,0,0,0,0,0x12},   1},     /* VM3 */
                                                 { 200,     -1,       13,   103,  {0,0,0,0,0,0x13},   0}},    /* VM4 */
                                                /* Additional parameters */
                                                0x893F, /* Extender TPID */
                                                5000};  /* MC group id */

/*****************************************************************************************************
  --------------          Global Variables Definition and Initialization  END      -----------------
 */
/* Initialization of main struct
 * This function allow to re-write default values or to leave default values without changes
 *
 * INPUT:
 *   params: new values for g_port_extender_cb_l3_mc
 */
int port_extender_cb_l3_mc_init(int unit, port_extender_cb_l3_mc_s *param){

    if (param != NULL) {
       sal_memcpy(&g_port_extender_cb_l3_mc, param, sizeof(g_port_extender_cb_l3_mc));
    }
    if (is_device_or_above(unit, JERICHO2)) {
        advanced_vlan_translation_mode = 1;
    } else {
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    }
    return extender__init(unit, g_port_extender_cb_l3_mc.extender_tpid);
}

/*
 * Return g_port_extender_cb_l3_mc default value
 */
void port_extender_cb_l3_mc_struct_get(int unit, port_extender_cb_l3_mc_s *param){

    sal_memcpy(param, &g_port_extender_cb_l3_mc, sizeof(g_port_extender_cb_l3_mc));
}

/* This function runs the main test function with given ports
 *
 * INPUT: unit     - unit
 *        port1  - port for VM1 and VM2
 *        port2  - port for VM3 and VM4
 */
int port_extender_cb_l3_mc_with_ports__start_run(int unit,  int port1, int port2)
{
    port_extender_cb_l3_mc_s param;

    port_extender_cb_l3_mc_struct_get(unit, &param);

    param.ports[0].port = port1;
    param.ports[1].port = port1;
    param.ports[2].port = port2;
    param.ports[3].port = port2;

    return port_extender_cb_l3_mc__start_run(unit, &param);
}


/* Main Function */
int port_extender_cb_l3_mc__start_run(int unit, port_extender_cb_l3_mc_s *param){

    int rv, port_id, i;
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_multicast_replication_t reps[NOF_EXTENDER_PORTS];
    create_l3_intf_s intf;

    bcm_multicast_t multicast_group_ing, multicast_group_egr;
    int num_cores, tm_port;
    bcm_gport_t temp[2], modport;
    bcm_gport_t dest_erp[2];
    int modid;
    int count_erp;
    int nof_erp_ports, nof_cores_for_traffic;

    rv = port_extender_cb_l3_mc_init(unit, param);
    if (rv != BCM_E_NONE){
        printf("Error, port_extender_cb_l3_mc_init unit %d, rv %d\n", unit, rv);
        return rv;
    }

    multicast_group_ing = multicast_group_egr = g_port_extender_cb_l3_mc.ipmc_index;
    /* create mc group */
    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &g_port_extender_cb_l3_mc.ipmc_index);
    } else {
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &multicast_group_ing);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("multicast group created: %x \n", g_port_extender_cb_l3_mc.ipmc_index);
    
    /*
     * Call ERP system ports to set ingress group replications with the FAP destination ERP port
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        num_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        printf(">>>num_cores: %d\n", num_cores);
        rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 1 * num_cores, temp, &count_erp);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        
        if (count_erp == 0)
        {
            printf("ERP is not enabled, Multicast creation failed \n");
            bcm_multicast_destroy(unit, multicast_group_ing);
            return BCM_E_PARAM;
        }
        printf(">>>count_erp: %d\n", count_erp);
        for (i = 0; i < count_erp; i++ ) {
            rv = bcm_stk_gport_sysport_get(unit, temp[i], &dest_erp[i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_stk_gport_sysport_get\n");
                return rv;
            }
        }

        /**ingress MC*/
        for (i = 0; i < count_erp; i++ ) {
            bcm_multicast_replication_t_init(&reps[i]);
            
            reps[i].port = dest_erp[i];
            reps[i].encap1 = g_port_extender_cb_l3_mc.ipmc_index;
        }
        nof_cores_for_traffic = *dnxc_data_get(unit, "device", "general", "nof_cores_for_traffic", NULL);
        if (nof_cores_for_traffic == 1)
        {
          nof_erp_ports = 1;
        }
        else
        {
          nof_erp_ports = count_erp;
        }

        /*
         * add only one ERP on ingress for adapter.
         */
        rv = bcm_multicast_add(unit, multicast_group_ing, BCM_MULTICAST_INGRESS_GROUP, nof_erp_ports, reps);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, in bcm_multicast_add for ingress\n");
            return rv;
        }
    }

    for (port_id = 0; port_id < NOF_EXTENDER_PORTS; port_id++)
    {
        if (is_device_or_above(unit, JERICHO2)) {
            rv = bcm_vlan_gport_add(unit, g_port_extender_cb_l3_mc.ports[port_id].vid, g_port_extender_cb_l3_mc.ports[port_id].port, 0);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_gport_add for vlan #%vsi\n", g_port_extender_cb_l2_mc.ports[port_id].vid);
                return rv;
            }
        }

        /* Define a VSI */
        rv = bcm_vswitch_create_with_id(unit, g_port_extender_cb_l3_mc.ports[port_id].vid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_create failed for vsi - %d, rv - %d\n", g_port_extender_cb_l3_mc.ports[port_id].vid, rv);
            return rv;
        }

        /* Configure Port-Extender unicast LIFs and Register E-Channel */
        extender__port_extender_port_s extender_port;
        extender_port.port = g_port_extender_cb_l3_mc.ports[port_id].port;
        extender_port.tag_vid = g_port_extender_cb_l3_mc.ports[port_id].vid;
        extender_port.extender_vid = g_port_extender_cb_l3_mc.ports[port_id].ecid;
        rv = extender__extender_port_configure(unit, &extender_port);
        if (rv != BCM_E_NONE) {
            printf("Error in extender__port_extender_set for  rv - %d\n", rv);
            return rv;
        }

        /* Attach the Extender-Port to a specific VSI */
        rv = bcm_vswitch_port_add(unit, g_port_extender_cb_l3_mc.ports[port_id].vid, extender_port.extender_gport_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_port_extender_cb_l3_mc.ports[port_id].vid, extender_port.extender_gport_id, rv);
            return rv;
        }
        g_port_extender_cb_l3_mc.ports[port_id].gport = extender_port.extender_gport_id;
        printf("Allocated extender_port_id - %x\n", extender_port.extender_gport_id);

        bcm_multicast_replication_t_init(reps[port_id]);
        reps[port_id].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
        reps[port_id].port = g_port_extender_cb_l3_mc.ports[port_id].port;

        /* Create L3 Interface */
        intf.my_global_mac = g_port_extender_cb_l3_mc.ports[port_id].mac;
        intf.my_lsb_mac =  g_port_extender_cb_l3_mc.ports[port_id].mac;
        intf.vsi = g_port_extender_cb_l3_mc.ports[port_id].vid;
        rv = l3__intf_rif__create(unit,&intf);
        if (rv != BCM_E_NONE) {
              printf("Fail  l3__intf_rif__create");
              return rv;
        }

        if (!is_device_or_above(unit, JERICHO2)) {
            /* cud outRif */
            rv = bcm_multicast_l3_encap_get(unit, g_port_extender_cb_l3_mc.ipmc_index,g_port_extender_cb_l3_mc.ports[port_id].port, g_port_extender_cb_l3_mc.ports[port_id].vid, &(reps[port_id].encap1));
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_l3_encap_get \n");
                return rv;
            }
            /* cud outLif */
            rv = bcm_multicast_extender_encap_get(unit, g_port_extender_cb_l3_mc.ipmc_index, g_port_extender_cb_l3_mc.ports[port_id].port, extender_port.extender_gport_id, &(reps[port_id].encap2));
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", g_port_extender_cb_l3_mc.ipmc_index, g_port_extender_cb_l3_mc.ports[port_id].port, extender_port.extender_gport_id);
                return rv;
            }
        } else {
            bcm_extender_port_t extender_port_tmp;
            bcm_gport_t dest_gport;
            bcm_if_t cuds[2];
            bcm_if_t rep_index;
            
            /**egress MC*/
            bcm_multicast_replication_t_init(&reps[port_id]);

            bcm_extender_port_t_init(&extender_port_tmp);
            extender_port_tmp.extender_port_id = extender_port.extender_gport_id;
            rv = bcm_extender_port_get(unit, &extender_port_tmp);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_dnx_extender_port_get.\n");
                return rv;
            }

            cuds[0] = intf.rif; /* OutRif */
            cuds[1] = extender_port_tmp.encap_id;/* OutLif */
            rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_multicast_encap_extension_create.\n");
            } 
            reps[port_id].encap1 = rep_index;

            BCM_GPORT_LOCAL_SET(dest_gport, g_port_extender_cb_l3_mc.ports[port_id].port);
            reps[port_id].port = dest_gport;
           
            rv = bcm_multicast_add(unit, multicast_group_egr, BCM_MULTICAST_EGRESS_GROUP, 1, &reps[port_id]);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
            {
                printf("Error, in bcm_multicast_add for egress\n");
                return rv;
            }          
        }

        if (g_port_extender_cb_l3_mc.ports[port_id].is_cascaded)
        {
            /* Configure cascaded ports */
            rv = extender__cascaded_port_configure(unit, g_port_extender_cb_l3_mc.ports[port_id].port);
            if (rv != BCM_E_NONE) {
                printf("Error, extender__cascaded_port_configure for port - %d, rv - %d\n", g_port_extender_cb_l3_mc.ports[port_id].port, rv);
                return rv;
            }
            
            if (!is_device_or_above(unit, JERICHO2)) {
                /* Configure the TPIDs for the port and their classification */
                rv = port__tpids__set(unit, g_port_extender_cb_l3_mc.ports[port_id].port, g_port_extender_cb_l3_mc.extender_tpid, 0x8100);
                if (rv != BCM_E_NONE) {
                    printf("Error, port__tpids__set for port - %d, rv - %d\n", port, rv);
                    return rv;
                }
            }
        }
        
        if (!is_device_or_above(unit, JERICHO2)) {
            /* EVE configuration for unicast outlifs */
            rv = port_extender_cb_l3_mc_eve_config(unit,
                                                port_id,
                                                g_port_extender_cb_l3_mc.ports[port_id].gport,
                                                g_port_extender_cb_l3_mc.ports[port_id].ecid);
            if (rv != BCM_E_NONE) {
                printf("Error in port_extender_cb_l3_mc_eve_config, port_id - %d, rv - %d\n", port_id, rv);
                return rv;
            }
        }
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_multicast_set(unit, g_port_extender_cb_l3_mc.ipmc_index,0, NOF_EXTENDER_PORTS, reps);
    }
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_set \n");
        return rv;
    }

    /** add routing table entry **/
    bcm_ipmc_addr_t ipmc_entry;
    bcm_ipmc_addr_t_init(&ipmc_entry);
    ipmc_entry.vrf = 0;
    ipmc_entry.group = g_port_extender_cb_l3_mc.ipmc_index;
    ipmc_entry.mc_ip_addr = mc_ip;
    ipmc_entry.mc_ip_mask = 0xffffffff;
    if (is_device_or_above(unit, JERICHO2)) {
        /*SIP empty with mask is illegal on JR2, set mask to 0x0.*/
        ipmc_entry.s_ip_addr = 0x0;
        ipmc_entry.s_ip_mask = 0x0;
    }


    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&ipmc_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    printf("mc group entry 0x%x added to routing table \n ", g_port_extender_cb_l3_mc.ipmc_index);
    return rv;
}

int port_extender_cb_l3_mc_eve_config(int unit, int port_id, int lif, int ecid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    int action_id = (5 + port_id);

    rv = vlan__port_translation__set(unit,
                                     ecid,
                                     g_port_extender_cb_l3_mc.ports[port_id].vid,
                                     lif,
                                     (10 + port_id) /* edit_profile */,
                                     0);
    if (rv != BCM_E_NONE) {
       printf("Fail  vlan__port_translation__set for LIF 0x%x ", lif);
       return rv;
    }


    rv = bcm_vlan_translate_action_id_create(unit,
                                             (BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID),
                                             &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionAdd;
    action.outer_tpid = g_port_extender_cb_l3_mc.extender_tpid;
    action.inner_tpid = 0x8100;
    action.dt_outer_pkt_prio = bcmVlanActionAdd;
    action.dt_inner_pkt_prio = bcmVlanActionAdd;
    rv = bcm_vlan_translate_action_id_set( unit,
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set, action = %d\n", action_id);
        return rv;
    }

    rv = vlan__translate_action_class__set(unit,
                                           action_id,
                                           (10 + port_id),
                                           0 /*tag_format*/,
                                           0);
    if (rv != BCM_E_NONE) {
        printf("Fail  vlan__egress_translate_action_class__set vlan edit profile %d  ", (10 + port_id));
        return rv;
    }

    return rv;
}

int port_extender_cb_l3_mc_cleanup(int unit)
{
    int rv = BCM_E_NONE, port_id;

    for (port_id = 0; port_id < NOF_EXTENDER_PORTS; port_id++)
    {
        rv = extender__extender_port_cleanup(unit, g_port_extender_cb_l3_mc.ports[port_id].gport);
        if (rv != BCM_E_NONE) {
            printf("\n\n Fail  extender__extender_port_cleanup rv = 0x%x\n\n",rv);
            return rv;
        }
    }
    /* Destroy MC group */
    rv = bcm_multicast_destroy(unit, g_port_extender_cb_l3_mc.ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Extender cleanup - pmf entries and groups destroy */
    if (!is_device_or_above(unit, JERICHO2)) { 
        rv = extender__cleanup(unit);
        if (rv != BCM_E_NONE) {
             printf("Fail  extender__cleanup ");
             return rv;
        }
    }

    return rv;
}

