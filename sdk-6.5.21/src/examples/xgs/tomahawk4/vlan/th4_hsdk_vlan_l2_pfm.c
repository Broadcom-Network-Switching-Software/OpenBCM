/*  Feature  : VLAN L2 pfm
 *
 *  Usage    : BCM.0> cint th4_hsdk_vlan_l2_pfm.c
 *
 *  config   : th4_hsdk_vlan_config.yml
 *
 *  Log file : th4_hsdk_vlan_l2_pfm_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+  egress_port1
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              |
 *  ingress_port     |              TH4             |  egress_port2
 *  +----------------+                              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |  egress_port3
 *                   |                              +-----------------+
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT configures L2 multicast using BCM APIs.
 *    This example shows the steps to set up L2 multicast along with Port
 *    Filtering Mode(PFM) on a per Vlan basis. The PFM controls the forwarding of
 *    known and unknown multicast packets.
 *
 *    PFM
 *    ===
 *    0 - All multicast packets are flooded to the entire Vlan
 *    1 - Known multicast packets are forwarded only to the ports in the
 *        multicast group. Unknown multicast packets are flooded to the Vlan.
 *    2 - Known multicast packets are forwarded only to the ports in the
 *        multicast group. Unknown multicast packets are dropped.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      1.1) Configure a vlan with 4 ports
 *      1.2) Configure a L2MC group egressing 2 egress ports
 *      1.3) Configure a known <vlan, MC-MAC> pointing to the L2MC group
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *      a) for each PFM mode = 0, 1, or 2 {
 *             set up PFM mode on the test vlan
 *             forward known multicast and unknown multicast in the vlan
 *         }
 *      b) Expected Result:
 *         ================
 *         For each ingress packet
 *         b.1) verify flexCtr on each egress port
 *         b.2) verify egress packets on each egress port
*/

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2, egress_port3;
bcm_pbmp_t ing_pbmp, egr_pbmp;
bcm_pbmp_t pbmp, ubmp;
bcm_field_group_t group;
int num_egress_ports = 3;
bcm_field_entry_t entry[num_egress_ports];
uint32_t stat_counter_id;
bcm_vlan_t vlan = 2;
bcm_multicast_t mc_group;


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
bcm_error_t portNumbersGet(int unit, int *port_list_ptr, int num_ports)
{
    int i = 0, port = 0, rv = 0, index = 0;
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
            port_list_ptr[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t     port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
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
    bcm_port_t     port;
    bcm_field_group_config_t group_config;
    int i = 0;
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

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry[i]));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry[i], port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry[i], bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry[i], bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry[i]));
        i++;
    }

    return BCM_E_NONE;
}

bcm_error_t
ingress_port_multi_cleanup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t     port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_cleanup port: %d cleanup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE));
    }

    return BCM_E_NONE;
}

bcm_error_t
egress_port_multi_cleanup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t     port;
    int            rv, i;

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
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE));
    }
    return BCM_E_NONE;
}


int rx_count;   /* Global received packet count */
int test_failed; /*Final result will be stored here */

unsigned char *expected_packets[1];
bcm_pbmp_t expected_pbmp[1];
uint32 packet_patterns = 1;


/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                 i, *count = (auto) cookie;
    void               *buffer;
    uint32              length;
    bcm_port_t          src_port;
    uint32              port;

    /* Get basic packet info */
    if (BCM_FAILURE(bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
        return BCM_PKTIO_RX_NOT_HANDLED;
    }
    /* Get source port metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_SRC_PORT_NUM, &port));

    printf("pktioRxCallback: received from port %d\n", port);

    src_port = port;

    for (i = 0; i < packet_patterns; i++) {
        if (sal_memcmp(buffer, expected_packets[i], length) == 0) {

            if (BCM_PBMP_MEMBER(expected_pbmp[i], src_port)) {
                (*count)++; /* bump up the rx count */
                printf("pktioRxCallback: rx_count = 0x%x\n", *count);

                /* remove from bitmap to make sure no duplicates */
                BCM_PBMP_PORT_REMOVE(expected_pbmp[i], src_port);
            }
            else
                test_failed = 1;  /* dupicate or unexpected egress ports */
            break;
        }
    }
    if (i == packet_patterns)
        test_failed = 1;   /* an unexpected packet */

    return BCM_PKTIO_RX_NOT_HANDLED;
}


const uint8         priority = 101;
/* Register callback function for received packets */
bcm_error_t
registerPktioRxCallback(int unit)
{
    const uint32        flags = 0;      /* Flags only used on match (for now) */

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
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0, rv;

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
    rv = bcm_port_stat_attach(unit, egress_port2, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_stat_attach(unit, egress_port3, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
cleanup_flex_ctr(int unit)
{
    int rv;

    /* Detach counter action. */
    rv = bcm_port_stat_detach_with_id(unit, egress_port1, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_stat_detach_with_id: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_stat_detach_with_id(unit, egress_port2, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_stat_detach_with_id: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_stat_detach_with_id(unit, egress_port3, stat_counter_id);
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



/* Create vlan and add port to vlan */
bcm_error_t
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* Adding egress ports to multicast group */

bcm_error_t
multicast_egress_add(int unit, bcm_multicast_t l2mc_group, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_gport_t gport;

    rv = bcm_port_gport_get(unit, port, &gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in gport get : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Adding port to multicast index */
    rv = bcm_multicast_egress_add(unit, l2mc_group, gport, BCM_IF_INVALID);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress add : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
multicast_egress_del(int unit, bcm_multicast_t l2mc_group, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_gport_t gport;

    rv = bcm_port_gport_get(unit, port, &gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in gport get : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Deleting port to multicast index */
    rv = bcm_multicast_egress_delete(unit, l2mc_group, gport, BCM_IF_INVALID);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress delete : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}



/* Configuring L2 Multicast */
bcm_error_t
configure_l2_multicast(int unit, bcm_pbmp_t pbm, bcm_multicast_t* l2mc_group)
{
    bcm_error_t rv;
    bcm_port_t  port;

    /* Create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2, l2mc_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in creating multicast group : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_ITER(pbm, port) {
        rv = multicast_egress_add(unit, *l2mc_group, port);
        if (BCM_FAILURE(rv)) {
            printf("Error in multicast_egress_add() : %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}


bcm_error_t
cleanup_l2_multicast(int unit, bcm_pbmp_t pbm, bcm_multicast_t l2mc_group)
{
    bcm_error_t rv;
    bcm_port_t  port;

    BCM_PBMP_ITER(pbm, port) {
        rv = multicast_egress_del(unit, l2mc_group, port);
        if (BCM_FAILURE(rv)) {
            printf("Error in multicast_egress_add() : %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Destory multicast group */
    rv = bcm_multicast_destroy(unit, l2mc_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in destroying multicast group : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Configuring Port Filtering Mode */
bcm_error_t
configure_pfm(int unit, bcm_vlan_t vlan, bcm_vlan_mcast_flood_t mode)
{
    bcm_error_t rv;
    bcm_vlan_control_vlan_t vlan_ctrl;
    int valid_field = BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK;

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    vlan_ctrl.l2_mcast_flood_mode = mode;

    rv = bcm_vlan_control_vlan_selective_set(unit, vlan, valid_field, &vlan_ctrl);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan control: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/* Adding multicast mac in L2 entry */
bcm_error_t
add_multicast_mac(int unit, bcm_mac_t mc_mac, bcm_vlan_t vlan, bcm_multicast_t l2mc_group)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(l2_addr, mc_mac, vlan);

    l2_addr.flags =  BCM_L2_MCAST;
    l2_addr.l2mc_group = l2mc_group;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l2_addr_add() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
del_multicast_mac(int unit, bcm_mac_t mc_mac, bcm_vlan_t vlan)
{
    bcm_error_t rv;

    rv = bcm_l2_addr_delete(unit, mc_mac, vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_l2_addr_delete() : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    1) Create a L2 multicast group and add egress_port1 and egress_port2 to
 *       this L2 multicast group.
 *    3) Add an entry into L2 table
 *       specifying the L2 multicast group as destination for <vlan, multicast MAC>
 */

bcm_error_t
configure_l2_mc(int unit)
{
    bcm_error_t rv;
    bcm_mac_t l2mc_mac = {0x01, 0x00, 0x04, 0x00, 0x01, 0x00};
    bcm_pbmp_t pbm;

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, egress_port1);
    BCM_PBMP_PORT_ADD(pbm, egress_port2);

    rv = configure_l2_multicast(unit, pbm, &mc_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring L2 multicast: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = add_multicast_mac(unit, l2mc_mac, vlan, mc_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding multicast mac: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
cleanup_l2_mc(int unit)
{
    bcm_error_t rv;
    bcm_mac_t l2mc_mac = {0x01, 0x00, 0x04, 0x00, 0x01, 0x00};
    bcm_pbmp_t pbm;

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, egress_port1);
    BCM_PBMP_PORT_ADD(pbm, egress_port2);

    rv = cleanup_l2_multicast(unit, pbm, mc_group);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring L2 multicast: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = del_multicast_mac(unit, l2mc_mac, vlan);
    if(BCM_FAILURE(rv)) {
        printf("\nError in deleting multicast mac: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}



/*
 *   (1) Select one ingress and three egress ports and configure them in Loopback mode.
 *   (2) Create a vlan with the ingress port and three ehress ports as memebers
 *   (2) Install a VFP rule to copy packets to CPU and drop the packets when it loops back on egress ports.
 *   (3) Start packet watcher.
 *   (4) Register the rx callback function
 *   (5) Call configure_l2_mc() to create L2 MC group with two ports and add a known MC using the L2 MC group into the L2 table
 *   (6) Configure flex Counters
 */
bcm_error_t test_setup(int unit)
{
    int port_list[4];
    int rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];
    egress_port3 = port_list[3];

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);
    BCM_PBMP_PORT_ADD(pbmp, egress_port3);
    BCM_PBMP_CLEAR(ubmp);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    /* set up ingress port */
    BCM_PBMP_CLEAR(ing_pbmp);
    BCM_PBMP_PORT_ADD(ing_pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, ing_pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }
    /* set up egress ports */
    BCM_PBMP_CLEAR(egr_pbmp);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port2);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port3);
    if (BCM_E_NONE != egress_port_multi_setup(unit, egr_pbmp)) {
        printf("egress_port_multi_setup() failed\n");
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    rv = registerPktioRxCallback(unit);
    if (BCM_FAILURE(rv)) {
        printf("registerPktioRxCallback() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_FAILURE((rv = configure_l2_mc(unit)))) {
        printf("configure_l2_mc() failed.\n");
        return -1;
    }

    if (BCM_E_NONE != setup_flex_ctr(unit)) {
        printf("setup_flex_ctr() failed.\n");
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");
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

    if (BCM_E_NONE != cleanup_l2_mc(unit)) {
        printf("cleanup_vlan_switching() failed.\n");
        return -1;
    }

    /* clean up learned l2 entries */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vlan, 0));


    /* Delete vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vlan, pbmp));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vlan));

    BCM_IF_ERROR_RETURN(ingress_port_multi_cleanup(unit, ing_pbmp));
    BCM_IF_ERROR_RETURN(egress_port_multi_cleanup(unit, egr_pbmp));

    bshell(unit, "pw stop");
    unregisterPktioRxCallback(unit);

    return BCM_E_NONE;
}



bcm_error_t
verify_flex_ctr(int unit, bcm_pbmp_t pbm, int pkt_cnt, int byte_cnt, int *flexCtr_pass)
{
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index;
    bcm_port_t     port;
    int rv;

    BCM_PBMP_ITER(pbm, port) {
        /* Get counter value */
        sal_memset(&counter_value, 0, sizeof(counter_value));
        counter_index = port;
        rv = bcm_flexctr_stat_sync_get(unit,
                                       stat_counter_id,
                                       num_entries,
                                       &counter_index,
                                       &counter_value);
        if (BCM_FAILURE(rv)) {
            printf("bcm_flexctr_stat_sync_get: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* print and verify counter value */
        printf("FlexCtr Get on port %d : %d packets / %d bytes\n", port,
               COMPILER_64_LO(counter_value.value[0]),
               COMPILER_64_LO(counter_value.value[1]));
        if (COMPILER_64_LO(counter_value.value[0]) != pkt_cnt) {
            printf("Packet verify failed. Expected packet stat %d but get %d\n", pkt_cnt, COMPILER_64_LO(counter_value.value[0]));
            *flexCtr_pass = 0;
        };
        if (COMPILER_64_LO(counter_value.value[1]) != byte_cnt) {
            printf("Packet verify failed. Expected bytes stat %d but get %d\n", byte_cnt, COMPILER_64_LO(counter_value.value[1]));
            *flexCtr_pass = 0;
        };

        /* clean up counter */
        sal_memset(&counter_value, 0, sizeof(counter_value));
        counter_index = port;
        rv = bcm_flexctr_stat_set(unit,
                                  stat_counter_id,
                                  num_entries,
                                  &counter_index,
                                  &counter_value);
        if (BCM_FAILURE(rv)) {
            printf("bcm_flexctr_stat_set: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return (BCM_E_NONE);
}



/*     This function does the following.
 *      a) for each PFM mode = 0, 1, or 2 {
 *             set up PFM mode on the test vlan
 *             forward known multicast and unknown multicast in the vlan
 *         }
 *      b) Expected Result:
 *         ================
 *         Perform the following checks for each ingress packet
 *         b.1) Verify flexCtr on each egress port
 *         b.2) Verify egress packets on each egress port
 */
/*Expected Egress packets */
unsigned char expected_egress_packet1[68] = {
        0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x02,
        0x08, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
        0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
        0x8E, 0x0B, 0xEC, 0x9E};

unsigned char expected_egress_packet2[68] = {
        0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x02,
        0x08, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
        0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
        0x5C, 0xA6, 0x4D, 0x2A};


bcm_error_t
test_verify(int unit)
{
    char   str[512];

    int expected_rx_count, pass1, pass2, pass3, pass4, pass5, pass6;
    int flexCtr_pass;
    bcm_vlan_mcast_flood_t mode;
    bcm_pbmp_t pbm;
    bcm_error_t rv;

    /* Port Filtering Mode */
    /*
     * Mode 0 - BCM_VLAN_MCAST_FLOOD_ALL
     * Mode 1 - BCM_VLAN_MCAST_FLOOD_UNKNOWN
     * Mode 2 - BCM_VLAN_MCAST_FLOOD_NONE
     */

    printf("/*******************************/\n");
    printf("Test of PFM MODE 0 \n");
    printf("/*******************************/\n");
    mode = BCM_VLAN_MCAST_FLOOD_ALL; /* Mode 0 */
    rv = configure_pfm(unit, vlan, mode);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring PFM for L2 multicast: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 3;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);

    pbm = expected_pbmp[0];
    printf("Transmiting known multicast packet on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x010004000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D8E0BEC9E; sleep 1", ingress_port);
    bshell(unit, str);

    rv = verify_flex_ctr(unit, pbm, 1, 72, &flexCtr_pass);
    if (BCM_FAILURE(rv)) {
        printf("verify_flex_ctr(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass1 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------------- \n");
    printf("    MODE 0: RESULT OF KNOWN MULTICAST PACKET FLOODING VERIFICATION  ");
    printf("\n----------------------------------------------------------------- \n");
    printf("Test1 = [%s]", pass1? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 3;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet2;;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);
    pbm = expected_pbmp[0];

    printf("Transmiting unknown multicast packet on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x010003000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D5CA64D2A; sleep 1", ingress_port);
    bshell(unit, str);

    rv = verify_flex_ctr(unit, pbm, 1, 72, &flexCtr_pass);
    if (BCM_FAILURE(rv)) {
        printf("verify_flex_ctr(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass2 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------------- \n");
    printf("    MODE 0: RESULT OF UNKNOWN UNICAST PACKET FLOODING VERIFICATION  ");
    printf("\n----------------------------------------------------------------- \n");
    printf("Test2 = [%s]", pass2? "PASS":"FAIL");
    printf("\n------------------------------------- \n");



    printf("/*******************************/\n");
    printf("Test of PFM MODE 1 \n");
    printf("/*******************************/\n");
    mode = BCM_VLAN_MCAST_FLOOD_UNKNOWN; /* Mode 1 */
    rv = configure_pfm(unit, vlan, mode);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring PFM for L2 multicast: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 2;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    pbm = expected_pbmp[0];

    printf("Transmiting known multicast packet on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x010004000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D8E0BEC9E; sleep 1", ingress_port);
    bshell(unit, str);

    rv = verify_flex_ctr(unit, pbm, 1, 72, &flexCtr_pass);
    if (BCM_FAILURE(rv)) {
        printf("verify_flex_ctr(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass3 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------------- \n");
    printf("    MODE 1: RESULT OF KNOWN MULTICAST PACKET FLOODING VERIFICATION  ");
    printf("\n----------------------------------------------------------------- \n");
    printf("Test3 = [%s]", pass3? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 3;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet2;;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);
    pbm = expected_pbmp[0];

    printf("Transmiting unknown multicast packet on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x010003000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D5CA64D2A; sleep 1", ingress_port);
    bshell(unit, str);

    rv = verify_flex_ctr(unit, pbm, 1, 72, &flexCtr_pass);
    if (BCM_FAILURE(rv)) {
        printf("verify_flex_ctr(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass4 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------------- \n");
    printf("    MODE 1: RESULT OF UNKNOWN UNICAST PACKET FLOODING VERIFICATION  ");
    printf("\n----------------------------------------------------------------- \n");
    printf("Test4 = [%s]", pass4? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    printf("/*******************************/\n");
    printf("Test of PFM MODE 2 \n");
    printf("/*******************************/\n");
    mode = BCM_VLAN_MCAST_FLOOD_NONE; /* Mode 2 */
    rv = configure_pfm(unit, vlan, mode);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring PFM for L2 multicast: %s.\n",
               bcm_errmsg(rv));
        return rv;
    }

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 2;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    pbm = expected_pbmp[0];

    printf("Transmiting known multicast packet on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x010004000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D8E0BEC9E; sleep 1", ingress_port);
    bshell(unit, str);

    rv = verify_flex_ctr(unit, pbm, 1, 72, &flexCtr_pass);
    if (BCM_FAILURE(rv)) {
        printf("verify_flex_ctr(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass5 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------------- \n");
    printf("    MODE 2: RESULT OF KNOWN MULTICAST PACKET FLOODING VERIFICATION  ");
    printf("\n----------------------------------------------------------------- \n");
    printf("Test5 = [%s]", pass5? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 0;
    flexCtr_pass = 1;
    expected_packets[0]  = NULL;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    pbm = expected_pbmp[0];

    printf("Transmiting unknown multicast packet on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x010003000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D5CA64D2A; sleep 1", ingress_port);
    bshell(unit, str);

    rv = verify_flex_ctr(unit, pbm, 0, 0, &flexCtr_pass);
    if (BCM_FAILURE(rv)) {
        printf("verify_flex_ctr(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass6 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------------- \n");
    printf("    MODE 2: RESULT OF UNKNOWN UNICAST PACKET FLOODING VERIFICATION  ");
    printf("\n----------------------------------------------------------------- \n");
    printf("Test6 = [%s]", pass6? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (pass1 && pass2 && pass3 && pass4 && pass5 && pass6)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;

}


/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_l2_mc())
 * c)demonstrates the functionality(done in verify()).
 */
bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;

    bshell(unit, "config show; a ; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    printf("Completed test setup successfully.\n");

    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() [FAIL]\n");
        return -1;
    }
    else
        printf("test_verify() [PASS]\n");

    if (BCM_FAILURE((rv = test_cleanup(unit)))) {
        printf("test_cleanup() failed.\n");
        return -1;
    }
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}
