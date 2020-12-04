/*  Feature  : L2 l2 callback
 *
 *  Usage    : BCM.0> cint th4_hsdk_l2_callback_register.c
 *
 *  config   : th4_hsdk_l2_config.yml
 *
 *  Log file : th4_hsdk_l2_callback_register.log
 *  
 *  Test Topology :
 *
 *                   +----------------------------+
 *                   |                            |
 *                   |                            |
 *    ingress_port1  |                            |  
 *                   |                            |
 *  -----------------+          SWITCH            |  egress_port
 *                   |                            +-----------------
 *    ingress_port2  |                            |
 *  -----------------+                            |
 *                   |                            |
 *                   |                            |
 *                   +----------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT configures L2 callback using BCM APIs.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *    Register l2 call back function. 
 * 
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *
 *      (3.1) Send a packet and make sure the callback is called with LEARN op for the src MAC.
 *      (3.2) Enable aging and make sure the callback is called with AGE op for the src MAC.
 *      (3.3) Add the src MAC manaully using SDK API, make sure the callback is called with ADD op for the src MAC
 *      (3.4) Send the same packet from another ingress port, make sure the callback is called with MOVE op for the src MAC 
 *      (3.5) Delete the src MAC manaully using SDK API, make sure the callback is called with DELETE op for the src MAC 
 *            Check egress packet at the egress ports.
 *            Ingress port will send two packets, one packet will be forwarded to egress port1,
 *            another packet will be forwarded to egress port2.
 *
 *      Expected Result:
 *      ================
 *      Event op, <vlan, mac> and port in the callback function for each expected event are the same as expected
*/

cint_reset();

bcm_port_t ingress_port1, ingress_port2;
bcm_port_t egress_port1;
bcm_vlan_t vid = 2;
char src_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
bcm_pbmp_t  ing_pbmp, egr_pbmp;

int max_index = 2;
char expected_mac[6];
bcm_vlan_t expected_vid;
bcm_port_t expected_port[2];
int expected_modid[2];
int expected_op[2];
int expected_index;
int test_failed = 0; /*Final result will be stored here */

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
  int i = 0, port = 0, index = 0;
  bcm_error_t rv;

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


/*Setup learning on port*/
bcm_error_t
port_learn_mode_set(int unit, bcm_port_t port, uint32 flags)
{
    bcm_error_t rv = BCM_E_NONE;
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
 * Configures the port in loopback mode and is configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet from the egress port.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t     port;

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
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

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_cleanup port: %d cleanup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));
    }
    return BCM_E_NONE;
}


void
dump_l2_addr(int unit, char *pfx, bcm_l2_addr_t *l2addr)
{
    printf("%s mac=%02x:%02x:%02x:%02x:%02x:%02x, vlan=%d, MODID=0x%x, PORT=0x%x\n",
           pfx,
           l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
           l2addr->mac[3], l2addr->mac[4], l2addr->mac[5],
           l2addr->vid, l2addr->modid, l2addr->port);
}

void l2_cb(
    int unit, 
    bcm_l2_addr_t *l2addr, 
    int op, 
    void *userdata)
{
    switch(op) {
    case BCM_L2_CALLBACK_DELETE:
	dump_l2_addr(unit, "L2 DEL:", l2addr);
        break;

    case BCM_L2_CALLBACK_ADD:
	dump_l2_addr(unit, "L2 ADD:", l2addr);
        break;

    case BCM_L2_CALLBACK_REPORT:
	dump_l2_addr(unit, "L2 REPORT:", l2addr);
        break;

    case BCM_L2_CALLBACK_LEARN_EVENT:
	dump_l2_addr(unit, "L2 LEARN:", l2addr);
        break;

    case BCM_L2_CALLBACK_AGE_EVENT:
	dump_l2_addr(unit, "L2 AGE:", l2addr);
        break;

    case BCM_L2_CALLBACK_MOVE_EVENT:
	dump_l2_addr(unit, "L2 MOVE:", l2addr);
        break;

    case BCM_L2_CALLBACK_REFRESH_EVENT:
	dump_l2_addr(unit, "L2 REFRESH:", l2addr);
        break;

    }

    if (expected_index >= max_index) {
        test_failed = 1;
        printf("wrong index\n");
        return;
    }

    if (op != expected_op[expected_index]) {
        test_failed = 1;
        printf("wrong op\n");
    }
    if (l2addr->vid != expected_vid) {
        test_failed = 1;
        printf("wrong vid\n");
    }
    if (sal_memcmp(l2addr->mac, expected_mac, 6)) {
        test_failed = 1;
        printf("wrong mac\n");
    }
    if (l2addr->port != expected_port[expected_index]) {
        test_failed = 1;
        printf("wrong port\n");
    }
    if (l2addr->modid != expected_modid[expected_index]) {
        test_failed = 1;
        printf("wrong modid\n");
    }

    expected_index++;

    return;
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

    ingress_port1 = port_list[0];
    ingress_port2 = port_list[1];
    egress_port1 = port_list[2];
    printf("Ingress port1 : %d\n", ingress_port1 );
    printf("Ingress port2: %d\n", ingress_port2);
    printf("Egress port1: %d\n", egress_port1);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port2);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port1, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port2, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));

    BCM_PBMP_CLEAR(ing_pbmp);
    BCM_PBMP_PORT_ADD(ing_pbmp, ingress_port1);
    BCM_PBMP_PORT_ADD(ing_pbmp, ingress_port2);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, ing_pbmp)) {
        printf("ingress_port_multi_setup() failed\n");
        return -1;
    }

    BCM_PBMP_CLEAR(egr_pbmp);
    BCM_PBMP_PORT_ADD(egr_pbmp, egress_port1);
    if (BCM_E_NONE != egress_port_multi_setup(unit, egr_pbmp)) {
        printf("egress_port_multi_setup() failed\n");
        return -1;
    }

    BCM_IF_ERROR_RETURN(bcm_l2_init(unit));
    BCM_IF_ERROR_RETURN(bcm_l2_addr_register(unit, l2_cb, NULL));
    return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int testCleanup(int unit)
{
    bcm_pbmp_t pbmp;

    BCM_IF_ERROR_RETURN(bcm_l2_addr_unregister(unit, l2_cb, NULL));

    /* clean up learned l2 entries */
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));

    /* Set default vlan. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port1, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port2, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, 1));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port2);
    BCM_PBMP_PORT_ADD(pbmp, egress_port1);

    /* Delete vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    BCM_IF_ERROR_RETURN(ingress_port_multi_cleanup(unit, ing_pbmp));
    BCM_IF_ERROR_RETURN(egress_port_multi_cleanup(unit, egr_pbmp));


    return BCM_E_NONE;
}

/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t testVerify(int unit)
{
    char str[512];
    int pass1, pass2, pass3, pass4;

    sal_memcpy(expected_mac, src_mac, 6);
    expected_vid = vid;
    expected_modid[0] = 0;
    expected_port[0] = ingress_port1;
    expected_op[0] = BCM_L2_CALLBACK_LEARN_EVENT;
    expected_index = 0;
    
    /* learn but do not forward */
    BCM_IF_ERROR_RETURN(port_learn_mode_set(unit, ingress_port1, BCM_PORT_LEARN_ARL));
 
    printf("Verify L2 learning from port %d\n", ingress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 2", ingress_port1);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(port_learn_mode_set(unit, ingress_port1, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));

    pass1 = (test_failed == 0);
    printf("\n-------------------------------------------- \n");
    printf("    RESULT OF l2 register cb for LEARN_EVENT  ");
    printf("\n-------------------------------------------- \n");
    printf("Test = [%s]", pass1? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    printf("Verify L2 aging\n");
    int age_seconds;
    sal_memcpy(expected_mac, src_mac, 6);
    expected_vid = vid;
    expected_modid[0] = 0;
    expected_port[0] = ingress_port1;
    expected_op[0] = BCM_L2_CALLBACK_AGE_EVENT;
    expected_index = 0;

    BCM_IF_ERROR_RETURN(bcm_l2_age_timer_get(unit, &age_seconds));
    BCM_IF_ERROR_RETURN(bcm_l2_age_timer_set(unit, 1));
    bshell(unit, "sleep quiet 5");
    BCM_IF_ERROR_RETURN(bcm_l2_age_timer_set(unit, age_seconds));

    pass2 = (test_failed == 0);
    printf("\n------------------------------------------ \n");
    printf("    RESULT OF l2 register cb for AGE_EVENT  ");
    printf("\n------------------------------------------ \n");
    printf("Test = [%s]", pass2? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    printf("Verify L2 add\n");
    bcm_l2_addr_t l2addr;
    uint8 mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};

    sal_memcpy(expected_mac, src_mac, 6);
    expected_vid = vid;
    expected_modid[0] = 0;
    expected_port[0] = ingress_port1;
    expected_op[0] = BCM_L2_CALLBACK_ADD;
    expected_index = 0;

    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.modid = 0;
    l2addr.port = ingress_port1;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));
    bshell(unit, "sleep quiet 2");

    pass3 = (test_failed == 0);
    printf("\n------------------------------------------ \n");
    printf("    RESULT OF l2 register cb for ADD_EVENT  ");
    printf("\n------------------------------------------ \n");
    printf("Test = [%s]", pass3? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    printf("Verify L2 delete and move\n");
    sal_memcpy(expected_mac, src_mac, 6);
    expected_vid = vid;
    expected_modid[0] = 0;
    expected_port[0] = ingress_port1;
    expected_op[0] = BCM_L2_CALLBACK_DELETE;
    expected_modid[1] = 0;
    expected_port[1] = ingress_port2;
    expected_op[1] = BCM_L2_CALLBACK_MOVE_EVENT;
    expected_index = 0;

    /* learn but do not forward */
    BCM_IF_ERROR_RETURN(port_learn_mode_set(unit, ingress_port2, BCM_PORT_LEARN_ARL));

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000001181000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 2", ingress_port2);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(port_learn_mode_set(unit, ingress_port2, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));

    pass4 = (test_failed == 0);
    printf("\n------------------------------------------------------- \n");
    printf("    RESULT OF l2 register cb for DELETE and  MOVE_EVENT  ");
    printf("\n------------------------------------------------------- \n");
    printf("Test = [%s]", pass4? "PASS":"FAIL");
    printf("\n------------------------------------- \n");

    if(pass1 && pass2 && pass3 && pass4)
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
