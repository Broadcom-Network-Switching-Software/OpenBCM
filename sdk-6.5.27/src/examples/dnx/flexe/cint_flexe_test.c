/* CINT to verify FlexE related funtions
 *
 * Please load below CINT flies:
 * cd $SDK/src/examples/dnx
 * cint ./port/cint_dynamic_port_add_remove.c
 * cint ./cint_flexe_test.c
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint ../sand/utility/cint_sand_utils_multicast.c
 * cint ../sand/cint_sand_tdm_examples.c
 */

/* FlexE PHY port array for FlexE group creation */
bcm_port_t flexe_phy_ports[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Logical PHY ID for each FlexE PHY in flexe_phy_ports[] */
int logical_phy_ids[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Global failover channel ID for FlexE flow set
 * It will be used by below functions in datapath creation:
 * - dnx_flexe_util_L1_datapath_xx
 * - dnx_flexe_util_distributed_L2_L3_datapath_xx
 * Need reset it to 0 if needn't failover anymore
 */
int failover_channel_id = 0;

/* If below flag is set, function dnx_flexe_util_group_create() will skip synce setting */
int skip_synce_in_grp_creation = 0;

/* Client Rx speed. It is used by dnx_flexe_util_client_speed_set() when flag BCM_PORT_RESOURCE_ASYMMETRICAL is set */
int client_rx_speed = 0;

/* Flag to enable or disable link traning in FlexE PHY creation */
int phy_cl72_enable = 1;

/* Flag to enable FlexE bypass mode */
int flexe_bypass_en = 0;

int ilkn_interleave_en = 0;

/* Parameters in FlexE_GEN2 datapath building */
int flexe_gen2_fap0_id = 0; /* FAP0 unit id in FlexE_Gen2 regression system */
int flexe_gen2_tdm_is_mc = 0; /* Use TDM MC configuration or not */
int flexe_gen2_tdm_grp_id = 5000;
int flexe_gen2_tdm_min_size = 64;
int flexe_gen2_tdm_max_size = 256;
bcm_port_flexe_phy_slot_mode_t flexe_gen2_grp_phy_slot_mode = bcmPortFlexePhySlot5G;

/*
 * Slots assignment for calendar A and calendar B
 * The array indicator is the slot location
 * The array value is relevant FlexE client ID
 * The 2 arrays will be used by funtion 'dnx_flexe_cint_calendar_slots_set'
 */
int Cal_Slots_A[1600]; /* FlexE_Gen1 uses the first 80 elements */
int Cal_Slots_B[1600]; /* FlexE_Gen1 uses the first 80 elements */

bcm_port_flexe_group_cal_t FLEXE_CAL_A = bcmPortFlexeGroupCalA;
bcm_port_flexe_group_cal_t FLEXE_CAL_B = bcmPortFlexeGroupCalB;

typedef enum flexe_port_type_t
{
    port_type_busA,
    port_type_busB_L1,
    port_type_busB_L2_L3,
    port_type_busC,
    port_type_eth_L1,
    port_type_eth_L2_L3,
    port_type_ilkn_L1,
    port_type_ilkn_L2_L3,
    port_type_1588_L1,
    port_type_1588_L2,
    port_type_oam_L1,
    port_type_oam_L2,
    port_type_L1_monitor,
    port_type_L2_mgmt_oam,
    port_type_L2_mgmt_ptp,
    port_type_protection,
    port_type_sar_otn,
    port_type_unknown
};

/* FlexE_Gen2 only supports BusB L2 client */
flexe_port_type_t port_type_busB = port_type_busB_L2_L3;

/* Check if FlexE PHY is in rx invalid status or not */
int dnx_flexe_rx_in_invalid_status(int unit) {
    int rx_valid_flag;

    rx_valid_flag = *(dnxc_data_get(unit, "nif", "flexe", "pcs_port_enable_support", NULL));

    return !rx_valid_flag;
}

int dnx_flexe_gen2_support(int unit, int is_adapter) {
    int support_flag = 0;
    int flexe_core_speed = 0;
    int is_flexe_gen2 = 0;
    int delay_exist = 0;

    flexe_core_speed = *(dnxc_data_get(unit, "nif", "flexe", "max_flexe_core_speed", NULL));
    is_flexe_gen2 = flexe_core_speed == 1600000;
    delay_exist = *(dnxc_data_get(unit, "device", "general", "delay_exist", NULL));

    if (is_flexe_gen2) {
        if (is_adapter != 0 && is_adapter != 1) {
            /* Don't care it is BCM-Adapter or not */
            support_flag = 1;
        } else {
            /* Check it is BCM-Adapter or not */
            support_flag = is_adapter == delay_exist;
        }
    }

    return support_flag;
}

/* Global variable to indicate if the CDU lane speed is reduced speed(25G) or not */
int is_reduced_cdu_lane_speed = 0;
void dnx_flexe_lane_speed_init(int unit0, int unit1) {
    int i, is_pam4_speed_supported;
    int units[2] = {unit0, unit1};

    is_reduced_cdu_lane_speed = 0;

    /* Check CDU lane speed on the 2 units */
    for (i = 0; i < 2; i++) {
        if (units[i] < 0) {continue;}

        /* Set is_reduced_cdu_lane_speed if any unit doesn't support pam4_speed(50G) */
        is_pam4_speed_supported = *(dnxc_data_get(units[i], "nif", "phys", "is_pam4_speed_supported", NULL));
        if (!is_pam4_speed_supported) {
            is_reduced_cdu_lane_speed = 1;
            break;
        }
    }
}

/* Function to clear slot array Cal_Slots_A[] or Cal_Slots_B[] */
int dnx_flexe_util_cal_slots_clear(bcm_port_flexe_group_cal_t cal_id)
{
    if (cal_id == bcmPortFlexeGroupCalA)
    {
        /* Reset all items in Cal_Slots_A[] to 0 */
        sal_memset(Cal_Slots_A, 0, sizeof(Cal_Slots_A));
    }
    else
    {
        /* Reset all items in Cal_Slots_B[] to 0 */
        sal_memset(Cal_Slots_B, 0, sizeof(Cal_Slots_B));
    }

    return BCM_E_NONE;
}

/* Function to verify if the provided port/client matches the expected port type or not */
int dnx_flexe_util_port_type_verify(
    int unit,
    bcm_port_t port_id,
    flexe_port_type_t port_type_exp)
{
    int L1_flag;
    uint32 flags_get;
    bcm_port_if_t if_type;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    flexe_port_type_t port_type_get = port_type_unknown;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_port_type_verify";

    if (dnx_flexe_gen2_support(unit, -1) && port_type_exp == port_type_busB_L1) {
        printf("**** %s: Warning - FlexE_Gen2 doesn't support BusB_L1 client. Quit. ****\n", proc_name);
        return BCM_E_NONE;
    }

    /* Get port setting */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    snprintf(error_msg, sizeof(error_msg), "(port %d) ****", port_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port_id, &flags_get, &interface_info, &mapping_info), error_msg);

    /* Parse the port type */
    L1_flag = flags_get & BCM_PORT_ADD_CROSS_CONNECT;
    if_type = interface_info.interface;
    if ((if_type == BCM_PORT_IF_NIF_ETH) && L1_flag)
    {
        /* Ethernet port used for L1 switch */
        port_type_get = port_type_eth_L1;
    }
    else if (if_type == BCM_PORT_IF_NIF_ETH)
    {
        /* Ethernet port used for L2&L3 switch */
        port_type_get = port_type_eth_L2_L3;
    }
    else if (if_type == BCM_PORT_IF_CPU)
    {
        /* CPU port used for L2&L3 switch */
        port_type_get = port_type_eth_L2_L3;
    }
    else if ((if_type == BCM_PORT_IF_ILKN) && L1_flag)
    {
        /* ILKN port used for L1 switch */
        port_type_get = port_type_ilkn_L1;
    }
    else if (if_type == BCM_PORT_IF_ILKN)
    {
        /* ILKN port used for L2&L3 switch */
        port_type_get = port_type_ilkn_L2_L3;
    }
    else if (if_type == BCM_PORT_IF_FLEXE_CLIENT)
    {
        /* FlexE client in BusA */
        port_type_get = port_type_busA;
    }
    else if (if_type == BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT)
    {
        if ((flags_get & BCM_PORT_ADD_FLEXE_MAC) && L1_flag)
        {
            /* FlexE client in BusB for L1 switch */
            port_type_get = port_type_busB_L1;
        }
        else if (flags_get & BCM_PORT_ADD_FLEXE_MAC)
        {
            /* FlexE client in BusB for L2&L3 switch */
            port_type_get = port_type_busB_L2_L3;
        }
        else if (flags_get & BCM_PORT_ADD_FLEXE_SAR)
        {
            /* FlexE client in BusC */
            port_type_get = port_type_busC;
        }
    }

    /* Compare the port type with expected type */
    if (port_type_get != port_type_exp)
    {
        /* printf("**** Port type %d mismathes with expected type %d(port %d, flags %d, type %d) ****\n", port_type_get&0xFF, port_type_exp&0xFF, port_id, flags_get, if_type); */
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/* Funtion to set egress default VLAN action */
int dnx_flexe_util_egr_default_vlan_action_set(int unit)
{
    int default_action_id = 0;
    uint32 flags_egress = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    bcm_vlan_action_set_t vid_action;
    
    bcm_vlan_action_set_t_init(&vid_action);
    vid_action.dt_outer = bcmVlanActionNone;
    vid_action.dt_inner = bcmVlanActionNone;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, flags_egress, default_action_id, &vid_action), "");

    return BCM_E_NONE;
}

/* Simple L2 application setting on the provided ports. It will do below settings:
 * 1. Created VLAN with the provided vlan_id
 * 2. Add in_port and out_port to the VLAN
 * 3. Add L2 entry: mac=00:00:00:00:11:11, vlan=vlan_id, port=in_port
 * 4. Add L2 entry: mac=00:00:00:00:22:22, vlan=vlan_id, port=out_port
 */
int dnx_flexe_util_L2_switch_config(
    int unit,
    bcm_port_t in_port,
    bcm_port_t out_port,
    int vlan_id)
{
    bcm_mac_t in_mac={0x00,0x00,0x00,0x00,0x11,0x11}; /* Dst_MAC to in_port */
    bcm_mac_t out_mac={0x00,0x00,0x00,0x00,0x22,0x22}; /* Dst_MAC to out_port */
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t tag_pbmp, untag_bmp;


    /* Create VLAN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vlan_id), "");

    /* Add ports into the VLAN */
    BCM_PBMP_CLEAR(untag_bmp);
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, in_port);
    BCM_PBMP_PORT_ADD(tag_pbmp, out_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, vlan_id, tag_pbmp, untag_bmp), "");

    /* Add L2 entry with in_mac */
    bcm_l2_addr_t_init(&l2_addr, in_mac, vlan_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = in_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "(in_mac)");

    /* Add L2 entry with out_mac */
    bcm_l2_addr_t_init(&l2_addr, out_mac, vlan_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = out_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "(out_mac)");

    return BCM_E_NONE;
}

/* Add or delete L2 entry based on above L2 application.
 * If oper_with_out_mac == 0:
 *     Add/delete L2 entry: mac=00:00:00:00:11:11, vlan=vlan_id, port=port
 * If oper_with_out_mac == 1:
 *     Add/delete L2 entry: mac=00:00:00:00:22:22, vlan=vlan_id, port=port
 * (If oper_add is set, means to add the L2 entry. Will delete the L2 entry if oper_add is 0)
 */
int dnx_flexe_util_L2_entry_oper(
    int unit,
    bcm_port_t port,
    int vlan_id,
    int oper_with_out_mac,
    int oper_add)
{
    bcm_mac_t in_mac = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_mac_t out_mac = {0x00,0x00,0x00,0x00,0x22,0x22};
    bcm_mac_t mac_oper = oper_with_out_mac ? out_mac : in_mac;
    bcm_l2_addr_t l2_addr;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_entry_oper";
    printf("**** %s: Operation ID %d with MAC flag %d ****\n", proc_name, oper_add, oper_with_out_mac);

    if (oper_add) {
        /* Add L2 entry */
        bcm_l2_addr_t_init(&l2_addr, mac_oper, vlan_id);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    } else {
        /* Delete the L2 entry */
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_delete(unit, mac_oper, vlan_id), "");
    }

    return BCM_E_NONE;
}

/* Replicate N*MC_Copies to port_rx(N=nof_copies) */
int dnx_flexe_util_mc_set (
    int unit,
    int eth_tx,
    int port_rx,
    int nof_copies,
    int vlan_id,
    bcm_multicast_t mc_grp,
    bcm_mac_t dst_mac)
{
    int i;
    int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    bcm_multicast_replication_t rep_array;
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t tag_pbmp, untag_bmp;
    char error_msg[200]={0,};

    /* Create MC group */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &mc_grp), "");

    /* Add MC members */
    bcm_multicast_replication_t_init(&rep_array);
    for (i = 0; i < nof_copies; i++) {
        rep_array.port = port_rx;
        snprintf(error_msg, sizeof(error_msg), "Index %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, mc_grp, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array), error_msg);
    }

    /* Create VLAN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vlan_id), "");

    /* Add ports into the VLAN */
    BCM_PBMP_CLEAR(untag_bmp);
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, eth_tx);
    BCM_PBMP_PORT_ADD(tag_pbmp, port_rx);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, vlan_id, tag_pbmp, untag_bmp), "");

    /* Add L2 entry with the MC group */
    bcm_l2_addr_t_init(&l2_addr, dst_mac, vlan_id);
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_grp;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    return BCM_E_NONE;
}

/* Advanced L2 entry operation:
 * It can add/remove L2 entry "dst_mac+vlan_id+out_port" according to the value of mac_oper
 * And it also has an option to create VLAN 'vlan_id' and add both in_port and out_port to the VLAN
 */
int dnx_flexe_util_L2_entry_advanced_set(
    int unit,
    bcm_port_t in_port,
    bcm_port_t out_port,
    bcm_mac_t dst_mac,
    int mac_oper,
    int vlan_id,
    int vlan_oper)
{
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t tag_pbmp, untag_bmp;


    /* VLAN setting */
    if (vlan_oper) {
        /* Create VLAN */
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vlan_id), "");

        /* Add ports into the VLAN */
        BCM_PBMP_CLEAR(untag_bmp);
        BCM_PBMP_CLEAR(tag_pbmp);
        BCM_PBMP_PORT_ADD(tag_pbmp, in_port);
        BCM_PBMP_PORT_ADD(tag_pbmp, out_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, vlan_id, tag_pbmp, untag_bmp), "");
    }

    /* L2 entry setting */
    if (mac_oper) {
        /* Add L2 entry with out_mac */
        bcm_l2_addr_t_init(&l2_addr, dst_mac, vlan_id);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = out_port;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "(out_mac)");
    } else {
        /* Delete the L2 entry */
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_delete(unit, dst_mac, vlan_id), "");
    }

    return BCM_E_NONE;
}

/* Simple L3 application setting on the provided ports. It will do below settings:
 * 1. Add in_port to in_vlan and out_port to out_vlan
 * 2. Point IP 1.1.1.1(VRF=0xAB) to in_port
 * 3. Point IP 2.2.2.2(VRF=0xAB) to out_port
 */
int dnx_flexe_util_L3_route_config(
    int unit,
    bcm_port_t in_port,
    bcm_port_t out_port,
    int in_vlan,
    int out_vlan)
{
    int ing_intf, egr_intf;
    int vrf = 0xab;
    uint32 sip = 0x01010101;
    uint32 dip = 0x02020202;
    bcm_ip6_t dip6 = {0x22, 0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x22, 0x22};
    bcm_mac_t in_mac = {0x00,0x00,0x00,0x00,0x11,0x11};  /* egress MAC or src_mac at in_port side */
    bcm_mac_t out_mac = {0x00,0x00,0x00,0x00,0x22,0x22}; /* egress MAC or src_mac at out_port side */
    bcm_mac_t my_mac = {0x00,0x00,0x00,0x00,0x33,0x33};
    bcm_pbmp_t tag_pbmp, untag_bmp;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t fec_ing = 0x2000CCFE; /* FEC ID attached on in_port */
    bcm_if_t fec_egr = 0x2000CCFF; /* FEC ID attached on out_port */
    bcm_l3_host_t l3_host;


    /* Create in_vlan and add in_port*/
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, in_vlan), "(in_vlan)");
    BCM_PBMP_CLEAR(untag_bmp);
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, in_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, in_vlan, tag_pbmp, untag_bmp), "(in_port)");

    /* Create out_vlan and add out_port*/
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, out_vlan), "(out_vlan)");
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, out_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, out_vlan, tag_pbmp, untag_bmp), "(out_port)");

    /* Create inRIF and outRIF */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, my_mac, 6);
    l3_intf.l3a_vid = in_vlan;
    l3_intf.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, &l3_intf), "(in_rif)");
    ing_intf = l3_intf.l3a_intf_id;

    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, my_mac, 6);
    l3_intf.l3a_vid = out_vlan;
    l3_intf.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, &l3_intf), "(out_rif)");
    egr_intf = l3_intf.l3a_intf_id;

    /* Create L3 nexthop at in_port side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, in_mac, 6);
    l3_egress.vlan = in_vlan;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_ing),
        "(EGRESS_ONLY, in_port)");
    l3_egress.intf = ing_intf;
    l3_egress.port = in_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_ing),
        "(INGRESS_ONLY, in_port)");

    /* Create L3 nexthop at out_port side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, out_mac, 6);
    l3_egress.vlan = out_vlan;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_egr),
        "(EGRESS_ONLY, out_port)");
    l3_egress.intf = egr_intf;
    l3_egress.port = out_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_egr),
        "(INGRESS_ONLY, out_port)");

    /* Add host entry with sip */
    bcm_l3_host_t_init(&l3_host);
    l3_host.l3a_ip_addr = sip;
    l3_host.l3a_vrf = vrf;
    l3_host.l3a_intf = fec_ing;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, &l3_host), "(sip)");

    /* Add host entry with dip */
    bcm_l3_host_t_init(&l3_host);
    l3_host.l3a_ip_addr = dip;
    l3_host.l3a_vrf = vrf;
    l3_host.l3a_intf = fec_egr;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, &l3_host), "(dip)");
 
    /* Add host entry with dip6 */
    bcm_l3_host_t_init(&l3_host);
    l3_host.l3a_ip6_addr = dip6;
    l3_host.l3a_vrf = vrf;
    l3_host.l3a_intf = fec_egr;
    l3_host.l3a_flags = BCM_L3_IP6;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, &l3_host), "(dip6)");

    return BCM_E_NONE;
}

/* Add or delete L3 route entry based on above L3 application.
 * If oper_with_dip == 0:
 *     Add/delete L3 entry: IP=1.1.1.1, VRF=0xAB
 * If oper_with_dip == 1:
 *     Add/delete L3 entry: IP=2.2.2.2, VRF=0xAB
 * (If oper_add == 1, means to add the L3 entry. Will delete the L3 entry if oper_add == 0)
 */
int dnx_flexe_util_L3_host_entry_oper(
    int unit,
    int oper_with_dip,
    int oper_add)
{
    int vrf = 0xab;
    uint32 sip = 0x01010101;
    uint32 dip = 0x02020202;
    uint32 oper_ip = oper_with_dip ? dip : sip;
    bcm_if_t fec_ing = 0x2000CCFE;
    bcm_if_t fec_egr = 0x2000CCFF;
    bcm_if_t oper_fec = oper_with_dip ? fec_egr : fec_ing;
    bcm_l3_host_t l3_host;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L3_host_entry_oper";
    printf("**** %s: Operation ID %d with IP %d ****\n", proc_name, oper_add, oper_ip);

    /* Configure bcm_l3_host_t */
    bcm_l3_host_t_init(&l3_host);
    l3_host.l3a_ip_addr = oper_ip;
    l3_host.l3a_vrf = vrf;
    l3_host.l3a_intf = oper_fec;

    if (oper_add) {
        /* Add host entry */
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, &l3_host), "");
    } else {
        /* Delete host entry */
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_delete(unit, &l3_host), "");
    }

    return BCM_E_NONE;
}

/* Function to enable or disable the TPID on specific port */
int dnx_flexe_util_port_tpid_cfg(
    int unit,
    bcm_port_t port,
    int tpid,
    int tpid_enable)
{
    char error_msg[200]={0,};
    int tag_format_class_id = tpid_enable ? 8 : 0;
    bcm_port_tpid_class_t tpid_class;


    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    tpid_class.port = port;
    tpid_class.tpid1 = tpid;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.tag_format_class_id = tag_format_class_id;

    snprintf(error_msg, sizeof(error_msg), "(port %d)", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, tpid_class), error_msg);

    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port, bcmPortClassVlanMember, port), error_msg);

    return BCM_E_NONE;
}

/* Funtion to create L1 ETH/ILKN port */
int dnx_flexe_util_L1_port_add(
    int unit,
    bcm_port_t port_id,
    int is_eth_port,
    int first_lane,
    int nof_lanes,
    int interface_id,
    int channel_id,
    int speed,
    int fec_type,
    int is_ixia_port)
{
    int i;
    uint32 flags = BCM_PORT_ADD_CROSS_CONNECT;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_resource_t resource;
    char error_msg[200]={0,};


    /* Auto allocate FEC type */
    if (fec_type < 0) {
        if (speed >= 200000) {
            fec_type = bcmPortPhyFecRs544_2xN;
        } else {
            fec_type = bcmPortPhyFecRsFec;
        }
    }

    /* Add channelized flag for ILKN port creation */
    if (!is_eth_port) {
        flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
        if (ilkn_interleave_en) {
            flags |= BCM_PORT_ADD_INGRESS_INTERLEAVING_ENABLE;
        }
    }

    /* Set interface info */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    interface_info.interface = is_eth_port ? BCM_PORT_IF_NIF_ETH : BCM_PORT_IF_ILKN;
    interface_info.phy_port = first_lane;
    interface_info.num_lanes= nof_lanes;
    interface_info.interface_id = interface_id;
    for (i = first_lane; i < (first_lane + nof_lanes); i++) {
        BCM_PBMP_PORT_ADD(interface_info.phy_pbmp, i);
    }

    /* Set mapping info */
    bcm_port_mapping_info_t_init(&mapping_info);
    mapping_info.core = 0;
    mapping_info.tm_port = port_id;
    mapping_info.num_priorities = 2;
    mapping_info.pp_port = -1;
    mapping_info.base_hr = -1;
    mapping_info.channel = channel_id;

    /* Set resource info */
    bcm_port_resource_t_init(&resource);
    resource.port = port_id;
    resource.speed = speed;
    resource.fec_type = fec_type;
    resource.link_training = !is_ixia_port;
    resource.phy_lane_config = 0;
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource.phy_lane_config);

    /* Create the L1 port dynamically */
    snprintf(error_msg, sizeof(error_msg), "(port %d)", port_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_add(unit, port_id, flags, &interface_info, &mapping_info), error_msg);

    /* Set port speed */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_set(unit, port_id, &resource), error_msg);

    /* Enable the L1 port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port_id, 1), error_msg);

    /* Linkscan setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_linkscan_set(unit, port_id, BCM_LINKSCAN_MODE_SW), error_msg);

    return BCM_E_NONE;
}

/* Funtion to create L1 ETH port(Includes normal L1 ETH and L1 MGMT port)
 * mgmt_channel     MGMT channel id (FlexE_Gen2 only). Values:
 *                  2 -     OAM channel for L1 OAM interface. The rate_mhz must be 25000.
 *                  3 -     PTP channel for L1 PTP interface. The rate_mhz must be 10000.
 *                  others  None MGMT channel. Normal ETH_L1 port will be created
 */
int dnx_flexe_util_L1_eth_add(
    int unit,
    bcm_port_t port_id,
    int first_lane,
    int nof_lanes,
    int rate_mhz,
    int fec_type,
    int mgmt_channel)
{
    int i;
    uint32 flags = BCM_PORT_ADD_CROSS_CONNECT;
    bcm_pbmp_t phy_pbmp;
    char error_msg[200]={0,};
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);


    if ((mgmt_channel == 2 || mgmt_channel == 3) && is_flexe_gen2) {
        flags = BCM_PORT_ADD_IF_FRAMER_MGMT;
    }

    /* Add the port with DPP reference code */
    BCM_PBMP_CLEAR(phy_pbmp);
    for (i = first_lane; i < (first_lane + nof_lanes); i++) {
        BCM_PBMP_PORT_ADD(phy_pbmp, i);
    }
    snprintf(error_msg, sizeof(error_msg), "(port %d)", port_id);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_eth_port(unit, port_id, phy_pbmp, rate_mhz, fec_type, flags, mgmt_channel), error_msg);

    return BCM_E_NONE;
}

/* Function to create L2 ETH/ILKN port */
int dnx_flexe_util_L2_port_add(
    int unit,
    bcm_port_t port,
    int first_phy,
    int if_type,
    int lane_nof,
    int speed,
    int fec_type)
{
    int i;
    int channel = 0;
    int num_priorities = 2;
    int flags = 0;
    int offset = 0;
    int header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    int header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    int nof_links, nof_cores, nof_links_per_core;
    dfe_mode_t dfe_mode = dfe_on;
    medium_type_t medium_type = copper_cable;
    bcm_port_resource_t resource;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    char error_msg[200]={0,};

    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;

    /* Auto allocate FEC type */
    if (fec_type < 0) {
        if (speed >= 200000) {
            fec_type = bcmPortPhyFecRs544_2xN;
        } else {
            fec_type = bcmPortPhyFecRsFec;
        }
    }

    /* Interface info */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    interface_info.phy_port = first_phy;
    interface_info.num_lanes = lane_nof;
    interface_info.interface = if_type;
    for (i = first_phy; i < (first_phy + lane_nof); i++) {
        BCM_PBMP_PORT_ADD(interface_info.phy_pbmp,i);
    }
    if (if_type == BCM_PORT_IF_ILKN){
        interface_info.interface_id = offset;
    }

    /* Mapping info */
    bcm_port_mapping_info_t_init(&mapping_info);
    mapping_info.channel = channel;
    mapping_info.core = first_phy / nof_links_per_core;
    mapping_info.tm_port = port;
    mapping_info.num_priorities = num_priorities;

    /* Resource info */
    bcm_port_resource_t_init(resource);
    resource.port = port;
    resource.speed = speed;
    resource.fec_type = fec_type;
    resource.link_training = 1;
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource.phy_lane_config);

    /* Port creation */
    snprintf(error_msg, sizeof(error_msg), "(port %d)", port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info,
              resource, dfe_mode, medium_type, header_type_in, header_type_out, flags), error_msg);

    /* TPID setting */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_tpid_cfg(unit, port, 0x8100, 1), error_msg);

    return BCM_E_NONE;
}

/* Funtion to create FlexE PHY */
int dnx_flexe_util_phy_port_add(
    int unit,
    bcm_port_t phy_port_id,
    int first_phy,
    int rate_mhz)
{
    int i;
    int nof_lanes = rate_mhz / 50000; /* 50G per lane */
    bcm_pbmp_t phy_pbmp;
    char error_msg[200]={0,};

    if (is_reduced_cdu_lane_speed) {
        nof_lanes = rate_mhz / 25000; /* 25G per lane */
    }


    /* Set phy_pbmp */
    BCM_PBMP_CLEAR(phy_pbmp);
    for (i = first_phy; i < (first_phy + nof_lanes); i++) {
        BCM_PBMP_PORT_ADD(phy_pbmp, i);
    }

    /* Start to create the FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(port %d)", phy_port_id);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_flexe_phy_port(unit, phy_port_id, phy_pbmp, rate_mhz, phy_cl72_enable, 0), error_msg);

    return BCM_E_NONE;
}

/* Funtion to create FlexE group with the FlexE PHY(s) in array flexe_phy_ports[] */
int dnx_flexe_util_group_create(
    int unit,
    bcm_gport_t flexe_grp_id,
    int nof_pcs)
{
    int i;
    uint32 flags = flexe_bypass_en ? BCM_PORT_FLEXE_BYPASS : 0;
    bcm_port_flexe_group_phy_info_t phy_info[nof_pcs];
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_group_create";

    /* Fill FlexE group PHY info */
    for (i = 0; i < nof_pcs; i++) {
        phy_info[i].port = flexe_phy_ports[i];
        phy_info[i].logical_phy_id = logical_phy_ids[i] ? logical_phy_ids[i] : flexe_phy_ports[i];
        phy_info[i].slot_mode = flexe_gen2_grp_phy_slot_mode;
        if (flexe_bypass_en) {
            /* The logical PHY ID should be 0 for bypass mode */
            phy_info[i].logical_phy_id = 0;
            printf("**** %s: Creating FlexE BYPASS group with flag %d ****\n", proc_name, flags);
        }
    }

    /* Start to create the FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(group %d)", flexe_grp_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_create(unit, flexe_grp_id, flags, nof_pcs, phy_info), error_msg);

    /* Synce setting when FlexE PHY not in Rx valid status */
    if (dnx_flexe_rx_in_invalid_status(unit) && !skip_synce_in_grp_creation) {
        printf("------ DBG_INFO: Starting synce setting on UNIT %d ------\n", unit);
        sal_sleep(1);
        bcm_switch_control_set(unit, bcmSwitchSynchronousPortClockSource,  flexe_phy_ports[0]);
        bcm_switch_control_set(unit, bcmSwitchSynchronousPortClockSourceDivCtrl, 2);
        sal_sleep(1);

        for (i = 0; i < nof_pcs; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to disable FlexE PHY %d", flexe_phy_ports[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_phy_ports[i], 0), error_msg);
            sal_sleep(1);
            snprintf(error_msg, sizeof(error_msg), "fail to enable FlexE PHY %d", flexe_phy_ports[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_phy_ports[i], 1), error_msg);
        }
    }

    return BCM_E_NONE;
}

/* Funtion to get FlexE group slot speed according to its PHY.slot_mode */
int dnx_flexe_util_group_slot_speed_get(
    int unit,
    bcm_gport_t flexe_grp)
{
    int grp_slot_speed = 0;
    int max_nof_pcs = 16;
    int actual_nof_pcs;
    uint32 flags = 0;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    bcm_port_flexe_phy_slot_mode_t grp_slot_mode;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_grp_slot_speed_get";

    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, flags, max_nof_pcs, &grp_phy_info, &actual_nof_pcs), "");
    if (actual_nof_pcs <= 0) {
        printf("**** %s: Error - No PHY in provided FlexE group ****\n", proc_name);
        return BCM_E_FAIL;
    }

    grp_slot_mode = grp_phy_info[0].slot_mode;
    print "grp_slot_mode is:\n";
    print grp_slot_mode;
    switch (grp_slot_mode)
    {
        case bcmPortFlexePhySlot5G:
        {
            grp_slot_speed = 5000;
            break;
        }
        case bcmPortFlexePhySlot1P25G:
        {
            grp_slot_speed = 1250;
            break;
        }
        case bcmPortFlexePhySlot1G:
        {
            grp_slot_speed = 1000;
            break;
        }
        default:
        {
            printf("%s Error: unknown grp_slot_mode %d\n", proc_name, grp_slot_mode);
            return BCM_E_FAIL;
        }
    }
    printf("**** %s done! grp_slot_speed is %d M ****\n", proc_name, grp_slot_speed);

    return grp_slot_speed;
}

int synce_flexe_phy_all[8] = {0};
int synce_flexe_grp_all[8] = {0};
int dnx_flexe_util_oh_alarm_check(
    int unit_cen,
    int unit_dis,
    int *oh_alarm_detected)
{
    int i;
    int grp_alarm_detected = 0;
    int phy_alarm_detected = 0;
    int grp_alarm_check_flag = 0;
    int phy_alarm_check_flag = 0;
    int flexe_phy_check_list[8] = {0};
    bcm_port_flexe_oh_alarm_t oh_alarm_cen;
    bcm_port_flexe_oh_alarm_t oh_alarm_dis;
    char error_msg[200]={0,};

    /* Check OH alarms in each FlexE group */
    for (i = 0; i < 8; i++) {
        if (synce_flexe_grp_all[i] != 0) {
            printf("======== Checking group %d ========\n", synce_flexe_grp_all[i]);
            snprintf(error_msg, sizeof(error_msg), "fail in group %d", synce_flexe_grp_all[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_cen, synce_flexe_grp_all[i], 0, &oh_alarm_cen), error_msg);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_dis, synce_flexe_grp_all[i], 0, &oh_alarm_dis), error_msg);
            sal_sleep(3);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_cen, synce_flexe_grp_all[i], 0, &oh_alarm_cen), error_msg);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_dis, synce_flexe_grp_all[i], 0, &oh_alarm_dis), error_msg);
            grp_alarm_check_flag = 1;

            if (oh_alarm_cen.alarm_active || oh_alarm_dis.alarm_active) {
                printf("======== OH alarm detected in group %d ========\n", synce_flexe_grp_all[i]);
                print oh_alarm_cen;
                print oh_alarm_dis;
                grp_alarm_detected = 1;
                break;
            }
        }
    }
    if (!grp_alarm_check_flag) {
        print "======== No FlexE group for OH alarm check ========";
    }

    /* Check OH alarms on each FlexE PHY */
    /** Copy FlexE PHY(s) from user provided list synce_flexe_phy_all[] */
    for (i = 0; i < 8; i++) {
        flexe_phy_check_list[i] = synce_flexe_phy_all[i];
        if (flexe_phy_check_list[i]) {
            phy_alarm_check_flag = 1;
        }
    }
    /** If no PHY provided in synce_flexe_phy_all[], copy FlexE PHY(s) from flexe_phy_ports[] 
     *  Note: Array flexe_phy_ports[] is used for FlexE group creation
     */
    if (!phy_alarm_check_flag) {
        print "======== No FlexE PHY for OH alarm check. Use default PHY(s) to check ========";
        for (i = 0; i < 8; i++) {
            flexe_phy_check_list[i] = flexe_phy_ports[i];
            if (flexe_phy_check_list[i]) {
                phy_alarm_check_flag = 1;
            }
        }
    }
    if (!phy_alarm_check_flag) {
        print "======== No FlexE PHY for OH alarm check ========";
    }
    /** Start to check OH alarm on each PHY in array flexe_phy_check_list[] */
    for (i = 0; i < 8; i++) {
        if (flexe_phy_check_list[i]) {
            printf("======== Checking PHY %d ========\n", flexe_phy_check_list[i]);
            snprintf(error_msg, sizeof(error_msg), "fail on PHY %d", flexe_phy_check_list[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_cen, flexe_phy_check_list[i], 0, &oh_alarm_cen), error_msg);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_dis, flexe_phy_check_list[i], 0, &oh_alarm_dis), error_msg);
            sal_sleep(3);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_cen, flexe_phy_check_list[i], 0, &oh_alarm_cen), error_msg);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit_dis, flexe_phy_check_list[i], 0, &oh_alarm_dis), error_msg);

            if (oh_alarm_cen.alarm_active || oh_alarm_dis.alarm_active) {
                printf("======== OH alarm detected on PHY %d ========\n", flexe_phy_check_list[i]);
                print oh_alarm_cen;
                print oh_alarm_dis;
                phy_alarm_detected = 1;
                break;
            }
        }
    }

    *oh_alarm_detected = grp_alarm_detected | phy_alarm_detected;
    return BCM_E_NONE;
}

/* Alarm check after Synce setting */
int dnx_flexe_util_synce_check(
    int unit_cen,
    int unit_dis,
    bcm_gport_t flexe_grp)
{
    bcm_port_flexe_oh_alarm_t oh_alarm_cen;
    bcm_port_flexe_oh_alarm_t oh_alarm_dis;
    int sync_error = 0;
    int max_round_allowed = 10;
    int i, round_index = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_synce_check";
    printf("======== %s: Starting synce check on the 2 devices ========\n",  proc_name);

    if (!dnx_flexe_rx_in_invalid_status(unit_cen)) {
        return BCM_E_NONE;
    }

    if (flexe_grp != 0) {
        /* Only check the OH alarms in provided group */
        sal_memset(synce_flexe_grp_all, 0, sizeof(synce_flexe_grp_all));
        synce_flexe_grp_all[0] = flexe_grp;
    } else {
        /* 
         * Check OH alarms in all valid groups in array synce_flexe_grp_all[] 
         * Please fill the array with FlexE group(s) firstly
         */
    }

    /* Get OH alarms in FlexE group(s) and PHY(s) */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_oh_alarm_check(unit_cen, unit_dis, &sync_error), "");
    if (sync_error != 0 && sync_error != 1) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_oh_alarm_check ****\n", proc_name, sync_error);
        return BCM_E_FAIL;
    }

    /* Try to reset FlexE ports */
    if (sync_error) {
        print "======== Resetting all PHYs ========\n";
        bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0");
        bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0xffff");
        bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0");
        bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0xffff");
        sal_sleep(4);

        /* Get OH alarms in FlexE group(s) and PHY(s) */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_oh_alarm_check(unit_cen, unit_dis, &sync_error), "");
        if (sync_error != 0 && sync_error != 1) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_oh_alarm_check ****\n", proc_name, sync_error);
            return BCM_E_FAIL;
        }

        if (sync_error) {
            print "======== Synce error detected after resetting all PHYs ========\n";
        } else {
            print "======== Synce error cleared on the 2 devices ========\n";
        }
    }

    /* Active synce on each FlexE PHY for several times */
    while (sync_error && round_index < max_round_allowed) {
        for (i = 0; i < 8; i++) {
            if (flexe_phy_ports[i]) {
                /* Active synce on each FlexE PHY */
                printf("======== Round %d : activing synce on PHY %d ========\n", round_index, flexe_phy_ports[i]);
                BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSource, flexe_phy_ports[i]),
                    "fail to set master clock");
                BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSourceDivCtrl, 2),
                    "fail to set master clock");
                sal_sleep(2);
                BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit_cen, bcmSwitchSynchronousPortClockSource, flexe_phy_ports[i]),
                    "fail to set backup clock");
                BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit_cen, bcmSwitchSynchronousPortClockSourceDivCtrl, 2),
                    "fail to set backup clock");

                /* Reset all PHYs */
                bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0");
                bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0xffff");
                bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0");
                bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0xffff");
                sal_sleep(3);

                /* Get OH alarms in FlexE group(s) and PHY(s) */
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_oh_alarm_check(unit_cen, unit_dis, &sync_error), "");
                if (sync_error != 0 && sync_error != 1) {
                    printf("**** %s: Error(%d), fail in dnx_flexe_util_oh_alarm_check ****\n", proc_name, sync_error);
                    return BCM_E_FAIL;
                }

                if (sync_error) {
                    print "======== Synce error detected after activing synce on the PHY ========\n";
                } else {
                    print "======== Synce error cleared on the 2 devices ========\n";
                    break;
                }
            }
        }
        round_index++;
    }

    if (sync_error) {
        dnx_flexe_util_read_flexe_alarms(unit_cen);
        dnx_flexe_util_read_flexe_alarms(unit_dis);
        printf("======== %s: Synce check FAILED on the 2 devices ========\n", proc_name);
        return BCM_E_FAIL;
    } else {
        /* Don't change synce setting while creating next FlexE group */
        skip_synce_in_grp_creation = 1;
    }
    printf("======== %s: Synce check PASSed on the 2 devices ========\n",  proc_name);

    return BCM_E_NONE;
}

/* Funtion to read FlexE alarms via registers */
int dnx_flexe_util_read_flexe_alarms(int unit)
{
        printf("======== Getting alarms on UNIT %d ========\n", unit);
        bshell(unit, "g FSCL_OH_RX_ALM");
        sal_sleep(3);
        print "-------------------------------------------\n";
        bshell(unit, "g FSCL_OH_RX_ALM");
}

/* Funtion to create FlexE BusA/B/C client with speed */
int dnx_flexe_util_client_create_with_speed(
    int unit,
    bcm_port_t client_id,
    flexe_port_type_t client_type,
    int speed,
    uint32 speed_flag)
{
    int i;
    int channel_id = 0;
    uint32 flags = 0;
    bcm_port_if_t intf_type;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode = dfe_none;
    medium_type_t medium_type = backplane;
    char error_msg[200]={0,};


    /* Parse the FlexE client type */
    if (client_type == port_type_busB_L1)
    {
        /* BusB client for L1 switching */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_MAC;
    }
    else if (client_type == port_type_busB_L2_L3)
    {
        /* BusB client for L2/L3 switching */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_MAC;
    }
    else if (client_type == port_type_1588_L1)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 81;
    }
    else if (client_type == port_type_1588_L2)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags =  BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 81;
    }
    else if (client_type == port_type_oam_L1)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 80;
    }
    else if (client_type == port_type_oam_L2)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 80;
    }
    else if (client_type == port_type_busC)
    {
        /* Virtual client in BusC side */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_SAR;
    }
    else
    {
        /* Standard FlexE client in BusA side */
        flags = 0;
        intf_type = BCM_PORT_IF_FLEXE_CLIENT;
    }

    /* Set interface info */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    interface_info.interface = intf_type;
    interface_info.interface_id = -1;

    /* Set mapping info */
    bcm_port_mapping_info_t_init(&mapping_info);
    mapping_info.channel = channel_id;
    mapping_info.core = 0; /* always 0 for Q2A */
    mapping_info.tm_port = client_id;
    mapping_info.pp_port = -1;
    mapping_info.base_hr = -1;
    mapping_info.base_q_pair = -1;
    mapping_info.num_priorities = 2;


    /* Set resource info */
    bcm_port_resource_t_init(&resource);
    resource.port = client_id;
    resource.speed = speed;
    resource.flags = speed_flag;
    resource.fec_type = 0;
    resource.phy_lane_config = 0;
    resource.link_training = 0;

    /* Asymmetrical client speeds */
    if (speed_flag == BCM_PORT_RESOURCE_ASYMMETRICAL) {
        resource.rx_speed = client_rx_speed;
    }

    snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, client_id, mapping_info, interface_info,
              resource, dfe_mode, medium_type,  BCM_SWITCH_PORT_HEADER_TYPE_ETH, BCM_SWITCH_PORT_HEADER_TYPE_ETH, flags), error_msg);

    /* IPG and port enable */
    if (client_type == port_type_busB_L1 || client_type == port_type_busB_L2_L3 || client_type == port_type_1588_L2)
    {
        /* IPG setting on BusB client */
        snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit, client_id, 0, BCM_PORT_DUPLEX_FULL, 88), error_msg);
    }

    /* VLAN setting on BusB L2&L3 clients */
    if (client_type == port_type_busB_L2_L3 || client_type == port_type_1588_L2)
    {
        snprintf(error_msg, sizeof(error_msg), "(port %d)", client_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_tpid_cfg(unit, client_id, 0x8100, 1), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to set FlexE client speed */
int dnx_flexe_util_client_speed_set(
    int unit,
    bcm_port_t client_id,
    int speed,
    uint32 speed_flag)
{
    char error_msg[200]={0,};
    bcm_port_resource_t resource;


    /* Set resource info */
    bcm_port_resource_t_init(&resource);
    resource.port = client_id;
    resource.speed = speed;
    resource.flags = speed_flag;
    resource.fec_type = 0;
    resource.phy_lane_config = 0;
    resource.link_training = 0;

    /* Asymmetrical client speeds */
    if (speed_flag & BCM_PORT_RESOURCE_ASYMMETRICAL) {
        resource.rx_speed = client_rx_speed;
    }

    /* Set port speed */
    snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_set(unit, client_id, &resource), error_msg);

    return BCM_E_NONE;
}

int dnx_flexe_util_client_speed_get(
    int unit,
    bcm_port_t client_id,
    int get_tx_speed,
    int *client_speed)
{
    bcm_port_resource_t resource_get;

    /* Get port resource */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, client_id, &resource_get), "");

    /* Return required client speed */
    if (get_tx_speed) {
        *client_speed = resource_get.speed;
    } else {
        *client_speed = resource_get.flags & BCM_PORT_RESOURCE_ASYMMETRICAL ? resource_get.rx_speed : resource_get.speed;
    }

    return BCM_E_NONE;
}

/* Funtion to set FlexE slots for both Mux&demux and overhead */
int dnx_flexe_util_calendar_slots_set(
    int unit,
    bcm_gport_t flexe_grp_id,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots)
{
    int i;
    int cal_slots[nof_slots];
    uint32 cal_flags;
    bcm_port_flexe_group_cal_t active_rx_cal;
    int nof_pcs;
    int all_phys_linkup = 1;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];

    /* Check all PHYs in the group should be linkup */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp_id, 0, 16, &grp_phy_info, &nof_pcs), "");
    for (i = 0; i < nof_pcs; i++) {
        if (dnx_flexe_util_phy_port_link_check(unit, grp_phy_info[i].port) != BCM_E_NONE) {
            all_phys_linkup = 0; /* Linkdwon PHY detected */
            break;
        }
    }

    if (dnx_flexe_gen2_support(unit, -1) && all_phys_linkup) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit, flexe_grp_id, BCM_PORT_FLEXE_RX, &active_rx_cal), "");
    }

    /* Start to assgin calendar slots for FlexE clients */
    for (i = 0; i < nof_slots; i++) {
        if (calendar_id == bcmPortFlexeGroupCalA)
        {
            cal_slots[i] = Cal_Slots_A[i];
        }
        else
        {
            cal_slots[i] = Cal_Slots_B[i];
        }
    }

    /* Configure calendar slots for Mux/demux */
    cal_flags = flags & ~BCM_PORT_FLEXE_OVERHEAD;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_set(unit, flexe_grp_id, cal_flags, calendar_id, nof_slots, cal_slots),
        "(mux/demux)");

    if ( dnx_flexe_gen2_support(unit, -1) && ((cal_flags==0) || (cal_flags & BCM_PORT_FLEXE_RX)) && all_phys_linkup) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_set(unit, flexe_grp_id, BCM_PORT_FLEXE_RX, active_rx_cal), "");
    }

    /* Configure calendar slots for overhead
     * The client ID in overhead will be the same as logical client ID
     */
    if ((cal_flags == 0 || (cal_flags & BCM_PORT_FLEXE_TX)) && !flexe_bypass_en) {
        cal_flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_OVERHEAD;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_set(unit, flexe_grp_id, cal_flags, calendar_id, nof_slots, cal_slots),
            "(overhead)");
    }

    return BCM_E_NONE;
}

/* Funtion to add/remove a FlexE PHY to/from a FlexE group */
int dnx_flexe_util_group_phy_add_remove(
    int unit,
    bcm_gport_t flexe_grp_id,
    bcm_port_t flexe_phy_port,
    uint32 logical_phy_id,
    int flag_to_add)
{
    int i;
    uint32 flags = 0;
    bcm_port_flexe_group_phy_info_t phy_info;


    /* Set phy info */
    phy_info.port = flexe_phy_port;
    phy_info.logical_phy_id = logical_phy_id;
    phy_info.slot_mode = flexe_gen2_grp_phy_slot_mode;

    /* Start to add/remove the FlexE phy port */
    if (flag_to_add)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_phy_add(unit, flexe_grp_id, flags, &phy_info), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_phy_remove(unit, flexe_grp_id, flags, &phy_info), "");
    }

    return BCM_E_NONE;
}

/* Funtion to check the link status of FlexE PHY */
int dnx_flexe_util_phy_port_link_check(
    int unit,
    bcm_port_t port)
{
    char error_msg[200]={0,};
    int link_status = BCM_PORT_LINK_STATUS_DOWN;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_phy_port_link_check";

    if (dnx_flexe_gen2_support(unit, 1)) {
        printf("**** %s: Skip link status check on adapter! ****\n", proc_name);
        return BCM_E_NONE;
    }

    /* Get the link status */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_link_status_get(unit, port, &link_status), "");
    if (link_status == BCM_PORT_LINK_STATUS_UP) {
        return BCM_E_NONE;
    }
    bshell(unit,"sleep 5");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_link_status_get(unit, port, &link_status), "");

    if(link_status != BCM_PORT_LINK_STATUS_UP) {
        /* The link is down. Wait for 10s and check the status again */
        bshell(unit,"sleep 10");

        snprintf(error_msg, sizeof(error_msg), "(port %d)", port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_link_status_get(unit, port, &link_status), error_msg);

        if(link_status != BCM_PORT_LINK_STATUS_UP) {
            printf("**** %s: unexpected link status %d on port %d ****\n", proc_name, link_status, port);
            return BCM_E_FAIL;
        }
    }
    else
    {
        /* The link is up */
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

/* Funtion to build or clear the datapath from L2 ETH port to L1 ETH port
 * This datapath is used to send packet to L1 ETH port via CPU
 * It is just for test purpose
 */
int dnx_flexe_util_datapath_L2_eth_to_L1_eth(
    int unit,
    bcm_port_t eth_port_L2,
    bcm_port_t eth_port_L1,
    int bandwidth,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int tdm_mc_id = 6001;
    bcm_port_t client_c = 98;
    bcm_port_t client_b_L1 = 98;
    bcm_port_t client_b_L2 = 99;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_datapath_L2_eth_to_L1_eth";

    if (!clear_flag) {
        /* Create BusB L1 client */
        if (!is_flexe_gen2) {
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L1, port_type_busB_L1, bandwidth, 0),
                "fail to create client_b_L1");
        } else {
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, bandwidth, 0),
                "fail to create client_c");
        }

        /* Create BusB L2 client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, bandwidth, 0),
            "fail to create client_b_L2");
    }

    /* Disable BusB clients */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L1, 0), "fail to disable client_b_L1");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 0), "fail to disable client_b_L2");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_L1, 0), "fail to disable eth_port_L1");
    }

    if (!clear_flag)
    {
        if (!is_flexe_gen2) {
            /** FlexE_Gen1: eth_port_L2 -> force_fwd -> client_b_L2 -> client_b_L1 -> L1_switch -> eth_port_L1 */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_L2, client_b_L2, 1),
                "fail in force_fwd setting(eth_port_L2 to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_b_L1),
                "fail in setting FlexE flow(client_b_L2 to client_b_L1)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, eth_port_L1),
                "fail in setting FlexE flow(client_b_L1 to eth_port_L1)");
        } else {
            /** FlexE_Gen2: eth_port_L2 -> client_b_L2 -> client_c -> client_c -> eth_port_L1 */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_L2, client_b_L2, 1),
                "fail in force_fwd setting(eth_port_L2 to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_c),
                "fail in setting FlexE flow(client_b_L2 to client_c)");
            BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example(unit, client_c, client_c, 0, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size, tdm_mc_id), "");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_c, 0, eth_port_L1),
                "fail in setting FlexE flow(client_c to eth_port_L1)");
        }
    }
    else
    {
        if (!is_flexe_gen2) {
            /** Clear FlexE_Gen1 datapath **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_L2, client_b_L2, 0),
                "fail in force_fwd setting(eth_port_L2 to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_L2, 0, client_b_L1),
                "fail in setting FlexE flow(client_b_L2 to client_b_L1)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_L1, 0, eth_port_L1),
                "fail in setting FlexE flow(client_b_L1 to eth_port_L1)");
        } else {
            /** Clear FlexE_Gen2 datapath **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_L2, client_b_L2, 0),
                "fail in force_fwd setting(eth_port_L2 to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_L2, 0, client_c),
                "fail in setting FlexE flow(client_b_L2 to eth_port_L1)");
            /* Add steps to clear TDM datapath client_c -> client_c */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_c, 0, eth_port_L1),
                "fail in setting FlexE flow(client_b_L2 to eth_port_L1)");
        }
    }

    /* Enable BusB clients */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L1, 1), "fail to enable client_b_L1");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 1), "fail to enable client_b_L2");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_L1, 1), "fail to enable eth_port_L1");
    }

    /* Remove BusB clients */
    if (clear_flag) {
        if (!is_flexe_gen2) {
            snprintf(error_msg, sizeof(error_msg), "(client %d)", client_b_L1);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_L1), error_msg);
        } else {
            snprintf(error_msg, sizeof(error_msg), "(client %d)", client_c);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_c), error_msg);
        }
        snprintf(error_msg, sizeof(error_msg), "(client %d)", client_b_L2);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_L2), error_msg);
    }

    return BCM_E_NONE;
}


/* Funtion to build or clear the datapath from L1 ETH port to L2 ETH port
 * This datapath is used to redirect received packets on L1 ETH port to L2 ETH port
 * It is just for test purpose
 */
int dnx_flexe_util_datapath_L1_eth_to_L2_eth(
    int unit,
    bcm_port_t eth_port_L1,
    bcm_port_t eth_port_L2,
    int bandwidth,
    int clear_flag,
    bcm_port_t dst_clientB_ori)
{
    int i;
    uint32 flags = 0;
    int tdm_mc_id = 6002;
    bcm_port_t client_c = 96;
    bcm_port_t client_b_L1 = 96;
    bcm_port_t client_b_L2 = 97;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    char error_msg[200]={0,};


    if (!clear_flag) {
        /* Create BusB L1 client */
        if (!is_flexe_gen2) {
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L1, port_type_busB_L1, bandwidth, 0),
                "fail to create client_b_L1");
        } else {
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, bandwidth, 0),
                "fail to create client_c");
        }

        /* Create BusB L2 client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, bandwidth, 0),
            "fail to create client_b_L2");
    }

    /* Disable BusB clients */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L1, 0), "fail to disable client_b_L1");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 0), "fail to disable client_b_L2");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_L1, 0), "fail to disable eth_port_L1");
        if (dst_clientB_ori > 0) {
            /* Disable the original dst_client in BusB */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_clientB_ori, 0), "fail to disable dst_clientB_ori");
        }
    }

    if (!clear_flag)
    {
        if (!is_flexe_gen2) {
            /** FlexE_Gen1: eth_port_L1 -> L1_switch -> client_b_L1 -> client_b_L2 > force_fwd -> eth_port_L2 */

            /** Clear original Eth2BusB FlexE flow */
            if (dst_clientB_ori > 0) {
                BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_port_L1, 0, dst_clientB_ori),
                    "fail to clear original Eth2BusB FlexE flow");
            }

            /** Create above datapath **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, eth_port_L1, 0, client_b_L1),
                "fail in setting FlexE flow(client_b_L2 to client_b_L1)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, client_b_L2),
                "fail in setting FlexE flow(client_b_L1 to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_L2, eth_port_L2, 1),
                "fail in force_fwd setting(client_b_L2 to eth_port_L2)");
        } else {
            /** FlexE_Gen2: eth_port_L1 -> client_c -> client_c -> client_b_L2 -> eth_port_L2 */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, eth_port_L1, 0, client_c),
                "fail in setting FlexE flow(eth_port_L1 to client_c)");
            BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example(unit, client_c, client_c, 0, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size, tdm_mc_id), "");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_c, 0, client_b_L2),
                "fail in setting FlexE flow(client_c to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_L2, eth_port_L2, 1),
                "fail in force_fwd setting(client_b_L2 to eth_port_L2)");
        }
    }
    else
    {
        if (!is_flexe_gen2) {
            /** Clear FlexE_Gen1 datapath **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_port_L1, 0, client_b_L1),
                "fail in setting FlexE flow(client_b_L2 to client_b_L1)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_L1, 0, client_b_L2),
                "fail in setting FlexE flow(client_b_L1 to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_L2, eth_port_L2, 0),
                "fail in force_fwd setting(client_b_L2 to eth_port_L2)");

            /** Restore original Eth2BusB FlexE flow **/
            if (dst_clientB_ori > 0) {
                BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, eth_port_L1, 0, dst_clientB_ori),
                    "fail to restore original Eth2BusB FlexE flow");
            }
        } else {
            /** Clear FlexE_Gen2 datapath **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_port_L1, 0, client_c),
                "fail in setting FlexE flow(eth_port_L1 to client_c)");
            /* Add steps to clear TDM datapath client_c -> client_c */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_c, 0, client_b_L2),
                "fail in setting FlexE flow(client_c to client_b_L2)");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_L2, eth_port_L2, 0),
                "fail in force_fwd setting(client_b_L2 to eth_port_L2)");
        }
    }

    /* Enable BusB clients */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L1, 1), "fail to enable client_b_L1");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 1), "fail to enable client_b_L2");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_L1, 1), "fail to enable eth_port_L1");
        if (dst_clientB_ori > 0) {
            /* Enable original dst_client in BusB */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_clientB_ori, 1), "fail to enable dst_clientB_ori");
        }
    }

    /* Remove BusB clients */
    if (clear_flag) {
        if (!is_flexe_gen2) {
            snprintf(error_msg, sizeof(error_msg), "(client %d)", client_b_L1);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_L1), error_msg);
        } else {
            snprintf(error_msg, sizeof(error_msg), "(client %d)", client_c);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_c), error_msg);
        }
        snprintf(error_msg, sizeof(error_msg), "(client %d)", client_b_L2);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_L2), error_msg);
    }

    return BCM_E_NONE;
}

int dnx_flexe_util_mod_port_get(
    int unit,
    bcm_port_t logic_port_id,
    bcm_gport_t * mod_gport)
{

    /* Get the module port of SAR or ILKN_TDM */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_gport_get(unit, logic_port_id, mod_gport), "");

    return BCM_E_NONE;
}

/* Funtion to get unit id and logical port id according to mod_gport
 * Please note that flexe_gen2_fap0 and flexe_gen2_fap1 should be the same device type
 */
int dnx_flexe_util_mod_port_info_get(
    bcm_gport_t mod_gport,
    int * unit_id,
    bcm_port_t * logic_port_id)
{
    int mod_id;
    int nof_cores_per_unit = *(dnxc_data_get(flexe_gen2_fap0_id, "device", "general", "nof_cores", NULL));
    int nof_faps_per_core = *(dnxc_data_get(flexe_gen2_fap0_id, "device", "general", "max_nof_fap_ids_per_core", NULL));
    int total_faps_per_unit = nof_cores_per_unit * nof_faps_per_core;

    if (BCM_GPORT_IS_MODPORT(mod_gport)) {
        mod_id = BCM_GPORT_MODPORT_MODID_GET(mod_gport);
    } else {
        print "dnx_flexe_util_mod_port_info_get: ERROR - mod_gport isn't a mod gport";
        return BCM_E_FAIL;
    }

    *unit_id = mod_id/total_faps_per_unit + flexe_gen2_fap0_id;
    *logic_port_id = BCM_GPORT_MODPORT_PORT_GET(mod_gport);

    return BCM_E_NONE;
}

/* Function to set/clear client to client L1 FlexE flow */
int dnx_flexe_util_L1_datapath_client_to_client(
    int unit,
    bcm_port_t src_client_id,
    bcm_port_t dst_client_id,
    int clear_flag)
{
    int i;
    int not_busB_L1 = 1; /* If 0, indicate the client is BusB L1 client */
    int not_busB_L2 = 1; /* If 0, indicate the client is BusB L2&L3 client */
    uint32 flags = 0;
    bcm_port_t client_id[2] = {src_client_id, dst_client_id};
    char error_msg[200]={0,};


    /* Disable BusB client before FlexE flow setting&clearing */
    for (i = 0; i < 2; i++) {
        if (!dnx_flexe_gen2_support(unit, -1)) {
            not_busB_L1 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L1);
        }
        not_busB_L2 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L2_L3);
        if (not_busB_L1 == 0 || not_busB_L2 == 0) {
            /* Disbale the port if it is BusB L1/L2/L3 client */
            snprintf(error_msg, sizeof(error_msg), "fail to disable BusB client %d", client_id[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id[i], 0), error_msg);
        }
    }

    /* Create or clear FlexE client to client L1 switch within FlexE core */
    if (!clear_flag)
    {
        /* Set FlexE client to client flow */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, src_client_id, failover_channel_id, dst_client_id),
            "fail in setting FlexE2FlexE L1 datapath");
    }
    else
    {
        /* Clear above client to client datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, src_client_id, failover_channel_id, dst_client_id),
            "fail in clearing FlexE2FlexE L1 datapath");
    }

    /* Enable BusB client after FlexE flow setting&clearing */
    for (i = 0; i < 2; i++) {
        if (!dnx_flexe_gen2_support(unit, -1)) {
            not_busB_L1 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L1);
        }
        not_busB_L2 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L2_L3);
        if (not_busB_L1 == 0 || not_busB_L2 == 0) {
            /* Enable the port if it is BusB L1/L2/L3 client */
            snprintf(error_msg, sizeof(error_msg), "fail to enable BusB client %d", client_id[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id[i], 1), error_msg);
        }
    }

    return BCM_E_NONE;
}

/* Function to set/clear FlexE to ETH L1 datapath */
int dnx_flexe_util_L1_datapath_flexe_to_eth(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busB,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;


    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L1), "Mismached port type");

    /* Disable BusB client */
    if (!dnx_flexe_gen2_support(unit, -1)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), "fail to disable BusB client and ETH port");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_eth_port, 0), "fail to disable BusB client and ETH port");

    /*
     * Start to create/clear L1 datapath: FlexE -> ETH
     */
    if (!clear_flag)
    {
        if (dnx_flexe_gen2_support(unit, -1)) {
            /** FlexE_Gen2: FlexE(client_id_busA) -> Framer Bypass -> ETH(dst_eth_port) */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busA, failover_channel_id, dst_eth_port),
                "fail in setting BusA2Eth L1 datapath");
        } else {
            /** FlexE_Gen1: FlexE(client_id_busA) -> TinyMAC(client_id_busB) -> ESB(From_FlexE) -> ETH(dst_eth_port) */

            /** Step1. set L1 datapath from FlexE to TinyMAC(client_id_busA -> client_id_busB) **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busA, failover_channel_id, client_id_busB),
                "fail in setting BusA2BusB L1 datapath");
            
            /** Step2. set L1 datapath from TinyMAC to ETH(client_id_busB -> ESB(From_FlexE) -> dst_eth_port) **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, dst_eth_port),
                "fail in setting BusB2Eth L1 datapath");
        }
    }
    else
    {
        /** Clear above FlexE to ETH port */
        if (dnx_flexe_gen2_support(unit, -1)) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busA, failover_channel_id, dst_eth_port),
                "fail in clearing BusA2Eth L1 datapath");
        } else {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, 0, dst_eth_port),
                "fail in clearing BusB2Eth L1 datapath");
            
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busA, failover_channel_id, client_id_busB),
                "fail in clearing BusA2BusB L1 datapath");
        }
    }

    /* Enable BusB client */
    if (!dnx_flexe_gen2_support(unit, -1)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), "fail to enable BusB client and ETH port");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_eth_port, 1), "fail to enable BusB client and ETH port");

    return BCM_E_NONE;
}

/* Function to set/clear ETH to FlexE L1 datapath */
int dnx_flexe_util_L1_datapath_eth_to_flexe(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i;
    uint32 flags = 0;


    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA), "Mismached port type");

    /* Disable BusB client */
    if (!dnx_flexe_gen2_support(unit, -1)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), "fail to disable BusB client and ETH port");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_eth_port, 0), "fail to disable BusB client and ETH port");

    /*
     * Start to create/clear L1 datapath: ETH -> FlexE
     * The internal datapath is:
     * ETH(src_eth_port) -> IRE -> TinyMAC(client_id_busB) -> FlexE(client_id_busA)
     */
    if (!clear_flag)
    {
        if (dnx_flexe_gen2_support(unit, -1)) {
            /** FlexE_Gen2: ETH(src_eth_port) -> Framer Bypass -> FlexE(client_id_busA) */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, src_eth_port, failover_channel_id, client_id_busA),
                "fail in setting Eth2BusA L1 datapath");
        } else {
            /** FlexE_Gen1: ETH(src_eth_port) -> IRE -> TinyMAC(client_id_busB) -> FlexE(client_id_busA) */

            /** Step1. set L1 datapath from ETH to TinyMAC(src_eth_port -> IRE -> client_id_busB) **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, src_eth_port, 0, client_id_busB),
                "fail in setting Eth2BusB L1 datapath");
            
            /** Step2. set L1 datapath from TinyMAC to FlexE(client_id_busB -> client_id_busA) **/
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, failover_channel_id, client_id_busA),
                "fail in setting BusB2BusA L1 datapath");
        }
    }
    else
    {
        /** Clear above L1 datapath */
        if (dnx_flexe_gen2_support(unit, -1)) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, src_eth_port, failover_channel_id, client_id_busA),
                "fail in clearing Eth2BusA L1 datapath");
        } else {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, failover_channel_id, client_id_busA),
                "fail in clearing BusB2BusA L1 datapath");
            
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, src_eth_port, 0, client_id_busB),
                "fail in clearing Eth2BusB L1 datapath");
        }
    }

    /* Enable BusB client */
    if (!dnx_flexe_gen2_support(unit, -1)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), "fail to enable BusB client and ETH eth port");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_eth_port, 1), "fail to enable BusB client and ETH eth port");

    return BCM_E_NONE;
}

/* Function to set/clear L1 datapath: FlexE->SAR->Dst_Port(local or remote)
 * dest_port: For FlexE_Gen1, it should be ILKN_L1 port 
 *            For FlexE_Gen2, it can be SAR client or ILKN_L1 port
 *            It should be a modPort if dest_port is a remote port on another unit(FlexE_Gen2 only)
 * This function is reused by dnx_flexe_util_L1_datapath_eth_sar_dst() as it has similar datapath
 */
int dnx_flexe_util_L1_datapath_flexe_sar_dst(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busC,
    bcm_port_t dest_port,
    int clear_flag)
{
    int i;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    uint32 flags = 0;
    int dst_unit = unit;
    int dst_logic_port = dest_port;
    int flexe_client_check_result; /* 0-Is FlexE(BusA) client, other values: Not FlexE(BusA) client */
    int eth_L1_check_result; /* 0-Is ETH L1 port, other values: Not ETH L1 port */

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_flexe_sar_dst";

    if (BCM_GPORT_IS_MODPORT(dest_port)) {
        /* Get dst unit id and its logical port */
        dnx_flexe_util_mod_port_info_get(dest_port, &dst_unit, &dst_logic_port);
        printf("**** %s: dest_port %d is a modPort at unit %d(port id %d) ****\n", proc_name, dest_port, dst_unit, dst_logic_port);
    }

    /* Port type check */
    flexe_client_check_result = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    eth_L1_check_result = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_eth_L1);
    if (is_flexe_gen2) {
        /** FlexE_Gen2,  src_port(client_id_busA) can be ETH_L1 or BusA client */
        if (flexe_client_check_result != 0 && eth_L1_check_result != 0) {
            printf("**** %s: Error - src_port %d isn't ETH_L1 or BusA client ****\n", proc_name, client_id_busA);
            return BCM_E_FAIL;
        }
    } else {
        /** FlexE_Gen1,  src_port(client_id_busA) should be BusA client */
        if (flexe_client_check_result != 0) {
            printf("**** %s: Error - src_port %d isn't BusA client ****\n", proc_name, client_id_busA);
            return BCM_E_FAIL;
        }
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC), "Mismached port type");
    if (!is_flexe_gen2) {
        /* For FlexE_Gen1, the dest_port must be ILKN_L1 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dest_port, port_type_ilkn_L1), "Mismached port type");
    }

    /* Disable ports */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(dst_unit, dst_logic_port, 0), "fail to disable dest_port");
    }
      
    /*
     * Start to create/clear L1 datapath: FlexE->SAR->Dst_Port(local or remote)
     */
    if (!clear_flag)
    {
        if (is_flexe_gen2) {
            /** FlexE_Gen2: FlexE(client_id_busA) -> SAR(client_id_busC) --TDM_Bypass--> dest_port */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busA, failover_channel_id, client_id_busC),
                "fail in setting BusA2BusC L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example_multi_device(unit, dst_unit, client_id_busC, dest_port, flexe_gen2_tdm_is_mc, flexe_gen2_tdm_grp_id, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size), "");
        } else {
            /** FlexE_Gen1: FlexE(client_id_busA) -> SAR(client_id_busC) -> ILKN(dest_port) */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busA, failover_channel_id, client_id_busC),
                "fail in setting BusA2BusC L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busC, 0, dest_port),
                "fail in setting BusC2ILKN L1 datapath");
        }
    }
    else
    {
        if (is_flexe_gen2) {
            /* WARNING: Need to confirm TDM clear cint here */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busA, failover_channel_id, client_id_busC),
                "fail in clearing BusA2BusC L1 datapath");
        } else {
            /* Clear above L1 datapath */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busC, 0, dest_port),
                "fail in clearing BusC2ILKN L1 datapath");
            
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busA, failover_channel_id, client_id_busC),
                "fail in clearing BusA2BusC L1 datapath");
        }
    }

    /* Enable ports */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(dst_unit, dst_logic_port, 1), "fail to enable dest_port");
    }

    return BCM_E_NONE;
}

/* Function to set/clear L1 datapath: Src_Port->SAR->FlexE (the SAR&FlexE can be remote clients)
 * src_port:        For FlexE_Gen1, it should be ILKN_L1 port
 *                  For FlexE_Gen2, it can be SAR client or ILKN_L1 port
 * client_id_busC:  If SAR&FlexE clients locate at remote device, it should be a modPort
 * This function is reused by dnx_flexe_util_L1_datapath_src_sar_eth() as it has similar datapath
 */
int dnx_flexe_util_L1_datapath_src_sar_flexe(
    int unit,
    bcm_port_t src_port,
    bcm_port_t client_id_busC,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int dst_unit = unit;
    int dst_sar_client = client_id_busC;
    int flexe_client_check_result; /* 0-Is FlexE(BusA) client, other values: Not FlexE(BusA) client */
    int eth_L1_check_result; /* 0-Is ETH L1 port, other values: Not ETH L1 port */

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_src_sar_flexe";

    if (BCM_GPORT_IS_MODPORT(client_id_busC)) {
        /* Get dst unit and its logical port */
        dnx_flexe_util_mod_port_info_get(client_id_busC, &dst_unit, &dst_sar_client);
        printf("**** %s: client_id_busC %d is a modPort at unit %d(port id %d) ****\n", proc_name, client_id_busC, dst_unit, dst_sar_client);
    }

    /* Port type check */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_port, port_type_ilkn_L1), "Mismached port type");
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(dst_unit, dst_sar_client, port_type_busC), "Mismached port type");
    flexe_client_check_result = dnx_flexe_util_port_type_verify(dst_unit, client_id_busA, port_type_busA);
    eth_L1_check_result = dnx_flexe_util_port_type_verify(dst_unit, client_id_busA, port_type_eth_L1);
    if (is_flexe_gen2) {
        /** FlexE_Gen2,  dst_port(client_id_busA) can be ETH_L1 or BusA client */
        if (flexe_client_check_result != 0 && eth_L1_check_result != 0) {
            printf("**** %s: Error - dst_port %d isn't ETH_L1 or BusA client ****\n", proc_name, client_id_busA);
            return BCM_E_FAIL;
        }
    } else {
        /** FlexE_Gen1,  dst_port(client_id_busA) should be BusA client */
        if (flexe_client_check_result != 0) {
            printf("**** %s: Error - dst_port %d isn't BusA client ****\n", proc_name, client_id_busA);
            return BCM_E_FAIL;
        }
    }

    /* Disable ports */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_port, 0), "fail to disable src_port");
    }

    /*
     * Start to create/clear L1 datapath: Src_Port -> SAR -> FlexE
     */
    if (!clear_flag)
    {
        if (is_flexe_gen2) {
            /** FlexE_Gen2: src_port(SAR/ILKN) --TDM_Bypass--> SAR(client_id_busC) --> FlexE(client_id_busA) */
            BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example_multi_device(unit, dst_unit, src_port, client_id_busC, flexe_gen2_tdm_is_mc, flexe_gen2_tdm_grp_id, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size), "");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(dst_unit, flags, dst_sar_client, failover_channel_id, client_id_busA),
                "fail in setting BusC2BusA L1 datapath");
        } else {
            /** FlexE_Gen1: ILKN(src_port) -> IRE -> SAR(client_id_busC) -> FlexE(client_id_busA) */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, src_port, 0, client_id_busC),
                "fail in setting ILKN2BusC L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busC, failover_channel_id, client_id_busA),
                "fail in setting BusC2BusA L1 datapath");
        }
    }
    else
    {
        if (is_flexe_gen2) {
            /* WARNING: Need to confirm TDM clear cint here */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(dst_unit, dst_sar_client, failover_channel_id, client_id_busA),
                "fail in clearing BusC2BusA L1 datapath");
        } else {
            /* Clear above L1 datapath */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busC, failover_channel_id, client_id_busA),
                "fail in clearing BusC2BusA L1 datapath");
            
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, src_port, 0, client_id_busC),
                "fail in clearing ILKN2BusC L1 datapath");
        }
    }

    /* Enable ports */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_port, 1), "fail to enable src_port");
    }

    return BCM_E_NONE;
}

/* Function to set/clear L1 datapath: ETH_L1->SAR->Dst_Port(local or remote)
 * dest_port: It can be SAR client or ILKN_L1 port
 *            It should be a modPort if dest_port is a remote port on another unit
 */
int dnx_flexe_util_L1_datapath_eth_sar_dst(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busC,
    bcm_port_t dest_port,
    int clear_flag)
{
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_eth_sar_dst";

    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1  ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* The datapath is similar with FlexE->SAR->TDM_Bypass->Dst_Port(local or remote)
     * Reuse dnx_flexe_util_L1_datapath_flexe_sar_dst() */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, src_eth_port, client_id_busC, dest_port, clear_flag), "");
    return BCM_E_NONE;
}

/* Function to set/clear L1 datapath: Src_Port->SAR->ETH (the SAR&ETH can be on remote device)
 * src_port:       It can be SAR client or ILKN_L1 port
 * client_id_busC: If SAR&ETH locate at remote device, it should be a modPort
 */
int dnx_flexe_util_L1_datapath_src_sar_eth(
    int unit,
    bcm_port_t src_port,
    bcm_port_t client_id_busC,
    bcm_port_t eth_port,
    int clear_flag)
{
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_src_sar_eth";

    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1  ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* The datapath is similar with Src_Port->SAR->FlexE
     * Reuse dnx_flexe_util_L1_datapath_src_sar_flexe() */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, src_port, client_id_busC, eth_port, clear_flag), "");
    return BCM_E_NONE;
}

/* Function to set/clear ETH to ILKN L1 datapath */
int dnx_flexe_util_L1_datapath_eth_to_ilkn(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busC,
    bcm_port_t dst_ilkn_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_eth_to_ilkn";

    if (is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen2. Please use dnx_flexe_util_L1_datapath_eth_sar_dst() instead  ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L1), "Mismached port type");

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_eth_port, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_ilkn_port, 0), "fail to disable ports");

    /*
     * Start to create/clear L1 datapath: ETH -> ILKN
     * The internal datapath is:
     * ETH(src_eth_port) -> IRE -> TinyMAC(client_id_busB) -> SAR(client_id_busC) -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from ETH to TinyMAC(src_eth_port -> IRE -> client_id_busB) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, src_eth_port, 0, client_id_busB),
            "fail in setting Eth2BusB L1 datapath");

        /** Step2. set L1 datapath from TinyMAC to SAR(client_id_busB -> client_id_busC) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, failover_channel_id, client_id_busC),
            "fail in setting BusB2BusC L1 datapath");

        /** Step3. set L1 datapath from SAR to ILKN(client_id_busC -> dst_ilkn_port) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busC, 0, dst_ilkn_port),
            "fail in setting BusC2ILKN L1 datapath");
    }
    else
    {
        /* Clear above L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busC, 0, dst_ilkn_port),
            "fail in clearing BusC2ILKN L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, failover_channel_id, client_id_busC),
            "fail in clearing BusB2BusC L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, src_eth_port, 0, client_id_busB),
            "fail in clearing Eth2BusB L1 datapath");
    }

    /* Enable BusB client */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_eth_port, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_ilkn_port, 1), "fail to enable ports");

    return BCM_E_NONE;
}

/* Function to set/clear ILKN to ETH L1 datapath */
int dnx_flexe_util_L1_datapath_ilkn_to_eth(
    int unit,
    bcm_port_t src_ilkn_port,
    bcm_port_t client_id_busC,
    bcm_port_t client_id_busB,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_ilkn_to_eth";

    if (is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen2. Please use dnx_flexe_util_L1_datapath_src_sar_eth() instead  ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L1), "Mismached port type");

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_ilkn_port, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_eth_port, 0), "fail to disable ports");

    /*
     * Start to create/clear L1 datapath: ILKN -> ETH
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> IRE -> SAR(client_id_busC) -> TinyMAC(client_id_busB) -> ESB(From_FlexE) -> ETH
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from ILKN to SAR(src_ilkn_port -> IRE -> client_id_busC) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, src_ilkn_port, 0, client_id_busC),
            "fail in setting ILKN2BusC L1 datapath");

        /** Step2. set L1 datapath from SAR to TinyMAC(client_id_busC -> client_id_busB) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busC, failover_channel_id, client_id_busB),
            "fail in setting BusC2BusB L1 datapath");

        /** Step3. set L1 datapath from TinyMAC to ETH(client_id_busB -> ESB(From_FlexE) -> dst_eth_port) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, dst_eth_port),
            "fail in setting BusB2Eth L1 datapath");
    }
    else
    {
        /* Clear above L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, 0, dst_eth_port),
            "fail in clearing BusB2Eth L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busC, failover_channel_id, client_id_busB),
            "fail in clearing BusC2BusB L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, src_ilkn_port, 0, client_id_busC),
            "fail in clearing ILKN2BusC L1 datapath");
    }

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), "fail to enbale ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_ilkn_port, 1), "fail to enbale ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_eth_port, 1), "fail to enbale ports");

    return BCM_E_NONE;
}

/* Function to set/clear FlexE to ILKN L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busB,
    bcm_port_t dst_ilkn_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn";

    if (is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen2 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L2_L3), "Mismached port type");

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_ilkn_port, 0), "fail to disable ports");

    /*
     * Start to create/clear L2&L3 datapath: FlexE -> ILKN
     * The internal datapath is:
     * FlexE(client_id_busA) -> flow_set -> TinyMAC(client_id_busB) -> flow_set -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from FlexE to TinyMAC(client_id_busA -> flow_set -> client_id_busB) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busA, 0, client_id_busB),
            "fail in setting BusA2BusB L1 datapath");

        /** Step2. set L1 datapath from BusB to ILKN(client_id_busB -> flow_set -> dst_ilkn_port **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, dst_ilkn_port),
            "fail in setting BusB2ILKN L1 datapath");
    }
    else
    {
        /* Clear FlexE -> ILKN L2/L3 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, 0, dst_ilkn_port),
            "fail in clearing BusB2ILKN L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busA, 0, client_id_busB),
            "fail in clearing BusA2BusB L1 datapath");
    }

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_ilkn_port, 1), "fail to enable ports");

    return BCM_E_NONE;
}

/* Function to set/clear ILKN to FlexE L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(
    int unit,
    bcm_port_t src_ilkn_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe";

    if (is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen2 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA), "Mismached port type");

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_ilkn_port, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), "fail to disable ports");

    /*
     * Start to create/clear L2&L3 datapath: ILKN -> FlexE
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> force_forward -> TinyMAC(client_id_busB) -> FlexE(client_id_busA)
     */
    if (!clear_flag)
    {
        /** Step1. Force foward all packets on src_ilkn_port to TinyMAC(client_id_busB) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, src_ilkn_port, client_id_busB, 1),
            "fail in bcm_port_force_forward_set(ILKN to TinyMAC)");

        /** Step2. set L1 datapath from TinyMAC to FlexE(client_id_busB -> client_id_busA) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, client_id_busA),
            "fail in setting BusB2BusA L1 datapath");
    }
    else
    {
        /* Clear above L2&L3 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, 0, client_id_busA),
            "fail in clearing BusB2BusA L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, src_ilkn_port, client_id_busB, 0),
            "fail in bcm_port_force_forward_set(ILKN to TinyMAC)");
    }

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, src_ilkn_port, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), "fail to enable ports");

    return BCM_E_NONE;
}

/* Function to set/clear ETH to ILKN L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_eth_to_ilkn(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busB0,
    bcm_port_t client_id_busB1,
    bcm_port_t dst_ilkn_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_eth_to_ilkn";

    if (is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen2 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB0, port_type_busB_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB1, port_type_busB_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L2_L3), "Mismached port type");

    /* Disable ports*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB0, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB1, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_ilkn_port, 0), "fail to disable ports");

    /*
     * Start to create/clear L2&L3 datapath: ETH -> ILKN
     * The internal datapath is:
     * ETH(src_eth_port) -> force_fwd -> TinyMAC(client_id_busB0) -> TinyMAC(client_id_busB1) -> flow_set -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. Force foward all packets on src_eth_port to TinyMAC(client_id_busB0) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, src_eth_port, client_id_busB0, 1),
            "fail in bcm_port_force_forward_set(ETH to TinyMAC)");

        /** Step2. Switch the traffic from BusB Tx to BusB Rx(client_id_busB0 -> client_id_busB1) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB0, 0, client_id_busB1),
            "fail in setting BusB2BusB L1 datapath");

        /** Step3. set L1 datapath from TinyMAC to ILKN(client_id_busB1 -> flow_set -> dst_ilkn_port) **/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB1, 0, dst_ilkn_port),
            "fail in setting BusB2ILKN L1 datapath");
    }
    else
    {
        /* Clear above L2/L3 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB1, 0, dst_ilkn_port),
            "fail in clearing BusB2ILKN L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB0, 0, client_id_busB1),
            "fail in clearing BusB2BusB L1 datapath");

        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, src_eth_port, client_id_busB0, 0),
            "fail in bcm_port_force_forward_set(ETH to TinyMAC)");
    }

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB0, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB1, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, dst_ilkn_port, 1), "fail to enable ports");

    return BCM_E_NONE;
}

/* Function to set/clear ILKN to ETH L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_eth(
    int unit,
    bcm_port_t src_ilkn_port,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_eth";

    if (is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen2 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L2_L3), "Mismached port type");

    /*
     * Start to create/clear L2&L3 datapath for ILKN -> ETH
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> IRE -> Q2A PP -> ETH(dst_eth_port)
     */

    /** Step1. Force foward all packets on src_ilkn_port to dst_eth_port **/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, src_ilkn_port, dst_eth_port, !clear_flag),
        "fail in bcm_port_force_forward_set(ILKN to ETH)");

    return BCM_E_NONE;
}

/* Function to set/clear FlexE to ETH L2 datapath */
int dnx_flexe_util_L2_datapath_flexe_to_eth(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busB,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    char error_msg[200]={0,};


    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L2_L3), "Mismached port type");

    /* The L2 datapath is:
     * FlexE(client_id_busA) -> TinyMAC(client_id_busB) -> force_forward -> ETH(dst_eth_port) */

    /* Disable BusB client */
    snprintf(error_msg, sizeof(error_msg), "fail to disable BusB client %d", client_id_busB);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), error_msg);

    /* Step1. Set or clear L1 datapath from FlexE to TinyMAC(client_id_busA -> client_id_busB) */
    if (!clear_flag)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busA, 0, client_id_busB),
            "fail in setting BusA2BusB L1 datapath");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busA, 0, client_id_busB),
            "fail in clearing BusA2BusB L1 datapath");
    }

    /* Enable BusB client */
    snprintf(error_msg, sizeof(error_msg), "fail to enable BusB client %d", client_id_busB);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), error_msg);

    /* Step2. Set L2 datapath from TinyMAC to ETH(client_id_busB -> force_forward -> dst_eth_port) */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_id_busB, dst_eth_port, !clear_flag),
        "fail in bcm_port_force_forward_set(BusB2Eth)");

    return BCM_E_NONE;
}

/* Function to set/clear ETH to FlexE L2 datapath */
int dnx_flexe_util_L2_datapath_eth_to_flexe(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    char error_msg[200]={0,};


    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L2_L3),
        "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3),
        "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA), "Mismached port type");

    /* The L2 datapath is:
     * ETH(src_eth_port) -> force_forward -> TinyMAC(client_id_busB) -> FlexE(client_id_busA) */

    /* Disable BusB client */
    snprintf(error_msg, sizeof(error_msg), "fail to disable BusB client %d", client_id_busB);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 0), error_msg);

    /* Step1. Set or clear L1 datapath from TinyMAC to FlexE(client_id_busB -> client_id_busA) */
    if (!clear_flag)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, client_id_busA),
            "fail in setting BusB2BusA L1 datapath");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_id_busB, 0, client_id_busA),
            "fail in clearing BusB2BusA L1 datapath");
    }

    /* Enable BusB client */
    snprintf(error_msg, sizeof(error_msg), "fail to enable BusB client %d", client_id_busB);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id_busB, 1), error_msg);

    /* Step2. Set or clear L2 datapath from ETH to TinyMAC */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, src_eth_port, client_id_busB, !clear_flag),
        "fail in bcm_port_force_forward_set(Eth2BusB)");

    return BCM_E_NONE;
}

/* Function to set/clear L2 datapath: FlexE->TMAC->Dst_Port(local or remote)
 * dst_port: It can be ETH_L2, ILKN_L2 or FlexE TMAC on local/remote unit
 */
int dnx_flexe_util_L2_datapath_flexe_to_dst(
    int src_unit,
    bcm_port_t flexe_client,
    bcm_port_t flexe_tmac,
    int dst_unit,
    bcm_port_t dst_port,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    bcm_gport_t dst_sys_gport;
    int is_flexe_gen2 = dnx_flexe_gen2_support(src_unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_datapath_flexe_to_dst";

    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1  ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get system GPORT of dst_port */
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(dst_unit, dst_port, &dst_sys_gport), "");

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(src_unit, flexe_client, port_type_busA), "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(src_unit, flexe_tmac, port_type_busB), "Mismached port type");

    /*
     * Start to build/clear datapath:
     * src_unit.flexe_client -> Framer_bypass -> src_unit.flexe_tmac -> force_forward -> dst_unit.dst_port
     */

    if (!clear_flag)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(src_unit, flags, flexe_client, failover_channel_id, flexe_tmac), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(src_unit, flexe_client, failover_channel_id, flexe_tmac), "");
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(src_unit, flexe_tmac, dst_sys_gport, !clear_flag), "");

    return BCM_E_NONE;
}

/* Function to set/clear L2 datapath: Src_Port->TMAC->FlexE(local or remote)
 * src_port  : It can be ETH_L2, ILKN_L2 or FlexE TMAC
 */
int dnx_flexe_util_L2_datapath_src_to_flexe(
    int src_unit,
    bcm_port_t src_port,
    int dst_unit,
    bcm_port_t dst_flexe_tmac,
    bcm_port_t dst_flexe_client,
    int clear_flag)
{
    int i;
    uint32 flags = 0;
    bcm_gport_t dst_sys_gport;
    int is_flexe_gen2 = dnx_flexe_gen2_support(src_unit, -1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_datapath_src_to_flexe";

    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1  ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get system GPORT of dst_flexe_tmac */
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(dst_unit, dst_flexe_tmac, &dst_sys_gport), "");

    /* Port type check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(dst_unit, dst_flexe_client, port_type_busA),
        "Mismached port type");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_type_verify(dst_unit, dst_flexe_tmac, port_type_busB),
        "Mismached port type");

    /*
     * Start to build/clear datapath:
     * src_unit.src_port -> force_forward -> dst_unit.dst_flexe_tmac -> Framer_bypass -> dst_unit.dst_flexe_client
     */

    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(src_unit, src_port, dst_sys_gport, !clear_flag), "");

    if (!clear_flag)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(dst_unit, flags, dst_flexe_tmac, failover_channel_id, dst_flexe_client), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(dst_unit, dst_flexe_tmac, failover_channel_id, dst_flexe_client), "");
    }

    return BCM_E_NONE;
}

/* Basic FlexE funtion test to verify bi-direction FlexE traffic */
int dnx_flexe_baisc_test_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_cen,
    bcm_port_t eth_port_dis,
    bcm_port_t phy_port)
{
    int is_flexe_gen2  = dnx_flexe_gen2_support(unit_cen, -1);
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 40;
    bcm_port_t client_b = 50;
    bcm_gport_t flexe_grp;
    int flexe_grp_bandwidth = is_flexe_gen2 ? 400000 : 100000;
    int client_speed = flexe_grp_bandwidth;
    int nof_client_slots = client_speed / 5000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    char error_msg[200]={0,};


    /***************************************************************************
     * FlexE PHY port(s) and group creation
     ***************************************************************************/

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs), error_msg);
    }

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create a busA client and a busB client on each Q2A. The client rate is 10G */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a, port_type_busA, client_speed, 0), error_msg);

        snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b, port_type_busB_L1, client_speed, 0), error_msg);
    }

    /***************************************************************************
     * Datapath creation:
     *                          Centralized Device          Centralized Device
     *                      +-----------------------+    +-----------------------+
     * eth_port_cen <--L1-->| client_b <-> client_a |<-->| client_a <-> client_b |<--L1--> eth_port_dis
     *                      +-----------------------+    +-----------------------+
     ***************************************************************************/

    /* Start to build below L1 ETH2FlexE datapath on centralized device:
     * eth_port_cen <-> client_b <-> client_a
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_cen, client_b, client_a, 0),
        "fail to set L1 datapath on Q2A_#0");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port_cen, 0),
        "fail to set L1 datapath on Q2A_#0");

    /* Start to build below L1 datapath on distributed device:
     * client_a <-> client_b <-> eth_port_dis
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_dis, client_b, client_a, 0),
        "fail to set FlexE2Eth datapath on Q2A_#1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_port_dis, 0),
        "fail to set FlexE2Eth datapath on Q2A_#1");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/
    for (i = 0; i < nof_client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_port), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to test FlexE to FlexE L1/L2/L3 switching within same FlexE group */
int dnx_flexe_centralized_datapath_flexe_to_flexe_in_1_grp_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    int fwd_type,
    bcm_port_t phy_port)
{
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    int vlan_in = 1111;
    int vlan_out = 2222;
    bcm_port_t client_a_0 = 51;
    bcm_port_t client_a_1 = 52;
    bcm_port_t client_b_0 = 61;
    bcm_port_t client_b_1 = 62;
    bcm_gport_t flexe_grp;
    flexe_port_type_t client_b_type_dis = fwd_type > 1 ? port_type_busB_L2_L3 : port_type_busB_L1;
    flexe_port_type_t client_b_types[2] = {port_type_busB_L2_L3, client_b_type_dis};
    int client_speed = 100000;
    int nof_slots_per_client = client_speed / 5000;
    int flexe_grp_bandwidth = 200000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    char error_msg[200]={0,};
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit_cen, -1);

    dnx_flexe_lane_speed_init(unit_cen, unit_dis);

    if (is_flexe_gen2 && fwd_type == 1) {
        /* FlexE_Gen2: ETH_L1.speed should equal to client_speed. Change client&group speed as 400G ETH_L1 is used for test */
        client_speed = 400000;
        nof_slots_per_client = client_speed / 5000;
        flexe_grp_bandwidth = 800000;
        nof_slots = flexe_grp_bandwidth / 5000;
    }

    /***************************************************************************
     * FlexE PHY port(s) and group creation
     ***************************************************************************/

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    if (is_reduced_cdu_lane_speed || (is_flexe_gen2 && fwd_type == 1)) {
        flexe_phy_ports[1] = phy_port+1;
        nof_pcs++;
    }
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail in dnx_flexe_util_group_create(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs), error_msg);
    }

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create 2 BusA clients and 2 BusB clients on each Q2A. The rate of each client is 10G */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_a_0);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a_0, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_a_1);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a_1, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_b_0);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b_0, client_b_types[i], client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_b_1);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b_1, client_b_types[i], client_speed, 0), error_msg);
    }

    /***************************************************************************
     * Datapath creation:
     *                             Centralized Device                     Centralized Device
     *                          +------------------------+         +--------------------------------------+
     * eth_port_tx --L1_switch->| client_b_0->client_a_0 |--grp0-->| client_a_0--->+--> client_b_0 -->+   |
     *                          |                        |         |               |                  |   |
     *                          |                        |         |               v                  V   |
     *                          |                        |         |           L1_switch            L2/L3 |
     *                          |                        |         |               |                  |   |
     *                          |                        |         |               V                  V   |
     * eth_port_rx <-L1_switch--| client_b_1<-client_a_1 |<--grp0--| client_a_1<---+<-- client_b_1 <--+   |
     *                          +------------------------+         +--------------------------------------+
     * (client_a_0 and client_a_1 are in same FlexE group 'grp0')
     ***************************************************************************/

    /* Start to build below L1 Eth2FlexE datapath on distributed Device:
     * eth_port_tx -> client_b_0 -> client_a_0
     */
    if (fwd_type == 1)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0),
            "fail to set Eth2FlexE L1 datapath on Q2A_#1");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0),
            "fail to set Eth2FlexE L2 datapath on Q2A_#1");
    }

    /* Start to build below FlexE2FlexE datapath on centralized device:
     * client_a_0 -> L1/L2/l3 -> client_a_1
     */
    if (fwd_type == 1)
    {
        /* Testing Flexe2Flexe L1 switch: The FlexE to FlexE switching is done in FlexE core */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath in unit %d", unit_cen);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_a_1, 0), error_msg);
    }
    else
    {
        /* Testing Flexe2Flexe L2/L3 switch. The datapath will be:
         * client_a_0 -> client_b_0 -> L2/L3 -> client_b_1 -> client_a_1
         * Now start to configure L1 datapath "client_a_0->client_b_0" and "client_b_1->client_a_1"
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_b_0, 0),
            "fail to set BusA2BusB L1 datapath on centralized Q2A");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_1, client_a_1, 0),
            "fail to set BusB2BusA L1 datapath on centralized Q2A");

        /* Start to do L2&L3 setting */
        if (fwd_type == 2) {
            /* L2 setting between client_b_0 and client_b_1 */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_switch_config(unit_cen, client_b_0, client_b_1, vlan_in), "");
        } else {
            /* L3 setting between client_b_0 and client_b_1 */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L3_route_config(unit_cen, client_b_0, client_b_1, vlan_in, vlan_out), "");
        }
    }

    /* Start to build below L1 FlexE2Eth datapath on distributed Device:
     * client_a_1 -> client_b_1 -> eth_port_rx
     */
    if (fwd_type == 1)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#1");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#1");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots_per_client*2; ii++) {
            Cal_Slots_A[ii] = ii < nof_slots_per_client ? client_a_0 : client_a_1;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_port), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to test FlexE to FlexE L1/L2/L3 switching within 2 FlexE groups */
int dnx_flexe_centralized_datapath_flexe_to_flexe_across_2_grps_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    int fwd_type,
    bcm_port_t phy_port_0,
    bcm_port_t phy_port_1)
{
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t phy_port[2] = {phy_port_0, phy_port_1};
    int vlan_in = 1111;
    int vlan_out = 2222;
    bcm_port_t client_a_0 = 51;
    bcm_port_t client_a_1 = 52;
    bcm_port_t client_b_0 = 61;
    bcm_port_t client_b_1 = 62;
    bcm_gport_t flexe_grp[2];
    flexe_port_type_t client_b_type_dis = fwd_type > 1 ? port_type_busB_L2_L3 : port_type_busB_L1;
    flexe_port_type_t client_b_types[2] = {port_type_busB_L2_L3, client_b_type_dis};
    int client_speed = 100000;
    int nof_slots_per_client = client_speed / 5000;
    int flexe_grp_bandwidth = 100000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    char error_msg[200]={0,};
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit_cen, -1);

    if (is_flexe_gen2 && fwd_type == 1) {
        /* FlexE_Gen2: ETH_L1.speed should equal to client_speed. Change client&group speed as 400G ETH_L1 is used for test */
        client_speed = 400000;
        nof_slots_per_client = client_speed / 5000;
        flexe_grp_bandwidth = 400000;
        nof_slots = flexe_grp_bandwidth / 5000;
    }

    /***************************************************************************
     * FlexE PHY port(s) and group creation
     ***************************************************************************/

    /* Create 2 FlexE groups with the 2 FlexE PHYs on each device */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[0], 1);
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[1], 2);
    for (i = 0; i < 2; i++) {
        for (ii = 0; ii < 2; ii++) {
            flexe_phy_ports[0] = phy_port[ii];
            snprintf(error_msg, sizeof(error_msg), "fail to create FlexE FG(unit %d, grp index %d)", unit[i], ii);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit[i], flexe_grp[ii], nof_pcs), error_msg);
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create 2 BusA clients and 2 BusB clients on each Q2A. The rate of each client is 10G */
    for (i = 0; i < 2; i++) {
        /* client_a_0 for flexe_grp[0] */
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_a_0);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a_0, port_type_busA, client_speed, 0), error_msg);
        /* client_a_1 for flexe_grp[1] */
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_a_1);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a_1, port_type_busA, client_speed, 0), error_msg);
        /* client_b_0 which is associated with client_a_0 */
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_b_0);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b_0, client_b_types[i], client_speed, 0), error_msg);
        /* client_b_1 which is associated with client_a_1 */
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_b_1);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b_1, client_b_types[i], client_speed, 0), error_msg);
    }

    /***************************************************************************
     * Datapath creation:
     *                             Centralized Device                     Centralized Device
     *                          +------------------------+         +--------------------------------------+
     * eth_port_tx --L1_switch->| client_b_0->client_a_0 |--grp0-->| client_a_0--->+--> client_b_0 -->+   |
     *                          |                        |         |               |                  |   |
     *                          |                        |         |               v                  V   |
     *                          |                        |         |           L1_switch            L2/L3 |
     *                          |                        |         |               |                  |   |
     *                          |                        |         |               V                  V   |
     * eth_port_rx <-L1_switch--| client_b_1<-client_a_1 |<--grp1--| client_a_1<---+<-- client_b_1 <--+   |
     *                          +------------------------+         +--------------------------------------+
     * (client_a_0 is in FlexE group 0. client_a_1 is in FlexE group 1)
     ***************************************************************************/

    /* Start to build below L1 Eth2FlexE datapath on distributed device:
     * eth_port_tx -> client_b_0 -> client_a_0
     */
    if (fwd_type ==  1)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0),
            "fail to set Eth2FlexE datapath on Q2A_#1");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0),
            "fail to set Eth2FlexE datapath on Q2A_#1");
    }

    /* Start to build below FlexE2FlexE datapath on centralized device:
     * client_a_0 -> L1/L2/l3 -> client_a_1
     */
    if (fwd_type ==  1)
    {
        /* L1 switch: The FlexE to FlexE switching is done in FlexE core */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath in unit %d", unit_cen);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_a_1, 0), error_msg);
    }
    else
    {
        /* L2/L3 switch: the FlexE to FlexE switching will be
         * client_a_0 -> client_b_0 -> L2/L3 -> client_b_1 -> client_a_1
         * Now start to configure L1 datapath "client_a_0->client_b_0" and "client_b_1->client_a_1" */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_b_0, 0),
            "fail to set BusA2BusB L1 datapath on centralized Q2A");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_1, client_a_1, 0),
            "fail to set BusB2BusA L1 datapath on centralized Q2A");

        /* Start to do L2&L3 setting */
        if (fwd_type == 2) {
            /* L2 setting between client_b_0 and client_b_1 */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_switch_config(unit_cen, client_b_0, client_b_1, vlan_in), "");
        } else {
            /* L3 setting between client_b_0 and client_b_1 */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L3_route_config(unit_cen, client_b_0, client_b_1, vlan_in, vlan_out), "");
        }
    }

    /* Start to build below L1 FlexE2Eth datapath on on distributed device:
     * client_a_1 -> client_b_1 -> eth_port_rx
     */
    if (fwd_type ==  1)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#1");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#1");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for client_a_0 in flexe_grp[0] */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots_per_client; ii++) {
            Cal_Slots_A[ii] =  client_a_0;
        }
        snprintf(error_msg, sizeof(error_msg), "fail to allocate slots for client_a_0 on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp[0], 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* Assign slots for client_a_1 in flexe_grp[1] */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots_per_client; ii++) {
            Cal_Slots_A[ii] =  client_a_1;
        }
        snprintf(error_msg, sizeof(error_msg), "fail to allocate slots for client_a_1 on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp[1], 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH group ID */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp[0], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp[1], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 2), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp[0]), "Synce check failed");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp[1]), "Synce check failed");

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "The 1st FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_port[0]), error_msg);

        snprintf(error_msg, sizeof(error_msg), "The 2nd FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_port[1]), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to test L1/L2/L3 datapath between ETH port and FlexE */
int dnx_flexe_centralized_datapath_between_eth_and_flexe_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    int is_L1_switch,
    bcm_port_t phy_port)
{
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit_cen, -1);
    bcm_port_t client_a_0 = 51;
    bcm_port_t client_a_1 = 52;
    bcm_port_t client_b_0 = 61;
    bcm_port_t client_b_1 = 62;
    bcm_gport_t flexe_grp;
    flexe_port_type_t client_b_type = is_L1_switch ? port_type_busB_L1 : port_type_busB_L2_L3;
    int client_speed = 100000;
    int nof_slots_per_client = client_speed / 5000;
    int flexe_grp_bandwidth = 200000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    char error_msg[200]={0,};


    dnx_flexe_lane_speed_init(unit_cen, unit_dis);

    if (is_L1_switch && is_flexe_gen2) {
        /* FlexE_Gen2: ETH_L1.speed should equal to client_speed. Change client&group speed as 400G ETH_L1 is used for test */
        client_speed = 400000;
        nof_slots_per_client = client_speed / 5000;
        flexe_grp_bandwidth = 800000;
        nof_slots = flexe_grp_bandwidth / 5000;
        nof_pcs = 2;
    }

    /***************************************************************************
     * The whole datapath is:
     *                             Centralized Device              Centralized Device
     *                          +------------------------+      +---------------------+
     * eth_port_tx --L1/L2/l3-> | client_b_0->client_a_0 |----->| client_a_0------>+  |
     *                          |                        |      |                  |  |
     * eth_port_rx <-L1/L2/l3-- | client_b_1->client_a_1 |<-----| client_a_1<------+  |
     *                          +------------------------+      +---------------------+
     ***************************************************************************/

    /***************************************************************************
     * FlexE PHY port(s) and group creation
     ***************************************************************************/

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    if (is_reduced_cdu_lane_speed || (is_L1_switch && is_flexe_gen2)) {
        flexe_phy_ports[1] = phy_port + 1;
        nof_pcs = 2;
    }
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs), error_msg);
    }

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create 2 BusA clients on each device. The rate of each client is 10G */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_a_0);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a_0, port_type_busA, client_speed, 0), error_msg);

        snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit[i], client_a_1);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a_1, port_type_busA, client_speed, 0), error_msg);
    }

    /* Create 2 BusB clients on centralized device */
    snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit_cen, client_b_0);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_0, client_b_type, client_speed, 0), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(unit %d, client %d)", unit_cen, client_b_1);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_1, client_b_type, client_speed, 0), error_msg);

    /***************************************************************************
     * Datapath creation:
     ***************************************************************************/

    /* Start to build below Eth2Flexe datapath on centralized device:
     * eth_port_tx -> L1/L2/L3 switch -> client_b_0 -> client_a_0
     */
    if (is_L1_switch)
    {
        /* Use Eth2FlexE L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b_0, client_a_0, 0),
            "fail to set Eth2FlexE L1 datapath on Q2A_#0");
    } else {
        /* Use Eth2FlexE L2&L3 datapath:
         * eth_port_tx -> L2/L3 switch -> client_b_0 -> client_a_0
         * For L2/L3 switching between Ethernet and BusB, it will be done in TCL script
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_0, client_a_0, 0),
            "fail to set BusB2BusA L2 datapath on Q2A_#0");
    }

    /* Start to build L1 FlexE2FlexE datapath on distributed device:
     * client_a_0 -> L1_switch -> client_a_1
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_0, client_a_1, 0),
        "fail to set FlexE2FlexE L1 datapath on Q2A_#1");

    /* Start to build below Flexe2Eth datapath on centralized device:
     * client_a_1 -> client_b_1-> L1/L2/L3 switch -> eth_port_rx
     */
    if (is_L1_switch)
    {
        /* Use Flexe2Eth L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_1, client_b_1, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#0");
    } else {
        /* Use Flexe2Eth L2&L3 datapath:
         * client_a_1 -> client_b_1-> L2/L3 switch -> eth_port_rx
         * For L2/L3 switching between BusB and Ethernet, it will be done in TCL script
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_1, client_b_1, 0),
            "fail to set BusA2BusB L2 datapath on Q2A_#0");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots_per_client*2; ii++) {
            Cal_Slots_A[ii] = ii < nof_slots_per_client ? client_a_0 : client_a_1;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* Group ID OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_port), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to test L1/L2/L3 datapath between FlexE and ILKN */
int dnx_flexe_distributed_datapath_between_flexe_and_ilkn_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    bcm_port_t ilkn_port,
    int is_L1_switch_cen,
    int is_L1_switch_dis,
    bcm_port_t phy_port)
{
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 51;
    bcm_port_t client_b = 52;
    bcm_port_t client_c = 53;
    bcm_gport_t flexe_grp;
    int client_speed = 100000;
    int nof_slots_per_client = client_speed / 5000;
    int flexe_grp_bandwidth = 200000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    flexe_port_type_t client_b_type_cen = is_L1_switch_cen ? port_type_busB_L1 : port_type_busB_L2_L3;
    flexe_port_type_t client_b_type_dis = port_type_busB_L2_L3;
    flexe_port_type_t client_b_type[2] = {client_b_type_cen, client_b_type_dis};
    char error_msg[200]={0,};


    /***************************************************************************
     * FlexE PHY port(s) and group creation
     ***************************************************************************/

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs), error_msg);
    }

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create a busA client and a busB client on each Q2A. The client rate is 10G */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b, client_b_type[i], client_speed, 0), error_msg);
    }

    /* Create busC client on distributed Q2A */
    snprintf(error_msg, sizeof(error_msg), "fail to create busC client(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c, port_type_busC, client_speed, 0), error_msg);

    /***************************************************************************
     * The datapath is:
     *
     *                         Centralized Device                     Distributed Device
     *                      +----------------------+      +-------------------------------------+
     *                      |                      |      |                                     |
     * eth_port_tx --L1/L2->\                      |      |                                     |
     *                       \                     |      |                                     |
     *                        client_b<-->client_a | <--> | client_a<-->client_b/c<-->ilkn_port |
     *                       /                     |      | (Flexe<->ILKN L1 or L2&L3 datapath) |
     * eth_port_rx <-L1/L2--/                      |      |                                     |
     *                      |                      |      |                                     |
     *                      +----------------------+      +-------------------------------------+
     *
     ***************************************************************************/

    if (is_L1_switch_cen)
    {
        /* Build Eth2FlexE L1 datapath on centralized Device */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b, client_a, 0),
            "fail to set Eth2FlexE L1 datapath on Q2A_#0");

        /* Build FlexE2Eth L1 datapath on centralized Device */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#0");
    }
    else
    {
        /* Build Eth2FlexE L1 datapath on centralized Device */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b, client_a, 0),
            "fail to set Eth2FlexE L1 datapath on Q2A_#0");

        /* Build FlexE2Eth L1 datapath on centralized Device */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port_rx, 0),
            "fail to set FlexE2Eth datapath on Q2A_#0");
    }

    /* Build Flexe2ILKN and ILKN2Flexe L1/L2/L3 datapath on distributed Device */
    if (is_L1_switch_dis)
    {
        /* Start to build Flexe2ILKN L1 datapath:
         * client_a -> client_c -> ilkn_port
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a, client_c, ilkn_port, 0),
            "fail to set Flexe2ILKN L1 datapath on Q2A_#1");

        /* Start to build ILKN2Flexe L1 datapath:
         * ilkn_port -> client_c -> client_a
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_port, client_c, client_a, 0),
            "fail to set ILKN2Flexe L1 datapath on Q2A_#1");
    }
    else
    {
        /* Start to build Flexe2ILKN L2/L3 datapath:
         * client_a -> client_b -> ilkn_port
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a, client_b, ilkn_port, 0),
            "fail to set Flexe2ILKN datapath on Q2A_#1");

        /* Start to build ILKN2Flexe L2/L3 datapath:
         * ilkn_port -> force_fwd -> client_b -> client_a
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_b, client_a, 0),
            "fail to set ILKN2Flexe L2/L3 datapath on Q2A_#1");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots_per_client; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_port), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to test L1/L2/L3 datapath between ETH and ILKN */
int dnx_flexe_distributed_datapath_between_eth_and_ilkn_main(
    int unit,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    bcm_port_t ilkn_port,
    int is_L1_switch)
{
    int i;
    int client_speed = 100000;
    bcm_port_t client_b_0 = 51;
    bcm_port_t client_b_1 = 52;
    bcm_port_t client_c = 53;
    flexe_port_type_t client_b0_type = is_L1_switch ? port_type_busB_L1 : port_type_busB_L2_L3;
    char error_msg[200]={0,};


    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create client_b_0 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_0, client_b0_type, client_speed, 0),
        "fail to create client_b_0");

    /* Create client_b_1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_1, port_type_busB_L2_L3, client_speed, 0),
        "fail to create client_b_1");

    /* Create client_c */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed, 0),
        "fail to create client_c");

    /***************************************************************************
     * Datapath creation
     ***************************************************************************/

    if (is_L1_switch)
    {
        /* Testing Eth2ILKN and ILKN2Eth L1 datapath. The whole datapath on distributed Q2A is:
         *
         *                                                Distributed Device
         *                                +----------------------------------------------+
         *      eth_port_tx --L1_switch-->\                                              |
         *                                 client_b_0<-->client_c<-->ilkn_port(loopback) |
         *      eth_port_rx <-L1_switch---/                                              |
         *                                +----------------------------------------------+
         */

         /* Start to build below Eth2ILKN L1 datapath:
          * eth_port_tx -> client_b_0 -> client_c -> ilkn_port
          */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_ilkn(unit, eth_port_tx, client_b_0, client_c, ilkn_port, 0),
            "fail to set Eth2ILKN L1 datapath");

        /* Start to build below ILKN2Eth L1 datapath:
         * ilkn_port -> client_c -> client_b_0 -> eth_port_rx
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_ilkn_to_eth(unit, ilkn_port, client_c, client_b_0, eth_port_rx, 0),
            "fail to set ILKN2Eth L1 datapath");
    }
    else
    {
        /* Testing Eth2ILKN and ILKN2Eth L2/L3 datapath. The whole datapath on distributed Q2A is:
         *
         *                                                Distributed Device
         *                                +------------------------------------------------------+
         *      eth_port_tx --force_fwd-->| client_b_0-->client_b_1-->ilkn_port(loopback)-->--+  |
         *                                |                                                   |  |
         *      eth_port_rx <-force_fwd---| <-------------force_fwd------------------------<--+  |
         *                                +------------------------------------------------------+
         */

         /* Start to build below Eth2ILKN L2/L3 datapath:
          * eth_port_tx -> force_fwd -> client_b_0 -> client_b_1 -> ilkn_port
          */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_eth_to_ilkn(unit, eth_port_tx, client_b_0, client_b_1, ilkn_port, 0),
            "fail to set Eth2ILKN L2/l3 datapath");

        /* Start to build below ILKN2Eth L2/L3 datapath:
         * ilkn_port -> force_fwd -> eth_port_rx
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_eth(unit, ilkn_port, eth_port_rx, 0),
            "fail to set ILKN2Eth L2/L3 datapath");

        snprintf(error_msg, sizeof(error_msg), "failed to set loopback on ilkn port %d", ilkn_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, ilkn_port, 1), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to create FlexE group with specific speed and FlexE PHY(s) 
 * Need put FlexE PHYs into array flexe_phy_ports[] before calling this function
 */
int dnx_flexe_group_bandwidth_test_with_specific_phys_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_400g_cen,
    bcm_port_t eth_400g_dis,
    bcm_port_t grp_speed,
    bcm_port_t nof_phy)
{
    int i, ii;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 41;
    bcm_port_t client_b = 42;
    bcm_gport_t flexe_grp;
    int nof_slots = grp_speed / 5000;
    char error_msg[200]={0,};


    /***************************************************************************
     * The whole datapath is:
     *                            Centralized Device         Centralized Device
     *                          +---------------------+    +---------------------+
     * eth_400g_cen<-L2_switch->| client_b<->client_a |<-->| client_a<->client_b |<-L2_switch->eth_400g_dis(self looped)
     *                          +---------------------+    +---------------------+
     *
     ***************************************************************************/

    /***************************************************************************
     * FlexE configuration on centralized device
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHYs in array flexe_phy_ports[] */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_phy), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a, port_type_busA, grp_speed, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b, port_type_busB_L2_L3, grp_speed, 0), error_msg);

    /* Create ETH2FlexE L2 datapath: eth_400g_cen -> client_b -> client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_400g_cen, client_b, client_a, 0),
        "fail to set Eth2FlexE L2 datapath on centralized device");

    /* Create FlexE2Eth L2 datapath: eth_400g_cen <- client_b <- client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_400g_cen, 0),
        "fail to set FlexE2Eth L2 datapath on centralized device");

    /* Calendar setting */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

    /* FlexeOhGroupID setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE configuration on distributed device
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHYs in array flexe_phy_ports[] */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_phy), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a, port_type_busA, grp_speed, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b, port_type_busB_L2_L3, grp_speed, 0), error_msg);

    /* Create ETH2FlexE L2 datapath: eth_400g_dis -> client_b -> client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_400g_dis, client_b, client_a, 0),
        "fail to set Eth2FlexE L2 datapath on distributed device");

    /* Create FlexE2Eth L2 datapath: eth_400g_dis <- client_b <- client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_400g_dis, 0),
        "fail to set FlexE2Eth L2 datapath on distributed device");

    /* Calendar setting */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

    /* FlexeOhGroupID setting */
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Verify link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy_ports[0]), error_msg);
    }

    /* Loopback setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_400g_dis, 1), "");

    /* IPG setting on L2 ETH ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit_cen, eth_400g_cen, 0, BCM_PORT_DUPLEX_FULL, 88), "fail in bcm_port_ifg_set");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit_dis, eth_400g_dis, 0, BCM_PORT_DUPLEX_FULL, 88), "fail in bcm_port_ifg_set");

    /* Synce check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Funtion to clear all FlexE settings which are generated by 
 * dnx_flexe_group_bandwidth_test_with_specific_phys_main()
 */
int dnx_flexe_group_bandwidth_test_with_specific_phys_clear(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_400g_cen,
    bcm_port_t eth_400g_dis,
    bcm_port_t grp_speed)
{
    int i, ii;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 41;
    bcm_port_t client_b = 42;
    bcm_gport_t flexe_grp;
    int nof_slots = grp_speed / 5000;
    char error_msg[200]={0,};
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);


    /***************************************************************************
     * The whole datapath is:
     *                            Centralized Device         Centralized Device
     *                          +---------------------+    +---------------------+
     * eth_400g_cen<-L1_switch->| client_b<->client_a |<-->| client_a<->client_b |<-L1_switch->eth_400g_dis(self looped)
     *                          +---------------------+    +---------------------+
     *
     ***************************************************************************/

    /* Clear calendar settings */
    for (i = 0; i < 2; i++) {
        /* Restore all slots to 0 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);
    }

    /***************************************************************************
     * Clear FlexE configuration on centralized device
     ***************************************************************************/

    /* Clear ETH2FlexE L2 datapath: eth_400g_cen -> client_b -> client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_400g_cen, client_b, client_a, 1),
        "fail to clear Eth2FlexE L2 datapath on centralized device");

    /* Clear FlexE2Eth L2 datapath: eth_400g_cen <- client_b <- client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_400g_cen, 1),
        "fail to clear FlexE2Eth L2 datapath on centralized device");

    /* Remove FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_cen, client_a), "fail to remove client_a on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_cen, client_b), "fail to remove client_b on centralized device");

    /* Delete FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit_cen, flexe_grp), error_msg);

    /***************************************************************************
     * Clear FlexE configuration on distributed device
     ***************************************************************************/

    /* Clear ETH2FlexE L2 datapath: eth_400g_dis -> client_b -> client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_400g_dis, client_b, client_a, 1),
        "fail to clear Eth2FlexE L2 datapath on distributed device");

    /* Clear FlexE2Eth L2 datapath: eth_400g_dis <- client_b <- client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_400g_dis, 1),
        "fail to clear FlexE2Eth L2 datapath on distributed device");

    /* Remove FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_dis, client_a),
        "fail to remove client_a on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_dis, client_b),
        "fail to remove client_b on distributed device");

    /* Delete FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit_dis, flexe_grp), error_msg);

    return BCM_E_NONE;
}

/* Funtion to create FlexE L1/L2/L3 datapaths under centralized mode */
int dnx_flexe_centralized_datapath_L1_L2_L3_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t eth_port_L1,
    bcm_port_t eth_port_L2,
    bcm_port_t eth_port_L3)
{
    int i;
    int l2_vlan = 1000;
    int in_vlan = 1111;
    int out_vlan = 2222;
    bcm_mac_t mac_l2 = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_mac_t my_mac = {0x00,0x00,0x00,0x00,0x33,0x33};
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 30; /* BusA client for L1 traffic */
    bcm_port_t client_a_L2 = 31; /* BusA client for L2 traffic */
    bcm_port_t client_a_L3 = 32; /* BusA client for L3 traffic */
    bcm_port_t client_b_L1 = 33; /* BusB client for L1 traffic */
    bcm_port_t client_b_L2 = 34; /* BusB client for L2 traffic */
    bcm_port_t client_b_L3 = 35; /* BusB client for L3 traffic */


    /***************************************************************************
     * The L1/L2/L3 datapaths are:
     *
     *                      Centralized Device                  Centralized Device
     *                 +----------------------------+      +----------------------------+
     * eth_port_L1<--> | client_b_L1<-->client_a_L1 | <--> | client_a_L1<-->client_b_L1 | <--> eth_port_L1(Loopback)
     * eth_port_L2<--> | client_b_L2<-->client_a_L2 | <--> | client_a_L2<-->client_b_L2 | <--> eth_port_L2
     * eth_port_L3<--> | client_b_L3<-->client_a_L3 | <--> | client_a_L3<-->client_b_L3 | <--> eth_port_L3
     *                 +----------------------------+      +----------------------------+
     *                    (L1 Traffic: L1 switch)              (L1 Traffic: L1 switch)
     *                    (L2 Traffic: L2 lookup)              (L2 Traffic: Busb2Eth-force_fwd, Eth2Busb-L2_lookup)
     *                    (L3 Traffic: L3 route )              (L3 Traffic: Busb2Eth-force_fwd, Eth2Busb-L2_lookup)
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, 5000, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L2, port_type_busA, 5000, 0),
        "fail to create client_a_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L3, port_type_busA, 5000, 0),
        "fail to create client_a_L3");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L1, 5000, 0),
        "fail to create client_b_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L2, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L3, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L3");

    /* Create L1 datapath: eth_port_L1<- L1_switch -> client_b_L1 <--> client_a_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_L1, client_b_L1, client_a_L1, 0),
        "fail to set Eth2FlexE L1 datapath on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_L1, client_b_L1, eth_port_L1, 0),
        "fail to set FlexE2Eth L1 datapath on centralized device");

    /* Create L2 datapath: eth_port_L2<- L2_switch -> client_b_L2 <--> client_a_L2 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_switch_config(unit_cen, eth_port_L2, client_b_L2, l2_vlan),
        "fail in L2 setting on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L2, client_a_L2, 0),
        "fail to set BusB2BusA flow on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L2, client_b_L2, 0),
        "fail to set BusA2BusB flow on centralized device");

    /* Create L3 datapath: eth_port_L3<- L3_route -> client_b_L3 <--> client_a_L3 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L3_route_config(unit_cen, eth_port_L3, client_b_L3, in_vlan, out_vlan),
        "fail in L3 setting on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L3, client_a_L3, 0),
        "fail to set BusB2BusA flow on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L3, client_b_L3, 0),
        "fail to set BusA2BusB flow on centralized device");

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L1, port_type_busA, 5000, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L2, port_type_busA, 5000, 0),
        "fail to create client_a_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L3, port_type_busA, 5000, 0),
        "fail to create client_a_L3");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_L1, port_type_busB_L1, 5000, 0),
        "fail to create client_b_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_L2, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_L3, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L3");

    /* Create datapath for L1 traffic: eth_port_L1<--> client_b_L1 <--> client_a_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_L1, client_b_L1, client_a_L1, 0),
        "fail to set Eth2FlexE L1 datapath on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_L1, client_b_L1, eth_port_L1, 0),
        "fail to set FlexE2Eth L1 datapath on centralized device");

    /* Create FlexE2Eth L2 datapath:
     * client_a_L2 --> client_b_L2 --> force_fwd -> eth_port_L2
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_L2, client_b_L2, eth_port_L2, 0),
        "fail to set BusB2BusA flow on distributed device");

    /* Create FlexE2Eth L2 datapath:
     * eth_port_L2 --L2_lookup--> client_b_L2 --> client_a_L2
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L2, client_a_L2, 0),
        "fail to set BusA2BusB flow on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_advanced_set(unit_dis, eth_port_L2, client_b_L2, mac_l2, 1, l2_vlan, 1),
        "fail in L2 setting on distributed device");

    /* Create FlexE2Eth L3 datapath:
     * client_a_L3 --> client_b_L3 --> force_fwd -> eth_port_L3
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_L3, client_b_L3, eth_port_L3, 0),
        "fail to set BusB2BusA flow on distributed device");

    /* Create FlexE2Eth L3 datapath:
     * eth_port_L3 --L2_lookup--> client_b_L3 --> client_a_L3
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L3, client_a_L3, 0),
        "fail to set BusA2BusB flow on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_advanced_set(unit_dis, eth_port_L3, client_b_L3, my_mac, 1, out_vlan, 1),
        "fail in L3 setting on distributed device");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 3 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_L1;
        Cal_Slots_A[1] = client_a_L2;
        Cal_Slots_A[2] = client_a_L3;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

        /* Egress default VLAN action set */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit[i]), error_msg);
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    /* Synce check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Funtion to create FlexE L1/L2/L3 datapaths under distributed mode */
int dnx_flexe_distributed_datapath_L1_L2_L3_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t eth_L1_tx,
    bcm_port_t eth_L1_rx,
    bcm_port_t eth_L23_tx,
    bcm_port_t eth_L23_rx,
    bcm_port_t ilkn_L1,
    bcm_port_t ilkn_L23)
{
    int i;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 30;  /* BusA client for L1 traffic */
    bcm_port_t client_a_L23 = 31; /* BusA client for L2&L3 traffic */
    bcm_port_t client_b_L1 = 32;  /* BusB client for L1 traffic */
    bcm_port_t client_b_L23 = 33; /* BusB client for L2&L3 traffic */
    bcm_port_t client_c = 34;     /* BusC client for L1 traffic */


    /***************************************************************************
     * The L1/L2/L3 datapaths are:
     *
     *                       Centralized Device                       Distributed Device
     *                +------------------------------+      +------------------------------------------+
     * eth_L1_tx ---->\                              |      |                                          |
     *                 \                             |      |                                          |
     *                  client_b_L1<-->client_a_L1   | <--> | client_a_L1<-->client_c<-->ilkn_L1       |
     *                 /                             |      | (Flexe<->ILKN L1 datapath for L1 traffic)|
     * eth_L1_rx <----/                              |      |                                          |
     *                                               |      |                                          |
     * eth_L23_tx --->\                              |      |                                          |
     *                 \                             |      | (Flexe<->ILKN L2&L3 datapath )           |
     *                  client_b_L23<-->client_a_L23 | <--> | client_a_L23<-->client_b_L23<-->ilkn_L23 |
     *                 /                             |      |                                          |
     * eth_L23_rx <---/                              |      |                                          |
     *                +------------------------------+      +------------------------------------------+
     *
     * (Use force_fwd to connect Ethernet ports and BusB clients on centralized device)
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, 5000, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L23, port_type_busA, 5000, 0),
        "fail to create client_a_L23");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L23, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L23");

    /* Create L1 Tx datapath: eth_L1_tx -force_fwd-> client_b_L1 --> client_a_L1 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, eth_L1_tx, client_b_L1, 1),
        "fail in bcm_port_force_forward_set(Eth2BusB1)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L1, client_a_L1, 0),
        "fail to set BusB2BusA flow on centralized device");
    /* Create L1 Rx datapath: client_a_L1 --> client_b_L1 -force_fwd-> eth_L1_rx */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b_L1, eth_L1_rx, 1),
        "fail in bcm_port_force_forward_set(BusB2Eth1)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L1, client_b_L1, 0),
        "fail to set BusA2BusB flow on centralized device");

    /* Create L2&L3 Tx datapath: eth_L23_tx -force_fwd-> client_b_L23 --> client_a_L23 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, eth_L23_tx, client_b_L23, 1),
        "fail in bcm_port_force_forward_set(Eth2BusB2)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L23, client_a_L23, 0),
        "fail to set BusB2BusA flow on centralized device");
    /* Create L2&L3 Rx datapath: client_a_L23 --> client_b_L23 -force_fwd-> eth_L23_rx */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b_L23, eth_L23_rx, 1),
        "fail in bcm_port_force_forward_set(BusB2Eth2)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L23, client_b_L23, 0),
        "fail to set BusA2BusB flow on centralized device");

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L1, port_type_busA, 5000, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L23, port_type_busA, 5000, 0),
        "fail to create client_a_L23");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_L23, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L23");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c, port_type_busC, 5000, 0),
        "fail to create client_c");

    /* Create Rx datapath for L1 traffic: client_a_L1 --> client_c --> ilkn_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a_L1, client_c, ilkn_L1, 0),
        "fail in setting FlexE2ILKN L1 datapath on distributed device");
    /* Create Tx datapath for L1 traffic: client_a_L1 <-- client_c <-- ilkn_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_L1, client_c, client_a_L1, 0),
        "fail in setting ILKN2Flexe L1 datapath on distributed device");

    /* Create Rx datapath for L2&L3 traffic: client_a_L23 --> client_b_L23 --> ilkn_L23 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a_L23, client_b_L23, ilkn_L23, 0),
        "fail in setting FlexE2ILKN L2&L3 datapath on distributed device");
    /* Create Tx datapath for L2&L3 traffic: client_a_L23 <-- client_b_L23 <-- ilkn_L23 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_L23, client_b_L23, client_a_L23, 0),
        "fail in setting ILKN2Flexe L2&L3 datapath on distributed device");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 3 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_L1;
        Cal_Slots_A[1] = client_a_L23;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    /* Synce check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}


/* Function to test FlexE failover datapaths(from BusA to BusA/B/C) */
int dnx_flexe_failover_src_in_busA_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx_0,
    bcm_port_t eth_port_rx_1,
    bcm_port_t eth_port_rx_2,
    bcm_port_t ilkn_port)
{
    int i;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on distributed Q2A */
    bcm_port_t client_a_src = 30; /* Failover src_port in busA */
    bcm_port_t client_a_dst = 31; /* Failover dst_port in busA */
    bcm_port_t client_b_dst = 32; /* Failover dst_port in busB */
    bcm_port_t client_c_dst = 33; /* Failover dst_port in busC */
    bcm_port_t client_b_dis = 34; /* BusB client to connect ILKN and eth_port_rx_2 */

    /* Client IDs on centralized Q2A */
    bcm_port_t client_a_tx = 30; /* FlexE client which send FlexE traffic to client_a_src */
    bcm_port_t client_a_rx = 31; /* FlexE client to receive failover traffic from client_a_dst */
    bcm_port_t client_b_tx = 32; /* BusB client to connect eth_port_tx and client_a_tx */
    bcm_port_t client_b_rx = 33; /* BusB client to connect client_a_rx and eth_port_rx_0 */


    /***************************************************************************
     * The failover datapaths are:
     * ================================================
     * Stage_1. Q2A_#0(centralized)
     *    eth_port_tx -> force_fwd -> client_b_tx ->  client_a_tx -> Stage 2
     * Stage_2. Q2A_#1(Distributed)
     *                                +--> client_c_dst -> ilkn_port -loopback-> client_c_dst -> client_a_src -> Stage_3.rx2
     *    -> client_a_src --failover--|--> client_b_dst -> L1 -> eth_port_rx_1
     *                                +--> client_a_dst -> Stage_3.rx0
     * Stage_3. Q2A_#0(centralized)
     *    rx0 -> client_a_rx -> client_b_rx -> force_fwd -> eth_port_rx_0
     *    rx2 -> client_a_tx -> client_b_tx -> force_fwd -> eth_port_rx_2
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_tx, port_type_busA, 10000, 0),
        "fail to create client_a_tx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_rx, port_type_busA, 10000, 0),
        "fail to create client_a_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_tx, port_type_busB_L2_L3, 10000, 0),
        "fail to create client_b_tx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_rx, port_type_busB_L2_L3, 10000, 0),
        "fail to create client_b_rx");

    /* Create L2 Tx datapath: eth_port_tx -> force_fwd -> client_b_tx ->  client_a_tx */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b_tx, client_a_tx, 0),
        "fail to set Tx datapath on Q2A_#0");

    /* Create L2 Rx datapath: client_a_tx -> client_b_tx -> force_fwd -> eth_port_rx_2 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a_tx, client_b_tx, eth_port_rx_2, 0),
        "fail to set Rx2 datapath on Q2A_#0");

    /* Create L2 Rx datapath: client_a_rx -> client_b_rx -> force_fwd -> eth_port_rx_0 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_rx, client_b_rx, 0),
        "fail to set L1 Rx datapath on Q2A_#0");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b_rx, eth_port_rx_0, 1),
        "fail in bcm_port_force_forward_set(busB to eth_rx0)");

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_src, port_type_busA, 10000, 0),
        "fail to create client_a_src");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_dst, port_type_busA, 10000, 0),
        "fail to create client_a_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_dst, port_type_busB_L1, 10000, 0),
        "fail to create client_b_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c_dst, port_type_busC, 10000, 0),
        "fail to create client_c_dst");

    /* Create the 1st failover path:  client_a_src -> failover channel 0 -> client_a_dst */
    failover_channel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_src, client_a_dst, 0),
        "fail to create the 1st failover path");

    /* Create the 2nd failover path: client_a_src -> failover channel 1 -> client_b_dst -> L1 -> eth_port_rx_1 */
    failover_channel_id = 1;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_src, client_b_dst, eth_port_rx_1, 0),
        "fail to create the 2nd failover path");

    /* Create the 3rd failover path:
     * client_a_src -> failover channel 2 -> client_c_dst -> ilkn_port --loopback -> client_c_dst -> client_a_src
     */
    failover_channel_id = 2;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a_src, client_c_dst, ilkn_port, 0),
        "fail to create the 3rd failover path(FlexE2ILKN)");
    failover_channel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_port, client_c_dst, client_a_src, 0),
        "fail to create the 3rd failover path(ILKN2FlexE)");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_src;
        Cal_Slots_A[1] = client_a_src;
        Cal_Slots_A[2] = client_a_dst;
        Cal_Slots_A[3] = client_a_dst;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to test FlexE failover datapaths(from BusB to BusA/B/C) */
int dnx_flexe_failover_src_in_busB_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx_0,
    bcm_port_t eth_port_rx_1,
    bcm_port_t eth_port_rx_2,
    bcm_port_t ilkn_port)
{
    int i;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on distributed Q2A */
    bcm_port_t client_b_src  = 30; /* Failover src_port in busB */
    bcm_port_t client_a0_dst = 31; /* The 1st failover dst_port in busA */
    bcm_port_t client_a1_dst = 32; /* The 2nd failover dst_port in busA */
    bcm_port_t client_c_dst  = 33; /* Failover dst_port in busC */

    /* Client IDs on centralized Q2A */
    bcm_port_t client_a0_rx = 31; /* FlexE client to receive failover traffic from client_a0_dst */
    bcm_port_t client_a1_rx = 32; /* FlexE client to receive failover traffic from client_a1_dst */
    bcm_port_t client_b0_rx = 33; /* BusB client to connect client_a0_rx and eth_port_rx_0 */
    bcm_port_t client_b1_rx = 34; /* BusB client to connect client_a1_rx and eth_port_rx_1 */


    /***************************************************************************
     * The failover datapaths are:
     * ================================================
     * Stage_1. Q2A_#1(Distributed)
     *                                            +--> client_c_dst  -> ilkn_port -loopback-> client_c_dst -> client_b_src -> eth_port_rx_2
     *    eth_port_tx -> client_b_src --failover--|--> client_a1_dst -> Stage_2.rx_path_1
     *                                            +--> client_a0_dst -> Stage_2.rx_path_0
     * Stage_2. Q2A_#0(Centralized)
     *    rx_path_0: client_a0_rx -> client_b0_rx -> force_fwd -> eth_port_rx_0
     *    rx_path_1: client_a1_rx -> client_b1_rx -> force_fwd -> eth_port_rx_1
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a0_dst, port_type_busA, 10000, 0),
        "fail to create client_a0_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a1_dst, port_type_busA, 10000, 0),
        "fail to create client_a1_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_src, port_type_busB_L1, 10000, 0),
        "fail to create client_b_src");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c_dst, port_type_busC, 10000, 0),
        "fail to create client_c_dst");

    /* Create the 1st failover path:  client_b_src -> failover channel 0 -> client_a0_dst */
    failover_channel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_src, client_a0_dst, 0),
        "fail to create the 1st failover path");

    /* Create the 2nd failover path:  client_b_src -> failover channel 1 -> client_a1_dst */
    failover_channel_id = 1;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_src, client_a1_dst, 0),
        "fail to create the 2nd failover path");

    /* Create the 3rd failover path:
     *    eth_port_tx -> client_b_src -> failover channel 2 -> client_c_dst -> ilkn_port --loopback-->
     * -> client_c_dst -> client_b_src -> eth_port_rx_2
     */
    failover_channel_id = 2;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_ilkn(unit_dis, eth_port_tx, client_b_src, client_c_dst, ilkn_port, 0),
        "fail to create the 3rd failover path(Eth2ILKN)");
    failover_channel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_ilkn_to_eth(unit_dis, ilkn_port, client_c_dst, client_b_src, eth_port_rx_2, 0),
        "fail to create the 3rd failover path(ILKN2Eth)");

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a0_rx, port_type_busA, 10000, 0),
        "fail to create client_a0_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a1_rx, port_type_busA, 10000, 0),
        "fail to create client_a1_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b0_rx, port_type_busB_L2_L3, 10000, 0),
        "fail to create client_b0_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b1_rx, port_type_busB_L2_L3, 10000, 0),
        "fail to create client_b1_rx");

    /* Create rx_path_0: client_a0_rx -> client_b0_rx -> force_fwd -> eth_port_rx_0 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a0_rx, client_b0_rx, 0),
        "fail to set rx_path_0");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b0_rx, eth_port_rx_0, 1),
        "fail in bcm_port_force_forward_set(busB to eth_rx0)");

    /* Create rx_path_1: client_a1_rx -> client_b1_rx -> force_fwd -> eth_port_rx_1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a1_rx, client_b1_rx, 0),
        "fail to set rx_path_1");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b1_rx, eth_port_rx_1, 1), "(busB to eth_rx1)");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a0_dst;
        Cal_Slots_A[1] = client_a0_dst;
        Cal_Slots_A[2] = client_a1_dst;
        Cal_Slots_A[3] = client_a1_dst;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to test FlexE failover datapaths(from BusC to BusA/B/C) */
int dnx_flexe_failover_src_in_busC_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx_0,
    bcm_port_t eth_port_rx_1,
    bcm_port_t eth_port_rx_2,
    bcm_port_t ilkn_port)
{
    int i;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on distributed Q2A */
    bcm_port_t client_c_src  = 30; /* Failover src_port in busC */
    bcm_port_t client_a0_dst = 31; /* The 1st failover dst_port in busA */
    bcm_port_t client_a1_dst = 32; /* The 2nd failover dst_port in busA */
    bcm_port_t client_b_dst  = 33; /* Failover dst_port in busB */

    /* Client IDs on centralized Q2A */
    bcm_port_t client_a0_rx = 31; /* FlexE client to receive failover traffic from client_a0_dst */
    bcm_port_t client_a1_rx = 32; /* FlexE client to receive failover traffic from client_a1_dst */
    bcm_port_t client_b0_rx = 33; /* BusB client to connect client_a0_rx and eth_port_rx_0 */
    bcm_port_t client_b1_rx = 34; /* BusB client to connect client_a1_rx and eth_port_rx_1 */


    /***************************************************************************
     * The failover datapaths are:
     * ================================================
     * Stage_1. Q2A_#1(Distributed)
     *    eth_port_tx -> client_b_dst -> client_c_src -> ilkn_port -loopback->
     *
     *                                +--> client_b_dst  -> eth_port_rx_2
     *    -> client_c_src --failover--|--> client_a1_dst -> Stage_2.rx_path_1
     *                                +--> client_a0_dst -> Stage_2.rx_path_0
     *
     * Stage_2. Q2A_#0(Centralized)
     *    rx_path_0: client_a0_rx -> client_b0_rx -> force_fwd -> eth_port_rx_0
     *    rx_path_1: client_a1_rx -> client_b1_rx -> force_fwd -> eth_port_rx_1
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a0_dst, port_type_busA, 10000, 0),
        "fail to create client_a0_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a1_dst, port_type_busA, 10000, 0),
        "fail to create client_a1_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_dst, port_type_busB_L1, 10000, 0),
        "fail to create client_b_dst");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c_src, port_type_busC, 10000, 0),
        "fail to create client_c_src");

    /* Create the 1st failover path:
     *    eth_port_tx -> client_b_dst -> client_c_src -> ilkn_port -loopback->
     * -> client_c_src -> failover channel 0 -> client_b_dst  -> eth_port_rx_2
     */
    failover_channel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_ilkn(unit_dis, eth_port_tx, client_b_dst, client_c_src, ilkn_port, 0),
        "fail to create the 1st failover path(Eth2ILKN)");
    failover_channel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_ilkn_to_eth(unit_dis, ilkn_port, client_c_src, client_b_dst, eth_port_rx_2, 0),
        "fail to create the 1st failover path(ILKN2Eth)");

    /* Create the 2nd failover path:  client_c_src -> failover channel 1 -> client_a0_dst  */
    failover_channel_id = 1;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_c_src, client_a0_dst, 0),
        "fail to create the 2nd failover path");

    /* Create the 3rd failover path:  client_c_src -> failover channel 2 -> client_a1_dst  */
    failover_channel_id = 2;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_c_src, client_a1_dst, 0),
        "fail to create the 3rd failover path");
    failover_channel_id = 0;

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a0_rx, port_type_busA, 10000, 0),
        "fail to create client_a0_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a1_rx, port_type_busA, 10000, 0),
        "fail to create client_a1_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b0_rx, port_type_busB_L2_L3, 10000, 0),
        "fail to create client_b0_rx");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b1_rx, port_type_busB_L2_L3, 10000, 0),
        "fail to create client_b1_rx");

    /* Create rx_path_0: client_a0_rx -> client_b0_rx -> force_fwd -> eth_port_rx_0 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a0_rx, client_b0_rx, 0),
        "fail to set rx_path_0");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b0_rx, eth_port_rx_0, 1), "(busB to eth_rx0)");

    /* Create rx_path_1: client_a1_rx -> client_b1_rx -> force_fwd -> eth_port_rx_1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a1_rx, client_b1_rx, 0),
        "fail to set rx_path_1");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b1_rx, eth_port_rx_1, 1), "(busB to eth_rx1)");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a0_dst;
        Cal_Slots_A[1] = client_a0_dst;
        Cal_Slots_A[2] = client_a1_dst;
        Cal_Slots_A[3] = client_a1_dst;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set OhCalInUse(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, FLEXE_CAL_A), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set OhLogicalPhyID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_phy, BCM_PORT_FLEXE_TX, bcmPortFlexeOhLogicalPhyID, flexe_phy), error_msg);

        /* Active FLEXE_CAL_A */
        snprintf(error_msg, sizeof(error_msg), "fail to active FLEXE_CAL_A on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to test FlexE_PHY/ETH_L1/ETH_L2 ports in same CDU */
int dnx_flexe_eth_port_and_flexe_phy_in_same_cdu_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t cen_port_L1,
    bcm_port_t cen_port_L2,
    bcm_port_t dis_port_1,
    bcm_port_t dis_port_2)
{
    int i;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 40; /* BusA client for L1 traffic */
    bcm_port_t client_a_L2 = 41; /* BusA client for L2 traffic */
    bcm_port_t client_b_L1 = 42; /* BusB client for L1 traffic */
    bcm_port_t client_b_L2 = 43; /* BusB client for L2 traffic */


    /***************************************************************************
     * The L1/L2 datapaths are:
     *
     *                      Centralized Device                  Centralized Device
     *                 +----------------------------+      +----------------------------+
     * cen_port_L1<--> | client_b_L1<-->client_a_L1 | <--> | client_a_L1<-->client_b_L1 | <--> dis_port_1
     * cen_port_L2<--> | client_b_L2<-->client_a_L2 | <--> | client_a_L2<-->client_b_L2 | <--> dis_port_2
     *                 +----------------------------+      +----------------------------+
     *                    (L1 Traffic: L1 switch)              (L1 Traffic: force_fwd)
     *                    (L2 Traffic: force_fwd)              (L2 Traffic: force_fwd)
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, 5000, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L2, port_type_busA, 5000, 0),
        "fail to create client_a_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L1, 5000, 0),
        "fail to create client_b_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L2, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L2");

    /* Create L1 datapath: cen_port_L1<- L1_switch -> client_b_L1 <--> client_a_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, cen_port_L1, client_b_L1, client_a_L1, 0),
        "fail to Eth2FlexE L1 datapath on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_L1, client_b_L1, cen_port_L1, 0),
        "fail to set FlexE2Eth L1 datapath on centralized device");

    /* Create L2 datapath: cen_port_L2<- force_fwd -> client_b_L2 <--> client_a_L2 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, cen_port_L2, client_b_L2, 1),
        "fail in bcm_port_force_forward_set(Eth2BusB2)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b_L2, cen_port_L2, 1),
        "fail in bcm_port_force_forward_set(BusB2Eth2)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L2, client_a_L2, 0),
        "fail to set BusB2BusA flow on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L2, client_b_L2, 0),
        "fail to set BusA2BusB flow on centralized device");

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L1, port_type_busA, 5000, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_L2, port_type_busA, 5000, 0),
        "fail to create client_a_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_L1, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_L2, port_type_busB_L2_L3, 5000, 0),
        "fail to create client_b_L2");

    /* Create datapath for L1 traffic: dis_port_1<- force_forward -> client_b_L1 <--> client_a_L1 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_dis, dis_port_1, client_b_L1, 1), "(Eth2BusB1)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_dis, client_b_L1, dis_port_1, 1), "(BusB2Eth1)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L1, client_a_L1, 0),
        "fail to set BusB2BusA flow on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_L1, client_b_L1, 0),
        "fail to set BusA2BusB flow on distributed device");

    /* Create datapath for L2 traffic: dis_port_2<- force_forward -> client_b_L2 <--> client_a_L2 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_dis, dis_port_2, client_b_L2, 1), "(Eth2BusB2)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_dis, client_b_L2, dis_port_2, 1), "(BusB2Eth2)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L2, client_a_L2, 0),
        "fail to set BusB2BusA flow on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_L2, client_b_L2, 0),
        "fail to set BusA2BusB flow on distributed device");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 3 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_L1;
        Cal_Slots_A[1] = client_a_L2;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    return BCM_E_NONE;
}

/* Common function with FlexE datapath between ETH and FlexE */
int dnx_flexe_common_datapath_between_eth_and_flexe_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t phy_id_start,
    int nof_phys,
    int phy_speed,
    int flexe_client_speed,
    bcm_port_t eth_cen,
    bcm_port_t eth_dis,
    int is_L1_fwd_cen,
    int is_L1_fwd_dis)
{
    int i, ii;
    int unit[2] = {unit_cen, unit_dis};
    int nof_client_slots = flexe_client_speed / 5000;
    int total_slots = (phy_speed * nof_phys) / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 60;
    bcm_port_t client_b = 61;
    flexe_port_type_t client_b_type_cen = is_L1_fwd_cen ? port_type_busB_L1: port_type_busB_L2_L3;
    flexe_port_type_t client_b_type_dis = is_L1_fwd_dis ? port_type_busB_L1: port_type_busB_L2_L3;
    char error_msg[200]={0,};


    /***************************************************************************
     * The datapath is:
     *
     *                      Centralized Device                     Centralized Device
     *                   +------------------------+             +------------------------+
     * eth_cen <-L1/L2-> | client_b <--> client_a | <- FlexE -> | client_a <--> client_b | <-L1/L2-> eth_dis
     *                   +------------------------+             +------------------------+
     *
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on centralized device
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    for (i = 0; i < nof_phys; i++) {
        flexe_phy_ports[i] = phy_id_start + i;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_phys), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a, port_type_busA, flexe_client_speed, 0),
        "fail to create client_a on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b, client_b_type_cen, flexe_client_speed, 0),
        "fail to create client_b on centralized device");

    if (is_L1_fwd_cen) {
        /* Create L1 datapath: eth_cen <- L1_switch -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 0),
            "fail to set Eth2FlexE L1 datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 0),
            "fail to set FlexE2Eth L1 datapath on centralized device");
    } else {
        /* Create L3 datapath: eth_cen <-L3_route-> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 0),
            "fail to set Eth2FlexE L2 datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 0),
            "fail to set FlexE2Eth L2 datapath on centralized device");
    }

    /***************************************************************************
     * FlexE Setting on distributed device
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_phys), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a, port_type_busA, flexe_client_speed, 0),
        "fail to create client_a on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b, client_b_type_dis, flexe_client_speed, 0),
        "fail to create client_b on distributed device");

    if (is_L1_fwd_dis) {
        /* Create FlexE datapath: eth_dis <- L1 -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 0),
            "fail to set FlexE2Eth L1 datapath on distributed device");

        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 0),
            "fail to set Eth2FlexE L1 datapath on distributed device");
    } else {
        /* Create FlexE datapath: eth_dis <-force_forward-> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 0),
            "fail to set FlexE2Eth L2 datapath on distributed device");

        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 0),
            "fail to set Eth2FlexE L2 datapath on distributed device");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the FlexE client */
        for (ii = 0; ii < nof_client_slots; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], phy_id_start), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to clear all FlexE configurations generated by dnx_flexe_common_datapath_between_eth_and_flexe_main() */
int dnx_flexe_common_datapath_between_eth_and_flexe_clear(
    int unit_cen,
    int unit_dis,
    int flexe_grp_speed,
    bcm_port_t eth_cen,
    bcm_port_t eth_dis,
    int is_L1_fwd_cen,
    int is_L1_fwd_dis)
{
    int i, ii;
    int unit[2] = {unit_cen, unit_dis};
    int total_slots = flexe_grp_speed / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 60;
    bcm_port_t client_b = 61;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    char error_msg[200]={0,};


    /***************************************************************************
     * The datapath is:
     *
     *                      Centralized Device                     Centralized Device
     *                   +------------------------+             +------------------------+
     * eth_cen <-L1/L2-> | client_b <--> client_a | <- FlexE -> | client_a <--> client_b | <-L1/L2-> eth_dis
     *                   +------------------------+             +------------------------+
     *
     ***************************************************************************/

    /***************************************************************************
     * Clear calendar and OH settings
     ***************************************************************************/

    /* Clear calendar settings */
    for (i = 0; i < 2; i++) {
        /* Restore all slots to 0 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    }

    /***************************************************************************
     * Clear FlexE Setting on centralized device
     ***************************************************************************/

    /* Clear FlexE datapaths */
    if (is_L1_fwd_cen) {
        /* Clear L1 datapath: eth_cen <- L1_switch -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 1),
            "fail to clear Eth2FlexE L1 datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 1),
            "fail to clear FlexE2Eth L1 datapath on centralized device");
    } else {
        /* Clear L2 datapath: eth_cen <- L2_switch -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 1),
            "fail to clear Eth2FlexE L2 datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 1),
            "fail to clear FlexE2Eth L2 datapath on centralized device");
    }

    /* Remove FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_cen, client_a), "fail to remove client_a on centralized device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_cen, client_b), "fail to remove client_b on centralized device");

    /* Delete FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit_cen, flexe_grp), error_msg);

    /***************************************************************************
     * FlexE Setting on distributed device
     ***************************************************************************/

    /* Clear FlexE datapaths */
    if (is_L1_fwd_dis) {
        /* Clear L1 datapath: eth_dis <- L1_switch -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 1),
            "fail to clear Eth2FlexE L1 datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 1),
            "fail to clear FlexE2Eth L1 datapath on centralized device");
    } else {
        /* Clear L2 datapath: eth_dis <- L2_switch -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 1),
            "fail to clear Eth2FlexE L2 datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 1),
            "fail to clear FlexE2Eth L2 datapath on centralized device");
    }

    /* Remove FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_dis, client_a), "fail to remove client_a on distributed device");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_dis, client_b), "fail to remove client_b on distributed device");

    /* Delete FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit_dis, flexe_grp), error_msg);

    return BCM_E_NONE;
}

/* Function to create a FlexE L1 snake in 1 FlexE group with maximal allowed clients and eth ports according given snake speed */
int dnx_flexe_snake_test_1_grp_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t flexe_phy,
    int snake_rate)
{
    int i, ii, unit;
    int units[2] = {unit_cen, unit_dis};
    int fec_type = bcmPortPhyFecNone;
    int nof_pcs = 1;
    int grp_speed = 400000;
    int client_speed = snake_rate;
    int eth_speed = snake_rate;
    int nof_lanes_per_eth = snake_rate / 25000; /* Use falcon ports */
    int nof_eth_ports = grp_speed / snake_rate;
    int nof_clients = grp_speed / snake_rate;
    int eth_start_lane = 28;
    int eth_lanes[nof_eth_ports];
    int total_slots = grp_speed / 5000;
    int nof_slots_per_client = client_speed / 5000;
    int nof_slots_assign = (nof_clients * client_speed) / 5000;
    bcm_port_t client_a_base = 40;
    bcm_port_t client_b_base = 50;
    bcm_port_t eth_port_base = 60;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};


    /* Set started lane for each ETH port */
    for (i = 0; i < nof_eth_ports; i++) {
        eth_lanes[i] = eth_start_lane + i * nof_lanes_per_eth;
        if (eth_lanes[i] >= 36) {
            eth_lanes[i] += 4; /* Skip XLGE6 and XLGE9 */
        }
    }

    /***************************************************************************
     * The snake datapath is:
     *
     *                   Centralized                Centralized
     *               +-----------------+       +--------------------+
     * eth_tx_rx <-->| BusB --> BusA_0 |<----->| BusA_0 <--> BusB_0 |<--> eth_0(loopback)
     *               |   ^        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   |      BusA_1 |<----->| BusA_1 <--> BusB_1 |<--> eth_1(loopback)
     *               |   |        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   |      BusA_2 |<----->| BusA_2 <--> BusB_2 |<--> eth_2(loopback)
     *               |   |        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   |       ...   |<----->|   ...  <-->  ...   |<--> ...
     *               |   |        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   +-<----BusA_N |<----->| BusA_N <--> BusB_N |<--> eth_N(loopback)
     *               +-----------------+       +--------------------+
     *
     **************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
    }

    /* Create BusB client */
    snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base, port_type_busB_L1, client_speed, 0), error_msg);

    /* Create traffic Tx and Rx datapath:
     * Tx: eth_tx_rx -> BusB -> BusA_0
     * Rx: BusA_N -> BusB -> eth_tx_rx
     */
    snprintf(error_msg, sizeof(error_msg), "fail to set Tx and Rx L1 datapath(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_tx_rx, client_b_base, client_a_base, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients-1, client_b_base,  eth_tx_rx, 0), error_msg);

    /* Create flexe to flexe flows to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        /* FlexE2FlexE: BusA_i -> BusA_i+1 */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0), error_msg);
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L1, client_speed, 0), error_msg);
    }

    /* Create L1 ETH ports */
    for (i = 0; i < nof_eth_ports; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to add ETH L1 port(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_port_add(unit, eth_port_base+i, 1, eth_lanes[i], nof_lanes_per_eth, 0, 0, eth_speed, fec_type, 0), error_msg);

        /* IPG setting */
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit, eth_port_base+i, 0, BCM_PORT_DUPLEX_FULL, 88), error_msg);
    }

    /* Connect L1 ports with FlexE clients */
    for (i = 0; i < nof_eth_ports; i++) {
        /* L1 datapath: BusA_i -> BusB_i -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+i, client_b_base+i, eth_port_base+i, 0), error_msg);

        /* L1 datapath: eth_i -> BusB_i -> BusA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_base+i, client_b_base+i, client_a_base+i, 0), error_msg);
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Check FlexE PHY link status */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", units[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(units[i], flexe_phy), error_msg);
    }

    /* Synce check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Function to translate ETH<->FlexE L1 datapath to L2 datapath or vice versa
 * It can only be called after function dnx_flexe_snake_test_1_grp_main[]
 */
int dnx_flexe_snake_test_datapath_L1_L2_exchange(
    int unit,
    int snake_rate,
    int eth_flexe_pair_index,
    int is_L1_to_L2)
{
    int i, ii;
    int fec_type = bcmPortPhyFecNone;
    int client_speed = snake_rate;
    int eth_speed = snake_rate;
    int nof_lanes_per_eth = snake_rate / 25000;
    int nof_eth_ports = 400000 / snake_rate;
    int eth_start_lane = 28;
    int eth_lanes[nof_eth_ports];
    bcm_pbmp_t pbmp;
    bcm_port_t client_a_base = 40;
    bcm_port_t client_b_base = 50;
    bcm_port_t eth_port_base = 60;
    bcm_port_t eth_port_id = eth_port_base + eth_flexe_pair_index;
    bcm_port_t client_b_id = client_b_base + eth_flexe_pair_index;
    bcm_port_t client_a_id = client_a_base + eth_flexe_pair_index;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_snake_test_datapath_L1_L2_exchange";
    printf("###### %s: exchange flag - %d; eth_flexe_link_pair - %d ######\n", proc_name, is_L1_to_L2, eth_flexe_pair_index);

    /* Set started lane for each ETH port */
    for (i = 0; i < nof_eth_ports; i++) {
        eth_lanes[i] = eth_start_lane + i * nof_lanes_per_eth;
        if (eth_lanes[i] >= 36) {
            eth_lanes[i] += 4; /* Skip XLGE6 and XLGE9 */
        }
    }

    /***************************************************************************
     * The snake datapath is:
     *
     *                   Centralized                Centralized
     *               +-----------------+       +--------------------+
     * eth_tx_rx <-->| BusB --> BusA_0 |<----->| BusA_0 <--> BusB_0 |<--> eth_0(loopback)
     *               |   ^        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   |      BusA_1 |<----->| BusA_1 <--> BusB_1 |<--> eth_1(loopback)
     *               |   |        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   |      BusA_2 |<----->| BusA_2 <--> BusB_2 |<--> eth_2(loopback)
     *               |   |        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   |       ...   |<----->|   ...  <-->  ...   |<--> ...
     *               |   |        |    |       |                    |
     *               |   |        v    |       |                    |
     *               |   +-<----BusA_N |<----->| BusA_N <--> BusB_N |<--> eth_N(loopback)
     *               +-----------------+       +--------------------+
     *
     **************************************************************************/

    /***************************************************************************
     * Clear the datapath between ETH port and FlexE client on distributed device
     ***************************************************************************/

    if (is_L1_to_L2) {
        /* Clear L1 datapath: eth_i -> BusB_i -> BusA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to clear Eth2FlexE L1 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 1), error_msg);

        /* Clear L1 datapath: BusA_i ->BusB_i -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to clear FlexE2Eth L1 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 1), error_msg);
    } else {
        /* Clear L2 datapath: eth_i -> force_forward -> BusB_i -> BusA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to clear Eth2FlexE L2 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 1), error_msg);

        /* Clear L2 datapath: BusA_i -> BusB_i -> force_forward  -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to clear FlexE2Eth L2 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 1), error_msg);
    }

    /***************************************************************************
     * Exchange L1 and L2 ports
     ***************************************************************************/

    if (is_L1_to_L2) {
        /* Remove the BusB L1 client */
        snprintf(error_msg, sizeof(error_msg), "fail to remove BusB L1 client(id %d)", client_b_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_id), error_msg);
        /* Readd the BusB client for L2 forwarding */
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB L2 client(id %d)", client_b_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_id, port_type_busB_L2_L3, client_speed, 0), error_msg);

        /* Delete the ETH L1 port */
        snprintf(error_msg, sizeof(error_msg), "fail to remove ETH L1 port(id %d)", eth_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_id, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_remove(unit, eth_port_id, 0), error_msg);

        /* Readd the ETH port for L2 forwarding */
        snprintf(error_msg, sizeof(error_msg), "(id %d)", eth_port_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_port_add(unit, eth_port_id, eth_lanes[eth_flexe_pair_index], BCM_PORT_IF_NIF_ETH, nof_lanes_per_eth, eth_speed, fec_type), error_msg);
        /* IPG setting */
        snprintf(error_msg, sizeof(error_msg), "(id %d)", eth_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit, eth_port_id, 0, BCM_PORT_DUPLEX_FULL, 88), error_msg);
    } else {
        /* Remove the BusB L2 client */
        snprintf(error_msg, sizeof(error_msg), "fail to remove BusB L2 client(id %d)", client_b_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_id), error_msg);
        /* Readd the BusB client for L1 forwarding */
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB L1 client(id %d)", client_b_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_id, port_type_busB_L1, client_speed, 0), error_msg);

        /* Delete the ETH L2 port */
        snprintf(error_msg, sizeof(error_msg), "fail to remove ETH L2 port %d", eth_port_id);
        BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_remove_port_full_example(unit, eth_port_id, 0), error_msg);
        /* Readd the ETH port for L1 forwarding */
        snprintf(error_msg, sizeof(error_msg), "fail to add ETH L1 port %d", eth_port_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_port_add(unit, eth_port_id, 1, eth_lanes[eth_flexe_pair_index], nof_lanes_per_eth, 0, 0, eth_speed, fec_type, 0), error_msg);
        /* IPG setting */
        snprintf(error_msg, sizeof(error_msg), "(id %d)", eth_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit, eth_port_id, 0, BCM_PORT_DUPLEX_FULL, 88), error_msg);
    }

    /***************************************************************************
     * Build new datapath
     ***************************************************************************/

    if (is_L1_to_L2) {
        /* L2 datapath: eth_i -> BusB_i -> busA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L2 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 0), error_msg);

        /* L2 datapath: BusA_i -> busB_i -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L2 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 0), error_msg);
    } else {
        /* L1 datapath: eth_i -> busB_i -> busA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 0), error_msg);

        /* L1 datapath: busA_i -> busB_i -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(index %d)", eth_flexe_pair_index);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 0), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to create 2 FlexE L1 snakes in 2 groups
 * And it has an option to re-add the 2nd group if 'readd_grp_b' is set
 */
 int dnx_flexe_snake_test_2_grps_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_trx,
    bcm_port_t flexe_phy_0,
    bcm_port_t flexe_phy_1,
    int snake_rate,
    int readd_grp_b)
{
    int i, ii, slot_iter, unit;
    int units[2] = {unit_cen, unit_dis};
    int fec_type = bcmPortPhyFecNone;
    int nof_pcs = 1;
    int client_speed = snake_rate;
    int eth_speed = snake_rate;
    int nof_lanes_per_eth = snake_rate / 25000;
    int nof_eth_ports = 400000 / snake_rate;
    int nof_clients = 400000 / snake_rate;
    int nof_clients_per_grp = nof_clients / 2;
    int eth_lanes[nof_eth_ports];
    int eth_start_lane = 28;
    int nof_slots_per_grp = (client_speed * nof_clients_per_grp) / 5000;
    int nof_slots_per_client = client_speed / 5000;
    bcm_port_t client_a_base = 40;
    bcm_port_t client_b_base = 50;
    bcm_port_t eth_port_base = 60;
    bcm_gport_t flexe_grp[2];
    bcm_gport_t flexe_phy[2] = {flexe_phy_0, flexe_phy_1};
    bcm_port_t client_id;
    char error_msg[200]={0,};


    /* Set started lane for each ETH port */
    for (i = 0; i < nof_eth_ports; i++) {
        eth_lanes[i] = eth_start_lane + i * nof_lanes_per_eth;
        if (eth_lanes[i] >= 36) {
            eth_lanes[i] += 4; /* Skip XLGE6 and XLGE9 */
        }
    }

    /* Disable synce setting */
    skip_synce_in_grp_creation = 1;

    /***************************************************************************
     * The snake datapath is:
     *                                 
     *                  Centralized                   Centralized         
     *             +--------------------+       +-------------------------+
     * eth_trx <-->| BusB --> BusA_0    |<----->| BusA_0   <--> BusB_0    |<--> eth_0(loopback)
     *             |   ^        |       |       |                         |
     *             |   |        v       |       |                         |
     *             |   |      BusA_1    |<----->| BusA_1   <--> BusB_1    |<--> eth_1(loopback)
     *             |   |        |       |       |                         |
     *             |   |        v       |       |                         |
     *             |   |       ...      |<----->|   ...    <-->  ...      |<--> ...
     *             |   |        |       |       |                         |
     *             |   |        v       |       |                         |
     *             |   +-<----BusA_N    |<----->| BusA_N   <--> BusB_N    |<--> eth_N(loopback)
     *             +--------------------+       +-------------------------+
     *                    Group_A ^                       Group_A ^
     *
     *                    Group_B v                       Group_B v
     *             +--------------------+       +-------------------------+
     *             |          BusA_N+1  |<----->| BusA_N+1 <--> BusB_N+1  |<--> eth_N+1(loopback)
     *             |            |       |       |                         |
     *             |            v       |       |                         |
     *             |          BusA_N+2  |<----->| BusA_N+2 <--> BusB_N+2  |<--> eth_N+2(loopback)
     *             |            |       |       |                         |
     *             |            v       |       |                         |
     *             |           ...      |<----->|   ...    <-->  ...      |<--> ...
     *             |            |       |       |                         |
     *             |            v       |       |                         |
     *             |          BusA_2N-1 |<----->| BusA_2N-1<--> BusB_2N-1 |<--> eth_2N-1(loopback)
     *             +--------------------+       +-------------------------+
     *
     **************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/

    /* Create 2 FlexE groups with the 2 200G FlexE PHYs */
    if (!readd_grp_b) {
        unit = unit_cen;
        for (i = 0; i < 2; i++) {
            BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[i], i);
            flexe_phy_ports[0] = flexe_phy[i];
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp[i], nof_pcs), error_msg);
        }

        /* Create BusA clients */
        for (i = 0; i < nof_clients; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
        }

        /* Create BusB client */
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base, port_type_busB_L1, client_speed, 0), error_msg);

        /* Create traffic Tx datapath: eth_trx -> BusB -> BusA_0 */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_trx, client_b_base, client_a_base, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients_per_grp-1, client_b_base, eth_trx, 0), error_msg);

        /* Create flexe to flexe flows to loop traffic back to distributed device */
        for (i = 0; i < nof_clients-1; i++) {
            /* FlexE2FlexE: BusA_i -> BusA_i+1 (skip the setting on the 20th client) */
            if (i != nof_clients_per_grp-1) {
                snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath(unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0), error_msg);
            }
        }

        /* Calendar and OH setting */
        for (i = 0; i < 2; i++) {
            /* Assign slots for FlexE clients */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
            for (slot_iter = 0; slot_iter < nof_slots_per_grp; slot_iter++) {
                client_id = (slot_iter / nof_slots_per_client) + client_a_base + nof_clients_per_grp*i;
                Cal_Slots_A[slot_iter] = client_id;
            }

            /* Assign slots and set OH client ID */
            snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp[i], 0, FLEXE_CAL_A, nof_slots_per_grp), error_msg);

            /* Set OH Group ID */
            snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp[i], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, i+1), error_msg);
        }
    }

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/

    /* Create 2 FlexE groups with the 2 200G FlexE PHYs */
    unit = unit_dis;
    for (i = 0; i < 2; i++) {
        if (readd_grp_b && i == 0) {
            /* Don't create GroupA If re-create GroupB */
            continue;
        }

        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[i], i);
        flexe_phy_ports[0] = flexe_phy[i];
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp[i], nof_pcs), error_msg);
    }

    /* Create BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if (readd_grp_b && i < nof_clients_per_grp) {
            /* Don't create clients in GroupA If re-create GroupB */
            continue;
        }

        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L1, client_speed, 0), error_msg);
    }

    /* Create L1 ETH ports */
    for (i = 0; i < nof_eth_ports; i++) {
        if (readd_grp_b && i < nof_clients_per_grp) {
            /* Don't create L1 ports in GroupA If re-create GroupB */
            continue;
        }

        snprintf(error_msg, sizeof(error_msg), "fail to add ETH L1 port(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_port_add(unit, eth_port_base+i, 1, eth_lanes[i], nof_lanes_per_eth, 0, 0, eth_speed, fec_type, 0), error_msg);
        /* IPG setting */
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit, eth_port_base+i, 0, BCM_PORT_DUPLEX_FULL, 88), error_msg);
    }

    /* Connect L1 ports with FlexE clients */
    for (i = 0; i < nof_eth_ports; i++) {
        if (readd_grp_b && i < nof_clients_per_grp) {
            /* Skip flow set in GroupA If re-create GroupB */
            continue;
        }

        /* L1 datapath: BusA_i -> BusB_i -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+i, client_b_base+i, eth_port_base+i, 0), error_msg);

        /* L1 datapath: eth_i -> BusB_i -> BusA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_base+i, client_b_base+i, client_a_base+i, 0), error_msg);
    }

    /* Calendar and OH setting */
    for (i = 0; i < 2; i++) {
        if (readd_grp_b && i == 0) {
            /* No OH setting on GroupA If re-create GroupB */
            continue;
        }

        /* Assign slots for FlexE clients */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (slot_iter = 0; slot_iter < nof_slots_per_grp; slot_iter++) {
            client_id = (slot_iter / nof_slots_per_client) + client_a_base + nof_clients_per_grp*i;
            Cal_Slots_A[slot_iter] = client_id;
        }

        /* Assign slots and set OH client ID */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp[i], 0, FLEXE_CAL_A, nof_slots_per_grp), error_msg);

        /* Set OH Group ID */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp[i], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, i+1), error_msg);
    }

    /* Synce check */
    if (!readd_grp_b) {
        flexe_phy_ports[0] = flexe_phy_0;
        if (dnx_flexe_rx_in_invalid_status(unit_dis)) {
            bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSource, flexe_phy_ports[0]);
            bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSourceDivCtrl, 2);
        }
        sal_sleep(5);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp[0]), "Synce check failed");
    }

    return BCM_E_NONE;
}

/* Function to delete group B which is generated by function dnx_flexe_snake_test_2_grps_main() */
int dnx_flexe_snake_test_2_grps_clear_grp_b(
    int unit_cen,
    int unit_dis,
    int snake_rate)
{
    int i, unit;
    int client_speed = snake_rate;
    int nof_eth_ports = 400000 / snake_rate;
    int nof_clients = 400000 / snake_rate;
    int nof_clients_per_grp = nof_clients / 2;
    int nof_slots_per_grp = (client_speed * nof_clients_per_grp) / 5000;
    bcm_port_t client_a_base = 40;
    bcm_port_t client_b_base = 50;
    bcm_port_t eth_port_base = 60;
    bcm_gport_t flexe_grp[2];
    char error_msg[200]={0,};

    /* Get group GPORT */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[0], 0);
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[1], 1);


    /***************************************************************************
     * The snake datapath is:
     *                                 
     *                  Centralized                   Centralized         
     *             +--------------------+       +-------------------------+
     * eth_trx <-->| BusB --> BusA_0    |<----->| BusA_0   <--> BusB_0    |<--> eth_0(loopback)
     *             |   ^        |       |       |                         |
     *             |   |        v       |       |                         |
     *             |   |      BusA_1    |<----->| BusA_1   <--> BusB_1    |<--> eth_1(loopback)
     *             |   |        |       |       |                         |
     *             |   |        v       |       |                         |
     *             |   |       ...      |<----->|   ...    <-->  ...      |<--> ...
     *             |   |        |       |       |                         |
     *             |   |        v       |       |                         |
     *             |   +-<----BusA_N    |<----->| BusA_N   <--> BusB_N    |<--> eth_N(loopback)
     *             +--------------------+       +-------------------------+
     *                    Group_A ^                       Group_A ^
     *
     *                    Group_B v                       Group_B v
     *             +--------------------+       +-------------------------+
     *             |          BusA_N+1  |<----->| BusA_N+1 <--> BusB_N+1  |<--> eth_N+1(loopback)
     *             |            |       |       |                         |
     *             |            v       |       |                         |
     *             |          BusA_N+2  |<----->| BusA_N+2 <--> BusB_N+2  |<--> eth_N+2(loopback)
     *             |            |       |       |                         |
     *             |            v       |       |                         |
     *             |           ...      |<----->|   ...    <-->  ...      |<--> ...
     *             |            |       |       |                         |
     *             |            v       |       |                         |
     *             |          BusA_2N-1 |<----->| BusA_2N-1<--> BusB_2N-1 |<--> eth_2N-1(loopback)
     *             +--------------------+       +-------------------------+
     *
     **************************************************************************/

    /***************************************************************************
     * Clear FlexE GroupB on distributed device
     ***************************************************************************/
    unit = unit_dis;

    /* Calendar and OH setting */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp[1], 0, FLEXE_CAL_A, nof_slots_per_grp), error_msg);

    /* Clear L1 flows in GroupB */
    for (i = 0; i < nof_eth_ports; i++) {
        if (i < nof_clients_per_grp) {
            /* Skip L1 flows in GroupA */
            continue;
        }

        /* Clear L1 datapath: BusA_i -> BusB_i -> eth_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+i, client_b_base+i, eth_port_base+i, 1), error_msg);

        /* Clear L1 datapath: eth_i -> BusB_i -> BusA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_base+i, client_b_base+i, client_a_base+i, 1), error_msg);
    }

    /* Remove BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if (i < nof_clients_per_grp) {
            /* Skip clients in GroupA */
            continue;
        }

        snprintf(error_msg, sizeof(error_msg), "fail to remove BusA L1 client(id %d)", client_a_base + i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit,  client_a_base+i), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to remove BusB L1 client(id %d)", client_b_base + i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit,  client_b_base+i), error_msg);
    }

    /* Remove L1 ETH ports */
    for (i = 0; i < nof_eth_ports; i++) {
        if (i < nof_clients_per_grp) {
            /* Skip L1 ports in GroupA */
            continue;
        }

        snprintf(error_msg, sizeof(error_msg), "fail to remove ETH L1 port(id %d)", eth_port_base + i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_base+i, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_remove(unit, eth_port_base+i, 0), error_msg);
    }

    /* Remove GroupB */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp[1]), "fail to remove GroupB");

    return BCM_E_NONE;
}

/* Function to delete group B which is generated by function dnx_flexe_snake_test_2_grps_main() */
int dnx_flexe_snake_with_max_busA_clients(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t flexe_phy,
    int client_speed,
    int nof_clients)
{
    int i, unit;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    int nof_slots_per_client = client_speed / 5000;
    int nof_slots_assign = (nof_clients * client_speed) / 5000;
    int nof_phys = 1;
    bcm_port_t client_a_base = 120;
    bcm_port_t client_b0 = 21;
    bcm_port_t client_b1 = 22;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    grp_speed = is_reduced_cdu_lane_speed ? 200000 : 400000;
    total_slots = grp_speed / 5000;
    nof_phys = is_reduced_cdu_lane_speed ? 2 : 1;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_snake_with_max_busA_clients";

    /* Make sure the number of clients is an even value and no less than 2 */
    if ((nof_clients < 2) || (nof_clients % 2)) {
        printf("**** %s: Error - The number of clients %d isn't an even value or less than 2****\n", proc_name, nof_clients);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * The snake datapath is:
     *
     *                Centralized              Centralized
     *            +------------------+       +--------------+
     * eth_tx --->| BusB0--> BusA_0  |------>| BusA_0       |
     *            |                  |       |   |          |
     *            |                  |       |   V          |
     *            |          BusA_1  |<------| BusA_1       |
     *            |            |     |       |              |
     *            |            v     |       |              |
     *            |          BusA_2  |------>| BusA_2       |
     *            |                  |       |   |          |
     *            |                  |       |   V          |
     *            |          BusA_3  |<------| BusA_3       |
     *            |            |     |       |              |
     *            |            v     |       |              |
     *            |           ...    |  ...> |  ...         |
     *            |                  |       |   |          |
     *            |                  |       |   V          |
     *            |           ...    | <...  |  ...         |
     *            |            |     |       |              |
     *            |            v     |       |              |
     *            |          BusA_N-1|------>| BusA_N-1     |
     *            |                  |       |   |          |
     *            |                  |       |   V          |
     * eth_rx <---| BusB1<-- BusA_N  |<------| BusA_N       |
     *            +------------------+       +--------------+
     *             (eth_tx and eth_rx can be same eth port)
     **************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (is_reduced_cdu_lane_speed) {
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), error_msg);

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
    }

    /* Create 2 BusB clients */
    snprintf(error_msg, sizeof(error_msg), "fail to create BusB clients(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b0, port_type_busB_L1, client_speed, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b1, port_type_busB_L1, client_speed, 0), error_msg);

    /* Create traffic Tx datapath: eth_tx -> BusB0 -> BusA_0 */
    snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_tx_rx, client_b0, client_a_base, 0), error_msg);

    /* Create traffic Rx datapath: BusA_N -> BusB1 -> eth_rx */
    snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients-1, client_b1, eth_tx_rx, 0), error_msg);

    /* Create flexe to flexe flows to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        if (i % 2) {
            /* FlexE2FlexE: BusA_i -> BusA_i+1 */
            snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0), error_msg);
        }
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (is_reduced_cdu_lane_speed) {
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), error_msg);

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
    }

    /* Create flexe to flexe flows to loop traffic back to centralized device */
    for (i = 0; i < nof_clients-1; i++) {
        if ((i % 2) == 0) {
            /* FlexE2FlexE: BusA_i -> BusA_i+1 */
            snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0), error_msg);
        }
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Synce check */
    sal_sleep(5);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Funtion to create a L2 snake with maximal allowed BusB clients */
int dnx_flexe_centralized_snake_with_max_busB_clients_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t flexe_phy,
    int client_speed,
    int nof_clients)
{
    int i, unit;
    int grp_speed = 400000;
    int nof_phys = 1;
    int total_slots = grp_speed / 5000;
    int nof_slots_per_client = client_speed / 5000;
    int nof_slots_assign = (nof_clients * client_speed) / 5000;
    bcm_port_t client_a_base = 20;
    bcm_port_t client_b_base = 120;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    grp_speed = is_reduced_cdu_lane_speed ? 200000 : 400000;
    total_slots = grp_speed / 5000;
    nof_phys = is_reduced_cdu_lane_speed ? 2 : 1;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_centralized_snake_with_max_busB_clients_main";

    /* Make sure the number of clients is an even value and no less than 2 */
    if ((nof_clients < 2) || (nof_clients % 2)) {
        printf("**** %s: Error - The number of clients %d isn't an even value or less than 2****\n", proc_name, nof_clients);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * The snake datapath is:
     *
     *                Centralized                   Centralized
     *            +--------------------+       +---------------------+
     * eth_tx --->| BusB_0 --> BusA_0  |------>| BusA_0 --> BusB_0   |
     *            |                    |       |              |      |
     *            |                    |       |              V      |
     *            | BusB_1 <-- BusA_1  |<------| BusA_1 <-- BusB_1   |
     *            |   |                |       |                     |
     *            |   v                |       |                     |
     *            | BusB_2 --> BusA_2  |------>| BusA_2 --> BusB_2   |
     *            |                    |       |              |      |
     *            |                    |       |              V      |
     *            | BusB_3 <-- BusA_3  |<------| BusA_3 <-- BusB_3   |
     *            |   |                |       |                     |
     *            |   v                |       |                     |
     *            |  ...   --> ...     |  ...> |  ...   -->  ...     |
     *            |                    |       |              |      |
     *            |                    |       |              V      |
     *            |  ...   <-- ...     | <...  |  ...   <--  ...     |
     *            |   |                |       |                     |
     *            |   v                |       |                     |
     *            | BusB_N-1-->BusA_N-1|------>| BusA_N-1-->BusB_N-1 |
     *            |                    |       |              |      |
     *            |                    |       |              V      |
     * eth_rx <---| BusB_N <-- BusA_N  |<------| BusA_N <-- BusB_N   |
     *            +--------------------+       +---------------------+
     *                (eth_tx and eth_rx can be same eth port)
     **************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (is_reduced_cdu_lane_speed) {
        /* Q2U 200G FlexE group needs 2*100G PHYs */
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), error_msg);

    /* Create BusA and busB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed, 0), error_msg);
    }

    /* Set FlexE flows between BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if (i % 2) {
            snprintf(error_msg, sizeof(error_msg), "fail to set FlexE flow from BusA to BusB(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);
        }

        if ((i % 2) == 0) {
            snprintf(error_msg, sizeof(error_msg), "fail to set FlexE flow from BusB to BusA(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
        }
    }

    /* Set BusB to BuBs L2 switching to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        if (i % 2) {
            /* BusB2BusB: BusB_i -> BusB_i+1 */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_base+i, client_b_base+i+1, 1), error_msg);
        }
    }

    /* Set L2 datapath between eth_tx_rx and BusB clients */
    snprintf(error_msg, sizeof(error_msg), "fail to set L2 datapath between ETH and BusB(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx_rx, client_b_base, 1), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_base+nof_clients-1, eth_tx_rx, 1), error_msg);

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (is_reduced_cdu_lane_speed) {
        /* Q2U 200G FlexE group needs 2*100G PHYs */
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), error_msg);

    /* Create BusA and busB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed, 0), error_msg);
    }

    /* Set FlexE flows between BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if ((i % 2) == 0) {
            snprintf(error_msg, sizeof(error_msg), "fail to set FlexE flow from BusA to BusB(unit %d)", unit);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);
        }

        if (i % 2) {
            snprintf(error_msg, sizeof(error_msg), "fail to set FlexE flow from BusA to BusBt(unit %d)", unit);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
        }
    }

    /* Set BusB to BuBs L2 switching to loop traffic back to centralized device */
    for (i = 0; i < nof_clients-1; i++) {
        if ((i % 2) == 0) {
            /* BusB2BusB: BusB_i -> BusB_i+1 */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_base+i, client_b_base+i+1, 1), error_msg);
        }
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Synce check */
    sal_sleep(5);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Function to create FlexE snake datapath with maximal allowed FlexE&ILKN L1 channels */
int dnx_flexe_distributed_snake_with_max_L1_channels_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t flexe_phy,
    int client_speed,
    int nof_clients)
{
    int i, unit;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    int nof_slots_per_client = client_speed / 5000;
    int nof_slots_assign = (nof_clients * client_speed) / 5000;
    bcm_port_t client_a_base = 20;
    bcm_port_t client_c_base = 300;
    bcm_port_t client_b0 = 11;
    bcm_port_t client_b1 = 12;
    bcm_port_t ilkn_port_start = 120;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};


    /***************************************************************************
     * The snake datapath is:
     *
     *                Centralized                Distributed
     *            +-----------------+       +--------------------+
     * eth_tx --->| BusB0--> BusA_0 |<----->| BusA_0 <--> BusC_0 |<--> ILKN_0(loopback)
     *            |            |    |       |                    |
     *            |            v    |       |                    |
     *            |          BusA_1 |<----->| BusA_1 <--> BusC_1 |<--> ILKN_1(loopback)
     *            |            |    |       |                    |
     *            |            v    |       |                    |
     *            |          BusA_2 |<----->| BusA_2 <--> BusC_2 |<--> ILKN_2(loopback)
     *            |            |    |       |                    |
     *            |            v    |       |                    |
     *            |           ...   |<----->|  ...   <-->  ...   |<--> ...
     *            |            |    |       |                    |
     *            |            v    |       |                    |
     * eth_rx --->| BusB1<-- BusA_N |<----->| BusA_N <--> BusC_N |<--> ILKN_N(loopback)
     *            +-----------------+       +--------------------+
     *                (eth_tx and eth_rx can be same eth port)
     **************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, 1), error_msg);

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
    }

    /* Create 2 BusB clients */
    snprintf(error_msg, sizeof(error_msg), "fail to create BusB clients(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b0, port_type_busB_L1, client_speed, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b1, port_type_busB_L1, client_speed, 0), error_msg);

    /* Create traffic Tx datapath: eth_tx -> BusB0 -> BusA_0 */
    snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_tx_rx, client_b0, client_a_base, 0), error_msg);

    /* Create traffic Rx datapath: BusA_N -> BusB1 -> eth_rx */
    snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients-1, client_b1, eth_tx_rx, 0), error_msg);

    /* Create flexe to flexe flows to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        /* FlexE2FlexE: BusA_i -> BusA_i+1 */
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2FlexE L1 datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0), error_msg);
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, 1), error_msg);

    /* Create BusA clients and BusC clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create BusC client(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c_base+i, port_type_busC, client_speed, 0), error_msg);
    }

    /* Create L1 ILKN ports */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create L1 ILKN port(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_port_add(unit, ilkn_port_start+i, 0, 28, 22, 2, i, 25781, bcmPortPhyFecNone, 0), error_msg);
    }

    /* Create L1 datapath for each FlexE&ILKN pair */
    for (i = 0; i < nof_clients; i++) {
        /* FlexE2ILKN: BusA_i -> BusC_i ->  ILKN_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set Flexe2ILKN datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a_base+i, client_c_base+i, ilkn_port_start+i, 0), error_msg);

        /* ILKN2Flexe: ILKN_i -> BusC_i -> BusA_i */
        snprintf(error_msg, sizeof(error_msg), "fail to set ILKN2Flexe datapath(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, ilkn_port_start+i, client_c_base+i, client_a_base+i, 0), error_msg);
    }

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Synce check */
    sal_sleep(5);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Function to set FlexE overhead and check relevant alarm */
int dnx_flexe_oh_change_and_alarm_check(
    int unit,
    uint32 port_set,
    uint32 port_get,
    bcm_port_flexe_oh_type_t oh_type,
    int value,
    int alarm_active_exp)
{
    bcm_port_flexe_oh_alarm_t oh_alarm_get;
    int is_adapter = *(dnxc_data_get(unit, "device", "general", "delay_exist", NULL));

    /* Change OH setting */
    if (value != -1) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, port_set, BCM_PORT_FLEXE_TX, oh_type, value), "");
        sal_sleep(2);
    }

    if (is_adapter) {
        print "Don't check alarms on adapter";
        return BCM_E_NONE;
    }

    /* Get OH alarms */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit, port_get, 0, &oh_alarm_get), "");
    sal_sleep(2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit, port_get, 0, &oh_alarm_get), "");

    /* Check if OH alarm(s) raised or cleared */
    if (oh_alarm_get.alarm_active != alarm_active_exp) {
        print oh_alarm_get;
        return BCM_E_FAIL;
    }

    /* Check OH alarms */
    switch (oh_type)
    {
        case bcmPortFlexeOhGroupID:
        {
            if (oh_alarm_get.group_id_mismatch != alarm_active_exp) {
                print oh_alarm_get;
                return BCM_E_FAIL;
            }
            break;
        }
        case bcmPortFlexeOhLogicalPhyID:
        {
            if (oh_alarm_get.phy_id_mismatch != alarm_active_exp && port_get < 1280) {
                /* SDK doesn't report alarm phy_id_mismatch on PHY port */
                print oh_alarm_get;
                return BCM_E_FAIL;
            }
            if (oh_alarm_get.phymap_mismatch != alarm_active_exp && port_get >= 1280) {
                /* SDK doesn't report alarm phymap_mismatch per FlexE group */
                print oh_alarm_get;
                return BCM_E_FAIL;
            }
            break;
        }
        default:
        {
            printf("unknown oh_type %d\n", oh_type);
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

/* Function to create 6 FlexE clients with different speeds
 * There is an option for slot assignment as below:
 * calendar_cfg - 0: Assign slots from 1st slot to last slot(synchronous slots in Rx and Tx)
 * calendar_cfg - 1: Assign slots from last slot to 1st slot(synchronous slots in Rx and Tx)
 * calendar_cfg - 2: Randomly assign slots for the 6 clients(asynchronous slots in Rx and Tx)
 */
int cal_slots_unit0[80];
int cal_slots_unit1[80];
int dnx_flexe_slots_assign_for_mixed_clients_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t cud1_id,
    bcm_port_t flexe_phy,
    int calendar_cfg)
{
    int i, ii, iii, unit_iter, unit;
    int nof_flexe_phys = 1;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    int last_slot = total_slots - 1;
    int vlan_id_base = 1000;
    bcm_port_t client_a_base = 30;
    bcm_port_t client_b_base = 130;
    bcm_port_t eth_ports[2] = {eth_tx_rx, cud1_id};
    bcm_port_t eth_port;
    int client_speed[80];
    int nof_clients;
    int client_id, nof_slots_per_client, slot_index;
    int slot_unuse[80], nof_slot_unuse, rnd_index;
    bcm_gport_t flexe_grp;
    uint32 cal_flags = 0;
    char error_msg[200]={0,};

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    grp_speed = is_reduced_cdu_lane_speed ? 200000 : 400000;
    total_slots = grp_speed / 5000;
    last_slot = total_slots - 1;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_slots_assign_for_mixed_clients_main";

    /* Initiate the number of clients and relevant speeds */
    nof_clients = 6;
    client_speed[0] = 5000;
    client_speed[1] = 10000;
    client_speed[2] = 15000;
    client_speed[3] = 25000;
    client_speed[4] = 75000;
    client_speed[5] = 270000;
    if (is_reduced_cdu_lane_speed && calendar_cfg == 0)
    {
        /* Verification for Q2U. Total client speed is 200G */
        client_speed[0] = 5000;
        client_speed[1] = 10000;
        client_speed[2] = 15000;
        client_speed[3] = 25000;
        client_speed[4] = 75000;
        client_speed[5] = 70000;
    }
    if (is_reduced_cdu_lane_speed && calendar_cfg >= 100)
    {
        /* Verification mixed client speeds on Q2U. Total client speed is 100G */
        client_speed[0] = 5000;
        client_speed[1] = 10000;
        client_speed[2] = 15000;
        client_speed[3] = 20000;
        client_speed[4] = 25000;
        client_speed[5] = 25000;
        calendar_cfg -= 100;
        if (calendar_cfg > 2) {
            printf("**** %s: Error(%d), invalid value of calendar_cfg. Should be in range 100~102. ****\n", proc_name);
            return BCM_E_FAIL;
        }
    }
    if (calendar_cfg == -1)
    {
        /* Used for FlexE OH test */
        client_speed[0] = 5000;
        client_speed[1] = 5000;
        client_speed[2] = 5000;
        client_speed[3] = 5000;
        client_speed[4] = 5000;
        client_speed[5] = 5000;
        calendar_cfg = 0;
    }
    if (calendar_cfg == -2)
    {
        /* Used for calendar switching test with 100G client */
        client_speed[0] = 100000;
        calendar_cfg = 0;
        nof_clients = 1;
    }
    if (calendar_cfg == -3)
    {
        /* Used for FlexE mirror test with shared TM port 
         * There are 20*5G clients 
         */
        calendar_cfg = 0;
        nof_clients = 20;
        for (i = 0; i < nof_clients; i++) {
            client_speed[i] = 5000;
        }
    }
    cal_flags = calendar_cfg > 1 ? BCM_PORT_FLEXE_TX : 0; /* Need calendar Rx setting if calendar_cfg is 2 */

    /***************************************************************************
     * Datapath:
     *
     *                           Centralized #0                      Centralized #1
     *                     +----------------------------+      +----------------------------+
     *               +<--->|<--> BusB_0   <--> BusA_0   |<---->|<--> BusA_0   <--> BusB_0   |<--->+
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_1   <--> BusA_1   |<---->|<--> BusA_1   <--> BusB_1   |<--->|
     *               |     |                            |      |                            |     |
     * eth_tx_rx<--->+<--->|<-->  ...     <-->  ...     |<---->|<-->  ...     <-->  ...     |<--->+<--->CDU1(Loopback)
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_N-1 <--> BusA_N-1 |<---->|<--> BusA_N-1 <--> BusB_N-1 |<--->|
     *               |     |                            |      |                            |     |
     *               +<--->|<--> BusB_N   <--> BusA_N   |<---->|<--> BusA_N   <--> BusB_N   |<--->+
     *                     +----------------------------+      +----------------------------+
     * (Centralized #0 -- eth_tx_rx->BusB_X: L2 lookup; BusB_X->eth_tx_rx: force_forward)
     * (Centralized #1 -- CDU1->BusB_X: L2 lookup; BusB_X->CDU1: force_forward)
     **************************************************************************/


    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;
        eth_port = eth_ports[unit_iter];

        /* Create FlexE group with the 400G FlexE PHY */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        if (flexe_phy) {
            /* Use the provided 400G PHY */
            flexe_phy_ports[0] = flexe_phy;
        } else {
            /* Use FlexE PHY(s) in array flexe_phy_ports[] */
            nof_flexe_phys = 0;
            for (i = 0; i < 8; i++) {
                if (flexe_phy_ports[i]) {
                    nof_flexe_phys++;
                } else {
                    break;
                }
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_flexe_phys), error_msg);

        /* Create BusA clients and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed[i], 0), error_msg);

            snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed[i], 0), error_msg);
        }

        /* Setting FlexE flows between BusA and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to set BusA2BusB FlexE flow(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);

            snprintf(error_msg, sizeof(error_msg), "fail to set BusB2BusA FlexE flow(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
        }

        /* L2 settings between ETH port and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            /* ETH to BusB_i: L2 lookup(DMAC=00:00:00:00:22:22, VID=vlan_id_base+i) */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_switch_config(unit, eth_port, client_b_base+i, vlan_id_base+i), error_msg);

            /* BusB_i to ETH: force_forward */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_base+i, eth_port, 1), error_msg);
        }

        /* Calendar and OH settings */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");

        if (calendar_cfg == 0) {
            /* Starting to assign slots from the 1st slot in the array */
            slot_index = 0;

            for (i = 0; i < nof_clients; i++) {
                client_id = client_a_base + i;
                nof_slots_per_client = client_speed[i] / 5000;

                for (ii = 0; ii < nof_slots_per_client; ii++) {
                    Cal_Slots_A[slot_index++] = client_id;
                }
            }
        } else if (calendar_cfg == 1) {
            /* Starting to assign slots from the last slot in the array */
            slot_index = last_slot;

            for (i = 0; i < nof_clients; i++) {
                client_id = client_a_base + i;
                nof_slots_per_client = client_speed[i] / 5000;

                for (ii = 0; ii < nof_slots_per_client; ii++) {
                    Cal_Slots_A[slot_index--] = client_id;
                }
            }
        } else {
            /* Rondomly assign slots for all the clients
             * Now mark all the 80 slots as unused slots
             */
            for (i = 0; i < total_slots; i++) {
                slot_unuse[i] = i;
            }
            nof_slot_unuse = total_slots;

            /* Start to select unused slots for each client */
            for (i = 0; i < nof_clients; i++) {
                client_id = client_a_base + i;
                nof_slots_per_client = client_speed[i] / 5000;

                for (ii = 0; ii < nof_slots_per_client; ii++) {
                    /* Randomly find out a unused slot from array slot_unuse[] */
                    rnd_index = sal_rand() % nof_slot_unuse;
                    slot_index = slot_unuse[rnd_index];
                    Cal_Slots_A[slot_index] = client_id;

                    /* Remove the selected slot from slot_unuse[] */
                    for (iii = rnd_index; iii < nof_slot_unuse - 1; iii++) {
                        slot_unuse[iii] = slot_unuse[iii+1];
                    }
                    nof_slot_unuse--;
                }
            }
        }

        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_A)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, FLEXE_CAL_A, total_slots), error_msg);

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 80; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_B)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, FLEXE_CAL_B, total_slots), error_msg);

        /* Group ID in overhead */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

        /* Store slot setting for calendar switching */
        for (i = 0; i < 80; i++) {
            if ($unit_iter) {
                cal_slots_unit1[i] = Cal_Slots_A[i];
            } else {
                cal_slots_unit0[i] = Cal_Slots_A[i];
            }
        }
    }

    /* Calendar Rx setting if calendar_cfg is 2(different slot assignment in Tx&Rx) */
    if (calendar_cfg == 2) {
        /* Set calendar Rx on centralized device with calendar Tx setting on distributed device */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_unit1[i];
            Cal_Slots_B[i] = cal_slots_unit1[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d), Rx", unit_cen);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots), error_msg);

        /* Set calendar Rx on distributed device with calendar Tx setting on centralized device */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_unit0[i];
            Cal_Slots_B[i] = cal_slots_unit0[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, Rx)", unit_dis);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots), error_msg);
    }

    /* Default VLAN action set */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit_cen), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit_dis), "");

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Loopback setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, cud1_id, 1), "");

    return BCM_E_NONE;
}

/* Funtion to do calendar switching based on the setting of funtion 
 * dnx_flexe_slots_assign_for_mixed_clients_main()
 */
int dnx_flexe_calendar_switch_with_mixed_clients_main(
    int unit_cen,
    int unit_dis,
    bcm_port_flexe_group_cal_t calendar_id,
    int client_index,
    int is_client_removal,
    int all_clients_removed,
    int is_sync_slots)
{
    int i, unit_iter, unit;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    bcm_port_t client_a_base = 30;
    int client_id = client_a_base + client_index;
    int cal_slots_init_tx[80], cal_slots_init_rx[80];
    int cal_slots_tx[80], cal_slots_rx[80];
    char error_msg[200]={0,};
    int cal_id_int;

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    grp_speed = is_reduced_cdu_lane_speed ? 200000 : 400000;
    total_slots = grp_speed / 5000;

    bcm_gport_t flexe_grp;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    bcm_port_flexe_group_cal_t cal_active;
    uint32 cal_flags = is_sync_slots ? 0 : BCM_PORT_FLEXE_TX;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_calendar_switch_with_mixed_clients_main";

    /***************************************************************************
     * Datapath:
     *
     *                           Centralized #0                       Centralized #1
     *                     +----------------------------+      +----------------------------+
     *               +<--->|<--> BusB_0   <--> BusA_0   |<---->|<--> BusA_0   <--> BusB_0   |<--->+
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_1   <--> BusA_1   |<---->|<--> BusA_1   <--> BusB_1   |<--->|
     *               |     |                            |      |                            |     |
     * eth_tx_rx<--->+<--->|<-->  ...     <-->  ...     |<---->|<-->  ...     <-->  ...     |<--->+<--->CDU1(Loopback)
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_N-1 <--> BusA_N-1 |<---->|<--> BusA_N-1 <--> BusB_N-1 |<--->|
     *               |     |                            |      |                            |     |
     *               +<--->|<--> BusB_N   <--> BusA_N   |<---->|<--> BusA_N   <--> BusB_N   |<--->+
     *                     +----------------------------+      +----------------------------+
     * (Centralized #0 -- eth_tx_rx->BusB_X: L2 lookup; BusB_X->eth_tx_rx: force_forward)
     * (Centralized #1 -- CDU1->BusB_X: L2 lookup; BusB_X->CDU1: force_forward)
     **************************************************************************/

    /* Do calendar switching on the 2 devices */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Check the provided calendar ID should be in inactive status */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active), error_msg);
        if (cal_active == calendar_id && !all_clients_removed) {
            printf("**** %s:  Error, the provided calendar ID is already in active status ****\n", proc_name);
            return BCM_E_FAIL;
        }
        if (cal_active != calendar_id && all_clients_removed) {
            printf("**** %s:  Error, the provided calendar ID isn't active calendar ID ****\n", proc_name);
            return BCM_E_FAIL;
        }

        /* Get slot assignment on active calendar */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 80, cal_slots_tx, &i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 80, cal_slots_rx, &i), error_msg);

        /* Load initial calendar setting */
        for (i = 0; i < 80; i++) {
            cal_slots_init_tx[i] = unit_iter ? cal_slots_unit1[i] : cal_slots_unit0[i];
            if (is_sync_slots) {
                cal_slots_init_rx[i] = unit_iter ? cal_slots_unit1[i] : cal_slots_unit0[i];
            } else {
                cal_slots_init_rx[i] = unit_iter ? cal_slots_unit0[i] : cal_slots_unit1[i];
            }
        }

        /* Add/remove the client into/from the inactive calendar */
        for (i = 0; i < 80; i++) {
            if (cal_slots_init_tx[i] == client_id) {
                /* Find out a slot for the client in initial slot array */
                cal_slots_tx[i] = is_client_removal ? 0 : client_id;
            }
            if (cal_slots_init_rx[i] == client_id) {
                /* Find out a slot for the client in initial slot array */
                cal_slots_rx[i] = is_client_removal ? 0 : client_id;
            }
        }

        /* Configure the inactive calendar(Tx or Tx+Rx) */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_tx[i];
            Cal_Slots_B[i] = cal_slots_tx[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, calendar_id, total_slots), error_msg);

        /* Configure the inactive calendar(Rx only for asynchronous slot assignment) */
        if (!is_sync_slots) {
            for (i = 0; i < 80; i++) {
                Cal_Slots_A[i] = cal_slots_rx[i];
                Cal_Slots_B[i] = cal_slots_rx[i];
            }
            snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, calendar_id, total_slots), error_msg);

        }

        /* Set the the inactive calendar as CalInUse in overhead before the switching */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, calendar_id), error_msg);

        /* Do calendar switching by activing the inactive calendar */
        cal_id_int = calendar_id;
        snprintf(error_msg, sizeof(error_msg), "fail to active calendar %d on unit %d", cal_id_int, unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id), error_msg);
    }
    sal_sleep(3);

    /* Set backup calendar to be the same as active calendar
     * It is used to avoid packet loss during calendar switching */
    for (i = 0; i < 80; i++) {
        Cal_Slots_A[i] = cal_slots_tx[i];
        Cal_Slots_B[i] = cal_slots_tx[i];
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_RX, !calendar_id, total_slots),
        "(cen_rx, cal_backup)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, !calendar_id, total_slots),
        "(dis_tx, cal_backup)");

    for (i = 0; i < 80; i++) {
        Cal_Slots_A[i] = cal_slots_rx[i];
        Cal_Slots_B[i] = cal_slots_rx[i];
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_TX, !calendar_id, total_slots),
        "(cen_tx, cal_backup)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_RX, !calendar_id, total_slots),
        "(dis_rx, cal_backup)");

    return BCM_E_NONE;
}

/* Funtion to test FlexE PHY removing and re-adding
 * It will create a FlexE group with given FlexE PHYs. And then assign slots on these PHYs according to phy_bmp
 */
int dnx_flexe_phy_add_remove_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t cud1_id,
    int max_phys,
    int phy_speed,
    int grp_index,
    int phy_bmp)
{
    int i, unit_iter, unit;
    bcm_gport_t flexe_grp;
    int grp_speed;
    int nof_phys_init; /* The initial number of FlexE PHYs in the grp. max_phys means maximal PHY number allowed in the grp */
    int nof_clients = max_phys;
    int client_speed = phy_speed;
    int total_slots;
    int nof_slots_per_client = client_speed / 5000;
    int phy_index, slot_index;
    int vlan_id_base = 1000 + grp_index*10;
    bcm_port_t client_a_base = 30 + grp_index*10;
    bcm_port_t client_b_base = 130 + grp_index*10;
    bcm_port_t eth_ports[2] = {eth_tx_rx, cud1_id};
    bcm_port_t eth_port;
    char error_msg[200]={0,};


    /* Comupte the initial number of FlexE PHYs in the group */
    for (phy_index = 0; phy_index < max_phys; phy_index++) {
        if ((1 << phy_index) & phy_bmp) {
            nof_phys_init++;
        }
    }
    grp_speed = nof_phys_init * phy_speed;
    total_slots = grp_speed / 5000;

    /***************************************************************************
     * Datapath:
     *
     *                           Centralized #0                      Centralized #1
     *                     +----------------------------+          +----------------------------+
     *               +<--->|<--> BusB_0   <--> BusA_0   |<--Phy0-->|<--> BusA_0   <--> BusB_0   |<--->+
     *               |     |                            |          |                            |     |
     *               |<--->|<--> BusB_1   <--> BusA_1   |<--Phy1-->|<--> BusA_1   <--> BusB_1   |<--->|
     *               |     |                            |          |                            |     |
     * eth_tx_rx<--->+<--->|<-->  ...     <-->  ...     |<--... -->|<-->  ...     <-->  ...     |<--->+<--->CDU1(Loopback)
     *               |     |                            |          |                            |     |
     *               |<--->|<--> BusB_N-1 <--> BusA_N-1 |<--... -->|<--> BusA_N-1 <--> BusB_N-1 |<--->|
     *               |     |                            |          |                            |     |
     *               +<--->|<--> BusB_N   <--> BusA_N   |<--PhyN-->|<--> BusA_N   <--> BusB_N   |<--->+
     *                     +----------------------------+          +----------------------------+
     * (Centralized #0 -- eth_tx_rx->BusB_X: L2 lookup; BusB_X->eth_tx_rx: force_forward)
     * (Centralized #1 -- CDU1->BusB_X: L2 lookup; BusB_X->CDU1: force_forward)
     **************************************************************************/

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;
        eth_port = eth_ports[unit_iter];

        /* Create FlexE group with the PHYs in array flexe_phy_ports[] */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys_init), error_msg);

        /* Create BusA clients and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0), error_msg);

            snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed, 0), error_msg);
        }

        /* Setting FlexE flows between BusA and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to set BusA2BusB FlexE flow(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);

            snprintf(error_msg, sizeof(error_msg), "fail to set BusB2BusA FlexE flow(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
        }

        /* L2 settings between ETH port and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            /* ETH to BusB_i: L2 lookup(DMAC=00:00:00:00:22:22, VID=vlan_id_base+i) */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_switch_config(unit, eth_port, client_b_base+i, vlan_id_base+i), error_msg);

            /* BusB_i to ETH: force_forward */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_base+i, eth_port, 1), error_msg);
        }

        /* Calendar setting */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        slot_index = 0;
        for (phy_index = 0; phy_index < max_phys; phy_index++) {
            if ((1 << phy_index) & phy_bmp) {
                /* Need assign slots on this PHY */
                for (i = 0; i < nof_slots_per_client; i++) {
                    Cal_Slots_A[slot_index++] = phy_index + client_a_base;
                }
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_A)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

        /* Group ID in overhead */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index), error_msg);
    }

    /* Default VLAN action set */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit_cen), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit_dis), "");

    /* Loopback setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, cud1_id, 1), "");

    /* Synce check */
    sal_sleep(6);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Funtion to clear or re-add slots on specific FlexE PHY
 * It ccouldn't be called before function dnx_flexe_phy_add_remove_main()
 */
int dnx_flexe_phy_cal_set_clear(
    int unit_cen,
    int unit_dis,
    int phy_speed,
    int phy_index,
    int grp_index,
    int clear_cal_flag,
    bcm_port_t client_id)
{
    int i, unit_iter, unit;
    bcm_gport_t flexe_grp;
    int total_slots;
    int nof_slots_per_phy = phy_speed / 5000;
    int cal_slots_tx[80];
    int client_id_in_cal = clear_cal_flag ? 0 : client_id;
    char error_msg[200]={0,};


    /* Get group GPORT */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Compute the started and ended slot  ID on the PHY */
    int slot_start_on_phy = nof_slots_per_phy * phy_index;
    int slot_end_on_phy = nof_slots_per_phy * (phy_index + 1) - 1;

    /***************************************************************************
     * Datapath:
     *
     *                           Centralized #0                      Centralized #1
     *                     +----------------------------+          +----------------------------+
     *               +<--->|<--> BusB_0   <--> BusA_0   |<--Phy0-->|<--> BusA_0   <--> BusB_0   |<--->+
     *               |     |                            |          |                            |     |
     *               |<--->|<--> BusB_1   <--> BusA_1   |<--Phy1-->|<--> BusA_1   <--> BusB_1   |<--->|
     *               |     |                            |          |                            |     |
     * eth_tx_rx<--->+<--->|<-->  ...     <-->  ...     |<--... -->|<-->  ...     <-->  ...     |<--->+<--->CDU1(Loopback)
     *               |     |                            |          |                            |     |
     *               |<--->|<--> BusB_N-1 <--> BusA_N-1 |<--... -->|<--> BusA_N-1 <--> BusB_N-1 |<--->|
     *               |     |                            |          |                            |     |
     *               +<--->|<--> BusB_N   <--> BusA_N   |<--PhyN-->|<--> BusA_N   <--> BusB_N   |<--->+
     *                     +----------------------------+          +----------------------------+
     * (Centralized #0 -- eth_tx_rx->BusB_X: L2 lookup; BusB_X->eth_tx_rx: force_forward)
     * (Centralized #1 -- CDU1->BusB_X: L2 lookup; BusB_X->CDU1: force_forward)
     **************************************************************************/

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Get calendar setting */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, 80, cal_slots_tx, &total_slots), error_msg);

        /* Set or clear BusA client on the PHY */
        for (i = 0; i < total_slots; i++) {
            if (i >= slot_start_on_phy && i <= slot_end_on_phy) {
                cal_slots_tx[i] = client_id_in_cal;
            }
        }

        /* Calendar setting */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (i = 0; i < total_slots; i++) {
            Cal_Slots_A[i] = cal_slots_tx[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to create mixed L1&L2 FlexE datapaths on the 2 devices
 */
int dnx_flexe_cen_dis_mix_L1_L2_traffic_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t eth_1,
    bcm_port_t eth_2,
    bcm_port_t ilkn_1,
    bcm_port_t ilkn_2,
    int eth1_is_L1,
    int eth2_is_L1)
{
    int i, ii;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 400000;
    int client_speed = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    int nof_slots_per_client = client_speed / 5000;
    bcm_gport_t flexe_grp;
    flexe_port_type_t client_b1_type = eth1_is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;
    flexe_port_type_t client_b2_type = eth2_is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;
    char error_msg[200]={0,};

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_1 = 30;
    bcm_port_t client_a_2 = 31;
    bcm_port_t client_b_1 = 32;
    bcm_port_t client_b_2 = 33;
    bcm_port_t client_c_1 = 34;
    bcm_port_t client_c_2 = 35;


    /***************************************************************************
     * The L1/L2/L3 datapaths are:
     *
     *                    Centralized Device                   Distributed Device
     *            +----------------------------+    +---------------------------------------------------+
     *            |                            |    |                                                   |
     *  eth_1<--->| client_b_1 <--> client_a_1 |<-->| client_a_1 <--> client_b_1/client_c_1 <--> ilkn_1 |
     *            |                            |    | (1st Flexe<->ILKN datapath)                       |
     *            |                            |    |                                                   |
     *            |                            |    | (2nd Flexe<->ILKN datapath)                       |
     *  eth_2<--->| client_b_2 <--> client_a_2 |<-->| client_a_2 <--> client_b_2/client_c_2 <--> ilkn_2 |
     *            |                            |    |                                                   |
     *            +----------------------------+    +---------------------------------------------------+
     *
     **************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_1, port_type_busA, client_speed, 0),
        "fail to create client_a_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_2, port_type_busA, client_speed, 0),
        "fail to create client_a_2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_1, client_b1_type, client_speed, 0),
        "fail to create client_b_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_2, client_b2_type, client_speed, 0),
        "fail to create client_b_2");

    /* Create the 1st datapath: eth_1 <--> client_b_1 <--> client_a_1 */
    if (eth1_is_L1) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_1, client_b_1, client_a_1, 0),
            "fail to set 1st datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_1, client_b_1, eth_1, 0),
            "fail to set 1st datapath on centralized device");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_1, client_b_1, client_a_1, 0),
            "fail to set 1st datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a_1, client_b_1, eth_1, 0),
            "fail to set 1st datapath on centralized device");
    }

    /* Create the 2nd datapath: eth_2 <---> client_b_2 <--> client_a_2 */
    if (eth2_is_L1) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_2, client_b_2, client_a_2, 0),
            "fail to set 2nd datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_2, client_b_2, eth_2, 0),
            "fail to set 2nd datapath on centralized device");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_2, client_b_2, client_a_2, 0),
            "fail to set 2nd datapath on centralized device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a_2, client_b_2, eth_2, 0),
            "fail to set 2nd datapath on centralized device");
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_1, port_type_busA, client_speed, 0),
        "fail to create client_a_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_2, port_type_busA, client_speed, 0),
        "fail to create client_a_2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_1, port_type_busB_L2_L3, client_speed, 0),
        "fail to create client_b_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_2, port_type_busB_L2_L3, client_speed, 0),
        "fail to create client_b_2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c_1, port_type_busC, client_speed, 0),
        "fail to create client_c_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c_2, port_type_busC, client_speed, 0),
        "fail to create client_c_2");

    /* Starting to create 1st datapath */
    if (eth1_is_L1) {
        /* Flexe&ILKN L1 datapath: client_a_1 <--> client_c_1 <--> ilkn_1 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a_1, client_c_1, ilkn_1, 0),
            "fail to set 1st datapath on distributed device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_1, client_c_1, client_a_1, 0),
            "fail to set 1st datapath on distributed device");
    } else {
        /* Flexe&ILKN L2 datapath: client_a_1 <--> client_b_1 <--> ilkn_1 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a_1, client_b_1, ilkn_1, 0),
            "fail to set 1st datapath on distributed device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_1, client_b_1, client_a_1, 0),
            "fail to set 1st datapath on distributed device");
    }

    /* Starting to create 2nd datapath */
    if (eth2_is_L1) {
        /* Flexe&ILKN L1 datapath: client_a_2 <--> client_c_2 <--> ilkn_2 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a_2, client_c_2, ilkn_2, 0),
            "fail to set 1st datapath on distributed device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_2, client_c_2, client_a_2, 0),
            "fail to set 1st datapath on distributed device");
    } else {
        /* Flexe&ILKN L2 datapath: client_a_2 <--> client_b_2 <--> ilkn_2 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a_2, client_b_2, ilkn_2, 0),
            "fail to set 1st datapath on distributed device");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_2, client_b_2, client_a_2, 0),
            "fail to set 1st datapath on distributed device");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots_per_client*2; ii++) {
            Cal_Slots_A[ii] = ii < nof_slots_per_client ? client_a_1 : client_a_2;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy), error_msg);
    }

    /* Synce check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Function to check OH alarm cal_mismatch */
int dnx_flexe_oh_alarm_cal_mismatch_check(
    int unit,
    bcm_port_t phy_id,
    int alarm_active_exp,
    bcm_port_flexe_group_cal_t cal_id,
    uint32 value_0,
    uint32 value_1,
    uint32 value_2)
{
    bcm_port_flexe_oh_alarm_t oh_alarm_get;

    /* Get OH alarms */
    sal_sleep(2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit, phy_id, 0, &oh_alarm_get), "");
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_alarm_get(unit, phy_id, 0, &oh_alarm_get), "");

    /* Check if OH alarm(s) raised or cleared */
    if (oh_alarm_get.alarm_active != alarm_active_exp) {
        print "---- Error: Mismached oh_alarm_get.alarm_active ----";
        print oh_alarm_get;
        return BCM_E_FAIL;
    }

    /* Check mismathed slots */
    if (cal_id == FLEXE_CAL_A) {
        if (oh_alarm_get.cal_a_mismatch[0] != value_0 || oh_alarm_get.cal_a_mismatch[1] != value_1 || oh_alarm_get.cal_a_mismatch[2] != value_2) {
            print "---- Error: Mismached values in cal_a_mismatch[] ----";
            print oh_alarm_get;
            return BCM_E_FAIL;
        }
    } else {
        if (oh_alarm_get.cal_b_mismatch[0] != value_0 || oh_alarm_get.cal_b_mismatch[1] != value_1 || oh_alarm_get.cal_b_mismatch[2] != value_2) {
            print "---- Error: Mismached values in cal_b_mismatch[] ----";
            print oh_alarm_get;
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

/* Function to create FlexE L1&L2 datapath with management ports on centralized and distributed devices */
int dnx_flexe_L1_L2_datapath_with_mgmt_ports(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy,
    bcm_port_t cen_port_L1,
    bcm_port_t cen_port_L2,
    bcm_port_t dis_port_L1)
{
    int i;
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    int client_speed = 10000;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 40; /* BusA client for L1 traffic on centralized device */
    bcm_port_t client_a_L2 = 41; /* BusA client for L2 traffic on centralized device */
    bcm_port_t client_b_L1 = 42; /* BusB client for L1 traffic on centralized device */
    bcm_port_t client_b_L2 = 43; /* BusB client for L2 traffic on centralized device */
    bcm_port_t client_a_1 = 40;  /* BusA client to send traffic on distributed device */
    bcm_port_t client_a_2 = 41;  /* BusA client to receive traffic on distributed device */
    bcm_port_t client_b_2 = 44;  /* BusB client on distributed device */


    /***************************************************************************
     * The L1/L2 datapaths are:
     *
     *                      Centralized Device                 Distributed Device
     * (phy_loopback)  +----------------------------+     +----------------------------+
     * cen_port_L1<--> | client_b_L1<---client_a_L1 |<----| client_a_1 <--------+      |
     *                 |      |                     |     |                     |      |
     * (Traffic Tx&Rx) |      v                     |     |                     |      |
     * cen_port_L2<--> | client_b_L2 -->client_a_L2 |---->| client_a_2 ---> client_b_2 |<--> dis_port_L1(phy_loopback)
     *                 +----------------------------+     +----------------------------+
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, client_speed, 0),
        "fail to create client_a_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_a_L2, port_type_busA, client_speed, 0),
        "fail to create client_a_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L1, client_speed, 0),
        "fail to create client_b_L1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_cen, client_b_L2, port_type_busB_L2_L3, client_speed, 0),
        "fail to create client_b_L2");

    /* Create L2 Tx datapath: cen_port_L2 -> force_fwd -> client_b_L2 -> client_a_L2 */
    BCM_IF_ERROR_RETURN_MSG( dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, cen_port_L2, client_b_L2, client_a_L2, 0),
        "fail to set L2 Tx datapath");

    /* Create L1 Rx datapath: client_a_L1 -> client_b_L1 -> cen_port_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_L1, client_b_L1, cen_port_L1, 0),
        "fail to set L1 Rx datapath");

    /* Redirect traffic from cen_port_L1 to cen_port_L2: cen_port_L1 -> client_b_L1 -> client_b_L2 -> cen_port_L2 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_cen, cen_port_L1, 0),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_cen, client_b_L1, 0),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit_cen, 0, cen_port_L1, 0, client_b_L1),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_cen, cen_port_L1, 1),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_cen, client_b_L1, 1),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L1, client_b_L2, 0),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit_cen, client_b_L2, cen_port_L2, 1),
        "fail to redirect traffic from cen_port_L1 to cen_port_L2");

    /* Calendar and OH settings */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    Cal_Slots_A[0] = client_a_1;
    Cal_Slots_A[1] = client_a_1;
    Cal_Slots_A[2] = client_a_2;
    Cal_Slots_A[3] = client_a_2;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_1, port_type_busA, client_speed, 0),
        "fail to create client_a_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a_2, port_type_busA, client_speed, 0),
        "fail to create client_a_2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_2, port_type_busB_L1, client_speed, 0),
        "fail to create client_b_2");

    /* Create L1 datapath: client_a_2 --> client_b_2 --> dis_port_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_2, client_b_2, dis_port_L1, 0),
        "fail to set L1 datapath(Flexe2Eth) on distributed device");

    /* Create L1 datapath: client_a_1 <-- client_b_2 <-- dis_port_L1 */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, dis_port_L1, client_b_2, client_a_1, 0),
        "fail to set L1 datapath(Eth2Flexe) on distributed device");

    /* Calendar and OH settings */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check PHY port link status */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit_dis, flexe_phy), "FlexE PHY port linkdown");

    return BCM_E_NONE;
}

/* Function to test asymmetrical client speeds between BusA and BusB */
int dnx_flexe_asym_client_speed_busA_busB(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_cen,
    bcm_port_t eth_port_dis,
    bcm_port_t phy_port,
    int is_L1_switch,
    int client_tx_rate,
    int client_rx_rate)
{
    int i, ii;
    int nof_pcs = 1;
    int unit;
    bcm_port_t client_a = 40;
    bcm_port_t client_b = 50;
    bcm_gport_t flexe_grp;
    int client_speed_tx;
    int client_speed_rx;
    int nof_slots_tx;
    int nof_slots_rx;
    int total_slots = 80; /* Need 400G FlexE PHY */
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit_cen, -1);
    int grp_slot_speed, grp_speed;
    bcm_port_resource_t port_res;
    uint32 flag_asym_rate = BCM_PORT_RESOURCE_ASYMMETRICAL;
    flexe_port_type_t client_b_type = is_L1_switch ? port_type_busB_L1 : port_type_busB_L2_L3;
    char error_msg[200]={0,};

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    total_slots = is_reduced_cdu_lane_speed ? 20 : 80;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_asym_client_speed_busA_busB";

    /***************************************************************************
     * Datapath:
     *                             Centralized #0               Centralized #1
     *                         +-----------------------+    +-----------------------+
     * eth_port_cen <--L1/L2-->| client_b <-> client_a |<-->| client_a <-> client_b |<--L1/L2--> eth_port_dis
     *                         +-----------------------+    +-----------------------+
     ***************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized #0
     ***************************************************************************/
    unit = unit_cen;

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    if (phy_port > 0) {
        flexe_phy_ports[0] = phy_port;
    } else {
        /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 16; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
        if (!nof_pcs) {
            printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        }
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create a busA client and a busB client with asymmetrical speeds */
    client_speed_tx = client_tx_rate;
    client_speed_rx = client_rx_rate;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, client_b_type, client_speed_tx, flag_asym_rate), error_msg);

    /* Start to build ETH2FlexE datapath: eth_port_cen <-> client_b <-> client_a */
    if (is_L1_switch) {
        snprintf(error_msg, sizeof(error_msg), "fail to set L1 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_cen, client_b, client_a, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_cen, 0), error_msg);
    } else {
        snprintf(error_msg, sizeof(error_msg), "fail to set L2 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_cen, client_b, client_a, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_cen, 0), error_msg);
    }

    /* Compute how many slots should be used */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    if (is_flexe_gen2) {
        /* Compute grp speed */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, flexe_phy_ports[0], &port_res), "");
        grp_speed =  port_res.speed * nof_pcs;
        total_slots = grp_speed / grp_slot_speed;
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / grp_slot_speed;
    nof_slots_rx = client_speed_rx / grp_slot_speed;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, tx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots), error_msg);

    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, rx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);

    /* FlexE OH setting */
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE setting on centralized #1
     ***************************************************************************/
    unit = unit_dis;

    /* Create FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create a busA client and a busB client with asymmetrical speeds */
    client_speed_tx = client_rx_rate;
    client_speed_rx = client_tx_rate;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, client_b_type, client_speed_tx, flag_asym_rate), error_msg);

    /* Start to build ETH2FlexE datapath: eth_port_dis <-> client_b <-> client_a */
    if (is_L1_switch) {
        snprintf(error_msg, sizeof(error_msg), "fail to set L1 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_dis, client_b, client_a, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_dis, 0), error_msg);
    } else {
        snprintf(error_msg, sizeof(error_msg), "fail to set L2 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_dis, client_b, client_a, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_dis, 0), error_msg);
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / grp_slot_speed;
    nof_slots_rx = client_speed_rx / grp_slot_speed;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, Tx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots), error_msg);

    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, Rx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);

    /* FlexE OH setting */
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check link status of FlexE PHY(s) */
    if (phy_port > 0) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit_cen, phy_port), "FlexE PHY port linkdown");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit_dis, phy_port), "FlexE PHY port linkdown");
    }

    return BCM_E_NONE;
}

/* Function to test asymmetrical client speeds under distributed mode */
int dnx_flexe_asym_client_speed_distributed_mode(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port,
    bcm_port_t ilkn_port,
    bcm_port_t phy_port,
    int is_L1_switch_cen,
    int is_L1_switch_dis,
    int rate_flexe2ilkn,
    int rate_ilkn2flexe)
{
    int i, ii;
    int nof_pcs = 1;
    int unit;
    bcm_port_t client_a = 51;
    bcm_port_t client_b = 52;
    bcm_port_t client_c = 53;
    bcm_gport_t flexe_grp;
    int client_speed_tx;
    int client_speed_rx;
    int nof_slots_tx;
    int nof_slots_rx;
    int total_slots = 80; /* Need 400G FlexE PHY */
    flexe_port_type_t client_b_type_cen = is_L1_switch_cen ? port_type_busB_L1 : port_type_busB_L2_L3;
    uint32 flag_asym_rate = BCM_PORT_RESOURCE_ASYMMETRICAL;
    char error_msg[200]={0,};


    /***************************************************************************
     * The datapath is:
     *
     *                         Centralized Device                     Distributed Device
     *                      +----------------------+      +-------------------------------------+
     *                      |                      |      |                                     |
     *    eth_port<--L1/L2->| client_b<-->client_a | <--> | client_a<-->client_b/c<-->ilkn_port |
     *                      |                      |      | (Flexe<->ILKN L1 or L2&L3 datapath) |
     *                      |                      |      |                                     |
     *                      +----------------------+      +-------------------------------------+
     *
     ***************************************************************************/

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/
    unit = unit_dis;

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients with asymmetrical speeds */
    client_speed_tx = rate_ilkn2flexe;
    client_speed_rx = rate_flexe2ilkn;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate), error_msg);
    if (is_L1_switch_dis) {
        snprintf(error_msg, sizeof(error_msg), "fail to create busC client(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed_tx, flag_asym_rate), error_msg);
    } else {
        snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB_L2_L3, client_speed_tx, flag_asym_rate), error_msg);
    }

    /* Build L1 or L2 datapath between FlexE and ILKN */
    if (is_L1_switch_dis)
    {
        /* Flexe2ILKN L1 datapath: client_a <-> client_c <-> ilkn_port */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a, client_c, ilkn_port, 0),
            "fail to set L1 datapath between FlexE and ILKN");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, ilkn_port, client_c, client_a, 0),
            "fail to set L1 datapath between FlexE and ILKN");
    }
    else
    {
        /* Flexe2ILKN L2/L3 datapath: client_a <-> client_b <-> ilkn_port */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit, client_a, client_b, ilkn_port, 0),
            "fail to set L2 datapath between FlexE and ILKN");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit, ilkn_port, client_b, client_a, 0),
            "fail to set L2 datapath between FlexE and ILKN");
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / 5000;
    nof_slots_rx = client_speed_rx / 5000;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, tx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots), error_msg);

    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, rx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);

    /* FlexE OH setting */
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/
    unit = unit_cen;

    /* Create FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients with asymmetrical speeds */
    client_speed_tx = rate_flexe2ilkn;
    client_speed_rx = rate_ilkn2flexe;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, client_b_type_cen, client_speed_tx, flag_asym_rate), error_msg);

    /* Build L1 or L2 datapath between ETH and FlexE */
    if (is_L1_switch_cen)
    {
        /* L1 datapath between ETH and FlexE  */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0),
            "fail to set L1 datapath between ETH and FlexE");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0),
            "fail to set L1 datapath between ETH and FlexE");
    }
    else
    {
        /* L2 datapath between ETH and FlexE  */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0),
            "fail to set L2 datapath between ETH and FlexE");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0),
            "fail to set L2 datapath between ETH and FlexE");
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / 5000;
    nof_slots_rx = client_speed_rx / 5000;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, tx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots), error_msg);

    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d, rx)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);

    /* FlexE OH setting */
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    /* Check link status of FlexE PHY(s) */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit_cen, phy_port), "FlexE PHY port linkdown");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(unit_dis, phy_port), "FlexE PHY port linkdown");

    return BCM_E_NONE;
}

/* Function to test asymmetrical client speeds between ETH_L1 and ILKN_L1 */
int dnx_flexe_asym_client_speed_busB_busC(
    int unit,
    bcm_port_t eth_port,
    bcm_port_t ilkn_port,
    int rate_eth2ilkn,
    int rate_ilkn2eth)
{
    int i;
    bcm_port_t client_b = 51;
    bcm_port_t client_c = 52;
    uint32 flag_asym_rate = BCM_PORT_RESOURCE_ASYMMETRICAL;


    /* Create client_b */
    client_rx_speed = rate_ilkn2eth; /* global variable for client Rx speed */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB_L1, rate_eth2ilkn, flag_asym_rate),
        "fail to create client_b");

    /* Create client_c */
    client_rx_speed = rate_eth2ilkn; /* global variable for client Rx speed */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, rate_ilkn2eth, flag_asym_rate),
        "fail to create client_c");

    /***************************************************************************
     * Datapath:
     *                                        Distributed Device
     *                          +---------------------------------------------+
     *                          |                                             |
     *  eth_port <--L1_switch-->| client_b<-->client_c<-->ilkn_port(loopback) |
     *                          |                                             |
     *                          +---------------------------------------------+
     *
     ****************************************************************************/

    /* Start to build Eth2ILKN L1 datapath: eth_port -> client_b -> client_c -> ilkn_port */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_ilkn(unit, eth_port, client_b, client_c, ilkn_port, 0),
        "fail to set Eth2ILKN L1 datapath");

    /* Start to build ILKN2Eth L1 datapath: ilkn_port -> client_c -> client_b -> eth_port */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_ilkn_to_eth(unit, ilkn_port, client_c, client_b, eth_port, 0),
        "fail to set ILKN2Eth L1 datapath");

    return BCM_E_NONE;
}

/* Function to test multiple clients with asymmetrical client speeds */
int dnx_flexe_cal_with_asym_client_speeds(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_cen,
    bcm_port_t eth_dis,
    bcm_port_t flexe_phy)
{
    int i, ii, iii, unit_iter, unit;
    int nof_flexe_phys = 1;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    int vlan_id_base = 1000;
    bcm_port_t client_a_base = 30;
    bcm_port_t client_b_base = 130;
    bcm_port_t eth_port, eth_ports[2] = {eth_cen, eth_dis};
    int client_speed_cen2dis[80];
    int client_speed_dis2cen[80];
    int client_rate_tx, client_rate_rx;
    int client_id, nof_slots_per_client, slot_index;
    bcm_gport_t flexe_grp;
    uint32 flag_asym_rate = BCM_PORT_RESOURCE_ASYMMETRICAL;
    char error_msg[200]={0,};

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    grp_speed = is_reduced_cdu_lane_speed ? 100000 : 400000;
    total_slots = grp_speed / 5000;


    /* 3 clients with asymmetrical speeds in Tx and Rx */
    int nof_clients = 3;
    client_speed_cen2dis[0] = 25000;
    client_speed_cen2dis[1] = 30000;
    client_speed_cen2dis[2] = 45000;
    client_speed_dis2cen[0] = 45000;
    client_speed_dis2cen[1] = 20000;
    client_speed_dis2cen[2] = 25000;

    /***************************************************************************
     * Datapath:
     *
     *                           Centralized #0                      Centralized #1
     *                     +----------------------------+      +----------------------------+
     *               +<--->|<--> BusB_0   <--> BusA_0   |<---->|<--> BusA_0   <--> BusB_0   |<--->+
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_1   <--> BusA_1   |<---->|<--> BusA_1   <--> BusB_1   |<--->|
     *               |     |                            |      |                            |     |
     *  eth_cen <--->+<--->|<-->  ...     <-->  ...     |<---->|<-->  ...     <-->  ...     |<--->+<---> eth_dis
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_N-1 <--> BusA_N-1 |<---->|<--> BusA_N-1 <--> BusB_N-1 |<--->|
     *               |     |                            |      |                            |     |
     *               +<--->|<--> BusB_N   <--> BusA_N   |<---->|<--> BusA_N   <--> BusB_N   |<--->+
     *                     +----------------------------+      +----------------------------+
     * (Centralized #0 -- eth_cen->BusB_X: L2 lookup; BusB_X->eth_cen: force_forward)
     * (Centralized #1 -- eth_dis->BusB_X: L2 lookup; BusB_X->eth_dis: force_forward)
     **************************************************************************/

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;
        eth_port = eth_ports[unit_iter];

        /***************************************************************************
         * FlexE group and client creation
         ***************************************************************************/

        /* Create FlexE group with the 400G FlexE PHY */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        flexe_phy_ports[0] = flexe_phy;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_flexe_phys), error_msg);

        /* Create BusA clients and BusB clients with asymmetrical speeds */
        for (i = 0; i < nof_clients; i++) {
            client_rate_tx = unit_iter ? client_speed_dis2cen[i] : client_speed_cen2dis[i];
            client_rate_rx = unit_iter ? client_speed_cen2dis[i] : client_speed_dis2cen[i];
            client_rx_speed = client_rate_rx; /* global variable for client Rx speed */

            snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, 0, 0), error_msg);
            snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3,client_rate_tx, flag_asym_rate), error_msg);
        }

        /***************************************************************************
         * Build datapath
         ***************************************************************************/

        /* Setting FlexE flows between BusA and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            snprintf(error_msg, sizeof(error_msg), "fail to set BusA2BusB FlexE flow(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);

            snprintf(error_msg, sizeof(error_msg), "fail to set BusB2BusA FlexE flow(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
        }

        /* L2 settings between ETH port and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            /* ETH to BusB_i: L2 lookup(DMAC=00:00:00:00:22:22, VID=vlan_id_base+i) */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_switch_config(unit, eth_port, client_b_base+i, vlan_id_base+i), error_msg);

            /* BusB_i to ETH: force_forward */
            snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_base+i, eth_port, 1), error_msg);
        }

        /***************************************************************************
         * Calendar setting in Tx direction
         ***************************************************************************/

        /* Allocate slots for each client in Tx direction */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        slot_index = 0;
        for (i = 0; i < nof_clients; i++) {
            client_id = client_a_base + i;
            client_rate_tx = unit_iter ? client_speed_dis2cen[i] : client_speed_cen2dis[i];
            nof_slots_per_client = client_rate_tx / 5000;

            for (ii = 0; ii < nof_slots_per_client; ii++) {
                Cal_Slots_A[slot_index++] = client_id;
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_A)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots), error_msg);

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 80; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_B)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_B, total_slots), error_msg);

        /* Use array cal_slots_unit0[] to store Tx slots */
        for (i = 0; i < 80; i++) {
            if (unit_iter == 0) {
                cal_slots_unit0[i] = Cal_Slots_A[i];
            }
        }

        /***************************************************************************
         * Calendar setting in Rx direction
         ***************************************************************************/

        /* Allocate slots for each client in Rx direction */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        slot_index = 0;
        for (i = 0; i < nof_clients; i++) {
            client_id = client_a_base + i;
            client_rate_rx = unit_iter ? client_speed_cen2dis[i] : client_speed_dis2cen[i];
            nof_slots_per_client = client_rate_rx / 5000;

            for (ii = 0; ii < nof_slots_per_client; ii++) {
                Cal_Slots_A[slot_index++] = client_id;
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_A)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 80; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_B)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots), error_msg);

        /* Use array cal_slots_unit1[] to store Rx slots */
        for (i = 0; i < 80; i++) {
            if (unit_iter == 0) {
                cal_slots_unit1[i] = Cal_Slots_A[i];
            }
        }

        /* Group ID in overhead */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Default VLAN action set */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit_cen), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_egr_default_vlan_action_set(unit_dis), "");

    /* Synce check */
    sal_sleep(3);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Funtion to test calendar switching with asymmetrical client speeds 
 * It should be called after function dnx_flexe_cal_with_asym_client_speeds() */
int dnx_flexe_calendar_switch_with_asym_client_speeds(
    int unit_cen,
    int unit_dis,
    bcm_gport_t flexe_grp,
    bcm_port_flexe_group_cal_t calendar_id,
    bcm_port_t client_id,
    int is_client_removal)
{
    int i, unit_iter, unit;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    bcm_port_flexe_group_cal_t cal_active;
    char error_msg[200]={0,};

    int cal_slots_init_tx[80], cal_slots_init_rx[80];
    int cal_slots_tx[80], cal_slots_rx[80];

    /* Update parameters for reduced lane speed */
    dnx_flexe_lane_speed_init(unit_cen, unit_dis);
    grp_speed = is_reduced_cdu_lane_speed ? 100000 : 400000;
    total_slots = grp_speed / 5000;


    /***************************************************************************
     * Datapath:
     *
     *                           Centralized #0                      Centralized #1
     *                     +----------------------------+      +----------------------------+
     *               +<--->|<--> BusB_0   <--> BusA_0   |<---->|<--> BusA_0   <--> BusB_0   |<--->+
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_1   <--> BusA_1   |<---->|<--> BusA_1   <--> BusB_1   |<--->|
     *               |     |                            |      |                            |     |
     *  eth_cen <--->+<--->|<-->  ...     <-->  ...     |<---->|<-->  ...     <-->  ...     |<--->+<---> eth_dis
     *               |     |                            |      |                            |     |
     *               |<--->|<--> BusB_N-1 <--> BusA_N-1 |<---->|<--> BusA_N-1 <--> BusB_N-1 |<--->|
     *               |     |                            |      |                            |     |
     *               +<--->|<--> BusB_N   <--> BusA_N   |<---->|<--> BusA_N   <--> BusB_N   |<--->+
     *                     +----------------------------+      +----------------------------+
     * (Centralized #0 -- eth_cen->BusB_X: L2 lookup; BusB_X->eth_cen: force_forward)
     * (Centralized #1 -- eth_dis->BusB_X: L2 lookup; BusB_X->eth_dis: force_forward)
     **************************************************************************/

    /* Do calendar switching on the 2 devices */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Get slot assignment on active calendar */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 80, cal_slots_tx, &i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 80, cal_slots_rx, &i), error_msg);

        /* Load initial calendar setting */
        for (i = 0; i < 80; i++) {
            cal_slots_init_tx[i] = unit_iter ? cal_slots_unit1[i] : cal_slots_unit0[i];
            cal_slots_init_rx[i] = unit_iter ? cal_slots_unit0[i] : cal_slots_unit1[i];
        }

        /* Add/remove the client into/from the inactive calendar */
        for (i = 0; i < 80; i++) {
            if (cal_slots_init_tx[i] == client_id) {
                /* Find out a slot for the client in initial slot array */
                cal_slots_tx[i] = is_client_removal ? 0 : client_id;
            }
            if (cal_slots_init_rx[i] == client_id) {
                /* Find out a slot for the client in initial slot array */
                cal_slots_rx[i] = is_client_removal ? 0 : client_id;
            }
        }

        /* Configure the inactive calendar(Tx) */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_tx[i];
            Cal_Slots_B[i] = cal_slots_tx[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, Tx)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id, total_slots), error_msg);

        /* Configure the inactive calendar(Rx) */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_rx[i];
            Cal_Slots_B[i] = cal_slots_rx[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, Rx)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, calendar_id, total_slots), error_msg);

        /* Set the the inactive calendar as CalInUse in overhead before the switching */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, calendar_id), error_msg);

        /* Do calendar switching by activing the inactive calendar */
        snprintf(error_msg, sizeof(error_msg), "fail to active calendar %d on unit %d", calendar_id, unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id), error_msg);
    }
    sal_sleep(3);

    /* Set backup calendar to be the same as active calendar
     * It is used to avoid packet loss during calendar switching
     */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Get slot assignment on active calendar */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 80, cal_slots_tx, &i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 80, cal_slots_rx, &i), error_msg);

        /* Do same setting on backup calendar */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_tx[i];
            Cal_Slots_B[i] = cal_slots_tx[i];
        }
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, !cal_active, total_slots),
            "(cen_rx, cal_backup)");

        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_rx[i];
            Cal_Slots_B[i] = cal_slots_rx[i];
        }
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, !cal_active, total_slots),
            "(dis_tx, cal_backup)");
    }

    return BCM_E_NONE;
}

int dnx_flexe_traffic_with_der_soft(
    int unit,
    bcm_port_t flexe_phy,
    bcm_port_t eth_1,
    bcm_port_t eth_2,
    int is_L1)
{
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int i;
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int client_speed = 10000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    int nof_slots_per_client = client_speed / 5000;
    bcm_gport_t flexe_grp;
    flexe_port_type_t client_b_type = is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;

    /* Client IDs */
    bcm_port_t client_a_1 = 30;
    bcm_port_t client_a_2 = 31;
    bcm_port_t client_b_1 = 32;
    bcm_port_t client_b_2 = 33;

    if (is_flexe_gen2 && is_L1) {
        /* FlexE_Gen2: ETH_L1.speed should equal to client_speed. Change client&group speed as 400G ETH_L1 is used for test */
        flexe_grp_bandwidth = 800000;
        client_speed = 400000;
        nof_slots = flexe_grp_bandwidth / 5000;
        nof_slots_per_client = client_speed / 5000;
        nof_pcs = 2; /* Use 2*400G_PHYs for test */
    }

    /***************************************************************************
     * Topology:
     *
     *                    Centralized Device
     *            +--------------------------------------+
     *            |                                      |
     *  eth_1<--->| client_b_1 ---> client_a_1(loopback) |
     *            |  <---\                        /      |
     *            |       \  /--<---<----<----<--/       |
     *            |        \/                            |
     *            |        /\                            |
     *            |       /  \--<---<----<----<--\       |
     *            |  <---/                        \      |
     *  eth_2<--->| client_b_2 ---> client_a_2(loopback) |
     * (loopback) |                                      |
     *            +--------------------------------------+
     *
     * Datapath:
     * eth_1 -> client_b_1 -> client_a_1 -> client_b_2 -> eth_2 ->
     * -> client_b_2 -> client_a_2 -> client_b_1 -> eth_1
     **************************************************************************/

    /***************************************************************************
     * Create FlexE group and clients
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    flexe_phy_ports[1] = nof_pcs == 2 ? flexe_phy + 1 : 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_1, port_type_busA, client_speed, 0),
        "fail to create client_a_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_2, port_type_busA, client_speed, 0),
        "fail to create client_a_2");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_1, client_b_type, client_speed, 0),
        "fail to create client_b_1");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_2, client_b_type, client_speed, 0),
        "fail to create client_b_2");

    /***************************************************************************
     * Build datapath
     ***************************************************************************/

    /* Disable ports before setting FlexE flows */
    if (!is_flexe_gen2 || !is_L1) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_1, 0), "fail to disable flexe ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_2, 0), "fail to disable flexe ports");
    }
    if (is_L1) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_1, 0), "fail to disable flexe ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_2, 0), "fail to disable flexe ports");
    }

    /* Start to build the L1 or L2 datapath */
    if (is_L1) {
        if (!is_flexe_gen2) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_1, 0, client_b_1),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, client_a_1),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_1, 0, client_b_2),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, eth_2),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_2, 0, client_b_2),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_a_2),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_2, 0, client_b_1),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, eth_1),
                "fail to build FlexE L1 datapath");
        } else {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_1, 0, client_a_1),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_1, 0, eth_2),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_2, 0, client_a_2),
                "fail to build FlexE L1 datapath");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_2, 0, eth_1),
                "fail to build FlexE L1 datapath");
        }
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_1, client_b_1, 1),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, client_a_1),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_1, 0, client_b_2),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_2, eth_2, 1),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_2, client_b_2, 1),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_a_2),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_2, 0, client_b_1),
            "fail to build FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_1, eth_1, 1),
            "fail to build FlexE L2 datapath");
    }

    /* Enable ports */
    if (!is_flexe_gen2 || !is_L1) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_1, 1), "fail to enable flexe ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_2, 1), "fail to enable flexe ports");
    }
    if (is_L1) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_1, 1), "fail to enable flexe ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_2, 1), "fail to enable flexe ports");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Set PHY loopback on eth_2 and flexe_phy */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_2, 2), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, 2), "");
    if (nof_pcs == 2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy+1, 2), "");
    }
    bshell(unit,"sleep 5");

    /* Calendar setting */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots_per_client*2; i++) {
        Cal_Slots_A[i] = i < nof_slots_per_client ? client_a_1 : client_a_2;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_slots), "");

    /* FlexE OH setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

/* Function to test MIB counters on FlexE ports(BusB_L1, BusB_L2 and ETH_L1) */
int dnx_flexe_mib_eth_and_busb_common(
    int unit,
    bcm_port_t eth_L2,
    bcm_port_t eth_L1,
    int port_speed)
{
    uint32 flags = 0;
    bcm_port_t client_a = 80;
    bcm_port_t client_b_L1 = 81;
    bcm_port_t client_b_L2 = 82;
    bcm_port_t flexe_phy = 20;
    int flexe_phy_speed = 50000;


    if (dnx_flexe_gen2_support(unit, -1)) {
        /***************************************************************************
         * FlexE Gen2 Datapath:
         * CPU.TxRx <---> eth_L2 <--force_fwd--> client_b_L2 <---> client_a <---> flexe_phy (loopback)
         ***************************************************************************/

        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_basic_cfg_main(unit, flexe_phy_speed, 0, port_speed, eth_L2, flexe_phy, 1, 0), "");
    } else {
        /***************************************************************************
         * FlexE client(s) creation
         ***************************************************************************/
        
        /* Create a busA client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, port_speed, 0),
            "fail to create busA client");
        
        /* Create BusB L1 client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L1, port_type_busB_L1, port_speed, 0),
            "fail to create client_b_L1");
        
        /* Create BusB L2 client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, port_speed, 0),
            "fail to create client_b_L2");

        /***************************************************************************
         * FlexE Gen1 Datapath:
         * CPU.TxRx <---> eth_L2 <--force_fwd--> client_b_L2 <--L1_switch--> client_b_L1 <--L1_switch--> eth_L1(loopback)
         ***************************************************************************/

        /* Disable ports */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_L1, 0), "fail to disable FlexE ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L1, 0), "fail to disable FlexE ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 0), "fail to disable FlexE ports");

        /* Set FlexE flows: client_b_L1 <--> eth_L1 */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, eth_L1),
            "fail to set FlexE flow between client_b_L1 and eth_L1");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, eth_L1, 0, client_b_L1),
            "fail to set FlexE flow between client_b_L1 and eth_L1");

        /* Set FlexE flows: client_b_L1 <--> client_b_L2 */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, client_b_L2),
            "fail to set FlexE flow between client_b_L1 and client_b_L2");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_b_L1),
            "fail to set FlexE flow between client_b_L1 and client_b_L2");

        /* Enable ports */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_L1, 1), "fail to enable FlexE ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L1, 1), "fail to enable FlexE ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 1), "fail to enable FlexE ports");

        /* Set L2 datapath: eth_L2 <--force_fwd--> client_b_L2 */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_L2, client_b_L2, 1), "fail in force_fwd setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_L2, eth_L2, 1), "fail in force_fwd setting");

        /* Set PHY loopback on eth_L1 */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_L1, 2), "");
    }

    return BCM_E_NONE;
}

/* Function to test MIB counters on ILKN_L1 port */
int dnx_flexe_mib_ilkn_L1_common(
    int unit,
    bcm_port_t eth_L2,
    bcm_port_t ilkn_L1,
    bcm_port_t flexe_phy,
    int client_speed)
{
    int i;
    uint32 flags = 0;
    bcm_port_t client_b_L2 = 80;
    bcm_port_t client_c = 81;
    bcm_port_t client_a = 82;
    bcm_gport_t flexe_grp;
    int nof_slots = client_speed / 5000;


    /***************************************************************************
     * FlexE group and client(s) creation
     ***************************************************************************/

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, 1), "");

    /* Create BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0),
        "fail to create busA client");

    /* Create BusB L2 client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, client_speed, 0),
        "fail to create client_b_L2");

    /* Create BusC client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed, 0),
        "fail to create busC client");

    /***************************************************************************
     * Datapath:
     * CPU.TxRx ---> eth_L2 --force_fwd--> client_b_L2 --L1_switch--> client_a -->(loopback)
     * ---> client_c --L1_switch--> ilkn_L1(loopback)
     ***************************************************************************/

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 0), "fail to disable FlexE ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ilkn_L1, 0), "fail to disable FlexE ports");

    /* Set FlexE flows: client_b_L2 --> client_a --> client_c --> ilkn_L1 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_a),
        "fail to set FlexE L1 datapath");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_a, 0, client_c),
        "fail to set FlexE L1 datapath");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_c, 0, ilkn_L1),
        "fail to set FlexE L1 datapath");

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_L2, 1), "fail to enable FlexE ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ilkn_L1, 1), "fail to enable FlexE ports");

    /* Set L2 datapath: eth_L2 --force_fwd--> client_b_L2 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_L2, client_b_L2, 1), "fail in force_fwd setting");

    /* Set PHY loopback on ilkn_L1 and FlexE PHY */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, ilkn_L1, 2), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, 2), "");

    /***************************************************************************
     * Calendar and OH setting
     ***************************************************************************/

    /* Assign slots for the FlexE client */
    for (i = 0; i < nof_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_slots), "");

    /* FlexE OH setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

int dnx_flexe_half_capability_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t phy_0,
    bcm_port_t phy_1,
    bcm_port_t eth_cen,
    bcm_port_t eth_dis,
    int group_speed,
    int client_speed)
{
    int i, ii, unit;
    int nof_phys = 2;
    int units[2] = {unit_cen, unit_dis};
    bcm_port_t eth_port, eth_ports[2] = {eth_cen, eth_dis};
    int total_slots = group_speed / 5000;
    int client_slots = client_speed / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 30;
    bcm_port_t client_b = 41;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_half_capability_main";

    /***************************************************************************
     * The datapath is:
     *
     *                    Centralized Device                     Distributed Device
     *                +------------------------+             +------------------------+
     * eth_cen <-L1-> | client_b <--> client_a | <- FlexE -> | client_a <--> client_b | <-L1-> eth_dis
     *                +------------------------+             +------------------------+
     *
     ***************************************************************************/

    /***************************************************************************
     * FlexE Setting on the 2 devices
     ***************************************************************************/

    for (i = 0; i < 2; i++) {
        unit = units[i];
        eth_port = eth_ports[i];
        skip_synce_in_grp_creation = 1; /* Skip synce setting */

        /* Create FlexE group with provided FlexE PHYs */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        flexe_phy_ports[0] = phy_0;
        flexe_phy_ports[1] = phy_1;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), error_msg);

        /* Create FlexE clients */
        snprintf(error_msg, sizeof(error_msg), "fail to create client_a(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create client_b(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB_L1, client_speed, 0), error_msg);

        /* Create L1 datapath: eth_port <- L1_switch -> client_b <--> client_a */
        snprintf(error_msg, sizeof(error_msg), "fail to set Eth2FlexE L1 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set FlexE2Eth L1 datapath(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0), error_msg);

        /* Assign slots on the 2nd PHY */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = total_slots/2; ii < (total_slots/2)+client_slots; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

        /* FlexE OH setting */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /***************************************************************************
     * Other Settings
     ***************************************************************************/

    /* Set TX channel map in FlexE core */
    if (group_speed == 100000) {
        bshell(unit_cen, "flexe core write register addr=0x18000 count=1 val=0x6FFFFFFF");
        bshell(unit_dis, "flexe core write register addr=0x18000 count=1 val=0x6FFFFFFF");
    } else if (group_speed == 400000) {
        bshell(unit_cen, "flexe core write register addr=0x18000 count=1 val=0xFFF0FFFF");
        bshell(unit_dis, "flexe core write register addr=0x18000 count=1 val=0xFFF0FFFF");
    } else {
        /* 200G group */
        bshell(unit_cen, "flexe core write register addr=0x18000 count=1 val=0xF4FFFFFF");
        bshell(unit_dis, "flexe core write register addr=0x18000 count=1 val=0xF4FFFFFF");
    }

    /* Set loopback on the redundant port(port 24) on both Q2As */
    if (phy_1 != 24) {
        printf("**** %s: Error, logical port ID of phy_1 isn't 24 ****\n", proc_name);
        return BCM_E_FAIL;
    }
    bshell(unit_cen, "phy set 24 reg=TLB_RX_prbs_chk_config fld=prbs_chk_en_auto_mode dt=0");
    bshell(unit_cen, "phy set 24 reg=TLB_RX_dig_lpbk_config fld=dig_lpbk_en dt=1");
    bshell(unit_dis, "phy set 24 reg=TLB_RX_prbs_chk_config fld=prbs_chk_en_auto_mode dt=0");
    bshell(unit_dis, "phy set 24 reg=TLB_RX_dig_lpbk_config fld=dig_lpbk_en dt=1");

    /* Check link status of PHY ports */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "FlexE PHY port linkdown on unit %d", units[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_phy_port_link_check(units[i], phy_0), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to create 7 L1 snakes with different speeds 
 * Used to test the scenario of multiple L1 clients with huge client speed differences
 */
int dnx_flexe_mixed_L1_clients_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy)
{
    int i, ii, iii, unit;
    int units[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    int slot_index = 0;
    int nof_slots_per_client;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};

    /* BusB clients */
    bcm_port_t BusB_200g = 30;
    bcm_port_t BusB_100g = 31;
    bcm_port_t BusB_50g  = 32;
    bcm_port_t BusB_25g  = 33;
    bcm_port_t BusB_10g  = 34;
    bcm_port_t BusB_5g_0 = 35;
    bcm_port_t BusB_5g_1 = 36;
    bcm_port_t BusB_5g_2 = 37;
    bcm_port_t BusB_clients[8] = {BusB_200g, BusB_100g, BusB_50g, BusB_25g, BusB_10g, BusB_5g_0, BusB_5g_1, BusB_5g_2};

    /* BusA clients */
    bcm_port_t BusA_200g = 40;
    bcm_port_t BusA_100g = 41;
    bcm_port_t BusA_50g  = 42;
    bcm_port_t BusA_25g  = 43;
    bcm_port_t BusA_10g  = 44;
    bcm_port_t BusA_5g_0 = 45;
    bcm_port_t BusA_5g_1 = 46;
    bcm_port_t BusA_5g_2 = 47;
    bcm_port_t BusA_clients[8] = {BusA_200g, BusA_100g, BusA_50g, BusA_25g, BusA_10g, BusA_5g_0, BusA_5g_1, BusA_5g_2};
    int client_speeds[8] = {200000, 100000, 50000, 25000, 10000, 5000, 5000, 5000};

    /* ETH_L1 ports */
    bcm_port_t eth_1g     = 13;
    bcm_port_t eth_10g    = 14;
    bcm_port_t eth_25g    = 15;
    bcm_port_t eth_50g    = 50;
    bcm_port_t eth_100g   = 51;
    bcm_port_t eth_100g_1 = 52;
    bcm_port_t eth_400g   = 1;


    /***************************************************************************
     * DataPath:
     *
     *                                      Device 0                             Device 1
     *                              +--------------------------+       +--------------------------+
     * IXIA_400g <--> eth_400g  <-->| BusB_200g <--> BusA_200g |<----->| BusA_200g <--> BusB_200g |<--> eth_400g(loopback)        <--[200g snake]
     *                              |                          |       |                          |
     * IXIA_100g <--> eth_100g  <-->| BusB_100g <--> BusA_100g |<----->| BusA_100g <--> BusB_100g |<--> eth_100g(loopback)        <--[100g snake]
     *                              |                          |       |                          |
     *                eth_50g   <-->| BusB_50g  <--> BusA_50g  |<----->| BusA_50g  <--> BusB_50g  |<--> eth_50g(loopback)         <--[50g snake]
     *                              |                          |       |                          |
     *                eth_25g   <-->| BusB_25g  <--> BusA_25g  |<----->| BusA_25g  <--> BusB_25g  |<--> eth_25g(loopback)         <--[25g snake]
     *                              |                          |       |                          |
     *      (loopback)eth_100g_1<-->| BusB_10g  <--> BusA_10g  |<----->| BusA_10g  <--> BusB_10g  |<--> eth_100g_1<-->IXIA_100g   <--[10g snake]
     *                              |                          |       |                          |
     *                eth_10g------>| BusB_5g_0 ---> BusA_5g_0 |------>| BusA_5g_0 ---> BusB_5g_0 |---> eth_10g-->+
     *                   ^          |                          |       |                          |    (loopback) |               <--[5g snake]
     *                   |          |                          |       |                          |               |
     *                   +<---------| BusB_5g_1 <--- BusA_5g_1 |<------| BusA_5g_1 <--- BusB_5g_1 |<--------------+
     *                              |                          |       |                          |
     *                eth_1g    <-->| BusB_5g_2 <--> BusA_5g_2 |<----->| BusA_5g_2 <--> BusB_5g_2 |<--> eth_1g(loopback)          <--[ETH_1G to Client_5G snake]
     *                              |                          |       |                          |
     *                              +--------------------------+       +--------------------------+
     *
     * Total client speeds:
     * 200G+100G+50G+25G+10G+5G*3=400G
     *
     * Mismatched ETH&client rates in below snakes:
     * 1. 200g snake: eth_speed=400g, client_speed=200g. No 200g IXIA connection. Use 400g IXIA port to Tx/Rx traffic
     * 2. 10g  snake: eth_speed=100g, client_speed=10g.  No 10g IXIA connection. Use 100g IXIA port to Tx/Rx traffic
     * 3. 5g   snake: eth_speed=10g,  client_speed=5g.   No 5G ETH port. Use 10G ETH instead
     * 4. ETH_1G to Client_5G snake: eth_speed=1g, client_speed=5g
     **************************************************************************/

    /***************************************************************************
     * Create FlexE groups and clients
     ***************************************************************************/

    for (i = 0; i < 2; i++) {
        unit = units[i];

        /* Create FlexE group with the FlexE PHY */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        flexe_phy_ports[0] = flexe_phy;
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

        for (ii = 0; ii < 8; ii++) {
            /* Create BusA clients */
            snprintf(error_msg, sizeof(error_msg), "fail to create BusA client(unit %d, index %d)", unit, ii);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, BusA_clients[ii], port_type_busA, client_speeds[ii], 0), error_msg);

            /* Create BusB clients */
            snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(unit %d, index %d)", unit, ii);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, BusB_clients[ii], port_type_busB_L1, client_speeds[ii], 0), error_msg);
        }
    }

    /***************************************************************************
     * DataPath creation
     ***************************************************************************/

    /* Build 200G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_400g  <-->| BusB_200g <--> BusA_200g |<----->| BusA_200g <--> BusB_200g |<--> eth_400g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        snprintf(error_msg, sizeof(error_msg), "fail to set 200g snake(Eth2Flexe)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_400g, BusB_200g, BusA_200g, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set 200g snake(Flexe2Eth)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_200g, BusB_200g, eth_400g, 0), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_400g, 2), "(eth_400g)");

    /* Build 100G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_100g  <-->| BusB_100g <--> BusA_100g |<----->| BusA_100g <--> BusB_100g |<--> eth_100g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        snprintf(error_msg, sizeof(error_msg), "fail to set 100g snake(Eth2Flexe)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_100g, BusB_100g, BusA_100g, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set 100g snake(Flexe2Eth)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_100g, BusB_100g, eth_100g, 0), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_100g, 2), "(eth_100g)");

    /* Build 50G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_50g   <-->| BusB_50g  <--> BusA_50g  |<----->| BusA_50g  <--> BusB_50g  |<--> eth_50g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        snprintf(error_msg, sizeof(error_msg), "fail to set 50g snake(Eth2Flexe)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_50g, BusB_50g, BusA_50g, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set 50g snake(Flexe2Eth)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_50g, BusB_50g, eth_50g, 0), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_50g, 2), "(eth_50g)");

    /* Build 25G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_25g   <-->| BusB_25g  <--> BusA_25g  |<----->| BusA_25g  <--> BusB_25g  |<--> eth_25g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        snprintf(error_msg, sizeof(error_msg), "fail to set 25g snake(Eth2Flexe)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_25g, BusB_25g, BusA_25g, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set 25g snake(Flexe2Eth)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_25g, BusB_25g, eth_25g, 0), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_25g, 2), "(eth_25g)");

    /* Build 10G snake as below:
     *                                   Device 0                            Device 1
     *                          +--------------------------+       +--------------------------+
     *  (loopback)eth_100g_1<-->| BusB_10g  <--> BusA_10g  |<----->| BusA_10g  <--> BusB_10g  |<--> eth_100g_1<-->IXIA_100g
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        snprintf(error_msg, sizeof(error_msg), "fail to set 10g snake(Eth2Flexe)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_100g_1, BusB_10g, BusA_10g, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set 10g snake(Flexe2Eth)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_10g, BusB_10g, eth_100g_1, 0), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_cen, eth_100g_1, 2), "(eth_100g_1)");

    /* Build ETH_1G to Client_5G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_1g    <-->| BusB_5g_2 <--> BusA_5g_2 |<----->| BusA_5g_2 <--> BusB_5g_2 |<--> eth_1g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        snprintf(error_msg, sizeof(error_msg), "fail to set 1g snake(Eth2Flexe)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_1g, BusB_5g_2, BusA_5g_2, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to set 1g snake(Flexe2Eth)(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_5g_2, BusB_5g_2, eth_1g, 0), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_1g, 2), "(eth_1g)");

    /* Build 5G snake as below:
     *                        Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_10g------>| BusB_5g_0 ---> BusA_5g_0 |------>| BusA_5g_0 ---> BusB_5g_0 |---> eth_10g-->+
     *     ^          |                          |       |                          |    (loopback) |
     *     |          |                          |       |                          |               |
     *     +<---------| BusB_5g_1 <--- BusA_5g_1 |<------| BusA_5g_1 <--- BusB_5g_1 |<--------------+
     *                +--------------------------+       +--------------------------+
     */
    /* Device 0 */
    snprintf(error_msg, sizeof(error_msg), "fail to set 5g snake on unit %d", unit_cen);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_10g, BusB_5g_0, BusA_5g_0, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, BusA_5g_1, BusB_5g_1, eth_10g, 0), error_msg);
    /* Device 1 */
    snprintf(error_msg, sizeof(error_msg), "fail to set 5g snake on unit %d", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_10g, BusB_5g_1, BusA_5g_1, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, BusA_5g_0, BusB_5g_0, eth_10g, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, eth_10g, 2), "(eth_10g)");

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Do below settings on the 2 devices */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        slot_index = 0;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");

        /* Iterate each client and assign slots for it according to its speed */
        for (ii = 0; ii < 8; ii++) {
            /* Compute the number of slots for current client */
            nof_slots_per_client = client_speeds[ii] / 5000;

            /* Assign slots for the client */
            for (iii = 0; iii < nof_slots_per_client; iii++) {
                Cal_Slots_A[slot_index++] = BusA_clients[ii];
            }
        }

        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);
    }

    /* Synce check */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp), "Synce check failed");

    return BCM_E_NONE;
}

/* Common FlexE datapath setting for FlexE PRD testing */
int rx_force_fwd = 0; /* If set, function dnx_flexe_prd_datapath_main will use force_fwd instead of L2_lookup in Rx datapath */
int dnx_flexe_prd_datapath_main(
    int unit,
    bcm_port_t flexe_phy,
    bcm_port_t client_prd,
    int client_speed,
    bcm_port_t eth_tx_lo,
    bcm_port_t eth_tx_hi,
    bcm_port_t eth_rx_lo,
    bcm_port_t eth_rx_hi)
{
    int i;
    int nof_phys = 1;
    int total_slots;
    int vlan_lo = 1000;
    int vlan_hi = 1001;
    uint32 flags = 0;
    bcm_gport_t flexe_grp;
    bcm_port_t client_b_tx = 61;
    bcm_port_t client_a = 62;
    bcm_mac_t mac_dst = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_port_resource_t resource;


    /* Get PHY speed to compute total slots */
	BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, flexe_phy, &resource), "");
	total_slots = resource.speed / 5000;

    /***************************************************************************
     * Datapat:
     *
     * eth_tx_lo ---->+   +----------------------------------------+
     *    (force_fwd) |-->| client_b_tx ----->\                    |
     * eth_tx_hi ---->+   |                    \                   |
     *                    |                     client_a(loopback) |
     * eth_rx_lo <----+   |                    /                   |
     *    (L2_lookup) |<--| client_prd  <-----/                    |
     * eth_rx_hi <----+   +----------------------------------------+
     *
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), "");

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_tx, port_type_busB_L2_L3, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_prd, port_type_busB_L2_L3, client_speed, 0), "");

    /* Disable BusB clients before building FlexE datapath */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_tx, 0), "(0)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 0), "(0)");

    /* Building Tx datapath: eth_tx_lo/eth_tx_hi --force_fwd--> client_b_tx --> client_a */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx_lo, client_b_tx, 1),
        "fail in bulding Tx datapath");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx_hi, client_b_tx, 1),
        "fail in bulding Tx datapath");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_tx, 0, client_a),
        "fail in bulding Tx datapath");

    /* Building Rx datapath: client_a --> client_prd --L2_lookup--> eth_rx_lo/eth_rx_hi */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_a, 0, client_prd),
        "fail in bulding Rx datapath(BusA2BusB)");
    if (!rx_force_fwd) {
        /* L2 entry: mac_dst + vlan_lo --> eth_rx_lo */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_advanced_set(unit, client_prd, eth_rx_lo, mac_dst, 1, vlan_lo, 1),
            "fail in L2 setting for eth_rx_lo");
        /* L2 entry: mac_dst + vlan_hi --> eth_rx_hi */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_advanced_set(unit, client_prd, eth_rx_hi, mac_dst, 1, vlan_hi, 1),
            "fail in L2 setting for eth_rx_hi");
    } else {
        print "~~~~~~~~~ using force_fwd in Rx datapath ~~~~~~~~~~~~~~";
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_prd, eth_rx_lo, 1),
            "fail in bulding Rx datapath(force_fwd)");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_prd, eth_rx_hi, 1),
            "fail in bulding Rx datapath(force_fwd)");
    }

    /* Enable BusB clients */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_tx, 1), "(1)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 1), "(1)");

    /* Loopback FlexE PHY */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY), "");

    /* Assign slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_speed/5000; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* FlexE OH setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

int dnx_flexe_prd_distributed_mode_main(
    int unit,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    bcm_port_t ilkn_port_hi,
    bcm_port_t ilkn_port_lo)
{
    int i;
    int client_speed = 100000;
    int client_speed_prd = 90000;
    bcm_port_t client_b = 51;
    bcm_port_t client_prd = 52;


    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create client_b */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB_L2_L3, client_speed, 0),
        "fail to create client_b");

    /* Create client_prd */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_prd, port_type_busB_L2_L3, client_speed_prd, 0),
        "fail to create client_prd");

    /* Set 2 scheduler priorities on client_prd */
    bcm_port_prio_config_t priority_config;
    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1;
    priority_config.priority_groups[0].num_of_entries = 0;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
    priority_config.priority_groups[1].num_of_entries = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_set(unit, client_prd, &priority_config), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 1), "");

    /* Get GPORTs for high&low priorities traffic on client_prd */
    bcm_gport_t gport_pri_hi;
    bcm_gport_t gport_pri_lo;
    BCM_GPORT_NIF_RX_PRIORITY_LOW_SET(gport_pri_hi, client_prd);
    BCM_GPORT_NIF_RX_PRIORITY_HIGH_SET(gport_pri_lo, client_prd);

    /***************************************************************************
     * Datapath creation
     ***************************************************************************/

    /* 
     *                                         Distributed Device
     *                            +---------------------------------------------------------------------+
     *                            |                         +---gport_pri_hi---ilkn_port_hi---->+       |
     *  eth_port_tx --force_fwd-->| client_b-->client_prd-->|                                   |-->+   |
     *                            |                         +---gport_pri_lo---ilkn_port_lo---->+   |   |
     *                            |                                                                 |   |
     *  eth_port_rx <-force_fwd---| <-------------force_fwd--------------------<--------------------+   |
     *                            +---------------------------------------------------------------------+
     */

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ilkn_port_hi, 0), "fail to disable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ilkn_port_lo, 0), "fail to disable ports");

    /* Start to build FlexE datapath as above topology shown */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_tx, client_b, 1), "fail in FlexE datapath setting");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b, 0, client_prd), "fail in FlexE datapath setting");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, gport_pri_hi, 0, ilkn_port_hi), "fail in FlexE datapath setting");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, gport_pri_lo, 0, ilkn_port_lo), "fail in FlexE datapath setting");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, ilkn_port_hi, eth_port_rx, 1), "fail in FlexE datapath setting");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, ilkn_port_lo, eth_port_rx, 1), "fail in FlexE datapath setting");

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ilkn_port_hi, 1), "fail to enable ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ilkn_port_lo, 1), "fail to enable ports");

    /* Loopback ILKN ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, ilkn_port_hi, 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, ilkn_port_lo, 1), "");

    return BCM_E_NONE;
}

int dnx_flexe_prd_itmh_main(
    int unit,
    bcm_port_t flexe_phy,
    bcm_port_t client_prd,
    int client_speed,
    bcm_port_t eth_tx,
    bcm_port_t eth_rx)
{
    int i;
    int nof_phys = 1;
    int total_slots;
    uint32 flags = 0;
    bcm_gport_t flexe_grp;
    bcm_port_t client_b_tx = 61;
    bcm_port_t client_a = 62;
    bcm_port_resource_t resource;


    /* Get PHY speed to compute total slots */
	BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, flexe_phy, &resource), "");
	total_slots = resource.speed / 5000;

    /***************************************************************************
     * Datapat:
     *                       +----------------------------------------+
     * eth_tx--(L1_switch)-->| client_b_tx ----->\                    |
     *                       |                    \                   |
     *                       |                     client_a(loopback) |
     *                       |                    /                   |
     * eth_rx<--(itmh_dst)-- | client_prd  <-----/                    |
     *                       +----------------------------------------+
     *
     * (eth_tx with inject ETH1oITMH packets to client_prd, and the packets will
     *  go to eth_tx according to ITMH.dst_port)
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_phys), "");

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_tx, port_type_busB_L1, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_prd, port_type_busB_L2_L3, client_speed, 0), "");

    /* Disable BusB clients before building FlexE datapath */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_tx, 0), "(0)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_tx, 0), "(0)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 0), "(0)");

    /* Building L1 Tx datapath: eth_tx --> client_b_tx --> client_a */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, eth_tx, 0, client_b_tx), "fail in bulding Tx datapath");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_b_tx, 0, client_a), "fail in bulding Tx datapath");

    /* Building Rx datapath: client_a --> client_prd */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, flags, client_a, 0, client_prd),
        "fail in bulding Rx datapath(BusA2BusB)");

    /* Enable BusB clients */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_tx, 1), "(1)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_b_tx, 1), "(1)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_prd, 1), "(1)");

    /* Loopback FlexE PHY */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY), "");

    /* Assign slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_speed/5000; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* FlexE OH setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

/* PRD control frame setting on FlexE client */
int dnx_flexe_prd_control_frame_set(
    int unit,
    bcm_port_t flexe_port,
    int control_frame_index,
    uint32 dmac_first_2bytes,
    uint32 dmac_last_4bytes,
    uint32 mask_first_2bytes,
    uint32 mask_last_4bytes,
    uint32 ether_type_code,
    uint32 ether_type_val
)
{
    uint32 flags = 0;
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_config;

    /* Set Ethernet type for ether_type_code recognition */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_custom_ether_type_set(unit, flexe_port, flags, ether_type_code, ether_type_val), "");

    /* Set control frame on the ether_type_code */
    COMPILER_64_SET(control_frame_config.mac_da_val, dmac_first_2bytes, dmac_last_4bytes);
    COMPILER_64_SET(control_frame_config.mac_da_mask,mask_first_2bytes, mask_last_4bytes);
    control_frame_config.ether_type_code= ether_type_code;
    control_frame_config.ether_type_code_mask= 0xF;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_control_frame_set(unit, flexe_port, flags, control_frame_index, &control_frame_config), "");

    return BCM_E_NONE;
}

/* FlexE PRD semantic test for FlexE_MAC_L2 */
int dnx_flexe_prd_sem_L2_main(
    int unit,
    bcm_port_t flexe_port_0,
    bcm_port_t flexe_port_1)
{
    int flags = 0;
    int i, ii, rv, port_index, header_type, val_get, val_get_1, val_get_2, val_get_3;
    int max_port_profile = 7;
    int priority_to_map, priority_get;
    int tpid_list[4]     = {bcmCosqControlIngressPortDropTpid1,
                            bcmCosqControlIngressPortDropTpid2,
                            bcmCosqControlIngressPortDropTpid3,
                            bcmCosqControlIngressPortDropTpid4};
    int priority_list[5] = {BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_0,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_1,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_2,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_3,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM};
    int port_pri_all = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1 | BCM_PORT_F_PRIORITY_2 | BCM_PORT_F_PRIORITY_3 | BCM_PORT_F_PRIORITY_TDM;
    int port_pri_list[6] = {BCM_PORT_F_PRIORITY_0, BCM_PORT_F_PRIORITY_1, BCM_PORT_F_PRIORITY_2, BCM_PORT_F_PRIORITY_3, BCM_PORT_F_PRIORITY_TDM, port_pri_all};
    int sch_pri_list[3] = {bcmPortNifSchedulerLow, bcmPortNifSchedulerHigh, bcmPortNifSchedulerTDM};
    int max_threshold_port_0, max_threshold_port_1;
    uint32 key, val32_get_0, val32_get_1;
    uint64 val_64b_get;
    bcm_port_t flexe_ports[2] = {flexe_port_0, flexe_port_1};
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_port_prio_config_t priority_config;
    bcm_port_prio_config_t priority_config_get;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_prd_sem_L2_main";
    print "~~~~~~~~~~~~~~~~~~~~ Starting semantic test for FlexE_MAC_L2 ~~~~~~~~~~~~~~~~~~~~";

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create BusB clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, flexe_port_0, port_type_busB_L2_L3, 5000, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, flexe_port_1, port_type_busB_L2_L3, 395000, 0), "");

    /* Set maximal RMC threshold */
    max_threshold_port_0 = 1 * (*(dnxc_data_get(unit, "nif", "prd", "rmc_fifo_2_threshold_resolution", NULL)));  /* 5000/5000 */
    max_threshold_port_1 = 79 * (*(dnxc_data_get(unit, "nif", "prd", "rmc_fifo_2_threshold_resolution", NULL))); /* 395000/5000 */

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_port_0, 0), "(disable)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_port_1, 0), "(disable)");

    /***************************************************************************
     * PRD Profile setting on FlexE ports
     ***************************************************************************/

    for (port_index = 0; port_index < 2; port_index++) {
        /* Iterate each PRD port profile */
        for (i = 0; i <= max_port_profile+1; i++) {
            rv = bcm_cosq_control_set(unit, flexe_ports[port_index], -1, bcmCosqControlIngressPortDropPortProfileMap, i);
            if (i <= max_port_profile) {
                snprintf(error_msg, sizeof(error_msg), "fail to set PRD PortProfileMap on port %d", flexe_ports[port_index]);
                BCM_IF_ERROR_RETURN_MSG(rv, error_msg);
            } else if (rv == BCM_E_NONE) {
                printf("**** %s: no error for invalid PRD port profile(port %d) ****\n", proc_name, flexe_ports[port_index]);
                return BCM_E_FAIL;
            }

            snprintf(error_msg, sizeof(error_msg), "fail to get PRD PortProfileMap on port %d", flexe_ports[port_index]);
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get(unit, flexe_ports[port_index], -1, bcmCosqControlIngressPortDropPortProfileMap, &val_get), error_msg);
            if (val_get != i && i <= max_port_profile)  {
                printf("**** %s: Error, returned PRD port profile %d doesn't equal to %d ****\n", proc_name, val_get, i);
                return BCM_E_FAIL;
            }
        }
    }

    /***************************************************************************
     * Hard stage mapping
     ***************************************************************************/

    /* Use invalid priority */
    key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(1, 0);
    cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    priority_to_map = 6;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port_0, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_map_set(port %d) ****\n", proc_name, flexe_ports[port_index]);
        return BCM_E_FAIL;
    }
    priority_to_map = 1;

    /* Use invalid key */
    key = 0x123;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port_0, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_map_set(port %d) ****\n", proc_name, flexe_ports[port_index]);
        return BCM_E_FAIL;
    }
    key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(1, 0);

    /* Use invalid cosq_map */
    priority_to_map = 1;
    cosq_map = 10;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port_0, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_map_set(port %d) ****\n", proc_name, flexe_ports[port_index]);
        return BCM_E_FAIL;
    }

    /* Set&get hard stage mapping on each FlexE port */
    for (port_index = 0; port_index < 2; port_index++) {
        for (header_type = 0; header_type < 5; header_type++) {
            /* Compute mapping key and cosq_map according to header_type */
            if (header_type == 0) {
                key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(header_type,0);
                cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
            } else if (header_type == 1 || header_type == 2) {
                key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(header_type,0);
                cosq_map = bcmCosqIngressPortDropTmTcDpPriorityTable;
            } else if (header_type == 3) {
                key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(header_type);
                cosq_map = bcmCosqIngressPortDropIpDscpToPriorityTable;
            } else {
                key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(header_type);
                cosq_map = bcmCosqIngressPortDropMplsExpToPriorityTable;
            }
            priority_to_map = priority_list[header_type];

            /* Set hard stage mapping */
            snprintf(error_msg, sizeof(error_msg), "(port_index %d, type %d)", port_index, header_type);
            rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_ports[port_index], flags, cosq_map, key, priority_to_map);
            if (priority_to_map == BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM) {
                /* L2 client doesn't support TDM pirority */
                if (rv == BCM_E_NONE) {
                    printf("**** %s: no error reported for TDM priority(port_index %d, type %d) ****\n", proc_name, port_index, header_type);
                    return BCM_E_FAIL;
                }

                /* Configure hard stage mapping again with supported priority */
                priority_to_map = BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_1;
                BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_map_set(unit, flexe_ports[port_index], flags, cosq_map, key, priority_to_map), error_msg);
            } else {
                BCM_IF_ERROR_RETURN_MSG(rv, error_msg);
            }

            /* Get hard stage mapping */
            BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_map_get(unit, flexe_ports[port_index], flags, cosq_map, key, &priority_get), error_msg);
            if (priority_get != priority_to_map) {
                printf("**** %s: Error, returned priority %d doesn't equal to %d(port_index %d, type %d) ****\n", proc_name, priority_get, priority_to_map, port_index, header_type);
                return BCM_E_FAIL;
            }
        }
    }

    /***************************************************************************
     * TPID/RMC_threshold/PRD_enable
     ***************************************************************************/

    /* Set TPID on FlexE port */
    for (i = 0; i < 4; i++) {
        /* Iterate each TPID */
        snprintf(error_msg, sizeof(error_msg), "fail to set TPID %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, flexe_port_0, -1, tpid_list[i], 0x9200+i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get(unit, flexe_port_0, -1, tpid_list[i], val_get), error_msg);
        if (val_get != 0x9200+i) {
            printf("**** %s: Error, TPID %d: returned value %d isn't %d ****\n", proc_name, i, val_get, 0x9200+i);
            return BCM_E_FAIL;
        }
    }

    /* Set RMC threshold(TDM priority conflicts with FlexE) */
    for (i = 0; i < 4; i++) {
        /* Set minimal threshold */
        snprintf(error_msg, sizeof(error_msg), "(min, pri %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_0, flags, priority_list[i], 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_0, flags, priority_list[i], &val32_get_0), error_msg);
        if (val32_get_0 != 0) {
            printf("**** %s: Error, threshold %d: returned value %d isn't 0 ****\n", proc_name, i, val32_get_0);
            return BCM_E_FAIL;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_1, flags, priority_list[i], 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_1, flags, priority_list[i], &val32_get_0), error_msg);
        if (val32_get_0 != 0) {
            printf("**** %s: Error, threshold %d: returned value %d isn't 0 ****\n", proc_name, i, val32_get_0);
            return BCM_E_FAIL;
        }

        /* Set maximal threshold */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_0, flags, priority_list[i], max_threshold_port_0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_0, flags, priority_list[i], &val32_get_0), error_msg);
        if (val32_get_0 != max_threshold_port_0) {
            printf("**** %s: Error, threshold %d: returned value %d isn't %d ****\n", proc_name, i, val32_get_0, max_threshold_port_0);
            return BCM_E_FAIL;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_1, flags, priority_list[i], max_threshold_port_1), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_1, flags, priority_list[i], &val32_get_0), error_msg);
        if (val32_get_0 != max_threshold_port_1) {
            printf("**** %s: Error, threshold %d: returned value %d isn't %d ****\n", proc_name, i, val32_get_0, max_threshold_port_1);
            return BCM_E_FAIL;
        }

        /* Set with outrange threshold */
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_0, flags, priority_list[i], max_threshold_port_0+1);
        if (rv == BCM_E_NONE) {
            printf("**** %s: no error for outrange threshold %d(pri %d) ****\n", proc_name, max_threshold_port_0+1, i);
            return BCM_E_FAIL;
        }
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_1, flags, priority_list[i], max_threshold_port_1+1);
        if (rv == BCM_E_NONE) {
            printf("**** %s: no error for outrange threshold %d(pri %d) ****\n", proc_name, max_threshold_port_1+1, i);
            return BCM_E_FAIL;
        }
    }

    /* Enable hard stage mapping and check */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_0, flags, 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_0, flags, &val_get), "");
    if (val_get != 1) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 1) ****\n", proc_name, val_get);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_1, flags, 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_1, flags, &val_get), "");
    if (val_get != 1) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 1) ****\n", proc_name, val_get);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Soft stage mapping
     ***************************************************************************/

    /* Set configurable ether_type to be recognized by PRD parser and check */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_custom_ether_type_set (unit, flexe_port_0, flags, 2, 0x1234), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_custom_ether_type_set (unit, flexe_port_1, flags, 6, 0xFFFF), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_custom_ether_type_get(unit, flexe_port_0, flags, 2, &val32_get_0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_custom_ether_type_get(unit, flexe_port_1, flags, 6, &val32_get_1), "");
    if (val32_get_0 != 0x1234 || val32_get_1 != 0xFFFF) {
        printf("**** %s: Error, val32_get_0 %d isn't 0x1234 or val32_get_1 %d isn't 0xFFFF ****\n", proc_name, rv, val32_get_0, val32_get_1);
        return BCM_E_FAIL;
    }

    /* Set key_id (per ether_type_code) */
    bcm_cosq_ingress_drop_flex_key_construct_id_t key_id;
    key_id.port = flexe_port_0;
    key_id.ether_type_code = 2;

    /* Set PRD soft stage (TCAM) key */
    int offset = 6;
    bcm_cosq_ingress_drop_flex_key_construct_t flex_key_config;
    bcm_cosq_ingress_drop_flex_key_construct_t flex_key_cfg_get;
    flex_key_config.offset_base = bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader;
    flex_key_config.ether_type_header_size = -1;
    flex_key_config.array_size = 4;
    flex_key_config.offset_array[0] = offset;
    flex_key_config.offset_array[1] = offset+1;
    flex_key_config.offset_array[2] = offset+2;
    flex_key_config.offset_array[3] = offset+3;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_construct_set(unit, key_id, flags, flex_key_config), "");

    /* Get PRD soft stage (TCAM) key */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get), "");
    if (flex_key_cfg_get.array_size != 4 ||
        flex_key_cfg_get.offset_array[0] != offset   ||
        flex_key_cfg_get.offset_array[1] != offset+1 ||
        flex_key_cfg_get.offset_array[2] != offset+2 ||
        flex_key_cfg_get.offset_array[3] != offset+3)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_cfg_get ****\n", proc_name);
        print flex_key_cfg_get;
        return BCM_E_FAIL;
    }

    /* Change PRD soft stage key */
    offset = 10;
    flex_key_config.offset_array[0] = offset;
    flex_key_config.offset_array[1] = offset+1;
    flex_key_config.offset_array[2] = offset+2;
    flex_key_config.offset_array[3] = offset+3;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_construct_set(unit, key_id, flags, flex_key_config), "");

    /* Get PRD soft stage (TCAM) key and check again */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get), "");
    if (flex_key_cfg_get.array_size != 4 ||
        flex_key_cfg_get.offset_array[0] != offset   ||
        flex_key_cfg_get.offset_array[1] != offset+1 ||
        flex_key_cfg_get.offset_array[2] != offset+2 ||
        flex_key_cfg_get.offset_array[3] != offset+3)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_cfg_get ****\n", proc_name);
        print flex_key_cfg_get;
        return BCM_E_FAIL;
    }

    /* Get and check PRD key on other ether_type(it should keep unchanged) */
    key_id.ether_type_code = 3;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get), "");
    if (flex_key_cfg_get.array_size != 4 ||
        flex_key_cfg_get.offset_array[0] != 0 ||
        flex_key_cfg_get.offset_array[1] != 0 ||
        flex_key_cfg_get.offset_array[2] != 0 ||
        flex_key_cfg_get.offset_array[3] != 0)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_cfg_get ****\n", proc_name);
        print flex_key_cfg_get;
        return BCM_E_FAIL;
    }

    /* Set PRD soft stage TCAM entrys */
    bcm_cosq_ingress_drop_flex_key_t ether_code;
    ether_code.value = 2;
    ether_code.mask = 0xF;

    bcm_cosq_ingress_drop_flex_key_t key_field_match_0;
    key_field_match_0.value = 0xAA;
    key_field_match_0.mask = 0xFF;
    bcm_cosq_ingress_drop_flex_key_t key_field_match_1;
    key_field_match_1.value = 0xBB;
    key_field_match_1.mask = 0xFF;
    bcm_cosq_ingress_drop_flex_key_t key_field_match_2;
    key_field_match_2.value = 0xCC;
    key_field_match_2.mask = 0xF0;
    bcm_cosq_ingress_drop_flex_key_t key_field_match_3;
    key_field_match_3.value = 0xDD;
    key_field_match_3.mask = 0x0F;

    /* Set the 1st TCAM entry */
    bcm_cosq_ingress_drop_flex_key_entry_t flex_key_info_0;
    flex_key_info_0.ether_code = ether_code;
    flex_key_info_0.num_key_fields = 1;
    flex_key_info_0.key_fields[0] = key_field_match_0;
    flex_key_info_0.priority = 1;

    uint32 key_index_0 = 10;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_0, flex_key_info_0), "");

    /* Set the 2nd TCAM entry */
    bcm_cosq_ingress_drop_flex_key_entry_t flex_key_info_1;
    flex_key_info_1.ether_code = ether_code;
    flex_key_info_1.num_key_fields = 4;
    flex_key_info_1.key_fields[0] = key_field_match_0;
    flex_key_info_1.key_fields[1] = key_field_match_1;
    flex_key_info_1.key_fields[2] = key_field_match_2;
    flex_key_info_1.key_fields[3] = key_field_match_3;
    flex_key_info_1.priority = 2;

    uint32 key_index_1 = 11;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_1, flex_key_info_1), "");

    /* Get and check 1st TCAM entry */
    bcm_cosq_ingress_drop_flex_key_entry_t flex_key_info_get;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_0, &flex_key_info_get), "");
    if (flex_key_info_get.ether_code.value != 2  ||
        flex_key_info_get.ether_code.mask  != 15 ||
        flex_key_info_get.num_key_fields   != 4  ||
        flex_key_info_get.priority         != 1  ||
        flex_key_info_get.key_fields[0].value != 0xAA ||
        flex_key_info_get.key_fields[0].mask  != 0xFF ||
        flex_key_info_get.key_fields[1].value != 0    ||
        flex_key_info_get.key_fields[1].mask  != 0    ||
        flex_key_info_get.key_fields[2].value != 0    ||
        flex_key_info_get.key_fields[2].mask  != 0    ||
        flex_key_info_get.key_fields[3].value != 0    ||
        flex_key_info_get.key_fields[3].mask  != 0)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_info_get(key_index_0) ****\n", proc_name);
        print flex_key_info_get;
        return BCM_E_FAIL;
    }

    /* Get and check 2nd TCAM entry */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_1, &flex_key_info_get), "");
    if (flex_key_info_get.ether_code.value != 2  ||
        flex_key_info_get.ether_code.mask  != 15 ||
        flex_key_info_get.num_key_fields   != 4  ||
        flex_key_info_get.priority         != 2  ||
        flex_key_info_get.key_fields[0].value != 0xAA ||
        flex_key_info_get.key_fields[0].mask  != 0xFF ||
        flex_key_info_get.key_fields[1].value != 0xBB ||
        flex_key_info_get.key_fields[1].mask  != 0xFF ||
        flex_key_info_get.key_fields[2].value != 0xCC ||
        flex_key_info_get.key_fields[2].mask  != 0xF0 ||
        flex_key_info_get.key_fields[3].value != 0xDD ||
        flex_key_info_get.key_fields[3].mask  != 0x0F)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_info_get(key_index_1) ****\n", proc_name);
        print flex_key_info_get;
        return BCM_E_FAIL;
    }

    /* Exchange entry setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_0, flex_key_info_1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_1, flex_key_info_0), "");

    /* Check the 2 entries again */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_0, &flex_key_info_get), "");
    if (flex_key_info_get.ether_code.value != 2  ||
        flex_key_info_get.ether_code.mask  != 15 ||
        flex_key_info_get.num_key_fields   != 4  ||
        flex_key_info_get.priority         != 2  ||
        flex_key_info_get.key_fields[0].value != 0xAA ||
        flex_key_info_get.key_fields[0].mask  != 0xFF ||
        flex_key_info_get.key_fields[1].value != 0xBB ||
        flex_key_info_get.key_fields[1].mask  != 0xFF ||
        flex_key_info_get.key_fields[2].value != 0xCC ||
        flex_key_info_get.key_fields[2].mask  != 0xF0 ||
        flex_key_info_get.key_fields[3].value != 0xDD ||
        flex_key_info_get.key_fields[3].mask  != 0x0F)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_info_get(key_index_1) ****\n", proc_name);
        print flex_key_info_get;
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_1, &flex_key_info_get), "");
    if (flex_key_info_get.ether_code.value != 2  ||
        flex_key_info_get.ether_code.mask  != 15 ||
        flex_key_info_get.num_key_fields   != 4  ||
        flex_key_info_get.priority         != 1  ||
        flex_key_info_get.key_fields[0].value != 0xAA ||
        flex_key_info_get.key_fields[0].mask  != 0xFF ||
        flex_key_info_get.key_fields[1].value != 0    ||
        flex_key_info_get.key_fields[1].mask  != 0    ||
        flex_key_info_get.key_fields[2].value != 0    ||
        flex_key_info_get.key_fields[2].mask  != 0    ||
        flex_key_info_get.key_fields[3].value != 0    ||
        flex_key_info_get.key_fields[3].mask  != 0)
    {
        printf("**** %s: Error, mismached value(s) in flex_key_info_get(key_index_0) ****\n", proc_name);
        print flex_key_info_get;
        return BCM_E_FAIL;
    }

    /* Enable PRD soft stage mapping */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_0, flags, 2), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_0, flags, &val_get), "");
    if (val_get != 2) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 2) ****\n", proc_name, val_get);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_1, flags, 2), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_1, flags, &val_get), "");
    if (val_get != 2) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 2) ****\n", proc_name, val_get);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Rx buffer setting
     ***************************************************************************/

    /* Initialize priority_config */
    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0 | BCM_PORT_F_PRIORITY_1;
    priority_config.priority_groups[0].num_of_entries = is_flexe_gen2 ? 4 : 0;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2 | BCM_PORT_F_PRIORITY_3;
    priority_config.priority_groups[1].num_of_entries = is_flexe_gen2 ? -1 : 0;

    /* Verify nof_priority_groups should be no greater than 2 */
    priority_config.nof_priority_groups = 3;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(0) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.nof_priority_groups = 2;

    /* Verify nof_priority_groups should be no less than 1 */
    priority_config.nof_priority_groups = 0;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(0) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.nof_priority_groups = 2;

    /* Verify TDM is only for L1 ports */
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(1) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;

    /* Verify with invalid nif_scheduler type */
    priority_config.priority_groups[0].sch_priority = 7;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(2) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;

    /* Verify num_of_entries should be 0 (For FlexE_Gen2, it should be -1 or >=1)*/
    priority_config.priority_groups[0].num_of_entries = is_flexe_gen2 ? 0 : 1;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(3) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].num_of_entries = is_flexe_gen2 ? 4 : 0;

    /* Verify source_priority can't be NULL */
    priority_config.priority_groups[1].source_priority = 0;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(4) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2 | BCM_PORT_F_PRIORITY_3;

    /* Verify with invliad source_priority */
    priority_config.priority_groups[0].source_priority = 60;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(5) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0 | BCM_PORT_F_PRIORITY_1;

    /* 2 same scheduler priorities */
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerLow;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error returned for bcm_port_priority_config_set(6) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;

    /* Configure the 1st client with 2 RMCs */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_set(unit, flexe_port_0, &priority_config), "(7)");
    /* Get RMC settings */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_get(unit, flexe_port_0, &priority_config_get), "(0)");
    /* Check RMCs setting */
    if (priority_config_get.nof_priority_groups != 2) {
        printf("**** %s: Error, the dumped nof_priority_groups isn't 2 ****\n", proc_name);
        print priority_config_get;
        return BCM_E_FAIL;
    }
    for (i = 0; i < 2; i++) {
        if (priority_config_get.priority_groups[i].sch_priority    != priority_config.priority_groups[i].sch_priority    ||
            priority_config_get.priority_groups[i].source_priority != priority_config.priority_groups[i].source_priority) {
            printf("**** %s: Error, mismathed priority_groups setting(index %d) ****\n", proc_name, i);
            print priority_config;
            print priority_config_get;
            return BCM_E_FAIL;
        }
    }

    /* Change RMC settings on the 1st client */
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0 | BCM_PORT_F_PRIORITY_2;
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_1 | BCM_PORT_F_PRIORITY_3;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_set(unit, flexe_port_0, &priority_config), "(8)");
    /* Get RMC setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_get(unit, flexe_port_0, &priority_config_get), "(1)");
    /* Check the dumped setting */
    if (priority_config_get.nof_priority_groups != 2) {
        printf("**** %s: Error, the dumped nof_priority_groups isn't 2 ****\n", proc_name);
        print priority_config_get;
        return BCM_E_FAIL;
    }
    for (i = 0; i < 2; i++) {
        if (priority_config_get.priority_groups[i].sch_priority    != priority_config.priority_groups[i].sch_priority    ||
            priority_config_get.priority_groups[i].source_priority != priority_config.priority_groups[i].source_priority) {
            printf("**** %s: Error, mismathed priority_groups setting(index %d) ****\n", proc_name, i);
            print priority_config;
            print priority_config_get;
            return BCM_E_FAIL;
        }
    }

    /* Get RMC setting on the 2nd client */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_get(unit, flexe_port_1, &priority_config_get), "(2)");
    /* Check the RMC setting shouldn't be effected */
    if (priority_config_get.nof_priority_groups != 1 || priority_config_get.priority_groups[0].sch_priority != bcmPortNifSchedulerLow) {
        printf("**** %s: Error, flexe_port_1 is effected by flexe_port_0 ****\n", proc_name);
        print priority_config_get;
        return BCM_E_FAIL;
    }

    /* Configure 1 RMC on each port with different parameters */
    for (port_index = 0; port_index < 2; port_index++) {
        for (i = 0; i < 2; i++) {
            for (ii = 0; ii < 6; ii++) {
                printf("**** Setting Rx buffer(port_index %d, i %d, sch_pri %d)****\n", port_index, i, sch_pri_list[i]);
                /* Set Rx buffer with different parameters */
                priority_config.nof_priority_groups = 1;
                priority_config.priority_groups[0].sch_priority = sch_pri_list[i];
                priority_config.priority_groups[0].source_priority = port_pri_list[ii];
                priority_config.priority_groups[0].num_of_entries = is_flexe_gen2 ? 4 : 0;
                rv  = bcm_port_priority_config_set(unit, flexe_ports[port_index], &priority_config);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail in bcm_port_priority_config_set(port_index %d, i %d, ii %d) ****\n", proc_name, rv, port_index, i, ii);
                    return rv;
                }

                /* Get Rx buffer and check */
                snprintf(error_msg, sizeof(error_msg), "(port_index %d, i %d, ii %d)", port_index, i, ii);
                BCM_IF_ERROR_RETURN_MSG(bcm_port_priority_config_get(unit, flexe_ports[port_index], &priority_config_get), error_msg);
                if (priority_config_get.nof_priority_groups != 1 ||
                    priority_config_get.priority_groups[0].sch_priority != sch_pri_list[i] ||
                    priority_config_get.priority_groups[0].source_priority != port_pri_list[ii]) {
                    printf("**** %s: Error, mismathed port priority setting(port_index %d, i %d, ii %d) ****\n", proc_name, port_index, i, ii);
                    print priority_config;
                    print priority_config_get;
                    return BCM_E_FAIL;
                }
            }
        }
    }

    /***************************************************************************
     * Misc settings
     ***************************************************************************/

    /* Set control frame and check */
    int control_frame_index = 1;
    uint32 mac_da_hi[2] = {0x1122, 0x7788};
    uint32 mac_da_lo[2] = {0x33445566, 0x99AABBCC};
    for (i = 0; i < 2; i++) {
        bcm_cosq_ingress_drop_control_frame_config_t control_frame_config;
        COMPILER_64_SET(control_frame_config.mac_da_val, mac_da_hi[i], mac_da_lo[i]);
        COMPILER_64_SET(control_frame_config.mac_da_mask, 0xffff, 0xffffffff);
        control_frame_config.ether_type_code= 2;
        control_frame_config.ether_type_code_mask= 0xF;
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_control_frame_set(unit, flexe_port_0, flags, control_frame_index, &control_frame_config), "");

        /* Get control frame and check */
        bcm_cosq_ingress_drop_control_frame_config_t control_frame_cfg_get;
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_control_frame_get(unit, flexe_port_0, flags, control_frame_index, &control_frame_cfg_get), "");
        int da_lo, da_hi, mask_lo, mask_hi;
        COMPILER_64_TO_32_LO(da_lo, control_frame_cfg_get.mac_da_val);
        COMPILER_64_TO_32_HI(da_hi, control_frame_cfg_get.mac_da_val);
        COMPILER_64_TO_32_LO(mask_lo, control_frame_cfg_get.mac_da_mask);
        COMPILER_64_TO_32_HI(mask_hi, control_frame_cfg_get.mac_da_mask);
        if (da_lo != mac_da_lo[i] || da_hi != mac_da_hi[i] || mask_lo != 0xffffffff || mask_hi != 0xffff ||
            control_frame_cfg_get.ether_type_code != 2 || control_frame_cfg_get.ether_type_code_mask != 0xF) {
            print "Mismatched control frame cfg:";
            print control_frame_cfg_get;
            return BCM_E_FAIL;
        }
    }

    /* Get PRD drop counter. FlexE_Gen2 not support it */
    if (!is_flexe_gen2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_stat_get(unit, flexe_port_0, snmpEtherStatsDropEvents, &val_64b_get),
            "fail to get PRD drop counter");
    }

    /* Set and get default priority */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_default_priority_set (unit, flexe_port_0, flags, 2), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_default_priority_get (unit, flexe_port_0, flags, &val32_get_0), "");
    if (val32_get_0 != 2) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_default_priority_get(val_get %d, exp 2) ****\n", proc_name, val32_get_0);
        return BCM_E_FAIL;
    }

    /* Set&get special label mapping */
    int label_index = sal_rand() % 4;
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t label_cfg_set, label_cfg_get;
    label_cfg_set.label_value = sal_rand() % 16;
    label_cfg_set.priority = sal_rand() % 4;
    label_cfg_set.is_tdm = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_mpls_special_label_set(unit, flexe_port_0, flags, label_index, label_cfg_set), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_mpls_special_label_get(unit, flexe_port_0, flags, label_index, &label_cfg_get), "");
    if (label_cfg_get.label_value !=  label_cfg_set.label_value ||
        label_cfg_get.priority != label_cfg_set.priority ||
        label_cfg_get.is_tdm != label_cfg_set.is_tdm) {
        print "Mismatched special label properties:";
        print label_cfg_set;
        print label_cfg_get;
        return BCM_E_FAIL;
    }

    /* FlexE client can't mark special label as TDM */
    label_cfg_set.is_tdm = 1;
    rv = bcm_cosq_ingress_port_drop_mpls_special_label_set(unit, flexe_port_0, flags, label_index, label_cfg_set);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error reported while mapping special label to TDM ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Configure ignore fields and check */
    for (i = 0; i < 2; i++) {
        /* Set */
        snprintf(error_msg, sizeof(error_msg), "fail to set ignore fields to %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgonreOuterTag, i), error_msg);

        /* Get and check */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, &val_get),
            "fail to get ignore fields");
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, &val_get_1),
            "fail to get ignore fields");
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, &val_get_2),
            "fail to get ignore fields");
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgonreOuterTag, &val_get_3),
            "fail to get ignore fields");
        if (val_get != i || val_get_1 != i || val_get_2 != i || val_get_3 != i) {
            printf("**** %s: The values of ignore fields aren't %d. They are %d, %d, %d and %d ****\n", proc_name, i, val_get, val_get_1, val_get_2, val_get_3);
            return BCM_E_FAIL;
        }
    }

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_port_0, 1), "(enable)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_port_1, 1), "(enable)");

    /* Disable PRD */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_0, flags, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_0, flags, &val_get), "");
    if (val_get != 0) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 0) ****\n", proc_name, val_get);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_1, flags, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_1, flags, &val_get), "");
    if (val_get != 0) {
        printf("**** %s: fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 0) ****\n", proc_name, val_get);
        return BCM_E_FAIL;
    }

    print "~~~~~~~~~~~~~~~~~~~~ End of semantic test for FlexE_MAC_L2 ~~~~~~~~~~~~~~~~~~~~";

    return BCM_E_NONE;
}

/* FlexE PRD semantic test for below FlexE L1 ports:
 * - flexe_port_type 0: ETH_L1
 * - flexe_port_type 1: FlexE MAC L1
 * - flexe_port_type 2: ILKN_L1
 * - flexe_port_type 3: FlexE_BusA_Client
 * - flexe_port_type 4: FlexE_BusC_Client
 * - flexe_port_type 5: FlexE_PHY
 * (Result: All PRD related APIs should return error for these ports)
 */
int dnx_flexe_prd_sem_L1_main(
    int unit,
    bcm_port_t flexe_port,
    int flexe_port_type)
{
    int flags = 0;
    int i, rv, port_index, header_type, val_get, val_get_1, val_get_2, val_get_3;
    int priority_to_map, priority_get;
    int tpid_list[4]     = {bcmCosqControlIngressPortDropTpid1,
                            bcmCosqControlIngressPortDropTpid2,
                            bcmCosqControlIngressPortDropTpid3,
                            bcmCosqControlIngressPortDropTpid4};
    int priority_list[5] = {BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_0,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_1,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_2,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_3,
                            BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM};
    int port_pri_all = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1 | BCM_PORT_F_PRIORITY_2 | BCM_PORT_F_PRIORITY_3 | BCM_PORT_F_PRIORITY_TDM;
    int port_pri_list[6] = {BCM_PORT_F_PRIORITY_0, BCM_PORT_F_PRIORITY_1, BCM_PORT_F_PRIORITY_2, BCM_PORT_F_PRIORITY_3, BCM_PORT_F_PRIORITY_TDM, port_pri_all};
    int sch_pri_list[3] = {bcmPortNifSchedulerLow, bcmPortNifSchedulerHigh, bcmPortNifSchedulerTDM};
    uint32 key, val32_get_0, val32_get_1;
    uint64 val_64b_get;
    bcm_cosq_ingress_port_drop_map_t cosq_map;
    bcm_port_prio_config_t priority_config;
    bcm_port_prio_config_t priority_config_get;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_prd_sem_L1_main";
    printf("~~~~~~~~~~~~~~~~~~~~ Starting semantic test(type %d) ~~~~~~~~~~~~~~~~~~~~\n", flexe_port_type);

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    if (flexe_port_type == 1) {
        /* Create BusB L1 client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, flexe_port, port_type_busB_L1, 10000, 0), "");
    } else if (flexe_port_type == 3) {
        /* Create BusA client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, flexe_port, port_type_busA, 10000, 0), "");
    } else if (flexe_port_type == 4) {
        /* Create BusC client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, flexe_port, port_type_busC, 10000, 0), "");
    }

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_port, 0), "(disable)");

    /***************************************************************************
     * PRD Profile setting on FlexE ports
     ***************************************************************************/

    rv = bcm_cosq_control_set(unit, flexe_port, -1, bcmCosqControlIngressPortDropPortProfileMap, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error returned for control field IngressPortDropPortProfileMap(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }

    rv = bcm_cosq_control_get(unit, flexe_port, -1, bcmCosqControlIngressPortDropPortProfileMap, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for control field IngressPortDropPortProfileMap(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Hard stage mapping
     ***************************************************************************/

    /* set */
    key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(1, 0);
    cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    priority_to_map = BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_0;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_map_set ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* get */
    rv = bcm_cosq_ingress_port_drop_map_get(unit, flexe_port, flags, cosq_map, key, &priority_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_map_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * TPID/RMC_threshold/PRD_enable
     ***************************************************************************/

    /* Set TPIDs on FlexE port */
    for (i = 0; i < 4; i++) {
        /* Iterate each TPID */
        rv = bcm_cosq_control_set(unit, flexe_port, -1, tpid_list[i], 0x9200+i);
        if (rv == BCM_E_NONE) {
            printf("**** %s: No error for control field %d (set) ****\n", proc_name, tpid_list[i]);
            return BCM_E_FAIL;
        }

        rv = bcm_cosq_control_get(unit, flexe_port, -1, tpid_list[i], val_get);
        if (rv == BCM_E_NONE) {
            printf("**** %s: No error for control field %d (get) ****\n", proc_name, tpid_list[i]);
            return BCM_E_FAIL;
        }
    }

    /* Set&get RMC threshold for each priority */
    for (i = 0; i < 5; i++) {
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port, flags, priority_list[i], 1);
        if (rv == BCM_E_NONE) {
            printf("**** %s: No error for bcm_cosq_ingress_port_drop_threshold_set(pri %d) ****\n", proc_name, priority_list[i]);
            return BCM_E_FAIL;
        }

        rv = bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port, flags, priority_list[i], &val32_get_0);
        if (rv == BCM_E_NONE) {
            printf("**** %s: No error for bcm_cosq_ingress_port_drop_threshold_get(pri %d) ****\n", proc_name, priority_list[i]);
            return BCM_E_FAIL;
        }
    }

    /* Enable PRD with different modes */
    for (i = 0; i < 3; i++) {
        rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port, flags, i);
        if (rv == BCM_E_NONE) {
            printf("**** %s: No error for bcm_cosq_ingress_port_drop_enable_set(mode %d) ****\n", proc_name, i);
            return BCM_E_FAIL;
        }
    }
    /* Get PRD mode */
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port, flags, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_enable_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Soft stage mapping
     ***************************************************************************/

    /* Set&get customer ether_type  */
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_set (unit, flexe_port, flags, 2, 0x1234);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_custom_ether_type_set ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_get(unit, flexe_port, flags, 2, &val32_get_0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_custom_ether_type_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Set key_id (per ether_type_code) */
    bcm_cosq_ingress_drop_flex_key_construct_id_t key_id;
    key_id.port = flexe_port;
    key_id.ether_type_code = 2;

    /* Set PRD soft stage (TCAM) key */
    int offset = 6;
    bcm_cosq_ingress_drop_flex_key_construct_t flex_key_config;
    bcm_cosq_ingress_drop_flex_key_construct_t flex_key_cfg_get;
    flex_key_config.offset_base = bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader;
    flex_key_config.ether_type_header_size = -1;
    flex_key_config.array_size = 4;
    flex_key_config.offset_array[0] = offset;
    flex_key_config.offset_array[1] = offset+1;
    flex_key_config.offset_array[2] = offset+2;
    flex_key_config.offset_array[3] = offset+3;
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_set(unit, key_id, flags, flex_key_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_flex_key_construct_set ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get PRD soft stage (TCAM) key */
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_flex_key_construct_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Set PRD soft stage TCAM entry */
    bcm_cosq_ingress_drop_flex_key_t ether_code;
    ether_code.value = 2;
    ether_code.mask = 0xF;

    bcm_cosq_ingress_drop_flex_key_t key_field_match_0;
    key_field_match_0.value = 0xAA;
    key_field_match_0.mask = 0xFF;

    bcm_cosq_ingress_drop_flex_key_entry_t flex_key_info_0;
    flex_key_info_0.ether_code = ether_code;
    flex_key_info_0.num_key_fields = 1;
    flex_key_info_0.key_fields[0] = key_field_match_0;
    flex_key_info_0.priority = 1;

    uint32 key_index_0 = 10;
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port, flags, key_index_0, flex_key_info_0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_flex_key_entry_set ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get PRD soft stage TCAM entry */
    bcm_cosq_ingress_drop_flex_key_entry_t flex_key_info_get;
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port, flags, key_index_0, &flex_key_info_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: No error for bcm_cosq_ingress_port_drop_flex_key_entry_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Rx buffer setting
     ***************************************************************************/

    /* Initialize priority_config */
    int entry_num = 4;
    if (flexe_port_type == 1 || flexe_port_type == 3 || flexe_port_type == 4) {
        /* BusB_L1, BusA, and BusC client */
        entry_num = 0;
    }
    priority_config.nof_priority_groups = 1;
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
    priority_config.priority_groups[0].source_priority = port_pri_all;
    priority_config.priority_groups[0].num_of_entries = entry_num;

    /* Verify sch_priority must be bcmPortNifSchedulerTDM */
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
    rv = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_port_priority_config_set(0) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;

    /* Verify source_priority must be IMB_PRD_PRIORITY_ALL */
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0;
    rv = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_port_priority_config_set(1) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].source_priority = port_pri_all;

    /* Verify nof_priority_groups should be no greater than 1 */
    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[1].source_priority = port_pri_all;
    priority_config.priority_groups[1].num_of_entries = entry_num;
    rv = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_port_priority_config_set(2) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.nof_priority_groups = 1;

    /* Set RMC with correct parameters */
    rv  = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (flexe_port_type < 2 && !dnx_flexe_gen2_support(unit,-1)) {
        BCM_IF_ERROR_RETURN_MSG(rv, "");
    } else if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_port_priority_config_set(port type %d) ****\n", proc_name, flexe_port_type);
        return BCM_E_FAIL;
    }

    /* Get RMC setting */
    rv = bcm_port_priority_config_get(unit, flexe_port, &priority_config_get);
    if (flexe_port_type < 2 && !dnx_flexe_gen2_support(unit,-1)) {
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN_MSG(rv, "");
        }
    } else {
        if (rv == BCM_E_NONE) {
            printf("**** %s: no error for bcm_port_priority_config_get(port type %d) ****\n", proc_name, flexe_port_type);
            return BCM_E_FAIL;
        }
    }

    /***************************************************************************
     * Misc settings
     ***************************************************************************/

    /* Set control frame */
    int control_frame_index = 1;
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_config;
    COMPILER_64_SET(control_frame_config.mac_da_val,0x1122,0x33445566);
    COMPILER_64_SET(control_frame_config.mac_da_mask,0xffff,0xffffffff);
    control_frame_config.ether_type_code= 2;
    control_frame_config.ether_type_code_mask= 0xF;
    rv = bcm_cosq_ingress_port_drop_control_frame_set(unit, flexe_port, flags, control_frame_index, &control_frame_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_control_frame_set ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get control frame and check */
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_cfg_get;
    rv = bcm_cosq_ingress_port_drop_control_frame_get(unit, flexe_port, flags, control_frame_index, &control_frame_cfg_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_control_frame_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Set and get default priority */
    rv = bcm_cosq_ingress_port_drop_default_priority_set (unit, flexe_port, flags, 2);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_default_priority_set ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_ingress_port_drop_default_priority_get (unit, flexe_port, flags, &val32_get_0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_default_priority_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Set&get special label properties */
    int label_index = sal_rand() % 4;
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t label_cfg_set, label_cfg_get;
    label_cfg_set.label_value = sal_rand() % 16;
    label_cfg_set.priority = sal_rand() % 4;
    label_cfg_set.is_tdm = sal_rand() % 2;
    rv = bcm_cosq_ingress_port_drop_mpls_special_label_set(unit, flexe_port, flags, label_index, label_cfg_set);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_mpls_special_label_set ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_ingress_port_drop_mpls_special_label_get(unit, flexe_port, flags, label_index, &label_cfg_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for bcm_cosq_ingress_port_drop_mpls_special_label_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Configure ignore fields */
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreIpDscp(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreMplsExp(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreInnerTag(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgonreOuterTag, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreOuterTag(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get ignore fields */
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreIpDscp(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreMplsExp(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreInnerTag(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgonreOuterTag, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: no error for control field IngressPortDropIgnoreOuterTag(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_port, 1), "(enable)");

    printf("~~~~~~~~~~~~~~~~~~~~ End of semantic test(type %d) ~~~~~~~~~~~~~~~~~~~~\n", flexe_port_type);

    return BCM_E_NONE;
}

/* Funtion to test maximal ESB bandwidth */
int dnx_flexe_esb_bw_test_main(
    int unit,
    bcm_port_t flexe_phy,
    bcm_port_t eth_flexe,
    int flexe_is_L1,
    bcm_port_t eth_l2_start,
    int nof_eths_in_none_flexe_snake)
{
    int i;
    int flexe_client_speed = 400000;
    int total_slots = flexe_client_speed / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 60;
    bcm_port_t client_b = 61;
    flexe_port_type_t client_b_type = flexe_is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;
    bcm_port_t eth_l2_src, eth_l2_dst;
    char error_msg[200]={0,};


    /***************************************************************************
     * Datapath:
     *                                           Q2A
     *                  +-------------------------------------------------+
     *                  |                                                 |
     *     IXIA_400G<-->|ETH_400G<------>BusB_400G<------>BusA_400G<----->|<-->FlexE_PHY_400G
     *                  |                                                 |      (loopback)
     *                  |                                                 |
     *     IXIA_100G<-->|eth0----->eth1----->eth2----->...----->ethN-->+  |
     *                  |  ^                                           |  |
     *                  |  |                                           V  |
     *                  |  +<------------------------------------------+  |
     *                  |                                                 |
     *                  +-------------------------------------------------+
     *
     *     when flexe_is_L1 == 0,
     *         ETH_400G is L2 port, BusB_400G is FlexE_L2 client
     *         Total NIF speed = 400G*2 + toal speed of eth_0~N
     *     when flexe_is_L1 == 1,
     *         ETH_400G is L1 port, BusB_400G is FlexE_L1 client
     *         Total NIF speed = 400G*1 + toal speed of eth_0~N
     ***************************************************************************/

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, 1), "");

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, flexe_client_speed, 0),
        "fail to create client_a");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, client_b_type, flexe_client_speed, 0),
        "fail to create client_b");

    if (flexe_is_L1) {
        /* Create L1 datapath: eth_flexe <- L1_switch -> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_flexe, client_b, client_a, 0),
            "fail to set Eth2FlexE L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_flexe, 0),
            "fail to set FlexE2Eth L1 datapath");
    } else {
        /* Create L2 datapath: eth_flexe <-force_fwd-> client_b <--> client_a */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_flexe, client_b, client_a, 0),
            "fail to set Eth2FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_flexe, 0),
            "fail to set FlexE2Eth L2 datapath");
    }

    /* Assign slots for the FlexE client */
    for (i = 0; i < total_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* FlexE OH setting */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1),
        "fail to set OhGroupID");

    /* Loopback the FlexE PHY */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, 2), "");

    /* Create L2 snake with the L2 ETH ports */
    for (i = 0; i < nof_eths_in_none_flexe_snake; i++) {
        eth_l2_src = eth_l2_start + i;
        eth_l2_dst = i == nof_eths_in_none_flexe_snake-1 ? eth_l2_start : eth_l2_src+1;

        /* Loopback the L2 ports */
        if (i) {
            snprintf(error_msg, sizeof(error_msg), "(port %d)", eth_l2_src);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_l2_src, 1), error_msg);
            printf("**** bcm_port_loopback_set(port %d) ****\n", eth_l2_src);
        }

        /* Force forwarding */
        snprintf(error_msg, sizeof(error_msg), "(src_port %d)", eth_l2_src);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_l2_src, eth_l2_dst, 1), error_msg);
        printf("**** bcm_port_force_forward_set(src_port %d, dst_port %d) ****\n", eth_l2_src, eth_l2_dst);
    }

    return BCM_E_NONE;
}

/* Funtion to test FlexE bypass mode */
int dnx_flexe_bypass_mode_test_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port,
    bcm_port_t ilkn_port,
    bcm_port_t phy_port,
    int grp_speed,
    int grp_index,
    int grp_bypass,
    int is_L1_switch_cen,
    int is_L1_switch_dis)
{
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_port_t client_c = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int client_speed = grp_speed;
    int flexe_grp_bandwidth = grp_speed;
    int nof_slots = flexe_grp_bandwidth / 5000;
    flexe_port_type_t client_b_type_cen = is_L1_switch_cen ? port_type_busB_L1 : port_type_busB_L2_L3;
    flexe_port_type_t client_b_type_dis = port_type_busB_L2_L3;
    flexe_port_type_t client_b_type[2] = {client_b_type_cen, client_b_type_dis};
    char error_msg[200]={0,};


    /***************************************************************************
     * FlexE PHY port(s) and group creation
     ***************************************************************************/

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    if (phy_port) {
        /* Use the provided PHY */
        flexe_phy_ports[0] = phy_port;
    } else {
        /* Use FlexE PHY(s) in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 8; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
    }
    flexe_bypass_en = grp_bypass; /* Enable bypass mode */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs), error_msg);
    }

    /***************************************************************************
     * FlexE client(s) creation
     ***************************************************************************/

    /* Create a busA client and a busB client on each device */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_a, port_type_busA, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit[i], client_b, client_b_type[i], client_speed, 0), error_msg);
    }

    /* Create busC client on distributed Q2A */
    snprintf(error_msg, sizeof(error_msg), "fail to create busC client(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_c, port_type_busC, client_speed, 0), error_msg);

    /***************************************************************************
     * The datapath is:
     *
     *                      Centralized Device                     Distributed Device
     *                   +----------------------+      +-------------------------------------+
     *                   |                      |      |                                     |
     * eth_port <-L1/L2->| client_b<-->client_a | <--> | client_a<-->client_b/c<-->ilkn_port |
     *                   |                      |      | (Flexe<->ILKN L1 or L2&L3 datapath) |
     *                   |                      |      |                                     |
     *                   +----------------------+      +-------------------------------------+
     *
     ***************************************************************************/

    if (is_L1_switch_cen)
    {
        /* Build Eth2FlexE L1 datapath on centralized Device */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 0),
            "fail to set L1 datapath on Q2A_#0");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 0),
            "fail to set L1 datapath on Q2A_#0");
    }
    else
    {
        /* Build Eth2FlexE L2 datapath on centralized Device */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 0),
            "fail to set L2 datapath on Q2A_#0");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 0),
            "fail to set L2 datapath on Q2A_#0");
    }

    /* Build Flexe2ILKN and ILKN2Flexe L1/L2 datapath on distributed Device */
    if (is_L1_switch_dis)
    {
        /* L1 datapath:
         * client_a <-> client_c <-> ilkn_port(loopback)
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a, client_c, ilkn_port, 0),
            "fail to set L1 datapath on Q2A_#1");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_port, client_c, client_a, 0),
            "fail to set L1 datapath on Q2A_#1");
    }
    else
    {
        /* L2 datapath:
         * client_a -> client_b -> ilkn_port -> loopback ->
         * -> ilkn_port -> force_fwd -> client_b -> client_a
         */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a, client_b, ilkn_port, 0),
            "fail to set Flexe2ILKN datapath on Q2A_#1");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_b, client_a, 0),
            "fail to set Flexe2ILKN datapath on Q2A_#1");
    }

    /***************************************************************************
     * Calendar and OH settings
     ***************************************************************************/

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        for (ii = 0; ii < nof_slots; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

        /* Set group ID in overhead */
        if (!grp_bypass) {
            snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index), error_msg);
        }
    }

    /* Loopback on ILKN port */
    snprintf(error_msg, sizeof(error_msg), "failed to set loopback on ilkn port %d", ilkn_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, ilkn_port, 1), error_msg);

    return BCM_E_NONE;
}

/* Funtion to clear FlexE bypass setting */
int dnx_flexe_bypass_mode_test_destroy(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port,
    bcm_port_t ilkn_port,
    int grp_speed,
    int grp_index,
    int is_L1_switch_cen,
    int is_L1_switch_dis)
{
    int i, ii;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_port_t client_c = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int nof_slots = grp_speed / 5000;
    char error_msg[200]={0,};


    /***************************************************************************
     * Clear FlexE calendar
     ***************************************************************************/

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    for (i = 0; i < 2; i++) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);
    }

    /***************************************************************************
     * Clear FlexE datapath:
     *
     *                      Centralized Device                     Distributed Device
     *                   +----------------------+      +-------------------------------------+
     *                   |                      |      |                                     |
     * eth_port <-L1/L2->| client_b<-->client_a | <--> | client_a<-->client_b/c<-->ilkn_port |
     *                   |                      |      | (Flexe<->ILKN L1 or L2&L3 datapath) |
     *                   |                      |      |                                     |
     *                   +----------------------+      +-------------------------------------+
     *
     ***************************************************************************/

    /* Clear L1/L2 datapath on centralized Device */
    if (is_L1_switch_cen)
    {
        /* Clear L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 1),
            "fail to clear L1 datapath on Q2A_#0");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 1),
            "fail to clear L1 datapath on Q2A_#0");
    }
    else
    {
        /* Clear L2 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 1),
            "fail to clear L2 datapath on Q2A_#0");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 1),
            "fail to clear L2 datapath on Q2A_#0");
    }

    /* Clear L1/L2 datapath on distributed Device */
    if (is_L1_switch_dis)
    {
        /* Clear L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit_dis, client_a, client_c, ilkn_port, 1),
            "fail to clear L1 datapath on Q2A_#1");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit_dis, ilkn_port, client_c, client_a, 1),
            "fail to clear L1 datapath on Q2A_#1");
    }
    else
    {
        /* Clear L2 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a, client_b, ilkn_port, 1),
            "fail to clear L2 datapath on Q2A_#1");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_b, client_a, 1),
            "fail to clear L2 datapath on Q2A_#1");
    }

    /***************************************************************************
     * Clear FlexE clients
     ***************************************************************************/

    /* Remove busA client and busB client on each device */
    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to remove busA client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit[i], client_a), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to remove busB client(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit[i], client_b), error_msg);
    }

    /* remove busC client on distributed Q2A */
    snprintf(error_msg, sizeof(error_msg), "fail to remove busC client(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_dis, client_c), error_msg);

    /***************************************************************************
     * Clear FlexE groups
     ***************************************************************************/

    for (i = 0; i < 2; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit[i], flexe_grp), error_msg);
    }

    return BCM_E_NONE;
}

/* Basic FlexE setting on single degvice */
int dnx_flexe_basic_cfg_main(
    int unit,
    int grp_speed,
    int grp_index,
    int client_speed,
    bcm_port_t eth_port,
    bcm_port_t flexe_phy,
    int flexe_phy_loopback,
    int is_flexe_L1)
{
    int i;
    int nof_pcs = 1;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_gport_t flexe_grp;
    int total_slots = grp_speed / 5000;
    int client_slots = client_speed / 5000;
    flexe_port_type_t client_b_type = is_flexe_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_basic_cfg_main";

    /* Create FlexE group */
    if (flexe_phy > 0) {
        /* Valid PHY port ID. Use it for FlexE group creation */
        flexe_phy_ports[0] = flexe_phy;
    } else {
        /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 8; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
        if (!nof_pcs) {
            printf("**** %s: Error, no FlexE PHY provided for FlexE group creation(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        }
    }
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

    /* Create busA client and busB client */
    snprintf(error_msg, sizeof(error_msg), "fail to create busA client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to create busB client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, client_b_type, client_speed, 0), error_msg);

    /***************************************************************************
     * FlexE datapath:
     *                           FlexE Core
     *                   +------------------------+
     *                   |                        |
     * eth_port <-L1/L2->| client_b <--> client_a | <--> flexe_phy[loopback(optional)]
     *                   |                        |
     *                   +------------------------+
     *
     ***************************************************************************/

    if (is_flexe_L1) {
        /* Build L1 datapath between eth_port and FlexE */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0),
            "fail to set L1 datapath between eth_port and FlexE");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0),
            "fail to set L1 datapath between eth_port and FlexE");
    } else {
        /* Build L2 datapath between eth_port and FlexE */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0),
            "fail to set L2 datapath between eth_port and FlexE");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0),
            "fail to set L2 datapath between eth_port and FlexE");
    }

    /* Loopback FlexE PHY */
    if (flexe_phy_loopback) {
        for (i = 0; i < nof_pcs; i++) {
            flexe_phy = flexe_phy_ports[i];
            snprintf(error_msg, sizeof(error_msg), "(flexe_phy %d)", flexe_phy);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY), error_msg);
        }
        if (dnx_flexe_gen2_support(unit, -1)) {
            /* Wait for PHY(s) linkup. The purpose is to speedup dnx_flexe_util_calendar_slots_set() */
            bshell(unit,"sleep 2");
        }
    }

    /* Assign calendar slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

    /* Set group ID in overhead */
    if (!flexe_bypass_en) {
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1), error_msg);
    }

    return BCM_E_NONE;
}

/* Funtion to destroy all FlexE settings which are done by dnx_flexe_basic_cfg_main */
int dnx_flexe_basic_cfg_destroy(
    int unit,
    int grp_speed,
    int grp_index,
    bcm_port_t eth_port,
    bcm_port_t flexe_phy,
    int is_flexe_L1)
{
    int i;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_gport_t flexe_grp;
    int total_slots = grp_speed / 5000;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_basic_cfg_destroy";

    /* Clear FlexE calendar */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

    /***************************************************************************
     * FlexE datapath:
     *                           FlexE Core
     *                   +------------------------+
     *                   |                        |
     * eth_port <-L1/L2->| client_b <--> client_a | <--> flexe_phy[loopback(optional)]
     *                   |                        |
     *                   +------------------------+
     *
     ***************************************************************************/

    if (is_flexe_L1)
    {
        /* Clear L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 1),
            "fail to clear FlexE L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 1),
            "fail to clear FlexE L1 datapath");
    }
    else
    {
        /* Clear L2 datapath */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 1),
            "fail to clear FlexE L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 1),
            "fail to clear FlexE L2 datapath");
    }

    /* Remove busA client and busB client */
    snprintf(error_msg, sizeof(error_msg), "fail to remove busA client(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_a), error_msg);
    if (!(dnx_flexe_gen2_support(unit, -1) && is_flexe_L1)) {
        snprintf(error_msg, sizeof(error_msg), "fail to remove busB client(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b), error_msg);
    }
    /* Clear FlexE group */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp), error_msg);

    /* Remove loopback on FlexE PHY. Need to clear loopback  */
    if (flexe_phy > 0) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, 0), "(flexe_phy)");
    } else {
        printf("**** %s: Warnning, please clear loopback setting manually on each PHY in the FlexE group!!! ****\n", proc_name);
    }

    return BCM_E_NONE;
}

/* Funtion L2 datapath between FlexE and COP under distributed mode */
int dnx_flexe_distributed_datapath_flexe_and_cop_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t ilkn_port,
    bcm_port_t flexe_phy)
{
    int i;
    int vid_in = 1111;
    int vid_out = 2222;
    int nof_pcs = 1;
    int grp_index = 1;
    int flexe_grp_speed = 200000;
    int nof_slots = flexe_grp_speed / 5000;
    uint32 flags = 0;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_port_t client_b_1 = 60;
    char error_msg[200]={0,};


    /*********************************************************************************************************************************
     * Topology:
     *
     *                            Centralized Device                                   Distributed Device
     *                        +----------------------+      +------------------------------------------------------------------------+
     *                        |                      |      |                                                                        |
     * eth_tx_rx<-force_fwd-->| client_b<-->client_a | <--> | client_a <--> client_b ----L2_lookup---> client_b_1 --loopback_B2B-->+ |
     *                        |                      |      |                   ^                  (VLAN Translation)              | |
     *                        |                      |      |                   |                                                  | |
     *                        |                      |      |                   +<--L2-- ilkn_port <-- client_b_1 <--loopback_B2B--+ |
     *                        |                      |      |                            (loopback)                                  |
     *                        +----------------------+      +------------------------------------------------------------------------+
     *
     *********************************************************************************************************************************/

    /***************************************************************************
     * FlexE setting on centralized device
     ***************************************************************************/
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_basic_cfg_main(unit_cen, flexe_grp_speed, grp_index, flexe_grp_speed, eth_tx_rx, flexe_phy, 0, 0),
        "on centralized device");

    /***************************************************************************
     * FlexE setting on distributed device
     ***************************************************************************/

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    flexe_phy_ports[0] = flexe_phy;
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs), error_msg);

    /* Create FlexE clients */
    snprintf(error_msg, sizeof(error_msg), "fail to create client_a client(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_a, port_type_busA, flexe_grp_speed, 0), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to create client_b client(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b, port_type_busB_L2_L3, flexe_grp_speed, 0), error_msg);
    snprintf(error_msg, sizeof(error_msg), "fail to create client_b_1 client(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit_dis, client_b_1, port_type_busB_L2_L3, flexe_grp_speed, 0), error_msg);

    /*
     * FlexE flow setting:
     * 1. client_a --> client_b
     * 2. client_b --> client_a
     * 3. client_b_1(tx) --> client_b_1(rx)
     * 4. client_b_1 --> ilkn_port
     */
    snprintf(error_msg, sizeof(error_msg), "(unit %d, disable)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_dis, client_b, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_dis, client_b_1, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_dis, ilkn_port, 0), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit_dis, flags, client_a, 0, client_b), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit_dis, flags, client_b, 0, client_a), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit_dis, flags, client_b_1, 0, client_b_1), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit_dis, flags, client_b_1, 0, ilkn_port), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(unit %d, enable)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_dis, client_b, 1), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_dis, client_b_1, 1), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit_dis, ilkn_port, 1), error_msg);

    /*
     * L2 setting:
     * 00:00:00:00:11:11+vid_in-->client_b_1 (for client_b->client_b_1 L2 lookup)
     * 00:00:00:00:11:11+vid_out-->client_b  (for ilkn_port->client_b L2 lookup)
     */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_oper(unit_dis, client_b_1, vid_in, 0, 1), error_msg);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_oper(unit_dis, client_b, vid_out, 0, 1), error_msg);

    /* Enable egress VLAN translation at client_b_1 */
    BCM_IF_ERROR_RETURN_MSG(egr_vlan_edit(unit_dis, client_b, client_b_1, vid_in, vid_out), error_msg);

    /* loopback ilkn_port and add it to vid_out */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit_dis, vid_out, ilkn_port, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit_dis, ilkn_port, 1), error_msg);

    /* Calendar setting */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < nof_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, 0, FLEXE_CAL_A, nof_slots), error_msg);

    /* FlexE OH setting */
    snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit_dis);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

    return BCM_E_NONE;
}

int dnx_flexe_busB_creation_with_sequence_main(
    int unit,
    int client_type,
    int eth_tx,
    int eth_rx,
    int eth_lb,
    int en_dis_sequence)
{
    int i;
    int client_speed = 10000;
    int nof_clients = 4;
    bcm_port_t client_b_0 = 50;
    bcm_port_t client_b_1 = 51;
    bcm_port_t client_b_2 = 52;
    bcm_port_t client_b_3 = 53;
    bcm_port_t busb_clients[4] = {client_b_0, client_b_1, client_b_2, client_b_3};
    flexe_port_type_t client_b_type;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_busB_creation_with_sequence_main";

    /***************************************************************************
     * Datapath:
     *
     *                  FlexE Core
     *              +--------------------+
     *              |                    |
     * eth_tx------>| client_b_0 -->+    |
     *              |               |    |
     *   eth_lb<----| client_b_1 <--+    |
     *      |       |                    |
     *      +------>| client_b_2 -->+    |
     *              |               |    |
     * eth_rx<------| client_b_3 <--+    |
     *              |                    |
     *              +--------------------+
     **************************************************************************/

    /*******************************************************************************
     * Create BusB clients with sequence
     *******************************************************************************/

    printf("**** %s: Creating %d BusB clients with sequence %d ****\n", proc_name, nof_clients, en_dis_sequence);
    for (i = 0; i < nof_clients; i++) {
        /* When client_type is set, client_b_1 and client_b_2 will be L1 clients */
        if (client_type == 1) {
            /* All clients are L1 clients */
            client_b_type = port_type_busB_L1;
        } else if (client_type == 2) {
            /* All clients are L2 clients */
            client_b_type = port_type_busB_L2_L3;
        } else {
            /* client_b_0&3 are L2 clients. client_b_1&2 are L1 clients */
            if (busb_clients[i] == client_b_0 || busb_clients[i] == client_b_3) {
                client_b_type = port_type_busB_L2_L3;
            } else {
                client_b_type = port_type_busB_L1;
            }
        }

        printf("\n**** %s: Creating BusB client(index %d) ****\n", proc_name, i);
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, busb_clients[i], client_b_type, client_speed, 0), error_msg);

        if (i == en_dis_sequence) {
            printf("\n**** %s: Disabling BusB client(index %d) ****\n", proc_name, en_dis_sequence);
            snprintf(error_msg, sizeof(error_msg), "fail to disable BusB client(index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, busb_clients[i], 0), error_msg);
        }
    }

    printf("\n**** %s: Enabling BusB client(index %d) ****\n", proc_name, en_dis_sequence);
    snprintf(error_msg, sizeof(error_msg), "fail to enable BusB client(index %d)", en_dis_sequence);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, busb_clients[en_dis_sequence], 1), error_msg);

    /*******************************************************************************
     * DataPath creation
     *******************************************************************************/

    /* Disable BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to disable BusB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, busb_clients[i], 0), error_msg);
    }

    if (client_type == 1) {
        /* Disable ETH ports */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_tx, 0), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 0), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_rx, 0), "fail to disable ETH ports");

        /* L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_tx, 0, client_b_0),
            "fail in L1 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_0, 0, client_b_1),
            "fail in L1 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, eth_lb),
            "fail in L1 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_lb, 0, client_b_2),
            "fail in L1 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_b_3),
            "fail in L1 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_3, 0, eth_rx),
            "fail in L1 datapath setting");

        /* Enable ETH ports */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_tx, 1), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 1), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_rx, 1), "fail to disable ETH ports");
    } else if (client_type == 2) {
        /* L2 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b_0, 1),
            "fail in L2 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_0, 0, client_b_1),
            "fail in L2 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_1, eth_lb, 1),
            "fail in L2 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_lb, client_b_2, 1),
            "fail in L2 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_b_3),
            "fail in L2 datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_3, eth_rx, 1),
            "fail in L2 datapath setting");
    } else {
        /* Mixed datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 0), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b_0, 1), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_0, 0, client_b_1), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, eth_lb), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_lb, 0, client_b_2), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_b_3), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_3, eth_rx, 1), "fail in mixed datapath setting");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 1), "fail in mixed datapath setting");
    }

    /* Enable BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to enable BusB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, busb_clients[i], 1), error_msg);
    }

    /* Loopback eth_lb */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_lb, BCM_PORT_LOOPBACK_PHY), "");

    return BCM_E_NONE;
}

int dnx_flexe_busB_creation_with_sequence_clear(
    int unit,
    int client_type,
    int eth_tx,
    int eth_rx,
    int eth_lb)
{
    int i;
    int nof_clients = 4;
    bcm_port_t client_b_0 = 50;
    bcm_port_t client_b_1 = 51;
    bcm_port_t client_b_2 = 52;
    bcm_port_t client_b_3 = 53;
    bcm_port_t busb_clients[4] = {client_b_0, client_b_1, client_b_2, client_b_3};
    char error_msg[200]={0,};


    /***************************************************************************
     * Datapath:
     *
     *                  FlexE Core
     *              +--------------------+
     *              |                    |
     * eth_tx------>| client_b_0 -->+    |
     *              |               |    |
     *   eth_lb<----| client_b_1 <--+    |
     *      |       |                    |
     *      +------>| client_b_2 -->+    |
     *              |               |    |
     * eth_rx<------| client_b_3 <--+    |
     *              |                    |
     *              +--------------------+
     **************************************************************************/

    /*******************************************************************************
     * DataPath creation
     *******************************************************************************/

    /* Disable BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to disable BusB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, busb_clients[i], 0), error_msg);
    }

    if (client_type == 1) {
        /* Disable ETH ports */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_tx, 0), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 0), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_rx, 0), "fail to disable ETH ports");

        /* L1 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_tx, 0, client_b_0), "fail to clear L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_0, 0, client_b_1), "fail to clear L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_1, 0, eth_lb), "fail to clear L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_lb, 0, client_b_2), "fail to clear L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_2, 0, client_b_3), "fail to clear L1 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_3, 0, eth_rx), "fail to clear L1 datapath");

        /* Enable ETH ports */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_tx, 1), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 1), "fail to disable ETH ports");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_rx, 1), "fail to disable ETH ports");
    } else if (client_type == 2) {
        /* L2 datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b_0, 0), "fail to clear L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_0, 0, client_b_1), "fail to clear L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_1, eth_lb, 0), "fail to clear L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_lb, client_b_2, 0), "fail to clear L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_2, 0, client_b_3), "fail to clear L2 datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_3, eth_rx, 0), "fail to clear L2 datapath");
    } else {
        /* Mixed datapath */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 0), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b_0, 0), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_0, 0, client_b_1), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_1, 0, eth_lb), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_lb, 0, client_b_2), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_2, 0, client_b_3), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_3, eth_rx, 0), "fail to clear mixed datapath");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_lb, 1), "fail to clear mixed datapath");
    }

    /* Enable BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to enable BusB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, busb_clients[i], 1), error_msg);
    }

    /*******************************************************************************
     * Remove BusB clients
     *******************************************************************************/

    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, busb_clients[i]), error_msg);
    }

    /* Loopback eth_lb */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_lb, BCM_PORT_LOOPBACK_NONE), "");

    return BCM_E_NONE;
}

/* Function to create L2 snake with BusB L2 clients */
int dnx_flexe_dpp_busb_l2(
    int unit,
    bcm_port_t flexe_phy,
    int client_speed,
    int nof_clients,
    bcm_port_t eth_port_in,
    bcm_port_t eth_port_out)
{
    int i;
    int nof_pcs = 1;
    bcm_port_t client_b_start = 151;
    bcm_port_t current_client_id, next_client_id;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};


    /* Create FlexE group */
    flexe_phy_ports[0] = flexe_phy;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), "");

    /* Create busB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "fail to create BusB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_start + i, port_type_busB_L2_L3, client_speed, 0), error_msg);
    }

    /***************************************************************************
     * FlexE datapath:
     *                           FlexE Core
     *                   +------------------------+
     *                   |                        |
     * eth_port_in------>| client_b_0(loopback)   |
     *                   |       |                |
     *                   |       v                |
     *                   | client_b_1(loopback)   |
     *                   |       |                |
     *                   |       v                |
     *                   | client_b_2(loopback)   |
     *                   |       |                |
     *                   |       v                |
     *                   |      ...               |
     *                   |       |                |
     *                   |       v                |
     *                   | client_b_n(loopback)   |
     *                   |       |                |
     * eth_port_out<-----|<------+                |
     *                   |                        |
     *                   +------------------------+
     *
     ***************************************************************************/

    /* Build above FlexE snake */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_in, client_b_start, 1), "(eth_in->client_b_start)");
    for (i = 0; i < nof_clients; i++) {
        current_client_id = client_b_start + i;
        next_client_id = client_b_start + i + 1;

        /** loopback itself */
        snprintf(error_msg, sizeof(error_msg), "fail in BusB to BusB flow setting(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, current_client_id, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, current_client_id, 0, current_client_id), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, current_client_id, 1), error_msg);

        /** Force forwarding */
        if (i == nof_clients - 1) {
            snprintf(error_msg, sizeof(error_msg), "(busb-->eth_out, index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, current_client_id, eth_port_out, 1), error_msg);
        } else {
            snprintf(error_msg, sizeof(error_msg), "(busb-->busb, index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, current_client_id, next_client_id, 1), error_msg);
        }
    }

    return BCM_E_NONE;
}

/* Function to clear all FlexE settings in dnx_flexe_dpp_busb_l2() */
int dnx_flexe_dpp_busb_l2_clear(
    int unit,
    int nof_clients,
    bcm_port_t eth_port_in,
    bcm_port_t eth_port_out)
{
    int i;
    bcm_port_t client_b_start = 151;
    bcm_port_t current_client_id, next_client_id;
    bcm_gport_t flexe_grp;
    char error_msg[200]={0,};


    /***************************************************************************
     * FlexE datapath:
     *                           FlexE Core
     *                   +------------------------+
     *                   |                        |
     * eth_port_in------>| client_b_0(loopback)   |
     *                   |       |                |
     *                   |       v                |
     *                   | client_b_1(loopback)   |
     *                   |       |                |
     *                   |       v                |
     *                   | client_b_2(loopback)   |
     *                   |       |                |
     *                   |       v                |
     *                   |      ...               |
     *                   |       |                |
     *                   |       v                |
     *                   | client_b_n(loopback)   |
     *                   |       |                |
     * eth_port_out<-----|<------+                |
     *                   |                        |
     *                   +------------------------+
     *
     ***************************************************************************/

    /* Clear the FlexE snake */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port_in, client_b_start, 0), "(eth_in->client_b_start)");
    for (i = 0; i < nof_clients; i++) {
        current_client_id = client_b_start + i;
        next_client_id = client_b_start + i + 1;

        /** Clear loopback */
        snprintf(error_msg, sizeof(error_msg), "fail in BusB to BusB flow clearing(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, current_client_id, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, current_client_id, 0, current_client_id), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, current_client_id, 1), error_msg);

        /** Clear force forwarding */
        if (i == nof_clients - 1) {
            snprintf(error_msg, sizeof(error_msg), "(busb-->eth_out, index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, current_client_id, eth_port_out, 0), error_msg);
        } else {
            snprintf(error_msg, sizeof(error_msg), "(busb-->busb, index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, current_client_id, next_client_id, 0), error_msg);
        }
    }

    /* Remove busB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_start + i), error_msg);
    }

    /* Remove FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp), "");

    return BCM_E_NONE;
}

int dnx_flexe_dpp_interrupt_check (int unit)
{
    reg_val v;

    /* REASSEMBLY_INTERRUPT */
    diag_reg_get(unit, "SPB_REASSEMBLY_INTERRUPT_REGISTER", v);
    if(v[0] != 0){
        print "\n======== REASSEMBLY_INTERRUPT detected =================\n";
        bshell(unit, "show interrupt");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/* Remove and then add BusB client back to the L2 snake which is created in dnx_flexe_dpp_busb_l2() */
int dnx_flexe_dpp_L2_busb_remove_add(
    int unit,
    int client_id,
    int client_speed,
    bcm_port_t dst_port,
    int repeat_times)
{
    int i;
    reg_val v;
    char error_msg[200]={0,};


    printf("Removing/adding FlexE client %d for %d times\n", client_id, repeat_times);
    for (i = 0; i < repeat_times; i++) {
        printf("Iteration %d / %d\n", i + 1, repeat_times);

        /* Remove */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_id, client_id, 1), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_id), "");
        sal_usleep(300 * 1000);

        /* Re-add */
        snprintf(error_msg, sizeof(error_msg), "fail to create busB client %d", client_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_id, port_type_busB_L2_L3, client_speed, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "on client %d", client_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_id, 0, client_id), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_id, 1), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_id, dst_port, 1), error_msg);
        sal_usleep(300 * 1000);

        diag_reg_get(unit, "SPB_REASSEMBLY_INTERRUPT_REGISTER", v);
        if(v[0] != 0){
            print "\n======== Unexpected interrupt detected =================\n";
            bshell(unit, "show interrupt");
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

/* Function to create FlexE client with uniform sequence */
int dnx_flexe_util_client_create(
    int unit,
    bcm_port_t client_id,
    flexe_port_type_t client_type,
    int speed,
    uint32 speed_flag)
{
    int i;
    int rx_speed = speed_flag == BCM_PORT_RESOURCE_ASYMMETRICAL ? client_rx_speed : speed;
    int channel_id = 0;
    int mgmt_channel = 0;
    uint32 flags = 0;
    uint32 if_flags = 0;
    bcm_port_if_t intf_type;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_client_create";

    /* Parse the FlexE client type */
    if (client_type == port_type_busB_L1)
    {
        /* BusB client for L1 switching */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_MAC;
    }
    else if (client_type == port_type_busB_L2_L3)
    {
        /* BusB client for L2/L3 switching */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_MAC;
    }
    else if (client_type == port_type_1588_L1)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 81;
    }
    else if (client_type == port_type_1588_L2)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags =  BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 81;
    }
    else if (client_type == port_type_oam_L1)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_CROSS_CONNECT | BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 80;
    }
    else if (client_type == port_type_oam_L2)
    {
        /* 1588 client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_MAC;
        channel_id = 80;
    }
    else if (client_type == port_type_busC)
    {
        /* Virtual client in BusC side */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_SAR;
    }
    else if (client_type == port_type_sar_otn)
    {
        /* SAR OTN */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_OTN_SAR;
    }
    else if (client_type == port_type_L1_monitor)
    {
        /* L1 monitor client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_STAT_ONLY | BCM_PORT_ADD_FLEXE_MAC;
    }
    else if (client_type == port_type_L2_mgmt_oam)
    {
        /* L2 OAM management client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_IF_FRAMER_MGMT | BCM_PORT_ADD_FLEXE_MAC;
        mgmt_channel = 2;
    }
    else if (client_type == port_type_L2_mgmt_ptp)
    {
        /* L2 PTP management client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_IF_FRAMER_MGMT | BCM_PORT_ADD_FLEXE_MAC;
        mgmt_channel = 3;
    }
    else if (client_type == port_type_protection)
    {
        /* BusB protection client */
        intf_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
        flags = BCM_PORT_ADD_FLEXE_MAC;
        if_flags = BCM_PORT_ADD_FLEXE_PROTECTION;
    }
    else
    {
        /* Standard FlexE client in BusA side */
        flags = 0;
        intf_type = BCM_PORT_IF_FLEXE_CLIENT;
    }

    if (dnx_flexe_gen2_support(unit, -1) && client_type == port_type_busB_L1) {
        printf("**** %s: Warning - FlexE_Gen2 doesn't support BusB_L1 client. Quit. ****\n", proc_name);
        return BCM_E_NONE;
    }
    if (!dnx_flexe_gen2_support(unit, -1) && (client_type == port_type_L1_monitor || client_type == port_type_L2_mgmt_oam || client_type == port_type_L2_mgmt_ptp || client_type == port_type_sar_otn)) {
        printf("**** %s: Warning - FlexE_Gen1 doesn't support port_type_L1_monitor/port_type_L2_mgmt_oam/port_type_L2_mgmt_ptp/port_type_sar_otn. Quit. ****\n", proc_name);
        return BCM_E_NONE;
    }

    /* Create the FlexE client */
    snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_flexe_client(unit, client_id, intf_type, channel_id, rx_speed, speed, flags, if_flags, mgmt_channel), error_msg);

    /* IPG and port enable */
    if (client_type == port_type_busB_L1 || client_type == port_type_busB_L2_L3 || client_type == port_type_1588_L2)
    {
        /* IPG setting on BusB client */
        if (!dnx_flexe_gen2_support(unit, -1)) {
            snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_ifg_set(unit, client_id, 0, BCM_PORT_DUPLEX_FULL, 88), error_msg);
        }
    }

    /* VLAN setting on BusB L2&L3 clients */
    if (client_type == port_type_busB_L2_L3 || client_type == port_type_1588_L2)
    {
        snprintf(error_msg, sizeof(error_msg), "(port %d)", client_id);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_port_tpid_cfg(unit, client_id, 0x8100, 1), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to remove FlexE client with uniform sequence */
int dnx_flexe_util_client_remove(
    int unit,
    bcm_port_t client_id)
{
    int i;
    uint32 flags = 0;
    char error_msg[200]={0,};


    /* Start to remove the FlexE client */
    snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_remove_port_full_example(unit, client_id, flags), error_msg);

    return BCM_E_NONE;
}

/* Function to set G.HAO speed */
int dnx_flexe_util_client_g_hao_speed_set(
    int unit,
    bcm_port_t client_id,
    int tx_speed,
    int rx_speed)
{
    flexe_port_type_t client_type = port_type_unknown;
    uint32 speed_flag = BCM_PORT_RESOURCE_G_HAO;
    char error_msg[200]={0,};


    /* Parse the FlexE client type */
    if (dnx_flexe_util_port_type_verify(unit, client_id, port_type_busB_L1) == BCM_E_NONE)
    {
        client_type = port_type_busB_L1;
    }
    if (dnx_flexe_util_port_type_verify(unit, client_id, port_type_busB_L2_L3) == BCM_E_NONE)
    {
        client_type = port_type_busB_L2_L3;
    }
    if (dnx_flexe_util_port_type_verify(unit, client_id, port_type_busC) == BCM_E_NONE)
    {
        client_type = port_type_busC;
    }

    /** asymetrical speed */
    if (tx_speed != rx_speed)
    {
        speed_flag |= BCM_PORT_RESOURCE_ASYMMETRICAL;
        client_rx_speed = rx_speed;
    }
    /* Set client speed */
    snprintf(error_msg, sizeof(error_msg), "(client %d)", client_id);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_speed_set(unit, client_id, tx_speed, speed_flag), error_msg);

    /* Reference code for BusB L2&L3 client */
    if (client_type == port_type_busB_L2_L3)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_tune_egr_ofp_rate_set(unit, client_id, tx_speed*1000, 1, 0), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_tune_scheduler_port_rates_set(unit, client_id, 0, tx_speed*1000, 1, 0), "");

        /** modify voq profiles, assuming local port */
        BCM_IF_ERROR_RETURN_MSG(appl_dnx_dynamic_port_speed_set(unit, 0, NULL, client_id), "");
    }
    if (client_type == port_type_busC) {
        if (*(dnxc_data_get(unit, "tdm", "params", "egress_bypass_enable", NULL)) == 0)
        {
            BCM_IF_ERROR_RETURN_MSG(dnx_tune_egr_ofp_rate_set(unit, client_id, tx_speed*1000, 1, 0), "");
        }
    }

    return BCM_E_NONE;
}

int dnx_flexe_g_hao_mux_demux_set(
    int unit_mux,
    int unit_demux,
    bcm_gport_t flexe_grp,
    int client_id,
    int rate_change_m,
    int is_rate_increase,
    int with_c_bit)
{
    int i, total_slots;
    bcm_port_flexe_group_cal_t active_cal_mux, inactive_cal_mux, active_cal_demux, inactive_cal_demux;
    bcm_port_flexe_group_cal_t
    int cal_slots[800];
    int grp_slot_speed;
    int nof_slots_to_modify;
    bcm_port_flexe_group_ghao_action_t ghao_action;
    int is_bcm_adapter = *(dnxc_data_get(unit_demux, "device", "general", "delay_exist", NULL));

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_g_hao_mux_demux_set";

    /* Compute how many slots need to be add(or remove) into(or from) the calendar */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit_mux, flexe_grp);
    nof_slots_to_modify = rate_change_m / grp_slot_speed;

    /* Get active and inactive calendars at unit_mux */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, &active_cal_mux), "(mux)");
    inactive_cal_mux = !active_cal_mux;

    /* Get active and inactive calendars at unit_demux */
    if (!is_bcm_adapter) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit_demux, flexe_grp, BCM_PORT_FLEXE_RX, &active_cal_demux), "(demux)");
    } else {
        /** At bcm_adapter, the active rx calendar isn't reliable as it can't be switched according to C bit */
        active_cal_demux = active_cal_mux;
    }
    inactive_cal_demux = !active_cal_demux;

    if (active_cal_mux != active_cal_demux) {
        print active_cal_mux;
        print active_cal_demux;
        printf("**** %s: Error, active_cal_mux != active_cal_demux ****\n", proc_name);
        return BCM_E_FAIL;
    }
    ghao_action = active_cal_mux == bcmPortFlexeGroupCalA ? bcmPortFlexeGHaoCalAToB : bcmPortFlexeGHaoCalBToA;

    /* Step 1. Set mux.inactive_cal.tx  */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, active_cal_mux, 800, cal_slots, &total_slots), "(mux)");
    for (i = 0; i < total_slots; i++) {
        if (is_rate_increase) {
            /** Find out null slot to add */
            if (cal_slots[i] == 0) {
                cal_slots[i] = client_id;
                nof_slots_to_modify--;
            }
        } else {
            /** Remove the client_id from calendar */
            if (cal_slots[i] == client_id) {
                cal_slots[i] = 0;
                nof_slots_to_modify--;
            }
        }
        if (nof_slots_to_modify == 0) {
            break;
        }
    }
    if (nof_slots_to_modify) {
        printf("**** %s: Error, No enough slot to add or remove ****\n", proc_name);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_set(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, inactive_cal_mux, total_slots, cal_slots), "(mux)");

    /* Step 2. Set demux.inactive_cal.rx  */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_set(unit_demux, flexe_grp, BCM_PORT_FLEXE_RX, inactive_cal_demux, total_slots, cal_slots), "(demux)");

    /* Step 3. mux.set_CR to inactive calendar */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalRequest, inactive_cal_mux), "(CR)");

    /* Step 4. demux.set_g_hao_action AToB/BToA and send CA */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_ghao_action_set(unit_demux, flexe_grp, BCM_PORT_FLEXE_RX, ghao_action), "(demux A/B)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_demux, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalAck, inactive_cal_mux), "(CA)");

    /* Step 5. mux.set_g_hao_action AToB/BToA and complete */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_ghao_action_set(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, ghao_action), "(mux A/B)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_ghao_action_set(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeGHaoComplete), "(mux complete)");

    /* Step 6. mux side send C bit */
    if (with_c_bit) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, inactive_cal_mux), "(C)");
    } else {
        printf("**** %s: Mux side can't send out C bit. Restoring calendar settings ****\n", proc_name);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_set(unit_mux, flexe_grp, BCM_PORT_FLEXE_TX, active_cal_mux), "");
    }

    /* Step 7. demux.set_g_hao_action 'Complete' after receiving C bit or 'Stop' without C bit */
    if (with_c_bit) {
        /** Receive C bit. Set action 'Complete' */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_ghao_action_set(unit_demux, flexe_grp, BCM_PORT_FLEXE_RX, bcmPortFlexeGHaoComplete), "(demux complete)");
    } else {
        printf("**** %s: Demux side can't receive C bit. Setting G.HAO action 'Stop' to restore settings ****\n", proc_name);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_ghao_action_set(unit_demux, flexe_grp, BCM_PORT_FLEXE_RX, bcmPortFlexeGHaoStop), "(demux complete)");
    }

    printf("**** %s: Mux/demux G.HAO setting done! ****\n", proc_name);
    return BCM_E_NONE;
}

/* Function to change FlexE datapath speed with G.HAO in direction 'unit_mux.client_src-->unit_demux.client_dst' */
int dnx_flexe_util_g_hao_datapath_speed_set(
    int unit_mux,
    bcm_gport_t flexe_grp,
    bcm_port_t client_src,
    bcm_port_t client_busa,
    int unit_demux,
    bcm_port_t client_dst,
    int old_speed,
    int new_speed)
{
    int get_tx_speed;
    int is_speed_increase;
    int tx_speed, rx_speed;
    int speed_diff;

    /*
     * |<-----  unit_mux  ----->|     |<----- unit_demux ----->|
     * client_src --> client_busa --> client_busa --> client_dst
     * --------------------------------------------------------> datapath speed
     * Replace datapath old_speed with new_speed
     */

    is_speed_increase = new_speed > old_speed;

    if (is_speed_increase) {
        /* Increase client speed: Change client speed from remote port to nearby port */

        /** Step 1. Change unit_demux.client_dst.rx_speed to new_speed(tx_speed keep unchanged) */
        get_tx_speed = 1;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_speed_get(unit_demux, client_dst, get_tx_speed, &tx_speed), "");
        rx_speed = new_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_g_hao_speed_set(unit_demux, client_dst, tx_speed, rx_speed), "(client_dst)");

        /** Step 2. Change FlexE client(client_busa) speed at both mux and demux side */
        speed_diff = new_speed - old_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_g_hao_mux_demux_set(unit_mux, unit_demux, flexe_grp, client_busa, speed_diff, is_speed_increase, 1), "");

        /** Step 3. Change unit_mux.client_src.tx_speed to new_speed(rx_speed keep unchanged) */
        get_tx_speed = 0;
        tx_speed = new_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_speed_get(unit_mux, client_src, get_tx_speed, &rx_speed), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_g_hao_speed_set(unit_mux, client_src, tx_speed, rx_speed), "(client_src)");
    } else {
        /* Decrease client speed: Change client speed from nearby port to remote port */

        /** Step 1. Set unit_mux.client_src.tx_speed to new_speed(rx_speed keep unchanged) */
        get_tx_speed = 0;
        tx_speed = new_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_speed_get(unit_mux, client_src, get_tx_speed, &rx_speed), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_g_hao_speed_set(unit_mux, client_src, tx_speed, rx_speed), "(client_src)");

        /** Step 2. Change FlexE client(client_busa) speed at both mux and demux side */
        speed_diff = old_speed - new_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_g_hao_mux_demux_set(unit_mux, unit_demux, flexe_grp, client_busa, speed_diff, is_speed_increase, 1), "");

        /** Step 3. Set unit_demux.client_dst.rx_speed to new_speed(tx_speed keep unchanged) */
        get_tx_speed = 1;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_speed_get(unit_demux, client_dst, get_tx_speed, &tx_speed), "");
        rx_speed = new_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_g_hao_speed_set(unit_demux, client_dst, tx_speed, rx_speed), "(client_dst)");
    }
    printf("====> FlexE datapath speed change done with G.HAO! Direction: UNIT %d->UNIT %d. Old_speed %d->New_speed %d\n", unit_mux, unit_demux, old_speed, new_speed);

    return BCM_E_NONE;
}

/* Function to create common FlexE L1 datapath on FlexE_Gen2 
 * datapath: flexe_phy <-->  client_a <--> client_c <--> dst_port(local or remote)
 * dst_port can be ILKN_L1 or SAR client
 */
int dnx_flexe_gen2_common_L1_datapath_main(
    int unit,
    int grp_index,
    int grp_speed,
    int client_speed,
    bcm_port_t flexe_phy,
    int flexe_phy_loopback,
    int dst_unit,
    int dst_port)
{
    int i;
    int nof_pcs = 1;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_c = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    int client_slots;
    bcm_gport_t mod_gport;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_common_L1_datapath_main";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create FlexE group */
    if (flexe_phy > 0) {
        /* Valid PHY port ID. Use it for FlexE group creation */
        flexe_phy_ports[0] = flexe_phy;
    } else {
        /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 16; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
        if (!nof_pcs) {
            printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        }
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Compute how many slots should be used */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    client_slots = client_speed / grp_slot_speed;

    /* Create busA client and busC client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0),
        "fail to create busA client");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed, 0),
        "fail to create busC client");

    /***************************************************************************
     * FlexE datapath:
     *                         Framer
     *                +------------------------+
     *                |                        |
     * flexe_phy <--> | client_a <--> client_c | <--> dst_port(ilkn_L1/sar_client, can be local or remote)
     *                |                        |
     *                +------------------------+
     *
     ***************************************************************************/

    /* Build L1 datapath: client_a --> client_c --> dst_port(ilkn_L1/sar_client, can be local or remote) */
    if (dst_unit == unit) {
        /** SAR to local port */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a, client_c, dst_port, 0), "");
    } else {
        /** SAR to remote port */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(dst_unit, dst_port, &mod_gport), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a, client_c, mod_gport, 0), "");
    }

    /* Build L1 datapath: dst_port --> client_c --> client_a */
    flexe_gen2_tdm_grp_id += flexe_gen2_tdm_is_mc;
    if (dst_unit == unit) {
        /** to local SAR */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(dst_unit, dst_port, client_c, client_a, 0), "");
    } else {
        /** to remote SAR */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit, client_c, &mod_gport), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(dst_unit, dst_port, mod_gport, client_a, 0), "");
    }

    /* Loopback FlexE PHY */
    if (flexe_phy_loopback) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
        for (i = 0; i < nof_pcs; i++) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, grp_phy_info[i].port, BCM_PORT_LOOPBACK_PHY), "");
        }
        /* Wait for PHY(s) linkup. The purpose is to speedup dnx_flexe_util_calendar_slots_set() */
        bshell(unit,"sleep 2");
    }

    /* Assign calendar slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Set group ID in overhead */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

/* Function to destroy common FlexE L1 datapath on FlexE_Gen2 */
int dnx_flexe_gen2_common_L1_datapath_destroy(
    int unit,
    int grp_index,
    int grp_speed,
    int dst_unit,
    int dst_port)
{
    int i;
    int nof_pcs;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_c = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    bcm_port_t flexe_phy;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_common_L1_datapath_destroy";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Clear FlexE datapath:
     *                         Framer
     *                +------------------------+
     *                |                        |
     * flexe_phy <--> | client_a <--> client_c | <--> dst_port(ilkn_L1/sar_client, can be local or remote)
     *                |                        |
     *                +------------------------+
     *
     ***************************************************************************/

    /* Compute how many slots are used */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;

    /* Clear FlexE calendar */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Clear L1 datapath: client_a --> client_c --> dst_port(ilkn_L1/sar_client, can be local or remote) */
    flexe_gen2_tdm_grp_id -= flexe_gen2_tdm_is_mc;
    if (dst_unit == unit) {
        /** SAR to local port */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a, client_c, dst_port, 1), "");
    } else {
        /** SAR to remote port */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(dst_unit, dst_port, &mod_gport), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a, client_c, mod_gport, 1), "");
    }

    /* Clear L1 datapath: dst_port --> client_c --> client_a */
    flexe_gen2_tdm_grp_id += flexe_gen2_tdm_is_mc;
    if (dst_unit == unit) {
        /** to local SAR */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(dst_unit, dst_port, client_c, client_a, 1), "");
    } else {
        /** to remote SAR */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit, client_c, &mod_gport), "");
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(dst_unit, dst_port, mod_gport, client_a, 1), "");
    }

    /* Remove busA client and busB client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_a), "fail to remove busA client");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_c), "fail to remove busB client");

    /* Disable Loopback on FlexE PHY(s) */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
    for (i = 0; i < nof_pcs; i++) {
        flexe_phy = grp_phy_info[i].port;
        snprintf(error_msg, sizeof(error_msg), "(flexe_phy %d)", flexe_phy);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_NONE), error_msg);
    }

    /* Clear FlexE group */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp), "");

    return BCM_E_NONE;
}

/* Function to create common FlexE L2 datapath on FlexE_Gen2 
 * datapath: flexe_phy <--> client_a <--> client_b <--> dst_port(local or remote)
 * dst_port can be eth_L2, ilkn_L2 or busB_client
 */
int dnx_flexe_gen2_common_L2_datapath_main(
    int unit,
    int grp_index,
    int grp_speed,
    int client_speed,
    bcm_port_t flexe_phy,
    int flexe_phy_loopback,
    int dst_unit,
    int dst_port)
{
    int i;
    int nof_pcs = 1;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    int client_slots;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_common_L2_datapath_main";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create FlexE group */
    if (flexe_phy > 0) {
        /* Valid PHY port ID. Use it for FlexE group creation */
        flexe_phy_ports[0] = flexe_phy;
    } else {
        /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 16; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
        if (!nof_pcs) {
            printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        }
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Compute how many slots should be used */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    client_slots = client_speed / grp_slot_speed;

    /* Create busA client and busB client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0),
        "fail to create busA client");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB, client_speed, 0),
        "fail to create busB client");

    /***************************************************************************
     * FlexE datapath:
     *                         Framer
     *                +------------------------+
     *                |                        |
     * flexe_phy <--> | client_a <--> client_b | <--> dst_port(eth_L2/ilkn_L2/busB_client, can be local or remote)
     *                |                        |
     *                +------------------------+
     *
     ***************************************************************************/

    /* Build L2 datapath: client_a --> client_b --> dst_port(eth/ilkn/busB_client, can be local or remote) */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a, client_b, dst_unit, dst_port, 0), "");

    /* Build L2 datapath: dst_port --> client_b --> client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_src_to_flexe(dst_unit, dst_port, unit, client_b, client_a, 0), "");

    /* Loopback FlexE PHY */
    if (flexe_phy_loopback) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
        for (i = 0; i < nof_pcs; i++) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, grp_phy_info[i].port, BCM_PORT_LOOPBACK_PHY), "");
        }
        /* Wait for PHY(s) linkup. The purpose is to speedup dnx_flexe_util_calendar_slots_set() */
        bshell(unit,"sleep 2");
    }
 
    /* Assign calendar slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Set group ID in overhead */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

/* Function to destroy common FlexE L2 datapath on FlexE_Gen2 */
int dnx_flexe_gen2_common_L2_datapath_destroy(
    int unit,
    int grp_index,
    int grp_speed,
    int dst_unit,
    int dst_port)
{
    int i;
    int nof_pcs;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    bcm_port_t flexe_phy;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_common_L2_datapath_destroy";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Clear FlexE datapath:
     *                         Framer
     *                +------------------------+
     *                |                        |
     * flexe_phy <--> | client_a <--> client_b | <--> dst_port(eth_L2/ilkn_L2/busB_client, can be local or remote)
     *                |                        |
     *                +------------------------+
     *
     ***************************************************************************/

    /* Compute how many slots are used */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;

    /* Clear FlexE calendar */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Clear L2 datapath: client_a --> client_b --> dst_port(eth/ilkn/busB_client, can be local or remote) */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a, client_b, dst_unit, dst_port, 1), "");

    /* Clear L2 datapath: dst_port --> client_b --> client_a */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_src_to_flexe(dst_unit, dst_port, unit, client_b, client_a, 1), "");

    /* Remove busA client and busB client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_a), "fail to remove busA client");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b), "fail to remove busB client");

    /* Disable Loopback on FlexE PHY(s) */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
    for (i = 0; i < nof_pcs; i++) {
        flexe_phy = grp_phy_info[i].port;
        snprintf(error_msg, sizeof(error_msg), "(flexe_phy %d)", flexe_phy);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_NONE), error_msg);
    }

    /* Clear FlexE group */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp), "");

    return BCM_E_NONE;
}

int dnx_flexe_gen2_common_mix_L1_L2_clients(
    int unit,
    int grp_index,
    int grp_speed,
    int client_speed,
    bcm_port_t flexe_phy,
    int flexe_phy_loopback,
    int client_1_is_L1,
    int client_2_is_L1,
    int dst_port_1,
    int dst_port_2)
{
    int i;
    int nof_pcs = 1;
    bcm_port_t client_a_1 = 30 + grp_index;
    bcm_port_t client_a_2 = 40 + grp_index;
    bcm_port_t client_b_1 = 50 + grp_index;
    bcm_port_t client_b_2 = 60 + grp_index;
    bcm_port_t client_c_1 = 70 + grp_index;
    bcm_port_t client_c_2 = 80 + grp_index;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    int client_slots;
    bcm_gport_t mod_gport;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_common_mix_L1_L2_clients";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create FlexE group */
    if (flexe_phy > 0) {
        /* Valid PHY port ID. Use it for FlexE group creation */
        flexe_phy_ports[0] = flexe_phy;
    } else {
        /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 16; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
        if (!nof_pcs) {
            printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        }
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Compute how many slots should be used */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    client_slots = client_speed / grp_slot_speed;

    /* Create busA/B/C clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_1, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_2, port_type_busA, client_speed, 0), "");
    if (client_1_is_L1) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c_1, port_type_busC, client_speed, 0), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_1, port_type_busB, client_speed, 0), "");
    }
    if (client_2_is_L1) {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c_2, port_type_busC, client_speed, 0), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_2, port_type_busB, client_speed, 0), "");
    }

    /***************************************************************************
     * FlexE datapath:
     *                                Framer
     *                   +--------------------------------------------+
     *                   |                                            |
     *                   | client_a_1 <--> client_b_1/client_c_1 <--> | <--> dst_port_1(local TMAC/ETH_L2 or sar/ILKN_TDM)
     * flexe_phy(s) <--> |                                            |
     *                   | client_a_2 <--> client_b_2/client_c_2 <--> | <--> dst_port_2(local TMAC/ETH_L2 or sar/ILKN_TDM)
     *                   +--------------------------------------------+
     *
     ***************************************************************************/

    if (client_1_is_L1) {
        /* Build L1 datapath: client_a_1 --> client_c_1 --> dst_port_1(local sar or ILKN_TDM) */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a_1, client_c_1, dst_port_1, 0), "");
        /* Build L1 datapath: dst_port_1 --> client_c_1 --> client_a_1 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, dst_port_1, client_c_1, client_a_1, 0), "");
    } else {
        /* Build L2 datapath: client_a_1 --> client_b_1 --> dst_port_1(local TMAC or ETH_L2) */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a_1, client_b_1, unit, dst_port_1, 0), "");
        /* Build L2 datapath: dst_port_1 --> client_b_1 --> client_a_1 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_src_to_flexe(unit, dst_port_1, unit, client_b_1, client_a_1, 0), "");
    }

    if (client_2_is_L1) {
        /* Build L1 datapath: client_a_2 --> client_c_2 --> dst_port_2(local sar or ILKN_TDM) */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a_2, client_c_2, dst_port_2, 0), "");
        /* Build L1 datapath: dst_port_2 --> client_c_2 --> client_a_2 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, dst_port_2, client_c_2, client_a_2, 0), "");
    } else {
        /* Build L2 datapath: client_a_2 --> client_b_2 --> dst_port_2(local TMAC or ETH_L2) */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a_2, client_b_2, unit, dst_port_2, 0), "");
        /* Build L2 datapath: dst_port_2 --> client_b_2 --> client_a_2 */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_src_to_flexe(unit, dst_port_2, unit, client_b_2, client_a_2, 0), "");
    }

    /* Loopback FlexE PHY */
    if (flexe_phy_loopback) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
        for (i = 0; i < nof_pcs; i++) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, grp_phy_info[i].port, BCM_PORT_LOOPBACK_PHY), "");
        }
        /* Wait for PHY(s) linkup. The purpose is to speedup dnx_flexe_util_calendar_slots_set() */
        bshell(unit,"sleep 2");
    }

    /* Assign calendar slots for BusA clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_slots * 2; i++) {
        Cal_Slots_A[i] = i < client_slots ? client_a_1 : client_a_2;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Set group ID in overhead */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

int dnx_flexe_gen2_mix_datapaths_in_1_grp(
    int unit1,
    int unit2,
    int grp_speed,
    int client_speed,
    int flexe_phy,
    int dapath_bmp)
{
    int i, ii, unit_iter, unit, dst_unit, dst_port, valid_datapaths;
    bcm_gport_t flexe_grp;
    int grp_index = 0;
    int nof_pcs = 0;
    int max_datapaths = 7;
    int client_a[7] = {30, 31, 32, 33, 34, 35, 36};
    int client_b[7] = {40, 41, 42, 43, 44, 45, 46};
    int client_c[7] = {0,  0,  52, 53, 0,  55, 56};
    int valid_client_a[7];
    int eth_tx = 1;
    int eth_rx = 2;
    int eth_L2 = 3;
    int eth_L1 = 4;
    int ilkn_tdm = 5;
    int grp_slot_speed;
    int total_slots;
    int client_slots;
    bcm_gport_t mod_gport;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit1, -1);
    bcm_mac_t dst_mac = {0x00,0x00,0x00,0x00,0x00,0xDA};
    char error_msg[200]={0,};
    bcm_port_resource_t resource_get;
    int eth_l1_speed;
    int slot_index;

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_mix_datapaths_in_1_grp";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /************************************************************************************************************
     * Test datapath:
     *
     *                             unit1                                            unit2
     *               +------------------------------+                  +------------------------------+
     *               |                              |                  |                              |
     * eth_tx -->+-->| client_b[0] <--> client_a[0] | <-flexe_phy(s)-> | client_a[0] <--> client_b[0] | <--> unit2.eth_L2(to local port, loopback)
     *           |-->| client_b[1] <--> client_a[1] | <-flexe_phy(s)-> | client_a[1] <--> client_b[1] | <--> unit1.eth_L2(to remote port, loopback)
     *           |-->| client_b[2] <--> client_a[2] | <-flexe_phy(s)-> | client_a[2] <--> client_c[2] | <--> unit2.ilkn_tdm(to local port, loopback)
     *           |-->| client_b[3] <--> client_a[3] | <-flexe_phy(s)-> | client_a[3] <--> client_c[3] | <--> unit1.ilkn_tdm(to remote port, loopback)
     *           |-->| client_b[4] <--> client_a[4] | <-flexe_phy(s)-> | client_a[4] <-->             | <--> unit2.eth_L1(to local ETH_L1,loopback)
     *           |-->| client_b[5] <--> client_a[5] | <-flexe_phy(s)-> | client_a[5] <--> client_c[5] | <--> unit1.client_c[5] <--> unit1.eth_L1(to remote ETH_L1, loopback)
     *           +-->| client_b[6] <--> client_a[6] | <-flexe_phy(s)-> | client_a[6] <--> client_c[6] | (to local SAR, TDM loopback)
     *               |                              |                  |                              |
     *               +------------------------------+                  +------------------------------+
     *
     * Traffic at unit1:
     * eth_tx --> client_b_x: L2 lookup     (Each client has 80G traffic loading. Total 400G Tx traffic)
     * client_b_x --> eth_rx: Force forward (Each client has 80G traffic loading. Total 400G Rx traffic)
     *************************************************************************************************************/

    if (dapath_bmp & 0x10 || dapath_bmp & 0x20) {
        /* ETH_L1 and BusA/BusC client should have same speed 
         * Get ETH_L1.speed and use it for FlexE client creation
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit2, eth_L1, &resource_get), "");
        eth_l1_speed = resource_get.speed;
    }

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit2 : unit1;

        /* Create FlexE group */
        if (flexe_phy > 0) {
            /* Valid PHY port ID. Use it for FlexE group creation */
            flexe_phy_ports[0] = flexe_phy;
            nof_pcs = 1;
        } else {
            /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
            nof_pcs = 0;
            for (i = 0; i < 16; i++) {
                if (flexe_phy_ports[i]) {
                    nof_pcs++;
                } else {
                    break;
                }
            }
            if (!nof_pcs) {
                printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), error_msg);

        /* Create FlexE clients */
        valid_datapaths = 0;
        for (i = 0; i < max_datapaths; i++) {
            if ((dapath_bmp & (1 << i)) == 0) {continue;}

            /** Create BusA clients */
            snprintf(error_msg, sizeof(error_msg), "(BusA, unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a[i], port_type_busA, client_speed, 0), error_msg);
            valid_client_a[valid_datapaths] = client_a[i];
            valid_datapaths++;

            /** Create BusB clients */
            if (unit == unit1 || (unit == unit2 && i < 2)) {
                snprintf(error_msg, sizeof(error_msg), "(BusB, unit %d, index %d)", unit, i);
                if (i == 4 || i ==5) {
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b[i], port_type_busB, eth_l1_speed, 0), error_msg);
                } else {
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b[i], port_type_busB, client_speed, 0), error_msg);
                }
            }

            /** Create BusC clients */
            if ( (unit == unit2 && client_c[i] > 0) || (unit == unit1 && i == 5) ) {
                snprintf(error_msg, sizeof(error_msg), "(BusC, unit %d, index %d)", unit, i);
                if (i == 4 || i ==5) {
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c[i], port_type_busC, eth_l1_speed, 0), error_msg);
                } else {
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c[i], port_type_busC, client_speed, 0), error_msg);
                }
            }
        }

        /* Start to create each datapath */
        for (i = 0; i < max_datapaths; i ++) {
            if ((dapath_bmp & (1 << i)) == 0) {continue;}
            printf("====> Creating datapath %d at unit %d\n", i, unit);

            if (unit == unit1) {
                /** unit1: All are L2 datapaths. Used to inject traffic
                 *  TX: eth_tx -> L2_Lookup -> client_b[i] -> client_a[i] ->
                 *  Rx: client_a[i] -> client_b[i] -> Force_Forward -> eth_rx
                 */

                /** TX: eth_tx -> L2_Lookup -> client_b[i]: L2 lookup(DMAC=00:00:00:00:00:DA, VID=client_b[i]) */
                snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_advanced_set(unit, eth_tx, client_b[i], dst_mac, 1, client_b[i], 1), error_msg);

                /** TX: client_b[i] -> client_a[i] */
                snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b[i], 0, client_a[i]), error_msg);

                /** RX: client_a[i] -> client_b[i] -> Force_Forward -> eth_rx */
                snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a[i], client_b[i], unit, eth_rx, 0), error_msg);
            } else {
                /** unit2: Mixed L1 and L2 datapaths */

                snprintf(error_msg, sizeof(error_msg), "fail to set FlexE datapath at unit2(index %d)", i);
                if (i < 2) {
                    /** L2 datapath: client_a[i] <--> client_b[i] <--> eth_L2(local or remote)
                     *  i=0, local port; i=1, remote port
                     */
                    dst_unit = i == 0 ? unit2 : unit1;
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a[i], client_b[i], dst_unit, eth_L2, 0), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_src_to_flexe(dst_unit, eth_L2, unit, client_b[i], client_a[i], 0), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(dst_unit, eth_L2, BCM_PORT_LOOPBACK_MAC), error_msg);
                } else if (i < 4) {
                    /** L1 datapath: client_a[i] -> client_c[i] -> ilkn_tdm(local or remote)
                     *  i=2, local eth; i=3, remote eth
                     */
                    dst_unit = i == 2 ? unit2 : unit1;
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(dst_unit, ilkn_tdm, &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a[i], client_c[i], mod_gport, 0), error_msg);

                    /** L1 datapath: client_a[i] <- client_c[i] <- dst_unit.ilkn_tdm */
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit, client_c[i], &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(dst_unit, ilkn_tdm, mod_gport, client_a[i], 0), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(dst_unit, ilkn_tdm, BCM_PORT_LOOPBACK_MAC), error_msg);
                } else if (i == 4) {
                    /** L1 datapath: client_a[i] <-> eth_L1(local) */
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a[i], 0, eth_L1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_L1, 0, client_a[i]), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_L1, BCM_PORT_LOOPBACK_PHY), error_msg);
                } else if (i == 5) {
                    /** L1 datapath: unit2.client_a[5] <--> unit2.client_c[5] <--> unit1.client_c[5] <--> unit1.eth_L1(to remote ETH_L1, and loopback) */
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit1, client_c[i], &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit2, client_a[i], client_c[i], mod_gport, 0), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit2, client_c[i], &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit1, client_c[i], mod_gport, client_a[i], 0), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit1, 0, client_c[i], 0, eth_L1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit1, 0, eth_L1, 0, client_c[i]), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit1, eth_L1, BCM_PORT_LOOPBACK_PHY), error_msg);
                } else if (i == 6) {
                    /** L1 datapath: client_a[6] <--> client_c[6](TDM loopback) */
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a[i], client_c[i], client_c[i], 0), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, client_c[i], client_c[i], client_a[i], 0), error_msg);
                }
            }
            printf("====> Datapath %d creation done at unit %d\n", i, unit);
        }

        /* Compute how many slots should be used */
        grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
        if (grp_slot_speed <= 0) {
            printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
            return BCM_E_FAIL;
        }
        total_slots = grp_speed / grp_slot_speed;

        /* Assign calendar slots for BusA clients */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        slot_index = 0;
        for (i = 0; i < max_datapaths; i++) {
            if ((dapath_bmp & (1 << i)) == 0) {continue;}

            if (i == 4 || i ==5) {
                client_slots = eth_l1_speed / grp_slot_speed;
            } else {
                client_slots = client_speed / grp_slot_speed;
            }

            for (ii = 0; ii < client_slots; ii++) {
                Cal_Slots_A[slot_index++] = client_a[i];
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

        /* Set group ID in overhead */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1), error_msg);
    }

    return BCM_E_NONE;
}

int dnx_flexe_gen2_mix_datapaths_in_1_grp_clear(
    int unit1,
    int unit2,
    int grp_speed,
    int dapath_bmp)
{
    int i, unit_iter, unit, dst_unit, dst_port;
    bcm_gport_t flexe_grp;
    int grp_index = 0;
    int max_datapaths = 7;
    int client_a[7] = {30, 31, 32, 33, 34, 35, 36};
    int client_b[7] = {40, 41, 42, 43, 44, 45, 46};
    int client_c[7] = {0,  0,  52, 53, 0,  55, 56};
    int eth_tx = 1;
    int eth_rx = 2;
    int eth_L2      = 3;
    int eth_L1      = 4;
    int ilkn_tdm    = 5;
    int grp_slot_speed;
    int total_slots;
    bcm_gport_t mod_gport;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit1, -1);
    char error_msg[200]={0,};

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_mix_datapaths_in_1_grp_clear";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /************************************************************************************************************
     * Test datapath:
     *
     *                             unit1                                            unit2
     *               +------------------------------+                  +------------------------------+
     *               |                              |                  |                              |
     * eth_tx -->+-->| client_b[0] <--> client_a[0] | <-flexe_phy(s)-> | client_a[0] <--> client_b[0] | <--> unit2.eth_L2(to local port, loopback)
     *           |-->| client_b[1] <--> client_a[1] | <-flexe_phy(s)-> | client_a[1] <--> client_b[1] | <--> unit1.eth_L2(to remote port, loopback)
     *           |-->| client_b[2] <--> client_a[2] | <-flexe_phy(s)-> | client_a[2] <--> client_c[2] | <--> unit2.ilkn_tdm(to local port, loopback)
     *           |-->| client_b[3] <--> client_a[3] | <-flexe_phy(s)-> | client_a[3] <--> client_c[3] | <--> unit1.ilkn_tdm(to remote port, loopback)
     *           |-->| client_b[4] <--> client_a[4] | <-flexe_phy(s)-> | client_a[4] <-->             | <--> unit2.eth_L1(to local ETH_L1,loopback)
     *           |-->| client_b[5] <--> client_a[5] | <-flexe_phy(s)-> | client_a[5] <--> client_c[5] | <--> unit1.client_c[5] <--> unit1.eth_L1(to remote ETH_L1, loopback)
     *           +-->| client_b[6] <--> client_a[6] | <-flexe_phy(s)-> | client_a[6] <--> client_c[6] | (to local SAR, TDM loopback)
     *               |                              |                  |                              |
     *               +------------------------------+                  +------------------------------+
     *
     * Traffic at unit1:
     * eth_tx --> client_b_x: L2 lookup     (Each client has 80G traffic loading. Total 400G Tx traffic)
     * client_b_x --> eth_rx: Force forward (Each client has 80G traffic loading. Total 400G Rx traffic)
     *************************************************************************************************************/

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit2 : unit1;

        /* Clear calendar */
        grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
        if (grp_slot_speed <= 0) {
            printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
            return BCM_E_FAIL;
        }
        total_slots = grp_speed / grp_slot_speed;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), error_msg);

        /* Start to clear each datapath */
        for (i = 0; i < max_datapaths; i ++) {
            if ((dapath_bmp & (1 << i)) == 0) {continue;}

            if (unit == unit1) {
                /** unit1: All are L2 datapaths. Used to inject traffic
                 *  TX: eth_tx -> L2_Lookup -> client_b[i] -> client_a[i] ->
                 *  Rx: client_a[i] -> client_b[i] -> Force_Forward -> eth_rx
                 */

                /** TX: eth_tx -> L2_Lookup -> client_b[i]: L2 lookup(DMAC=00:00:00:00:22:22, VID=client_b[i])
                 *  The L2 settings will be cleared in TCL shell
                 */

                /** TX: client_b[i] -> client_a[i] */
                snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b[i], 0, client_a[i]), error_msg);

                /** RX: client_a[i] -> client_b[i] -> Force_Forward -> eth_rx */
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a[i], client_b[i], unit, eth_rx, 1), error_msg);
            } else {
                /** unit2: Mixed L1 and L2 datapaths */

                snprintf(error_msg, sizeof(error_msg), "fail to clear FlexE datapath at unit2(index %d)", i);
                if (i < 2) {
                    /** L2 datapath: client_a[i] <--> client_b[i] <--> eth_L2(local or remote)
                     *  i=0, local port; i=1, remote port
                     */
                    dst_unit = i == 0 ? unit2 : unit1;
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_flexe_to_dst(unit, client_a[i], client_b[i], dst_unit, eth_L2, 1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_datapath_src_to_flexe(dst_unit, eth_L2, unit, client_b[i], client_a[i], 1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(dst_unit, eth_L2, BCM_PORT_LOOPBACK_NONE), error_msg);
                } else if (i < 4) {
                    /** L1 datapath: client_a[i] -> client_c[i] -> ilkn_tdm(local or remote)
                     *  i=2, local eth; i=3, remote eth
                     */
                    dst_unit = i == 2 ? unit2 : unit1;
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(dst_unit, ilkn_tdm, &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a[i], client_c[i], mod_gport, 1), error_msg);

                    /** L1 datapath: client_a[i] <- client_c[i] <- dst_unit.ilkn_tdm */
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit, client_c[i], &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(dst_unit, ilkn_tdm, mod_gport, client_a[i], 1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(dst_unit, ilkn_tdm, BCM_PORT_LOOPBACK_NONE), error_msg);
                } else if (i == 4) {
                    /** L1 datapath: client_a[i] <-> eth_L1 */
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_a[i], 0, eth_L1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, eth_L1, 0, client_a[i]), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_L1, BCM_PORT_LOOPBACK_NONE), error_msg);
                } else if (i == 5) {
                    /** L1 datapath: unit2.client_a[5] <--> unit2.client_c[5] <--> unit1.client_c[5] <--> unit1.eth_L1(to remote ETH_L1, and loopback) */
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit1, client_c[i], &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit2, client_a[i], client_c[i], mod_gport, 1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_mod_port_get(unit2, client_c[i], &mod_gport), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit1, client_c[i], mod_gport, client_a[i], 1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit1, client_c[i], 0, eth_L1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit1, eth_L1, 0, client_c[i]), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit1, eth_L1, BCM_PORT_LOOPBACK_NONE), error_msg);
                } else if (i == 6) {
                    /** L1 datapath: client_a[6] <--> client_c[6](TDM loopback) */
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_flexe_sar_dst(unit, client_a[i], client_c[i], client_c[i], 1), error_msg);
                    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_src_sar_flexe(unit, client_c[i], client_c[i], client_a[i], 1), error_msg);
                }
            }
        }

        /* Remove FlexE clients */
        for (i = 0; i < max_datapaths; i++) {
            if ((dapath_bmp & (1 << i)) == 0) {continue;}

            /** Remove BusA clients */
            snprintf(error_msg, sizeof(error_msg), "(BusA, unit %d, index %d)", unit, i);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_a[i]), error_msg);

            /** Remove BusB clients */
            if (unit == unit1 || (unit == unit2 && i < 2)) {
                snprintf(error_msg, sizeof(error_msg), "(BusB, unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b[i]), error_msg);
            }

            /** Remove BusC clients */
            if ( (unit == unit2 && client_c[i] > 0) || (unit == unit1 && i == 5) ) {
                snprintf(error_msg, sizeof(error_msg), "(BusC, unit %d, index %d)", unit, i);
                BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_c[i]), error_msg);
            }
        }

        /* Destroy FlexE group */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to create L2 snake with specific number of FlexE clients and TMAC clients */
int dnx_flexe_gen2_L2_snake_main(
    int unit,
    int grp_index,
    int grp_speed,
    int client_speed,
    int nof_clients,
    bcm_port_t eth_tx,
    bcm_port_t eth_rx,
    int client_a_start,
    int client_b_start)
{
    int i;
    int nof_pcs;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    int client_slots;
    int total_client_slots;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int client_a_ids[320];
    int client_b_ids[320];
    int client_id, nof_clients_added;
    int reserved_port = 255; /* PP-DSP 255 is reserved */
    int reserved_port_1 = 218; /* Reserved for SAT */
    int base_vid = 1111;
    int in_vid, out_vid;
    bcm_mac_t mac_da = {0x00,0x00,0x00,0x00,0x11,0x11};
    char error_msg[200]={0,};

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_L2_snake_main";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create FlexE group with PHYs in array flexe_phy_ports[] */
    nof_pcs = 0;
    for (i = 0; i < 16; i++) {
        if (flexe_phy_ports[i]) {
            nof_pcs++;
        } else {
            break;
        }
    }
    if (!nof_pcs) {
        printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Compute how many slots should be used */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    client_slots = client_speed / grp_slot_speed;
    total_client_slots = client_slots * nof_clients;

    /* Create busA clients */
    i = 0;
    while (i < nof_clients) {
        client_id = client_a_start + i;
        snprintf(error_msg, sizeof(error_msg), "fail to create busA client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_id, port_type_busA, client_speed, 0), error_msg);
        client_a_ids[i] = client_id;
        i++;
    }

    /* Create busB clients */
    i = 0;
    nof_clients_added = 0;
    while (nof_clients_added < nof_clients) {
        client_id = client_b_start + i;
        if (client_id == reserved_port || client_id == reserved_port_1) {
            i++;
            continue;
        }
        snprintf(error_msg, sizeof(error_msg), "fail to create busB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_id, port_type_busB, client_speed, 0), error_msg);
        client_b_ids[nof_clients_added] = client_id;
        nof_clients_added++;
        i++;
    }

    /***************************************************************************
     * FlexE datapath:
     *
     *                +---------------------------+
     *                |                           |
     *  FlexE_PHY<--->| BusA_0 <--> BusB_0<-------|<--eth_tx
     *  (loopback)    |               |           |
     *                |               V           |
     *  FlexE_PHY<--->| BusA_1 <--> BusB_1        |
     *  (loopback)    |               |           |
     *                |               V           |
     *  FlexE_PHY<--->| BusA_2 <--> BusB_2        |
     *  (loopback)    |               |           |
     *                |               V           |
     *     .....      | .....  <--> ......        |
     *                |               |           |
     *                |               V           |
     *  FlexE_PHY<--->| BusA_N <--> BusB_N------->|-->eth_rx
     * (loopback)     |                           |
     *                +---------------------------+
     *
     ***************************************************************************/

    /* Build FlexE flows: client_a_x <--> client_b_x */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_ids[i], 0, client_a_ids[i]), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_ids[i], 0, client_b_ids[i]), error_msg);
    }

    /* Hook BusB clients: BusB_X -->L2--> BusB_X+1
     * PKT_In(MAC_DA+VID_X) --> BusB_X --> L2_lookup --> BusB_X --> PKT_Out(MAC_DA+VID_X+1)
     */
    for (i = 0; i < nof_clients; i++) {
        if (i == nof_clients-1) {continue;}

        /** VID translation at BusB_X and BusB_X+1 */
        in_vid = base_vid + i;
        out_vid = base_vid + i + 1;
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(egr_vlan_edit(unit, client_b_ids[i], client_b_ids[i+1], in_vid, out_vid), error_msg);

        /** Add L2 entry */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L2_entry_advanced_set(unit, client_b_ids[i], client_b_ids[i+1], mac_da, 1, in_vid, 0), error_msg);
    }

    /* Hook eth ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b_ids[0], 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_ids[nof_clients-1], eth_rx, 1), "");

    /* Loopback FlexE PHY */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
    for (i = 0; i < nof_pcs; i++) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, grp_phy_info[i].port, BCM_PORT_LOOPBACK_PHY), "");
    }
    /* Wait for PHY(s) linkup. The purpose is to speedup dnx_flexe_util_calendar_slots_set() */
    bshell(unit,"sleep 2");

    /* Assign calendar slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < total_slots; i++) {
        if (i >= total_client_slots) {
            /** All FlexE clients have allocated slot(s) */
            break;
        }

        client_id = client_a_ids[i / client_slots];
        Cal_Slots_A[i] = client_id;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Set group ID in overhead */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

/* Function to destroy all settings in dnx_flexe_gen2_L2_snake_main() */
int dnx_flexe_gen2_L2_snake_destroy(
    int unit,
    int grp_index,
    int grp_speed,
    int nof_clients,
    int client_a_start,
    int client_b_start)
{
    int i;
    int nof_pcs;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    bcm_port_t flexe_phy;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int reserved_port = 255; /* PP-DSP 255 is reserved */
    int reserved_port_1 = 218; /* Reserved for SAT */
    int client_a_id, client_b_id;
    int client_a_iter = 0;
    int client_b_iter = 0;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_L2_snake_destroy";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Compute how many slots are used */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;

    /* Clear FlexE calendar */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Clear FlexE flows and remove clients */
    for (client_a_iter = 0; client_a_iter < nof_clients; client_a_iter++) {
        client_a_id = client_a_start + client_a_iter;
        client_b_id = client_b_start + client_b_iter;
        if (client_b_id == reserved_port || client_b_id == reserved_port_1) {
            client_b_iter++;
            client_b_id = client_b_start + client_b_iter;
        }

        /* Clear FlexE flows */
        snprintf(error_msg, sizeof(error_msg), "(index %d)", client_a_iter);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_id, 0, client_a_id), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_a_id, 0, client_b_id), error_msg);

        /* Remove busA client and busB client */
        snprintf(error_msg, sizeof(error_msg), "fail to remove busA client(index %d)", client_a_iter);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_a_id), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fail to remove busB client(index %d)", client_a_iter);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_id), error_msg);
        client_b_iter++;
    }

    /* Disable Loopback on FlexE PHY(s) */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
    for (i = 0; i < nof_pcs; i++) {
        flexe_phy = grp_phy_info[i].port;
        snprintf(error_msg, sizeof(error_msg), "(flexe_phy %d)", flexe_phy);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_NONE), error_msg);
    }

    /* Clear FlexE group */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit, flexe_grp), "");

    /* Clear all L2 entries */
    bshell(unit, "l2 clear all");

    return BCM_E_NONE;
}

/* Function to create L1 snake with specific number of FlexE clients and SAR clients */
int dnx_flexe_gen2_L1_snake_main(
    int unit,
    int client_speed,
    int nof_clients,
    int eth_L1,
    int client_c_start)
{
    int i;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int client_c_ids[320];
    int client_id, nof_clients_added;
    int reserved_port = 255; /* PP-DSP 255 is reserved */
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_L1_snake_main";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    if (nof_clients % 2) {
        printf("**** %s: Error - nof_clients should be an even number ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create busC clients */
    i = 0;
    nof_clients_added = 0;
    while (nof_clients_added < nof_clients) {
        client_id = client_c_start + i;
        if (client_id == reserved_port) {
            i++;
            continue;
        }
        snprintf(error_msg, sizeof(error_msg), "fail to create busC client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_id, port_type_busC, client_speed, 0), error_msg);
        client_c_ids[nof_clients_added] = client_id;
        nof_clients_added++;
        i++;
    }

    /***************************************************************************
     * FlexE datapath:
     *
     *                +-------------------------------------------------------------+
     *                |                                                             |
     *                | (TDM_Loopback)BusC_0<---------------------------------------|<--eth_L1
     *                |                 |                                       +-->|-->
     *                |                 +-------------->BusC_1(Inner switch)    ^   |
     *                |                                    |                    |   |
     *                | (TDM_Loopback)BusC_2<--------------+                    |   |
     *                |                 |                                       |   |
     *                |                 +-------------->BusC_3(Inner switch)    |   |
     *                |                                    |                    |   |
     *                | (TDM_Loopback)BusC_4<--------------+                    |   |
     *                |                 |                                       |   |
     *                |                 +-------------->BusC_5(Inner switch)    |   |
     *                |                                    |                    |   |
     *                | (TDM_Loopback)BusC_6<--------------+                    |   |
     *                |                 |                                       |   |
     *                |                 +-------------->  ...                   |   |
     *                |                                    |                    |   |
     *                | (TDM_Loopback) .... <--------------+                    |   |
     *                |                 |                                       |   |
     *                |                 +-------------->BusC_N-3(Inner switch)  |   |
     *                |                                    |                    |   |
     *                | (TDM_Loopback)BusC_N-2<------------+                    |   |
     *                |                 |                                       |   |
     *                |                 v                                       |   |
     *                |                 +-------------->BusC_N-1(Inner switch)->+   |
     *                |                                                             |
     *                +-------------------------------------------------------------+
     *
     ***************************************************************************/

    /* Hook eth_L1 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_L1, 0, client_c_ids[0]), "(eth_L1)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_c_ids[nof_clients-1], 0, eth_L1), "(eth_L1)");

    /* TDM setting:  BusC_X --> TDM_Bypass --> BusC_X+1 (X should be an event number) */
    for (i = 0; i < nof_clients; i++) {
        if ((i % 2) == 0) {
            snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example_multi_device(unit, unit, client_c_ids[i], client_c_ids[i+1], 0, flexe_gen2_tdm_grp_id, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size), error_msg);
        }
    }

    /* FlexE flow:  BusC_X --> Framer_Bypass --> BusC_X+1 (X should be a odd number) */
    for (i = 0; i < nof_clients - 1; i++) {
        if (i % 2) {
            snprintf(error_msg, sizeof(error_msg), "(C->A, index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_c_ids[i], 0, client_c_ids[i+1]), error_msg);
        }
    }

    return BCM_E_NONE;
}

/* Function to create mixed L1&L2 snake with specific number of TAMC clients and SAR clients */
int dnx_flexe_gen2_mix_L1_L2_snake_main(
    int unit,
    int client_speed,
    int total_clients,
    bcm_port_t eth_tx,
    bcm_port_t eth_rx,
    int client_b_start,
    int client_c_start)
{
    int i;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int client_b_ids[320];
    int client_c_ids[320];
    int client_id, dest_client, nof_clients_added;
    int reserved_port_0 = 255; /* PP-DSP 255 is reserved */
    int reserved_port_1 = 510;
    int reserved_port_2 = 511;
    int reserved_port_3 = 218;
    int nof_clients = total_clients / 2;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_mix_L1_L2_snake_main";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create busB clients */
    i = 0;
    nof_clients_added = 0;
    while (nof_clients_added < nof_clients) {
        client_id = client_b_start + i;
        if (client_id == reserved_port_0 || client_id == reserved_port_1 || client_id == reserved_port_2 || client_id == reserved_port_3) {
            i++;
            continue;
        }
        snprintf(error_msg, sizeof(error_msg), "fail to create busB client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_id, port_type_busB, client_speed, 0), error_msg);
        client_b_ids[nof_clients_added] = client_id;
        nof_clients_added++;
        i++;
    }

    /* Create busC clients */
    i = 0;
    nof_clients_added = 0;
    while (nof_clients_added < nof_clients) {
        client_id = client_c_start + i;
        if (client_id == reserved_port_0 || client_id == reserved_port_1 || client_id == reserved_port_2 || client_id == reserved_port_3) {
            i++;
            continue;
        }
        snprintf(error_msg, sizeof(error_msg), "fail to create busC client(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_id, port_type_busC, client_speed, 0), error_msg);
        client_c_ids[nof_clients_added] = client_id;
        nof_clients_added++;
        i++;
    }

    /***************************************************************************
     * FlexE datapath:
     *  +-----------------------------------------------+
     *  |                                               |
     *  |                           BusB_0<-------------|<---eth_tx
     *  |                              |                |
     *  | (TDM_Loopback)BusC_0 <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_1(L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback)BusC_1 <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_2(L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback)BusC_2 <-------+                |
     *  |                  |                            |
     *  |                  +------>  .... (L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback) ....  <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_N(L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback)BusC_N <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_0------------->|--->eth_rx
     *  |                                               |
     *  +-----------------------------------------------+
     ***************************************************************************/

    /* Build FlexE flows: client_a_x <--> client_b_x */
    for (i = 0; i < nof_clients; i++) {
        /* FlexE flow: BusB_X --> BusC_X */
        snprintf(error_msg, sizeof(error_msg), "(B->C, index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b_ids[i], 0, client_c_ids[i]), error_msg);

        /* FlexE flow: BusC_X --> BusB_X+1 */
        dest_client = i == nof_clients -1 ? client_b_ids[0] : client_b_ids[i+1];
        snprintf(error_msg, sizeof(error_msg), "(C->B, index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_c_ids[i], 0, dest_client), error_msg);

        /* Loopback BusB clients(L2_force_fwd) */
        if (i) {
            snprintf(error_msg, sizeof(error_msg), "(B->B, index %d)", i);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_ids[i], client_b_ids[i], 1), error_msg);
        }

        /* Loopback BusC clients(TDM_Bypass) */
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example_multi_device(unit, unit, client_c_ids[i], client_c_ids[i], 0, flexe_gen2_tdm_grp_id, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size), error_msg);
    }

    /* Hook eth ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b_ids[0], 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b_ids[0], eth_rx, 1), "");

    return BCM_E_NONE;
}

int dnx_flexe_gen2_mix_L1_L2_snake_destroy(
    int unit,
    int total_clients,
    int client_b_start,
    int client_c_start)
{
    int i;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int client_b_ids[320];
    int client_c_ids[320];
    int client_id, dest_client, client_idx;
    int reserved_port_0 = 255; /* PP-DSP 255 is reserved */
    int reserved_port_1 = 510;
    int reserved_port_2 = 511;
    int reserved_port_3 = 218;
    int nof_clients = total_clients / 2;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_mix_L1_L2_snake_destroy";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Init busB client IDs */
    i = 0;
    client_idx = 0;
    while (client_idx < nof_clients) {
        client_id = client_b_start + i;
        if (client_id == reserved_port_0 || client_id == reserved_port_1 || client_id == reserved_port_2 || client_id == reserved_port_3) {
            i++;
            continue;
        }
        client_b_ids[client_idx++] = client_id;
        i++;
    }

    /* Init busC client IDs */
    i = 0;
    client_idx = 0;
    while (client_idx < nof_clients) {
        client_id = client_c_start + i;
        if (client_id == reserved_port_0 || client_id == reserved_port_1 || client_id == reserved_port_2 || client_id == reserved_port_3) {
            i++;
            continue;
        }
        client_c_ids[client_idx++] = client_id;
        i++;
    }

    /***************************************************************************
     * FlexE datapath:
     *  +-----------------------------------------------+
     *  |                                               |
     *  |                           BusB_0<-------------|<---eth_tx
     *  |                              |                |
     *  | (TDM_Loopback)BusC_0 <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_1(L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback)BusC_1 <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_2(L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback)BusC_2 <-------+                |
     *  |                  |                            |
     *  |                  +------>  .... (L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback) ....  <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_N(L2_loopback) |
     *  |                              |                |
     *  | (TDM_Loopback)BusC_N <-------+                |
     *  |                  |                            |
     *  |                  +------> BusB_0------------->|--->eth_rx
     *  |                                               |
     *  +-----------------------------------------------+
     ***************************************************************************/

    /* Clear FlexE flows */
    for (i = 0; i < nof_clients; i++) {
        /* FlexE flow: BusB_X --> BusC_X */
        snprintf(error_msg, sizeof(error_msg), "(B->C, index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_b_ids[i], 0, client_c_ids[i]), error_msg);

        /* FlexE flow: BusC_X --> BusB_X+1 */
        dest_client = i == nof_clients -1 ? client_b_ids[0] : client_b_ids[i+1];
        snprintf(error_msg, sizeof(error_msg), "(C->B, index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_clear(unit, client_c_ids[i], 0, dest_client), error_msg);
    }

    /* Remove BusB&BusC clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(BusB, index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_b_ids[i]), error_msg);
        snprintf(error_msg, sizeof(error_msg), "(BusC, index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit, client_c_ids[i]), error_msg);
    }

    return BCM_E_NONE;
}

int dnx_flexe_gen2_snake_with_mix_clients(
    int unit,
    int grp_index,
    int grp_speed,
    int client_speed,
    bcm_port_t flexe_phy,
    bcm_port_t eth_L1,
    bcm_port_t eth_tx,
    bcm_port_t eth_rx)
{
    int i;
    int nof_pcs;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    int client_slots;
    int total_client_slots;
    bcm_port_flexe_group_phy_info_t grp_phy_info[16];
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);
    int client_a_0 = 120;
    int client_a_1 = 121;
    int client_a_2 = 122;
    int client_b   = 130;
    int client_c_0 = 140;
    int client_c_1 = 141;
    int client_a_id[3] = {120, 121, 122};

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_snake_with_mix_clients";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - This function doesn't support FlexE_Gen1 ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create FlexE group */
    if (flexe_phy > 0) {
        /* Valid PHY port ID. Use it for FlexE group creation */
        flexe_phy_ports[0] = flexe_phy;
        nof_pcs = 1;
    } else {
        /* Invalid PHY port ID. Auto detect FlexE PHYs in array flexe_phy_ports[] */
        nof_pcs = 0;
        for (i = 0; i < 16; i++) {
            if (flexe_phy_ports[i]) {
                nof_pcs++;
            } else {
                break;
            }
        }
        if (!nof_pcs) {
            printf("**** %s: Error, no FlexE PHY provided(nof_pcs is %d) ****\n", proc_name, nof_pcs);
        }
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_0, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_1, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_2, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c_0, port_type_busC, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c_1, port_type_busC, client_speed, 0), "");

    /***************************************************************************
     * FlexE datapath:
     *
     *    eth_tx --------> client_b -------> client_a_0(FlexE PHY Loopback)
     *                                             |
     *                   +<-- client_c_0 <---------+
     *              (TDM)|
     *                   +--> client_c_1 ---> client_a_1(FlexE PHY Loopback)
     *                                             |
     *                   +<-- eth_L1 <-------------+
     *         (loopback)|
     *                   +--> eth_L1 ---> client_a_2(FlexE PHY Loopback)
     *                                             |
     *    eth_rx <-------- client_b <--------------+
     *
     ****************************************************************************/


    /* Hook eth ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_tx, client_b, 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b, eth_rx, 1), "");

    /* FlexE flows */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b, 0, client_a_0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_0, 0, client_c_0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_c_1, 0, client_a_1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_1, 0, eth_L1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_L1, 0, client_a_2), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a_2, 0, client_b), "");

    /* TDM setting */
    BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example_multi_device(unit, unit, client_c_0, client_c_1, 0, flexe_gen2_tdm_grp_id, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size), "");

    /* Loopback FlexE PHY(s) and eth_L1 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_get(unit, flexe_grp, 0, 16, &grp_phy_info, &nof_pcs), "");
    for (i = 0; i < nof_pcs; i++) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, grp_phy_info[i].port, BCM_PORT_LOOPBACK_PHY), "(phy)");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_L1, BCM_PORT_LOOPBACK_PHY), "(eth_L1)");
    /* Wait for PHY(s) linkup. The purpose is to speedup dnx_flexe_util_calendar_slots_set() */
    bshell(unit,"sleep 2");

    /* Compute how many slots should be used */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    client_slots = client_speed / grp_slot_speed;
    total_client_slots = client_slots * 3;

    /* Assign calendar slots for BusA clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < total_client_slots; i++) {
        Cal_Slots_A[i] = client_a_id[i / client_slots];
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Set group ID in overhead */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1),
        "fail to set OhGroupID");

    return BCM_E_NONE;
}

/* Function to create FlexE clients with different speeds
 * There is an option for slot assignment as below:
 * calendar_cfg - 0: Assign slots from 1st slot to last slot(synchronous slots in Rx and Tx)
 * calendar_cfg - 1: Assign slots from last slot to 1st slot(synchronous slots in Rx and Tx)
 * calendar_cfg - 2: Randomly assign slots for all clients(asynchronous slots in Rx and Tx)
 */
int gen2_cal_slots_unit0[800];
int gen2_cal_slots_unit1[800];
int dnx_flexe_gen2_mixed_clients_main(
    int unit_0,
    int unit_1,
    int grp_speed,
    bcm_port_t eth_tx,
    int calendar_cfg,
    int cfg_index)
{
    int i, ii, iii, unit_iter, unit;
    int nof_flexe_phys = 0;
    int total_slots;
    int last_slot;
    int vlan_id_base = 1000;
    bcm_port_t client_a_base = 30;
    bcm_port_t client_b_base = 130;
    int client_speed[80];
    int nof_clients;
    int client_id, nof_slots_per_client, slot_index;
    int slot_unuse[800], nof_slot_unuse, rnd_index;
    bcm_gport_t flexe_grp;
    uint32 cal_flags = 0;
    int grp_slot_speed;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_mixed_clients_main";

    /* Initiate the number of clients and client speeds */
    if (cfg_index == 0) {
        nof_clients = 5;
        client_speed[0] = 5000;
        client_speed[1] = 10000;
        client_speed[2] = 30000;
        client_speed[3] = 55000;
        client_speed[4] = 1500000;
        flexe_gen2_grp_phy_slot_mode = bcmPortFlexePhySlot5G;
    }
    if (cfg_index == 1) {
        nof_clients = 5;
        client_speed[0] = 1000;
        client_speed[1] = 14000;
        client_speed[2] = 35000;
        client_speed[3] = 50000;
        client_speed[4] = 700000;
        flexe_gen2_grp_phy_slot_mode = bcmPortFlexePhySlot1G;
    }
    if (cfg_index == 2) {
        nof_clients = 5;
        client_speed[0] = 1250;
        client_speed[1] = 12500;
        client_speed[2] = 36250;
        client_speed[3] = 50000;
        client_speed[4] = 700000;
        flexe_gen2_grp_phy_slot_mode = bcmPortFlexePhySlot1P25G;
    }
    if (cfg_index == 3) {
        nof_clients = 4;
        client_speed[0] = 400000;
        client_speed[1] = 400000;
        client_speed[2] = 400000;
        client_speed[3] = 400000;
        flexe_gen2_grp_phy_slot_mode = bcmPortFlexePhySlot5G;
    }

    /***************************************************************************
     * Datapath:
     *
     *                           unit_0                           unit_1
     *                +---------------------------+     +--------------------------------------+
     *           +--->| --> BusB_0  <--> BusA_0   |<--->| --> BusA_0   <--> BusB_0  (loopback) |
     *           |    |                           |     |                                      |
     *           |--->| --> BusB_1  <--> BusA_1   |<--->| --> BusA_1   <--> BusB_1  (loopback) |
     *           |    |                           |     |                                      |
     * eth_tx--->+--->| -->  ...    <-->  ...     |<--->| -->  ...     <-->  ...               |
     *           |    |                           |     |                                      |
     *           |--->| --> BusB_N-1<--> BusA_N-1 |<--->| --> BusA_N-1 <--> BusB_N-1(loopback) |
     *           |    |                           |     |                                      |
     *           +--->| --> BusB_N  <--> BusA_N   |<--->| --> BusA_N   <--> BusB_N  (loopback) |
     *                +---------------------------+     +--------------------------------------+
     * unit_0 -- eth_tx->BusB_X: L2_MC lookup; Configured in TCL shell.
     *           BusA_X->BusB_X->Black_Hole
     **************************************************************************/

    /***************************************************************************
     * Create FlexE datapath at unit_0
     ***************************************************************************/
    unit = unit_0;

    /* Create FlexE group with FlexE PHYs in array flexe_phy_ports[] */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    nof_flexe_phys = 0;
    for (i = 0; i < 16; i++) {
        if (flexe_phy_ports[i]) {
            nof_flexe_phys++;
        } else {
            break;
        }
    }
    if (!nof_flexe_phys) {
        printf("**** %s: Error, no FlexE PHY provided(nof_flexe_phys is %d) ****\n", proc_name, nof_flexe_phys);
    }
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_flexe_phys), error_msg);

    /* Create BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed[i], 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB, client_speed[i], 0), error_msg);
    }

    /* Setting FlexE flows between BusA and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
    }

    /* Forward all TAMC Rx packets to Black_Hole */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG( bcm_port_force_forward_set(unit, client_b_base+i, BCM_GPORT_BLACK_HOLE, 1), error_msg);
    }

    /***************************************************************************
     * Create FlexE datapath at unit_1
     ***************************************************************************/
    unit = unit_1;

    /* Create FlexE group with FlexE PHYs in array flexe_phy_ports[] */
    snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_flexe_phys), error_msg);

    /* Create BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed[i], 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB, client_speed[i], 0), error_msg);
    }

    /* Setting FlexE flows between BusA and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0), error_msg);
    }

    /* Loopback all TAMC Rx packets */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(unit %d, index %d)", unit, i);
        BCM_IF_ERROR_RETURN_MSG( bcm_port_force_forward_set(unit, client_b_base+i, client_b_base+i, 1), error_msg);
    }

    /***************************************************************************
     * Calendar settings on the 2 units
     ***************************************************************************/

    /* calendar_cfg:
     * - 0/1, Use synchronous slots in both Tx and Rx directions
     * - 2    Use asynchronous slots in Tx and Rx directions(different slot assignment but nof_tx_slots = nof_rx_slots).
     */
    cal_flags = calendar_cfg > 1 ? BCM_PORT_FLEXE_TX : 0;

    /* Compute total slots according to grp_speed and grp_slot_speed */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    last_slot = total_slots - 1;

    /* Start to assign slots according to calendar_cfg */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_1 : unit_0;

        /* Calendar and OH settings */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
        if (calendar_cfg == 0) {
            /* Starting to assign slots from the 1st slot in the array */
            slot_index = 0;

            for (i = 0; i < nof_clients; i++) {
                client_id = client_a_base + i;
                nof_slots_per_client = client_speed[i] / grp_slot_speed;

                for (ii = 0; ii < nof_slots_per_client; ii++) {
                    Cal_Slots_A[slot_index++] = client_id;
                }
            }
        } else if (calendar_cfg == 1) {
            /* Starting to assign slots from the last slot in the array */
            slot_index = last_slot;

            for (i = 0; i < nof_clients; i++) {
                client_id = client_a_base + i;
                nof_slots_per_client = client_speed[i] / grp_slot_speed;

                for (ii = 0; ii < nof_slots_per_client; ii++) {
                    Cal_Slots_A[slot_index--] = client_id;
                }
            }
        } else {
            /* Rondomly assign slots for all the clients
             * Now mark all the slots as unused slots
             */
            for (i = 0; i < total_slots; i++) {
                slot_unuse[i] = i;
            }
            nof_slot_unuse = total_slots;

            /* Start to select unused slots for each client */
            for (i = 0; i < nof_clients; i++) {
                client_id = client_a_base + i;
                nof_slots_per_client = client_speed[i] / grp_slot_speed;

                for (ii = 0; ii < nof_slots_per_client; ii++) {
                    /* Randomly find out a unused slot from array slot_unuse[] */
                    rnd_index = sal_rand() % nof_slot_unuse;
                    slot_index = slot_unuse[rnd_index];
                    Cal_Slots_A[slot_index] = client_id;

                    /* Remove the selected slot from slot_unuse[] */
                    for (iii = rnd_index; iii < nof_slot_unuse - 1; iii++) {
                        slot_unuse[iii] = slot_unuse[iii+1];
                    }
                    nof_slot_unuse--;
                }
            }
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_A)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, FLEXE_CAL_A, total_slots), error_msg);

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 800; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, CAL_B)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, FLEXE_CAL_B, total_slots), error_msg);

        /* Group ID in overhead */
        snprintf(error_msg, sizeof(error_msg), "fail to set OhGroupID(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1), error_msg);

        /* Store slot setting for calendar switching */
        for (i = 0; i < 800; i++) {
            if ($unit_iter) {
                gen2_cal_slots_unit1[i] = Cal_Slots_A[i];
            } else {
                gen2_cal_slots_unit0[i] = Cal_Slots_A[i];
            }
        }
    }

    /* Calendar Rx setting if calendar_cfg is 2(different slot assignment in Tx&Rx) */
    if (calendar_cfg == 2) {
        /* Set calendar Rx at uni_0 with calendar Tx setting at unit_1 */
        for (i = 0; i < 800; i++) {
            Cal_Slots_A[i] = gen2_cal_slots_unit1[i];
            Cal_Slots_B[i] = gen2_cal_slots_unit1[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, Rx)", unit_0);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_0, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_0, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots), error_msg);

        /* Set calendar Rx at unit_1 with calendar Tx setting at unit_0 */
        for (i = 0; i < 800; i++) {
            Cal_Slots_A[i] = gen2_cal_slots_unit0[i];
            Cal_Slots_B[i] = gen2_cal_slots_unit0[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d, Rx)", unit_1);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_1, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_1, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots), error_msg);
    }

    return BCM_E_NONE;
}

/* Function to destroy all settings in dnx_flexe_gen2_mixed_clients_main() */
int dnx_flexe_gen2_mixed_clients_destroy(
    int unit_0,
    int unit_1,
    int grp_speed,
    int nof_clients)
{
    int i;
    int total_slots;
    bcm_port_t client_a_base = 30;
    bcm_port_t client_b_base = 130;
    bcm_gport_t flexe_grp;
    uint32 cal_flags = 0;
    int grp_slot_speed;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_mixed_clients_destroy";

    /* Compute total slots */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit_0, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;

    /* Clear calendar */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_B), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_0, flexe_grp, cal_flags, FLEXE_CAL_A, total_slots), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_0, flexe_grp, cal_flags, FLEXE_CAL_B, total_slots), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_1, flexe_grp, cal_flags, FLEXE_CAL_A, total_slots), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_1, flexe_grp, cal_flags, FLEXE_CAL_B, total_slots), "");

    /* Clear FlexE flows */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_0, client_a_base+i, client_b_base+i, 1), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_0, client_b_base+i, client_a_base+i, 1), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_1, client_a_base+i, client_b_base+i, 1), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_datapath_client_to_client(unit_1, client_b_base+i, client_a_base+i, 1), error_msg);
    }

    /* Remove FlexE clients */
    for (i = 0; i < nof_clients; i++) {
        snprintf(error_msg, sizeof(error_msg), "(index %d)", i);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_0, client_a_base+i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_0, client_b_base+i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_1, client_a_base+i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_remove(unit_1, client_b_base+i), error_msg);
    }

    /* Destroy FlexE group */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit_0, flexe_grp), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_destroy(unit_1, flexe_grp), "");

    return BCM_E_NONE;
}

/* Function to switch calendar based on the setting in dnx_flexe_gen2_mixed_clients_main() 
 * @calendar_id             Calendar id which will be switched to
 * @client_index            BusA client index. It is used to add or remove FlexE client during calendar switching
 *                          If it is -1, there will be no client adding&removal during calendar switching
 * @all_clients_removed     A flag to indicate all FlexE clients have been removed before the calendar switching
 *                          If set, the 'calendar_id' should be the active canlendar. Or 'calendar_id' should be the inactive canlendar
 */
int dnx_flexe_gen2_calendar_switch_with_mixed_clients_main(
    int unit_0,
    int unit_1,
    int grp_speed,
    bcm_port_flexe_group_cal_t calendar_id,
    int client_index,
    int is_client_removal,
    int all_clients_removed,
    int is_sync_slots)
{
    int i, unit_iter, unit;
    int total_slots;
    bcm_port_t client_a_base = 30;
    int client_id = client_a_base + client_index;
    int cal_slots_init_tx[800], cal_slots_init_rx[800];
    int active_cal_slots_tx[800], active_cal_slots_rx[800];
    int grp_slot_speed;
    char error_msg[200]={0,};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_calendar_switch_with_mixed_clients_main";

    bcm_gport_t flexe_grp;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    bcm_port_flexe_group_cal_t cal_active;
    uint32 cal_flags = is_sync_slots ? 0 : BCM_PORT_FLEXE_TX;

    /* Compute total slots according to grp_speed and grp_slot_speed */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;

    /***************************************************************************
     * Datapath:
     *
     *                           unit_0                           unit_1
     *                +---------------------------+     +--------------------------------------+
     *           +--->| --> BusB_0  <--> BusA_0   |<--->| --> BusA_0   <--> BusB_0  (loopback) |
     *           |    |                           |     |                                      |
     *           |--->| --> BusB_1  <--> BusA_1   |<--->| --> BusA_1   <--> BusB_1  (loopback) |
     *           |    |                           |     |                                      |
     * eth_tx--->+--->| -->  ...    <-->  ...     |<--->| -->  ...     <-->  ...               |
     *           |    |                           |     |                                      |
     *           |--->| --> BusB_N-1<--> BusA_N-1 |<--->| --> BusA_N-1 <--> BusB_N-1(loopback) |
     *           |    |                           |     |                                      |
     *           +--->| --> BusB_N  <--> BusA_N   |<--->| --> BusA_N   <--> BusB_N  (loopback) |
     *                +---------------------------+     +--------------------------------------+
     * unit_0 -- eth_tx->BusB_X: L2_MC lookup; Configured in TCL shell.
     *           BusA_X->BusB_X->Black_Hole
     **************************************************************************/

    /* Do calendar switching on the 2 devices */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_1 : unit_0;

        /* Check the provided calendar ID should be in inactive status */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active), error_msg);
        if (cal_active == calendar_id && !all_clients_removed) {
            printf("**** %s:  Error, the provided calendar ID is already in active status ****\n", proc_name);
            return BCM_E_FAIL;
        }
        if (cal_active != calendar_id && all_clients_removed) {
            printf("**** %s:  Error, the provided calendar ID isn't active calendar ID ****\n", proc_name);
            return BCM_E_FAIL;
        }

        /* Get slot assignment on active calendar */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 800, active_cal_slots_tx, &i), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 800, active_cal_slots_rx, &i), error_msg);

        /* Load initial calendar setting */
        for (i = 0; i < 800; i++) {
            cal_slots_init_tx[i] = unit_iter ? gen2_cal_slots_unit1[i] : gen2_cal_slots_unit0[i];
            if (is_sync_slots) {
                cal_slots_init_rx[i] = unit_iter ? gen2_cal_slots_unit1[i] : gen2_cal_slots_unit0[i];
            } else {
                cal_slots_init_rx[i] = unit_iter ? gen2_cal_slots_unit0[i] : gen2_cal_slots_unit1[i];
            }
        }

        /* Add/remove the client into/from the inactive calendar */
        for (i = 0; i < 800; i++) {
            if (cal_slots_init_tx[i] == client_id) {
                /* Find out a slot for the client in initial tx slot array */
                active_cal_slots_tx[i] = is_client_removal ? 0 : client_id;
            }
            if (cal_slots_init_rx[i] == client_id) {
                /* Find out a slot for the client in initial rx slot array */
                active_cal_slots_rx[i] = is_client_removal ? 0 : client_id;
            }
        }

        /* Configure the inactive calendar(Tx or Tx+Rx) */
        for (i = 0; i < 800; i++) {
            Cal_Slots_A[i] = active_cal_slots_tx[i];
            Cal_Slots_B[i] = active_cal_slots_tx[i];
        }
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, calendar_id, total_slots), error_msg);

        /* Configure the inactive calendar(Rx only for asynchronous slot assignment) */
        if (!is_sync_slots) {
            for (i = 0; i < 800; i++) {
                Cal_Slots_A[i] = active_cal_slots_rx[i];
                Cal_Slots_B[i] = active_cal_slots_rx[i];
            }
            snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, calendar_id, total_slots), error_msg);

        }

        /* Set the the inactive calendar as CalInUse in overhead before the switching */
        snprintf(error_msg, sizeof(error_msg), "(unit %d)", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, calendar_id), error_msg);

        /* Do calendar switching by activing the inactive calendar */
        snprintf(error_msg, sizeof(error_msg), "fail to active calendar %d on unit %d", calendar_id, unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_group_cal_active_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id), error_msg);
    }
    sal_sleep(3);

    /* Set backup calendar to be the same as active calendar
     * It is used to avoid packet loss during calendar switching
     * Now active_cal_slots_tx[] records Tx slots at unit1
     * Now active_cal_slots_rx[] records Rx slots at unit1
     */

    for (i = 0; i < 800; i++) {
        Cal_Slots_A[i] = active_cal_slots_tx[i];
        Cal_Slots_B[i] = active_cal_slots_tx[i];
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_0, flexe_grp, BCM_PORT_FLEXE_RX, !calendar_id, total_slots),
        "(cen_rx, cal_backup)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_1, flexe_grp, BCM_PORT_FLEXE_TX, !calendar_id, total_slots),
        "(dis_tx, cal_backup)");

    for (i = 0; i < 800; i++) {
        Cal_Slots_A[i] = active_cal_slots_rx[i];
        Cal_Slots_B[i] = active_cal_slots_rx[i];
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_0, flexe_grp, BCM_PORT_FLEXE_TX, !calendar_id, total_slots),
        "(cen_tx, cal_backup)");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit_1, flexe_grp, BCM_PORT_FLEXE_RX, !calendar_id, total_slots),
        "(dis_rx, cal_backup)");

    return BCM_E_NONE;
}

bcm_field_entry_t oh_oam_filter_ent_def;
bcm_field_entry_t oh_oam_filter_ent;
int dnx_flexe_oh_oam_filter_set(
    int unit,
    int mgmt_port,
    bcm_field_context_t context_id)
{
    int i;
    uint32 flags = BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    bcm_field_group_t fg_id = 60;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_gport_t sys_gport;

    BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, mgmt_port);

    /* Create field group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage =  bcmFieldStageIngressPMF1;

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifySrcPort;
    fg_info.qual_types[1] = bcmFieldQualifyDstMac;
    fg_info.qual_types[2] = bcmFieldQualifySrcMac;
    fg_info.qual_types[3] = bcmFieldQualifyEtherTypeUntagged;

    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_types[1] = bcmFieldActionRedirect;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, flags, &fg_info, &fg_id), "");

    /* Attaching the FG */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    for (i = 0; i < fg_info.nof_quals; i++)
    {
        attach_info.key_info.qual_types[i] = fg_info.qual_types[i];
    }
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (i = 0; i < fg_info.nof_actions; i++)
    {
        attach_info.payload_info.action_types[i] = fg_info.action_types[i];
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[3].input_arg = 0;
    attach_info.key_info.qual_info[3].offset = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info), "");

    /* Add a low priority PMF entry to drop all packets at the MGMT port */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 0xFFFF;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifySrcPort;
    ent_info.entry_qual[0].value[0] = sys_gport;
    ent_info.entry_qual[0].mask[0] = 0xFFFF;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionPrioIntNew;
    ent_info.entry_action[0].value[0] = 1;
    ent_info.entry_action[1].type = bcmFieldActionRedirect;
    ent_info.entry_action[1].value[0] = BCM_GPORT_BLACK_HOLE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id, &ent_info, &oh_oam_filter_ent_def), "");

    return BCM_E_NONE;
}

int dnx_flexe_oh_oam_filter_entry_add(
    int unit,
    int mgmt_port,
    int oam_type_len,
    bcm_mac_t dst_mac,
    bcm_mac_t src_mac,
    int dst_port,
    uint32 priority)
{
    bcm_field_group_t fg_id = 60;
    bcm_field_entry_t filter_ent_id;
    bcm_field_entry_info_t ent_info;
    bcm_gport_t sys_gport;
    uint32 dmac_32lsb, dmac_16msb;
    uint32 smac_32lsb, smac_16msb;

    /* Check the TCAM entry priority should be higher than default filter entry */
    if (priority >= 0xFFFF) {
        printf("**** Error: Provided entry priority %d should less than 0xFFFF ****\n", priority);
        return BCM_E_FAIL;
    }

    dmac_16msb = (dst_mac[0] << 8) + dst_mac[1];
    smac_16msb = (src_mac[0] << 8) + src_mac[1];
    dmac_32lsb = (dst_mac[2] << 24) + (dst_mac[3] << 16) + (dst_mac[4] << 8) + dst_mac[5];
    smac_32lsb = (src_mac[2] << 24) + (src_mac[3] << 16) + (src_mac[4] << 8) + src_mac[5];

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority;
    ent_info.nof_entry_quals = 4;
    ent_info.nof_entry_actions = 2;

    /* Filter SrcPort */
    BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, mgmt_port);
    ent_info.entry_qual[0].type = bcmFieldQualifySrcPort;
    ent_info.entry_qual[0].value[0] = sys_gport;
    ent_info.entry_qual[0].mask[0] = 0xFFFF;

    /* Filter DstMac */
    ent_info.entry_qual[1].type = bcmFieldQualifyDstMac;
    ent_info.entry_qual[1].value[0] = dmac_32lsb;
    ent_info.entry_qual[1].value[1] = dmac_16msb;
    ent_info.entry_qual[1].mask[0] = 0xFFFFFFFF;
    ent_info.entry_qual[1].mask[1] = 0xFFFF;

    /* Filter SrcMac */
    ent_info.entry_qual[2].type = bcmFieldQualifySrcMac;
    ent_info.entry_qual[2].value[0] = smac_32lsb;
    ent_info.entry_qual[2].value[1] = smac_16msb;
    ent_info.entry_qual[2].mask[0] = 0xFFFFFFFF;
    ent_info.entry_qual[2].mask[1] = 0xFFFF;

    /* Filter T/L field */
    ent_info.entry_qual[3].type = bcmFieldQualifyEtherTypeUntagged;
    ent_info.entry_qual[3].value[0] = oam_type_len;
    ent_info.entry_qual[3].mask[0] = 0xFFFF;

    /* Forward matched packets to dst_port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, dst_port);
    ent_info.entry_action[0].type = bcmFieldActionPrioIntNew;
    ent_info.entry_action[0].value[0] = 2;
    ent_info.entry_action[1].type = bcmFieldActionRedirect;
    ent_info.entry_action[1].value[0] = sys_gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id, &ent_info, &oh_oam_filter_ent), "");

    return BCM_E_NONE;
}

/* Typical datapath test on single device */
int dnx_flexe_gen2_typical_datapath_test(
    int unit,
    int grp_speed,
    bcm_port_t flexe_phy,
    bcm_port_t cpu_tx,
    bcm_port_t eth_rx,
    bcm_port_t eth_x,
    bcm_port_t ilkn_l2,
    bcm_port_t ilkn_tdm,
    int with_ilkn)
{
    int i;
    int nof_pcs = 1;
    int grp_index = 1;
    bcm_gport_t flexe_grp;
    int grp_slot_speed;
    int total_slots;
    int client_speed = 10000;
    int client_slots;
    int client_a = 120;
    int client_b = 121;
    int client_c = 122;
    int is_flexe_gen2 = dnx_flexe_gen2_support(unit, -1);

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_gen2_typical_datapath_test";

    /* Not support FlexE_Gen1 */
    if (!is_flexe_gen2) {
        printf("**** %s: Error - FlexE_Gen1 doesn't support this test! ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Create FlexE group */
    flexe_phy_ports[0] = flexe_phy;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs), "");

    /* Create FlexE clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_b, port_type_busB, client_speed, 0), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed, 0), "");

    /***************************************************************************
     * FlexE datapath:
     *
     *    cpu_tx ----> eth_x(loopback) --> L2 --> client_b ------> client_a
     *                                                                 | (flexe_phy loopback)
     *    client_c <-- L1 <--(loopback)ilkn_tdm <-- L1 <-- client_c <--+
     *        |
     *        +--> client_b --> L2 --> ilkn_l2(loopback) --> eth_rx
     *
     ****************************************************************************/

    /*
     * Build following datapath:
     * cpu_tx ----> eth_x(phy_loopback) --> L2 --> client_b --> client_a(flexe_phy loopback)
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, cpu_tx, eth_x, 1), "(cpu_tx->eth_x)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_x, BCM_PORT_LOOPBACK_PHY), "(eth_x)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_x, client_b, 1), "(eth_x->client_b)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_b, 0, client_a), "(client_b->client_a)");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY), "(flexe_phy)");

    /*
     * Build following datapath:
     * client_c <-- L1 <--(phy_loopback)ilkn_tdm <-- L1 <-- client_c <-- client_a
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_a, 0, client_c), "(client_a->client_c)");
    if (with_ilkn) {
        BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example(unit, client_c, ilkn_tdm, 0, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size, 6001), "(client_c->ilkn_tdm)");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, ilkn_tdm, BCM_PORT_LOOPBACK_PHY), "(ilkn_tdm)");
        BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example(unit, ilkn_tdm, client_c, 0, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size, 6002), "(ilkn_tdm->client_c)");
    } else {
        /* with_ilkn is 0: build TDM datapath client_c->client_c */
        BCM_IF_ERROR_RETURN_MSG(cint_dnx_tdm_sar_example(unit, client_c, client_c, 0, flexe_gen2_tdm_min_size, flexe_gen2_tdm_max_size, 6001), "(client_c->client_c)");
    }

    /*
     * Build following datapath:
     * client_c --> client_b --> L2 --> ilkn_l2(phy_loopback) --> eth_rx
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_c, 0, client_b), "(client_c->client_b)");
    if (with_ilkn) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b, ilkn_l2, 1), "(client_b->ilkn_l2)");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, ilkn_l2, BCM_PORT_LOOPBACK_PHY), "(ilkn_l2)");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, ilkn_l2, eth_rx, 1), "(ilkn_l2->eth_rx)");
    } else {
        /* with_ilkn is 0: build L2 datapath client_b-->eth_rx */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_b, eth_rx, 1), "(client_b->eth_rx)");
    }

    /* Compute FlexE slots */
    grp_slot_speed = dnx_flexe_util_group_slot_speed_get(unit, flexe_grp);
    if (grp_slot_speed <= 0) {
        printf("**** %s: Error, fail in dnx_flexe_util_group_slot_speed_get() ****\n", proc_name);
        return BCM_E_FAIL;
    }
    total_slots = grp_speed / grp_slot_speed;
    client_slots = client_speed / grp_slot_speed;

    /* Assign calendar slots for BusA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A), "");
    for (i = 0; i < client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots), "");

    /* Set group ID in overhead */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index + 1), "fail to set OhGroupID");

    return BCM_E_NONE;
}

