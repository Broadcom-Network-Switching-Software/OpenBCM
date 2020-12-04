/*
 * Feature  : traffic class to queue mapping
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_tc_to_queue_map_per_port.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_tc_to_queue_map_per_port_log.txt
 *
 * Test Topology :
 *                        +-------------------+
 *                        |                   |
 *                        |                   |
 *         ingress_port   |       TH4         | Egress port
 *         -------------->+                   +--------->
 *          VLAN 2        |        L2         |  VLAN 2
 *                        |                   |
 *                        |                   |
 *                        +-------------------+
 *
 * This script can verify below cosq gport APIs:
 * ========
 *     bcm_cosq_gport_mapping_set
 *     bcm_cosq_gport_mapping_get
 *
 * Summary:
 * ========
 * This cint example demonstrate per port traffic class to queue mapping setting/getting.
 *
 *   Detailed steps done in the CINT script:
 *   =======================================
 *   1) Step1 - Test Setup (Done in test_setup())
 *   =======================================
 *    a) Build the COSQ gport list, front panel port list
 *    b) Select two ports and set up L2 switching path between the ports
 *
 *   2) Step2 - Configuration (Done in test_setup()):
 *   ==============================================
 *    a) Save the default TC to queue mapping
 *    b) Set the mapping from TC to queue via bcm_cosq_gport_mapping_set()
 *    c) Set up flex counter per queue
 *
 *   3) Step3 - Verification (Done in test_verify()):
 *   ==============================================
 *     a) Verify the result via tc_to_q_mapping_get
 *         Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports.
 *        Expected Result:
 *        ================
 *        The retrieved mapping should be aligned with the configured one as below:
 *                      TC:  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
 *        queue(for UC):  {3, 2, 1, 0, 7, 6, 5, 4, 4, 3,  2,  1, 1,  2,  3, 4}
 *        queue(for MC):  {3, 2, 1, 0, 3, 3, 3, 3, 3, 3,  2,  1, 1,  2,  3, 3}
 *     b) Transmit a packet to the ingress port and create VLAN, with priority 0,
 *        expect the packet to egress on the queue mapped, verify with flex counter
 *
 *   4) Step4 - verify (Done in test_cleanup())
 *   ==========================================
 *      a) Reset the TC to queue to default mapping
 *      b) Remove the per queue flex counter
 *      c) Delete L2 forwarding path
 */

cint_reset();

int unit = 0;

/* Glabal variable definition */
int CPU_PORT = 0;
int MAX_PORTS = 272;
int MAX_FP_PORTS = 256;
int MAX_QUEUES = 12;
int MAX_CPU_QUEUES = 48;
int MAX_SCH_NODES = 12;

int prev_port = -1;
int free_ucast_index = 0;
int free_mcast_index = 0;
int free_sched_index = 0;
int valid_port_number = 0;
int port_list[MAX_FP_PORTS];

bcm_gport_t ucast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t mcast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t sched_gport[MAX_PORTS][MAX_SCH_NODES];
bcm_gport_t cpu_mcast_q_gport[MAX_CPU_QUEUES];
bcm_gport_t cpu_sched_gport[MAX_SCH_NODES];

/* TC to Q mapping */
int MAX_TC = 16;
int newUcTcToQMap[] = {7, 6, 5, 4, 3, 2, 1, 0, 4, 3, 2, 1, 1, 2, 3, 4};
int newMcTcToQMap[] = {3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bcm_gport_t save_ucast_q_gport[MAX_PORTS][MAX_TC];
bcm_gport_t save_mcast_q_gport[MAX_PORTS][MAX_TC];

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_vlan_t vid = 2;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;

/* This function gives required number of ports. */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (port= 1; port < MAX_PORTS; port++) {
        port_list[i] = -1;
        if (BCM_PBMP_MEMBER(&ports_pbmp, port)&& (i < num_ports)) {
            port_list[i] = port;
            i++;
        }
    }

    if (i == 0) {
       printf("portNumbersGet() failed \n");
       return -1;
    }
    valid_port_number = i;

    return BCM_E_NONE;
}

/*
 * Set up L2 forwarding path
 */
int
l2_setup(int unit)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t addr;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, upbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port, vid));
    bcm_port_control_set(unit, ingress_port, bcmPortControlTrustIncomingVlan, 1);

    /* Add a L2 address on front panel port. */
    bcm_l2_addr_t_init(&addr, dst_mac, vid);
    addr.port = egress_port;
    addr.flags |= BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

    return BCM_E_NONE;
}

int
l2_cleanup(int unit)
{
    /* Delete L2 address */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port, 1));

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    return BCM_E_NONE;
}

int
flex_ctr_setup(int unit, bcm_vlan_t vlan)
{
    bcm_error_t rv;
    bcm_flexctr_action_t flexctr_action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;

    bcm_flexctr_action_t_init(&flexctr_action);

    flexctr_action.source = bcmFlexctrSourceEgrVlan;
    flexctr_action.mode = bcmFlexctrCounterModeNormal;
    flexctr_action.hint = 0;
    /* Total number of counters */
    flexctr_action.index_num = 16;  /* total 16 queues */

    index_op = &flexctr_action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrQueueNum;
    index_op->mask_size[0] = 4;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &flexctr_action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 4;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &flexctr_action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    rv = bcm_flexctr_action_create(unit, options, &flexctr_action, &stat_counter_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flex counter action create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Attach counter action to a VLAN. */
    rv = bcm_vlan_stat_attach(unit, vlan, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_attach %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
flex_ctr_cleanup(int unit)
{
    int rv;

    /* Detach counter action. */
    rv = bcm_vlan_stat_detach_with_id(unit, vid, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_vlan_stat_detach_with_id: %s\n", bcm_errmsg(rv));
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

int
flex_ctr_check(int unit, int queue)
{
    int rv;
    uint32 num_entries = 1;
    uint32 counter_index = queue;

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index, &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
  
    /* print counter value. */
    printf("FlexCtr Get : %d packets / %d bytes\n",
            COMPILER_64_LO(counter_value.value[0]), COMPILER_64_LO(counter_value.value[1]));

    if (COMPILER_64_LO(counter_value.value[0]) != 1) {
        return -1;
    }

    return BCM_E_NONE;
}


static bcm_error_t
cosq_gport_traverse_callback(int unit, bcm_gport_t port,
                             int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
     int local_port = 0;

     switch (flags) {
         case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 free_ucast_index = 0;
                 free_mcast_index = 0;
                 free_sched_index = 0;
                 prev_port = local_port;
                 printf("cosq_gport_traverse_callback: gport for local_port: %d\n", local_port);
             }

             /*printf("cosq_gport_traverse_callback ucast queue: local_port=%d, mod_port=0x%x, numq=%d, gport=0x%x\n", local_port, port, numq, gport);*/
             if (CPU_PORT == local_port) {
                 return BCM_E_PARAM;
             }
             ucast_q_gport[local_port][free_ucast_index] = gport;
             free_ucast_index++;
             break;

         case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 free_ucast_index = 0;
                 free_mcast_index = 0;
                 free_sched_index = 0;
                 prev_port = local_port;
                 printf("cosq_gport_traverse_callback: gport for local_port: %d\n", local_port);
             }

             /*printf("cosq_gport_traverse_callback mcast queue: local_port=%d, mod_port=0x%x, numq=%d, gport=0x%x\n", local_port, port, numq, gport);*/
             if (CPU_PORT == local_port) {
                 cpu_mcast_q_gport[free_mcast_index] = gport;
             } else {
                 mcast_q_gport[local_port][free_mcast_index] = gport;
             }
             free_mcast_index++;
             break;

         case BCM_COSQ_GPORT_SCHEDULER:
             /*printf("cosq_gport_traverse_callback scheduler: local_port=%d, mod_port=0x%x, numq=%d, gport=0x%x\n", prev_port, port, numq, gport);*/
             if (CPU_PORT == prev_port) {
                 cpu_sched_gport[free_sched_index] = gport;
             } else {
                 sched_gport[local_port][free_sched_index] = gport;
             }
             free_sched_index++;
             break;
         default:
             return BCM_E_PARAM;
     }

    return 1;
}

/*
 * Build queue list for all ports
 */
static bcm_error_t
cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb, void *user_data)
{
    int ret_val = 0;

    ret_val = bcm_cosq_gport_traverse(unit, cb, NULL);
    if (ret_val  < BCM_E_NONE) {
        printf("ERR(%d): bcm_cosq_gport_traverse function unit = %d\n", ret_val, unit);
        return -1;
    }

    free_ucast_index = 0;
    free_mcast_index = 0;
    free_sched_index = 0;
    prev_port = -1;

    return 1;
}

/*
 * Set the TC to queue map using the gport for one port using bcm_cosq_gport_mapping_set()
 */
int port_tc_to_q_mapping_set (int unit, int port)
{
    int rv = BCM_E_NONE;
    int tc = 0;
    int queue_id = 0;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping set */
    for(tc = 0; tc < MAX_TC; tc ++) {
        queue_id = newUcTcToQMap[tc];
        queue_gport = ucast_q_gport[port][queue_id];
        rv = bcm_cosq_gport_mapping_set(unit, port, tc,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        queue_gport, 0);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to map TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                   port, tc, queue_id, bcm_errmsg(rv));
            return rv;
        }
        printf("Unicast: Map TC to Egress queue sucessfully (P, TC, Q) = (%d, %d, %x) - %s\n",
                port, tc, queue_id, bcm_errmsg(rv));
    }

    /* Tc to Multicast Q mapping set */
    for(tc = 0; tc < MAX_TC; tc ++) {
        queue_id = newMcTcToQMap[tc];
        queue_gport = mcast_q_gport[port][queue_id];
        rv = bcm_cosq_gport_mapping_set(unit, port, tc,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        queue_gport, 0);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to map TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                   port, tc, queue_id, bcm_errmsg(rv));
            return rv;
        }
        printf("Multicast: map TC to Egress queue sucessfully (P, TC, Q) = (%d, %d, %x) - %s\n",
                port, tc, queue_id, bcm_errmsg(rv));
    }

    return rv;
}

/*
 * Set the TC to queue map for all ports
 */
int tc_to_q_mapping_set (int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;

/*
    for (i = 0; i < valid_port_number; i++) {
        rv = port_tc_to_q_mapping_set(unit, port_list[i]);
        if (rv != BCM_E_NONE) {
            printf("Unable to tc_to_q_mapping_set on port %d\r\n", port_list[i]);
            return rv;
        }
    }
*/
    rv = port_tc_to_q_mapping_set(unit, ingress_port);
    if (rv != BCM_E_NONE) {
        printf("Unable to tc_to_q_mapping_set on port %d\r\n", ingress_port);
        return rv;
    }

    rv = port_tc_to_q_mapping_set(unit, egress_port);
    if (rv != BCM_E_NONE) {
        printf("Unable to tc_to_q_mapping_set on port %d\r\n", egress_port);
        return rv;
    }

    return rv;
}

/*
 * Get the TC to queue gport map for one port
 */
int port_tc_to_q_mapping_get (int unit, int port)
{
    int rv = BCM_E_NONE;
    int tc = 0;
    int cosq = -1;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv));
    }

    /* Tc to Multicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv));
    }

    return rv;
}

int tc_to_q_mapping_get (int unit)
{
  int rv = BCM_E_NONE;
  int i = 0;

  printf("==> tc_to_q_mapping_get:\n");

/*
  for (i = 0; i < valid_port_number; i++) {
      rv = port_tc_to_q_mapping_get(unit, port_list[i]);
      if (rv != BCM_E_NONE) {
          printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", port_list[i]);
          return rv;
      }
  }
*/

  rv = port_tc_to_q_mapping_get(unit, ingress_port);
  if (rv != BCM_E_NONE) {
      printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", ingress_port);
      return rv;
  }

  rv = port_tc_to_q_mapping_get(unit, egress_port);
  if (rv != BCM_E_NONE) {
      printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", egress_port);
      return rv;
  }

  return rv;
}

/*
 * Get the TC to queue gport map for one port
 */
int save_port_tc_to_q_mapping (int unit, int port)
{
    int rv = BCM_E_NONE;
    int tc = 0;
    int cosq = -1;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        save_ucast_q_gport[port][tc] = queue_gport;
        /* printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv)); */
    }

    /* Tc to Multicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        save_mcast_q_gport[port][tc] = queue_gport;
        /* printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv)); */
    }

    return rv;
}

int save_tc_to_q_mapping (int unit)
{
  int rv = BCM_E_NONE;
  int i = 0;

  printf("--> save_tc_to_q_mapping:\n");

  for (i = 0; i < valid_port_number; i++) {
      rv = save_port_tc_to_q_mapping(unit, port_list[i]);
      if (rv != BCM_E_NONE) {
          printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", port_list[i]);
          return rv;
      }
  }

  return rv;
}

int restore_port_tc_to_q_mapping(int unit, int port)
{
  int rv = BCM_E_NONE;
  int tc = 0;
  int queue_id = 0;
  int queue_gport = 0;

  /* Tc to Unicast Q mapping set */
  for(tc = 0; tc < MAX_TC; tc ++) {
      queue_gport = save_ucast_q_gport[port][tc];
      queue_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(queue_gport);
      rv = bcm_cosq_gport_mapping_set(unit, port, tc,
                                      BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                      queue_gport, 0);
      if (rv != BCM_E_NONE) {
          printf("Unicast: Unable to Restore TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rv));
          return rv;
      }
      /* printf("Unicast: Restore TC to Egress queue sucessfully (P, TC, Q) = (%d, %d, %x) - %s\n",
              port, tc, queue_id, bcm_errmsg(rv)); */
  }

  /* Tc to Multicast Q mapping set */
  for(tc = 0; tc < MAX_TC; tc ++) {
      queue_gport = save_mcast_q_gport[port][tc];
      queue_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(queue_gport);
      rv = bcm_cosq_gport_mapping_set(unit, port, tc,
                                      BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                      queue_gport, 0);
      if (rv != BCM_E_NONE) {
          printf("Multicast: Unable to Restore TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                 port, tc, queue_id, bcm_errmsg(rv));
          return rv;
      }
      /* printf("Multicast: Restore TC to Egress queue sucessfully (P, TC, Q) = (%d, %d, %x) - %s\n",
              port, tc, queue_id, bcm_errmsg(rv)); */
  }

  return rv;
}

/*
 * Set the TC to queue map for all ports
 */
int restore_tc_to_q_mapping(int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;

    printf("--> restore_tc_to_q_mapping:\n");

    for (i = 0; i < valid_port_number; i++) {
        rv = restore_port_tc_to_q_mapping(unit, port_list[i]);
        if (rv != BCM_E_NONE) {
            printf("Unable to restore_port_tc_to_q_mapping on port %d\r\n", port_list[i]);
            return rv;
        }
    }

    return rv;
}

bcm_error_t
test_setup(int unit)
{
    int rv = BCM_E_NONE;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, MAX_FP_PORTS)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Ingress port : %d\n", ingress_port);
    printf("Egress port: %d\n", egress_port);

    printf("==> Build queue GPORT list:\n");

    /* Retrieve all the scheduler gport */
    BCM_IF_ERROR_RETURN(cosq_gport_traverse(unit, cosq_gport_traverse_callback, NULL));

    printf("==> Set up TC to queue mapping ...\n");

    /* Save the default TC to queue mapping */
    if (BCM_FAILURE((rv = save_tc_to_q_mapping(unit)))) {
       printf("save_tc_to_q_mapping() failed.\n");
       return -1;
    }

    /* Set the mapping from TC to queue via bcm_cosq_gport_mapping_set */
    if (BCM_FAILURE((rv = tc_to_q_mapping_set(unit)))) {
       printf("tc_to_q_mapping_set() failed.\n");
       return -1;
    }

    /* Set up L2 forwarding */
    if (BCM_E_NONE != l2_setup(unit)) {
        printf("l2_setup() failed.\n");
        return -1;
    }

    /* Set up per queue flex counter */
    if (BCM_E_NONE != flex_ctr_setup(unit, vid)) {
        printf("flex_ctr_setup() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

/* Function to verify the result */
bcm_error_t test_verify(int unit)
{
    char str[256];

    /* Retrieve the mapping via bcm_cosq_gport_mapping_get for all the valid front panel ports. */
    if (BCM_FAILURE(tc_to_q_mapping_get(unit))) {
       printf("tc_to_q_mapping_get() failed.\n");
       return -1;
    }

    printf("==> Verify unicast pkt from port %d forwarding to port %d\n", ingress_port, egress_port);

    /* Send a packet with priority 0 to ingress port */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_ALL));
    snprintf(str, 256, "tx 1 pbm=%d data=0x00000000001100000000002281000002FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DDFC5C3CB; sleep quiet 3", ingress_port);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(flex_ctr_check(unit, newUcTcToQMap[0]));

    printf("Per queue packet counter check successful.\n");

    return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int test_cleanup(int unit)
{
    printf("==>Cleaning up...\n");

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_NONE));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_NONE));

    /* Remove the per queue flex counter */
    if (BCM_E_NONE != flex_ctr_cleanup(unit)) {
        printf("flex_ctr_cleanup() failed.\n");
        return -1;
    }

    /* Delete L2 forwarding */
    if (BCM_E_NONE != l2_cleanup(unit)) {
        printf("l2_cleanup() failed.\n");
        return -1;
    }

    /* Restore TC to Queue mapping to default */
    if (BCM_E_NONE != restore_tc_to_q_mapping(unit)) {
        printf("restore_tc_to_q_mapping() failed.\n");
        return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; attach; version");

    /* Retrieve all the front panel port lists */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
    }

    /* Verify the result via tc_to_q_mapping_get and packet test */
    if (BCM_FAILURE((rv = test_verify(unit)))) {
         printf("test_verify() failed.\n");
         return -1;
    }

    /* Cleanup */
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

