/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/* CINT to verify FlexE related funtions
 *
 * Please load below CINT flies:
 * cd $SDK/src/examples/dnx
 * cint ./internal/cint_nif_phy_data.c
 * cint ./internal/cint_nif_test.c
 * cint ./port/cint_dynamic_port_add_remove.c
 * cint ./internal/port/cint_test_dynamic_port_add.c
 * cint ./internal/flexe/cint_flexe_test.c
 */

/* FlexE PHY port array for FlexE group creation */
bcm_port_t flexe_phy_ports[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Logical PHY ID for each FlexE PHY in flexe_phy_ports[] */
int logical_phy_ids[8] = {0, 0, 0, 0, 0, 0, 0, 0};

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

/*
 * Slots assignment for calendar A and calendar B
 * The array indicator is the slot location
 * The array value is relevant FlexE client ID
 * The 2 arrays will be used by funtion 'dnx_flexe_cint_calendar_slots_set'
 */
int Cal_Slots_A[80];
int Cal_Slots_B[80];

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
    port_type_unknown
};

int is_q2a_a0_only(int unit) {
    int rv;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	    printf("Error in bcm_info_get\n");
  	    return rv;
    }

    if ((info.device == 0x8480 || info.device == 0x8481) && info.revision == 0x1) {
        return 1;
    } else {
        return 0;
    }
}

int dev_q2u_check(int unit0, int unit1) {
    int i, rv;
    int units[2] = {unit0, unit1};
    int q2u_flags[2] = {-1, -1};
    bcm_info_t info;

    for (i = 0; i < 2; i++) {
        if (units[i] < 0) {continue;}

        rv = bcm_info_get(units[i], &info);
        if (rv != BCM_E_NONE) {
  	        printf("Error in bcm_info_get\n");
  	        return rv;
        }
        q2u_flags[i] = info.device == 0x8280 ? 1 : 0;
    }

    if (q2u_flags[0] == -1 && q2u_flags[1] == -1) {
        printf("**** Error: bad unit IDs %d, %d ****\n", q2u_flags[0], q2u_flags[1]);
        return BCM_E_FAIL;
    } else if (q2u_flags[0] != -1 && q2u_flags[1] != -1) {
        /* Return 1 if any unit is Q2U */
        return (q2u_flags[0] || q2u_flags[1]);
    } else {
        /* Only 1 unit is valid */
        return (q2u_flags[0] == -1 ? q2u_flags[1] : q2u_flags[0]);
    }
}

/* Global variable to indicate if Q2U exists in the system 
 * Need set it manually if the unit ID isn't 0
 */
int dev_is_q2u = dev_q2u_check(0, -1);

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
    int L1_flag, rv;
    uint32 flags_get;
    bcm_port_if_t if_type;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    flexe_port_type_t port_type_get = port_type_unknown;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_port_type_verify";

    /* Get port setting */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    rv = bcm_port_get(unit, port_id, &flags_get, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_get(port %d) ****\n", proc_name, rv, port_id);
        return rv;
    }

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
    int rv, default_action_id = 0;
    uint32 flags_egress = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    bcm_vlan_action_set_t vid_action;
    
    bcm_vlan_action_set_t_init(&vid_action);
    vid_action.dt_outer = bcmVlanActionNone;
    vid_action.dt_inner = bcmVlanActionNone;
    rv = bcm_vlan_translate_action_id_set(unit, flags_egress, default_action_id, &vid_action);
    if (rv != BCM_E_NONE) {
        printf("**** Error(%d), fail in bcm_vlan_translate_action_id_set ****\n", rv);
        return rv;
    }

    return rv;
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
    int rv;
    bcm_mac_t in_mac={0x00,0x00,0x00,0x00,0x11,0x11}; /* Dst_MAC to in_port */
    bcm_mac_t out_mac={0x00,0x00,0x00,0x00,0x22,0x22}; /* Dst_MAC to out_port */
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t tag_pbmp, untag_bmp;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_switch_config";

    /* Create VLAN */
    rv = bcm_vlan_create(unit, vlan_id);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_vlan_create ****\n", proc_name, rv);
        return rv;
    }

    /* Add ports into the VLAN */
    BCM_PBMP_CLEAR(untag_bmp);
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, in_port);
    BCM_PBMP_PORT_ADD(tag_pbmp, out_port);
    rv = bcm_vlan_port_add(unit, vlan_id, tag_pbmp, untag_bmp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_vlan_port_add ****\n", proc_name, rv);
        return rv;
    }

    /* Add L2 entry with in_mac */
    bcm_l2_addr_t_init(&l2_addr, in_mac, vlan_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = in_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("**** %s: Error(%d), fail in bcm_l2_addr_add(in_mac) ****\n", proc_name, rv);
        return rv;
    }

    /* Add L2 entry with out_mac */
    bcm_l2_addr_t_init(&l2_addr, out_mac, vlan_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("**** %s: Error(%d), fail in bcm_l2_addr_add(out_mac) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int rv;
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
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("**** %s: Error(%d), fail in bcm_l2_addr_add ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Delete the L2 entry */
        rv = bcm_l2_addr_delete(unit, mac_oper, vlan_id);
        if (rv != BCM_E_NONE)
        {
            printf("**** %s: Error(%d), fail in bcm_l2_addr_delete ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
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
    int rv;
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t tag_pbmp, untag_bmp;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_entry_advanced_set";

    /* VLAN setting */
    if (vlan_oper) {
        /* Create VLAN */
        rv = bcm_vlan_create(unit, vlan_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_vlan_create ****\n", proc_name, rv);
            return rv;
        }

        /* Add ports into the VLAN */
        BCM_PBMP_CLEAR(untag_bmp);
        BCM_PBMP_CLEAR(tag_pbmp);
        BCM_PBMP_PORT_ADD(tag_pbmp, in_port);
        BCM_PBMP_PORT_ADD(tag_pbmp, out_port);
        rv = bcm_vlan_port_add(unit, vlan_id, tag_pbmp, untag_bmp);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_vlan_port_add ****\n", proc_name, rv);
            return rv;
        }
    }

    /* L2 entry setting */
    if (mac_oper) {
        /* Add L2 entry with out_mac */
        bcm_l2_addr_t_init(&l2_addr, dst_mac, vlan_id);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = out_port;
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("**** %s: Error(%d), fail in bcm_l2_addr_add(out_mac) ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Delete the L2 entry */
        rv = bcm_l2_addr_delete(unit, dst_mac, vlan_id);
        if (rv != BCM_E_NONE)
        {
            printf("**** %s: Error(%d), fail in bcm_l2_addr_delete ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
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
    int rv;
    int ing_intf, egr_intf;
    int vrf = 0xab;
    uint32 sip = 0x01010101;
    uint32 dip = 0x02020202;
    bcm_mac_t in_mac = {0x00,0x00,0x00,0x00,0x11,0x11};  /* egress MAC or src_mac at in_port side */
    bcm_mac_t out_mac = {0x00,0x00,0x00,0x00,0x22,0x22}; /* egress MAC or src_mac at out_port side */
    bcm_mac_t my_mac = {0x00,0x00,0x00,0x00,0x33,0x33};
    bcm_pbmp_t tag_pbmp, untag_bmp;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t fec_ing = 0x2000CCFE; /* FEC ID attached on in_port */
    bcm_if_t fec_egr = 0x2000CCFF; /* FEC ID attached on out_port */
    bcm_l3_host_t l3_host;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L3_route_config";

    /* Create in_vlan and add in_port*/
    rv = bcm_vlan_create(unit, in_vlan);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_vlan_create(in_vlan) ****\n", proc_name, rv);
        return rv;
    }
    BCM_PBMP_CLEAR(untag_bmp);
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, in_port);
    rv = bcm_vlan_port_add(unit, in_vlan, tag_pbmp, untag_bmp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_vlan_port_add(in_port) ****\n", proc_name, rv);
        return rv;
    }

    /* Create out_vlan and add out_port*/
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_vlan_create(out_vlan) ****\n", proc_name, rv);
        return rv;
    }
    BCM_PBMP_CLEAR(tag_pbmp);
    BCM_PBMP_PORT_ADD(tag_pbmp, out_port);
    rv = bcm_vlan_port_add(unit, out_vlan, tag_pbmp, untag_bmp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_vlan_port_add(out_port) ****\n", proc_name, rv);
        return rv;
    }

    /* Create inRIF and outRIF */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, my_mac, 6);
    l3_intf.l3a_vid = in_vlan;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (rv) {
        printf("**** %s: Error(%d), fail in bcm_vlan_port_add(in_rif) ****\n", proc_name, rv);
        return rv;
    }
    ing_intf = l3_intf.l3a_intf_id;

    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, my_mac, 6);
    l3_intf.l3a_vid = out_vlan;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (rv) {
        printf("**** %s: Error(%d), fail in bcm_vlan_port_add(out_rif) ****\n", proc_name, rv);
        return rv;
    }
    egr_intf = l3_intf.l3a_intf_id;

    /* Create L3 nexthop at in_port side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, in_mac, 6);
    l3_egress.vlan = in_vlan;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_ing);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_l3_egress_create(EGRESS_ONLY, in_port) ****\n", proc_name, rv);
        return rv;
    }
    l3_egress.intf = ing_intf;
    l3_egress.port = in_port;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_ing);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_l3_egress_create(INGRESS_ONLY, in_port) ****\n", proc_name, rv);
        return rv;
    }

    /* Create L3 nexthop at out_port side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, out_mac, 6);
    l3_egress.vlan = out_vlan;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_egr);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_l3_egress_create(EGRESS_ONLY, out_port) ****\n", proc_name, rv);
        return rv;
    }
    l3_egress.intf = egr_intf;
    l3_egress.port = out_port;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_egr);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_l3_egress_create(INGRESS_ONLY, out_port) ****\n", proc_name, rv);
        return rv;
    }

    /* Add host entry with sip */
    bcm_l3_host_t_init(&l3_host);
    l3_host.l3a_ip_addr = sip;
    l3_host.l3a_vrf = vrf;
    l3_host.l3a_intf = fec_ing;
    rv = bcm_l3_host_add(unit, &l3_host);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_l3_host_add(sip) ****\n", proc_name, rv);
        return rv;
    }

    /* Add host entry with dip */
    bcm_l3_host_t_init(&l3_host);
    l3_host.l3a_ip_addr = dip;
    l3_host.l3a_vrf = vrf;
    l3_host.l3a_intf = fec_egr;
    rv = bcm_l3_host_add(unit, &l3_host);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_l3_host_add(dip) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int rv;
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
        rv = bcm_l3_host_add(unit, &l3_host);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_l3_host_add ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Delete host entry */
        rv = bcm_l3_host_delete(unit, &l3_host);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_l3_host_delete ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
}

/* Function to enable or disable the TPID on specific port */
int dnx_flexe_util_port_tpid_cfg(
    int unit,
    bcm_port_t port,
    int tpid,
    int tpid_enable)
{
    int rv;
    int tag_format_class_id = tpid_enable ? 8 : 0;
    bcm_port_tpid_class_t tpid_class;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_port_tpid_cfg";

    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    tpid_class.port = port;
    tpid_class.tpid1 = tpid;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.tag_format_class_id = tag_format_class_id;

    rv = bcm_port_tpid_class_set(unit, tpid_class);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_tpid_class_set(port %d) ****\n", proc_name, rv, port);
        return rv;
    }

    rv = bcm_port_class_set(unit, port, bcmPortClassVlanMember, port);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_class_set(port %d) ****\n", proc_name, rv, port);
        return rv;
    }

    return rv;
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
    int i, rv;
    uint32 flags = BCM_PORT_ADD_CROSS_CONNECT;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_resource_t resource;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_port_add";

    /* Add channelized flag for ILKN port creation */
    if (!is_eth_port) {
        flags |= BCM_PORT_ADD_CONFIG_CHANNELIZED;
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
    rv = bcm_port_add(unit, port_id, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_add(port %d) ****\n", proc_name, rv, port_id);
        return rv;
    }

    /* Set port speed */
    rv = bcm_port_resource_set(unit, port_id, &resource);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_resource_set(port %d) ****\n", proc_name, rv, port_id);
        return rv;
    }

    /* Enable the L1 port */
    rv = bcm_port_enable_set(unit, port_id, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(port %d) ****\n", proc_name, rv, phy_port_id);
        return rv;
    }

    /* Linkscan setting */
    rv = bcm_port_linkscan_set(unit, port_id, BCM_LINKSCAN_MODE_SW);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_linkscan_set(port %d) ****\n", proc_name, rv, phy_port_id);
        return rv;
    }

    return rv;
}

/* Funtion to create FlexE PHY */
int dnx_flexe_util_phy_port_add(
    int unit,
    bcm_port_t phy_port_id,
    int first_phy,
    int rate_mhz)
{
    int i, rv;
    int nof_lanes = rate_mhz / 50000; /* 50G per lane */
    bcm_pbmp_t phy_pbmp;

    if (dev_is_q2u == 1) {
        nof_lanes = rate_mhz / 25000; /* 25G per lane */
    } else if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_phy_port_add";

    /* Set phy_pbmp */
    BCM_PBMP_CLEAR(phy_pbmp);
    for (i = first_phy; i < (first_phy + nof_lanes); i++) {
        BCM_PBMP_PORT_ADD(phy_pbmp, i);
    }

    /* Start to create the FlexE PHY */
    rv = cint_dyn_port_add_flexe_phy_port(unit, phy_port_id, phy_pbmp, rate_mhz, phy_cl72_enable);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in cint_dyn_port_add_flexe_phy_port(port %d) ****\n", proc_name, rv, phy_port_id);
        return rv;
    }

    return rv;
}

/* Funtion to create FlexE group with the FlexE PHY(s) in array flexe_phy_ports[] */
int dnx_flexe_util_group_create(
    int unit,
    bcm_gport_t flexe_grp_id,
    int nof_pcs)
{
    int i, rv;
    uint32 flags = flexe_bypass_en ? BCM_PORT_FLEXE_BYPASS : 0;
    bcm_port_flexe_group_phy_info_t phy_info[nof_pcs];

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_group_create";

    /* Fill FlexE group PHY info */
    for (i = 0; i < nof_pcs; i++) {
        phy_info[i].port = flexe_phy_ports[i];
        phy_info[i].logical_phy_id = logical_phy_ids[i] ? logical_phy_ids[i] : flexe_phy_ports[i];
        if (flexe_bypass_en) {
            /* The logical PHY ID should be 0 for bypass mode */
            phy_info[i].logical_phy_id = 0;
            printf("**** %s: Creating FlexE BYPASS group with flag %d ****\n", proc_name, flags);
        }

        /* Disable FlexE PHY port */
        rv = bcm_port_enable_set(unit, flexe_phy_ports[i], 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to disable FlexE PHY %d ****\n", proc_name, rv, flexe_phy_ports[i]);
            return rv;
        }
    }

    /* Start to create the FlexE group */
    rv = bcm_port_flexe_group_create(unit, flexe_grp_id, flags, nof_pcs, phy_info);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_create(group %d) ****\n", proc_name, rv, flexe_grp_id);
        return rv;
    }

    /* Enable each FlexE PHY port */
    for (i = 0; i < nof_pcs; i++) {
        rv = bcm_port_enable_set(unit, flexe_phy_ports[i], 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to enable FlexE PHY %d ****\n", proc_name, rv, flexe_phy_ports[i]);
            return rv;
        }
    }

    /* Synce setting(Q2A_A0 only) */
    if (is_q2a_a0_only(unit) && !skip_synce_in_grp_creation) {
        printf("------ DBG_INFO: Starting synce setting on UNIT %d ------\n", unit);
        sal_sleep(1);
        bcm_switch_control_set(unit, bcmSwitchSynchronousPortClockSource,  flexe_phy_ports[0]);
        bcm_switch_control_set(unit, bcmSwitchSynchronousPortClockSourceDivCtrl, 2);
        sal_sleep(1);

        for (i = 0; i < nof_pcs; i++) {
            rv = bcm_port_enable_set(unit, flexe_phy_ports[i], 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to disable FlexE PHY %d ****\n", proc_name, rv, flexe_phy_ports[i]);
                return rv;
            }
            sal_sleep(1);
            rv = bcm_port_enable_set(unit, flexe_phy_ports[i], 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to enable FlexE PHY %d ****\n", proc_name, rv, flexe_phy_ports[i]);
                return rv;
            }
        }
    }

    return rv;
}

/* Alarm check after Synce setting */
int flexe_phy_all[8] = {0};
int dnx_flexe_util_synce_check(
    int unit_cen,
    int unit_dis,
    bcm_gport_t flexe_grp)
{
    bcm_port_flexe_oh_alarm_t oh_alarm_cen;
    bcm_port_flexe_oh_alarm_t oh_alarm_dis;
    int sync_error = 0;
    int max_round_allowed = 1;
    int rv, i, round_index = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_synce_check";
    printf("======== %s: Starting synce check on FlexE group %d ========\n",  proc_name, flexe_grp);

    if (!is_q2a_a0_only(unit_cen)) {
        return BCM_E_NONE;
    }

    /* Get group alarms and check synce error */
    rv = bcm_port_flexe_oh_alarm_get(unit_cen, flexe_grp, 0, &oh_alarm_cen);
    rv |= bcm_port_flexe_oh_alarm_get(unit_dis, flexe_grp, 0, &oh_alarm_dis);
    sal_sleep(3);
    rv |= bcm_port_flexe_oh_alarm_get(unit_cen, flexe_grp, 0, &oh_alarm_cen);
    rv |= bcm_port_flexe_oh_alarm_get(unit_dis, flexe_grp, 0, &oh_alarm_dis);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_alarm_get ****\n", proc_name, rv);
        return rv;
    }
    if (oh_alarm_cen.alarm_active || oh_alarm_dis.alarm_active) {
        print "======== Synce error detected ========\n";
        print oh_alarm_cen;
        print oh_alarm_dis;
        dnx_flexe_util_read_flexe_alarms(unit_cen);
        dnx_flexe_util_read_flexe_alarms(unit_dis);
        sync_error = 1;
    }

    /* Try to reset FlexE ports */
    if (sync_error) {
        bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0");
        bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0xffff");
        bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0");
        bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0xffff");
        sal_sleep(4);

        /* Get group alarms and check synce error */
        rv = bcm_port_flexe_oh_alarm_get(unit_cen, flexe_grp, 0, &oh_alarm_cen);
        rv |= bcm_port_flexe_oh_alarm_get(unit_dis, flexe_grp, 0, &oh_alarm_dis);
        sal_sleep(3);
        rv |= bcm_port_flexe_oh_alarm_get(unit_cen, flexe_grp, 0, &oh_alarm_cen);
        rv |= bcm_port_flexe_oh_alarm_get(unit_dis, flexe_grp, 0, &oh_alarm_dis);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_alarm_get ****\n", proc_name, rv);
            return rv;
        }
        if (!oh_alarm_cen.alarm_active && !oh_alarm_dis.alarm_active) {
            print "======== Synce error cleared ========\n";
            sync_error = 0;
        }
    }

    /* Active synce on each FlexE PHY for several times */
    while (sync_error && round_index < max_round_allowed) {
        for (i = 0; i < 8; i++) { 
            if (flexe_phy_ports[i]) {
                /* Active synce on each FlexE PHY */
                printf("======== Round %d : activing synce on PHY %d ========\n", round_index, flexe_phy_ports[i]);
                rv = bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSource, flexe_phy_ports[i]);
                rv |= bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSourceDivCtrl, 2);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail to set master clock ****\n", proc_name, rv);
                    return rv;
                }
                sal_sleep(2);
                rv = bcm_switch_control_set(unit_cen, bcmSwitchSynchronousPortClockSource, flexe_phy_ports[i]);
                rv |= bcm_switch_control_set(unit_cen, bcmSwitchSynchronousPortClockSourceDivCtrl, 2);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail to set backup clock ****\n", proc_name, rv);
                    return rv;
                }

                /* Reset all PHYs */
                bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0");
                bshell(unit_cen, "s FLEXEWP_TX_PORT_RSTB 0xffff");
                bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0");
                bshell(unit_dis, "s FLEXEWP_TX_PORT_RSTB 0xffff");

                sal_sleep(3);
                dnx_flexe_util_read_flexe_alarms(unit_cen);
                dnx_flexe_util_read_flexe_alarms(unit_dis);
                sal_sleep(3);

                /* Get group alarms and check synce error */
                rv = bcm_port_flexe_oh_alarm_get(unit_cen, flexe_grp, 0, &oh_alarm_cen);
                rv |= bcm_port_flexe_oh_alarm_get(unit_dis, flexe_grp, 0, &oh_alarm_dis);
                sal_sleep(3);
                rv |= bcm_port_flexe_oh_alarm_get(unit_cen, flexe_grp, 0, &oh_alarm_cen);
                rv |= bcm_port_flexe_oh_alarm_get(unit_dis, flexe_grp, 0, &oh_alarm_dis);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_alarm_get ****\n", proc_name, rv);
                    return rv;
                }

                if (!oh_alarm_cen.alarm_active && !oh_alarm_dis.alarm_active) {
                    print "======== Synce error cleared ========\n";
                    sync_error = 0;
                    break;
                }
            }
        }
        round_index++;
    }

    if (sync_error) {
        print oh_alarm_cen;
        print oh_alarm_dis;
        return BCM_E_FAIL;
    } else {
        /* Don't change synce setting while creating next FlexE group */
        skip_synce_in_grp_creation = 1;
    }
    printf("======== %s: Synce check PASSed on FlexE group %d ========\n",  proc_name, flexe_grp);

    return BCM_E_NONE;
}

/* Funtion to read FlexE alarms via registers */
int dnx_flexe_util_read_flexe_alarms(int unit)
{
        printf("======== Getting alarms on UNIT %d ========\n", unit);
        bshell(unit, "flexe core read register addr=0x14020");
        bshell(unit, "flexe core read register addr=0x14060");
        bshell(unit, "flexe core read register addr=0x140a0");
        bshell(unit, "flexe core read register addr=0x140e0");
        bshell(unit, "flexe core read register addr=0x14120");
        bshell(unit, "flexe core read register addr=0x14160");
        bshell(unit, "flexe core read register addr=0x141a0");
        bshell(unit, "flexe core read register addr=0x141e0");
        sal_sleep(2);
        print "-------------------------------------------\n";
        bshell(unit, "flexe core read register addr=0x14020");
        bshell(unit, "flexe core read register addr=0x14060");
        bshell(unit, "flexe core read register addr=0x140a0");
        bshell(unit, "flexe core read register addr=0x140e0");
        bshell(unit, "flexe core read register addr=0x14120");
        bshell(unit, "flexe core read register addr=0x14160");
        bshell(unit, "flexe core read register addr=0x141a0");
        bshell(unit, "flexe core read register addr=0x141e0");
}

/* Funtion to create FlexE client with specific client type */
int dnx_flexe_util_client_create(
    int unit,
    bcm_port_t client_id,
    flexe_port_type_t client_type,
    int speed,
    uint32 speed_flag)
{
    int i, rv;
    int channel_id = 0;
    uint32 flags = 0;
    bcm_port_if_t intf_type;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;

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

    /* Start to create the FlexE client */
    rv = bcm_port_add(unit, client_id, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_add(client %d) ****\n", proc_name, rv, client_id);
        return rv;
    }

    /* Set client speed */
    rv = dnx_flexe_util_client_speed_set(unit, client_id, speed, speed_flag);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_client_speed_set(client %d) ****\n", proc_name, rv, client_id);
        return rv;
    }

    /* Reference code for BusB L2&L3 client */
    if ((client_type == port_type_busB_L2_L3) || (client_type == port_type_1588_L2))
    {
        rv = dnx_tune_egr_ofp_rate_set(unit, client_id, speed*1000, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_tune_egr_ofp_rate_set ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_tune_scheduler_port_rates_set(unit, client_id, 0, speed*1000, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_tune_scheduler_port_rates_set ****\n", proc_name, rv);
            return rv;
        }
        rv = cint_dyn_port_dnx_port_dyanamic_add_packet_processing_properties_set(unit, client_id, BCM_SWITCH_PORT_HEADER_TYPE_ETH, BCM_SWITCH_PORT_HEADER_TYPE_ETH);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in cint_dyn_port_dnx_port_dyanamic_add_packet_processing_properties_set ****\n", proc_name, rv);
            return rv;
        }
        rv = appl_dnx_dynamic_port_create(unit, client_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in appl_dnx_dynamic_port_create ****\n", proc_name, rv);
            return rv;
        }
    }

    /* IPG and port enable */
    if (client_type == port_type_busB_L1 || client_type == port_type_busB_L2_L3 || client_type == port_type_1588_L2)
    {
        /* IPG setting on BusB client */
        rv = bcm_port_ifg_set(unit, client_id, 0, BCM_PORT_DUPLEX_FULL, 88);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_ifg_set(client %d) ****\n", proc_name, rv, client_id);
            return rv;
        }

        rv = bcm_port_enable_set(unit, client_id, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to enbale BusB client %d ****\n", proc_name, rv, client_id);
            return rv;
        }
    }

    /* VLAN setting on BusB L2&L3 clients */
    if (client_type == port_type_busB_L2_L3 || client_type == port_type_1588_L2)
    {
        rv = dnx_flexe_util_port_tpid_cfg(unit, client_id, 0x8100, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_port_tpid_cfg(port %d) ****\n", proc_name, rv, client_id);
            return rv;
        }
    }

    return rv;
}

/*  Funtion to remove a FlexE client */
int dnx_flexe_util_client_remove(
    int unit,
    bcm_port_t client_id)
{
    int i, rv;
    uint32 flags = 0;
    flexe_port_type_t client_type;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_client_remove";

    /* Parse the FlexE client type */
    if (dnx_flexe_util_port_type_verify(unit, client_id, port_type_busB_L1) == BCM_E_NONE)
    {
        client_type = port_type_busB_L1;
    }
    if (dnx_flexe_util_port_type_verify(unit, client_id, port_type_busB_L2_L3) == BCM_E_NONE)
    {
        client_type = port_type_busB_L2_L3;
    }

    /* Disable BusB client */
    if (client_type == port_type_busB_L1 || client_type == port_type_busB_L2_L3)
    {
        rv = bcm_port_enable_set(unit, client_id, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to disable client %d ****\n", proc_name, rv, client_id);
            return rv;
        }
    }

    /* Reference code for BusB L2&L3 client */
    if (client_type == port_type_busB_L2_L3)
    {
        rv = appl_dnx_dynamic_port_destroy(unit, client_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in appl_dnx_dynamic_port_destroy ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_tune_egr_ofp_rate_set(unit, client_id, 0, 1, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_tune_egr_ofp_rate_set ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_tune_scheduler_port_rates_set(unit, client_id, 0, 0, 1, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_tune_scheduler_port_rates_set ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Start to remove the FlexE client */
    rv = bcm_port_remove(unit, client_id, flags);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_remove(client %d) ****\n", proc_name, rv, client_id);
        return rv;
    }

    return rv;
}

/* Funtion to set FlexE client speed */
int dnx_flexe_util_client_speed_set(
    int unit,
    bcm_port_t client_id,
    int speed,
    uint32 speed_flag)
{
    int rv;
    bcm_port_resource_t resource;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_client_speed_set";

    if (speed <= 0) {
        printf("**** %s: skip speed setting on client %d ****\n", proc_name, client_id);
        return BCM_E_NONE;
    }

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

    /* Set port speed */
    rv = bcm_port_resource_set(unit, client_id, &resource);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_resource_set(client %d) ****\n", proc_name, rv, client_id);
        return rv;
    }

    return rv;
}

/* Funtion to set FlexE slots for both Mux&demux and overhead */
int dnx_flexe_util_calendar_slots_set(
    int unit,
    bcm_gport_t flexe_grp_id,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots)
{
    int i, rv;
    int cal_slots[nof_slots];
    uint32 cal_flags;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_calendar_slots_set";

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
    rv = bcm_port_flexe_group_cal_slots_set(unit, flexe_grp_id, cal_flags, calendar_id, nof_slots, cal_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_slots_set(mux/demux) ****\n", proc_name, rv);
        return rv;
    }

    /* Configure calendar slots for overhead
     * The client ID in overhead will be the same as logical client ID
     */
    if ((cal_flags == 0 || (cal_flags & BCM_PORT_FLEXE_TX)) && !flexe_bypass_en) {
        cal_flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_OVERHEAD;
        rv = bcm_port_flexe_group_cal_slots_set(unit, flexe_grp_id, cal_flags, calendar_id, nof_slots, cal_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_slots_set(overhead) ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
}

/* Funtion to add/remove a FlexE PHY to/from a FlexE group */
int dnx_flexe_util_group_phy_add_remove(
    int unit,
    bcm_gport_t flexe_grp_id,
    bcm_port_t flexe_phy_port,
    uint32 logical_phy_id,
    int flag_to_add)
{
    int i, rv;
    uint32 flags = 0;
    bcm_port_flexe_group_phy_info_t phy_info;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_group_phy_add_remove";

    /* Set phy info */
    phy_info.port = flexe_phy_port;
    phy_info.logical_phy_id = logical_phy_id;

    /* Start to add/remove the FlexE phy port */
    if (flag_to_add)
    {
        rv = bcm_port_flexe_group_phy_add(unit, flexe_grp_id, flags, &phy_info);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_phy_add ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        rv = bcm_port_flexe_group_phy_remove(unit, flexe_grp_id, flags, &phy_info);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_phy_remove ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
}

/* Funtion to check the link status of FlexE PHY */
int dnx_flexe_util_phy_port_link_check(
    int unit,
    bcm_port_t port)
{
    int rv;
    int link_status = BCM_PORT_LINK_STATUS_DOWN;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_phy_port_link_check";

    /* Get the link status */
    bshell(unit,"sleep 5");
    rv = bcm_port_link_status_get(unit, port, &link_status);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_link_status_get(port %d) ****\n", proc_name, rv, port);
        return rv;
    }

    if(link_status != BCM_PORT_LINK_STATUS_UP) {
        /* The link is down. Wait for 20s and check the status again */
        bshell(unit,"sleep 20");

        rv = bcm_port_link_status_get(unit, port, &link_status);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_link_status_get(port %d) ****\n", proc_name, rv, port);
            return rv;
        }

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
    int i, rv;
    uint32 flags = 0;
    bcm_port_t client_b_L1 = 98;
    bcm_port_t client_b_L2 = 99;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_datapath_L2_eth_to_L1_eth";

    if (!clear_flag) {
        /* Create BusB L1 client */
        rv = dnx_flexe_util_client_create(unit, client_b_L1, port_type_busB_L1, bandwidth, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
            return rv;
        }

        /* Create BusB L2 client */
        rv = dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, bandwidth, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Disable BusB clients */
    rv = bcm_port_enable_set(unit, client_b_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, client_b_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable client_b_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, eth_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable eth_port_L1 ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear below L2_L3_ETH to L1_ETH datapath:
     * eth_port_L2 -> force_fwd -> client_b_L2 -> client_b_L1 -> L1_switch -> eth_port_L1
     */
    if (!clear_flag)
    {
        /** Create above datapath **/
        rv = bcm_port_force_forward_set(unit, eth_port_L2, client_b_L2, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in force_fwd setting(eth_port_L2 to client_b_L2) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_b_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L2 to client_b_L1) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, eth_port_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L1 to eth_port_L1) ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /** Clear above datapath **/
        rv = bcm_port_force_forward_set(unit, eth_port_L2, client_b_L2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in force_fwd setting(eth_port_L2 to client_b_L2) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_flexe_flow_clear(unit, client_b_L2, 0, client_b_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L2 to client_b_L1) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_flexe_flow_clear(unit, client_b_L1, 0, eth_port_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L1 to eth_port_L1) ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB clients */
    rv = bcm_port_enable_set(unit, client_b_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, client_b_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable client_b_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, eth_port_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable eth_port_L1 ****\n", proc_name, rv);
        return rv;
    }

    /* Remove BusB clients */
    if (clear_flag) {
        rv = dnx_flexe_util_client_remove(unit, client_b_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_remove(client %d) ****\n", proc_name, rv, client_b_L1);
            return rv;
        }
        rv = dnx_flexe_util_client_remove(unit, client_b_L2);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_remove(client %d) ****\n", proc_name, rv, client_b_L2);
            return rv;
        }
    }

    return rv;
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
    int i, rv;
    uint32 flags = 0;
    bcm_port_t client_b_L1 = 98;
    bcm_port_t client_b_L2 = 99;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_datapath_L1_eth_to_L2_eth";

    if (!clear_flag) {
        /* Create BusB L1 client */
        rv = dnx_flexe_util_client_create(unit, client_b_L1, port_type_busB_L1, bandwidth, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
            return rv;
        }

        /* Create BusB L2 client */
        rv = dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, bandwidth, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Disable BusB clients */
    rv = bcm_port_enable_set(unit, client_b_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, client_b_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable client_b_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, eth_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable eth_port_L1 ****\n", proc_name, rv);
        return rv;
    }
    if (dst_clientB_ori > 0) {
        /* Disable the original dst_client in BusB */
        rv = bcm_port_enable_set(unit, dst_clientB_ori, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to disable dst_clientB_ori ****\n", proc_name, rv);
            return rv;
        }
    }

    /*
     * Start to create/clear below L1_ETH to L2_L3_ETH datapath:
     * eth_port_L1 -> L1_switch -> client_b_L1 -> client_b_L2 > force_fwd -> eth_port_L2
     */
    if (!clear_flag)
    {
        /* Clear original Eth2BusB FlexE flow */
        if (dst_clientB_ori > 0) {
            rv = bcm_port_flexe_flow_clear(unit, eth_port_L1, 0, dst_clientB_ori);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to clear original Eth2BusB FlexE flow ****\n", proc_name, rv);
                return rv;
            }
        }

        /** Create above datapath **/
        rv = bcm_port_flexe_flow_set(unit, flags, eth_port_L1, 0, client_b_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L2 to client_b_L1) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, client_b_L2);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L1 to client_b_L2) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_force_forward_set(unit, client_b_L2, eth_port_L2, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in force_fwd setting(client_b_L2 to eth_port_L2) ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /** Clear above datapath **/
        rv = bcm_port_flexe_flow_clear(unit, eth_port_L1, 0, client_b_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L2 to client_b_L1) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_flexe_flow_clear(unit, client_b_L1, 0, client_b_L2);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE flow(client_b_L1 to client_b_L2) ****\n", proc_name, rv);
            return rv;
        }
        rv = bcm_port_force_forward_set(unit, client_b_L2, eth_port_L2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in force_fwd setting(client_b_L2 to eth_port_L2) ****\n", proc_name, rv);
            return rv;
        }

        /** Restore original Eth2BusB FlexE flow **/
        if (dst_clientB_ori > 0) {
            rv = bcm_port_flexe_flow_set(unit, flags, eth_port_L1, 0, dst_clientB_ori);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to restore original Eth2BusB FlexE flow ****\n", proc_name, rv);
                return rv;
            }
        }
    }

    /* Enable BusB clients */
    rv = bcm_port_enable_set(unit, client_b_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, client_b_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable client_b_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_enable_set(unit, eth_port_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable eth_port_L1 ****\n", proc_name, rv);
        return rv;
    }
    if (dst_clientB_ori > 0) {
        /* Enable original dst_client in BusB */
        rv = bcm_port_enable_set(unit, dst_clientB_ori, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to enable dst_clientB_ori ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Remove BusB clients */
    if (clear_flag) {
        rv = dnx_flexe_util_client_remove(unit, client_b_L1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_remove(client %d) ****\n", proc_name, rv, client_b_L1);
            return rv;
        }
        rv = dnx_flexe_util_client_remove(unit, client_b_L2);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_remove(client %d) ****\n", proc_name, rv, client_b_L2);
            return rv;
        }
    }

    return rv;
}

/* Function to set/clear client to client L1 FlexE flow */
int dnx_flexe_util_L1_datapath_client_to_client(
    int unit,
    bcm_port_t src_client_id,
    bcm_port_t dst_client_id,
    int clear_flag)
{
    int i, rv;
    int not_busB_L1 = 1; /* If 0, indicate the client is BusB L1 client */
    int not_busB_L2 = 1; /* If 0, indicate the client is BusB L2&L3 client */
    uint32 flags = 0;
    bcm_port_t client_id[2] = {src_client_id, dst_client_id};

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_client_to_client";

    /* Disable BusB client before FlexE flow setting&clearing */
    for (i = 0; i < 2; i++) {
        not_busB_L1 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L1);
        not_busB_L2 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L2_L3);
        if (not_busB_L1 == 0 || not_busB_L2 == 0) {
            /* Disbale the port if it is BusB L1/L2/L3 client */
            rv = bcm_port_enable_set(unit, client_id[i], 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to disable BusB client %d ****\n", proc_name, rv, client_id[i]);
                return rv;
            }
        }
    }

    /* Create or clear FlexE client to client L1 switch within FlexE core */
    if (!clear_flag)
    {
        /* Set FlexE client to client flow */
        rv = bcm_port_flexe_flow_set(unit, flags, src_client_id, failover_channel_id, dst_client_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting FlexE2FlexE L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above client to client datapath */
        rv = bcm_port_flexe_flow_clear(unit, src_client_id, failover_channel_id, dst_client_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing FlexE2FlexE L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB client after FlexE flow setting&clearing */
    for (i = 0; i < 2; i++) {
        not_busB_L1 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L1);
        not_busB_L2 = dnx_flexe_util_port_type_verify(unit, client_id[i], port_type_busB_L2_L3);
        if (not_busB_L1 == 0 || not_busB_L2 == 0) {
            /* Enable the port if it is BusB L1/L2/L3 client */
            rv = bcm_port_enable_set(unit, client_id[i], 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to enable BusB client %d ****\n", proc_name, rv, client_id[i]);
                return rv;
            }
        }
    }

    return rv;
}

/* Function to set/clear FlexE to ETH L1 datapath */
int dnx_flexe_util_L1_datapath_flexe_to_eth(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busB,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_flexe_to_eth";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    rv |= bcm_port_enable_set(unit, dst_eth_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable BusB client and ETH port ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L1 datapath: FlexE -> ETH
     * The internal datapath is:
     * FlexE(client_id_busA) -> TinyMAC(client_id_busB) -> ESB(From_FlexE) -> ETH(dst_eth_port)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from FlexE to TinyMAC(client_id_busA -> client_id_busB) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busA, failover_channel_id, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusA2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from TinyMAC to ETH(client_id_busB -> ESB(From_FlexE) -> dst_eth_port) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, dst_eth_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2Eth L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above FlexE to ETH port */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, 0, dst_eth_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2Eth L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, client_id_busA, failover_channel_id, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusA2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    rv |= bcm_port_enable_set(unit, dst_eth_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable BusB client and ETH port ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear ETH to FlexE L1 datapath */
int dnx_flexe_util_L1_datapath_eth_to_flexe(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_eth_to_flexe";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    rv |= bcm_port_enable_set(unit, src_eth_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable BusB client and ETH port ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L1 datapath: ETH -> FlexE
     * The internal datapath is:
     * ETH(src_eth_port) -> IRE -> TinyMAC(client_id_busB) -> FlexE(client_id_busA)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from ETH to TinyMAC(src_eth_port -> IRE -> client_id_busB) **/
        rv = bcm_port_flexe_flow_set(unit, flags, src_eth_port, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting Eth2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from TinyMAC to FlexE(client_id_busB -> client_id_busA) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, failover_channel_id, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L1 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, failover_channel_id, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, src_eth_port, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing Eth2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    rv |= bcm_port_enable_set(unit, src_eth_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable BusB client and ETH eth port ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear FlexE to ILKN L1 datapath */
int dnx_flexe_util_L1_datapath_flexe_to_ilkn(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busC,
    bcm_port_t dst_ilkn_port,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_flexe_to_ilkn";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, dst_ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable dst_ilkn_port ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L1 datapath: FlexE -> ILKN
     * The internal datapath is:
     * FlexE(client_id_busA) -> SAR(client_id_busC) -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from FlexE to SAR(client_id_busA -> client_id_busC) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busA, failover_channel_id, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusA2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from SAR to ILKN(client_id_busC -> dst_ilkn_port) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busC, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusC2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L1 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busC, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusC2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, client_id_busA, failover_channel_id, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusA2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, dst_ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable dst_ilkn_port ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear ILKN to FlexE L1 datapath */
int dnx_flexe_util_L1_datapath_ilkn_to_flexe(
    int unit,
    bcm_port_t src_ilkn_port,
    bcm_port_t client_id_busC,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_ilkn_to_flexe";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, src_ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable src_ilkn_port ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L1 datapath: ILKN -> FlexE
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> IRE -> SAR(client_id_busC) -> FlexE(client_id_busA)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from ILKN to SAR(src_ilkn_port -> IRE -> client_id_busC) **/
        rv = bcm_port_flexe_flow_set(unit, flags, src_ilkn_port, 0, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting ILKN2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from SAR to FlexE(client_id_busC -> client_id_busA) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busC, failover_channel_id, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusC2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L1 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busC, failover_channel_id, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusC2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, src_ilkn_port, 0, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing ILKN2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, src_ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable src_ilkn_port ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_eth_to_ilkn";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    rv |= bcm_port_enable_set(unit, src_eth_port, 0);
    rv |= bcm_port_enable_set(unit, dst_ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable ports ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L1 datapath: ETH -> ILKN
     * The internal datapath is:
     * ETH(src_eth_port) -> IRE -> TinyMAC(client_id_busB) -> SAR(client_id_busC) -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from ETH to TinyMAC(src_eth_port -> IRE -> client_id_busB) **/
        rv = bcm_port_flexe_flow_set(unit, flags, src_eth_port, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting Eth2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from TinyMAC to SAR(client_id_busB -> client_id_busC) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, failover_channel_id, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step3. set L1 datapath from SAR to ILKN(client_id_busC -> dst_ilkn_port) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busC, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusC2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L1 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busC, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusC2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, failover_channel_id, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, src_eth_port, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing Eth2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    rv |= bcm_port_enable_set(unit, src_eth_port, 1);
    rv |= bcm_port_enable_set(unit, dst_ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable ports ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L1_datapath_ilkn_to_eth";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busC, port_type_busC);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    rv |= bcm_port_enable_set(unit, src_ilkn_port, 0);
    rv |= bcm_port_enable_set(unit, dst_eth_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable ports ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L1 datapath: ILKN -> ETH
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> IRE -> SAR(client_id_busC) -> TinyMAC(client_id_busB) -> ESB(From_FlexE) -> ETH
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from ILKN to SAR(src_ilkn_port -> IRE -> client_id_busC) **/
        rv = bcm_port_flexe_flow_set(unit, flags, src_ilkn_port, 0, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting ILKN2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from SAR to TinyMAC(client_id_busC -> client_id_busB) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busC, failover_channel_id, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusC2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step3. set L1 datapath from TinyMAC to ETH(client_id_busB -> ESB(From_FlexE) -> dst_eth_port) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, dst_eth_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2Eth L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L1 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, 0, dst_eth_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2Eth L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, client_id_busC, failover_channel_id, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusC2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, src_ilkn_port, 0, client_id_busC);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing ILKN2BusC L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    rv |= bcm_port_enable_set(unit, src_ilkn_port, 1);
    rv |= bcm_port_enable_set(unit, dst_eth_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enbale ports ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear FlexE to ILKN L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busB,
    bcm_port_t dst_ilkn_port,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    rv |= bcm_port_enable_set(unit, dst_ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable ports ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L2&L3 datapath: FlexE -> ILKN
     * The internal datapath is:
     * FlexE(client_id_busA) -> flow_set -> TinyMAC(client_id_busB) -> flow_set -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. set L1 datapath from FlexE to TinyMAC(client_id_busA -> flow_set -> client_id_busB) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busA, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusA2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from BusB to ILKN(client_id_busB -> flow_set -> dst_ilkn_port **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear FlexE -> ILKN L2/L3 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, client_id_busA, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusA2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    rv |= bcm_port_enable_set(unit, dst_ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable ports ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear ILKN to FlexE L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(
    int unit,
    bcm_port_t src_ilkn_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, src_ilkn_port, 0);
    rv |= bcm_port_enable_set(unit, client_id_busB, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable ports ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L2&L3 datapath: ILKN -> FlexE
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> force_forward -> TinyMAC(client_id_busB) -> FlexE(client_id_busA)
     */
    if (!clear_flag)
    {
        /** Step1. Force foward all packets on src_ilkn_port to TinyMAC(client_id_busB) **/
        rv = bcm_port_force_forward_set(unit, src_ilkn_port, client_id_busB, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(ILKN to TinyMAC) ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. set L1 datapath from TinyMAC to FlexE(client_id_busB -> client_id_busA) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L2&L3 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, 0, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_force_forward_set(unit, src_ilkn_port, client_id_busB, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(ILKN to TinyMAC) ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, src_ilkn_port, 1);
    rv |= bcm_port_enable_set(unit, client_id_busB, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable ports ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_eth_to_ilkn";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB0, port_type_busB_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB1, port_type_busB_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_ilkn_port, port_type_ilkn_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* Disable ports*/
    rv = bcm_port_enable_set(unit, client_id_busB0, 0);
    rv |= bcm_port_enable_set(unit, client_id_busB1, 0);
    rv |= bcm_port_enable_set(unit, dst_ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable ports ****\n", proc_name, rv);
        return rv;
    }

    /*
     * Start to create/clear L2&L3 datapath: ETH -> ILKN
     * The internal datapath is:
     * ETH(src_eth_port) -> force_fwd -> TinyMAC(client_id_busB0) -> TinyMAC(client_id_busB1) -> flow_set -> ILKN(dst_ilkn_port)
     */
    if (!clear_flag)
    {
        /** Step1. Force foward all packets on src_eth_port to TinyMAC(client_id_busB0) **/
        rv = bcm_port_force_forward_set(unit, src_eth_port, client_id_busB0, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(ETH to TinyMAC) ****\n", proc_name, rv);
            return rv;
        }

        /** Step2. Switch the traffic from BusB Tx to BusB Rx(client_id_busB0 -> client_id_busB1) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB0, 0, client_id_busB1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /** Step3. set L1 datapath from TinyMAC to ILKN(client_id_busB1 -> flow_set -> dst_ilkn_port) **/
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB1, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear above L2/L3 datapath */
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB1, 0, dst_ilkn_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_flexe_flow_clear(unit, client_id_busB0, 0, client_id_busB1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_force_forward_set(unit, src_eth_port, client_id_busB0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(ETH to TinyMAC) ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, client_id_busB0, 1);
    rv |= bcm_port_enable_set(unit, client_id_busB1, 1);
    rv |= bcm_port_enable_set(unit, dst_ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable ports ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear ILKN to ETH L2&L3 datapath */
int dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_eth(
    int unit,
    bcm_port_t src_ilkn_port,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_eth";


    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, src_ilkn_port, port_type_ilkn_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /*
     * Start to create/clear L2&L3 datapath for ILKN -> ETH
     * The internal datapath is:
     * ILKN(src_ilkn_port) -> IRE -> Q2A PP -> ETH(dst_eth_port)
     */

    /** Step1. Force foward all packets on src_ilkn_port to dst_eth_port **/
    rv = bcm_port_force_forward_set(unit, src_ilkn_port, dst_eth_port, !clear_flag);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(ILKN to ETH) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear FlexE to ETH L2 datapath */
int dnx_flexe_util_L2_datapath_flexe_to_eth(
    int unit,
    bcm_port_t client_id_busA,
    bcm_port_t client_id_busB,
    bcm_port_t dst_eth_port,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_datapath_flexe_to_eth";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, dst_eth_port, port_type_eth_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* The L2 datapath is:
     * FlexE(client_id_busA) -> TinyMAC(client_id_busB) -> force_forward -> ETH(dst_eth_port) */

    /* Disable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable BusB client %d ****\n", proc_name, rv, client_id_busB);
        return rv;
    }

    /* Step1. Set or clear L1 datapath from FlexE to TinyMAC(client_id_busA -> client_id_busB) */
    if (!clear_flag)
    {
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busA, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusA2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        rv = bcm_port_flexe_flow_clear(unit, client_id_busA, 0, client_id_busB);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusA2BusB L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable BusB client %d ****\n", proc_name, rv, client_id_busB);
        return rv;
    }

    /* Step2. Set L2 datapath from TinyMAC to ETH(client_id_busB -> force_forward -> dst_eth_port) */
    rv = bcm_port_force_forward_set(unit, client_id_busB, dst_eth_port, !clear_flag);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(BusB2Eth) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to set/clear ETH to FlexE L2 datapath */
int dnx_flexe_util_L2_datapath_eth_to_flexe(
    int unit,
    bcm_port_t src_eth_port,
    bcm_port_t client_id_busB,
    bcm_port_t client_id_busA,
    int clear_flag)
{
    int i, rv;
    uint32 flags = 0;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_util_L2_datapath_eth_to_flexe";

    /* Port type check */
    rv = dnx_flexe_util_port_type_verify(unit, src_eth_port, port_type_eth_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busB, port_type_busB_L2_L3);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_port_type_verify(unit, client_id_busA, port_type_busA);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Mismached port type ****\n", proc_name);
        return rv;
    }

    /* The L2 datapath is:
     * ETH(src_eth_port) -> force_forward -> TinyMAC(client_id_busB) -> FlexE(client_id_busA) */

    /* Disable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable BusB client %d ****\n", proc_name, rv, client_id_busB);
        return rv;
    }

    /* Step1. Set or clear L1 datapath from TinyMAC to FlexE(client_id_busB -> client_id_busA) */
    if (!clear_flag)
    {
        rv = bcm_port_flexe_flow_set(unit, flags, client_id_busB, 0, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in setting BusB2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        rv = bcm_port_flexe_flow_clear(unit, client_id_busB, 0, client_id_busA);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in clearing BusB2BusA L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB client */
    rv = bcm_port_enable_set(unit, client_id_busB, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable BusB client %d ****\n", proc_name, rv, client_id_busB);
        return rv;
    }

    /* Step2. Set or clear L2 datapath from ETH to TinyMAC */
    rv = bcm_port_force_forward_set(unit, src_eth_port, client_id_busB, !clear_flag);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(Eth2BusB) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Basic FlexE funtion test to verify bi-direction FlexE traffic */
int dnx_flexe_baisc_test_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_port_cen,
    bcm_port_t eth_port_dis,
    bcm_port_t phy_port)
{
    int i, ii, rv;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 40;
    bcm_port_t client_b = 50;
    bcm_gport_t flexe_grp;
    int flexe_grp_bandwidth = 100000;
    int client_speed = 100000;
    int nof_client_slots = client_speed / 5000;
    int nof_slots = flexe_grp_bandwidth / 5000;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_baisc_test_main";

    /***************************************************************************
     * FlexE PHY port(s) and group creation
 */

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create a busA client and a busB client on each Q2A. The client rate is 10G */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_client_create(unit[i], client_a, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        rv = dnx_flexe_util_client_create(unit[i], client_b, port_type_busB_L1, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * Datapath creation:
     *                          Centralized Device          Centralized Device
     *                      +-----------------------+    +-----------------------+
     * eth_port_cen <--L1-->| client_b <-> client_a |<-->| client_a <-> client_b |<--L1--> eth_port_dis
     *                      +-----------------------+    +-----------------------+
 */

    /* Start to build below L1 ETH2FlexE datapath on centralized device:
     * eth_port_cen <-> client_b <-> client_a
     */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_cen, client_b, client_a, 0);
    rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port_cen, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L1 datapath on Q2A_#0 ****\n", proc_name, rv);
        return rv;
    }

    /* Start to build below L1 datapath on distributed device:
     * client_a <-> client_b <-> eth_port_dis
     */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_dis, client_b, client_a, 0);
    rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_port_dis, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#1 ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */
    for (i = 0; i < nof_client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_centralized_datapath_flexe_to_flexe_in_1_grp_main";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * FlexE PHY port(s) and group creation
 */

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    if (dev_is_q2u) {
        flexe_phy_ports[1] = phy_port+1;
        nof_pcs++;
    }
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create 2 BusA clients and 2 BusB clients on each Q2A. The rate of each client is 10G */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_client_create(unit[i], client_a_0, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_a_0);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit[i], client_a_1, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_a_1);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit[i], client_b_0, client_b_types[i], client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_b_0);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit[i], client_b_1, client_b_types[i], client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_b_1);
            return rv;
        }
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
 */

    /* Start to build below L1 Eth2FlexE datapath on distributed Device:
     * eth_port_tx -> client_b_0 -> client_a_0
     */
    if (fwd_type == 1)
    {
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Start to build below FlexE2FlexE datapath on centralized device:
     * client_a_0 -> L1/L2/l3 -> client_a_1
     */
    if (fwd_type == 1)
    {
        /* Testing Flexe2Flexe L1 switch: The FlexE to FlexE switching is done in FlexE core */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_a_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath in unit %d ****\n", proc_name, rv, unit_cen);
            return rv;
        }
    }
    else
    {
        /* Testing Flexe2Flexe L2/L3 switch. The datapath will be:
         * client_a_0 -> client_b_0 -> L2/L3 -> client_b_1 -> client_a_1
         * Now start to configure L1 datapath "client_a_0->client_b_0" and "client_b_1->client_a_1"
         */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_b_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set BusA2BusB L1 datapath on centralized Q2A ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_1, client_a_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set BusB2BusA L1 datapath on centralized Q2A ****\n", proc_name, rv);
            return rv;
        }

        /* Start to do L2&L3 setting */
        if (fwd_type == 2) {
            /* L2 setting between client_b_0 and client_b_1 */
            rv = dnx_flexe_util_L2_switch_config(unit_cen, client_b_0, client_b_1, vlan_in);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L2_switch_config ****\n", proc_name, rv);
                return rv;
            }
        } else {
            /* L3 setting between client_b_0 and client_b_1 */
            rv = dnx_flexe_util_L3_route_config(unit_cen, client_b_0, client_b_1, vlan_in, vlan_out);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L3_route_config ****\n", proc_name, rv);
                return rv;
            }
        }
    }

    /* Start to build below L1 FlexE2Eth datapath on distributed Device:
     * client_a_1 -> client_b_1 -> eth_port_rx
     */
    if (fwd_type == 1)
    {
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots_per_client*2; ii++) {
            Cal_Slots_A[ii] = ii < nof_slots_per_client ? client_a_0 : client_a_1;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_centralized_L1_datapath_flexe_to_flexe_across_2_grps_main";

    /***************************************************************************
     * FlexE PHY port(s) and group creation
 */

    /* Create 2 FlexE groups with the 2 FlexE PHYs on each device */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[0], 1);
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[1], 2);
    for (i = 0; i < 2; i++) {
        for (ii = 0; ii < 2; ii++) {
            flexe_phy_ports[0] = phy_port[ii];
            rv = dnx_flexe_util_group_create(unit[i], flexe_grp[ii], nof_pcs);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create FlexE FG(unit %d, grp index %d) ****\n", proc_name, rv, unit[i], ii);
                return rv;
            }
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create 2 BusA clients and 2 BusB clients on each Q2A. The rate of each client is 10G */
    for (i = 0; i < 2; i++) {
        /* client_a_0 for flexe_grp[0] */
        rv = dnx_flexe_util_client_create(unit[i], client_a_0, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_a_0);
            return rv;
        }
        /* client_a_1 for flexe_grp[1] */
        rv = dnx_flexe_util_client_create(unit[i], client_a_1, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_a_1);
            return rv;
        }
        /* client_b_0 which is associated with client_a_0 */
        rv = dnx_flexe_util_client_create(unit[i], client_b_0, client_b_types[i], client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_b_0);
            return rv;
        }
        /* client_b_1 which is associated with client_a_1 */
        rv = dnx_flexe_util_client_create(unit[i], client_b_1, client_b_types[i], client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_b_1);
            return rv;
        }
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
 */

    /* Start to build below L1 Eth2FlexE datapath on distributed device:
     * eth_port_tx -> client_b_0 -> client_a_0
     */
    if (fwd_type ==  1)
    {
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_port_tx, client_b_0, client_a_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Start to build below FlexE2FlexE datapath on centralized device:
     * client_a_0 -> L1/L2/l3 -> client_a_1
     */
    if (fwd_type ==  1)
    {
        /* L1 switch: The FlexE to FlexE switching is done in FlexE core */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_a_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath in unit %d ****\n", proc_name, rv, unit_cen);
            return rv;
        }
    }
    else
    {
        /* L2/L3 switch: the FlexE to FlexE switching will be
         * client_a_0 -> client_b_0 -> L2/L3 -> client_b_1 -> client_a_1
         * Now start to configure L1 datapath "client_a_0->client_b_0" and "client_b_1->client_a_1" */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_0, client_b_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set BusA2BusB L1 datapath on centralized Q2A ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_1, client_a_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set BusB2BusA L1 datapath on centralized Q2A ****\n", proc_name, rv);
            return rv;
        }

        /* Start to do L2&L3 setting */
        if (fwd_type == 2) {
            /* L2 setting between client_b_0 and client_b_1 */
            rv = dnx_flexe_util_L2_switch_config(unit_cen, client_b_0, client_b_1, vlan_in);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L2_switch_config ****\n", proc_name, rv);
                return rv;
            }
        } else {
            /* L3 setting between client_b_0 and client_b_1 */
            rv = dnx_flexe_util_L3_route_config(unit_cen, client_b_0, client_b_1, vlan_in, vlan_out);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L3_route_config ****\n", proc_name, rv);
                return rv;
            }
        }
    }

    /* Start to build below L1 FlexE2Eth datapath on on distributed device:
     * client_a_1 -> client_b_1 -> eth_port_rx
     */
    if (fwd_type ==  1)
    {
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_1, client_b_1, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for client_a_0 in flexe_grp[0] */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots_per_client; ii++) {
            Cal_Slots_A[ii] =  client_a_0;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp[0], 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to allocate slots for client_a_0 on unit %d ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* Assign slots for client_a_1 in flexe_grp[1] */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots_per_client; ii++) {
            Cal_Slots_A[ii] =  client_a_1;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp[1], 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to allocate slots for client_a_1 on unit %d ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH group ID */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp[0], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        rv |= bcm_port_flexe_oh_set(unit[i], flexe_grp[1], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 2);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp[0]);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp[1]);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_port[0]);
        if (rv != BCM_E_NONE) {
            printf("**** %s: The 1st FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }

        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_port[1]);
        if (rv != BCM_E_NONE) {
            printf("**** %s: The 2nd FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_centralized_datapath_between_eth_and_flexe_main";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * The whole datapath is:
     *                             Centralized Device              Centralized Device
     *                          +------------------------+      +---------------------+
     * eth_port_tx --L1/L2/l3-> | client_b_0->client_a_0 |----->| client_a_0------>+  |
     *                          |                        |      |                  |  |
     * eth_port_rx <-L1/L2/l3-- | client_b_1->client_a_1 |<-----| client_a_1<------+  |
     *                          +------------------------+      +---------------------+
 */

    /***************************************************************************
     * FlexE PHY port(s) and group creation
 */

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    if (dev_is_q2u) {
        flexe_phy_ports[1] = phy_port + 1;
        nof_pcs = 2;
    }
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create 2 BusA clients on each device. The rate of each client is 10G */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_client_create(unit[i], client_a_0, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_a_0);
            return rv;
        }

        rv = dnx_flexe_util_client_create(unit[i], client_a_1, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
                   proc_name, rv, unit[i], client_a_1);
            return rv;
        }
    }

    /* Create 2 BusB clients on centralized device */
    rv = dnx_flexe_util_client_create(unit_cen, client_b_0, client_b_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
               proc_name, rv, unit_cen, client_b_0);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_1, client_b_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create(unit %d, client %d) ****\n",
               proc_name, rv, unit_cen, client_b_1);
        return rv;
    }

    /***************************************************************************
     * Datapath creation:
 */

    /* Start to build below Eth2Flexe datapath on centralized device:
     * eth_port_tx -> L1/L2/L3 switch -> client_b_0 -> client_a_0
     */
    if (is_L1_switch)
    {
        /* Use Eth2FlexE L1 datapath */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b_0, client_a_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Use Eth2FlexE L2&L3 datapath:
         * eth_port_tx -> L2/L3 switch -> client_b_0 -> client_a_0
         * For L2/L3 switching between Ethernet and BusB, it will be done in TCL script
         */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_0, client_a_0, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set BusB2BusA L2 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Start to build L1 FlexE2FlexE datapath on distributed device:
     * client_a_0 -> L1_switch -> client_a_1
     */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_0, client_a_1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath on Q2A_#1 ****\n", proc_name, rv);
        return rv;
    }

    /* Start to build below Flexe2Eth datapath on centralized device:
     * client_a_1 -> client_b_1-> L1/L2/L3 switch -> eth_port_rx
     */
    if (is_L1_switch)
    {
        /* Use Flexe2Eth L1 datapath */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_1, client_b_1, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Use Flexe2Eth L2&L3 datapath:
         * client_a_1 -> client_b_1-> L2/L3 switch -> eth_port_rx
         * For L2/L3 switching between BusB and Ethernet, it will be done in TCL script
         */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_1, client_b_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set BusA2BusB L2 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots_per_client*2; ii++) {
            Cal_Slots_A[ii] = ii < nof_slots_per_client ? client_a_0 : client_a_1;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* Group ID OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_distributed_datapath_between_flexe_and_ilkn_main";

    /***************************************************************************
     * FlexE PHY port(s) and group creation
 */

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create a busA client and a busB client on each Q2A. The client rate is 10G */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_client_create(unit[i], client_a, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit[i], client_b, client_b_type[i], client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Create busC client on distributed Q2A */
    rv = dnx_flexe_util_client_create(unit_dis, client_c, port_type_busC, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busC client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

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
 */

    if (is_L1_switch_cen)
    {
        /* Build Eth2FlexE L1 datapath on centralized Device */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }

        /* Build FlexE2Eth L1 datapath on centralized Device */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Build Eth2FlexE L1 datapath on centralized Device */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }

        /* Build FlexE2Eth L1 datapath on centralized Device */
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Build Flexe2ILKN and ILKN2Flexe L1/L2/L3 datapath on distributed Device */
    if (is_L1_switch_dis)
    {
        /* Start to build Flexe2ILKN L1 datapath:
         * client_a -> client_c -> ilkn_port
         */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a, client_c, ilkn_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Flexe2ILKN L1 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }

        /* Start to build ILKN2Flexe L1 datapath:
         * ilkn_port -> client_c -> client_a
         */
        rv = dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_c, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set ILKN2Flexe L1 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Start to build Flexe2ILKN L2/L3 datapath:
         * client_a -> client_b -> ilkn_port
         */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a, client_b, ilkn_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Flexe2ILKN datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }

        /* Start to build ILKN2Flexe L2/L3 datapath:
         * ilkn_port -> force_fwd -> client_b -> client_a
         */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set ILKN2Flexe L2/L3 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots_per_client; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_port);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
}

/* Funtion to test L1/L2/L3 datapath between ETH and ILKN */
int dnx_flexe_distributed_datapath_between_eth_and_ilkn_main(
    int unit,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    bcm_port_t ilkn_port,
    int is_L1_switch)
{
    int i, rv;
    int client_speed = 100000;
    bcm_port_t client_b_0 = 51;
    bcm_port_t client_b_1 = 52;
    bcm_port_t client_c = 53;
    flexe_port_type_t client_b0_type = is_L1_switch ? port_type_busB_L1 : port_type_busB_L2_L3;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_distributed_datapath_between_eth_and_ilkn_main";

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create client_b_0 */
    rv = dnx_flexe_util_client_create(unit, client_b_0, client_b0_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_0 ****\n", proc_name, rv);
        return rv;
    }

    /* Create client_b_1 */
    rv = dnx_flexe_util_client_create(unit, client_b_1, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_1 ****\n", proc_name, rv);
        return rv;
    }

    /* Create client_c */
    rv = dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Datapath creation
 */

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
        rv = dnx_flexe_util_L1_datapath_eth_to_ilkn(unit, eth_port_tx, client_b_0, client_c, ilkn_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2ILKN L1 datapath ****\n", proc_name, rv);
            return rv;
        }

        /* Start to build below ILKN2Eth L1 datapath:
         * ilkn_port -> client_c -> client_b_0 -> eth_port_rx
         */
        rv = dnx_flexe_util_L1_datapath_ilkn_to_eth(unit, ilkn_port, client_c, client_b_0, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set ILKN2Eth L1 datapath ****\n", proc_name, rv);
            return rv;
        }
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
        rv = dnx_flexe_util_distributed_L2_L3_datapath_eth_to_ilkn(unit, eth_port_tx, client_b_0, client_b_1, ilkn_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2ILKN L2/l3 datapath ****\n", proc_name, rv);
            return rv;
        }

        /* Start to build below ILKN2Eth L2/L3 datapath:
         * ilkn_port -> force_fwd -> eth_port_rx
         */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_eth(unit, ilkn_port, eth_port_rx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set ILKN2Eth L2/L3 datapath ****\n", proc_name, rv);
            return rv;
        }

        rv = bcm_port_loopback_set(unit, ilkn_port, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), failed to set loopback on ilkn port %d ****\n", proc_name, rv, ilkn_port);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 41;
    bcm_port_t client_b = 42;
    bcm_gport_t flexe_grp;
    int nof_slots = grp_speed / 5000;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_group_bandwidth_test_with_specific_phys_main";

    /***************************************************************************
     * The whole datapath is:
     *                            Centralized Device         Centralized Device
     *                          +---------------------+    +---------------------+
     * eth_400g_cen<-L2_switch->| client_b<->client_a |<-->| client_a<->client_b |<-L2_switch->eth_400g_dis(self looped)
     *                          +---------------------+    +---------------------+
 */

    /***************************************************************************
     * FlexE configuration on centralized device
 */

    /* Create FlexE group with the provided FlexE PHYs in array flexe_phy_ports[] */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_phy);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a, port_type_busA, grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b, port_type_busB_L2_L3, grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create ETH2FlexE L2 datapath: eth_400g_cen -> client_b -> client_a */
    rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_400g_cen, client_b, client_a, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE2Eth L2 datapath: eth_400g_cen <- client_b <- client_a */
    rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_400g_cen, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L2 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Calendar setting */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* FlexeOhGroupID setting */
    rv = bcm_port_flexe_oh_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /***************************************************************************
     * FlexE configuration on distributed device
 */

    /* Create FlexE group with the provided FlexE PHYs in array flexe_phy_ports[] */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_phy);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a, port_type_busA, grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b, port_type_busB_L2_L3, grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create ETH2FlexE L2 datapath: eth_400g_dis -> client_b -> client_a */
    rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_400g_dis, client_b, client_a, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE2Eth L2 datapath: eth_400g_dis <- client_b <- client_a */
    rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_400g_dis, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L2 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Calendar setting */
    rv = dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* FlexeOhGroupID setting */
    rv = bcm_port_flexe_oh_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Verify link status of FlexE PHY(s) */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy_ports[0]);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, ii, rv;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 41;
    bcm_port_t client_b = 42;
    bcm_gport_t flexe_grp;
    int nof_slots = grp_speed / 5000;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_group_bandwidth_test_with_specific_phys_clear";

    /***************************************************************************
     * The whole datapath is:
     *                            Centralized Device         Centralized Device
     *                          +---------------------+    +---------------------+
     * eth_400g_cen<-L1_switch->| client_b<->client_a |<-->| client_a<->client_b |<-L1_switch->eth_400g_dis(self looped)
     *                          +---------------------+    +---------------------+
 */

    /* Clear calendar settings */
    for (i = 0; i < 2; i++) {
        /* Restore all slots to 0 */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * Clear FlexE configuration on centralized device
 */

    /* Clear ETH2FlexE L2 datapath: eth_400g_cen -> client_b -> client_a */
    rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_400g_cen, client_b, client_a, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to clear Eth2FlexE L2 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Clear FlexE2Eth L2 datapath: eth_400g_cen <- client_b <- client_a */
    rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_400g_cen, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to clear FlexE2Eth L2 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Remove FlexE clients */
    rv = dnx_flexe_util_client_remove(unit_cen, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_a on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_remove(unit_cen, client_b);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_b on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Delete FlexE group */
    rv = bcm_port_flexe_group_destroy(unit_cen, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_destroy(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /***************************************************************************
     * Clear FlexE configuration on distributed device
 */

    /* Clear ETH2FlexE L2 datapath: eth_400g_dis -> client_b -> client_a */
    rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_400g_dis, client_b, client_a, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to clear Eth2FlexE L2 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Clear FlexE2Eth L2 datapath: eth_400g_dis <- client_b <- client_a */
    rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_400g_dis, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to clear FlexE2Eth L2 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Remove FlexE clients */
    rv = dnx_flexe_util_client_remove(unit_dis, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_a on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_remove(unit_dis, client_b);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_b on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Delete FlexE group */
    rv = bcm_port_flexe_group_destroy(unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_destroy(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    return rv;
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
    int i, rv;
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

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 30; /* BusA client for L1 traffic */
    bcm_port_t client_a_L2 = 31; /* BusA client for L2 traffic */
    bcm_port_t client_a_L3 = 32; /* BusA client for L3 traffic */
    bcm_port_t client_b_L1 = 33; /* BusB client for L1 traffic */
    bcm_port_t client_b_L2 = 34; /* BusB client for L2 traffic */
    bcm_port_t client_b_L3 = 35; /* BusB client for L3 traffic */

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_centralized_datapath_L1_L2_L3_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L2, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L3, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L3 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L1, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L2, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L3, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L3 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L1 datapath: eth_port_L1<- L1_switch -> client_b_L1 <--> client_a_L1 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port_L1, client_b_L1, client_a_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_L1, client_b_L1, eth_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 datapath: eth_port_L2<- L2_switch -> client_b_L2 <--> client_a_L2 */
    rv = dnx_flexe_util_L2_switch_config(unit_cen, eth_port_L2, client_b_L2, l2_vlan);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in L2 setting on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L2, client_a_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L2, client_b_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Create L3 datapath: eth_port_L3<- L3_route -> client_b_L3 <--> client_a_L3 */
    rv = dnx_flexe_util_L3_route_config(unit_cen, eth_port_L3, client_b_L3, in_vlan, out_vlan);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in L3 setting on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L3, client_a_L3, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L3, client_b_L3, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L1, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L2, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L3, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L3 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_L1, port_type_busB_L1, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_L2, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_L3, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L3 ****\n", proc_name, rv);
        return rv;
    }

    /* Create datapath for L1 traffic: eth_port_L1<--> client_b_L1 <--> client_a_L1 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_port_L1, client_b_L1, client_a_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_L1, client_b_L1, eth_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE2Eth L2 datapath:
     * client_a_L2 --> client_b_L2 --> force_fwd -> eth_port_L2
     */
     rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_L2, client_b_L2, eth_port_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE2Eth L2 datapath:
     * eth_port_L2 --L2_lookup--> client_b_L2 --> client_a_L2
     */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L2, client_a_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L2_entry_advanced_set(unit_dis, eth_port_L2, client_b_L2, mac_l2, 1, l2_vlan, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in L2 setting on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE2Eth L3 datapath:
     * client_a_L3 --> client_b_L3 --> force_fwd -> eth_port_L3
     */
     rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a_L3, client_b_L3, eth_port_L3, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE2Eth L3 datapath:
     * eth_port_L3 --L2_lookup--> client_b_L3 --> client_a_L3
     */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L3, client_a_L3, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L2_entry_advanced_set(unit_dis, eth_port_L3, client_b_L3, my_mac, 1, out_vlan, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in L3 setting on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 3 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_L1;
        Cal_Slots_A[1] = client_a_L2;
        Cal_Slots_A[2] = client_a_L3;
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* Egress default VLAN action set */
        rv = dnx_flexe_util_egr_default_vlan_action_set(unit[i]);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set dnx_flexe_util_egr_default_vlan_action_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 30;  /* BusA client for L1 traffic */
    bcm_port_t client_a_L23 = 31; /* BusA client for L2&L3 traffic */
    bcm_port_t client_b_L1 = 32;  /* BusB client for L1 traffic */
    bcm_port_t client_b_L23 = 33; /* BusB client for L2&L3 traffic */
    bcm_port_t client_c = 34;     /* BusC client for L1 traffic */

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_distributed_datapath_L1_L2_L3_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L23, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L23 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L23, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L23 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L1 Tx datapath: eth_L1_tx -force_fwd-> client_b_L1 --> client_a_L1 */
    rv = bcm_port_force_forward_set(unit_cen, eth_L1_tx, client_b_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(Eth2BusB1) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L1, client_a_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on centralized device ****\n", proc_name, rv);
        return rv;
    }
    /* Create L1 Rx datapath: client_a_L1 --> client_b_L1 -force_fwd-> eth_L1_rx */
    rv = bcm_port_force_forward_set(unit_cen, client_b_L1, eth_L1_rx, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(BusB2Eth1) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L1, client_b_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2&L3 Tx datapath: eth_L23_tx -force_fwd-> client_b_L23 --> client_a_L23 */
    rv = bcm_port_force_forward_set(unit_cen, eth_L23_tx, client_b_L23, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(Eth2BusB2) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L23, client_a_L23, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on centralized device ****\n", proc_name, rv);
        return rv;
    }
    /* Create L2&L3 Rx datapath: client_a_L23 --> client_b_L23 -force_fwd-> eth_L23_rx */
    rv = bcm_port_force_forward_set(unit_cen, client_b_L23, eth_L23_rx, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(BusB2Eth2) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L23, client_b_L23, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L1, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L23, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L23 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_L23, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L23 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_c, port_type_busC, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c ****\n", proc_name, rv);
        return rv;
    }

    /* Create Rx datapath for L1 traffic: client_a_L1 --> client_c --> ilkn_L1 */
    rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a_L1, client_c, ilkn_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in setting FlexE2ILKN L1 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }
    /* Create Tx datapath for L1 traffic: client_a_L1 <-- client_c <-- ilkn_L1 */
    rv = dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_L1, client_c, client_a_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in setting ILKN2Flexe L1 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create Rx datapath for L2&L3 traffic: client_a_L23 --> client_b_L23 --> ilkn_L23 */
    rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a_L23, client_b_L23, ilkn_L23, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in setting FlexE2ILKN L2&L3 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }
    /* Create Tx datapath for L2&L3 traffic: client_a_L23 <-- client_b_L23 <-- ilkn_L23 */
    rv = dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_L23, client_b_L23, client_a_L23, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in setting ILKN2Flexe L2&L3 datapath on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 3 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_L1;
        Cal_Slots_A[1] = client_a_L23;
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;

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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_failover_src_in_busA_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a_tx, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_tx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_rx, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_tx, port_type_busB_L2_L3, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_tx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_rx, port_type_busB_L2_L3, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_rx ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 Tx datapath: eth_port_tx -> force_fwd -> client_b_tx ->  client_a_tx */
    rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port_tx, client_b_tx, client_a_tx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Tx datapath on Q2A_#0 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 Rx datapath: client_a_tx -> client_b_tx -> force_fwd -> eth_port_rx_2 */
    rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a_tx, client_b_tx, eth_port_rx_2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Rx2 datapath on Q2A_#0 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 Rx datapath: client_a_rx -> client_b_rx -> force_fwd -> eth_port_rx_0 */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_rx, client_b_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L1 Rx datapath on Q2A_#0 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_cen, client_b_rx, eth_port_rx_0, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(busB to eth_rx0) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a_src, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_src ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_dst, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_dst, port_type_busB_L1, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_c_dst, port_type_busC, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c_dst ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 1st failover path:  client_a_src -> failover channel 0 -> client_a_dst */
    failover_channel_id = 0;
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_src, client_a_dst, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 1st failover path ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 2nd failover path: client_a_src -> failover channel 1 -> client_b_dst -> L1 -> eth_port_rx_1 */
    failover_channel_id = 1;
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_src, client_b_dst, eth_port_rx_1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 2nd failover path ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 3rd failover path:
     * client_a_src -> failover channel 2 -> client_c_dst -> ilkn_port --loopback -> client_c_dst -> client_a_src
     */
    failover_channel_id = 2;
    rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a_src, client_c_dst, ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 3rd failover path(FlexE2ILKN) ****\n", proc_name, rv);
        return rv;
    }
    failover_channel_id = 0;
    rv = dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_c_dst, client_a_src, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 3rd failover path(ILKN2FlexE) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_src;
        Cal_Slots_A[1] = client_a_src;
        Cal_Slots_A[2] = client_a_dst;
        Cal_Slots_A[3] = client_a_dst;
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;

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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_failover_src_in_busB_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a0_dst, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a0_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a1_dst, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a1_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_src, port_type_busB_L1, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_src ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_c_dst, port_type_busC, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c_dst ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 1st failover path:  client_b_src -> failover channel 0 -> client_a0_dst */
    failover_channel_id = 0;
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_src, client_a0_dst, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 1st failover path ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 2nd failover path:  client_b_src -> failover channel 1 -> client_a1_dst */
    failover_channel_id = 1;
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_src, client_a1_dst, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 2nd failover path ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 3rd failover path:
     *    eth_port_tx -> client_b_src -> failover channel 2 -> client_c_dst -> ilkn_port --loopback-->
     * -> client_c_dst -> client_b_src -> eth_port_rx_2
     */
    failover_channel_id = 2;
    rv = dnx_flexe_util_L1_datapath_eth_to_ilkn(unit_dis, eth_port_tx, client_b_src, client_c_dst, ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 3rd failover path(Eth2ILKN) ****\n", proc_name, rv);
        return rv;
    }
    failover_channel_id = 0;
    rv = dnx_flexe_util_L1_datapath_ilkn_to_eth(unit_dis, ilkn_port, client_c_dst, client_b_src, eth_port_rx_2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 3rd failover path(ILKN2Eth) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a0_rx, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a0_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a1_rx, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a1_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b0_rx, port_type_busB_L2_L3, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b0_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b1_rx, port_type_busB_L2_L3, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b1_rx ****\n", proc_name, rv);
        return rv;
    }

    /* Create rx_path_0: client_a0_rx -> client_b0_rx -> force_fwd -> eth_port_rx_0 */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a0_rx, client_b0_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set rx_path_0 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_cen, client_b0_rx, eth_port_rx_0, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(busB to eth_rx0) ****\n", proc_name, rv);
        return rv;
    }

    /* Create rx_path_1: client_a1_rx -> client_b1_rx -> force_fwd -> eth_port_rx_1 */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a1_rx, client_b1_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set rx_path_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_cen, client_b1_rx, eth_port_rx_1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(busB to eth_rx1) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a0_dst;
        Cal_Slots_A[1] = client_a0_dst;
        Cal_Slots_A[2] = client_a1_dst;
        Cal_Slots_A[3] = client_a1_dst;
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;

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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_failover_src_in_busC_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a0_dst, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a0_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a1_dst, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a1_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_dst, port_type_busB_L1, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_dst ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_c_src, port_type_busC, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c_src ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 1st failover path:
     *    eth_port_tx -> client_b_dst -> client_c_src -> ilkn_port -loopback->
     * -> client_c_src -> failover channel 0 -> client_b_dst  -> eth_port_rx_2
     */
    failover_channel_id = 0;
    rv = dnx_flexe_util_L1_datapath_eth_to_ilkn(unit_dis, eth_port_tx, client_b_dst, client_c_src, ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 1st failover path(Eth2ILKN) ****\n", proc_name, rv);
        return rv;
    }
    failover_channel_id = 0;
    rv = dnx_flexe_util_L1_datapath_ilkn_to_eth(unit_dis, ilkn_port, client_c_src, client_b_dst, eth_port_rx_2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 1st failover path(ILKN2Eth) ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 2nd failover path:  client_c_src -> failover channel 1 -> client_a0_dst  */
    failover_channel_id = 1;
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_c_src, client_a0_dst, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 2nd failover path ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 3rd failover path:  client_c_src -> failover channel 2 -> client_a1_dst  */
    failover_channel_id = 2;
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_c_src, client_a1_dst, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create the 3rd failover path ****\n", proc_name, rv);
        return rv;
    }
    failover_channel_id = 0;

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a0_rx, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a0_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a1_rx, port_type_busA, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a1_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b0_rx, port_type_busB_L2_L3, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b0_rx ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b1_rx, port_type_busB_L2_L3, 10000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b1_rx ****\n", proc_name, rv);
        return rv;
    }

    /* Create rx_path_0: client_a0_rx -> client_b0_rx -> force_fwd -> eth_port_rx_0 */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a0_rx, client_b0_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set rx_path_0 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_cen, client_b0_rx, eth_port_rx_0, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(busB to eth_rx0) ****\n", proc_name, rv);
        return rv;
    }

    /* Create rx_path_1: client_a1_rx -> client_b1_rx -> force_fwd -> eth_port_rx_1 */
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a1_rx, client_b1_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set rx_path_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_cen, client_b1_rx, eth_port_rx_1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(busB to eth_rx1) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a0_dst;
        Cal_Slots_A[1] = client_a0_dst;
        Cal_Slots_A[2] = client_a1_dst;
        Cal_Slots_A[3] = client_a1_dst;
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, FLEXE_CAL_A);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhCalInUse(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
        rv = bcm_port_flexe_oh_set(unit[i], flexe_phy, BCM_PORT_FLEXE_TX, bcmPortFlexeOhLogicalPhyID, flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhLogicalPhyID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* Active FLEXE_CAL_A */
        rv = bcm_port_flexe_group_cal_active_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to active FLEXE_CAL_A on unit %d ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    bcm_gport_t flexe_grp;

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 40; /* BusA client for L1 traffic */
    bcm_port_t client_a_L2 = 41; /* BusA client for L2 traffic */
    bcm_port_t client_b_L1 = 42; /* BusB client for L1 traffic */
    bcm_port_t client_b_L2 = 43; /* BusB client for L2 traffic */

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_eth_port_and_flexe_phy_in_same_cdu_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L2, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L1, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L2, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L1 datapath: cen_port_L1<- L1_switch -> client_b_L1 <--> client_a_L1 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, cen_port_L1, client_b_L1, client_a_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to Eth2FlexE L1 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_L1, client_b_L1, cen_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 datapath: cen_port_L2<- force_fwd -> client_b_L2 <--> client_a_L2 */
    rv = bcm_port_force_forward_set(unit_cen, cen_port_L2, client_b_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(Eth2BusB2) ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_cen, client_b_L2, cen_port_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(BusB2Eth2) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L2, client_a_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_a_L2, client_b_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L1, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_L2, port_type_busA, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_L1, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_L2, port_type_busB_L2_L3, 5000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }

    /* Create datapath for L1 traffic: dis_port_1<- force_forward -> client_b_L1 <--> client_a_L1 */
    rv = bcm_port_force_forward_set(unit_dis, dis_port_1, client_b_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(Eth2BusB1) ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_dis, client_b_L1, dis_port_1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(BusB2Eth1) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L1, client_a_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_L1, client_b_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create datapath for L2 traffic: dis_port_2<- force_forward -> client_b_L2 <--> client_a_L2 */
    rv = bcm_port_force_forward_set(unit_dis, dis_port_2, client_b_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(Eth2BusB2) ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_dis, client_b_L2, dis_port_2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(BusB2Eth2) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_b_L2, client_a_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusB2BusA flow on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_L1_datapath_client_to_client(unit_dis, client_a_L2, client_b_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set BusA2BusB flow on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 3 FlexE clients in BusA */
        Cal_Slots_A[0] = client_a_L1;
        Cal_Slots_A[1] = client_a_L2;
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_client_slots = flexe_client_speed / 5000;
    int total_slots = (phy_speed * nof_phys) / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 60;
    bcm_port_t client_b = 61;
    flexe_port_type_t client_b_type_cen = is_L1_fwd_cen ? port_type_busB_L1: port_type_busB_L2_L3;
    flexe_port_type_t client_b_type_dis = is_L1_fwd_dis ? port_type_busB_L1: port_type_busB_L2_L3;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_datapath_between_eth_and_flexe_common";

    /***************************************************************************
     * The datapath is:
     *
     *                      Centralized Device                     Centralized Device
     *                   +------------------------+             +------------------------+
     * eth_cen <-L1/L2-> | client_b <--> client_a | <- FlexE -> | client_a <--> client_b | <-L1/L2-> eth_dis
     *                   +------------------------+             +------------------------+
 */

    /***************************************************************************
     * FlexE Setting on centralized device
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    for (i = 0; i < nof_phys; i++) {
        flexe_phy_ports[i] = phy_id_start + i;
    }
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a, port_type_busA, flexe_client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b, client_b_type_cen, flexe_client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b on centralized device ****\n", proc_name, rv);
        return rv;
    }

    if (is_L1_fwd_cen) {
        /* Create L1 datapath: eth_cen <- L1_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Create L3 datapath: eth_cen <-L3_route-> client_b <--> client_a */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L2 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE Setting on distributed device
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a, port_type_busA, flexe_client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b, client_b_type_dis, flexe_client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b on distributed device ****\n", proc_name, rv);
        return rv;
    }

    if (is_L1_fwd_dis) {
        /* Create FlexE datapath: eth_dis <- L1 -> client_b <--> client_a */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }

        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Create FlexE datapath: eth_dis <-force_forward-> client_b <--> client_a */
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L2 datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }

        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the FlexE client */
        for (ii = 0; ii < nof_client_slots; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], phy_id_start);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
    int unit[2] = {unit_cen, unit_dis};
    int total_slots = flexe_grp_speed / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 60;
    bcm_port_t client_b = 61;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_common_datapath_between_eth_and_flexe_clear";

    /***************************************************************************
     * The datapath is:
     *
     *                      Centralized Device                     Centralized Device
     *                   +------------------------+             +------------------------+
     * eth_cen <-L1/L2-> | client_b <--> client_a | <- FlexE -> | client_a <--> client_b | <-L1/L2-> eth_dis
     *                   +------------------------+             +------------------------+
 */

    /***************************************************************************
     * Clear calendar and OH settings
 */

    /* Clear calendar settings */
    for (i = 0; i < 2; i++) {
        /* Restore all slots to 0 */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * Clear FlexE Setting on centralized device
 */

    /* Clear FlexE datapaths */
    if (is_L1_fwd_cen) {
        /* Clear L1 datapath: eth_cen <- L1_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear Eth2FlexE L1 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE2Eth L1 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Clear L2 datapath: eth_cen <- L2_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_cen, client_b, client_a, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear Eth2FlexE L2 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_cen, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE2Eth L2 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Remove FlexE clients */
    rv = dnx_flexe_util_client_remove(unit_cen, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_a on centralized device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_remove(unit_cen, client_b);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_b on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /* Delete FlexE group */
    rv = bcm_port_flexe_group_destroy(unit_cen, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_destroy(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on distributed device
 */

    /* Clear FlexE datapaths */
    if (is_L1_fwd_dis) {
        /* Clear L1 datapath: eth_dis <- L1_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear Eth2FlexE L1 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE2Eth L1 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Clear L2 datapath: eth_dis <- L2_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_dis, eth_dis, client_b, client_a, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear Eth2FlexE L2 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit_dis, client_a, client_b, eth_dis, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE2Eth L2 datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Remove FlexE clients */
    rv = dnx_flexe_util_client_remove(unit_dis, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_a on distributed device ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_remove(unit_dis, client_b);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove client_b on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Delete FlexE group */
    rv = bcm_port_flexe_group_destroy(unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_destroy(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    return rv;
}

/* Function to create a FlexE L1 snake in 1 FlexE group with maximal allowed clients and eth ports according given snake speed */
int dnx_flexe_snake_test_1_grp_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t flexe_phy,
    int snake_rate)
{
    int i, ii, rv, unit;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_snake_test_1_grp_main";

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
 */

    /***************************************************************************
     * FlexE setting on centralized device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create BusB client */
    rv = dnx_flexe_util_client_create(unit, client_b_base, port_type_busB_L1, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create BusB client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create traffic Tx and Rx datapath:
     * Tx: eth_tx_rx -> BusB -> BusA_0
     * Rx: BusA_N -> BusB -> eth_tx_rx
     */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_tx_rx, client_b_base, client_a_base, 0);
    rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients-1, client_b_base,  eth_tx_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Tx and Rx L1 datapath(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create flexe to flexe flows to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        /* FlexE2FlexE: BusA_i -> BusA_i+1 */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on distributed device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L1, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create L1 ETH ports */
    for (i = 0; i < nof_eth_ports; i++) {
        rv = dnx_flexe_util_L1_port_add(unit, eth_port_base+i, 1, eth_lanes[i], nof_lanes_per_eth, 0, 0, eth_speed, fec_type, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to add ETH L1 port(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }

        /* IPG setting */
        rv = bcm_port_ifg_set(unit, eth_port_base+i, 0, BCM_PORT_DUPLEX_FULL, 88);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_ifg_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Connect L1 ports with FlexE clients */
    for (i = 0; i < nof_eth_ports; i++) {
        /* L1 datapath: BusA_i -> BusB_i -> eth_i */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+i, client_b_base+i, eth_port_base+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }

        /* L1 datapath: eth_i -> BusB_i -> BusA_i */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_base+i, client_b_base+i, client_a_base+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Check FlexE PHY link status */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(units[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, ii, rv;
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
 */

    /***************************************************************************
     * Clear the datapath between ETH port and FlexE client on distributed device
 */

    if (is_L1_to_L2) {
        /* Clear L1 datapath: eth_i -> BusB_i -> BusA_i */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear Eth2FlexE L1 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }

        /* Clear L1 datapath: BusA_i ->BusB_i -> eth_i */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE2Eth L1 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }
    } else {
        /* Clear L2 datapath: eth_i -> force_forward -> BusB_i -> BusA_i */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear Eth2FlexE L2 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }

        /* Clear L2 datapath: BusA_i -> BusB_i -> force_forward  -> eth_i */
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE2Eth L2 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }
    }

    /***************************************************************************
     * Exchange L1 and L2 ports
 */

    if (is_L1_to_L2) {
        /* Remove the BusB L1 client */
        rv = dnx_flexe_util_client_remove(unit, client_b_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove BusB L1 client(id %d) ****\n", proc_name, rv, client_b_id);
            return rv;
        }
        /* Readd the BusB client for L2 forwarding */
        rv = dnx_flexe_util_client_create(unit, client_b_id, port_type_busB_L2_L3, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB L2 client(id %d) ****\n", proc_name, rv, client_b_id);
            return rv;
        }

        /* Delete the ETH L1 port */
        rv = bcm_port_enable_set(unit, eth_port_id, 0);
        rv |= bcm_port_remove(unit, eth_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove ETH L1 port(id %d) ****\n", proc_name, rv, eth_port_id);
            return rv;
        }

        /* Readd the ETH port for L2 forwarding */
        rv = cint_dpp_port_add(unit, eth_port_id, eth_lanes[eth_flexe_pair_index], BCM_PORT_IF_NIF_ETH, nof_lanes_per_eth, eth_speed, fec_type);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in cint_dpp_port_add(id %d) ****\n", proc_name, rv, eth_port_id);
            return rv;
        }
        /* IPG setting */
        rv = bcm_port_ifg_set(unit, eth_port_id, 0, BCM_PORT_DUPLEX_FULL, 88);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_ifg_set(id %d) ****\n", proc_name, rv, eth_port_id);
            return rv;
        }
    } else {
        /* Remove the BusB L2 client */
        rv = dnx_flexe_util_client_remove(unit, client_b_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove BusB L2 client(id %d) ****\n", proc_name, rv, client_b_id);
            return rv;
        }
        /* Readd the BusB client for L1 forwarding */
        rv = dnx_flexe_util_client_create(unit, client_b_id, port_type_busB_L1, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB L1 client(id %d) ****\n", proc_name, rv, client_b_id);
            return rv;
        }

        /* Delete the ETH L2 port */
        rv = dynamic_port_remove(unit, eth_port_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove ETH L2 port %d ****\n", proc_name, rv, eth_port_id);
            return rv;
        }
        /* Readd the ETH port for L1 forwarding */
        rv = dnx_flexe_util_L1_port_add(unit, eth_port_id, 1, eth_lanes[eth_flexe_pair_index], nof_lanes_per_eth, 0, 0, eth_speed, fec_type, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to add ETH L1 port %d ****\n", proc_name, rv, eth_port_id);
            return rv;
        }
        /* IPG setting */
        rv = bcm_port_ifg_set(unit, eth_port_id, 0, BCM_PORT_DUPLEX_FULL, 88);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_ifg_set(id %d) ****\n", proc_name, rv, eth_port_id);
            return rv;
        }
    }

    /***************************************************************************
     * Build new datapath
 */

    if (is_L1_to_L2) {
        /* L2 datapath: eth_i -> BusB_i -> busA_i */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }

        /* L2 datapath: BusA_i -> busB_i -> eth_i */
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L2 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }
    } else {
        /* L1 datapath: eth_i -> busB_i -> busA_i */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_id, client_b_id, client_a_id, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }

        /* L1 datapath: busA_i -> busB_i -> eth_i */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_id, client_b_id, eth_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(index %d) ****\n", proc_name, rv, eth_flexe_pair_index);
            return rv;
        }
    }

    return rv;
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
    int i, ii, slot_iter, rv, unit;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_snake_test_2_grps_main";

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
 */

    /***************************************************************************
     * FlexE setting on centralized device
 */

    /* Create 2 FlexE groups with the 2 200G FlexE PHYs */
    if (!readd_grp_b) {
        unit = unit_cen;
        for (i = 0; i < 2; i++) {
            BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[i], i);
            flexe_phy_ports[0] = flexe_phy[i];
            rv = dnx_flexe_util_group_create(unit, flexe_grp[i], nof_pcs);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* Create BusA clients */
        for (i = 0; i < nof_clients; i++) {
            rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* Create BusB client */
        rv = dnx_flexe_util_client_create(unit, client_b_base, port_type_busB_L1, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB client(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create traffic Tx datapath: eth_trx -> BusB -> BusA_0 */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_trx, client_b_base, client_a_base, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients_per_grp-1, client_b_base, eth_trx, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create flexe to flexe flows to loop traffic back to distributed device */
        for (i = 0; i < nof_clients-1; i++) {
            /* FlexE2FlexE: BusA_i -> BusA_i+1 (skip the setting on the 20th client) */
            if (i != nof_clients_per_grp-1) {
                rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                    return rv;
                }
            }
        }

        /* Calendar and OH setting */
        for (i = 0; i < 2; i++) {
            /* Assign slots for FlexE clients */
            dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
            for (slot_iter = 0; slot_iter < nof_slots_per_grp; slot_iter++) {
                client_id = (slot_iter / nof_slots_per_client) + client_a_base + nof_clients_per_grp*i;
                Cal_Slots_A[slot_iter] = client_id;
            }

            /* Assign slots and set OH client ID */
            rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp[i], 0, FLEXE_CAL_A, nof_slots_per_grp);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
                return rv;
            }

            /* Set OH Group ID */
            rv = bcm_port_flexe_oh_set(unit, flexe_grp[i], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, i+1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
                return rv;
            }
        }
    }

    /***************************************************************************
     * FlexE setting on distributed device
 */

    /* Create 2 FlexE groups with the 2 200G FlexE PHYs */
    unit = unit_dis;
    for (i = 0; i < 2; i++) {
        if (readd_grp_b && i == 0) {
            /* Don't create GroupA If re-create GroupB */
            continue;
        }

        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[i], i);
        flexe_phy_ports[0] = flexe_phy[i];
        rv = dnx_flexe_util_group_create(unit, flexe_grp[i], nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if (readd_grp_b && i < nof_clients_per_grp) {
            /* Don't create clients in GroupA If re-create GroupB */
            continue;
        }

        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L1, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create L1 ETH ports */
    for (i = 0; i < nof_eth_ports; i++) {
        if (readd_grp_b && i < nof_clients_per_grp) {
            /* Don't create L1 ports in GroupA If re-create GroupB */
            continue;
        }

        rv = dnx_flexe_util_L1_port_add(unit, eth_port_base+i, 1, eth_lanes[i], nof_lanes_per_eth, 0, 0, eth_speed, fec_type, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to add ETH L1 port(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
        /* IPG setting */
        rv = bcm_port_ifg_set(unit, eth_port_base+i, 0, BCM_PORT_DUPLEX_FULL, 88);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_ifg_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Connect L1 ports with FlexE clients */
    for (i = 0; i < nof_eth_ports; i++) {
        if (readd_grp_b && i < nof_clients_per_grp) {
            /* Skip flow set in GroupA If re-create GroupB */
            continue;
        }

        /* L1 datapath: BusA_i -> BusB_i -> eth_i */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+i, client_b_base+i, eth_port_base+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }

        /* L1 datapath: eth_i -> BusB_i -> BusA_i */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_base+i, client_b_base+i, client_a_base+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Calendar and OH setting */
    for (i = 0; i < 2; i++) {
        if (readd_grp_b && i == 0) {
            /* No OH setting on GroupA If re-create GroupB */
            continue;
        }

        /* Assign slots for FlexE clients */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (slot_iter = 0; slot_iter < nof_slots_per_grp; slot_iter++) {
            client_id = (slot_iter / nof_slots_per_client) + client_a_base + nof_clients_per_grp*i;
            Cal_Slots_A[slot_iter] = client_id;
        }

        /* Assign slots and set OH client ID */
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp[i], 0, FLEXE_CAL_A, nof_slots_per_grp);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Set OH Group ID */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp[i], BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, i+1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Synce check */
    if (!readd_grp_b) {
        flexe_phy_ports[0] = flexe_phy_0;
        if (is_q2a_a0_only(unit_dis)) {
            bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSource, flexe_phy_ports[0]);
            bcm_switch_control_set(unit_dis, bcmSwitchSynchronousPortClockSourceDivCtrl, 2);
        }
        sal_sleep(5);
        rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp[0]);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Synce check failed ****\n", proc_name);
            return rv;
        }
    }

    return rv;
}

/* Function to delete group B which is generated by function dnx_flexe_snake_test_2_grps_main() */
int dnx_flexe_snake_test_2_grps_clear_grp_b(
    int unit_cen,
    int unit_dis,
    int snake_rate)
{
    int i, rv, unit;
    int client_speed = snake_rate;
    int nof_eth_ports = 400000 / snake_rate;
    int nof_clients = 400000 / snake_rate;
    int nof_clients_per_grp = nof_clients / 2;
    int nof_slots_per_grp = (client_speed * nof_clients_per_grp) / 5000;
    bcm_port_t client_a_base = 40;
    bcm_port_t client_b_base = 50;
    bcm_port_t eth_port_base = 60;
    bcm_gport_t flexe_grp[2];

    /* Get group GPORT */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[0], 0);
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp[1], 1);

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_snake_test_2_grps_clear_grp_b";

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
 */

    /***************************************************************************
     * Clear FlexE GroupB on distributed device
 */
    unit = unit_dis;

    /* Calendar and OH setting */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp[1], 0, FLEXE_CAL_A, nof_slots_per_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Clear L1 flows in GroupB */
    for (i = 0; i < nof_eth_ports; i++) {
        if (i < nof_clients_per_grp) {
            /* Skip L1 flows in GroupA */
            continue;
        }

        /* Clear L1 datapath: BusA_i -> BusB_i -> eth_i */
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+i, client_b_base+i, eth_port_base+i, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }

        /* Clear L1 datapath: eth_i -> BusB_i -> BusA_i */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_base+i, client_b_base+i, client_a_base+i, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Remove BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if (i < nof_clients_per_grp) {
            /* Skip clients in GroupA */
            continue;
        }

        rv = dnx_flexe_util_client_remove(unit,  client_a_base+i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove BusA L1 client(id %d) ****\n", proc_name, rv, client_a_base+i);
            return rv;
        }
        rv = dnx_flexe_util_client_remove(unit,  client_b_base+i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove BusB L1 client(id %d) ****\n", proc_name, rv, client_b_base+i);
            return rv;
        }
    }

    /* Remove L1 ETH ports */
    for (i = 0; i < nof_eth_ports; i++) {
        if (i < nof_clients_per_grp) {
            /* Skip L1 ports in GroupA */
            continue;
        }

        rv = bcm_port_enable_set(unit, eth_port_base+i, 0);
        rv |= bcm_port_remove(unit, eth_port_base+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove ETH L1 port(id %d) ****\n", proc_name, rv, eth_port_base+i);
            return rv;
        }
    }

    /* Remove GroupB */
    rv = bcm_port_flexe_group_destroy(unit, flexe_grp[1]);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove GroupB ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, rv, unit;
    int grp_speed = dev_is_q2u ? 200000 : 400000;
    int total_slots = grp_speed / 5000;
    int nof_slots_per_client = client_speed / 5000;
    int nof_slots_assign = (nof_clients * client_speed) / 5000;
    int nof_phys = dev_is_q2u ? 2 : 1;
    bcm_port_t client_a_base = 120;
    bcm_port_t client_b0 = 21;
    bcm_port_t client_b1 = 22;
    bcm_gport_t flexe_grp;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_snake_with_max_busA_clients";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

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
 */

    /***************************************************************************
     * FlexE setting on centralized device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (dev_is_q2u) {
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create 2 BusB clients */
    rv = dnx_flexe_util_client_create(unit, client_b0, port_type_busB_L1, client_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_b1, port_type_busB_L1, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create BusB clients(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create traffic Tx datapath: eth_tx -> BusB0 -> BusA_0 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_tx_rx, client_b0, client_a_base, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create traffic Rx datapath: BusA_N -> BusB1 -> eth_rx */
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients-1, client_b1, eth_tx_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create flexe to flexe flows to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        if (i % 2) {
            /* FlexE2FlexE: BusA_i -> BusA_i+1 */
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on distributed device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (dev_is_q2u) {
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create flexe to flexe flows to loop traffic back to centralized device */
    for (i = 0; i < nof_clients-1; i++) {
        if ((i % 2) == 0) {
            /* FlexE2FlexE: BusA_i -> BusA_i+1 */
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Synce check */
    sal_sleep(5);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv, unit;
    int grp_speed = dev_is_q2u ? 200000 : 400000;
    int nof_phys = dev_is_q2u ? 2 : 1;
    int total_slots = grp_speed / 5000;
    int nof_slots_per_client = client_speed / 5000;
    int nof_slots_assign = (nof_clients * client_speed) / 5000;
    bcm_port_t client_a_base = 20;
    bcm_port_t client_b_base = 120;
    bcm_gport_t flexe_grp;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_centralized_snake_with_max_busB_clients_main";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

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
 */

    /***************************************************************************
     * FlexE setting on centralized device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (dev_is_q2u) {
        /* Q2U 200G FlexE group needs 2*100G PHYs */
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA and busB clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Set FlexE flows between BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if (i % 2) {
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set FlexE flow from BusA to BusB(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        if ((i % 2) == 0) {
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set FlexE flow from BusB to BusA(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }
    }

    /* Set BusB to BuBs L2 switching to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        if (i % 2) {
            /* BusB2BusB: BusB_i -> BusB_i+1 */
            rv = bcm_port_force_forward_set(unit, client_b_base+i, client_b_base+i+1, 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }
    }

    /* Set L2 datapath between eth_tx_rx and BusB clients */
    rv = bcm_port_force_forward_set(unit, eth_tx_rx, client_b_base, 1);
    rv |= bcm_port_force_forward_set(unit, client_b_base+nof_clients-1, eth_tx_rx, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L2 datapath between ETH and BusB(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on distributed device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    if (dev_is_q2u) {
        /* Q2U 200G FlexE group needs 2*100G PHYs */
        flexe_phy_ports[1] = flexe_phy + 1;
    }
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA and busB clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Set FlexE flows between BusA clients and BusB clients */
    for (i = 0; i < nof_clients; i++) {
        if ((i % 2) == 0) {
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set FlexE flow from BusA to BusB(unit %d) ****\n", proc_name, rv, unit);
                return rv;
            }
        }

        if (i % 2) {
            rv |= dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set FlexE flow from BusA to BusB(unit %d) ****\n", proc_name, rv, unit);
                return rv;
            }
        }
    }

    /* Set BusB to BuBs L2 switching to loop traffic back to centralized device */
    for (i = 0; i < nof_clients-1; i++) {
        if ((i % 2) == 0) {
            /* BusB2BusB: BusB_i -> BusB_i+1 */
            rv = bcm_port_force_forward_set(unit, client_b_base+i, client_b_base+i+1, 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Synce check */
    sal_sleep(5);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv, unit;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_distributed_snake_with_max_L1_channels_main";

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
 */

    /***************************************************************************
     * FlexE setting on centralized device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_cen;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create 2 BusB clients */
    rv = dnx_flexe_util_client_create(unit, client_b0, port_type_busB_L1, client_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_b1, port_type_busB_L1, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create BusB clients(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create traffic Tx datapath: eth_tx -> BusB0 -> BusA_0 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_tx_rx, client_b0, client_a_base, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create traffic Rx datapath: BusA_N -> BusB1 -> eth_rx */
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a_base+nof_clients-1, client_b1, eth_tx_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create flexe to flexe flows to loop traffic back to distributed device */
    for (i = 0; i < nof_clients-1; i++) {
        /* FlexE2FlexE: BusA_i -> BusA_i+1 */
        rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_a_base+i+1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2FlexE L1 datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on distributed device
 */

    /* Create FlexE group with the the 400G FlexE PHY */
    unit = unit_dis;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create BusA clients and BusC clients */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit, client_c_base+i, port_type_busC, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create BusC client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create L1 ILKN ports */
    for (i = 0; i < nof_clients; i++) {
        rv = dnx_flexe_util_L1_port_add(unit, ilkn_port_start+i, 0, 28, 22, 2, i, 25781, bcmPortPhyFecNone, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create L1 ILKN port(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Create L1 datapath for each FlexE&ILKN pair */
    for (i = 0; i < nof_clients; i++) {
        /* FlexE2ILKN: BusA_i -> BusC_i ->  ILKN_i */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit, client_a_base+i, client_c_base+i, ilkn_port_start+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Flexe2ILKN datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }

        /* ILKN2Flexe: ILKN_i -> BusC_i -> BusA_i */
        rv = dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit, ilkn_port_start+i, client_c_base+i, client_a_base+i, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set ILKN2Flexe datapath(unit %d, index %d) ****\n", proc_name, rv, unit, i);
            return rv;
        }
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_assign; i++) {
        Cal_Slots_A[i] = (i / nof_slots_per_client) + client_a_base;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Synce check */
    sal_sleep(5);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int rv;
    bcm_port_flexe_oh_alarm_t oh_alarm_get;

    /* Change OH setting */
    if (value != -1) {
        rv = bcm_port_flexe_oh_set(unit, port_set, BCM_PORT_FLEXE_TX, oh_type, value);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_set ****\n", proc_name, rv);
            return rv;
        }
        sal_sleep(2);
    }

    /* Get OH alarms */
    rv = bcm_port_flexe_oh_alarm_get(unit, port_get, 0, &oh_alarm_get);
    sal_sleep(2);
    rv |= bcm_port_flexe_oh_alarm_get(unit, port_get, 0, &oh_alarm_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_alarm_get ****\n", proc_name, rv);
        return rv;
    }

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
    int i, ii, iii, rv, unit_iter, unit;
    int nof_flexe_phys = 1;
    int grp_speed = dev_is_q2u ? 200000 : 400000;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_slots_assign_for_mixed_clients_main";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

    /* Initiate the number of clients and relevant speeds */
    nof_clients = 6;
    client_speed[0] = 5000;
    client_speed[1] = 10000;
    client_speed[2] = 15000;
    client_speed[3] = 25000;
    client_speed[4] = 75000;
    client_speed[5] = 270000;
    if (dev_is_q2u == 1 && calendar_cfg == 0)
    {
        /* Verification for Q2U. Total client speed is 200G */
        client_speed[0] = 5000;
        client_speed[1] = 10000;
        client_speed[2] = 15000;
        client_speed[3] = 25000;
        client_speed[4] = 75000;
        client_speed[5] = 70000;
    }
    if (dev_is_q2u == 1 && calendar_cfg >= 100)
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
 */


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
        rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_flexe_phys);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create BusA clients and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed[i], 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed[i], 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* Setting FlexE flows between BusA and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set BusA2BusB FlexE flow(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set BusB2BusA FlexE flow(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* L2 settings between ETH port and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            /* ETH to BusB_i: L2 lookup(DMAC=00:00:00:00:22:22, VID=vlan_id_base+i) */
            rv = dnx_flexe_util_L2_switch_config(unit, eth_port, client_b_base+i, vlan_id_base+i);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L2_switch_config(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            /* BusB_i to ETH: force_forward */
            rv = bcm_port_force_forward_set(unit, client_b_base+i, eth_port, 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* Calendar and OH settings */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);

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

        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_A) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 80; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, FLEXE_CAL_B, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_B) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Group ID in overhead */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

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
        rv = dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
        rv |= dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, Rx) ****\n", proc_name, rv, unit_cen);
            return rv;
        }

        /* Set calendar Rx on distributed device with calendar Tx setting on centralized device */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_unit0[i];
            Cal_Slots_B[i] = cal_slots_unit0[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
        rv |= dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, Rx) ****\n", proc_name, rv, unit_dis);
            return rv;
        }
    }

    /* Default VLAN action set */
    rv = dnx_flexe_util_egr_default_vlan_action_set(unit_cen);
    rv |= dnx_flexe_util_egr_default_vlan_action_set(unit_dis);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_egr_default_vlan_action_set ****\n", proc_name, rv);
        return rv;
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv, unit_iter, unit;
    int grp_speed = dev_is_q2u ? 200000 : 400000;
    int total_slots = grp_speed / 5000;
    bcm_port_t client_a_base = 30;
    int client_id = client_a_base + client_index;
    int cal_slots_init_tx[80], cal_slots_init_rx[80];
    int cal_slots_tx[80], cal_slots_rx[80];

    bcm_gport_t flexe_grp;
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    bcm_port_flexe_group_cal_t cal_active;
    uint32 cal_flags = is_sync_slots ? 0 : BCM_PORT_FLEXE_TX;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_calendar_switch_with_mixed_clients_main";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

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
 */

    /* Do calendar switching on the 2 devices */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Check the provided calendar ID should be in inactive status */
        rv = bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_active_get(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        if (cal_active == calendar_id && !all_clients_removed) {
            printf("**** %s:  Error, the provided calendar ID is already in active status ****\n", proc_name);
            return BCM_E_FAIL;
        }
        if (cal_active != calendar_id && all_clients_removed) {
            printf("**** %s:  Error, the provided calendar ID isn't active calendar ID ****\n", proc_name);
            return BCM_E_FAIL;
        }

        /* Get slot assignment on active calendar */
        rv = bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 80, cal_slots_tx, &i);
        rv |= bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 80, cal_slots_rx, &i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_slots_get(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

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
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, cal_flags, calendar_id, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Configure the inactive calendar(Rx only for asynchronous slot assignment) */
        if (!is_sync_slots) {
            for (i = 0; i < 80; i++) {
                Cal_Slots_A[i] = cal_slots_rx[i];
                Cal_Slots_B[i] = cal_slots_rx[i];
            }
            rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, calendar_id, total_slots);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
                return rv;
            }

        }

        /* Set the the inactive calendar as CalInUse in overhead before the switching */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, calendar_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Do calendar switching by activing the inactive calendar */
        rv = bcm_port_flexe_group_cal_active_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to active calendar %d on unit %d ****\n", proc_name, rv, calendar_id, unit);
            return rv;
        }
    }
    sal_sleep(3);

    /* Set backup calendar to be the same as active calendar
     * It is used to avoid packet loss during calendar switching */
    for (i = 0; i < 80; i++) {
        Cal_Slots_A[i] = cal_slots_tx[i];
        Cal_Slots_B[i] = cal_slots_tx[i];
    }
    rv = dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_RX, !calendar_id, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(cen_rx, cal_backup) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, !calendar_id, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(dis_tx, cal_backup) ****\n", proc_name, rv);
        return rv;
    }

    for (i = 0; i < 80; i++) {
        Cal_Slots_A[i] = cal_slots_rx[i];
        Cal_Slots_B[i] = cal_slots_rx[i];
    }
    rv = dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_TX, !calendar_id, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(cen_tx, cal_backup) ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_RX, !calendar_id, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(dis_rx, cal_backup) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, rv, unit_iter, unit;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_phy_add_remove_main";

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
 */

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;
        eth_port = eth_ports[unit_iter];

        /* Create FlexE group with the PHYs in array flexe_phy_ports[] */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
        rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys_init);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create BusA clients and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, client_speed, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3, client_speed, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* Setting FlexE flows between BusA and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set BusA2BusB FlexE flow(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set BusB2BusA FlexE flow(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* L2 settings between ETH port and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            /* ETH to BusB_i: L2 lookup(DMAC=00:00:00:00:22:22, VID=vlan_id_base+i) */
            rv = dnx_flexe_util_L2_switch_config(unit, eth_port, client_b_base+i, vlan_id_base+i);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L2_switch_config(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            /* BusB_i to ETH: force_forward */
            rv = bcm_port_force_forward_set(unit, client_b_base+i, eth_port, 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* Calendar setting */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        slot_index = 0;
        for (phy_index = 0; phy_index < max_phys; phy_index++) {
            if ((1 << phy_index) & phy_bmp) {
                /* Need assign slots on this PHY */
                for (i = 0; i < nof_slots_per_client; i++) {
                    Cal_Slots_A[slot_index++] = phy_index + client_a_base;
                }
            }
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_A) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Group ID in overhead */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Default VLAN action set */
    rv = dnx_flexe_util_egr_default_vlan_action_set(unit_cen);
    rv |= dnx_flexe_util_egr_default_vlan_action_set(unit_dis);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_egr_default_vlan_action_set ****\n", proc_name, rv);
        return rv;
    }

    /* Synce check */
    sal_sleep(6);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv, unit_iter, unit;
    bcm_gport_t flexe_grp;
    int total_slots;
    int nof_slots_per_phy = phy_speed / 5000;
    int cal_slots_tx[80];
    int client_id_in_cal = clear_cal_flag ? 0 : client_id;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_phy_cal_set_clear";

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
 */

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Get calendar setting */
        rv = bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, 80, cal_slots_tx, &total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_slots_get(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Set or clear BusA client on the PHY */
        for (i = 0; i < total_slots; i++) {
            if (i >= slot_start_on_phy && i <= slot_end_on_phy) {
                cal_slots_tx[i] = client_id_in_cal;
            }
        }

        /* Calendar setting */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (i = 0; i < total_slots; i++) {
            Cal_Slots_A[i] = cal_slots_tx[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    return rv;
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
    int i, ii, rv;
    int unit[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 400000;
    int client_speed = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    int nof_slots_per_client = client_speed / 5000;
    bcm_gport_t flexe_grp;
    flexe_port_type_t client_b1_type = eth1_is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;
    flexe_port_type_t client_b2_type = eth2_is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_1 = 30;
    bcm_port_t client_a_2 = 31;
    bcm_port_t client_b_1 = 32;
    bcm_port_t client_b_2 = 33;
    bcm_port_t client_c_1 = 34;
    bcm_port_t client_c_2 = 35;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_cen_dis_mix_L1_L2_traffic_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a_1, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_2, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_1, client_b1_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_2, client_b2_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_2 ****\n", proc_name, rv);
        return rv;
    }

    /* Create the 1st datapath: eth_1 <--> client_b_1 <--> client_a_1 */
    if (eth1_is_L1) {
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_1, client_b_1, client_a_1, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_1, client_b_1, eth_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1st datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_1, client_b_1, client_a_1, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a_1, client_b_1, eth_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1st datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Create the 2nd datapath: eth_2 <---> client_b_2 <--> client_a_2 */
    if (eth2_is_L1) {
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_2, client_b_2, client_a_2, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_2, client_b_2, eth_2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 2nd datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_2, client_b_2, client_a_2, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a_2, client_b_2, eth_2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 2nd datapath on centralized device ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a_1, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_2, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_1, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_2, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_c_1, port_type_busC, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_c_2, port_type_busC, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c_2 ****\n", proc_name, rv);
        return rv;
    }

    /* Starting to create 1st datapath */
    if (eth1_is_L1) {
        /* Flexe&ILKN L1 datapath: client_a_1 <--> client_c_1 <--> ilkn_1 */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a_1, client_c_1, ilkn_1, 0);
        rv |= dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_1, client_c_1, client_a_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1st datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Flexe&ILKN L2 datapath: client_a_1 <--> client_b_1 <--> ilkn_1 */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a_1, client_b_1, ilkn_1, 0);
        rv |= dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_1, client_b_1, client_a_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1st datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Starting to create 2nd datapath */
    if (eth2_is_L1) {
        /* Flexe&ILKN L1 datapath: client_a_2 <--> client_c_2 <--> ilkn_2 */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a_2, client_c_2, ilkn_2, 0);
        rv |= dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_2, client_c_2, client_a_2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1st datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Flexe&ILKN L2 datapath: client_a_2 <--> client_b_2 <--> ilkn_2 */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a_2, client_b_2, ilkn_2, 0);
        rv |= dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_2, client_b_2, client_a_2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1st datapath on distributed device ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients in BusA */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots_per_client*2; ii++) {
            Cal_Slots_A[ii] = ii < nof_slots_per_client ? client_a_1 : client_a_2;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Check PHY port link status to make sure the 2 FlexE phy ports can linkup */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_phy_port_link_check(unit[i], flexe_phy);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    /* Synce check */
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int rv;
    bcm_port_flexe_oh_alarm_t oh_alarm_get;

    /* Get OH alarms */
    sal_sleep(2);
    rv = bcm_port_flexe_oh_alarm_get(unit, phy_id, 0, &oh_alarm_get);
    sal_sleep(3);
    rv |= bcm_port_flexe_oh_alarm_get(unit, phy_id, 0, &oh_alarm_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_alarm_get ****\n", proc_name, rv);
        return rv;
    }

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
    int i, rv;
    int nof_pcs = 1;
    int flexe_grp_bandwidth = 50000;
    int nof_slots = flexe_grp_bandwidth / 5000;
    int client_speed = 10000;
    bcm_gport_t flexe_grp;

    /* Client IDs on the 2 devices */
    bcm_port_t client_a_L1 = 40; /* BusA client for L1 traffic on centralized device */
    bcm_port_t client_a_L2 = 41; /* BusA client for L2 traffic on centralized device */
    bcm_port_t client_b_L1 = 42; /* BusB client for L1 traffic on centralized device */
    bcm_port_t client_b_L2 = 43; /* BusB client for L2 traffic on centralized device */
    bcm_port_t client_a_1 = 40;  /* BusA client to send traffic on distributed device */
    bcm_port_t client_a_2 = 41;  /* BusA client to receive traffic on distributed device */
    bcm_port_t client_b_2 = 44;  /* BusB client on distributed device */

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_eth_port_and_flexe_phy_in_same_cdu_main";

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
 */

    /***************************************************************************
     * FlexE Setting on Q2A_#0(centralized)
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_cen, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L1, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_a_L2, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_L2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L1, port_type_busB_L1, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_cen, client_b_L2, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 Tx datapath: cen_port_L2 -> force_fwd -> client_b_L2 -> client_a_L2 */
    rv =  dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, cen_port_L2, client_b_L2, client_a_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L2 Tx datapath ****\n", proc_name, rv);
        return rv;
    }

    /* Create L1 Rx datapath: client_a_L1 -> client_b_L1 -> cen_port_L1 */
    rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a_L1, client_b_L1, cen_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L1 Rx datapath ****\n", proc_name, rv);
        return rv;
    }

    /* Redirect traffic from cen_port_L1 to cen_port_L2: cen_port_L1 -> client_b_L1 -> client_b_L2 -> cen_port_L2 */
    rv = bcm_port_enable_set(unit_cen, cen_port_L1, 0);
    rv |= bcm_port_enable_set(unit_cen, client_b_L1, 0);
    rv |= bcm_port_flexe_flow_set(unit_cen, 0, cen_port_L1, 0, client_b_L1);
    rv |= bcm_port_enable_set(unit_cen, cen_port_L1, 1);
    rv |= bcm_port_enable_set(unit_cen, client_b_L1, 1);
    rv |= dnx_flexe_util_L1_datapath_client_to_client(unit_cen, client_b_L1, client_b_L2, 0);
    rv |= bcm_port_force_forward_set(unit_cen, client_b_L2, cen_port_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to redirect traffic from cen_port_L1 to cen_port_L2 ****\n", proc_name, rv);
        return rv;
    }

    /* Calendar and OH settings */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    Cal_Slots_A[0] = client_a_1;
    Cal_Slots_A[1] = client_a_1;
    Cal_Slots_A[2] = client_a_2;
    Cal_Slots_A[3] = client_a_2;
    rv = dnx_flexe_util_calendar_slots_set(unit_cen, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit_cen, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit_cen);
        return rv;
    }

    /***************************************************************************
     * FlexE Setting on Q2A_#1(Distributed)
 */

    /* Create FlexE group with the provided FlexE PHY */
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a_1, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_a_2, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_2, port_type_busB_L1, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_2 ****\n", proc_name, rv);
        return rv;
    }

    /* Create L1 datapath: client_a_2 --> client_b_2 --> dis_port_L1 */
    rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, client_a_2, client_b_2, dis_port_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L1 datapath(Flexe2Eth) on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Create L1 datapath: client_a_1 <-- client_b_2 <-- dis_port_L1 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, dis_port_L1, client_b_2, client_a_1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set L1 datapath(Eth2Flexe) on distributed device ****\n", proc_name, rv);
        return rv;
    }

    /* Calendar and OH settings */
    rv = dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Check PHY port link status */
    rv = dnx_flexe_util_phy_port_link_check(unit_dis, flexe_phy);
    if (rv != BCM_E_NONE) {
        printf("**** %s: FlexE PHY port linkdown ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, ii, rv;
    int nof_pcs = 1;
    int unit;
    bcm_port_t client_a = 40;
    bcm_port_t client_b = 50;
    bcm_gport_t flexe_grp;
    int client_speed_tx;
    int client_speed_rx;
    int nof_slots_tx;
    int nof_slots_rx;
    int total_slots = dev_is_q2u ? 20 : 80; /* Need 400G FlexE PHY */
    uint32 flag_asym_rate = BCM_PORT_RESOURCE_ASYMMETRICAL;
    flexe_port_type_t client_b_type = is_L1_switch ? port_type_busB_L1 : port_type_busB_L2_L3;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_asym_client_speed_busA_busB";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Datapath:
     *                             Centralized #0               Centralized #1
     *                         +-----------------------+    +-----------------------+
     * eth_port_cen <--L1/L2-->| client_b <-> client_a |<-->| client_a <-> client_b |<--L1/L2--> eth_port_dis
     *                         +-----------------------+    +-----------------------+
 */

    /***************************************************************************
     * FlexE setting on centralized #0
 */
    unit = unit_cen;

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create a busA client and a busB client with asymmetrical speeds */
    client_speed_tx = client_tx_rate;
    client_speed_rx = client_rx_rate;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b, client_b_type, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Start to build ETH2FlexE datapath: eth_port_cen <-> client_b <-> client_a */
    if (is_L1_switch) {
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_cen, client_b, client_a, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_cen, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_cen, client_b, client_a, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_cen, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L2 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / 5000;
    nof_slots_rx = client_speed_rx / 5000;
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, tx) ****\n", proc_name, rv, unit);
        return rv;
    }

    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, rx) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on centralized #1
 */
    unit = unit_dis;

    /* Create FlexE group */
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create a busA client and a busB client with asymmetrical speeds */
    client_speed_tx = client_rx_rate;
    client_speed_rx = client_tx_rate;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b, client_b_type, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Start to build ETH2FlexE datapath: eth_port_dis <-> client_b <-> client_a */
    if (is_L1_switch) {
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port_dis, client_b, client_a, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_dis, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port_dis, client_b, client_a, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port_dis, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L2 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / 5000;
    nof_slots_rx = client_speed_rx / 5000;
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, Tx) ****\n", proc_name, rv, unit);
        return rv;
    }

    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, Rx) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check link status of FlexE PHY(s) */
    rv = dnx_flexe_util_phy_port_link_check(unit_cen, phy_port);
    rv |= dnx_flexe_util_phy_port_link_check(unit_dis, phy_port);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), FlexE PHY port linkdown ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, ii, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_asym_client_speed_distributed_mode";

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
 */

    /***************************************************************************
     * FlexE setting on distributed device
 */
    unit = unit_dis;

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = phy_port;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create FlexE clients with asymmetrical speeds */
    client_speed_tx = rate_ilkn2flexe;
    client_speed_rx = rate_flexe2ilkn;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    if (is_L1_switch_dis) {
        rv = dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed_tx, flag_asym_rate);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busC client(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    } else {
        rv = dnx_flexe_util_client_create(unit, client_b, port_type_busB_L2_L3, client_speed_tx, flag_asym_rate);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Build L1 or L2 datapath between FlexE and ILKN */
    if (is_L1_switch_dis)
    {
        /* Flexe2ILKN L1 datapath: client_a <-> client_c <-> ilkn_port */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit, client_a, client_c, ilkn_port, 0);
        rv |= dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit, ilkn_port, client_c, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath between FlexE and ILKN ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Flexe2ILKN L2/L3 datapath: client_a <-> client_b <-> ilkn_port */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit, client_a, client_b, ilkn_port, 0);
        rv |= dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit, ilkn_port, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L2 datapath between FlexE and ILKN ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / 5000;
    nof_slots_rx = client_speed_rx / 5000;
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, tx) ****\n", proc_name, rv, unit);
        return rv;
    }

    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, rx) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on centralized device
 */
    unit = unit_cen;

    /* Create FlexE group */
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create FlexE clients with asymmetrical speeds */
    client_speed_tx = rate_flexe2ilkn;
    client_speed_rx = rate_ilkn2flexe;
    client_rx_speed = client_speed_rx; /* global variable for client Rx speed */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b, client_b_type_cen, client_speed_tx, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Build L1 or L2 datapath between ETH and FlexE */
    if (is_L1_switch_cen)
    {
        /* L1 datapath between ETH and FlexE  */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath between ETH and FlexE ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* L2 datapath between ETH and FlexE  */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L2 datapath between ETH and FlexE ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Different calendar settings in Tx and Rx */
    nof_slots_tx = client_speed_tx / 5000;
    nof_slots_rx = client_speed_rx / 5000;
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_tx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, tx) ****\n", proc_name, rv, unit);
        return rv;
    }

    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_rx; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, rx) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    /* Check link status of FlexE PHY(s) */
    rv = dnx_flexe_util_phy_port_link_check(unit_cen, phy_port);
    rv |= dnx_flexe_util_phy_port_link_check(unit_dis, phy_port);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), FlexE PHY port linkdown ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to test asymmetrical client speeds between ETH_L1 and ILKN_L1 */
int dnx_flexe_asym_client_speed_busB_busC(
    int unit,
    bcm_port_t eth_port,
    bcm_port_t ilkn_port,
    int rate_eth2ilkn,
    int rate_ilkn2eth)
{
    int i, rv;
    bcm_port_t client_b = 51;
    bcm_port_t client_c = 52;
    uint32 flag_asym_rate = BCM_PORT_RESOURCE_ASYMMETRICAL;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_asym_client_speed_busB_busC";

    /* Create client_b */
    client_rx_speed = rate_ilkn2eth; /* global variable for client Rx speed */
    rv = dnx_flexe_util_client_create(unit, client_b, port_type_busB_L1, rate_eth2ilkn, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b ****\n", proc_name, rv);
        return rv;
    }

    /* Create client_c */
    client_rx_speed = rate_eth2ilkn; /* global variable for client Rx speed */
    rv = dnx_flexe_util_client_create(unit, client_c, port_type_busC, rate_ilkn2eth, flag_asym_rate);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_c ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Datapath:
     *                                        Distributed Device
     *                          +---------------------------------------------+
     *                          |                                             |
     *  eth_port <--L1_switch-->| client_b<-->client_c<-->ilkn_port(loopback) |
     *                          |                                             |
     *                          +---------------------------------------------+
 */

    /* Start to build Eth2ILKN L1 datapath: eth_port -> client_b -> client_c -> ilkn_port */
    rv = dnx_flexe_util_L1_datapath_eth_to_ilkn(unit, eth_port, client_b, client_c, ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set Eth2ILKN L1 datapath ****\n", proc_name, rv);
        return rv;
    }

    /* Start to build ILKN2Eth L1 datapath: ilkn_port -> client_c -> client_b -> eth_port */
    rv = dnx_flexe_util_L1_datapath_ilkn_to_eth(unit, ilkn_port, client_c, client_b, eth_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set ILKN2Eth L1 datapath ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to test multiple clients with asymmetrical client speeds */
int dnx_flexe_cal_with_asym_client_speeds(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_cen,
    bcm_port_t eth_dis,
    bcm_port_t flexe_phy)
{
    int i, ii, iii, rv, unit_iter, unit;
    int nof_flexe_phys = 1;
    int grp_speed = dev_is_q2u ? 100000 : 400000;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_cal_with_asym_client_speeds";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

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
 */

    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;
        eth_port = eth_ports[unit_iter];

        /***************************************************************************
         * FlexE group and client creation
 */

        /* Create FlexE group with the 400G FlexE PHY */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        flexe_phy_ports[0] = flexe_phy;
        rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_flexe_phys);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create BusA clients and BusB clients with asymmetrical speeds */
        for (i = 0; i < nof_clients; i++) {
            client_rate_tx = unit_iter ? client_speed_dis2cen[i] : client_speed_cen2dis[i];
            client_rate_rx = unit_iter ? client_speed_cen2dis[i] : client_speed_dis2cen[i];
            client_rx_speed = client_rate_rx; /* global variable for client Rx speed */

            rv = dnx_flexe_util_client_create(unit, client_a_base+i, port_type_busA, 0, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
            rv = dnx_flexe_util_client_create(unit, client_b_base+i, port_type_busB_L2_L3,client_rate_tx, flag_asym_rate);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /***************************************************************************
         * Build datapath
 */

        /* Setting FlexE flows between BusA and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_a_base+i, client_b_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set BusA2BusB FlexE flow(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            rv = dnx_flexe_util_L1_datapath_client_to_client(unit, client_b_base+i, client_a_base+i, 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set BusB2BusA FlexE flow(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /* L2 settings between ETH port and BusB clients */
        for (i = 0; i < nof_clients; i++) {
            /* ETH to BusB_i: L2 lookup(DMAC=00:00:00:00:22:22, VID=vlan_id_base+i) */
            rv = dnx_flexe_util_L2_switch_config(unit, eth_port, client_b_base+i, vlan_id_base+i);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in dnx_flexe_util_L2_switch_config(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }

            /* BusB_i to ETH: force_forward */
            rv = bcm_port_force_forward_set(unit, client_b_base+i, eth_port, 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(unit %d, index %d) ****\n", proc_name, rv, unit, i);
                return rv;
            }
        }

        /***************************************************************************
         * Calendar setting in Tx direction
 */

        /* Allocate slots for each client in Tx direction */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        slot_index = 0;
        for (i = 0; i < nof_clients; i++) {
            client_id = client_a_base + i;
            client_rate_tx = unit_iter ? client_speed_dis2cen[i] : client_speed_cen2dis[i];
            nof_slots_per_client = client_rate_tx / 5000;

            for (ii = 0; ii < nof_slots_per_client; ii++) {
                Cal_Slots_A[slot_index++] = client_id;
            }
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_A) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 80; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, FLEXE_CAL_B, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_B) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Use array cal_slots_unit0[] to store Tx slots */
        for (i = 0; i < 80; i++) {
            if (unit_iter == 0) {
                cal_slots_unit0[i] = Cal_Slots_A[i];
            }
        }

        /***************************************************************************
         * Calendar setting in Rx direction
 */

        /* Allocate slots for each client in Rx direction */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        slot_index = 0;
        for (i = 0; i < nof_clients; i++) {
            client_id = client_a_base + i;
            client_rate_rx = unit_iter ? client_speed_cen2dis[i] : client_speed_dis2cen[i];
            nof_slots_per_client = client_rate_rx / 5000;

            for (ii = 0; ii < nof_slots_per_client; ii++) {
                Cal_Slots_A[slot_index++] = client_id;
            }
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_A) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Do the same setting on calendar_B to avoid pacekt loss during calendar switching */
        for (i = 0; i < 80; i++) {
            Cal_Slots_B[i] = Cal_Slots_A[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, FLEXE_CAL_B, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, CAL_B) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Use array cal_slots_unit1[] to store Rx slots */
        for (i = 0; i < 80; i++) {
            if (unit_iter == 0) {
                cal_slots_unit1[i] = Cal_Slots_A[i];
            }
        }

        /* Group ID in overhead */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Default VLAN action set */
    rv = dnx_flexe_util_egr_default_vlan_action_set(unit_cen);
    rv |= dnx_flexe_util_egr_default_vlan_action_set(unit_dis);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_egr_default_vlan_action_set ****\n", proc_name, rv);
        return rv;
    }

    /* Synce check */
    sal_sleep(3);
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv, unit_iter, unit;
    int grp_speed = dev_is_q2u ? 100000 : 400000;
    int total_slots = grp_speed / 5000;
    bcm_port_flexe_group_cal_t cal_active;

    int cal_slots_init_tx[80], cal_slots_init_rx[80];
    int cal_slots_tx[80], cal_slots_rx[80];

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_calendar_switch_with_asym_client_speeds";

    /* Q2U detection */
    if (dev_is_q2u < 0) {
        printf("**** Error: Fail in Q2U detection. dev_is_q2u %d should be in range 0~1 ****\n", dev_is_q2u);
        return BCM_E_FAIL;
    }

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
 */

    /* Do calendar switching on the 2 devices */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Get slot assignment on active calendar */
        rv = bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active);
        rv |= bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 80, cal_slots_tx, &i);
        rv |= bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 80, cal_slots_rx, &i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_slots_get(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

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
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, Tx) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Configure the inactive calendar(Rx) */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_rx[i];
            Cal_Slots_B[i] = cal_slots_rx[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, calendar_id, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d, Rx) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Set the the inactive calendar as CalInUse in overhead before the switching */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhCalInUse, calendar_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_oh_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Do calendar switching by activing the inactive calendar */
        rv = bcm_port_flexe_group_cal_active_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, calendar_id);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to active calendar %d on unit %d ****\n", proc_name, rv, calendar_id, unit);
            return rv;
        }
    }
    sal_sleep(3);

    /* Set backup calendar to be the same as active calendar
     * It is used to avoid packet loss during calendar switching
     */
    for (unit_iter = 0; unit_iter < 2; unit_iter++) {
        unit = unit_iter ? unit_dis : unit_cen;

        /* Get slot assignment on active calendar */
        rv = bcm_port_flexe_group_cal_active_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, &cal_active);
        rv |= bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_TX, cal_active, 80, cal_slots_tx, &i);
        rv |= bcm_port_flexe_group_cal_slots_get(unit, flexe_grp, BCM_PORT_FLEXE_RX, cal_active, 80, cal_slots_rx, &i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_cal_slots_get(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Do same setting on backup calendar */
        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_tx[i];
            Cal_Slots_B[i] = cal_slots_tx[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, !cal_active, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(cen_rx, cal_backup) ****\n", proc_name, rv);
            return rv;
        }

        for (i = 0; i < 80; i++) {
            Cal_Slots_A[i] = cal_slots_rx[i];
            Cal_Slots_B[i] = cal_slots_rx[i];
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, BCM_PORT_FLEXE_RX, !cal_active, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(dis_tx, cal_backup) ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
}

int dnx_flexe_traffic_with_der_soft(
    int unit,
    bcm_port_t flexe_phy,
    bcm_port_t eth_1,
    bcm_port_t eth_2,
    int is_L1)
{
    int i, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_traffic_with_der_soft";

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
 */

    /***************************************************************************
     * Create FlexE group and clients
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit, client_a_1, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_a_2, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a_2 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b_1, client_b_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_1 ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b_2, client_b_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_2 ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Build datapath
 */

    /* Disable ports before setting FlexE flows */
    rv = bcm_port_enable_set(unit, client_b_1, 0);
    rv |= bcm_port_enable_set(unit, client_b_2, 0);
    if (is_L1) {
        rv |= bcm_port_enable_set(unit, eth_1, 0);
        rv |= bcm_port_enable_set(unit, eth_2, 0);
    }
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable flexe ports ****\n", proc_name, rv);
        return rv;
    }

    /* Start to build the L1 or L2 datapath */
    if (is_L1) {
        rv = bcm_port_flexe_flow_set(unit, 0, eth_1, 0, client_b_1);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, client_a_1);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_a_1, 0, client_b_2);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, eth_2);
        rv |= bcm_port_flexe_flow_set(unit, 0, eth_2, 0, client_b_2);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_a_2);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_a_2, 0, client_b_1);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, eth_1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to build FlexE L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    } else {
        rv = bcm_port_force_forward_set(unit, eth_1, client_b_1, 1);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_b_1, 0, client_a_1);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_a_1, 0, client_b_2);
        rv |= bcm_port_force_forward_set(unit, client_b_2, eth_2, 1);
        rv |= bcm_port_force_forward_set(unit, eth_2, client_b_2, 1);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_b_2, 0, client_a_2);
        rv |= bcm_port_flexe_flow_set(unit, 0, client_a_2, 0, client_b_1);
        rv |= bcm_port_force_forward_set(unit, client_b_1, eth_1, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to build FlexE L2 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, client_b_1, 1);
    rv |= bcm_port_enable_set(unit, client_b_2, 1);
    if (is_L1) {
        rv |= bcm_port_enable_set(unit, eth_1, 1);
        rv |= bcm_port_enable_set(unit, eth_2, 1);
    }
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable flexe ports ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Calendar setting */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots_per_client*2; i++) {
        Cal_Slots_A[i] = i < nof_slots_per_client ? client_a_1 : client_a_2;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set ****\n", proc_name, rv);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID ****\n", proc_name, rv);
        return rv;
    }

    /* Set PHY loopback on eth_2 and flexe_phy */
    rv = bcm_port_loopback_set(unit, eth_2, 2);
    rv |= bcm_port_loopback_set(unit, flexe_phy, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to test MIB counters on FlexE ports(BusB_L1, BusB_L2 and ETH_L1) */
int dnx_flexe_mib_eth_and_busb_common(
    int unit,
    bcm_port_t eth_L2,
    bcm_port_t eth_L1,
    int port_speed)
{
    int rv;
    uint32 flags = 0;
    bcm_port_t client_a = 80;
    bcm_port_t client_b_L1 = 81;
    bcm_port_t client_b_L2 = 82;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_mib_eth_and_busb_common";

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create a busA client */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, port_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client ****\n", proc_name, rv);
        return rv;
    }

    /* Create BusB L1 client */
    rv = dnx_flexe_util_client_create(unit, client_b_L1, port_type_busB_L1, port_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L1 ****\n", proc_name, rv);
        return rv;
    }

    /* Create BusB L2 client */
    rv = dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, port_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Datapath:
     * CPU.TxRx <---> eth_L2 <--force_fwd--> client_b_L2 <--L1_switch--> client_b_L1 <--L1_switch--> eth_L1(loopback)
 */

    /* Disable ports */
    rv = bcm_port_enable_set(unit, eth_L1, 0);
    rv |= bcm_port_enable_set(unit, client_b_L1, 0);
    rv |= bcm_port_enable_set(unit, client_b_L2, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable FlexE ports ****\n", proc_name, rv);
        return rv;
    }

    /* Set FlexE flows: client_b_L1 <--> eth_L1 */
    rv = bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, eth_L1);
    rv |= bcm_port_flexe_flow_set(unit, flags, eth_L1, 0, client_b_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE flow between client_b_L1 and eth_L1 ****\n", proc_name, rv);
        return rv;
    }

    /* Set FlexE flows: client_b_L1 <--> client_b_L2 */
    rv = bcm_port_flexe_flow_set(unit, flags, client_b_L1, 0, client_b_L2);
    rv |= bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_b_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE flow between client_b_L1 and client_b_L2 ****\n", proc_name, rv);
        return rv;
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, eth_L1, 1);
    rv |= bcm_port_enable_set(unit, client_b_L1, 1);
    rv |= bcm_port_enable_set(unit, client_b_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable FlexE ports ****\n", proc_name, rv);
        return rv;
    }

    /* Set L2 datapath: eth_L2 <--force_fwd--> client_b_L2 */
    rv = bcm_port_force_forward_set(unit, eth_L2, client_b_L2, 1);
    rv |= bcm_port_force_forward_set(unit, client_b_L2, eth_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in force_fwd setting ****\n", proc_name, rv);
        return rv;
    }

    /* Set PHY loopback on eth_L1 */
    rv = bcm_port_loopback_set(unit, eth_L1, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Function to test MIB counters on ILKN_L1 port */
int dnx_flexe_mib_ilkn_L1_common(
    int unit,
    bcm_port_t eth_L2,
    bcm_port_t ilkn_L1,
    bcm_port_t flexe_phy,
    int client_speed)
{
    int i, rv;
    uint32 flags = 0;
    bcm_port_t client_b_L2 = 80;
    bcm_port_t client_c = 81;
    bcm_port_t client_a = 82;
    bcm_gport_t flexe_grp;
    int nof_slots = client_speed / 5000;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_mib_ilkn_L1_common";

    /***************************************************************************
     * FlexE group and client(s) creation
 */

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create ****\n", proc_name, rv);
        return rv;
    }

    /* Create BusA client */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client ****\n", proc_name, rv);
        return rv;
    }

    /* Create BusB L2 client */
    rv = dnx_flexe_util_client_create(unit, client_b_L2, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_L2 ****\n", proc_name, rv);
        return rv;
    }

    /* Create BusC client */
    rv = dnx_flexe_util_client_create(unit, client_c, port_type_busC, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busC client ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Datapath:
     * CPU.TxRx ---> eth_L2 --force_fwd--> client_b_L2 --L1_switch--> client_a -->(loopback)
     * ---> client_c --L1_switch--> ilkn_L1(loopback)
 */

    /* Disable ports */
    rv = bcm_port_enable_set(unit, client_b_L2, 0);
    rv |= bcm_port_enable_set(unit, ilkn_L1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to disable FlexE ports ****\n", proc_name, rv);
        return rv;
    }

    /* Set FlexE flows: client_b_L2 --> client_a --> client_c --> ilkn_L1 */
    rv = bcm_port_flexe_flow_set(unit, flags, client_b_L2, 0, client_a);
    rv |= bcm_port_flexe_flow_set(unit, flags, client_a, 0, client_c);
    rv |= bcm_port_flexe_flow_set(unit, flags, client_c, 0, ilkn_L1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set FlexE L1 datapath ****\n", proc_name, rv);
        return rv;
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, client_b_L2, 1);
    rv |= bcm_port_enable_set(unit, ilkn_L1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to enable FlexE ports ****\n", proc_name, rv);
        return rv;
    }

    /* Set L2 datapath: eth_L2 --force_fwd--> client_b_L2 */
    rv = bcm_port_force_forward_set(unit, eth_L2, client_b_L2, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in force_fwd setting ****\n", proc_name, rv);
        return rv;
    }

    /* Set PHY loopback on ilkn_L1 and FlexE PHY */
    rv = bcm_port_loopback_set(unit, ilkn_L1, 2);
    rv |= bcm_port_loopback_set(unit, flexe_phy, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH setting
 */

    /* Assign slots for the FlexE client */
    for (i = 0; i < nof_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set ****\n", proc_name, rv);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int i, ii, rv, unit;
    int nof_phys = 2;
    int units[2] = {unit_cen, unit_dis};
    bcm_port_t eth_port, eth_ports[2] = {eth_cen, eth_dis};
    int total_slots = group_speed / 5000;
    int client_slots = client_speed / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 30;
    bcm_port_t client_b = 41;

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
 */

    /***************************************************************************
     * FlexE Setting on the 2 devices
 */

    for (i = 0; i < 2; i++) {
        unit = units[i];
        eth_port = eth_ports[i];
        skip_synce_in_grp_creation = 1; /* Skip synce setting */

        /* Create FlexE group with provided FlexE PHYs */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        flexe_phy_ports[0] = phy_0;
        flexe_phy_ports[1] = phy_1;
        rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create FlexE clients */
        rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create client_a(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit, client_b, port_type_busB_L1, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create client_b(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Create L1 datapath: eth_port <- L1_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* Assign slots on the 2nd PHY */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = total_slots/2; ii < (total_slots/2)+client_slots; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        /* FlexE OH setting */
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /***************************************************************************
     * Other Settings
 */

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
        rv = dnx_flexe_util_phy_port_link_check(units[i], phy_0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: FlexE PHY port linkdown on unit %d ****\n", proc_name, unit[i]);
            return rv;
        }
    }

    return rv;
}

/* Function to create 7 L1 snakes with different speeds 
 * Used to test the scenario of multiple L1 clients with huge client speed differences
 */
int dnx_flexe_mixed_L1_clients_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t flexe_phy)
{
    int i, ii, iii, rv, unit;
    int units[2] = {unit_cen, unit_dis};
    int nof_pcs = 1;
    int grp_speed = 400000;
    int total_slots = grp_speed / 5000;
    int slot_index = 0;
    int nof_slots_per_client;
    bcm_gport_t flexe_grp;

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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_mixed_L1_clients_main";

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
 */

    /***************************************************************************
     * Create FlexE groups and clients
 */

    for (i = 0; i < 2; i++) {
        unit = units[i];

        /* Create FlexE group with the FlexE PHY */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
        flexe_phy_ports[0] = flexe_phy;
        rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        for (ii = 0; ii < 8; ii++) {
            /* Create BusA clients */
            rv = dnx_flexe_util_client_create(unit, BusA_clients[ii], port_type_busA, client_speeds[ii], 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusA client(unit %d, index %d) ****\n", proc_name, rv, unit, ii);
                return rv;
            }

            /* Create BusB clients */
            rv = dnx_flexe_util_client_create(unit, BusB_clients[ii], port_type_busB_L1, client_speeds[ii], 0);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to create BusB client(unit %d, index %d) ****\n", proc_name, rv, unit, ii);
                return rv;
            }
        }
    }

    /***************************************************************************
     * DataPath creation
 */

    /* Build 200G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_400g  <-->| BusB_200g <--> BusA_200g |<----->| BusA_200g <--> BusB_200g |<--> eth_400g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_400g, BusB_200g, BusA_200g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 200g snake(Eth2Flexe)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_200g, BusB_200g, eth_400g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 200g snake(Flexe2Eth)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(unit_dis, eth_400g, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_400g) ****\n", proc_name, rv);
        return rv;
    }

    /* Build 100G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_100g  <-->| BusB_100g <--> BusA_100g |<----->| BusA_100g <--> BusB_100g |<--> eth_100g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_100g, BusB_100g, BusA_100g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 100g snake(Eth2Flexe)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_100g, BusB_100g, eth_100g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 100g snake(Flexe2Eth)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(unit_dis, eth_100g, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_100g) ****\n", proc_name, rv);
        return rv;
    }

    /* Build 50G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_50g   <-->| BusB_50g  <--> BusA_50g  |<----->| BusA_50g  <--> BusB_50g  |<--> eth_50g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_50g, BusB_50g, BusA_50g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 50g snake(Eth2Flexe)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_50g, BusB_50g, eth_50g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 50g snake(Flexe2Eth)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(unit_dis, eth_50g, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_50g) ****\n", proc_name, rv);
        return rv;
    }

    /* Build 25G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_25g   <-->| BusB_25g  <--> BusA_25g  |<----->| BusA_25g  <--> BusB_25g  |<--> eth_25g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_25g, BusB_25g, BusA_25g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 25g snake(Eth2Flexe)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_25g, BusB_25g, eth_25g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 25g snake(Flexe2Eth)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(unit_dis, eth_25g, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_25g) ****\n", proc_name, rv);
        return rv;
    }

    /* Build 10G snake as below:
     *                                   Device 0                            Device 1
     *                          +--------------------------+       +--------------------------+
     *  (loopback)eth_100g_1<-->| BusB_10g  <--> BusA_10g  |<----->| BusA_10g  <--> BusB_10g  |<--> eth_100g_1<-->IXIA_100g
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_100g_1, BusB_10g, BusA_10g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 10g snake(Eth2Flexe)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_10g, BusB_10g, eth_100g_1, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 10g snake(Flexe2Eth)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(unit_cen, eth_100g_1, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_100g_1) ****\n", proc_name, rv);
        return rv;
    }

    /* Build ETH_1G to Client_5G snake as below:
     *                         Device 0                            Device 1
     *                +--------------------------+       +--------------------------+
     *  eth_1g    <-->| BusB_5g_2 <--> BusA_5g_2 |<----->| BusA_5g_2 <--> BusB_5g_2 |<--> eth_1g(loopback)
     */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_1g, BusB_5g_2, BusA_5g_2, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1g snake(Eth2Flexe)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, BusA_5g_2, BusB_5g_2, eth_1g, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set 1g snake(Flexe2Eth)(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(unit_dis, eth_1g, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_1g) ****\n", proc_name, rv);
        return rv;
    }

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
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_10g, BusB_5g_0, BusA_5g_0, 0);
    rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, BusA_5g_1, BusB_5g_1, eth_10g, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set 5g snake on unit %d ****\n", proc_name, rv, unit_cen);
        return rv;
    }
    /* Device 1 */
    rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_dis, eth_10g, BusB_5g_1, BusA_5g_1, 0);
    rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_dis, BusA_5g_0, BusB_5g_0, eth_10g, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set 5g snake on unit %d ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = bcm_port_loopback_set(unit_dis, eth_10g, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(eth_10g) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Do below settings on the 2 devices */
    for (i = 0; i < 2; i++) {
        unit = units[i];
        slot_index = 0;
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);

        /* Iterate each client and assign slots for it according to its speed */
        for (ii = 0; ii < 8; ii++) {
            /* Compute the number of slots for current client */
            nof_slots_per_client = client_speeds[ii] / 5000;

            /* Assign slots for the client */
            for (iii = 0; iii < nof_slots_per_client; iii++) {
                Cal_Slots_A[slot_index++] = BusA_clients[ii];
            }
        }

        rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }

        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Synce check */
    rv = dnx_flexe_util_synce_check(unit_cen, unit_dis, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Synce check failed ****\n", proc_name);
        return rv;
    }

    return rv;
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
    int i, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_prd_datapath_main";

    /* Get PHY speed to compute total slots */
	rv = bcm_port_resource_get(unit, flexe_phy, &resource);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_resource_get ****\n", proc_name, rv);
        return rv;
    }
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
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_b_tx, port_type_busB_L2_L3, client_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_prd, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create ****\n", proc_name, rv);
        return rv;
    }

    /* Disable BusB clients before building FlexE datapath */
    rv = bcm_port_enable_set(unit, client_b_tx, 0);
    rv |= bcm_port_enable_set(unit, client_prd, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(0) ****\n", proc_name, rv);
        return rv;
    }

    /* Building Tx datapath: eth_tx_lo/eth_tx_hi --force_fwd--> client_b_tx --> client_a */
    rv = bcm_port_force_forward_set(unit, eth_tx_lo, client_b_tx, 1);
    rv |= bcm_port_force_forward_set(unit, eth_tx_hi, client_b_tx, 1);
    rv |= bcm_port_flexe_flow_set(unit, flags, client_b_tx, 0, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bulding Tx datapath ****\n", proc_name, rv);
        return rv;
    }

    /* Building Rx datapath: client_a --> client_prd --L2_lookup--> eth_rx_lo/eth_rx_hi */
    rv = bcm_port_flexe_flow_set(unit, flags, client_a, 0, client_prd);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bulding Rx datapath(BusA2BusB) ****\n", proc_name, rv);
        return rv;
    }
    if (!rx_force_fwd) {
        /* L2 entry: mac_dst + vlan_lo --> eth_rx_lo */
        rv = dnx_flexe_util_L2_entry_advanced_set(unit, client_prd, eth_rx_lo, mac_dst, 1, vlan_lo, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in L2 setting for eth_rx_lo ****\n", proc_name, rv);
            return rv;
        }
        /* L2 entry: mac_dst + vlan_hi --> eth_rx_hi */
        rv = dnx_flexe_util_L2_entry_advanced_set(unit, client_prd, eth_rx_hi, mac_dst, 1, vlan_hi, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in L2 setting for eth_rx_hi ****\n", proc_name, rv);
            return rv;
        }
    } else {
        print "~~~~~~~~~ using force_fwd in Rx datapath ~~~~~~~~~~~~~~";
        rv = bcm_port_force_forward_set(unit, client_prd, eth_rx_lo, 1);
        rv |= bcm_port_force_forward_set(unit, client_prd, eth_rx_hi, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bulding Rx datapath(force_fwd) ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Enable BusB clients */
    rv = bcm_port_enable_set(unit, client_b_tx, 1);
    rv |= bcm_port_enable_set(unit, client_prd, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(1) ****\n", proc_name, rv);
        return rv;
    }

    /* Loopback FlexE PHY */
    rv = bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set ****\n", proc_name, rv);
        return rv;
    }

    /* Assign slots for BusA client */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < client_speed/5000; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set ****\n", proc_name, rv);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int dnx_flexe_prd_itmh_main(
    int unit,
    bcm_port_t flexe_phy,
    bcm_port_t client_prd,
    int client_speed,
    bcm_port_t eth_tx,
    bcm_port_t eth_rx)
{
    int i, rv;
    int nof_phys = 1;
    int total_slots;
    uint32 flags = 0;
    bcm_gport_t flexe_grp;
    bcm_port_t client_b_tx = 61;
    bcm_port_t client_a = 62;
    bcm_port_resource_t resource;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_prd_itmh_main";

    /* Get PHY speed to compute total slots */
	rv = bcm_port_resource_get(unit, flexe_phy, &resource);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_resource_get ****\n", proc_name, rv);
        return rv;
    }
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
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_phys);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_b_tx, port_type_busB_L1, client_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_prd, port_type_busB_L2_L3, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create ****\n", proc_name, rv);
        return rv;
    }

    /* Disable BusB clients before building FlexE datapath */
    rv = bcm_port_enable_set(unit, eth_tx, 0);
    rv |= bcm_port_enable_set(unit, client_b_tx, 0);
    rv |= bcm_port_enable_set(unit, client_prd, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(0) ****\n", proc_name, rv);
        return rv;
    }

    /* Building L1 Tx datapath: eth_tx --> client_b_tx --> client_a */
    rv = bcm_port_flexe_flow_set(unit, flags, eth_tx, 0, client_b_tx);
    rv |= bcm_port_flexe_flow_set(unit, flags, client_b_tx, 0, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bulding Tx datapath ****\n", proc_name, rv);
        return rv;
    }

    /* Building Rx datapath: client_a --> client_prd */
    rv = bcm_port_flexe_flow_set(unit, flags, client_a, 0, client_prd);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bulding Rx datapath(BusA2BusB) ****\n", proc_name, rv);
        return rv;
    }

    /* Enable BusB clients */
    rv = bcm_port_enable_set(unit, eth_tx, 1);
    rv |= bcm_port_enable_set(unit, client_b_tx, 1);
    rv |= bcm_port_enable_set(unit, client_prd, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(1) ****\n", proc_name, rv);
        return rv;
    }

    /* Loopback FlexE PHY */
    rv = bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set ****\n", proc_name, rv);
        return rv;
    }

    /* Assign slots for BusA client */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < client_speed/5000; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set ****\n", proc_name, rv);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID ****\n", proc_name, rv);
        return rv;
    }

    return rv;
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
    int rv;
    uint32 flags = 0;
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_config;

    /* Set Ethernet type for ether_type_code recognition */
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_set(unit, flexe_port, flags, ether_type_code, ether_type_val);
    if (rv != BCM_E_NONE) {
        printf("**** Error(%d): fail in bcm_cosq_ingress_port_drop_custom_ether_type_set ****\n", rv);
        return rv;
    }

    /* Set control frame on the ether_type_code */
    COMPILER_64_SET(control_frame_config.mac_da_val, dmac_first_2bytes, dmac_last_4bytes);
    COMPILER_64_SET(control_frame_config.mac_da_mask,mask_first_2bytes, mask_last_4bytes);
    control_frame_config.ether_type_code= ether_type_code;
    control_frame_config.ether_type_code_mask= 0xF;

    rv = bcm_cosq_ingress_port_drop_control_frame_set(unit, flexe_port, flags, control_frame_index, &control_frame_config);
    if (rv != BCM_E_NONE) {
        printf("**** Error(%d): fail in bcm_cosq_ingress_port_drop_control_frame_set ****\n", rv);
        return rv;
    }

    return rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_prd_sem_L2_main";
    print "~~~~~~~~~~~~~~~~~~~~ Starting semantic test for FlexE_MAC_L2 ~~~~~~~~~~~~~~~~~~~~";

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create BusB clients */
    rv = dnx_flexe_util_client_create(unit, flexe_port_0, port_type_busB_L2_L3, 5000, 0);
    rv |= dnx_flexe_util_client_create(unit, flexe_port_1, port_type_busB_L2_L3, 395000, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create() ****\n", proc_name, rv);
        return rv;
    }

    /* Set maximal RMC threshold */
    max_threshold_port_0 = 1;  /* 5000/5000 */
    max_threshold_port_1 = 79; /* 395000/5000 */

    /* Disable ports */
    rv = bcm_port_enable_set(unit, flexe_port_0, 0);
    rv |= bcm_port_enable_set(unit, flexe_port_1, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(disable) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * PRD Profile setting on FlexE ports
 */

    for (port_index = 0; port_index < 2; port_index++) {
        /* Iterate each PRD port profile */
        for (i = 0; i <= max_port_profile+1; i++) {
            rv = bcm_cosq_control_set(unit, flexe_ports[port_index], -1, bcmCosqControlIngressPortDropPortProfileMap, i);
            if (rv != BCM_E_NONE && i <= max_port_profile) {
                printf("**** %s: Error(%d), fail to set PRD PortProfileMap on port %d ****\n", proc_name, rv, flexe_ports[port_index]);
                return BCM_E_FAIL;
            }
            if (rv == BCM_E_NONE && i > max_port_profile) {
                printf("**** %s: Error(%d), no error for invalid PRD port profile(port %d) ****\n", proc_name, rv, flexe_ports[port_index]);
                return BCM_E_FAIL;
            }

            rv = bcm_cosq_control_get(unit, flexe_ports[port_index], -1, bcmCosqControlIngressPortDropPortProfileMap, &val_get);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to get PRD PortProfileMap on port %d ****\n", proc_name, rv, flexe_ports[port_index]);
                return BCM_E_FAIL;
            }
            if (val_get != i && i <= max_port_profile)  {
                printf("**** %s: Error, returned PRD port profile %d doesn't equal to %d ****\n", proc_name, val_get, i);
                return BCM_E_FAIL;
            }
        }
    }

    /***************************************************************************
     * Hard stage mapping
 */

    /* Use invalid priority */
    key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(1, 0);
    cosq_map = bcmCosqIngressPortDropEthPcpDeiToPriorityTable;
    priority_to_map = 6;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port_0, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error for bcm_cosq_ingress_port_drop_map_set(port %d) ****\n", proc_name, rv, flexe_ports[port_index]);
        return BCM_E_FAIL;
    }
    priority_to_map = 1;

    /* Use invalid key */
    key = 0x123;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port_0, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error for bcm_cosq_ingress_port_drop_map_set(port %d) ****\n", proc_name, rv, flexe_ports[port_index]);
        return BCM_E_FAIL;
    }
    key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(1, 0);

    /* Use invalid cosq_map */
    priority_to_map = 1;
    cosq_map = 10;
    rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_port_0, flags, cosq_map, key, priority_to_map);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error for bcm_cosq_ingress_port_drop_map_set(port %d) ****\n", proc_name, rv, flexe_ports[port_index]);
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
            rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_ports[port_index], flags, cosq_map, key, priority_to_map);
            if (priority_to_map == BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM) {
                /* L2 client doesn't support TDM pirority */
                if (rv == BCM_E_NONE) {
                    printf("**** %s: Error(%d), no error reported for TDM priority(port_index %d, type %d) ****\n", proc_name, rv, port_index, header_type);
                    return BCM_E_FAIL;
                }

                /* Configure hard stage mapping again with supported priority */
                priority_to_map = BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_1;
                rv = bcm_cosq_ingress_port_drop_map_set(unit, flexe_ports[port_index], flags, cosq_map, key, priority_to_map);
            }
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_map_set(port_index %d, type %d) ****\n", proc_name, rv, port_index, header_type);
                return rv;
            }

            /* Get hard stage mapping */
            rv = bcm_cosq_ingress_port_drop_map_get(unit, flexe_ports[port_index], flags, cosq_map, key, &priority_get);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_map_get(port_index %d, type %d) ****\n", proc_name, rv, port_index, header_type);
                return rv;
            }
            if (priority_get != priority_to_map) {
                printf("**** %s: Error, returned priority %d doesn't equal to %d(port_index %d, type %d) ****\n", proc_name, priority_get, priority_to_map, port_index, header_type);
                return BCM_E_FAIL;
            }
        }
    }

    /***************************************************************************
     * TPID/RMC_threshold/PRD_enable
 */

    /* Set TPID on FlexE port */
    for (i = 0; i < 4; i++) {
        /* Iterate each TPID */
        rv = bcm_cosq_control_set(unit, flexe_port_0, -1, tpid_list[i], 0x9200+i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set TPID %d ****\n", proc_name, rv, i);
            return rv;
        }
        rv = bcm_cosq_control_get(unit, flexe_port_0, -1, tpid_list[i], val_get);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to get TPID %d ****\n", proc_name, rv, i);
            return rv;
        }
        if (val_get != 0x9200+i) {
            printf("**** %s: Error, TPID %d: returned value %d isn't %d ****\n", proc_name, i, val_get, 0x9200+i);
            return BCM_E_FAIL;
        }
    }

    /* Set RMC threshold(TDM priority conflicts with FlexE) */
    for (i = 0; i < 4; i++) {
        /* Set minimal threshold */
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_0, flags, priority_list[i], 0);
        rv = bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_0, flags, priority_list[i], &val32_get_0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_threshold_set/get(min, pri %d) ****\n", proc_name, rv, i);
            return rv;
        }
        if (val32_get_0 != 0) {
            printf("**** %s: Error, threshold %d: returned value %d isn't 0 ****\n", proc_name, i, val32_get_0);
            return BCM_E_FAIL;
        }
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_1, flags, priority_list[i], 0);
        rv = bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_1, flags, priority_list[i], &val32_get_0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_threshold_set/get(min, pri %d) ****\n", proc_name, rv, i);
            return rv;
        }
        if (val32_get_0 != 0) {
            printf("**** %s: Error, threshold %d: returned value %d isn't 0 ****\n", proc_name, i, val32_get_0);
            return BCM_E_FAIL;
        }

        /* Set maximal threshold */
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_0, flags, priority_list[i], max_threshold_port_0);
        rv = bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_0, flags, priority_list[i], &val32_get_0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_threshold_set/get(max, pri %d) ****\n", proc_name, rv, i);
            return rv;
        }
        if (val32_get_0 != max_threshold_port_0) {
            printf("**** %s: Error, threshold %d: returned value %d isn't %d ****\n", proc_name, i, val32_get_0, max_threshold_port_0);
            return BCM_E_FAIL;
        }
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_1, flags, priority_list[i], max_threshold_port_1);
        rv = bcm_cosq_ingress_port_drop_threshold_get(unit, flexe_port_1, flags, priority_list[i], &val32_get_0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_threshold_set/get(max, pri %d) ****\n", proc_name, rv, i);
            return rv;
        }
        if (val32_get_0 != max_threshold_port_1) {
            printf("**** %s: Error, threshold %d: returned value %d isn't %d ****\n", proc_name, i, val32_get_0, max_threshold_port_1);
            return BCM_E_FAIL;
        }

        /* Set with outrange threshold */
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_0, flags, priority_list[i], max_threshold_port_0+1);
        if (rv == BCM_E_NONE) {
            printf("**** %s: Error(%d), no error for outrange threshold %d(pri %d) ****\n", proc_name, rv, max_threshold_port_0+1, i);
            return BCM_E_FAIL;
        }
        rv = bcm_cosq_ingress_port_drop_threshold_set(unit, flexe_port_1, flags, priority_list[i], max_threshold_port_1+1);
        if (rv == BCM_E_NONE) {
            printf("**** %s: Error(%d), no error for outrange threshold %d(pri %d) ****\n", proc_name, rv, max_threshold_port_1+1, i);
            return BCM_E_FAIL;
        }
    }

    /* Enable hard stage mapping and check */
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_0, flags, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_0, flags, &val_get);
    if (rv != BCM_E_NONE || val_get != 1) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 1) ****\n", proc_name, rv, val_get);
        return BCM_E_FAIL;
    }

    rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_1, flags, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_1, flags, &val_get);
    if (rv != BCM_E_NONE || val_get != 1) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 1) ****\n", proc_name, rv, val_get);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Soft stage mapping
 */

    /* Set configurable ether_type to be recognized by PRD parser and check */
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_set (unit, flexe_port_0, flags, 2, 0x1234);
    rv |= bcm_cosq_ingress_port_drop_custom_ether_type_set (unit, flexe_port_1, flags, 6, 0xFFFF);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_custom_ether_type_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_custom_ether_type_get(unit, flexe_port_0, flags, 2, &val32_get_0);
    rv |= bcm_cosq_ingress_port_drop_custom_ether_type_get(unit, flexe_port_1, flags, 6, &val32_get_1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_custom_ether_type_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_set(unit, key_id, flags, flex_key_config);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_construct_set ****\n", proc_name, rv);
        return rv;
    }

    /* Get PRD soft stage (TCAM) key */
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_construct_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_set(unit, key_id, flags, flex_key_config);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_construct_set ****\n", proc_name, rv);
        return rv;
    }

    /* Get PRD soft stage (TCAM) key and check again */
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_construct_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_construct_get(unit, &key_id, flags, &flex_key_cfg_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_construct_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_0, flex_key_info_0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_set ****\n", proc_name, rv);
        return rv;
    }

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
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_1, flex_key_info_1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_set ****\n", proc_name, rv);
        return rv;
    }

    /* Get and check 1st TCAM entry */
    bcm_cosq_ingress_drop_flex_key_entry_t flex_key_info_get;
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_0, &flex_key_info_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_1, &flex_key_info_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_0, flex_key_info_1);
    rv |= bcm_cosq_ingress_port_drop_flex_key_entry_set(unit, flexe_port_0, flags, key_index_1, flex_key_info_0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_set ****\n", proc_name, rv);
        return rv;
    }

    /* Check the 2 entries again */
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_0, &flex_key_info_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_flex_key_entry_get(unit, flexe_port_0, flags, key_index_1, &flex_key_info_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_flex_key_entry_get ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_0, flags, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_0, flags, &val_get);
    if (rv != BCM_E_NONE || val_get != 2) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 2) ****\n", proc_name, rv, val_get);
        return BCM_E_FAIL;
    }

    rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_1, flags, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_1, flags, &val_get);
    if (rv != BCM_E_NONE || val_get != 2) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 2) ****\n", proc_name, rv, val_get);
        return BCM_E_FAIL;
    }

    /***************************************************************************
     * Rx buffer setting
 */

    /* Initialize priority_config */
    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0 | BCM_PORT_F_PRIORITY_1;
    priority_config.priority_groups[0].num_of_entries = 0;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2 | BCM_PORT_F_PRIORITY_3;
    priority_config.priority_groups[1].num_of_entries = 0;

    /* Verify nof_priority_groups should be no greater than 2 */
    priority_config.nof_priority_groups = 3;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(0) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.nof_priority_groups = 2;

    /* Verify nof_priority_groups should be no less than 1 */
    priority_config.nof_priority_groups = 0;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(0) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.nof_priority_groups = 2;

    /* Verify TDM is only for L1 ports */
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(1) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;

    /* Verify with invalid nif_acheduler type */
    priority_config.priority_groups[0].sch_priority = 7;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(2) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;

    /* Verify num_of_entries should be 0 */
    priority_config.priority_groups[0].num_of_entries = 1;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(3) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].num_of_entries = 0;

    /* Verify source_priority can't be NULL */
    priority_config.priority_groups[1].source_priority = 0;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(4) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2 | BCM_PORT_F_PRIORITY_3;

    /* Verify with invliad source_priority */
    priority_config.priority_groups[0].source_priority = 60;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(5) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0 | BCM_PORT_F_PRIORITY_1;

    /* 2 same scheduler priorities */
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerLow;
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error(%d), no error returned for bcm_port_priority_config_set(6) ****\n", proc_name, rv);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerHigh;

    /* Configure the 1st client with 2 RMCs */
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_priority_config_set(7) ****\n", proc_name, rv);
        return rv;
    }
    /* Get RMC settings */
    rv = bcm_port_priority_config_get(unit, flexe_port_0, &priority_config_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_priority_config_get(0) ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_port_priority_config_set(unit, flexe_port_0, &priority_config);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_priority_config_set(8) ****\n", proc_name, rv);
        return rv;
    }
    /* Get RMC setting */
    rv = bcm_port_priority_config_get(unit, flexe_port_0, &priority_config_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_priority_config_get(1) ****\n", proc_name, rv);
        return rv;
    }
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
    rv = bcm_port_priority_config_get(unit, flexe_port_1, &priority_config_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_priority_config_get(2) ****\n", proc_name, rv);
        return rv;
    }
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
                priority_config.priority_groups[0].num_of_entries = 0;
                rv  = bcm_port_priority_config_set(unit, flexe_ports[port_index], &priority_config);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail in bcm_port_priority_config_set(port_index %d, i %d, ii %d) ****\n", proc_name, rv, port_index, i, ii);
                    return rv;
                }

                /* Get Rx buffer and check */
                rv = bcm_port_priority_config_get(unit, flexe_ports[port_index], &priority_config_get);
                if (rv != BCM_E_NONE) {
                    printf("**** %s: Error(%d), fail in bcm_port_priority_config_get(port_index %d, i %d, ii %d) ****\n", proc_name, rv, port_index, i, ii);
                    return rv;
                }
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
 */

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
        rv = bcm_cosq_ingress_port_drop_control_frame_set(unit, flexe_port_0, flags, control_frame_index, &control_frame_config);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_control_frame_set ****\n", proc_name, rv);
            return rv;
        }

        /* Get control frame and check */
        bcm_cosq_ingress_drop_control_frame_config_t control_frame_cfg_get;
        rv = bcm_cosq_ingress_port_drop_control_frame_get(unit, flexe_port_0, flags, control_frame_index, &control_frame_cfg_get);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_control_frame_get ****\n", proc_name, rv);
            return rv;
        }
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

    /* Get PRD drop counter */
    rv = bcm_stat_get(unit, flexe_port_0, snmpEtherStatsDropEvents, &val_64b_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to get PRD drop counter ****\n", proc_name, rv);
        return rv;
    }

    /* Set and get default priority */
    rv = bcm_cosq_ingress_port_drop_default_priority_set (unit, flexe_port_0, flags, 2);
    rv |= bcm_cosq_ingress_port_drop_default_priority_get (unit, flexe_port_0, flags, &val32_get_0);
    if (rv != BCM_E_NONE || val32_get_0 != 2) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_default_priority_set/get ****\n", proc_name, rv);
        return rv;
    }

    /* Set&get special label mapping */
    int label_index = sal_rand() % 4;
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t label_cfg_set, label_cfg_get;
    label_cfg_set.label_value = sal_rand() % 16;
    label_cfg_set.priority = sal_rand() % 4;
    label_cfg_set.is_tdm = 0;
    rv = bcm_cosq_ingress_port_drop_mpls_special_label_set(unit, flexe_port_0, flags, label_index, label_cfg_set);
    rv |= bcm_cosq_ingress_port_drop_mpls_special_label_get(unit, flexe_port_0, flags, label_index, &label_cfg_get);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_mpls_special_label_set/get ****\n", proc_name, rv);
        return rv;
    }
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
        rv = bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, i);
        rv |= bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, i);
        rv |= bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, i);
        rv |= bcm_cosq_control_set (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgonreOuterTag, i);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set ignore fields to %d ****\n", proc_name, rv, i);
            return rv;
        }

        /* Get and check */
        rv = bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, &val_get);
        rv |= bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, &val_get_1);
        rv |= bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, &val_get_2);
        rv |= bcm_cosq_control_get (unit, flexe_port_0, -1, bcmCosqControlIngressPortDropIgonreOuterTag, &val_get_3);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to get ignore fields ****\n", proc_name, rv);
            return rv;
        }
        if (val_get != i || val_get_1 != i || val_get_2 != i || val_get_3 != i) {
            printf("**** %s: The values of ignore fields aren't %d. They are %d, %d, %d and %d ****\n", proc_name, i, val_get, val_get_1, val_get_2, val_get_3);
            return BCM_E_FAIL;
        }
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, flexe_port_0, 1);
    rv |= bcm_port_enable_set(unit, flexe_port_1, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(enable) ****\n", proc_name, rv);
        return rv;
    }

    /* Disable PRD */
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_0, flags, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_0, flags, &val_get);
    if (rv != BCM_E_NONE || val_get != 0) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 0) ****\n", proc_name, rv, val_get);
        return BCM_E_FAIL;
    }

    rv = bcm_cosq_ingress_port_drop_enable_set(unit, flexe_port_1, flags, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_set ****\n", proc_name, rv);
        return rv;
    }
    rv = bcm_cosq_ingress_port_drop_enable_get(unit, flexe_port_1, flags, &val_get);
    if (rv != BCM_E_NONE || val_get != 0) {
        printf("**** %s: Error(%d), fail in bcm_cosq_ingress_port_drop_enable_get(val_get %d, exp 0) ****\n", proc_name, rv, val_get);
        return BCM_E_FAIL;
    }

    print "~~~~~~~~~~~~~~~~~~~~ End of semantic test for FlexE_MAC_L2 ~~~~~~~~~~~~~~~~~~~~";

    return rv;
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
 */

    if (flexe_port_type == 1) {
        /* Create BusB L1 client */
        rv = dnx_flexe_util_client_create(unit, flexe_port, port_type_busB_L1, 10000, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create() ****\n", proc_name, rv);
            return rv;
        }
    } else if (flexe_port_type == 3) {
        /* Create BusA client */
        rv = dnx_flexe_util_client_create(unit, flexe_port, port_type_busA, 10000, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create() ****\n", proc_name, rv);
            return rv;
        }
    } else if (flexe_port_type == 4) {
        /* Create BusC client */
        rv = dnx_flexe_util_client_create(unit, flexe_port, port_type_busC, 10000, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_client_create() ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, flexe_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(disable) ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * PRD Profile setting on FlexE ports
 */

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
 */

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
 */

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
 */

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
 */

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
    rv  = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_port_priority_config_set(0) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;

    /* Verify source_priority must be IMB_PRD_PRIORITY_ALL */
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0;
    rv  = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_port_priority_config_set(1) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.priority_groups[0].source_priority = port_pri_all;

    /* Verify nof_priority_groups should be no greater than 1 */
    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[1].source_priority = port_pri_all;
    priority_config.priority_groups[1].num_of_entries = entry_num;
    rv  = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_port_priority_config_set(2) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    priority_config.nof_priority_groups = 1;

    /* Set RMC with correct parameters */
    rv  = bcm_port_priority_config_set(unit, flexe_port, &priority_config);
    if (flexe_port_type < 2) {
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_priority_config_set ****\n", proc_name, rv);
            return rv;
        }
    } else {
        if (rv == BCM_E_NONE) {
            printf("**** %s: Error, no error for bcm_port_priority_config_set(port type %d) ****\n", proc_name, flexe_port_type);
            return BCM_E_FAIL;
        }
    }

    /* Get RMC setting */
    rv = bcm_port_priority_config_get(unit, flexe_port, &priority_config_get);
    if (flexe_port_type == 0 || flexe_port_type == 1) {
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_priority_config_get ****\n", proc_name, rv);
            return BCM_E_FAIL;
        }
    } else {
        if (rv == BCM_E_NONE) {
            printf("**** %s: Error, no error for bcm_port_priority_config_get(port type %d) ****\n", proc_name, flexe_port_type);
            return BCM_E_FAIL;
        }
    }

    /***************************************************************************
     * Misc settings
 */

    /* Set control frame */
    int control_frame_index = 1;
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_config;
    COMPILER_64_SET(control_frame_config.mac_da_val,0x1122,0x33445566);
    COMPILER_64_SET(control_frame_config.mac_da_mask,0xffff,0xffffffff);
    control_frame_config.ether_type_code= 2;
    control_frame_config.ether_type_code_mask= 0xF;
    rv = bcm_cosq_ingress_port_drop_control_frame_set(unit, flexe_port, flags, control_frame_index, &control_frame_config);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_cosq_ingress_port_drop_control_frame_set ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get control frame and check */
    bcm_cosq_ingress_drop_control_frame_config_t control_frame_cfg_get;
    rv = bcm_cosq_ingress_port_drop_control_frame_get(unit, flexe_port, flags, control_frame_index, &control_frame_cfg_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_cosq_ingress_port_drop_control_frame_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Set and get default priority */
    rv = bcm_cosq_ingress_port_drop_default_priority_set (unit, flexe_port, flags, 2);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_cosq_ingress_port_drop_default_priority_set ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_ingress_port_drop_default_priority_get (unit, flexe_port, flags, &val32_get_0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_cosq_ingress_port_drop_default_priority_get ****\n", proc_name);
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
        printf("**** %s: Error, no error for bcm_cosq_ingress_port_drop_mpls_special_label_set ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_ingress_port_drop_mpls_special_label_get(unit, flexe_port, flags, label_index, &label_cfg_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for bcm_cosq_ingress_port_drop_mpls_special_label_get ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Configure ignore fields */
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreIpDscp(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreMplsExp(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreInnerTag(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_set (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgonreOuterTag, 0);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreOuterTag(set) ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Get ignore fields */
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreIpDscp, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreIpDscp(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreMplsExp, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreMplsExp(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgnoreInnerTag, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreInnerTag(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }
    rv = bcm_cosq_control_get (unit, flexe_port, -1, bcmCosqControlIngressPortDropIgonreOuterTag, &val_get);
    if (rv == BCM_E_NONE) {
        printf("**** %s: Error, no error for control field IngressPortDropIgnoreOuterTag(get) ****\n", proc_name);
        return BCM_E_FAIL;
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, flexe_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(enable) ****\n", proc_name, rv);
        return rv;
    }

    printf("~~~~~~~~~~~~~~~~~~~~ End of semantic test(type %d) ~~~~~~~~~~~~~~~~~~~~\n", flexe_port_type);

    return rv;
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
    int i, rv;
    int flexe_client_speed = 400000;
    int total_slots = flexe_client_speed / 5000;
    bcm_gport_t flexe_grp;
    bcm_port_t client_a = 60;
    bcm_port_t client_b = 61;
    flexe_port_type_t client_b_type = flexe_is_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;
    bcm_port_t eth_l2_src, eth_l2_dst;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_esb_bw_test_main";

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
 */

    /* Create FlexE group with the provided FlexE PHY */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 1);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create ****\n", proc_name, rv);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, flexe_client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a ****\n", proc_name, rv);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b, client_b_type, flexe_client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b ****\n", proc_name, rv);
        return rv;
    }

    if (flexe_is_L1) {
        /* Create L1 datapath: eth_flexe <- L1_switch -> client_b <--> client_a */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_flexe, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L1 datapath ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_flexe, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Create L2 datapath: eth_flexe <-force_fwd-> client_b <--> client_a */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_flexe, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Eth2FlexE L2 datapath ****\n", proc_name, rv);
            return rv;
        }
        rv = dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_flexe, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set FlexE2Eth L2 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Assign slots for the FlexE client */
    for (i = 0; i < total_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set ****\n", proc_name, rv);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID ****\n", proc_name, rv);
        return rv;
    }

    /* Loopback the FlexE PHY */
    rv = bcm_port_loopback_set(unit, flexe_phy, 2);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set ****\n", proc_name, rv);
        return rv;
    }

    /* Create L2 snake with the L2 ETH ports */
    for (i = 0; i < nof_eths_in_none_flexe_snake; i++) {
        eth_l2_src = eth_l2_start + i;
        eth_l2_dst = i == nof_eths_in_none_flexe_snake-1 ? eth_l2_start : eth_l2_src+1;

        /* Loopback the L2 ports */
        if (i) {
            rv = bcm_port_loopback_set(unit, eth_l2_src, 1);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail in bcm_port_loopback_set(port %d) ****\n", proc_name, rv, eth_l2_src);
                return rv;
            }
            printf("**** bcm_port_loopback_set(port %d) ****\n", eth_l2_src);
        }

        /* Force forwarding */
        rv = bcm_port_force_forward_set(unit, eth_l2_src, eth_l2_dst, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_force_forward_set(src_port %d) ****\n", proc_name, rv, eth_l2_src);
            return rv;
        }
        printf("**** bcm_port_force_forward_set(src_port %d, dst_port %d) ****\n", eth_l2_src, eth_l2_dst);
    }

    return rv;
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
    int i, ii, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_bypass_mode_test_main";

    /***************************************************************************
     * FlexE PHY port(s) and group creation
 */

    /* Create FlexE group with above FlexE PHY port on each Q2A */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    flexe_phy_ports[0] = phy_port;
    flexe_bypass_en = grp_bypass; /* Enable bypass mode */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_group_create(unit[i], flexe_grp, nof_pcs);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /***************************************************************************
     * FlexE client(s) creation
 */

    /* Create a busA client and a busB client on each device */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_client_create(unit[i], client_a, port_type_busA, client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
        rv = dnx_flexe_util_client_create(unit[i], client_b, client_b_type[i], client_speed, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* Create busC client on distributed Q2A */
    rv = dnx_flexe_util_client_create(unit_dis, client_c, port_type_busC, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busC client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

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
 */

    if (is_L1_switch_cen)
    {
        /* Build Eth2FlexE L1 datapath on centralized Device */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Build Eth2FlexE L2 datapath on centralized Device */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L2 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Build Flexe2ILKN and ILKN2Flexe L1/L2 datapath on distributed Device */
    if (is_L1_switch_dis)
    {
        /* L1 datapath:
         * client_a <-> client_c <-> ilkn_port(loopback)
         */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a, client_c, ilkn_port, 0);
        rv |= dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_c, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* L2 datapath:
         * client_a -> client_b -> ilkn_port -> loopback ->
         * -> ilkn_port -> force_fwd -> client_b -> client_a
         */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a, client_b, ilkn_port, 0);
        rv |= dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_b, client_a, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set Flexe2ILKN datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Calendar and OH settings
 */

    /* Iterate each Q2A with below configurations */
    for (i = 0; i < 2; i++) {
        /* Assign slots for the 2 FlexE clients on calendar A */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_slots; ii++) {
            Cal_Slots_A[ii] = client_a;
        }
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }

        /* Set group ID in overhead */
        if (!grp_bypass) {
            rv = bcm_port_flexe_oh_set(unit[i], flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index);
            if (rv != BCM_E_NONE) {
                printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit[i]);
                return rv;
            }
        }
    }

    /* Loopback on ILKN port */
    rv = bcm_port_loopback_set(unit_dis, ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), failed to set loopback on ilkn port %d ****\n", proc_name, rv, ilkn_port);
        return rv;
    }

    return rv;
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
    int i, ii, rv;
    int nof_pcs = 1;
    int unit[2] = {unit_cen, unit_dis};
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_port_t client_c = 60 + grp_index;
    bcm_gport_t flexe_grp;
    int nof_slots = grp_speed / 5000;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_bypass_mode_test_destroy";

    /***************************************************************************
     * Clear FlexE calendar
 */

    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    for (i = 0; i < 2; i++) {
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        rv = dnx_flexe_util_calendar_slots_set(unit[i], flexe_grp, 0, FLEXE_CAL_A, nof_slots);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
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
 */

    /* Clear L1/L2 datapath on centralized Device */
    if (is_L1_switch_cen)
    {
        /* Clear L1 datapath */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 1);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear L1 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear L2 datapath */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit_cen, eth_port, client_b, client_a, 1);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit_cen, client_a, client_b, eth_port, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear L2 datapath on Q2A_#0 ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Clear L1/L2 datapath on distributed Device */
    if (is_L1_switch_dis)
    {
        /* Clear L1 datapath */
        rv = dnx_flexe_util_L1_datapath_flexe_to_ilkn(unit_dis, client_a, client_c, ilkn_port, 1);
        rv |= dnx_flexe_util_L1_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_c, client_a, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear L1 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear L2 datapath */
        rv = dnx_flexe_util_distributed_L2_L3_datapath_flexe_to_ilkn(unit_dis, client_a, client_b, ilkn_port, 1);
        rv |= dnx_flexe_util_distributed_L2_L3_datapath_ilkn_to_flexe(unit_dis, ilkn_port, client_b, client_a, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear L2 datapath on Q2A_#1 ****\n", proc_name, rv);
            return rv;
        }
    }

    /***************************************************************************
     * Clear FlexE clients
 */

    /* Remove busA client and busB client on each device */
    for (i = 0; i < 2; i++) {
        rv = dnx_flexe_util_client_remove(unit[i], client_a);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove busA client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
        rv = dnx_flexe_util_client_remove(unit[i], client_b);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to remove busB client(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    /* remove busC client on distributed Q2A */
    rv = dnx_flexe_util_client_remove(unit_dis, client_c);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove busC client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /***************************************************************************
     * Clear FlexE groups
 */

    for (i = 0; i < 2; i++) {
        rv = bcm_port_flexe_group_destroy(unit[i], flexe_grp);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_flexe_group_destroy(unit %d) ****\n", proc_name, rv, unit[i]);
            return rv;
        }
    }

    return rv;
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
    int i, rv;
    int nof_pcs = 1;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_gport_t flexe_grp;
    int total_slots = grp_speed / 5000;
    int client_slots = client_speed / 5000;
    flexe_port_type_t client_b_type = is_flexe_L1 ? port_type_busB_L1 : port_type_busB_L2_L3;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_basic_cfg_main";

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Create busA client and busB client */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busA client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit, client_b, client_b_type, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create busB client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE datapath:
     *                           FlexE Core
     *                   +------------------------+
     *                   |                        |
     * eth_port <-L1/L2->| client_b <--> client_a | <--> flexe_phy[loopback(optional)]
     *                   |                        |
     *                   +------------------------+
 */

    if (is_flexe_L1) {
        /* Build L1 datapath between eth_port and FlexE */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L1 datapath between eth_port and FlexE ****\n", proc_name, rv);
            return rv;
        }
    } else {
        /* Build L2 datapath between eth_port and FlexE */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 0);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 0);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set L2 datapath between eth_port and FlexE ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Assign calendar slots for BusA client */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < client_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Set group ID in overhead */
    if (!flexe_bypass_en) {
        rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, grp_index);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit);
            return rv;
        }
    }

    /* Loopback FlexE PHY */
    if (flexe_phy_loopback) {
        rv = bcm_port_loopback_set(unit, flexe_phy, BCM_PORT_LOOPBACK_PHY);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail in bcm_port_loopback_set(flexe_phy) ****\n", proc_name, rv);
            return rv;
        }
    }

    return rv;
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
    int i, rv;
    bcm_port_t client_a = 40 + grp_index;
    bcm_port_t client_b = 50 + grp_index;
    bcm_gport_t flexe_grp;
    int total_slots = grp_speed / 5000;

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_basic_cfg_destroy";

    /* Clear FlexE calendar */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, total_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /***************************************************************************
     * FlexE datapath:
     *                           FlexE Core
     *                   +------------------------+
     *                   |                        |
     * eth_port <-L1/L2->| client_b <--> client_a | <--> flexe_phy[loopback(optional)]
     *                   |                        |
     *                   +------------------------+
 */

    if (is_flexe_L1)
    {
        /* Clear L1 datapath */
        rv = dnx_flexe_util_L1_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 1);
        rv |= dnx_flexe_util_L1_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE L1 datapath ****\n", proc_name, rv);
            return rv;
        }
    }
    else
    {
        /* Clear L2 datapath */
        rv = dnx_flexe_util_L2_datapath_eth_to_flexe(unit, eth_port, client_b, client_a, 1);
        rv |= dnx_flexe_util_L2_datapath_flexe_to_eth(unit, client_a, client_b, eth_port, 1);
        if (rv != BCM_E_NONE) {
            printf("**** %s: Error(%d), fail to clear FlexE L2 datapath ****\n", proc_name, rv);
            return rv;
        }
    }

    /* Remove busA client and busB client */
    rv = dnx_flexe_util_client_remove(unit, client_a);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove busA client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }
    rv = dnx_flexe_util_client_remove(unit, client_b);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to remove busB client(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Clear FlexE group */
    rv = bcm_port_flexe_group_destroy(unit, flexe_grp);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_group_destroy(unit %d) ****\n", proc_name, rv, unit);
        return rv;
    }

    /* Remove loopback on FlexE PHY */
    rv = bcm_port_loopback_set(unit, flexe_phy, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_loopback_set(flexe_phy) ****\n", proc_name, rv);
        return rv;
    }

    return rv;
}

/* Funtion L2 datapath between FlexE and COP under distributed mode */
int dnx_flexe_distributed_datapath_flexe_and_cop_main(
    int unit_cen,
    int unit_dis,
    bcm_port_t eth_tx_rx,
    bcm_port_t ilkn_port,
    bcm_port_t flexe_phy)
{
    int i, rv;
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

    /* Set proc_name */
    char *proc_name;
    proc_name = "dnx_flexe_distributed_datapath_flexe_and_cop_main";

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
 */

    /***************************************************************************
     * FlexE setting on centralized device
 */
    rv = dnx_flexe_basic_cfg_main(unit_cen, flexe_grp_speed, grp_index, flexe_grp_speed, eth_tx_rx, flexe_phy, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_basic_cfg_main on centralized device ****\n", proc_name, rv);
        return rv;
    }

    /***************************************************************************
     * FlexE setting on distributed device
 */

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, grp_index);
    flexe_phy_ports[0] = flexe_phy;
    rv = dnx_flexe_util_group_create(unit_dis, flexe_grp, nof_pcs);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_group_create(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Create FlexE clients */
    rv = dnx_flexe_util_client_create(unit_dis, client_a, port_type_busA, flexe_grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_a client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b, port_type_busB_L2_L3, flexe_grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = dnx_flexe_util_client_create(unit_dis, client_b_1, port_type_busB_L2_L3, flexe_grp_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to create client_b_1 client(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /*
     * FlexE flow setting:
     * 1. client_a --> client_b
     * 2. client_b --> client_a
     * 3. client_b_1(tx) --> client_b_1(rx)
     * 4. client_b_1 --> ilkn_port
     */
    rv = bcm_port_enable_set(unit_dis, client_b, 0);
    rv |= bcm_port_enable_set(unit_dis, client_b_1, 0);
    rv |= bcm_port_enable_set(unit_dis, ilkn_port, 0);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(unit %d, disable) ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = bcm_port_flexe_flow_set(unit_dis, flags, client_a, 0, client_b);
    rv |= bcm_port_flexe_flow_set(unit_dis, flags, client_b, 0, client_a);
    rv |= bcm_port_flexe_flow_set(unit_dis, flags, client_b_1, 0, client_b_1);
    rv |= bcm_port_flexe_flow_set(unit_dis, flags, client_b_1, 0, ilkn_port);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_flexe_flow_set(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }
    rv = bcm_port_enable_set(unit_dis, client_b, 1);
    rv |= bcm_port_enable_set(unit_dis, client_b_1, 1);
    rv |= bcm_port_enable_set(unit_dis, ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in bcm_port_enable_set(unit %d, enable) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /*
     * L2 setting:
     * 00:00:00:00:11:11+vid_in-->client_b_1 (for client_b->client_b_1 L2 lookup)
     * 00:00:00:00:11:11+vid_out-->client_b  (for ilkn_port->client_b L2 lookup)
     */
    rv = dnx_flexe_util_L2_entry_oper(unit_dis, client_b_1, vid_in, 0, 1);
    rv |= dnx_flexe_util_L2_entry_oper(unit_dis, client_b, vid_out, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_L2_entry_oper(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Enable egress VLAN translation at client_b_1 */
    rv = egr_vlan_edit(unit_dis, client_b, client_b_1, vid_in, vid_out);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in egr_vlan_edit(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* loopback ilkn_port and add it to vid_out */
    rv = bcm_vlan_gport_add(unit_dis, vid_out, ilkn_port, 0);
    rv |= bcm_port_loopback_set(unit_dis, ilkn_port, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in loopback and vlan member adding(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* Calendar setting */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (i = 0; i < nof_slots; i++) {
        Cal_Slots_A[i] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit_dis, flexe_grp, 0, FLEXE_CAL_A, nof_slots);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail in dnx_flexe_util_calendar_slots_set(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    /* FlexE OH setting */
    rv = bcm_port_flexe_oh_set(unit_dis, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("**** %s: Error(%d), fail to set OhGroupID(unit %d) ****\n", proc_name, rv, unit_dis);
        return rv;
    }

    return rv;
}

