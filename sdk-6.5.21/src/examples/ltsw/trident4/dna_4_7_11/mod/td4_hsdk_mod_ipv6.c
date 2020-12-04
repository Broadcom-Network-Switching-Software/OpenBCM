/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Mirror-On-Drop, packets dropped in MMU are able to be mirrored to an MTP and encapsulated with an IPv6 PSAMP header.
 *
 * Usage    : BCM.0> cint TD4_HSDK_MOD_ipv6.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_MOD_ipv6_log.txt
 *
 * Test Topology :
 *
 *                                +----------------------+      
 *                                |                      |                             
 *                                |                      |                              Monitor
 * SMAC 00::bb:bb                 |                      |  MTP (VLAN=200)              +---+
 *          +---+          VLAN=1 |                      +------------------------------+   |
 * Host.    |   +-----------------+      Trident4        |                              +---+  
 *          +---+    ingress_port |                      |                              09::99
 *                                |                      |           
 *                                |                      |                              Host
 *                                |                      |  egress port                 +---+         
 *                                |                      +------------------------------+   |
 *                                |                      |                              +---+  
 *                                |                      |                              aa::aa
 *                                +----------------------+
 * Summary:
 * ========
 * Cint example to demonstrate MOD configuration for IPv6 PSAMP Initiation -
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects five ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other one as
 *        egress_port, the others are used to generate the storm traffic for the test.
 *
 *     b) Install an IFP rule to copy egress packets to CPU.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_mod()):
 *   =========================================================
 *     a) Configure an MOD configuration.
 *        This does the necessary configurations of MOD global settings, vlan,
 *        port setting, PSAMP tunnel setup. 
 *
 *     b) Install an IFP rule to enalbe MOD on the ingress port.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     (a) Configure ports in loop (snake-test-like setting) via mod_storm_generator()
 * 
 *     (b) Check the configurations by 'vlan show'
 *
 *     (c) Transmit 1 known unicast packets to the loop ports and generate line rate 
 *         traffic to cause MMU drop.
 *
 *     (d) Start PacketWatcher and log some packets. Dump one packet to see whether the IPv6
 *         PSMAP header is encasuplated or not.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the tunnel packet on the egress port with the IPv6 PSAMP encapsulation.
 *       The Ingress port, egress port, drop reasons can be found in the PSAMP header.
 */

/*
Drop reason encoding:
  1 = Ingress Limit 
  2 = Shared Pool Limit 
  3 = Egress port SP Shared Limit
  4 = Egress queue Shared Limit
  5 = Egress WRED drop 
*/
/*


Packet:

NETWORK-TO-NETWORK-:-
Decap-Encap-case: packet sent from ingress_port(network) to egress_port(network)


Ingress Packets: (No. of packets = 7)
======

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00:00:aa:aa
    src= 00:00:00:00:bb:bb
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 1
     type= 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 2.2.2.2
  ###[ UDP ]###
          sport= 53
          dport= 53

Ingress Hex Packet:
tx port=1 data=0x00000000AAAA00000000BBBB810000010800450000500001000040117497010101010202020200350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Egress Packet:
======

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00_00:09:99
    src= 00:00:00_00:06:66
    type= 0x9100
  ###[ 802.1Q ]###
     vlan= 200
     type= 0x800
  ###[ IPV6 ]###
     version= 6
     NextHheader= udp(17)
     src= 101:101:3500:7000:0:1009:807:605
     dst= 202:202:3600:7000:0:102:304:506
  ###[ UDP ]###
     sport= 0x1234
     dport= 0x4321
     len= 146
     checksum= 0
  ###[ IPFIX ]###     
     IPFIX Verions= 0x0a
     len= 0x8a
     seq num=0x47
     Domain id= 0x64 (BCM_MIRROR_IPFIX_OBSERVATION_DOMAIN_DFLT)
  ###[ PSAMP ]###     
     PSAMP Template ID= 0x1234 (BCM_MIRROR_IPFIX_TEMPLATE_ID_DFLT)
     len= 0x7a
     Egress port of the dropped packet= 0x14(20)
     Ingress port= 1
     Drop reason= 0x9 >> 1 = 4 (Egress queue Shared Limit)
     variable flag= 0xff
  ###[ Ethernet ]###
                 dst= 00:00:00:00:aa:aa
                 src= 00:00:00:00:bb:bb
                 type= 0x800
  ###[ IPv4 ]###
        version= 4

Egress Hex Dump:
data[0000]: {000000000999} {000000000666} 8100 0001
data[0010]: 9100 00c8 86dd 6020 0000 0092 1110 0101 
data[0020]: 0101 3500 7000 0000 1009 0807 0605 0202
data[0030]: 0202 3600 7000 0000 0102 0304 0506 1234
data[0040]: 4321 0092 0000 000a 008a 0000 0000 0000
data[0050]: 0047 0000 0064 1234 007a 0000 0000 0000
data[0060]: 0000 0000 0000 0014 0001 0009 0000 00ff
data[0070]: 005e 0000 0000 aaaa 0000 0000 bbbb 0800
data[0080]: 4500 0050 0001 0000 4011 7497 0101 0101
data[0090]: 0202 0202 0035 0035 003c 32f6 7465 7374
data[00a0]: 7465 7374 7465 7374 7465 7374 7465 7374
data[00b0]: 7465 7374 7465 7374 7465 7374 7465 7374
data[00c0]: 7465 7374 7465 7374 7465 7374 7465 7374


*/

/* Reset C interpreter*/
cint_reset();

/**** global constants ****/
const int   IN_PORT_NUM = 5;   // 0: test port; 1~3: storm traffic generator; 4: UC dest port
const int   OUT_PORT_NUM = 1;  // MTP

bcm_port_t ingress_port[IN_PORT_NUM];
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
    BCM_FIELD_ASET_ADD(group_config->aset, bcmFieldActionMirrorOnDropEnable);
    BCM_FIELD_ASET_ADD(group_config->aset, bcmFieldActionMirrorOnDropProfileSet);        
    group_config->mode = bcmFieldGroupModeAuto;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, group_config));

    return BCM_E_NONE;
}


/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule MOD the packets ingressing
 * on the specified port to MTP.
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port,
                    bcm_field_group_config_t group_config,
                    bcm_field_entry_t *entry, int profile_id)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, *entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionMirrorOnDropEnable, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, bcmFieldActionMirrorOnDropProfileSet, profile_id, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, *entry));
    printf("Enabling MOD on port %d with MOD profile %d...done! \n", port, profile_id);

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified set of ports to CPU. Ports are also configured
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
bcm_field_group_config_t group_config;
bcm_error_t
test_setup(int unit)
{
    int i;
    int port_list[IN_PORT_NUM + OUT_PORT_NUM];
    bcm_field_entry_t entry[IN_PORT_NUM + OUT_PORT_NUM];
    
    if (BCM_E_NONE != portNumbersGet(unit, port_list, IN_PORT_NUM + OUT_PORT_NUM)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    if (BCM_E_NONE !=field_group_setup(unit, &group_config)){
        printf("field_group_setup() failed to create field group\n");
        return -1;
    }

    for (i = 0; i < IN_PORT_NUM; i++){   
        ingress_port[i] = port_list[i];
        // install the fp rule for ingress port after creating MOD profile
    }
    for (i = IN_PORT_NUM; i < (IN_PORT_NUM + OUT_PORT_NUM); i++){   
        egress_port[i - IN_PORT_NUM] = port_list[i];
        if (BCM_E_NONE != egress_port_setup(unit, egress_port[i - IN_PORT_NUM], group_config, &entry[i])) {
            printf("egress_port_setup(port2) failed for port %d\n", egress_port[i - IN_PORT_NUM]);
            return -1;
        }
    }

    return BCM_E_NONE;
}


/************************** MOD Sub-Routines ****************************/

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port, int untagged)
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
    if (untagged) {
        BCM_PBMP_PORT_ADD(upbmp, port); 
    }
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

bcm_error_t
vlan_remove_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);  
    return bcm_vlan_port_remove(unit, vid, pbmp);
}

/* Add L2 Entry */
bcm_error_t
mod_l2_add(int unit, bcm_mac_t mac, bcm_vlan_t vid, int port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vid);
    l2_addr.flags   = BCM_L2_STATIC;
    l2_addr.port    = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding static L2 entry: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


bcm_error_t 
mod_wred_setup(int unit, int egr_port) 
{
    bcm_error_t rv = BCM_E_NONE;    
    bcm_cosq_gport_discard_t discard_cfg; 

    bcm_cosq_gport_discard_t_init(&discard_cfg);
    discard_cfg.flags = BCM_COSQ_DISCARD_ENABLE |            /*WRED_EN=1 */
                        BCM_COSQ_DISCARD_BYTES|
                        BCM_COSQ_DISCARD_COLOR_ALL |
                        BCM_COSQ_DISCARD_PORT;        
    discard_cfg.gain                = 15;
    discard_cfg.refresh_time        = 10;
    discard_cfg.min_thresh          = 1*256;    /* Queue depth in bytes to begin dropping. */
    discard_cfg.max_thresh          = 4*256;    /* Queue depth in bytes to drop all packets */
    discard_cfg.drop_probability    = 100;      /*Drop probability at max threshold*/
    rv = bcm_cosq_gport_discard_set(unit, egr_port, -1, &discard_cfg);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_discard_set~Resp-protocol returned '%s'\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
mod_stat_print(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    uint64_t value;
    // CTR_TM_MIRROR_ON_DROP_BUFFER_POOL (QUEUE_PKT/ DROP_PKT/ CELL_USAGE/ BUFFER_POOL (key))
    // bcmCosqModStatPoolBytesCurrent   -> CELL_USAGE
    // bcmCosqModStatDroppedPackets     -> DROP_PKT
    // bcmCosqModStatEnqueuedPackets    -> QUEUE_PKT
    //  rv = bcm_cosq_mod_stat_get(unit, bcm_cosq_object_id_t *id, bcm_cosq_mod_stat_t stat,
    //                   uint64_t *value)

    return rv;    
}

const int FLAGS_MOD_ENCAP_IPV4 = 0x1;
const int FLAGS_MOD_ENCAP_IPV6 = 0x2;

bcm_error_t 
mod_mirror_destination_create(int unit, int flags, bcm_gport_t mirror_dest_gport,
                                             bcm_gport_t *mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mirror_destination_t mirror_dest_info;

    /* IPv4 related parameters */
    bcm_mac_t src_mac = {00, 00, 00, 00, 0x06, 0x66};
    bcm_mac_t dst_mac = {00, 00, 00, 00, 0x09, 0x99};
    uint16 tpid = 0x9100;
    bcm_vlan_t vid = 200;
    bcm_ip_t src_ip = 0x06060606;
    bcm_ip_t dst_ip = 0x09090909;
    bcm_ip6_t src_ip6 = {0x1,1,1,1,0x35,0,0x70,0,0,0,0x10,0x9,8,7,6,5};
    bcm_ip6_t dst_ip6 = {0x2,2,2,2,0x36,0,0x70,0,0,0,0x1,0x2,3,4,5,6};   
    uint16 udp_src_port = 0x1234;
    uint16 udp_dst_port = 0x4321;
    
    bcm_mirror_destination_t_init(mirror_dest_info);
    mirror_dest_info.gport = mirror_dest_gport;
    mirror_dest_info.flags = BCM_MIRROR_DEST_TUNNEL_WITH_SEQ;
    mirror_dest_info.initial_seq_number = 10;
    mirror_dest_info.flags2 = BCM_MIRROR_DEST_FLAGS2_MOD_TUNNEL_PSAMP;
    sal_memcpy(mirror_dest_info.src_mac, src_mac, 6);
    sal_memcpy(mirror_dest_info.dst_mac, dst_mac, 6);
    mirror_dest_info.vlan_id = vid;
    mirror_dest_info.tpid = tpid;
    mirror_dest_info.version = 4;
    mirror_dest_info.src_addr = src_ip;
    mirror_dest_info.dst_addr = dst_ip;
    mirror_dest_info.ttl = 16;
    mirror_dest_info.udp_src_port = udp_src_port;
    mirror_dest_info.udp_dst_port = udp_dst_port;

    if (flags == FLAGS_MOD_ENCAP_IPV6) {
        mirror_dest_info.version = 6;
        mirror_dest_info.src6_addr = src_ip6;
        mirror_dest_info.dst6_addr = dst_ip6;     
    }


    rv = bcm_mirror_destination_create(unit, mirror_dest_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring mirror destination: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Creating mirror destination 0x%x...done!\n", mirror_dest_info.mirror_dest_id);
    *mirror_dest_id = mirror_dest_info.mirror_dest_id;

    return rv;
}

bcm_error_t 
mod_mirror_destination_add(int unit, bcm_gport_t mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mirror_source_t source;

    // TM_MIRROR_ON_DROP_ENCAP_PROFILE (MIRROR_ENCAP_ID/ TM_MIRROR_ON_DROP_ENCAP_PROFILE_ID(key) = 0 only one entry)
    // TM_MIRROR_ON_DROP_DESTINATION (UC_Q/ PORT_ID/ TM_MIRROR_ON_DROP_DESTINATION_ID(key) = 0)
    bcm_mirror_source_t_init(&source);
    source.type = bcmMirrorSourceTypeCosqMod;
    rv = bcm_mirror_source_dest_add(unit, &source, mirror_dest_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding source-to-destination for MOD: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Adding source-to-destination for MOD...done!\n");

    return rv;
}

bcm_error_t
mod_init(int unit, int *profile_id)
{
    bcm_error_t rv = BCM_E_NONE;
    // TM_MIRROR_ON_DROP_CONTROL (RESERVED_LIMIT_CELLS/ MIRROR_ON_DROP)
    bcm_cosq_mod_control_t control;

    bcm_cosq_mod_control_t_init(&control);
    control.valid_flags = BCM_COSQ_MOD_CONTROL_ENABLE_VALID |
                          BCM_COSQ_MOD_CONTROL_POOL_LIMIT_VALID;
    control.enable = 1;
    control.pool_limit = (0x100 * 256); // in bytes (256 bytes/cell)
    rv = bcm_cosq_mod_control_set(unit, &control);
    if (BCM_FAILURE(rv)) {
        printf("\nError in enabling MOD: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Enabling MOD...done!\n");

    // TM_MIRROR_ON_DROP_PROFILE (PERCENTAGE_0_25/ PERCENTAGE_25_50/ PERCENTAGE_50_75/ PERCENTAGE_75_100/ TM_MIRROR_ON_DROP_PROFILE_ID (key))
    bcm_cosq_mod_profile_t profile;

    rv = bcm_cosq_mod_profile_create(unit, 0, profile_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in creating MOD profile: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Creating MOD prifile %d...done!\n", *profile_id);

    bcm_cosq_mod_profile_t_init(&profile);
    profile.percent_0_25    = 0x1; // sample if a 16-bit random number is less than the 16-bit number
    profile.percent_25_50   = 0x2;
    profile.percent_50_75   = 0x3;
    profile.percent_75_100  = 0x4; // sample packets in (4/0xFFFF) % probability when buffer usage = 75%~100%
    profile.enable = 1;
    rv = bcm_cosq_mod_profile_set(unit, *profile_id, &profile);
    if (BCM_FAILURE(rv)) {
        printf("\nError in setting MOD profile: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Setting MOD profile...done!\n");

    return rv;
}

bcm_error_t
mod_storm_generator(int unit)
{
    int i, untagged = 1;
    bcm_mac_t mac = "00:00:00:00:AA:AA";
    // VLAN-1: untagged port 1, 2, 3    (port 1 PVID = 2)
    // VLAN-2: untagged port 0, 1       (port 0 PVID = 3)
    // VLAN-3: untagged port 0, 4       (port 4 PVID = 4)
    // VLAN-4: untagged port 4
    // storm -> port 1 -> PVID 2 -> port 0 -> L2 lookup(DA, vlan-3) + FP actions -> port 4 (rate limit) -> drop
    // MOD to -> port 5
    for (i = 1; i < IN_PORT_NUM; i++){
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port[i], BCM_PORT_LOOPBACK_MAC));
    }
    BCM_IF_ERROR_RETURN(vlan_remove_port(unit, 1, 0)); // CPU
    BCM_IF_ERROR_RETURN(vlan_remove_port(unit, 1, ingress_port[0]));
    BCM_IF_ERROR_RETURN(vlan_remove_port(unit, 1, ingress_port[4]));
    BCM_IF_ERROR_RETURN(vlan_remove_port(unit, 1, egress_port[0]));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port[1], 2));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, 2, ingress_port[0], untagged));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, 2, ingress_port[1], untagged));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port[0], 3));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, 3, ingress_port[0], untagged));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, 3, ingress_port[4], untagged));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port[4], 4));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, 4, ingress_port[4], untagged));
    BCM_IF_ERROR_RETURN(mod_l2_add(unit, mac, 3, ingress_port[4]));
    BCM_IF_ERROR_RETURN(bcm_port_rate_egress_set(unit, ingress_port[4], 10000, 8192));
    return BCM_E_NONE;
}

bcm_error_t
config_mod(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t mtp_gport = BCM_GPORT_INVALID;
    bcm_gport_t mirror_dest_id = BCM_GPORT_INVALID;  
    int         profile_id;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(mod_init(unit, &profile_id));

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port[0], group_config, &entry, profile_id)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port[0]);
            return -1;
    }

    BCM_GPORT_MODPORT_SET(mtp_gport, 0, egress_port[0]);
    BCM_IF_ERROR_RETURN(mod_mirror_destination_create(unit, FLAGS_MOD_ENCAP_IPV6, mtp_gport, &mirror_dest_id));

    BCM_IF_ERROR_RETURN(mod_mirror_destination_add(unit, mirror_dest_id));
    
    return rv;
}


void
verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    char str[512];

    rv = mod_storm_generator(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in configuring mod_storm_generator: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bshell(unit, "e; vlan show");
    bshell(unit, "e; clear c");
    printf("\nSending known unicast packet to ingress_port side: %d\n", ingress_port[2]);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA00000000BBBB810000010800450000500001000040117497010101010202020200350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port[2]);
    bshell(unit, str);
    sal_sleep(2);
    bshell(unit, "show c");

    bshell(unit, "pw start quiet log 2"); // "log" instead of "report" 
    bshell(unit, "pw dump options +pmd +raw");
    sal_sleep(1);    
    bshell(unit, "pw dump 1");

    printf("\nLT Table Dump (pkt/byte drops on egress port):\n");
    snprintf(str, 512, "bsh -c 'lt CTR_TM_UC_Q_DROP lookup port_id=%d buffer_pool=0 tm_uc_q_id=0'", ingress_port[4]);    
    bshell(unit, str);

    return;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_mod())
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

    print "~~~ #2) config_mod(): ** start **";
    if (BCM_FAILURE((rv = config_mod(unit)))) {
        printf("config_mod() failed.\n");
        return -1;
    }
    print "~~~ #2) config_mod(): ** end **";

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
