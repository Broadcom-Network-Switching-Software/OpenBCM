/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880), Verifying ingress/egress mirror over HG3 port 
 *
 * Usage    : BCM.0> cint TD4_HSDK_Mirror_over_hg3.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_Mirror_over_hg3_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   egress port 
 *                                  |                      |------------------>
 *               mirror source port |                      | 
 *                        --------->|         TD4          | 
 *                   ingress packet |        56880         |  HG3 port
 *                                  |                      |-------->
 *                                  |                      | 
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate mirroring packet over hg3 port
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. They are used as ingress port/egress port/hg3 port.
 *
 *     b) Install an IFP rule to copy ingress & egress & hg3 packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 * 
 *   2) Step2 - Configure mirroring over hg3(Done in configure_port_mirror()):
 *
 *     a) Configure L2 flow.
 *
 *     b) Configure hg3 property on selected port(Done in configure_hg3()).
 *        Packet going to the remote MTP mod/port will be through this hg3.
 * 
 *     c) Create a mirror destination for the remote MTP mod/port.
 *
 *     d) Configure port based ingress and egress mirror
 *
 *   3) Step3 - Verification (Done in Verify()):
 *   ===========================================
 *
 *     a) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     b) Expected Result:
 *     ===================
 *       We can see the two forwarding packet from ingress port/egress port.
 *       We can see two mirrored packet loopback from hg3. 
 *       The PMD information is printed and please check KB0028394 for the detail
 */
    

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port; //source port
bcm_port_t egress_port; //egress port
bcm_port_t hg3_port;   //local hg3 port
bcm_vlan_t forward_vlan = 0x15;
int remote_modid = 4;
int remote_port = 7;
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
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;
    bcm_port_t     port;

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

/* Static L2 entry for forwarding */
int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vlan_t vid, int port)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vid);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
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
    hg3_port = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    bcm_pbmp_t     pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_PBMP_PORT_ADD(pbmp, hg3_port);

    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d and %d", egress_port, hg3_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

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
    bshell(unit, "pw start");
    bshell(unit, "pw report +raw +pmd");
    printf("\n\nSending packet to ingress_port:%d forwarded to egress port %d\n", ingress_port, egress_port);
    printf("\nExpect ingress and egress mirrored to remote mod %d/port %d through local hg3 port %d\n\n", remote_modid, remote_port, hg3_port);

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA00000000BBBB810000150800450000500001000040117497010101010202020200350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);

    return;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, bcm_pbmp_t pbmp)
{
    bcm_pbmp_t upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(upbmp);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

int
mirror_system_config(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_mirror_init(unit));

    return BCM_E_NONE;
}

bcm_error_t configure_port_mirror(int unit, bcm_port_t src_port, bcm_port_t dst_port,
                                         bcm_port_t hg3_port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t src_gport = BCM_GPORT_INVALID;
    bcm_gport_t dst_gport = BCM_GPORT_INVALID;
    bcm_gport_t remote_mtp_gport;
    bcm_mirror_destination_t mirror_dest_info;
    bcm_gport_t mirror_dest_id = BCM_GPORT_INVALID;
    bcm_pbmp_t pbmp;
    
    BCM_IF_ERROR_RETURN(mirror_system_config(unit));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, forward_vlan, pbmp));

    printf("Add L2 Address for egress port %d.\n", egress_port);
    rv = add_to_l2_table(unit, "00:00:00:00:aa:aa", forward_vlan, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Configure HG3 on port %d. Packet going to remote mod %d/port %d will be through this hg3 port\n", hg3_port, remote_modid, remote_port);
    BCM_IF_ERROR_RETURN(configure_hg3(unit, hg3_port, remote_modid, remote_port));
    /* Get the Mirror To Port GPORT ID */
    BCM_GPORT_MODPORT_SET(remote_mtp_gport, remote_modid, remote_port);

    printf("Configure remote mtp to be mod %d/port %d\n", remote_modid, remote_port);
    bcm_mirror_destination_t_init(mirror_dest_info);
    mirror_dest_info.gport = remote_mtp_gport;

    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, mirror_dest_info));
    mirror_dest_id = mirror_dest_info.mirror_dest_id;

    /* Get the Mirror Port GPORT ID */
    BCM_GPORT_MODPORT_SET(src_gport, unit, src_port);
    /* configure port ingress mirror */
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, src_gport, BCM_MIRROR_PORT_INGRESS, mirror_dest_id));
    
    /* Get the Mirror Port GPORT ID */
    BCM_GPORT_MODPORT_SET(dst_gport, unit, dst_port);
    /* configure port egress mirror */
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, dst_gport, BCM_MIRROR_PORT_EGRESS, mirror_dest_id));
    return rv;
}

bcm_error_t configure_hg3(int unit, bcm_port_t local_port, int remote_modid, int remote_port)
{
    bcm_port_encap_config_t encap_config;
    bcm_pbmp_t pbmp;
    encap_config.encap = BCM_PORT_ENCAP_HIGIG3;
    bcm_port_encap_config_set(unit, local_port, encap_config);
    /*Configure local_port to be Higig3*/
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHigig3Ethertype, 0x123));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, remote_port);
    /*Enable remote port forwarding*/
    BCM_IF_ERROR_RETURN(bcm_stk_module_remote_port_enable_set(unit, remote_modid, pbmp));

    /*go to remote mod throguth local_port*/
    BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, remote_modid, local_port));
    
    return BCM_E_NONE;
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

    print "~~~ #2) configure_port_mirror(): ** start **";
    if (BCM_FAILURE((rv = configure_port_mirror(unit, ingress_port, egress_port, hg3_port)))) {
        printf("configure_port_mirror() failed.\n");
        return -1;
    }
    print "~~~ #2) configure_port_mirror(): ** end **";

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
