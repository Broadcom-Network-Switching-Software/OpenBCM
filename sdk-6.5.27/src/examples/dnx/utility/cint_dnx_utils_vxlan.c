/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Utility function for VXLAN in Jericho2 devices.
 */


/*
 * Define learn mode
 *   0. Learn FEC only, tunnel point to ARP
 *   1. Learn outlif + port, tunnel has to be symmetric
 *   2. Learn outlif + FEC, tunnel is symmetric, ARP is brought out by FEC
 *   3. Learn outlif + FEC, tunnel is asymmetric, ARP is brought out by FEC
 */
enum dnx_utils_vxlan_learn_mode_t {
    LEARN_FEC_ONLY = 0,
    LEARN_SYM_OUTLIF_PORT = 1,
    LEARN_SYM_OUTLIF_FEC = 2,
    LEARN_ASYM_OUTLIF_FEC = 3
};


/* Configure vxlan port */
struct dnx_utils_vxlan_port_add_s {
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



int dnx_utils_vxlan_multicast_create(int unit, int vpn) 
{
    char error_msg[200]={0,};
    int system_headers_mode;
    int flags; 

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = dnx_utils_is_jr2_system_header_mode(unit);
    if (egress_mc && (system_headers_mode == 0)) {
        flags = BCM_MULTICAST_EGRESS_GROUP;
    } else {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
    }
    flags |= BCM_MULTICAST_WITH_ID;

    snprintf(error_msg, sizeof(error_msg), "flags 0x%08X", flags);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &vpn), error_msg);

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, vpn, 0, nof_cores, mreps), "");
    }    
    return BCM_E_NONE; 
}


int dnx_utils_vxlan_open_vpn(int unit, int vpn, int port_class, int vni){

    BCM_IF_ERROR_RETURN_MSG(dnx_utils_vxlan_multicast_create(unit, vpn), "");

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    if(port_class == 0)
    {
        vpn_config.match_port_class = port_class;
        vpn_config.vnid = vni;
    }
    else
    {
        vpn_config.match_port_class = 0;
        vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit,&vpn_config), "");
    
    if(port_class != 0)
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
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn;
        config.vni              = vni;
        config.network_domain   = port_class;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    }

    return BCM_E_NONE;
}


int dnx_utils_vxlan_multicast_destroy(int unit, int vpn) 
{
    int flags;

    int system_headers_mode = dnx_utils_is_jr2_system_header_mode(unit);
    if (egress_mc && (system_headers_mode == 0)) {
        flags = BCM_MULTICAST_EGRESS_GROUP;
    } else {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
    }
       
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_group_destroy(unit, flags, vpn), "");
    return BCM_E_NONE; 
}

int dnx_utils_vxlan_close_vpn(int unit, int vpn, int port_class, int vni) 
{

    /* Remove the mapping */
    if (port_class != 0)
    {
        bcm_vxlan_network_domain_config_t config;
        bcm_vxlan_network_domain_config_t_init(&config);
        /*
         * Update network domain for the created VNI->VSI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn;
        config.vni              = vni;
        config.network_domain   = port_class;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_remove(unit, &config), "");

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn;
        config.vni              = vni;
        config.network_domain   = port_class;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_remove(unit, &config), "");
    }

    BCM_IF_ERROR_RETURN_MSG(dnx_utils_vxlan_multicast_destroy(unit, vpn), "");

    /* Destroy VPN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_destroy(unit, vpn), "");

    return BCM_E_NONE;
}

int dnx_utils_vxlan_port_add(int unit, dnx_utils_vxlan_port_add_s *vxlan_port_add) {
    char error_msg[200]={0,};
    bcm_vxlan_port_t vxlan_port;

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
    /* if fwd domain lookup fail, and trap is not supported, we use a default fwd domain */
    if (!*dnxc_data_get(unit, "trap", "ingress", "fwd_domain_mode_trap", NULL)) {
        vxlan_port.default_vpn = vxlan_port_add->default_vpn;
    }


    if (vxlan_port.flags | BCM_VXLAN_PORT_WITH_ID) {
        vxlan_port.vxlan_port_id = vxlan_port_add->vxlan_port_id;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_add(unit,vxlan_port_add->vpn,&vxlan_port), "");

    vxlan_port_add->vxlan_port_id = vxlan_port.vxlan_port_id;


    if (vxlan_port_add->add_to_mc_group)
    {
         uint8 is_fec_only = (vxlan_port_add->egress_if != 0) && (vxlan_port_add->out_tunnel == 0);

        /* Add vxlan port to the VPN flooding MC group */
        if (vxlan_port_add->is_ecmp) {
            /* The FEC that is passed to vxlan port is an ECMP.
               In this case add the tunnel initiator interface to the VSI flooding MC group */
            BCM_IF_ERROR_RETURN_MSG(multicast__egress_object_add(unit, vxlan_port_add->vpn, vxlan_port.match_port,
                                              vxlan_port_add->out_tunnel_if, 1 /* is egress */), "");
        }
        else if (is_fec_only) {
            int encap_id =  BCM_L3_ITF_VAL_GET(vxlan_port_add->out_tunnel_if); 
            snprintf(error_msg, sizeof(error_msg), "mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x",
                vxlan_port_add->vpn, vxlan_port.match_port, vxlan_port_add->out_tunnel_if);
            BCM_IF_ERROR_RETURN_MSG(multicast__add_multicast_entry(unit, vxlan_port_add->vpn, &(vxlan_port.match_port), &encap_id, 1, egress_mc /* is_egress */), error_msg);
        }
        else {
            BCM_IF_ERROR_RETURN_MSG(dnx_utils_multicast_vxlan_port_add(unit, vxlan_port_add->vpn, vxlan_port.match_port,
                                           vxlan_port.vxlan_port_id), "");
            printf("vxlan_port.vxlan_port_id: %x\n", vxlan_port.vxlan_port_id);
        }
    }

    return BCM_E_NONE;
}

/*
 * add gport of type vlan-port to the multicast
 */
int dnx_utils_multicast_vxlan_port_add(int unit, int mc_group_id, int sys_port, int gport){
    char error_msg[200]={0,};
    int rv = BCM_E_NONE;
    int encap_id;
    int flags;
    bcm_multicast_replication_t replications; 

    snprintf(error_msg, sizeof(error_msg), "mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x",
        mc_group_id, sys_port, gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_vxlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id), error_msg);
    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, egress_mc /* is_egress */);
    if (rv == BCM_E_EXISTS) {
        printf("The MC entry already exists \n");
        rv = BCM_E_NONE;
    }
    snprintf(error_msg, sizeof(error_msg), "mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x",
        mc_group_id, sys_port, encap_id);
    BCM_IF_ERROR_RETURN_MSG(rv, error_msg);
    return BCM_E_NONE;
}

void dnx_utils_vxlan_port_s_clear(dnx_utils_vxlan_port_add_s* vxlan_port_add) {

    sal_memset(vxlan_port_add, 0, sizeof(*vxlan_port_add));

    /* by default, egress orientatino is set by vxlan port */
    vxlan_port_add->set_egress_orientation_using_vxlan_port_add = 1;

    /** By default add the port to default flooding group */
    vxlan_port_add->add_to_mc_group = 1;

}

/** require config to have been initialized with flags */
void dnx_utils_vxlan_network_domain_config_add_fwd_domain_set(int fwd_domain, bcm_vxlan_network_domain_config_t * config)
{
    uint8 is_l2 = (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING);
    if (is_l2) {
        config->vsi = fwd_domain;
    }
    else {
        config->vrf = fwd_domain;
    }
}

int dnx_utils_vxlan_network_domain_config_add(int unit, int vni, int network_domain, int fwd_domain, int flags)
{
    bcm_vxlan_network_domain_config_t config;
    config.flags = flags;
    dnx_utils_vxlan_network_domain_config_add_fwd_domain_set(fwd_domain, &config);
    config.vni = vni;
    config.network_domain = network_domain;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    return BCM_E_NONE;
}

