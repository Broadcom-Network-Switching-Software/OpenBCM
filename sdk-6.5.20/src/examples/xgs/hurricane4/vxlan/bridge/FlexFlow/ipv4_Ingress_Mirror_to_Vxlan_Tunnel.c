/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : HR4, Verifying ingress mirror packet to vxlan tunnel feature
 *
 * Usage    : BCM.0> cint ipv4_Ingress_Mirror_to_Vxlan_Tunnel.c
 *
 * config   : BCM56277_A1-PORT-FLEXFLOW.bcm
 *
 * Log file : ipv4_Ingress_Mirror_to_Vxlan_Tunnel_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |
 *                                  |                      |
 *               mirror source port |                      | Vxlan tunnel port(MTP port)
 *                        --------->|         HR4          |-------->
 *                   ingress packet |                      | tunnel header + Vxlan header + payload
 *                                  |                      |
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
 *     a) Check the configurations by 'dmirror show'
 *
 *     b) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the vxlan tunnel packet on the egress port(MTP port)
 */


/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port; 
bcm_port_t egress_port;
bcm_vlan_t ingress_port_vlan = 0x15;
uint32 mirror_flags = BCM_MIRROR_PORT_INGRESS;

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
	bcm_field_group_t group;
	bcm_field_entry_t entry;

	BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
	BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

	BCM_FIELD_QSET_INIT(qset);
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

	BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

	BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

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
egress_port_setup(int unit, bcm_port_t port)
{
	bcm_field_qset_t qset;
	bcm_field_group_t group;
	bcm_field_entry_t entry;

	BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
	BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

	BCM_FIELD_QSET_INIT(qset);
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

	BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

	BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

	BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
	BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
	BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

	BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

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
	int port_list[2];

	if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
		printf("portNumbersGet() failed\n");
		return -1;
	}

	ingress_port = port_list[0];
	egress_port = port_list[1];

	if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
		printf("ingress_port_setup() failed for port %d\n", ingress_port);
		return -1;
	}

	if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
		printf("egress_port_setup() failed for port %d\n", egress_port);
		return -1;
	}

	bshell(unit, "pw start report +raw +decode");

	return BCM_E_NONE;
}

void
verify(int unit)
{
	char str[256];
	char str1[512];

	bshell(unit, "hm ieee");
	bshell(unit, "vlan show");
	bshell(unit, "dmirror show");
	bshell(unit, "clear c");
	printf("\n\n\nSending L2 unicast packet to ingress_port:%d\n", ingress_port);
	snprintf(str, 256, "tx 1 pbm=%d data=00000000003300000000002281000015002e000000000000000012345678123456791234567a1234567b1234567c1234567d1234567e1234567f12345680123456811234; sleep quiet 1", ingress_port);
	bshell(unit, str);
	bshell(unit, "show c");

	printf("\n\n\nSending ipv4 unicast packet to ingress_port:%d\n", ingress_port);
	snprintf(str1, 512, "tx 1 pbm=%d data=0x00000000AAAA00000000BBBB810000150800450000500001000040117497010101010202020200350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
	bshell(unit, str1);
	bshell(unit, "show c");

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
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchDirectedMirroring, TRUE));
    BCM_IF_ERROR_RETURN(bcm_mirror_init(unit));

    return BCM_E_NONE;
}

bcm_error_t vxlan_mirror_destination_create(int unit, bcm_gport_t mirror_dest_gport,
		                                     bcm_gport_t *mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mirror_destination_t mirror_dest_info;

    /* ERSPAN related parameters */
    bcm_mac_t vxlan_src_mac = {0x00, 0x00, 0x00, 0x00, 0x06, 0x66};
    bcm_mac_t vxlan_dst_mac = {0x00, 0x00, 0x00, 0x00, 0x09, 0x99};
    uint16 vxlan_tpid = 0x8100;
    bcm_vlan_t vxlan_vid = 200;
    bcm_ip_t vxlan_src_ip = 0x06060606;
    bcm_ip_t vxlan_dst_ip = 0x09090909;
    uint16 vxlan_udp_src_port = 0xffff;
    uint16 vxlan_udp_dst_port = 8472;
    uint32 vxlan_vni = 0x654321;

    BCM_IF_ERROR_RETURN(mirror_system_config(unit));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, ingress_port_vlan, ingress_port));

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


bcm_error_t configure_mirror(int unit, bcm_gport_t mirror_src_gport, uint32 mirror_flags,
		                     bcm_gport_t mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* configure port ingress/egress/true egress mirror */
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, mirror_src_gport, mirror_flags, mirror_dest_id));

    return rv;
}

bcm_error_t configure_vxlan_port_mirror(int unit, uint32 mirror_flags, bcm_port_t src_port,
		                                 bcm_port_t dest_port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t src_gport = BCM_GPORT_INVALID, mtp_gport = BCM_GPORT_INVALID;
    bcm_gport_t mirror_dest_id = BCM_GPORT_INVALID;
    bcm_vlan_control_vlan_t control;
    bcm_port_match_info_t match_info;

    /* Get the Mirror To Port GPORT ID */
    BCM_GPORT_MODPORT_SET(mtp_gport, 0, dest_port);

    /* Create VXLAN mirror destination description */
    BCM_IF_ERROR_RETURN(vxlan_mirror_destination_create(unit, mtp_gport, &mirror_dest_id));

    /* Get the Mirror Port GPORT ID */
    BCM_GPORT_MODPORT_SET(src_gport, unit, src_port);
    BCM_IF_ERROR_RETURN(configure_mirror(unit, src_gport, mirror_flags, mirror_dest_id));

   /*
    * VNID field in egress packet is derived from EGR_VLAN_XLATE table, not from FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_ table
    */
    bcm_vlan_control_vlan_t_init(&control);
    control.flags = BCM_VLAN_L2_CLASS_ID_ONLY;
    control.if_class = 100;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_selective_set(unit, ingress_port_vlan, BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK, &control));

    /* assign the vlan class_id as the I2E_CLASS_ID */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlFieldEgressClassSelect, bcmPortEgressClassSelectVlan));

    /*configure the gbp_sid in EVXLT stage. The gbp_sid is retrieved from mirror gport */
    bcm_port_match_info_t_init(&match_info);
    match_info.port = ingress_port;
    /* match_vlan is used as the class_id when the BCM_PORT_MATCH_PORT_I2E_CLASS criteria is used */
    match_info.match_vlan = control.if_class;
    match_info.match = BCM_PORT_MATCH_PORT_I2E_CLASS;
    BCM_IF_ERROR_RETURN(bcm_port_match_add(unit, mirror_dest_id, &match_info));

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
	bshell(unit, "config show; a ; version; cancun stat");

	print "~~~ #1) test_setup(): ** start **";
	if (BCM_FAILURE((rv = test_setup(unit)))) {
		printf("test_setup() failed.\n");
		return -1;
	}
	print "~~~ #1) test_setup(): ** end **";

	print "~~~ #2) configure_vxlan_port_mirror(): ** start **";
	if (BCM_FAILURE((rv = configure_vxlan_port_mirror(unit, mirror_flags, ingress_port, egress_port)))) {
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
