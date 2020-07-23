/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880), Verifying Mirror on Drop feature. 
 *
 * Usage    : BCM.0> cint TD4_HSDK_Mirror_on_drop.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_Mirror_on_drop_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   egress port(dropped) 
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
 * Cint example to demonstrate mirroring on drop
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. 
 *        They are used as ingress port/egress port/mtp
 *
 *   2) Step2 - Configure MoD (Done in configure_mod_mirror()):
 *   =============================================
 *     a) Configure mod cosq on the egress port to drop the packet
 * 
 *     b) Configure mod destination on MTP with mirrored packet in PSAMP format
 *
 *     c) Configure mod profile and enable MOD by IFP.
 *
 *     d) Setup L2 flow from ingress port to egress port
 *
 *   3) Step3 - Verification (Done in Verify()):
 *   ===========================================
 *
 *     a) Transmit the packet. The packet is expected to be forwarded to egress port.
 *        The contents of the packet are printed on screen.
 *
 *     b) Expected Result:
 *     ===================
 *       We can see one mirrored packet in PSAMP formwat. 
 *       Mod stats will be shown in the screen.
 */
    

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port; //source port
bcm_port_t egress_port;
bcm_port_t mtp_port; //MTP port
bcm_vlan_t forward_vlan = 0x2;
bcm_gport_t mirror_dest_id = BCM_GPORT_INVALID;
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
    
    printf("ingress_port_setup port: %d setup\n", port);
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
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, gcfg1.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

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

bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t port;
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 3;
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
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[3];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    mtp_port = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_PBMP_PORT_ADD(pbmp, mtp_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];
    uint64 val;
    int rv = BCM_E_NONE;

    bshell(unit, "vlan show");
    bshell(unit, "clear c");

    printf("\n\nSending unicast packet to ingress port %d, expected to be dropped and mirrored to port %d in PSAMP format\n", ingress_port, mtp_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000bbbbbb002a107777008100000208004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);

    bshell(unit, "sleep 2");

    bcm_cosq_object_id_t id;
    uint64 value1, value2, value3;

    bcm_cosq_object_id_t_init(&id);
    id.buffer = 0;
    
    printf("\n\n========== Display Mod Stats ==========");
    BCM_IF_ERROR_RETURN(bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatDroppedPackets, &value1));
    printf("\nMOD pool full dropped packet:  %d", COMPILER_64_LO(value1));
    BCM_IF_ERROR_RETURN(bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatEnqueuedPackets, &value2));
    printf("\nMOD mirrored packet:           %d", COMPILER_64_LO(value2));
    BCM_IF_ERROR_RETURN(bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatPoolBytesCurrent, &value3));    
    printf("\nMOD pool current used count:   %d\n", COMPILER_64_LO(value3));
    return;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

int
mirror_system_config(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_mirror_init(unit));

    return BCM_E_NONE;
}


bcm_error_t configure_mod_mirror_dest(int unit, bcm_port_t mtp_port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t gport;
    uint8 dest_mac[6] = {0x00, 0x11, 0x00, 0xbb, 0xbb, 0xbb};
    uint8 src_mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    bcm_mirror_source_t source;
    int count = 0;
    
    bcm_mirror_destination_t mtp;
    bcm_mirror_destination_t_init(&mtp);
    BCM_GPORT_MODPORT_SET(gport, 0, mtp_port);
    mtp.gport=gport;
    mtp.flags2=BCM_MIRROR_DEST_FLAGS2_MOD_TUNNEL_PSAMP;
    sal_memcpy(mtp.dst_mac, dest_mac, 6);
    sal_memcpy(mtp.src_mac, src_mac, 6);
    mtp.tpid=0x8100;
    mtp.vlan_id=1;
    mtp.version=4;
    mtp.src_addr=0x021010a0;
    mtp.dst_addr=0x111010a0;
    mtp.ttl=2;
    mtp.tos=2;
    mtp.df=1;
    mtp.cosq = 1;
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit,&mtp));
    
    bcm_mirror_source_t_init(&source);
    source.type = bcmMirrorSourceTypeCosqMod;
    BCM_IF_ERROR_RETURN(bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id));
    BCM_IF_ERROR_RETURN(bcm_mirror_source_dest_get_all(unit, &source, 1, &mtp.mirror_dest_id, &count));

    return rv;
}

bcm_error_t enable_mod(int unit,bcm_port_t port)
{

    bcm_cosq_mod_control_t control;
    bcm_cosq_mod_control_t_init(&control);
    control.valid_flags = BCM_COSQ_MOD_CONTROL_ENABLE_VALID | BCM_COSQ_MOD_CONTROL_POOL_LIMIT_VALID;
    control.enable = 1;
    control.pool_limit = 65024; /* 256 Cells. */

    BCM_IF_ERROR_RETURN(bcm_cosq_mod_control_set(unit, &control));
    BCM_IF_ERROR_RETURN(bcm_cosq_mod_control_get(unit, &control));

    bcm_cosq_mod_profile_t profile;
    int profile_id = 4;

    bcm_cosq_mod_profile_t_init(&profile);
    profile.percent_0_25 = 0xffff;
    profile.percent_25_50 = 0x03ff;
    profile.percent_50_75 = 0x001f;
    profile.percent_75_100 = 0x000f;

    BCM_IF_ERROR_RETURN(bcm_cosq_mod_profile_create(unit, BCM_COSQ_MOD_PROFILE_OPTIONS_WITH_ID, &profile_id));
    BCM_IF_ERROR_RETURN(bcm_cosq_mod_profile_set(unit, profile_id, &profile));

    bcm_field_qset_t qual;                                                            
    BCM_FIELD_QSET_INIT(qual);                                                        
    BCM_FIELD_QSET_ADD(qual, bcmFieldQualifyInPort);                                
    BCM_FIELD_QSET_ADD(qual, bcmFieldQualifyStageIngress);                
    BCM_FIELD_QSET_ADD(qual, bcmFieldQualifyDstMac);
                                                                                    
    bcm_field_aset_t action;                                                        
    BCM_FIELD_ASET_INIT(action);                                                    
    BCM_FIELD_ASET_ADD(action, bcmFieldActionMirrorOnDropEnable);                    
    BCM_FIELD_ASET_ADD(action, bcmFieldActionMirrorOnDropProfileSet);                
                                                                           
    bcm_field_group_config_t gcfg1;    
    bcm_field_entry_t entry_id;
    bcm_field_group_config_t_init(gcfg1);                                           
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_ASET;                                   
    gcfg1.qset= qual;                                                               
    gcfg1.aset= action;                                                               
    gcfg1.priority= 4;                                                               
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));                               
                                                                                   
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, gcfg1.group, &entry_id));                   
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry_id, port, 0xff));                        
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_id, bcmFieldActionMirrorOnDropEnable, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_id, bcmFieldActionMirrorOnDropProfileSet, profile_id, 1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_id));
    return BCM_E_NONE;
}

bcm_error_t configure_mod_cosq(int unit, bcm_port_t port)
{
    int i=0;
    bcm_cosq_control_t type;
    
    for (i=0;i<8;i++) {
        type = bcmCosqControlEgressUCQueueGroupMinEnable;
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, i, type, 0));
        type = bcmCosqControlEgressUCSharedDynamicEnable;
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, i, type, 0));
        type = bcmCosqControlEgressUCQueueLimitEnable;
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, i, type, 0));
        type = bcmCosqControlEgressUCQueueSharedLimitBytes;
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, i, type, 0));
        type = bcmCosqControlUCEgressPool;
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, i, type, 0));
        type = bcmCosqControlEgressPortPoolSharedLimitBytes;
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit, port, i, type, 0));
    }
    return BCM_E_NONE;
}

bcm_error_t setup_traffic(int unit, bcm_port_t ingress_port, bcm_port_t egress_port)
{
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t addr; 
    uint8 mac[6] = {0x00, 0x00, 0x00, 0xbb, 0xbb, 0xbb};
    
    /* Prepare port bitmap for VLAN */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    
    /* Create a vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, forward_vlan));
    
    /* Add vlan member ports */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, forward_vlan, pbmp, pbmp));
        
    /* Add a L2 address on front panel port */
    bcm_l2_addr_t_init(&addr, mac, forward_vlan);
    addr.port= egress_port;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));
    return BCM_E_NONE;
}

bcm_error_t configure_mod_mirror(int unit, bcm_port_t ingress_port, bcm_port_t egress_port, bcm_port_t mtp_port)
{
    int rv;
    printf("        a) Configure mod cosq on the egress port %d to drop the packet\n", egress_port);
    rv = configure_mod_cosq(unit, egress_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error in configure_mod_cosq():%s\n", bcm_errmsg(rv));
    }
    printf("        b) Configure mod destination on MTP %d with mirrored packet in PSAMP format\n", mtp_port);
    rv = configure_mod_mirror_dest(unit, mtp_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error in configure_mod_mirror_dest():%s\n", bcm_errmsg(rv));
    }
    printf("        c) Configure mod profile and enable MOD by IFP\n");
    rv = enable_mod(unit, ingress_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error in enable_mod():%s\n", bcm_errmsg(rv));
    }
    printf("        d) Setup L2 flow from ingress port to egress port\n");
    rv = setup_traffic(unit, ingress_port, egress_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error in setup_traffic():%s\n", bcm_errmsg(rv));
    }
    return rv;
}
/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) configure_mod_mirror(): ** start **";
    if (BCM_FAILURE((rv = configure_mod_mirror(unit, ingress_port, egress_port, mtp_port)))) {
        printf("configure_mod_mirror() failed.\n");
        return -1;
    }
    print "~~~ #2) configure_mod_mirror(): ** end **";

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
