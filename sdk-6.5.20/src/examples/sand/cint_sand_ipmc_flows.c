/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~IPMC FLOWS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_sand_ipmc_flows.c
 * Purpose: Example of various IPMC flows.
 */

/* Main struct
 * holds data structures relevant for the hereby exemplified flows
 */
struct ipmc_flows_info_s {

    bcm_ip_t mc_ip_arr[4];
    bcm_ip_t src_ip_arr[4];
    int ipmc_indices[8]; /* this array's entries will keep the indices of the groups without change*/
    int ipmc_mc_groups[8]; /* this array's entries will be changed after creating multicast groups*/
    int forwarding_ports_1[3];
    int forwarding_ports_2[2];
    int forwarding_ports_3[2];
    int forwarding_ports_4[2];
};

ipmc_flows_info_s ipmc_flows_info1;

/*my_macs*/
bcm_mac_t mac_address_1 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
bcm_mac_t mac_address_2 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
bcm_mac_t mac_address_3 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
bcm_mac_t mac_address_4 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x03};
bcm_mac_t mac_address_5 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x04};
bcm_mac_t mac1 = {0x01, 0x00, 0x5E, 0x00, 0x01, 0x03};
bcm_mac_t mac2 = {0x01, 0x00, 0x5E, 0x00, 0x01, 0x02};

/*
 * 1: for IPMC packet with VRF !=0  (VPN) forwarding is according to VRF, G
 * 0: for IPMC packet forwarding is according to <RIF,G,SIP> regardless the VRF value
 */
int ipmc_vpn_lookup_enable = 1;

/*
 * 1: egress multicast
 * 0: ingress multicase
 */
int ipmc_egress_multicast_enable = 0;


/* Init function. configures relevant data to be placed
 * in the above declared data structures
 */
void
ipmc_flows_init (
    int unit,
    int outP,
    int outP2,
    int outP3)
{
    int ipmc_index_offset;
    int ipmc_index_upper_bound;
    int i;

    ipmc_flows_info1.forwarding_ports_1[0] = outP; /* Different combinations of ports representing multiple destinations for a certain mc group*/
    ipmc_flows_info1.forwarding_ports_1[1] = outP2;
    ipmc_flows_info1.forwarding_ports_1[2] = outP3;

    ipmc_flows_info1.forwarding_ports_2[0] = outP ;
    ipmc_flows_info1.forwarding_ports_2[1] = outP2;

    ipmc_flows_info1.forwarding_ports_3[0] = outP2 ;
    ipmc_flows_info1.forwarding_ports_3[1] = outP3;

    ipmc_flows_info1.forwarding_ports_4[0] = outP ;
    ipmc_flows_info1.forwarding_ports_4[1] = outP3;

    ipmc_flows_info1.mc_ip_arr[0] = 0xE0000102; /* 224.0.1.2 */
    ipmc_flows_info1.mc_ip_arr[1] = 0xE0000103; /* 224.0.1.3 */
    ipmc_flows_info1.mc_ip_arr[2] = 0xE0000104; /* 224.0.1.4 */
    ipmc_flows_info1.mc_ip_arr[3] = 0xE0000105; /* 224.0.1.5 */

    ipmc_flows_info1.src_ip_arr[0] = 0xC0A8000B; /* 192.168.0.11 */
    ipmc_flows_info1.src_ip_arr[1] = 0xC0A8000C; /* 192.168.0.12 */
    ipmc_flows_info1.src_ip_arr[2] = 0xC0A8000D; /* 192.168.0.13 */
    ipmc_flows_info1.src_ip_arr[3] = 0xC0A8000E; /* 192.168.0.14 */

    if (is_device_or_above(unit, JERICHO2)) {
        ipmc_index_offset = 0;
        ipmc_index_upper_bound = *(dnxc_data_get(unit,  "multicast", "params", "max_ing_mc_groups", NULL));
    }
    else if  (is_device_or_above(unit, QUMRAN_AX)) {
        ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmaps");
        ipmc_index_upper_bound = diag_test_mc(unit,"get_nof_ingress_mcids");
    } else {
        ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmap_groups");
        ipmc_index_upper_bound = diag_test_mc(unit,"get_nof_ingress_mcids");
    }

    sal_srand(sal_time());
    for (i=0; i <= 5; i++) {  /* picking random values for the mc groups */
        ipmc_flows_info1.ipmc_mc_groups[i] = (sal_rand() % ipmc_index_upper_bound);
        if(ipmc_flows_info1.ipmc_mc_groups[i] < ipmc_index_offset) {
            ipmc_flows_info1.ipmc_mc_groups[i] += ipmc_index_offset; /* ipmc index will be in range (4k,32k)*/
        }
        while (is_in_array(ipmc_flows_info1.ipmc_mc_groups, i-1, ipmc_flows_info1.ipmc_mc_groups[i])){ /* verifying that we don't get a previous value */
            ipmc_flows_info1.ipmc_mc_groups[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset;
        }

        ipmc_flows_info1.ipmc_indices[i] = ipmc_flows_info1.ipmc_mc_groups[i];
    }
}

/* utility function that checks whether a certain value already exists in a given array*/
int is_in_array(
    int *arr,
    int last_index,
    int val)
{
    int i;

    for (i=0; i <= last_index ; i++) {
        if (val == arr[i]) {
            return 1;
        }
    }

    return 0;
}

/* Destroys objects created in this feature*/
int
ipmc_flows_destroy(int unit, int nof_mc_indices){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i <= nof_mc_indices - 1; i++) {
        rv = bcm_multicast_destroy(unit, ipmc_flows_info1.ipmc_indices[i]);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            printf("Error, in multicast_destroy\n");
            return rv;
        }
    }

    rv = bcm_l3_intf_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_intf_delete_all\n");
        return rv;
    }

    return rv;

}

/*
 * To check the IPMC bridge configuration using the flag BCM_IPMC_L2 this cint creates one IN-RIF and
 * three entries LEM<FID,G> bridge, SSM LPM<FID,G,S> bridge and LPM<VRF,G,S,IN-RIF> where each entry
 * output through different out-port.
 * It expected that all entries will be created regardless to the IN-RIF IPMC state and that the each
 * entry will be hit according the entering packet and the IN-RIF configuration.
 */
int ipmc_force_bridge(int unit,int inport, int outP1, int outP2, int outP3 ) {
    bcm_ipmc_addr_t data;
    uint32 IPMcV4 = 0xE0000100;
    uint32 SIP    = 0xC0A8000B;
    uint32 CLEAR_Pmask = 0xFFFFFFFF;
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;
    bcm_mac_t MyMac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
    sand_utils_l3_port_s l3_port_structure;
    int vlan = 300;
    int rv,i;
    int interfaceID = 50;
    int ports[3];
    int ipmcGroups[3];
    bcm_multicast_t mc_id;

    ports[0] = outP1;
    ports[1] = outP2;
    ports[2] = outP3;

    /** get different IPMC group range */
    ipmc_flows_init(unit, outP1, outP2, outP3);

    if (is_device_or_above(unit, JERICHO2) )
    {
        sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, inport, vlan);
        rv = sand_utils_l3_port_set(unit, &l3_port_structure);
        if (rv != BCM_E_NONE)
        {
           printf("Error, sand_utils_l3_port_set\n");
           return rv;
        }
    }

    /* create L3 interface */
    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, MyMac, 6);
    intf.l3a_vid = vlan;
    intf.l3a_vrf = vlan;
    intf.l3a_intf_id = vlan;
    intf.l3a_flags = BCM_L3_WITH_ID;

    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }
    vlan =400;

    if (is_device_or_above(unit, JERICHO2) )
    {
        sand_utils_l3_port_s_common_init(unit, 0, &l3_port_structure, inport, vlan);
        rv = sand_utils_l3_port_set(unit, &l3_port_structure);
        if (rv != BCM_E_NONE)
        {
           printf("Error, sand_utils_l3_port_set\n");
           return rv;
        }
    }


    bcm_l3_intf_t_init(&intf);

    sal_memcpy(intf.l3a_mac_addr, MyMac, 6);
    intf.l3a_vid = vlan;
    intf.l3a_vrf = vlan;
    intf.l3a_intf_id = vlan;
    intf.l3a_flags = BCM_L3_WITH_ID;

    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf= vlan;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
    }

    int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3);
    for (i=0;i<3;i++) {
        ipmcGroups[i] = ipmc_flows_info1.ipmc_mc_groups[i];
        if (is_device_or_above(unit, JERICHO2)) {
            if (i < 2) {
                /* bridge */
                mc_id = create_ip_mc_group(unit, flags, ipmcGroups[i], ports[i], 1, 0, FALSE);
            } else {
                /* route */
                mc_id = create_ip_mc_group(unit, flags, ipmcGroups[i], ports[i], 1,300, FALSE);
            }
            if (mc_id != ipmcGroups[i]) {
                printf("Error, create_ip_mc_group");
                return rv;
            }
        } else {
            rv = bcm_multicast_create(unit, flags, &ipmcGroups[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_create \n");
                return rv;
            }
            if (i < 2) {
                rv = bcm_multicast_ingress_add(unit, ipmcGroups[i], ports[i] , 0);
                if (rv != BCM_E_NONE) {
                    /* bridge */
                    printf("Error, bcm_multicast_ingress_add");
                    return rv;
                }
            } else {
                /* route */
                add_ingress_multicast_forwarding_with_encap(unit,ipmcGroups[i], &ports[i] ,1,300,0);
            }
        }
     }

    /*
     * create an bridge entry LEM<FID, G>
     */
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = IPMcV4;
    data.mc_ip_mask = CLEAR_Pmask;
    data.s_ip_addr = 0x0;
    data.s_ip_mask = CLEAR_Pmask;
    if (is_device_or_above(unit, JERICHO2)) {
        /*SIP empty with mask is illegal on JR2, set mask to 0x0.*/
        data.s_ip_mask = 0x0;
    }
    data.vid = vlan;
    data.flags |= BCM_IPMC_L2;
    data.group = ipmcGroups[0];
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }


    /*
     * create an bridge entry SSM LEM<FID, G,S>
     */

    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = IPMcV4;
    data.mc_ip_mask = CLEAR_Pmask;
    data.s_ip_addr = SIP;
    data.s_ip_mask = CLEAR_Pmask;
    data.vid = vlan;
    data.flags |= BCM_IPMC_L2;
    data.group = ipmcGroups[1];

    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    /*
     * create a route entry LPM<VRF,G,S,IN-RIF>
     */
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = IPMcV4;
    data.mc_ip_mask = CLEAR_Pmask;
    data.s_ip_addr = SIP;
    data.s_ip_mask = CLEAR_Pmask;
    data.vid = vlan;
    data.vrf = vlan;
    data.group = ipmcGroups[2];


    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}

int
add_l3_interface_multicast_forwarding (
    int unit,
    int ipmc_index,
    int *ports,
    int nof_ports,
    int l3InterfaceId)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_multicast_replication_t replications[8];
    int flags = 0;

    for (i=0; i < nof_ports; i++) {
        bcm_multicast_replication_t_init(&replications[i]);
        replications[i].encap1 = l3InterfaceId;
        BCM_GPORT_LOCAL_SET(replications[i].port, ports[i]);
    }

    flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
    rv = bcm_multicast_set(unit, ipmc_index, flags, nof_ports, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_set\n");
        return rv;
    }

    return rv;
}

/*
 * Adding ports to ingress MC group with encapsulation
 * AVT - advanced_vlan_translation_mode
 */
int
add_ingress_multicast_forwarding_with_encap (
    int unit,
    int ipmc_index,
    int *ports,
    int nof_ports,
    int vlan,
    int AVT)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_if_t encap_id;
    bcm_vlan_port_t vp;
    bcm_multicast_replication_t replications[8];
    int flags = 0;
    int action_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_port_tag_format_class_t tag_format_stag = 2;

    for (i=0; i < nof_ports; i++)
    {
        bcm_vlan_port_t_init(&vp);
        vp.port = ports[i];
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        if (!is_device_or_above(unit, JERICHO2)) {
            /*
             * On JR2, vsi should be 0, otherwise it will be virtual_ac. Here a out lif is needed.
             */
            vp.vsi = vlan;
            vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vlan;
        }
        vp.match_vlan = vlan;

        rv = bcm_vlan_port_find(unit, &vp);
        if (rv != BCM_E_NONE) {
            rv = bcm_vlan_port_create(unit, &vp);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_create\n");
                return rv;
            }
        }


        if (AVT /*advanced_vlan_translation_mode*/) {
            rv = vlan__port_translation__set(unit, vlan, BCM_VLAN_NONE, vp.vlan_port_id, 2, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, vlan_port_translation_set\n");
                return rv;
            }

            rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_create\n");
            }

            bcm_vlan_action_set_t_init(&action);
            action.outer_tpid = 0x8100;
            action.dt_outer = bcmVlanActionReplace;
            rv = bcm_vlan_translate_action_id_set(unit,BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_set\n");
            }

            if (is_device_or_above(unit, JERICHO2)){
                /* Create Tag-Struct for S-TAG */
                rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID|BCM_PORT_TPID_CLASS_EGRESS_ONLY, bcmTagStructTypeSTag, &tag_format_stag);
                if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                    printf("Error, in bcm_port_tpid_class_create( oPort )\n");
                    return rv;
                }
            }

            bcm_port_tpid_class_t port_tpid_class;
            bcm_port_tpid_class_t_init(&port_tpid_class);

            port_tpid_class.tpid1 = 0x8100;
            port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
            port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
            port_tpid_class.port =  ports[i];
            port_tpid_class.tag_format_class_id = tag_format_stag;

            rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_tpid_class_set( oPort )\n");
                return rv;
            }

            action_class.vlan_edit_class_id = 2;
            action_class.tag_format_class_id = tag_format_stag;
            action_class.vlan_translation_action_id = action_id;
            action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
            rv= bcm_vlan_translate_action_class_set( unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set\n");
            }
        }

        rv = bcm_multicast_vlan_encap_get(unit, ipmc_index, 0/*not in use in the function*/, vp.vlan_port_id, &encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_vlan_encap_get\n");
            return rv;
        }

        bcm_multicast_replication_t_init(&replications[i]);
        replications[i].encap1 = encap_id;
        BCM_GPORT_LOCAL_SET(replications[i].port, ports[i]);
    }

    flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
    rv = bcm_multicast_set(unit, ipmc_index, flags, nof_ports, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_set\n");
        return rv;
    }

    return rv;
}

/* Adding ports to egress MC group*/
int add_egress_multicast_forwarding(int unit, int ipmc_index, int *ports , int nof_ports, int vlan){

    int rv = BCM_E_NONE;
    int i;
    uint32 flags;
    bcm_multicast_replication_t rep_array;

    flags = (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_EGRESS_GROUP;
    for (i=0; i < nof_ports; i++) {
        bcm_multicast_replication_t_init(rep_array);
        rep_array.port = ports[i];

        /** L2 MC, the rif doesn't make sense.*/
        rep_array.encap1 = 0;
        rv = bcm_multicast_add(unit, ipmc_index, flags, 1, rep_array);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_add: port %d vlan: %d \n", ports[i] ,vlan);
            return rv;
        }
    }

    return rv;
}


/* adding ports to VLAN flood MC group
 * In the ipmc flows hereby presented, flooding is determined by an MC group id which is in fact vsi
 */
int
add_ingress_multicast_flooding(
    int unit,
    int vlan,
    int *ports,
    int nof_ports)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_multicast_replication_t replications[8];
    int flags = 0;
    int ipmc_index = vlan;

    for (i=0; i < nof_ports; i++) {
        bcm_multicast_replication_t_init(&replications[i]);
        BCM_GPORT_LOCAL_SET(replications[i].port, ports[i]);
    }

    flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
    rv = bcm_multicast_set(unit, ipmc_index, flags, nof_ports, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_set\n");
        return rv;
    }

    return rv;
}

int
create_rif_vlan (
    bcm_mac_t mac_address,
    int unit,
    int vlan,
    int global_route_enable,
    bcm_l3_intf_t* intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;

    bcm_l3_intf_t_init(intf);
    sal_memcpy(intf->l3a_mac_addr, mac_address, 6);
    intf->l3a_vid = intf->l3a_vrf = vlan;

    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf = intf->l3a_vrf;
    if (global_route_enable) {
        ingress_intf.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf->l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}

/* Enable/Disable IPMC for RIF
 * For enabling, pass enable == 1
 * For disabling, pass enable == 0
*/
int enable_rif_ipmc (
    int unit,
    bcm_l3_intf_t* intf,
    int enable)
{
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


    rv = bcm_l3_ingress_create(unit, &ingress_intf, &(intf->l3a_intf_id));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}


int set_rif_ipmc_l2 (
    int unit,
    bcm_if_t intf,
    int enable)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;

    bcm_l3_ingress_t_init(&ingress_intf);

    rv = bcm_l3_ingress_get(unit, intf, &ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    if (enable) {
        ingress_intf.flags |= BCM_L3_INGRESS_L3_MCAST_L2;
    }
    else {
        ingress_intf.flags &= ~BCM_L3_INGRESS_L3_MCAST_L2;
    }

    ingress_intf.flags |= BCM_L3_INGRESS_WITH_ID;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}

/* In our context, this function configures one of the following two states:
 * 1. RIF.IPMC disabled: the function configures the lookup in LEM:
 *    In this case, we put bcmSwitchL3McastL2.
 *    if enabled == 0, we perform a lookup with <FID,DA>
 *    if enabled == 1, we perform a lookup with <FID,DIP>
 *
 * 2. RIF.IPMC enabled: the function configures the packet's forwarding following a miss in LEM and TCAM
 *    In this case, we put bcmSwitchUnknownIpmcAsMcast.
 *    if enabled == 0, the packet is L3 MC forwarded with default vrf.
 *    if enabled == 1, the packet is L2 flooded on the vsi.
 */
int
switch_control_set (
    int unit,
    bcm_switch_control_t switch_control,
    int enabled,
    bcm_if_t* intfs,
    int nof_intf)
{
    int rv;
    int i;

    if ((bcmSwitchL3McastL2 == switch_control) && is_device_or_above(unit, JERICHO2)) {
        /* bcmSwitchL3McastL2 is not supported in JR2 devices.
         * global L3McastL2 mode can no longer be configured, instead use per RIF option. */
        for (i = 0; i < nof_intf; i++) {
            rv = set_rif_ipmc_l2(unit,intfs[i],enabled);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_switch_control_set \n");
                return rv;
            }
        }
    } else {
        rv = bcm_switch_control_set(unit,switch_control,enabled);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set \n");
            return rv;
        }
    }

    return rv;
}

/* Creates a L2 forwarding entry. The entry will be accessed by LEM<FID,DA> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a destination MAC address. The packet will exit with vid == vlan.
 */
int
create_l2_forwarding_entry_da (
    int unit,
    bcm_mac_t mac,
    int l2mc_group,
    int vlan)
{

    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    /* Configurations relevant for LEM<FID,DA> lookup*/
    bcm_l2_addr_t_init(&l2_addr,mac,vlan);

    l2_addr.flags = BCM_L2_MCAST;
    l2_addr.l2mc_group = l2mc_group;

    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    return rv;
}

/* Creates a forwarding entry. src_ip == 0x0 implies L2 forwarding. The entry will be accessed by LEM<FID,DIP> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a (MC) destination ip.
 * src_ip > 0x0 implies L3 forwarding. The entry will be accessed by TCAM <RIF, SIP, DIP> Lookup                                   .
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip.                                                                                                                                 .
 * src_ip == -1 implies creation of entry without sip.
 * src_ip == 0 implies vid =0
 * mc_bridge == 1 => we can accept sip = 0 and vid != 0 (valid value after dont care value)                                                                                                                                                                                                              .
 */
int
create_forwarding_entry_dip_sip_inner (
    int unit,
    bcm_ip_t mc_ip,
    bcm_ip_t src_ip,
    int ipmc_index,
    int vlan,
    int mc_bridge,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    if ((src_ip != -1 && src_ip != 0) || mc_bridge) {
        data.s_ip_addr = src_ip;
        data.s_ip_mask = 0xffffffff;
        data.vid = vlan;
    }

    if (data.s_ip_addr == 0) {
        data.s_ip_mask = 0;
    }

    data.flags = flags;
    data.group = ipmc_index;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}

/* Don't use create_forwarding_entry_dip_sip() because it isn't working well as description */
int
create_forwarding_entry_dip_sip2 (
    int unit,
    bcm_ip_t mc_ip,
    bcm_ip_t src_ip,
    int ipmc_index,
    int vlan)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    data.s_ip_addr = src_ip;
    if ((src_ip != 0)) {
        data.s_ip_mask = 0xffffffff;
        data.vid = vlan;
    }

    data.flags = 0;
    data.group = ipmc_index;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip2 \n");
        return rv;
    }

    return rv;
}

/* Creates a L3 forwarding entry.
 * vrf != 0: The entry will be accessed by LEM<VRF,DIP> Lookup.
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip and vrf.                                                                                                                                  .
 * vrf == 0: The entry will be accessed following a miss in LEM<VRF,DIP> and TCAM<RIF,SIP,DIP> lookup and                                                                                                                                                                                                                                      .
 * bcmSwitchUnknownIpmcAsMcast == 0.                                                                                                                                                                                                                                                                                                                                             .
 */
int
create_forwarding_entry_vrf (
    int unit,
    bcm_ip_t mc_ip,
    int ipmc_index,
    int vrf,
    bcm_mac_t mac,
    int vlan,
    bcm_l3_intf_t* intf,
    bcm_if_t l3a_intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_host_t data;
    bcm_ipmc_addr_t info;

    bcm_l3_host_t_init(&data);
    bcm_ipmc_addr_t_init(&info);

    /* Create the entry */
    if (vrf != 0) {
        /* Instead of using the bcm_l3_host_add API to create a host,
         * the bcm_ipmc_add API should be used  providing <VRF, G, RIF> as key */
        if (is_device_or_above(unit,JERICHO2)) {
            info.vrf = intf->l3a_vrf;
            info.vid = intf->l3a_intf_id;
            info.mc_ip_addr = mc_ip;
            info.mc_ip_mask = 0xffffffff;
            info.flags = 0;
            info.group = ipmc_index;
            rv = bcm_ipmc_add(unit,&info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_ipmc_add \n");
                return rv;
            }
        } else {
            data.l3a_flags = 0x0;
            data.l3a_vrf = intf->l3a_vrf;
            data.l3a_intf = intf->l3a_intf_id;
            data.l3a_ip_addr = mc_ip;
            data.l3a_nexthop_mac = mac;
            BCM_GPORT_MCAST_SET(data.l3a_port_tgid, ipmc_index);
            rv = bcm_l3_host_add(unit, &data);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l3_host_add\n");
                return rv;
            }
        }
    }
    else{ /* vrf==0, so we create a default vrf forwarding*/
        if (is_device_or_above(unit,JERICHO)) {
            info.vid = 0;
        } else {
            info.vid = vlan;
        }
        /*Make sure that the MC address is valid, regardless of mask.*/
        info.mc_ip_addr = 0xE0000000;
        info.flags = 0x0;
        info.l3a_intf = l3a_intf;
        info.vrf = vrf;
        rv = bcm_ipmc_add(unit,&info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_ipmc_add\n");
            return rv;
        }
    }

    return rv;
}

/*
 * This cint is used to test the V4MC with IPMC disable case handling in per RIF configuration with the
 * BCM_L3_INGRESS_L3_MCAST_L2 flag and to make sure that the IPMC enable case is still working.
 *
 * Section 1: Create RIFs
 * Section 2: Creating MC groups
 * Section 3: Setting information relevant to MC forwarding, upon successful hits in LEM
 * Section 4: Defining entries for LEM which will forward the packet to the relevant MC group
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ipmc_flows.c
 * cint
 * ipmc_flows_ipmc_disabled_l3_mcast_l2_per_rif(0,200,201,202,203,4000,4001,4002,4003);
 * exit;
 *
 * /* ip multicast(IPoE) hit L3 interface 1 */
 * tx 1 psrc=200 data=0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 * /* ip multicast(IPoE) hit L3 interface 2 */
 * tx 1 psrc=200 data=0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 * /* ip multicast(IPoE) hit L3 interface 3 */
 * tx 1 psrc=200 data=0x01005e00010400020500000281000fa2080045000035000000008000990fc0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e000104000c0002000381000fa3080045000035000000007f009a0fc0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e000104000c0002000381000fa3080045000035000000007f009a0fc0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 */

int
ipmc_flows_ipmc_disabled_l3_mcast_l2_per_rif (
    int unit,
    int inP,
    int outP,
    int outP2,
    int outP3,
    int vlan1,
    int vlan2,
    int vlan3,
    int vlan4)
{

    int i,rv = BCM_E_NONE;
    bcm_l3_intf_t intf[4];
    bcm_l3_ingress_t ingress_intf[4];
    int vlans[4];
    bcm_mac_t  mac_addr[4];
    uint8 SwitchL3McastL2_on[4];
    uint8 disabel_ipmc[4];
    int is_adv_vt;

    ipmc_flows_init(unit, outP, outP2, outP3);
    rv = bcm_vlan_gport_add(unit, vlan1, inP, 0);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, vlan2, inP, 0);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, vlan3, inP, 0);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    /* Section 1:
     * creating interfaces
     */

    /*Interface 1*/
    vlans[0] = vlan1;
    sal_memcpy(mac_addr[0], mac_address_1, 6);
    SwitchL3McastL2_on[0] = 1;
    disabel_ipmc[0] = 1;

    /*Interface 2*/
    vlans[1] = vlan2;
    sal_memcpy(mac_addr[1], mac_address_2, 6);
    SwitchL3McastL2_on[1] = 0;
    disabel_ipmc[1] = 1;

    /*Interface 3*/
    vlans[2] = vlan3;
    sal_memcpy(mac_addr[2], mac_address_3, 6);
    SwitchL3McastL2_on[2] = 0;
    disabel_ipmc[2] = 0;

    /*Interface 4*/
    vlans[3] = vlan4;
    sal_memcpy(mac_addr[3], mac_address_4, 6);
    SwitchL3McastL2_on[3] = 0;
    disabel_ipmc[3] = 0;

    is_adv_vt = is_advanced_vlan_translation_mode(unit);

    for(i = 0; i < 4; i++)
    {
        bcm_l3_intf_t_init(&intf[i]);

        sal_memcpy(intf[i].l3a_mac_addr, mac_addr[i], 6);
        intf[i].l3a_vrf = vlans[i];
        intf[i].l3a_vid = vlans[i];

        rv = bcm_l3_intf_create(unit, intf[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_create");
        }

        bcm_l3_ingress_t_init(&ingress_intf[i]);

        /*disable IPMC*/
        ingress_intf[i].flags = BCM_L3_INGRESS_WITH_ID;

        if(disabel_ipmc[i] == 1)
        {
            ingress_intf[i].flags |=  BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
        }
        else
        {
            ingress_intf[i].flags |=  BCM_L3_INGRESS_GLOBAL_ROUTE;
        }

        if(SwitchL3McastL2_on[i] == 1)
        {
            ingress_intf[i].flags |=  BCM_L3_INGRESS_L3_MCAST_L2;
        }

        rv = bcm_l3_ingress_create(unit, &ingress_intf[i], &intf[i].l3a_intf_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_create \n");
        }
    }


    /* Section 2:
     * creating MC groups
     */

    /* Case 1 - MC group with  ipmc_index */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0]));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Case 2 - L2 MC Group with l2mc_group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1]));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Case 3 - MC group with  ipmc_index */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[2]));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Section 3:
     * Setting information relevant to MC forwarding, upon successful hits in LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM: <FID(15), G(28)> lookup */
    rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.forwarding_ports_1, 3, vlans[0], is_adv_vt);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap \n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM: <FID, DA> lookup */
    rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[1], ipmc_flows_info1.forwarding_ports_2, 2, vlans[1], is_adv_vt);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap \n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<vrf,dip> lookup */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[2],ipmc_flows_info1.forwarding_ports_3, 2 ,intf[3].l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* Section 4:
     * defining entries for LEM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip_inner(unit,ipmc_flows_info1.mc_ip_arr[0],0x0,ipmc_flows_info1.ipmc_mc_groups[0],vlans[0],1,BCM_IPMC_L2);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip\n");
        return rv;
    }

    rv = create_l2_forwarding_entry_da(unit,mac1,ipmc_flows_info1.ipmc_mc_groups[1],vlans[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, create_l2_forwarding_entry_da\n");
        return rv;
    }

    rv = create_forwarding_entry_dip_sip_inner(unit,ipmc_flows_info1.mc_ip_arr[2],ipmc_flows_info1.src_ip_arr[2],ipmc_flows_info1.ipmc_mc_groups[2],vlans[2],1,0);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }

    return rv;
}

/*
 *  This function exhibits the usage of an ipmc flow that is determined by the unsetting of the RIF IPMC. This flow includes four sub-flows,
 *  which are employed by different sections in the test.
 *
 *  Section 1: Disables RIF IPMC to enable L2 MC forwarding.
 *  Section 2: Creating a vsi to enable VLAN MC flooding.
 *  Section 3: Creating MC groups: first two groups are the destination of L2 MC forwarding. The other two are the destination of VLAN MC flooding.
 *  Section 4: Sets information for MC forwarding upon successful hit in LEM. The entries are built in Section 7.
 *  Section 5: Sets information for VLAN MC flooding upon miss in LEM. In these cases, packets will be sent with a dip and/or dst_mac that won't be matched in LEM.
 *  Section 6: Configures the lookup in LEM: bcmSwitchL3McastL2 == 1 leads to <FID,DIP> lookup, while bcmSwitchL3McastL2 == 0 leads to <FID,DA> lookup.
 *  Section 7: Configuring the LEM entries that will be matched with (Multicast compatible) dip or dst_mac.
 *
 *  We present the following sub-flows:
 *
 *  1. bcmSwitchL3McastL2 == 1 && lookup in LEM<FID,DIP> is successful: packet is sent with dip that is matched to the entry. Upon this successful hit,
 *  the packet will be forwarded according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3 with vid == vlan1.
 *  Relevant sections: 1.1, 3.1, 4.1, 6, 7.1.
 *
 *  2. bcmSwitchL3McastL2 == 0 && lookup in LEM<FID,DA> is successful: packet is sent with dst_mac that is matched to the entry. Upon this successful hit,
 *  the packet will be forwarded according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3 with vid == vlan2.
 *  Relevant sections: 1.2, 3.2, 4.2, 6, 7.2.
 *
 *  3. bcmSwitchL3McastL2 == 1 && lookup in LEM<FID,DIP> is unsuccessful: packet is sent with dip that is unmatched to the entry. To reproduce this
 *  scenario, simply send a packet with a different mc_ip than the one  configured in the function. Upon this miss,the packet will be flooded according
 *  to the VLAN flood MC group created in this function (vlan == vlan3). It will arrive at ports outP, outP2 with vid == vlan3.
 *  Relevant sections: 2.1, 3.3, 5.1, 6.
 *
 *  4. bcmSwitchL3McastL2 == 0 && lookup in LEM<FID,DA> is unsuccessful: packet is sent with dst_mac that is unmatched to the entry. To reproduce this
 *  scenario, simply send a packet with a different mac than the one configured in the function .Upon this miss,
 *  the packet will be flooded according to the VLAN flood MC group created in this function (vlan == vlan4). It will arrive at ports outP, outP2 with vid == vlan4.
 *  Relevant sections: 2.2, 3.4, 5.2, 6.
 *
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ipmc_flows.c
 * cint
 * ipmc_flows_rif_ipmc_disabled(0,200,201,202,203,4000,4001,4002,4003);
 * exit;
 *
 * /* ip multicast(IPoE) - lookup in LEM<FID,DIP> successfully */
 * tx 1 psrc=200 data=0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000350000000080009913c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * set_rif_ipmc_l2(0,4000,0);
 * exit;
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * set_rif_ipmc_l2(0,4001,0);
 * exit;
 *
 * /* ip multicast(IPoE) - lookup in LEM<FID,DA> successfully */
 * tx 1 psrc=200 data=0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000350000000080009911c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * set_rif_ipmc_l2(0,4000,1);
 * exit;
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * set_rif_ipmc_l2(0,4001,1);
 * exit;
 *
 * /* ip multicast(IPoE) - lookup in LEM<FID,DIP> unsuccessfully */
 * tx 1 psrc=200 data=0x01005e00010400020500000281000fa2080045000035000000008000990fc0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010400020500000281000fa281000fa2080045000035000000008000990fc0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010400020500000281000fa281000fa2080045000035000000008000990fc0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * set_rif_ipmc_l2(0,4000,0);
 * exit;
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * set_rif_ipmc_l2(0,4001,0);
 * exit;
 *
 * /* ip multicast(IPoE) - lookup in LEM<FID,DA> unsuccessfully */
 * tx 1 psrc=200 data=0x01005e00010500020500000381000fa3080045000035000000008000990dc0a8000ee0000105000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010500020500000381000fa381000fa3080045000035000000008000990dc0a8000ee0000105000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010500020500000381000fa381000fa3080045000035000000008000990dc0a8000ee0000105000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 */
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * ipmc_flows_destroy(0,6);
 * exit;
 */

int ipmc_ssm_enable = 0;

int ipmc_flows_rif_ipmc_disabled (
    int unit,
    int inP,
    int outP,
    int outP2,
    int outP3,
    int vlan1,
    int vlan2,
    int vlan3,
    int vlan4)
{

    int rv = BCM_E_NONE;
    int SwitchL3McastL2_enabled = 1;
    int flags;
    bcm_if_t intfs[2];
    bcm_l3_intf_t intf1, intf2;
    int advanced_vlan_translation_mode = is_advanced_vlan_translation_mode(unit);
    int L2_flag;

    ipmc_flows_info1.ipmc_indices[6] = vlan3;
    ipmc_flows_info1.ipmc_indices[7] = vlan4;

    ipmc_flows_init(unit, outP, outP2, outP3);

    if (is_device_or_above(unit, JERICHO2)) {
        /* On JR2, Soc ipmc_set_entry_type_by_rif is not supported */
        L2_flag = BCM_IPMC_L2;
    } else {
        /* In case the ipmc_set_entry_type_by_rif is set the IPMC entry L2/L3 will be determine be the RIF IPMC state,
         * otherwise it will be set by the BCM_IPMC_L2 flag */
        L2_flag = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_set_entry_type_by_rif", 0) == 1) ? 0 : BCM_IPMC_L2;
    }

    /* Section 1
     * create a rif, then disable ipmc
     */
    rv = create_rif_vlan(mac_address_1, unit , vlan1, 1/*global_route*/, &intf1);/* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_2, unit , vlan2, 1/*global_route*/, &intf2);/* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf1, 0); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf2, 0); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }


    /* Section 2:
     * creating a vsi; will be used as a parameter for L2 flooding
     */
    rv = bcm_vswitch_create_with_id(unit, vlan3); /* 2.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vlan4); /* 2.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }


    /* Section 3:
     * creating MC groups
     */

    /* Create the L2 MC Group with ipmc_index */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0])); /* 3.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    if (ipmc_ssm_enable) {
        /* Create the L2 MC Group with ipmc_index */
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1])); /* 3.1.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[2])); /* 3.1.2 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
    }

    if (ipmc_egress_multicast_enable == 1) {
        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;

        if (is_device_or_above(unit, JERICHO2)) {
            /* Create fabric mc for replicators to egress MC*/
            rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, ipmc_flows_info1.ipmc_mc_groups[5], ipmc_flows_info1.ipmc_mc_groups[5]);
            if (rv != BCM_E_NONE) {
                printf("Error, multicast__open_ingress_mc_group_for_egress_mc \n");
                return rv;
            }
        }
    } else{
        flags = BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID;
    }
    /* Create the L2 MC Group with l2mc_group */
    rv = bcm_multicast_create(unit, flags, &(ipmc_flows_info1.ipmc_mc_groups[5])); /* 3.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan3 */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID, &vlan3); /* 3.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan4 */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID, &vlan4); /* 3.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }


    /* Section 4:
     * Setting inforamtion relevant to MC forwarding, upon successful hits in LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<FID,DIP> lookup */
    rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.forwarding_ports_1, 3, vlan1,advanced_vlan_translation_mode); /* 4.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap \n");
        return rv;
    }

    if (ipmc_ssm_enable) {
        /* set information relevant to MC forwarding: ipmc_index will connect the MC group to <FID,DIP,SIP> lookup */
        rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[1], ipmc_flows_info1.forwarding_ports_1, 3, vlan2,advanced_vlan_translation_mode); /* 4.1.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, add_ingress_multicast_forwarding_with_encap \n");
            return rv;
        }
    }

    /* set information relevant to MC forwarding: l2mc_group will connect the MC group to LEM<FID,DA> lookup*/
    if (ipmc_egress_multicast_enable == 1) {
       rv = add_egress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[5], ipmc_flows_info1.forwarding_ports_1, 3, vlan2); /* 4.2 */
    } else{
       rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[5], ipmc_flows_info1.forwarding_ports_1, 3, vlan2, advanced_vlan_translation_mode); /* 4.2 */
    }
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap/add_egress_multicast_forwarding \n");
        return rv;
    }


    /* Section 5:
     * Setting inforamtion relevant to VLAN MC flooding, upon misses in LEM
     */

    /* set information relevant to MC flooding with vid == vlan3*/
    rv = add_ingress_multicast_flooding(unit, vlan3, ipmc_flows_info1.forwarding_ports_2, 2); /* 5.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }

    /* set information relevant to MC flooding with vid == vlan4*/
    rv = add_ingress_multicast_flooding(unit, vlan4,  ipmc_flows_info1.forwarding_ports_2, 2); /* 5.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }


    /* Section 6:
     * Configuring the lookup in LEM:
     * if SwitchL3McastL2_enabled == 0, we perform a lookup with <FID,DA>
     * if SwitchL3McastL2_enabled == 1, we perform a lookup with <FID,DIP>
    */
    intfs[0] = intf1.l3a_intf_id;
    intfs[1] = intf2.l3a_intf_id;
    rv = switch_control_set(unit, bcmSwitchL3McastL2, SwitchL3McastL2_enabled, intfs, 2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    /* Section 7:
     * defining entries for LEM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], 0x0, ipmc_flows_info1.ipmc_mc_groups[0], vlan1,1,L2_flag); /* 7.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip\n");
        return rv;
    }

    if (ipmc_ssm_enable) {
        /* configure entry to <FID,DIP,SIP>*/
        rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], 0xc0a8000f/*sip*/ ,ipmc_flows_info1.ipmc_mc_groups[1], vlan1,1,L2_flag); /* 7.1.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
        rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], 0xc0a8000e/*sip*/ ,ipmc_flows_info1.ipmc_mc_groups[2], vlan1,1,L2_flag); /* 7.1.2 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
    }

    rv = create_l2_forwarding_entry_da(unit, mac1, ipmc_flows_info1.ipmc_mc_groups[5], vlan2); /* 7.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_l2_forwarding_entry_da\n");
        return rv;
    }

    return rv;
}

/*
 *  This function exhibits the usage of an ipmc flow that is determined by the setting of the RIF IPMC. This flow includes six sub-flows,
 *  which are employed by different sections in the test.
 *
 *  Section 1: Enables RIF IPMC to enable MC forwarding.
 *  Section 2: Creating a vsi to enable VLAN MC flooding.
 *  Section 3: Creating MC groups: first four groups are the destination of L3 MC forwarding.
 *             The other two are the destination of VLAN MC flooding and L3 forwarding with default VRF.
 *  Section 4: Sets information for MC forwarding upon successful hit in LEM and/or LPM. The entries are built in Section 7.
 *  Section 5: Sets information for VLAN MC flooding upon miss in LEM and LPM or L3 forwarding with default vrf.
 *             In these cases, packets will be sent with a dip that won't be matched in LEM and TCAM.
 *  Section 6: (Jer1 only, bcmSwitchUnknownIpmcAsMcast is not supported by JR2)
 *             Configures the sub flow after a miss in LEM and/or LPM:
 *             bcmSwitchUnknownIpmcAsMcast == 1 leads to flooding on vsi, while bcmSwitchUnknownIpmcAsMcast == 0
 *             leads to L3 MC forwarding with default vrf.
 *  Section 7: Configuring the entries that will be matched with (Multicast compatible) dip (and/or sip in LPM).
 *             This also includes forwarding with default vrf.
 *
 *  We present the following sub-flows:
 *
 *  1. If soc property ipmc_vpn_lookup_enable == 1,
 *      Packet arrives with a dip that matches a LEM entry. The packet will be forwarded, upon the successful hit,
 *      according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3.
 *      Otherwise(ipmc_vpn_lookup_enable == 0), Packet arrives with sip + dip that matches a LEM entry.
 *      The packet will be forwarded, upon the successful hit, according to the multicast group created in this function.
 *      It will arrive at ports outP, outP2, outP3.
 *    Relevant sections: 1.1, 3.1, 4.1, 7.4.
 *
 *  2. Packet arrives with a dip and sip that matches a LPM entry, configured with DIP + SIP. The packet will be forwarded, upon the successful hit,
 *   according to the multicast group created in this function. It will arrive at ports outP2, outP3.
 *   Relevant sections: 1.2, 3.2, 4.2, 7.1.
 *
 *  3. Packet arrives with only a dip that matches a LPM entry . The packet will be forwarded, upon the successful hit,
 *   according to the multicast group created in this function. It will arrive at ports outP, outP2.
 *   Relevant sections: 1.3, 3.3, 4.3, 7.2.
 *
 *  4. If soc property ipmc_vpn_lookup_enable == 1,
 *      Packet arrives with a dip that matches both LEM and LPM entries . The packet will be forwarded, upon the successful hit,
 *      according to the multicast group that is defined in the LPM entry. It will arrive at ports outP, outP3.
 *      Otherwise(ipmc_vpn_lookup_enable == 0), Packet arrives with a dip that matches LPM entries, and sip+dip that matches LEM entries.
 *      Because LPM > LEM in priority in case both match, the packet will be forwarded, upon the successful hit
 *      according to the multicast group that is defined in the LPM entry.It will arrive at ports outP, outP3.
 *    Relevant sections: 1.4, 3.4, 4.4, 7.3.
 *
 *  5. (Jer1 only) Packet arrives with a dip that is unmatched in the LEM and LPM . Upon setting bcmSwitchUnknownIpmcAsMcast == 1,
 *  The packet will be L2 flooded on the given vsi (vlan). It will arrive at ports outP, outP2.
 *  sections: 2.1, 3.5, 5.1, 6.1.
 *
 *  6. (Jer1 only) Packet arrives with a dip that is unmatched in the LEM and LPM . Upon setting bcmSwitchUnknownIpmcAsMcast == 0,
 *  The packet will be L3 forwarded on the given vrf. It will arrive at ports outP, outP2.
 *  sections: 1.5, 3.6, 6.1 (unset bcmSwitchUnknownIpmcAsMcast), 7.5.
 *
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_ipmc_flows.c
 * cint
 * ipmc_flows_rif_ipmc_enabled(0,200,201,202,203,4000,4001,4002,4003,4009,4005);
 * exit;
 *
 * /* ip multicast(IPoE) */
 * tx 1 psrc=200 data=0x01005e0001020002050000000800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa00800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 *
 * /* ip multicast(IPoE) */
 * tx 1 psrc=200 data=0x01005e0001030002050000010800450000b60000000040ffd793c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000b60000000040ffd793c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa10800450000b60000000040ffd793c0a8000ce0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 *
 * /* ip multicast(IPoE) */
 * tx 1 psrc=200 data=0x01005e0001030002050000010800450000b60000000040ffd793c0a8000de0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa20800450000b60000000040ffd793c0a8000de0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010300020500000181000fa20800450000b60000000040ffd793c0a8000de0000103000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 *
 * /* ip multicast(IPoE) */
 * tx 1 psrc=200 data=0x01005e0001020002050000000800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa30800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010200020500000081000fa30800450000b60000000040ffd793c0a8000be0000102000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 *
 * /* ip multicast(IPoE) */
 * tx 1 psrc=200 data=0x01005e0001040002050000020800450000b60000000040ffd793c0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010400020500000281000fa50800450000b60000000040ffd793c0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010400020500000281000fa50800450000b60000000040ffd793c0a8000de0000104000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * switch_control_set(0,246,0,0,0);
 * exit;
 *
 * /* ip multicast(IPoE) */
 * tx 1 psrc=200 data=0x01005e0001040002050000020800450000b60000000040ffd793c0a8000ee0000105000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000
 *
 * /* Received packets on unit 0 should be: */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010400020500000281000fa90800450000b60000000040ffd793c0a8000ee0000105000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 * /* Source port: 0, Destination port: 0 */
 * /* Data: 0x01005e00010400020500000281000fa90800450000b60000000040ffd793c0a8000ee0000105000102030405060708090a0b0c0d0e0f101112131415161718191a1aaaaa0000000000000000000000000000000001000002000000000000000000000000 */
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_ipmc_flows.c
 * cint
 * ipmc_flows_destroy(0,6);
 * exit;
 */
int
ipmc_flows_rif_ipmc_enabled (
    int unit,
    int inP,
    int outP,
    int outP2,
    int outP3,
    int rif1,
    int rif2,
    int rif3,
    int rif4,
    int rif5,
    int vlan)
{
    int rv = BCM_E_NONE;
    int SwitchUnknownIpmcAsMcast_enabled = 1;

    bcm_l3_intf_t intf1, intf2, intf3, intf4, intf5;
    bcm_vrf_t vrf = 2;

    ipmc_flows_info1.ipmc_indices[6] = vlan;
    ipmc_vpn_lookup_enable = (is_device_or_above(unit, JERICHO2)) ? 1 : soc_property_get(unit , "ipmc_vpn_lookup_enable", 1);

    ipmc_flows_init(unit, outP, outP2, outP3);

    /* Section 1:
     * create a rif, then enable ipmc
     */
    rv = create_rif_vlan(mac_address_1, unit, rif1, 1/*global_route*/, &intf1); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_2, unit, rif2, 1/*global_route*/, &intf2); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_3, unit, rif3, 1/*global_route*/, &intf3); /* 1.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_4, unit, rif4, 1/*global_route*/, &intf4); /* 1.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_5, unit, rif5, 1/*global_route*/, &intf5); /* 1.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    /* Section 2:
     * creating a vsi; will be used as a parameter for L2 flooding
     */
    rv = bcm_vswitch_create_with_id(unit, vlan); /* 2.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }

    /* Section 3:
     * creating MC groups
     */

    /* In the following 4 calls we create the L3 MC Group with an index taken from the main struct's data structure */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0])); /* 3.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1])); /* 3.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[2])); /* 3.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[3])); /* 3.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &vlan); /* 3.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L3 MC Group with default vrf */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[4])); /* 3.6 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Section 4:
     * Setting inforamtion relevant to L3 MC forwarding, upon successful hits in LPM and/or LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<vrf,dip> lookup */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[0],ipmc_flows_info1.forwarding_ports_1, 3 ,intf1.l3a_intf_id); /* 4.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }
    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LPM<rif,sip,dip> lookup with sip + dip given */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[1],ipmc_flows_info1.forwarding_ports_2, 2 ,intf2.l3a_intf_id); /* 4.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LPM<rif,sip,dip> lookup with only dip given */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[2],ipmc_flows_info1.forwarding_ports_3, 2 ,intf3.l3a_intf_id); /* 4.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LPM<rif,sip,dip>, LEM<vrf,dip> lookup. LPM is chosen. */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[3],ipmc_flows_info1.forwarding_ports_4, 2 ,intf4.l3a_intf_id); /* 4.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* Section 5:
     * Setting inforamtion relevant to VLAN MC flooding, upon misses in LEM and TCAM
     */

    /* set information relevant to MC flooding with vid == vlan*/
    rv = add_ingress_multicast_flooding(unit, vlan, ipmc_flows_info1.forwarding_ports_2, 2); /* 5.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }

    /* set information relevant to L3 MC forwarding with default vrf,  vid == rif5*/
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[4],ipmc_flows_info1.forwarding_ports_2, 2 ,intf5.l3a_intf_id); /* 5.2 */

    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* bcmSwitchUnknownIpmcAsMcast is not supported by JR2 */
        /* Section 6:
         * Configuring the sub flow taken after a miss in LEM and TCAM:
         * if bcmSwitchUnknownIpmcAsMcast == 1, the packet will be flooded on vsi.
         * if bcmSwitchUnknownIpmcAsMcast == 0, the packet will be L3 MC forwarded with default vrf.
         */
        rv = bcm_switch_control_set(unit, bcmSwitchUnknownIpmcAsMcast, SwitchUnknownIpmcAsMcast_enabled); /* 6.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set \n");
            return rv;
        }
    }

    /* Section 7:
     * defining entries for LEM and LPM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip_inner(unit,ipmc_flows_info1.mc_ip_arr[1],ipmc_flows_info1.src_ip_arr[1],ipmc_flows_info1.ipmc_mc_groups[1],rif2,0,0); /* 7.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip_inner, step - 7.1\n");
        return rv;
    }

    rv = create_forwarding_entry_dip_sip_inner(unit,ipmc_flows_info1.mc_ip_arr[1],-1,ipmc_flows_info1.ipmc_mc_groups[2],rif3,0,0); /* 7.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip_inner, step - 7.2\n");
        return rv;
    }

    if (ipmc_vpn_lookup_enable) {
        rv = create_forwarding_entry_dip_sip_inner(unit,ipmc_flows_info1.mc_ip_arr[0],ipmc_flows_info1.src_ip_arr[0],ipmc_flows_info1.ipmc_mc_groups[3],rif4,0,0); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip_inner, step - 7.3\n");
            return rv;
        }

        rv = create_forwarding_entry_vrf(unit, ipmc_flows_info1.mc_ip_arr[0],ipmc_flows_info1.ipmc_mc_groups[0], vrf, mac2, rif1, &intf1, 0); /* 7.4 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_vrf, step - 7.4\n");
            return rv;
        }
    } else {
        /* Add a none-exact match IPMC entry in TCAM */
        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[3],0,ipmc_flows_info1.ipmc_mc_groups[3], rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3.1\n");
            return rv;
        }
        /* Add a exact match IPMC entry in LEM */
        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[3],ipmc_flows_info1.src_ip_arr[3],ipmc_flows_info1.ipmc_mc_groups[0],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3.2\n");
            return rv;
        }

        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[2],ipmc_flows_info1.src_ip_arr[2],ipmc_flows_info1.ipmc_mc_groups[0],rif1); /* 7.4 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.4\n");
            return rv;
        }
    }

    bcm_gport_t mc_gport;
    sand_utils_l3_fec_s l3_fec;
    sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
    BCM_GPORT_MCAST_SET(mc_gport, ipmc_flows_info1.ipmc_mc_groups[4]);
    l3_fec.destination = mc_gport;
    l3_fec.tunnel_gport = rif5;
    l3_fec.tunnel_gport2 = 0;
    rv = sand_utils_l3_fec_create(unit, &l3_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    rv = create_forwarding_entry_vrf(unit, 0, ipmc_flows_info1.ipmc_mc_groups[4],0, mac2, rif5, NULL, l3_fec.l3eg_fec_id);  /* 7.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_vrf\n");
        return rv;
    }

    return rv;
}

/*
 * This cint is used to test the V4MC with IPMC enable case handling in per RIF configuration
 * and to make sure that the IPMC disable case is forward by L2 MC after deleting RIF.
 *
 * Section 1: Create RIFs
 * Section 2: Creating MC groups
 * Section 3: Setting information relevant to MC forwarding, upon successful hits in LEM
 * Section 4: Defining entries for LEM which will forward the packet to the relevant MC group
*/
int
ipmc_flows_ipmc_per_rif (
    int unit,
    int inP,
    int outP,
    int outP2,
    int outP3,
    int vlan)
{

    int rv = BCM_E_NONE;
    bcm_l3_intf_t intf;
    int is_adv_vt = is_advanced_vlan_translation_mode(unit);

    ipmc_flows_init(unit, outP, outP2, outP3);
    rv = bcm_vlan_gport_add(unit, vlan, inP, 0);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    /* Section 1:
         * create a rif, then enable ipmc
         */
    rv = create_rif_vlan(mac_address_1, unit, vlan, 1/*global_route*/, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan()\n");
        return rv;
    }

    /* Section 2:
         * creating MC groups
         */

    /* Case 1 - MC group with  ipmc_index */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2) ? 0 :BCM_MULTICAST_TYPE_L3) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0]));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create with ipmc_index \n");
        return rv;
    }

    /* Case 2 - L2 MC Group with l2mc_group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2) ? 0: BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1]));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create() with l2mc_group \n");
        return rv;
    }

    /* Section 3:
     * Setting information relevant to MC forwarding, upon successful hits in LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<vrf,dip> lookup */
    rv = add_l3_interface_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.forwarding_ports_1, 3, intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM: <FID, DA> lookup */
    rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[1], ipmc_flows_info1.forwarding_ports_2, 2, vlan, is_adv_vt);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap \n");
        return rv;
    }

    /* Section 4:
     * defining entries for LEM which will forward the packet to the relevant mc group
     */

    rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], ipmc_flows_info1.src_ip_arr[0], ipmc_flows_info1.ipmc_mc_groups[0], vlan, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip_inner()\n");
        return rv;
    }

    rv = create_l2_forwarding_entry_da(unit, mac1, ipmc_flows_info1.ipmc_mc_groups[1], vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, create_l2_forwarding_entry_da()\n");
        return rv;
    }

    return rv;
}

