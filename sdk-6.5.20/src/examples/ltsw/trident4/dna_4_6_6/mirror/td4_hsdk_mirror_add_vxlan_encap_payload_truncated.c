/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880), Verifying ingress mirror packet to vxlan tunnel feature
 *
 * Usage    : BCM.0> cint TD4_HSDK_Mirror_add_Vxlan_Encap_payload_truncated.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_Mirror_add_Vxlan_Encap_payload_truncated_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   egress port 
 *                                  |                      |------------------>
 *               mirror source port |                      | 
 *                        --------->|         TD4          | 
 *                   ingress packet |        56880         |  Vxlan tunnel port(MTP port)
 *                                  |                      |-------->
 *                                  |                      | tunnel header + Vxlan header + payload truncated
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate ingress mirror packet to vxlan tunnel
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
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
 *   2) Step2 - Configuration (done in configure_vxlan_port_mirror()):
 *   =========================================================
 *     a) Configure MTP port to add vxlan tunnel header to ingress mirrored packet on source port
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'dump sw mirror'
 *
 *     b) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the vxlan tunnel packet on the MTP
 *
 *     d) Reconfigure the vxlan mirror with payload truncation flag added.
 *
 *     e) Transmit the known unicast packet again. The contents of the packet
 *        are printed on screen.
 *
 *     f) Expected Result:
 *     ===================
 *       We can see the vxlan tunnel packet with payload truncated on the MTP
 */
    

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port; //source port
bcm_port_t egress_port; //egress port
bcm_port_t mtp_port;   //MTP port
bcm_vlan_t ingress_port_vlan = 0x15;
uint32 mirror_flags = BCM_MIRROR_PORT_INGRESS;
bcm_mirror_destination_t mirror_dest_info;

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
    mtp_port = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    bcm_pbmp_t     pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_PBMP_PORT_ADD(pbmp, mtp_port);

    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d and %d", egress_port, mtp_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];
    bcm_gport_t mirror_src_gport;
    bcm_gport_t mirror_dest_id;

    bshell(unit, "vlan show");
    bshell(unit, "dump sw mirror");
    
    bshell(unit, "clear c");
    printf("\n\n\nSending to ingress_port:%d, packet will be forwarded to port %d and mirror to port %d in vxlan format \n", ingress_port, egress_port, mtp_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA00000000BBBB810000150800450000500001000040117497010101010202020200350035003C32F6746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374;  sleep quiet 1", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");

    mirror_dest_id = mirror_dest_info.mirror_dest_id;
    mirror_dest_info.flags =  BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_WITH_ID;
    mirror_dest_info.truncate = BCM_MIRROR_PAYLOAD_TRUNCATE;
    print bcm_mirror_destination_create(unit, mirror_dest_info);
    BCM_GPORT_MODPORT_SET(mirror_src_gport, unit, ingress_port);
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, mirror_src_gport, mirror_flags, mirror_dest_id));

    bshell(unit, "clear c");
    printf("\nSending again and mirrored packet payload will be truncated in the mtp %d\n", mtp_port);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_ports(int unit, int vid, bcm_pbmp_t pbmp)
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

bcm_error_t vxlan_mirror_destination_create(int unit, bcm_gport_t mirror_dest_gport,
                                             bcm_gport_t *mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* ERSPAN related parameters */
    bcm_mac_t vxlan_src_mac = {0x00, 0x00, 0x00, 0x00, 0x06, 0x66};
    bcm_mac_t vxlan_dst_mac = {0x00, 0x00, 0x00, 0x00, 0x09, 0x99};
    uint16 vxlan_tpid = 0x8100;
    bcm_vlan_t vxlan_vid = 200;
    bcm_ip_t vxlan_src_ip = 0x06060606;
    bcm_ip_t vxlan_dst_ip = 0x09090909;
    uint16 vxlan_udp_src_port = 0xffff;
    //uint16 vxlan_udp_dst_port = 4789;
    uint16 vxlan_udp_dst_port = 8472;
    uint32 vxlan_vni = 0x654321;

    bcm_mirror_destination_t_init(mirror_dest_info);
    mirror_dest_info.gport = mirror_dest_gport;
    mirror_dest_info.flags2 = BCM_MIRROR_DEST_FLAGS2_TUNNEL_VXLAN;
    sal_memcpy(mirror_dest_info.src_mac, vxlan_src_mac, 6);
    sal_memcpy(mirror_dest_info.dst_mac, vxlan_dst_mac, 6);
    mirror_dest_info.vlan_id = vxlan_vid;
    mirror_dest_info.tpid = vxlan_tpid;
    mirror_dest_info.version = 4;
    mirror_dest_info.src_addr = vxlan_src_ip;
    mirror_dest_info.dst_addr = vxlan_dst_ip;
    mirror_dest_info.ttl = 16;
    mirror_dest_info.udp_src_port = vxlan_udp_src_port;
    mirror_dest_info.udp_dst_port = vxlan_udp_dst_port;
    mirror_dest_info.vni = vxlan_vni;

    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, mirror_dest_info));
    *mirror_dest_id = mirror_dest_info.mirror_dest_id;
    print *mirror_dest_id;

    return rv;
}



bcm_error_t configure_vxlan_port_mirror(int unit, uint32 mirror_flags, bcm_port_t src_port,
                                         bcm_port_t dest_port, bcm_port_t mtp_port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t src_gport = BCM_GPORT_INVALID, mtp_gport = BCM_GPORT_INVALID;
    bcm_gport_t mirror_dest_id = BCM_GPORT_INVALID;
    int my_modid = 0;

    bcm_pbmp_t pbmp;
    
    BCM_IF_ERROR_RETURN(mirror_system_config(unit));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, src_port);
    BCM_PBMP_PORT_ADD(pbmp, dest_port);
    BCM_IF_ERROR_RETURN(vlan_create_add_ports(unit, ingress_port_vlan, pbmp));

    printf("Add L2 Address for egress port %d.\n", dest_port);
    rv = add_to_l2_table(unit, "00:00:00:00:aa:aa", ingress_port_vlan, dest_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &my_modid));
    /* Get the Mirror To Port GPORT ID */
    BCM_GPORT_MODPORT_SET(mtp_gport, 0, mtp_port);

    /* Create VXLAN mirror destination description */
    BCM_IF_ERROR_RETURN(vxlan_mirror_destination_create(unit, mtp_gport, &mirror_dest_id));

    /* Get the Mirror Port GPORT ID */
    BCM_GPORT_MODPORT_SET(src_gport, unit, src_port);
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, src_gport, mirror_flags, mirror_dest_id));

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

    print "~~~ #2) configure_vxlan_port_mirror(): ** start **";
    if (BCM_FAILURE((rv = configure_vxlan_port_mirror(unit, mirror_flags, ingress_port, egress_port, mtp_port)))) {
        printf("configure_vxlan_port_mirror() failed.\n");
        return -1;
    }
    print "~~~ #2) configure_vxlan_port_mirror(): ** end **";

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
