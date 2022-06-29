/*
 * Multicast utility functions
 */

int egress_mc = 0;
/*
 * If set to a non-zero value then extra printouts are ejected to help visibility
 * for debugging.
 */
int print_detailed_dnx_utils_multicast = 0;

/* Adding entries to MC group
 * ipmc_index:  mc group
 * ports: array of ports to add to the mc group
 * cud 
 * nof_mc_entries: number of entries to add to the mc group
 * is_ingress:  if true, add ingress mc entries, otherwise, add egress mc 
 * 
 */
int multicast__add_multicast_entry(int unit, int ipmc_index, int *ports, int *cud, int nof_mc_entries, int is_egress) {
    int ii;
    uint32 flags;
    int rv = BCM_E_NONE;
    int replication_max = 30;
    bcm_multicast_replication_t rep_array[nof_mc_entries];
    bcm_multicast_replication_t out_rep_array[replication_max];
    char *proc_name;

    proc_name = "multicast__add_multicast_entry";    
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): ENTER. mc_group %d nof_mc_entries %d ports[0] 0x%X is_egress %d\r\n", proc_name, ipmc_index, nof_mc_entries, ports[0], is_egress);
    }

    int rep_count;

    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n");
    }
    for (ii=0; ii<nof_mc_entries; ii++)
    {
        bcm_multicast_replication_t_init(&rep_array[ii]);
        rep_array[ii].port = BCM_GPORT_IS_MODPORT(ports[ii]) ? BCM_GPORT_MODPORT_PORT_GET(ports[ii]) : ports[ii];
        rep_array[ii].encap1= cud[ii];
        if (print_detailed_dnx_utils_multicast)
        {
            printf("%s(): ii %d rep_array.port 0x%X rep_array.encap1 0x%X\r\n", proc_name, ii, rep_array[ii].port, rep_array[ii].encap1);
        }
    }
    if (is_egress) {
        flags = BCM_MULTICAST_EGRESS_GROUP;
    } else {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
    } 
    rv = bcm_multicast_add(unit,ipmc_index,flags,nof_mc_entries,rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_multicast_add: port[0] %d encap_id[0]: %d \n", proc_name, ports[0], cud[0]);
        return rv;
    }
    rv = bcm_multicast_get(unit, ipmc_index, flags, replication_max, out_rep_array, &rep_count);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_multicast_get: port[0] %d encap_id[0]: %d \n", proc_name, ports[0], cud[0]);
        return rv;
    }
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): rep_count %d. Get multicast group rep array: \r\n", proc_name, rep_count);
        for (ii=0; ii < rep_count; ii++)
        {
            printf("%s(): ii %d out_rep_array.port 0x%X out_rep_array.encap1 0x%X\r\n", proc_name, ii, out_rep_array[ii].port, out_rep_array[ii].encap1);
        }
    }

    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s():EXIT\r\n", proc_name);
    }
    return rv;
}

/* Deleting entries from a MC group
 * ipmc_index:  mc group
 * ports: array of ports to delete from the mc group
 * cud 
 * nof_mc_entries: number of entries to delete from the mc group
 * is_ingress:  if true, delete ingress mc entries, otherwise, delete egress mc 
 * 
 */
int multicast__delete_multicast_entry(int unit, int ipmc_index, int *ports, int *cud, int nof_mc_entries, int is_egress) {
    int ii;
    uint32 flags;
    int rv = BCM_E_NONE;
    int replication_max = 30;
    bcm_multicast_replication_t rep_array[nof_mc_entries];
    bcm_multicast_replication_t out_rep_array[replication_max];
    char *proc_name;

    proc_name = "multicast__delete_multicast_entry";    
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): ENTER. mc_group %d nof_mc_entries %d ports[0] 0x%X is_egress %d\r\n", proc_name, ipmc_index, nof_mc_entries, ports[0], is_egress);
    }

    int rep_count;

    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n");
    }
    for (ii=0; ii<nof_mc_entries; ii++)
    {
        bcm_multicast_replication_t_init(&rep_array[ii]);
        rep_array[ii].port = BCM_GPORT_IS_MODPORT(ports[ii]) ? BCM_GPORT_MODPORT_PORT_GET(ports[ii]) : ports[ii];
        rep_array[ii].encap1= cud[ii];
        if (print_detailed_dnx_utils_multicast)
        {
            printf("%s(): ii %d rep_array.port 0x%X rep_array.encap 0x%X\r\n", proc_name, ii, rep_array[ii].port, rep_array[ii].encap1);
        }
    }		
    if (is_egress) {
        flags = BCM_MULTICAST_EGRESS_GROUP;
    } else {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
    } 
    rv = bcm_multicast_delete(unit,ipmc_index,flags,nof_mc_entries,rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_multicast_delete: port[0] %d encap_id[0]: %d \n", proc_name, ports[0], cud[0]);
        return rv;
    }
    rv = bcm_multicast_get(unit, ipmc_index, flags, replication_max, out_rep_array, &rep_count);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_multicast_get: port[0] %d encap_id[0]: %d \n", proc_name, ports[0], cud[0]);
        return rv;
    }
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): rep_count %d. Get multicast group rep array: \r\n", proc_name, rep_count);
        for (ii=0; ii < rep_count; ii++)
        {
            printf("%s(): ii %d out_rep_array.port 0x%X out_rep_array.encap1 0x%X\r\n", proc_name, ii, out_rep_array[ii].port, out_rep_array[ii].encap1);
        }
    }

    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s():EXIT\r\n", proc_name);
    }
    return rv;
}

/*
 * '*mc_group_id' is expected to contain the multicast group ID to be created.
 */
int multicast__open_mc_group(int unit, int *mc_group_id, int extra_flags)
{
    int rv = BCM_E_NONE;
    int flags;
    uint32 flags_get = 0;
    unsigned long flags_hex;
    char *proc_name;

    proc_name = "multicast__open_mc_group";    
    /*
     * First check if exists
     * In case yes, destroy before open, to ensure it not exist.
     */
    rv = bcm_multicast_group_get(unit, *mc_group_id, &flags_get);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_multicast_group_get, mc_group_id %d\n", proc_name, *mc_group_id);
        return rv;
    }
    flags_get &= ~BCM_MULTICAST_WITH_ID;
    if (flags_get)
    {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            rv = bcm_multicast_group_destroy(unit, flags_get, *mc_group_id);
        } else {
            rv = bcm_multicast_destroy(unit, *mc_group_id);
        }
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Multicast group exists, try to destroy %d.\n",proc_name, *mc_group_id);
            printf("%s(): Error, bcm_multicast_destroy, flags_get %d mc_group_id %d \n", proc_name,flags_get,*mc_group_id);
            return rv;
        }
    }



    if (print_detailed_dnx_utils_multicast)
    {
        printf("%s(): egress_mc: %d \n",proc_name, egress_mc);
    }
    /* create ingress/egress MC */
    if (egress_mc) {
        flags = BCM_MULTICAST_EGRESS_GROUP;
    } else {
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
    }
    flags = flags | BCM_MULTICAST_WITH_ID | extra_flags;
    rv = bcm_multicast_create(unit, flags, mc_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_multicast_create, flags $flags mc_group_id $mc_group_id \n", proc_name);
        return rv;
    }
    flags_hex = flags;
    if (print_detailed_dnx_utils_multicast)
    {
        printf("%s(): Created mc_group %d with flags 0x%08X\n", proc_name, *mc_group_id, flags_hex);
    }
    return rv;
}

int multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(int unit, int ing_mc_group_id, int egr_mc_group_id) {
    bcm_error_t rv = BCM_E_NONE;
    int erp_port[2];
    int count_erp;
    int sysport;
    bcm_multicast_replication_t ing_reps[2];
    int ing_rep_index;

    int is_fabric_enabled = *(dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL));
    int is_mesh = *(dnxc_data_get(unit, "fabric", "general", "connect_mode", NULL)) != 2; /** FE */

    if (is_fabric_enabled && is_mesh) {
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
        rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 2, erp_port, &count_erp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_internal_get \n");
            return rv;
        }
        for (ing_rep_index = 0; ing_rep_index < count_erp; ing_rep_index++) {
            rv = bcm_stk_gport_sysport_get(unit, erp_port[ing_rep_index], &sysport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_gport_sysport_get \n");
                return rv;
            }
            ing_reps[ing_rep_index].port = sysport;
            ing_reps[ing_rep_index].encap1 = egr_mc_group_id;
        }
        rv = bcm_multicast_set(unit, ing_mc_group_id, BCM_MULTICAST_INGRESS_GROUP, count_erp, ing_reps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_set \n");
            return rv;
        }
    }

    return rv;
}


int multicast__populate_destid_array(int unit, bcm_port_t port, bcm_module_t *destids, uint32 *destid_count)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 dummy_flags, found = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 core = 0;
    int nof_cores;
    char *proc_name;

    proc_name = "multicast__populate_destid_array";
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): ENTER. *destid_count %d port %d\r\n", proc_name, *destid_count, port);
    }
    /*
     * For Jericho 2 API bcm_fabric_multicast_set with an array of the cores on which we have replications.
     * In order to do that, check the core of each destination and populate the destids array.
     */
    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != rv)
    {
        printf("%s(): bcm_port_get failed \n", proc_name);
        return rv;
    }

    /*
     * Check if core already exists in array
     */
    nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    for (core = 0; core < nof_cores; core++)
    {
        if (destids[core] == mapping_info.core)
        {
            found = TRUE;
            break;
        }
    }
    /*
     * If the core does not already exists in the array, add it and
     * increment the destid_count.
     */
    if (!found)
    {
        destids[*destid_count] = mapping_info.core;
        *destid_count += 1;
    }
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): EXIT\r\n", proc_name);
    }
    return rv;
}

int multicast__open_egress_mc_group_with_local_ports(int unit, int mc_group_id, int *dest_local_port_id, int *cud, int num_of_ports, int extra_flags)
{
    int ii;
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    int gport_info_out_port[num_of_ports];
    bcm_cosq_gport_type_t gport_type = bcmCosqGportTypeLocalPort;
    uint32 destid_count;
    int nof_destids = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    bcm_module_t destids[nof_destids];
    int ing_mc_group_id = mc_group_id;
    int erp_port;
    int count_erp;
    int sysport;    
    bcm_multicast_replication_t ing_reps;
    char *proc_name;

    proc_name = "multicast__open_egress_mc_group_with_local_ports";
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): ENTER. num_of_ports %d nof_destids %d\r\n", proc_name, num_of_ports, nof_destids);
    }
    egress_mc = 1;
    
    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n", proc_name);
        return rv;
    }
    destid_count = 0;
    sal_memset(destids, 0xFF, sizeof(destids));
    sal_memset(gport_info_out_port, 0, sizeof(gport_info_out_port));
    for (ii=0; ii < num_of_ports; ii++)
    {
        if (print_detailed_dnx_utils_multicast)
        {
            printf("\r\n %s(): ii %d dest_local_port_id[%d] %d cud[%d] %d\r\n", proc_name, ii, ii, dest_local_port_id[ii], ii, cud[ii]);
        }
        sal_memset(&gport_info,0,sizeof(gport_info));
        BCM_GPORT_LOCAL_SET(gport_info.in_gport,dest_local_port_id[ii]); 
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_gport_handle_get, gport_type $gport_type \n");
            return rv;
        }
        gport_info_out_port[ii] = gport_info.out_gport;

        int connect_mode_fe = 2;
        if (*dnxc_data_get(unit, "fabric", "general", "connect_mode", NULL) != connect_mode_fe)
        {
            /*
             * Populate the destids array with only the needed destination IDs
             */
            rv = multicast__populate_destid_array(unit, dest_local_port_id[ii], destids, &destid_count);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, multicast__populate_destid_array\n", proc_name);
                return rv;
            }
            if (print_detailed_dnx_utils_multicast)
            {
                printf("\r\n %s(): ii %d destid_count %d\r\n", proc_name, ii, destid_count);
            }
        }
    }
    rv = multicast__add_multicast_entry(unit, mc_group_id, gport_info_out_port, cud, num_of_ports, egress_mc);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id dest_gport $gport_info.out_gport \n");
        return rv;
    }
    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): destids -\r\n", proc_name);
        for (ii=0; ii < nof_destids; ii++)
        {
            printf("%s(): ii %d destids[%d] %d\r\n", proc_name, ii, ii, destids[ii]);
        }
        printf("\r\n");
    }

    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))
    {
        if (print_detailed_dnx_utils_multicast)
        {
            printf("\r\n %s(): With fabric: Call bcm_fabric_multicast_set(). mc_group_id %d\r\n", proc_name, mc_group_id);
        }
        int connect_mode_fe = 2;
        if (*dnxc_data_get(unit, "fabric", "general", "connect_mode", NULL) != connect_mode_fe)
        {
            rv = bcm_fabric_multicast_set(unit, mc_group_id, 0, destid_count, destids);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, bcm_fabric_multicast_set, mc_group_id $mc_group_id \n", proc_name);
                return rv;
            }
        }
    }

    if (print_detailed_dnx_utils_multicast)
    {
        printf("\r\n %s(): EXIT\r\n", proc_name);
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

/*
 * add gport to the multicast
 */
int multicast__gport_encap_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress)
{
    int encap_id;
    int rv = BCM_E_NONE;

    /* update Multicast to have the added port  */
    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
            rv = bcm_multicast_vpls_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_TUNNEL(gport)) {
            encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);
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

/*
 * add gport to the multicast
 */
int multicast__gport_encap_delete(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress)
{
    int encap_id;
    int rv = BCM_E_NONE;

    /* update Multicast to have the added port  */
    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
            rv = bcm_multicast_vpls_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_TUNNEL(gport)) {
            encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);
        } else {
            rv = BCM_E_UNAVAIL;
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in multicast__gport_encap_delete mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
            return rv;
        }

    } else {
        encap_id = BCM_IF_INVALID;
    }

    rv = multicast__delete_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__delete_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  encap:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }
    
    return rv;
}

/*
 * Multicast utility object -- end
 */
