/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Trace Event, copy 'Ingress Pipeline: Station Move for Static L2 Source address' packets to CPU.
 *
 * Usage    : BCM.0> cint TD4_HSDK_TraceEvent_ip_copy_to_cpu.c
 *
 * Config   : bcm56880_a0-generic-32x400_event.config.yml
 *
 * Log file : TD4_HSDK_TraceEvent_ip_copy_to_cpu_log.txt
 *
 * Test Topology :
 *
 *
 *                     VLAN=22   +----------------------+          VLAN=22              +---+
 *                               |                      +-------------------------------+   |
 *                               |                      |egress_port[0]                 +---+
 *                               |                      |        00::02
 *         +---+                 |                      |                               +---+
 *         |   +-----------------+                      +-------------------------------+   |
 *         +---+    ingress_port |      Trident-4       |egress_port[1]                 +---+
 *        00::01                 |                      |
 *                               |                      |
 *                               |                      |
 *                               |                      |
 *                               |                      |                               +---+
 *                               |                      +-------------------------------+   |
 *                               |                      |mirror_to_port                 +---+
 *                               |                      |
 *                               |                      |
 *                               +----------------------+
 *
 * Summary:
 * ========
 * CINT example to demonstrate "Trace Event" feature by copying 'Station Move for
 * Static L2 Source address' packets to CPU.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects four ports and configure them in Loopback mode. Out of these four
 *        ports, one port is used as ingress_port and two ports are used as egress_
 *        ports. All of these 3 ports are in the same VLAN=22. The remaining one
 *        port is used as mirror-to port.
 *
 *     b) Install an IFP rule to copy mirrored packets to CPU and start packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature
 *     configuration.
 *
 *   2) Step2 - Configuration (Done in config_event_action()):
 *   =========================================================
 *     a) Configure the "Trace Event" configuration.
 *        This does the necessary configurations of the "Trace Event" settings, VLAN,
 *        and L2 FDB.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show' and 'l2 show'.
 *
 *     b) Transmit one packet with DA=00:00:00:aa:bb:cc & SA=00:00:00:00:00:01 and
 *        one packet with DA=00:00:00:aa:bb:cc & SA=00:00:00:00:00:02 to ingress port.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that the first packet is flooded into other VLAN domain (egress_ports).
 *       For the second packet, it is dropped but copied to CPU due to 'Station Move for
 *       Static L2 Source address'.
 *       Also, 'show counters' shows the TX/RX information of packet forwarding.
 */

/*
Packets:

Ingress Packets: (No. of packets = 2)
======

  ###[ Ethernet ]###
    dst= 00:00:00:aa:bb:cc
    src= 00:00:00:00:00:01 / 00:00:00:00:00:02
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 22
     type= 0x800
     ......
     ......

Ingress Hex Packets (network):
======
tx port=1 data=000000aabbcc00000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

tx port=1 data=000000aabbcc00000000000281000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374


Packet's MetaData / Content (received by CPU) :
======

Packet#1 -
<<None>>

Packet#2 -
[bcmPWN.0]Packet[1]: data[0000]: {000000aabbcc} {000000000002} 8100 0016
[bcmPWN.0]Packet[1]: data[0010]: 0800 4500 0082 0000 4000 3f11 65b7 c0a8
[bcmPWN.0]Packet[1]: data[0020]: 0101 0a0a 0a01 d903 12b5 006e 790b 0800
[bcmPWN.0]Packet[1]: data[0030]: 0000 0123 4500 0000 0000 bbbb 0000 0000
[bcmPWN.0]Packet[1]: data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
[bcmPWN.0]Packet[1]: data[0050]: 4011 7497 0202 0202 0101 0101 0035 0035
[bcmPWN.0]Packet[1]: data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
[bcmPWN.0]Packet[1]: data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
[bcmPWN.0]Packet[1]: data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
[bcmPWN.0]Packet[1]: data[0090]: 7465 7374 7465 7374
[bcmPWN.0]
[bcmPWN.0][RX metadata information]
        PKT_LENGTH                      :0x9c(156)
        SRC_PORT_NUM                    :0x1(1)
        SWITCH                          :0x1(1)
        MATCH_ID_LO                     :0x88905222(-2003807710)
        MATCH_ID_HI                     :0x1014(4116)
        MPB_FLEX_DATA_TYPE              :0x3(3)
        EGR_ZONE_REMAP_CTRL             :0x1(1)
        MULTICAST                       :0x1(1)
        COPY_TO_CPU                     :0x1(1)
[FLEX fields]
        EVENT_TRACE_VECTOR_15_0         :0x4
        DROP_CODE_15_0                  :0x202
        PARSER_VHLEN_0_15_0             :0x8a8a
        VFI_15_0                        :0x16
        L2_IIF_10_0                     :0x1
        SYSTEM_SOURCE_15_0              :0x1
        INGRESS_PP_PORT_7_0             :0x1
        SYSTEM_DESTINATION_15_0         :0x800
        EFFECTIVE_TTL_7_0               :0x3f
        VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0:0x2
        SYSTEM_OPCODE_3_0               :0x3
        PKT_MISC_CTRL_0_3_0             :0x1
[RX reasons]
        L2_SRC_STATIC_MOVE

*/


/* Reset C interpreter*/
cint_reset();
int VLAN_MEMBERS = 3;

int vlan=22;
bcm_port_t egress_port[VLAN_MEMBERS];
bcm_port_t ingress_port;
bcm_port_t mirror_to_port;


/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int i = 0, port = 0, rv = 0;

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
 * Configures the port in loopback mode.
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule discards all incoming
 * packets. This is to avoid continuous loopback of
 * the packet.
 */
bcm_error_t
multi_egress_port_setup(int unit, bcm_port_t *port)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;
    int i;

    for(i=1; i<VLAN_MEMBERS; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port[i], BCM_PORT_LOOPBACK_MAC));
    }

    bcm_field_group_config_t_init(&group_config);
	group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
	group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    for(i=1; i<VLAN_MEMBERS; i++) {
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port[i], BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the incoming packets.
 * Ports are also configured to discard all packets.
 */
bcm_error_t
mirror_to_port_setup(int unit, bcm_port_t port)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
	group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
	group_config.priority = 3;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress, egress
 * and mirror_to port. Check ingress_port_setup(), multi_egress_port_setup(),
 * and mirror_to_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[VLAN_MEMBERS+1]; /*Container for ingress/egress/mirror-to ports */
    int i=0;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, VLAN_MEMBERS+1)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    for(i=0; i<VLAN_MEMBERS; i++)
    {
        egress_port[i] = port_list[i];
    }
    ingress_port   = port_list[0];
    mirror_to_port = port_list[VLAN_MEMBERS];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }
    if (BCM_E_NONE != multi_egress_port_setup(unit, egress_port)) {
        printf("multi_egress_port_setup() failed");
        return -1;
    }
    if (BCM_E_NONE != mirror_to_port_setup(unit, mirror_to_port)) {
        printf("mirror_to_port_setup() failed");
        return -1;
    }

    bshell(unit, "pw start; pw report +raw; pw report +pmd");

    return BCM_E_NONE;
}


void
verify(int unit)
{
    char str[512];
    char pkt_data[2][512];
    int  rv, i=0;

    /*Initalize the pkt_data with the actual ingress packets to be used for testing*/
    snprintf(pkt_data[0],512,"%s","000000aabbcc00000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[1],512,"%s","000000aabbcc00000000000281000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");

//  bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "clear counters");

    for(i=0; i<sizeof(pkt_data)/512; i++)
    {
        printf("\nSending unknown unicast packet No.[%d] to ingress_port access side:%d\n", i+1, ingress_port);
        snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 8", ingress_port, pkt_data[i]);
        bshell(unit, str);
        bshell(unit, "show counters");
    }

    print("\nGet the event counter for double confirmation:\n");
    uint64 val;
    rv = bcm_stat_trace_event_counter_sync_get(unit, bcmPktTraceEventIngressL2SrcStaticMove, &val);
    uint32 val_low = COMPILER_64_LO(val);
    if (BCM_FAILURE(rv)) {
        printf("Error in getting the event counter: %s.\n", bcm_errmsg(rv));
    }
    else {
        printf("Counter 'bcmPktTraceEventIngressL2SrcStaticMove': %x.\n", val_low);
    }

    return;
}


/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if (BCM_FAILURE(rv) && (rv!= BCM_E_EXISTS)) {
        printf("\vlan_create_add_port() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Static L2 entry for forwarding */
int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
}


/* EventAction Configuration function */
bcm_error_t
config_event_action(int unit)
{
    int rv, i=0;

    /* VLAN creation */
    printf("Create a VLAN and add all member ports.\n");
    rv = vlan_create_add_port(unit, vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in VLAN create: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    for(i=1; i<VLAN_MEMBERS; i++) {
        rv = vlan_create_add_port(unit, vlan, egress_port[i]);
        if (BCM_FAILURE(rv)) {
            printf("\nError in VLAN create: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Set the port learning mode */
    printf("Set the port learning mode.\n");
    rv = bcm_port_learn_set(unit, ingress_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Port-Learn config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    for(i=1; i<VLAN_MEMBERS; i++) {
        rv = bcm_port_learn_set(unit, egress_port[i], BCM_PORT_LEARN_FWD);
        if (BCM_FAILURE(rv)) {
            printf("\nError in Port-Learn config: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    rv = bcm_port_learn_set(unit, mirror_to_port, BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Port-Learn config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a static L2 MAC address on egress_port[1] */
    printf("Create a static L2 MAC address on egress_port[1].\n");
    rv = add_to_l2_table(unit, "00:00:00:00:00:02", vlan, egress_port[1]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in L2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

	/* COPY-TO-CPU for Trace event 'Station Move for Static L2 Source address'. */
    printf("Enable the specific trace event and copy to CPU.\n");
	bcm_switch_trace_event_mon_t trace_event_mon;
	bcm_switch_trace_event_mon_t_init(&trace_event_mon);
    trace_event_mon.trace_event = bcmPktTraceEventIngressL2SrcStaticMove ;
	trace_event_mon.actions    |= BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
	rv = bcm_switch_trace_event_mon_set(unit, &trace_event_mon);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Trace-Event config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_event_action())
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

    print "~~~ #2) config_event_action(): ** start **";
    if (BCM_FAILURE((rv = config_event_action(unit)))) {
        printf("config_event_action() failed.\n");
        return -1;
    }
    print "~~~ #2) config_event_action(): ** end **";

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
