/*
 * Feature  : Mirror packets ingressing on a port with PSAMP
 *            IPv6 encapsulation.
 *
 * Usage    : BCM.0> cint th4_hsdk_mirror_psamp_ipv6.c
 *
 * config   : th4_hsdk_mirroring_config.yml
 *
 * Log file : th4_hsdk_mirror_psamp_ipv6_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |                   |  egress_port1 (switch port)
 *                      |                   +-------------->
 *                      |                   |  VLAN 20
 *        ingress_port  |                   |
 *       -------------->+       TH4         |
 *        VLAN 20       |                   |
 *                      |                   |  mtp_port (MTP)
 *                      |                   +-------------->
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *       L2 entry : dst_mac=0x00:00:00:00:00:22, VLAN=20 --> PORT=egress_port1
 *
 * Summary:
 * ========
 *   Cint script to demonstrate ingress mirror the traffic ingressing on
 *   ingress_port to the mirror-to-port (mtp_port) with PSAMP
 *   IPv6 encapsulation.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects 3 ports and configure them in Loopback mode. Out of these
 *        3 ports, one port is used as ingress port and the other two ports as
 *        egress ports.
 *
 *     b) Install an IFP rule to copy egress packets to CPU and start
 *        packet watcher.
 *
 *     c) Add an entry in the L2 entry table for MAC address "00:00:00:00:00:22"
 *        in VLAN 20 to switch packet to egress port (egress_port1).
 *
 *     c) Configuration (Done in setup_mirror_psamp()):
 *   =========================================================
 *       c.1) Create mirroring destination descriptor (mirror_dest_id) with
 *            PSAMP encapsulation.
 *            - Descriptor contains mirrored traffic destination (gport) and
 *              mirrored traffic encapsulation.
 *       c.2) Add mirroring destination to the ingress port.
 *       c.3) L2 learn mode is set up on ingress and egress ports.
 *
 *   2) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Transmit the packet on the ingress port (ingress_port) and loopback.
 *
 *     Ingress Packet:
 *       0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
 *       0x81, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04,
 *       0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02,
 *       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01,
 *       0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
 *       0xbc, 0x94, 0x97, 0x7f
 *
 *     b) Expected Result:
 *     ===================
 *     The packet capture shows the egress packet transmitted on outgoing port
 *     (egress_port1) as per the forwarding path setup.
 *
 *     Egress Packet 1:
 *       0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
 *       0x81, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04,
 *       0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02,
 *       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01,
 *       0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
 *       0xbc, 0x94, 0x97, 0x7f
 *
 *     The packet capture shows the egress packet transmitted on outgoing port
 *     (mtp_port) as per the mirror PSAMP setup.
 *
 *     Egress Packet 2:
 *       0x00, 0x00, 0x00, 0x01, 0x02, 0x33, 0x00, 0x00, 0x00, 0x01, 0x02, 0x44,
 *       0x81, 0x00, 0x00, 0x0a, 0x86, 0xdd, 0x60, 0x00, 0x00, 0x00, 0x00, 0x68,
 *       0x11, 0x05, 0x01, 0x01, 0x01, 0x01, 0x35, 0x00, 0x70, 0x00, 0x00, 0x00,
 *       0x10, 0x09, 0x08, 0x07, 0x06, 0x05, 0x02, 0x02, 0x02, 0x02, 0x36, 0x00, 
 *       0x70, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x12, 0x34,
 *       0x43, 0x21, 0x00, 0x68, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x60, 0x00, 0x00,
 *       0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x78, 0x00, 0x28,
 *       0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
 *       0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00,
 *       0x00, 0x11, 0x81, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00,
 *       0x06, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x0a,
 *       0x0a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x0a, 0x03, 
 *       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
 *       0x0c, 0x0d, 0xbc, 0x94, 0x97, 0x7f
 */

/* Reset C interpreter*/
cint_reset();

int               num_egress_ports = 2;
bcm_field_group_t group;
bcm_field_entry_t entry[num_egress_ports];
bcm_gport_t       mirror_dest_id;
bcm_port_t        ingress_port;
bcm_port_t        egress_port1;
bcm_port_t        mtp_port;
bcm_vlan_t        switch_vlan=20;
bcm_pbmp_t        ing_pbmp, egr_pbmp;
bcm_pbmp_t        pbmp, ubmp;
uint32_t          stat_counter_id;
const uint8       priority = 101;

unsigned char    *expected_packets[2];
bcm_pbmp_t        expected_pbmp[2];
uint32            packet_patterns = 2;
int               test_failed;         /* Final result will be stored here */
int               rx_count;            /* Global received packet count */

int               debug = 0;    /* Make it '1' to dump debug informations */

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again.
 */
int checkPortAssigned(int *port_index_list, int no_entries, int index)
{
    int                i=0;

    for (i= 0; i < no_entries; i++) {
        if (port_index_list[i] == index) {
            return 1;
        }
    }

    /* no entry found */
    return 0;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports.
 */
bcm_error_t portNumbersGet(int unit, int *port_list_ptr, int num_ports)
{
    bcm_port_config_t  configP;
    bcm_pbmp_t         ports_pbmp;
    int                tempports[BCM_PBMP_PORT_MAX];
    int                port_index_list[num_ports];
    int                i = 0, port = 0, rv = 0, index = 0;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",
                bcm_errmsg(rv));
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
            port_list_ptr[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode.
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t         port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet from the egress port.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_config_t    group_config;
    bcm_port_t                  port;
    int                         i = 0;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    group = group_config.group;

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry[i]));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry[i], port,
                                                  BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry[i],
                                                 bcmFieldActionCopyToCpu,
                                                 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry[i],
                                                 bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry[i]));

        i++;
    }

    return BCM_E_NONE;
}

bcm_error_t
ingress_port_multi_cleanup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t         port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_cleanup port: %d cleanup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_NONE));
    }

    return BCM_E_NONE;
}

bcm_error_t
egress_port_multi_cleanup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t         port;
    int                rv, i;

    rv = bcm_field_group_remove(unit, group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_remove: %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (i = 0; i < num_egress_ports; i++) {
        rv = bcm_field_entry_destroy(unit, entry[i]);
        if (BCM_FAILURE(rv)) {
            printf("bcm_field_entry_destroy: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_field_group_destroy(unit, group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_destroy: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_cleanup port: %d cleanup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_NONE));
    }

    return BCM_E_NONE;
}


/* Register callback function for received packets. */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32       flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktioRxCallback, priority, &rx_count, flags);
}

bcm_error_t
unregisterPktioRxCallback(int unit)
{
    return bcm_pktio_rx_unregister(unit, pktioRxCallback, priority);
}

bcm_error_t
setup_flex_ctr(int unit)
{
    bcm_flexctr_index_operation_t  *index_op = NULL;
    bcm_flexctr_value_operation_t  *value_a_op = NULL;
    bcm_flexctr_value_operation_t  *value_b_op = NULL;
    bcm_flexctr_action_t            action = {0};
    int                             options = 0, rv;

    /* Counter index is egress port. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrEgressPort;
    index_op->mask_size[0] = 9;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    action.source = bcmFlexctrSourceEgrPort;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;
    action.index_num = 512;
    /* Enable to count egress mirrored packets. */
    action.egress_mirror_count_enable=1;

    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("stat_counter_id = 0x%x\n", stat_counter_id);

    /* Attach counter action to egress ports. */
    rv = bcm_port_stat_attach(unit, egress_port1, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_stat_attach(unit, mtp_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
cleanup_flex_ctr(int unit)
{
    int                rv;

    /* Detach counter action. */
    rv = bcm_port_stat_detach_with_id(unit, egress_port1, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_stat_detach_with_id: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_stat_detach_with_id(unit, mtp_port, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_stat_detach_with_id: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Destroy counter action. */
    rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Mirror ingress PSAMP configuration. */
bcm_error_t
setup_mirror_psamp(int unit)
{
    bcm_mirror_destination_t    mirror_dest;
    bcm_gport_t                 mtp_gport;
    bcm_error_t                 rv = BCM_E_NONE;
    uint32                      mirror_flags = 0;

    bcm_ip6_t     src_ip6 = {0x1,1,1,1,0x35,0,0x70,0,0,0,0x10,0x9,8,7,6,5};
    bcm_ip6_t     dst_ip6  = {0x2,2,2,2,0x36,0,0x70,0,0,0,0x1,0x2,3,4,5,6};
    bcm_vlan_t    vid = 10;
    uint16        tpid = 0x8100;
    uint16        udp_src_port = 0x1234;
    uint16        udp_dst_port = 0x4321;

    /* Get the GPORT ID for the MTP port. */
    bcm_port_gport_get(unit, mtp_port, &mtp_gport);

    /* Copy packet samples to mirror destinations */
    print bcm_port_control_set(unit, ingress_port,
                               bcmPortControlSampleIngressDest,
                               BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR);

    /* Port Sampling Rate Configuration */
    /* Set the threshold value of Ingress sampler as 1 for ingress port */
    print bcm_port_control_set(unit, ingress_port,
                               bcmPortControlSampleIngressRate, 1);

    print bcm_port_control_set(unit, ingress_port,
                               bcmPortControlSampleIngressEnable, TRUE);

    printf("PSAMP mirror destination create\n");
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags    |= BCM_MIRROR_DEST_TUNNEL_PSAMP;
    mirror_dest.gport     = mtp_gport;
    mirror_dest.dst_mac   = "00:00:00:01:02:33";
    mirror_dest.src_mac   = "00:00:00:01:02:44";
    mirror_dest.tpid      = tpid;
    mirror_dest.vlan_id   = vid;
    mirror_dest.version   = 6;
    mirror_dest.src6_addr = src_ip6;
    mirror_dest.dst6_addr = dst_ip6;
    mirror_dest.ttl       = 5;
    mirror_dest.udp_src_port = udp_src_port;
    mirror_dest.udp_dst_port = udp_dst_port;
    mirror_dest.initial_seq_number = 0xFF;
    mirror_dest.psamp_epoch = 0x1;
    mirror_dest.ipfix_version = 0xA;
    mirror_dest.template_id = 0x28;
    mirror_dest.meta_data = 0x1234;
    mirror_dest.meta_data_type = bcmMirrorPsampFmt2HeaderUserMeta;
    mirror_dest.observation_domain = 0x78;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror dest create: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    mirror_dest_id = mirror_dest.mirror_dest_id;

    mirror_flags |= BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_SFLOW |
                    BCM_MIRROR_PORT_PSAMP;
    printf("Mirror port destination add\n");
    rv = bcm_mirror_port_dest_add(unit, ingress_port,
                                  mirror_flags,
                                  mirror_dest_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror port dest add: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Learn mode port config. */
    rv = port_learn_mode_set(unit, ingress_port,
                             BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Drop egress_port received packet to avoid looping back. */
    rv = port_learn_mode_set(unit, egress_port1, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = port_learn_mode_set(unit, mtp_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/* Cleanup configurations. */
bcm_error_t
cleanup_mirror_psamp(int unit)
{
    bcm_error_t                 rv = BCM_E_NONE;
    uint32                      mirror_flags = 0;

    /* Delete mirroring destination from a port. */
    mirror_flags |= BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_SFLOW |
                    BCM_MIRROR_PORT_PSAMP;
    rv = bcm_mirror_port_dest_delete(unit, ingress_port, mirror_flags,
                                     mirror_dest_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mirror_port_dest_delete: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Remove mirror destination descriptor. */
    rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mirror_destination_destroy: %s\n",
                                        bcm_errmsg(rv));
        return rv;
    }

    /* Restore Learn mode port config. */
    rv = port_learn_mode_set(unit, ingress_port,
                             BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = port_learn_mode_set(unit, egress_port1,
                             BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = port_learn_mode_set(unit, mtp_port,
                             BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    bcm_mac_t          dst_mac = "00:00:00:00:00:22";
    int                num_ports = 3;
    int                port_list[num_ports];
    int                rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    mtp_port = port_list[2];

    printf("Ingress port : %d\n", ingress_port);
    printf("Egress port1: %d\n", egress_port1);
    printf("MTP port: %d\n", mtp_port);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);

    BCM_PBMP_CLEAR(ubmp);

    /* Create a VLAN. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, switch_vlan));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, switch_vlan, pbmp, ubmp));

    /* Set port default VLAN id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port,
                                                   switch_vlan));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1,
                                                   switch_vlan));

    BCM_PBMP_CLEAR(ing_pbmp);
    BCM_PBMP_PORT_ADD(ing_pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, ing_pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(egr_pbmp);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(egr_pbmp, mtp_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, egr_pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure static L2 entry for forwarding. */
    rv = add_l2_static_entry(unit, 0, dst_mac, switch_vlan, egress_port1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in add_l2_static_entry: %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_E_NONE != setup_mirror_psamp(unit)) {
        printf("setup_mirror_psamp() failed.\n");
        return -1;
    }

    if (BCM_E_NONE != setup_flex_ctr(unit)) {
        printf("setup_flex_ctr() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}


/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    if (BCM_E_NONE != cleanup_flex_ctr(unit)) {
        printf("cleanup_flex_ctr() failed.\n");
        return -1;
    }

    if (BCM_E_NONE != cleanup_mirror_psamp(unit)) {
        printf("cleanup_mirror_psamp() failed.\n");
        return -1;
    }

    /* Clean up learned L2 entries. */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, switch_vlan, 0));

    /* Set default vlan. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, 1));

    /* Delete vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, switch_vlan, pbmp));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, switch_vlan));

    BCM_IF_ERROR_RETURN(ingress_port_multi_cleanup(unit, ing_pbmp));
    BCM_IF_ERROR_RETURN(egress_port_multi_cleanup(unit, egr_pbmp));

    bshell(unit, "pw stop");
    unregisterPktioRxCallback(unit);

    return BCM_E_NONE;
}

/* Static L2 entry for forwarding. */
bcm_error_t
add_l2_static_entry(int unit, uint32 flag, bcm_mac_t mac, bcm_vpn_t vpn_id,
                    bcm_port_t port)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC | flag;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/* Setup learning on port. */
bcm_error_t
port_learn_mode_set(int unit, bcm_port_t port, uint32 flags)
{
    bcm_error_t        rv = BCM_E_NONE;

    rv = bcm_port_control_set(unit, port, bcmPortControlL2Learn, flags);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_control_set(L2Learn): %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_control_set(unit, port, bcmPortControlL2Move, flags);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_control_set(L2Move): %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Rx callback function for applications using the HSDK PKTIO. */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                i, *count = (auto) cookie;
    void              *buffer;
    uint32             length;
    bcm_port_t         src_port;
    uint32             port;

    /* Get basic packet info. */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer,
                                           &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    /* Get source port metadata. */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get (unit, packet,
                                                 bcmPktioPmdTypeRx,
                                                 BCM_PKTIO_RXPMD_SRC_PORT_NUM,
                                                 &port));

    printf("pktioRxCallback: received from port 0x%x\n", port);

    src_port = port;

    for (i = 0; i < packet_patterns; i++) {
        if ((sal_memcmp(buffer, expected_packets[i], length) == 0) &&
            BCM_PBMP_MEMBER(expected_pbmp[i], src_port)) {

            (*count)++; /* Bump up the rx count */
            printf("pktioRxCallback: rx_count = 0x%x\n", *count);

            /* Remove from bitmap to make sure no duplicates. */
            BCM_PBMP_PORT_REMOVE(expected_pbmp[i], src_port);
        }
    }
    if ((i == packet_patterns) && (count == 0)) {
        test_failed = 1;   /* An unexpected packet. */
    }

    return BCM_PKTIO_RX_NOT_HANDLED;
}


/*Expected egress packets */
unsigned char expected_egress_packet1[64] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
        0x81, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01,
        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
        0xbc, 0x94, 0x97, 0x7f};

unsigned char expected_egress_packet2[162] = {
        0x00, 0x00, 0x00, 0x01, 0x02, 0x33, 0x00, 0x00, 0x00, 0x01, 0x02, 0x44,
        0x81, 0x00, 0x00, 0x0a, 0x86, 0xdd, 0x60, 0x00, 0x00, 0x00, 0x00, 0x68,
        0x11, 0x05, 0x01, 0x01, 0x01, 0x01, 0x35, 0x00, 0x70, 0x00, 0x00, 0x00,
        0x10, 0x09, 0x08, 0x07, 0x06, 0x05, 0x02, 0x02, 0x02, 0x02, 0x36, 0x00, 
        0x70, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x12, 0x34,
        0x43, 0x21, 0x00, 0x68, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x60, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x78, 0x00, 0x28,
        0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
        0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x11, 0x81, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00,
        0x06, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0a, 0x0a,
        0x0a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x0a, 0x03, 
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0xbc, 0x94, 0x97, 0x7f};

/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify.
 */
bcm_error_t testVerify(int unit)
{
    bcm_flexctr_counter_value_t counter_value;
    char                        str[512];
    uint32                      num_entries = 1;
    uint32                      counter_index;
    int                         expected_rx_count, pass;
    int                         flexCtr_pass = 1;
    int                         count = 1;
    int                         length = 64;
    int                         length_mtp = 162;
    int                         rv;

    rx_count = 0;
    test_failed = 0;
    expected_rx_count = 2;

    expected_packets[0]  = expected_egress_packet1;
    expected_packets[1]  = expected_egress_packet2;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);
    BCM_PBMP_CLEAR(expected_pbmp[1]);
    BCM_PBMP_PORT_ADD(expected_pbmp[1], mtp_port);

    if (debug) {
        bshell(unit, "mirror show");
        bshell(unit, "dump sw mirror");
        bshell(unit, "bsh -c 'lt MIRROR_CONTROL traverse -l'");
    }

    printf("Verify known unicast pkt from port %d forwarding to the egress port %d \n",
                                   ingress_port, egress_port1);
    snprintf(str, 512, "tx %d pbm=%d data=0x00000000002200000000001181000014080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 3", 1, ingress_port);
    bshell(unit, str);

    /* Get counter value for egress_port1 */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port1;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Print counter value. */
    printf("FlexCtr Get on port_egress1 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port1: Packet verify failed. Expected packet stat %d but get %d\n",
                                   count,
                                   COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != (length+4)) {
        printf("Egress_port1: Packet verify failed. Expected bytes stat %d but get %d\n",
                                   (length+4),
                                   COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for mtp_port. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = mtp_port;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Print counter value. */
    printf("FlexCtr Get on port_mtp_port : %d packets / %d bytes\n",
                                   COMPILER_64_LO(counter_value.value[0]),
                                   COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("mtp_port: Packet verify failed. Expected packet stat %d but get %d\n",
                                   count,
                                   COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    }

    if (COMPILER_64_LO(counter_value.value[1]) != (length_mtp+4)) {
        printf("mtp_port: Packet verify failed. Expected bytes stat %d but get %d\n",
                                   (length_mtp+4),
                                   COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    }

    pass = ((test_failed == 0) && (rx_count == expected_rx_count) &&
            (flexCtr_pass));
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF INGRESS MIRROR VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test = [%s]", pass? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (pass)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}


/*
 * execute:
 *  This function does the following
 *  a) Test setup
 *  b) Actual configuration (done in setup_mirror_psamp())
 *  c) Demonstrates the functionality (done in verify())
 */
bcm_error_t
execute(void)
{
    int                unit = 0;
    int                rv;

    if (debug) {
        bshell(unit, "config show; attach ; version");
    }

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) verify(): ** start **";
    testVerify(unit);
    print "~~~ #2) verify(): ** end **";

    print "~~~ #3) test_cleanup(): ** start **";
    if (BCM_FAILURE((rv = test_cleanup(unit)))) {
        printf("test_cleanup() failed.\n");
        return -1;
    }
    print "~~~ #3) test_cleanup(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}
