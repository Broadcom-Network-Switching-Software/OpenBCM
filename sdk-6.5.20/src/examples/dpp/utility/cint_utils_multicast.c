/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

int egress_mc = 0;

/* Adding entries to MC group
 * ipmc_index:  mc group
 * ports: array of ports to add to the mc group
 * cud 
 * nof_mc_entries: number of entries to add to the mc group
 * is_ingress:  if true, add ingress mc entry, otherwise, add egress mc 
 * see add_ingress_multicast_forwarding from cint_ipmc_flows.c
 */
int multicast__add_multicast_entry(int unit, int ipmc_index, int *ports, int *cud, int nof_mc_entries, int is_egress) {
    int rv = BCM_E_NONE;
    int i;

    /* update Multicast to have the added port  */
    for (i=0;i<nof_mc_entries;i++)
    {
        if(!is_device_or_above(unit, JERICHO2))
        {
            /* egress MC */
            if (is_egress) {
                rv = bcm_multicast_egress_add(unit,ipmc_index,ports[i],cud[i]);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_multicast_egress_add: port %d encap_id: %d \n", ports[i], cud[i]);
                    return rv;
                }
            }
            /* ingress MC */
            else {
                rv = bcm_multicast_ingress_add(unit,ipmc_index,ports[i],cud[i]);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_multicast_ingress_add: port %d encap_id: %d \n", ports[i], cud[i]);
                    return rv;
                }
            }
        }
        else
        {
            bcm_multicast_replication_t rep_array;
            int flags;

            flags = BCM_MULTICAST_TYPE_L2 | ((is_egress) ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP);
            bcm_multicast_replication_t_init(&rep_array);

            rep_array.port = ports[i];
            rep_array.encap1 = cud[i];
            rv = bcm_multicast_add(unit, ipmc_index, flags, 1, &rep_array);
        }
    }

    return rv;
}

/*
 * add gport of type mpls-port to the multicast
 */
int multicast__mpls_port_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress) {
    int encap_id;
    int rv;
    
    rv = bcm_multicast_vpls_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vpls_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int multicast__vlan_port_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress) {
    int encap_id;
    int rv;

    /* update Multicast to have the added port  */
    if(!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
            return rv;
        }
    }
    else
    {
        encap_id = gport & 0xfffff;
    }
    

    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    return rv;
}





/*
 * add gport of type vlan-port to the multicast
 */
int multicast__egress_object_add(int unit, int mc_group_id, int sys_port, bcm_if_t intf, uint8 is_egress){

    int encap_id;
    int rv;
    
    rv = bcm_multicast_egress_object_encap_get(unit, mc_group_id, intf, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_egress_object_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  intf:  0x%08x \n", mc_group_id, sys_port, intf);
        return rv;
    }
    rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
    /* if the MC entry already exist, then we shouldn't add it twice to the multicast group */
    if (rv == BCM_E_EXISTS) {
        printf("The MC entry already exists \n");
        rv = BCM_E_NONE; 
    } else if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }
    
    return rv;
}




/*
 * add gport of type vlan-port to the multicast
 */
int multicast__vxlan_port_add(int unit, int mc_group_id, int sys_port, int gport){

    int encap_id;
    int rv = BCM_E_NONE;
    
    rv = bcm_multicast_vxlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vxlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
    /* if the MC entry already exist, then we shouldn't add it twice to the multicast group */
    if (rv == BCM_E_EXISTS) {
        printf("The MC entry already exists \n");
        rv = BCM_E_NONE; 
    } else if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
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

    printf("Created mc_group %d \n", *mc_group_id);

    return rv;
}

int multicast__open_ingress_mc_group_with_local_ports(int unit, int mc_group_id, int *dest_local_port_id, int *cud, int num_of_ports, int extra_flags) {
	int i;
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type = bcmCosqGportTypeLocalPort;

	egress_mc = 0;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    for(i=0;i<num_of_ports;i++) {
        BCM_GPORT_LOCAL_SET(gport_info.in_gport,dest_local_port_id[i]); 
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get, gport_type $gport_type \n");
            return rv;
        }

        rv = multicast__add_multicast_entry(unit, mc_group_id, &gport_info.out_gport, &cud[i], 1, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id dest_gport $gport_info.out_gport \n");
            return rv;
        }
    }
    
    return rv;
}

int multicast__open_ingress_mc_group_with_gports(int unit, int mc_group_id, int *gport, int *cud, int num_of_ports, int extra_flags) {
    bcm_error_t rv = BCM_E_NONE;

	egress_mc = 0;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, gport, cud, num_of_ports, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id \n");
        return rv;
    }
    
    return rv;
}
int multicast__open_egress_mc_group_with_local_ports(int unit, int mc_group_id, int *dest_local_port_id, int *cud, int num_of_ports, int extra_flags) {
    int i;
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type = bcmCosqGportTypeLocalPort;

    egress_mc = 1;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    for(i=0;i<num_of_ports;i++) {
        BCM_GPORT_LOCAL_SET(gport_info.in_gport,dest_local_port_id[i]); 
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get, gport_type $gport_type \n");
            return rv;
        }

        rv = multicast__add_multicast_entry(unit, mc_group_id, &gport_info.out_gport, &cud[i], 1, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id dest_gport $gport_info.out_gport \n");
            return rv;
        }
    }

    return rv;
}

int multicast__open_egress_mc_group_with_gports(int unit, int mc_group_id, int *gport, int *cud, int num_of_ports, int extra_flags) {
    bcm_error_t rv = BCM_E_NONE;

	egress_mc = 1;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, gport, cud, num_of_ports, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id \n");
        return rv;
    }
    
    return rv;
}


struct multicast_forwarding_entry_dip_sip_s {
    bcm_ip_t mc_ip;
    bcm_ip_t src_ip;
    bcm_ip6_t mc_ip6;
    bcm_ip6_t src_ip6;
    uint8 use_ipv6;
};

/* Creates a forwarding entry. src_ip == 0x0 implies L2 forwarding. The entry will be accessed by LEM<FID,DIP> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a (MC) destination ip.
 * src_ip > 0x0 implies L3 forwarding. The entry will be accessed by TCAM <RIF, SIP, DIP> Lookup.
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip.                                                                                                                                 .
 * src_ip == -1 implies creation of entry without sip.                                                                                                                                                                                                                            .
 */
int multicast__create_forwarding_entry_dip_sip(int unit, multicast_forwarding_entry_dip_sip_s *entry, int ipmc_index, int vlan, int vrf) {
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    bcm_ip6_t ip6_full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);


    data.flags = 0x0;
    data.group = ipmc_index;
/*  data.vrf = vrf;*/

    if (entry->use_ipv6) {
        data.flags |= BCM_IPMC_IP6;
        sal_memcpy(data.mc_ip6_addr, entry->mc_ip6, sizeof(entry->mc_ip6));
        sal_memcpy(data.mc_ip6_mask, ip6_full_mask, sizeof(entry->mc_ip6));
        
        if (entry->src_ip != -1 && entry->src_ip != 0) {
            data.vid = vlan;
            sal_memcpy(data.s_ip6_addr, entry->src_ip6, sizeof(data.s_ip6_addr));
            sal_memset(data.s_ip_mask, 0xff, sizeof(data.s_ip_mask));
        }
    } else {
        data.mc_ip_addr = entry->mc_ip;
        data.mc_ip_mask = 0xffffffff;
    
        if (entry->src_ip != -1 && entry->src_ip != 0) {
            data.vid = vlan;
            data.s_ip_addr = entry->src_ip;
            data.s_ip_mask = 0xffffffff;
        }
    }
  
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}


/* Set LSP (tunnel_id) as cud with a given multicast group.  */
int
multicast__mpls_encap_add(int unit, int mc_group_id, int sys_port, bcm_if_t tunnel_id){

    int encap_id;
    int rv;

    rv = bcm_multicast_egress_object_encap_get(unit, mc_group_id, tunnel_id, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_egress_object_encap_get mc_group_id:  0x%08x   gport:  0x%08x \n", mc_group_id, tunnel_id);
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }

    return rv;
}

/* Set vlan port as cud with a given multicast group.  */
int
multicast__vlan_encap_add(int unit, int mc_group_id, int sys_port, bcm_gport_t vlan_port_id){

    int encap_id;
    int rv;

    rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, vlan_port_id, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_egress_object_encap_get mc_group_id:  0x%08x   gport:  0x%08x \n", mc_group_id, tunnel_id);
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }

    return rv;
}

/*
 * add gport to the multicast. It can be used to replace multicast__mpls_encap_add and multicast__vlan_encap_add.
 */
int multicast__gport_encap_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress)
{
    int encap_id;
    int rv;

    /* update Multicast to have the added port  */
    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
            rv = bcm_multicast_vpls_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else {
            rv = BCM_E_UNAVAIL;
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in multicast__gport_encap_add mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
            return rv;
        }

    } else {
        encap_id = BCM_IF_INVALID;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  encap:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }

    return rv;
}


/* Enable/Disable IPMC for RIF
 * For enabling, pass enable == 1
 * For disabling, pass enable == 0
*/
int multicast__enable_rif_ipmc(int unit, bcm_l3_intf_t* intf, int enable){

    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_intf_t aux_intf;

    aux_intf.l3a_vid = intf->l3a_vid;
    aux_intf.l3a_mac_addr = intf->l3a_mac_addr;

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;

    if (!enable) {
        ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }

    rv = bcm_l3_intf_find(unit, &aux_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_find\n");
        return rv;
    }


    ingress_intf.vrf = aux_intf.l3a_vrf;


    rv = bcm_l3_ingress_create(unit, &ingress_intf, intf->l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}

int multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(int unit, int ing_mc_group_id, int egr_mc_group_id) {
    bcm_error_t rv = BCM_E_NONE;
    int erp_port;
    int count_erp;
    int sysport;
    bcm_multicast_replication_t ing_reps;

    if (is_device_or_above(unit, JERICHO2)) {
        if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
            /* Configure the MC group to work with both cores */        
            bcm_module_t mreps[2] = {0, 1};
            int nof_mreps = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));;

            rv = bcm_fabric_multicast_set(unit, egr_mc_group_id, 0, nof_mreps, mreps);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_fabric_multicast_set \n");
                return rv;
            }
        } else {                
            rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &ing_mc_group_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_create \n");
                return rv;
            }
            rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 1, &erp_port, &count_erp);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_internal_get \n");
                return rv;
            }
            rv = bcm_stk_gport_sysport_get(unit, erp_port, &sysport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_gport_sysport_get \n");
                return rv;
            }
            ing_reps.port = sysport;
            ing_reps.encap1 = egr_mc_group_id;
            rv = bcm_multicast_set(unit, ing_mc_group_id, BCM_MULTICAST_INGRESS_GROUP, 1, ing_reps);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_set \n");
                return rv;
            }
        }    
    }
    
    return rv;
}


