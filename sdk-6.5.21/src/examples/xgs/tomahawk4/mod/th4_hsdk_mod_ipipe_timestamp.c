/*
 * Feature  : This test script includes timestamp in mod packet.
 *            Mirror packets dropped in ingress pipeline for invalid vlan
 *            and IFP drop events.
 *
 * Usage    : BCM.0> cint th4_hsdk_mod_ipipe_timestamp.c
 *
 * config   : th4_hsdk_mod_config.yml
 *
 * Log file : th4_hsdk_mod_ipipe_timestamp_log.txt
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
 *   Cint script to demonstrate ingress mirror the traffic which is dropped on
 *   ingress_port with timestamping.
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
 *    2) Step2 - Configuration (Done in setup_mirror_ingress()):
 *   =========================================================
 *       1) Create mirroring destination descriptor (mirror_dest_id).
 *            - Descriptor contains mirrored traffic destination (gport).
 *       2) Create MOD profile for invalidvlan and fpingdrop event group.
 *       3) Create mirror source of type ingressmod
 *       4) L2 learn mode is set up on ingress and egress ports.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Transmit the packet on the ingress port (ingress_port) and loopback.
 *
 *     Ingress Packet 1:
 *      00 00 00 00 00 22 00 00 00 00 00 11
 *      81 00 00 1A 08 06 00 01 08 00 06 04
 *      00 01 00 00 00 00 00 02 0A 0A 0A 02
 *      00 00 00 00 00 00 0A 0A 0A 03 00 01
 *      02 03 04 05 06 07 08 09 0A 0B 0C 0D BC 94 97 7F
 *
 *     Ingress Packet 1:
 *      00 00 00 00 00 22 00 00 00 00 00 0B
 *      81 00 00 1A 08 06 00 01 08 00 06 04
 *      00 01 00 00 00 00 00 02 0A 0A 0A 02
 *      00 00 00 00 00 00 0A 0A 0A 03 00 01
 *      02 03 04 05 06 07 08 09 0A 0B 0C 0D BC 94 97 7F
 *     b) Expected Result:
 *     ===================
 *     The packet capture shows the egress packet transmitted on outgoing port
 *     (mtp_port) as per the mirror setup.
 *
 *     Egress Packet 1:
 *     00 11 12 13 14 15 00 01 02 03 04 05 81 00 00 05
 *     08 00 45 02 00 88 00 00 40 00 02 11 44 04 02 10
 *     10 A0 11 10 10 A0 00 00 00 00 00 74 00 00 00 0A
 *     00 6C AA BB CC 00 00 00 00 01 00 00 00 64 12 34
 *     00 5C AA BB CC 00 00 00 00 00 00 00 00 00 00 00
 *     00 01 0C 00 00 AE 00 FF 00 40 00 00 00 00 00 22
 *     00 00 00 00 00 11 81 00 00 1A 08 06 00 01 08 00
 *     06 04 00 01 00 00 00 00 00 02 0A 0A 0A 02 00 00
 *     00 00 00 00 0A 0A 0A 03 00 01 02 03 04 05 06 07
 *     08 09 0A 0B 0C 0D BC 94 97 7F
 *     The packet capture shows the egress packet transmitted on outgoing port
 *     (mtp_port) as per the mirror setup.
 *
 *     Egress Packet 2:
 *     00 11 12 13 14 15 00 01 02 03 04 05 81 00 00 05
 *     08 00 45 02 00 88 00 00 40 00 02 11 44 04 02 10
 *     10 A0 11 10 10 A0 00 00 00 00 00 74 00 00 00 0A
 *     00 6C AA BB CC 00 00 00 00 02 00 00 00 64 12 34
 *     00 5C AA BB CC 00 00 00 00 00 00 00 00 00 00 00
 *     00 01 0C 00 00 AE 00 FF 00 40 00 00 00 00 00 22
 *     00 00 00 00 00 0B 81 00 00 1A 08 06 00 01 08 00
 *     06 04 00 01 00 00 00 00 00 02 0A 0A 0A 02 00 00
 *     00 00 00 00 0A 0A 0A 03 00 01 02 03 04 05 06 07
 *     08 09 0A 0B 0C 0D BC 94 97 7F
 */

/* Reset C interpreter*/
cint_reset();

int               num_egress_ports = 2;
bcm_field_group_t group;
bcm_field_entry_t entry[num_egress_ports];
bcm_gport_t       mirror_dest_id;
bcm_port_t        ingress_port = 1;
bcm_port_t        egress_port1 = 2;
bcm_port_t        mtp_port = 3;
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

/*Mod params*/
bcm_mirror_ingress_mod_event_group_t mod_group;
bcm_mirror_source_t source;
int profile_id;

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

/*
 * Configure acl rule to drop ingress packet
 */
bcm_error_t
ingress_port_ifp_drop(int unit, bcm_port_t port)
{
    bcm_field_group_config_t    group_config;
    bcm_field_entry_t entry;
    uint8 mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x0b};
    uint8 mac_mask[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcMac(unit, entry, mac,
                                                 mac_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                  BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/* Update ToD. */
bcm_error_t
setup_tod(int unit)
{
    uint32 stages, secs[2];
    bcm_time_tod_t tod_set;
    bcm_time_ts_counter_t ts_cnt;
    int rv = 0;

    bcm_time_tod_t_init(&tod_set);

    /* To make timestamp work properly, the ToD MUST be updated periodically. */
    /* Usually the ToD should be updated by a thread or something equivalent. */
    /* Some devices e.g. TH4_B0 and TH4G support ToD auto-update, but TH4_A0 does not support auto-update */
    /* The script here is just for testing.*/
    stages = BCM_TIME_STAGE_ALL;
    tod_set.auto_update = 1;
    tod_set.ts.nanoseconds = 0;
    secs[0] =0xaabbcc00;
    secs[1] = 0;
    sal_memcpy(&tod_set.ts.seconds, secs, sizeof(secs));

    rv = bcm_time_tod_set(unit, stages, &tod_set);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_time_tod_set: %s\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Mirror ingress configuration. */
bcm_error_t
setup_mirror_ingress(int unit)
{
    bcm_mirror_destination_t    mirror_dest;
    bcm_gport_t                 mtp_gport;
    bcm_error_t                 rv = BCM_E_NONE;
    uint32                      mirror_flags = 0;
    uint8 dest_mac[6] = {0x00, 0x11, 0x12, 0x13, 0x14, 0x15};
    uint8 src_mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

    bcm_mirror_ingress_mod_event_t events[2] =
                    {bcmMirrorIngModEventInvalidVlan,
                     bcmMirrorIngModEventFpIngDrop};

    bcm_mirror_ingress_mod_event_profile_t event_profile;

    /* Create ingress mirror-on-drop event groups. */
    rv = bcm_mirror_ingress_mod_event_group_create(unit,
                                        events, 2, &mod_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_mirror_ingress_mod_event_group_create: %s\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create ingress mirror-on-drop event profile. */
    bcm_mirror_ingress_mod_event_profile_t_init(&event_profile);
    event_profile.reason_code = 0xc;
    event_profile.sample_rate = 0xFFFFFFFF;
    event_profile.priority = 3;

    rv = bcm_mirror_ingress_mod_event_profile_create(unit,
                                    &event_profile, &profile_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mirror_ingress_mod_event_profile_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add event groups to profile. */
    rv = bcm_mirror_ingress_mod_event_profile_group_add(unit,
                                        profile_id, mod_group, 1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mirror_ingress_mod_event_profile_group_add: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Get the GPORT ID for the MTP port. */
    bcm_port_gport_get(unit, mtp_port, &mtp_gport);

    printf("Mirror destination create\n");
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags  = BCM_MIRROR_DEST_TUNNEL_WITH_SEQ;
    mirror_dest.flags2 = BCM_MIRROR_DEST_FLAGS2_MOD_TUNNEL_PSAMP;
    mirror_dest.gport  = mtp_gport;
    sal_memcpy(mirror_dest.dst_mac, dest_mac, 6);
    sal_memcpy(mirror_dest.src_mac, src_mac, 6);
    mirror_dest.tpid=0x8100;
    mirror_dest.vlan_id=5;
    mirror_dest.version=4;
    mirror_dest.src_addr=0x021010a0;
    mirror_dest.dst_addr=0x111010a0;
    mirror_dest.ttl=2;
    mirror_dest.tos=2;
    mirror_dest.df=1;
    mirror_dest.ipfix_version = 0xA;
    mirror_dest.initial_seq_number = 0x1;
    mirror_dest.meta_data = 0xae;
    mirror_dest.meta_data_type = bcmMirrorPsampFmt2HeaderUserMeta;
    mirror_dest.timestamp_mode = bcmMirrorTimestampModePTP;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror dest create: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    mirror_dest_id = mirror_dest.mirror_dest_id;

    /*set up Ipipe mod destination*/
    bcm_mirror_source_t_init(&source);
    source.type = bcmMirrorSourceTypeIngressMod;
    rv = bcm_mirror_source_dest_add(unit, &source, mirror_dest_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_mirror_source_dest_add: %s\n",bcm_errmsg(rv));
        return rv;
    }

    /* setup ifp drop for SrcMac 0xb */
    rv = ingress_port_ifp_drop(unit, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in ingress_port_ifp_drop: %s\n", bcm_errmsg(rv));
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

    rv = setup_tod(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in setup_tod: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Cleanup configurations. */
bcm_error_t
cleanup_mirror_ingress(int unit)
{
    bcm_error_t                 rv = BCM_E_NONE;
    uint32                      mirror_flags = 0;

    /* Remove group from profile. */
    rv = bcm_mirror_ingress_mod_event_profile_group_delete(unit,
                                         profile_id, mod_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mirror_ingress_mod_event_profile_group_delete: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Remove mod profile*/
    rv = bcm_mirror_ingress_mod_event_profile_destroy(unit,profile_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mirror_ingress_mod_event_profile_destroy: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Remove ingress mirror-on-drop event groups. */
    rv = bcm_mirror_ingress_mod_event_group_destroy(unit,mod_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_mirror_ingress_mod_event_group_destroy: %s\n",bcm_errmsg(rv));
        return rv;
    }

    /*Remove ingress mod*/
    rv = bcm_mirror_source_dest_delete(unit, &source, mirror_dest_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_mirror_source_dest_delete: %s\n",bcm_errmsg(rv));
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
        printf("egress_port_multi_setup() failed for port %d\n", egress_port1);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

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

    if (BCM_E_NONE != setup_mirror_ingress(unit)) {
        printf("setup_mirror_ingress() failed.\n");
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

    if (BCM_E_NONE != cleanup_mirror_ingress(unit)) {
        printf("cleanup_mirror_ingress() failed.\n");
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
    /*Packet pattern maybe different based on timestamp generated*/
    for (i = 0; i < packet_patterns; i++) {
        if (BCM_PBMP_MEMBER(expected_pbmp[i], src_port)) {

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
unsigned char expected_egress_packet1[154] = {
        0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x81, 0x00, 0x00, 0x05,
        0x08, 0x00, 0x45, 0x02, 0x00, 0x88, 0x00, 0x00, 0x40, 0x00, 0x02, 0x11, 0x44, 0x04, 0x02, 0x10,
        0x10, 0xA0, 0x11, 0x10, 0x10, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x0A,
        0x00, 0x6C, 0xAA, 0xBB, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x64, 0x12, 0x34,
        0x00, 0x5C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x0C, 0x00, 0x00, 0xAE, 0x00, 0xFF, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00, 0x00, 0x1A, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00,
        0x06, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0xBC, 0x94, 0x97, 0x7F
};

unsigned char expected_egress_packet2[154] = {
        0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x81, 0x00, 0x00, 0x05,
        0x08, 0x00, 0x45, 0x02, 0x00, 0x88, 0x00, 0x00, 0x40, 0x00, 0x02, 0x11, 0x44, 0x04, 0x02, 0x10,
        0x10, 0xA0, 0x11, 0x10, 0x10, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x0A,
        0x00, 0x6C, 0xAA, 0xBB, 0xCC, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x64, 0x12, 0x34,
        0x00, 0x5C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x0C, 0x00, 0x00, 0xAE, 0x00, 0xFF, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x81, 0x00, 0x00, 0x1A, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00,
        0x06, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0xBC, 0x94, 0x97, 0x7F
};

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
    int                         length_mtp = 154;
    int                         rv;

    rx_count = 0;
    test_failed = 0;
    expected_rx_count = 2;

    expected_packets[0]  = expected_egress_packet1;
    expected_packets[1]  = expected_egress_packet2;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], mtp_port);
    BCM_PBMP_CLEAR(expected_pbmp[1]);
    BCM_PBMP_PORT_ADD(expected_pbmp[1], mtp_port);

    if (debug) {
        bshell(unit, "mirror show");
        bshell(unit, "dump sw mirror");
        bshell(unit, "bsh -c 'lt MIRROR_CONTROL traverse -l'");
    }

    printf("Verify known unicast pkt from port %d with invalid vlan \n",
                                                ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000000220000000000118100001A080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 3",count, ingress_port);
    bshell(unit, str);

    printf("Verify known unicast pkt from port %d matching acl rule drop \n",
                                                ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x00000000002200000000000B8100001A080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 3",count, ingress_port);
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
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port1: Packet verify failed. Expected packet stat %d but get %d\n",
                                   count,
                                   COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port1: Packet verify failed. Expected bytes stat 0 but get %d\n",
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

    if (COMPILER_64_LO(counter_value.value[0]) != 2) {
        printf("mtp_port: Packet verify failed. Expected packet stat %d but get %d\n",
                                   count,
                                   COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    }

    if (COMPILER_64_LO(counter_value.value[1]) != (length_mtp+4)*2) {
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
 *  b) Actual configuration (done in setup_mirror_ingress())
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
