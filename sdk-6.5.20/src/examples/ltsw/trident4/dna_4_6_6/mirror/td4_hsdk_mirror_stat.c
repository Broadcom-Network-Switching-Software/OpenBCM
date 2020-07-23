/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880), Verifying ingress/egress mirror packet stats. 
 *
 * Usage    : BCM.0> cint TD4_HSDK_Mirror_stat.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_Mirror_stat_log.txt
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
 * Cint example to demonstrate mirroring stat
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *

 *     b) Configure L2 flow
 * 
 *     c) Create a mirror destination for MTP port
 *
 *     d) Configure ingress and egress mirror to that MTP.
 *
 *   3) Step3 - Verification (Done in Verify()):
 *   ===========================================
 *
 *     a) Transmit the packet. The packet is expected to be broadcast to egress port.
 *        The contents of the packet are printed on screen.
 *
 *     b) Expected Result:
 *     ===================
 *       We can see both ingress mirror stat and egress mirror stat. 
 *       "show c" will prove two packets are mirrored out from MTP
 */


/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port; //source port
bcm_port_t egress_port; 
bcm_port_t mtp_port; //MTP port
bcm_vlan_t forward_vlan = 0x15;
uint32 ingress_mirror_flags = BCM_MIRROR_PORT_INGRESS;
uint32 egress_mirror_flags = BCM_MIRROR_PORT_EGRESS;
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
 * Configures the port in loopback mode 
 */

bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    
    printf("ingress_port_setup port: %d setup\n", port);
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and use IFP to drop loopback traffic
 */

bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;
    
    printf("egress_port_setup port: %d setup\n", port);
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 3;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionDrop);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit,gcfg1.group,&entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */

bcm_error_t
mtp_port_setup(int unit, bcm_port_t port)
{
    printf("mtp_port_setup port: %d setup\n", port);
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    printf("Block traffic ingress from MTP to avoid continuous traffic. This is for test purpose\n");
    BCM_IF_ERROR_RETURN(bcm_port_stp_set(unit, port, BCM_STG_STP_BLOCK));
    
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

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    if (BCM_E_NONE != mtp_port_setup(unit, mtp_port)) {
        printf("mtp_port_setup() failed for port %d\n", mtp_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}
    
void
verify(int unit)
{
    char str[512];
    uint64 ingress_mirror_val;
    uint64 egress_mirror_val;
    uint64 out_pkt;    
    int rv = BCM_E_NONE;

    bshell(unit, "vlan show");
    bshell(unit, "clear c");
    printf("\n\nSending packet to ingress_port:%d, broadcast to egress_port %d, expected to be ingress and egress mirrored to port %d\n", ingress_port, egress_port, mtp_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA00000000BBBB810000150800450000500001000040117497010101010202020200350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
    bshell(unit, str);
    rv = get_mirror_stat(unit, mirror_dest_id, &ingress_mirror_val, &egress_mirror_val);
    if(rv == BCM_E_NONE)
    {
        printf("\n\n======== Verify Mirror Result========\n");
        printf("Ingress Mirror counter get: %d\n", COMPILER_64_LO(ingress_mirror_val));
        printf("Egress Mirror counter get:  %d\n", COMPILER_64_LO(egress_mirror_val));        
    }
    else
    {
        printf("Error in get_mirror_stat: %s\n", bcm_errmsg(rv));
    }
    
    COMPILER_64_ZERO(out_pkt);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, mtp_port, snmpDot1dTpPortOutFrames, &out_pkt));
    printf("Mirrored out : %d packets from MTP: %d\n", COMPILER_64_LO(out_pkt), mtp_port);
    if (COMPILER_64_LO(out_pkt) != 2) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
        return -1;
    }
    else {
        printf("Packet verify OK.\n\n");
    }        
    bshell(unit, "show c");        

    return;
}
    
/* Create vlan and add port to vlan */
int
vlan_create_add_pbmp(int unit, int vid, bcm_pbmp_t pbmp)
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

bcm_error_t configure_mirror(int unit, bcm_gport_t mirror_src_gport, uint32 mirror_flags,
                             bcm_gport_t mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;

    /* configure port ingress/egress/true egress mirror */
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, mirror_src_gport, mirror_flags, mirror_dest_id));

    return rv;
}

bcm_error_t configure_port_mirror(int unit, bcm_port_t src_port,
                                  bcm_port_t dest_port, bcm_port_t mtp_port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t src_gport = BCM_GPORT_INVALID, mtp_gport = BCM_GPORT_INVALID, dest_gport = BCM_GPORT_INVALID;
    bcm_mirror_destination_t mirror_dest_info;
    int modid;
    bcm_pbmp_t pbmp;
    
    BCM_IF_ERROR_RETURN(mirror_system_config(unit));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, src_port);
    BCM_PBMP_PORT_ADD(pbmp, dest_port);
        
    BCM_IF_ERROR_RETURN(vlan_create_add_pbmp(unit, forward_vlan, pbmp));

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &modid));
    /* Get the Mirror To Port GPORT ID */
    BCM_GPORT_MODPORT_SET(mtp_gport, modid, mtp_port);

    bcm_mirror_destination_t_init(mirror_dest_info);
    mirror_dest_info.gport = mtp_gport;

    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, mirror_dest_info));
    mirror_dest_id = mirror_dest_info.mirror_dest_id;

    /* Get the Mirror Port GPORT ID */
    BCM_GPORT_MODPORT_SET(src_gport, unit, src_port);
    BCM_GPORT_MODPORT_SET(dest_gport, unit, dest_port);
     
    /* configure port ingress/egress/true egress mirror */
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, src_gport, ingress_mirror_flags, mirror_dest_id));
    BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, dest_gport, egress_mirror_flags, mirror_dest_id));

    return rv;
}

bcm_error_t get_mirror_stat(int unit, bcm_gport_t mirror_dest_id, uint64 *ingress_mirror_val, uint64 *egress_mirror_val)
{
    bcm_mirror_stat_object_t object;
    bcm_mirror_stat_object_t_init(&object);
    object.source_flags = BCM_MIRROR_SOURCE_INGRESS;
    object.mirror_dest_id = mirror_dest_id;
    BCM_IF_ERROR_RETURN(bcm_mirror_stat_get(unit, &object, bcmMirrorStatPackets, ingress_mirror_val));

    object.source_flags = BCM_MIRROR_SOURCE_EGRESS;
    object.mirror_dest_id = mirror_dest_id;
    BCM_IF_ERROR_RETURN(bcm_mirror_stat_get(unit, &object, bcmMirrorStatPackets, egress_mirror_val));
    
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
    if (BCM_FAILURE((rv = configure_port_mirror(unit, ingress_port, egress_port, mtp_port)))) {
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
