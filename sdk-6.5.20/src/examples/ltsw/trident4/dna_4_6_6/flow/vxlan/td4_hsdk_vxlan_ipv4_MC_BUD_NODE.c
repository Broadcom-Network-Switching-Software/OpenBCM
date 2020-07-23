/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880) Vxlan Bridging, Network to network & Acess known multicast Flow
 *
 *
 * Usage    : BCM.0> cint TD4_HSDK_VxLAN_ipv4_MC_BUD_NODE.c
 *
 * config   : bcm56880_a0-generic-32x400_lb.config.yml
 *
 * Log file : TD4_HSDK_VxLAN_ipv4_MC_BUD_NODE_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   VLAN-22        Underlay-NextHop
 *                                  |                      |  ingress_port      +---+
 *   SMAC 00::11:11                 |                      +--------------------+   |
 *            +---+          VLAN-21|                      |00::22:22           +---+
 *   Host(acc)|   +-----------------+      Trident-4       |  Network_port      00::02
 *            +---+    Egress_port  |                      |
 *                                  |                      |
 *            +---+          VLAN-23|                      |
 *     Network|   +-----------------|                      |
 *            +---+    Egress_port  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN Bridging configuration for Multicast ingressing on Bud Node
 *             Network to network -> IPMC-transit mode (Pass 1)
 *             Network to Access  -> Termination (Pass 2)
 *
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects 3 ports and configure them in Loopback mode. Out of these
 *        3 ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan()):
 *   =========================================================
 *     a) Configure a VxLAN Bridging configuration .
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *        V6 is FlexFlow feature and hence requires FLow Options to be used
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show'
 *
 *     b) Transmit the known multicast packet. The contents of the packet
 *        are printed on screen.
 *
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the 2 packets : ipmc routed packet on network and decapped packet on the egress access port
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 */

/*
Packet FLow for known MC:


packet sent from ingress_port(network) to multiple egress port(access/network)

Ingress Packet:
======

Header Info

###[ Ethernet ]###
  dst= 01:00:5E:00:00:0A
  src= 00:00:00:00:00:02
  type= 0x8100
###[ 802.1Q ]###
  vlan= 22
  type= 0x0800
###[ IPv4 ]###
  version= 4
  nh= UDP
  src= 192.168.1.1
  dst= 225.0.0.10
###[ UDP ]###
  sport= 55555
  dport= 4789
###[ VXLAN ]###
  flags= Instance
  reserved1= 0
  vni= 0x12345
  reserved2= 0x0
###[ Ethernet ]###
  dst= 01:00:5e:01:01:01
  src= 00:00:00:00:BB:BB
  type= 0x800
###[ IP ]###
  version= 4
  proto= udp
  src= 2.2.2.2
  dst= 225.1.1.1
###[ UDP ]###
###[ Raw ]###

Ingress Hex Packet:
tx 1 pbm=1 data=0x01005E00000A00000000000281000016080045000082000040003F1198B7C0A80101E100000AD90312b5006ED6B4080000000123450001005E01010100000000BBBB080045000050000100004011949602020202E101010100350035003C52F574657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Egress Packet(Network) :
======

Header Info

###[ Ethernet ]###
  dst= 01:00:5E:00:00:0A
  src= 00:00:00:00:33:33
  type= 0x8100
###[ 802.1Q ]###
  vlan= 23
  type= 0x0800
###[ IPv4 ]###
  version= 4
  nh= UDP
  src= 192.168.1.1
  dst= 225.0.0.10
###[ UDP ]###
  sport= 55555
  dport= 4789
###[ VXLAN ]###
  flags= Instance
  reserved1= 0
  vni= 0x12345
  reserved2= 0x0
###[ Ethernet ]###
  dst= 01:00:5e:01:01:01
  src= 00:00:00:00:BB:BB
  type= 0x800
###[ IP ]###
  version= 4
  proto= udp
  src= 2.2.2.2
dst= 225.1.1.1
###[ UDP ]###
###[ Raw ]###

data[0000]: {01005e00000a} {000000003333} 8100 0017
data[0010]: 0800 4500 0082 0000 4000 3e11 99b7 c0a8 
data[0020]: 0101 e100 000a d903 12b5 006e d6b4 0800 
data[0030]: 0000 0123 4500 0100 5e01 0101 0000 0000 
data[0040]: bbbb 0800 4500 0050 0001 0000 4011 9496 
data[0050]: 0202 0202 e101 0101 0035 0035 003c 52f5 
data[0060]: 7465 7374 7465 7374 7465 7374 7465 7374 
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374 
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374 
data[0090]: 7465 7374 

Egress Packet(access) :
======

Header Info

###[ Ethernet ]###
  dst= 01:00:5e:01:01:01
  src= 00:00:00:00:BB:BB
  type= 0x8100
###[ 802.1Q ]###
  vlan= 1
  type= 0x800
###[ IP ]###
  version= 4
  proto= udp
  src= 2.2.2.2
  dst= 225.1.1.1
###[ UDP ]###

Packet Dump
data[0000]: {01005e010101} {00000000bbbb} 8100 0001
data[0010]: 0800 4500 0050 0001 0000 4011 9496 0202 
data[0020]: 0202 e101 0101 0035 0035 003c 52f5 7465 
data[0030]: 7374 7465 7374 7465 7374 7465 7374 7465 
data[0040]: 7374 7465 7374 7465 7374 7465 7374 7465 
data[0050]: 7374 7465 7374 7465 7374 7465 7374 7465 
data[0060]: 7374 

*/

/* Reset C interpreter*/
cint_reset();

/**** global constants ****/
const int   IN_PORT_NUM = 1;   // fix
const int   OUT_PORT_NUM = 2;  // 1 or 2

bcm_port_t ingress_port;
bcm_port_t egress_port[OUT_PORT_NUM];

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
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
 * Creates a Field Group for copying/dropping packets 
 * on a certain ingress port.
 */
bcm_error_t
field_group_setup(int unit, bcm_field_group_config_t *group_config)
{
    bcm_field_group_config_t_init(group_config);
    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyInPort); 
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyVpn); 
    BCM_FIELD_ASET_ADD(group_config->aset, bcmFieldActionCopyToCpu);   
    BCM_FIELD_ASET_ADD(group_config->aset, bcmFieldActionDrop);
    group_config->mode = bcmFieldGroupModeAuto;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, group_config));

    return BCM_E_NONE;
}


/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port, bcm_vlan_t vid,
                    bcm_field_group_config_t group_config,
                    bcm_field_entry_t *entry)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, *entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_Vpn(unit, *entry, vid, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, *entry, BCM_FIELD_ENTRY_PRIO_HIGHEST));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, *entry));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, bcm_port_t port, bcm_vlan_t vid,
                    bcm_field_group_config_t group_config,
                    bcm_field_entry_t *entry)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, *entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_Vpn(unit, *entry, vid, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, *entry, BCM_FIELD_ENTRY_PRIO_HIGHEST));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, *entry));

    // Default entry to drop all traffic from egress_port
    bcm_field_entry_t default_entry;
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &default_entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, default_entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, default_entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, default_entry, BCM_FIELD_ENTRY_PRIO_LOWEST));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, default_entry));

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int i;
    int port_list[IN_PORT_NUM + OUT_PORT_NUM];
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry[IN_PORT_NUM + OUT_PORT_NUM];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, IN_PORT_NUM + OUT_PORT_NUM)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    if (BCM_E_NONE !=field_group_setup(unit, &group_config)){
        printf("field_group_setup() failed to create field group\n");
        return -1;
    }

    ingress_port = port_list[0];
    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port, net1_vid, group_config, &entry[0])) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    bcm_vlan_t egress_port_vid[OUT_PORT_NUM];
    egress_port_vid[0] = 1; /* customer_port, PVID is added after tunnel decap */
    egress_port_vid[1] = net2_vid; /* provider_port2 */
    for (i = IN_PORT_NUM; i < (IN_PORT_NUM + OUT_PORT_NUM); i++){   
        egress_port[i - IN_PORT_NUM] = port_list[i];
        if (BCM_E_NONE != egress_port_setup(unit, egress_port[i - IN_PORT_NUM], egress_port_vid[i - IN_PORT_NUM], group_config, &entry[i])) {
            printf("egress_port_setup(port2) failed for port %d\n", egress_port[i - IN_PORT_NUM]);
            return -1;
        }
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}


/************************** VXLAN Sub-Routines ****************************/

/**** global constants ****/
const int   max_vxlan_port_num_per_phy_port = 8;
const int   FLAGS_PROVIDER      = 0x1;
const int   FLAGS_CUSTOMER      = 0x2;
const int   FLAGS_REMOTE_CHIP   = 0x4;
const int   FLAGS_MC_ONLY       = 0x8;

const int   VXLAN_UDP_DST_PORT  = 4789;      // IANA (RFC 7348)

const int   FLAGS_FLOW_IPV6     = 0x1;
const int   FLAGS_FLOW_CLASSIC  = 0x2;

const int   FLAGS_TUNNEL_MC     = 0x1;
const int   FLAGS_TUNNEL_MC_BUD = 0x2;

/* flow API */
bcm_flow_handle_t   flow_handle;
int                 flow_flag;         

/**** Data Structures ****/
struct vxlan_port_s {
    bcm_vpn_t       vpn;    
    bcm_flow_port_t flow_port;    
    int             vid;
    bcm_mac_t       local_mac;
    bcm_mac_t       remote_mac;  
    bcm_if_t        intf_id;    // Required For Tunnel 
    bcm_if_t        egr_id;      
    uint32          vnid; 

    /* Specific for Network port */
    bcm_flow_encap_criteria_t criteria;    
    bcm_ip_t        remote_ip4;
    bcm_ip_t        local_ip4;    
    bcm_ip6_t       remote_ip6;
    bcm_ip6_t       local_ip6;
    int             ttl;
    uint32          vnid;
    int             dvp_group;
    bcm_ip_t        mcast_ip4;
    bcm_ip6_t       mcast_ip6;
    bcm_mac_t       mcast_mac;
    int             mc_tnl_flag;
};


struct vxlan_phy_port_s {
    int             modid;
    int             port_id;
    bcm_gport_t     gport_id;    
    vxlan_port_s    vxlan_port[max_vxlan_port_num_per_phy_port];    
    int             vxlan_port_num;    
    int             flags;
};


/**** Internal Sub-functions ****/

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS)) {
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);  
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Add L2 Entry */
bcm_error_t
vxlan_l2_add(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags   = BCM_L2_STATIC;
    l2_addr.port    = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding static L2 entry: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Add L2 Station MAC for VXLAN Termination */
bcm_error_t
vxlan_l2_station_add(int unit, bcm_mac_t mac)
{
    bcm_error_t rv = BCM_E_NONE;
    int station_id;
    bcm_l2_station_t l2_station;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding L2 station: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;    
}

/* Create L3 ingress interface */
bcm_error_t
create_l3_ingress_interface(int unit, bcm_vlan_t vid, bcm_if_t *l3_iif)
{
    bcm_error_t             rv = BCM_E_NONE; 
    bcm_l3_ingress_t        l3_ingress; 
    bcm_vlan_control_vlan_t vlan_ctrl;  

    printf("Create network L3 ingress interface\n");
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.vrf = 0;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, l3_iif);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring L3 ingress interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Config vpn(0x%x) to l3_iif(0x%x) mapping.\n", vid, *l3_iif);
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    vlan_ctrl.ingress_if = *l3_iif;
    rv = bcm_vlan_control_vlan_selective_set(unit, vid, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vid to l3_iif: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/* Create L3 Egress Interface */
bcm_error_t 
vxlan_l3_intf_create(int unit, bcm_vlan_t vlan, bcm_mac_t mac_addr, int is_net_intf, 
                        bcm_flow_handle_t flow_handle, bcm_if_t * l3_intf_id)
{
    bcm_l3_intf_t l3_interface;

    /* Create L3 Interface */
    bcm_l3_intf_t_init(&l3_interface);
    if (is_net_intf)  {  
        l3_interface.l3a_flags = BCM_L3_ADD_TO_ARL;   
            if (flow_handle)  {  
                l3_interface.flow_handle = flow_handle;         
                l3_interface.l3a_intf_flags  = BCM_L3_INTF_UNDERLAY; 
            }
    }
    sal_memcpy(l3_interface.l3a_mac_addr, mac_addr, sizeof(mac_addr));
    l3_interface.l3a_vid = vlan;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_interface));
    *l3_intf_id = l3_interface.l3a_intf_id;
    
    return BCM_E_NONE;
}

/* Create L3 Egress Object */
bcm_error_t 
vxlan_l3_egress_create(int unit, uint32 flags, uint32 flags2, bcm_if_t l3_if, bcm_mac_t mac_addr, bcm_vlan_t vlan, 
                           bcm_gport_t gport, bcm_flow_handle_t flow_handle, bcm_gport_t flow_port_id, bcm_if_t *egr_obj)
{
    bcm_l3_egress_t             l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2        = flags2;
    l3_egress.intf          = l3_if;
    l3_egress.flow_handle   = flow_handle;
    sal_memcpy(l3_egress.mac_addr,  mac_addr, sizeof(mac_addr));
    l3_egress.vlan          = vlan;
    l3_egress.port          = gport;
    l3_egress.encap_id      = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);   
     
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, flags , &l3_egress, egr_obj));
    
    return BCM_E_NONE;
}

bcm_error_t 
vxlan_vpn_create(int unit, bcm_multicast_t bc_group, bcm_vpn_t vpn)
{
    bcm_error_t           rv;    
    bcm_flow_vpn_config_t vpn_info;

    bcm_flow_vpn_config_t_init (&vpn_info);
    vpn_info.flags = BCM_FLOW_VPN_ELAN | BCM_FLOW_VPN_WITH_ID;
    vpn_info.broadcast_group         = bc_group;
    vpn_info.unknown_unicast_group   = bc_group;
    vpn_info.unknown_multicast_group = bc_group;
    rv = bcm_flow_vpn_create(unit, &vpn, &vpn_info);
    if (BCM_FAILURE(rv)) { 
        printf("\r\n bcm_vxlan_vpn_create = %s, vpn = %d\n", bcm_errmsg(rv), vpn);  
    } else {
        printf("Creating VXLAN VPN (0x%x)... done!\n", vpn);
    } 
        
    return BCM_E_NONE;
}

bcm_error_t
vxlan_access_port_create(int unit, bcm_vpn_t vpn, vxlan_phy_port_s *vxlan_access_port)
{
    vxlan_port_s            * v_port = &vxlan_access_port->vxlan_port[vxlan_access_port->vxlan_port_num];
    bcm_error_t             rv = BCM_E_NONE;   
    bcm_flow_port_t         flow_port;
    bcm_flow_match_config_t match_info;
    bcm_flow_port_encap_t   peinfo;

    rv = vlan_create_add_port(unit, v_port->vid, vxlan_access_port->port_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add ports to VPN to pass membership check */
    rv = bcm_vlan_gport_add(unit, vpn, vxlan_access_port->gport_id, 0);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring VPN membership check: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    // Create  Access Flow Port (VP).
    bcm_flow_port_t_init(&flow_port);
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_port_create() failed: %s\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Creating Access Flow Port (0x%x)... done!\n", flow_port.flow_port_id);
    }

    // Access: VPN assignment, port + outer_VID -> VPN.
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria       = BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN;
    match_info.valid_elements = BCM_FLOW_MATCH_PORT_VALID |
                                BCM_FLOW_MATCH_VLAN_VALID | 
                                BCM_FLOW_MATCH_VPN_VALID;
    match_info.port           = (vxlan_access_port->gport_id & 0xff);  // use port id as port group
    match_info.vlan           = v_port->vid;
    match_info.vpn            = vpn;
    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);   
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match(VPN): %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Creating Access Flow Match for VPN... done!\n");
    }

    // Access: Add flow match (match: port criteria) -> SVP.
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria         = BCM_FLOW_MATCH_CRITERIA_PORT;
    match_info.port             = vxlan_access_port->gport_id;
    match_info.flow_port        = flow_port.flow_port_id;
    match_info.valid_elements   = BCM_FLOW_MATCH_PORT_VALID |
                                  BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match(SVP): %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Creating Access Flow Match for SVP... done!\n");
    }
                                     
    v_port->flow_port       = flow_port;
    v_port->vpn             = vpn;   
    vxlan_access_port->vxlan_port_num++;
    
    return BCM_E_NONE;
}    

bcm_error_t
vxlan_network_port_create(int unit, bcm_vpn_t vpn, vxlan_phy_port_s *vxlan_network_port)
{   
    vxlan_port_s            * v_port = &vxlan_network_port->vxlan_port[vxlan_network_port->vxlan_port_num];    
    bcm_error_t             rv = BCM_E_NONE;
    bcm_flow_port_t         flow_port;
    bcm_flow_encap_config_t einfo;
    bcm_flow_match_config_t match_info;
    bcm_flow_port_encap_t   peinfo;
    int  is_ipmc_only = (FLAGS_MC_ONLY == (vxlan_network_port->flags & FLAGS_MC_ONLY));

    rv = vlan_create_add_port(unit, v_port->vid, vxlan_network_port->port_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add ports to VPN to pass membership check */
    rv = bcm_vlan_gport_add(unit, vpn, vxlan_network_port->gport_id, 0);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring VPN membership check: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    // Create network L3 interface
    rv = vxlan_l3_intf_create(unit, v_port->vid, v_port->local_mac, 1,
                                        ((is_ipmc_only) ? 0 : flow_handle), &v_port->intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Creating Network Interface (0x%x)... done!\n", v_port->intf_id);
    }

    if (is_ipmc_only) {
        printf("Create Network port for IPMC domain... done!\n");
        return BCM_E_NONE;
    }

    // Create  Network Flow Port.
    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = BCM_FLOW_PORT_NETWORK;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_port_create() failed: %s\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Create Network Flow Port (0x%x)... done!\n", flow_port.flow_port_id);
    }

    // Create network L3 Egress object
    rv = vxlan_l3_egress_create(unit, 0, BCM_L3_FLAGS2_UNDERLAY, v_port->intf_id, 
                                    v_port->remote_mac, v_port->vid,
                                    vxlan_network_port->gport_id, flow_handle,
                                    flow_port.flow_port_id, &v_port->egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Create Network Egress Object (0x%x)... done!\n", v_port->egr_id);
    }

    // Network: encap add, (VFI,DVP) or (VFI)-->VNID
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle       = flow_handle;
    einfo.criteria          = v_port->criteria;
    einfo.vnid              = v_port->vnid;
    einfo.vpn               = vpn;
    einfo.dvp_group         = v_port->dvp_group;
    einfo.valid_elements    = BCM_FLOW_ENCAP_VNID_VALID |
                              BCM_FLOW_ENCAP_VPN_VALID;
    if (BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP == 
               (einfo.criteria & BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP)) {
        einfo.valid_elements |= BCM_FLOW_ENCAP_DVP_GROUP_VALID;      
    } 
    rv = bcm_flow_encap_add(unit, &einfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nNetwork: bcm_flow_encap_add() failed: %s\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Creating Flow Encap... done!\n");
    }

    // Network: Configure match condition to derive VPN (Match: {VN_ID}, Derive: VFI (VPN))
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle    = flow_handle;
    match_info.criteria       = BCM_FLOW_MATCH_CRITERIA_VN_ID;
    match_info.vpn            = vpn;
    match_info.vnid           = v_port->vnid;
    match_info.valid_elements = BCM_FLOW_MATCH_VPN_VALID |
                                BCM_FLOW_MATCH_VNID_VALID;
    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_EXISTS) {
            printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
            return rv;
        } else {
            printf("Warning: Flow Match (VNID 0x%x -> VFI 0x%x) Entry exists!\n", v_port->vnid, vpn);
        }
    } else {
        printf("Adding Flow Match (VNID -> VFI)... done!\n");
    }

    // Network; Set  Encapsulation. VP=>egress-obj{Device,Port}
    bcm_flow_port_encap_t_init(&peinfo);
    peinfo.flow_handle      = flow_handle;
    peinfo.dvp_group        = v_port->dvp_group;;
    peinfo.flow_port        = flow_port.flow_port_id;
    peinfo.egress_if        = v_port->egr_id;
    peinfo.valid_elements   = BCM_FLOW_PORT_ENCAP_DVP_GROUP_VALID |
                              BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
    rv = bcm_flow_port_encap_set(unit, &peinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network encap: %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Setting Port Encap... done!\n");
    }

    // Network: Add  flow match (match: SIP => network SVP)
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle      = flow_handle;
    match_info.criteria         = BCM_FLOW_MATCH_CRITERIA_SIP;     
    match_info.flow_port        = flow_port.flow_port_id;
    match_info.valid_elements   = BCM_FLOW_MATCH_FLOW_PORT_VALID;
    if (flow_flag == FLAGS_FLOW_CLASSIC) {
        match_info.sip          = v_port->remote_ip4;
        match_info.valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
    } else {      
        match_info.sip6         = v_port->remote_ip6;
        match_info.valid_elements |= BCM_FLOW_MATCH_SIP_V6_VALID;
    }
    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Adding Flow Match (SIP -> SVP)... done!\n");
    }

    v_port->flow_port           = flow_port;
    v_port->vpn                 = vpn;   
    vxlan_network_port->vxlan_port_num++;
    
    return rv;
}

bcm_error_t 
vxlan_tunnel_create(int unit, vxlan_port_s * v_port)
{      
    bcm_error_t                     rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t     tiinfo;
    bcm_flow_tunnel_terminator_t    ttinfo;

    // Tunnel_init: Create  Flow Tunnel Initiator
    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    tiinfo.flow_handle      = flow_handle;
    tiinfo.valid_elements   = BCM_FLOW_TUNNEL_INIT_TTL_VALID;                           
    tiinfo.ttl              = v_port->ttl;
    tiinfo.flow_port        = v_port->flow_port.flow_port_id;
    if (flow_flag == FLAGS_FLOW_CLASSIC) {
        tiinfo.type     = bcmTunnelTypeVxlan;
        tiinfo.sip      = v_port->local_ip4;
        if (v_port->mc_tnl_flag) {
            tiinfo.dip  = v_port->mcast_ip4;
        } else {
            tiinfo.dip  = v_port->remote_ip4;
        }
        tiinfo.valid_elements |= BCM_FLOW_TUNNEL_INIT_DIP_VALID |
                                 BCM_FLOW_TUNNEL_INIT_SIP_VALID;        
    } else {
        tiinfo.type     = bcmTunnelTypeVxlan6;
        tiinfo.sip6     = v_port->local_ip6;
        if (v_port->mc_tnl_flag) {
            tiinfo.dip6 = v_port->mcast_ip6;
        } else {        
            tiinfo.dip6 = v_port->remote_ip6;
        }
        tiinfo.valid_elements |= BCM_FLOW_TUNNEL_INIT_DIP6_VALID |
                                 BCM_FLOW_TUNNEL_INIT_SIP6_VALID;
    }

    rv = bcm_flow_tunnel_initiator_create(unit, &tiinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring init-tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    } else {
        printf("Creating VXLAN init-tunnel... done!\n");
    } 

    // Tunnel_term: Create  Flow Tunnel Terminator
    bcm_flow_tunnel_terminator_t_init(&ttinfo);
    ttinfo.flow_handle = flow_handle;
    if (flow_flag == FLAGS_FLOW_CLASSIC) {
        ttinfo.type = bcmTunnelTypeVxlan;
        ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP_VALID;
        if (v_port->mc_tnl_flag) {
            ttinfo.dip = v_port->mcast_ip4;
        } else { 
            ttinfo.dip = v_port->local_ip;   
        }     
    } else {
        ttinfo.type = bcmTunnelTypeVxlan6;
        ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP6_VALID;
        if (v_port->mc_tnl_flag) {
            ttinfo.dip6 = v_port->mcast_ip6;        
        } else {
            ttinfo.dip6 = v_port->local_ip6;
        }
    }
    if (v_port->mc_tnl_flag == FLAGS_TUNNEL_MC_BUD) {
        ttinfo.multicast_flag = BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
    }   
    rv = bcm_flow_tunnel_terminator_create(unit, &ttinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_EXISTS) {
            printf("\nError in configuring term-tunnel: %s.\n", bcm_errmsg(rv));
            return rv;
        } else {
            printf("Warning: The VxLAN DIP already created!\n");
        }        
    } else {
        printf("Creating VXLAN term-tunnel... done!\n");
    } 

    return BCM_E_NONE;
}


bcm_error_t
vxlan_multicast_port_add(int unit, int create_group, int flags, bcm_multicast_t *mc_group, 
                                 vxlan_phy_port_s *vxlan_port, int vxlan_port_idx)
{
    vxlan_port_s            * v_port = &vxlan_port->vxlan_port[vxlan_port_idx];        
    bcm_error_t             rv = BCM_E_NONE;
    bcm_if_t                encap_id = BCM_IF_INVALID;
    bcm_multicast_encap_t   mc_encap;

    if(create_group) {
        rv = bcm_multicast_create(unit, flags, mc_group);
        if (BCM_FAILURE(rv)) {
            printf("\nError in creating Mcast Group: %s.\n", bcm_errmsg(rv));
            return rv;
        }
        printf("Creating Mcast Group (0x%x)... done!\n", *mc_group);
    }

    if (vxlan_port->flags == FLAGS_CUSTOMER) { // Access

    } else { // Network
        bcm_multicast_encap_t_init(&mc_encap);
        if (flags == BCM_MULTICAST_TYPE_FLOW) { // Tunnel
            mc_encap.encap_type = bcmMulticastEncapTypeL2Tunnel;
            mc_encap.ul_egress_if   = v_port->egr_id;        
            mc_encap.l3_intf        = 0;  
            mc_encap.port_id        = v_port->flow_port.flow_port_id;              
        } else {  // Transit      
            mc_encap.encap_type = bcmMulticastEncapTypeL3;
            mc_encap.ul_egress_if   = 0;
            mc_encap.l3_intf        = v_port->intf_id;
        }
        rv = bcm_multicast_encap_create(unit, &mc_encap, &encap_id);
        if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
            printf("\nError in creating Mcast Encap ID: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_multicast_egress_add(unit, *mc_group, vxlan_port->gport_id, encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in egress add: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Adding port (0x%x) to Mcast Group (0x%x)... done.\n", vxlan_port->gport_id, *mc_group);

    return rv;
}

                                   
bcm_error_t
vxlan_init(int unit, int flags)
{
    BCM_IF_ERROR_RETURN( bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    if (flags == FLAGS_FLOW_CLASSIC) {
        BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle));
    } else {
        BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "VXLAN", &flow_handle));
    }
    flow_flag = flags;

    return BCM_E_NONE;
}

bcm_error_t
vxlan_port_create(int unit, int modid, bcm_port_t port_id, int flags, vxlan_phy_port_s * port)
{
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    if (my_modid == modid) {  
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_id, &port->gport_id));  
        BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_id, bcmPortControlVxlanTunnelbasedVnId, 0));
        if (FLAGS_PROVIDER == flags) {    
            BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_id, bcmPortControlVxlanEnable, 1));
        } else {
            BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_id, bcmPortControlVxlanEnable, 0));
            BCM_IF_ERROR_RETURN( bcm_vlan_control_port_set(unit, port_id, bcmVlanTranslateIngressEnable, 1));

            /* Configure ING_L2_IIF_TABLE.PORT_GROUP_ID - use port id as the port group */
            BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, port->gport_id, bcmPortClassIngress, (port->gport_id & 0xff)));

        }
    } else {
        BCM_GPORT_MODPORT_SET(port->gport_id, modid, port_id);
        flags |= FLAGS_REMOTE_CHIP;
    }   
    port->modid             = modid;      
    port->port_id           = port_id;
    port->flags             = flags;
    port->vxlan_port_num    = 0;

    return BCM_E_NONE;
}


/************************** Main Routine ****************************/
bcm_vpn_t           vxlan_vpn = 0x8064;
vxlan_phy_port_s    customer_port, provider_port1, provider_port2;

/* Access Port Coinfigurations */
bcm_vlan_t      acc_vid = 21;
bcm_mac_t       acc_local_mac = "00:00:00:00:11:11";
bcm_mac_t       acc_remote_mac = "00:00:00:00:00:01";

/* Network Port 1 Coinfigurations */
bcm_vlan_t      net1_vid = 22;
bcm_mac_t       net1_local_mac = "00:00:00:00:22:22";
bcm_mac_t       net1_remote_mac = "00:00:00:00:00:02";
bcm_ip_t        net1_remote_ip4 = 0xC0A80101; /* 192.168.1.1 Remote IP */
bcm_ip_t        net1_local_ip4  = 0x0A0A0A01; /* 10.10.10.1 Local IP */
bcm_ip_t        net1_mcast_ip4  = 0xE100000A;
bcm_mac_t       mcast_mac = "01:00:5E:00:00:0A";
uint32          vnid = 0x12345; 
int             dvp_group = 10 + 2048;
bcm_flow_encap_criteria_t criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP;

/* Network Port 2 Coinfigurations */
bcm_vlan_t      net2_vid = 23;
bcm_mac_t       net2_local_mac = "00:00:00:00:33:33";

/* Multicast Groups*/
bcm_multicast_t vxlan_bcast_group = 0;
bcm_multicast_t mcast_group = 0;
bcm_multicast_t mcast_group_transit = 0;

const int   CREATE      = 1;
const int   NOT_CREATE  = 0;

bcm_error_t
config_vxlan(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int my_modid;

    BCM_IF_ERROR_RETURN(vxlan_init(unit, FLAGS_FLOW_CLASSIC));
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(vxlan_port_create(unit, my_modid, ingress_port,    FLAGS_PROVIDER, &provider_port1));    
    BCM_IF_ERROR_RETURN(vxlan_port_create(unit, my_modid, egress_port[0],  FLAGS_CUSTOMER, &customer_port));
    BCM_IF_ERROR_RETURN(vxlan_port_create(unit, my_modid, egress_port[1],  FLAGS_PROVIDER, &provider_port2));

    /* Multicast group for non-UC */
    BCM_IF_ERROR_RETURN( bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &vxlan_bcast_group));
    
    /* Create VXLAN VPN */
    BCM_IF_ERROR_RETURN( vxlan_vpn_create(unit, vxlan_bcast_group, vxlan_vpn));

    /* Create VXLAN VP for access port */
    customer_port.vxlan_port[0].vid = acc_vid;
    BCM_IF_ERROR_RETURN(vxlan_access_port_create(unit, vxlan_vpn, &customer_port));
     
    /* Create VXLAN VP for network ports */
    provider_port1.vxlan_port[0].vid        = net1_vid;
    provider_port1.vxlan_port[0].local_mac  = net1_local_mac;
    provider_port1.vxlan_port[0].remote_mac = net1_remote_mac;
    provider_port1.vxlan_port[0].criteria   = criteria;
    provider_port1.vxlan_port[0].vnid       = vnid;
    provider_port1.vxlan_port[0].dvp_group  = dvp_group;
    provider_port1.vxlan_port[0].remote_ip4 = net1_remote_ip4;
    BCM_IF_ERROR_RETURN(vxlan_network_port_create(unit, vxlan_vpn, &provider_port1));

    provider_port2.vxlan_port[0].vid        = net2_vid;
    provider_port2.vxlan_port[0].local_mac  = net2_local_mac;
    provider_port2.flags                   |= FLAGS_MC_ONLY;
    BCM_IF_ERROR_RETURN(vxlan_network_port_create(unit, vxlan_vpn, &provider_port2));
 
    /* Create init & term tunnel */
    provider_port1.vxlan_port[0].ttl        = 0x3f;
    provider_port1.vxlan_port[0].local_ip4  = net1_local_ip4;
    provider_port1.vxlan_port[0].mcast_ip4  = net1_mcast_ip4;
    provider_port1.vxlan_port[0].mc_tnl_flag = FLAGS_TUNNEL_MC_BUD;
    BCM_IF_ERROR_RETURN(vxlan_tunnel_create(unit, &provider_port1.vxlan_port[0]));

    /* Known L2 Mcast Group config */
    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, CREATE, BCM_MULTICAST_TYPE_FLOW, &mcast_group, 
                            &customer_port, 0));


    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, NOT_CREATE, BCM_MULTICAST_TYPE_FLOW, &mcast_group, 
                            &provider_port1, 0));

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, "01:00:5e:01:01:01", vxlan_vpn);
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mcast_group;
    rv= bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
            printf("\nError in l2 MC entry config: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    /* Default Mcast Group (for bc/non-uc) config */
    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, NOT_CREATE, BCM_MULTICAST_TYPE_FLOW, &vxlan_bcast_group, 
                            &customer_port, 0));

    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, NOT_CREATE, BCM_MULTICAST_TYPE_FLOW, &vxlan_bcast_group, 
                            &provider_port1, 0));

    // Transit Mcast Group config
    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, CREATE, BCM_MULTICAST_TYPE_L3, &mcast_group_transit, 
                            &provider_port2, 0));

    // L3 Ingress Interface config
    bcm_if_t l3_iif;
    rv = create_l3_ingress_interface(unit, net1_vid, &l3_iif);
    if (BCM_FAILURE(rv)) {
        printf("\nError in creating L3 iif: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    bcm_ipmc_addr_t ip_mcast;
    bcm_ipmc_addr_t_init(&ip_mcast);
    ip_mcast.flags          = BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_L3_IPMC;
    ip_mcast.vid            = net1_vid;
    ip_mcast.s_ip_addr      = net1_remote_ip4;
    ip_mcast.mc_ip_addr     = net1_mcast_ip4;
    ip_mcast.mc_ip_mask     = 0xffffffff; // host: all 1's 
    ip_mcast.group = mcast_group_transit;
    ip_mcast.vrf            = 0;
    ip_mcast.ing_intf       = l3_iif; // set for non-subnet     
    ip_mcast.mtu            = 1500;
    ip_mcast.rp_id          = 0;
    rv = bcm_ipmc_add(unit, &ip_mcast);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Mcast config Network: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Verification:
 *  Ingress ipv4 packet is tunneled to a ipv4 vxlan tunnel header
 */
void
verify(int unit)
{
    char str[512];

    bshell(unit, "e; vlan show");
    bshell(unit, "e; l2 show");
    bshell(unit, "e; clear c");
    bshell(unit, "e; multicast show");
    bshell(unit, "e; ipmc table show");
    printf("\nSending known multicast packet to ingress_port network side: port-%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005E00000A00000000000281000016080045000082000040003F1198B7C0A80101E100000AD90312b5006ED6B4080000000123450001005E01010100000000BBBB080045000050000100004011949602020202E101010100350035003C52F574657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);

    bshell(unit, str);
    bshell(unit, "show c");

    /* dump l2 table to see if SA learned againt SVP */
    bshell(unit, "l2 show");

    return;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vxlan())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
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

    print "~~~ #2) config_vxlan(): ** start **";
    if (BCM_FAILURE((rv = config_vxlan(unit)))) {
        printf("config_vxlan() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vxlan(): ** end **";

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
