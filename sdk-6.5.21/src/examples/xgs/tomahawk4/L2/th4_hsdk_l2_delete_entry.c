/*  Feature  : L2 delete l2 entry
 *
 *  Usage    : BCM.0> cint th4_hsdk_l2_delete_entry.c
 *
 *  config   : th4_hsdk_l2_config.yml
 *
 *  Log file : th4_hsdk_l2_delete_entry.log
 *  
 *  Test Topology :
 *
 *                   +----------------------------+
 *                   |                            |
 *                   |                            |
 *       port 1      |                            |    port 3
 *  -----------------+                            +-----------------
 *                   |          SWITCH            |
 *                   |                            |
 *       port 2      |                            |    port 4
 *  -----------------+                            +-----------------
 *                   |                            |
 *                   |                            |
 *                   +----------------------------+
 *
 *  Summary:
 *  ========
 *    This CINT script uses BCM API to delete L2 enties by port, mac, and vlan respectively
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *    Create a vlan with 4 ports
 * 
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ==========================================
 *
 *      (3.1) add <vlan, mac1> to port 1, verify the L2 entry in the L2 table and then delete 
 *            delete the L2 entry by mac1
 *      (3.2) add <vlan, mac2> to port 2, verify the L2 entry in the L2 table and then delete 
 *            delete the L2 entry by port 2
 *      (3.3) add <vlan, mac3> to port 3, <vlan, mac4> to port 4, verify the L2 entries in the L2 table 
 *            and then delete the L2 entries by vlan
 *
 *      Expected Result:
 *      ================
 *      After the l2 delete API, the l2 entry/entries are deleted as verified by bcm_l2_addr_get() 
 */

cint_reset();

bcm_port_t port1, port2, port3, port4;
bcm_vlan_t vid = 2;
bcm_pbmp_t  ing_pbmp, egr_pbmp;

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
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t addr;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    int port_list[4];
    char   str[512];
    bcm_error_t rv;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    port1 = port_list[0];
    port2 = port_list[1];
    port3 = port_list[2];
    port4 = port_list[3];
    printf("port1 : %d\n", port1);
    printf("port2 : %d\n", port2);
    printf("port3 : %d\n", port3);
    printf("port4 : %d\n", port4);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    BCM_PBMP_PORT_ADD(pbmp, port4);

    /* Create a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

    /* Add vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

    /* Set port default vlan id. */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port1, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port2, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port3, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port4, vid));

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
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port1, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port2, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port3, 1));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port4, 1));

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    BCM_PBMP_PORT_ADD(pbmp, port4);

    /* Delete vlan member ports. */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));

    /* Delete a vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

    return BCM_E_NONE;
}

bcm_l2_addr_t l2addr;
uint8 mac1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
uint8 mac2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
uint8 mac3[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
uint8 mac4[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};


/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t testVerify(int unit)
{
    int pass, pass_delete_by_port = 0, pass_delete_by_mac = 0, pass_delete_by_vlan = 0;
    int rv, rv1, rv2;

    bcm_l2_addr_t_init(&l2addr, mac1, vid);
    l2addr.modid = 0;
    l2addr.port = port1;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    bcm_l2_addr_t_init(&l2addr, mac2, vid);
    l2addr.modid = 0;
    l2addr.port = port2;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    bcm_l2_addr_t_init(&l2addr, mac3, vid);
    l2addr.modid = 0;
    l2addr.port = port3;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    bcm_l2_addr_t_init(&l2addr, mac4, vid);
    l2addr.modid = 0;
    l2addr.port = port4;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    BCM_IF_ERROR_RETURN(bcm_l2_addr_get(unit, mac1, vid, &l2addr));
    BCM_IF_ERROR_RETURN(bcm_l2_addr_get(unit, mac2, vid, &l2addr));
    BCM_IF_ERROR_RETURN(bcm_l2_addr_get(unit, mac3, vid, &l2addr));
    BCM_IF_ERROR_RETURN(bcm_l2_addr_get(unit, mac4, vid, &l2addr));
    
    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_mac(unit, mac1, 0));
    rv = bcm_l2_addr_get(unit, mac1, vid, &l2addr);
    if (rv == BCM_E_NOT_FOUND) {
        pass_delete_by_mac = 1;
        printf("L2 Delete by mac passes\n");
    }

    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_port(unit, 0, port2, 0));
    rv = bcm_l2_addr_get(unit, mac2, vid, &l2addr);
    if (rv == BCM_E_NOT_FOUND) {
        pass_delete_by_port = 1;
        printf("L2 Delete by port passes\n");
    }

    BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan(unit, vid, 0));
    rv1 = bcm_l2_addr_get(unit, mac3, vid, &l2addr);
    rv2 = bcm_l2_addr_get(unit, mac4, vid, &l2addr);
    if ((rv1 == BCM_E_NOT_FOUND) && (rv2 == BCM_E_NOT_FOUND)) {
        pass_delete_by_vlan = 1;
        printf("L2 Delete by vlan passes\n");
    }

    pass = pass_delete_by_port && pass_delete_by_mac && pass_delete_by_vlan;
    printf("\n------------------------------------- \n");
    printf("    RESULT OF L2 DELETE VERIFICATION  ");
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
