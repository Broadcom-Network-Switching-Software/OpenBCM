/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Drop Event, trigger several drop events in egress pipeline and check they are properly counted to related Drop Event counters.
 *
 * Usage    : BCM.0> cint TD4_HSDK_DropEvent_ep_counters.c
 *
 * Config   : bcm56880_a0-generic-32x400_event.config.yml
 *
 * Log file : TD4_HSDK_DropEvent_ep_counters_log.txt
 *
 * Test Topology :
 *
 *
 *                     VLAN=22   +----------------------+          VLAN=22              +---+
 *                               |                      +-------------------------------+   |
 *                               |                      |egress_port[1]                 +---+
 *                               |                      |     00::02:01
 *         +---+                 |                      |                               +---+
 *         |   +-----------------+                      +-------------------------------+   |
 *         +---+    ingress_port |      Trident-4       |egress_port[2]                 +---+
 *        00::01                 |                      |     00::02:02
 *                               |                      |                               +---+
 *                               |                      +-------------------------------+   |
 *                               |                      |egress_port[3]                 +---+
 *                               |                      |     00::02:03
 *                               |                      |
 *                               +----------------------+
 *
 * Summary:
 * ========
 * CINT example to demonstrate "Drop Event" feature for counting some kind of drop
 * reasons in egress pipeline to the related Drop Event Counters.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects four ports and configure them in Loopback mode. Out of these four
 *        ports, one port is used as ingress_port and other ports are used as egress
 *        ports.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU per egress port,
 *        and start packet watcher to dump CPU-received packets.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature
 *     configuration.
 *
 *   2) Step2 - Configuration (Done in config_ep_drop_action()):
 *   =========================================================
 *     a) Configure the "Drop Event" configuration.
 *        This does the necessary configurations to create some drop conditions for
 *        specific egress ports.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show' and 'l2 show'.
 *
 *     b) Transmit three packets with DA=00:00:00:00:02:01/00:00:00:00:02:02/00:00:00:00:02:03,
 *        SA=00:00:00:00:00:01 and VLAN=22 to ingress port.
 *
 *     c) Expected Result:
 *     ===================
 *       For the 1st packet, we can see that the packet is unicasted to the egress_port[1].
 *       For the 2nd packet, it is unicasted to the egress port[2]. However, due to EFP dorp,
 *           the packet is discarded at egress pipeline. The Drop Event counter (EFP) is increased.
 *       For the 3rd packet, it is unicasted to the egress port[3. However, due to STP blocking,
 *           the packet is discarded at egress pipeline. The Drop Event counter (STP Blocking) is increased.
 *       Also, 'show counters' shows the TX/RX information of packet forwarding.
 */

/*
Packets:

Ingress Packets: (No. of packets = 2)
======

  ###[ Ethernet ]###
    dst= 00:00:00:00:02:01/00:00:00:00:02:02/00:00:00:00:02:03
    src= 00:00:00:00:00:01
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 22
     type= 0x800
     ......
     ......

Ingress Hex Packets:
======
tx port=1 data=00000000020100000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

tx port=1 data=00000000020200000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

tx port=1 data=00000000020300000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374


Packet's MetaData / Content (received by CPU) :
======

Packet - #1
Packet[1]: data[0000]: {000000000201} {000000000001} 8100 0016
Packet[1]: data[0010]: 0800 4500 0082 0000 4000 3f11 65b7 c0a8
Packet[1]: data[0020]: 0101 0a0a 0a01 d903 12b5 006e 790b 0800
Packet[1]: data[0030]: 0000 0123 4500 0000 0000 bbbb 0000 0000
Packet[1]: data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
Packet[1]: data[0050]: 4011 7497 0202 0201 0101 0101 0035 0035
Packet[1]: data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
Packet[1]: data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
Packet[1]: data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
Packet[1]: data[0090]: 7465 7374 7465 7374

Packet - #2
<None>

Packet - #3
<None>

*/


/* Reset C interpreter*/
cint_reset();
int VLAN_MEMBERS = 4;

int vlan=22;
bcm_port_t egress_port[VLAN_MEMBERS];
bcm_port_t ingress_port;


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
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
	group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    for(i=1; i<VLAN_MEMBERS; i++) {
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port[i], BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}


/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress/egress ports.
 * Check ingress_port_setup() and multi_egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[VLAN_MEMBERS]; /*Container for ingress/egress/mirror-to ports */
    int i=0;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, VLAN_MEMBERS)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    for(i=0; i<VLAN_MEMBERS; i++)
    {
        egress_port[i] = port_list[i];
    }
    ingress_port   = port_list[0];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }
    if (BCM_E_NONE != multi_egress_port_setup(unit, egress_port)) {
        printf("multi_egress_port_setup() failed");
        return -1;
    }

    bshell(unit, "pw start; pw report +raw; pw report +pmd");

    return BCM_E_NONE;
}


void
verify(int unit)
{
    char str[512];
    char pkt_data[3][512];
    int  rv, i=0;

    /*Initalize the pkt_data with the actual ingress packets to be used for testing*/
    snprintf(pkt_data[0],512,"%s","00000000020100000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[1],512,"%s","00000000020200000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[2],512,"%s","00000000020300000000000181000016080045000082000040003F1165B7C0A801010A0A0A01D90312B5006E790B080000000123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202010101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");

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

        print("\nCheck the event counter for confirmation:\n");
        uint64 val;
        rv = bcm_stat_drop_event_counter_sync_get(unit, bcmPktDropEventEgressEfpDrop, &val);
        uint32 val_low = COMPILER_64_LO(val);
        if (BCM_FAILURE(rv)) {
            printf("Error in getting the event counter: %s.\n", bcm_errmsg(rv));
        }
        else {
            printf("Counter 'bcmPktDropEventEgressEfpDrop': %x.\n", val_low);
        }
        rv = bcm_stat_drop_event_counter_sync_get(unit, bcmPktDropEventEgressStgBlockDrop, &val);
        val_low = COMPILER_64_LO(val);
        if (BCM_FAILURE(rv)) {
            printf("Error in getting the event counter: %s.\n", bcm_errmsg(rv));
        }
        else {
            printf("Counter 'bcmPktDropEventEgressStgBlockDrop': %x.\n", val_low);
        }
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
config_ep_drop_action(int unit)
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

    /* Create some static L2 MAC addresses on egress_port[1]/[2]/[3] */
    printf("Create some static L2 MAC addresses on egress_port[1]/[2]/[3].\n");
    rv = add_to_l2_table(unit, "00:00:00:00:02:01", vlan, egress_port[1]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in L2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = add_to_l2_table(unit, "00:00:00:00:02:02", vlan, egress_port[2]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in L2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = add_to_l2_table(unit, "00:00:00:00:02:03", vlan, egress_port[3]);
    if (BCM_FAILURE(rv)) {
        printf("\nError in L2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a EFP rule to drop the packet sent out from the egress_port[2] */
    printf("Create a EFP rule to drop the packets on egress_port[2].\n");
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyOutPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 0;
    rv = bcm_field_group_config_create(unit, &group_config);
    if (BCM_FAILURE(rv)) {
        printf("\nError in creating EFP group: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_entry_create(unit, group_config.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("\nError in creating EFP entry: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_qualify_OutPort(unit, entry, egress_port[2], BCM_FIELD_EXACT_MATCH_MASK);
    if (BCM_FAILURE(rv)) {
        printf("\nError in applying EFP qualifies: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in applying EFP actions: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("\nError in installing EFP rule: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the STP state of egress_port[3] to 'BLOCK'*/
    printf("Set the STP state of egress_port[3] to 'BLOCK'.\n");
    rv = bcm_stg_stp_set(unit, 1, egress_port[3], BCM_STG_STP_BLOCK);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring STP state: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_ep_drop_action())
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

    print "~~~ #2) config_ep_drop_action(): ** start **";
    if (BCM_FAILURE((rv = config_ep_drop_action(unit)))) {
        printf("config_ep_drop_action() failed.\n");
        return -1;
    }
    print "~~~ #2) config_ep_drop_action(): ** end **";

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
