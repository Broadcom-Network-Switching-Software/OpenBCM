/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880), Verifying ingress/egress mirror packet by using IFP 
 *
 * Usage    : BCM.0> cint TD4_HSDK_Mirror_use_IFP.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_Mirror_use_IFP_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   egress port 
 *                                  |                      |------------------>
 *               mirror source port |                      | 
 *                        --------->|         TD4          | 
 *                   ingress packet |        56880         |  MTP port
 *                                  |                      |-------->
 *                                  |                      | 
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate mirroring packet by IFP
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testsetup()):
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
 *     c) Configure L2 flow
 * 
 *     d) Create a mirror destination for MTP port
 *
 *     e) Configure IFP to do ingress and egress mirror
 *
 *   3) Step3 - Verification (Done in testVerify()):
 *   ===========================================
 *
 *     a) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     b) Expected Result:
 *     ===================
 *       We can see the two mirrored packet on the MTP port.
 */

/* Reset C interpreter*/
cint_reset();
int ingress_port, egress_port1, egress_port2;
bcm_gport_t mirror_dest_id;

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
        if (checkPortAssigned(port_index_list, i, index) == 0)
        {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}


/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;
    bcm_port_t         port;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 2;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionDrop);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, gcfg1.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

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
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 3;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionCopyToCpu);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit,gcfg1.group,&entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

bcm_error_t configure_ifp_mirror(int unit, bcm_port_t port, bcm_gport_t mirror_dest_id)
{

    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 4;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionMirrorIngress);
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionMirrorEgress);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit,gcfg1.group,&entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionMirrorIngress, 0, mirror_dest_id));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionMirrorEgress, 0, mirror_dest_id));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;

}

/*
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
    bcm_vlan_t vid = 2;
    uint8 dest_mac[6] = {0x00, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t addr;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    int modid;
    bcm_gport_t gport;

    int port_list[3];
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];
    printf("Ingress port: %d\n",ingress_port);
    printf("Egress    port: %d\n",egress_port1);
    printf("Mirrored port: %d\n",egress_port2);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);

    /* Create a vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vid));

    /* Add a L2 address on front panel port */
    bcm_l2_addr_t_init(&addr, dest_mac, vid);
    addr.port = egress_port1;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
	    printf("ingress_port_setup() failed for port %d\n", ingress_port_1);
	    return -1;
    }
    
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);
    
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
	    printf("egress_port_multi_setup() failed for port %d\n", egress_port1);
	    return -1;
    }

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &modid));
    BCM_GPORT_MODPORT_SET(gport, modid, egress_port2);

    /* Mirror Port */
    bcm_mirror_destination_t mtp;
    bcm_mirror_destination_t_init(&mtp);
    mtp.gport=gport;
    /* Create Mirror destination. */
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit,&mtp));

    mirror_dest_id = mtp.mirror_dest_id;

    BCM_IF_ERROR_RETURN(configure_ifp_mirror(unit, ingress_port, mirror_dest_id));
    
    return BCM_E_NONE;
}

/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t testVerify(int unit)
{
    char     str[512];
    uint64 in_pkt, out_pkt;

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report +raw +decode");

    /* Send pacekt to ingress_port */
    printf("\n******************************\n");
    printf("******Traffic test start******\n");
    printf("Packet comes into ingress port %d, expect forward to port %d\n", ingress_port, egress_port1);
    printf("Two packets will be mirror to port %d by using IFP, one for IFP ingress mirror action and the other for IFP egress mirror action\n", egress_port2);
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

    printf("--------IFP Mirrored port (%d) stats--------\n", egress_port2);
    COMPILER_64_ZERO(out_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port2, snmpDot1dTpPortOutFrames, &out_pkt));
    printf("IFP Mirror : %d packets, One for ingress mirror and the other for egress mirror\n", COMPILER_64_LO(out_pkt));
    if (COMPILER_64_LO(out_pkt) != 2) {
        printf("Packet verify failed. Expected out packet 2 but get %d\n", COMPILER_64_LO(out_pkt));
        return -1;
    }
    bshell(unit, "show c");

    return BCM_E_NONE;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in testVerify()).
 * c)clen up
 */
bcm_error_t testExecute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    rv = testSetup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("Completed test setup successfully.\n");

    rv = testVerify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify() failed\n");
        return -1;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print testExecute();
}

