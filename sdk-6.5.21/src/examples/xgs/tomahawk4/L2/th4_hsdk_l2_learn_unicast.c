/*  Feature  : L2 l2 learning and known unicast forwarding
 *
 *  Usage    : BCM.0> cint th4_hsdk_l2_learn_unicast.c
 *
 *  config   : th4_hsdk_l2_config.yml
 *
 *  Log file : th4_hsdk_l2_learn_unicast.log
 *  
 *  Test Topology :
 *
 *                   +----------------------------+
 *                   |                            |
 *                   |                            |
 *                   |                            |  egress_port1
 *    ingress_port   |                            +-----------------
 *  -----------------+          SWITCH            |
 *                   |                            |
 *                   |                            |  egress_port2
 *                   |                            +-----------------
 *                   |                            |
 *                   |                            |
 *                   +----------------------------+
 *
 *  Summary:
 *  ========
 *    This CINT script verifies L2 learning and unicast forwarding using the learned L2 entry  
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *    Create a vlan with 3 ports and send packet from egress_port1 to learn a L2 entry for <vlan, mac2>
 * 
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ===============================================
 *
 *      (3.1) Verify the existence of the learned L2 entry <vlan, mac2>
 *      (3.2) Send a unicast packet from ingres_port destined to <vlan, mac2> and verify it is forwarded to egress_port1
 *
 *      Expected Result:
 *      ================
 *      L2 entry <vlan, mac2> is learned and unicast packet destined to the learned L2 entry is forwarded to the learned port
 */

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2;

bcm_vlan_t vid = 2;
bcm_field_group_t group;
bcm_pbmp_t  ing_pbmp, egr_pbmp;
int num_egress_ports = 2;
bcm_field_entry_t entry[num_egress_ports];

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


int rx_count = 0;   /* Global received packet count */
int test_failed = 0; /*Final result will be stored here */

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


/*
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t addr;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    int port_list[3];
    uint32 learn_flags;
    char   str[512];
    bcm_error_t rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];
    printf("Ingress port : %d\n", ingress_port );
    printf("Egress port1: %d\n", egress_port1);
    printf("Egress port2: %d\n", egress_port2);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vid));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, BCM_PORT_LOOPBACK_MAC));
    /* first learn <vid=2, mac=0x22> at egress_port1 */
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000001100000000002281000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", egress_port1);
    bshell(unit, str);
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, 0));

    BCM_PBMP_CLEAR(ing_pbmp);
    BCM_PBMP_PORT_ADD(ing_pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, ing_pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(egr_pbmp);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port2);
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

    return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int testCleanup(int unit)
{
    bcm_pbmp_t pbmp;


    /* clean up learned l2 entries */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Set default vlan. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, 1));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);

    /* Delete vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    BCM_IF_ERROR_RETURN(ingress_port_multi_cleanup(unit, ing_pbmp));
    BCM_IF_ERROR_RETURN(egress_port_multi_cleanup(unit, egr_pbmp));

    bshell(unit, "pw stop");
    unregisterPktioRxCallback(unit);

    return BCM_E_NONE;
}

unsigned char *expected_packets[1];
bcm_pbmp_t expected_pbmp[1];
uint32 packet_patterns = 1; /* only one expected packet pattern */



/* Rx callback function for applications using the HSDK PKTIO */
bcm_pktio_rx_t
pktioRxCallback(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int                i, *count = (auto) cookie;
    void               *buffer;
    uint32              length;
    bcm_port_t          src_port;
    uint32              port; 

    /* Get basic packet info */
    if (BCM_FAILURE
        (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {
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
        0x00, 0x02, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x14, 0x14, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0xbc, 0x94, 0x97, 0x7f};


/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t testVerify(int unit)
{
    char   str[512];
    int expected_rx_count, pass;

    uint8 mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_l2_addr_t l2addr;

    /* verify the learned L2 entry */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_get(unit, mac, vid, &l2addr));

    printf("Learned L2: vlan=%d mac=%02x:%02x:%02x:%02x:%02x:%02x, mod=0x%x, port=0x%x\n", 
           l2addr.vid, l2addr.mac[0], l2addr.mac[1], l2addr.mac[2],
           l2addr.mac[3], l2addr.mac[4], l2addr.mac[5], l2addr.modid, l2addr.port);

    /* verify unicast forwarding using the learned L2 entry */
    expected_rx_count = 1;
    expected_packets[0]  = expected_egress_packet1;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);

    printf("Verify UC pkt from port %d forwarding to port %d\n", ingress_port, egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]", pass? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if (pass)
        return BCM_E_NONE;
    else
        return BCM_E_FAIL;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in testVerify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  rv = testSetup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
    return -1;
  }
  printf("Completed test setup successfully.\n");

  rv = testVerify(unit);
  if (BCM_FAILURE(rv)) {
    printf("testVerify() failed\n");
    return -1;
  }
  rv = testCleanup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testCleanup() failed\n");
    return -1;
  }
  printf("Completed test verify successfully.\n");

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
