/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Riot Multicast, IPv4 access to network tunnel initiation flow
 *            for IPMC routing and switching 
 *            VxLAN GBP SID added in VxLAN header by IPMC table entry that's hit
 *
 * Usage    : BCM.0> cint td4_hsdk_vxlan_gbp_ipv4_AtoN_mc_l3.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_vxlan_gbp_ipv4_AtoN_mc_l3_log.txt
 *
 * Test Topology :
 *                                  +----------------------+ 	L3 routing ->
 *                                  |                      |   VLAN-22        Underlay-NextHop
 *                                  |                      |  egress_port       +---+
 *   SMAC 00::bb:bb                 |                      +--------------------+   |
 *            +---+          VLAN-21|                      |01:00:5e:02:02:02   +---+
 *   Host(acc)|   +-----------------+      Trident-3       |                    00::02
 *            +---+    ingress_port |                      |
 *                                  |                      |   VLAN-23       Underlay-NextHop
 *                                  |                      |  egress_port       +---+
 *                                  |                      +--------------------+   |       
 *                                  |                      |01:00:5e:02:02:02   +---+
 *                                  |                      |                    00::02
 *                                  |                      |  L2 Switching (No modification of inner header) -->
 *                                  +----------------------+
 *
 *
 * Summary:
 * 
 * Cint example to demonstrate VxLAN Riot tunnel initiation for known multicast traffic -
 *   ipmc route generate two copies, one performs l3 routing to vxlan tunnel, the other performs l2 bridging to vxlan tunnel.
 *   Additionally, by means of IPMC entry that's hit and associated FP rule 
 *   - VxLAN GBP 'G' bit is set in VxLAN header
 *   - VxLAN GBP SID is added in VxLAN header
 *   
 *
 * Detailed steps done in the CINT script:
 * 
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan()):
 *   
 *     a) Configure a VxLAN Bridging configuration with one access port and one network port.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   
 *     a) Check the configurations by 'vlan show', 'l2 show',traversing the vpn and vp's
 *
 *     b) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     
 *       We can see the tunnel terminated packet on the access port and
 *       'show c' to check the Tx/Rx packet stats/counters.
 */
/*
Packet:
Known Multicast case (encap, Access=>Network)

Input (Access):
  ###[ Ethernet ]###
    dst= 01:00:5e:01:01:01
    src= 00:00:00:00:11:11
    type= 0x8100
  ###[ 802.1Q ]###
    vlan= 21
    type= 0x800
  ###[ IP ]###
    version= 4
    proto= udp
    src= 1.1.1.1
    dst= 225.1.1.1
  ###[ UDP ]###

Input Hex:
Known multicast case:
01005E01010100000000111181000015080045000050000040004011569901010101E101010100350035003C54F774657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Egress Packet#1:
Header Info

  ###[ Ethernet ]###
    dst= 01:00:5e:02:02:02
    src= 00:00:00:00:22:22
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 23
     type= 0x800
  ###[ IP ]###
     version= 4
     proto= udp
     src= 10.10.10.1
     dst= 225.2.1.1
  ###[ UDP ]###
     sport= 49152
     dport= 4789
  ###[ VXLAN ]###
     flags= Group, Instance
     Group Policy ID = 4011(0x0FAB) <- GBP SID added
     vni= 0
     reserved2= 0x0
  ###[ Ethernet ]###
                 dst= 01:00:5e:01:01:01
                 src= 00:00:00:00:11:11
                 type= 0x8100
  ###[ 802.1Q ]###
     vlan= 21
     type= 0x8000
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 225.1.1.1
  ###[ UDP ]###

Output (Network_1, L3 Routing):
data[0000]: {01005e020202} {000000002222} 8100 0017
data[0010]: 0800 4500 0086 0000 0000 3f11 8559 0a0a
data[0020]: 0a01 e102 0101 c000 12b5 0072 0000 8800
data[0030]: 0fab 0000 0000 0100 5e01 0101 0000 0000
data[0040]: 1111 8100 0015 0800 4500 0050 0000 4000
data[0050]: 4011 5699 0101 0101 e101 0101 0035 0035
data[0060]: 003c 54f7 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 7465 7374 

Egress Packet#2:
Header Info

  ###[ Ethernet ]###
    dst= 01:00:5e:02:02:02
    src= 00:00:00:00:22:22
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 22
     type= 0x800
  ###[ IP ]###
     version= 4
     proto= udp
     src= 10.10.10.1
     dst= 225.2.1.2
  ###[ UDP ]###
     sport= 49152
     dport= 4789
  ###[ VXLAN ]###
     flags= Group, Instance
     Group Policy ID = 4011(0x0FAB) <- GBP SID added
     vni= 0
     reserved2= 0x0
  ###[ Ethernet ]###
                 dst= 01:00:5e:01:01:01
                 src= 00:00:00:00:0a:0a
                 type= 0x8100
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 225.1.1.1
  ###[ UDP ]###

Output (Network_2, L2 Bridging):
data[0000]: {01005e020202} {000000002222} 8100 0016
data[0010]: 0800 4500 0082 0000 0000 3f11 845c 0a0a
data[0020]: 0a01 e102 0202 c000 12b5 006e 0000 8800
data[0030]: 0fab 0000 0000 0100 5e01 0101 0000 0000
data[0040]: 0a0a 0800 4500 0050 0000 4000 3f11 5799
data[0050]: 0101 0101 e101 0101 0035 0035 003c 54f7
data[0060]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 

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
    BCM_FIELD_QSET_ADD(&group_config->qset, bcmFieldQualifyInPort); 
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
ingress_port_setup(int unit, bcm_port_t port,
                    bcm_field_group_config_t group_config,
                    bcm_field_entry_t *entry)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, *entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionCopyToCpu, 0, 0));
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
egress_port_setup(int unit, bcm_port_t port,
                    bcm_field_group_config_t group_config,
                    bcm_field_entry_t *entry)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, *entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, *entry));

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
    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port, group_config, &entry[0])) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    for (i = IN_PORT_NUM; i < (IN_PORT_NUM + OUT_PORT_NUM); i++){   
        egress_port[i - IN_PORT_NUM] = port_list[i];
        if (BCM_E_NONE != egress_port_setup(unit, egress_port[i - IN_PORT_NUM], group_config, &entry[i])) {
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
	bcm_if_t		ol_intf_id;
	bcm_if_t		ol_egr_id;

    /* Specific for Network port */
    bcm_flow_encap_criteria_t criteria;    
    bcm_ip_t        remote_ip;
    bcm_ip_t        local_ip;    
    bcm_ip6_t       remote_ip6;
    bcm_ip6_t       local_ip6;
    int             ttl;
    uint32          vnid;
    int             dvp_group;
    bcm_ip_t        mcast_ip;
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
create_l3_ingress_interface(int unit, bcm_vlan_t vid, bcm_vrf_t vrf, bcm_if_t *l3_iif)
{
    bcm_error_t             rv = BCM_E_NONE; 
    bcm_l3_ingress_t        l3_ingress; 
    bcm_vlan_control_vlan_t vlan_ctrl;  

    printf("Create network L3 ingress interface\n");
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.vrf = vrf;
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
        match_info.sip          = v_port->remote_ip;
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
    tiinfo.valid_elements   |=  BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID;
    tiinfo.udp_dst_port     = VXLAN_UDP_DST_PORT;
    if (flow_flag == FLAGS_FLOW_CLASSIC) {
        tiinfo.type     = bcmTunnelTypeVxlan;
        tiinfo.sip      = v_port->local_ip;
        if (v_port->mc_tnl_flag) {
            tiinfo.dip  = v_port->mcast_ip;
        } else {
            tiinfo.dip  = v_port->remote_ip;
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
            ttinfo.dip = v_port->mcast_ip;
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
							bcm_if_t ol_l3_intf_id, vxlan_phy_port_s *vxlan_port, int vxlan_port_idx)
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
            mc_encap.l3_intf        = ol_l3_intf_id;  
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

/* FP rule for Adding GBP SID in Outgoing packet's VxLAN header */
bcm_error_t
config_fp_vxlan_gbp_l3_classid(int unit, uint16 class_id)
{
    bcm_field_qset_t qual;
    int entry_id;

    BCM_FIELD_QSET_INIT(qual);
    BCM_FIELD_QSET_ADD(qual,bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(qual,bcmFieldQualifyStageIngress);

    bcm_field_aset_t action;
    BCM_FIELD_ASET_INIT(action);
    BCM_FIELD_ASET_ADD(action,bcmFieldActionVxlanGbpEnable);
    BCM_FIELD_ASET_ADD(action,bcmFieldActionAssignVxlanGbpId);

    bcm_field_group_config_t gcfg1;
    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.aset= action;
    gcfg1.qset= qual;
    gcfg1.aset= action;
    gcfg1.priority= 3;
    gcfg1.mode= bcmFieldGroupModeAuto;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, gcfg1.group, &entry_id));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstClassL3(unit, entry_id, class_id, 0xFFFF)); /* Qualify class ID */

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_id, bcmFieldActionVxlanGbpEnable, 0, 0)); /* Set G bit */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_id, bcmFieldActionAssignVxlanGbpId, class_id, 0)); /*i Assign GBP SID */

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_id));

    return BCM_E_NONE;
}

/************************** Main Routine ****************************/
bcm_vpn_t           vxlan_vpn = 0x8064;
bcm_vpn_t			vxlan_vpn_acc2 = 0x8065;
bcm_vpn_t			vxlan_vpn_net1 = 0x8066;

vxlan_phy_port_s    customer_port, provider_port1, provider_port2;

/* Access Port 1 Coinfigurations */
bcm_vlan_t      acc_vid = 21;
bcm_mac_t       acc_local_mac = "00:00:00:00:11:11";
bcm_mac_t       acc_remote_mac = "00:00:00:00:00:01";
bcm_ip_t        acc_local_ip   = 0x01010101;  /* 1.1.1.1 */


/* Network Port 1 Coinfigurations */
bcm_vlan_t      net1_vid = 22;
bcm_mac_t       net1_local_mac = "00:00:00:00:22:22";
bcm_mac_t       net1_remote_mac = "01:00:5e:02:02:02";
bcm_ip_t        net1_remote_ip  = 0x0a0a0a02;  /* 10.10.10.2 */
bcm_ip_t        net1_local_ip   = 0x0a0a0a01;  /* 10.10.10.1 */
bcm_ip_t        net1_mcast_ip   = 0xe1020202;  /* 225.2.2.2 */
int             net1_dvp_group  = 3010;
bcm_vlan_t		net1_ol_vid = 0xa;
bcm_mac_t		net1_ol_mac = "00:00:00:00:0a:0a";
bcm_if_t 		net1_ol_l3_intf_id = 0;

/* Network Port 2 Coinfigurations */
bcm_vlan_t      net2_vid = 23;
bcm_mac_t       net2_local_mac = "00:00:00:00:22:22";
bcm_mac_t       net2_remote_mac = "01:00:5e:02:02:02";
bcm_ip_t        net2_remote_ip  = 0x0a0a0a03;  /* 10.10.10.3 */
bcm_ip_t        net2_local_ip   = 0x0a0a0a01;  /* 10.10.10.1 */
bcm_ip_t        net2_mcast_ip   = 0xe1020101;  /* 225.2.1.1 */
int             net2_dvp_group  = 3011;
bcm_vlan_t		net2_ol_vid = 0xb;
bcm_mac_t		net2_ol_mac = "00:00:00:00:0b:0b";
bcm_if_t		net2_ol_l3_intf_id = 0;
bcm_flow_encap_criteria_t criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP;

bcm_vrf_t 		vrf = 1; /* Routing VRF */

uint32          vnid = 0x12345; 
uint16          vxlan_gbp_sid = 0x0FAB; /*GBP SID to be added*/
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
    BCM_IF_ERROR_RETURN(vxlan_port_create(unit, my_modid, ingress_port,    FLAGS_CUSTOMER, &customer_port));    
	BCM_IF_ERROR_RETURN(vxlan_port_create(unit, my_modid, egress_port[0],  FLAGS_PROVIDER, &provider_port1));
    BCM_IF_ERROR_RETURN(vxlan_port_create(unit, my_modid, egress_port[1],  FLAGS_PROVIDER, &provider_port2));
    

    /* Multicast group for non-UC */
    BCM_IF_ERROR_RETURN( bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &vxlan_bcast_group));

    /* Create VXLAN VPN */
    BCM_IF_ERROR_RETURN( vxlan_vpn_create(unit, vxlan_bcast_group, vxlan_vpn));
	BCM_IF_ERROR_RETURN( vxlan_vpn_create(unit, vxlan_bcast_group, vxlan_vpn_acc2));
	BCM_IF_ERROR_RETURN( vxlan_vpn_create(unit, vxlan_bcast_group, vxlan_vpn_net1));

    /* Create VXLAN VP for access ports */
    customer_port.vxlan_port[0].vid = acc_vid;
	customer_port.vxlan_port[0].local_ip = acc_local_ip;
    BCM_IF_ERROR_RETURN(vxlan_access_port_create(unit, vxlan_vpn, &customer_port));
	
	/* Create L3 IIF for access VP */
	bcm_if_t l3_iif;
    BCM_IF_ERROR_RETURN(create_l3_ingress_interface(unit, vxlan_vpn, vrf, &l3_iif));

    /* Create VXLAN VP for network port 1 */
    provider_port1.vxlan_port[0].vid        = net1_vid;
    provider_port1.vxlan_port[0].local_mac  = net1_local_mac;
    provider_port1.vxlan_port[0].remote_mac = net1_remote_mac;
    provider_port1.vxlan_port[0].criteria   = criteria;
    provider_port1.vxlan_port[0].vnid       = vnid;
    provider_port1.vxlan_port[0].dvp_group  = net1_dvp_group;
    provider_port1.vxlan_port[0].remote_ip  = net1_remote_ip;
    BCM_IF_ERROR_RETURN(vxlan_network_port_create(unit, vxlan_vpn_net1, &provider_port1));

    /* Create init & term tunnel for network port 1*/
    provider_port1.vxlan_port[0].ttl        = 0x3f;
    provider_port1.vxlan_port[0].local_ip   = net1_local_ip;
    provider_port1.vxlan_port[0].mcast_ip   = net1_mcast_ip;
    provider_port1.vxlan_port[0].mc_tnl_flag = FLAGS_TUNNEL_MC;
    BCM_IF_ERROR_RETURN(vxlan_tunnel_create(unit, &provider_port1.vxlan_port[0]));
	
	/* Create OL L3 INTF for network port 1, routed copy*/
	printf ("Create Overlay L3 Interface for network port 1\n");
	BCM_IF_ERROR_RETURN(vxlan_l3_intf_create(unit, net1_ol_vid, net1_ol_mac, 0, 0, &net1_ol_l3_intf_id));
	 
		
    /* Create VXLAN VP for network port 2 */
    provider_port2.vxlan_port[0].vid        = net2_vid;
    provider_port2.vxlan_port[0].local_mac  = net2_local_mac;
    provider_port2.vxlan_port[0].remote_mac = net2_remote_mac;
    provider_port2.vxlan_port[0].criteria   = criteria;
    provider_port2.vxlan_port[0].vnid       = vnid;
    provider_port2.vxlan_port[0].dvp_group  = net2_dvp_group;
    provider_port2.vxlan_port[0].remote_ip  = net2_remote_ip;
    BCM_IF_ERROR_RETURN(vxlan_network_port_create(unit, vxlan_vpn, &provider_port2));
    
    /* Create init & term tunnel for network port 2*/
    provider_port2.vxlan_port[0].ttl        = 0x3f;
    provider_port2.vxlan_port[0].local_ip   = net2_local_ip;
    provider_port2.vxlan_port[0].mcast_ip   = net2_mcast_ip;
    provider_port2.vxlan_port[0].mc_tnl_flag = FLAGS_TUNNEL_MC;
    BCM_IF_ERROR_RETURN(vxlan_tunnel_create(unit, &provider_port2.vxlan_port[0]));
	
	/* Create OL L3 INTF for network port 2, switched copy*/
	printf ("Create Overlay L3 Interface for network port 2\n");
	bcm_l3_intf_t l3_intf;
	bcm_l3_intf_t_init(&l3_intf);
	l3_intf.l3a_flags |= BCM_L3_L2ONLY;
	sal_memcpy(l3_intf.l3a_mac_addr, net2_ol_mac, sizeof(net2_ol_mac));
	l3_intf.l3a_vid = net2_ol_vid;
	BCM_IF_ERROR_RETURN(bcm_l3_intf_create(0, &l3_intf));
	net2_ol_l3_intf_id = l3_intf.l3a_intf_id;
	
	
	printf("Create a l3 mc group\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &mcast_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
    /* Known L3 Mcast Group config */
    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, NOT_CREATE, BCM_MULTICAST_TYPE_FLOW, &mcast_group, 
                            0, &customer_port, 0));
	

    BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, NOT_CREATE, BCM_MULTICAST_TYPE_FLOW, &mcast_group, 
                            net1_ol_l3_intf_id, &provider_port1, 0));
							
	BCM_IF_ERROR_RETURN(vxlan_multicast_port_add(unit, NOT_CREATE, BCM_MULTICAST_TYPE_FLOW, &mcast_group, 
                            net2_ol_l3_intf_id, &provider_port2, 0));
	
	/* Add IPMC entry */
	printf("Add IPMC entry\n");
    bcm_ipmc_addr_t ipmc;
	bcm_ipmc_addr_t_init(&ipmc);
	ipmc.flags = 0;
	ipmc.s_ip_addr = customer_port.vxlan_port[0].local_ip;
	ipmc.mc_ip_addr = provider_port1.vxlan_port[0].mcast_ip;
	ipmc.vrf = vrf;
	ipmc.port_tgid = customer_port.port_id;
    ipmc.mod_id = customer_port.modid;
    ipmc.group = mcast_group;
    ipmc.lookup_class = vxlan_gbp_sid; /*Set GBP-SID as class ID*/
	//print ipmc;

    BCM_IF_ERROR_RETURN (bcm_ipmc_add(unit, &ipmc)); 

    rv = config_fp_vxlan_gbp_l3_classid(unit, vxlan_gbp_sid);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_fp_vxlan_gbp_l3_classid: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


/*
 * Verification:
 *   In Packet:
 *   ----------
 *  01005E010101000000001111
 *  810000150800450000500000
 *  40004011569901010101E101
 *  010100350035003C54F77465
 *  737474657374746573747465
 *  737474657374746573747465
 *  737474657374746573747465
 *  7374746573747465737474657374
 *
 *   Out Packet:
 *   -----------
 *  01005e02 02020000 00002222 81000016
 *  08004525 00820000 00003f11 84370a0a
 *  0a01e102 0202ffff 2118006e 00000000
 *  00000123 45000100 5e010101 00000000
 *  11118100 00150800 45000050 00004000
 *  40115699 01010101 e1010101 00350035
 *  003c54f7 74657374 74657374 74657374
 *  74657374 74657374 74657374 74657374
 *  74657374 74657374 74657374 74657374
 *  74657374 ae10d5d7
 */
void
verify(int unit)
{
    char str[1184];

    bshell(unit, "e; vlan show");
    bshell(unit, "e; l2 show");
    bshell(unit, "e; clear c");
    bshell(unit, "e; multicast show");
    bshell(unit, "e; ipmc table show");
    printf("\nSending known multicast packet to ingress_port access side:%d\n", ingress_port);
    snprintf(str, 1184, "tx 1 pbm=%d data=0x01005E01010100000000111181000015080045000050000040004011569901010101E101010100350035003C54F774657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
    bshell(unit, str);
    sal_sleep(2);
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
