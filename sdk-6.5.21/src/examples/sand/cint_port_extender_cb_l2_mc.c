/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * Purpose: The CINT demonstrates a Controlling bridge configuration as part of a Port Extender (802.BR) application.
 *
 * 1. The basic example handles L2 MC traffic.
 * 
 *                                                    _____________________
 *                 __________                        |                     |                           __________
 *      LIF 1   __|          |                       |         CB          |                          |          |__  LIF 3
 *              __|   Port   | <-------------------->|<------------------> |------------------------->|  Port    |__
 *              __|          |      Cascaded-Port 1  |                     |       Cascaded-Port 2    |          |__
 *      LIF 2   __| Extender |                       |                     |                          | Extender |__
 *                |__________|                       |                     |                          |__________|
 *                                                   |_____________________|
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
 * cint cint_utils_l2.c
 * cint cint_utils_vlan.c
 * cint cint_utils_extender.c
 * cint cint_port_extender_cb_l2_mc.c
 * cint
 *
 * port_extender_cb_l2_mc__start_run(int unit, port_extender_cb_l2_mc_s *param) - param = NULL for default params
 *
 * All default values could be re-written with initialization of the global structure 'g_port_extender_cb_l2_mc', before calling the main function.
 * In order to re-write only part of the values, call 'port_extender_cb_uc_struct_get(port_extender_cb_l2_mc_s)' function and re-write the values
 * prior to calling the main function.
 */

/*****************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int NOF_EXTENDER_PORTS = 3;


/* Per Port Struct */
struct port_extender_cb_l2_mc__port_s {
    bcm_gport_t     port;
    bcm_gport_t     gport;
    bcm_gport_t     mc_gport;
    bcm_vlan_t      vid;
    bcm_vlan_t      ecid;
    bcm_mac_t       mac;
    uint8           is_cascaded;          /* Mark only once per phy port */
};


/*  Main Struct  */
struct port_extender_cb_l2_mc_s {
    port_extender_cb_l2_mc__port_s     ports[NOF_EXTENDER_PORTS];
    bcm_vlan_t                      vsi;             /* also serves as MC group id */
    uint32                          extender_tpid;
    uint16                          mc_ecid;
    bcm_mac_t                       mc_mac;
};

/* Initialization of global struct*/
port_extender_cb_l2_mc_s g_port_extender_cb_l2_mc = { /* Ports configuration
                                                 port      gport   mc_gport  vid   ecid       MAC          is_cascaded       */
                                                {{ 0,       -1,       -1,     10,   100,  {0,0,0,0,0,0x10},   1},     /* VM1 */
                                                 { 0,       -1,       -1,     11,   101,  {0,0,0,0,0,0x11},   0},     /* VM2 */
                                                 { 200,     -1,       -1,     12,   102,  {0,0,0,0,0,0x12},   1}},    /* VM3 */
                                                /* Additional parameters */
                                                5,                      /* VSI */
                                                0x893F,             /* Extender TPID */
                                                0x111,              /* Multicast ECID */
                                                {0,0,0,0,0,0x55}};  /* Multicast MAC */   

/*****************************************************************************************************
  --------------          Global Variables Definition and Initialization  END      -----------------
 */
/* Initialization of main struct
 * This function allow to re-write default values or to leave default values without changes
 *
 * INPUT:
 *   params: new values for g_port_extender_cb_l2_mc
 */
int port_extender_cb_l2_mc_init(int unit, port_extender_cb_l2_mc_s *param){

    if (param != NULL) {
       sal_memcpy(&g_port_extender_cb_l2_mc, param, sizeof(g_port_extender_cb_l2_mc));
    }
    
    if (is_device_or_above(unit, JERICHO2)) {
        advanced_vlan_translation_mode = 1;
    } else {
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    }
    return extender__init(unit, g_port_extender_cb_l2_mc.extender_tpid);
}

/*
 * Return g_port_extender_cb_l2_mc default value
 */
void port_extender_cb_l2_mc_struct_get(int unit, port_extender_cb_l2_mc_s *param){

    sal_memcpy(param, &g_port_extender_cb_l2_mc, sizeof(g_port_extender_cb_l2_mc));
}

/* This function runs the main test function with given ports
 *
 * INPUT: unit     - unit
 *        port1  - port for VM1 and VM2
 *        port2  - port for VM3
 */
int port_extender_cb_l2_mc_with_ports__start_run(int unit,  int port1, int port2)
{
    port_extender_cb_l2_mc_s param;

    port_extender_cb_l2_mc_struct_get(unit, &param);

    param.ports[0].port = port1;
    param.ports[1].port = port1;
    param.ports[2].port = port2;

    return port_extender_cb_l2_mc__start_run(unit, &param);
}


/* Main Function */
int port_extender_cb_l2_mc__start_run(int unit, port_extender_cb_l2_mc_s *param){

    int rv;
    int port_id;
    int flags;
    int encap_id;
    bcm_multicast_t mc_group;

    rv = port_extender_cb_l2_mc_init(unit, param);
    if (rv != BCM_E_NONE){
        printf("Error, port_extender_cb_l2_mc_init unit %d, rv %d\n", unit, rv);
        return rv;
    }

    /* Define a VSI for the switch */
    rv = bcm_vswitch_create_with_id(unit, g_port_extender_cb_l2_mc.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create failed for vsi - %d, rv - %d\n", g_port_extender_cb_l2_mc.vsi, rv);
        return rv;
    }

    mc_group = g_port_extender_cb_l2_mc.vsi;

    /* MC Group */
    rv = bcm_multicast_create(unit, (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID| (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2)), &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    for (port_id = 0; port_id < NOF_EXTENDER_PORTS; port_id++) 
    {
        if (is_device_or_above(unit, JERICHO2)) {
            rv = bcm_vlan_gport_add(unit, g_port_extender_cb_l2_mc.ports[port_id].vid, g_port_extender_cb_l2_mc.ports[port_id].port, 0);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_gport_add for vlan #%vsi\n", g_port_extender_cb_l2_mc.ports[port_id].vid);
                return rv;
            }
        }
    
        if (g_port_extender_cb_l2_mc.ports[port_id].is_cascaded) 
        {
            /* Configure cascaded ports */
            rv = extender__cascaded_port_configure(unit, g_port_extender_cb_l2_mc.ports[port_id].port);
            if (rv != BCM_E_NONE) {
                printf("Error, extender__cascaded_port_configure for port - %d, rv - %d\n", g_port_extender_cb_l2_mc.ports[port_id].port, rv);
                return rv;
            }     

            if (!is_device_or_above(unit, JERICHO2)) {
                /* Configure the TPIDs for the port and their classification */
                rv = port__tpids__set(unit, g_port_extender_cb_l2_mc.ports[port_id].port, g_port_extender_cb_l2_mc.extender_tpid, 0x8100);
                if (rv != BCM_E_NONE) {
                    printf("Error, port__tpids__set for port - %d, rv - %d\n", port, rv);
                    return rv;
                }
                rv = port__tag_classification__set(unit, g_port_extender_cb_l2_mc.ports[port_id].port, 5 /* tag_format */, g_port_extender_cb_l2_mc.extender_tpid, 0xFFFFFFFF);
                if (rv != BCM_E_NONE) {
                    printf("Error, port__tag_classification__set for port - %d, rv - %d\n", port, rv);
                    return rv;
                }
            }

            /* Configure Port-Extender mcast OutLIF  (per cascaded port) */
            extender__port_extender_port_s extender_port_mc;
            extender_port_mc.extender_vid = g_port_extender_cb_l2_mc.mc_ecid;
            extender_port_mc.port = g_port_extender_cb_l2_mc.ports[port_id].port;
            rv = extender__extender_port_mc_configure(unit, &extender_port_mc);
            if (rv != BCM_E_NONE) {
                printf("Error in extender__extender_port_mc_configure, port - %d, rv - %d\n", g_port_extender_cb_l2_mc.ports[port_id].port, rv);
            }
            g_port_extender_cb_l2_mc.ports[port_id].mc_gport = extender_port_mc.extender_gport_id;
            printf("Allocated MC extender_port_id - %x\n", extender_port_mc.extender_gport_id);

            /* EVE configuration for multicast outlifs */
            rv = port_extender_cb_l2_mc_eve_config(unit, 
                                                port_id, 
                                                g_port_extender_cb_l2_mc.ports[port_id].mc_gport, 
                                                g_port_extender_cb_l2_mc.mc_ecid, 
                                                0); /* Not first call */
            if (rv != BCM_E_NONE) {
                printf("Error in port_extender_cb_l2_mc_eve_config, port_id - %d, rv - %d\n", port_id, rv);
            }
            
            if (!is_device_or_above(unit, JERICHO2)) {
                rv = bcm_multicast_extender_encap_get(unit, mc_group, g_port_extender_cb_l2_mc.ports[port_id].port, g_port_extender_cb_l2_mc.ports[port_id].mc_gport, &encap_id);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_multicast_extender_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group, g_port_extender_cb_l2_mc.ports[port_id].port, g_port_extender_cb_l2_mc.ports[port_id].mc_gport);
                    return rv;
                }
                rv = bcm_multicast_ingress_add(unit, mc_group, g_port_extender_cb_l2_mc.ports[port_id].port, encap_id);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_multicast_egress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group, g_port_extender_cb_l2_mc.ports[port_id].port, encap_id);
                    return rv;
                }
            } else {
                bcm_multicast_replication_t rep_array;

                bcm_multicast_replication_t_init(&rep_array);
                rep_array.port = g_port_extender_cb_l2_mc.ports[port_id].port;
                rep_array.encap1 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(extender_port_mc.extender_gport_id);
                rv = bcm_multicast_add(unit, mc_group, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in bcm_multicast_add in Port= %d,encap = 0x%x\n", rep_array.port, rep_array.encap1);
                    return rv;
                }
            }
        }

        /* Configure Port-Extender unicast LIFs and Register E-Channel */
        extender__port_extender_port_s extender_port;
        extender_port.port = g_port_extender_cb_l2_mc.ports[port_id].port;
        extender_port.tag_vid = g_port_extender_cb_l2_mc.ports[port_id].vid;
        extender_port.extender_vid = g_port_extender_cb_l2_mc.ports[port_id].ecid;
        rv = extender__extender_port_configure(unit, &extender_port);
        if (rv != BCM_E_NONE) {
            printf("Error in extender__port_extender_set for  rv - %d\n", rv);
            return rv;
        }

        /* Attach the Extender-Port to a specific VSI */
        rv = bcm_vswitch_port_add(unit, g_port_extender_cb_l2_mc.vsi, extender_port.extender_gport_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_port_extender_cb_l2_mc.vsi, extender_port.extender_gport_id, rv);
        }
        g_port_extender_cb_l2_mc.ports[port_id].gport = extender_port.extender_gport_id;
        printf("Allocated UC extender_port_id - %x\n", extender_port.extender_gport_id);

        /* EVE configuration for unicast outlifs */
        rv = port_extender_cb_l2_mc_eve_config(unit, 
                                            port_id, 
                                            g_port_extender_cb_l2_mc.ports[port_id].gport, 
                                            g_port_extender_cb_l2_mc.ports[port_id].ecid,
                                            1); /* first call - create action_ids */
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_l2_mc_eve_config, port_id - %d, rv - %d\n", port_id, rv);
        }

        /* Configure a UC mac address for the VM */
        l2__mact_properties_s vm_mact_entry = { g_port_extender_cb_l2_mc.ports[port_id].gport,
                                            g_port_extender_cb_l2_mc.ports[port_id].mac,
                                            g_port_extender_cb_l2_mc.vsi};
        rv = l2__mact_entry_create(unit, &vm_mact_entry);
        if (rv != BCM_E_NONE) {
            printf("Error in l2__mact_entry_create, rv - %d\n", rv);
            return rv;
        }
    }
    

    /* Add Multicast MAC entry */
    bcm_l2_addr_t mc_addr;
    bcm_l2_addr_t_init(&mc_addr, g_port_extender_cb_l2_mc.mc_mac, g_port_extender_cb_l2_mc.vsi);
    mc_addr.flags |= (BCM_L2_STATIC | BCM_L2_MCAST);
    mc_addr.l2mc_group = mc_group;
    rv = bcm_l2_addr_add(unit, mc_addr);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_addr_add, rv - %d\n", rv);
        return rv;
    }
    return rv;
}

int port_extender_cb_l2_mc_eve_config(int unit, int port_id, int lif, int ecid, uint8 is_first)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    int action_id = (5 + port_id);

    rv = vlan__port_translation__set(unit, 
                                     ecid,
                                     g_port_extender_cb_l2_mc.ports[port_id].vid,
                                     lif,
                                     (10 + port_id) /* edit_profile */,
                                     0);
    if (rv != BCM_E_NONE) {
       printf("Fail  vlan__port_translation__set for LIF 0x%x ", lif);
       return rv;
    }


    if (is_first) {
        rv = bcm_vlan_translate_action_id_create(unit, 
                                                 (BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID), 
                                                 &action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = g_port_extender_cb_l2_mc.extender_tpid;
        action.inner_tpid = 0x8100;
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
                                               5 /*tag_format*/,
                                               0);
        if (rv != BCM_E_NONE) {
            printf("Fail  vlan__egress_translate_action_class__set vlan edit profile %d  ", (10 + port_id));
            return rv;
        }
    }
    return rv;
}

