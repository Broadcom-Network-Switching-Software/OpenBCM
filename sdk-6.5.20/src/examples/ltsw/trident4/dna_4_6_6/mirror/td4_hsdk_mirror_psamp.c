/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */             

/*
 * Feature  : Ingress Mirroring (PSAMP)
 *                                           
 * Usage    : BCM.0> cint td4_hsdk_mirror_psamp.c
 *                                               
 * config   : bcm56880_a0-generic-32x400_config.yml
 *                                                 
 * Log file : td4_hsdk_mirror_psamp_log.txt        
 *                                                 
 * Test Topology :                                 
 #                +---------------------------+    
 #                |                           |    
 #                |                           |
 #                |   BCM56880                |
 #                |                           +------------------>
 # -------------> |                           | egress_port_1
 # ingress_port   |                           |
 #                |                           |------------------>
 #                |                           | egress_port_2 (MTP Port)
 #                |                           |
 #                +---------------------------+
 #
 * Summary:
 * ========
 * Cint example to demonstrate Ingress sampling using PSAMP.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        three ports, one port is used as ingress_port and the other two
 *        egress_port.
 *     b) Configure to Drop ingress packets at egress port 1 and 2
 *     c) Configure "copy to cpu" ingress packets received at egress port 2.
 *
 *   2) Step2 - Configuration (Done in config_psamp_mirror()):
 *   =========================================================
 *     a) enable ingress mirroring on ingress_port and make cpu_port as MTP
 *     b) Set MTP.flag as BCM_MIRROR_DEST_TUNNEL_PSAMP | BCM_MIRROR_DEST_TUNNEL_WITH_SEQ
 *     c) Provide PSAMP encapsulation details in MTP structure members.
 *     d) Create a sample profile and attach it to ingress_port.
 * 
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) start packet watcher to dump packets received on cpu port.
 *     b) Transmit 1 packet to ingress port using tx command 
 *     c) Expected Result:
 *     ===================
 *       We can see the packet is looped back on the ingress port and goes out of egress port 1.
 *       Also, the ingressed packet is mirrored to MTP port (egress port 2).
 *       and the mirrored packet looped back and send to CPU port which is dumped by packet watcher.
 *       Packet Dump Output:
 *       [bcmPWN.0]Packet[2]: data[0000]: {000000000011} {000000000022} 8100 0064
 *       [bcmPWN.0]Packet[2]: data[0010]: 0800 4500 008c 0000 0000 0511 8b38 0a0a
 *       [bcmPWN.0]Packet[2]: data[0020]: 0a0a 0b0b 0b0b 5566 7788 0078 0000 000a
 *       [bcmPWN.0]Packet[2]: data[0030]: 0070 0000 0000 ffee ffee 0000 0078 0028
 *       [bcmPWN.0]Packet[2]: data[0040]: 0060 0000 0000 0000 0000 0000 0000 0001
 *       [bcmPWN.0]Packet[2]: data[0050]: 0002 003d 1234 00ff 0044 00bb bbbb bbbb
 *       [bcmPWN.0]Packet[2]: data[0060]: 0011 2233 4455 8100 0002 0800 4500 001c
 *       [bcmPWN.0]Packet[2]: data[0070]: 0000 0000 4002 f95d c0a8 001e c0a8 0014
 *       [bcmPWN.0]Packet[2]: data[0080]: 1164 ee9b 0000 0000 0001 0203 0405 0607
 *       [bcmPWN.0]Packet[2]: data[0090]: 0809 0a0b 0c0d 0e0f 1011 e544 d79b 
*/

cint_reset();
int ingress_port, egress_port1, egress_port2, cpu_port = 0;
bcm_gport_t mirror_dest_id;
uint32 flags = BCM_MIRROR_PORT_INGRESS;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
*/
int checkPortAssigned(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i= 0; i < no_entries; i++) {
        if (port_index_list[i] == index)
            return 1;
    }

    /* no entry found */
    return 0;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0,rv = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand()% port;
        if (checkPortAssigned(port_index_list, i, index) == 0){
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * This function is pre-test setup.
 */
int test_setup(int unit)
{
    bcm_vlan_t vid = 2;
    uint8 dest_mac[6] = {0x00, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t addr;
    bcm_switch_trace_event_mon_t trace;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;

    int port_list[3];
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];
    printf("Ingress port: %d\n",ingress_port);
    printf("Egress  port: %d\n",egress_port1);
    printf("Mirrored port: %d\n",egress_port2);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, cpu_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);

    /* Create a vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vid));

    /* Set port mac loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port2, lb_mode));

    /* Add a L2 address on front panel port */
    bcm_l2_addr_t_init(&addr, dest_mac, vid);
    addr.port=egress_port1;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    /* Drop packet. */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port1, 0));
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port2, BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_ARL));

    /* Set COPY_TO_CPU action for CML trace event. */
    bcm_switch_trace_event_mon_t_init(&trace);
    trace.trace_event = bcmPktTraceEventIngressCmlFlags;
    trace.actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
    BCM_IF_ERROR_RETURN(bcm_switch_trace_event_mon_set(unit, &trace));
   
    return BCM_E_NONE;
}

/* config psamp mirror */
bcm_error_t
config_psamp_mirror(int unit)
{
    int vid=100;
    int modid;
    bcm_gport_t gport;
    int profile_id;
    bcm_mirror_sample_type_t type;
    bcm_mirror_sample_profile_t profile;
    bcm_mirror_source_t source;    
    
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &modid));
    BCM_GPORT_MODPORT_SET(gport, modid, egress_port2);

    /* Mirror Port */
    bcm_mirror_destination_t mtp;
    bcm_mirror_destination_t_init(&mtp);
    mtp.gport=gport;
    mtp.dst_mac = "00:00:00:00:00:11"; 
    mtp.src_mac = "00:00:00:00:00:22"; 
    mtp.tpid = 0x8100; 
    mtp.vlan_id = vid; 
    mtp.version = 4; 
    mtp.src_addr = 0x0a0a0a0a; 
    mtp.dst_addr = 0x0b0b0b0b; 
    mtp.ttl = 5;
    mtp.udp_src_port = 0x5566;
    mtp.udp_dst_port = 0x7788;
    mtp.initial_seq_number = 0xFFEEFFEE;
    mtp.psamp_epoch = 0x1;
    mtp.ipfix_version = 0xA;
    mtp.template_id = 0x28;
    mtp.meta_data = 0x1234;
    mtp.meta_data_type = bcmMirrorPsampFmt2HeaderUserMeta;    
    mtp.observation_domain = 0x78;

    mtp.flags = BCM_MIRROR_DEST_TUNNEL_PSAMP | BCM_MIRROR_DEST_TUNNEL_WITH_SEQ;  
    
    /* Create Mirror destination. */
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit,&mtp));

    /* Create sample profile. */
    type = bcmMirrorSampleTypeIngress;
    BCM_IF_ERROR_RETURN(bcm_mirror_sample_profile_create(unit, 0, type, &profile_id));

    bcm_mirror_sample_profile_t_init(&profile);
    profile.enable = TRUE;
    profile.rate = 0x1fffffff;
    profile.pool_count_enable = TRUE;
    profile.sample_count_enable = TRUE;
    profile.trace_enable = TRUE;
    profile.mirror_mode = bcmMirrorSampleMirrorModeBelowRate;

    /* Set sample profile. */
    BCM_IF_ERROR_RETURN(bcm_mirror_sample_profile_set(unit, type, profile_id, &profile));
    
    bcm_mirror_source_t_init(&source);
    source.type = bcmMirrorSourceTypeSampleIngress;
    source.sample_profile_id = profile_id;
    BCM_IF_ERROR_RETURN(bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id));
    
    /* Set sample profile id on ingress port. */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlIngressSampleProfile, profile_id));
    return BCM_E_NONE;
}

/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t verify(int unit)
{
    char   str[512];
    uint64 in_pkt, out_pkt;

    /* start packet watcher */ 
    bshell(unit, "pw start");
    bshell(unit, "pw report all");
    
    /* Send pacekt to ingress_port */
    printf("\n******************************\n");
    printf("******Traffic test start******\n");
    printf("Packet comes into ingress port %d, export forward to %d and mirror to %d\n", ingress_port, egress_port1, egress_port2);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00BBBBBBBBBB0011223344558100000208004500001C000000004002F95DC0A8001EC0A800141164EE9B00000000000102030405060708090A0B0C0D0E0F1011E544D79B;sleep 5", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);

    bshell(unit, "show c");
    bshell(unit, "pw stop");

    printf("\n******Port stats check******\n");
    printf("--------Ingress port (%d) stats--------\n", ingress_port);
    COMPILER_64_ZERO(in_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ingress_port, snmpDot1dTpPortInFrames, &in_pkt));
    printf("Rx : %d packets\n", COMPILER_64_LO(in_pkt));

    printf("--------Egress port (%d) stats--------\n", egress_port1);
    COMPILER_64_ZERO(out_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port1, snmpDot1dTpPortOutFrames, &out_pkt));
    printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
    if (COMPILER_64_LO(out_pkt) != 1) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
        return -1;
    }

    printf("--------Mirrored port (%d) stats--------\n", egress_port2);
    COMPILER_64_ZERO(out_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port2, snmpDot1dTpPortOutFrames, &out_pkt));
    printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
    if (COMPILER_64_LO(out_pkt) != 1) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
        return -1;
    }

    return BCM_E_NONE;
}
/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_psamp_mirror())
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

    print "~~~ #2) config_psamp_mirror(): ** start **";
    if (BCM_FAILURE((rv = config_psamp_mirror(unit)))) {
        printf("config_psamp_mirror() failed.\n");
        return -1;
    }
    print "~~~ #2) config_psamp_mirror(): ** end **";

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
