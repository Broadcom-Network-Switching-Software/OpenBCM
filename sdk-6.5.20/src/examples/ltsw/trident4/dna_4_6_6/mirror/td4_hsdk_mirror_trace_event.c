/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
      
/*
 * Feature  : TD4(56880), Verifying Trace Event Mirroring 
 *
 * Usage    : BCM.0> cint TD4_HSDK_Mirror_trace_event.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_Mirror_trace_event_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   
 *                                  |                      |
 *                    ingress port  |                      | 
 *                        --------->|         TD4          | 
 *                   ingress packet |        56880         |  MTP port
 *                                  |                      |-------->
 *                                  |                      | 
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate trace event mirroring
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testsetup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. One is ingress port and the other is MTP.
 *
 *     c) add static src L2 entry with src port different from packet ingress port to cause static move.
 * 
 *     d) Create a mirror destination for MTP port
 *
 *     e) Configure mirror trace event to be bcmPktTraceEventIngressL2SrcStaticMove
 *
 *   3) Step3 - Verification (Done in testVerify()):
 *   ===========================================
 *
 *     b) Transmit the packet. 
 *        The source mac of that packet is the same as the added L2_ENTRY but the ingress port is different.
 *        This will cause the trace event IngressL2SrcStaticMove.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the one mirrored packet on the MTP port.
 */

/* Reset C interpreter*/
cint_reset();
int ingress_port, mtp_port;
bcm_gport_t mirror_dest_id;
bcm_mirror_source_t source;
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
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
mtp_port_setup(int unit, bcm_port_t port)
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
    gcfg1.priority= 2;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionDrop);
	
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit,gcfg1.group,&entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. 
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

bcm_error_t configure_trace_event_mirror(int unit, bcm_gport_t mirror_dest_id)
{

    bcm_mirror_source_t_init(&source);
    source.type = bcmMirrorSourceTypePktTraceEvent;
    source.trace_event = bcmPktTraceEventIngressL2SrcStaticMove ;
    /* Add a mirror destination to a source port. */
    BCM_IF_ERROR_RETURN(bcm_mirror_source_dest_add(unit, &source, mirror_dest_id));
    return BCM_E_NONE;

}

/*
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
    bcm_vlan_t vid = 2;
    uint8 src_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t addr;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    int modid;
    bcm_gport_t gport;

    int port_list[2];
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    mtp_port = port_list[1];
    printf("Ingress port: %d\n",ingress_port);
    printf("Mirrored to port: %d\n", mtp_port);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);

    /* Create a vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));

    /* Add a static L2 address on ingress_port + 1 to cause static move */
    bcm_l2_addr_t_init(&addr, src_mac, vid);
    addr.port = ingress_port + 1;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != mtp_port_setup(unit, mtp_port)) {
        printf("mtp_port_setup() failed for port %d\n", mtp_port);
        return -1;
    }

    printf("Block traffic ingress from MTP to avoid continuous traffic. This is for test only\n");
    BCM_IF_ERROR_RETURN(bcm_port_stp_set(unit, mtp_port, BCM_STG_STP_BLOCK));
   
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &modid));
    BCM_GPORT_MODPORT_SET(gport, modid, mtp_port);
    
    /* Mirror Port */
    bcm_mirror_destination_t mtp;
    bcm_mirror_destination_t_init(&mtp);
    mtp.gport = gport;
    /* Create Mirror destination. */
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit,&mtp));

    mirror_dest_id = mtp.mirror_dest_id;

    BCM_IF_ERROR_RETURN(configure_trace_event_mirror(unit, mirror_dest_id));
    
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
    bshell(unit, "pw report +pmd");
    bshell(unit, "pw report +raw");

    /* Send pacekt to ingress_port */
    printf("\n******************************\n");
    printf("******Traffic test start******\n");
    printf("Packet comes into ingress port %d, hit the static L2_ENTRY with different source port\n", ingress_port);
    printf("Matching trace reason IngressL2SrcStaticMove, packet is mirror to port %d\n", mtp_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00BBBBBBBBBB0011223344558100000208004500001C000000004002F95DC0A8001EC0A800141164EE9B00000000000102030405060708090A0B0C0D0E0F1011E544D79B;sleep 3", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);

    bshell(unit, "show c");
    bshell(unit, "l2 show");

    printf("\n******Port stats check******\n");
    printf("--------Ingress port (%d) stats--------\n", ingress_port);
    COMPILER_64_ZERO(in_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ingress_port, snmpDot1dTpPortInFrames, &in_pkt));
    printf("Rx : %d packets\n", COMPILER_64_LO(in_pkt));

    printf("-------- MTP (%d) stats--------\n", mtp_port);
    COMPILER_64_ZERO(out_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, mtp_port, snmpDot1dTpPortOutFrames, &out_pkt));
    printf("Mirrored out: %d packets\n", COMPILER_64_LO(out_pkt));
    if (COMPILER_64_LO(out_pkt) != 1) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
        return -1;
    }

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
