/*  Feature  : VLAN membership
 *
 *  Usage    : BCM.0> cint th4_hsdk_vlan_stg_membership.c
 *
 *  config   : th4_hsdk_vlan_config.yml
 *
 *  Log file : th4_hsdk_vlan_stg_membership_log.txt
 *
 *  Test Topology :
 *
 *                   +----------------------------+
 *                   |                            |  egress_port1
 *                   |                            +-----------------
 *                   |                            |
 *    ingress_port   |                            |  egress_port2
 *  -----------------+            SWITCH          +-----------------
 *                   |                            |
 *                   |                            |  egress_port3
 *                   |                            +-----------------
 *                   |                            |
 *                   |                            |  egress_port4
 *                   |                            +-----------------
 *                   |                            |
 *                   +----------------------------+
 *
 *  Summary:
 *  ========
 *    This CINT configures per port per vlan ifiltering/efiltering using BCM APIs.
 *    Packet forwarding is verified after each per port per vlan ifiltering/efiltering change.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      1.1) Add vlan1 with ingress_port, egress_port1, and egress_port2
 *      1.2) Add vlan2 with ingress_port, egress_port3, and egress_port4
 *      1.3) Create a stg with vlan1 and vlan2
 *      1.4) Initialze all ports to be FORWARDING in the stg
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ======================================================
 *      a)
 *         a.1) send an UUC in vlan1 from ingress_port, expect the packet to be forwarded to
 *              egress_port1 and egress_port2
 *         a.2) send an UUC in vlan2 from ingress_port, expect the packet to be forwarded to
 *              egress_port3 and egress_port4
 *         a.3) remove ingress_port from vlan1 as an ingress member and repeat (a.1), expect no packet to be
 *              forwarded.
 *         a.4) remove ingress_port from vlan2 as an ingress member and repeat (a.2), expect no packet to be
 *              forwarded.
 *         a.5) remove egress_port2 from vlan1 as a egress member and repeat (a.1), expect the packet to be
 *              forwarded to egress_port1 only
 *         a.6) remove egress_port4 from vlan2 as a egress member and repeat (a.2), expect the packet to be
 *              forwarded to egress_port3 only
 *
 *      b) Expected Result:
 *         ================
 *         Perform the following checks for each ingress packet
 *         b.1) Verify flexCtr on each egress port
 *         b.2) Verify egress packets on each egress port
 */

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1;
bcm_port_t egress_port2;
bcm_port_t egress_port3;
bcm_port_t egress_port4;
bcm_gport_t gport_ingress;
bcm_gport_t gport_egress1;
bcm_gport_t gport_egress2;
bcm_gport_t gport_egress3;
bcm_gport_t gport_egress4;

bcm_vlan_t vlan1 = 20, vlan2 = 30;
bcm_stg_t stg1;

bcm_pbmp_t ing_pbmp, egr_pbmp;
bcm_pbmp_t pbmp1, ubmp1, pbmp2, ubmp2;
bcm_field_group_t group;
int num_egress_ports = 4;
bcm_field_entry_t entry[num_egress_ports];
uint32_t stat_counter_id;

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
setup_stg(int unit)
{
    int rv;

    rv = bcm_stg_create(unit,&stg1);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_stg_create(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    print stg1;
    bcm_stg_vlan_remove_all(unit, stg1);
    rv = bcm_stg_vlan_add(unit, stg1, vlan1);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_stg_vlan_add(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_stg_vlan_add(unit, stg1, vlan2);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_stg_vlan_add(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_stg_stp_set(unit, stg1, ingress_port, BCM_STG_STP_FORWARD);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stg_stp_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_stg_stp_set(unit, stg1, egress_port1, BCM_STG_STP_FORWARD);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stg_stp_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_stg_stp_set(unit, stg1, egress_port2, BCM_STG_STP_FORWARD);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stg_stp_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_stg_stp_set(unit, stg1, egress_port3, BCM_STG_STP_FORWARD);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stg_stp_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_stg_stp_set(unit, stg1, egress_port4, BCM_STG_STP_FORWARD);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stg_stp_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
cleanup_stg(int unit)
{
    int rv;

    bcm_stg_vlan_remove_all(unit, stg1);
    rv = bcm_stg_vlan_remove_all(unit, stg1);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_stg_vlan_remove_all(): %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_stg_destroy(unit, stg1);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_stg_destroy(): %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}


bcm_error_t
setup_pkt_filter(int unit)
{
    int rv;
    uint32 flags;

    /* Enable ifilter/efilter on the ports */
    rv = bcm_port_vlan_member_set(unit, ingress_port, 
                                  BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port1, 
                                  BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port2, 
                                  BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port3, 
                                  BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port4, 
                                  BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


bcm_error_t
cleanup_pkt_filter(int unit)
{

    int rv;

    /* Enable ifilter/efilter on the ports */
    rv = bcm_port_vlan_member_set(unit, ingress_port, BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port1, BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port2, BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port3, BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit, egress_port4, BCM_PORT_VLAN_MEMBER_EGRESS|BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_vlan_member_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
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
    rv = bcm_port_stat_attach(unit, egress_port4, stat_counter_id);
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
    rv = bcm_port_stat_detach_with_id(unit, egress_port4, stat_counter_id);
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
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int num_ports = 5;
    int rv;
    int port_list[num_ports];
    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];
    egress_port3 = port_list[3];
    egress_port4 = port_list[4];
    printf("Ingress port : %d\n", ingress_port );
    printf("Egress port1: %d\n", egress_port1);
    printf("Egress port2: %d\n", egress_port2);
    printf("Egress port3: %d\n", egress_port3);
    printf("Egress port4: %d\n", egress_port4);

    bcm_port_gport_get(unit, ingress_port, &gport_ingress);
    bcm_port_gport_get(unit, egress_port1, &gport_egress1);
    bcm_port_gport_get(unit, egress_port2, &gport_egress2);
    bcm_port_gport_get(unit, egress_port3, &gport_egress3);
    bcm_port_gport_get(unit, egress_port4, &gport_egress4);

    BCM_PBMP_CLEAR(pbmp1);
    BCM_PBMP_PORT_ADD(pbmp1, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp1, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp1, egress_port2);
    BCM_PBMP_CLEAR(ubmp1);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan1));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan1, pbmp1, ubmp1));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vlan1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vlan1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vlan1));

    BCM_PBMP_CLEAR(pbmp2);
    BCM_PBMP_PORT_ADD(pbmp2, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp2, egress_port3);
    BCM_PBMP_PORT_ADD(pbmp2, egress_port4);
    BCM_PBMP_CLEAR(ubmp2);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan2));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan2, pbmp2, ubmp2));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port3, vlan2));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port4, vlan2));

    BCM_PBMP_CLEAR(ing_pbmp);
    BCM_PBMP_PORT_ADD(ing_pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, ing_pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }
    BCM_PBMP_CLEAR(egr_pbmp);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port2);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port3);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port4);
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

    if (BCM_E_NONE != setup_stg(unit)) {
        printf("setup_stg() failed.\n");
        return -1;
    }

    if (BCM_E_NONE != setup_pkt_filter(unit)) {
        printf("setup_pkt_filter() failed.\n");
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

    if (BCM_E_NONE != cleanup_pkt_filter(unit)) {
        printf("cleanup_pkt_filter() failed.\n");
        return -1;
    }

    if (BCM_E_NONE != cleanup_stg(unit)) {
        printf("cleanup_vlan_switching() failed.\n");
        return -1;
    }

    /* clean up learned l2 entries */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vlan1, 0));
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vlan2, 0));

    /* Set default vlan. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port3, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port4, 1));

    /* Delete vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vlan1, pbmp1));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vlan2, pbmp2));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vlan1));
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vlan2));

    BCM_IF_ERROR_RETURN(ingress_port_multi_cleanup(unit, ing_pbmp));
    BCM_IF_ERROR_RETURN(egress_port_multi_cleanup(unit, egr_pbmp));

    bshell(unit, "pw stop");
    unregisterPktioRxCallback(unit);

    return BCM_E_NONE;
}



int rx_count;   /* Global received packet count */
int test_failed; /*Final result will be stored here */

uint32 packet_patterns = 1;
unsigned char *expected_packets[1];
bcm_pbmp_t expected_pbmp[1];


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

    printf("pktioRxCallback: received from port 0x%x\n", port);

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


/*Expected Egress packets */
unsigned char expected_egress_packet1[64] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00,
        0x00, 0x14, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0xbc, 0x94, 0x97, 0x7f};

unsigned char expected_egress_packet2[64] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00,
        0x00, 0x1e, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0A, 0x0A, 0x0A, 0x03, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0xbc, 0x94, 0x97, 0x7f};


/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t testVerify(int unit)
{
    char   str[512];
    int expected_rx_count, pass1, pass2, pass3, pass4, pass5, pass6;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index;
    int flexCtr_pass = 1;
    int rv;

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 2;
    expected_packets[0]  = expected_egress_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);

    printf("Verify unknown unicast pkt from port %d forwarding to the other 2 ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000014080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
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

    /* print counter value */
    printf("FlexCtr Get on port_egress1 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port1: Packet verify failed. Expected packet stat %d but get %d\n", 1, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 68) {
        printf("Egress_port1: Packet verify failed. Expected bytes stat %d but get %d\n", 68, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for egress_port2. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port2;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress2 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port2: Packet verify failed. Expected packet stat %d but get %d\n", 1, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 68) {
        printf("Egress_port2: Packet verify failed. Expected bytes stat %d but get %d\n", 68, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* clean up flex counters */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port1;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port2;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }


    pass1 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF VLAN MEMBERSHIP VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test1 = [%s]", pass1? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n\n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 2;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port4);

    printf("Verify unknown unicast pkt from port %d forwarding to the other 2 ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000220000000000118100001e080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);


    /* Get counter value for egress_port3 */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port3;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress3 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port3: Packet verify failed. Expected packet stat %d but get %d\n", 1, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 68) {
        printf("Egress_port3: Packet verify failed. Expected bytes stat %d but get %d\n", 68, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for egress_port4. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port4;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress4 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port4: Packet verify failed. Expected packet stat %d but get %d\n", 1, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 68) {
        printf("Egress_port4: Packet verify failed. Expected bytes stat %d but get %d\n", 68, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* clean up flex counters */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port3;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port4;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass2 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF VLAN MEMBERSHIP VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test2 = [%s]", pass2? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n\n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 0;
    flexCtr_pass = 1;
    expected_packets[0]  = NULL;
    BCM_PBMP_CLEAR(expected_pbmp[0]);

    rv = bcm_vlan_gport_extended_delete(unit, vlan1, gport_ingress, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_extended_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Verify unknown unicast pkt from port %d forwarding to the other 2 ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000220000000000118100000a080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
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

    /* print counter value */
    printf("FlexCtr Get on port_egress1 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port1: Packet verify failed. Expected packet stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port1: Packet verify failed. Expected bytes stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for egress_port2. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port2;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress2 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port2: Packet verify failed. Expected packet stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port2: Packet verify failed. Expected bytes stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* clean up flex counters */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port1;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port2;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vlan1, gport_ingress, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass3 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF VLAN MEMBERSHIP VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test3 = [%s]", pass3? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n\n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 0;
    flexCtr_pass = 1;
    expected_packets[0]  = NULL;
    BCM_PBMP_CLEAR(expected_pbmp[0]);

    rv = bcm_vlan_gport_extended_delete(unit, vlan2, gport_ingress, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_extended_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Verify unknown unicast pkt from port %d forwarding to the other 2 ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000220000000000118100001e080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);


    /* Get counter value for egress_port3 */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port3;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress3 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port3: Packet verify failed. Expected packet stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port3: Packet verify failed. Expected bytes stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for egress_port4. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port4;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress4 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port4: Packet verify failed. Expected packet stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port4: Packet verify failed. Expected bytes stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* clean up flex counters */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port3;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port4;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vlan2, gport_ingress, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass4 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF VLAN MEMBERSHIP VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test4 = [%s]", pass4? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n\n");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    expected_packets[0]  = expected_egress_packet1;
    flexCtr_pass = 1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);

    rv = bcm_vlan_gport_extended_delete(unit, vlan1, gport_egress2, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_extended_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Verify unknown unicast pkt from port %d forwarding to the other 2 ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000014080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
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

    /* print counter value */
    printf("FlexCtr Get on port_egress1 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port1: Packet verify failed. Expected packet stat %d but get %d\n", 1, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 68) {
        printf("Egress_port1: Packet verify failed. Expected bytes stat %d but get %d\n", 68, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for egress_port2. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port2;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress2 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port2: Packet verify failed. Expected packet stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port2: Packet verify failed. Expected bytes stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* clean up flex counters */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port1;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port2;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vlan1, gport_egress2, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass5 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF VLAN MEMBERSHIP VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test5 = [%s]", pass5? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n\n");


    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    flexCtr_pass = 1;
    expected_packets[0]  = expected_egress_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);

    rv = bcm_vlan_gport_extended_delete(unit, vlan2, gport_egress4, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_extended_delete() %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Verify unknown unicast pkt from port %d forwarding to the other 2 ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000220000000000118100001e080600010800060400010000000000020A0A0A020000000000000A0A0A03000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);


    /* Get counter value for egress_port3 */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port3;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress3 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        printf("Egress_Port3: Packet verify failed. Expected packet stat %d but get %d\n", 1, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 68) {
        printf("Egress_port3: Packet verify failed. Expected bytes stat %d but get %d\n", 68, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    /* Get counter value for egress_port4. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port4;
    rv = bcm_flexctr_stat_sync_get(unit,
                                   stat_counter_id,
                                   num_entries,
                                   &counter_index,
                                   &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* print counter value */
    printf("FlexCtr Get on port_egress4 : %d packets / %d bytes\n",
           COMPILER_64_LO(counter_value.value[0]),
           COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != 0) {
        printf("Egress_Port4: Packet verify failed. Expected packet stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[0]));
        flexCtr_pass = 0;
    };
    if (COMPILER_64_LO(counter_value.value[1]) != 0) {
        printf("Egress_port4: Packet verify failed. Expected bytes stat %d but get %d\n", 0, COMPILER_64_LO(counter_value.value[1]));
        flexCtr_pass = 0;
    };

    rv = bcm_vlan_gport_add(unit, vlan2, gport_egress4, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_gport_add() %s\n", bcm_errmsg(rv));
        return rv;
    }

    pass6 = (test_failed == 0) && (rx_count == expected_rx_count) && flexCtr_pass;
    printf("\n----------------------------------------------------------- \n");
    printf("    RESULT OF VLAN MEMBERSHIP VERIFICATION  ");
    printf("\n----------------------------------------------------------- \n");
    printf("Test6 = [%s]", pass6? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n\n");

    /* clean up flex counters */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port3;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }

    sal_memset(&counter_value, 0, sizeof(counter_value));
    counter_index = egress_port4;
    rv = bcm_flexctr_stat_set(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_set %s\n", bcm_errmsg(rv));
        return rv;
    }


    if (pass1 && pass2 && pass3 && pass4 && pass5 && pass6)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}


/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in setup_vlan_switching())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    int unit = 0;
    int rv;
    bshell(unit, "config show; attach ; version");
    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";
    print "~~~ #2) verify(): ** start **";
    if (BCM_FAILURE((rv = testVerify(unit)))) {
        printf("testVerify() [FAIL]\n");
        return -1;
    }
    else
        printf("testVerify() [PASS]\n");

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
