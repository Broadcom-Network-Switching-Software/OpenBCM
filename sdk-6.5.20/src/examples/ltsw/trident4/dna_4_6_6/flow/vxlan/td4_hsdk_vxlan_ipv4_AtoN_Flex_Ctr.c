/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880) Vxlan Bridging, IPv4 access to network tunnel initiation Flow, add sd tag (100) 
 *            in payload while egressing on network port. Also, attaching flex counters for VPN and Flow port
 *
 * Usage    : BCM.0> cint TD4_HSDK_VxLAN_ipv4_AtoN_Flex_Ctr.c
 *
 * config   : bcm56880_a0-generic-32x400_vxlan.config.yml
 *
 * Log file : TD4_HSDK_VxLAN_ipv4_AtoN_Flex_Ctr_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   VLAN-22        Underlay-NextHop
 *                                  |                      |  egress_port       +---+
 *   SMAC 00::bb:bb                 |                      +--------------------+   |
 *            +---+          VLAN-21|                      |00::22:22           +---+
 *   Host(acc)|   +-----------------+      Trident-4       |                    00::02
 *            +---+    ingress_port |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN Bridging configuration for tunnel Initiation -
 *             - access to network (encap flow)
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan_ipv4_vtep()):
 *   =========================================================
 *     a) Configure a VxLAN Bridging configuration with SDTAG operations.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show',traversing the vpn and vp's
 *
 *     b) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the tunnel packet on the egress port with the IPv4 encapsulation
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 *       Also, the flex counter values for VPN and Flow port are printed on screen
 */

/*
Packet:

ACCESS-TO-NETWORK:-
Encap-case: packet sent from ingress_port(access) to egress_port(network)

Ingress Packet:
======

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00:00:aa:aa
    src= 00:00:00:00:bb:bb
    type= 0x8100
  ###[ 802.1Q ]###
    vlan= 21
    type= 0x800
  ###[ IP ]###
    version= 4
    proto= udp
    src= 1.1.1.1
    dst= 2.2.2.2
  ###[ UDP ]###

Ingress Hex Packet:
tx 1 pbm=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174BA010101010202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C

Egress Packet :
======

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00:00:00:02
    src= 00:00:00:00:22:22
    type= 0x8100
  ###[ 802.1Q ]###
    vlan= 22
    type= 0x800
  ###[ IP ]###
    version= 4
    proto= udp
    src= 10.10.10.1
    dst= 192.168.1.1
  ###[ UDP ]###
    sport= 49152
    dport= 4789
  ###[ VXLAN ]###
    flags= Instance
    reserved1= 0
    vni= 0x12345
    reserved2= 0x0
  ###[ Ethernet ]###
    dst= 00:00:00:00:aa:aa
    src= 00:00:00:00:bb:bb
    type= 0x800
  ###[ 802.1Q ]###
    outer_vlan= 100
    type= 0x800
    inner_vlan= 21
    type= 0x800
  ###[ IP ]###
    version= 4
    proto= udp
    src= 1.1.1.1
    dst= 2.2.2.2
  ###[ UDP ]###

Packet Dump
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 006c 0000 0000 3f11 a5cd 0a0a 
data[0020]: 0a01 c0a8 0101 c000 12b5 0058 0000 0800 
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000 
data[0040]: bbbb 8100 0064 8100 0015 0800 4500 002e 
data[0050]: 0000 0000 4011 74ba 0101 0101 0202 0202 
data[0060]: 0035 0036 001a b0f8 0001 0203 0405 0607 
data[0070]: 0809 0a0b 0c0d 0e0f 1011 33cf 192c 

*/

/* Reset C interpreter*/
cint_reset();

bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_gport_t ingress_phy_gport;
bcm_gport_t egress_phy_gport;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port*/
    bcm_vlan_t    acc_vlan; /* access vlan*/
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    bcm_gport_t   acc_gport; /* Output:Access VP gport */
    /* sd_tag_preserve
       0: Not Preserve 
       1: Preserve 
    */
    int           sd_tag_preserve; 
    uint32        ing_flow_stat_id; /* Ingress Flow Flex Counter ID */    
    uint32        egr_flow_stat_id; /* Egress Flow Flex Counter ID */        
};

/* struct for Network Port params */
struct vxlan_net_cfg_t {
    bcm_port_t    phy_Gport;/* Input : gport of the port*/
    bcm_vlan_t    net_vlan;
    bcm_mac_t     net_local_mac;
    bcm_mac_t     net_remote_mac;
    bcm_ip_t      remote_ip;
    uint32        vxlan_vnid;
    bcm_ip_t      local_ip;
    uint16        udp_sp;
    bcm_gport_t   nw_gport; /* Output:Nw VP gport */
    /* sd_tag_operation
       0: NOOP 
       1: Add 
       2: Delete 
    */ 
    int           sd_tag_operation;   
    bcm_vlan_t    sd_tag_vid;  
    uint32        ing_flow_stat_id; /* Ingress Flow Flex Counter ID */    
    uint32        egr_flow_stat_id; /* Egress Flow Flex Counter ID */        
};

struct vxlan_cfg_t {
    bcm_vpn_t       vxlan_vpn;
    uint32          vxlan_vnid; /*use vnid for vp sharing*/
    bcm_multicast_t mc_group;
    uint32          ing_vpn_stat_id; /* Ingress VPN Flex Counter ID */
    uint32          egr_vpn_stat_id; /* Egress VPN Flex Counter ID */    
};

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

    if (BCM_E_NONE != bcm_port_gport_get(unit, ingress_port, &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, egress_port, &(egress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port);
        return -1;
    }

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
 * Verification:
 *   In Packet:
 *   ----------
 *   00000000AAAA00000000BBBB
 *   8100001508004500002E0000
 *   0000401174BA010101010202
 *   020200350036001AB0F80001
 *   02030405060708090A0B0C0D
 *   0E0F101133CF192C
 *
 *   Out Packet:
 *   -----------
 *   {000000000002} {000000002222} 8100 0016
 *   0800 4500 006c 0000 0000 3f11 a5cd 0a0a 
 *   0a01 c0a8 0101 c000 12b5 0058 0000 0800 
 *   0000 0123 4500 0000 0000 aaaa 0000 0000 
 *   bbbb 8100 0064 8100 0015 0800 4500 002e 
 *   0000 0000 4011 74ba 0101 0101 0202 0202 
 *   0035 0036 001a b0f8 0001 0203 0405 0607 
 *   0809 0a0b 0c0d 0e0f 1011 33cf 192c 
 */
void verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "clear c");
    printf("\nSending known unicast packet to ingress_port access side:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174BA010101010202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C; sleep quiet 1", ingress_port);
    printf("%s", str);
    bshell(unit, str);
    bshell(unit, "show c");

    /* dump l2 table to see if SA learned againt SVP */
    bshell(unit, "l2 show");

    /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);
    flexctr_vpn_stat_get(unit, app_vxlan_vpn.vpn, app_vxlan_vpn.ing_vpn_stat_id, app_vxlan_vpn.egr_vpn_stat_id);
    flexctr_flow_stat_get(unit, app_vxlan_acc_port.flow_gport, app_vxlan_acc_port.ing_flow_stat_id, "Ingress Flow");
    flexctr_flow_stat_get(unit, app_vxlan_net_port.flow_gport, app_vxlan_net_port.egr_flow_stat_id, "Egress Flow");    

    return;
}

/*
 * Flex counter functions 
 */
void
vxlan_flow_counter_attach(int unit, bcm_gport_t flow_port_id, uint32 *ing_flow_stat_id, uint32 *egr_flow_stat_id)
{
    /* Configuration to Attach Flex Counters */
    bcm_flow_stat_info_t flow_stat_info;
    bcm_flexctr_action_t action;
    uint32_t             stat_counter_id;
    uint32               num_entries = 1;
    uint32               counter_index = 0;
    int                  options;

    /* Configure Flex Counter to count on ING SVP Table using bcmFlexctrSourceSvp */
    sal_memset(&action, 0, sizeof(action));
    action.source = bcmFlexctrSourceSvp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_operation.object[0] = bcmFlexctrObjectConstZero;
    action.index_operation.mask_size[0] = 16;
    action.index_operation.mask_size[1] = 1;

    action.operation_a.select = bcmFlexctrValueSelectCounterValue;
    action.operation_a.object[0] = bcmFlexctrObjectConstOne;
    action.operation_a.type = bcmFlexctrValueOperationTypeInc;
    action.operation_a.mask_size[0] = 16;
    action.operation_a.mask_size[1] = 1;

    action.operation_b.select = bcmFlexctrValueSelectPacketLength;
    action.operation_b.type = bcmFlexctrValueOperationTypeInc;
    action.operation_b.mask_size[0] = 16;
    action.operation_b.mask_size[1] = 1;
    action.index_num = 1;

    options = 0;
    /* Create Flex Counter Action based on the above Criteria */
    BCM_IF_ERROR_RETURN(bcm_flexctr_action_create(unit, options, &action, &stat_counter_id));
    printf("Ingress Flow Flex Counter Stat ID : 0x%x\n",stat_counter_id);

    /* Attach created Flex Counter to  net_flow_port for counting per ING SVP*/
    bcm_flow_stat_info_t_init(&flow_stat_info);
    flow_stat_info.valid_elements = BCM_FLOW_STAT_FLOW_PORT_VALID;
    flow_stat_info.flow_port = flow_port_id;
    BCM_IF_ERROR_RETURN(bcm_flow_stat_attach(unit, &flow_stat_info, 0, NULL, stat_counter_id));

    *ing_flow_stat_id = stat_counter_id;

    /* per DVP Counter */
    sal_memset(&action, 0, sizeof(action));
    action.source = bcmFlexctrSourceDvp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_operation.object[0] = bcmFlexctrObjectConstZero;
    action.index_operation.mask_size[0] = 16;
    action.index_operation.mask_size[1] = 1;

    action.operation_a.select = bcmFlexctrValueSelectCounterValue;
    action.operation_a.object[0] = bcmFlexctrObjectConstOne;
    action.operation_a.type = bcmFlexctrValueOperationTypeInc;
    action.operation_a.mask_size[0] = 16;
    action.operation_a.mask_size[1] = 1;

    action.operation_b.select = bcmFlexctrValueSelectPacketLength;
    action.operation_b.type = bcmFlexctrValueOperationTypeInc;
    action.operation_b.mask_size[0] = 16;
    action.operation_b.mask_size[1] = 1;
    action.index_num = 1;

    options = 0;
    /* Create Flex Counter Action based on the above Criteria */
    BCM_IF_ERROR_RETURN(bcm_flexctr_action_create(unit, options, &action, &stat_counter_id));
    printf("Egress Flow Flex Counter Stat ID : 0x%x\n",stat_counter_id);

    /* Attach created Flex Counter to  net_flow_port for counting per ING SVP*/
    bcm_flow_stat_info_t_init(&flow_stat_info);
    flow_stat_info.valid_elements = BCM_FLOW_STAT_FLOW_PORT_VALID;
    flow_stat_info.flow_port = flow_port_id;
    BCM_IF_ERROR_RETURN(bcm_flow_stat_attach(unit, &flow_stat_info, 0, NULL, stat_counter_id));

    *egr_flow_stat_id = stat_counter_id;

    return BCM_E_NONE;
}

void
vxlan_vpn_counter_attach(int unit, bcm_vpn_t vpn, uint32 *ing_vpn_stat_id, uint32 *egr_vpn_stat_id)
{
    /* Configuration to Attach Flex Counters */
    bcm_flow_stat_info_t flow_stat_info;
    bcm_flexctr_action_t action;    
    uint32               num_entries = 1;
    uint32               counter_index = 0;
    int                  options;

    /* Configure Flex Counter to count on ING VPN Table using bcmFlexctrSourceIngVpn */
    sal_memset(&action, 0, sizeof(action));
    action.source = bcmFlexctrSourceIngVpn;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_operation.object[0] = bcmFlexctrObjectConstZero;
    action.index_operation.mask_size[0] = 16;
    action.index_operation.mask_size[1] = 1;

    action.operation_a.select = bcmFlexctrValueSelectCounterValue;
    action.operation_a.object[0] = bcmFlexctrObjectConstOne;
    action.operation_a.type = bcmFlexctrValueOperationTypeInc;
    action.operation_a.mask_size[0] = 16;
    action.operation_a.mask_size[1] = 1;

    action.operation_b.select = bcmFlexctrValueSelectPacketLength;
    action.operation_b.type = bcmFlexctrValueOperationTypeInc;
    action.operation_b.mask_size[0] = 16;
    action.operation_b.mask_size[1] = 1;
    action.index_num = 1;

    options = 0;
    /* Create Flex Counter Action based on the above Criteria */
    BCM_IF_ERROR_RETURN(bcm_flexctr_action_create(unit, options, &action, ing_vpn_stat_id));
    printf("Ingress VPN Flex Counter Stat ID : 0x%x\n", *ing_vpn_stat_id);

    /* Attach created Flex Counter to VPN for counting per ING VPN */
    bcm_flow_stat_info_t_init(&flow_stat_info);
    flow_stat_info.valid_elements = BCM_FLOW_STAT_VPN_VALID;
    flow_stat_info.vpn = vpn;
    BCM_IF_ERROR_RETURN(bcm_flow_stat_attach(unit, &flow_stat_info, 0, NULL, *ing_vpn_stat_id));

    /* Configure Flex Counter to count on EGR VPN Table using bcmFlexctrSourceEgrVpn */
    sal_memset(&action, 0, sizeof(action));
    action.source = bcmFlexctrSourceEgrVpn;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_operation.object[0] = bcmFlexctrObjectConstZero;
    action.index_operation.mask_size[0] = 16;
    action.index_operation.mask_size[1] = 1;

    action.operation_a.select = bcmFlexctrValueSelectCounterValue;
    action.operation_a.object[0] = bcmFlexctrObjectConstOne;
    action.operation_a.type = bcmFlexctrValueOperationTypeInc;
    action.operation_a.mask_size[0] = 16;
    action.operation_a.mask_size[1] = 1;

    action.operation_b.select = bcmFlexctrValueSelectPacketLength;
    action.operation_b.type = bcmFlexctrValueOperationTypeInc;
    action.operation_b.mask_size[0] = 16;
    action.operation_b.mask_size[1] = 1;
    action.index_num = 1;

    options = 0;
    /* Create Flex Counter Action based on the above Criteria */
    BCM_IF_ERROR_RETURN(bcm_flexctr_action_create(unit, options, &action, egr_vpn_stat_id));
    printf("Egress VPN Flex Counter Stat ID : 0x%x\n", *egr_vpn_stat_id);

    /* Attach created Flex Counter to VPN for counting per ING VFI */
    bcm_flow_stat_info_t_init(&flow_stat_info);
    flow_stat_info.valid_elements = BCM_FLOW_STAT_VPN_VALID;
    flow_stat_info.vpn = vpn;
    BCM_IF_ERROR_RETURN(bcm_flow_stat_attach(unit, &flow_stat_info, 0, NULL, *egr_vpn_stat_id));
    return BCM_E_NONE;
}

int
flexctr_flow_stat_get (int unit, bcm_gport_t flow_port_id, uint32 flow_counter_id, char *flow_direction) 
{
    bcm_flexctr_counter_value_t counter_value;
    int                            rv;
    uint32 num_entries = 1;
    uint32 counter_indexes = 0;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, flow_counter_id, num_entries,
                              &counter_indexes, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Print counter value. */
    printf("*** Flex Counters collected on %s 0x%x (stat_id=0x%x)  Packets / Bytes  : 0x%08x / 0x%08x \n",
            flow_direction, flow_port_id, flow_counter_id, COMPILER_64_LO(counter_value.value[0]),
            COMPILER_64_LO(counter_value.value[1]));

    return BCM_E_NONE;
}

int
flexctr_vpn_stat_get (int unit, bcm_vpn_t vpn, uint32 ing_vpn_stat_id, uint32 egr_vpn_stat_id) 
{
    bcm_flexctr_counter_value_t counter_value;
    int                            rv;
    uint32 num_entries = 1;
    uint32 counter_indexes = 0;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, ing_vpn_stat_id, num_entries,
                              &counter_indexes, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Print counter value. */
    printf("*** Flex Counters collected on ING VPN 0x%x (stat_id=0x%x)  Packets / Bytes  : 0x%08x / 0x%08x \n",
            vpn, ing_vpn_stat_id, COMPILER_64_LO(counter_value.value[0]),
            COMPILER_64_LO(counter_value.value[1]));

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, egr_vpn_stat_id, num_entries,
                              &counter_indexes, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* Print counter value. */
    printf("*** Flex Counters collected on EGR VPN 0x%x (stat_id=0x%x)  Packets / Bytes  : 0x%08x / 0x%08x \n",
            vpn, egr_vpn_stat_id, COMPILER_64_LO(counter_value.value[0]),
            COMPILER_64_LO(counter_value.value[1]));

    return BCM_E_NONE;
}

/* Create vlan and add port to vlan */
int vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

int vxlan_system_config(int unit, vxlan_cfg_t *vxlan_cfg)
{
    printf("Configure Vxlan Global Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    return BCM_E_NONE;
}

/* vxlan configuration specific to access physical port */
int vxlan_access_port_config(int unit, bcm_gport_t gport_acc)
{
    printf("Configure Vxlan Access Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gport_acc, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_acc, bcmPortControlVxlanEnable, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_acc, bcmPortControlVxlanTunnelbasedVnId, 0));
    return BCM_E_NONE;
}

/* vxlan configuration specific to network physical port */
int vxlan_network_port_config(int unit, bcm_gport_t gport_net)
{
    printf("Configure Vxlan Network Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net, bcmPortControlVxlanTunnelbasedVnId, 0));
    return BCM_E_NONE;
}

/* Create L3 interface */
int create_l3_interface(int unit, int flags, int intf_flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_flags = intf_flags;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;
    return rv;
}

int vxlan_create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
        int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 |= flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

int create_vxlan_vpn(int unit, vxlan_cfg_t *vxlan_cfg)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t mc_group=0;

    printf("Create Multicast group for UUC/MC/BC.\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &mc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Create Flow VPN.\n");

    bcm_flow_vpn_config_t vpn_info;
    bcm_flow_vpn_config_t_init (&vpn_info);
    vpn_info.flags = BCM_FLOW_VPN_ELAN;
    vpn_info.broadcast_group         = mc_group;
    vpn_info.unknown_unicast_group   = mc_group;
    vpn_info.unknown_multicast_group = mc_group;
    BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, &(vxlan_cfg->vxlan_vpn), &vpn_info));

    BCM_IF_ERROR_RETURN(vxlan_vpn_counter_attach(unit, vxlan_cfg->vxlan_vpn, &vxlan_cfg->ing_vpn_stat_id,
        &vxlan_cfg->egr_vpn_stat_id));
    vxlan_cfg->mc_group=mc_group;

    return BCM_E_NONE;
}

/* Common code for Access/Network VP's */
int create_vxlan_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t *vp)
{
    bcm_error_t rv = BCM_E_NONE;
    printf("Create Flow VP.\n");
    bcm_flow_port_t  flow_port;
    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = flags;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_port_create() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    *vp = flow_port.flow_port_id;
    return BCM_E_NONE;
}

/* Match criteria configuration */
int vxlan_flow_match_add(int unit, bcm_flow_match_config_t match_info)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_match_add() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Encap configuration */
int vxlan_flow_port_encap_set(int unit, bcm_flow_port_encap_t peinfo)
{
    bcm_error_t rv = BCM_E_NONE;
    rv =  bcm_flow_port_encap_set(unit, &peinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_port_encap_set() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Static L2 entry for forwarding */
int add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/* Access Vxlan VP creation */
int config_vxlan_access(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_acc_cfg_t *vxlan_acc, bcm_gport_t *acc_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Access Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_port_t                   phy_port;

    rv = bcm_port_local_get(unit, vxlan_acc->phy_Gport, &(phy_port));
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting LocalPort: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_create_add_port(unit, vxlan_acc->acc_vlan, phy_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, phy_port, vxlan_acc->acc_vlan));
    /* Set default inner tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, phy_port, 0x9100));
    /* Set default outer tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, phy_port, 0x8100));
    /* Set port learning */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, phy_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    /* Port must be added to VPN to pass membership check */
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, vxlan_acc->phy_Gport, 0));

    rv = vxlan_access_port_config(unit, vxlan_acc->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Access Flow Port (VP).\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, 0, acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    vxlan_acc->acc_gport=(*acc_flow_port);

    printf("Access: Add flow match (match: port,vlan criteria).\n");
    int acc_port_class = vxlan_acc->phy_Gport & 0xff;
    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, vxlan_acc->phy_Gport, bcmPortClassIngress, acc_port_class));
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria  = BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN;
    match_info.port      = acc_port_class;
    match_info.vlan      = vxlan_acc->acc_vlan;
    match_info.vpn       = vxlan_cfg->vxlan_vpn;
    match_info.valid_elements = (BCM_FLOW_MATCH_PORT_VALID | BCM_FLOW_MATCH_VLAN_VALID |
                                 BCM_FLOW_MATCH_VPN_VALID);
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match (VPN assignment): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Access SVP assignment */
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle = flow_handle;
    match_info.criteria = BCM_FLOW_MATCH_CRITERIA_PORT;
    match_info.flow_port = vxlan_acc->acc_gport;
    match_info.port = vxlan_acc->phy_Gport;
    match_info.valid_elements = BCM_FLOW_MATCH_PORT_VALID | BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match (SVP assignment): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* SD_TAG Operations */
    /* Ingress vlan preserving control ING_L2_IIF_TABLE(VLAN_TAG_PRESERVE_CTRL) */
    bcm_vlan_action_set_t vlan_action;

    bcm_vlan_action_set_t_init(&vlan_action);
    printf("vxlan_acc->sd_tag_preserve = %s\n", vxlan_acc->sd_tag_preserve?"PRESERVE":"DO NOT PRESERVE");
    if (vxlan_acc->sd_tag_preserve) {
        vlan_action.outer_tag = bcmVlanActionNone;
    } else {
        vlan_action.outer_tag = bcmVlanActionDelete;
    }
    vlan_action.forwarding_domain = vxlan_cfg->vxlan_vpn;
    BCM_IF_ERROR_RETURN(bcm_vlan_translate_action_add(unit, acc_port_class, bcmVlanTranslateKeyPortGroupOuter, vxlan_acc->acc_vlan, 0, &vlan_action));
    BCM_IF_ERROR_RETURN(vxlan_flow_counter_attach(unit, vxlan_acc->acc_gport, &vxlan_acc->ing_flow_stat_id, &vxlan_acc->egr_flow_stat_id));

    return BCM_E_NONE;
}

/* Tunnel creation and associating it with Nw VP */
int config_vxlan_tunnel(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_net_cfg_t *vxlan_net, bcm_gport_t net_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Tunnel Configuration variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t  tiinfo;
    bcm_flow_tunnel_terminator_t ttinfo;

    printf("Tunnel_init: Create Flow Tunnel Initiator.\n");
    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    tiinfo.flow_handle = flow_handle;
    tiinfo.type = bcmTunnelTypeVxlan;
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
        BCM_FLOW_TUNNEL_INIT_SIP_VALID |
        BCM_FLOW_TUNNEL_INIT_TTL_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.sip = vxlan_net->local_ip;
    tiinfo.dip = vxlan_net->remote_ip;
    tiinfo.ttl = 0x3f;
    rv = bcm_flow_tunnel_initiator_create(0,&tiinfo,0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Tunnel_term: Create Flow Tunnel Terminator.\n");
    bcm_flow_tunnel_terminator_t_init(&ttinfo);
    ttinfo.flow_handle = flow_handle;
    ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP_VALID;
    ttinfo.dip = vxlan_net->local_ip;
    ttinfo.type = bcmTunnelTypeVxlan;
    rv = bcm_flow_tunnel_terminator_create(0,&ttinfo,0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* vxlan network VP creation . tunnel is not created */
int config_vxlan_network(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_net_cfg_t *vxlan_net, bcm_gport_t *net_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Network Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t      net_intf_id;
    bcm_if_t      net_egr_id;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_flow_encap_config_t      einfo;
    bcm_port_t                   phy_port;

    rv = bcm_port_local_get(unit, vxlan_net->phy_Gport, &(phy_port));
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting LocalPort: %s.\n", bcm_errmsg(rv));
        return rv;
    }


    rv = vlan_create_add_port(unit, vxlan_net->net_vlan, phy_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, phy_port, vxlan_net->net_vlan));
    /* Set default inner tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, phy_port, 0x9100));
    /* Set default outer tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, phy_port, 0x8100));
    /* Set port learning */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, phy_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    /* Port must be added to VPN to pass membership check */
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, vxlan_net->phy_Gport, 0));

    rv = vxlan_network_port_config(unit, vxlan_net->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 interface\n");
    rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, BCM_L3_INTF_UNDERLAY, vxlan_net->net_local_mac, vxlan_net->net_vlan, &net_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 Egress object.\n");
    rv = vxlan_create_egr_obj(unit, BCM_L3_FLOW_ONLY|BCM_L3_FLAGS2_UNDERLAY, net_intf_id, vxlan_net->net_remote_mac,
            vxlan_net->phy_Gport, vxlan_net->net_vlan, &net_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Network Flow Port.\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, BCM_FLOW_PORT_NETWORK, net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_net->nw_gport=*net_flow_port;

    printf("Network: encap add, VFI-->VNID\n");
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
    einfo.vnid = vxlan_net->vxlan_vnid;
    einfo.vpn  = vxlan_cfg->vxlan_vpn;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID | BCM_FLOW_ENCAP_VPN_VALID;
    rv = bcm_flow_encap_add(unit, &einfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nNetwork: bcm_flow_encap_add() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* SD_TAG Operations */
    /* Egress vlan action in EGR_VFI(TAG_ACTION,VLAN_0,VLAN_1) */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vxlan_cfg->vxlan_vpn, &vlan_ctrl));

    if (vxlan_net->sd_tag_operation == 0) { /* NOOP */
        /* Keeps the original payload VLAN */
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, *net_flow_port, bcmVlanPortPayloadTagsDelete, 0));
    } else if (vxlan_net->sd_tag_operation == 1) { /* Add */
        vlan_ctrl.egress_action.outer_tag = bcmVlanActionAdd;
        /* Keeps the original payload VLAN */
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, *net_flow_port, bcmVlanPortPayloadTagsDelete, 0));
    } else if (vxlan_net->sd_tag_operation == 2) { /* Delete */
        vlan_ctrl.egress_action.outer_tag = bcmVlanActionNone;
        /* Delete the original payload VLAN */
        BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, *net_flow_port, bcmVlanPortPayloadTagsDelete, 1));
    }

    vlan_ctrl.egress_action.new_outer_vlan = vxlan_net->sd_tag_vid;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vxlan_cfg->vxlan_vpn, vlan_ctrl));

    printf("Network: Configure match condition to derive VPN (Match: {VN_ID}, Derive: VFI (VPN))\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle    = flow_handle;
    match_info.criteria       = BCM_FLOW_MATCH_CRITERIA_VN_ID;
    match_info.vpn            = vxlan_cfg->vxlan_vpn;
    match_info.vnid           = vxlan_net->vxlan_vnid;
    match_info.valid_elements = BCM_FLOW_MATCH_VPN_VALID |
        BCM_FLOW_MATCH_VNID_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network; Set Encapsulation. VP=>egress-obj{Device,Port}.\n");
    bcm_flow_port_encap_t_init(&peinfo);
    peinfo.flow_handle = flow_handle;
    peinfo.flow_port = *net_flow_port;
    peinfo.egress_if = net_egr_id;
    peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_PORT_VALID |
        BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
    rv = vxlan_flow_port_encap_set(unit, peinfo);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network encap: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network: Add flow match (match: SIP => network SVP)\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle = flow_handle;
    match_info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
    match_info.sip = vxlan_net->remote_ip;
    match_info.flow_port = *net_flow_port;
    match_info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
        BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(vxlan_flow_counter_attach(unit, vxlan_net->nw_gport, &vxlan_net->ing_flow_stat_id, &vxlan_net->egr_flow_stat_id));

    return BCM_E_NONE;
}

/* Global Variables */
struct app_vxlan_vpn_t {
    bcm_vpn_t vpn;
    uint32 ing_vpn_stat_id;
    uint32 egr_vpn_stat_id;    
};
struct app_vxlan_port_t {
    bcm_gport_t flow_gport;
    uint32 ing_flow_stat_id;
    uint32 egr_flow_stat_id;
};

app_vxlan_vpn_t app_vxlan_vpn;
app_vxlan_port_t app_vxlan_acc_port;
app_vxlan_port_t app_vxlan_net_port;

/* vxlan Configuration function */
bcm_error_t config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        0x7010,
        0,
        0,
        0,                  /* ing_vpn_stat_id */
        0                   /* egr_vpn_stat_id */
    };

    vxlan_acc_cfg_t vxlan_acc = {
        ingress_phy_gport, /* port */
        21,/*vlan */
        "00:00:00:00:11:11",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        0,
        1,                  /* sd_tag_preserve */
        0,                  /* ing_flow_stat_id */
        0                   /* egr_flow_stat_id */
    };

    vxlan_net_cfg_t vxlan_net = {
        egress_phy_gport, /* port */
        22, /* vlan */
        "00:00:00:00:22:22",/* local mac */
        "00:00:00:00:00:02",/* remote mac */
        0xC0A80101,  /*192.168.1.1 remote IP*/
        0x12345,     /* vnid */
        0x0a0a0a01,  /*10.10.10.1 local IP*/
        0xffff,
        0,
        1,           /* sd_tag_operation = Add */
        100,         /* new sd_tag_vid value */
        0,                  /* ing_flow_stat_id */
        0                   /* egr_flow_stat_id */
    };
    /* flow API */
    bcm_flow_handle_t            flow_handle;
    bcm_gport_t   acc_flow_port;
    bcm_gport_t   net_flow_port;

    /* working variables */
    uint32 flags=0;

    rv = vxlan_system_config(unit, &vxlan_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in global setting: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Get flow-handler for Classic_Vxlan\n");
    rv = bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting flow handle: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vxlan_vpn(unit, &vxlan_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vpn create: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    app_vxlan_vpn.vpn = vxlan_cfg.vxlan_vpn;
    app_vxlan_vpn.ing_vpn_stat_id = vxlan_cfg.ing_vpn_stat_id;
    app_vxlan_vpn.egr_vpn_stat_id = vxlan_cfg.egr_vpn_stat_id;

    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc, &acc_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    app_vxlan_acc_port.flow_gport = acc_flow_port;
    app_vxlan_acc_port.ing_flow_stat_id = vxlan_acc.ing_flow_stat_id;
    app_vxlan_acc_port.egr_flow_stat_id = vxlan_acc.egr_flow_stat_id;

    rv = config_vxlan_network(unit, &vxlan_cfg, &vxlan_net, &net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    app_vxlan_net_port.flow_gport = net_flow_port;
    app_vxlan_net_port.ing_flow_stat_id = vxlan_net.ing_flow_stat_id;
    app_vxlan_net_port.egr_flow_stat_id = vxlan_net.egr_flow_stat_id;

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net, net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Access-to-Network: Add payload-dest-mac Address => Network-flow-port (VP).\n");
    rv = add_to_l2_table(unit, "00:00:00:00:aa:aa", vxlan_cfg.vxlan_vpn, net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* verifying the VPN VP configuration using traverse*/
int vpn_traverse(int unit,bcm_vpn_t vpn, bcm_flow_vpn_config_t *info, void *data)
{
    int maxArr=20;
    int count=0,iter=0;
    bcm_flow_port_t flow_portArr[maxArr];

    printf("\n Vpn :%d \n",vpn);
    printf("\n VpnInfo : \n");
    print *info;
    BCM_IF_ERROR_RETURN(bcm_flow_port_get_all(unit,vpn,maxArr,flow_portArr,&count));

    printf("\n Flow Ports %d\n",count);
    for(iter=0;(iter<count) && (iter < maxArr);++iter) {
        print flow_portArr[iter];
    }
}

/* verifying the VPN configuration */
bcm_error_t show_vxlan_vtep_config(int unit)
{
    uint32 null_user_data=0;
    void  *user_data_ptr=&null_user_data;
    bcm_flow_vpn_traverse(unit,vpn_traverse,user_data_ptr);
    return 0;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vxlan_ipv4_vtep())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t execute(void)
{
    bcm_error_t rv;
    int unit = 0;

    print "config show; attach; version";
    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_vxlan_ipv4_vtep(): ** start **";
    if (BCM_FAILURE((rv = config_vxlan_ipv4_vtep(unit)))) {
        printf("config_vxlan_ipv4_vtep() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vxlan_ipv4_vtep(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}