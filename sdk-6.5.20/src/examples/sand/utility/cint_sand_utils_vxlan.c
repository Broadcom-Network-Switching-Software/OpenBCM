/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Utility function for VXLAN in Jericho2 devices.
 */


/* Configure vxlan port */
struct vxlan_port_add_s {
    uint32      vpn;        /* vpn to which we add the vxlan port  */
    bcm_gport_t in_port;    /* network side port for learning info. */
    bcm_gport_t in_tunnel;  /* ip tunnel terminator gport */
    bcm_gport_t out_tunnel; /* ip tunnel initiator gport */
    bcm_if_t    egress_if;  /* FEC entry, contains ip tunnel encapsulation information */
    uint32      flags;      /* vxlan flags */
    int         set_egress_orientation_using_vxlan_port_add;
    int         network_group_id; /* forwarding group for vxlan port. Used for orientation filter */
    int         is_ecmp;        /* indicate that FEC that is passed to vxlan port is an ECMP */
    bcm_if_t    out_tunnel_if;  /* ip tunnel initiator itf */
    bcm_gport_t vxlan_port_id;  /* returned vxlan gport */
    int         add_to_mc_group; /* Can be used to disabel adding a port to the default flooding group */
    bcm_vpn_t   default_vpn; /* default vpn, in case vni->vsi miss */
};

int sand_utils_vxlan_open_vpn(int unit, int vpn, int port_class, int vni){
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    int flags;

    proc_name = "vxlan_open_vpn";
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 \n", proc_name);
        return rv;
    }

    if(!device_is_jericho2) {

        rv = multicast__open_mc_group(unit, &vpn, BCM_MULTICAST_TYPE_L2);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, multicast__open_mc_group\n", proc_name);
            return rv;
        }
    } else {
        int system_headers_mode;
        /** JR2: Create fabric MC for each core in case of egress Multicast. */
        system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
        flags = (egress_mc && (system_headers_mode == 0)?BCM_MULTICAST_EGRESS_GROUP:BCM_MULTICAST_INGRESS_GROUP) | BCM_MULTICAST_WITH_ID;
        rv = bcm_multicast_create(unit, flags, &vpn);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__open_mc_group\n");
            return rv;
        }

        /** JR2: Create fabric MC for each core in case of egress Multicast. */
        if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
        {
            int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
            bcm_module_t mreps[2];
            mreps[0] = 0;
            mreps[1] = 1; /* Ignore it in single core devices*/
            rv = bcm_fabric_multicast_set(unit, vpn, 0, nof_cores, mreps);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_fabric_multicast_set \n");
                return rv;
            }
        }
    }

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    if((!device_is_jericho2) || (device_is_jericho2 && (port_class == 0)))
    {
        vpn_config.match_port_class = port_class;
        vpn_config.vnid = vni;
    }
    else
    {
        vpn_config.match_port_class = 0;
        vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    }

    rv = bcm_vxlan_vpn_create(unit,&vpn_config);
    if(rv != BCM_E_NONE) {
        printf("%s(): error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    if((device_is_jericho2) && (port_class != 0))
    {
        bcm_vxlan_network_domain_config_t config;
        bcm_vxlan_network_domain_config_t_init(&config);
        /*
         * Update network domain for the created VNI->VSI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING ;
        config.vsi              = vpn;
        config.vni              = vni;
        config.network_domain   = port_class;
        rv = bcm_vxlan_network_domain_config_add(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vxlan_network_domain_config_add \n");
            return rv;
        }

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn;
        config.vni              = vni;
        config.network_domain   = port_class;
        rv = bcm_vxlan_network_domain_config_add(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vxlan_network_domain_config_add \n");
            return rv;
        }
    }

    return rv;
}

int vxlan__vxlan_port_add(int unit, vxlan_port_add_s *vxlan_port_add) {
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t vxlan_port;
    int device_is_jericho2;

    bcm_vxlan_port_t_init(&vxlan_port);

    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = vxlan_port_add->in_port;
    vxlan_port.match_tunnel_id = vxlan_port_add->in_tunnel;
    vxlan_port.flags = vxlan_port_add->flags;
    if(vxlan_port_add->egress_if != 0) {
        vxlan_port.egress_if = vxlan_port_add->egress_if;
        vxlan_port.flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
    }
    if (vxlan_port_add->out_tunnel != 0){
        vxlan_port.egress_tunnel_id = vxlan_port_add->out_tunnel;
    }

    vxlan_port.network_group_id = vxlan_port_add->network_group_id;
    vxlan_port.default_vpn = vxlan_port_add->default_vpn;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 \n");
        return rv;
    }

    if (!device_is_jericho2) {
        /*
         * Pre-JR2 flag indicating network orientation.
         * Used to configure ingress orientation and optionally egress orientation
         */
        vxlan_port.flags |= BCM_VXLAN_PORT_NETWORK;

        /* if no outlif is provided, can't configure egress orientation */
        if (vxlan_port_add->out_tunnel == 0){
            printf("can't configure outlif orientation, since tunnel initiator gport isn't provided \n");
        } else {
            /* set network orientation at the egress using bcm_port_class_set */
            if (!vxlan_port_add->set_egress_orientation_using_vxlan_port_add) {
                rv = bcm_port_class_set(unit, vxlan_port_add->out_tunnel, bcmPortClassForwardEgress, vxlan_port_add->network_group_id);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_class_set \n");
                    return rv;
                }
                printf("egress orientation using bcm_port_class_set \n");

                if (vxlan_port_add->egress_if != 0) {
                    vxlan_port.egress_tunnel_id = 0;
                }
            }
        }
    }

    rv = bcm_vxlan_port_add(unit,vxlan_port_add->vpn,&vxlan_port);
    if(rv != BCM_E_NONE) {
        printf("error bcm_vxlan_port_add \n");
        return rv;
    }

    vxlan_port_add->vxlan_port_id = vxlan_port.vxlan_port_id;

    if (vxlan_port_add->add_to_mc_group)
    {
        uint8 is_fec_only = (vxlan_port_add->egress_if != 0) && (vxlan_port_add->out_tunnel == 0);

        /* Add vxlan port to the VPN flooding MC group */
        if (vxlan_port_add->is_ecmp) {
            /* The FEC that is passed to vxlan port is an ECMP.
               In this case add the tunnel initiator interface to the VSI flooding MC group */
            rv = multicast__egress_object_add(unit, vxlan_port_add->vpn, vxlan_port.match_port,
                                              vxlan_port_add->out_tunnel_if, 1 /* is egress */);
            if (rv != BCM_E_NONE) {
                printf("Error, multicast__egress_object_add\n");
                return rv;
            }
        }
        else if (is_fec_only) {
            int encap_id =  BCM_L3_ITF_VAL_GET(vxlan_port_add->out_tunnel_if); 
            rv = multicast__add_multicast_entry(unit, vxlan_port_add->vpn, &vxlan_port.match_port, &encap_id, 1, 0 /* is_egress */);
            if (rv != BCM_E_NONE) {
                    printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", 
                           vxlan_port_add->vpn, vxlan_port.match_port, vxlan_port_add->out_tunnel_if);
                    return rv;
            }
        }
        else {
            rv = multicast__vxlan_port_add(unit, vxlan_port_add->vpn, vxlan_port.match_port,
                                           vxlan_port.vxlan_port_id);
            printf("vxlan_port.vxlan_port_id: %x\n", vxlan_port.vxlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, multicast_vxlan_port_add\n");
                return rv;
            }
        }
    }

    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int multicast__vxlan_port_add(int unit, int mc_group_id, int sys_port, int gport){    
    int rv = BCM_E_NONE;
    int encap_id;
    int flags;
    bcm_multicast_replication_t replications; 
    
    rv = bcm_multicast_vxlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vxlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, egress_mc /* is_egress */);
    if (rv == BCM_E_EXISTS) {
        printf("The MC entry already exists \n");
        rv = BCM_E_NONE;
    }
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }

    return rv;
}

int multicast__open_mc_group(int unit, int *mc_group_id, int extra_flags) {
    int rv = BCM_E_NONE;
    int flags;

    /* destroy before open, to ensure it not exist */
    rv = bcm_multicast_destroy(unit, *mc_group_id);

    printf("egress_mc: %d \n", egress_mc);

    flags = BCM_MULTICAST_WITH_ID | extra_flags;
    /* create ingress/egress MC */
    if (egress_mc) {
        flags |= BCM_MULTICAST_EGRESS_GROUP;
    } else {
        flags |= BCM_MULTICAST_INGRESS_GROUP;
    }

    rv = bcm_multicast_create(unit, flags, mc_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create, flags $flags mc_group_id $mc_group_id \n");
        return rv;
    }

    printf("Created mc_group 0x%x \n", *mc_group_id);

    return rv;
}

int vxlan_port_s_clear(vxlan_port_add_s* vxlan_port_add) {

    sal_memset(vxlan_port_add, 0, sizeof(*vxlan_port_add));

    /* by default, egress orientatino is set by vxlan port */
    vxlan_port_add->set_egress_orientation_using_vxlan_port_add = 1;

    /** By default add the port to default flooding group */
    vxlan_port_add->add_to_mc_group = 1;

    return BCM_E_NONE;
}


