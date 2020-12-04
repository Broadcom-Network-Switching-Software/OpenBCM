/*  Feature  : L2 port flood block
 *
 *  Usage    : BCM.0> cint th4_hsdk_l2_port_flood_block.c
 *
 *  config   : th4_hsdk_l2_config.yml
 *
 *  Log file : th4_hsdk_l2_port_flood_block.log
 *  
 *  Test Topology :
 *
 *                   +----------------------------+
 *                   |                            |  egress_port1
 *                   |                            +-----------------
 *                   |                            |
 *    ingress_port   |                            |  egress_port2
 *  -----------------+          SWITCH            +-----------------
 *                   |                            |
 *                   |                            |  egress_port3
 *                   |                            +-----------------
 *                   |                            |
 *                   +----------------------------+
 *
 *  Summary:
 *  ========
 *    This CINT tests L2 port flood block using bcm_port_flood_block_set() BCM API.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *       (1.1) create a vlan with 4 ports
 * 
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ======================================================
 *      (3.1) Send a UUC packet and make sure the packet is 
 *            flooded to the other 3 ports configured in the vlan
 *      (3.2) Send a UMC packet and make sure the packet is 
 *            flooded to the other 3 ports configured in the vlan
 *      (3.3) Send a BC packet and make sure the packet is 
 *            flooded to the other 3 ports configured in the vlan
 *      (3.4) Call bcm_port_flood_block_set(unit, ingress_port, egress_port3, 
 *                                          BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST) and 
 *            verify that UUC packet will not not flooded to egress_port3
 *      (3.5) Call bcm_port_flood_block_set(unit, ingress_port, egress_port3, 
 *                                          BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST) and 
 *            verify that UMC packet will not not flooded to egress_port3
 *      (3.6) Call bcm_port_flood_block_set(unit, ingress_port, egress_port3, 
 *                                          BCM_PORT_FLOOD_BLOCK_BCAST) and 
 *            verify that BC packet will not not flooded to egress_port3
 *
 *      Expected Result:
 *      ================
 *      Packet content, port bitmap receiving the packet and total received packet count
 *      are all correct
 */

cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2, egress_port3;

bcm_vlan_t vid = 2;
bcm_field_group_t group;
bcm_pbmp_t  ing_pbmp, egr_pbmp;
bcm_l2_addr_t addr;
uint8 mac[6] = {0x01, 0x00, 0x00, 0xbb, 0xbb, 0xbb};
int num_egress_ports = 3;
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
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    int port_list[4];
    bcm_error_t rv;
    uint32_t flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
    bcm_gport_t egress_gport1, egress_gport2;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port1 = port_list[1];
    egress_port2 = port_list[2];
    egress_port3 = port_list[3];
    printf("Ingress port : %d\n", ingress_port );
    printf("Egress port1: %d\n", egress_port1);
    printf("Egress port2: %d\n", egress_port2);
    printf("Egress port3: %d\n", egress_port3);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);
    BCM_PBMP_PORT_ADD(pbmp, egress_port3);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port3, vid));

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

    /* clean up l2 entries */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Set default vlan. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port3, 1));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port2);
    BCM_PBMP_PORT_ADD(pbmp, egress_port3);

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

unsigned char expected_egress_packet2[64] = {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00,
        0x00, 0x02, 0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x14, 0x14, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0xbc, 0x94, 0x97, 0x7f};

unsigned char expected_egress_packet3[64] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x81, 0x00,
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
    uint64 in_pkt, out_pkt;
    int expected_rx_count, pass1, pass2, pass3, pass4, pass5, pass6;
    bcm_error_t rv;

    rx_count = 0;
    expected_rx_count = 3;
    expected_packets[0]  = expected_egress_packet1;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);

    printf("Verify UUC pkt from port %d forwarding to the other ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass1 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test1 = [%s]", pass1? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    rx_count = 0;
    expected_rx_count = 3;
    expected_packets[0]  = expected_egress_packet2;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);

    printf("Verify UMC pkt from port %d forwarding to the other ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x01000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass2 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test2 = [%s]", pass2? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    rx_count = 0;
    expected_rx_count = 3;
    expected_packets[0]  = expected_egress_packet3;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port3);

    printf("Verify BC pkt from port %d forwarding to the other ports\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0xffffffffffff00000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass3 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test3 = [%s]", pass3? "PASS":"FAIL");
    printf("\n------------------------------------- \n");


    rv = bcm_port_flood_block_set(unit, ingress_port, egress_port3, BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST);
    rx_count = 0;
    expected_rx_count = 2;
    expected_packets[0]  = expected_egress_packet1;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);

    printf("Verify UUC pkt from port %d forwarding to the other ports with UUC blocking to port %d\n", ingress_port, egress_port3);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass4 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test4 = [%s]", pass4? "PASS":"FAIL");
    printf("\n------------------------------------- \n");


    rv = bcm_port_flood_block_set(unit, ingress_port, egress_port3, BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST);
    rx_count = 0;
    expected_rx_count = 2;
    expected_packets[0]  = expected_egress_packet2;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);

    printf("Verify UMC pkt from port %d forwarding to the other ports with UMC blocking to port %d\n", ingress_port, egress_port3);
    snprintf(str, 512, "tx 1 pbm=%d data=0x01000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass5 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test5 = [%s]", pass5? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    rv = bcm_port_flood_block_set(unit, ingress_port, egress_port3, BCM_PORT_FLOOD_BLOCK_BCAST);
    rx_count = 0;
    expected_rx_count = 2;
    expected_packets[0]  = expected_egress_packet3;

    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port2);

    printf("Verify BC pkt from port %d forwarding to the other ports with BC blocking to port %d\n", ingress_port, egress_port3);
    snprintf(str, 512, "tx 1 pbm=%d data=0xffffffffffff00000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port);
    bshell(unit, str);

    pass6 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test6 = [%s]", pass6? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    rv = bcm_port_flood_block_set(unit, ingress_port, egress_port3, 0);
    if(pass1 && pass2 && pass3 && pass4 && pass5 && pass6)
    {
        printf("Test = PASS\n");
        return BCM_E_NONE;
    }
    else
    {
        printf("Test = FAIL\n");
        return BCM_E_FAIL;
    }
    
    return BCM_E_NONE;
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
