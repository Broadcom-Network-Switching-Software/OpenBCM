/*~~~~~~~~~~~~~~~~~~~~~~~Scheduled Fabric+Egress Multicast~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File name: cint_dnx_sch_fab_eg_mc.c

Notes:
This cint is intended to show how to implement Scheduled Fabric+Egress MC in DNX devices.
The packet will enter a device with a MC MAC DA. The MACT will then assign a MC group to the packet.
The MC ID will be configured as an egress MC, which will cause (in case the system has an FE device) the packet
to go to one of the FMQ, and from there to the fabric.
The FE device will then replicate the cells to each relevant FAP device (FAP devices that have ports that are
members of the MC group).
The packet will then enter the egress device's egress pipe, which will perform a lookup at the MC DB.
The result of this lookup will be a single port -> a dedicated recycle port.
This port's header_type_in and header_type_out must be configured in the SOC properties as follows:
tm_port_header_type_in_228=STACKING
tm_port_header_type_out_228=STACKING
The CUD field in the MC DB is populated with a new MC group ID that will be used in the 2nd pass on the egress device.

Packet will re-enter the ingress pipe of the egress device.
The device will recieve the new MC ID from the Multicast field in the FTMH of the packet coming from the recycle port.
ITM will then perform an Ingress lookup at the MC DB and will recieve from there a list of all ports that the packet
needs to be replicated to.
Since now it's an ingress MC - all packets will be replicated and scheduled (they will enter their respective VOQ).

Scheduled Fabric+Egress MC(option #1)
JR2 socs and reference cint:

Define a dedicated recycle port in the SOC properties and set its header type:
 * Test Scenario - start
config add ucode_port_228=RCY:core_0.228
config add tm_port_header_type_in_228=STACKING
config add tm_port_header_type_out_228=STACKING
tr 141

cint ../../../../src/examples/dnx/multicast/cint_dnx_sch_fab_eg_mc.c
cint ../../../../src/examples/dnx/field/cint_field_mc_two_pass.c
cint
field_mc_two_pass_main(0);
print sch_fabric_egress_mc_2_pass_example(0, 200, 201, 202, 203);
exit;

*Send a packet with the following:
tx 1 psrc=200 data=01005e000001000000000002810000019999450000b2000000000000ba4d0000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d

*MAC DA: 01:00:5e:00:00:01
*Vlan ID: 1
 * Test Scenario - end
*/

int recycle_port_mc = 228;
bcm_mac_t dest_mac = {0x01,0x00,0x5e,0x00,0x00,0x01};
bcm_vlan_t vid = 1;
int eg_mc_id = 6000;
int ing_mc_id = 5000;
int nof_ports = 3;
int mc_cuds[nof_ports] = {10,20,30};
int trunk_id = 1;
bcm_gport_t dest_ports[3] = {0,0,0};

int sch_fabric_egress_mc_2_pass_example(int unit, int in_port, int out_port1, int out_port2, int out_port3) {
    bcm_l2_addr_t l2add;
    bcm_gport_t gport;
    bcm_trunk_member_t trunk;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_t stk_tid = 0;
    uint32 flags = 0;
    bcm_multicast_replication_t replications[nof_ports];
    int port_index = 0;
    int member_count = 1;
    dest_ports[0] = out_port1;
    dest_ports[1] = out_port2;
    dest_ports[2] = out_port3;

    /*For iPMF context selection*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, recycle_port_mc, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, 7), "");

    /* Create Egress MC group and add the recycle port as member with CUD set to ingress MC ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &eg_mc_id), "(ingress)");

    /* Add destination port to the egress MC Group */
    flags = BCM_MULTICAST_EGRESS_GROUP;
    BCM_GPORT_LOCAL_SET(replications[0].port, recycle_port_mc);
    replications[0].encap1 = ing_mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, eg_mc_id, flags, 1, replications), "");

    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
        /* Configure the MC group to work with both cores */
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[nof_cores];
        uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));
        int nof_copies = 0;
        int core_index;
        for(core_index = 0; core_index < nof_cores; core_index++) {
            if ( valid_cores_bmp & (1 << (core_index))){
                mreps[nof_copies]= core_index;
                nof_copies ++;
            }
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, eg_mc_id, 0, nof_copies, mreps), "");
    }

    /* Create an Ingress MC group and add the final port destinations to it so they will be scheduled */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ing_mc_id), "(ingress)");

    flags = BCM_MULTICAST_INGRESS_GROUP;
    for (port_index = 0; port_index < (nof_ports - 1); port_index++)
    {
        BCM_GPORT_LOCAL_SET(replications[port_index].port, dest_ports[port_index]);
        replications[port_index].encap1 = 0;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, ing_mc_id, flags, (nof_ports - 1), replications), "");

    bcm_l2_addr_t_init(&l2add, dest_mac, vid);
    l2add.l2mc_group = eg_mc_id;
    l2add.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2add), "");

    return BCM_E_NONE;
}

int sch_fabric_egress_mc_2_pass_example_with_lag_destination(
    int unit,
    int recycle_port_mc,
    int eg_mc_id,
    int ing_mc_id,
    bcm_gport_t trunk_gport)
{
    uint32 flags = 0;
    bcm_multicast_replication_t replication;
    bcm_multicast_replication_t replications_erp[2];
    int member_count = 1;
    uint32 max_nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    bcm_gport_t erp_ports[2];
    bcm_gport_t sysport_erp = 0;
    int count_erp = 0;
    int index;

    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))
    {
        /* If Fabric is supported use Fabric MC */
        flags = (BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID);
    }
    else
    {
        /* If Fabric is not supported use Ingress + Egress MC */
        flags = (BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID);
    }
    /* Create Egress MC group and add the recycle port as member with CUD set to ingress MC ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &eg_mc_id), "(ingress)");

    /* Add destination port to the egress MC Group */
    flags = BCM_MULTICAST_EGRESS_GROUP;
    BCM_GPORT_LOCAL_SET(replication.port, recycle_port_mc);
    replication.encap1 = ing_mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, eg_mc_id, flags, 1, &replication), "");

    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[nof_cores];
        uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));
        int nof_mreps = 0;
        int core_index;
        for(core_index = 0; core_index < nof_cores; core_index++) {
            if ( valid_cores_bmp & (1 << (core_index))){
                mreps[nof_mreps]= core_index;
                nof_mreps ++;
            }
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, eg_mc_id, 0, nof_mreps, mreps), "");
    }
    else
    {
        /* Configure the MC group to work with both cores by using Ingress + Egress MC */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, max_nof_cores, erp_ports, &count_erp), "");

        if (count_erp == 0)
        {
            printf("ERP is not enabled, Ingress + Egress Multicast creation failed \n");
            return BCM_E_PARAM;
        }

        for (index = 0; index < count_erp; index++)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, erp_ports[index], &sysport_erp), "");
            replications_erp.port = sysport_erp;
            replications_erp.encap1 = eg_mc_id;
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, eg_mc_id, BCM_MULTICAST_INGRESS_GROUP, 1, &replications_erp), "");
        }
    }

    /* Create an Ingress MC group and add the final port destinations to it so they will be scheduled */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ing_mc_id),
        "(ingress)");

    flags = BCM_MULTICAST_INGRESS_GROUP;
    replication.port = trunk_gport;
    replication.encap1 = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, ing_mc_id, flags, 1, replication), "");

    return BCM_E_NONE;
}

/*
Scheduled Fabric+Egress MC(option #3)
JR2 socs and reference cint:
TBD

Send a packet with the following:
tx 1 psrc=200 data=01005e000001000000000002810000019999450000b2000000000000ba4d0000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d

MAC DA: 01:00:5e:00:00:01
Vlan ID: 1
*/
int sch_fabric_egress_mc_2_pass_dest_flow_example(int unit, int in_port, int out_port1, int out_port2, int out_port3) {
    bcm_l2_addr_t l2add;
    bcm_gport_t gport;
    bcm_trunk_member_t trunk;
    bcm_trunk_info_t trunk_info;
    int member_count = 1;
    bcm_trunk_t stk_tid = 0;
    uint32 flags = 0;
    int port_index = 0;
    bcm_multicast_replication_t replications[nof_ports];

    int voq_connector_ids[3] = {0xF000, 0xF008, 0xF010};  /* Default for Jericho , allocated with ID by API bcm_cosq_voq_connector_gport_add() */
    bcm_gport_t flow_gport = 0; /* JR: Allocated by API bcm_cosq_gport_add();  JR2: Allocated by API bcm_cosq_gport_add() with flag BCM_COSQ_GPORT_TM_FLOW_ID*/
    int number_queue = 8;
    uint32 additional_flags = BCM_COSQ_GPORT_TM_FLOW_ID;

    dest_ports[0] = out_port1;
    dest_ports[1] = out_port2;
    dest_ports[2] = out_port3;

    /* Create Egress MC group and add the recycle port as member with CUD set to ingress MC ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &eg_mc_id), "(ingress)");

    /* Add destination port to the egress MC Group */
    flags = BCM_MULTICAST_EGRESS_GROUP;
    for (port_index = 0; port_index < (nof_ports - 1); port_index++)
    {
        BCM_GPORT_LOCAL_SET(replications[port_index].port, recycle_port_mc);
        replications[port_index].encap1 = mc_cuds[port_index];
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, eg_mc_id, flags, (nof_ports - 1), replications), "");

    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
        /* Configure the MC group to work with both cores */
        bcm_module_t mreps[2];
        uint32 max_nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
        mreps[0] = 0;
        mreps[1] = 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, eg_mc_id, 0, max_nof_cores, mreps), "");
    }

    /* L2 MAC destianation is mcast */
    bcm_l2_addr_t_init(&l2add, dest_mac, vid);
    l2add.l2mc_group = eg_mc_id;
    l2add.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2add), "");

    return BCM_E_NONE;
}

/* Option ==1: sch_fabric_egress_mc_2_pass_example(unit, 1)
    Option ==2: TBD
    Option ==3: sch_fabric_egress_mc_2_pass_dest_flow_example(unit , 3)
  */
int sch_fabric_egress_mc_2_pass_clean_up(int unit, int option) {
    char error_msg[200]={0,};
    int port_index = 0;
    bcm_trunk_t stk_tid = 0;
    uint32 flags = 0;
    bcm_multicast_replication_t replications[nof_ports];

    /* Delete the L2 entry */
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_delete(unit, dest_mac, vid), "");

    /* Delete the egress replications */
    flags = BCM_MULTICAST_EGRESS_GROUP;
    BCM_GPORT_LOCAL_SET(replications[0].port, recycle_port_mc);
    replications[0].encap1 = ing_mc_id;
    snprintf(error_msg, sizeof(error_msg), "for egress MC_ID[%d]", eg_mc_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_delete(unit, eg_mc_id, flags, 1, &replications), error_msg);

    /* Destory egress mc group */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, eg_mc_id), error_msg);

    if (option == 1)
    {
        /* Delete the ingress replications */
        flags = BCM_MULTICAST_INGRESS_GROUP;
        for (port_index = 0; port_index < (nof_ports - 1); port_index++)
        {
            BCM_GPORT_LOCAL_SET(replications[port_index].port, dest_ports[port_index]);
            replications[port_index].encap1 = 0;
        }
        snprintf(error_msg, sizeof(error_msg), "for ingress MC_ID[%d]", ing_mc_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_delete(unit, ing_mc_id, flags, 1, replications), error_msg);

        /* Destory ingress mc group */
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, ing_mc_id), error_msg);
    }

    return BCM_E_NONE;
}

/*
 * Create an MC group
 * - replications - an array of replications.
 * - mc_group - the MC group
 * - mc_mode
 */
int
sch_fabric_egress_mc_ipmc_create_multicast(
    int ingress_unit,
    int egress_unit,
    bcm_multicast_replication_t * replications,
    int nof_replications,
    bcm_multicast_t mc_group)
{
    int create_flags = BCM_MULTICAST_WITH_ID;
    int ingress_create_flags = BCM_MULTICAST_WITH_ID;
    int egress_create_flags = BCM_MULTICAST_WITH_ID;
    int set_flags = 0;
    int mcdb_unit = 0;
    int mc_id_idx = ing_mc_id;
    int is_fabric_supported = 1;

    is_fabric_supported = *(dnxc_data_get(egress_unit, "fabric", "general", "blocks_exist", NULL));

    /*
     * Ingress Mc,
     */

    mcdb_unit = ingress_unit;
    set_flags = BCM_MULTICAST_INGRESS_GROUP;
    create_flags |= set_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(ingress_unit, create_flags, &mc_id_idx), "");

    if (is_fabric_supported)
    {
        /*
         * Egress Mc,
         */
        mcdb_unit = egress_unit;
        ingress_create_flags |= BCM_MULTICAST_EGRESS_GROUP;
        egress_create_flags |= BCM_MULTICAST_EGRESS_GROUP;
        mc_id_idx = mc_group;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(egress_unit, egress_create_flags, &mc_id_idx), "");
    }
    else
    {
        /*
         * Ingress Mc,
         */
        mcdb_unit = ingress_unit;
        ingress_create_flags |= BCM_MULTICAST_INGRESS_GROUP;
        egress_create_flags |= BCM_MULTICAST_INGRESS_GROUP;
        mc_id_idx = mc_group;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(ingress_unit, ingress_create_flags, &mc_id_idx), "");
    }
    /*
     * Fabric MC creation
     */
    int fe_unit = egress_unit;
    int fe_mc_id = mc_group;
    int dest_fap_id = 0;
    bcm_gport_t gport;

    if (is_fabric_supported) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport,recycle_port_mc);
        BCM_IF_ERROR_RETURN_MSG(bcm_stk_sysport_gport_get(egress_unit, gport, &gport), "");
        dest_fap_id = BCM_GPORT_MODPORT_MODID_GET(gport);
        bcm_fabric_multicast_set(fe_unit, fe_mc_id, 0, 1, &dest_fap_id);
    }

    /*Ingress MCDB setting */
    set_flags = BCM_MULTICAST_INGRESS_GROUP;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(ingress_unit, ing_mc_id, set_flags, nof_replications, replications), "");

    if (is_fabric_supported)
    {
        /*
         * Egress MCDB
         * One entry, point to RCY port. cud = ing_mc_id
         */
        BCM_GPORT_LOCAL_SET(replications[0].port, recycle_port_mc);
        replications[0].encap1 = ing_mc_id;
        set_flags = BCM_MULTICAST_EGRESS_GROUP;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(egress_unit, mc_group, set_flags, 1, replications), "");
    }
    else
    {
        /*
         * Ingress MCDB
         * One entry, point to RCY port. cud = ing_mc_id
         */
        BCM_GPORT_LOCAL_SET(replications[0].port, recycle_port_mc);
        replications[0].encap1 = ing_mc_id;
        set_flags = BCM_MULTICAST_INGRESS_GROUP;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(ingress_unit, mc_group, set_flags, 1, replications), "");

    }

    return BCM_E_NONE;
}

/*
 * forward entry adding(IPv4 Route)
 * INPUT:
 *   unit    - traffic incoming unit
 */
int sch_fabric_egress_mc_ipmc_forward_entry_configuration(
    int unit,
    int out_sys_port)
{
    int pp_db_replication_list[2];
    int pp_db_rep_index = 0x300000;
    int l3_mc_id;

    /*
     * 1. Add replication into the group
     */
    bcm_multicast_replication_t replications[5];
    int rep_count = 4;
    int out_port = 0;

    out_port = out_sys_port;
    /* 1st copy: to In-RIF*/
    bcm_multicast_replication_t_init(&replications[0]);
    replications[0].encap1 = cint_ipmc_route_info.in_rif;
    replications[0].port = out_port;

    /* 2nd copy: to Out-RIF1 without ARP*/
    bcm_multicast_replication_t_init(&replications[1]);
    replications[1].encap1 = cint_ipmc_route_info.out_rif;
    replications[1].port = out_port;

    /* 3rd copy: to Out-RIF2 with ARP*/
    bcm_multicast_replication_t_init(&replications[2]);
    replications[2].encap1 = cint_ipmc_route_info.arp_encap_id;
    replications[2].port = out_port;

    /* 4th copy: to vpn tunnel with one pointer */
    bcm_multicast_replication_t_init(&replications[3]);
    replications[3].encap1 = cint_ipmc_route_info.tunnel_encap_id[0];
    replications[3].port = out_port;

    BCM_IF_ERROR_RETURN_MSG(sch_fabric_egress_mc_ipmc_create_multicast(unit, unit, replications,rep_count, cint_ipmc_route_info.mc_group), "");

    /*
     * 2. Add IPv4 IPMC entry
     */
    BCM_MULTICAST_L3_SET(l3_mc_id,cint_ipmc_route_info.mc_group);
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_route_info.mc_group_ip, 0xFFFFFFFF, cint_ipmc_route_info.host_ip, 0xFFFFFFFF,
                       cint_ipmc_route_info.in_rif, cint_ipmc_route_info.vrf, cint_ipmc_route_info.mc_group, 0, 0), "to LEM table");

    return BCM_E_NONE;
}

/*
 * ipv4 2nd pass muliticast configuration
 * INPUT:
 *   in_port    - traffic incoming port
 *   out_sys_port - traffic outgoing port
 */
int sch_fabric_egress_mc_ipmc_configuration(
    int unit,
    int in_sys_port,
    int out_sys_port)
{
    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_init(unit, unit, in_sys_port, out_sys_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ingress_port_configuration(unit, cint_ipmc_route_info.in_local_port, cint_ipmc_route_info.in_rif), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_l3_intf_configuration(unit, cint_ipmc_route_info.in_local_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_l2_in_lif_configuration(unit), "");

    BCM_IF_ERROR_RETURN_MSG(sch_fabric_egress_mc_ipmc_forward_entry_configuration(unit, out_sys_port), "");

    /*For iPMF context selection*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, recycle_port_mc, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, 7), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_egress_port_configuration(unit,cint_ipmc_route_info.out_local_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_encapsulation_configuration(unit,cint_ipmc_route_info.out_local_port), "");
    return BCM_E_NONE;
}

/*
 * 1. JR2 fabric+egress 2 pass mc
 * 2. Compensation config
 * 3. STIF config
 */
int sch_fabric_egress_ipmc_2_pass_single_copy(int unit, int in_port, int out_port1) {
    bcm_gport_t gport;
    uint32 flags = 0;
    bcm_multicast_replication_t replications[2];
    cint_ipmc_info.in_port = in_port;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    uint32 sip_part_mask = 0xFFFF0000;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_cosq_pkt_size_adjust_info_t adjust_info;
    int crps_port_delta_1st_pass = 4; /* CRC */
    int crps_port_delta_2nd_pass = -38; /* CRC - system header */
    int header_append_size = 14; /* ETH0 */
    int stif_port = 1;
    bcm_stat_stif_source_t source;
    bcm_stat_stif_record_format_element_t ingress_record_format_elements[6], egress_record_format_elements[4];
    int nof_elements_ingress, nof_elements_egress;
    default_vlan_port_id_out[unit] = 0;
    /*
     * 1. Set In-Port to In ETh-RIF
     * 2. Set Out-Port
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_ipmc_info.in_port, cint_ipmc_info.intf_in), "intf_in");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");

    /*
     * 2. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address), "intf_in");

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ipmc_info.intf_in;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    /*
     * 4. Set Outgoing ETH-RIF properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipmc_info.intf_out, cint_ipmc_info.intf_out_mac_address), "");

    /* 5.1st pass: create Egress MC group and add the recycle port as member with CUD set to ingress MC ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &eg_mc_id), "(ingress)");

    /* Add destination port to the egress MC Group */
    flags = BCM_MULTICAST_EGRESS_GROUP;
    BCM_GPORT_LOCAL_SET(replications[0].port, recycle_port_mc);
    replications[0].encap1 = ing_mc_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, eg_mc_id, flags, 1, replications), "");

    if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
        /* Configure the MC group to work with both cores */
        int nof_cores = *dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL);
        bcm_module_t mreps[nof_cores];
        uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));
        int nof_copies = 0;
        int core_index;
        for(core_index = 0; core_index < nof_cores; core_index++) {
            if ( valid_cores_bmp & (1 << (core_index))){
                mreps[nof_copies]= core_index;
                nof_copies ++;
            }
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, eg_mc_id, 0, nof_copies, mreps), "");
    }

    /* 6. 2nd pass: create an Ingress MC group and add the final port destinations to it so they will be scheduled */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ing_mc_id), "(ingress)");

    flags = BCM_MULTICAST_INGRESS_GROUP;
    /* single copy */
    BCM_GPORT_LOCAL_SET(replications[0].port, out_port1);
    replications[0].encap1 = cint_ipmc_info.intf_out;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, ing_mc_id, flags, 1, replications), "");

    /* 7. Add IPMC entry */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, dip_mask, cint_ipmc_info.sip, sip_mask,
                       cint_ipmc_info.intf_in, cint_ipmc_info.vrf, cint_ipmc_info.mc_group, 0, 0), "to LEM tabl");

    /* 8. Set EVE action to None for 2nd copy */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, default_vlan_port_id_out[unit],
                                                0,0,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                0,           0,
                                                3, 0, 0), "");

    /* 9. iPMF context selection*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, recycle_port_mc, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, 7), "");

    /* 10. Ingress compensation config */
    /* 1st pass crps compensation for for CRC 4B */
    BCM_GPORT_LOCAL_SET(adjust_info.gport,in_port);
    adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;
    /* Set per queue compensation */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, crps_port_delta_1st_pass), "");

    /* 2nd pass crps compensation for CRC - system_header = -38 */
    BCM_GPORT_LOCAL_SET(adjust_info.gport,recycle_port_mc);
    adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;
    /* Set per queue compensation */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, crps_port_delta_2nd_pass), "");

    /* Header append profile, compensation for ETH0 14B */
    int profile_id=0;
    BCM_GPORT_PROFILE_SET(adjust_info.gport,profile_id);
    adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;
    /* Set per queue compensation */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, header_append_size), "");

    /* 11. Disable header compensation on ingress ports */
    /* 1st pass */
    bcm_stat_counter_command_id_key_t key;
    key.core_id = BCM_CORE_ALL;
    key.source = bcmStatCounterInterfaceIngressReceivePp;
    key.command_id = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_pkt_size_adjust_select_set(unit, 0, key, bcmStatPktSizeSelectCounterIngressHeaderTruncate, 0), "1st pass");

    /* 2nd pass rcy port */
    key.source = bcmStatCounterInterfaceIngressReceivePp;
    key.command_id = 2;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_pkt_size_adjust_select_set(unit, 0, key, bcmStatPktSizeSelectCounterIngressHeaderTruncate, 0), "1st pass");

    /* 12. STIF config */
    /* source mapping config*/
    /* rcy port is set to core 0 */
    flags = 0;
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_stif_source_mapping_set(unit, flags, source, stif_port), "for ingress");

    source.core = 0;
    source.src_type = bcmStatStifSourceEgressDequeue;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_stif_source_mapping_set(unit, flags, source, stif_port), "for egress");

    /* report format config*/
    /* ingress */
    ingress_record_format_elements[0].element_type = bcmStatStifRecordElementIngressDispositionIsDrop;
    ingress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[1].element_type = bcmStatStifRecordElementIngressIncomingDropPrecedence;
    ingress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[2].element_type = bcmStatStifRecordElementIngressTrafficClass;
    ingress_record_format_elements[2].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[3].element_type = bcmStatStifRecordElementIngressQueueNumber;
    ingress_record_format_elements[3].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[4].element_type = bcmStatStifRecordElementOpCode;
    ingress_record_format_elements[4].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[5].element_type = bcmStatStifRecordElementPacketSize;
    ingress_record_format_elements[5].mask = BCM_STAT_FULL_MASK;
    nof_elements_ingress = 6;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_stif_record_format_set(unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_record_format_elements), "INGRESS");

    /* egress */
    egress_record_format_elements[0].element_type = bcmStatStifRecordElementEgressMetaDataTrafficClass;
    egress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    egress_record_format_elements[1].element_type = bcmStatStifRecordElementEgressMetaDataDropPrecedence;
    egress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    /* For EGRESS - MetaData elements should always be used full
     *  - mask BCM_STAT_FULL_MASK should always be set */
    egress_record_format_elements[2].element_type = bcmStatStifRecordElementOpCode;
    egress_record_format_elements[2].mask = BCM_STAT_FULL_MASK;
    egress_record_format_elements[3].element_type = bcmStatStifRecordElementPacketSize;
    egress_record_format_elements[3].mask = BCM_STAT_FULL_MASK;

    nof_elements_egress = 4;
    BCM_IF_ERROR_RETURN_MSG(bcm_stat_stif_record_format_set(unit, BCM_STAT_EGRESS, nof_elements_egress, egress_record_format_elements), "EGRESS");

    return BCM_E_NONE;
}
